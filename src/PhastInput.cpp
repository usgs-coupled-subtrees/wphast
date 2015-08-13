#include "stdafx.h"
#include "PhastInput.h"

#include <cassert> // assert macro

// COMMENT: {12/3/2008 3:09:21 PM}#if defined(__WPHAST__) && !defined(_DEBUG)
// COMMENT: {12/3/2008 3:09:21 PM}#include "phqalloc.h"
// COMMENT: {12/3/2008 3:09:21 PM}#else
// COMMENT: {12/3/2008 3:09:21 PM}#ifdef _DEBUG
// COMMENT: {12/3/2008 3:09:21 PM}#define _CRTDBG_MAP_ALLOC
// COMMENT: {12/3/2008 3:09:21 PM}#include <crtdbg.h>
// COMMENT: {12/3/2008 3:09:21 PM}#endif
// COMMENT: {12/3/2008 3:09:21 PM}#endif
#include "srcinput/message.h"
#include "srcinput/Prism.h"
#include "SeException.h"
#include "srcinput/wphast.h"  /* must be last */

extern PHRQ_io input_phrq_io;

extern char  *prefix, *transport_name, *chemistry_name, *database_name;
extern int   input_error;
extern int   simulation;
extern int   count_warnings;

extern FILE  *input_file;
extern FILE  *database_file;
extern FILE  *log_file;
extern FILE  *echo_file;
extern FILE  *std_error;
extern FILE  *error_log;
extern FILE  *hst_file;
extern FILE  *transport_file;
extern FILE  *chemistry_file;

extern int clean_up(void);
extern void initialize(void);

extern int load(bool bWritePhastTmp);
extern int read(void);
extern int accumulate(bool bWritePhastTmp);

extern int load_first_sim(bool bWritePhastTmp);
extern int load_next_sim(bool bWritePhastTmp);
extern int phast_input(bool bWritePhastTmp);

extern int write_hst(void);
char * string_duplicate (const char *token);

CPhastInput* CPhastInput::s_instance = 0;

static _se_translator_function prev_se_translator_function = 0;

CPhastInput::CPhastInput(PHRQ_io& io, const char *szPrefix, const char *szDatabase, bool save_prisms)
: m_io(io)
, m_prefix(szPrefix)
, m_database(szDatabase)
, m_save_prisms(save_prisms)
{
	prev_se_translator_function = _set_se_translator(CSeException::SeTranslator);	

	assert(this->m_save_prism_list.size() == 0);
	if (this->m_save_prisms)
	{
		this->m_save_prism_list.swap(Prism::prism_list);
		assert(Prism::prism_list.size() == 0);
	}
	this->DoInitialize();
}

CPhastInput::~CPhastInput(void)
{
	try
	{
		this->DoCleanUp();
	}
	catch (...)
	{
		if (this->m_save_prisms)
		{
			Prism::prism_list.swap(this->m_save_prism_list);
		}
		_set_se_translator(prev_se_translator_function);
		throw;
	}
	if (this->m_save_prisms)
	{
		Prism::prism_list.swap(this->m_save_prism_list);
	}
	_set_se_translator(prev_se_translator_function);
}

void CPhastInput::DoInitialize(void)
{
	::std_error = ::fopen("NUL", "w");
	::hst_file  = ::fopen("NUL", "w");

	::prefix         = NULL;
	::transport_name = NULL;
	::chemistry_name = NULL;
	::database_name  = NULL;

	::input_error    = 0;
	::count_warnings = 0;
	::simulation     = 0;

	::add_message_callback(::default_handler, NULL);
	::add_message_callback(CPhastInput::Output, this);

	::initialize();

	::prefix = ::string_duplicate(this->m_prefix.c_str());
	if (!this->m_database.empty())
	{
		::database_name = ::string_duplicate(this->m_database.c_str());
	}
}

void CPhastInput::DoCleanUp(void)
{
	extern void Clear_NNInterpolatorList(void);
	extern void Clear_file_data_map(void);
	extern void Clear_KDtreeList(void);

	::clean_up();
	::clean_up_message();

// COMMENT: {9/1/2009 8:21:30 PM}	// clean up shapefile/prism
// COMMENT: {9/1/2009 8:21:30 PM}	Clear_NNInterpolatorList();
// COMMENT: {9/1/2009 8:21:30 PM}	Clear_file_data_map();
// COMMENT: {9/1/2009 8:21:30 PM}	Clear_KDtreeList();

// COMMENT: {12/2/2008 3:00:12 PM}#if !defined(_DEBUG)
// COMMENT: {12/2/2008 3:00:12 PM}	::PHRQ_free_all();
// COMMENT: {12/2/2008 3:00:12 PM}#endif
}

