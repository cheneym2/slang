//TEST(windows):COMPILE: tests/library/module-library-empty.slang -o module-library.slang-module
//TEST(windows):COMPILE: tests/library/module-library-empty.slang -o module-library.slang-module -profile lib_6_6 -embed-dxil

// module-library-empty.slang

module "module-library-empty";

public cbuffer SceneConstants {};
