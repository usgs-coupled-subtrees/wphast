#include "StdAfx.h"
#include "TestCParser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void TestCWParser::test(void)
{
	std::string str("END");
	std::istringstream iss(str);
	CWParser* pParser = new CWParser(iss);
	CPPUNIT_ASSERT(pParser != NULL);
	delete pParser;
}
