#include "background.h"
#include <cassert>
#include "Affin.h"

void BackGround::Initialize(Model* modelLeft, Model* modelRight) {
	//NULL�|�C���^�`�F�b�N
	assert(modelLeft);
	assert(modelRight);
	//�����Ƃ��Ď󂯎�����f�[�^�������o�ϐ��ɋL�^����
	modelLeft_ = modelLeft;
	modelRight_ = modelRight;
	//�������W���Z�b�g
	for (int i = 0; i < 2; i++) {
		float z = i * 12000;
		worldTransformRight_[i].translation_ = Vector3{ -218,-1,z };
		worldTransformLeft_[i].translation_ = Vector3{ 219,-1,z };

		//���[���h�ϊ��̏�����
		worldTransformRight_[i].Initialize();
		worldTransformLeft_[i].Initialize();

		worldTransformRight_[i].scale_ = Vector3{ 50,20,10 };
		worldTransformLeft_[i].scale_ = Vector3{ 50,20,10 };
	}

	speed_ = 3;
}


void BackGround::Update(float speed) {
	for (int i = 0; i < 2; i++) {
		worldTransformRight_[i].translation_.z -= speed;
		worldTransformLeft_[i].translation_.z -= speed;

		if (worldTransformRight_[i].translation_.z < -12000) {
			worldTransformRight_[i].translation_.z = 12000;
		}
		if (worldTransformLeft_[i].translation_.z < -12000) {
			worldTransformLeft_[i].translation_.z = 12000;
		}
		//�s��X�V
		AffinTrans::affin(worldTransformRight_[i]);
		worldTransformRight_[i].TransferMatrix();

		AffinTrans::affin(worldTransformLeft_[i]);
		worldTransformLeft_[i].TransferMatrix();
	}
}

void BackGround::Demo() {
	for (int i = 0; i < 2; i++) {
		worldTransformRight_[i].translation_.z -= 1;
		worldTransformLeft_[i].translation_.z -= 1;

		if (worldTransformRight_[i].translation_.z < -12000) {
			worldTransformRight_[i].translation_.z = 12000;
		}
		if (worldTransformLeft_[i].translation_.z < -12000) {
			worldTransformLeft_[i].translation_.z = 12000;
		}
		//�s��X�V
		AffinTrans::affin(worldTransformRight_[i]);
		worldTransformRight_[i].TransferMatrix();

		AffinTrans::affin(worldTransformLeft_[i]);
		worldTransformLeft_[i].TransferMatrix();
	}
}

void BackGround::Draw(ViewProjection viewProjection) {
	//3D���f����`��
	for (int i = 0; i < 2; i++) {
		modelRight_->Draw(worldTransformRight_[i], viewProjection);
		modelLeft_->Draw(worldTransformLeft_[i], viewProjection);
	}
}