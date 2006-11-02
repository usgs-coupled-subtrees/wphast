#pragma once

class CSaveCurrentDirectory
{
public:
	CSaveCurrentDirectory(void);
	~CSaveCurrentDirectory(void);

	BOOL SetCurrentDirectory(LPCTSTR lpPathName);
protected:
	TCHAR *path;
};
