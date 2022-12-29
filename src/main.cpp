#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "jsvm/jsvm.h"
#include <d3d11.h>
#include <tchar.h>
#include <algorithm>
#include <stdint.h>
#include <vector>
#include "stack_trace.hpp"
#include "logger.hpp"

struct App {
  ID3D11Device *device = nullptr;
  ID3D11DeviceContext *context = nullptr;
  IDXGISwapChain *swapChain = nullptr;
  ID3D11RenderTargetView *targetView = nullptr;

  void create_render_target()
  {
    ID3D11Texture2D* pBackBuffer;
    swapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    device->CreateRenderTargetView(pBackBuffer, NULL, &targetView);
    pBackBuffer->Release();
  }

  // Forward declarations of helper functions
  bool create_device(HWND hWnd)
  {
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &swapChain, &device, &featureLevel, &context) != S_OK)
      return false;

    create_render_target();
    return true;
  }

  void cleanup_render_target()
  {
    if (targetView) {
      targetView->Release();
      targetView = NULL;
    }
  }

  void cleanup_device()
  {
    cleanup_render_target();
    if (swapChain) { swapChain->Release(); swapChain = NULL; }
    if (context) { context->Release(); context = NULL; }
    if (device) { device->Release(); device = NULL; }
  }
};

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
struct Window {
  App &app;
  HWND hwnd = nullptr;
  bool device_ok = false;
  WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("wc.sway"), NULL };

  struct ACCENTPOLICY
  {
    int nAccentState;
    int nFlags;
    int nColor;
    int nAnimationId;
  };
  struct WINCOMPATTRDATA
  {
    int nAttribute;
    PVOID pData;
    ULONG ulDataSize;
  };

  typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINCOMPATTRDATA*);
  const HINSTANCE hModule_User32 = LoadLibraryA("user32.dll");
  const pSetWindowCompositionAttribute SetWindowCompositionAttribute = (pSetWindowCompositionAttribute)GetProcAddress(hModule_User32, "SetWindowCompositionAttribute");

  void StartAero(HWND hwnd, int type, COLORREF color, bool blend)
  {
    if (SetWindowCompositionAttribute)
    {
      ACCENTPOLICY policy = { type == 0 ? 3 : 4, 0, 0, 0 };
      if (blend)
      {
        policy.nFlags = 3;
        policy.nColor = color;
      }
      else if (type == 1)
      {
        policy.nFlags = 1;
        policy.nColor = 0x10000000;
      }
      else
      {
        policy.nFlags = 0;
        policy.nColor = 0;
      }
      WINCOMPATTRDATA data = { 19, &policy, sizeof(ACCENTPOLICY) };
      SetWindowCompositionAttribute(hwnd, &data);
    }
  }

  void SetWindowBlur(HWND hWnd)
  {
    bool isBlend = false;
    StartAero(hWnd, 1, 0, isBlend);
  }

  void cleanup() {
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);
  }

  void destroy() {
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);
  }

  void create_imgui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(app.device, app.context);
  }

  void frame_begin() {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
  }

  void frame_end() {
    ImGui::Render();

    ImVec4 clear_color = ImVec4(0.15f, 0.55f, 0.60f, 0.00f);
    const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
    app.context->OMSetRenderTargets(1, &app.targetView, NULL);
    app.context->ClearRenderTargetView(app.targetView, clear_color_with_alpha);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    app.swapChain->Present(1, 0); // Present with vsync
  }

  Window(App &a) : app(a) {
    ::RegisterClassEx(&wc);
    hwnd = ::CreateWindow(wc.lpszClassName, _T("sway"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    LONG lStyle = GetWindowLong(hwnd, GWL_STYLE);
    lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU );
    SetWindowLong(hwnd, GWL_STYLE, lStyle);

    SetWindowBlur(hwnd);

    RECT screenRect;
    BOOL fResult = SystemParametersInfo(SPI_GETWORKAREA, 0, &screenRect, 0);

    int xPos = GetSystemMetrics(SM_CXSCREEN);
    int yPos = screenRect.bottom - 60;

    SetWindowPos(hwnd, HWND_TOPMOST, screenRect.left , yPos, (screenRect.right - screenRect.left), 60, 0);

    if (!app.create_device(hwnd)) {
      app.cleanup_device();
      cleanup();
      device_ok = false;
    }

    device_ok = true;
    create_imgui();
  }
};

struct FRAMInfo {
  uint64_t UsagePercentage;
  uint64_t TotalPhysicalMemory;
  uint64_t FreePhysicalMemory;
  uint64_t TotalPageFileSize;
  uint64_t FreePageFileSize;
  uint64_t TotalVirtualMemory;
  uint64_t FreeVirtualMemory;
  uint64_t ExtendedMemory;
};

FRAMInfo GetRAMInfo()
{
  MEMORYSTATUSEX statex;
  statex.dwLength = sizeof(statex);
  GlobalMemoryStatusEx(&statex);

  FRAMInfo i;
  i.UsagePercentage = (uint64_t)statex.dwMemoryLoad;
  i.TotalPhysicalMemory = (uint64_t)statex.ullTotalPhys;
  i.FreePhysicalMemory = (uint64_t)statex.ullAvailPhys;
  i.TotalPageFileSize = (uint64_t)statex.ullTotalPageFile;
  i.FreePageFileSize = (uint64_t)statex.ullAvailPageFile;
  i.TotalVirtualMemory = (uint64_t)statex.ullTotalVirtual;
  i.FreeVirtualMemory = (uint64_t)statex.ullAvailVirtual;
  i.ExtendedMemory = (uint64_t)statex.ullAvailExtendedVirtual;
  return i;
}

App app;
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    crashhandler::install();
    Logger::registerWriter(Logger::consolelog, "");
    Window window(app);

    if (window.device_ok)
        return 1;

    js_vm_setup();

    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
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

        // Start the Dear ImGui frame
        window.frame_begin();

        auto memload = [] (void *, int i) {
          static std::vector<float> data;
          static float last_info_update = 0;
          if (data.size() < i+1) data.resize(i+1, 0.f);
          if (last_info_update + 0.5f < ImGui::GetTime()) {
            auto raminfo = GetRAMInfo();
            data.erase(data.begin());
            data.push_back(raminfo.UsagePercentage / 100.f);
            last_info_update = ImGui::GetTime();
          }
          return data[i];
        };
        static int display_count = 70;
        bool p_open = true;

        ImGui::SetNextWindowPos(ImVec2{0, 0});
        ImGui::SetNextWindowSize(ImVec2{140, 60});
        ImGui::Begin("#a", &p_open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
        ImGui::PlotHistogram("", memload, NULL, display_count, 0, NULL, -1.0f, 1.0f, ImVec2(140, 100));
        ImGui::PopStyleColor(1);
        ImGui::SetCursorPos({10, 5});
        ImGui::Text("RAM");
        auto raminfo = GetRAMInfo();
        ImGui::SetCursorPos({10, 20});
        const float dwMBFactor = (float)(1024.f * 1024.f * 1024.f);
        ImGui::Text("%0.1f/%0.1f Gb (%02u%%)", (raminfo.UsagePercentage / 100.f) * (raminfo.TotalPhysicalMemory / dwMBFactor), (raminfo.TotalPhysicalMemory / dwMBFactor), raminfo.UsagePercentage);
        ImGui::End();

        window.frame_end();
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    app.cleanup_device();
    window.destroy();

    return 0;
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg) {
    case WM_SIZE:
        if (app.device != NULL && wParam != SIZE_MINIMIZED) {
            app.cleanup_render_target();
            app.swapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            app.create_render_target();
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
