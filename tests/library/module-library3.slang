//TEST(windows):COMPILE: tests/library/module-library3.slang -o module-library3.slang-module -profile lib_6_6 -embed-dxil -dump-ir

// module-library3.slang

module "module-library3";

int resource_in_parameter(StructuredBuffer<int> buffer)
{
    return buffer[0];
}
