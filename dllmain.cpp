#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include <Windows.h>
#include <iostream>
#include <d3d9.h>
#include <d3dx9.h>
#include "hooks.h"
#include "imgui\imgui.h"
#include "imgui\imgui_impl_win32.h"
#include "imgui\imgui_impl_dx9.h"

LPDIRECT3D9 g_pD3D = NULL;
LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
D3DPRESENT_PARAMETERS g_d3dpp = {};

BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam)
{
	HWND* phWnd = (HWND*)lParam;
	DWORD wndProcID;

	GetWindowThreadProcessId(handle, &wndProcID);

	if (GetCurrentProcessId() != wndProcID)
	{
		return TRUE;
	}

	*phWnd = handle;

	return FALSE;
}

HWND GetProcessWindow()
{
	HWND w;

	EnumWindows(EnumWindowsCallback, (LPARAM)&w);

	return w;
}

bool GetD3D9Device(HWND hwnd, void** pTable, size_t size)
{
	
	if (!pTable) 
	{
		return false;
	}

	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if (!g_pD3D) 
	{
		return false;
	}

	D3DPRESENT_PARAMETERS d3dpp = {};
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = GetProcessWindow();
	d3dpp.Windowed = false;
	
	g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dDevice);

	if (!g_pd3dDevice)
	{
		d3dpp.Windowed = !d3dpp.Windowed;
		g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dDevice);

		if (!g_pd3dDevice) 
		{
			g_pD3D->Release();
			return false;
		}

		memcpy(pTable, *reinterpret_cast<void***>(g_pd3dDevice), size);

		g_pd3dDevice->Release();
		g_pD3D->Release();
		return true;

	}
}

void CleanUpDeviceD3D()
{
	if (g_pd3dDevice)
	{
		g_pd3dDevice->Release();
		g_pd3dDevice = NULL;
	}

	if (g_pD3D)
	{
		g_pD3D->Release();
		g_pD3D = NULL;
	}
}

DWORD WINAPI MainThread(PVOID base)
{
	HWND window = GetProcessWindow();
	void* d3d9Device[119];

	if (GetD3D9Device(window, d3d9Device, sizeof(d3d9Device)))
	{
		InstallHooks(window, d3d9Device);

		while (true)
		{
			if (GetAsyncKeyState(VK_END))
			{
				CleanUpDeviceD3D();
				FreeLibraryAndExitThread(static_cast<HMODULE>(base), 1);
			}
		}
	}

	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(nullptr, NULL, MainThread, hModule, NULL, nullptr);
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
