#pragma once
#include "Model.h"
#include "WorldTransform.h"
///<summary>
///���H
/// </summary>
class Load {
public:
	///<summary>
	///������
	/// </summary>
	void Initialize(Model* model);

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
	WorldTransform worldTransform_[30];
	//���f��
	Model* model_;

	float speed_;
};