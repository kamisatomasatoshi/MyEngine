#pragma once
#include "DirectXCore.h"
#include "GameScene.h"

#include "TextureManager.h"
#include "WinApp.h"
#include "Input.h"
#include "FPS.h"
#include "ImGuiManager.h"

#include "SceneManager.h"
#include "FbxModel.h"

#include <memory>

class Framework {
public:
	//virtual�����o�֐�
	//������
	virtual void Initialize();

	//�X�V
	virtual void Update();

	//�|�X�g�G�t�F�N�g
	virtual void PostEffectDraw()=0;

	//�`��
	virtual void Draw() = 0;

	//�I������
	virtual void Finalize();

	//�I���`�F�b�N
	virtual bool isPlayMyGame();

public://�f�X�g���N�^
	virtual ~Framework() = default;

public:
	//�����o�֐�
	//���s
	void Run();

	WinApp* GetWinApp() { return winApp_; }
	DirectXCore* GetDirectXCore() { return directXCore_; }

protected:

	//�V���O���g��
	WinApp* winApp_ = nullptr;
	DirectXCore* directXCore_ = nullptr;
	Input* input_ = nullptr;
	SceneManager* sceneManager_;
	TextureManager* TextureManager_ = nullptr;


	//�V���O���g���ȊO
	std::unique_ptr<ImGuiManager> imGui;
	std::unique_ptr <FPS> fps;
	std::unique_ptr <AbstractSceneFactory> sceneFactory_;
	std::unique_ptr<FbxModel> fbx;



	bool isEndRequst = false;
};