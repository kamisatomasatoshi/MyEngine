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
    /// �p�C�v���C������
    /// </summary>
    static void CreatGraphicsPipelineState();

    /// <summary>
    /// �V�[���`��O����
    /// </summary>
    /// <param name="cmdList">�R�}���h���X�g</param>
    static void PreDrawScene(ID3D12GraphicsCommandList* cmdList);

    static void Draw(ID3D12GraphicsCommandList* cmdList);

    /// <summary>
    /// �V�[���`��㏈��
    /// </summary>
    /// <param name="cmdList">�R�}���h����</param>
    static void PostDrawScene();

 
   

private://�ÓI�����o�ϐ�

    Input* input = nullptr;
    static const float clearColor[4];

    Vector4 color_ = { 1, 1, 1, 1 };

    static ID3D12Device* device_;

    static ID3D12GraphicsCommandList* commandList;

    static Sprite::VertexPosUv vertices[4];

    static Sprite::VertexPosUv* vertMap;

    static Sprite::ConstBufferData constData;

    static Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff;	//���_�o�b�t�@

    //���_�o�b�t�@�r���[�̍쐬
    static D3D12_VERTEX_BUFFER_VIEW vbView;
    static Microsoft::WRL::ComPtr<ID3D12Resource> texBuff[2];

    static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeapSRV;
    //�[�x�o�b�t�@
    static Microsoft::WRL::ComPtr<ID3D12Resource> depthBuff;
    //RTV�p�̃f�X�N���v�^�q�[�v
    static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeapRTV;
    //DSV�p�̃f�X�N���v�^�q�[�v
    static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeapDSV;

    static Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
    static Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;

    static Microsoft::WRL::ComPtr<ID3D12Resource> constBuff; // �萔�o�b�t�@
};

