#include "StdAfx.h"
#include "TestPHAST_Transform.h"

#include "srcinput/PHAST_Transform.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void TestPHAST_Transform::test(void)
{
// COMMENT: {12/9/2010 5:23:29 PM}	double ox = 4.;
// COMMENT: {12/9/2010 5:23:29 PM}	double oy = 3.;
// COMMENT: {12/9/2010 5:23:29 PM}	double oz = 0.;
	double ox = 10.;
	double oy = 5.;
	double oz = 0.;

	// 3 : 4 : 5
	// 36.869897645844021296855612559093
	// 53.130102354155978703144387440907
// COMMENT: {12/9/2010 5:23:05 PM}	double angle_degrees = 36.869897645844021296855612559093;
	double angle_degrees = 90.;
// COMMENT: {12/9/2010 4:53:26 PM}	double scale_x = 1.;
// COMMENT: {12/9/2010 4:53:26 PM}	double scale_y = 1.;
// COMMENT: {12/9/2010 4:53:26 PM}	double scale_z = 1.;
	double scale_x = 2.;
	double scale_y = 1.;
	double scale_z = 1.;

	PHAST_Transform map_to_grid(ox, oy, oz, angle_degrees, scale_x, scale_y, scale_z);

// COMMENT: {12/9/2010 5:27:15 PM}	double p1x = 5.;
// COMMENT: {12/9/2010 5:27:15 PM}	double p1y = 5.;
// COMMENT: {12/9/2010 5:27:15 PM}	double p1z = 0.;
	double p1x = 1.;
	double p1y = 1.;
	double p1z = 0.;
	Point p1(p1x, p1y, p1z);

	map_to_grid.Transform(p1);
// COMMENT: {12/9/2010 5:23:48 PM}	CPPUNIT_ASSERT_DOUBLES_EQUAL(p1.x(), 2.0*scale_x, ::pow((double)10, (double)-DBL_DIG));
// COMMENT: {12/9/2010 5:23:48 PM}	CPPUNIT_ASSERT_DOUBLES_EQUAL(p1.y(), 1.0*scale_y, ::pow((double)10, (double)-DBL_DIG));
// COMMENT: {12/9/2010 5:23:48 PM}	CPPUNIT_ASSERT_DOUBLES_EQUAL(p1.z(), 0.0*scale_z, ::pow((double)10, (double)-DBL_DIG));

// COMMENT: {12/9/2010 5:23:53 PM}	map_to_grid.Inverse_transform(p1);
// COMMENT: {12/9/2010 5:23:53 PM}	CPPUNIT_ASSERT_DOUBLES_EQUAL(p1.x(), p1x, ::pow((double)10, (double)-DBL_DIG));
// COMMENT: {12/9/2010 5:23:53 PM}	CPPUNIT_ASSERT_DOUBLES_EQUAL(p1.y(), p1y, ::pow((double)10, (double)-DBL_DIG));
// COMMENT: {12/9/2010 5:23:53 PM}	CPPUNIT_ASSERT_DOUBLES_EQUAL(p1.z(), p1z, ::pow((double)10, (double)-DBL_DIG));

	double p2x = 6.;
	double p2y = 7.;
	double p2z = 0.;
	Point p2(p2x, p2y, p2z);

	map_to_grid.Transform(p2);
// COMMENT: {12/9/2010 5:24:03 PM}	CPPUNIT_ASSERT_DOUBLES_EQUAL(p2.x(), 4.0*scale_x, ::pow((double)10, (double)-DBL_DIG));
// COMMENT: {12/9/2010 5:24:03 PM}	CPPUNIT_ASSERT_DOUBLES_EQUAL(p2.y(), 2.0*scale_y, ::pow((double)10, (double)-DBL_DIG));
// COMMENT: {12/9/2010 5:24:03 PM}	CPPUNIT_ASSERT_DOUBLES_EQUAL(p2.z(), 0.0*scale_z, ::pow((double)10, (double)-DBL_DIG));

	map_to_grid.Inverse_transform(p2);
// COMMENT: {12/9/2010 5:24:06 PM}	CPPUNIT_ASSERT_DOUBLES_EQUAL(p2.x(), p2x, ::pow((double)10, (double)-DBL_DIG));
// COMMENT: {12/9/2010 5:24:06 PM}	CPPUNIT_ASSERT_DOUBLES_EQUAL(p2.y(), p2y, ::pow((double)10, (double)-DBL_DIG));
// COMMENT: {12/9/2010 5:24:06 PM}	CPPUNIT_ASSERT_DOUBLES_EQUAL(p2.z(), p2z, ::pow((double)10, (double)-DBL_DIG));

	//{{
	TRACE("\nPhast Matrix:\n");
	TRACE("%g %g %g %g\n", map_to_grid.trans(0, 0),  map_to_grid.trans(0, 1),  map_to_grid.trans(0, 2),  map_to_grid.trans(0, 3));
	TRACE("%g %g %g %g\n", map_to_grid.trans(1, 0),  map_to_grid.trans(1, 1),  map_to_grid.trans(1, 2),  map_to_grid.trans(1, 3));
	TRACE("%g %g %g %g\n", map_to_grid.trans(2, 0),  map_to_grid.trans(2, 1),  map_to_grid.trans(2, 2),  map_to_grid.trans(2, 3));
	TRACE("%g %g %g %g\n", map_to_grid.trans(3, 0),  map_to_grid.trans(3, 1),  map_to_grid.trans(3, 2),  map_to_grid.trans(3, 3));

// COMMENT: {12/9/2010 7:06:33 PM}	TRACE("\nPhast Inverse Matrix:\n");
// COMMENT: {12/9/2010 7:06:33 PM}	TRACE("%g %g %g %g\n", map_to_grid.inverse(0, 0),  map_to_grid.inverse(0, 1),  map_to_grid.inverse(0, 2),  map_to_grid.inverse(0, 3));
// COMMENT: {12/9/2010 7:06:33 PM}	TRACE("%g %g %g %g\n", map_to_grid.inverse(1, 0),  map_to_grid.inverse(1, 1),  map_to_grid.inverse(1, 2),  map_to_grid.inverse(1, 3));
// COMMENT: {12/9/2010 7:06:33 PM}	TRACE("%g %g %g %g\n", map_to_grid.inverse(2, 0),  map_to_grid.inverse(2, 1),  map_to_grid.inverse(2, 2),  map_to_grid.inverse(2, 3));
// COMMENT: {12/9/2010 7:06:33 PM}	TRACE("%g %g %g %g\n", map_to_grid.inverse(3, 0),  map_to_grid.inverse(3, 1),  map_to_grid.inverse(3, 2),  map_to_grid.inverse(3, 3));


// COMMENT: {12/9/2010 7:05:08 PM}	vtkTransform *vtrans = vtkTransform::New();
// COMMENT: {12/9/2010 7:05:08 PM}// COMMENT: {12/9/2010 4:52:02 PM}	vtrans->Translate(ox, oy, oz);
// COMMENT: {12/9/2010 7:05:08 PM}// COMMENT: {12/9/2010 4:52:02 PM}	vtrans->Translate(-ox, -oy, -oz);
// COMMENT: {12/9/2010 7:05:08 PM}// COMMENT: {12/9/2010 6:09:59 PM}	vtrans->RotateZ(-angle_degrees);
// COMMENT: {12/9/2010 7:05:08 PM}// COMMENT: {12/9/2010 6:09:59 PM}	vtrans->RotateX(0.0);
// COMMENT: {12/9/2010 7:05:08 PM}// COMMENT: {12/9/2010 6:09:59 PM}	vtrans->RotateY(0.0);
// COMMENT: {12/9/2010 7:05:08 PM}	//vtrans->RotateWXYZ(-angle_degrees, 0, 0, 1);
// COMMENT: {12/9/2010 7:05:08 PM}	vtrans->Scale(scale_x, scale_y, scale_z);
// COMMENT: {12/9/2010 7:05:08 PM}	vtrans->RotateZ(-angle_degrees);
// COMMENT: {12/9/2010 7:05:08 PM}	vtrans->Translate(-ox, -oy, -oz);
// COMMENT: {12/9/2010 7:05:08 PM}// COMMENT: {12/9/2010 6:12:11 PM}	vtrans->RotateX(0.0);
// COMMENT: {12/9/2010 7:05:08 PM}// COMMENT: {12/9/2010 6:12:11 PM}	vtrans->RotateY(0.0);
// COMMENT: {12/9/2010 7:05:08 PM}// COMMENT: {12/9/2010 6:12:11 PM}

	vtkTransform *vtrans = vtkTransform::New();
	vtrans->PostMultiply();
	vtrans->Translate(-ox, -oy, -oz);
	vtrans->RotateZ(-angle_degrees);
	vtrans->Scale(scale_x, scale_y, scale_z);

	double m[16];
	vtkMatrix4x4 *m4x4 = vtrans->GetMatrix();
	vtkMatrix4x4::DeepCopy(m, m4x4);

	TRACE("\nvtkTransform Matrix:\n");
	TRACE("%g %g %g %g\n", m[0],  m[1],  m[2],  m[3]);
	TRACE("%g %g %g %g\n", m[4],  m[5],  m[6],  m[7]);
	TRACE("%g %g %g %g\n", m[8],  m[9],  m[10], m[11]);
	TRACE("%g %g %g %g\n", m[12], m[13], m[14], m[15]);

	double *vpt1 = vtrans->TransformDoublePoint(p1x, p1y, p1z);
// COMMENT: {12/9/2010 5:24:25 PM}	CPPUNIT_ASSERT_DOUBLES_EQUAL(vpt1[0], 2.0*scale_x, ::pow((double)10, (double)-DBL_DIG));
// COMMENT: {12/9/2010 5:24:25 PM}	CPPUNIT_ASSERT_DOUBLES_EQUAL(vpt1[1], 1.0*scale_y, ::pow((double)10, (double)-DBL_DIG));
// COMMENT: {12/9/2010 5:24:25 PM}	CPPUNIT_ASSERT_DOUBLES_EQUAL(vpt1[2], 0.0*scale_z, ::pow((double)10, (double)-DBL_DIG));

	double *vpt2 = vtrans->TransformDoublePoint(p2x, p2y, p2z);
// COMMENT: {12/9/2010 5:24:27 PM}	CPPUNIT_ASSERT_DOUBLES_EQUAL(vpt2[0], 4.0*scale_x, ::pow((double)10, (double)-DBL_DIG));
// COMMENT: {12/9/2010 5:24:27 PM}	CPPUNIT_ASSERT_DOUBLES_EQUAL(vpt2[1], 2.0*scale_y, ::pow((double)10, (double)-DBL_DIG));
// COMMENT: {12/9/2010 5:24:27 PM}	CPPUNIT_ASSERT_DOUBLES_EQUAL(vpt2[2], 0.0*scale_z, ::pow((double)10, (double)-DBL_DIG));

	vtrans->Delete();
	//}}
}

