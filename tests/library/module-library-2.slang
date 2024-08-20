module common;

public struct ShadowHitInfo
{
    public bool isHit;
    public uint seed;
};

public struct Attributes
{
    public float2 bary;
};

// Generate random uint in [0, 2^24)
uint lcg(inout uint prev)
{
    const uint LCG_A = 1664525u;
    const uint LCG_C = 1013904223u;
    prev = (LCG_A * prev + LCG_C);
    return prev & 0x00FFFFFF;
}

// Generate random float in [0, 1)
public float rnd(inout uint prev)
{
    return ((float) lcg(prev) / (float) 0x01000000);
}
