#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <dwmapi.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include <string>

class Menu
{
public:
	Menu();
	Menu(HINSTANCE hInstance, INT cmd_show, const int screen_width, const int screen_height);
	~Menu();
	void Zero();

	bool Init(HINSTANCE hInstance, INT cmd_show, const int screen_width, const int screen_height);
	void ImGuiInit();

	void StartRenderFrame();
	void EndRenderFrame();
	
	void HandleKeys();

	void HandleMessages();

	void CloseD3DandImGui();
	void CloseWin32();
	void SetRunning(bool value) { cheatisrunning = value; }
	bool GetRunning() { return cheatisrunning; }

	bool esp_enabled;
	int esp_coloring_used;
	float* constespcolor;
	float thicknessmult;
	int frametime; 
private:

	HWND window;
	WNDCLASSEXW wndc;

	ID3D11Device* device;
	ID3D11DeviceContext* device_context;
	IDXGISwapChain* swap_chain;
	ID3D11RenderTargetView* render_targe_view;

	int screen_width;
	int screen_height;

	
	bool cheatisrunning;

	
	
	HWND cs2window;
	bool pressednum8;
	bool pressednum0;
	bool rendermenu;

	
};

