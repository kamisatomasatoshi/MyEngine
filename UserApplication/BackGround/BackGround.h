#pragma once
#include "Model.h"
#include "WorldTransform.h"
///<summary>
///���H
/// </summary>
class BackGround {
public:
	///<summary>
	///������
	/// </summary>
	void Initialize(Model* modelLeft, Model* modelRight);

	///<summary>
	///�X�V
	/// </summary>
	void Update(float speed);

	///<summary>
	///�`��
	/// </summary>
	void Draw(ViewProjection viewProjection);

	/// <summary>
	/// �^�C�g����ʗp
	/// </summary>
	void Demo();

private:
	//���[���h�ϊ��f�[�^
	WorldTransform worldTransformRight_[2];
	WorldTransform worldTransformLeft_[2];
	//���f��
	Model* modelRight_;
	Model* modelLeft_;

	float speed_;
};