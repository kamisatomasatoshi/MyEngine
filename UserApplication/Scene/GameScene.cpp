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
#include "LevelLoader.h"

GameScene::GameScene() {}
GameScene::~GameScene() {
	//model_.reset();
	delete model1;
	delete camera;
	delete object1;
	//safe_delete(levelData_);
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
	
	camera->SetTarget(cPos);
	camera->SetDistance(100.0f);

	model1 = FbxLoader::GetInstance()->LoadModelFromFile("boneTest");
	cubeModel = FbxLoader::GetInstance()->LoadModelFromFile("cube");
	//デバイスのセット
	Object3d::SetDevice(dxCommon_->GetDevice());
	//カメラのセット
	Object3d::SetCamera(camera);
	//グラフィックスパイプライン生成
	Object3d::CreateGraphicsPipeline();

	//レベルデータの読み込み
	levelData_ = LevelLoader::LoadFile("test");
	

	
	object1 = new Object3d;
	object1->Initialize();
	object1->SetModel(model1);
	object1->PlayAnimation();
	//cube = new Object3d;
	//cube->Initialize();
	//cube->SetModel(cubeModel);
	//cube->PlayAnimation();
	
	//models.insert(std::make_pair("Cube", cubeModel));
	

	// レベルデータからオブジェクトを生成、配置
	//for (auto& objectData : levelData_->objects) {
	//	// ファイル名から登録済みモデルを検索
	//	FbxModel* model = nullptr;
	//	decltype(models)::iterator it = models.find(objectData.fileName);
	//	if (it != models.end()) {
	//		model = it->second;
	//	}

	//	// モデルを指定して3Dオブジェクトを生成
	//	Object3d* newObject = Object3d::Create();

	//	// 座標
	//	DirectX::XMFLOAT3 pos;
	//	DirectX::XMStoreFloat3(&pos, objectData.translation);
	//	newObject->SetPosition(pos);

	//	// 回転角
	//	DirectX::XMFLOAT3 rot;
	//	DirectX::XMStoreFloat3(&rot, objectData.rotation);
	//	newObject->SetRotation(rot);

	//	// 座標
	//	DirectX::XMFLOAT3 scale;
	//	DirectX::XMStoreFloat3(&scale, objectData.scaling);
	//	newObject->SetScale(scale);
	//	
	//	// 配列に登録
	//	objects.push_back(newObject);
	//}

	
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
	if (input_->PushKey(DIK_A))
	{
		cPos.x -= move.x;
	}
	if (input_->PushKey(DIK_D))
	{
		cPos.x += move.x;
	}
	if (input_->PushKey(DIK_W))
	{
		cPos.z += move.z;
	}
	if (input_->PushKey(DIK_S))
	{
		cPos.z -= move.z;
	}
	camera->SetTarget(cPos);
	//全ての衝突をチェック
	//collisionManager->CheckAllCollisions();

	/*for (auto& object : objects) {
		object->Update();
	}*/

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
	//object1->Draw(commandList);

	/*for (auto& object : objects) {
		object->Draw(commandList);
	}*/
	

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

