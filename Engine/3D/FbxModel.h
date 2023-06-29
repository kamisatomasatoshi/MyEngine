#pragma once

#include <string>
#include <vector>
#include <DirectXMath.h>
#include <DirectXTex.h>
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>

#include <Mesh.h>
#include "Material.h"
#include <Light.h>
#include <WorldTransform.h>
#include <ViewProjection.h>
#include <fbxsdk.h>



// �m�[�h
struct Node
{
	// ���O
	std::string name;
	// ���[�J���X�P�[��
	DirectX::XMVECTOR scaling = { 1,1,1,0 };
	// ���[�J����]�p
	DirectX::XMVECTOR rotation = { 0,0,0,0 };
	// ���[�J���ړ�
	DirectX::XMVECTOR translation = { 0,0,0,1 };
	// ���[�J���ό`�s��
	DirectX::XMMATRIX transform;
	// �O���[�o���ό`�s��
	DirectX::XMMATRIX globalTransform;
	// �e�m�[�h
	Node* parent = nullptr;
};

class FbxModel
{
public:
	// �t�����h�N���X
	friend class FbxLoader;
	//�萔
	static const int MAX_BONE_INDICES = 4;

public://�T�u�N���X
	//���_�f�[�^�\����
	struct VertexPosNormalUv
	{
		DirectX::XMFLOAT3 pos;//���������W
		DirectX::XMFLOAT3 normal;//�@���x�N�g��
		DirectX::XMFLOAT3 uv;//uv���W
		UINT boneIndex[MAX_BONE_INDICES];
		float boneWeight[MAX_BONE_INDICES];
	};

	//�{�[���\����
	struct Bone {
		//���O
		std::string name;
		//�����p���̋t�s��
		DirectX::XMMATRIX invInitialPose;
		//�N���X�^�[
		FbxCluster* fbxCluster;
		//�R���X�g���N�^
		Bone(const std::string& name) {
			this->name = name;
		}

	};

	FbxScene* fbxScene = nullptr;

	//���b�V�������m�[�h
	Node* meshNode = nullptr;
	//���_�f�[�^�z��
	std::vector<VertexPosNormalUv> vertices;
	//���_�C���f�b�N�X
	std::vector<unsigned short> indices;

	//�A���r�G���g�W��
	DirectX::XMFLOAT3 ambient = { 1,1,1 };
	//�f�B�t���[�Y
	DirectX::XMFLOAT3 diffuse = { 1,1,1 };
	//�e�N�X�`�����^�f�[�^
	DirectX::TexMetadata metadata = {};
	//�X�N���b�`�C���[�W
	DirectX::ScratchImage scratchImage = {};
public://�萔
	static const int MAX_BONE_INDICES = 4;

private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using TexMetadata = DirectX::TexMetadata;
	using ScratchImage = DirectX::ScratchImage;
	// std::���ȗ�
	using string = std::string;
	template <class T> using vector = std::vector<T>;

	//���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff;
	//�C���f�b�N�X�o�b�t�@
	ComPtr<ID3D12Resource> indexBuff;
	//�e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> texbuff;
	//���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	//�C���f�b�N�X�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	//SRV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;

private:
	// Microsoft::WRL::���ȗ�
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;


public:

	// �萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferData
	{
		Vector4 color;	// �F (RGBA)
		Matrix4 mat;	// �R�c�ϊ��s��
	};

private:
	static const std::string kBaseDirectory;
	static const std::string kDefaultModelName;

private: // �ÓI�����o�ϐ�
	// �f�X�N���v�^�T�C�Y
	static UINT sDescriptorHandleIncrementSize_;
	// �R�}���h���X�g
	static ID3D12GraphicsCommandList* sCommandList_;
	// ���[�g�V�O�l�`��
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> sRootSignature_;
	// �p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState_;
	// ���C�g
	static std::unique_ptr<LightGroup> lightGroup;



public: // �ÓI�����o�֐�
	// �ÓI������
	static void StaticInitialize();

	// �O���t�B�b�N�X�p�C�v���C���̏�����
	static void InitializeGraphicsPipeline();

	// 3D���f������
	static FbxModel* Create();

	// OBJ�t�@�C�����烁�b�V������
	static FbxModel* CreateFromFbx(const std::string& modelname, bool smoothing = false);

	// �`��O����
	static void PreDraw(ID3D12GraphicsCommandList* commandList);

	// �`��㏈��
	static void PostDraw();

	//�o�b�t�@����
	void CreateBuffers(ID3D12Device* device);

public: // �����o�֐�
	// �f�X�g���N�^
	~FbxModel();

	// ������
	void Initialize();

	//void Draw(
	//	const WorldTransform& worldTransform, const ViewProjection& viewProjection);

	//void Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection);

	// �`��
	void Draw(ID3D12GraphicsCommandList* cmdList);

	// ���f���̕ό`�s��擾
	const XMMATRIX& GetModelTransform() { return meshNode->globalTransform; }

	// ���b�V���R���e�i���擾
	inline const std::vector<Mesh*>& GetMeshes() { return meshes_; }

	std::vector<Bone>& GetBones() { return bones; }

	FbxScene* GetFbxScene() { return fbxScene; }

private:

	// �m�[�h�z��
	std::vector<Node> nodes;
	
	std::vector<Bone> bones;


	// ���O
	std::string name_;
	// ���b�V���R���e�i
	std::vector<Mesh*> meshes_;
	// �}�e���A���R���e�i
	std::unordered_map<std::string, Material*> materials_;
	// �f�t�H���g�}�e���A��
	Material* defaultMaterial_ = nullptr;




};

