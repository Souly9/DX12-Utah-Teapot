#pragma once

class Camera
{
public:
	Camera();
	Camera(float fov, float zNear, float zFar, float aspectRatio, DirectX::XMVECTOR pos, DirectX::XMVECTOR rot);
	virtual ~Camera();

	// Updates all matrices
	void UpdateMatrices();

	void LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR upDirection);
	void Translate(DirectX::FXMVECTOR newPos);
	
	/**
	 * Getters for the camera's matrices
	 */
	DirectX::XMMATRIX GetViewMatrix() const;
	DirectX::XMMATRIX GetProjectionMatrix() const;
	DirectX::XMMATRIX GetViewProjectionMatrix() const;

	/**
	 * Getters and Setters for camera variables needed to adjust the matrices
	 */
	void set_FoV(float fov);
	// Using the vector calling convention as we're pasÅLsing in a vector solely used for calculations
	void XM_CALLCONV SetPositions(DirectX::FXMVECTOR position);
	DirectX::XMFLOAT3 GetPosition() const;

	void XM_CALLCONV SetRotation(DirectX::FXMVECTOR rotation);
	DirectX::XMVECTOR GetRotation() const;

private:

	__declspec(align(16)) struct AlignedCameraData
	{
		DirectX::XMVECTOR m_position;
		DirectX::XMVECTOR m_rotationQuat;
		
		DirectX::XMMATRIX m_viewMatrix;
		DirectX::XMMATRIX m_projMatrix;

		DirectX::XMMATRIX m_viewProjMatrix;
	};
	AlignedCameraData *m_pData;

	DirectX::XMFLOAT3 m_positionFloat;
	float m_FoV;
	float m_zNear;
	float m_zFar;
	float m_AspectRatio;

	mutable bool m_needsUpdate;
};

