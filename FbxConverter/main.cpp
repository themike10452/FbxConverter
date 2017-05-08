#include "FbxConverter.h"

int main(int argc, char* argv[])
{
    if (argc < 3) return -1;

    auto converter = new FbxConverter();
    converter->Initialize();
    converter->LoadScene( argv[1] );
    converter->Export( argv[2] );
    
    delete converter;

    return 0;
}