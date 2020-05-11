#pragma once

#include <d3d9.h>
#include <d3dx9.h>

typedef HRESULT(APIENTRY* EndScene)(IDirect3DDevice9* pDevice);
typedef HRESULT(APIENTRY* SetStreamSource)(IDirect3DDevice9* pDevice, UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT sstride);
typedef HRESULT(APIENTRY* SetVertexDeclaration)(IDirect3DDevice9* pDevice, IDirect3DVertexDeclaration9* pDecl);
typedef HRESULT(APIENTRY* SetVertexShaderConstantF)(IDirect3DDevice9* pDevice, UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount);
typedef HRESULT(APIENTRY* SetVertexShader)(IDirect3DDevice9* pDevice, IDirect3DVertexShader9* pShader);
typedef HRESULT(APIENTRY* SetPixelShader)(IDirect3DDevice9* pDevice, IDirect3DPixelShader9* pShader);
typedef HRESULT(APIENTRY* DrawIndexedPrimitive)(IDirect3DDevice9* pDevice, D3DPRIMITIVETYPE arg0, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount);

void InstallHooks(HWND window, void** pd3d9Device);