void TestPHAST_Transform::vtkProp3DEquivalence(void)
{
	double origin[3] = {500.0, 400.0, 300.0};
	double angle = -20.73;
	PHAST_Transform ptrans(origin[0], origin[1], origin[2], angle);

	vtkProp3D *prop3d = vtkActor::New();

	prop3d->SetPosition(-origin[0], -origin[1], -origin[2]);
	prop3d->SetOrigin(origin[0], origin[1], origin[2]);
	prop3d->SetOrientation(0, 0, -angle);

	double m[16];
	prop3d->GetMatrix(m);

	TRACE("\nprop3d Matrix:\n");
	TRACE("%g %g %g %g\n", m[0],  m[1],  m[2],  m[3]);
	TRACE("%g %g %g %g\n", m[4],  m[5],  m[6],  m[7]);
	TRACE("%g %g %g %g\n", m[8],  m[9],  m[10], m[11]);
	TRACE("%g %g %g %g\n", m[12], m[13], m[14], m[15]);

	TRACE("\nPhast Matrix:\n");
	TRACE("%g %g %g %g\n", ptrans.trans(0, 0),  ptrans.trans(0, 1),  ptrans.trans(0, 2),  ptrans.trans(0, 3));
	TRACE("%g %g %g %g\n", ptrans.trans(1, 0),  ptrans.trans(1, 1),  ptrans.trans(1, 2),  ptrans.trans(1, 3));
	TRACE("%g %g %g %g\n", ptrans.trans(2, 0),  ptrans.trans(2, 1),  ptrans.trans(2, 2),  ptrans.trans(2, 3));
	TRACE("%g %g %g %g\n", ptrans.trans(3, 0),  ptrans.trans(3, 1),  ptrans.trans(3, 2),  ptrans.trans(3, 3));

	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[0],  ptrans.trans(0, 0), ::pow((double)10, (double)-FLT_DIG));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[1],  ptrans.trans(0, 1), ::pow((double)10, (double)-FLT_DIG));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[2],  ptrans.trans(0, 2), ::pow((double)10, (double)-FLT_DIG));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[3],  ptrans.trans(0, 3), ::pow((double)10, (double)-FLT_DIG));

	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[4],  ptrans.trans(1, 0), ::pow((double)10, (double)-FLT_DIG));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[5],  ptrans.trans(1, 1), ::pow((double)10, (double)-FLT_DIG));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[6],  ptrans.trans(1, 2), ::pow((double)10, (double)-FLT_DIG));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[7],  ptrans.trans(1, 3), ::pow((double)10, (double)-FLT_DIG));

	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[8],  ptrans.trans(2, 0), ::pow((double)10, (double)-FLT_DIG));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[9],  ptrans.trans(2, 1), ::pow((double)10, (double)-FLT_DIG));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[10], ptrans.trans(2, 2), ::pow((double)10, (double)-FLT_DIG));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[11], ptrans.trans(2, 3), ::pow((double)10, (double)-FLT_DIG));

	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[12], ptrans.trans(3, 0), ::pow((double)10, (double)-FLT_DIG));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[13], ptrans.trans(3, 1), ::pow((double)10, (double)-FLT_DIG));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[14], ptrans.trans(3, 2), ::pow((double)10, (double)-FLT_DIG));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[15], ptrans.trans(3, 3), ::pow((double)10, (double)-FLT_DIG));

	prop3d->Delete();
}

