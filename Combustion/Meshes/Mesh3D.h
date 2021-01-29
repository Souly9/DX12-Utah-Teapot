#pragma once
#include <stdafx.h>
// Helper class to easier access meshes read from .obj
class Mesh3D
{
public:
	Mesh3D();
	virtual ~Mesh3D();

	std::vector<unsigned int> m_indices;
	std::vector<Vertex> m_vertices;
	std::vector<Position> m_positions;

	unsigned int m_vboID;
	unsigned int m_iboID;
	int m_numPrimitives;
};

