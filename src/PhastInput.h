#pragma once

#include <string>
#include <list>
#include "Parser.h"

class Prism;

class CPhastInput
{
public:
	static CPhastInput* New(std::istream& is, const char *pcsz_prefix);
	void Delete(void);
	static CParser* GetParser() {return (s_instance) ? &s_instance->m_parser : 0;} 
	void Run(void);
	void WritePhastTmp(const char* szPhastTmp);

	void Load(void);
// COMMENT: {3/31/2005 6:21:49 PM}	void LoadFirstStressPeriod(void);
// COMMENT: {3/31/2005 6:21:49 PM}	bool LoadNextStressPeriod(void);

	int GetErrorCount(void)const;
	LPCTSTR GetErrorMsg(void)const;

	int GetWarningCount(void)const;
	LPCTSTR GetWarningMsg(void)const;

	static int Output(const int type, const char *err_str, const int stop, void *cookie, const char *format, va_list args);

private:
	CPhastInput(std::istream& is, const char *pcsz_prefix);
	~CPhastInput(void);
	void DoInitialize(void);
	void DoCleanUp(void);
private:
	static CPhastInput* s_instance;
	CParser m_parser;
	std::string m_prefix;
	// static CParser* s_pParser;
	std::string m_s_error;
	std::string m_s_warning;
	std::list<Prism*> m_save_prism_list;
};
