#include <d3d11.h>
#include "_sprintf.h"
#include "MinHook.h"
#include <d3d11.h>
#pragma comment(lib,"d3d11")
#include <stdio.h>
#include "D3D11Renderer.h"
#include <iostream>


#define DEBUG 1

ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
IDXGISwapChain* pSwapChain = NULL;
ID3D11RenderTargetView* pRenderTargetView = NULL;

char buffer[128];

typedef HRESULT(__stdcall* tD3D11Present)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
typedef void(__stdcall* tD3D11DrawIndexed)(ID3D11DeviceContext* pContext, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);
typedef void(__stdcall* tD3D11CreateQuery)(ID3D11Device* pDevice, const D3D11_QUERY_DESC* pQueryDesc, ID3D11Query** ppQuery);
typedef void(__stdcall* tD3D11EndScene)(ID3D11Device* pDevice, const D3D11_QUERY_DESC* pQueryDesc, ID3D11Query** ppQuery);

namespace Hooks
{
	//ORIGINAL FUNCTIONS

	tD3D11CreateQuery oCreateQuery;
	tD3D11Present oPresent;
	tD3D11DrawIndexed oDrawIndexed;

	//HOOK FUNCTIONS

	HRESULT hkD3D11Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
}
bool bOnce = false;

D3D11Renderer renderer;
HRESULT __stdcall Hooks::hkD3D11Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (!bOnce)
	{
		//OutputDebugString("\n[DLL] Initializing Renderer \n");
		// Initialize
		renderer.swapChain = pSwapChain;
		renderer.device = NULL;
		renderer.deviceContext = NULL;
		renderer.mVS = NULL;
		renderer.mPS = NULL;
		renderer.transparency = NULL;
		renderer.mInputLayout = NULL;
		renderer.mVertexBuffer = NULL;

		renderer.stateSaver.m_savedState = false;
		renderer.stateSaver.m_featureLevel = D3D_FEATURE_LEVEL_11_0;
		renderer.stateSaver.m_pContext = NULL;
		renderer.stateSaver.m_primitiveTopology = (D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED);
		renderer.stateSaver.m_pInputLayout = (NULL);
		renderer.stateSaver.m_pBlendState = (NULL);
		renderer.stateSaver.m_sampleMask = (0xffffffff);
		renderer.stateSaver.m_pDepthStencilState = (NULL);
		renderer.stateSaver.m_stencilRef = (0);
		renderer.stateSaver.m_pRasterizerState = (NULL);
		renderer.stateSaver.m_pPSSRV = (NULL);
		renderer.stateSaver.m_pSamplerState = (NULL);
		renderer.stateSaver.m_pVS = (NULL);
		renderer.stateSaver.m_numVSClassInstances = (0);
		renderer.stateSaver.m_pVSConstantBuffer = (NULL);
		renderer.stateSaver.m_pGS = (NULL);
		renderer.stateSaver.m_numGSClassInstances = (0);
		renderer.stateSaver.m_pGSConstantBuffer = (NULL);
		renderer.stateSaver.m_pGSSRV = (NULL);
		renderer.stateSaver.m_pPS = (NULL);
		renderer.stateSaver.m_numPSClassInstances = (0);
		renderer.stateSaver.m_pHS = (NULL);
		renderer.stateSaver.m_numHSClassInstances = (0);
		renderer.stateSaver.m_pDS = (NULL);
		renderer.stateSaver.m_numDSClassInstances = (0);
		renderer.stateSaver.m_pVB = (NULL);
		renderer.stateSaver.m_vertexStride = (0);
		renderer.stateSaver.m_vertexOffset = (0);
		renderer.stateSaver.m_pIndexBuffer = (NULL);
		renderer.stateSaver.m_indexFormat = (DXGI_FORMAT_UNKNOWN);
		renderer.stateSaver.m_indexOffset = (0);
		for (int i = 0; i < 4; ++i)
			renderer.stateSaver.m_blendFactor[i] = 0.0f;
		for (int i = 0; i < 256; ++i)
		{
			renderer.stateSaver.m_pVSClassInstances[i] = NULL;
			renderer.stateSaver.m_pGSClassInstances[i] = NULL;
			renderer.stateSaver.m_pPSClassInstances[i] = NULL;
			renderer.stateSaver.m_pHSClassInstances[i] = NULL;
			renderer.stateSaver.m_pDSClassInstances[i] = NULL;
		}

		//OutputDebugString("\n[DLL] Initializing Renderer 2 \n");
		if (!renderer.Initialize()) {
		//	OutputDebugString("\n[DLL] Initializing Renderer Failed \n");
		}
		else {
		//	OutputDebugString("\n[DLL] Initializing Renderer Worked \n");
		}


		
		bOnce = true;
	}

	renderer.PreRender();
	renderer.BeginScene();
	renderer.DrawLine(200.f, 200.f, 300.f, 200.f, Color(255.f, 255.f, 0.f, 0.f));
	renderer.DrawLine(300.f, 200.f, 300.f, 300.f, Color(255.f, 255.f, 0.f, 0.f));
	renderer.DrawLine(300.f, 300.f, 200.f, 300.f, Color(255.f, 255.f, 0.f, 0.f));
	renderer.DrawLine(200.f, 200.f, 200.f, 300.f, Color(255.f, 255.f, 0.f, 0.f));
	renderer.PostRender();
	renderer.EndScene();
	return Hooks::oPresent(pSwapChain, SyncInterval, Flags);
}


void InitHook()
{
	//OutputDebugString("\n[DLL] Starting Hooking Process \n");

	// pointer chain to the swapchain of r6s
	DWORD64 r6base = (DWORD64)GetModuleHandleA(NULL);
	DWORD64 swapchainptrtmp = *(DWORD64*)(r6base + 0x5399A08);
	DWORD64 swapchainptrtmp2 = *(DWORD64*)(swapchainptrtmp + 0x790);
	DWORD64 swapchainptr = *(DWORD64*)(swapchainptrtmp2 + 0x28);
	DWORD_PTR* pSwapChainVT = reinterpret_cast<DWORD_PTR*>(swapchainptr);
	pSwapChainVT = reinterpret_cast<DWORD_PTR*>(pSwapChainVT[0]);

	
	Hooks::oPresent = reinterpret_cast<tD3D11Present>(pSwapChainVT[8]);

	MH_STATUS dbg;
	// Minhook hooking library for hooking the present
	dbg = MH_Initialize();
	dbg = MH_CreateHook(
		(void*) pSwapChainVT[8],														
		(void*)Hooks::hkD3D11Present,
		(void**)&Hooks::oPresent
	);
	dbg = MH_EnableHook(nullptr);
	
}