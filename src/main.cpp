#include <Windows.h>
#include <d3d11.h>

#include <dwmapi.h>
#include <string>

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include "memory.h"
#include "offsets.h"
#include "Vec3.h"
#include "Menu.h"
using namespace offsets;


//GLOBAL VARIABLES
memory Memory("cs2.exe");

int screen_width  = 1280;
int screen_height = 720;

INT APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT cmd_show) {

	Menu menu;
	//if the game is not opened before we try to run we try some more
	if (!Memory.ProcessOpen()) {
		OutputDebugStringA("Process not open! Attempting to find process.\n");
		//we try for 5 seconds to find if the game is opened
		for (int tries = 0; tries < 10; tries++) {
			OutputDebugStringA("Retrying...\n");
			Memory.SetProcIDandHandle("cs2.exe");
			Sleep(500);
		}
		//if the game is still not open we close everything and exit
		if (!Memory.ProcessOpen()) {
			OutputDebugStringA("Failed to open proces. Exiting now\n");
			menu.SetRunning(false);
		}
	}

	const auto clientaddr = Memory.GetModuleAddress("client.dll");
	if (!clientaddr) { OutputDebugStringA("Game not open! Could not find the address of the module specified.\n"); menu.SetRunning(false); }

	//if it fails initialisation we exit
	if (!menu.Init(hInstance, cmd_show, screen_width, screen_height))
		return 1;
	
	ULONGLONG framestarttick = 0ULL;
	int deltaticks = 0;
	while (menu.GetRunning())
	{
		menu.frametime = deltaticks;

		framestarttick = GetTickCount64();


		//it gets the total time from the previous loop
		Sleep(16);
		menu.HandleKeys();

		menu.HandleMessages();
		if (!menu.GetRunning()) break;

		menu.StartRenderFrame();


		//aici e codul custom
		auto localplayer = Memory.Read<uintptr_t>(clientaddr + dwLocalPlayerPawn);

		auto entitylist = Memory.Read<uintptr_t>(clientaddr + dwEntityList);

		for (int i = 0; i < 64; i++)
		{
			const auto listEntry = Memory.Read<uintptr_t>(entitylist + 0x10);
			if (!listEntry)
				continue;
		
			auto currentController = Memory.Read<uintptr_t>(listEntry + i * 120);
			if (!currentController)
				continue;
		
			int pawnHandle = Memory.Read<int>(currentController + m_hPlayerPawn);
			if (!pawnHandle)
				continue;

			auto entry2 = Memory.Read<uintptr_t>(entitylist + 0x8 * ((pawnHandle & 0x7FFF) >> 9) + 0x10);
		
			auto currentPawn = Memory.Read<uintptr_t>(entry2 + 0x78 * (pawnHandle & 0x1FF));
		
			auto viewmatrix = Memory.Read<ViewMatrix>(clientaddr + dwViewMatrix);

			if (menu.esp_enabled) {

				auto player_health = Memory.Read<BYTE>(currentPawn + m_iHealth);
				
				if (player_health) {
					auto player_team = Memory.Read<BYTE>(currentPawn + m_iTeamNum);

					auto localplayerteam = Memory.Read<BYTE>(localplayer + m_iTeamNum);

					if (player_team != localplayerteam) {
						auto absorigin = Memory.Read<Vec3>(currentPawn + m_vOldOrigin);

						Vec3 eyepos = Memory.Read<Vec3>(currentPawn + m_vecViewOffset);
						eyepos.x += absorigin.x;
						eyepos.y += absorigin.y;
						eyepos.z += absorigin.z;
						//locations of these objects on the screen
						ImVec2 head, feet;

						//checks if the head and the feet are visible so we don't render what we can't see
						if (Vec3::World2Screen(feet, viewmatrix.vm, absorigin) && (Vec3::World2Screen(head, viewmatrix.vm, eyepos)))
						{
							float width = (head.y - feet.y);

							ImVec2 topleft;
							topleft.y = head.y;
							topleft.x = head.x + width / 3;

							ImVec2 bottomright;
							bottomright.y = feet.y;
							bottomright.x = feet.x - width / 3;

							ImU32 color;
							if (menu.esp_coloring_used) { // if we render the boxes with a color we choose
								color = ImGui::ColorConvertFloat4ToU32(ImVec4(menu.constespcolor[0], menu.constespcolor[1], menu.constespcolor[2], 1.f));
							}
							else {
								auto health = Memory.Read<int>(currentPawn + m_iHealth);
								int green = 255 - (int)((100.f - health) * 2.55f);
								int red = (int)((100.f - health) * 2.55f);
								color = ImColor(red, green, 0);
							}
							ImGui::GetBackgroundDrawList()->AddRect(topleft, bottomright, color, 0.f, NULL, menu.thicknessmult * -width / 50.f);
							ImGui::GetBackgroundDrawList()->AddCircleFilled(head, menu.thicknessmult * -width / 25.f, ImColor(255, 255, 255));

						}
					}
				}
				
				
			}
			
		}

		menu.EndRenderFrame();

		deltaticks = int(GetTickCount64() - framestarttick);
		if (deltaticks < 32) Sleep(32 - deltaticks);

	}

	return 0;
}
