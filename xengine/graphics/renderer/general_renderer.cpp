#include "general_renderer.h"

#include <vendor/glad/glad.h>

namespace xengine
{
	void GeneralRenderer::RenderMesh(Mesh * mesh)
	{
		glBindVertexArray(mesh->VAO());

		if (mesh->indices.size() > 0)
			glDrawElements(mesh->Topology(), static_cast<GLsizei>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
		else
			glDrawArrays(mesh->Topology(), 0, static_cast<GLsizei>(mesh->positions.size()));
	}

	void GeneralRenderer::RenderMesh(Mesh * mesh, Material * material)
	{
		material->shader->Use();

		// flush all active ogl settings
		material->UpdateOglStatus(); // note: OglStatus may lock to prevent from being modified

		// flush all uniforms binded with material to shader
		material->UpdateShaderUniforms();

		RenderMesh(mesh);
	}

	void GeneralRenderer::RenderSingleCommand(const RenderCommand & command)
	{
		Material* material = command.material;
		Mesh* mesh = command.mesh;

		// set command-specific uniforms
		material->shader->Use();
		material->shader->SetUniform("model", command.transform);
		material->shader->SetUniform("prevModel", command.prevTransform);

		RenderMesh(mesh, material);
	}

	void GeneralRenderer::RenderSingleCommand(const RenderCommand & command, Camera * camera)
	{
		Material* material = command.material;

		// TODO: set different names from these in uniform buffer
		material->shader->Use();
		material->shader->SetUniform("projection", camera->GetProjection());
		material->shader->SetUniform("view", camera->GetView());
		material->shader->SetUniform("camPos", camera->GetPosition());

		RenderSingleCommand(command);
	}

	void GeneralRenderer::Blit(FrameBuffer * from, FrameBuffer * to, unsigned int type)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, from->ID());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, to->ID());
		glBlitFramebuffer(0, 0, from->Width(), from->Height(), 0, 0, to->Width(), to->Height(), type, GL_NEAREST);
	}

	void GeneralRenderer::Blit(FrameBuffer * from, unsigned int width, unsigned height, unsigned int type)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, from->ID());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, from->Width(), from->Height(), 0, 0, width, height, type, GL_NEAREST);
	}
}