void TestPHAST_Transform::vtkProp3DEquivalence2(void)
{
	double origin[3] = {4., 3., 0.0};
// COMMENT: {12/15/2010 5:14:56 PM}	double angle = 36.869897645844021296855612559093;
	double angle = 36.87;
	PHAST_Transform ptrans(origin[0], origin[1], origin[2], angle);

	vtkProp3D *prop3d = vtkActor::New();

	prop3d->SetPosition(-origin[0], -origin[1], -origin[2]);
	prop3d->SetOrigin(origin[0], origin[1], origin[2]);
	prop3d->SetOrientation(0, 0, -angle);

	double m[16];
	prop3d->GetMatrix(m);

	TRACE("\nprop3d Matrix:\n");
	TRACE("%g %g %g %g\n", m[0],  m[1],  m[2],  m[3]);
	TRACE("%g %g %g %g\n", m[4],  m[5],  m[6],  m[7]);
	TRACE("%g %g %g %g\n", m[8],  m[9],  m[10], m[11]);
	TRACE("%g %g %g %g\n", m[12], m[13], m[14], m[15]);

	TRACE("\nPhast Matrix:\n");
	TRACE("%g %g %g %g\n", ptrans.trans(0, 0),  ptrans.trans(0, 1),  ptrans.trans(0, 2),  ptrans.trans(0, 3));
	TRACE("%g %g %g %g\n", ptrans.trans(1, 0),  ptrans.trans(1, 1),  ptrans.trans(1, 2),  ptrans.trans(1, 3));
	TRACE("%g %g %g %g\n", ptrans.trans(2, 0),  ptrans.trans(2, 1),  ptrans.trans(2, 2),  ptrans.trans(2, 3));
	TRACE("%g %g %g %g\n", ptrans.trans(3, 0),  ptrans.trans(3, 1),  ptrans.trans(3, 2),  ptrans.trans(3, 3));

	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[0],  ptrans.trans(0, 0), ::pow((double)10, (double)-FLT_DIG));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[1],  ptrans.trans(0, 1), ::pow((double)10, (double)-FLT_DIG));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[2],  ptrans.trans(0, 2), ::pow((double)10, (double)-FLT_DIG));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[3],  ptrans.trans(0, 3), ::pow((double)10, (double)-FLT_DIG));

	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[4],  ptrans.trans(1, 0), ::pow((double)10, (double)-FLT_DIG));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[5],  ptrans.trans(1, 1), ::pow((double)10, (double)-FLT_DIG));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[6],  ptrans.trans(1, 2), ::pow((double)10, (double)-FLT_DIG));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[7],  ptrans.trans(1, 3), ::pow((double)10, (double)-FLT_DIG));

	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[8],  ptrans.trans(2, 0), ::pow((double)10, (double)-FLT_DIG));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[9],  ptrans.trans(2, 1), ::pow((double)10, (double)-FLT_DIG));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[10], ptrans.trans(2, 2), ::pow((double)10, (double)-FLT_DIG));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[11], ptrans.trans(2, 3), ::pow((double)10, (double)-FLT_DIG));

	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[12], ptrans.trans(3, 0), ::pow((double)10, (double)-FLT_DIG));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[13], ptrans.trans(3, 1), ::pow((double)10, (double)-FLT_DIG));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[14], ptrans.trans(3, 2), ::pow((double)10, (double)-FLT_DIG));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[15], ptrans.trans(3, 3), ::pow((double)10, (double)-FLT_DIG));

	Point p1(6, 6, 0);
	ptrans.Transform(p1);
	TRACE("\n(6,6,0)=(%g,%g,%g):\n", p1.x(), p1.y(), p1.z());

	Point p2(8, 7, 1);
	ptrans.Transform(p2);
	TRACE("\n(8,7,1)=(%g,%g,%g):\n", p2.x(), p2.y(), p2.z());

	prop3d->Delete();
}


