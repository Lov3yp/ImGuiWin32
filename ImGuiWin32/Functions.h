#include <iostream>
#include <windows.h>
#include <d3d9.h>
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_dx9.h"
#include "Imgui/imgui_impl_win32.h"
#pragma comment(lib,"d3d9.lib")

namespace OverlayWindow {
    WNDCLASSEX WindowClass;
    HWND Hwnd;
    LPCSTR Name;
}

namespace DirectX9Interface {
    LPDIRECT3D9 Direct3D9 = NULL;
    LPDIRECT3DDEVICE9 pDevice = NULL;
    D3DPRESENT_PARAMETERS pParams = { NULL };
    MSG Message = { NULL };
}

bool CreateDeviceD3D(HWND hWnd) {
    if ((DirectX9Interface::Direct3D9 = Direct3DCreate9(D3D_SDK_VERSION)) == NULL) {
        return false;
    }
    ZeroMemory(&DirectX9Interface::pParams, sizeof(DirectX9Interface::pParams));
    DirectX9Interface::pParams.Windowed = TRUE;
    DirectX9Interface::pParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
    DirectX9Interface::pParams.BackBufferFormat = D3DFMT_UNKNOWN;
    DirectX9Interface::pParams.EnableAutoDepthStencil = TRUE;
    DirectX9Interface::pParams.AutoDepthStencilFormat = D3DFMT_D16;
    DirectX9Interface::pParams.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    if (DirectX9Interface::Direct3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &DirectX9Interface::pParams, &DirectX9Interface::pDevice) < 0) {
        return false;
    }
    return true;
}

void ClearD3D() {
    if (DirectX9Interface::pDevice) {
        DirectX9Interface::pDevice->Release();
        DirectX9Interface::pDevice = NULL;
    }

    if (DirectX9Interface::Direct3D9) {
        DirectX9Interface::Direct3D9->Release();
        DirectX9Interface::Direct3D9 = NULL;
    }
}

void ClearAll() {
    ClearD3D();
    UnregisterClass(OverlayWindow::WindowClass.lpszClassName, OverlayWindow::WindowClass.hInstance);
}

void ResetDevice() {
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = DirectX9Interface::pDevice->Reset(&DirectX9Interface::pParams);
    if (hr == D3DERR_INVALIDCALL) {
        IM_ASSERT(0);
    }
    ImGui_ImplDX9_CreateDeviceObjects();
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg) {
    case WM_SIZE:
        if (DirectX9Interface::pDevice != NULL && wParam != SIZE_MINIMIZED) {
            DirectX9Interface::pParams.BackBufferWidth = LOWORD(lParam);
            DirectX9Interface::pParams.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

std::string RandomString(int len) {
    srand(time(NULL));
    std::string str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string newstr;
    int pos;
    while (newstr.size() != len) {
        pos = ((rand() % (str.size() - 1)));
        newstr += str.substr(pos, 1);
    }
    return newstr;
}