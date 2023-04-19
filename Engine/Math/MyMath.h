#pragma once
#include"Vector3.h"
#include"Matrix4.h"
#include "WinApp.h"
#include <DirectXMath.h>
#include <Vector4.h>

//�֗��n�܂Ƃ�
namespace MyMath {

	const float PI = 3.141592f;

	Matrix4 Initialize();
	Matrix4 Scale(const Vector3& scale);
	Matrix4 Rotation(const Vector3& rotation, int X_1_Y_2_Z_3_XYZ_6);
	Matrix4 Translation(const Vector3& move);

	//float MyMathToRadian(float x) { return x * (PI / 180); }

	//�x�N�g���̐��K��(�����؂�ɂ��)
	Vector3 vector3Normalize(const Vector3& v);
	//�x�N�g���ƍs��̊|���Z(�o��Vector3)
	Vector3 MatVector(Matrix4 matrix4, Vector3 vector3);
	Vector3 MulVector3(Vector3 vector3, Vector3 s);

	const Vector3 SubVec(Vector3 v, Vector3 v2);

	Vector3 GetWorldTransform(Matrix4 matrix4);
	const Vector3 AddVector3(const Vector3 v1, const Vector3 v2);
	Vector3 DivVecMat(const Vector3& vector3, const Matrix4& matrix4);


	Vector4 Vec4Mat4Mul(const Vector4& vec, const Matrix4& mat);

	Matrix4 setViewportMat(WinApp* window, const Vector3& v);
	Matrix4 MatrixInverse(Matrix4 pOut);
	Matrix4 MakeInverse(const Matrix4* mat);

	Matrix4 ConvertXMMATtoMat4(DirectX::XMMATRIX XMMatrix);

	Matrix4 LookAtLH(Vector3 eye, Vector3 target, Vector3 up);
	Matrix4 PerspectiveFovLH(float fovAngleY, float  aspectRatio, float  nearZ, float farZ);

	Matrix4 MakeIdentity();

	// �l��͈͓��ɔ[�߂�
	float Clamp(float Value, const float low, const float high);

	Matrix4 Matrix4Orthographic(
		float viewLeft, float viewRight, float viewBottom, float viewTop, float nearZ, float farZ);

	float GetAngle(float angle);

};