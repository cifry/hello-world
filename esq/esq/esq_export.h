#ifndef esq_export_h
#define esq_export_h

#include "esq.h"
#include "windows.h"

_ESQ_BEGIN

class exprot {
public:
	static IEngine* engine(char* filename, IHandler* handler) {
		HMODULE module = ::LoadLibraryA(filename);
		if (module) {
			typedef _ESQ IEngine*(*IEngine_PTR)(_ESQ IHandler* handler);
			return ((IEngine_PTR)::GetProcAddress(module, "engine"))(handler);
		} else {
			throw _ESQ exception("load DLL failed!", ::GetLastError());
			return nullptr;
		}
	}
};

_ESQ_END

#endif