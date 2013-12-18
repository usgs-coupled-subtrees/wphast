#include "StdAfx.h"
#include "TestPrism.h"

#include <sstream>
#include "WPhastDoc.h"
#include "Global.h"
#include "srcinput/NNInterpolator/NNInterpolator.h"
#include "srcinput/Filedata.h"
#include "FakeFiledata.h"
extern int input_error;

#include <vtkDebugLeaks.h>
#include <vtkAssemblyPaths.h>
#include <vtkAssemblyPath.h>
#include <vtkAssemblyNode.h>
#include <vtkCellTypes.h>
#include <vtkIntArray.h>
#include <vtkCellLinks.h>
#include <vtkMergePoints.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkPropAssembly.h>
#include <vtkTransform.h>
#include <vtkMatrix4x4.h>
#include <vtkAxes.h>
#include <vtkGeometryFilter.h>
#include <vtkFeatureEdges.h>
#include <vtkPlaneSource.h>
#include <vtkLODActor.h>
#include <vtkStructuredGrid.h>
#include <vtkVertex.h>
#include <vtkLine.h>
#include <vtkQuad.h>
#include <vtkHexahedron.h>
#include <vtkPlane.h>
#include <vtkImageData.h>
#include <vtkPixel.h>
#include <vtkVoxel.h>
#include <vtkOutlineFilter.h>
#include <vtkOutlineSource.h>
#include <vtkCutter.h>
#include <vtkContourValues.h>
#include <vtkConeSource.h>

#include "AxesActor.h"
#include "MediaZoneActor.h"
#include "srcWedgeSource.h"
#include "ICHeadZoneActor.h"
#include "ICChemZoneActor.h"
#include "BCZoneActor.h"
#include "WellActor.h"
#include "RiverActor.h"
#include "GridActor.h"
#include "GridLineWidget.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void TestPrism::setUp(void)
{
}

void TestPrism::testOStream(void)
{
	const char input[] = //"\t-prism"
		"\t\t-top       constant MAP 80\n"
		"\t\t-bottom    constant MAP -300\n"
		"\t\t-perimeter points   MAP\n"
		"\t\t\t278300 824600 80\n"
		"\t\t\t280600 824600 80\n"
		"\t\t\t279600 819600 80\n";

	::input_error = 0;
	std::istringstream iss(input);

	Prism aprism;
	while(aprism.Read(iss) && iss);

	std::ostringstream oss;
	oss << aprism;
	TRACE(oss.str().c_str());
	CPPUNIT_ASSERT(::input_error == 0);
}

