#pragma once
#include <stdafx.h>
class Shape
{
public:
	Shape() : m_modelMatrix(), m_rotQuat(), m_position(), m_scaleFactor(1.0f) {}
	~Shape() {}

	void Scale(float scaleFactor) {m_scaleFactor = scaleFactor;}
	void Rotate(DirectX::XMVECTOR* rotQuat);
	void Translate(DirectX::XMVECTOR* translator);
protected:
	DirectX::XMMATRIX m_modelMatrix;
	DirectX::XMVECTOR m_rotQuat;
	DirectX::XMVECTOR m_position;
	float m_scaleFactor;
};

class Square : Shape
{
public:
	Square(uint32_t length, uint32_t width) : Shape(), m_length(length), m_width(width){}
	~Square(){}

	void SetWidth(uint32_t width) {m_width = width;}
	void SetLength(uint32_t length) {m_length = length;}

	

private:
	uint32_t m_length;
	uint32_t m_width;
};

class Circle : Shape
{
	
private:
	void Subdivide();
};
