#include "PhastInput.h"

#include <cassert> // assert macro

extern "C"
{
#include "phqalloc.h"  // PHRQ_free_all
#include "srcinput/message.h"

extern char  *prefix, *transport_name, *chemistry_name, *database_name;
extern int   input_error;
extern int   simulation;
extern int   count_warnings;

extern FILE  *input_file;
extern FILE  *input;                        
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
extern int load_first_sim(bool bWritePhastTmp);
extern int load_next_sim(bool bWritePhastTmp);
extern int phast_input(bool bWritePhastTmp);

extern int write_hst(void);
char * string_duplicate (const char *token);
}

CPhastInput* CPhastInput::s_instance = 0;

CPhastInput::CPhastInput(std::istream& is, const char *szPrefix)
: m_parser(is)
, m_prefix(szPrefix)
{
	this->DoInitialize();
}

CPhastInput::~CPhastInput(void)
{
	this->DoCleanUp();
}

void CPhastInput::DoInitialize(void)
{
	__try
	{
		::std_error = ::fopen("NUL", "w");
		::input     = ::fopen("NUL", "w");
		::echo_file = ::fopen("NUL", "w");
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
	}
	__except ( ::ExceptionFilter(::GetExceptionCode()) )
	{
		// An error occurred
		assert(false);
	}
}

void CPhastInput::DoCleanUp(void)
{
	__try
	{
		::clean_up();
		::clean_up_message();
#if !defined(SHOW_MEM_LEAKS)
		::PHRQ_free_all();
#endif
	}
	__except ( ::ExceptionFilter(::GetExceptionCode()) )
	{
		// An error occurred
		assert(false);
	}
}

CPhastInput* CPhastInput::New(std::istream& is, const char *pcsz_prefix)
{
	if (CPhastInput::s_instance)
	{
		assert(false);  // only one instance allowed
		return 0;
	}

	CPhastInput::s_instance = new CPhastInput(is, pcsz_prefix);
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
	__try
	{
		::phast_input(false);
	}
	__except ( ::ExceptionFilter(::GetExceptionCode()) )
	{
		/* An error occurred */
	}

}

void CPhastInput::Load(void)
{
	char name[MAX_LENGTH];
	__try
	{
		assert(::chemistry_name == NULL);
		if (::chemistry_name == NULL)
		{
			::strcpy(name, prefix);
			::strcat(name, ".chem.dat");
			::chemistry_name = ::string_duplicate(name);
		}
		assert(database_name == NULL);
		if (::database_name == NULL)
		{
			::database_name = ::string_duplicate("phast.dat");
		}
		::load(false);
	}
	__except ( ::ExceptionFilter(::GetExceptionCode()) )
	{
		// An error occurred
		assert(::input_error > 0);
	}
}

// COMMENT: {3/31/2005 6:21:12 PM}void CPhastInput::LoadFirstStressPeriod(void)
// COMMENT: {3/31/2005 6:21:12 PM}{
// COMMENT: {3/31/2005 6:21:12 PM}	char name[MAX_LENGTH];
// COMMENT: {3/31/2005 6:21:12 PM}
// COMMENT: {3/31/2005 6:21:12 PM}	__try
// COMMENT: {3/31/2005 6:21:12 PM}	{
// COMMENT: {3/31/2005 6:21:12 PM}		assert(::chemistry_name == NULL);
// COMMENT: {3/31/2005 6:21:12 PM}		if (::chemistry_name == NULL)
// COMMENT: {3/31/2005 6:21:12 PM}		{
// COMMENT: {3/31/2005 6:21:12 PM}			::strcpy(name, prefix);
// COMMENT: {3/31/2005 6:21:12 PM}			::strcat(name, ".chem.dat");
// COMMENT: {3/31/2005 6:21:12 PM}			::chemistry_name = ::string_duplicate(name);
// COMMENT: {3/31/2005 6:21:12 PM}		}
// COMMENT: {3/31/2005 6:21:12 PM}		assert(database_name == NULL);
// COMMENT: {3/31/2005 6:21:12 PM}		if (::database_name == NULL)
// COMMENT: {3/31/2005 6:21:12 PM}		{
// COMMENT: {3/31/2005 6:21:12 PM}			::database_name = ::string_duplicate("phast.dat");
// COMMENT: {3/31/2005 6:21:12 PM}		}
// COMMENT: {3/31/2005 6:21:12 PM}		::load_first_sim(false);
// COMMENT: {3/31/2005 6:21:12 PM}	}
// COMMENT: {3/31/2005 6:21:12 PM}	__except ( ::ExceptionFilter(::GetExceptionCode()) )
// COMMENT: {3/31/2005 6:21:12 PM}	{
// COMMENT: {3/31/2005 6:21:12 PM}		// An error occurred
// COMMENT: {3/31/2005 6:21:12 PM}		assert(::input_error > 0);
// COMMENT: {3/31/2005 6:21:12 PM}	}
// COMMENT: {3/31/2005 6:21:12 PM}
// COMMENT: {3/31/2005 6:21:12 PM}}

