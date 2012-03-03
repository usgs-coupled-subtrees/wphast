#include "StdAfx.h"
#include "TestCMapActor.h"

#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTubeFilter.h>
#include <vtkActor.h>
#include <vtkAxes.h>
#include <vtkPolyDataMapper.h>
#include <vtkBMPReader.h>
#include <vtkTexture.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>

#include "MapActor.h"

#include "GridLODActor.h"
#include "Units.h"
#include "SiteMap3.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void TestCMapActor::testSpin(void)
{
// COMMENT: {3/15/2004 8:55:28 PM}	vtkBMPReader *reader = vtkBMPReader::New();
// COMMENT: {3/15/2004 8:55:28 PM}	reader->SetFileName("ok1.bmp");
// COMMENT: {3/15/2004 8:55:28 PM}
// COMMENT: {3/15/2004 8:55:28 PM}	vtkTexture *texture = vtkTexture::New();
// COMMENT: {3/15/2004 8:55:28 PM}	texture->SetInput(reader->GetOutput());
// COMMENT: {3/15/2004 8:55:28 PM}
// COMMENT: {3/15/2004 8:55:28 PM}	vtkPlaneSource *plane = vtkPlaneSource::New();
// COMMENT: {3/15/2004 8:55:28 PM}
// COMMENT: {3/15/2004 8:55:28 PM}	vtkPolyDataMapper *planeMapper = vtkPolyDataMapper::New();
// COMMENT: {3/15/2004 8:55:28 PM}	planeMapper->SetInput(plane->GetOutput());
// COMMENT: {3/15/2004 8:55:28 PM}
// COMMENT: {3/15/2004 8:55:28 PM}	vtkActor *actor = vtkActor::New();
// COMMENT: {3/15/2004 8:55:28 PM}	actor->SetMapper(planeMapper);
// COMMENT: {3/15/2004 8:55:28 PM}	actor->SetTexture(texture);

	CMapActor *actor = CMapActor::New();
	CPPUNIT_ASSERT(actor != NULL);

	CSiteMap3 siteMap3;
	siteMap3.FileName      = "ok1.bmp";

	CWorldTransform wtrans;
	wtrans.LoadWorldFile("ok1.bpw");
	siteMap3.SetWorldTransform(wtrans);
	// siteMap.SetWorldFileName("ok1.bpw"); // siteMap.m_worldFileName = "ok1.bpw";

	actor->SetSiteMap3(siteMap3); // may throw
// COMMENT: {5/20/2004 2:54:06 PM}	CPPUNIT_ASSERT(actor->SetFileName("ok1.bmp") == 1);
// COMMENT: {5/20/2004 2:54:06 PM}	CPPUNIT_ASSERT(actor->SetWorldFileName("ok1.bpw") == 1);

	/**
	double xPos = 621099.81;
	double yPos = 3896414.25;
	double angl = 0.0;
	double zPos = 400.0;

	double xPos = 613756.5;
	double yPos = 3907288.25;
	double angl = 9.53;
	double zPos = 400.0;

	double xPos = 620700.69;
	double yPos = 3915630.25;
	double angl = 0.0;
	double zPos = 400.0;

	double xPos = 668243.88;
	double yPos = 3915674;
	double angl = 90;
	double zPos = 400.0;

	double xPos = 614488.81;
	double yPos = 3846117.75;
	double angl = 15;
	double zPos = 400.0;

	double xPos = 584597.94;
	double yPos = 3880072.5;
	double angl = -15;
	double zPos = 400.0;
	**/


	double xPos = 584597.94;
	double yPos = 3880072.5;
	double angl = -15;
	double zPos = 400.0;
	// TODO change to SetSiteMap CPPUNIT_ASSERT(actor->PlaceMap(xPos, yPos, zPos, angl) == 1);

	//{{
	CGridLODActor *grid = CGridLODActor::New();
	CPPUNIT_ASSERT(grid != NULL);

	/**
	double lengt = 90000;
	double width = 48000;

	double lengt = 92136.08;
	double width = 18920.84;

	double lengt = 47426.19;
	double width = 39377.5;

	double lengt = 39337.75;
	double width = 47573.13;

	double lengt = 121517.63;
	double width = 112444.01;

	double lengt = 125549.71;
	double width = 108311.71;
	**/
	double lengt = 125549.71;
	double width = 108311.71;

	CGrid x, y, z;
    x.SetUniformRange(0, lengt, 16);
	y.SetUniformRange(0, width, 9);
	z.SetUniformRange(0, 400, 5);
// COMMENT: {3/18/2004 3:52:49 PM}    x.SetUniformRange(0, 2, 16);
// COMMENT: {3/18/2004 3:52:49 PM}	y.SetUniformRange(0, 1, 9);
// COMMENT: {3/18/2004 3:52:49 PM}	z.SetUniformRange(0, .1, 5);
	CUnits units;

	grid->SetGrid(x, y, z, units);
	grid->GetProperty()->SetColor(1.0, 0.8, 0.6);
	//}}

	// graphics stuff
	vtkRenderer *renderer = vtkRenderer::New();

	vtkRenderWindow *renWin = vtkRenderWindow::New();
	renWin->AddRenderer(renderer);

	vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
	iren->SetRenderWindow(renWin);

	// set up renderer
	actor->SetScale(100, 100, 100);
	grid->SetScale(100, 100, 100);

	renderer->AddActor(actor);
	renderer->AddActor(grid);

	actor->SetScale(1, 1, 1);
	grid->SetScale(1, 1, 1);

  
// COMMENT: {3/17/2004 5:29:11 PM}	renderer->SetBackground(1,1,1);
	renWin->SetSize(600, 600);

	// interact with data
	iren->Initialize();

	renWin->Render();
	iren->Start();

	// Clean up
	renderer->Delete();
	renWin->Delete();
	iren->Delete();

// COMMENT: {3/15/2004 8:56:19 PM}	reader->Delete();
// COMMENT: {3/15/2004 8:56:19 PM}	texture->Delete();
// COMMENT: {3/15/2004 8:56:19 PM}	plane->Delete();
// COMMENT: {3/15/2004 8:56:19 PM}	planeMapper->Delete();

	actor->Delete();
	grid->Delete();
}