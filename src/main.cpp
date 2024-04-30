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

int screen_width;
int screen_height;

INT APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT cmd_show) {

	Menu menu;

	//if the game is not opened we throw error message
	if (!Memory.ProcessOpen()) {
		OutputDebugStringA("[DEBUG TEXT]Process not open!\n");

		MessageBoxA(NULL, "Game not open!", NULL, MB_OK);
	}

	const auto clientaddr = Memory.GetModuleAddress("client.dll");
	if (!clientaddr) { OutputDebugStringA("[DEBUG TEXT]Game not open! Could not find the address of the module specified.\n"); menu.SetRunning(false); }

	//if it fails initialisation we exit
	if (!menu.Init(hInstance, cmd_show))
		return 1;

	{
		ImVec2 temp = menu.GetDimensions();
		screen_height = (int)temp.y;
		screen_width  = (int)temp.x;
	}

	ULONGLONG framestarttick = 0ULL;
	int deltaticks = 0;
	while (menu.GetRunning())
	{
		menu.frametime = deltaticks;

		framestarttick = GetTickCount64();

		menu.HandleKeys();

		menu.HandleMessages();
		if (!menu.GetRunning()) break;

		menu.StartRenderFrame();

		//aici e codul custom
		auto localplayer = Memory.Read<uintptr_t>(clientaddr + dwLocalPlayerPawn);

		auto entitylist = Memory.Read<uintptr_t>(clientaddr + dwEntityList);

		//entity loop
		for (int i = 0; i < 64; i++)
		{
			const auto listEntry = Memory.Read<uintptr_t>(entitylist + 0x10);
			if (!listEntry)
				continue;
		
			auto currentController = Memory.Read<uintptr_t>(listEntry + i * 120ULL);
			if (!currentController)
				continue;
		
			int pawnHandle = Memory.Read<int>(currentController + m_hPlayerPawn);
			if (!pawnHandle)
				continue;

			auto entry2 = Memory.Read<uintptr_t>(entitylist + 0x8ULL * ((pawnHandle & 0x7FFF) >> 9) + 0x10);
		
			auto currentPawn = Memory.Read<uintptr_t>(entry2 + 0x78ULL * (pawnHandle & 0x1FF));
		
			auto viewmatrix = Memory.Read<ViewMatrix>(clientaddr + dwViewMatrix);

			if (menu.IsEspEnabled()) {

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
							auto health = Memory.Read<int>(localplayer + m_iHealth);// useless memory read if we choose const color
							menu.RenderEsp(head, feet, health);

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
