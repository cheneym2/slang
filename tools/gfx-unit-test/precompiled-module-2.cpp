#include "core/slang-basic.h"
#include "core/slang-blob.h"
#include "core/slang-io.h"
#include "core/slang-memory-file-system.h"
#include "gfx-test-util.h"
#include "gfx-util/shader-cursor.h"
#include "slang-gfx.h"
#include "unit-test/slang-unit-test.h"

using namespace gfx;

namespace gfx_test
{
// Test that mixing precompiled and non-precompiled modules is working.

static Slang::Result precompileProgram(
    gfx::IDevice* device,
    ISlangMutableFileSystem* fileSys,
    const char* shaderModuleName,
    bool precompileToTarget)
{
    printf("File, Line: %s, %d\n", __FILE__, __LINE__);
    fflush(stdout);
    fflush(stderr);
    Slang::ComPtr<slang::ISession> slangSession;
    SLANG_RETURN_ON_FAIL(device->getSlangSession(slangSession.writeRef()));
    slang::SessionDesc sessionDesc = {};
    auto searchPaths = getSlangSearchPaths();
    sessionDesc.searchPathCount = searchPaths.getCount();
    sessionDesc.searchPaths = searchPaths.getBuffer();
    auto globalSession = slangSession->getGlobalSession();
    globalSession->createSession(sessionDesc, slangSession.writeRef());
    printf("File, Line: %s, %d\n", __FILE__, __LINE__);
    fflush(stdout);
    fflush(stderr);
    slang::IModule* module;
    {
        Slang::ComPtr<slang::IBlob> diagnosticsBlob;
        module = slangSession->loadModule(shaderModuleName, diagnosticsBlob.writeRef());
        diagnoseIfNeeded(diagnosticsBlob);
    }
    if (!module)
        return SLANG_FAIL;
    printf("File, Line: %s, %d\n", __FILE__, __LINE__);
    fflush(stdout);
    fflush(stderr);
    if (precompileToTarget)
    {
        SlangCompileTarget target;
        switch (device->getDeviceInfo().deviceType)
        {
        case gfx::DeviceType::DirectX12:
            target = SLANG_DXIL;
            break;
        case gfx::DeviceType::Vulkan:
            target = SLANG_SPIRV;
            break;
        default:
            return SLANG_FAIL;
        }

        ComPtr<slang::IModulePrecompileService_Experimental> precompileService;
        if (module->queryInterface(
                slang::SLANG_UUID_IModulePrecompileService_Experimental,
                (void**)precompileService.writeRef()) == SLANG_OK)
        {
            Slang::ComPtr<slang::IBlob> diagnosticsBlob;
            precompileService->precompileForTarget(target, diagnosticsBlob.writeRef());
            diagnoseIfNeeded(diagnosticsBlob);
        }
    }
    printf("File, Line: %s, %d\n", __FILE__, __LINE__);
    // Write loaded modules to memory file system.
    for (SlangInt i = 0; i < slangSession->getLoadedModuleCount(); i++)
    {
        auto module = slangSession->getLoadedModule(i);
        auto path = module->getFilePath();
        if (path)
        {
            auto name = module->getName();
            ComPtr<ISlangBlob> outBlob;
            module->serialize(outBlob.writeRef());
            fileSys->saveFileBlob((Slang::String(name) + ".slang-module").getBuffer(), outBlob);
        }
    }
    printf("File, Line: %s, %d\n", __FILE__, __LINE__);
    fflush(stdout);
    fflush(stderr);
    return SLANG_OK;
}

void precompiledModule2TestImplCommon(
    IDevice* device,
    UnitTestContext* context,
    bool precompileToTarget)
{
    printf("File, Line: %s, %d\n", __FILE__, __LINE__);
    Slang::ComPtr<ITransientResourceHeap> transientHeap;
    ITransientResourceHeap::Desc transientHeapDesc = {};
    transientHeapDesc.constantBufferSize = 4096;
    GFX_CHECK_CALL_ABORT(
        device->createTransientResourceHeap(transientHeapDesc, transientHeap.writeRef()));
    printf("File, Line: %s, %d\n", __FILE__, __LINE__);
    fflush(stdout);
    fflush(stderr);
    // First, load and compile the slang source.
    ComPtr<ISlangMutableFileSystem> memoryFileSystem =
        ComPtr<ISlangMutableFileSystem>(new Slang::MemoryFileSystem());

    ComPtr<IShaderProgram> shaderProgram;
    slang::ProgramLayout* slangReflection;
    GFX_CHECK_CALL_ABORT(precompileProgram(
        device,
        memoryFileSystem.get(),
        "precompiled-module-imported",
        precompileToTarget));

    // Next, load the precompiled slang program.
    Slang::ComPtr<slang::ISession> slangSession;
    device->getSlangSession(slangSession.writeRef());
    slang::SessionDesc sessionDesc = {};
    sessionDesc.targetCount = 1;
    slang::TargetDesc targetDesc = {};
    printf("File, Line: %s, %d\n", __FILE__, __LINE__);
    switch (device->getDeviceInfo().deviceType)
    {
    case gfx::DeviceType::DirectX12:
        targetDesc.format = SLANG_DXIL;
        targetDesc.profile = device->getSlangSession()->getGlobalSession()->findProfile("sm_6_1");
        break;
    case gfx::DeviceType::Vulkan:
        targetDesc.format = SLANG_SPIRV;
        targetDesc.profile = device->getSlangSession()->getGlobalSession()->findProfile("GLSL_460");
        break;
    }
    printf("File, Line: %s, %d\n", __FILE__, __LINE__);
    fflush(stdout);
    fflush(stderr);
    sessionDesc.targets = &targetDesc;
    sessionDesc.fileSystem = memoryFileSystem.get();
    auto globalSession = slangSession->getGlobalSession();
    globalSession->createSession(sessionDesc, slangSession.writeRef());
    printf("File, Line: %s, %d\n", __FILE__, __LINE__);
    const char* moduleSrc = R"(
            import "precompiled-module-imported";

            // Main entry-point. 

            using namespace ns;

            [shader("compute")]
            [numthreads(4, 1, 1)]
            void computeMain(
                uint3 sv_dispatchThreadID : SV_DispatchThreadID,
                uniform RWStructuredBuffer <float> buffer)
            {
                buffer[sv_dispatchThreadID.x] = helperFunc() + helperFunc1();
            }
        )";
    memoryFileSystem->saveFile("precompiled-module.slang", moduleSrc, strlen(moduleSrc));
    printf("File, Line: %s, %d\n", __FILE__, __LINE__);
    fflush(stdout);
    fflush(stderr);
    GFX_CHECK_CALL_ABORT(loadComputeProgram(
        device,
        slangSession,
        shaderProgram,
        "precompiled-module",
        "computeMain",
        slangReflection));
    printf("File, Line: %s, %d\n", __FILE__, __LINE__);
    ComputePipelineStateDesc pipelineDesc = {};
    pipelineDesc.program = shaderProgram.get();
    ComPtr<gfx::IPipelineState> pipelineState;
    printf("File, Line: %s, %d\n", __FILE__, __LINE__);
    fflush(stdout);
    fflush(stderr);
    GFX_CHECK_CALL_ABORT(
        device->createComputePipelineState(pipelineDesc, pipelineState.writeRef()));
    printf("File, Line: %s, %d\n", __FILE__, __LINE__);
    fflush(stdout);
    fflush(stderr);
    
