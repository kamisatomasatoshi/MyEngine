#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"
#include <Windows.h>
#include <d3d12.h>
#include <string>
#include <wrl.h>
#include<memory>

/// <summary>
/// �X�v���C�g
/// </summary>
class Sprite {

public: // �T�u�N���X
	/// <summary>
	/// ���_�f�[�^�\����
	/// </summary>
	struct VertexPosUv {
		Vector3 pos; // xyz���W
		Vector2 uv;  // uv���W
	};

	/// <summary>
	/// �萔�o�b�t�@�p�f�[�^�\����
	/// </summary>
	struct ConstBufferData {
		Vector4 color; // �F (RGBA)
		Matrix4 mat;   // �R�c�ϊ��s��
	};

public: // �ÓI�����o�֐�
	//
	~Sprite();

	// �ÓI������
	static void StaticInitialize(ID3D12Device* device,const std::wstring& directoryPath = L"Resources/");

	// �`��O����
	static void PreDraw(ID3D12GraphicsCommandList* cmdList, int blendMode = 1);

	/// �`��㏈��
	static void PostDraw();


	// �X�v���C�g����
	static std::unique_ptr<Sprite> Create(uint32_t textureHandle);

private: // �ÓI�����o�ϐ�
	// ���_��
	static const int kVertNum = 4;
	// �f�o�C�X
	static ID3D12Device* Device_;
	// �f�X�N���v�^�T�C�Y
	static UINT DescriptorHandleSize_;
	// �R�}���h���X�g
	static ID3D12GraphicsCommandList* CommandList_;
	// ���[�g�V�O�l�`��
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> RootSignature_;
	// �p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, 6>PipelineStates_;
	// �ˉe�s��
	static Matrix4 Matrix4Projection_;

public: // �����o�֐�

	// �R���X�g���N�^
	Sprite();
	// �R���X�g���N�^
	Sprite(uint32_t textureHandle,Vector2 size);


	// ������
	bool Initialize();

	// �e�N�X�`���n���h���̐ݒ�
	void SetTextureHandle(uint32_t textureHandle);

	uint32_t GetTextureHandle() { return textureHandle_; }


	// ���W�̐ݒ�
	void SetPosition(const Vector2& position);

	const Vector2& GetPosition() { return position_; }

	// �p�x�̐ݒ�
	void SetRotation(float rotation);

	float GetRotation() { return rotation_; }

	// �T�C�Y�̐ݒ�
	void SetSize(const Vector2& size);

	const Vector2& GetSize() { return size_; }

	// �A���J�[�|�C���g�̐ݒ�
	void SetAnchorPoint(const Vector2& anchorpoint);

	const Vector2& GetAnchorPoint() { return anchorPoint_; }

	// �F�̐ݒ�
	void SetColor(const Vector4& color) { color_ = color; };

	const Vector4& GetColor() { return color_; }

	// ���E���]�̐ݒ�
	void SetIsFlipX(bool isFlipX);

	bool GetIsFlipX() { return isFlipX_; }

	// �㉺���]�̐ݒ�
	void SetIsFlipY(bool isFlipY);

	bool GetIsFlipY() { return isFlipY_; }

	// �e�N�X�`���͈͐ݒ�
	void SetTextureRect(const Vector2& texBase, const Vector2& texSize);

	void SetDrawUpdate(Vector2 Position, Vector4 Color);


	/// <summary>
	/// �`��
	/// </summary>
	void Draw(Vector2 Position,Vector4 Color, int blendMode = 1);

private: // �����o�ϐ�
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;
	VertexPosUv* vertMap_ = nullptr;
	ConstBufferData* constMap_ = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vbView_{};
	UINT textureHandle_ = 0;
	float rotation_ = 0.0f;
	Vector2 position_{};
	Vector2 size_ = { 100.0f, 100.0f };
	Vector2 anchorPoint_ = { 0, 0 };
	Matrix4 matWorld_{};
	Vector4 color_ = { 1, 1, 1, 1 };
	bool isFlipX_ = false;
	bool isFlipY_ = false;
	Vector2 texBase_ = { 0, 0 };
	Vector2 texSize_ = { 100.0f, 100.0f };
	D3D12_RESOURCE_DESC resourceDesc_;

private: // �����o�֐�
	/// <summary>
	/// ���_�f�[�^�]��
	/// </summary>
	void TransferVertices();
};
