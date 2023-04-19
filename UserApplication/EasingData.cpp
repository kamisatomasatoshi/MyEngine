#include "EasingData.h"
#include<time.h>
#include<cmath>

void EasingData::Update()
{



	if (!isActive) {
		//�A�N�e�B�u����Ȃ��Ȃ牽�����Ȃ�
		return;
	}

	//���ݎ������擾
	nowCount = clock();
	//�擾���ꂽ�C�[�W���O�J�n���ԂŏI�����Ԃ����܂�
	endCount = nowCount - startCount;
	//�^�C�}�[�̐i�݋��float�^�ɕϊ�
	endTimer = static_cast<float>(endCount) / 1'000.0f;
	//(�^�C�}�[�̐i�s�x/�ő厞��)�ŃC�[�W���O�̐i�݋���v�Z
	timeRate = endTimer / maxTime * 60.0f;
	//�i�s������1�ȏ�ɂȂ�����(�i�s�x100%)�C�[�W���O�I��
	if (timeRate >= 1.0f) {
		timeRate = 1.0f;
		isActive = false;
	}
}

void EasingData::Start(float animationFrame)
{
	startCount = clock();
	maxTime = animationFrame;
	isActive = true;
}