void TestPHAST_Transform::vtkProp3DEquivalenceWithScale(void)
{
	double origin[3] = {500.0, 400.0, 300.0};
	double angle = -20.73;
	double scale[3] = {2.0, 1.0, 1.0};
	PHAST_Transform ptrans(origin[0], origin[1], origin[2], angle, scale[0], scale[1], scale[2]);

	vtkProp3D *prop3d = vtkActor::New();

// COMMENT: {12/9/2010 6:20:22 PM}	prop3d->SetPosition(-origin[0], -origin[1], -origin[2]);
// COMMENT: {12/9/2010 6:20:22 PM}	prop3d->SetOrigin(origin[0], origin[1], origin[2]);
// COMMENT: {12/9/2010 6:20:22 PM}	prop3d->SetOrientation(0, 0, -angle);
// COMMENT: {12/9/2010 6:20:22 PM}	prop3d->SetScale(scale[0], scale[1], scale[2]);

	//{{
	prop3d->SetPosition(-origin[0], -origin[1], -origin[2]);
	prop3d->SetOrigin(origin[0], origin[1], origin[2]);
	prop3d->SetOrientation(0, 0, -angle);
	prop3d->SetScale(scale[0], scale[1], scale[2]);
	//}}


	double m[16];
	prop3d->GetMatrix(m);

	TRACE("\nprop3d Matrix:\n");
	TRACE("%g %g %g %g\n", m[0],  m[1],  m[2],  m[3]);
	TRACE("%g %g %g %g\n", m[4],  m[5],  m[6],  m[7]);
	TRACE("%g %g %g %g\n", m[8],  m[9],  m[10], m[11]);
	TRACE("%g %g %g %g\n", m[12], m[13], m[14], m[15]);

	TRACE("\nPhast Matrix:\n");
	TRACE("%g %g %g %g\n", ptrans.trans(0, 0),  ptrans.trans(0, 1),  ptrans.trans(0, 2),  ptrans.trans(0, 3));
	TRACE("%g %g %g %g\n", ptrans.trans(1, 0),  ptrans.trans(1, 1),  ptrans.trans(1, 2),  ptrans.trans(1, 3));
	TRACE("%g %g %g %g\n", ptrans.trans(2, 0),  ptrans.trans(2, 1),  ptrans.trans(2, 2),  ptrans.trans(2, 3));
	TRACE("%g %g %g %g\n", ptrans.trans(3, 0),  ptrans.trans(3, 1),  ptrans.trans(3, 2),  ptrans.trans(3, 3));

// COMMENT: {12/9/2010 6:19:22 PM}	vtkTransform *vtrans = vtkTransform::New();
// COMMENT: {12/9/2010 6:19:22 PM}	vtrans->Translate(-origin[0], -origin[1], -origin[2]);
// COMMENT: {12/9/2010 6:19:22 PM}	vtrans->Translate(origin[0], origin[1], origin[2]);
// COMMENT: {12/9/2010 6:19:22 PM}	vtrans->RotateZ(-angle);
// COMMENT: {12/9/2010 6:19:22 PM}	vtrans->RotateX(0.0);
// COMMENT: {12/9/2010 6:19:22 PM}	vtrans->RotateY(0.0);
// COMMENT: {12/9/2010 6:19:22 PM}	vtrans->Scale(scale[0], scale[1], scale[2]);
// COMMENT: {12/9/2010 6:19:22 PM}	vtrans->Translate(origin[0], origin[1], origin[2]);
// COMMENT: {12/9/2010 6:19:22 PM}
// COMMENT: {12/9/2010 6:19:22 PM}	vtkMatrix4x4 *m4x4 = vtrans->GetMatrix();
// COMMENT: {12/9/2010 6:19:22 PM}	vtkMatrix4x4::DeepCopy(m, m4x4);
// COMMENT: {12/9/2010 6:19:22 PM}
// COMMENT: {12/9/2010 6:19:22 PM}	TRACE("\nvtkTransform Matrix:\n");
// COMMENT: {12/9/2010 6:19:22 PM}	TRACE("%g %g %g %g\n", m[0],  m[1],  m[2],  m[3]);
// COMMENT: {12/9/2010 6:19:22 PM}	TRACE("%g %g %g %g\n", m[4],  m[5],  m[6],  m[7]);
// COMMENT: {12/9/2010 6:19:22 PM}	TRACE("%g %g %g %g\n", m[8],  m[9],  m[10], m[11]);
// COMMENT: {12/9/2010 6:19:22 PM}	TRACE("%g %g %g %g\n", m[12], m[13], m[14], m[15]);
// COMMENT: {12/9/2010 6:19:22 PM}
// COMMENT: {12/9/2010 6:19:22 PM}	vtrans->Delete();

// COMMENT: {12/8/2010 11:36:32 PM}	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[0],  ptrans.trans(0, 0), ::pow((double)10, (double)-FLT_DIG));
// COMMENT: {12/8/2010 11:36:32 PM}	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[1],  ptrans.trans(0, 1), ::pow((double)10, (double)-FLT_DIG));
// COMMENT: {12/8/2010 11:36:32 PM}	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[2],  ptrans.trans(0, 2), ::pow((double)10, (double)-FLT_DIG));
// COMMENT: {12/8/2010 11:36:32 PM}	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[3],  ptrans.trans(0, 3), ::pow((double)10, (double)-FLT_DIG));
// COMMENT: {12/8/2010 11:36:32 PM}
// COMMENT: {12/8/2010 11:36:32 PM}	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[4],  ptrans.trans(1, 0), ::pow((double)10, (double)-FLT_DIG));
// COMMENT: {12/8/2010 11:36:32 PM}	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[5],  ptrans.trans(1, 1), ::pow((double)10, (double)-FLT_DIG));
// COMMENT: {12/8/2010 11:36:32 PM}	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[6],  ptrans.trans(1, 2), ::pow((double)10, (double)-FLT_DIG));
// COMMENT: {12/8/2010 11:36:32 PM}	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[7],  ptrans.trans(1, 3), ::pow((double)10, (double)-FLT_DIG));
// COMMENT: {12/8/2010 11:36:32 PM}
// COMMENT: {12/8/2010 11:36:32 PM}	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[8],  ptrans.trans(2, 0), ::pow((double)10, (double)-FLT_DIG));
// COMMENT: {12/8/2010 11:36:32 PM}	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[9],  ptrans.trans(2, 1), ::pow((double)10, (double)-FLT_DIG));
// COMMENT: {12/8/2010 11:36:32 PM}	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[10], ptrans.trans(2, 2), ::pow((double)10, (double)-FLT_DIG));
// COMMENT: {12/8/2010 11:36:32 PM}	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[11], ptrans.trans(2, 3), ::pow((double)10, (double)-FLT_DIG));
// COMMENT: {12/8/2010 11:36:32 PM}
// COMMENT: {12/8/2010 11:36:32 PM}	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[12], ptrans.trans(3, 0), ::pow((double)10, (double)-FLT_DIG));
// COMMENT: {12/8/2010 11:36:32 PM}	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[13], ptrans.trans(3, 1), ::pow((double)10, (double)-FLT_DIG));
// COMMENT: {12/8/2010 11:36:32 PM}	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[14], ptrans.trans(3, 2), ::pow((double)10, (double)-FLT_DIG));
// COMMENT: {12/8/2010 11:36:32 PM}	CPPUNIT_ASSERT_DOUBLES_EQUAL(m[15], ptrans.trans(3, 3), ::pow((double)10, (double)-FLT_DIG));

	prop3d->Delete();
}

