#include <windows.h>
#include <d2d1.h>
#include <iostream>
#pragma comment(lib, "d2d1.lib")

int main() {
    ID2D1Factory* pFactory = nullptr;
    HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory);
    std::cout << "HRESULT: " << std::hex << hr << std::endl;
    return 0;
}
