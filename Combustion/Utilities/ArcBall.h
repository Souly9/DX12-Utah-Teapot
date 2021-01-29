#pragma once
#include <stdafx.h>
#include <DirectXMath.h>

/**
 * Header only utility class to implement rotation through ArcBall
 */
class ArcBall
{
public:
	ArcBall() :
		m_isDragging(false),
		m_isDirty(true),
		m_mousePos(),
		m_mouseDown(),
		m_arcPosFrom(),
		m_arcPosTo(),
		m_rotationMatrix(),
		m_rotQuat(DirectX::XMVectorSet(0, 0, 0, 1)),
		m_rotQuatDown(DirectX::XMVectorSet(0, 0, 0, 1)),
		m_currentQuat()
	{
	}

	~ArcBall()
	{
	}

	void UpdatePos(const DirectX::XMFLOAT2& center, float radius)
	{
		m_center = center;
		m_radius = radius;
		m_mouseDown = m_center;
		m_mousePos = m_center;
	}

	void SetMousePos(const DirectX::XMFLOAT2& pos)
	{
		m_mousePos = pos;
		m_isDirty = true;
	}

	void StartDragging()
	{
		m_isDragging = true;
		m_mouseDown = m_mousePos;
		m_isDirty = true;
	}

	void StopDragging()
	{
		m_isDragging = false;
		m_rotQuatDown = m_currentQuat;
		m_isDirty = true;
	}

	void GetRotationMatrix(DirectX::XMMATRIX& rslt)
	{
		if (m_isDirty)
		{
			m_arcPosFrom = ConvertToSphere(m_mouseDown);
			m_arcPosTo = ConvertToSphere(m_mousePos);
			if (m_isDragging)
			{
				m_rotQuat = FromToQuat(m_arcPosFrom, m_arcPosTo);
				m_currentQuat = DirectX::XMQuaternionMultiply(m_rotQuat, m_rotQuatDown);
			}
			m_rotationMatrix = DirectX::XMMatrixRotationQuaternion(m_currentQuat);
		}
		rslt = m_rotationMatrix;
	}

	void GetRotationMatrixAdditive(const DirectX::XMVECTOR& otherQuat, DirectX::XMMATRIX& rslt)
	{
		if (m_isDirty)
		{
			m_arcPosFrom = ConvertToSphere(m_mouseDown);
			m_arcPosTo = ConvertToSphere(m_mousePos);
			if (m_isDragging)
			{
				m_rotQuat = FromToQuat(m_arcPosFrom, m_arcPosTo);
				m_currentQuat = DirectX::XMQuaternionMultiply(m_rotQuatDown,m_rotQuat);
			}
		}
		DirectX::XMVECTOR addQuat = DirectX::XMQuaternionNormalizeEst(otherQuat);
		m_currentQuat = DirectX::XMQuaternionMultiply(m_currentQuat, addQuat);

		m_rotationMatrix = DirectX::XMMatrixRotationQuaternion(m_currentQuat);
		rslt = m_rotationMatrix;
	}

private:
	DirectX::XMVECTOR ConvertToSphere(const DirectX::XMFLOAT2& mousePos)
	{
		DirectX::XMVECTOR scaledDownMouse = DirectX::XMVectorSet(
			(mousePos.x - m_center.x) / m_radius,
			(mousePos.y - m_center.y) / m_radius,
			0,
			1
		);
		float dist = DirectX::XMVector2Dot(scaledDownMouse, scaledDownMouse).m128_f32[0];

		if (dist > 1.0f)
		{
			// Distance bigger than radius
			dist = sqrt(dist);
			scaledDownMouse = DirectX::XMVectorDivide(scaledDownMouse,
			                                          DirectX::XMVectorSet(dist, dist, dist, 1));
			scaledDownMouse.m128_f32[2] = 0;
		}
		else
		{
			scaledDownMouse.m128_f32[2] = sqrt(1.0 - dist);
		}

		return scaledDownMouse;
	}

	// Half-way Quaternion based procedure to find the rotation between two vectors
	// Simplified the equations to get rid of the square roots and alike
	DirectX::XMVECTOR FromToQuat(const DirectX::FXMVECTOR& from, const DirectX::FXMVECTOR& to)
	{
		return DirectX::XMVectorSet(
			from.m128_f32[1] * to.m128_f32[2] - from.m128_f32[2] * to.m128_f32[1],
			from.m128_f32[2] * to.m128_f32[0] - from.m128_f32[0] * to.m128_f32[2],
			from.m128_f32[0] * to.m128_f32[1] - from.m128_f32[1] * to.m128_f32[0],
			from.m128_f32[0] * to.m128_f32[0] + from.m128_f32[1] * to.m128_f32[1] + from.m128_f32[2] * to.m128_f32[2]);
	}

	float m_radius;
	
	bool m_isDragging;
	bool m_isDirty;
	
	DirectX::XMFLOAT2 m_center;
	DirectX::XMFLOAT2 m_mousePos;
	DirectX::XMFLOAT2 m_mouseDown;
	
	DirectX::XMVECTOR m_arcPosFrom;
	DirectX::XMVECTOR m_arcPosTo;
	DirectX::XMVECTOR m_rotQuat;
	DirectX::XMVECTOR m_rotQuatDown;
	DirectX::XMVECTOR m_currentQuat;

	DirectX::XMMATRIX m_rotationMatrix;
};