void TestPHAST_Transform::CoordinateTransforms(void)
{
	// Set up transform
	// map [=] cm
	// grid [=] in

	double origin[3] = {4., 3., 0.};
	double angle     = 36.869897645844021296855612559093;

	double map_horz_to_si = 0.01;
	double horz_to_si     = 0.0254;

	double map_vert_to_si = 0.01;
	double vert_to_si     = 0.0254;

	//
	// SI to Grid
	//

	//
	// SI to Map
	//

	//
	// Grid to Map
	//
	{
// COMMENT: {1/27/2011 3:19:01 PM}		double scale_h = map_horz_to_si / horz_to_si;
// COMMENT: {1/27/2011 3:19:01 PM}		double scale_v = map_vert_to_si / vert_to_si;
// COMMENT: {1/27/2011 3:19:01 PM}		PHAST_Transform map_to_grid(origin[0], origin[1], origin[2], angle, scale_h, scale_h, scale_v);
// COMMENT: {1/27/2011 3:19:01 PM}		PHAST_Transform grid_to_map = map_to_grid.Inverse_transform(
// COMMENT: {1/27/2011 3:19:01 PM}
// COMMENT: {1/27/2011 3:19:01 PM}		vtkTransform *trans = vtkTransform::New();
// COMMENT: {1/27/2011 3:19:01 PM}		trans->Scale(scale_h, scale_h, scale_v);
// COMMENT: {1/27/2011 3:19:01 PM}		trans->RotateZ(-angle);
// COMMENT: {1/27/2011 3:19:01 PM}		trans->Translate(-origin[0], -origin[1], -origin[2]);
// COMMENT: {1/27/2011 3:19:01 PM}		trans
// COMMENT: {1/27/2011 3:19:01 PM}
// COMMENT: {1/27/2011 3:19:01 PM}		double m[16];
// COMMENT: {1/27/2011 3:19:01 PM}		vtkMatrix4x4 *m4x4 = trans->GetMatrix();
// COMMENT: {1/27/2011 3:19:01 PM}		vtkMatrix4x4::DeepCopy(m, m4x4);	
// COMMENT: {1/27/2011 3:19:01 PM}		trans->Delete();
	}

	//
	// Map to Grid
	//
	{
		
		double scale_h = map_horz_to_si / horz_to_si;
		double scale_v = map_vert_to_si / vert_to_si;
		PHAST_Transform map_to_grid(origin[0], origin[1], origin[2], angle, scale_h, scale_h, scale_v);

		vtkTransform *trans = vtkTransform::New();
		trans->Scale(scale_h, scale_h, scale_v);
		trans->RotateZ(-angle);
		trans->Translate(-origin[0], -origin[1], -origin[2]);

		double m[16];
		vtkMatrix4x4 *m4x4 = trans->GetMatrix();
		vtkMatrix4x4::DeepCopy(m, m4x4);	
		trans->Delete();

		CPPUNIT_ASSERT_DOUBLES_EQUAL(m[0],  map_to_grid.trans(0, 0), ::pow((double)10, (double)-FLT_DIG));
		CPPUNIT_ASSERT_DOUBLES_EQUAL(m[1],  map_to_grid.trans(0, 1), ::pow((double)10, (double)-FLT_DIG));
		CPPUNIT_ASSERT_DOUBLES_EQUAL(m[2],  map_to_grid.trans(0, 2), ::pow((double)10, (double)-FLT_DIG));
		CPPUNIT_ASSERT_DOUBLES_EQUAL(m[3],  map_to_grid.trans(0, 3), ::pow((double)10, (double)-FLT_DIG));

		CPPUNIT_ASSERT_DOUBLES_EQUAL(m[4],  map_to_grid.trans(1, 0), ::pow((double)10, (double)-FLT_DIG));
		CPPUNIT_ASSERT_DOUBLES_EQUAL(m[5],  map_to_grid.trans(1, 1), ::pow((double)10, (double)-FLT_DIG));
		CPPUNIT_ASSERT_DOUBLES_EQUAL(m[6],  map_to_grid.trans(1, 2), ::pow((double)10, (double)-FLT_DIG));
		CPPUNIT_ASSERT_DOUBLES_EQUAL(m[7],  map_to_grid.trans(1, 3), ::pow((double)10, (double)-FLT_DIG));

		CPPUNIT_ASSERT_DOUBLES_EQUAL(m[8],  map_to_grid.trans(2, 0), ::pow((double)10, (double)-FLT_DIG));
		CPPUNIT_ASSERT_DOUBLES_EQUAL(m[9],  map_to_grid.trans(2, 1), ::pow((double)10, (double)-FLT_DIG));
		CPPUNIT_ASSERT_DOUBLES_EQUAL(m[10], map_to_grid.trans(2, 2), ::pow((double)10, (double)-FLT_DIG));
		CPPUNIT_ASSERT_DOUBLES_EQUAL(m[11], map_to_grid.trans(2, 3), ::pow((double)10, (double)-FLT_DIG));

		CPPUNIT_ASSERT_DOUBLES_EQUAL(m[12], map_to_grid.trans(3, 0), ::pow((double)10, (double)-FLT_DIG));
		CPPUNIT_ASSERT_DOUBLES_EQUAL(m[13], map_to_grid.trans(3, 1), ::pow((double)10, (double)-FLT_DIG));
		CPPUNIT_ASSERT_DOUBLES_EQUAL(m[14], map_to_grid.trans(3, 2), ::pow((double)10, (double)-FLT_DIG));
		CPPUNIT_ASSERT_DOUBLES_EQUAL(m[15], map_to_grid.trans(3, 3), ::pow((double)10, (double)-FLT_DIG));
	}
}

