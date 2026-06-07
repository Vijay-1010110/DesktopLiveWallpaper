#include <windows.h>
#include <iostream>
int main() {
    HWND progman = FindWindowW(L"Progman", NULL);
    WNDCLASSEXW wcex = { sizeof(WNDCLASSEX) };
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = DefWindowProc;
    wcex.hInstance = GetModuleHandle(NULL);
    wcex.lpszClassName = L"WeatherLiveWallpaperClassTest";
    if (!RegisterClassExW(&wcex)) { std::cout << "No reg\n"; return 1; }
    
    HWND hwnd = CreateWindowExW(0, L"WeatherLiveWallpaperClassTest", L"Test", WS_CHILD | WS_VISIBLE, 0, 0, 100, 100, progman, NULL, GetModuleHandle(NULL), NULL);
    if (!hwnd) { std::cout << "No hwnd. Error: " << GetLastError() << "\n"; return 1; }
    std::cout << "Success\n";
    return 0;
}
