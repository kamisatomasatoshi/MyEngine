﻿#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <random>
#include <fstream>
#include "FbxLoader.h"
#include"ImGuiManager.h"
#include <CollisionAttribute.h>
#include "Collision.h"
#include"PostEffect.h"
#include "Object3d.h"


GameScene::GameScene() {}
GameScene::~GameScene() {
	//model_.reset();
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCore::GetInstance();
	winApp_ = WinApp::GetInstance();
	input_ = Input::GetInstance();

	camera = new DebugCamera(WinApp::window_width, WinApp::window_height, input_);
	//当たり判定
	collisionManager = CollisionManager::GetInstance();

	//model_.reset(Model::CreateFromOBJ("UFO", true));

	viewProjection_.eye = { 0,10,-10 };

	sceneManager_ = SceneManager::GetInstance();

	viewProjection_.Initialize();
	viewProjection_.UpdateMatrix();

	worldTransform_.Initialize();
	
	//FbxLoader::GetInstance()->LoadModelFromFile("boneTest");
	

	//boxCollision = std::make_unique<BoxCollision>();

	//model_->SetPolygonExplosion({ 0.0f,1.0f,0.0f,0.0f });
	UINT tex = TextureManager::GetInstance()->Load("effect1.png");

	//stageModel_.reset(Model::CreateFromOBJ("stage", true));

	//stageWorldTransform_.Initialize();


	//stageWorldTransform_.scale_ = { 52,50,52 };

	//stageWorldTransform_.translation_ = Vector3(0, -2.1f, 0);
	
	camera->SetTarget({ 0,20,0 });
	camera->SetDistance(100.0f);

	model1 = FbxLoader::GetInstance()->LoadModelFromFile("boneTest");

	//デバイスのセット
	Object3d::SetDevice(dxCommon_->GetDevice());
	//カメラのセット
	Object3d::SetCamera(camera);
	//グラフィックスパイプライン生成
	Object3d::CreateGraphicsPipeline();
	
	object1 = new Object3d;
	object1->Initialize();
	object1->SetModel(model1);
	
}

void GameScene::Update() {

	if(ImGui::Button("break")) {
		static int a = 0;
		a++;
	}
	camera->Update();
	viewProjection_.UpdateMatrix();
	//object1->PlayAnimation();
	object1->Update();

	//全ての衝突をチェック
	//collisionManager->CheckAllCollisions();

}

void GameScene::PostEffectDraw()
{
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	PostEffect::PreDrawScene(commandList);

	ParticleManager::PreDraw(commandList);

	//

	ParticleManager::PostDraw();

	PostEffect::PostDrawScene();
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画

	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	//// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);
	//ここから描画開始
	


	//stageModel_->Draw(stageWorldTransform_,viewProjection_);
	object1->Draw(commandList);
	

	//3Dオブジェクト描画後処理
	Model::PostDraw();

	FbxModel::PreDraw(commandList);

	//fbxmodel->Draw(worldTransform_, viewProjection_);

	FbxModel::PostDraw();


#pragma endregion

#pragma region ポストエフェクトの描画

	PostEffect::Draw(commandList);

#pragma endregion

#pragma region 前景スプライト描画



#pragma endregion
}

void GameScene::Finalize()
{
}

