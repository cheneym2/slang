// precompiled-dxil-2.slang

//TEST(windows):COMPILE: tests/library/module-library-2.slang -o tests/library/module-library-2.slang-module -embed-dxil -profile lib_6_6 -incomplete-library
//TEST(windows):COMPILE: tests/library/precompiled-dxil-2.slang -stage anyhit -entry shadow -target dxil -o dxr-sh-modules.dxil

import "module-library-2";

[shader("anyhit")]
void shadow(inout ShadowHitInfo payload, Attributes attrib)
{
    if (rnd(payload.seed) > 0.5)
    {
        payload.isHit = true;
        AcceptHitAndEndSearch();
        return;
    }

    IgnoreHit();
}
