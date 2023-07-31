#pragma once
#include "WorldTransform.h"
#include "Model.h"
#include <assert.h>
#include "Input.h"
#include "affin.h"
#include <memory>
#include "Sprite.h"
#include <list>
#include "BaseCollider.h"
#include <SphereCollider.h>
#include "Vector4.h"
#include <Easing.h>
#include"ParticleManager.h"
#include "Object3d.h"
class Player {

protected:

	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	Player();
	~Player();

	/// <summary>
	/// 初期化
	/// <summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// <summary>
	void Move();

	/// <summary>
	/// 更新
	/// <summary>
	void Update(const ViewProjection& viewProjection);


	/// <summary>
	/// 描画
	/// <summary>
	void Draw(ViewProjection viewProjection_, ID3D12GraphicsCommandList* cmdlist);

	

	Vector3 bVelocity(Vector3 velocity, WorldTransform& worldTransform);
	Vector3 GetWorldPosition();
	float GetRadius() { return radius; }

private:
	void Collision();
private:

	
	//ワールド変換データ
	WorldTransform worldTransform_;
	WorldTransform oldWorldTransform_;
	
	Input* input_ = nullptr;

	// コライダー
	BaseCollider* collider = nullptr;

	static const int SphereCount = 20;

	BaseCollider* AttackCollider[SphereCount];
	Vector3 colliderPos[SphereCount];
	Matrix4 worldSpherePos[SphereCount];
	bool makeColliders = false;
	//モデル
	Object3d* player;
	FbxModel* playerModel = nullptr;
	
	XMFLOAT3 pos = {10,0,0};
	XMFLOAT3 moveSpeed = { 3,3,3 };

	Matrix4 CameraRot;
	Matrix4 EnemyPos;
	Vector3 Rot;
	Vector3 Avoidance;
	Vector3 PlayerMoveMent;
	Vector3 cameraLook;

	Matrix4 ParticlePos;



	
	float x = 0;
	float radius = 2.0f;//当たり判定半径
	float Window_Width;
	float Window_Height;
	float playerSpeed = 0.01f;
	float playerAvoidance = 0.0f;




};