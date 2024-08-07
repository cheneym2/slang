//TEST(windows):COMPILE: tests/library/module-library.slang -o module-library.slang-module
//TEST(windows):COMPILE: tests/library/module-library.slang -o module-library.slang-module -profile lib_6_6 -embed-dxil

// module-library.slang

module "module-library";

public float3 encode_errors(float3 color)
{
    return any(isinf(color) | isnan(color)) ? float3(0.0f, 0.0f, 1.0e+30f) : color;
}
