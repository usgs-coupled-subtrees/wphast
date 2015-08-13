#pragma once

#include <string>
#include <list>
#include "WParser.h"
#include "PHRQ_io.h"

class Prism;

class CPhastInput
{
public:
	static CPhastInput* New(std::istream& is, const std::string sPrefix, const std::string sDatabase, bool save_prisms = true);
	void Delete(void);
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
	explicit CPhastInput(PHRQ_io& io, const char *szPrefix, const char *szDatabase, bool save_prisms);
	~CPhastInput(void);
	void DoInitialize(void);
	void DoCleanUp(void);
private:
	static CPhastInput* s_instance;
	PHRQ_io& m_io;
	std::string m_prefix;
	std::string m_database;
	// static CParser* s_pParser;
	std::string m_s_error;
	std::string m_s_warning;
	std::list<Prism*> m_save_prism_list;
	bool m_save_prisms;
};

