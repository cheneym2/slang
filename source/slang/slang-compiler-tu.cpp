// slang-compiler-tu.cpp: Compiles translation units to target language
// and emit precompiled blobs into IR

#include "../core/slang-basic.h"
#include "slang-compiler.h"
#include "slang-ir-insts.h"
#include "slang-ir-util.h"
#include "slang-capability.h"

#define DEBUG_PRECOMPILE_DXIL 0

namespace Slang
{
    SLANG_NO_THROW SlangResult SLANG_MCALL Module::precompileForTarget(
        SlangCompileTarget target,
        slang::IBlob** outDiagnostics)
    {
        if (target != SLANG_DXIL)
        {
            return SLANG_FAIL;
        }
        CodeGenTarget targetEnum = CodeGenTarget(target);

        auto module = getIRModule();
        auto linkage = getLinkage();
        auto builder = IRBuilder(module);

        DiagnosticSink sink(linkage->getSourceManager(), Lexer::sourceLocationLexer);
        applySettingsToDiagnosticSink(&sink, &sink, linkage->m_optionSet);
        applySettingsToDiagnosticSink(&sink, &sink, m_optionSet);

        TargetRequest* targetReq = new TargetRequest(linkage, targetEnum);

        List<RefPtr<ComponentType>> allComponentTypes;
        allComponentTypes.add(this); // Add Module as a component type

        for (auto entryPoint : this->getEntryPoints())
        {
            allComponentTypes.add(entryPoint); // Add the entry point as a component type
        }

        auto composite = CompositeComponentType::create(
            linkage,
            allComponentTypes);

        TargetProgram tp(composite, targetReq);
        tp.getOrCreateLayout(&sink);
        Slang::Index const entryPointCount = m_entryPoints.getCount();
        tp.getOptionSet().add(CompilerOptionName::GenerateWholeProgram, true);

        switch (targetReq->getTarget())
        {
        case CodeGenTarget::DXIL:
            tp.getOptionSet().add(CompilerOptionName::Profile, Profile::RawEnum::DX_Lib_6_6);
            break;
        }

        CodeGenContext::EntryPointIndices entryPointIndices;

        entryPointIndices.setCount(entryPointCount);
        for (Index i = 0; i < entryPointCount; i++)
            entryPointIndices[i] = i;
        CodeGenContext::Shared sharedCodeGenContext(&tp, entryPointIndices, &sink, nullptr);
        CodeGenContext codeGenContext(&sharedCodeGenContext);

        // Mark all public functions as exported, ensure there's at least one
        bool hasAtLeastOneFunction = false;

        for (auto inst : module->getGlobalInsts())
        {            
            if (inst->getOp() == kIROp_Func)
            {
#if DEBUG_PRECOMPILE_DXIL
                auto name = inst->findDecoration<IRNameHintDecoration>();
#endif

                bool hasResourceType = false;

                // DXIL does not permit HLSLStructureBufferType in exported functions
                auto type = as<IRFuncType>(inst->getFullType());
                auto argCount = type->getOperandCount();
                for (UInt aa = 0; aa < argCount; ++aa)
                {
                    auto operand = type->getOperand(aa);
                    if (operand->getOp() == kIROp_HLSLStructuredBufferType || operand->getOp() == kIROp_MatrixType)
                    {
#if DEBUG_PRECOMPILE_DXIL
                        printf("Skipping function with resource or matrix type:\n");
                        name->dump();
#endif
                        hasResourceType = true;
                        break;
                    }
                }

                if (!hasResourceType)
                {
                    if (isSimpleHLSLDataType(inst))
                    {
#if DEBUG_PRECOMPILE_DXIL
                        if (inst->findDecoration<IRUnsafeForceInlineEarlyDecoration>())
                        {                            
                            printf("Skipping function with unsafe force inline early:\n");
                            name->dump();
                        }

#endif

                        if (!inst->findDecoration<IRUnsafeForceInlineEarlyDecoration>())
                        {
                            bool hasBody = false;
                            for (auto child : inst->getChildren())
                            {
                                if (child->getOp() == kIROp_Block)
                                {
                                    hasBody = true;
                                    break;
                                }
                            }
                            if (hasBody)
                            {
#if DEBUG_PRECOMPILE_DXIL
                                printf("Exporting function:\n");
                                name->dump();
                                if (inst->findDecoration<IRFromStdLibDecoration>())
                                {
                                    printf("!!!!!!!!!!!!!!!! Unexpectedly exporting function from stdlib:\n");
                                    name->dump();
                                }                            
#endif

                                // add HLSL export decoration to inst to preserve it in precompilation
                                hasAtLeastOneFunction = true;
                                builder.addDecorationIfNotExist(inst, kIROp_HLSLExportDecoration);                             
                            }
#if DEBUG_PRECOMPILE_DXIL
                            else
                            {
                                printf("Skipping function without body:\n");
                                name->dump();
                            }
#endif
                        }
                    }
                }
            }
        }

        // Avoid emitting precompiled blob if there are no functions to export
        if (hasAtLeastOneFunction)
        {
            ComPtr<IArtifact> outArtifact;
            SlangResult res = codeGenContext.emitTranslationUnit(outArtifact);
            sink.getBlobIfNeeded(outDiagnostics);
            if (res != SLANG_OK)
            {
                return res;
            }

            // Mark all exported functions as available in dxil
            for (auto inst : module->getGlobalInsts())
            {
                if (inst->getOp() == kIROp_Func)
                {
                    // Add available in dxil decoration to function if it was exported
                    if (inst->findDecoration<IRHLSLExportDecoration>() != nullptr)
                    {
                        builder.addDecorationIfNotExist(inst, kIROp_AvailableInDXILDecoration);
                    }
                }
            }

            ISlangBlob* blob;
            outArtifact->loadBlob(ArtifactKeep::Yes, &blob);

            // Add the precompiled blob to the module
            builder.setInsertInto(module);

            switch (targetReq->getTarget())
            {
            case CodeGenTarget::DXIL:
                builder.emitEmbeddedDXIL(blob);
                /*
                printf("Embeddedd DXIL blob\n");
                FILE* f = fopen("embedded_dxil.bin", "wb");
                if (!f)
                {
                    printf("Failed to write embedded DXIL blob to embedded_dxil.bin\n");
                }
                fwrite(blob->getBufferPointer(), 1, blob->getBufferSize(), f);
                fclose(f);
                printf("Wrote embedded DXIL blob to embedded_dxil.bin\n");
                */
                break;
            }
        }

        return SLANG_OK;
    }
}
