//TEST(windows):COMPILE: tests/library/module-library2.slang -o module-library2.slang-module -profile lib_6_6 -embed-dxil

// module-library2.slang

module "module-library2";

public void add_flag(inout uint flags, uint to_add)
{
    flags |= to_add;
}

public void toggle_flag(inout uint flags, uint to_toggle)
{
    flags ^= to_toggle;
}

public void remove_flag(inout uint flags, uint to_remove)
{
    flags &= ~to_remove;
}

public bool has_flag(int flags, uint to_check)
{
    return (flags & to_check) != 0;
}

// Helper to make NaN and INF values visible in the output image.
public float3 encode_errors(float3 color)
{
    return any(isnan(color) | isinf(color)) ? float3(0.0f, 0.0f, 1.0e+30f) : color;
}

// convert float4x3 to 4x4, to be compatible with the slang compiler
public float4x4 to4x4(float3x4 source)
{
    return float4x4(source[0], source[1], source[2], float4(0.0f, 0.0f, 0.0f, 1.0f));
}

public float3 offset_ray(const float3 p, const float3 n)
{
    const float origin = 1.0f / 32.0f;
    const float float_scale = 1.0f / 65536.0f;
    const float int_scale = 256.0f;

    const int3 of_i = int3(int_scale * n);

    float3 p_i = float3(asfloat(asint(p.x) + ((p.x < 0.0f) ? -of_i.x : of_i.x)),
                        asfloat(asint(p.y) + ((p.y < 0.0f) ? -of_i.y : of_i.y)),
                        asfloat(asint(p.z) + ((p.z < 0.0f) ? -of_i.z : of_i.z)));

    return float3(abs(p.x) < origin ? p.x + float_scale * n.x : p_i.x,
                  abs(p.y) < origin ? p.y + float_scale * n.y : p_i.y,
                  abs(p.z) < origin ? p.z + float_scale * n.z : p_i.z);
}
