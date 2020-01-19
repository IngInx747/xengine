#pragma once
#ifndef XE_MODEL_LOADER_H
#define XE_MODEL_LOADER_H

#include "model.h"

struct aiNode;
struct aiScene;

namespace xengine
{
	// load model from file of wave-front format
	Model * LoadModel_Impl_Assimp(const std::string& path);
}

#endif // !XE_MODEL_LOADER_H
