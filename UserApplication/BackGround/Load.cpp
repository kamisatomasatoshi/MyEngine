#include "Load.h"
#include <cassert>
#include "Affin.h"

void Load::Initialize(Model* model) {
	//NULL�|�C���^�`�F�b�N
	assert(model);

	//�����Ƃ��Ď󂯎�����f�[�^�������o�ϐ��ɋL�^����
	model_ = model;

	//�������W���Z�b�g
	for (int i = 0; i < 30; i++) {
		float z = i * 39 - 117;
		worldTransform_[i].translation_ = Vector3{ 0,-0.9,z };

		//���[���h�ϊ��̏�����
		worldTransform_[i].Initialize();

		worldTransform_[i].scale_ = Vector3{ 10,10,10 };
	}

	speed_ = 3;
}


void Load::Update(float speed) {
	for (int i = 0; i < 30; i++) {
		worldTransform_[i].translation_.z -= speed;

		if (worldTransform_[i].translation_.z < -117) {
			worldTransform_[0].translation_.z = worldTransform_[29].translation_.z + (37 - speed);
			if (i != 0) {
				worldTransform_[i].translation_.z = worldTransform_[i - 1].translation_.z + 39;
			}
		}
		//�s��X�V
		AffinTrans::affin(worldTransform_[i]);
		worldTransform_[i].TransferMatrix();
	}
}

void Load::Demo() {
	for (int i = 0; i < 30; i++) {
		worldTransform_[i].translation_.z -= 1;

		if (worldTransform_[i].translation_.z < -117) {
			worldTransform_[0].translation_.z = worldTransform_[29].translation_.z + (37 - 2);
			if (i != 0) {
				worldTransform_[i].translation_.z = worldTransform_[i - 1].translation_.z + 39 - 5;
			}
		}
		//�s��X�V
		AffinTrans::affin(worldTransform_[i]);
		worldTransform_[i].TransferMatrix();
	}
}


void Load::Draw(ViewProjection viewProjection) {
	//3D���f����`��
	for (int i = 0; i < 30; i++) {
		model_->Draw(worldTransform_[i], viewProjection);
	}
}