void TestPrism::testHDFSerialize(void)
{
	const char input[] =
		"\t-perimeter SHAPE     map  ..\\setup\\phast\\examples\\capecod\\ArcData\\coast.shp\n"
		"\t-top       arcraster map  ..\\setup\\phast\\examples\\capecod\\ArcData\\elevation-90.txt\n"
		"\t-bottom    arcraster map  ..\\setup\\phast\\examples\\capecod\\ArcData\\bedrock-90.txt\n";

#ifdef SAVE
	const char input[] =
		"\t-perimeter SHAPE     grid  ..\\setup\\phast\\examples\\capecod\\ArcData\\coast.shp\n"
		"\t-top       arcraster grid  ..\\setup\\phast\\examples\\capecod\\ArcData\\elevation-90.txt\n"
		"\t-bottom    arcraster grid  ..\\setup\\phast\\examples\\capecod\\ArcData\\bedrock-90.txt\n";
#endif

	::input_error = 0;
	std::istringstream iss(input);

	Prism prism;
	while(prism.Read(iss))
	{
		if (iss.rdstate() & std::ios::eofbit) break;
		iss.clear();
	}
	CPPUNIT_ASSERT(::input_error == 0);
	prism.Tidy();
	CPPUNIT_ASSERT(::input_error == 0);

	hid_t file_id = H5Fcreate("Test/prism.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
	CPPUNIT_ASSERT(file_id > 0);

	herr_t e = CGlobal::HDFSerializePrism(true, file_id, prism);
	CPPUNIT_ASSERT(e >= 0);

	herr_t s = H5Fclose(file_id);
	CPPUNIT_ASSERT(e >= 0);

	file_id = H5Fopen("Test/prism.h5", H5F_ACC_RDONLY, H5P_DEFAULT);
	CPPUNIT_ASSERT(file_id > 0);

	Prism cp;
	e = CGlobal::HDFSerializePrism(false, file_id, cp);
	CPPUNIT_ASSERT(e >= 0);
	cp.Tidy();

	s = H5Fclose(file_id);
	CPPUNIT_ASSERT(e >= 0);

	CPPUNIT_ASSERT(prism == cp);

	Clear_NNInterpolatorList();
	FakeFiledata::Clear_fake_file_data_list();
	Clear_file_data_map();
	Clear_KDtreeList();
}

#include "StopWatch.h"

void TestPrism::testCopyCtor(void)
{
	const char input[] =
		"\t-perimeter SHAPE      GRID   ..\\setup\\phast\\examples\\ex5\\ArcData\\SimplePonds.shp\n"
		"\t-top       CONSTANT   GRID   20\n"
		"\t-bottom    SHAPE      GRID   ..\\setup\\phast\\examples\\ex5\\ArcData\\SimpleBath.shp 10\n";

	// Set_bounding_box requires the domain to be set
	//
	::domain = zone(275000, 810000, -120, 285000, 830000, 20);
	::input_error = 0;
	std::istringstream iss(input);

	Prism prism;
	while(prism.Read(iss))
	{
		if (iss.rdstate() & std::ios::eofbit) break;
		iss.clear();
	}
	CPPUNIT_ASSERT(::input_error == 0);
	prism.Tidy();
	CPPUNIT_ASSERT(::input_error == 0);

	CPPUNIT_ASSERT(prism.Point_in_polyhedron(Point(277760., 819191., 10.)));
	CPPUNIT_ASSERT(prism.Point_in_polyhedron(Point(280300., 820760., 10.)));
	CPPUNIT_ASSERT(prism.Point_in_polyhedron(Point(281536., 820145., 10.)));

	Prism cp(prism);

	CPPUNIT_ASSERT(prism == cp);

	CPPUNIT_ASSERT(cp.Point_in_polyhedron(Point(277760., 819191., 10.)));
	CPPUNIT_ASSERT(cp.Point_in_polyhedron(Point(280300., 820760., 10.)));
	CPPUNIT_ASSERT(cp.Point_in_polyhedron(Point(281536., 820145., 10.)));

	Clear_NNInterpolatorList();
	FakeFiledata::Clear_fake_file_data_list();
	Clear_file_data_map();
	Clear_KDtreeList();
	::domain = zone(0, 0, 0, 0, 0, 0);

	delete ::map_to_grid;
	::map_to_grid = 0;
}

void TestPrism::timeMemDebug(void)
{
// COMMENT: {12/2/2010 4:50:39 PM}	// turn off memory tracking
// COMMENT: {12/2/2010 4:50:39 PM}	BOOL save = ::AfxEnableMemoryTracking(FALSE);
// COMMENT: {12/2/2010 4:50:39 PM}
// COMMENT: {12/2/2010 4:50:39 PM}	const char input[] =
// COMMENT: {12/2/2010 4:50:39 PM}		"\t-perimeter SHAPE     map  ..\\setup\\phast\\examples\\capecod\\ArcData\\coast.shp\n"
// COMMENT: {12/2/2010 4:50:39 PM}		"\t-top       arcraster map  ..\\setup\\phast\\examples\\capecod\\ArcData\\elevation-90.txt\n"
// COMMENT: {12/2/2010 4:50:39 PM}		"\t-bottom    arcraster map  ..\\setup\\phast\\examples\\capecod\\ArcData\\bedrock-90.txt\n";
// COMMENT: {12/2/2010 4:50:39 PM}
// COMMENT: {12/2/2010 4:50:39 PM}	::map_to_grid = new PHAST_Transform(0, 0, 0, 0, 1, 1, 1);
// COMMENT: {12/2/2010 4:50:39 PM}
// COMMENT: {12/2/2010 4:50:39 PM}	CStopWatch sw;
// COMMENT: {12/2/2010 4:50:39 PM}	sw.startTimer();
// COMMENT: {12/2/2010 4:50:39 PM}
// COMMENT: {12/2/2010 4:50:39 PM}	// Set_bounding_box requires the domain to be set
// COMMENT: {12/2/2010 4:50:39 PM}	//
// COMMENT: {12/2/2010 4:50:39 PM}	::domain = zone(275000, 810000, -120, 285000, 830000, 20);
// COMMENT: {12/2/2010 4:50:39 PM}	::input_error = 0;
// COMMENT: {12/2/2010 4:50:39 PM}	std::istringstream iss(input);
// COMMENT: {12/2/2010 4:50:39 PM}
// COMMENT: {12/2/2010 4:50:39 PM}	Prism prism;
// COMMENT: {12/2/2010 4:50:39 PM}	while(prism.Read(iss))
// COMMENT: {12/2/2010 4:50:39 PM}	{
// COMMENT: {12/2/2010 4:50:39 PM}		if (iss.rdstate() & std::ios::eofbit) break;
// COMMENT: {12/2/2010 4:50:39 PM}		iss.clear();
// COMMENT: {12/2/2010 4:50:39 PM}	}
// COMMENT: {12/2/2010 4:50:39 PM}	CPPUNIT_ASSERT(::input_error == 0);
// COMMENT: {12/2/2010 4:50:39 PM}	prism.Tidy();
// COMMENT: {12/2/2010 4:50:39 PM}	CPPUNIT_ASSERT(::input_error == 0);
// COMMENT: {12/2/2010 4:50:39 PM}
// COMMENT: {12/2/2010 4:50:39 PM}	prism.Point_in_polyhedron(Point(277760., 819191., 5.));
// COMMENT: {12/2/2010 4:50:39 PM}
// COMMENT: {12/2/2010 4:50:39 PM}	Clear_NNInterpolatorList();
// COMMENT: {8/9/2011 6:49:41 PM}	FakeFiledata::Clear_fake_file_data_list();
// COMMENT: {12/2/2010 4:50:39 PM}	Clear_file_data_map();
// COMMENT: {12/2/2010 4:50:39 PM}	Clear_KDtreeList();
// COMMENT: {12/2/2010 4:50:39 PM}	::domain = zone(0, 0, 0, 0, 0, 0);
// COMMENT: {12/2/2010 4:50:39 PM}
// COMMENT: {12/2/2010 4:50:39 PM}	sw.stopTimer();
// COMMENT: {12/2/2010 4:50:39 PM}	delete ::map_to_grid;
// COMMENT: {12/2/2010 4:50:39 PM}	::map_to_grid = 0;
// COMMENT: {12/2/2010 4:50:39 PM}	TRACE("TestPrism::timeMemDebug time = %g\n", sw.getElapsedTime());
// COMMENT: {12/2/2010 4:50:39 PM}
// COMMENT: {12/2/2010 4:50:39 PM}	// restore memory tracking
// COMMENT: {12/2/2010 4:50:39 PM}	::AfxEnableMemoryTracking(save);
}

void TestPrism::PolygonIntersectsSelf(void)
{
	std::vector<Point> pts;
	pts.push_back(Point(0, 0, 0));
	pts.push_back(Point(0, 1, 0));
	pts.push_back(Point(1, 0, 0));
	pts.push_back(Point(1, 1, 0));
	CPPUNIT_ASSERT(Prism::Polygon_intersects_self(pts));

	std::vector<Point> pts2;
	pts2.push_back(Point(0, 0, 0));
	pts2.push_back(Point(0, 1, 0));
	pts2.push_back(Point(1, 0, 0));
	pts2.push_back(Point(1, 1, 0));
	pts2.push_back(Point(0, 0, 0));
	CPPUNIT_ASSERT(Prism::Polygon_intersects_self(pts2));

	std::vector<Point> pts3;
	pts3.push_back(Point(0, 0, 0));
	pts3.push_back(Point(0, 1, 0));
	pts3.push_back(Point(1, 1, 0));
	pts3.push_back(Point(1, 0, 0));
	CPPUNIT_ASSERT(!Prism::Polygon_intersects_self(pts3));

	std::vector<Point> pts4;
	pts4.push_back(Point(0, 0, 0));
	pts4.push_back(Point(0, 1, 0));
	pts4.push_back(Point(1, 1, 0));
	pts4.push_back(Point(1, 0, 0));
	pts4.push_back(Point(0, 0, 0));
	CPPUNIT_ASSERT(!Prism::Polygon_intersects_self(pts4));

	std::vector<Point> pts5;
	pts5.push_back(Point(0, 0, 0));
	pts5.push_back(Point(0, 1, 0));
	pts5.push_back(Point(1, 1, 0));
	CPPUNIT_ASSERT(!Prism::Polygon_intersects_self(pts5));

	std::vector<Point> pts6;
	CPPUNIT_ASSERT(!Prism::Polygon_intersects_self(pts6));

	// 4 collinear points
	std::vector<Point> pts7;
	pts7.push_back(Point(0, 0, 0));
	pts7.push_back(Point(2, 0, 0));
	pts7.push_back(Point(1, 0, 0));
	pts7.push_back(Point(3, 0, 0));
	CPPUNIT_ASSERT(Prism::Polygon_intersects_self(pts7));

	// 3 collinear points (horiz)
	std::vector<Point> pts8;
	pts8.push_back(Point(0, 0, 0));
	pts8.push_back(Point(2, 0, 0));
	pts8.push_back(Point(1, 0, 0));
	CPPUNIT_ASSERT(Prism::Polygon_intersects_self(pts8));

	// 3 collinear points (vert)
	std::vector<Point> pts9;
	pts9.push_back(Point(0, 0, 0));
	pts9.push_back(Point(0, 2, 0));
	pts9.push_back(Point(0, 1, 0));
	CPPUNIT_ASSERT(Prism::Polygon_intersects_self(pts9));

	// right triangle
	std::vector<Point> pts10;
	pts10.push_back(Point(0, 0, 0));
	pts10.push_back(Point(0, 1, 0));
	pts10.push_back(Point(1, 0, 0));
	CPPUNIT_ASSERT(!Prism::Polygon_intersects_self(pts10));

	// square
	std::vector<Point> pts11;
	pts11.push_back(Point(0, 0, 0));
	pts11.push_back(Point(0, 1, 0));
	pts11.push_back(Point(1, 1, 0));
	pts11.push_back(Point(1, 0, 0));
	CPPUNIT_ASSERT(!Prism::Polygon_intersects_self(pts11));

	// 3 (first==last) collinear points (horiz)
	std::vector<Point> pts12;
	pts12.push_back(Point(0, 0, 0));
	pts12.push_back(Point(2, 0, 0));
	pts12.push_back(Point(1, 0, 0));
	pts12.push_back(Point(0, 0, 0));
	CPPUNIT_ASSERT(Prism::Polygon_intersects_self(pts12));

	// 3 (first==last) collinear points (vert)
	std::vector<Point> pts13;
	pts13.push_back(Point(0, 0, 0));
	pts13.push_back(Point(0, 2, 0));
	pts13.push_back(Point(0, 1, 0));
	pts13.push_back(Point(0, 0, 0));
	CPPUNIT_ASSERT(Prism::Polygon_intersects_self(pts9));

	std::vector<Point> pts14;
	pts14.push_back(Point(-1,  0, 0));
	pts14.push_back(Point( 0,  0, 0));
	pts14.push_back(Point( 0,  1, 0));
	pts14.push_back(Point( 0, -1, 0));
	pts14.push_back(Point(-1, -1, 0));
	pts14.push_back(Point(-1,  0, 0));
	CPPUNIT_ASSERT(Prism::Polygon_intersects_self(pts14));

	// collinear non-overlapping parallel to xaxis
	std::vector<Point> pts15;
	pts15.push_back(Point(0,  0, 0));
	pts15.push_back(Point(1,  0, 0));
	pts15.push_back(Point(2,  0, 0));
	pts15.push_back(Point(3,  0, 0));
	pts15.push_back(Point(0,  1, 0));
	CPPUNIT_ASSERT(!Prism::Polygon_intersects_self(pts15));

	// collinear overlapping parallel to xaxis
	std::vector<Point> pts15a;
	pts15a.push_back(Point(0,  0, 0));
	pts15a.push_back(Point(2,  0, 0));
	pts15a.push_back(Point(1,  0, 0));
	pts15a.push_back(Point(3,  0, 0));
	pts15a.push_back(Point(0,  1, 0));
	CPPUNIT_ASSERT(Prism::Polygon_intersects_self(pts15a));

	// collinear non-overlapping parallel to yaxis
	std::vector<Point> pts16;
	pts16.push_back(Point(0,  0, 0));
	pts16.push_back(Point(0,  1, 0));
	pts16.push_back(Point(0,  2, 0));
	pts16.push_back(Point(0,  3, 0));
	pts16.push_back(Point(1,  0, 0));
	CPPUNIT_ASSERT(!Prism::Polygon_intersects_self(pts16));

	// collinear overlapping parallel to yaxis
	std::vector<Point> pts16a;
	pts16a.push_back(Point(0,  0, 0));
	pts16a.push_back(Point(0,  2, 0));
	pts16a.push_back(Point(0,  1, 0));
	pts16a.push_back(Point(0,  3, 0));
	pts16a.push_back(Point(1,  0, 0));
	CPPUNIT_ASSERT(Prism::Polygon_intersects_self(pts16a));
}

void TestPrism::TestPointsCtor(void)
{
	std::vector< Point > pts;
	pts.push_back(Point(0, 0, 0));
	pts.push_back(Point(1, 0, 0));
	pts.push_back(Point(1, 1, 0));
	pts.push_back(Point(0, 1, 0));

	std::ostringstream oss;
	oss.precision(DBL_DIG);
	oss << "-perimeter POINTS grid" << std::endl;

	std::vector<Point>::iterator it = pts.begin();
	for (; it != pts.end(); ++it)
	{
		double *pt = (*it).get_coord();
		oss << pt[0] << " ";
		oss << pt[1] << " ";
		oss << pt[2] << std::endl;
	}

	std::istringstream iss(oss.str());
	Prism pr;
	CPPUNIT_ASSERT(pr.Read(iss));
	pr.bottom.Set_coordinate_system(PHAST_Transform::GRID);
	pr.top.Set_coordinate_system(PHAST_Transform::GRID);
	pr.bottom.Set_user_coordinate_system(PHAST_Transform::GRID);
	pr.top.Set_user_coordinate_system(PHAST_Transform::GRID);

	Prism pc(pts);

	CPPUNIT_ASSERT(pr == pc);
}

void TestPrism::testOperatorEquals(void)
{
	const char input[] =
		"\t-perimeter SHAPE      GRID   ..\\setup\\phast\\examples\\ex5\\ArcData\\SimplePonds.shp\n"
		"\t-top       CONSTANT   GRID   20\n"
		"\t-bottom    SHAPE      GRID   ..\\setup\\phast\\examples\\ex5\\ArcData\\SimpleBath.shp 10\n";

	// Set_bounding_box requires the domain to be set
	//
	::domain = zone(275000, 810000, -120, 285000, 830000, 20);
	::input_error = 0;
	std::istringstream iss(input);

	Prism prism;
	while(prism.Read(iss))
	{
		if (iss.rdstate() & std::ios::eofbit) break;
		iss.clear();
	}
	CPPUNIT_ASSERT(::input_error == 0);
	prism.Tidy();
	CPPUNIT_ASSERT(::input_error == 0);

	CPPUNIT_ASSERT(prism.Point_in_polyhedron(Point(277760., 819191., 10.)));
	CPPUNIT_ASSERT(prism.Point_in_polyhedron(Point(280300., 820760., 10.)));
	CPPUNIT_ASSERT(prism.Point_in_polyhedron(Point(281536., 820145., 10.)));

	Prism equals;
	equals = prism;

	CPPUNIT_ASSERT(prism == equals);

	CPPUNIT_ASSERT(equals.Point_in_polyhedron(Point(277760., 819191., 10.)));
	CPPUNIT_ASSERT(equals.Point_in_polyhedron(Point(280300., 820760., 10.)));
	CPPUNIT_ASSERT(equals.Point_in_polyhedron(Point(281536., 820145., 10.)));

	Clear_NNInterpolatorList();
	FakeFiledata::Clear_fake_file_data_list();
	Clear_file_data_map();
	Clear_KDtreeList();
	::domain = zone(0, 0, 0, 0, 0, 0);

	delete ::map_to_grid;
	::map_to_grid = 0;
}
