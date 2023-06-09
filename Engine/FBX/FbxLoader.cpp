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


FbxLoader* FbxLoader::GetInstance()
{
    static FbxLoader instance;
    return &instance;
}

const std::string FbxLoader::defaultTextureFileName = "white1x1.png";

void FbxLoader::Initialize(ID3D12Device* device)
{
    //再初期化チェック
    assert(fbxManager == nullptr);
    //引数からメンバ変数代入
    this->device = device;
    //FBXマネージャー生成
    fbxManager = FbxManager::Create();
    //FBXマネージャー入出力設定
    FbxIOSettings* ios = FbxIOSettings::Create(fbxManager, IOSROOT);
    //FBXインポータの生成
    fbxImporter = FbxImporter::Create(fbxManager, "");

}

void FbxLoader::LoadModelFromFile(const string& modelName)
{
    //モデルと同じ名前のフォルダ空読み込み
    const string directoryPath = baseDirectory + modelName + "/";
    //拡張子.fbxを付加
    const string fileName = modelName + ".fbk";
    //連結してフルパスを得る
    const string fullpath = directoryPath + fileName;

    //ファイル名を指定してFBXファイルを読み込む
    if (!fbxImporter->Initialize(fullpath.c_str(), -1, fbxManager->GetIOSettings()))
    {
        assert(0);
    }
    //シーン作成
    FbxScene* fbxScene =
        FbxScene::Create(fbxManager, "fbxScene");

    //ファイルからロードしたFBXの情報をシーンインポート
    fbxImporter->Import(fbxScene);
    //モデル生成
    FbxModel* model = new FbxModel();
    model->name_ = modelName;
    //FBXノードの数を取得
    int nodeCount = fbxScene->GetNodeCount();
    //あらかじめ必要分をメモリ確保することで、アドレスがずれるのを予防
    model->nodes.reserve(nodeCount);
    //ルートノードから順に解析してモデルに流し込む
    ParseNodeRecursive(model, fbxScene->GetRootNode());
    //FBXシーン解放
    fbxScene->Destroy();
}