    const int numberCount = 4;
    float initialData[] = {0.0f, 0.0f, 0.0f, 0.0f};
    IBufferResource::Desc bufferDesc = {};
    bufferDesc.sizeInBytes = numberCount * sizeof(float);
    bufferDesc.format = gfx::Format::Unknown;
    bufferDesc.elementSize = sizeof(float);
    bufferDesc.allowedStates = ResourceStateSet(
        ResourceState::ShaderResource,
        ResourceState::UnorderedAccess,
        ResourceState::CopyDestination,
        ResourceState::CopySource);
    bufferDesc.defaultState = ResourceState::UnorderedAccess;
    bufferDesc.memoryType = MemoryType::DeviceLocal;

    ComPtr<IBufferResource> numbersBuffer;
    printf("File, Line: %s, %d\n", __FILE__, __LINE__);
    fflush(stdout);
    fflush(stderr);
    
    GFX_CHECK_CALL_ABORT(
        device->createBufferResource(bufferDesc, (void*)initialData, numbersBuffer.writeRef()));
    printf("File, Line: %s, %d\n", __FILE__, __LINE__);
    fflush(stdout);
    fflush(stderr);
    
    ComPtr<IResourceView> bufferView;
    IResourceView::Desc viewDesc = {};
    viewDesc.type = IResourceView::Type::UnorderedAccess;
    viewDesc.format = Format::Unknown;
    GFX_CHECK_CALL_ABORT(
        device->createBufferView(numbersBuffer, nullptr, viewDesc, bufferView.writeRef()));
    printf("File, Line: %s, %d\n", __FILE__, __LINE__);
    fflush(stdout);
    fflush(stderr);
    
