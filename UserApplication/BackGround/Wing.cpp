#include "wing.h"
#include <cassert>
#include "Affin.h"

void Wing::Initialize(Model* model) {
	//NULL�|�C���^�`�F�b�N
	assert(model);

	//�����Ƃ��Ď󂯎�����f�[�^�������o�ϐ��ɋL�^����
	model_ = model;

	//�������W���Z�b�g
	worldTransform_.translation_ = Vector3{ 0,0,0 };

	//���[���h�ϊ��̏�����
	worldTransform_.Initialize();

	worldTransform_.scale_ = Vector3{ 5,5,10 };

	//�ϐ�������
	make = false;
	time = 0;
}

void Wing::Update(Vector3 pos) {
	//�v���C���[�̍��W�����킹��
	worldTransform_.translation_ = pos;
	worldTransform_.rotation_.z += 10;

	//�^�C�}�[�J�E���g
	if (time > 0) {
		time--;
	}
	else {
		make = false;
	}

	//�s��X�V
	AffinTrans::affin(worldTransform_);
	worldTransform_.TransferMatrix();

}

void Wing::Draw(ViewProjection viewProjection) {
	if (make == true) {
		//3D���f����`��
		model_->Draw(worldTransform_, viewProjection);
	}
}

void Wing::Distance() {
	make = true;
	time = 1;
}