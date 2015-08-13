#pragma once

#include <windows.h>

LONG ExceptionFilter(DWORD dwExceptionCode);
DWORD GetExceptionCode(VOID);

// see winerror.h
#define MAKESOFTWAREEXCEPTION(Severity, Facility, Exception) \
	((DWORD) ( \
	/* Severity code */      (Severity       ) |   \
	/* MS(0) or Cust(1) */   (1         << 29) |   \
	/* Reserved(0) */        (0         << 28) |   \
	/* Facility code */      (Facility  << 16) |   \
	/* Exception code */     (Exception <<  0)))

#define INPUT_CONTAINS_ERRORS MAKESOFTWAREEXCEPTION(ERROR_SEVERITY_ERROR, FACILITY_NULL, 2)

#include <iosfwd>
#include <string>

// COMMENT: {8/12/2015 6:39:26 PM}#define KEYWORD 3
#define TRUE 1
#define FALSE 0
#define OK 1
#define MAX_LENGTH 256
// COMMENT: {8/12/2015 6:39:32 PM}#define EMPTY 2
// COMMENT: {8/12/2015 6:39:37 PM}#define OPTION 8
#define STOP 1
#define ERROR 0
#define CONTINUE 0

#define OPTION_EOF -1
#define OPTION_KEYWORD -2
#define OPTION_ERROR -3
#define OPTION_DEFAULT -4
#define OPTION_DEFAULT2 -5

// COMMENT: {8/12/2015 10:58:31 PM}class CWParser : public PHRQ_io
// COMMENT: {8/12/2015 10:58:31 PM}{
// COMMENT: {8/12/2015 10:58:31 PM}public:
// COMMENT: {8/12/2015 10:58:31 PM}	CWParser(void);
// COMMENT: {8/12/2015 10:58:31 PM}	CWParser(std::istream& input);
// COMMENT: {8/12/2015 10:58:31 PM}	virtual ~CWParser(void);
// COMMENT: {8/12/2015 10:58:31 PM}
// COMMENT: {8/12/2015 10:58:31 PM}// COMMENT: {2/25/2005 2:11:44 PM}	LPCTSTR GetErrorMsg(void);
// COMMENT: {8/12/2015 10:58:31 PM}
// COMMENT: {8/12/2015 10:58:31 PM}	// phastinput overrides
// COMMENT: {8/12/2015 10:58:31 PM}// COMMENT: {8/12/2015 6:34:16 PM}	int get_logical_line(int *l);
// COMMENT: {8/12/2015 10:58:31 PM}// COMMENT: {8/12/2015 6:34:16 PM}	int get_line(void);
// COMMENT: {8/12/2015 10:58:31 PM}// COMMENT: {8/12/2015 6:49:56 PM}	PHRQ_io::LINE_TYPE get_logical_line(int *l);
// COMMENT: {8/12/2015 10:58:31 PM}// COMMENT: {8/12/2015 6:49:56 PM}	PHRQ_io::LINE_TYPE get_line(void);
// COMMENT: {8/12/2015 10:58:31 PM}// COMMENT: {2/25/2005 2:11:51 PM}	int error_msg (const char *err_str, const int stop);
// COMMENT: {8/12/2015 10:58:31 PM}
// COMMENT: {8/12/2015 10:58:31 PM}
// COMMENT: {8/12/2015 10:58:31 PM}protected:
// COMMENT: {8/12/2015 10:58:31 PM}// COMMENT: {8/12/2015 9:27:26 PM}	std::istream& m_input_stream;
// COMMENT: {8/12/2015 10:58:31 PM}	std::ostream& m_output_stream;
// COMMENT: {8/12/2015 10:58:31 PM}	std::ostream& m_error_stream;
// COMMENT: {8/12/2015 10:58:31 PM}
// COMMENT: {8/12/2015 10:58:31 PM}	std::string   m_errors;
// COMMENT: {8/12/2015 10:58:31 PM}};
