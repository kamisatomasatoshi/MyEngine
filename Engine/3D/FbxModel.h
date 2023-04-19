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

// ノード
struct Node
{
	// 名前
	std::string name;
	// ローカルスケール
	DirectX::XMVECTOR scaling = { 1,1,1,0 };
	// ローカル回転角
	DirectX::XMVECTOR rotation = { 0,0,0,0 };
	// ローカル移動
	DirectX::XMVECTOR translation = { 0,0,0,1 };
	// ローカル変形行列
	DirectX::XMMATRIX transform;
	// グローバル変形行列
	DirectX::XMMATRIX globalTransform;
	// 親ノード
	Node* parent = nullptr;
};

class FbxModel
{
public:
	// フレンドクラス
	friend class FbxLoader;

public://定数
	static const int MAX_BONE_INDICES = 4;

private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using TexMetadata = DirectX::TexMetadata;
	using ScratchImage = DirectX::ScratchImage;
	// std::を省略
	using string = std::string;
	template <class T> using vector = std::vector<T>;

public: // サブクラス
	// 頂点データ構造体
	struct VertexPosNormalUvFbxSkin
	{
		DirectX::XMFLOAT3 pos; // xyz座標
		DirectX::XMFLOAT3 normal; // 法線ベクトル
		DirectX::XMFLOAT2 uv;  // uv座標

		UINT boneIndex[MAX_BONE_INDICES];
		float boneWeight[MAX_BONE_INDICES];
	};

	//ボーン構造体
	struct Bone {
		std::string name;

		DirectX::XMMATRIX invInitialPose;



		Bone(const std::string& name) {
			this->name = name;
		}

	};

private:
	// Microsoft::WRL::を省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;


public:

	// 定数バッファ用データ構造体
	struct ConstBufferData
	{
		Vector4 color;	// 色 (RGBA)
		Matrix4 mat;	// ３Ｄ変換行列
	};

private:
	static const std::string kBaseDirectory;
	static const std::string kDefaultModelName;

private: // 静的メンバ変数
	// デスクリプタサイズ
	static UINT sDescriptorHandleIncrementSize_;
	// コマンドリスト
	static ID3D12GraphicsCommandList* sCommandList_;
	// ルートシグネチャ
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> sRootSignature_;
	// パイプラインステートオブジェクト
	static Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState_;
	// ライト
	static std::unique_ptr<LightGroup> lightGroup;

public: // 静的メンバ関数
	// 静的初期化
	static void StaticInitialize();

	// グラフィックスパイプラインの初期化
	static void InitializeGraphicsPipeline();

	// 3Dモデル生成
	static FbxModel* Create();

	// OBJファイルからメッシュ生成
	static FbxModel* CreateFromFbx(const std::string& modelname, bool smoothing = false);

	// 描画前処理
	static void PreDraw(ID3D12GraphicsCommandList* commandList);

	// 描画後処理
	static void PostDraw();

public: // メンバ関数
	// デストラクタ
	~FbxModel();

	// 初期化
	void Initialize();

	// 描画
	//void Draw(
	//	const WorldTransform& worldTransform, const ViewProjection& viewProjection);

	void Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection);


	// メッシュコンテナを取得
	inline const std::vector<Mesh*>& GetMeshes() { return meshes_; }

	std::vector<Bone>& GetBones() { return bones; }

private:

	// ノード配列
	std::vector<Node> nodes;
	
	std::vector<Bone> bones;


	// 名前
	std::string name_;
	// メッシュコンテナ
	std::vector<Mesh*> meshes_;
	// マテリアルコンテナ
	std::unordered_map<std::string, Material*> materials_;
	// デフォルトマテリアル
	Material* defaultMaterial_ = nullptr;




};

