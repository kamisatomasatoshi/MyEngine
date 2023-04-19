#pragma once
#include "Model.h"
#include "WorldTransform.h"
///<summary>
///��
/// </summary>
class Wing {
public:
	///<summary>
	///������
	/// </summary>
	void Initialize(Model* model);

	///<summary>
	///�X�V
	/// </summary>
	void Update(Vector3 pos);

	///<summary>
	///�`��
	/// </summary>
	void Draw(ViewProjection viewProjection);

	//����
	void Distance();

private:
	//���[���h�ϊ��f�[�^
	WorldTransform worldTransform_;
	//���f��
	Model* model_;
	//���Ԃƃt���O
	bool make;
	int time;
};