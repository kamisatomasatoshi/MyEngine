#include "DirectXCore.h"
#include "FbxModel.h"
#include <algorithm>
#include <cassert>
#include <d3dcompiler.h>
#include <fstream>
#include <sstream>

#pragma comment(lib, "d3dcompiler.lib")

using namespace std;
using namespace Microsoft::WRL;

/// <summary>
/// �ÓI�����o�ϐ��̎���
/// </summary>
const std::string FbxModel::kBaseDirectory = "Resources/";
const std::string FbxModel::kDefaultModelName = "cube";
UINT FbxModel::sDescriptorHandleIncrementSize_ = 0;
ID3D12GraphicsCommandList* FbxModel::sCommandList_ = nullptr;
ComPtr<ID3D12RootSignature> FbxModel::sRootSignature_;
ComPtr<ID3D12PipelineState> FbxModel::sPipelineState_;
std::unique_ptr<LightGroup> FbxModel::lightGroup;

void FbxModel::StaticInitialize() {

	// �p�C�v���C��������
	InitializeGraphicsPipeline();

	// ���C�g����
	lightGroup.reset(LightGroup::Create());
}

void FbxModel::InitializeGraphicsPipeline() {
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob;    // ���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob;    // �s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob; // �G���[�I�u�W�F�N�g


	// ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/shaders/ObjVS.hlsl", // �V�F�[�_�t�@�C����
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
	result = D3DCompileFromFile(
		L"Resources/shaders/ObjPS.hlsl", // �V�F�[�_�t�@�C����
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
	  {// �@���x�N�g��(1�s�ŏ������ق������₷��)
	   "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
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
	// gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	// gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	//  �f�v�X�X�e���V���X�e�[�g
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	// �����_�[�^�[�Q�b�g�̃u�����h�ݒ�
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // RBGA�S�Ẵ`�����l����`��
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	// �u�����h�X�e�[�g�̐ݒ�
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	// �[�x�o�b�t�@�̃t�H�[�}�b�g
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// ���_���C�A�E�g�̐ݒ�
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	// �}�`�̌`��ݒ�i�O�p�`�j
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;                       // �`��Ώۂ�1��
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0�`255�w���RGBA
	gpipeline.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O

	// �f�X�N���v�^�����W
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 ���W�X�^

	// ���[�g�p�����[�^
	CD3DX12_ROOT_PARAMETER rootparams[5];
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[2].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[3].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[4].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL);

	// �X�^�e�B�b�N�T���v��
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	// ���[�g�V�O�l�`���̐ݒ�
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(
		_countof(rootparams), rootparams, 1, &samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	// �o�[�W������������̃V���A���C�Y
	result = D3DX12SerializeVersionedRootSignature(
		&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	// ���[�g�V�O�l�`���̐���
	result = DirectXCore::GetInstance()->GetDevice()->CreateRootSignature(
		0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&sRootSignature_));
	assert(SUCCEEDED(result));

	gpipeline.pRootSignature = sRootSignature_.Get();

	// �O���t�B�b�N�X�p�C�v���C���̐���
	result = DirectXCore::GetInstance()->GetDevice()->CreateGraphicsPipelineState(
		&gpipeline, IID_PPV_ARGS(&sPipelineState_));
	assert(SUCCEEDED(result));
}

FbxModel* FbxModel::Create() {
	// �������m��
	FbxModel* instance = new FbxModel;
	instance->Initialize();

	return instance;
}

FbxModel* FbxModel::CreateFromFbx(const std::string& modelname, bool smoothing) {
	// �������m��
	FbxModel* instance = new FbxModel;
	instance->Initialize();

	return instance;
}

void FbxModel::PreDraw(ID3D12GraphicsCommandList* commandList) {
	// PreDraw��PostDraw���y�A�ŌĂ΂�Ă��Ȃ���΃G���[
	assert(FbxModel::sCommandList_ == nullptr);

	// �R�}���h���X�g���Z�b�g
	sCommandList_ = commandList;

	// �p�C�v���C���X�e�[�g�̐ݒ�
	commandList->SetPipelineState(sPipelineState_.Get());
	// ���[�g�V�O�l�`���̐ݒ�
	commandList->SetGraphicsRootSignature(sRootSignature_.Get());
	// �v���~�e�B�u�`���ݒ�
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void FbxModel::PostDraw() {
	// �R�}���h���X�g������
	sCommandList_ = nullptr;
}

void FbxModel::CreateBuffers(ID3D12Device* device)
{
	HRESULT result;
	//���_�e�[�^�S�̃T�C�Y
	UINT sizeVB =
		static_cast<UINT>(sizeof(VertexPosNormalUv) *
			vertices.size());
	//���_�o�b�t�@����
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	//���_�o�b�t�@�ւ̃f�[�^�]��
	VertexPosNormalUv* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result))
	{
		std::copy(vertices.begin(), vertices.end(),
			vertMap);
		vertBuff->Unmap(0, nullptr);
	}

	//���_�o�b�t�@�r���[�iVBV�j�̍쐬
	vbView.BufferLocation =
		vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(vertices[0]);
	//���_�C���f�b�N�X�S�̂̃T�C�Y
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices.size());
	//�C���f�b�N�X�o�b�t�@����
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeIB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));
	 
	//�C���f�b�N�X�o�b�t�@�ւ̃f�[�^���M
	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	if (SUCCEEDED(result))
	{
		std::copy(indices.begin(), indices.end(), indexMap);
		indexBuff->Unmap(0, nullptr);
	}

	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;


}

