#pragma once
#include "Object3d.h"
#include "DirectXCore.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "affin.h"
#include"ParticleManager.h"
#include "DebugCamera.h"
#include "BaseScene.h"
#include "SceneManager.h"

#include "FbxModel.h"

#include<memory>
#include<vector>

#include "BoxCollision.h"

//#include "Player.h"
//#include"Boss.h"
#include <CollisionManager.h>
//#include"MiniFish.h"

/// <summary>
/// ゲームシーン
/// </summary>
/// 
struct LevelData;

class GameScene : public BaseScene {
protected:
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;


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

	LevelData* levelData_=nullptr;

	FbxModel* model1 = nullptr;
	FbxModel* cubeModel = nullptr;

	Object3d* object1 = nullptr;
	Object3d* cube = nullptr;

	DebugCamera* camera = nullptr;

	std::unordered_map<std::string, FbxModel*>models;
	std::vector<Object3d*> objects;

	//ビュープロジェクション
	ViewProjection viewProjection_;
	//ワールド変換データ
	WorldTransform worldTransform_;
	//テクスチャハンドル
	uint32_t textureHandle_ = 0;
	// 3Dモデル
	//std::unique_ptr<Model> model_;

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
	XMFLOAT3 cPos = { 0,5,60 };
	XMFLOAT3 move = { 1,1,1 };

	

	// 3Dモデル
	//std::unique_ptr<Model> stageModel_;
	//ワールド変換データ
	//WorldTransform stageWorldTransform_;
};