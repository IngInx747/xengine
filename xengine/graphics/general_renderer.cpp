#include "general_renderer.h"

#include <glad/glad.h>

namespace xengine
{
	void RenderMesh(Mesh * mesh)
	{
		glBindVertexArray(mesh->VAO());

		if (mesh->IBO())
			glDrawElements(mesh->Topology(), mesh->NumIds(), GL_UNSIGNED_INT, 0);
		else
			glDrawArrays(mesh->Topology(), 0, mesh->NumVtx());

		glBindVertexArray(0);
	}

	void RenderMesh(Mesh * mesh, Material * material)
	{
		material->shader.Bind();

		// flush all active ogl settings
		material->UpdateOglStatus(); // note: OglStatus may lock to prevent from being modified

		// flush all uniforms binded with material to shader
		material->UpdateShaderUniforms();

		RenderMesh(mesh);
	}

	void Blit(FrameBuffer * from, FrameBuffer * to, unsigned int type)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, from->ID());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, to->ID());
		glBlitFramebuffer(0, 0, from->Width(), from->Height(), 0, 0, to->Width(), to->Height(), type, GL_NEAREST);
	}

	void Blit(const FrameBuffer & from, FrameBuffer & to, unsigned int type)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, from.ID());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, to.ID());
		glBlitFramebuffer(0, 0, from.Width(), from.Height(), 0, 0, to.Width(), to.Height(), type, GL_NEAREST);
	}

	void Blit(FrameBuffer * from, unsigned int width, unsigned height, unsigned int type)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, from->ID());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, from->Width(), from->Height(), 0, 0, width, height, type, GL_NEAREST);
	}

	void Blit(const FrameBuffer & from, unsigned int width, unsigned height, unsigned int type)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, from.ID());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, from.Width(), from.Height(), 0, 0, width, height, type, GL_NEAREST);
	}
}