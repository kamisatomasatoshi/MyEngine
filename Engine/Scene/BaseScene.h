#pragma once
#include "WinApp.h"
#include"DirectXCore.h"

class SceneManager;

//�V�[�����N���X
class BaseScene {

public:
	//virtual�����o�֐�
	//������
	virtual void Initialize() = 0;

	//�X�V
	virtual void Update() = 0;

	virtual void PostEffectDraw()=0;

	//�`��
	virtual void Draw() = 0;

	//�`��
	virtual void Finalize() = 0;

public://�����o�֐�

	virtual ~BaseScene() = default;

	virtual void SetSceneManager(SceneManager* sceneManager) { sceneManager_ = sceneManager; };


private:
	//�V�[���}�l�[�W���[
	SceneManager* sceneManager_ = nullptr;

};