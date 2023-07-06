#pragma once

#include <DirectXMath.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <string>
#include <wrl.h>
#include "Vector3.h"

/// <summary>
/// �}�e���A��
/// </summary>
class Material {
private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public: // �T�u�N���X
	// �萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferData {
		XMFLOAT3 ambient;  // �A���r�G���g�W��
		float pad1;        // �p�f�B���O
		XMFLOAT3 diffuse;  // �f�B�t���[�Y�W��
		float pad2;        // �p�f�B���O
		XMFLOAT3 specular; // �X�y�L�����[�W��
		float alpha;       // �A���t�@
	};

public: // �ÓI�����o�֐�
	/// <summary>
	/// �}�e���A������
	/// </summary>
	/// <returns>�������ꂽ�}�e���A��</returns>
	static Material* Create();

public:
	std::string name_;            // �}�e���A����
	XMFLOAT3 ambient_;            // �A���r�G���g�e���x
	XMFLOAT3 diffuse_;            // �f�B�t���[�Y�e���x
	XMFLOAT3 specular_;           // �X�y�L�����[�e���x
	float alpha_;                 // �A���t�@
	std::string textureFilename_; // �e�N�X�`���t�@�C����

public:
	/// <summary>
	/// �萔�o�b�t�@�̎擾
	/// </summary>
	/// <returns></returns>
	ID3D12Resource* GetConstantBuffer() { return constBuff_.Get(); }

	/// �e�N�X�`���ǂݍ���
	/// </summary>
	/// <param name="directoryPath">�ǂݍ��݃f�B���N�g���p�X</param>
	void LoadTexture(const std::string& directoryPath);

	/// <summary>
	/// �X�V
	/// </summary>
	void Update();

	/// <summary>
	/// �O���t�B�b�N�X�R�}���h�̃Z�b�g
	/// </summary>
	/// <param name="commandList">�R�}���h���X�g</param>
	/// <param name="rooParameterIndexMaterial">�}�e���A���̃��[�g�p�����[�^�ԍ�</param>
	/// <param name="rooParameterIndexTexture">�e�N�X�`���̃��[�g�p�����[�^�ԍ�</param>
	void SetGraphicsCommand(
		ID3D12GraphicsCommandList* commandList, UINT rooParameterIndexMaterial,
		UINT rooParameterIndexTexture);

	/// <summary>
	/// �O���t�B�b�N�X�R�}���h�̃Z�b�g�i�e�N�X�`�������ւ��Łj
	/// </summary>
	/// <param name="commandList">�R�}���h���X�g</param>
	/// <param name="rooParameterIndexMaterial">�}�e���A���̃��[�g�p�����[�^�ԍ�</param>
	/// <param name="rooParameterIndexTexture">�e�N�X�`���̃��[�g�p�����[�^�ԍ�</param>
	/// <param name="textureHandle">�����ւ���e�N�X�`���n���h��</param>
	void SetGraphicsCommand(
		ID3D12GraphicsCommandList* commandList, UINT rooParameterIndexMaterial,
		UINT rooParameterIndexTexture, uint32_t textureHandle);

	// �Z�b�g�e�N�X�`���n���h��
	void SetTextureHadle(uint32_t& textureHandles) { textureHandle_ = textureHandles; }

	//// �e�N�X�`���n���h��
	//uint32_t GetTextureHadle(size_t index) { return textureHandle_[index]; }

	void SetLight(float alpha);

	void SetLight(Vector3 ambient, Vector3 diffuse, Vector3 specular, float alpha);

private:
	// �萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuff_;
	// �萔�o�b�t�@�̃}�b�v
	ConstBufferData* constMap_ = nullptr;
	// �e�N�X�`���n���h��
	uint32_t textureHandle_ = 0;
	
private:
	// �R���X�g���N�^
	Material() {
		ambient_ = { 0.3f, 0.3f, 0.3f };
		diffuse_ = { 0.0f, 0.0f, 0.0f };
		specular_ = { 0.0f, 0.0f, 0.0f };
		alpha_ = 1.0f;
	}

	/// <summary>
	/// ������
	/// </summary>
	void Initialize();

	/// <summary>
	/// �萔�o�b�t�@�̐���
	/// </summary>
	void CreateConstantBuffer();
};
