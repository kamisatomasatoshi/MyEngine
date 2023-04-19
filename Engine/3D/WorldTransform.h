#pragma once
#include "Vector3.h"
#include "Matrix4.h"
#include "MyMath.h"
#include <d3d12.h>
#include <wrl.h>
#include "Quaternion.h"

// �萔�o�b�t�@�p�f�[�^�\����
struct ConstBufferDataWorldTransform {
	Matrix4 matWorld;           // ���[�J�� �� ���[���h�ϊ��s��
};

/// <summary>
/// ���[���h�ϊ��f�[�^
/// </summary>
struct WorldTransform {
	// �萔�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;
	// �}�b�s���O�ς݃A�h���X
	ConstBufferDataWorldTransform* constMap = nullptr;
	// ���[�J���X�P�[��
	Vector3 scale_ = { 1, 1, 1 };
	// X,Y,Z�����̃��[�J����]�p
	Vector3 rotation_ = { 0, 0, 0 };
	// ���[�J�����W
	Vector3 translation_ = { 0, 0, 0 };
	//�C�ӎ��̃��[�J����]
	Quaternion quaternion = { 0,0,0,0 };
	// ���[�J�� �� ���[���h�ϊ��s��
	Matrix4 matWorld_;
	//���̕��̂̌����Ă������
	Vector3 look = { 0,0,0 };
	Vector3 lookBack = { 0,0,0 };
	Vector3 lookRight = { 0,0,0 };
	Vector3 lookLeft = { 0,0,0 };
	// �e�ƂȂ郏�[���h�ϊ��ւ̃|�C���^
	const WorldTransform* parent_ = nullptr;

	Matrix4 matRot;

	//��]���I�C���[�p�ɂ���]��
	bool isEuler = false;

	/// <summary>
	/// ������
	/// </summary>
	void Initialize();
	/// <summary>
	/// �萔�o�b�t�@����
	/// </summary>
	void CreateConstBuffer();
	/// <summary>
	/// �}�b�s���O����
	/// </summary>
	void Map();
	/// <summary>
	/// �s���]������
	/// </summary>
	void TransferMatrix();

	void SetRot(const Vector3& rot);

	void SetMatRot(const Matrix4& mat);

	void MoveRot(const Vector3& move);

	void SetQuater(const Quaternion& quater);

	void MoveQuater(const Quaternion& move);
	
	Quaternion& GetQuaternion();
	
	Vector3 GetLook(Matrix4 matRot,Vector3 at);
};