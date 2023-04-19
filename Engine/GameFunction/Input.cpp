#include "Input.h"
#include "WinApp.h"
#include <cassert>

#include <basetsd.h>
#include <commctrl.h>
#include <dbt.h>
#include <oleauto.h>
#include <shellapi.h>
#include <wbemidl.h>
#include <wrl/client.h>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

#include <dinputd.h>
#include <memory>

Input* Input::Input_ = nullptr;

Input* Input::GetInstance() {
	if (Input_ == nullptr)
	{
		Input_ = new Input();
	}

	return Input_;
}

Input::~Input() {
	if (devKeyboard_) {
		devKeyboard_->Unacquire();
	}
}

void Input::Destroy()
{
	delete Input_;
}

void Input::Initialize()
{
	WinApp* winApp = WinApp::GetInstance();

	hwnd_ = winApp->Gethwnd();
	HRESULT result = S_FALSE;

	// DirectInput�I�u�W�F�N�g�̐���
	result = DirectInput8Create(
		winApp->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dInput_, nullptr);
	assert(SUCCEEDED(result));

	// �L�[�{�[�h�f�o�C�X�̐���
	result = dInput_->CreateDevice(GUID_SysKeyboard, &devKeyboard_, NULL);
	assert(SUCCEEDED(result));

	// ���̓f�[�^�`���̃Z�b�g
	result = devKeyboard_->SetDataFormat(&c_dfDIKeyboard); // �W���`��
	assert(SUCCEEDED(result));

	// �r�����䃌�x���̃Z�b�g
	result = devKeyboard_->SetCooperativeLevel(
		hwnd_, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

	//�}�E�X�f�o�C�X�̏�����
	mouse = std::make_unique<Mouse>();
	mouse->Initialize(dInput_.Get());
}

void Input::Update()
{
	// �O��̃L�[���͂�ۑ�
	memcpy(keyPre, key, sizeof(key));

	// �L�[�{�[�h���̎擾�J�n
	devKeyboard_->Acquire();

	// �S�L�[�̓��͏�Ԃ��擾����
	devKeyboard_->GetDeviceState(sizeof(key), key);

	//�}�E�X�X�V
	mouse->Update();
}

bool Input::PushKey(BYTE keyNumber)
{
	// �w�肵���L�[�������Ă����true��Ԃ�
	if (key[keyNumber]) {
		return true;
	}

	// �����łȂ����false��Ԃ�
	return false;
}

bool Input::TriggerKey(BYTE keyNumber)
{
	// �w��L�[��O�t���[���ŉ����Ă��Ȃ��A���̃t���[���ŉ����Ă����true��Ԃ�
	if (!keyPre[keyNumber] && key[keyNumber]) {
		return true;
	}

	// �����łȂ����false��Ԃ�
	return false;
}

bool Input::ReleasedKey(BYTE keyNumber)
{
	// �w��L�[��O�t���[���ŉ����Ă��āA���̃t���[���ŉ����Ă��Ȃ����true��Ԃ�
	if (keyPre[keyNumber] && !key[keyNumber]) {
		return true;
	}

	// �����łȂ����false��Ԃ�
	return false;
}

bool Input::MouseInputTrigger(int button)
{
	return mouse->MouseInputTrigger(button);
}

bool Input::MouseInputing(int button)
{
	return mouse->MouseInput(button);
}

bool Input::MouseOffTrigger(int button)
{
	return mouse->MouseOffTrigger(button);
}

const Vector2 Input::GetMousePos()const
{
	return mouse->GetMousePos();
}

const Vector3 Input::GetMouseMove()
{
	return mouse->GetMouseMove();
}