#include "DirectXCore.h"
#include "ViewProjection.h"
#include "WinApp.h"
#include <cassert>
#include <d3dx12.h>
#include "MyMath.h"
#include<DirectXMath.h>

void ViewProjection::Initialize() {
	CreateConstBuffer();
	Map();
	UpdateMatrix();
}

void ViewProjection::CreateConstBuffer() {
	HRESULT result;

	// �q�[�v�v���p�e�B
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// ���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC resourceDesc =
		CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataViewProjection) + 0xff) & ~0xff);

	// �萔�o�b�t�@�̐���
	result = DirectXCore::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProps, // �A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuff_));
	assert(SUCCEEDED(result));
}

void ViewProjection::Map() {
	// �萔�o�b�t�@�Ƃ̃f�[�^�����N
	HRESULT result = constBuff_->Map(0, nullptr, (void**)&constMap);
	assert(SUCCEEDED(result));
}

void ViewProjection::UpdateMatrix() {

	// �r���[�s��̐���
	Matrix4 tmp = MyMath::LookAtLH(eye, target, up);
	matView = MyMath::MatrixInverse(tmp);

	// �������e�ɂ��ˉe�s��̐���
	matProjection = MyMath::PerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ);

	cameraLook = target - eye;
	cameraLook.y = 0;
	cameraLook.norm();

	// �萔�o�b�t�@�ɏ�������
	constMap->view = matView;
	constMap->projection = matProjection;
	constMap->cameraPos = eye;

	//���_���W
	Vector3 eyePosition = eye;
	//�����_���WX
	Vector3 targetPosition = target;
	//(����)�����
	Vector3 upVector = up;

	//�J����Z��(���_����)
	Vector3 cameraAxisZ = targetPosition - eyePosition;

	//�x�N�g���𐳋K��
	cameraAxisZ.normalize();

	//�J������X��(�E����)
	Vector3 cameraAxisX;
	//X���͏������Z���̊O�ςŋ��܂�
	cameraAxisX = upVector.cross(cameraAxisZ);
	//�x�N�g���𐳋K��
	cameraAxisX.normalize();

	//�J������Y��(�����)
	Vector3 cameraAxisY;
	//Y���͏������Z���̊O�ςŋ��܂�
	cameraAxisY = cameraAxisZ.cross(cameraAxisX);
	//�x�N�g���𐳋K��
	cameraAxisY.normalize();

	//���_���W��-1���|�������W
	Vector3 reverseEyePosition = eyePosition * -1;
	//�J�����̈ʒu���烏�[���h���_�ւ̃x�N�g��(�J�������W�n)
	float tX = cameraAxisX.dot(reverseEyePosition);
	float tY = cameraAxisY.dot(reverseEyePosition);
	float tZ = cameraAxisZ.dot(reverseEyePosition);
	//��̃x�N�g���ɂ܂Ƃ߂�
	Vector3 translation = { tX, tY, tZ };

}
