#ifdef SKIP_WPHAST
#include <windows.h>
#include "WParser.h"


extern int   max_line;
extern char  *line;
extern char  *line_save;
extern int   next_keyword;

extern void malloc_error(void);
extern int check_key (char *str);
extern int copy_token (char *token_ptr, char **ptr, int *length);

#include <cassert>   // assert macro
#include <iostream>  // std::cerr std::cin std::cout
#include "srcinput/wphast.h"

int add_char_to_line(int *i, char c);

CWParser::CWParser(void)
: m_output_stream(std::cout)
, m_error_stream(std::cerr)
{
}

CWParser::CWParser(std::istream& input)
: m_output_stream(std::cout)
, m_error_stream(std::cerr)

{
	this->push_istream(&input, false);
}

CWParser::~CWParser(void)
{
}

PHRQ_io::LINE_TYPE CWParser::get_logical_line(int *l)
{
/*
 *   Reads file fp until end of line, ";", or eof
 *   stores characters in line_save
 *   reallocs line_save and line if more space is needed
 *
 *   returns:
 *           EOF on empty line on end of file or
 *           OK otherwise
 *           *l returns length of line
 */
	int i, j;
	int pos;
	char c;
	i = 0;
	while ((j = this->m_input_stream.get()) != EOF)
	{
		c = (char) j;
		if (c == '#')
		{
			/* ignore all chars after # until newline */
			do
			{
				c = (char) j;
				if (c == '\n')
				{
					break;
				}
				add_char_to_line (&i, c);
			}
			while ((j = this->m_input_stream.get()) != EOF);
		}
		if (c == ';')
			break;
		if (c == '\n')
		{
			break;
		}
		if (c == '\\')
		{
			pos = i;
			add_char_to_line (&i, c);
			while ((j = this->m_input_stream.get()) != EOF)
			{
				c = (char) j;
				if (c == '\\')
				{
					pos = i;
					add_char_to_line (&i, c);
					continue;
				}
				if (c == '\n')
				{
					/* remove '\\' */
					for (; pos < i; pos++)
					{
						line_save[pos] = line_save[pos + 1];
					}
					i--;
					break;
				}
				add_char_to_line (&i, c);
				if (!isspace (j))
					break;
			}
		}
		else
		{
			add_char_to_line (&i, c);
		}
	}
	if (j == EOF && i == 0)
	{
		*l = 0;
		line_save[i] = '\0';
		return (PHRQ_io::LT_EOF);
	}
	line_save[i] = '\0';
	*l = i;
	return (PHRQ_io::LT_OK);
}

/* ---------------------------------------------------------------------- */
int
add_char_to_line (int *i, char c)
/* ---------------------------------------------------------------------- */
{
  if (*i + 20 >= max_line)
  {
    max_line *= 2;
    line_save =
      (char *) realloc (line_save, (size_t) max_line * sizeof (char));
    if (line_save == NULL)
      malloc_error ();
    line = (char *) realloc (line, (size_t) max_line * sizeof (char));
    if (line == NULL)
      malloc_error ();
  }
  line_save[*i] = c;
  *i += 1;
  return (OK);
}

/* ---------------------------------------------------------------------- */
PHRQ_io::LINE_TYPE CWParser::get_line(void)
/* ---------------------------------------------------------------------- */
{
/*
 *   Read a line from input file put in "line".
 *   Copy of input line is stored in "line_save".
 *   Characters after # are discarded in line but retained in "line_save"
 *
 *   Arguments:
 *      fp is file name
 *   Returns:
 *      EMPTY,
 *      EOF,
 *      KEYWORD,
 *      OK,
 *      OPTION
 */
 	int i, j, empty, l;
	PHRQ_io::LINE_TYPE return_value;
	char *ptr;
	char token[MAX_LENGTH];

	return_value = PHRQ_io::LT_EMPTY;
	while (return_value == PHRQ_io::LT_EMPTY) {
/*
 *   Eliminate all characters after # sign as a comment
 */
		i=-1;
		j=0;
		empty=TRUE;
/*
 *   Get line, check for eof
 */
		if (this->get_logical_line(&l) == EOF) {
			next_keyword=0;
			return (PHRQ_io::LT_EOF);
		}
/*
 *   Get long lines
 */
		j = l;
		ptr = strchr (line_save, '#');
		if (ptr != NULL) {
			j = (int)(ptr - line_save);
		}
		strncpy(line, line_save, (unsigned) j);
		line[j] = '\0';
		for (i = 0; i < j; i++) {
			if (! isspace((int)line[i]) ) {
				empty = FALSE;
				break;
			}
		}
/*
 *   New line character encountered
 */

		if (empty == TRUE) {
			return_value=PHRQ_io::LT_EMPTY;
		} else {
			return_value=PHRQ_io::LT_OK;
		}
	}
/*
 *   Determine return_value
 */
	if (return_value == PHRQ_io::LT_OK) {
		if ( ::check_key(line) == TRUE) {
			return_value=PHRQ_io::LT_KEYWORD;
		} else {
			ptr = line;
			copy_token(token, &ptr, &i);
			if (token[0] == '-' && isalpha((int)token[1])) {
				return_value = PHRQ_io::LT_OPTION;
			}
		}
	}
	return (return_value);
}

// COMMENT: {2/25/2005 2:12:55 PM}/* ---------------------------------------------------------------------- */
// COMMENT: {2/25/2005 2:12:55 PM}int CWParser::error_msg (const char *err_str, const int stop)
// COMMENT: {2/25/2005 2:12:55 PM}/* ---------------------------------------------------------------------- */
// COMMENT: {2/25/2005 2:12:55 PM}{
// COMMENT: {2/25/2005 2:12:55 PM}	this->m_errors += "ERROR: ";
// COMMENT: {2/25/2005 2:12:55 PM}	this->m_errors += err_str;
// COMMENT: {2/25/2005 2:12:55 PM}	this->m_errors += "\n";
// COMMENT: {2/25/2005 2:12:55 PM}	if (stop == STOP) {
// COMMENT: {2/25/2005 2:12:55 PM}		this->m_errors += "Stopping.\n";
// COMMENT: {2/25/2005 2:12:55 PM}		::RaiseException(INPUT_CONTAINS_ERRORS, 0, 0, NULL);
// COMMENT: {2/25/2005 2:12:55 PM}	}
// COMMENT: {2/25/2005 2:12:55 PM}	return OK;
// COMMENT: {2/25/2005 2:12:55 PM}}

// COMMENT: {2/25/2005 2:13:00 PM}LPCTSTR CWParser::GetErrorMsg(void)
// COMMENT: {2/25/2005 2:13:00 PM}{
// COMMENT: {2/25/2005 2:13:00 PM}	return this->m_errors.c_str();
// COMMENT: {2/25/2005 2:13:00 PM}}

#endif /* SKIP_WPHAST */