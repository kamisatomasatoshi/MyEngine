#include "FbxLoader.h"

#include <cassert>

#include "Mesh.h"
#include "Material.h"
#include "TextureManager.h"

using namespace DirectX;

/// <summary>
/// 静的メンバ変数の実体
/// </summary>
const std::string FbxLoader::baseDirectory = "Resources/";
const std::string FbxLoader::defaultTextureFileName = "white1x1.png";

FbxLoader* FbxLoader::GetInstance()
{
	static FbxLoader instance;
	return &instance;
}

//void FbxLoader::ConvertMatrixFromFbx(DirectX::XMMATRIX* dst, const FbxAMatrix& src) {
//
//	for (int i = 0; i < 4; i++) {
//		for (int j = 0; j < 4; j++) {
//			dst->r[i].m128_f32[j] = (float)src.Get(i, j);
//		}
//	}
//
//}

void FbxLoader::Initialize(ID3D12Device* device)
{
	// 引数からメンバ変数に代入
	this->device = device;

	flag |= aiProcess_Triangulate;
	flag |= aiProcess_PreTransformVertices;
	flag |= aiProcess_CalcTangentSpace;
	flag |= aiProcess_GenSmoothNormals;
	flag |= aiProcess_GenUVCoords;
	flag |= aiProcess_RemoveRedundantMaterials;
	flag |= aiProcess_OptimizeMeshes;

	flag |= aiProcess_ConvertToLeftHanded;
}

void FbxLoader::Finalize()
{

}

FbxModel* FbxLoader::LoadModelFromFile(const string& modelName)
{
	// モデルと同じ名前のフォルダから読み込む
	const string directoryPath = baseDirectory + modelName + "/";
	// 拡張子.fbxを付加
	const string fileName = modelName + ".fbx";
	// 連結してフルパスを得る
	const string fullpath = directoryPath + fileName;

	mScene = aiImportFile(fullpath.c_str(), flag);

	// ファイル名を指定してFBXファイルを読み込む
	if (!mScene) {
		assert(0);
	}


	// モデル生成
	FbxModel* model = new FbxModel();
	model->name_ = modelName;

	// FBXノードの数を取得
	UINT32 nodeCount = 0;
	GetNodeNum(mScene->mRootNode, nodeCount);

	// あらかじめ必要数分のメモリを確保することで、アドレスがずれるのを予防
	model->nodes.reserve(nodeCount);


	// ルートノードから順に解析してモデルに流し込む
	ParseNodeRecursive(model, mScene->mRootNode);
	// FBXシーン解放
	aiReleaseImport(mScene);

	return model;
}

//void FbxLoader::ParseSkin(Model* model, FbxMesh* fbxMesh) {
//
//	FbxSkin* fbxSkin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(0, FbxDeformer::eSkin));
//
//	if (fbxSkin == nullptr) {
//
//		//各頂点について処理
//		for (int i = 0; i < model->vertices.size(); i++) {
//			//最初のボーン(単位行列)の影響を100%にする
//			model->vertices[i].boneIndex[0] = 0;
//			model->vertices[i].boneWeight[0] = 1.0f;
//		}
//
//		return;
//	}
//
//	std::vector<Model::Bone>& bones = model->bones;
//
//	int clusterCount = fbxSkin->GetClusterCount();
//	bones.reserve(clusterCount);
//
//	for (int i = 0; i < clusterCount; i++) {
//
//		FbxCluster* fbxCluster = fbxSkin->GetCluster(i);
//
//		const char* boneName = fbxCluster->GetLink()->GetName();
//
//		bones.emplace_back(Model::Bone(boneName));
//		Model::Bone& bone = bones.back();
//
//		bone.fbxCluster = fbxCluster;
//
//		FbxAMatrix fbxMat;
//		fbxCluster->GetTransformLinkMatrix(fbxMat);
//
//		XMMATRIX initialPose;
//		ConvertMatrixFromFbx(&initialPose, fbxMat);
//
//		bone.invInitialPose = XMMatrixInverse(nullptr, initialPose);
//
//	}
//
//	struct WeightSet {
//		UINT index;
//		float weight;
//	};
//
//	std::vector<std::list<WeightSet>> weightLists(model->vertices.size());
//
//	for (int i = 0; i < clusterCount; i++) {
//
//		FbxCluster* fbxCluster = fbxSkin->GetCluster(i);
//
//		int controlPointIndicesCount = fbxCluster->GetControlPointIndicesCount();
//
//		int* controlPointIndices = fbxCluster->GetControlPointIndices();
//		double* controlPointWeights = fbxCluster->GetControlPointWeights();
//
//		for (int j = 0; j < controlPointIndicesCount; j++) {
//			int vertIndex = controlPointIndices[j];
//
//			float weight = (float)controlPointWeights[j];
//
//			weightLists[vertIndex].emplace_back(WeightSet{ (UINT)i,weight });
//		}
//	}
//
//	auto& vertices = model->vertices;
//
//	for (int i = 0; i < vertices.size(); i++) {
//
//		auto& weightList = weightLists[i];
//
//		weightList.sort([](auto const& lhs, auto const& rhs) {
//
//			return lhs.weight > rhs.weight;
//			});
//
//		int weightArrayIndex = 0;
//
//		for (auto& weightSet : weightList) {
//
//			vertices[i].boneIndex[weightArrayIndex] = weightSet.index;
//			vertices[i].boneWeight[weightArrayIndex] = weightSet.weight;
//
//			if (++weightArrayIndex >= FbxModel::MAX_BONE_INDICES) {
//				float weight = 0.0f;
//
//				for (int j = 1; j < Model::MAX_BONE_INDICES; j++) {
//					weight += vertices[i].boneWeight[j];
//				}
//				vertices[i].boneWeight[0] = 1.0f - weight;
//				break;
//			}
//
//		}
//
//	}
//
//}

