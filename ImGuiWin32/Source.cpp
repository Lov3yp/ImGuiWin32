#include "Functions.h"

bool ShowMenu = true;
bool ImGui_Initialised = false;

void Render() {
    ImVec2 ScreenRes{ 0, 0 };
    ImVec2 WindowPos{ 0, 0 };
    ImVec2 WindowSize{ 600, 400 };

    if (ImGui_Initialised == false) {
        RECT ScreenRect;
        GetWindowRect(GetDesktopWindow(), &ScreenRect);
        ScreenRes = ImVec2(float(ScreenRect.right), float(ScreenRect.bottom));
        WindowPos.x = (ScreenRes.x - WindowSize.x) * 0.5f;
        WindowPos.y = (ScreenRes.y - WindowSize.y) * 0.5f;
        ImGui_Initialised = true;
    }

    ImGui::SetNextWindowPos(ImVec2(WindowPos.x, WindowPos.y), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(WindowSize.x, WindowSize.y));
    ImGui::Begin("Majdev", &ShowMenu, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);
    ImGui::End();
}

int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    OverlayWindow::Name = RandomString(10).c_str();
    OverlayWindow::WindowClass = {
        sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, OverlayWindow::Name, NULL
    };

    RegisterClassEx(&OverlayWindow::WindowClass);
    OverlayWindow::Hwnd = CreateWindow(OverlayWindow::Name, OverlayWindow::Name, WS_POPUP, 0, 0, 5, 5, NULL, NULL, OverlayWindow::WindowClass.hInstance, NULL);
    if (!CreateDeviceD3D(OverlayWindow::Hwnd)) {
        ClearAll();
        return 1;
    }

    ShowWindow(OverlayWindow::Hwnd, SW_HIDE);
    UpdateWindow(OverlayWindow::Hwnd);

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantTextInput || ImGui::GetIO().WantCaptureKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui_ImplWin32_Init(OverlayWindow::Hwnd);
    ImGui_ImplDX9_Init(DirectX9Interface::pDevice);

    ZeroMemory(&DirectX9Interface::Message, sizeof(DirectX9Interface::Message));
    while (DirectX9Interface::Message.message != WM_QUIT) {

        if (PeekMessage(&DirectX9Interface::Message, NULL, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&DirectX9Interface::Message);
            DispatchMessage(&DirectX9Interface::Message);
            continue;
        }

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        {
            Render();
        }
        ImGui::EndFrame();

        DirectX9Interface::pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
        if (DirectX9Interface::pDevice->BeginScene() >= 0) {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            DirectX9Interface::pDevice->EndScene();
        }

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        HRESULT result = DirectX9Interface::pDevice->Present(NULL, NULL, NULL, NULL);
        if (result == D3DERR_DEVICELOST && DirectX9Interface::pDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
            ResetDevice();
        }
        if (!ShowMenu) {
            DirectX9Interface::Message.message = WM_QUIT;
        }
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    DestroyWindow(OverlayWindow::Hwnd);
    ClearAll();
    return 0;
}