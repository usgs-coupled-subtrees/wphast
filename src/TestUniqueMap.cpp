#include "StdAfx.h"
#include "TestUniqueMap.h"

#include "srcinput/UniqueMap.h"

#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

std::map<size_t, int> s_data_map;
std::map<int, size_t> s_inverse_map;

void TestUniqueMap::testPushBack(void)
{
	std::vector<int> ivector;
	ivector.push_back(1);

	UniqueMap<int> imap;
	//imap.push_back(1);
	CPPUNIT_ASSERT(imap.push_back(1) == 0);
	CPPUNIT_ASSERT(imap.push_back(1) == 0);
	CPPUNIT_ASSERT(imap.push_back(1) == 0);
	CPPUNIT_ASSERT(imap.push_back(1) == 0);

	CPPUNIT_ASSERT(imap.at(0) == 1);
}

void TestUniqueMap::testPushBackPointers(void)
{
	UniqueMap<int*> imap;
	CPPUNIT_ASSERT(imap.size() == 0);

	int* i = new int(3);

	CPPUNIT_ASSERT(imap.push_back(i) == 0);		
	CPPUNIT_ASSERT(imap.size() == 1);

	CPPUNIT_ASSERT(imap.push_back(i) == 0);		
	CPPUNIT_ASSERT(imap.size() == 1);

	CPPUNIT_ASSERT(imap.push_back(i) == 0);		
	CPPUNIT_ASSERT(imap.size() == 1);

	CPPUNIT_ASSERT(imap.push_back(i) == 0);		
	CPPUNIT_ASSERT(imap.size() == 1);

	CPPUNIT_ASSERT(imap.push_back(i) == 0);		
	CPPUNIT_ASSERT(imap.size() == 1);

	CPPUNIT_ASSERT(imap.push_back(i) == 0);		
	CPPUNIT_ASSERT(imap.size() == 1);

	CPPUNIT_ASSERT(imap.find(0) != imap.end());
	CPPUNIT_ASSERT(*(*imap.find(0)) == 3);

	UniqueMap<int*>::iterator iter = imap.begin();
	for (; iter != imap.end(); ++iter)
	{
		CPPUNIT_ASSERT(*(*iter) == 3);
		delete (*iter);
	}
	imap.clear();	
	CPPUNIT_ASSERT(imap.size() == 0);
}

void TestUniqueMap::testErase(void)
{
	UniqueMap<int> imap;
	CPPUNIT_ASSERT(imap.size() == 0);
	
	CPPUNIT_ASSERT(imap.push_back(6) == 0);
	CPPUNIT_ASSERT(imap.size() == 1);
	imap.erase(0);
	CPPUNIT_ASSERT(imap.size() == 0);
}

void TestUniqueMap::testReplace(void)
{
	UniqueMap<int> imap;
	CPPUNIT_ASSERT(imap.size() == 0);
	
	CPPUNIT_ASSERT(imap.push_back(6) == 0);
	CPPUNIT_ASSERT(imap.size() == 1);
	CPPUNIT_ASSERT(imap.at(0) == 6);

	imap.replace(0, 5);
	CPPUNIT_ASSERT(imap.size() == 1);
	CPPUNIT_ASSERT(imap.at(0) == 5);
}