void FbxLoader::ParseNodeRecursive(FbxModel* model, aiNode* fbxNode, Node* parent)
{
	//// ノード名を取得
	//string name = fbxNode->GetName();
	// モデルにノードを追加
	model->nodes.emplace_back();
	Node& node = model->nodes.back();
	// ノード名を取得
	node.name = fbxNode->mName.C_Str();

	node.transform =
	{
		fbxNode->mTransformation.a1, fbxNode->mTransformation.a2, fbxNode->mTransformation.a3, fbxNode->mTransformation.a4,
		fbxNode->mTransformation.b1, fbxNode->mTransformation.b2, fbxNode->mTransformation.b3, fbxNode->mTransformation.b4,
		fbxNode->mTransformation.c1, fbxNode->mTransformation.c2, fbxNode->mTransformation.c3, fbxNode->mTransformation.c4,
		fbxNode->mTransformation.d1, fbxNode->mTransformation.d2, fbxNode->mTransformation.d3, fbxNode->mTransformation.d4,
	};

	//グローバル変形行列の計算
	node.globalTransform = node.transform;

	if (parent) {
		node.parent = parent;
		// 親の変形を乗算
		node.globalTransform *= parent->globalTransform;
	}

	for (UINT32 i = 0; i < fbxNode->mNumMeshes; i++) {
		aiMesh* aimesh = mScene->mMeshes[fbxNode->mMeshes[i]];
		if (aimesh) {
			model->meshes_.emplace_back();
			model->meshes_.back() = new Mesh();
			model->meshes_.back()->name_ = aimesh->mName.C_Str();

			ParseMesh(model, aimesh);
		}
	}

	// 子ノードに対して再帰呼び出し
	for (int i = 0; i < fbxNode->mNumChildren; i++) {
		ParseNodeRecursive(model, fbxNode->mChildren[i], &node);
	}
}

void FbxLoader::ParseMesh(FbxModel* model, aiMesh* fbxMesh)
{

	// 頂点座標読み取り
	ParseMeshVertices(model, fbxMesh);
	// 面を構成するデータの読み取り
	ParseMeshFaces(model, fbxMesh);
	// マテリアルの読み取り
	ParseMaterial(model, fbxMesh, mScene->mMaterials[fbxMesh->mMaterialIndex]);

	//スキニング情報の読み取り
	//ParseSkin(model, fbxMesh);

}