void FbxLoader::ParseNodeRecursive(FbxModel* model, FbxNode* fbxNode, Node* parent)
{
    //ノード名を取得
    string name = fbxNode->GetName();
    //モデルにノードを追加（Todo）
    model->nodes.emplace_back();
    Node& node = model->nodes.back();
    //ノード名を取得
    node.name = fbxNode->GetName();
    //FBXノードのローカル移動情報
    FbxDouble3 rotation = fbxNode->LclRotation.Get();
    FbxDouble3 scaling = fbxNode->LclScaling.Get();
    FbxDouble3 translation = fbxNode->LclTranslation.Get();
    //形式変換して代入
    node.rotation = { (float)rotation[0],(float)rotation[1],(float)rotation[2],0.0f };
    node.scaling = { (float)scaling[0],(float)scaling[1],(float)scaling[2],0.0f };
    node.translation = { (float)translation[0],(float)translation[1],(float)translation[2],1.0f };

    //回転角をDegreeからラジアンに変換
    node.rotation.m128_f32[0] = XMConvertToRadians(node.rotation.m128_f32[0]);
    node.rotation.m128_f32[1] = XMConvertToRadians(node.rotation.m128_f32[1]);
    node.rotation.m128_f32[2] = XMConvertToRadians(node.rotation.m128_f32[2]);

    //スケール、回転、平行移動行列の計算
    XMMATRIX matScaling, matRotation, matTranslation;
    matScaling = XMMatrixScalingFromVector(node.scaling);
    matRotation = XMMatrixRotationRollPitchYawFromVector(node.rotation);

    //ローカル変形行列の計算
    node.transform = XMMatrixIdentity();
    node.transform *= matScaling;
    node.transform *= matRotation;
    node.transform *= matTranslation;
    //FBXノードの情報を解析してノードに記録（Todo）
    
    //グローバル変数行列の計算
    node.globalTransform = node.transform;
    if (parent)
    {
        node.parent = parent;
        //親の変形を乗算
        node.globalTransform *= parent->globalTransform;
    }
    //FBXノードのメッシュ情報を解析（Todo）
    FbxNodeAttribute* fbxNodeAttribute = fbxNode->GetNodeAttribute();

    if (fbxNodeAttribute)
    {
        if (fbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
        {
            model->meshNode = &node;
            ParseMesh(model, fbxNode);
        }
    }
    //子ノードに対して再起呼び出し
    for (int i = 0; i < fbxNode->GetChildCount(); i++)
    {
        ParseNodeRecursive(model, fbxNode->GetChild(i),&node);
    }
}

void FbxLoader::ParseMesh(FbxModel* model, FbxNode* fbxNode)
{
    //ノードのメッシュ取得
    FbxMesh* fbxMesh = fbxNode->GetMesh();

    //頂点座標読み取り
    ParseMeshVertices(model, fbxMesh);
    //面を構成するデータの読み取り
    ParseMeshFaces(model, fbxMesh);
    //マテリアルの読み取り
    ParseMaterial(model, fbxNode);
}

void FbxLoader::ParseMeshVertices(FbxModel* model, FbxMesh* fbxMesh)
{
    auto& vertices = model->vertices;

    //頂点座標データの数
    const int controlPointsCount =
        fbxMesh->GetControlPointsCount();
    //必要数だけ頂点データ配列を確保
    FbxModel::VertexPosNormalUv vert{};
    model->vertices.resize(controlPointsCount, vert);

    //FBXメッシュの頂点座標配列を取得
    FbxVector4* pCoord = fbxMesh->GetControlPoints();

    //FBXメッシュの全頂点座標ｗモデル内の配列にコピーする
    for (int i = 0; i < controlPointsCount; i++)
    {
        FbxModel::VertexPosNormalUv& vertex = vertices[i];
        vertex.pos.x = (float)pCoord[i][0];
        vertex.pos.y = (float)pCoord[i][1];
        vertex.pos.z = (float)pCoord[i][2];
    }
}

void FbxLoader::ParseMeshFaces(FbxModel* model, FbxMesh* fbxMesh)
{
    auto& vertices = model->vertices;
    auto& indices = model->indices;

    //ファイルに複数メッシュのモデルは非対応
    assert(indices.size() == 0);
    //面の数
    const int polygonCount = fbxMesh->GetPolygonCount();
    //UVデータの数
    const int textureUVCount = fbxMesh->GetTextureUVCount();
    //UV名リスト
    FbxStringList uvNames;
    fbxMesh->GetUVSetNames(uvNames);
    //面ごとの情報読み取り
    for (int i = 0; i < polygonCount; i++)
    {
        //面を構成する頂点の数w取得(３なら三角ポリゴン）
        const int polygonSize = fbxMesh->GetPolygonSize(i);
        assert(polygonSize <= 4);

        //１頂点ずつ処理
        for (int j = 0; j < polygonSize; j++)
        {
            //ｆｂｘ頂点配列のインデックス
            int index = fbxMesh->GetPolygonVertex(i, j);
            assert(index >= 0);

            //頂点法線読み込み
            FbxModel::VertexPosNormalUv& vertex = vertices[index];
            FbxVector4 normal;
            if (fbxMesh->GetPolygonVertexNormal(i, j, normal))
            {
                vertex.normal.x = (float)normal[0];
                vertex.normal.y = (float)normal[1];
                vertex.normal.z = (float)normal[2];
            }

            //テクスチャＵＶ読み込み
            if (textureUVCount > 0)
            {
                FbxVector2 uvs;
                bool lUnmappedUV;
                //０番決め打ち
                if (fbxMesh->GetPolygonVertexUV(i, j, uvNames[0], uvs, lUnmappedUV))
                {
                    vertex.uv.x = (float)uvs[0];
                    vertex.uv.y = (float)uvs[1];

                    //インデックス配列に頂点インデックス追加
                    //３頂点目までなら
                    if (j < 3)
                    {
                        //１頂点を追加し、ほかの二点と三角形を構築する
                        indices.push_back(index);
                    }
                    //４頂点目
                    else 
                    {
                        //3点追加し、
                        //４角形の０，１，２，３，の内、２，３，０で三角形を構築する
                        int index2 = indices[indices.size() - 1];
                        int index3 = index;
                        int index0 = indices[indices.size() - 3];

                        indices.push_back(index2);
                        indices.push_back(index3);
                        indices.push_back(index0);
                    }
                }
            }

        }
    }
}

void FbxLoader::ParseMaterial(FbxModel* model, FbxNode* fbxNode)
{
    const int materialCount = fbxNode->GetMaterialCount();
    if (materialCount > 0)
    {
        //先頭マテリアルを取得
        FbxSurfaceMaterial* material = fbxNode->GetMaterial(0);
        //テクスチャを読み込んだか銅貨を表すフラグ
        bool textureLoaded = false;

        if (material)
        {
            if (material->GetClassId().Is(FbxSurfaceLambert::ClassId))
            {
                FbxSurfaceLambert* lambert =
                    static_cast<FbxSurfaceLambert*>(material);
                //環境光係数
                FbxPropertyT<FbxDouble3> ambient = lambert->Ambient;
                model->ambient.x = (float)ambient.Get()[0];
                model->ambient.y = (float)ambient.Get()[1];
                model->ambient.z = (float)ambient.Get()[2];

                //拡散反射光係数
                FbxPropertyT<FbxDouble3> diffuse = lambert->Diffuse;
                model->diffuse.x = (float)ambient.Get()[0];
                model->diffuse.y = (float)ambient.Get()[1];
                model->diffuse.z = (float)ambient.Get()[2];

                //ディフューズテクスチャを取り出す
                const FbxProperty diffuseProperty =
                    material->FindProperty(FbxSurfaceMaterial::sDiffuse);
                if (diffuseProperty.IsValid())
                {
                    const FbxFileTexture* texture = diffuseProperty.GetSrcObject<FbxFileTexture>();
                    if (texture)
                    {
                        const char* filepath = texture->GetFileName();
                        //ファイルパスからファイル名抽出
                        string path_str(filepath);
                        string name = ExtractFileName(path_str);
                        //テクスチャ読み込み
                        LoadTexture(model, baseDirectory + model->name_ + "/" + name);
                        textureLoaded = true;
                    }
                }
            }

        }
        //テクスチャがない場合は白テクスチャを貼る
        if (!textureLoaded)
        {
            LoadTexture(model, baseDirectory + defaultTextureFileName);
        }
    }
}

void FbxLoader::LoadTexture(FbxModel* model, const std::string& fullpath)
{
    HRESULT result = S_FALSE;
    //WICテクスチャロード
    TexMetadata& metadata = model->metadata;
    ScratchImage& scratchImg = model->scratchImage;
    //ユニコード文字列に変換
    wchar_t wfilepath[128];
    MultiByteToWideChar(CP_ACP, 0, fullpath.c_str(), -1, wfilepath, _countof(wfilepath));
    result = LoadFromWICFile(
        wfilepath, WIC_FLAGS_NONE,
        &metadata, scratchImg);
    if (FAILED(result))
    {
        assert(0);
    }
}

std::string FbxLoader::ExtractFileName(const std::string& path)
{
    size_t pos1;
    // 区切り文字 '￥￥'が出てくる一番最後の部分を検索
    pos1 = path.rfind('¥¥');
    if (pos1 != string::npos)
    {
        return path.substr(pos1 + 1, path.size() - pos1 - 1);
    }
    //区切り文字'/'がでてくる一番最後の部分を検索
    pos1 = path.rfind('/');
    if (pos1 != string::npos)
    {
        return path.substr(pos1 + 1, path.size() - pos1 - 1);
    }

    return path;
}

void FbxLoader::Finalize()
{
    fbxImporter->Destroy();
    fbxManager->Destroy();

}


/// <summary>
/// 静的メンバ変数の実体
/// </summary>
//const std::string FbxLoader::baseDirectory = "Resources/";
//const std::string FbxLoader::defaultTextureFileName = "white1x1.png";

//FbxLoader* FbxLoader::GetInstance()
//{
//	static FbxLoader instance;
//	return &instance;
//}
//
////void FbxLoader::ConvertMatrixFromFbx(DirectX::XMMATRIX* dst, const FbxAMatrix& src) {
////
////	for (int i = 0; i < 4; i++) {
////		for (int j = 0; j < 4; j++) {
////			dst->r[i].m128_f32[j] = (float)src.Get(i, j);
////		}
////	}
////
////}
//
//void FbxLoader::Initialize(ID3D12Device* device)
//{
//	// 引数からメンバ変数に代入
//	this->device = device;
//
//	flag |= aiProcess_Triangulate;
//	flag |= aiProcess_PreTransformVertices;
//	flag |= aiProcess_CalcTangentSpace;
//	flag |= aiProcess_GenSmoothNormals;
//	flag |= aiProcess_GenUVCoords;
//	flag |= aiProcess_RemoveRedundantMaterials;
//	flag |= aiProcess_OptimizeMeshes;
//
//	flag |= aiProcess_ConvertToLeftHanded;
//}
//
//void FbxLoader::Finalize()
//{
//
//}
//
//FbxModel* FbxLoader::LoadModelFromFile(const string& modelName)
//{
//	// モデルと同じ名前のフォルダから読み込む
//	const string directoryPath = baseDirectory + modelName + "/";
//	// 拡張子.fbxを付加
//	const string fileName = modelName + ".fbx";
//	// 連結してフルパスを得る
//	const string fullpath = directoryPath + fileName;
//
//	mScene = aiImportFile(fullpath.c_str(), flag);
//
//	// ファイル名を指定してFBXファイルを読み込む
//	if (!mScene) {
//		assert(0);
//	}
//
//
//	// モデル生成
//	FbxModel* model = new FbxModel();
//	model->name_ = modelName;
//
//	// FBXノードの数を取得
//	UINT32 nodeCount = 0;
//	GetNodeNum(mScene->mRootNode, nodeCount);
//
//	// あらかじめ必要数分のメモリを確保することで、アドレスがずれるのを予防
//	model->nodes.reserve(nodeCount);
//
//
//	// ルートノードから順に解析してモデルに流し込む
//	ParseNodeRecursive(model, mScene->mRootNode);
//	// FBXシーン解放
//	aiReleaseImport(mScene);
//
//	return model;
//}
//
////void FbxLoader::ParseSkin(Model* model, FbxMesh* fbxMesh) {
////
////	FbxSkin* fbxSkin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(0, FbxDeformer::eSkin));
////
////	if (fbxSkin == nullptr) {
////
////		//各頂点について処理
////		for (int i = 0; i < model->vertices.size(); i++) {
////			//最初のボーン(単位行列)の影響を100%にする
////			model->vertices[i].boneIndex[0] = 0;
////			model->vertices[i].boneWeight[0] = 1.0f;
////		}
////
////		return;
////	}
////
////	std::vector<Model::Bone>& bones = model->bones;
////
////	int clusterCount = fbxSkin->GetClusterCount();
////	bones.reserve(clusterCount);
////
////	for (int i = 0; i < clusterCount; i++) {
////
////		FbxCluster* fbxCluster = fbxSkin->GetCluster(i);
////
////		const char* boneName = fbxCluster->GetLink()->GetName();
////
////		bones.emplace_back(Model::Bone(boneName));
////		Model::Bone& bone = bones.back();
////
////		bone.fbxCluster = fbxCluster;
////
////		FbxAMatrix fbxMat;
////		fbxCluster->GetTransformLinkMatrix(fbxMat);
////
////		XMMATRIX initialPose;
////		ConvertMatrixFromFbx(&initialPose, fbxMat);
////
////		bone.invInitialPose = XMMatrixInverse(nullptr, initialPose);
////
////	}
////
////	struct WeightSet {
////		UINT index;
////		float weight;
////	};
////
////	std::vector<std::list<WeightSet>> weightLists(model->vertices.size());
////
////	for (int i = 0; i < clusterCount; i++) {
////
////		FbxCluster* fbxCluster = fbxSkin->GetCluster(i);
////
////		int controlPointIndicesCount = fbxCluster->GetControlPointIndicesCount();
////
////		int* controlPointIndices = fbxCluster->GetControlPointIndices();
////		double* controlPointWeights = fbxCluster->GetControlPointWeights();
////
////		for (int j = 0; j < controlPointIndicesCount; j++) {
////			int vertIndex = controlPointIndices[j];
////
////			float weight = (float)controlPointWeights[j];
////
////			weightLists[vertIndex].emplace_back(WeightSet{ (UINT)i,weight });
////		}
////	}
////
////	auto& vertices = model->vertices;
////
////	for (int i = 0; i < vertices.size(); i++) {
////
////		auto& weightList = weightLists[i];
////
////		weightList.sort([](auto const& lhs, auto const& rhs) {
////
////			return lhs.weight > rhs.weight;
////			});
////
////		int weightArrayIndex = 0;
////
////		for (auto& weightSet : weightList) {
////
////			vertices[i].boneIndex[weightArrayIndex] = weightSet.index;
////			vertices[i].boneWeight[weightArrayIndex] = weightSet.weight;
////
////			if (++weightArrayIndex >= FbxModel::MAX_BONE_INDICES) {
////				float weight = 0.0f;
////
////				for (int j = 1; j < Model::MAX_BONE_INDICES; j++) {
////					weight += vertices[i].boneWeight[j];
////				}
////				vertices[i].boneWeight[0] = 1.0f - weight;
////				break;
////			}
////
////		}
////
////	}
////
////}
//
//void FbxLoader::ParseNodeRecursive(FbxModel* model, aiNode* fbxNode, Node* parent)
//{
//	//// ノード名を取得
//	//string name = fbxNode->GetName();
//	// モデルにノードを追加
//	model->nodes.emplace_back();
//	Node& node = model->nodes.back();
//	// ノード名を取得
//	node.name = fbxNode->mName.C_Str();
//
//	node.transform =
//	{
//		fbxNode->mTransformation.a1, fbxNode->mTransformation.a2, fbxNode->mTransformation.a3, fbxNode->mTransformation.a4,
//		fbxNode->mTransformation.b1, fbxNode->mTransformation.b2, fbxNode->mTransformation.b3, fbxNode->mTransformation.b4,
//		fbxNode->mTransformation.c1, fbxNode->mTransformation.c2, fbxNode->mTransformation.c3, fbxNode->mTransformation.c4,
//		fbxNode->mTransformation.d1, fbxNode->mTransformation.d2, fbxNode->mTransformation.d3, fbxNode->mTransformation.d4,
//	};
//
//	//グローバル変形行列の計算
//	node.globalTransform = node.transform;
//
//	if (parent) {
//		node.parent = parent;
//		// 親の変形を乗算
//		node.globalTransform *= parent->globalTransform;
//	}
//
//	for (UINT32 i = 0; i < fbxNode->mNumMeshes; i++) {
//		aiMesh* aimesh = mScene->mMeshes[fbxNode->mMeshes[i]];
//		if (aimesh) {
//			model->meshes_.emplace_back();
//			model->meshes_.back() = new Mesh();
//			model->meshes_.back()->name_ = aimesh->mName.C_Str();
//
//			ParseMesh(model, aimesh);
//		}
//	}
//
//	// 子ノードに対して再帰呼び出し
//	for (int i = 0; i < fbxNode->mNumChildren; i++) {
//		ParseNodeRecursive(model, fbxNode->mChildren[i], &node);
//	}
//}
//
//void FbxLoader::ParseMesh(FbxModel* model, aiMesh* fbxMesh)
//{
//
//	// 頂点座標読み取り
//	ParseMeshVertices(model, fbxMesh);
//	// 面を構成するデータの読み取り
//	ParseMeshFaces(model, fbxMesh);
//	// マテリアルの読み取り
//	ParseMaterial(model, fbxMesh, mScene->mMaterials[fbxMesh->mMaterialIndex]);
//
//	//スキニング情報の読み取り
//	//ParseSkin(model, fbxMesh);
//
//}
//
//void FbxLoader::ParseMeshVertices(FbxModel* model, aiMesh* fbxMesh)
//{
//	auto& vertices = model->meshes_.back()->vertices_;
//
//	// 頂点座標データの数
//	const int controlPointsCount = fbxMesh->mNumVertices;
//
//	// 必要数だけ頂点データ配列を確保
//	Mesh::VertexPosNormalUv vert{};
//	model->meshes_.back()->vertices_.resize(controlPointsCount, vert);
//
//
//	// FBXメッシュの全頂点座標をモデル内の配列にコピーする。
//	for (int i = 0; i < controlPointsCount; i++) {
//		Mesh::VertexPosNormalUv& vertex = vertices[i];
//		// 座標のコピー
//		aiVector3D position = fbxMesh->mVertices[i];
//
//		vertex.pos.x = position.x;
//		vertex.pos.y = position.y;
//		vertex.pos.z = position.z;
//
//		// 法線のコピー
//		aiVector3D normal = fbxMesh->mNormals[i];
//
//		vertex.normal.x = normal.x;
//		vertex.normal.y = normal.y;
//		vertex.normal.z = normal.z;
//
//	}
//}
//
//void FbxLoader::ParseMeshFaces(FbxModel* model, aiMesh* fbxMesh)
//{
//	auto& vertices = model->meshes_.back()->vertices_;
//	auto& indices = model->meshes_.back()->indices_;
//
//	// 面の数
//	const int polygonCount = fbxMesh->mNumVertices;
//
//	aiVector3D zero3D(0.0f, 0.0f, 0.0f);
//
//	// 面ごとの情報読み取り
//	for (int i = 0; i < polygonCount; i++) {
//		aiVector3D* uv = (fbxMesh->HasTextureCoords(0)) ? &(fbxMesh->mTextureCoords[0][i]) : &zero3D;
//
//		vertices[i].uv = Vector2(uv->x,uv->y);
//	}
//	
//	indices.resize(fbxMesh->mNumFaces * 3);
//
//	for (UINT i = 0; i < fbxMesh->mNumFaces; i++) {
//		const aiFace& face = fbxMesh->mFaces[i];
//
//		for (UINT j = 0; j < face.mNumIndices; j++) {
//			indices[i * 3 + j] = face.mIndices[j];
//		}
//
//	}
//
//}
//
//void FbxLoader::ParseMaterial(FbxModel* model, aiMesh* fbxMesh, aiMaterial* aimaterial)
//{
//
//	auto& material = model->meshes_.back()->material_;
//
//	material = Material::Create();
//
//	aiColor3D ambient(0.3f, 0.3f, 0.3f);
//	aimaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
//	material->ambient_ = XMFLOAT3(ambient.r, ambient.g, ambient.b);
//
//	aiColor3D diffuse(0.0f, 0.0f, 0.0f);
//	aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
//	material->diffuse_ = XMFLOAT3(diffuse.r, diffuse.g, diffuse.b);
//
//	aiColor3D specular(0.0f, 0.0f, 0.0f);
//	aimaterial->Get(AI_MATKEY_COLOR_SPECULAR, specular);
//	material->specular_ = XMFLOAT3(specular.r, specular.g, specular.b);
//
//	aiString str;
//	aimaterial->Get(AI_MATKEY_NAME, str);
//	material->name_ = str.C_Str();
//
//	std::vector<uint32_t> deffuseMap = LoadMatrixerialTextures(aimaterial, aiTextureType_DIFFUSE, "Diffuse", mScene, model->name_);
//	uint32_t map = deffuseMap.size();
//
//	material->SetTextureHadle(map);
//
//}
//
//
//std::string FbxLoader::ExtractFileName(const std::string& path)
//{
//	size_t pos1;
//	// 区切り文字 '\\' が出てくる一番最後の部分を検索
//	pos1 = path.rfind('\\');
//	if (pos1 != string::npos) {
//		return path.substr(pos1 + 1, path.size() - pos1 - 1);
//	}
//	// 区切り文字 '/' が出てくる一番最後の部分を検索
//	pos1 = path.rfind('/');
//	if (pos1 != string::npos) {
//		return path.substr(pos1 + 1, path.size() - pos1 - 1);
//	}
//
//	return path;
//}
//
//void FbxLoader::GetNodeNum(const aiNode* node, UINT32& num)
//{
//	if (node->mChildren)
//	{
//		for (UINT32 i = 0; i < node->mNumChildren; i++)
//		{
//			GetNodeNum(node->mChildren[i], num);
//		}
//	}
//
//	num++;
//}
//
//std::vector<uint32_t> FbxLoader::LoadMatrixerialTextures(aiMaterial* cmatrix, aiTextureType type, std::string typeName, const aiScene* scene_,const std::string& modelName)
//{
//	std::vector<uint32_t> textures;
//
//	for (size_t i = 0; i < cmatrix->GetTextureCount(type); i++)
//	{
//		aiString str;
//		cmatrix->GetTexture(type, static_cast<UINT>(i), &str);
//		uint32_t texture;
//		{
//			std::string filename = ExtractFileName(std::string(str.C_Str()));
//			filename = modelName + '\\' + filename;
//			texture = TextureManager::Load(filename);
//		}
//
//		textures.push_back(texture);
//	}
//	return textures;
//}


