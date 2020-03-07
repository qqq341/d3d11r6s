// dllmain.cpp : Definiert den Einstiegspunkt für die DLL-Anwendung.
#include "pch.h"
#include "_sprintf.h"
#include "d3d11hook.h"


#define DEBUG 1

int
NTAPI
DllMain(
	_In_ PVOID DllHandle,
	_In_ ULONG FwdReason,
	_In_opt_ PCONTEXT Reserved
)
{
	UNREFERENCED_PARAMETER(DllHandle);
	UNREFERENCED_PARAMETER(FwdReason);
	UNREFERENCED_PARAMETER(Reserved);


	InitHook();
	

	return true;
}

