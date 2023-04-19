#pragma once

#include <Windows.h>
#include <cstdlib>
#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <wrl.h>


class DirectXCore {

public://�����o�֐�

	// �V���O���g���C���X�^���X�̎擾
	static DirectXCore* GetInstance();

	void DirectXCoreInitialize(HWND hwnd, int window_width, int window_height);


	/// <summary>
	/// �`��O�㏈��
	/// </summary>
	void PreDraw();
	void PostDraw();

	// �����_�[�^�[�Q�b�g�̃N���A
	void ClearRenderTarget();

	// �[�x�o�b�t�@�̃N���A
	void ClearDepthBuffer();

	void Destroy();

	// �f�o�C�X�̎擾
	ID3D12Device* GetDevice() { return device.Get(); }

	// �`��R�}���h���X�g�̎擾
	ID3D12GraphicsCommandList* GetCommandList() { return commandList.Get(); }

	//�o�b�N�o�b�t�@�̐����擾
	size_t GetBackBufferCount()const { return backBuffers.size(); }

private://�����o�ϐ�

	// Direct3D�֘A
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;
	Microsoft::WRL::ComPtr<ID3D12Device> device;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffers;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthBuffer;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;

	int backBufferWidth_ = 0;
	int backBufferHeight_ = 0;

	//�t�F���X�̐���
	Microsoft::WRL::ComPtr < ID3D12Fence> fence;
	UINT64 fenceVal = 0;

	HWND hwnd_;

	static DirectXCore* DirectXCore_;

private://�����o�ϐ�
	DirectXCore() = default;
	~DirectXCore() = default;
	DirectXCore(const DirectXCore&) = delete;
	const DirectXCore& operator=(const DirectXCore&) = delete;

	// DXGI�f�o�C�X������
	void InitializeDevice();

	// �X���b�v�`�F�[���̐���
	void InitializeSwapChain();

	// �R�}���h�֘A������
	void InitializeCommand();

	// �����_�[�^�[�Q�b�g����
	void InitializeRenderTargets();

	// �[�x�o�b�t�@����
	void InitializeDepthBuffer();

	// �t�F���X����
	void InitializeFence();

};