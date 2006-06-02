#include "StdAfx.h"
#include "TestCParser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void TestCParser::test(void)
{
	std::string str("END");
	std::istringstream iss(str);
	CParser* pParser = new CParser(iss);
	CPPUNIT_ASSERT(pParser != NULL);
	delete pParser;
}
