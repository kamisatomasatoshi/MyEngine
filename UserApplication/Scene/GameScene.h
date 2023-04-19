#pragma once

#include "DirectXCore.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "affin.h"
#include"ParticleManager.h"

#include "BaseScene.h"
#include "SceneManager.h"

#include "FbxModel.h"

#include<memory>
#include<vector>

#include "BoxCollision.h"

//#include "Player.h"
//#include "GameCamera.h"
//#include"Boss.h"
#include <CollisionManager.h>
//#include"MiniFish.h"

enum class GamePhase {
	GameTutorial,	//チュートリアル
	GameMovie1,		//ムービー1(チュートリアル→ボス戦への遷移)
	GameBoss1,		//ボス戦第一形態
	GameMovie2,		//ムービー2(ボスの形態変化)
	GameBoss2,		//ボス戦第二形態
	GameMovie3,		//ムービー3(ボスが死ぬとき)
};

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene : public BaseScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

public:


	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update() override;

	void PostEffectDraw() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

	// 終了処理
	void Finalize() override;


private: // メンバ変数
	WinApp* winApp_ = nullptr;
	DirectXCore* dxCommon_ = nullptr;
	Input* input_ = nullptr;


	//ビュープロジェクション
	ViewProjection viewProjection_;
	//ワールド変換データ
	WorldTransform worldTransform_;
	//テクスチャハンドル
	uint32_t textureHandle_ = 0;
	// 3Dモデル
	std::unique_ptr<Model> model_;

	//当たり判定
	CollisionManager* collisionManager = nullptr;

	//ボックス当たり判定
	std::unique_ptr <BoxCollision> boxCollision;

	//シーンマネージャー
	SceneManager* sceneManager_ = nullptr;

	//Fbxモデル
	//std::unique_ptr<FbxModel> fbxmodel;

	//Boss boss;
	//デバッグによる生成用
	

	

	// 3Dモデル
	std::unique_ptr<Model> stageModel_;
	//ワールド変換データ
	WorldTransform stageWorldTransform_;
};