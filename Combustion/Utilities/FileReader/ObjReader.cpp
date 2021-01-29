#pragma once
#include <stdafx.h>
#include <ObjReader.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

Mesh3D ObjReader::readOBJ(const char* filename)
{
	std::ifstream stream;
	stream.open(filename);
	
	std::string line;

	Mesh3D mesh;
	while(getline(stream, line))
	{
		// Get the specifier of the line (v, vt...)
		std::vector<std::string> c = splitString(line, ' ');
		
		// Did we read all of it?
		if(c.size() == 0) continue;

		if(c[0].compare("v") == 0)
			processPosition(c);
		else if(c[0].compare("vt") == 0)
			processTexCoord(c);
		else if(c[0].compare("vn") == 0)
			processNormal(c);
		else if(c[0].compare("f") == 0)
			processFace(c, mesh);
	}
	mesh.m_numPrimitives = mesh.m_indices.size();
	mesh.m_positions = m_temporaryPositions;
	return mesh;
}

std::vector<std::string> ObjReader::splitString(std::string& str, char delimiter)
{
	std::vector<std::string> words;
	std::string word;
	std::stringstream stream(str);

	while(std::getline(stream, word, delimiter))
		words.push_back(word);

	return words;
}

void ObjReader::processPosition(std::vector<std::string>& c)
{
	m_temporaryPositions.push_back(
		Position(std::stof(c[1]), std::stof(c[2]),std::stof(c[3])));
}

void ObjReader::processTexCoord(std::vector<std::string>& c)
{
	m_temporaryTexCoords.push_back(
		TexCoord(std::stof(c[1]), std::stof(c[2])));
}

void ObjReader::processNormal(std::vector<std::string>& c)
{
	m_temporaryNormals.push_back(
		Normal(std::stof(c[1]), std::stof(c[2]), std::stof(c[3])));
}

void ObjReader::processFace(std::vector<std::string>& c, Mesh3D& readMesh)
{
	for (int i = 1; i < c.size(); ++i)
	{
		auto iter = m_vertexHashMap.find(c[i]);

		if (iter == m_vertexHashMap.end())
		{
			std::vector<std::string> index = splitString(c[i], '/');
			Vertex vert{ m_temporaryPositions[std::stoi(index[0]) - 1].p, m_temporaryNormals[std::stoi(index[2]) - 1].np, m_temporaryTexCoords[std::stoi(index[1]) - 1].uv};
		
			m_vertexHashMap[c[i]] = readMesh.m_vertices.size();
			readMesh.m_vertices.push_back(vert);
		}
		else
		{
			readMesh.m_vertices.push_back(readMesh.m_vertices[m_vertexHashMap.at(c[i])]);
		}
	}
	if(c.size() == 5)
	{
		readMesh.m_indices.push_back(m_vertexHashMap[c[1]]);
		readMesh.m_indices.push_back(m_vertexHashMap[c[2]]);
		readMesh.m_indices.push_back(m_vertexHashMap[c[4]]);
		
		readMesh.m_indices.push_back(m_vertexHashMap[c[2]]);
		readMesh.m_indices.push_back(m_vertexHashMap[c[3]]);
		readMesh.m_indices.push_back(m_vertexHashMap[c[4]]);
		return;
	}
	readMesh.m_indices.push_back(m_vertexHashMap[c[1]]);
	readMesh.m_indices.push_back(m_vertexHashMap[c[2]]);
	readMesh.m_indices.push_back(m_vertexHashMap[c[3]]);
}


