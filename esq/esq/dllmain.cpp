#include "esq_engine.hpp"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		_ESQ_SERVICE startup();
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		_ESQ_SERVICE shutdown();
	}
	return TRUE;
}

extern "C" __declspec(dllexport) _ESQ IEngine* engine(_ESQ IHandler* handler)
{
	return new _ESQ engine(*handler);
}
