#include "Menu.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK window_procedure(HWND window, UINT message, WPARAM w_param, LPARAM l_param) {
	if (ImGui_ImplWin32_WndProcHandler(window, message, w_param, l_param)) {
		return 0L;
	}

	if (message == WM_DESTROY) {
		PostQuitMessage(0);
		return 0L;
	}

	return DefWindowProcW(window, message, w_param, l_param);
}
void Menu::Zero() {
	screen_height = 0;
	screen_width = 0;

	window = 0;

	device = nullptr;
	device_context = nullptr;
	swap_chain = nullptr;
	render_targe_view = nullptr;

	frametime = 0;
	cheatisrunning = true;

	esp_enabled = false;
	esp_coloring_used = 0;
	constespcolor = new float[3]();
	cs2window = 0;
	pressednum8 = false;
	pressednum0 = false;
	rendermenu = false;

	thicknessmult = 1.f;

	Init_Success = false;

	//we need to set the cbSize because of versioning
	wndc.cbSize = sizeof(WNDCLASSEXW);
}

Menu::Menu() {
	Zero(); // we only initialise every var 0
}
Menu::Menu(HINSTANCE hInstance, INT cmd_show, const int screen_width, const int screen_height) 
{
	Zero();

	Init(hInstance, cmd_show, screen_width, screen_height);
}

Menu::~Menu() {
	CloseD3DandImGui();
	CloseWin32();
	delete constespcolor;
}

bool Menu::Init(HINSTANCE hInstance, INT cmd_show, const int screen_width, const int screen_height) {
	Init_Success = true;

	this->screen_width = screen_width;
	this->screen_height = screen_height;

	wndc.style = CS_HREDRAW | CS_VREDRAW;
	wndc.lpfnWndProc = window_procedure;
	wndc.hInstance = hInstance;
	wndc.lpszClassName = L"MY WINDOW!";

	RegisterClassExW(&wndc);

	LPCWSTR windowname = L"Masic window";

	window = CreateWindowExW(
		WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED,
		wndc.lpszClassName,
		windowname,
		WS_POPUP,
		0,
		0,
		screen_width,
		screen_height,
		NULL,
		NULL,
		wndc.hInstance,
		NULL
	);

	SetLayeredWindowAttributes(window, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);

	RECT client_Area{};
	GetClientRect(window, &client_Area);

	RECT window_area{};
	GetWindowRect(window, &window_area);

	POINT diff{};
	ClientToScreen(window, &diff);

	MARGINS margins{ window_area.left + (diff.x - window_area.left),
					window_area.top + (diff.y - window_area.top),
					client_Area.right, client_Area.left };
	DwmExtendFrameIntoClientArea(window, &margins);

	DXGI_SWAP_CHAIN_DESC sd{};
	sd.BufferDesc.RefreshRate.Numerator = 60U;
	sd.BufferDesc.RefreshRate.Denominator = 1U;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.SampleDesc.Count = 1U;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 2U;
	sd.OutputWindow = window;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;


	D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0U,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&swap_chain,
		&device,
		NULL,
		&device_context);

	ID3D11Texture2D* back_buffer{};
	swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer);
	if (!back_buffer) {
		Init_Success = false;
	}
	else {
		device->CreateRenderTargetView(back_buffer, nullptr, &render_targe_view);
		back_buffer->Release();
	}
	ShowWindow(window, cmd_show);
	UpdateWindow(window);


	ImGuiInit();

	return Init_Success;
}
void Menu::ImGuiInit() {
	ImGui::CreateContext();
	ImGui::StyleColorsDark(); // it styles the menu to dark

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(device, device_context);

}

void Menu::StartRenderFrame() {

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();

	if (rendermenu) {
		ImGuiWindowFlags flags = 64; // resizes the window to fit everything it contains
		ImGui::Begin("ESP OPTIONS", NULL, flags);

		float fps;
		if (frametime == 0) fps = 0;
		else fps = 1000.f / frametime;

		ImGui::Checkbox("ESP ACTIVE", &esp_enabled);

		ImGui::SliderFloat("Thickness multiplier", &thicknessmult, 0.01f, 1.f);

		ImGui::RadioButton("Health based esp", &esp_coloring_used, 0);
		ImGui::RadioButton("Constant esp", &esp_coloring_used, 1);

		if (esp_coloring_used == 1) // show the color picker only if we want to select a custom color
			ImGui::ColorPicker3("ESP COLOR", constespcolor, 0);

		ImGui::Text("DEBUG DATA:");
		ImGui::Text("FPS: %s", std::to_string(fps).c_str());
		//quit button
		if (ImGui::Button("Quit"))cheatisrunning = false;
	}
	ImGui::Render();

}

void Menu::EndRenderFrame() {
	ImGui::Render();

	if (!render_targe_view)
		cheatisrunning = false;
	else {

		device_context->OMSetRenderTargets(1U, &render_targe_view, nullptr);

		float blankcolor[4]{};
		device_context->ClearRenderTargetView(render_targe_view, blankcolor);

		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		swap_chain->Present(0U, 0U);
	}
}

void Menu::HandleKeys() {

	if (GetAsyncKeyState(VK_NUMPAD9)) cheatisrunning = false;

	//we toggle between the option menu and the game
	if (GetAsyncKeyState(VK_NUMPAD0)) {
		if (!pressednum0) {
			rendermenu = !rendermenu;
			pressednum0 = true;

			//if i want to show the menu
			if (rendermenu) {
				cs2window = GetForegroundWindow();
				SetWindowLongPtrA(window, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_TRANSPARENT); // we make the menu's window interactable
				SetForegroundWindow(window);//we select the window so it is active aleady when we click 

			}
			else {
				SetWindowLongPtrA(window, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED);
				SetWindowPos(window, NULL, 0, 0, screen_width, screen_height, NULL);
				SetForegroundWindow(cs2window);
			}
		}

	}
	else pressednum0 = false;

	//we toggle the esp on/off
	if (GetAsyncKeyState(VK_NUMPAD8)) {
		if (!pressednum8) {
			esp_enabled = !esp_enabled;
			pressednum8 = true;
		}
	}
	else pressednum8 = false;

}

void Menu::HandleMessages() {
	MSG msg{};
	while (PeekMessage(&msg, window, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg); // it sends the message to window_procedure(...) . It handles our messages
		if (msg.message == WM_QUIT) {
			cheatisrunning = false;
		}

	}
}
void Menu::CloseD3DandImGui() {
	if (Init_Success) {
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();

		ImGui::DestroyContext();
	}
	if (swap_chain) swap_chain->Release();
	if (device_context)device_context->Release();
	if (device) device->Release();
	if (render_targe_view)render_targe_view->Release();

}
void Menu::CloseWin32() {
	if(window)	DestroyWindow(window);
	UnregisterClassW(wndc.lpszClassName, wndc.hInstance);
}
