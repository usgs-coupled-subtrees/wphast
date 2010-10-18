#include "StdAfx.h"
#include "TestPHAST_Transform.h"

#include "srcinput/PHAST_Transform.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void TestPHAST_Transform::test(void)
{
	double ox = 4.;
	double oy = 3.;
	double oz = 0.;

	// 3 : 4 : 5
	// 36.869897645844021296855612559093
	// 53.130102354155978703144387440907
	double angle_degrees = 36.869897645844021296855612559093;
	double scale_x = 1.;
	double scale_y = 1.;
	double scale_z = 1.;

	PHAST_Transform map_to_grid(ox, oy, oz, angle_degrees, scale_x, scale_y, scale_z);

	double p1x = 5.;
	double p1y = 5.;
	double p1z = 0.;
	Point p1(p1x, p1y, p1z);

	map_to_grid.Transform(p1);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(p1.x(), 2.0, ::pow((double)10, (double)-DBL_DIG));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(p1.y(), 1.0, ::pow((double)10, (double)-DBL_DIG));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(p1.z(), 0.0, ::pow((double)10, (double)-DBL_DIG));

	map_to_grid.Inverse_transform(p1);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(p1.x(), p1x, ::pow((double)10, (double)-DBL_DIG));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(p1.y(), p1y, ::pow((double)10, (double)-DBL_DIG));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(p1.z(), p1z, ::pow((double)10, (double)-DBL_DIG));

	double p2x = 6.;
	double p2y = 7.;
	double p2z = 0.;
	Point p2(p2x, p2y, p2z);

	map_to_grid.Transform(p2);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(p2.x(), 4.0, ::pow((double)10, (double)-DBL_DIG));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(p2.y(), 2.0, ::pow((double)10, (double)-DBL_DIG));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(p2.z(), 0.0, ::pow((double)10, (double)-DBL_DIG));

	map_to_grid.Inverse_transform(p2);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(p2.x(), p2x, ::pow((double)10, (double)-DBL_DIG));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(p2.y(), p2y, ::pow((double)10, (double)-DBL_DIG));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(p2.z(), p2z, ::pow((double)10, (double)-DBL_DIG));
}
