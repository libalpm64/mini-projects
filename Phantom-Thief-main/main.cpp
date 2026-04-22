#include "Memory/Memory.h"
#include "Offsets/Offsets.h"
#include <d3d9.h>
#include <dwmapi.h>
#include <imgui.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>
// ... other necessary includes

class Overlay {
private:
    HWND hwnd;
    IDirect3D9Ex* d3d;
    IDirect3DDevice9Ex* device;
    D3DPRESENT_PARAMETERS params;
    
public:
    Overlay() : hwnd(nullptr), d3d(nullptr), device(nullptr) {}
    
    bool Initialize() {
        // Create overlay window
        WNDCLASSEX wc = {
            sizeof(WNDCLASSEX),
            0,
            DefWindowProc,
            0,
            0,
            nullptr,
            LoadIcon(nullptr, IDI_APPLICATION),
            LoadCursor(nullptr, IDC_ARROW),
            nullptr,
            nullptr,
            "ESPOverlay",
            LoadIcon(nullptr, IDI_APPLICATION)
        };

        if (!RegisterClassEx(&wc))
            return false;

        // Create transparent window
        hwnd = CreateWindowEx(
            WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED,
            "ESPOverlay",
            "ESP",
            WS_POPUP,
            0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
            nullptr,
            nullptr,
            nullptr,
            nullptr
        );

        if (!hwnd)
            return false;

        // Set window transparency
        SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 255, LWA_ALPHA);
        
        // Initialize Direct3D
        if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &d3d)))
            return false;

        ZeroMemory(&params, sizeof(params));
        params.Windowed = TRUE;
        params.SwapEffect = D3DSWAPEFFECT_DISCARD;
        params.hDeviceWindow = hwnd;
        params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
        params.BackBufferFormat = D3DFMT_A8R8G8B8;
        params.BackBufferWidth = GetSystemMetrics(SM_CXSCREEN);
        params.BackBufferHeight = GetSystemMetrics(SM_CYSCREEN);
        params.EnableAutoDepthStencil = TRUE;
        params.AutoDepthStencilFormat = D3DFMT_D16;

        if (FAILED(d3d->CreateDeviceEx(
            D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL,
            hwnd,
            D3DCREATE_HARDWARE_VERTEXPROCESSING,
            &params,
            nullptr,
            &device)))
            return false;

        // Initialize ImGui
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX9_Init(device);

        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);

        return true;
    }

    void BeginRender() {
        device->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
        device->BeginScene();
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    void EndRender() {
        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        device->EndScene();
        device->Present(nullptr, nullptr, nullptr, nullptr);
    }

    ~Overlay() {
        if (device) {
            device->Release();
            device = nullptr;
        }
        if (d3d) {
            d3d->Release();
            d3d = nullptr;
        }
        DestroyWindow(hwnd);
    }
};

class ESP {
private:
    Memory& memory;
    uintptr_t baseAddress;
    uintptr_t va_text;  // Add va_text for proper address calculation

    uintptr_t GetUWorld() {
        return memory.Read<uintptr_t>(va_text + Offsets::UWORLD);  // Use va_text instead of baseAddress
    }

    struct PlayerInfo {
        Vector3 position;
        bool isVisible;
        float health;
        float shield;
        std::string name;
    };

public:
    ESP(Memory& mem, uintptr_t base, uintptr_t vaText) 
        : memory(mem), baseAddress(base), va_text(vaText) {}

    void Render() {
        auto uworld = GetUWorld();
        if (!uworld) {
            LOG("Failed to get UWorld\n");
            return;
        }

        auto gameInstance = memory.Read<uintptr_t>(uworld + Offsets::GAME_INSTANCE);
        auto localPlayers = memory.Read<uintptr_t>(gameInstance + Offsets::LOCAL_PLAYERS);
        auto localPlayer = memory.Read<uintptr_t>(localPlayers);
        auto playerController = memory.Read<uintptr_t>(localPlayer + Offsets::PLAYER_CONTROLLER);
        auto localPawn = memory.Read<uintptr_t>(playerController + Offsets::LOCAL_PAWN);

        if (!localPawn) return;

        auto gameState = memory.Read<uintptr_t>(uworld + Offsets::GAME_STATE);
        auto playerArray = memory.Read<uintptr_t>(gameState + Offsets::PLAYER_ARRAY);
        auto playerCount = memory.Read<int>(playerArray + 0x8);

        // Create scatter handle for batch reading
        auto handle = memory.CreateScatterHandle();

        for (int i = 0; i < playerCount; i++) {
            auto playerState = memory.Read<uintptr_t>(playerArray + (i * 0x8));
            if (!playerState) continue;

            auto pawn = memory.Read<uintptr_t>(playerState + Offsets::PAWN_PRIVATE);
            if (!pawn || pawn == localPawn) continue;

            auto mesh = memory.Read<uintptr_t>(pawn + Offsets::MESH);
            if (!mesh) continue;

            // Get player position
            auto rootComponent = memory.Read<uintptr_t>(pawn + Offsets::ROOT_COMPONENT);
            auto position = memory.Read<Vector3>(rootComponent + Offsets::RELATIVE_LOCATION);

            // Draw ESP box
            Vector2 screenPos; // You'll need to implement WorldToScreen
            if (WorldToScreen(position, screenPos)) {
                ImGui::GetForegroundDrawList()->AddRect(
                    ImVec2(screenPos.x - 20, screenPos.y - 40),
                    ImVec2(screenPos.x + 20, screenPos.y + 40),
                    IM_COL32(255, 0, 0, 255)
                );
            }
        }

        memory.CloseScatterHandle(handle);
    }
};

int main() {
    // Initialize Memory class
    Memory memory;
    LOG("Loading libraries...\n");

    // Wait for Fortnite window
    HWND hwnd = NULL;
    while (!hwnd) {
        hwnd = FindWindowA(NULL, "Fortnite  ");
        if (hwnd) break;
        Sleep(3000);
    }

    // Get process ID and base address
    handler::process_id = handler::find_process("FortniteClient-Win64-Shipping.exe");
    if (!handler::process_id) {
        LOG("Failed to find Fortnite process\n");
        return 1;
    }

    auto virtualaddy = handler::find_image();
    auto cr3 = handler::fetch_cr3();

    // Find va_text
    uintptr_t va_text = 0;
    for (auto i = 0; i < 25; i++) {
        if (memory.Read<__int32>(virtualaddy + (i * 0x1000) + 0x250) == 0x6F41C00) {
            va_text = virtualaddy + ((i + 1) * 0x1000);
            break;
        }
    }

    if (!va_text) {
        LOG("Failed to find va_text\n");
        return 1;
    }

    LOG("Base Address -> %p\n", virtualaddy);
    LOG("CR3 -> %p\n", cr3);
    LOG("VAText -> %p\n", va_text);

    // Initialize overlay
    Overlay overlay;
    if (!overlay.Initialize()) {
        LOG("Failed to initialize overlay\n");
        return 1;
    }

    // Initialize ESP
    ESP esp(memory, virtualaddy, va_text);

    // Main loop
    while (true) {
        overlay.BeginRender();
        esp.Render();
        overlay.EndRender();
        
        Sleep(1);
    }

    return 0;
}
