//TEST:SIMPLE(filecheck=CHECK): -target glsl -entry computeMain -stage compute -profile sm_5_0
//TEST:SIMPLE(filecheck=CHECK_IGNORE_CAPS): -target glsl -emit-spirv-directly -entry computeMain -stage compute  -profile sm_5_0 -ignore-capabilities


// CHECK_IGNORE_CAPS-NOT: error 36107

// CHECK: error 36107
// CHECK-SAME: entrypoint 'computeMain' does not support compilation target 'glsl' with stage 'compute'
// CHECK: capabilitySimplification1.slang(21): note: see using of 'WaveMultiPrefixProduct'
// CHECK-NOT: see using of 'WaveMultiPrefixProduct'
// CHECK: {{.*}}.meta.slang({{.*}}): note: see definition of 'WaveMultiPrefixProduct'
// CHECK: {{.*}}.meta.slang({{.*}}): note: see declaration of 'require'

void nestedSafeCall()
{
    AllMemoryBarrier();
}

void nestedBadCall()
{
    WaveMultiPrefixProduct(1, 0);
}

void nestedCall()
{
    nestedSafeCall();
    nestedBadCall();
}

[numthreads(1,1,1)]
void computeMain()
{
    nestedCall();
}
