#pragma once

#include "Direct3D11.hpp"
#include <RapidJSON/document.h>
#include <map>
#include <string>

class Mesh
{

public:

	struct Vertex
	{
		FLOAT x, y, z;
	};

	static std::map<std::string, Mesh> FromJSONArray (const rapidjson::Value& array);

	static Mesh FromJSONObject (const rapidjson::Value& object);

	Mesh (Vertex * pVertices, int cVertices, SHORT * pIndices, int cIndices);

	void Destroy ();

private:

	bool m_bDestroyed;
	Vertex * const m_pVertices;
	const int m_cVertices;
	SHORT * const m_pIndices;
	const int m_cIndices;

};