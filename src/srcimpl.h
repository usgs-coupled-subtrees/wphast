// srcimpl.h


// SRCAPI is used on global public functions
#ifndef SRCAPI
	#define SRCAPI __stdcall
#endif


// misc helpers
//BOOL AFXAPI AfxFullPath(LPTSTR lpszPathOut, LPCTSTR lpszFileIn);
BOOL SRCAPI SrcFullPath(LPTSTR lpszPathOut, LPCTSTR lpszFileIn);

//void AFXAPI AfxGetRoot(LPCTSTR lpszPath, CString& strRoot);
void SRCAPI SrcGetRoot(LPCTSTR lpszPath, CString& strRoot);

//BOOL AFXAPI AfxComparePath(LPCTSTR lpszPath1, LPCTSTR lpszPath2)
BOOL SRCAPI SrcComparePath(LPCTSTR lpszPath1, LPCTSTR lpszPath2);

#ifndef _AFX_NO_OLE_SUPPORT
class SRC_COM
{
public:
	HRESULT CreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter,
		REFIID riid, LPVOID* ppv);
	HRESULT GetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv);
};

//CString AFXAPI AfxStringFromCLSID(REFCLSID rclsid);
//BOOL AFXAPI AfxGetInProcServer(LPCTSTR lpszCLSID, CString& str);
//BOOL AFXAPI AfxResolveShortcut(CWnd* pWnd, LPCTSTR pszShortcutFile,
//	LPTSTR pszPath, int cchPath);

CString SRCAPI SrcStringFromCLSID(REFCLSID rclsid);
BOOL SRCAPI SrcGetInProcServer(LPCTSTR lpszCLSID, CString& str);
BOOL SRCAPI SrcResolveShortcut(CWnd* pWnd, LPCTSTR pszShortcutFile,
	LPTSTR pszPath, int cchPath);
#endif // _AFX_NO_OLE_SUPPORT