    // We have done all the set up work, now it is time to start recording a command buffer for
    // GPU execution.
    {
        ICommandQueue::Desc queueDesc = {ICommandQueue::QueueType::Graphics};
        auto queue = device->createCommandQueue(queueDesc);

        auto commandBuffer = transientHeap->createCommandBuffer();
        auto encoder = commandBuffer->encodeComputeCommands();

        auto rootObject = encoder->bindPipeline(pipelineState);

        ShaderCursor entryPointCursor(
            rootObject->getEntryPoint(0)); // get a cursor the the first entry-point.
        // Bind buffer view to the entry point.
        entryPointCursor.getPath("buffer").setResource(bufferView);

        encoder->dispatchCompute(1, 1, 1);
        encoder->endEncoding();
        commandBuffer->close();
        queue->executeCommandBuffer(commandBuffer);
        queue->waitOnHost();
    }
    printf("File, Line: %s, %d\n", __FILE__, __LINE__);
    fflush(stdout);
    fflush(stderr);
    
    compareComputeResult(device, numbersBuffer, Slang::makeArray<float>(3.0f, 3.0f, 3.0f, 3.0f));
    printf("File, Line: %s, %d\n", __FILE__, __LINE__);
    fflush(stdout);
    fflush(stderr);
}

void precompiledModule2TestImpl(IDevice* device, UnitTestContext* context)
{
    printf("File, Line: %s, %d\n", __FILE__, __LINE__);
    fflush(stdout);
    precompiledModule2TestImplCommon(device, context, false);
    printf("File, Line: %s, %d\n", __FILE__, __LINE__);
    fflush(stdout);
}

void precompiledTargetModule2TestImpl(IDevice* device, UnitTestContext* context)
{
    printf("File, Line: %s, %d\n", __FILE__, __LINE__);
    fflush(stdout);
    precompiledModule2TestImplCommon(device, context, false);
    printf("File, Line: %s, %d\n", __FILE__, __LINE__);
    fflush(stdout);
}

SLANG_UNIT_TEST(precompiledModule2D3D12)
{
    runTestImpl(precompiledModule2TestImpl, unitTestContext, Slang::RenderApiFlag::D3D12);
}

SLANG_UNIT_TEST(precompiledTargetModule2D3D12)
{
    printf("Starting precompiledTargetModule2D3D12\n");
    runTestImpl(precompiledTargetModule2TestImpl, unitTestContext, Slang::RenderApiFlag::D3D12);
}

SLANG_UNIT_TEST(precompiledModule2Vulkan)
{
    runTestImpl(precompiledModule2TestImpl, unitTestContext, Slang::RenderApiFlag::Vulkan);
}

SLANG_UNIT_TEST(precompiledTargetModule2Vulkan)
{
    printf("Starting precompiledTargetModule2Vulkan\n");
    fflush(stdout);
    runTestImpl(precompiledTargetModule2TestImpl, unitTestContext, Slang::RenderApiFlag::Vulkan);
    printf("Finished precompiledTargetModule2Vulkan\n");
    fflush(stdout);
}

} // namespace gfx_test
