#pragma once
#include "Framework.h"

// �Q�[���S��
class MainGame : public Framework
{
public: // �����o�֐�
	MainGame() = default;
	~MainGame() = default;

	// ������
	void Initialize() override;
	// �I��
	void Finalize() override;
	// �X�V
	void Update() override;
	//PostEffectDraw
	void PostEffectDraw() override; 
	// �`��
	void Draw() override;

private:
	// �Q�[���I���t���O
	bool endRequest_ = false;
};