void FbxLoader::ParseMeshVertices(FbxModel* model, aiMesh* fbxMesh)
{
	auto& vertices = model->meshes_.back()->vertices_;

	// 頂点座標データの数
	const int controlPointsCount = fbxMesh->mNumVertices;

	// 必要数だけ頂点データ配列を確保
	Mesh::VertexPosNormalUv vert{};
	model->meshes_.back()->vertices_.resize(controlPointsCount, vert);


	// FBXメッシュの全頂点座標をモデル内の配列にコピーする。
	for (int i = 0; i < controlPointsCount; i++) {
		Mesh::VertexPosNormalUv& vertex = vertices[i];
		// 座標のコピー
		aiVector3D position = fbxMesh->mVertices[i];

		vertex.pos.x = position.x;
		vertex.pos.y = position.y;
		vertex.pos.z = position.z;

		// 法線のコピー
		aiVector3D normal = fbxMesh->mNormals[i];

		vertex.normal.x = normal.x;
		vertex.normal.y = normal.y;
		vertex.normal.z = normal.z;

	}
}

void FbxLoader::ParseMeshFaces(FbxModel* model, aiMesh* fbxMesh)
{
	auto& vertices = model->meshes_.back()->vertices_;
	auto& indices = model->meshes_.back()->indices_;

	// 面の数
	const int polygonCount = fbxMesh->mNumVertices;

	aiVector3D zero3D(0.0f, 0.0f, 0.0f);

	// 面ごとの情報読み取り
	for (int i = 0; i < polygonCount; i++) {
		aiVector3D* uv = (fbxMesh->HasTextureCoords(0)) ? &(fbxMesh->mTextureCoords[0][i]) : &zero3D;

		vertices[i].uv = Vector2(uv->x,uv->y);
	}
	
	indices.resize(fbxMesh->mNumFaces * 3);

	for (UINT i = 0; i < fbxMesh->mNumFaces; i++) {
		const aiFace& face = fbxMesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++) {
			indices[i * 3 + j] = face.mIndices[j];
		}

	}

}

void FbxLoader::ParseMaterial(FbxModel* model, aiMesh* fbxMesh, aiMaterial* aimaterial)
{

	auto& material = model->meshes_.back()->material_;

	material = Material::Create();

	aiColor3D ambient(0.3f, 0.3f, 0.3f);
	aimaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
	material->ambient_ = XMFLOAT3(ambient.r, ambient.g, ambient.b);

	aiColor3D diffuse(0.0f, 0.0f, 0.0f);
	aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
	material->diffuse_ = XMFLOAT3(diffuse.r, diffuse.g, diffuse.b);

	aiColor3D specular(0.0f, 0.0f, 0.0f);
	aimaterial->Get(AI_MATKEY_COLOR_SPECULAR, specular);
	material->specular_ = XMFLOAT3(specular.r, specular.g, specular.b);

	aiString str;
	aimaterial->Get(AI_MATKEY_NAME, str);
	material->name_ = str.C_Str();

	std::vector<uint32_t> deffuseMap = LoadMatrixerialTextures(aimaterial, aiTextureType_DIFFUSE, "Diffuse", mScene, model->name_);
	uint32_t map = deffuseMap.size();

	material->SetTextureHadle(map);

}


std::string FbxLoader::ExtractFileName(const std::string& path)
{
	size_t pos1;
	// 区切り文字 '\\' が出てくる一番最後の部分を検索
	pos1 = path.rfind('\\');
	if (pos1 != string::npos) {
		return path.substr(pos1 + 1, path.size() - pos1 - 1);
	}
	// 区切り文字 '/' が出てくる一番最後の部分を検索
	pos1 = path.rfind('/');
	if (pos1 != string::npos) {
		return path.substr(pos1 + 1, path.size() - pos1 - 1);
	}

	return path;
}

void FbxLoader::GetNodeNum(const aiNode* node, UINT32& num)
{
	if (node->mChildren)
	{
		for (UINT32 i = 0; i < node->mNumChildren; i++)
		{
			GetNodeNum(node->mChildren[i], num);
		}
	}

	num++;
}

std::vector<uint32_t> FbxLoader::LoadMatrixerialTextures(aiMaterial* cmatrix, aiTextureType type, std::string typeName, const aiScene* scene_,const std::string& modelName)
{
	std::vector<uint32_t> textures;

	for (size_t i = 0; i < cmatrix->GetTextureCount(type); i++)
	{
		aiString str;
		cmatrix->GetTexture(type, static_cast<UINT>(i), &str);
		uint32_t texture;
		{
			std::string filename = ExtractFileName(std::string(str.C_Str()));
			filename = modelName + '\\' + filename;
			texture = TextureManager::Load(filename);
		}

		textures.push_back(texture);
	}
	return textures;
}