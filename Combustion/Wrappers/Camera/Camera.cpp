#include <stdafx.h>
#include "Camera.h"

Camera::Camera()
{
}

Camera::Camera(float fov, float zNear, float zFar, float aspectRatio,
	DirectX::XMVECTOR pos, 
	DirectX::XMVECTOR rot) 
	: m_FoV(fov),
	m_zNear(zNear),
	m_zFar(zFar),
	m_AspectRatio(aspectRatio)
{
	m_pData = (AlignedCameraData*)_aligned_malloc(sizeof(AlignedCameraData), 16);
	m_pData->m_position = pos;
	m_pData->m_rotationQuat = rot;
	DirectX::XMStoreFloat3(&m_positionFloat, m_pData->m_position);
}

Camera::~Camera()
{
	_aligned_free(m_pData);
}

void Camera::UpdateMatrices()
{
	//m_pData->m_viewMatrix = DirectX::XMMatrixLookAtLH(DirectX::XMVectorSet(0, 0, -50, 1), DirectX::XMVectorSet(0, 0, 10, 1), DirectX::XMVectorSet(0, 1, 0, 0));
	if(m_needsUpdate)
	{
		m_pData->m_projMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(m_FoV), m_AspectRatio, 0.1f, 100.0f);;
		m_pData->m_viewProjMatrix = DirectX::XMMatrixMultiply(m_pData->m_viewMatrix, m_pData->m_projMatrix);
	}
}

void Camera::LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR upDirection)
{
	using namespace DirectX;
	DirectX::FXMVECTOR zAxis = DirectX::XMVector3Normalize(target - pos);
	DirectX::FXMVECTOR xAxis =  DirectX::XMVector3Normalize(
		DirectX::XMVector3Cross(upDirection, zAxis));
	DirectX::FXMVECTOR yAxis = DirectX::XMVector3Cross(zAxis, xAxis);

	DirectX::FXMVECTOR zeroVec = DirectX::FXMVECTOR{0,0,0,1};
	DirectX::FXMMATRIX orientationMatrix{xAxis, yAxis, zAxis, zeroVec};
	DirectX::FXMMATRIX translationMatrix{1, 0, 0, 0,
										 0, 1, 0, 0,
										 0, 0, 1, 0,
										 -pos.m128_f32[0], -pos.m128_f32[1], -pos.m128_f32[2], 1};
	
	m_pData->m_viewMatrix = DirectX::XMMatrixMultiply(translationMatrix, orientationMatrix);
	//m_pData->m_viewMatrix = DirectX::XMMatrixLookAtLH(pos, target, upDirection);
	m_needsUpdate = true;
}

void Camera::Translate(DirectX::FXMVECTOR newPos)
{
	m_pData->m_position = DirectX::XMVectorAdd(m_pData->m_position, newPos);
}

DirectX::XMMATRIX Camera::GetViewMatrix() const
{
	return m_pData->m_viewMatrix;
}

DirectX::XMMATRIX Camera::GetProjectionMatrix() const
{
	return m_pData->m_projMatrix;
}

DirectX::XMMATRIX Camera::GetViewProjectionMatrix() const
{
	 return m_pData->m_viewProjMatrix;
}

void Camera::set_FoV(float fov)
{
	m_FoV = fov;
}

void Camera::SetPositions(DirectX::FXMVECTOR position)
{
	m_pData->m_position = position;
}

DirectX::XMFLOAT3 Camera::GetPosition() const
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMStoreFloat3(&pos, m_pData->m_position);
	return pos;
}

void Camera::SetRotation(DirectX::FXMVECTOR rotation)
{
	m_pData->m_rotationQuat = rotation;
}

DirectX::XMVECTOR Camera::GetRotation() const
{
	return m_pData->m_rotationQuat;
}
