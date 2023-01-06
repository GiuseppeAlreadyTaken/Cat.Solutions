#include <iostream>
#include <d3d9.h>
#include <tchar.h>
#include <D3dx9tex.h>
#include <format>

#pragma comment(lib, "D3dx9")

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include "includes.h"
#include "ui.h"
#include "fonts.h"

static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

ImFont* Inter;
ImFont* Inter14;

char Username[256]{};
char Password[256]{};
char Key[256]{};
char RegisterUsername[256]{};
char RegisterPassword[256]{};
static bool Dashboard = false;

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void SpotifyPlayer() {

    char Title[256];
    HWND Spotify = FindWindowA("Chrome_WidgetWin_0", NULL);
    GetWindowTextA(Spotify, Title, sizeof(Title));

    if (!IsWindowVisible(Spotify)) {

        ImGui::Text("Open Spotify First!");

    }
    else {

        std::string STitle = Title;

        if (STitle.find("Spotify")) {

            ImGui::Text((const char*)Title);

        }
        else {

            ImGui::Text("Paused");

        }

    }

}

void InputText(char* Var, const char* ItemName, const char* Hint, ImVec2 CursorPos, float Width, bool PasswordOrNot) {

    ImGui::PushFont(Inter);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 214 / 2 - ImGui::CalcTextSize(Var[0] == 0 ? Hint : Var).x / 2.f, 17.5f });

    ImGui::SetCursorPos(CursorPos);
    ImGui::SetNextItemWidth(Width);
    ImGui::InputTextWithHint(ItemName, Hint, Var, 256, PasswordOrNot == true ? ImGuiInputTextFlags_Password : ImGuiInputTextFlags_None | ImGuiInputTextFlags_CharsNoBlank );

    if (ImGui::IsItemFocused()) {

        if ((ImGui::IsKeyPressed(ImGuiKey_LeftCtrl)) && (ImGui::IsKeyPressed(ImGuiKey_V))) {

            std::strncpy(Var, ImGui::GetClipboardText(), 256);

        }

    }

    ImGui::PopStyleVar();
    ImGui::PopFont();

}

void Chat() {

    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.f, 0.f, 0.f, 0.f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(35.f / 255.f, 35.f / 255.f, 35.f / 255.f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 15 , 15 });
    ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 0);

    ImGui::SetCursorPos({ 345 , 15 });
    ImGui::BeginListBox("##Chat", { 280 , 298 });


    for (int i = 0; i < KeyAuthApp.data.channeldata.size(); i++)
    {

        std::string message = KeyAuthApp.data.channeldata[i].timestamp + " " + KeyAuthApp.data.channeldata[i].author + " : " + KeyAuthApp.data.channeldata[i].message + "\n";

        ImGui::PushFont(Inter14);

        ImGui::Text(message.c_str());

        ImGui::PopFont();

    }

    ImGui::EndListBox();

    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(3);

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 15 , 12.6f });
    ImGui::PushFont(Inter14);

    static char input[256] = "";
    ImGui::SetCursorPos({ 345 , 315 });
    ImGui::SetNextItemWidth(185);
    ImGui::InputText("##Message", input, 256, ImGuiInputTextFlags_EnterReturnsTrue);

    ImGui::PopFont();

    ImGui::PopStyleVar();

    ImGui::SetCursorPos({ 530 , 315 });
    if (ImGui::Button("Send", ImVec2(95, 40)))
    {
        if ((std::string)input == "!clear") {

            KeyAuthApp.webhook("BfuhVa4lA8", "General");

            KeyAuthApp.chatsend("Cleared Chat", "General");

        }

        KeyAuthApp.chatsend(input, "General");

        KeyAuthApp.data.channeldata.clear();
        KeyAuthApp.chatget("General");

    }

}

