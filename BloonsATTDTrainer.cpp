//#include <iostream>
#include <vector>
#include <string>
#include <Windows.h>
#include <TlHelp32.h>
#include <cstdio>
#include <thread>
#include <chrono>

#include "proc.h"
#include "offsets.h"

#include "vendor/ImGui/imgui.h"
#include "vendor/ImGui/backends/imgui_impl_dx9.h"
#include "vendor/ImGui/backends/imgui_impl_win32.h"
#include <d3d9.h>
#include <tchar.h>

// Data
static LPDIRECT3D9              g_pD3D = nullptr;
static LPDIRECT3DDEVICE9        g_pd3dDevice = nullptr;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int main()
{
  FreeConsole();
  //get ProcId of the target process
  DWORD procId = GetProcId(L"btdadventuretime.exe");
  
  //GameAssembly base
  uintptr_t gaBase = GetModuleBaseAddress(procId, L"GameAssembly.dll");
  
  
  //get handle to process
  HANDLE hProc = 0;
  hProc = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

  Offsets offsets(hProc, gaBase);
 
  //ImGui stuff
  WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
  ::RegisterClassExW(&wc);
  HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"BATTD Trainer", WS_OVERLAPPEDWINDOW, 100, 100, 900, 550, nullptr, nullptr, wc.hInstance, nullptr);

  // Initialize Direct3D
  if (!CreateDeviceD3D(hwnd))
  {
    CleanupDeviceD3D();
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
    return 1;
  }

  // Show the window
  ::ShowWindow(hwnd, SW_SHOWDEFAULT);
  ::UpdateWindow(hwnd);

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  //ImGui::StyleColorsLight();

  // Setup Platform/Renderer backends
  ImGui_ImplWin32_Init(hwnd);
  ImGui_ImplDX9_Init(g_pd3dDevice);

  // Our state
  ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
  
  //Cheat Init
  int commonOrbs = 0;   //0 
  int uncommonOrbs = 0; //1 
  int rareOrbs = 0;     //2 
  int superRareOrbs = 0;//3 
  int epicOrbs = 0;     //4 

  std::string commonOrbsStr;
  std::string uncommonOrbsStr;
  std::string rareOrbsStr;
  std::string superRareOrbsStr;
  std::string epicOrbsStr;

  int addOrbs = 1;

  bool moneyHack = false;
  bool healthHack = false;
  bool essenceHack = false;
  bool crystalHack = false;
  

  auto getOrbs = [&]()
  {
    ReadProcessMemory(hProc, (BYTE*)offsets.commonOrbAddr, &commonOrbs, sizeof(commonOrbs), nullptr);
    ReadProcessMemory(hProc, (BYTE*)offsets.uncommonOrbAddr, &uncommonOrbs, sizeof(uncommonOrbs), nullptr);
    ReadProcessMemory(hProc, (BYTE*)offsets.rareOrbAddr, &rareOrbs, sizeof(rareOrbs), nullptr);
    ReadProcessMemory(hProc, (BYTE*)offsets.superRareOrbAddr, &superRareOrbs, sizeof(superRareOrbs), nullptr);
    ReadProcessMemory(hProc, (BYTE*)offsets.epicOrbAddr, &epicOrbs, sizeof(epicOrbs), nullptr);
  };

  auto getOrbsStrings = [&]()
  {
    commonOrbsStr = "Common orbs: " + std::to_string(commonOrbs);
    uncommonOrbsStr = "Uncommon orbs: " + std::to_string(uncommonOrbs);
    rareOrbsStr = "Rare orbs: " + std::to_string(rareOrbs);
    superRareOrbsStr = "Super rare orbs: " + std::to_string(superRareOrbs);
    epicOrbsStr = "Epic orbs: " + std::to_string(epicOrbs);
  };

  auto setOrb = [&](unsigned int orb, int* valuePtr)
  {
    WriteProcessMemory(hProc, (BYTE*)(offsets.commonOrbAddr + (0x10 * orb)), valuePtr, sizeof(int), NULL);
  };

  // Main loop
  bool done = false;
  while (!done)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    //Cheat Logic
    getOrbs();
    getOrbsStrings();


    // Poll and handle messages (inputs, window resize, etc.)
    // See the WndProc() function below for our to dispatch events to the Win32 backend.
    MSG msg;
    while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
    {
      ::TranslateMessage(&msg);
      ::DispatchMessage(&msg);
      if (msg.message == WM_QUIT)
        done = true;
    }
    if (done)
      break;

    // Handle window resize (we don't resize directly in the WM_SIZE handler)
    if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
    {
      g_d3dpp.BackBufferWidth = g_ResizeWidth;
      g_d3dpp.BackBufferHeight = g_ResizeHeight;
      g_ResizeWidth = g_ResizeHeight = 0;
      ResetDevice();
    }

    // Start the Dear ImGui frame
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos);
    ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);
    
    {
      ImGui::Begin("Trainer Main", nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_NoDecoration | 
                                            ImGuiWindowFlags_::ImGuiWindowFlags_NoResize );
      ImGui::SetWindowFontScale(1.2f);
      //get and draw current orbs
      ImGui::Text(commonOrbsStr.c_str());
      ImGui::Text(uncommonOrbsStr.c_str());
      ImGui::Text(rareOrbsStr.c_str());
      ImGui::Text(superRareOrbsStr.c_str());
      ImGui::Text(epicOrbsStr.c_str());
      //slider to change orbs
      ImGui::SliderInt("How many orbs to add", &addOrbs, 1, 1000);

      //button to add the number of orbs in slider
      ImGui::Text("Add Orbs:");
      if (ImGui::Button("Common"))
      {
        int newOrbs = commonOrbs + addOrbs;
        setOrb(0, &newOrbs);
      }
      ImGui::SameLine();
      if (ImGui::Button("Uncommon"))
      {
        int newOrbs = uncommonOrbs + addOrbs;
        setOrb(1, &newOrbs);
      }
      ImGui::SameLine();
      if (ImGui::Button("Rare"))
      {
        int newOrbs = rareOrbs + addOrbs;
        setOrb(2, &newOrbs);
      }
      ImGui::SameLine();
      if (ImGui::Button("Super rare"))
      {
        int newOrbs = superRareOrbs + addOrbs;
        setOrb(3, &newOrbs);
      }
      ImGui::SameLine();
      if (ImGui::Button("Epic"))
      {
        int newOrbs = epicOrbs + addOrbs;
        setOrb(4, &newOrbs);
      }
      
      bool lastMoneyHack = moneyHack;
      ImGui::Checkbox("In-game money hack", &moneyHack);
      
      if(lastMoneyHack != moneyHack)
      {
        if (moneyHack)
        {
          int patch = 0x0f59;
          WriteProcessMemory(hProc, (BYTE*)offsets.dynamicCashOpCodeAddr, &patch, 1, nullptr );
        }
      
        if (!moneyHack )
        {
          int patch = 0x0f58;
          WriteProcessMemory(hProc, (BYTE*)offsets.dynamicCashOpCodeAddr, &patch, 1, nullptr);
        }
      }

      bool lastHealthHack = healthHack;
      ImGui::Checkbox("Health hack", &healthHack);
      
      if (lastHealthHack != healthHack)
      {
        if(healthHack)
        {
          long long patch = 0x9090909090;
          WriteProcessMemory(hProc, (BYTE*)offsets.dynamicHealthOpCodeAddr, &patch, 5, nullptr);
        }
        else
        {
          long long patch = 0x00d97896e8;
          WriteProcessMemory(hProc, (BYTE*)offsets.dynamicHealthOpCodeAddr, &patch, 5, nullptr);
        }
      }
      
      bool lastEssenceHack = essenceHack;
      ImGui::Checkbox("Cosmic essence hack", &essenceHack);
      if (lastEssenceHack != essenceHack)
      {
        if(essenceHack)
        {
          long long patch = 0x9090909090;
          WriteProcessMemory(hProc, (BYTE*)offsets.dynamicEssenceOpCodeAddr, &patch, 5, nullptr);
        }
        else
        {
          long long patch = 0x0009ffc8e8;
          WriteProcessMemory(hProc, (BYTE*)offsets.dynamicEssenceOpCodeAddr, &patch, 5, nullptr);
        }
      }
 
      bool lastCrystalHack = crystalHack;
      ImGui::Checkbox("Martial crystals hack", &crystalHack);
      if(lastCrystalHack != crystalHack)
      {
        if (crystalHack)
        {
          long long patch = 0x04234584b8;
          WriteProcessMemory(hProc, (BYTE*)offsets.dynamicCrystalOpCodeAddr, &patch, 5, nullptr);
        }
        else
        {
          long long patch = 0x00fd29cfe8;
          WriteProcessMemory(hProc, (BYTE*)offsets.dynamicCrystalOpCodeAddr, &patch, 5, nullptr);
        }
      }
      ImGui::End();
    }

    // Rendering
    ImGui::EndFrame();
    g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
    g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
    D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
    g_pd3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
    if (g_pd3dDevice->BeginScene() >= 0)
    {
      ImGui::Render();
      ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
      g_pd3dDevice->EndScene();
    }
    HRESULT result = g_pd3dDevice->Present(nullptr, nullptr, nullptr, nullptr);
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
  
  
  std::getchar();
}

bool CreateDeviceD3D(HWND hWnd)
{
  if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr)
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
  if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
  if (g_pD3D) { g_pD3D->Release(); g_pD3D = nullptr; }
}

void ResetDevice()
{
  ImGui_ImplDX9_InvalidateDeviceObjects();
  HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
  if (hr == D3DERR_INVALIDCALL)
    IM_ASSERT(0);
  ImGui_ImplDX9_CreateDeviceObjects();
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
    return true;

  switch (msg)
  {
  case WM_SIZE:
    if (wParam == SIZE_MINIMIZED)
      return 0;
    g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
    g_ResizeHeight = (UINT)HIWORD(lParam);
    return 0;
  case WM_SYSCOMMAND:
    if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
      return 0;
    break;
  case WM_DESTROY:
    ::PostQuitMessage(0);
    return 0;
  }
  return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
