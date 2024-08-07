//TEST_IGNORE_FILE:

// export-library2.slang

cbuffer Constants {
    float x;
    float y;
}

interface MyInterface
{
    int myMethod(int a);
}

struct MyType : MyInterface
{
    int myMethod(int a)
    {
        return a * 3;
    }
}

int genericFunc<T: MyInterface>(T arg)
{
    return arg.myMethod(3);
}

int normalFuncUsesGeneric(int a)
{
    MyType obj;
    return genericFunc(obj);
}

int normalFunc(int a)
{
    return a - 2;
}
