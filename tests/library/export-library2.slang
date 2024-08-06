//TEST_IGNORE_FILE:

// export-library2.slang

cbuffer Constants {
    float x;
    float y;
}

int doThing(int b)
{
    return b * 3;
}


int foo(int a)
{
    return a * a + 1 + doThing(a + 2);
}
