#include "mesh_manager.h"

#include <cstdio>
#include <cstdarg> // Variadic Function

#include <vendor/glad/glad.h>
#include <vendor/glm/glm.hpp>

#include <vendor/assimp/scene.h>
#include <vendor/assimp/Importer.hpp>
#include <vendor/assimp/postprocess.h>

#include <utility/log.h>

#include "primitive.h"

namespace xengine
{
	std::unordered_map<std::string, Mesh> MeshManager::g_localTable{};
	std::unordered_map<std::string, Mesh> MeshManager::g_globalTable{};
	Mesh MeshManager::_nullMesh;

	void MeshManager::Initialize()
	{
		generateDefaultMesh();
		_nullMesh = g_globalTable["cube"];
	}

	void MeshManager::Clear()
	{
		ClearLocal();
		ClearGlobal();
	}

	void MeshManager::ClearLocal()
	{
		g_localTable.clear();
	}

	void MeshManager::ClearGlobal()
	{
		g_globalTable.clear();
	}

	Mesh MeshManager::LoadLocalPrimitive(std::string type, ...)
	{
		va_list args;
		va_start(args, type);
		Mesh mesh = loadPrimitive(g_localTable, type, args);
		va_end(args);
		return mesh;
	}

	Mesh MeshManager::LoadGlobalPrimitive(std::string type, ...)
	{
		va_list args;
		va_start(args, type);
		Mesh mesh = loadPrimitive(g_globalTable, type, args);
		va_end(args);
		return mesh;
	}

	Mesh MeshManager::loadPrimitive(std::unordered_map<std::string, Mesh>& table, std::string type, va_list args_list)
	{
		Mesh mesh;

		// Note: Parameters of functions that correspond to ... are promoted before passing
		// to your variadic function. char and short are promoted to int, float is promoted
		// to double, etc.

		unsigned int argc = 0;

		std::string name = type;

		if (type == "quad")
		{
			auto it = table.find(name);
			if (it != table.end()) return it->second;

			mesh = Quad();
		}
		else if (type == "cube")
		{
			auto it = table.find(name);
			if (it != table.end()) return it->second;

			mesh = Cube();
		}
		else if (type == "plane")
		{
			auto it = table.find(name);
			if (it != table.end()) return it->second;

			mesh = Plane();
		}
		else if (type == "sphere")
		{
			unsigned int xseg = 16, yseg = 8;

			xseg = va_arg(args_list, unsigned int);
			yseg = va_arg(args_list, unsigned int);

			name.append("_");
			name.append(std::to_string(xseg));
			name.append("_");
			name.append(std::to_string(yseg));

			auto it = table.find(name);
			if (it != table.end()) return it->second;

			mesh = Sphere(xseg, yseg);
		}
		else if (type == "torus")
		{
			unsigned int xseg = 16, yseg = 8;
			float r1 = 1.0f, r2 = 0.25f;

			xseg = va_arg(args_list, unsigned int);
			yseg = va_arg(args_list, unsigned int);
			r1 = static_cast<float>(va_arg(args_list, double));
			r2 = static_cast<float>(va_arg(args_list, double));

			name.append("_");
			name.append(std::to_string(xseg));
			name.append("_");
			name.append(std::to_string(yseg));
			name.append("_");
			name.append(std::to_string(r1));
			name.append("_");
			name.append(std::to_string(r2));

			auto it = table.find(name);
			if (it != table.end()) return it->second;

			mesh = Torus(xseg, yseg, r1, r2);
		}
		else if (false)
		{
			// TODO: more primitives
		}
		else
		{
			Log::Message("[MeshManager] Primitive \"" + type + "\" not supported", Log::ERROR);
			return _nullMesh;
		}

		if (!mesh)
		{
			Log::Message("[MeshManager] Primitive \"" + name + "\" loading failed", Log::ERROR);
			return _nullMesh;
		}

		Log::Message("[MeshManager] Mesh \"" + name + "\" loaded successfully", Log::INFO);
		table[name] = mesh;

		return mesh;
	}

	Mesh MeshManager::Get(const std::string & name)
	{
		{
			auto it = g_localTable.find(name);
			if (it != g_localTable.end()) return it->second;
		}

		{
			auto it = g_globalTable.find(name);
			if (it != g_globalTable.end()) return it->second;
		}

		Log::Message("[MeshManager] Material \"" + name + "\" not found", Log::WARN);
		return _nullMesh;
	}

	void MeshManager::generateDefaultMesh()
	{
		LoadGlobalPrimitive("quad");
		LoadGlobalPrimitive("plane");
		LoadGlobalPrimitive("cube");
		LoadGlobalPrimitive("sphere", 16, 8);
		LoadGlobalPrimitive("sphere", 256, 128);
	}
}