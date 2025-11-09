#include <merc/module.h>
#include <merc/windows-utils.h>
#include <merc/paths.h>
#include <merc/vstvideoeffect.h>
#include <windows.h>
#include <wrl.h>
using Microsoft::WRL::ComPtr;
#include <d3d12.h>
#include <D3DCompiler.h>

struct Triangle : merc::VideoEffect
{
    static const char* getCategory() { return kVstVideoEffectClass; }
    static const char* getSubCategory() { return merc::videoSubCategory::object; }
    static const char* getName() { return "triangle"; }

    Steinberg::tresult PLUGIN_API setup(ID3D12Device& device, DXGI_FORMAT renderTargetFormat, UINT msaaSampleCount) override
    {
        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc
        {
            .Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
        };
        ComPtr<ID3DBlob> serializedRootSignature;
        ComPtr<ID3DBlob> rootSignatureSerializationError;
        THROW_IF_FAILED(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serializedRootSignature, &rootSignatureSerializationError));
        THROW_IF_FAILED(device.CreateRootSignature(0, serializedRootSignature->GetBufferPointer(), serializedRootSignature->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));

        ComPtr<ID3DBlob> vertexShader;
        ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
        UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        UINT compileFlags = 0;
#endif
        const std::wstring shadersPath{ (merc::getModuleResourceDir() / "triangle.hlsl").wstring() };
        THROW_IF_FAILED(D3DCompileFromFile(shadersPath.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
        THROW_IF_FAILED(D3DCompileFromFile(shadersPath.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));

        D3D12_INPUT_ELEMENT_DESC inputElementDescs[]
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc
        {
            .pRootSignature = rootSignature.Get(),
            .VS = { vertexShader->GetBufferPointer(), vertexShader->GetBufferSize() },
            .PS = { pixelShader->GetBufferPointer(), pixelShader->GetBufferSize() },
            .BlendState =
            {
                .RenderTarget =
            {
                {
                    FALSE,FALSE,
                    D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
                    D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
                    D3D12_LOGIC_OP_NOOP,
                    D3D12_COLOR_WRITE_ENABLE_ALL,
                }
        }
        },
            .SampleMask = UINT_MAX,
            .RasterizerState =
            {
                .FillMode = D3D12_FILL_MODE_SOLID,
                .CullMode = D3D12_CULL_MODE_BACK,
                .DepthClipEnable = TRUE,
                .MultisampleEnable = TRUE,
                .AntialiasedLineEnable = FALSE,
                .ForcedSampleCount = 0
        },
            .InputLayout = { inputElementDescs, _countof(inputElementDescs) },
            .PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
            .NumRenderTargets = 1,
            .RTVFormats = { renderTargetFormat },
            .SampleDesc = { .Count = msaaSampleCount },
        };
        THROW_IF_FAILED(device.CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)));

        // Note: using upload heaps to transfer static data like vert buffers is not
        // recommended. Every time the GPU needs it, the upload heap will be marshalled
        // over. Please read up on Default Heap usage. An upload heap is used here for
        // code simplicity and because there are very few verts to actually transfer.
        D3D12_HEAP_PROPERTIES vertexBufferHeapProperties{ .Type = D3D12_HEAP_TYPE_UPLOAD };
        D3D12_RESOURCE_DESC vertexBufferResourceDesc
        {
            .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
            .Width = vertexBufferSize,
            .Height = 1,
            .DepthOrArraySize = 1,
            .MipLevels = 1,
            .SampleDesc = { .Count = 1 },
            .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR
        };
        THROW_IF_FAILED(device.CreateCommittedResource(
            &vertexBufferHeapProperties,
            D3D12_HEAP_FLAG_NONE,
            &vertexBufferResourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&vertexBuffer)));
        vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
        vertexBufferView.StrideInBytes = vertexBufferStride;
        vertexBufferView.SizeInBytes = vertexBufferSize;

        return Steinberg::kResultOk;
    }

    Steinberg::tresult PLUGIN_API draw(ID3D12GraphicsCommandList& commandList) override
    {
        float* vertexData;
        D3D12_RANGE vertexBufferReadRange{ 0, 0 };
        THROW_IF_FAILED(vertexBuffer->Map(0, &vertexBufferReadRange, reinterpret_cast<void**>(&vertexData)));
        vertexData[ 0] = 0.0f;  vertexData[ 1] = 0.4f;  vertexData[ 2] = 0.0f;
        vertexData[ 3] = 1.0f; vertexData[ 4] = 1.0f; vertexData[ 5] = 1.0f; vertexData[ 6] = 1.0f;
        vertexData[ 7] = 0.25f; vertexData[ 8] = -0.2f; vertexData[ 9] = 0.0f;
        vertexData[10] = 1.0f; vertexData[11] = 1.0f; vertexData[12] = 1.0f; vertexData[13] = 1.0f;
        vertexData[14] = -0.25f; vertexData[15] = -0.2f; vertexData[16] = 0.0f;
        vertexData[17] = 0.0f; vertexData[18] = 1.0f; vertexData[19] = 1.0f; vertexData[20] = 1.0f;
        vertexBuffer->Unmap(0, nullptr);

        commandList.SetGraphicsRootSignature(rootSignature.Get());
        commandList.SetPipelineState(pipelineState.Get());

        commandList.IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList.IASetVertexBuffers(0, 1, &vertexBufferView);
        commandList.DrawInstanced(3, 1, 0, 0);

        return Steinberg::kResultOk;
    }

    ComPtr<ID3D12RootSignature> rootSignature;
    ComPtr<ID3D12PipelineState> pipelineState;
    static constexpr UINT vertexBufferStride{ 7 * sizeof(float) };
    static constexpr UINT vertexBufferSize{ 3 * vertexBufferStride };
    ComPtr<ID3D12Resource> vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
};

template struct merc::Module<Triangle>;
