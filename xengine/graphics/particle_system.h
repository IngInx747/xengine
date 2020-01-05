#pragma once
#ifndef XE_PARTICLE_SYSTEM_H
#define XE_PARTICLE_SYSTEM_H

#include <model/object.h>

#include "shader.h"
#include "texture.h"

namespace xengine
{
	class ParticleSystem : public GeometryObject
	{
	public:
		ParticleSystem();

		virtual void Reset() = 0;
		virtual void Render() = 0;
		virtual void DeleteGpuData() = 0;
	};

	class PSFirework : public ParticleSystem
	{
	public:
		struct Particle
		{
			glm::vec3 position;
			glm::vec3 velocity;
			unsigned int age;
			unsigned int type;

			Particle();
		};

	public:
		PSFirework();
		~PSFirework();

		void Initialize();

		virtual void Reset();
		virtual void Render();
		virtual void DeleteGpuData();

		virtual void SetPosition(const glm::vec3& position);

		void SetParticleTexture(const Texture& texture);

	private:
		void updateParticles();
		void renderParticles();

	private:
		unsigned int m_vaos[2]; // particle buffer objects
		unsigned int m_vbos[2]; // particle buffer objects
		unsigned int m_tfos[2]; // transform feedback objects
		unsigned int m_currId = 0;

		unsigned int m_numParticle; // number of particles
		unsigned int m_time = 0; // frame count
		bool m_first = true; // first draw or not

		Shader m_particleRenderShader; // render particles with billboarding
		Shader m_particleUpdateShader; // calculate particles

		Texture m_particleTexture; // texture on every particle
		Texture m_noise; // random noise
	};
}

#endif // !XE_PARTICLE_SYSTEM_H
