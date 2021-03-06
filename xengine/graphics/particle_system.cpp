#include "particle_system.h"

#include <vector>
#include <random>
#include <functional> // std::bind

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "shader_manager.h"
#include "texture_manager.h"

namespace xengine
{
	////////////////////////////////////////////////////////////////
	// Particle System
	////////////////////////////////////////////////////////////////

	PSFirework::Particle::Particle()
		:
		position(),
		velocity(),
		age(0),
		type(0)
	{}

	ParticleSystem::ParticleSystem()
		:
		GeometryObject()
	{
		aabbLocal = AABB({ -1, -1, -1 }, { 1, 1, 1 });
		aabbGlobal.BuildFromTransform(aabbLocal, transform);
	}

	////////////////////////////////////////////////////////////////
	// Particle System: Firework
	////////////////////////////////////////////////////////////////

	PSFirework::PSFirework()
		:
		ParticleSystem()
	{
	}

	PSFirework::~PSFirework()
	{
		DeleteGpuData();
	}

	void PSFirework::Initialize()
	{
		std::uniform_real_distribution<float> random_dist(0.0f, 1.0f);
		std::default_random_engine random_dist_generator;
		auto dice = std::bind(random_dist, random_dist_generator);

		/// particles
		// generate particles raw data
		m_numParticle = 1000;

		std::vector<Particle> particles(m_numParticle);

		// note: Particles will stay in (0,0,0) if not given specific position and
		// during the very first batches of generations particles that not reached
		// by updating geometry shader will appear at (0,0,0).
		// After a few gnerations when all particles are reached by the shader, no
		// particle will appear at (0,0,0).

		// buffer data to GPU
		glGenVertexArrays(2, m_vaos);
		glGenBuffers(2, m_vbos);
		glGenTransformFeedbacks(2, m_tfos);

		for (unsigned int i = 0; i < 2; i++)
		{
			glBindVertexArray(m_vaos[i]);
			glBindBuffer(GL_ARRAY_BUFFER, m_vbos[i]);
			glBufferData(GL_ARRAY_BUFFER, m_numParticle * sizeof(Particle), particles.data(), GL_DYNAMIC_DRAW);

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			glEnableVertexAttribArray(3);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)0); // vec3: position
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)12); // vec3: velocity
			glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(Particle), (const GLvoid*)24); // uint: age
			glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, sizeof(Particle), (const GLvoid*)28); // uint: type
			//glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)24); // uint: age
			//glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)28); // uint: type
		}

		// Let the two transform feedback buffers point to the two VBO buffers.
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_tfos[0]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_vbos[0]);
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_tfos[1]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_vbos[1]);

		/// shaders
		// simulation shader
		m_particleUpdateShader.Generate();
		{
			m_particleUpdateShader.AttachVertexShader(ReadShaderSource("shaders/particle/particle_system.update.firework.vs"));
			m_particleUpdateShader.AttachGeometryShader(ReadShaderSource("shaders/particle/particle_system.update.firework.gs"));

			// specify the attributes that go into the transform feedback buffer
			const GLchar* varyings[4];
			varyings[0] = "Position1";
			varyings[1] = "Velocity1";
			varyings[2] = "Age1";
			varyings[3] = "Type1";
			glTransformFeedbackVaryings(m_particleUpdateShader.ID(), 4, varyings, GL_INTERLEAVED_ATTRIBS);
		}
		m_particleUpdateShader.Link();

		m_particleUpdateShader.Bind();
		{
			m_particleUpdateShader.SetUniform("gDeltaTime", 0.01f); // particle update rate
			m_particleUpdateShader.SetUniform("gLauncherLifetime", 2);
			m_particleUpdateShader.SetUniform("gShellLifetime", 500);
			m_particleUpdateShader.SetUniform("gSecondaryShellLifetime", 100);
			m_particleUpdateShader.SetUniform("gRandomTexture", 0);
			m_particleUpdateShader.SetUniform("gPositionOriginal", glm::vec3(0, 0, 0));
			m_particleUpdateShader.SetUniform("gVelocityOriginal", glm::vec3(0, 2.0f, 0));
			m_particleUpdateShader.SetUniform("gGrativityAccel", glm::vec3(0, -1.0f, 0));
		}
		m_particleUpdateShader.Unbind();

		// rendering shader
		m_particleRenderShader.AttachVertexShader(ReadShaderSource("shaders/particle/particle_system.render.billboard.vs"));
		m_particleRenderShader.AttachGeometryShader(ReadShaderSource("shaders/particle/particle_system.render.billboard.gs"));
		m_particleRenderShader.AttachFragmentShader(ReadShaderSource("shaders/particle/particle_system.render.billboard.fs"));
		m_particleRenderShader.GenerateAndLink();

		m_particleRenderShader.Bind();
		{
			m_particleRenderShader.SetUniform("gColorMap", 0);
			m_particleRenderShader.SetUniform("gBillboardSize", 0.1f);
		}
		m_particleRenderShader.Unbind();

		/// textures
		std::vector<glm::vec3> noises;
		for (unsigned int i = 0; i < 1000; i++)
			noises.push_back({ dice(), dice(), dice() });

		m_noise.Generate1D(137, GL_RGB, GL_RGB, GL_FLOAT, &noises[0]);
		m_noise.SetWrapS(GL_REPEAT);
		m_noise.SetFilterMin(GL_LINEAR);

		m_particleTexture = TextureManager::Get("white");
	}

	void PSFirework::SetPosition(const glm::vec3 & position)
	{
		GeometryObject::SetPosition(position);

		m_particleUpdateShader.Bind();
		m_particleUpdateShader.SetUniform("gPositionOriginal", position);
		m_particleUpdateShader.Unbind();
	}

	void PSFirework::SetParticleTexture(const Texture & texture)
	{
		m_particleTexture = texture;
	}

	void PSFirework::Reset()
	{
		m_time = 0;
	}

	void PSFirework::Render()
	{
		m_time += 1;

		updateParticles();

		renderParticles();

		m_currId = 1 - m_currId;
	}

	void PSFirework::DeleteGpuData()
	{
		if (m_vaos[0])
		{
			glDeleteVertexArrays(2, m_vaos);
			m_vaos[0] = m_vaos[1] = 0;
		}

		if (m_vbos[0])
		{
			glDeleteBuffers(2, m_vbos);
			m_vbos[0] = m_vbos[1] = 0;
		}

		if (m_tfos[0])
		{
			glDeleteTransformFeedbacks(2, m_tfos);
			m_tfos[0] = m_tfos[1] = 0;
		}
	}

	void PSFirework::updateParticles()
	{
		m_particleUpdateShader.Bind();
		m_particleUpdateShader.SetUniform("gTime", m_time);

		m_noise.Bind(0); // gRandomTexture

		glEnable(GL_RASTERIZER_DISCARD);

		glBindVertexArray(m_vaos[m_currId]);
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_tfos[1 - m_currId]);

		glBeginTransformFeedback(GL_POINTS);
		{
			if (m_first)
			{
				glDrawArrays(GL_POINTS, 0, 1); // 1 or m_numParticle
				m_first = false;
			}
			else
			{
				glDrawTransformFeedback(GL_POINTS, m_tfos[m_currId]);
			}
		}
		glEndTransformFeedback();

		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
		glBindVertexArray(0);

		glDisable(GL_RASTERIZER_DISCARD);
	}

	void PSFirework::renderParticles()
	{
		m_particleRenderShader.Bind();

		m_particleTexture.Bind(0); // gColorMap

		glBindVertexArray(m_vaos[m_currId]);

		glDrawTransformFeedback(GL_POINTS, m_tfos[1 - m_currId]);

		glBindVertexArray(0);
	}
}