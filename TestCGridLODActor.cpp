#include "StdAfx.h"
#include "TestCGridLODActor.h"

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

#include "GridLODActor.h"
#include "Units.h"
#include "MapActor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void TestCGridLODActor::testOK(void)
{
	CGridLODActor *actor = CGridLODActor::New();
	CPPUNIT_ASSERT(actor != NULL);

	CGrid x, y, z;
    x.SetUniformRange(0, 90000, 16);
	y.SetUniformRange(0, 48000, 9);
	z.SetUniformRange(0, 400, 5);
	CUnits units;

	actor->SetGrid(x, y, z, units);

// COMMENT: {3/17/2004 5:16:22 PM}	CMapActor *actor = CMapActor::New();
// COMMENT: {3/17/2004 5:16:22 PM}	CPPUNIT_ASSERT(actor != NULL);
// COMMENT: {3/17/2004 5:16:22 PM}	CPPUNIT_ASSERT(actor->SetFileName("ok1.bmp") == 1);
// COMMENT: {3/17/2004 5:16:22 PM}	CPPUNIT_ASSERT(actor->SetWorldFileName("ok1.bpw") == 1);
// COMMENT: {3/17/2004 5:16:22 PM}
// COMMENT: {3/17/2004 5:16:22 PM}	double xPos = 621099.81;
// COMMENT: {3/17/2004 5:16:22 PM}	double yPos = 3896414.25;
// COMMENT: {3/17/2004 5:16:22 PM}	double zPos = 400.0;
// COMMENT: {3/17/2004 5:16:22 PM}	CPPUNIT_ASSERT(actor->PlaceMap(xPos, yPos, zPos, 0.0) == 1);



	// graphics stuff
	vtkRenderer *renderer = vtkRenderer::New();

	vtkRenderWindow *renWin = vtkRenderWindow::New();
	renWin->AddRenderer(renderer);

	vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
	iren->SetRenderWindow(renWin);

	// set up renderer
	renderer->AddActor(actor);
  
	//renderer->SetBackground(1,1,1);
	renWin->SetSize(300,300);

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

}
