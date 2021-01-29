#pragma once

struct Light
{
public:
	Light(DirectX::XMFLOAT4 pos, DirectX::XMFLOAT4 color) :
	m_position(pos),
	m_color(color) {}
	Light() :
	m_position(0,0,0,0),
	m_color(0, 0,0,0) {}
	
	void SetPosition(const DirectX::XMFLOAT4& pos) {m_position = pos;}
	void SetColor(const DirectX::XMFLOAT4& color) {m_color = color;}
	DirectX::XMFLOAT4 GetPosition() {return m_position;}
	DirectX::XMFLOAT4 GetColor() {return m_color;}

	bool operator==(Light other)
	{
	}
private:

	DirectX::XMFLOAT4 m_position;
	DirectX::XMFLOAT4 m_color;
};

struct PointLight : Light
{
public:
	PointLight(DirectX::XMFLOAT4 pos, DirectX::XMFLOAT4 color) : Light(pos, color) {}
	PointLight() : Light() {}
};


