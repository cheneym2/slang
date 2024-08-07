// precompiled-dxil2.slang

// A test that uses slang-modules with embedded precompiled DXIL.
// The test compiles both library slang (export-library.slang) and entrypoint slang (this file) to two slang-modules, each with -embed-dxil options.
// The result is two slang-modules that are then linked together in a third slangc invocation.
// Internally, slang does not use the IR in the modules to link, but rather their embedded DXIL.
// The test passes if there is no errror thrown.
// TODO: Check if final linkage used only the precompiled dxil.

//TEST(windows):COMPILE: tests/library/export-library2.slang -o tests/library/export-library2.slang-module -embed-dxil -profile lib_6_6 -incomplete-library
//TEST(windows):COMPILE: tests/library/precompiled-dxil2.slang -target dxil -stage anyhit -entry anyhit -o tests/library/linked.dxil

import "export-library2";

struct Payload
{
    int val;
}

struct Attributes
{
    float2 bary;
}

[shader("anyhit")]
void anyhit(inout Payload payload, Attributes attrib)
{
    payload.val = normalFunc(x * y) + normalFuncUsesGeneric(y);
}
