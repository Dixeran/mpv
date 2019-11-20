#include <windows.h>
 HRESULT CheckExportDef(int a, int b) {
	if (a + b > 0) return S_OK;
	else return E_UNEXPECTED;
}