#pragma once
#include "WorldTransform.h"
#include "Model.h"
#include <assert.h>
#include "affin.h"



class Enemy {

public:
	/// <summary>
	/// ������
	/// </summary>
	void Initialize(Model* model);

	/// <summary>
	/// �X�V
	/// </summary>
	void Move();

	/// <summary>
	/// �X�V
	/// </summary>
	void Update();

	/// <summary>
	/// �`��
	/// </summary>
	void Draw(ViewProjection viewProjection_);

	//����ł邩
	bool IsDead() const { return isDead_; }

	//������ς���
	Vector3 bVelocity(Vector3& velocity, WorldTransform& worldTransform);
	//���[���h���W���擾
	Vector3 GetWorldPosition();


private:

	//���[���h�ϊ��f�[�^
	WorldTransform worldTransform_;
	//���f��
	std::unique_ptr<Model> model_ = nullptr;
	
	
	//�f�X�t���O
	bool isDead_ = false;


};