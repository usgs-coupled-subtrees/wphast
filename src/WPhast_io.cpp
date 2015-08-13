#include "StdAfx.h"
#include "WPhast_io.h"

#include "Utils.h"
#include "Parser.h"


WPhast_io::WPhast_io(std::set<CString> &files)
: ufiles(files)
{
}

WPhast_io::~WPhast_io(void)
{
}

/* ---------------------------------------------------------------------- */
WPhast_io::LINE_TYPE WPhast_io::
get_line(void)
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
	int i;
	bool empty;
	std::string stdtoken;
	bool continue_loop = true;;

	PHRQ_io::LINE_TYPE return_value;
	// loop for include files
	for (;;)
	{
		if (this->get_istream() == NULL)
		{
			break;
		}
		return_value = LT_EMPTY;
		while (return_value == LT_EMPTY)
		{
			/*
			*   Eliminate all characters after # sign as a comment
			*/
			i = -1;
			empty = true;
			/*
			*   Get line, check for eof
			*/
			continue_loop = false;

			if (get_logical_line() == LT_EOF)
			{
				//pop next file
				this->pop_istream();
				continue_loop = true;
				break;
			}
			/*
			*   Get long lines
			*/
			bool empty = true;
			m_line = m_line_save.substr(0, m_line_save.find_first_of('#'));
			for (unsigned int i = 0; i < m_line.size(); ++i)
			{
				if (!::isspace(m_line[i]))
				{
					empty = false;
					break;
				}
			}

			if (this->accumulate)
			{
				this->accumulated.append(m_line_save);
				this->accumulated.append("\n");
			}
			//
			// New line character encountered
			//
			return_value = (empty ? LT_EMPTY : LT_OK);

		}
		if (continue_loop) continue;
		//
		// Determine return_value
		//
		if (return_value == LT_OK)
		{
			if (check_key(m_line.begin(), m_line.end()))
			{
				return_value = LT_KEYWORD;
			}
			else
			{
				std::string::iterator beg = m_line.begin();
				std::string::iterator end = m_line.end();
				std::string token;
				CParser::copy_token(token, beg, end);

				if (token.size() > 1 && token[0] == '-' &&::isalpha(token[1]))
				{
					return_value = LT_OPTION;
				}
			}
		}

		// add new include file to stack
		std::string::iterator beg = m_line.begin();
		std::string::iterator end = m_line.end();
		CParser::copy_token(stdtoken, beg, end);
		Utilities::str_tolower(stdtoken);
		if ((strstr(stdtoken.c_str(),"include$") == stdtoken.c_str()) ||
			(strstr(stdtoken.c_str(),"include_file") == stdtoken.c_str()))
		{
			std::string file_name;
			file_name.assign(beg, end);
			file_name = trim(file_name);

			if (file_name.size() > 0)
			{
				std::ifstream *next_stream = new std::ifstream(file_name.c_str(), std::ios_base::in);
				if (!next_stream->is_open())
				{
					std::ostringstream errstr;
					errstr << "\n***********  Could not open include file " << file_name << ". ***********\n\n";
					delete next_stream;
#if defined(__WPHAST__)
					warning_msg(errstr.str().c_str());
					continue;
#else
					output_msg(errstr.str().c_str());
					error_msg(errstr.str().c_str(), OT_STOP);
#endif
				}
#if defined(__WPHAST__)
				this->ufiles.insert(file_name.c_str());
#endif
				this->push_istream(next_stream);
				continue;
			}
		}
		return return_value;
	}
	m_next_keyword = Keywords::KEY_END;
	return LT_EOF;
}
