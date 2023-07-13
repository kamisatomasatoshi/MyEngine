#include "Sprite.h"
#include "TextureManager.h"
#include <cassert>
#include <d3dcompiler.h>
#include <d3dx12.h>
#include "MyMath.h"
#include "WinApp.h"
#include "DirectXCore.h"

#pragma comment(lib, "d3dcompiler.lib")

using namespace Microsoft::WRL;

/// <summary>
/// �ÓI�����o�ϐ��̎���
/// </summary>
ID3D12Device* Sprite::Device_ = nullptr;
UINT Sprite::DescriptorHandleSize_;
ID3D12GraphicsCommandList* Sprite::CommandList_ = nullptr;
ComPtr<ID3D12RootSignature> Sprite::RootSignature_;
std::array<ComPtr<ID3D12PipelineState>, 6> Sprite::PipelineStates_;
Matrix4 Sprite::Matrix4Projection_;

Sprite::~Sprite()
{
	vertBuff_->Release();
	constBuff_->Release();

}

void Sprite::StaticInitialize(
	ID3D12Device* device,const std::wstring& directoryPath) {
	// nullptr�`�F�b�N
	assert(device);

	Device_ = device;

	// �f�X�N���v�^�T�C�Y���擾
	DescriptorHandleSize_ =
		Device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob;    // ���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob;    // �s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob; // �G���[�I�u�W�F�N�g

	// ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	std::wstring vsFile = directoryPath + L"/shaders/SpriteVS.hlsl";
	result = D3DCompileFromFile(
		vsFile.c_str(), // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "vs_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0, &vsBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n(
			(char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize(), errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// �s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	std::wstring psFile = directoryPath + L"/shaders/SpritePS.hlsl";
	result = D3DCompileFromFile(
		psFile.c_str(), // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "ps_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0, &psBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n(
			(char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize(), errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());

		exit(1);
	}

	// ���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
	  {// xy���W(1�s�ŏ������ق������₷��)
	   "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
	   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	  {// uv���W(1�s�ŏ������ق������₷��)
	   "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT,
	   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};

	// �O���t�B�b�N�X�p�C�v���C���̗����ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	// �T���v���}�X�N
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�
	// ���X�^���C�U�X�e�[�g
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	// �f�v�X�X�e���V���X�e�[�g
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS; // ��ɏ㏑�����[��

	// �[�x�o�b�t�@�̃t�H�[�}�b�g
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// ���_���C�A�E�g�̐ݒ�
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	// �}�`�̌`��ݒ�i�O�p�`�j
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;                            // �`��Ώۂ�1��
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0�`255�w���RGBA
	gpipeline.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O

	// �f�X�N���v�^�����W
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 ���W�X�^

	// ���[�g�p�����[�^
	CD3DX12_ROOT_PARAMETER rootparams[2] = {};
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

	// �X�^�e�B�b�N�T���v���[
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc =
		CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR); // s0 ���W�X�^
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

	// ���[�g�V�O�l�`���̐ݒ�
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(
		_countof(rootparams), rootparams, 1, &samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	// �o�[�W������������̃V���A���C�Y
	result = D3DX12SerializeVersionedRootSignature(
		&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	// ���[�g�V�O�l�`���̐���
	result = Device_->CreateRootSignature(
		0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&RootSignature_));
	assert(SUCCEEDED(result));

	gpipeline.pRootSignature = RootSignature_.Get();

	// �����_�[�^�[�Q�b�g�̃u�����h�ݒ�B�u�����h�Ȃ�
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // RBGA�S�Ẵ`�����l����`��
	blenddesc.BlendEnable = false;
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	// �O���t�B�b�N�X�p�C�v���C���̐���
	result = Device_->CreateGraphicsPipelineState(
		&gpipeline, IID_PPV_ARGS(&PipelineStates_[0]));
	assert(SUCCEEDED(result));

	// �ʏ탿�u�����h
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	gpipeline.BlendState.RenderTarget[0] = blenddesc;
	result = Device_->CreateGraphicsPipelineState(
		&gpipeline, IID_PPV_ARGS(&PipelineStates_[1]));
	assert(SUCCEEDED(result));

	// ���Z
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_ONE;
	gpipeline.BlendState.RenderTarget[0] = blenddesc;
	result = Device_->CreateGraphicsPipelineState(
		&gpipeline, IID_PPV_ARGS(&PipelineStates_[2]));
	assert(SUCCEEDED(result));

	// ���Z
	blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_ONE;
	gpipeline.BlendState.RenderTarget[0] = blenddesc;
	result = Device_->CreateGraphicsPipelineState(
		&gpipeline, IID_PPV_ARGS(&PipelineStates_[3]));
	assert(SUCCEEDED(result));

	// ��Z
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_ZERO;
	blenddesc.DestBlend = D3D12_BLEND_SRC_COLOR;
	gpipeline.BlendState.RenderTarget[0] = blenddesc;
	result = Device_->CreateGraphicsPipelineState(
		&gpipeline, IID_PPV_ARGS(&PipelineStates_[4]));
	assert(SUCCEEDED(result));

	// �X�N���[��
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blenddesc.DestBlend = D3D12_BLEND_ONE;
	gpipeline.BlendState.RenderTarget[0] = blenddesc;
	result = Device_->CreateGraphicsPipelineState(
		&gpipeline, IID_PPV_ARGS(&PipelineStates_[5]));
	assert(SUCCEEDED(result));

	// �ˉe�s��v�Z
	Matrix4Projection_ =
		MyMath::Matrix4Orthographic(
			0.0f, (float)WinApp::window_width, (float)WinApp::window_height, 0.0f, 0.0f, 1.0f);
}

void Sprite::PreDraw(ID3D12GraphicsCommandList* commandList, int blendMode) {

	// �R�}���h���X�g���Z�b�g
	CommandList_ = commandList;

	// �p�C�v���C���X�e�[�g�̐ݒ�
	CommandList_->SetPipelineState(PipelineStates_[blendMode].Get());
	// ���[�g�V�O�l�`���̐ݒ�
	CommandList_->SetGraphicsRootSignature(RootSignature_.Get());
	// �v���~�e�B�u�`���ݒ�
	CommandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void Sprite::PostDraw() {
	// �R�}���h���X�g������
	Sprite::CommandList_ = nullptr;
}

std::unique_ptr<Sprite> Sprite::Create(uint32_t textureHandle) {
	// ���T�C�Y
	Vector2 size = { 100.0f, 100.0f };

	{
		// �e�N�X�`�����擾
		const D3D12_RESOURCE_DESC& resDesc =
			TextureManager::GetInstance()->GetResoureDesc(textureHandle);
		// �X�v���C�g�̃T�C�Y���e�N�X�`���̃T�C�Y�ɐݒ�
		size = { (float)resDesc.Width, (float)resDesc.Height };
	}

	// Sprite�̃C���X�^���X�𐶐�
	Sprite* sprite =
		new Sprite(textureHandle,size);
	if (sprite == nullptr) {
		return nullptr;
	}

	// ������
	if (!sprite->Initialize()) {
		delete sprite;
		assert(0);
		return nullptr;
	}

	return std::unique_ptr<Sprite>(sprite);
}

Sprite::Sprite() {}

Sprite::Sprite(
	uint32_t textureHandle,Vector2 size) {
	size_ = size;
	anchorPoint_ = Vector2(0.5f,0.5f);
	matWorld_ = MyMath::MakeIdentity();
	textureHandle_ = textureHandle;
	texSize_ = size;
}

bool Sprite::Initialize() {
	// nullptr�`�F�b�N
	assert(Device_);

	HRESULT result = S_FALSE;

	resourceDesc_ = TextureManager::GetInstance()->GetResoureDesc(textureHandle_);

	{
		// �q�[�v�v���p�e�B
		CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		// ���\�[�X�ݒ�
		CD3DX12_RESOURCE_DESC resourceDesc =
			CD3DX12_RESOURCE_DESC::Buffer(sizeof(VertexPosUv) * kVertNum);

		// ���_�o�b�t�@����
		result = Device_->CreateCommittedResource(
			&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, IID_PPV_ARGS(&vertBuff_));
		assert(SUCCEEDED(result));

		// ���_�o�b�t�@�}�b�s���O
		result = vertBuff_->Map(0, nullptr, (void**)&vertMap_);
		assert(SUCCEEDED(result));
	}

	// ���_�o�b�t�@�ւ̃f�[�^�]��
	TransferVertices();

	// ���_�o�b�t�@�r���[�̍쐬
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeof(VertexPosUv) * 4;
	vbView_.StrideInBytes = sizeof(VertexPosUv);

	{
		// �q�[�v�v���p�e�B
		CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		// ���\�[�X�ݒ�
		CD3DX12_RESOURCE_DESC resourceDesc =
			CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff);

		// �萔�o�b�t�@�̐���
		result = Device_->CreateCommittedResource(
			&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, IID_PPV_ARGS(&constBuff_));
		assert(SUCCEEDED(result));
	}

	// �萔�o�b�t�@�}�b�s���O
	result = constBuff_->Map(0, nullptr, (void**)&constMap_);
	assert(SUCCEEDED(result));

	return true;
}

void Sprite::SetTextureHandle(uint32_t textureHandle) {
	textureHandle_ = textureHandle;
	resourceDesc_ = TextureManager::GetInstance()->GetResoureDesc(textureHandle_);
}

void Sprite::SetRotation(float rotation) {
	rotation_ = rotation;

	// ���_�o�b�t�@�ւ̃f�[�^�]��
	TransferVertices();
}

void Sprite::SetPosition(const Vector2& position) {
	position_ = position;

	// ���_�o�b�t�@�ւ̃f�[�^�]��
	TransferVertices();
}

void Sprite::SetSize(const Vector2& size) {
	size_ = size;

	// ���_�o�b�t�@�ւ̃f�[�^�]��
	TransferVertices();
}

void Sprite::SetAnchorPoint(const Vector2& anchorpoint) {
	anchorPoint_ = anchorpoint;

	// ���_�o�b�t�@�ւ̃f�[�^�]��
	TransferVertices();
}

void Sprite::SetIsFlipX(bool isFlipX) {
	isFlipX_ = isFlipX;

	// ���_�o�b�t�@�ւ̃f�[�^�]��
	TransferVertices();
}

void Sprite::SetIsFlipY(bool isFlipY) {
	isFlipY_ = isFlipY;

	// ���_�o�b�t�@�ւ̃f�[�^�]��
	TransferVertices();
}

void Sprite::SetTextureRect(const Vector2& texBase, const Vector2& texSize) {
	texBase_ = texBase;
	texSize_ = texSize;

	// ���_�o�b�t�@�ւ̃f�[�^�]��
	TransferVertices();
}

void Sprite::SetDrawUpdate(Vector2 Position, Vector4 Color){

	color_ = Color;

	SetPosition(Position);
}

void Sprite::Draw(Vector2 Position, Vector4 Color, int blendMode) {

	SetDrawUpdate(Position, Color);

	// ���[���h�s��̍X�V
	matWorld_ = MyMath::MakeIdentity();
	matWorld_ *= MyMath::Rotation(Vector3(0,0,rotation_),3);
	matWorld_ *= MyMath::Translation(Vector3(position_.x, position_.y, 0.0f));

	// �萔�o�b�t�@�Ƀf�[�^�]��
	constMap_->color = color_;
	constMap_->mat = matWorld_ * Matrix4Projection_; // �s��̍���


	// �R�}���h���X�g���Z�b�g
	CommandList_ = DirectXCore::GetInstance()->GetCommandList();

	// �p�C�v���C���X�e�[�g�̐ݒ�
	CommandList_->SetPipelineState(PipelineStates_[blendMode].Get());
	// ���[�g�V�O�l�`���̐ݒ�
	CommandList_->SetGraphicsRootSignature(RootSignature_.Get());
	// �v���~�e�B�u�`���ݒ�
	CommandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// ���_�o�b�t�@�̐ݒ�
	CommandList_->IASetVertexBuffers(0, 1, &vbView_);

	// �萔�o�b�t�@�r���[���Z�b�g
	CommandList_->SetGraphicsRootConstantBufferView(0, constBuff_->GetGPUVirtualAddress());
	// �V�F�[�_���\�[�X�r���[���Z�b�g
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(CommandList_, 1, textureHandle_);
	// �`��R�}���h
	CommandList_->DrawInstanced(4, 1, 0, 0);
}

void Sprite::TransferVertices() {
	HRESULT result = S_FALSE;

	// �����A����A�E���A�E��
	enum { LB, LT, RB, RT };

	float left = (0.0f - anchorPoint_.x) * size_.x;
	float right = (1.0f - anchorPoint_.x) * size_.x;
	float top = (0.0f - anchorPoint_.y) * size_.y;
	float bottom = (1.0f - anchorPoint_.y) * size_.y;
	if (isFlipX_) { // ���E����ւ�
		left = -left;
		right = -right;
	}

	if (isFlipY_) { // �㉺����ւ�
		top = -top;
		bottom = -bottom;
	}

	// ���_�f�[�^
	VertexPosUv vertices[kVertNum];

	vertices[LB].pos = { left, bottom, 0.0f };  // ����
	vertices[LT].pos = { left, top, 0.0f };     // ����
	vertices[RB].pos = { right, bottom, 0.0f }; // �E��
	vertices[RT].pos = { right, top, 0.0f };    // �E��

	// �e�N�X�`�����擾
	{
		float tex_left = texBase_.x / resourceDesc_.Width;
		float tex_right = (texBase_.x + texSize_.x) / resourceDesc_.Width;
		float tex_top = texBase_.y / resourceDesc_.Height;
		float tex_bottom = (texBase_.y + texSize_.y) / resourceDesc_.Height;

		vertices[LB].uv = { tex_left, tex_bottom };  // ����
		vertices[LT].uv = { tex_left, tex_top };     // ����
		vertices[RB].uv = { tex_right, tex_bottom }; // �E��
		vertices[RT].uv = { tex_right, tex_top };    // �E��
	}

	// ���_�o�b�t�@�ւ̃f�[�^�]��
	memcpy(vertMap_, vertices, sizeof(vertices));
}
