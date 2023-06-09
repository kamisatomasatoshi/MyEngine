﻿#include "FbxLoader.h"

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

void FbxLoader::ConvertMatrixFromFbx(DirectX::XMMATRIX* dst, const FbxAMatrix& src)
{
    //行
    for (int i = 0; i < 4; i++)
    {
        //列
        for (int j = 0; j < 4; j++)
        {
            //1要素コピー
            dst->r[i].m128_f32[j] = (float)src.Get(i, j);

        }
    }
}

void FbxLoader::Initialize(ID3D12Device* device)
{
    // 再初期化チェック
    assert(fbxManager == nullptr);

    // 引数からメンバ変数に代入
    this->device = device;

    // FBXマネージャの生成
    fbxManager = FbxManager::Create();

    // FBXマネージャの入出力設定
    FbxIOSettings* ios = FbxIOSettings::Create(fbxManager, IOSROOT);
    fbxManager->SetIOSettings(ios);

    // FBXインポータの生成
    fbxImporter = FbxImporter::Create(fbxManager, "");
}

void FbxLoader::Finalize()
{
    // 各種FBXインスタンスの破棄
    fbxImporter->Destroy();
    fbxManager->Destroy();
}

FbxModel* FbxLoader::LoadModelFromFile(const string& modelName)
{
    // モデルと同じ名前のフォルダから読み込む
    const string directoryPath = baseDirectory + modelName + "/";
    // 拡張子.fbxを付加
    const string fileName = modelName + ".fbx";
    // 連結してフルパスを得る
    const string fullpath = directoryPath + fileName;

    // ファイル名を指定してFBXファイルを読み込む
    if (!fbxImporter->Initialize(fullpath.c_str(), -1, fbxManager->GetIOSettings())) {
        assert(0);
    }

    // シーン生成
    FbxScene* fbxScene = FbxScene::Create(fbxManager, "fbxScene");

    // ファイルからロードしたFBXの情報をシーンにインポート
    fbxImporter->Import(fbxScene);

    // モデル生成
    FbxModel* model = new FbxModel();
    model->name_ = modelName;
    // FBXノードの数を取得
    int nodeCount = fbxScene->GetNodeCount();
    // あらかじめ必要数分のメモリを確保することで、アドレスがずれるのを予防
    model->nodes.reserve(nodeCount);
    // ルートノードから順に解析してモデルに流し込む
    ParseNodeRecursive(model, fbxScene->GetRootNode());
    // FBXシーン解放
    //fbxScene->Destroy();
    model->fbxScene = fbxScene;
    // バッファ生成
    model->CreateBuffers(device);

    return model;
}

