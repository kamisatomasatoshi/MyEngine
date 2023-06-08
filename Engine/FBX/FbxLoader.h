#pragma once

#include "FbxModel.h"
#include <string>
#include "fbxsdk.h"
#include <d3d12.h>
#include <d3dx12.h>


class FbxLoader
{

private:
	//std::を省略
	using string = std::string;

public://定数
	//モデル格納ルートパス
	static const string baseDirectory;

public:
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static FbxLoader* GetInstance();

	void Initialize(ID3D12Device* device);

	void LoadModelFromFile(const string& modelName);

	void ParseNodeRecursive(FbxModel* model, FbxNode* fbxnode, Node* parent = nullptr);

	///メッシュ読み取り
	void ParseMesh(FbxModel* model, FbxNode* fbxNode);
	//頂点座標読み取り
	void ParseMeshVertices(FbxModel* model, FbxMesh* fbxMesh);
	///面情報読み取り
	void ParseMeshFaces(FbxModel* model, FbxMesh* fbxMesh);
	///マテリアル読み取り
	void ParseMaterial(FbxModel* model, FbxNode* fbxNode);
	//テクスチャ読み込み
	void LoadTexture(FbxModel* model, const std::string& fullpath);

	std::string ExtractFileName(const std::string& path);

	static const string defaultTextureFileName;

	void Finalize();

private:
	ID3D12Device* device = nullptr;

	FbxManager* fbxManager = nullptr;

	FbxImporter* fbxImporter = nullptr;


	// privateなコンストラクタ（シングルトンパターン）
	FbxLoader() = default;
	// privateなデストラクタ（シングルトンパターン）
	~FbxLoader() = default;
	// コピーコンストラクタを禁止（シングルトンパターン）
	FbxLoader(const FbxLoader& obj) = delete;
	// コピー代入演算子を禁止（シングルトンパターン）
	void operator=(const FbxLoader& obj) = delete;
};

//private: // エイリアス
//	// std::を省略
//	using string = std::string;
//
//public: // 定数
//	// モデル格納ルートパス
//	static const string baseDirectory;
//	// テクスチャがない場合の標準テクスチャファイル名
//	static const string defaultTextureFileName;
//public:
//	/// <summary>
//	/// シングルトンインスタンスの取得
//	/// </summary>
//	/// <returns>インスタンス</returns>
//	static FbxLoader* GetInstance();
//
//public://静的メンバ関数
//
//	/// <summary>
//	/// FBXの行列をXMMatrixに変換
//	/// </summary>
//	/// <param name="dst">書き込み先</param>
//	/// <param name="src">もととなるFBX行列</param>
//	//static void ConvertMatrixFromFbx(DirectX::XMMATRIX* dst, const FbxAMatrix& src);
//
//
//public:
//	/// <summary>
//	/// 初期化
//	/// </summary>
//	/// <param name="device">D3D12デバイス</param>
//	void Initialize(ID3D12Device* device);
//
//	/// <summary>
//	/// 後始末
//	/// </summary>
//	void Finalize();
//
//	/// <summary>
//	/// ファイルからFBXモデル読込
//	/// </summary>
//	/// <param name="modelName">モデル名</param>
//	FbxModel* LoadModelFromFile(const string& modelName);
//
//
//	/*void ParseSkin(FbxModel* model);*/
//
//
//	void GetNodeNum(const aiNode* node, UINT32& num);
//
//private:
//	// D3D12デバイス
//	ID3D12Device* device = nullptr;
//	// FBXマネージャ
//	//FbxManager* fbxManager = nullptr;
//	// FBXインポータ
//	//FbxImporter* fbxImporter = nullptr;
//
//private:
//	// privateなコンストラクタ（シングルトンパターン）
//	FbxLoader() = default;
//	// privateなデストラクタ（シングルトンパターン）
//	~FbxLoader() = default;
//	// コピーコンストラクタを禁止（シングルトンパターン）
//	FbxLoader(const FbxLoader& obj) = delete;
//	// コピー代入演算子を禁止（シングルトンパターン）
//	void operator=(const FbxLoader& obj) = delete;
//
//	/// <summary>
//	/// 再帰的にノード構成を解析
//	/// </summary>
//	/// <param name="model">読み込み先モデルオブジェクト</param>
//	/// <param name="fbxNode">解析対象のノード</param>
//	/// <param name="parent">親ノード</param>
//	void ParseNodeRecursive(FbxModel* model, aiNode* fbxNode, Node* parent = nullptr);
//
//	/// <summary>
//	/// メッシュ読み取り
//	/// </summary>
//	/// <param name="model">読み込み先モデルオブジェクト</param>
//	/// <param name="fbxNode">解析対象のノード</param>
//	void ParseMesh(FbxModel* model, aiMesh* fbxNode);
//	// 頂点座標読み取り
//	void ParseMeshVertices(FbxModel* model, aiMesh* fbxMesh);
//	// 面情報読み取り
//	void ParseMeshFaces(FbxModel* model, aiMesh* fbxMesh);
//	// マテリアル読み取り
//	void ParseMaterial(FbxModel* model, aiMesh* fbxMesh, aiMaterial* aimaterial);
//
//	// ディレクトリを含んだファイルパスからファイル名を抽出する
//	std::string ExtractFileName(const std::string& path);
//
//	std::vector<uint32_t> LoadMatrixerialTextures(aiMaterial* cmatrix, aiTextureType type, std::string typeName, const aiScene* scene_, const std::string& modelName);
//
//	int flag = 0;
//
//	const aiScene* mScene;