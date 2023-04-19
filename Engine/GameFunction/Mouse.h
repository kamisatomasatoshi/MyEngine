#pragma once
#include"ErrorException.h"
#include <dinput.h>
#include"Vector2.h"
#include"Vector3.h"
#include"WinApp.h"
#include <wrl.h>
#include "MyMath.h"


class Mouse
{
private:
	//�}�E�X
	Microsoft::WRL::ComPtr<IDirectInputDevice8> mouseDevice;
	DIMOUSESTATE2 mouseState = {};
	DIMOUSESTATE2 oldMouseState = {};
	//�}�E�X�̃Q�[����ԓ��ł̃��C
	Vector2 mousePos;
	Vector3 mouseMove;

public:

	/// <summary>
	/// ������
	/// </summary>
	void Initialize(IDirectInput8* dinput);

	/// <summary>
	/// �X�V
	/// </summary>
	void Update();

	/// <summary>
	/// �}�E�X�{�^���̃g���K�[����
	/// </summary>
	/// <param name="button">�`�F�b�N�������{�^��</param>
	/// <returns>��������</returns>
	bool MouseInputTrigger(int button);

	/// <summary>
	/// �}�E�X�{�^���̓���
	/// </summary>
	/// <param name="button">�`�F�b�N�������{�^��</param>
	/// <returns>��������</returns>
	bool MouseInput(int button);

	/// <summary>
	/// �}�E�X�{�^���̗������u��
	/// </summary>
	/// <param name="button">�`�F�b�N�������{�^��</param>
	/// <returns>��������</returns>
	bool MouseOffTrigger(int button);

	/// <summary>
	/// �}�E�X�̈ʒu
	/// </summary>
	/// <returns>�}�E�X�̈ʒu</returns>
	const Vector2 GetMousePos()const;

	/// <summary>
	/// �}�E�X�̈ʒu
	/// </summary>
	/// <returns>�}�E�X�̈ʒu</returns>
	const Vector3 GetMouseMove();

	char PADDING[4];
};