void FbxLoader::ParseNodeRecursive(FbxModel* model, FbxNode* fbxNode, Node* parent)
{
    //// ノード名を取得
    //string name = fbxNode->GetName();
    // モデルにノードを追加
    model->nodes.emplace_back();
    Node& node = model->nodes.back();
    // ノード名を取得
    node.name = fbxNode->GetName();

    // FBXノードのローカル移動情報
    FbxDouble3 rotation = fbxNode->LclRotation.Get();
    FbxDouble3 scaling = fbxNode->LclScaling.Get();
    FbxDouble3 translation = fbxNode->LclTranslation.Get();

    // 形式変換して代入
    node.rotation = { (float)rotation[0], (float)rotation[1], (float)rotation[2], 0.0f };
    node.scaling = { (float)scaling[0], (float)scaling[1], (float)scaling[2], 0.0f };
    node.translation = { (float)translation[0], (float)translation[1], (float)translation[2], 1.0f };

    // スケール、回転、平行移動行列の計算
    XMMATRIX matScaling, matRotation, matTranslation;
    matScaling = XMMatrixScalingFromVector(node.scaling);
    matRotation = XMMatrixRotationRollPitchYawFromVector(node.rotation);
    matTranslation = XMMatrixTranslationFromVector(node.translation);

    // ローカル変形行列の計算
    node.transform = XMMatrixIdentity();
    node.transform *= matScaling; // ワールド行列にスケーリングを反映
    node.transform *= matRotation; // ワールド行列に回転を反映
    node.transform *= matTranslation; // ワールド行列に平行移動を反映

    //グローバル変形行列の計算
    node.globalTransform = node.transform;
    if (parent) {
        node.parent = parent;
        // 親の変形を乗算
        node.globalTransform *= parent->globalTransform;
    }

    // FBXノードのメッシュ情報を解析
    FbxNodeAttribute* fbxNodeAttribute = fbxNode->GetNodeAttribute();

    if (fbxNodeAttribute) {
        if (fbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
        {
            model->meshNode = &node;
            ParseMesh(model, fbxNode);
        }
    }

    // 子ノードに対して再帰呼び出し
    for (int i = 0; i < fbxNode->GetChildCount(); i++) {
        ParseNodeRecursive(model, fbxNode->GetChild(i), &node);
    }
}

void FbxLoader::ParseMesh(FbxModel* model, FbxNode* fbxNode)
{
    // ノードのメッシュを取得
    FbxMesh* fbxMesh = fbxNode->GetMesh();

    // 頂点座標読み取り
    ParseMeshVertices(model, fbxMesh);
    // 面を構成するデータの読み取り
    ParseMeshFaces(model, fbxMesh);
    // マテリアルの読み取り
    ParseMaterial(model, fbxNode);
    //スキニング
    ParseSkin(model, fbxMesh);

}

void FbxLoader::ParseMeshVertices(FbxModel* model, FbxMesh* fbxMesh)
{
    auto& vertices = model->vertices;

    // 頂点座標データの数
    const int controlPointsCount = fbxMesh->GetControlPointsCount();
    // 必要数だけ頂点データ配列を確保
    FbxModel::VertexPosNormalUv vert{};
    model->vertices.resize(controlPointsCount, vert);

    // FBXメッシュの頂点座標配列を取得
    FbxVector4* pCoord = fbxMesh->GetControlPoints();

    // FBXメッシュの全頂点座標をモデル内の配列にコピーする。
    for (int i = 0; i < controlPointsCount; i++) {
        FbxModel::VertexPosNormalUv& vertex = vertices[i];
        // 座標のコピー
        vertex.pos.x = (float)pCoord[i][0];
        vertex.pos.y = (float)pCoord[i][1];
        vertex.pos.z = (float)pCoord[i][2];
    }
}

void FbxLoader::ParseMeshFaces(FbxModel* model, FbxMesh* fbxMesh)
{
    auto& vertices = model->vertices;
    auto& indices = model->indices;

    // 1ファイルに複数メッシュのモデルは非対応
    assert(indices.size() == 0);

    // 面の数
    const int polygonCount = fbxMesh->GetPolygonCount();

    // UVデータの数
    const int textureUVCount = fbxMesh->GetTextureUVCount();
    // UV名リスト
    FbxStringList uvNames;
    fbxMesh->GetUVSetNames(uvNames);

    // 面ごとの情報読み取り
    for (int i = 0; i < polygonCount; i++) {
        // 面を構成する頂点の数を取得（3なら三角形ポリゴン)
        const int polygonSize = fbxMesh->GetPolygonSize(i);
        assert(polygonSize <= 4);

        // 1頂点ずつ処理
        for (int j = 0; j < polygonSize; j++) {
            // FBX頂点配列のインデックス
            int index = fbxMesh->GetPolygonVertex(i, j);
            assert(index >= 0);

            // 頂点法線読込
            FbxModel::VertexPosNormalUv& vertex = vertices[index];
            FbxVector4 normal;
            if (fbxMesh->GetPolygonVertexNormal(i, j, normal)) {
                vertex.normal.x = (float)normal[0];
                vertex.normal.y = (float)normal[1];
                vertex.normal.z = (float)normal[2];
            }

            // テクスチャUV読込
            if (textureUVCount > 0) {
                FbxVector2 uvs;
                bool lUnmappedUV;
                // 0番決め打ちで読込
                if (fbxMesh->GetPolygonVertexUV(i, j, uvNames[0], uvs, lUnmappedUV)) {
                    vertex.uv.x = (float)uvs[0];
                    vertex.uv.y = (float)uvs[1];
                }
            }

            // インデックス配列に頂点インデックス追加
            // 3頂点目までなら
            if (j < 3) {
                // 1点追加し、他の2点と三角形を構築する
                indices.push_back(index);
            }
            // 4頂点目
            else {
                // 3点追加し、四角形の0,1,2,3の内 2,3,0で三角形を構築する
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

void FbxLoader::ParseMaterial(FbxModel* model, FbxNode* fbxNode)
{
    const int materialCount = fbxNode->GetMaterialCount();
    if (materialCount > 0) {
        // 先頭のマテリアルを取得
        FbxSurfaceMaterial* material = fbxNode->GetMaterial(0);

        bool textureLoaded = false;

        if (material) {

            // FbxSurfaceLambertクラスかどうかを調べる
            if (material->GetClassId().Is(FbxSurfaceLambert::ClassId))
            {
                FbxSurfaceLambert* lambert = static_cast<FbxSurfaceLambert*>(material);

                //環境光係数
                FbxPropertyT<FbxDouble3> ambient = lambert->Ambient;
                model->ambient.x = (float)ambient.Get()[0];
                model->ambient.y = (float)ambient.Get()[1];
                model->ambient.z = (float)ambient.Get()[2];

                //拡散反射光係数
                FbxPropertyT<FbxDouble3> diffuse = lambert->Diffuse;
                model->diffuse.x = (float)diffuse.Get()[0];
                model->diffuse.y = (float)diffuse.Get()[1];
                model->diffuse.z = (float)diffuse.Get()[2];
            }

            // ディフューズテクスチャを取り出す
            const FbxProperty diffuseProperty = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
            if (diffuseProperty.IsValid())
            {
                const FbxFileTexture* texture = diffuseProperty.GetSrcObject<FbxFileTexture>();
                if (texture) {
                    const char* filepath = texture->GetFileName();
                    // ファイルパスからファイル名抽出
                    string path_str(filepath);
                    string name = ExtractFileName(path_str);
                    // テクスチャ読み込み
                    LoadTexture(model, baseDirectory + model->name_ + "/" + name);
                    textureLoaded = true;
                }
            }
        }

        // テクスチャがない場合は白テクスチャを貼る
        if (!textureLoaded) {
            LoadTexture(model, baseDirectory + defaultTextureFileName);
        }
    }
}

void FbxLoader::LoadTexture(FbxModel* model, const std::string& fullpath)
{
    HRESULT result = S_FALSE;

    // WICテクスチャのロード
    TexMetadata& metadata = model->metadata;
    ScratchImage& scratchImg = model->scratchImage;

    // ユニコード文字列に変換
    wchar_t wfilepath[128];
    MultiByteToWideChar(CP_ACP, 0, fullpath.c_str(), -1, wfilepath, _countof(wfilepath));

    result = LoadFromWICFile(
        wfilepath, WIC_FLAGS_NONE,
        &metadata, scratchImg);
    if (FAILED(result)) {
        assert(0);
    }
}

void FbxLoader::ParseSkin(FbxModel* model, FbxMesh* fbxMesh)
{
    FbxSkin* fbxSkin =
        static_cast<FbxSkin*>(fbxMesh->GetDeformer(0, FbxDeformer::eSkin));
    //スキニング情報がないなら終了
    if (fbxSkin == nullptr)
    {
        //各頂点についての処理
        for (int i = 0; i < model->vertices.size(); i++)
        {
            //最初のボーン（単位行列）の影響１００％にする
            model->vertices[i].boneIndex[0] = 0;
            model->vertices[i].boneWeight[0] = 1.0f;
        }

        return;
    }

    //ボーン配列の参照
    std::vector<FbxModel::Bone>& bones = model->bones;

    //ボーンの数
    int clusterCount = fbxSkin->GetClusterCount();
    bones.reserve(clusterCount);

    //全てのボーンについて
    for (int i = 0; i < clusterCount; i++)
    {
        //FBXボーン情報
        FbxCluster* fbxCluster = fbxSkin->GetCluster(i);

        //ボーン自体のノードの名前を取得
        const char* boneName = fbxCluster->GetLink()->GetName();
        
        //新しくボーンを追加し、追加したボーンの参照を得る
        bones.emplace_back(FbxModel::Bone(boneName));
        FbxModel::Bone& bone = bones.back();
        //自作ボーンとFBXのボーンを紐づける
        bone.fbxCluster = fbxCluster;

        //FBXから初期姿勢行列を取得する
        FbxAMatrix fbxMat;
        fbxCluster->GetTransformLinkMatrix(fbxMat);

        //XMMatrix型に変換する
        XMMATRIX initialPose;
        ConvertMatrixFromFbx(&initialPose, fbxMat);

        //初期姿勢行列の逆行列を得る
        bone.invInitialPose = XMMatrixInverse(nullptr, initialPose);
    }

    struct WeightSet
    {
        UINT index;
        float weight;
    };

    //二次元配列（ジャグ配列）
    //List:頂点が影響を受けるボーンの全リスト
    //vector:それを全頂点分
    std::vector<std::list<WeightSet>>
        weightLists(model->vertices.size());

    //全てのボーンについて
    for (int i = 0; i < clusterCount; i++)
    {
        //ボーン情報
        FbxCluster* fbxCluster = fbxSkin->GetCluster(i);
        //このボーンに影響を受ける頂点の数
        int controlPointIndicesCount = fbxCluster->GetControlPointIndicesCount();
        //このボーンに影響を受ける頂点の配列
        int* controlPointIndices = fbxCluster->GetControlPointIndices();
        double* controlPointWeights = fbxCluster->GetControlPointWeights();

        //影響を受ける全頂点について
        for (int j = 0; j < controlPointIndicesCount; j++)
        {
            //頂点番号
            int vertIndex = controlPointIndices[j];
            //スキンウェイト
            float weight = (float)controlPointWeights[j];
            //その頂点の影響を受けるボーンリストに、ボーンとウェイトのペアを追加
            weightLists[vertIndex].emplace_back(WeightSet{ (UINT)i, weight });
        }

    }
    //頂点配列書き換え用の参照
    auto& vertices = model->vertices;
    //各頂点について処理
    for (int i = 0; i < vertices.size(); i++)
    {
        //頂点ウエイトから最も大きい４つを選択
        auto& weightList = weightLists[i];
        //大小比較用のラムダ式を指定して降順にソート

        weightList.sort(
            [](auto const& lhs, auto const rhs)
            {
                //左の要素の方が大きければtrue それでなければfalseをかえす
                return lhs.weight > rhs.weight;
            }
        );

        int weightArrayIndex = 0;
        //降順ソート済みからのウェイトリストから
        for (auto& weightSet : weightList)
        {
            //頂点データに書き込み
            vertices[i].boneIndex[weightArrayIndex] = weightSet.index;
            vertices[i].boneWeight[weightArrayIndex] = weightSet.weight;
            //４つに達したら終了
            if (++weightArrayIndex >= FbxModel::MAX_BONE_INDICES)
            {
                float weight = 0.0f;
                //２番目以降のウェイトを合計
                for (int j = 0; j < FbxModel::MAX_BONE_INDICES; j++)
                {
                    weight += vertices[i].boneWeight[j];
                }
                //合計で1.0f(100%)になるように調整
                vertices[i].boneWeight[0] = 1.0f - weight;
                break;
            }
        }
    }
    
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


