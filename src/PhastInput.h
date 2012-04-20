#pragma once

#include <string>
#include <list>
#include "WParser.h"

class Prism;

class CPhastInput
{
public:
	static CPhastInput* New(std::istream& is, const char *szPrefix, bool save_prisms = true);
	void Delete(void);
	static CWParser* GetParser() {return (s_instance) ? &s_instance->m_parser : 0;} 
	void Run(void);
	void WritePhastTmp(const char* szPhastTmp);

	void Load(void);
	void Read(void);
	void Accumulate(bool bWritePhastTmp);

	int GetErrorCount(void)const;
	LPCTSTR GetErrorMsg(void)const;

	int GetWarningCount(void)const;
	LPCTSTR GetWarningMsg(void)const;

	static int Output(const int type, const char *err_str, const int stop, void *cookie, const char *format, va_list args);

private:
	CPhastInput(std::istream& is, const char *pcsz_prefix, bool save_prisms);
	~CPhastInput(void);
	void DoInitialize(void);
	void DoCleanUp(void);
private:
	static CPhastInput* s_instance;
	CWParser m_parser;
	std::string m_prefix;
	// static CParser* s_pParser;
	std::string m_s_error;
	std::string m_s_warning;
	std::list<Prism*> m_save_prism_list;
	bool m_save_prisms;
};

