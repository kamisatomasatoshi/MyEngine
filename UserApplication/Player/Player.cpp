#include "Player.h"
#include "WinApp.h"
#include"MyMath.h"
#include "CollisionManager.h"
#include <CollisionAttribute.h>


Player::Player()
{
}

Player::~Player()
{
	
}

void Player::Initialize() {
	
	playerModel = FbxLoader::GetInstance()->LoadModelFromFile("cube");
	player = new Object3d;
	player->Initialize();
	player->SetModel(playerModel);
	player->SetPosition(pos);
	
	

	for (int i = 0; i < SphereCount; i++) {
		// コリジョンマネージャに追加
		AttackCollider[i] = new SphereCollider(Vector4(0, radius, 0, 0), radius);
		CollisionManager::GetInstance()->AddCollider(AttackCollider[i]);
	}

	//worldTransform_.translation_ = { 0,0,-100 };

	//ワールド変換の初期化
	worldTransform_.Initialize();
	oldWorldTransform_.Initialize();
	

	collider->Update(worldTransform_.matWorld_);
	collider->SetAttribute(COLLISION_ATTR_ALLIES);

	

	worldTransform_.TransferMatrix();
	oldWorldTransform_.TransferMatrix();
	
}


void Player::Update(const ViewProjection& viewProjection) {

	Move();

	worldTransform_.TransferMatrix();
	oldWorldTransform_.TransferMatrix();
	

	collider->Update(worldTransform_.matWorld_);
}

void Player::Move() {
	if (input_->PushKey(DIK_A))
	{
		pos.x -= moveSpeed.x;
	}
	if (input_->PushKey(DIK_D))
	{
		pos.x += moveSpeed.x;
	}
	if (input_->PushKey(DIK_W))
	{
		pos.z += moveSpeed.z;
	}
	if (input_->PushKey(DIK_S))
	{
		pos.z -= moveSpeed.z;
	}

	player->SetPosition(pos);
}







void Player::Draw(ViewProjection viewProjection_, ID3D12GraphicsCommandList* cmdlist) {

	
	//playerModel_->Draw(worldTransform_, viewProjection_);
	player->Draw(cmdlist);
	
}




Vector3 Player::bVelocity(Vector3 velocity, WorldTransform& worldTransform) {

	Vector3 result = { 0, 0, 0 };


	result.x = velocity.x * worldTransform.matWorld_.m[0][0] +
		velocity.y * worldTransform.matWorld_.m[1][0] +
		velocity.z * worldTransform.matWorld_.m[2][0];

	result.y = velocity.x * worldTransform.matWorld_.m[0][1] +
		velocity.y * worldTransform.matWorld_.m[1][1] +
		velocity.z * worldTransform.matWorld_.m[2][1];

	result.z = velocity.x * worldTransform.matWorld_.m[0][2] +
		velocity.y * worldTransform.matWorld_.m[1][2] +
		velocity.z * worldTransform.matWorld_.m[2][2];

	return result;
}

void Player::Collision()
{
	
}



Vector3 Player::GetWorldPosition() {

	//ワールド座標を入れる変数
	Vector3 worldPos;
	//ワールド行列移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}