void TestPHAST_Transform::RotateGrid(void)
{
	double origin[3] = {4., 3., 0.};
	double angle     = 36.869897645844021296855612559093;
	double xcoord[2] = {2., 4.};
	double ycoord[2] = {1., 2.};
	double zcoord[2] = {0., 1.};

	double midpt[3];
	midpt[0] = (xcoord[0] + xcoord[1]) / 2.0;
	midpt[1] = (ycoord[0] + ycoord[1]) / 2.0;
	midpt[2] = (zcoord[0] + zcoord[1]) / 2.0;

	double zero[3] = {0., 0., 0.};
	double out[3];

	double zrot = 53.130102354155978703144387440907;

	vtkTransform *map2grid = vtkTransform::New();
	map2grid->Scale(1, 1, 1);
	map2grid->RotateZ(-angle);
	map2grid->Translate(-origin[0], -origin[1], -origin[2]);
	map2grid->Inverse();

	vtkTransform *grid2map = map2grid;

	double midptGrid[3];
	grid2map->TransformPoint(midpt, midptGrid);

	vtkTransform *t = vtkTransform::New();
	t->Translate(midptGrid[0], midptGrid[1], midptGrid[2]);
	t->Inverse();

	double in[3];
	in[0] = origin[0];
	in[1] = origin[1];
	in[2] = origin[2];
	t->TransformPoint(in, out);

	double rout[3];

	vtkTransform *r = vtkTransform::New();
	r->RotateZ(zrot);
	r->TransformPoint(out, rout);

	double neworig[3];
	t->Inverse();
	t->TransformPoint(rout, neworig);

	r->Delete();
	t->Delete();
	map2grid->Delete();
}

