#include "ui/imgui.h"
#include "ui/imgui_impl_win32.h"
#include "ui/imgui_impl_dx11.h"
#include "jsvm/jsvm.h"
#include <d3d11.h>
#include <tchar.h>
#include <algorithm>
#include <stdint.h>
#include <vector>
#include "stack_trace.hpp"
#include "logger.hpp"
#include "modules.hpp"

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

  void GetDisplayScale(double &h_Scale, double &v_Scale)
  {
      //auto activeWindow = GetActiveWindow();
      HWND activeWindow = GetDesktopWindow();
      HMONITOR monitor = MonitorFromWindow(activeWindow, MONITOR_DEFAULTTONEAREST);

      // Get the logical width and height of the monitor
      MONITORINFOEX monitorInfoEx;
      monitorInfoEx.cbSize = sizeof(monitorInfoEx);
      GetMonitorInfo(monitor, &monitorInfoEx);
      long cxLogical = monitorInfoEx.rcMonitor.right - monitorInfoEx.rcMonitor.left;
      long cyLogical = monitorInfoEx.rcMonitor.bottom - monitorInfoEx.rcMonitor.top;

      // Get the physical width and height of the monitor
      DEVMODE devMode;
      devMode.dmSize = sizeof(devMode);
      devMode.dmDriverExtra = 0;
      EnumDisplaySettings(monitorInfoEx.szDevice, ENUM_CURRENT_SETTINGS, &devMode);
      DWORD cxPhysical = devMode.dmPelsWidth;
      DWORD cyPhysical = devMode.dmPelsHeight;

      // Calculate the scaling factor
      h_Scale = ((double)cxPhysical / (double)cxLogical);
      v_Scale = ((double)cyPhysical / (double)cyLogical);

      // Round off to 2 decimal places
      h_Scale = round(h_Scale * 100.0) / 100.0;
      v_Scale = round(v_Scale * 100.0) / 100.0;

      //Logger::info( "Horizonzal scaling:{} Vertical scaling:{}", h_Scale, v_Scale);
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

    update_window_pos();

    if (!app.create_device(hwnd)) {
      app.cleanup_device();
      cleanup();
      device_ok = false;
    }

    device_ok = true;
    create_imgui();
  }

  void update_window_pos() {
      RECT screenRect;
      BOOL fResult = SystemParametersInfo(SPI_GETWORKAREA, 0, &screenRect, 0);

      int app_window_height = js_get_option("window_height");
      int app_window_width = js_get_option("window_width");
      int app_window_ypos = js_get_option("window_ypos");
      int app_window_xpos = js_get_option("window_xpos");
      int xscreen = GetSystemMetrics(SM_CXSCREEN);
      int xPos = app_window_xpos < 0 ? screenRect.left : app_window_xpos;
      int yPos = app_window_ypos < 0 ? (screenRect.bottom - app_window_height) : app_window_ypos;
      int width = app_window_width > 0 ? app_window_width : (screenRect.right - screenRect.left);

      SetWindowPos(hwnd, HWND_TOPMOST, xPos, yPos, width, app_window_height, 0);
      double ws, hs;
      GetDisplayScale(ws, hs);
      js_set_option("window_xscale", ws);
      js_set_option("window_yscale", ws);
  }
};

App app;
#ifdef WIN_SUBSYSTEM_WINDOWS
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
#else
int main(int argc, char **argv)
#endif
{
    crashhandler::install();
    Logger::registerWriter(Logger::consolelog, "");
    Logger::info("console logger registered");

    js_vm_setup();

    Window window(app);
    if (!window.device_ok)
        return 1;

    Logger::info("device ok");
    js_subscribe_native_callback(cb_on_change_scripts, [&] () { window.update_window_pos(); });

    Logger::info("start modules register");
    modules::register_modules(nullptr, 0);

    bool done = false;
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

        window.frame_begin();

        modules::process_modules(nullptr, 0);
        js_vm_resolve_frame_callbacks();

        window.frame_end();

        js_vm_sync((int)ImGui::GetTime() * 1000);
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