FbxModel::~FbxModel() {
	for (auto m : meshes_) {
		delete m;
	}
	meshes_.clear();

	for (auto m : materials_) {
		delete m.second;
	}
	materials_.clear();
}

void FbxModel::Initialize() {
	// ���f���ǂݍ���
	//LoadModelFromFile(modelname);

	// ���b�V���̃}�e���A���`�F�b�N
	for (auto& m : meshes_) {
		// �}�e���A���̊��蓖�Ă��Ȃ�
		if (m->GetMaterial() == nullptr) {
			if (defaultMaterial_ == nullptr) {
				// �f�t�H���g�}�e���A���𐶐�
				defaultMaterial_ = Material::Create();
				defaultMaterial_->name_ = "no material";
				materials_.emplace(defaultMaterial_->name_, defaultMaterial_);
			}
			// �f�t�H���g�}�e���A�����Z�b�g
			m->SetMaterial(defaultMaterial_);
		}
	}

	// ���b�V���̃o�b�t�@����
	for (auto& m : meshes_) {

		Vector3 ambient_ = { 1.0f, 1.0f, 1.0f };
		Vector3 diffuse_ = { 0.0f, 0.0f, 0.0f };
		Vector3 specular_ = { 0.0f, 0.0f, 0.0f };
		float alpha_ = 1.0f;

		m->SetLight(ambient_, diffuse_, specular_, alpha_);

		m->CreateBuffers();
	}

	// �}�e���A���̐��l��萔�o�b�t�@�ɔ��f
	for (auto& m : materials_) {

		m.second->Update();
	}

}


void FbxModel::Draw(
	const WorldTransform& worldTransform, const ViewProjection& viewProjection) {

	for (int i = 0; i < meshes_.size(); i++) {

		// ���C�g�̕`��
		lightGroup->Draw(sCommandList_, 4);

		// CBV���Z�b�g�i���[���h�s��j
		sCommandList_->SetGraphicsRootConstantBufferView(0, worldTransform.constBuff_->GetGPUVirtualAddress());

		// CBV���Z�b�g�i�r���[�v���W�F�N�V�����s��j
		sCommandList_->SetGraphicsRootConstantBufferView(1, viewProjection.constBuff_->GetGPUVirtualAddress());


		// �S���b�V����`��
		meshes_[i]->Draw(sCommandList_, 2, 3,2);
	}
}
//
//void FbxModel::Draw(
//	const WorldTransform& worldTransform, const ViewProjection& viewProjection,
//	uint32_t textureHadle) {
//
//	// ���C�g�̕`��
//	lightGroup->Draw(sCommandList_, 4);
//
//	// CBV���Z�b�g�i���[���h�s��j
//	sCommandList_->SetGraphicsRootConstantBufferView(0, worldTransform.constBuff_->GetGPUVirtualAddress());
//
//	// CBV���Z�b�g�i�r���[�v���W�F�N�V�����s��j
//	sCommandList_->SetGraphicsRootConstantBufferView(1, viewProjection.constBuff_->GetGPUVirtualAddress());
//
//	// �S���b�V����`��
//	for (auto& mesh : meshes_) {
//		mesh->Draw(
//			sCommandList_, 2, 3,
//			textureHadle);
//	}
//}
