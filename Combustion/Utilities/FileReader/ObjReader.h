#pragma once

class ObjReader
{
public:
	Mesh3D readOBJ(const char *filename);

protected:
	std::map<std::string, unsigned int> m_vertexHashMap;
	std::vector<Position> m_temporaryPositions;
	std::vector<TexCoord> m_temporaryTexCoords;
	std::vector<Normal> m_temporaryNormals;

private:
	void processPosition(std::vector<std::string> &c);
	void processTexCoord(std::vector<std::string> &c);
	void processNormal(std::vector<std::string> &c);
	void processFace(std::vector<std::string> &c, Mesh3D& readMesh);

	std::vector<std::string> splitString(std::string &str, char delimiter);
};