void TestPHAST_Transform::RotateGrid2(void)
{
	double map_horz_to_si = 1.;
	double horz_to_si     = 0.3048;

	double map_vert_to_si = 1.;
	double vert_to_si     = 0.3048;

	double grid_origin[3] = { 4. , 3., 0. };
	double grid_angle = 36.869897645844021296855612559093;


	// Set up transform
	double scale_h = 1;
	double scale_v = 1;
	scale_h = map_horz_to_si / horz_to_si;
	scale_v = map_vert_to_si / vert_to_si;

	PHAST_Transform map_to_grid = PHAST_Transform(grid_origin[0], grid_origin[1], grid_origin[2],
		grid_angle, scale_h, scale_h, scale_v);

	Point g1(5., 5., 0.);
	Point g2(6., 7., 1.);

	map_to_grid.Transform(g1);
	map_to_grid.Transform(g2);

	TRACE("g1 = (%g, %g, %g)\n", g1.get_coord()[0], g1.get_coord()[1], g1.get_coord()[2]);
	TRACE("g2 = (%g, %g, %g)\n", g2.get_coord()[0], g2.get_coord()[1], g2.get_coord()[2]);

	Point r1(5.2, 6.4, 0.);
	Point r2(6.0, 7.0, 1.);
	Point r3(6.6, 6.2, 1.);

	map_to_grid.Transform(r1);
	map_to_grid.Transform(r2);
	map_to_grid.Transform(r3);

	TRACE("r1 = (%g, %g, %g)\n", r1.get_coord()[0], r1.get_coord()[1], r1.get_coord()[2]);
	TRACE("r2 = (%g, %g, %g)\n", r2.get_coord()[0], r2.get_coord()[1], r2.get_coord()[2]);
	TRACE("r3 = (%g, %g, %g)\n", r3.get_coord()[0], r3.get_coord()[1], r3.get_coord()[2]);

	Point p1(3.99, 5.61, 0);
	Point p2(4.57, 5.76, 0);
	Point p3(5.03, 6.11, 0);
	Point p4(4.62, 6.67, 0);

	map_to_grid.Transform(p1);
	map_to_grid.Transform(p2);
	map_to_grid.Transform(p3);
	map_to_grid.Transform(p4);

	TRACE("p1 = (%g, %g, %g)\n", p1.get_coord()[0], p1.get_coord()[1], p1.get_coord()[2]);
	TRACE("p2 = (%g, %g, %g)\n", p2.get_coord()[0], p2.get_coord()[1], p2.get_coord()[2]);
	TRACE("p3 = (%g, %g, %g)\n", p3.get_coord()[0], p3.get_coord()[1], p3.get_coord()[2]);
	TRACE("p4 = (%g, %g, %g)\n", p4.get_coord()[0], p4.get_coord()[1], p4.get_coord()[2]);

	PHAST_Transform orig(grid_origin[0], grid_origin[1], grid_origin[2],
		grid_angle, 1., 1., 1.);

	Point t(0, 0, 0.75);
	Point b(0, 0, 0.25);

	map_to_grid.Transform(t);
	map_to_grid.Transform(b);
	TRACE("t = (%g, %g, %g)\n", t.get_coord()[0], t.get_coord()[1], t.get_coord()[2]);
	TRACE("b = (%g, %g, %g)\n", b.get_coord()[0], b.get_coord()[1], b.get_coord()[2]);


	Point zf21(1.7403472661972, 0.885080754756927, 0);
	Point zf22(3.35031771659851, 1.21303772926331, 1);

	orig.Inverse_transform(zf21);
	orig.Inverse_transform(zf22);
	map_to_grid.Transform(zf21);
	map_to_grid.Transform(zf22);
	TRACE("zf21 = (%g, %g, %g)\n", zf21.get_coord()[0], zf21.get_coord()[1], zf21.get_coord()[2]);
	TRACE("zf22 = (%g, %g, %g)\n", zf22.get_coord()[0], zf22.get_coord()[1], zf22.get_coord()[2]);

	Point b21(2.2, 1.2, 0);
	Point b22(2.6, 1.6, 1);

	orig.Inverse_transform(b21);
	orig.Inverse_transform(b22);
	map_to_grid.Transform(b21);
	map_to_grid.Transform(b22);
	TRACE("b21 = (%g, %g, %g)\n", b21.get_coord()[0], b21.get_coord()[1], b21.get_coord()[2]);
	TRACE("b22 = (%g, %g, %g)\n", b22.get_coord()[0], b22.get_coord()[1], b22.get_coord()[2]);


	Point b31(6, 6, 0);
	Point b32(8, 7, 1);

	orig.Inverse_transform(b31);
	orig.Inverse_transform(b32);
	map_to_grid.Transform(b31);
	map_to_grid.Transform(b32);
	TRACE("b31 = (%g, %g, %g)\n", b31.get_coord()[0], b31.get_coord()[1], b31.get_coord()[2]);
	TRACE("b32 = (%g, %g, %g)\n", b32.get_coord()[0], b32.get_coord()[1], b32.get_coord()[2]);


}