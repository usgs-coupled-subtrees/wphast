#include "StdAfx.h"
#include "TestCZone.h"

#include "Zone.h"
#include "srcinput/Cube.h"

Cube *read_cube(char **next_char);


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void TestCZone::testCopyCtor(void)
{
	CZone z;
	z.zone_defined = TRUE;
	z.x1 = 1;
	z.x2 = 2;
	z.y1 = 3;
	z.y2 = 4;
	z.z1 = 5;
	z.z2 = 6;

	CZone Z(z);
	CPPUNIT_ASSERT(z.zone_defined == Z.zone_defined);
	CPPUNIT_ASSERT(z.x1 == Z.x1);
	CPPUNIT_ASSERT(z.x2 == Z.x2);
	CPPUNIT_ASSERT(z.y1 == Z.y1);
	CPPUNIT_ASSERT(z.y2 == Z.y2);
	CPPUNIT_ASSERT(z.z1 == Z.z1);
	CPPUNIT_ASSERT(z.z2 == Z.z2);
}

void TestCZone::testCopyCtorFromStruct(void)
{
	struct zone z;
	z.zone_defined = TRUE;
	z.x1 = 1;
	z.x2 = 2;
	z.y1 = 3;
	z.y2 = 4;
	z.z1 = 5;
	z.z2 = 6;

	CZone Z(z);
	CPPUNIT_ASSERT(z.zone_defined == Z.zone_defined);
	CPPUNIT_ASSERT(z.x1 == Z.x1);
	CPPUNIT_ASSERT(z.x2 == Z.x2);
	CPPUNIT_ASSERT(z.y1 == Z.y1);
	CPPUNIT_ASSERT(z.y2 == Z.y2);
	CPPUNIT_ASSERT(z.z1 == Z.z1);
	CPPUNIT_ASSERT(z.z2 == Z.z2);
}

void TestCZone::testOperatorEqual(void)
{
	struct zone z;
	z.zone_defined = TRUE;
	z.x1 = 1;
	z.x2 = 2;
	z.y1 = 3;
	z.y2 = 4;
	z.z1 = 5;
	z.z2 = 6;

	CZone Z;
	Z = z;
	CPPUNIT_ASSERT(Z == z);
	CPPUNIT_ASSERT(Z == Z);

	CPPUNIT_ASSERT(z.zone_defined == Z.zone_defined);
	CPPUNIT_ASSERT(z.x1 == Z.x1);
	CPPUNIT_ASSERT(z.x2 == Z.x2);
	CPPUNIT_ASSERT(z.y1 == Z.y1);
	CPPUNIT_ASSERT(z.y2 == Z.y2);
	CPPUNIT_ASSERT(z.z1 == Z.z1);
	CPPUNIT_ASSERT(z.z2 == Z.z2);
}

void TestCZone::testOperatorEqualsEquals(void)
{
	struct zone z;
	z.zone_defined = TRUE;
	z.x1 = 1;
	z.x2 = 2;
	z.y1 = 3;
	z.y2 = 4;
	z.z1 = 5;
	z.z2 = 6;

	CZone Z;
	Z.zone_defined = TRUE;
	Z.x1 = 1;
	Z.x2 = 2;
	Z.y1 = 3;
	Z.y2 = 4;
	Z.z1 = 5;
	Z.z2 = 6;

	CPPUNIT_ASSERT(Z == z);
	CPPUNIT_ASSERT(Z == Z);

	Z.zone_defined = FALSE;
	z.zone_defined = TRUE;
	CPPUNIT_ASSERT(!(Z == z));

	Z.zone_defined = UNDEFINED;
	z.zone_defined = TRUE;
	CPPUNIT_ASSERT(!(Z == z));

	Z.zone_defined = TRUE;
	z.zone_defined = FALSE;
	CPPUNIT_ASSERT(!(Z == z));

	Z.zone_defined = TRUE;
	z.zone_defined = UNDEFINED;
	CPPUNIT_ASSERT(!(Z == z));

	Z.zone_defined = TRUE;
	z.zone_defined = TRUE;
	z.z2 = z.z1;
	CPPUNIT_ASSERT(!(Z == z));
}

void TestCZone::testSerialize(void)
{
	const char FILENAME[] = "TestCZone.h5";
	char *input = "1 3 5 2 4 6";

	Cube* pCube = ::read_cube(&input);
	CPPUNIT_ASSERT(pCube != 0);

	struct zone z2;
	z2.zone_defined = TRUE;
	z2.x1 = 1;
	z2.x2 = 2;
	z2.y1 = 3;
	z2.y2 = 4;
	z2.z1 = 5;
	z2.z2 = 6;

    CZone Z1(*pCube->Get_box());
	CZone Z2(z2);
	CPPUNIT_ASSERT(Z1 == Z2);

	// Create the file.
    hid_t file = H5Fcreate(FILENAME, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
	CPPUNIT_ASSERT(file >= 0);
	Z1.Serialize(true, file);

	// Close file
	CPPUNIT_ASSERT(H5Fclose(file) >= 0);

	// Open file
	file = H5Fopen(FILENAME, H5F_ACC_RDONLY, H5P_DEFAULT);

	CZone Z3;
	CPPUNIT_ASSERT(!(Z3 == Z1));
	Z3.Serialize(false, file);
	CPPUNIT_ASSERT(Z3 == Z1);

	CPPUNIT_ASSERT(H5Fclose(file) >= 0);
	CPPUNIT_ASSERT(::remove(FILENAME) == 0);

	delete pCube;
}
