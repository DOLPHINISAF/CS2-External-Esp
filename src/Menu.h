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
	Menu(HINSTANCE hInstance, INT cmd_show);
	~Menu();
	void Zero();

	bool Init(HINSTANCE hInstance, INT cmd_show);
	void ImGuiInit();

	void StartRenderFrame();

	void RenderEsp(ImVec2 head, ImVec2 feet, int health = 0);

	void EndRenderFrame();
	
	void HandleKeys();

	void HandleMessages();

	void CloseD3DandImGui();
	void CloseWin32();

	void SetRunning(bool value) { cheatisrunning = value; }
	bool GetRunning() { return cheatisrunning; }

	HWND GetHWND() { return window; }
	ImVec2 GetDimensions() { return ImVec2((float)screen_width, (float)screen_height); }

	bool IsEspEnabled() { return esp_enabled; }
	
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

	bool esp_enabled;
	int esp_coloring_used;
	float* constespcolor;
	float thicknessmult;

	HWND cs2window;
	bool pressednum8;
	bool pressednum0;
	bool rendermenu;

	
	//error checking
	bool Init_Success;
};

