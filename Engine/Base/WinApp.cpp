#include "WinApp.h"
#include <string>
#include<imgui_impl_win32.h>


WinApp* WinApp::WinApp_ = nullptr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	//Imgui�p�E�B���h�E�v���V�[�W���Ăяo��
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
		return true;


	//���b�Z�[�W�ɉ����ăQ�[���ŗL�̏������s��
	switch (msg) {
		//�E�C���h�E���j�����ꂽ
	case WM_DESTROY:
		//OS�ɑ΂��āA�A�v���̏I����`����
		PostQuitMessage(0);
		return false;
	}

	//�W���̃��b�Z�[�W�������s��
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

WinApp* WinApp::GetInstance() {
	if (WinApp_ == nullptr)
	{
		WinApp_ = new WinApp();
	}

	return WinApp_;
}

void WinApp::Destroy()
{
	delete WinApp_;
}

void WinApp::MakeWindow(std::wstring title) {

	// �E�B���h�E�N���X�̐ݒ�
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc; // �E�B���h�E�v���V�[�W����ݒ�
	w.lpszClassName = title.c_str(); // �E�B���h�E�N���X��
	w.hInstance = GetModuleHandle(nullptr); // �E�B���h�E�n���h��
	w.hCursor = LoadCursor(NULL, IDC_ARROW); // �J�[�\���w��
	// �E�B���h�E�N���X��OS�ɓo�^����
	RegisterClassEx(&w);
	// �E�B���h�E�T�C�Y{ X���W Y���W ���� �c�� }
	RECT wrc = { 0, 0, window_width, window_height };
	// �����ŃT�C�Y��␳����
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	// �E�B���h�E�I�u�W�F�N�g�̐���
	hwnd = CreateWindow(w.lpszClassName, // �N���X��
		title.c_str(), // �^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW, // �W���I�ȃE�B���h�E�X�^�C��
		100, // �\��X���W(OS�ɔC����)
		50, // �\��Y���W(OS�ɔC����)
		wrc.right - wrc.left, // �E�B���h�E����
		wrc.bottom - wrc.top, // �E�B���h�E�c��
		nullptr, // �e�E�B���h�E�n���h��
		nullptr, // ���j���[�n���h��
		w.hInstance, // �Ăяo���A�v���P�[�V�����n���h��
		nullptr); // �I�v�V����
	// �E�B���h�E��\����Ԃɂ���
	ShowWindow(hwnd, SW_SHOW);

}


bool WinApp::ProcessMessage() {
	MSG msg{}; // ���b�Z�[�W

	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) // ���b�Z�[�W������H
	{
		TranslateMessage(&msg); // �L�[���̓��b�Z�[�W�̏���
		DispatchMessage(&msg);  // �E�B���h�E�v���V�[�W���Ƀ��b�Z�[�W�𑗂�
	}

	if (msg.message == WM_QUIT) // �I�����b�Z�[�W�������烋�[�v�𔲂���
	{
		return true;
	}

	return false;
}

void WinApp::DeleteGameWindow() {
	// �E�B���h�E�N���X��o�^����
	UnregisterClass(w.lpszClassName, w.hInstance);
}