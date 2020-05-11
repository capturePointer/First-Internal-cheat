#include <iostream>
#include <tchar.h>
#include "External/Detours.h"
#include "hooks.h"
#include "imgui\imgui.h"
#include "imgui\imgui_impl_win32.h"
#include "imgui\imgui_impl_dx9.h"
#include "PlayerModels.h"

#pragma comment(lib, "External/detours.lib")

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Original hooks
static WNDPROC wndProc_orig = 0;
static EndScene EndScene_orig = 0;
static SetStreamSource SetStreamSource_orig = 0;
static SetVertexDeclaration SetVertexDeclaration_orig = 0;
static SetVertexShaderConstantF SetVertexShaderConstantF_orig = 0;
static SetVertexShader SetVertexShader_orig = 0;
static SetPixelShader SetPixelShader_orig = 0;
static DrawIndexedPrimitive  DrawIndexedPrimitive_orig = 0;

static HWND window = 0;
static bool show_menu = false;
static bool chams = false;


IDirect3DPixelShader9* shaderback = 0;
IDirect3DPixelShader9* shaderfront = 0;
UINT stride;
D3DVERTEXELEMENT9 decl[MAXD3DDECLLENGTH];
UINT numElements, mStartRegister, mVectorCount, vSize, pSize;
IDirect3DVertexShader9* vShader;
IDirect3DPixelShader9* pShader;



HRESULT GenerateShader(IDirect3DDevice9* pDevice, IDirect3DPixelShader9** pShader, float r, float g, float b)
{
	TCHAR szShader[256];
	ID3DXBuffer* pShaderBuf = NULL;

	_stprintf_s(szShader, _T("ps.1.1\ndef c0, %f, %f, %f\nmov r0,c0"), r, g, b);

	if (FAILED(D3DXAssembleShader(szShader, sizeof(szShader), NULL, NULL, 0, &pShaderBuf, NULL)))
		return E_FAIL;

	if (FAILED(pDevice->CreatePixelShader((const DWORD*)pShaderBuf->GetBufferPointer(), pShader)))
		return E_FAIL;

	return D3D_OK;
}



LRESULT WINAPI WndProc_hook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (show_menu && ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}

	return CallWindowProc(wndProc_orig, hWnd, msg, wParam, lParam);
}

HRESULT APIENTRY EndScene_hook(IDirect3DDevice9* pDevice)
{
	static bool init = false;
	static bool insertWasPressed = false;
	static bool isToggled = false;

	if (!shaderback)
	{
		GenerateShader(pDevice, &shaderback, 211.0f / 255.0f, 177.0f / 255.0f, 31.0f / 255.0f);
	}

	if (!shaderfront)
	{
		GenerateShader(pDevice, &shaderfront, 31.0f / 255.0f, 99.0f / 255.0f, 155.0f / 255.0f);
	}

	if (!init)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();

		ImGui::StyleColorsDark();
		ImGui_ImplWin32_Init(window);
		ImGui_ImplDX9_Init(pDevice);
	}

	bool insertPressed = GetAsyncKeyState(VK_INSERT);

	if (!insertWasPressed && insertPressed)
	{
		show_menu = !show_menu;
	}

	insertWasPressed = insertPressed;

	if (show_menu)
	{
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Menu", &show_menu);
		ImGui::Checkbox("Chams", &chams);

		if (ImGui::Button("Rage quit"))
		{
			isToggled = true;
			chams = true;
		}

		if (isToggled)
		{
			ImGui::Text("Button press detected");
		}

		ImGui::End();
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	}

	return EndScene_orig(pDevice);
}

HRESULT APIENTRY SetStreamSource_hook(IDirect3DDevice9* pDevice, UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT sstride)
{
	if (StreamNumber == 0)
	{
		//stride = sstride;
	}

	return SetStreamSource_orig(pDevice, StreamNumber, pStreamData, OffsetInBytes, sstride);
}

HRESULT APIENTRY SetVertexDeclaration_hook(IDirect3DDevice9* pDevice, IDirect3DVertexDeclaration9* pDecl)
{
	if (pDecl != NULL)
	{
		//pDecl->GetDeclaration(decl, &numElements);
	}

	return SetVertexDeclaration_orig(pDevice, pDecl);
}

HRESULT APIENTRY SetVertexShaderConstantF_hook(IDirect3DDevice9* pDevice, UINT StartRegister, const float* pConstantData, UINT Vector4fCount)
{
	if (pConstantData != NULL)
	{
		//mStartRegister = StartRegister;
		//mVectorCount = Vector4fCount;
	}

	return SetVertexShaderConstantF_orig(pDevice, StartRegister, pConstantData, Vector4fCount);
}

HRESULT APIENTRY SetVertexShader_hook(IDirect3DDevice9* pDevice, IDirect3DVertexShader9* veShader)
{
	if (veShader != NULL)
	{
		//vShader = veShader;
		//vShader->GetFunction(NULL, &vSize);
	}

	return SetVertexShader_orig(pDevice, veShader);
}

HRESULT APIENTRY SetPixelShader_hook(IDirect3DDevice9* pDevice, IDirect3DPixelShader9* piShader)
{
	if (piShader != NULL)
	{
		//pShader = piShader;
		//pShader->GetDevice(NULL, &pSize);
	}

	return SetPixelShader_orig(pDevice, piShader);
}

HRESULT APIENTRY DrawIndexedPrimitive_hook(IDirect3DDevice9* pDevice, D3DPRIMITIVETYPE arg0, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{
	if (((T_Models) || (CT_Models)))
	{
		pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
		pDevice->SetPixelShader(shaderback);

		DrawIndexedPrimitive_orig(pDevice, arg0, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);

		pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
		pDevice->SetPixelShader(shaderfront);

		return S_OK;
	}

	return DrawIndexedPrimitive_orig(pDevice, arg0, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
}\

void InstallHooks(HWND hWindow, void** pd3d9Device)
{
	window = hWindow;

	wndProc_orig = (WNDPROC)SetWindowLongPtr(hWindow, GWLP_WNDPROC, (LONG_PTR)WndProc_hook);
	EndScene_orig = (EndScene)Detours::X86::DetourFunction((uintptr_t)pd3d9Device[42], (uintptr_t)EndScene_hook);
	DrawIndexedPrimitive_orig = (DrawIndexedPrimitive)Detours::X86::DetourFunction((uintptr_t)pd3d9Device[82], (uintptr_t)DrawIndexedPrimitive_hook);
	SetVertexDeclaration_orig = (SetVertexDeclaration)Detours::X86::DetourFunction((uintptr_t)pd3d9Device[87], (uintptr_t)SetVertexDeclaration_hook);
	SetVertexShader_orig = (SetVertexShader)Detours::X86::DetourFunction((uintptr_t)pd3d9Device[92], (uintptr_t)SetVertexShader_hook);
	SetVertexShaderConstantF_orig = (SetVertexShaderConstantF)Detours::X86::DetourFunction((uintptr_t)pd3d9Device[94], (uintptr_t)SetVertexShaderConstantF_hook);
	SetStreamSource_orig = (SetStreamSource)Detours::X86::DetourFunction((uintptr_t)pd3d9Device[100], (uintptr_t)SetStreamSource_hook);
	SetPixelShader_orig = (SetPixelShader)Detours::X86::DetourFunction((uintptr_t)pd3d9Device[107], (uintptr_t)SetPixelShader_hook);
}
