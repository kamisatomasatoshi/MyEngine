#pragma once
class EasingData
{
public:

	/// <summary>
	/// �^�C�}�[�𓮂����X�V����
	/// </summary>
	void Update();

	/// <summary>
	/// �C�[�W���O���J�n����֐�
	/// </summary>
	/// <param name="animationFrame">��f�ŃC�[�W���O���s����</param>
	void Start(float animationFrame);

	//�C�[�W���O���N�����Ă��邩�ǂ���(�i�s������0���傫��1��菬������)
	bool GetActive() { return (isActive && timeRate > 0.0f); }

	float GetTimeRate()const { return timeRate; }

private:
	long long nowCount = 0;		//PC���̌��ݎ���
	long long startCount = 0;
	long long endCount = 0;
	float maxTime = 0;
	float endTimer = 0;
	float timeRate = 0;	//�C�[�W���O�̐i�s����(0��0%�A1��100%)
	float isActive = false;

};

