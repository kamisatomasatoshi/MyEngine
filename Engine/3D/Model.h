#pragma once

#include "TextureManager.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Mesh.h"
#include "Light.h"
#include <string>
#include <unordered_map>
#include <vector>
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"

/// <summary>
/// ���f���f�[�^
/// </summary>
class Model {
private:
	// Microsoft::WRL::���ȗ�
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	

public:
	
	// �萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferPolygonExplosion
	{
		float _Destruction = 0.0f;
		float _ScaleFactor = 1.0f;
		float _RotationFactor = 0.0f;
		float _PositionFactor = 0.0f;
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
	static Model* Create();

	// OBJ�t�@�C�����烁�b�V������
	static Model* CreateFromOBJ(const std::string& modelname, bool smoothing = false);

	// �`��O����
	static void PreDraw(ID3D12GraphicsCommandList* commandList);

	// �`��㏈��
	static void PostDraw();

public: // �����o�֐�
	// �f�X�g���N�^
	~Model();

	// ������
	void Initialize(const std::string& modelname, bool smoothing = false);

	// �`��
	void Draw(
		const WorldTransform& worldTransform, const ViewProjection& viewProjection);
	void Draw(
		const WorldTransform& worldTransform, const ViewProjection& viewProjection,
		uint32_t textureHadle);


	// ���b�V���R���e�i���擾
	inline const std::vector<Mesh*>& GetMeshes() { return meshes_; }

	/// <summary>
	/// ���_�z����擾
	/// </summary>
	/// <returns>���_�z��</returns>
	inline const std::vector<Mesh::VertexPosNormalUv>& GetVertices() { return vertices; }

	/// <summary>
	/// �C���f�b�N�X�z����擾
	/// </summary>
	/// <returns>�C���f�b�N�X�z��</returns>
	inline const std::vector<unsigned short>& GetIndices() { return indices; }

	const ConstBufferPolygonExplosion GetPolygonExplosion() {return *constMap ; }

	const void SetPolygonExplosion(ConstBufferPolygonExplosion polygonExplosion) {*constMap=polygonExplosion; }

	//�A���t�@�l�ύX
	void SetAlpha(float alpha);

private: // �����o�ϐ�

	// ���O
	std::string name_;
	// ���b�V���R���e�i
	std::vector<Mesh*> meshes_;
	// �}�e���A���R���e�i
	std::unordered_map<std::string, Material*> materials_;
	// �f�t�H���g�}�e���A��
	Material* defaultMaterial_ = nullptr;

	//���_�f�[�^
	std::vector<Mesh::VertexPosNormalUv>vertices;
	//���_�C���f�b�N�X
	std::vector<unsigned short> indices;

	//�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuff_;

	ConstBufferPolygonExplosion* constMap;


private: // �����o�֐�

	// ���f���ǂݍ���
	void LoadModel(const std::string& modelname, bool smoothing);

	// �}�e���A���ǂݍ���
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);

	// �}�e���A���o�^
	void AddMaterial(Material* material);

	// �e�N�X�`���ǂݍ���
	void LoadTextures();
};
