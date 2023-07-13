#pragma once

#include "Material.h"
#include "DirectXCore.h"
#include <Windows.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <unordered_map>
#include <vector>
#include <wrl.h>
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

/// <summary>
/// �`��f�[�^
/// </summary>
class Mesh {
	friend class FbxLoader;

private: // �G�C���A�X
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;


public: // �T�u�N���X
	// ���_�f�[�^�\���́i�e�N�X�`������j
	struct VertexPosNormalUv {
		Vector3 pos;    // xyz���W
		Vector3 normal; // �@���x�N�g��
		Vector2 uv;     // uv���W
	};

	~Mesh();

public: // �����o�֐�
	/// <summary>
	/// ���O���擾
	/// </summary>
	/// <returns>���O</returns>
	const std::string& GetName() { return name_; }

	/// <summary>
	/// ���O���Z�b�g
	/// </summary>
	/// <param name="name">���O</param>
	void SetName(const std::string& name_);

	/// <summary>
	/// ���_�f�[�^�̒ǉ�
	/// </summary>
	/// <param name="vertex">���_�f�[�^</param>
	void AddVertex(const VertexPosNormalUv& vertex);

	/// <summary>
	/// ���_�C���f�b�N�X�̒ǉ�
	/// </summary>
	/// <param name="index">�C���f�b�N�X</param>
	void AddIndex(unsigned short index);

	/// <summary>
	/// ���_�f�[�^�̐����擾
	/// </summary>
	/// <returns>���_�f�[�^�̐�</returns>
	inline size_t GetVertexCount() { return vertices_.size(); }

	/// <summary>
	/// �G�b�W�������f�[�^�̒ǉ�
	/// </summary>
	/// <param name="indexPosition">���W�C���f�b�N�X</param>
	/// <param name="indexVertex">���_�C���f�b�N�X</param>
	void AddSmoothData(unsigned short indexPosition, unsigned short indexVertex);

	/// <summary>
	/// ���������ꂽ���_�@���̌v�Z
	/// </summary>
	void CalculateSmoothedVertexNormals();

	/// <summary>
	/// �}�e���A���̎擾
	/// </summary>
	/// <returns>�}�e���A��</returns>
	Material* GetMaterial() { return material_; }

	/// <summary>
	/// �}�e���A���̊��蓖��
	/// </summary>
	/// <param name="material">�}�e���A��</param>
	void SetMaterial(Material* material);


	void SetLight(float alpha);

	void SetLight(Vector3 ambient, Vector3 diffuse, Vector3 specular, float alpha);

	/// <summary>
	/// �o�b�t�@�̐���
	/// </summary>
	void CreateBuffers();

	/// <summary>
	/// ���_�o�b�t�@�擾
	/// </summary>
	/// <returns>���_�o�b�t�@</returns>
	const D3D12_VERTEX_BUFFER_VIEW& GetVBView() { return vbView_; }

	/// <summary>
	/// �C���f�b�N�X�o�b�t�@�擾
	/// </summary>
	/// <returns>�C���f�b�N�X�o�b�t�@</returns>
	const D3D12_INDEX_BUFFER_VIEW& GetIBView() { return ibView_; }

	/// <summary>
	/// �`��
	/// </summary>
	/// <param name="commandList">���ߔ��s��R�}���h���X�g</param>
	/// <param name="rooParameterIndexMaterial">�}�e���A���̃��[�g�p�����[�^�ԍ�</param>
	/// <param name="rooParameterIndexTexture">�e�N�X�`���̃��[�g�p�����[�^�ԍ�</param>
	void Draw(
		ID3D12GraphicsCommandList* commandList, UINT rooParameterIndexMaterial,
		UINT rooParameterIndexTexture);

	/// <summary>
	/// �`��i�e�N�X�`�������ւ��Łj
	/// </summary>
	/// <param name="commandList">���ߔ��s��R�}���h���X�g</param>
	/// <param name="rooParameterIndexMaterial">�}�e���A���̃��[�g�p�����[�^�ԍ�</param>
	/// <param name="rooParameterIndexTexture">�e�N�X�`���̃��[�g�p�����[�^�ԍ�</param>
	/// <param name="textureHandle">�����ւ���e�N�X�`���n���h��</param>
	void Draw(
		ID3D12GraphicsCommandList* commandList, UINT rooParameterIndexMaterial,
		UINT rooParameterIndexTexture, uint32_t textureHandle);

	/// <summary>
	/// ���_�z����擾
	/// </summary>
	/// <returns>���_�z��</returns>
	inline const std::vector<VertexPosNormalUv>& GetVertices() { return vertices_; }

	/// <summary>
	/// �C���f�b�N�X�z����擾
	/// </summary>
	/// <returns>�C���f�b�N�X�z��</returns>
	inline const std::vector<unsigned short>& GetIndices() { return indices_; }

private: // �����o�ϐ�
	// ���O
	std::string name_;
	// ���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff_;
	// �C���f�b�N�X�o�b�t�@
	ComPtr<ID3D12Resource> indexBuff_;
	// ���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};
	// �C���f�b�N�X�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW ibView_ = {};
	// ���_�f�[�^�z��
	std::vector<VertexPosNormalUv> vertices_;
	// ���_�C���f�b�N�X�z��
	std::vector<unsigned short> indices_;
	// ���_�@���X���[�W���O�p�f�[�^
	std::unordered_map<unsigned short, std::vector<unsigned short>> smoothData_;
	// �}�e���A��
	Material* material_ = nullptr;
};