void MainPage() {

    InputText(Username, "##Username", "Username", { 60 , 117 }, 214.f, false);
    InputText(Password, "##Password", "Password", { 60 , 190 }, 214.f, true);

    ImGui::PushFont(Inter);

    ImGui::SetCursorPos({ 60, 263 });
    if (ImGui::Button("Login", { 214 , 50 })) {

        KeyAuthApp.login(Username, Password);

        if (!KeyAuthApp.data.success) {

            Utils::Log(KeyAuthApp.data.message.c_str(), 2);
            return;

        }

        Dashboard = true;

    }

    ImGui::PopFont();

    InputText(RegisterUsername, "##RegisterUsername", "Username", { 390 , 80 }, 214, false);
    InputText(RegisterPassword, "##RegisterPassword", "Password", { 390 , 153 }, 214, false);
    InputText(Key, "##Key", "Key", { 390 , 226 }, 214, false);

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(180.f / 255.f, 50.f / 255.f, 50.f / 255.f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(200.f / 255.f, 70.f / 255.f, 70.f / 255.f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(230.f / 255.f, 100.f / 255.f, 100.f / 255.f, 1.f));
    ImGui::PushFont(Inter);

    ImGui::SetCursorPos({ 390, 299 });
    if (ImGui::Button("Register", { 214 , 50 })) {

        KeyAuthApp.regstr(RegisterUsername, RegisterPassword, Key);

        if (!KeyAuthApp.data.success) {

            Utils::Log(KeyAuthApp.data.message.c_str(), KeyAuthApp.data.success == false ? 2 : 1);
            return;

        }

    }

    ImGui::PopFont();
    ImGui::PopStyleColor(3);

}

void DashboardPage() {

    static float SizeY = 0;
    static int Speed = 10;

    if (!(SizeY >= 370)) {

        SizeY += Speed;

    }

    ImGui::SetCursorPos({ 0 , 30 });
    ImGui::BeginChild("Dashboard", { 650 , SizeY });

    ImGui::SetCursorPos({ 60 , 175 - 30 });
    ImGui::Button("Load", { 214 , 50 });

    Chat();

    ImGui::EndChild();

}

void Header() {

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(25.f / 255.f, 25.f / 255.f, 25.f / 255.f, 1.0f));

    ImGui::BeginChild("Login Header", { 650 , 30 }, false);

    ImGui::PushFont(Inter);

    ImGui::SetCursorPos({ 18 , 8 });
    ImGui::TextColored(ImVec4(60.f / 255.f, 100.f / 255.f, 150.f / 255.f, 1.f), "Cat");
    ImGui::SameLine(40.f);
    ImGui::TextColored(ImVec4(100.f / 255.f, 100.f / 255.f, 100.f / 255.f, 1.f), "Solutions - Made by Giuseppe#5102 with love :3");

    ImGui::PopFont();

    ImGui::EndChild();

    ImGui::PopStyleColor();


}

void ui::Init()
{

    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"ImGui Example", NULL };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui DirectX9 Example", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
    }

    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO&    io    = ImGui::GetIO(); (void)io;
    ImGuiStyle* style = &ImGui::GetStyle();

    ImGui::StyleColorsDark();

    style->WindowRounding              = 5;
    style->FrameRounding               = 5;
    style->WindowBorderSize            = 1;
    style->WindowPadding               = { 0 , 0 };
    style->Colors[ ImGuiCol_Border   ] = ImVec4(0.f, 0.f, 0.f, 178.5f / 255.f);
    style->Colors[ ImGuiCol_WindowBg ] = ImVec4(20.f / 255.f, 20.f / 255.f, 20.f / 255.f, 1.0f);
    style->Colors[ ImGuiCol_FrameBg  ] = ImVec4(25.f / 255.f, 25.f / 255.f, 25.f / 255.f, 1.0f);
    style->Colors[ ImGuiCol_Button   ] = ImVec4(50.f / 255.f, 110.f / 255.f, 180.f / 255.f, 1.f);

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    
    io.Fonts->AddFontDefault();
    Inter = io.Fonts->AddFontFromMemoryTTF(InterSemiBold, sizeof(InterSemiBold), 16);
    Inter14 = io.Fonts->AddFontFromMemoryTTF(InterSemiBold, sizeof(InterSemiBold), 14);

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    bool done = false;
    initialized = true;
    while (!done)
    {

        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        {
            //ImGui::SetNextWindowPos({ 5 , 5 });
            ImGui::SetNextWindowSize({ 650 , 400 });

            ImGui::Begin("Main", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);

            Header();

            if (Dashboard == false)
                MainPage();

            if (Dashboard)
                DashboardPage();

            ImGui::End();

            ImGui::PopStyleVar(4);
            ImGui::PopStyleColor(3);
        }

        //if (show_another_window)
        {
            static float alpha = 0.f;
            static float speed = 15.0f;

            if (!(alpha >= 255.f)) {

                alpha += speed;

            }

            ImGui::SetNextWindowPos({ 5 , 5 });
            ImGui::SetNextWindowSize({ 450 , 150 });

            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0 , 0 });
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.f, 0.f, 0.f, 178.5f / 255.f));
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(20.f / 255.f, 20.f / 255.f, 20.f / 255.f, alpha / 255.f));
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(25.f / 255.f, 25.f / 255.f, 25.f / 255.f, 1.0f));

            ImGui::Begin("Another Window", &show_another_window, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

            ImGui::BeginChild("Player Header", { 450 , 30 }, false);

            ImGui::PushFont(Inter);
            ImGui::SetCursorPos({ 18 , 8 });
            ImGui::TextColored(ImVec4(82.f / 255.f, 152.f / 255.f, 60.f / 255.f, 1.f), "Spotify");
            ImGui::SameLine(18.f + ImGui::CalcTextSize("Spotify").x);
            ImGui::TextColored(ImVec4(100.f / 255.f, 100.f / 255.f, 100.f / 255.f, 1.f), "Player");

            ImGui::PopFont();

            ImGui::EndChild();

            ImGui::SetCursorPos({ 80, 80 });
            ImGui::PushFont(Inter);
            SpotifyPlayer();
            ImGui::PopFont();

            ImGui::End();

            ImGui::PopStyleVar(3);
            ImGui::PopStyleColor(3);
        }

        // Rendering
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}