#include "render_command.h"

namespace xengine
{
	RenderCommand::RenderCommand() : mesh(nullptr), material(nullptr)
	{}

	RenderCommand::RenderCommand(Mesh* mesh, Material* material) : mesh(mesh), material(material)
	{}
}