CPhastInput* CPhastInput::New(std::istream& is, const std::string sPrefix, const std::string sDatabase, bool save_prisms /*= true*/)
{
	if (CPhastInput::s_instance)
	{
		assert(false);  // only one instance allowed
		return 0;
	}
	
	input_phrq_io.push_istream(&is, false);
	CPhastInput::s_instance = new CPhastInput(input_phrq_io, sPrefix.c_str(), sDatabase.c_str(), save_prisms);
	return CPhastInput::s_instance;
}

void CPhastInput::Delete(void)
{
	if (CPhastInput::s_instance)
	{
		delete CPhastInput::s_instance;
		CPhastInput::s_instance = 0;
	}
}

void CPhastInput::Run(void)
{
	::phast_input(false);
}

/*
Load split into two routines
Read and Accumulate
*/
void CPhastInput::Load(void)
{
	char name[MAX_LENGTH];
	assert(::chemistry_name == NULL);
	if (::chemistry_name == NULL)
	{
		::strcpy(name, prefix);
		::strcat(name, ".chem.dat");
		::chemistry_name = ::string_duplicate(name);
	}
	if (::database_name == NULL)
	{
		::database_name = ::string_duplicate("phast.dat");
	}
	::load(false);
}

/*
Load split into two routines
Read and Accumulate
*/
void CPhastInput::Read(void)
{
	char name[MAX_LENGTH];
	assert(::chemistry_name == NULL);
	if (::chemistry_name == NULL)
	{
		::strcpy(name, prefix);
		::strcat(name, ".chem.dat");
		::chemistry_name = ::string_duplicate(name);
	}
	if (::database_name == NULL)
	{
		::database_name = ::string_duplicate("phast.dat");
	}
	::read();
}

/*
Load split into two routines
Read and Accumulate
*/
void CPhastInput::Accumulate(bool bWritePhastTmp)
{
	::accumulate(false);
}

void CPhastInput::WritePhastTmp(const char* szPhastTmp)
{
	char name[MAX_LENGTH];

	assert(::chemistry_name == NULL);
	if (::chemistry_name == NULL)
	{
		::strcpy(name, prefix);
		::strcat(name, ".chem.dat");
		::chemistry_name = ::string_duplicate(name);
	}
	if (::database_name == NULL)
	{
		::database_name = ::string_duplicate("phast.dat");
	}

	::fclose(::hst_file);
	if ((::hst_file = ::fopen(szPhastTmp, "w")) == NULL)
	{
		return;
	}
	::load(true);
}

long ExceptionFilter(unsigned long dwExceptionCode)
{
	long lReturn;
	char buffer[200];

	switch (dwExceptionCode)
	{
	case INPUT_CONTAINS_ERRORS :
		lReturn = EXCEPTION_EXECUTE_HANDLER;
		break;
	default :
		// a hardware exception has occurred
		switch (dwExceptionCode)
		{
		case EXCEPTION_ACCESS_VIOLATION :
#ifdef _DEBUG
			return EXCEPTION_CONTINUE_SEARCH;
#endif
			// handle exception probably a NULL pointer error
			++::input_error;
			::sprintf(buffer, "Handled exception (%u):EXCEPTION_ACCESS_VIOLATION.\nStopping\n", dwExceptionCode);
			::error_msg(buffer, CONTINUE);
			lReturn = EXCEPTION_EXECUTE_HANDLER;
			break;
		default :
			++input_error;
			::sprintf(buffer, "Unhandled exception (%u).\nStopping\n", dwExceptionCode);
			::error_msg(buffer, CONTINUE);
			// let operating system handle error
			lReturn = EXCEPTION_CONTINUE_SEARCH;
		}
	}
	return lReturn;
}

LPCTSTR CPhastInput::GetErrorMsg(void)const
{
	return this->m_s_error.c_str();;
}

int CPhastInput::GetErrorCount(void)const
{
	return ::input_error;
}

LPCTSTR CPhastInput::GetWarningMsg(void)const
{
	return this->m_s_warning.c_str();;
}

int CPhastInput::GetWarningCount(void)const
{
	return ::count_warnings;
}

int CPhastInput::Output(const int type, const char *err_str, const int stop, void *cookie, const char *format, va_list args)
{
	assert(cookie);
	CPhastInput* pThis = (CPhastInput*) cookie;

	if (pThis)
	{
		switch(type)
		{
		case OUTPUT_ERROR:
			pThis->m_s_error += "ERROR: ";
			pThis->m_s_error += err_str;
			pThis->m_s_error += "\n";
			break;
		case OUTPUT_WARNING:
			pThis->m_s_warning += "Warning: ";
			pThis->m_s_warning += err_str;
			pThis->m_s_warning += "\n";
			break;
		}
	}
	if (stop == STOP)
	{
		throw pThis->GetErrorCount();
	}
	return OK;
}