// COMMENT: {3/31/2005 6:21:18 PM}bool CPhastInput::LoadNextStressPeriod(void)
// COMMENT: {3/31/2005 6:21:18 PM}{
// COMMENT: {3/31/2005 6:21:18 PM}	int n;
// COMMENT: {3/31/2005 6:21:18 PM}	__try
// COMMENT: {3/31/2005 6:21:18 PM}	{
// COMMENT: {3/31/2005 6:21:18 PM}		assert(::chemistry_name != NULL);
// COMMENT: {3/31/2005 6:21:18 PM}		assert(::database_name != NULL);
// COMMENT: {3/31/2005 6:21:18 PM}		n = ::load_next_sim(false);
// COMMENT: {3/31/2005 6:21:18 PM}	}
// COMMENT: {3/31/2005 6:21:18 PM}	__except ( ::ExceptionFilter(::GetExceptionCode()) )
// COMMENT: {3/31/2005 6:21:18 PM}	{
// COMMENT: {3/31/2005 6:21:18 PM}		// An error occurred
// COMMENT: {3/31/2005 6:21:18 PM}		assert(::input_error > 0);
// COMMENT: {3/31/2005 6:21:18 PM}		n = 0;
// COMMENT: {3/31/2005 6:21:18 PM}	}
// COMMENT: {3/31/2005 6:21:18 PM}	if (n == EOF) return false;
// COMMENT: {3/31/2005 6:21:18 PM}	return true;
// COMMENT: {3/31/2005 6:21:18 PM}}

void CPhastInput::WritePhastTmp(const char* szPhastTmp)
{
	char name[MAX_LENGTH];

	__try
	{
		assert(::chemistry_name == NULL);
		if (::chemistry_name == NULL)
		{
			::strcpy(name, prefix);
			::strcat(name, ".chem.dat");
			::chemistry_name = ::string_duplicate(name);
		}
		assert(::database_name == NULL);
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
// COMMENT: {3/29/2005 1:38:59 PM}		::load_first_sim(true);
// COMMENT: {3/29/2005 1:38:59 PM}		while(true)
// COMMENT: {3/29/2005 1:38:59 PM}		{
// COMMENT: {3/29/2005 1:38:59 PM}			if (::load_next_sim(true) == EOF) break;
// COMMENT: {3/29/2005 1:38:59 PM}		}
	}
	__except ( ::ExceptionFilter(::GetExceptionCode()) )
	{
		/* An error occurred */
	}
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
		// a hardware exception has occured
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
			assert(args == NULL);
			pThis->m_s_error += "ERROR: ";
			pThis->m_s_error += err_str;
			pThis->m_s_error += "\n";
			break;
		case OUTPUT_WARNING:
			assert(args == NULL);
			pThis->m_s_warning += "Warning: ";
			pThis->m_s_warning += err_str;
			pThis->m_s_warning += "\n";
			break;			
		}
	}
	if (stop == STOP)
	{
		::RaiseException(INPUT_CONTAINS_ERRORS, 0, 0, NULL);
	}
	return OK;
}
