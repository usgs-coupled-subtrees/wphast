#include "StdAfx.h"
#include "TestPrism.h"

#include <sstream>

extern int input_error;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void TestPrism::testOStream(void)
{
	const char input[] = //"\t-prism"
		"\t\t-top constant 80"
		"\t\t-bottom constant -300"
		"\t\t-perimeter points"
		"\t\t\t278300 824600 80"
		"\t\t\t280600 824600 80"
		"\t\t\t279600 819600 80";
	::input_error = 0;
	std::istringstream iss(input);

	Prism aprism;
	while(aprism.Read(iss) && iss);

	std::ostringstream oss;
	oss << aprism;
	TRACE(oss.str().c_str());
	CPPUNIT_ASSERT(::input_error == 0);
}