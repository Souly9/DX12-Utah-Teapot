#pragma once

// Helper class to easier store the mesh data read from .obj files

struct Position {
	Position(float x, float y, float z) : p(x, y, z) {}
	DirectX::XMFLOAT3 p;
};

struct Normal {
	Normal(float x, float y, float z) : np(x, y, z) {}
	DirectX::XMFLOAT3 np;
};

struct TexCoord {
	TexCoord(float x, float y) : uv(x, y) {}
	DirectX::XMFLOAT2 uv;
};

struct Vertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 TexCoord;
};