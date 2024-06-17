// precompiled-dxil-lib-test.slang

// A test to try out the basics of using libraries that can be linked (by downstream tools), via the HLSL `export` keyword.
// (This is in contrast to Slang modules and Slangs own linkage mechanisms)

// This didn't work for lib_6_2 when compiling via DXC (!). Even though it's stated elsewhere the feature is available from 6.1

//TEST:COMPILE: tests/library/export-library.slang -o tests/library/precompiled-dxil-library-nodxil.slang-module
//TEST:COMPILE: tests/library/export-library.slang -o tests/library/precompiled-dxil-library-dxil.slang-module -embed-dxil -profile lib_6_6

//TEST:COMPILE: tests/library/precompiled-dxil-lib-test.slang -o tests/library/precompiled-dxil-entry-test-nodxil.slang-module -incomplete-library
//TEST:COMPILE: tests/library/precompiled-dxil-lib-test.slang -o tests/library/precompiled-dxil-entry-test-dxil.slang-module -embed-dxil -profile lib_6_6 -incomplete-library

// Test the produced kernel.

// TODOEST:COMPARE_COMPUTE_EX:-slang -compute -profile cs_6_3 -dx12 -use-dxil -shaderobj -Xslang... -r tests/library/export-library.dxil -incomplete-library -X.

extern int foo(int a);

//TEST_INPUT:ubuffer(data=[0 0 0 0 ], stride=4):out,name outputBuffer
RWStructuredBuffer<int> outputBuffer;

[shader("compute")]
[numthreads(4, 1, 1)]
void computeMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    int index = (int)dispatchThreadID.x;
       
    outputBuffer[index] = foo(index);
}
