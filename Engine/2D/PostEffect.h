#pragma once
#include "Sprite.h"
#include "WinApp.h"
#include "Input.h"
class PostEffect
{
public:
    ~PostEffect();

    static void Initialize(ID3D12Device* device);
    
    /// <summary>
    /// パイプライン生成
    /// </summary>
    static void CreatGraphicsPipelineState();

    /// <summary>
    /// シーン描画前処理
    /// </summary>
    /// <param name="cmdList">コマンドリスト</param>
    static void PreDrawScene(ID3D12GraphicsCommandList* cmdList);

    static void Draw(ID3D12GraphicsCommandList* cmdList);

    /// <summary>
    /// シーン描画後処理
    /// </summary>
    /// <param name="cmdList">コマンド処理</param>
    static void PostDrawScene();

 
   

private://静的メンバ変数

    Input* input = nullptr;
    static const float clearColor[4];

    Vector4 color_ = { 1, 1, 1, 1 };

    static ID3D12Device* device_;

    static ID3D12GraphicsCommandList* commandList;

    static Sprite::VertexPosUv vertices[4];

    static Sprite::VertexPosUv* vertMap;

    static Sprite::ConstBufferData constData;

    static Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff;	//頂点バッファ

    //頂点バッファビューの作成
    static D3D12_VERTEX_BUFFER_VIEW vbView;
    static Microsoft::WRL::ComPtr<ID3D12Resource> texBuff[2];

    static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeapSRV;
    //深度バッファ
    static Microsoft::WRL::ComPtr<ID3D12Resource> depthBuff;
    //RTV用のデスクリプタヒープ
    static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeapRTV;
    //DSV用のデスクリプタヒープ
    static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeapDSV;

    static Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
    static Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;

    static Microsoft::WRL::ComPtr<ID3D12Resource> constBuff; // 定数バッファ
};

