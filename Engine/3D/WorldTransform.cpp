#include "WorldTransform.h"
#include "DirectXCore.h"
#include <cassert>
#include <Quaternion.h>

void WorldTransform::Initialize() {
	CreateConstBuffer();
	Map();
	TransferMatrix();
}

void WorldTransform::CreateConstBuffer() {

	HRESULT result;

	// �q�[�v�v���p�e�B
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// ���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC resourceDesc =
		CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataWorldTransform) + 0xff) & ~0xff);

	// �萔�o�b�t�@�̐���
	result = DirectXCore::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProps, // �A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuff_));
	assert(SUCCEEDED(result));

}

void WorldTransform::Map() {

	//�萔�o�b�t�@�̃}�b�s���O
	HRESULT result = constBuff_->Map(0, nullptr, (void**)&constMap);
	assert(SUCCEEDED(result));

}

void WorldTransform::TransferMatrix() {

	Matrix4 matScale, matTrans;

	//�X�P�[���A��]�A���s�ړ��s��̌v�Z
	matScale = MyMath::Scale(scale_);
	
	//�I�C���[�p�ŉ�]���Ă���Ȃ��]�p�̃}�g���b�N�X��������
	if (isEuler) {
		matRot = MyMath::Initialize();
		//matRot *= MyMath::Rotation(rotation_, 6);
		matRot = quaternion.Rotate();
	}

	matTrans = MyMath::Translation(translation_);

	//���[���h�s��̍���
	matWorld_ = MyMath::Initialize();//�ό`�����Z�b�g
	matWorld_ *= matScale;//���[���h�s��ɃX�P�[�����O�𔽉f
	matWorld_ *= matRot;//���[���h�s��ɉ�]�𔽉f
	matWorld_ *= matTrans;//���[���h�s��ɕ��s�ړ��𔽉f
	
	look = GetLook(matRot, Vector3(-1,0,0));
	lookBack = GetLook(matRot, Vector3(1, 0, 0));
	lookRight = GetLook(matRot, Vector3(0, 0, 1));
	lookLeft = GetLook(matRot, Vector3(0, 0, -1));

	//�e�I�u�W�F�N�g�������
	if (parent_) {
		//�e�I�u�W�F�N�g�̃��[���h�s����|����
		matWorld_ *= parent_->matWorld_;
	}
	//�萔�o�b�t�@�ւ̃f�[�^�]��
	constMap->matWorld = matWorld_;

}


void WorldTransform::SetRot(const Vector3& rot)
{
	isEuler = true;
	rotation_ = rot;

	quaternion.SeTEuler(rotation_);
}

void WorldTransform::SetMatRot(const Matrix4& mat)
{
	//�I�C���[�p�̉�]�t���O��false��
	isEuler = false;
	matRot = mat;
}

void WorldTransform::MoveRot(const Vector3& move)
{
	rotation_ += move;

	quaternion.SeTEuler(rotation_);
}

void WorldTransform::SetQuater(const Quaternion& quater)
{
	quaternion = quater;

	rotation_ = quaternion.GetEulerAngles();
}

void WorldTransform::MoveQuater(const Quaternion& move)
{
	quaternion += move;

	rotation_ = quaternion.GetEulerAngles();
}

Quaternion& WorldTransform::GetQuaternion()
{
	return quaternion;
}

Vector3 WorldTransform::GetLook(Matrix4 matRot,Vector3 at)
{
	Vector3 Pos = MyMath::GetWorldTransform(matWorld_);
	Vector3 look_ = MyMath::MatVector(matRot, at);

	return Pos + look_;
}

