#include "StdAfx.h"
#include "Utilities.h"
#include <sstream>

double CUtilities::GetDlgItemDouble(CWnd* pWnd, UINT nID)
{
	ASSERT(pWnd);
	if (pWnd)
	{
		CWnd* child;
		if (child = pWnd->GetDlgItem(nID))
		{
			CString str;
			child->GetWindowText(str);
			return CUtilities::ParseDouble(str);
		}
	}
	throw CUtilities::ParseException();
}

double CUtilities::ParseDouble(LPCSTR lpszString)
{
	double d;
	if (lpszString && ::strlen(lpszString) && ::_stscanf(lpszString, _T("%lf"), &d))
	{
		return d;
	}
	throw CUtilities::ParseException();
}

void CUtilities::Subdivide(std::vector<double> &values, double start, double end, int numSubInterval, double multiplier)
{
	double delta;
    if (multiplier == 1)
	{
        delta = (end - start)/numSubInterval;
    }
	else
	{
        delta = (end - start)*(multiplier - 1) /
			(::pow(multiplier, numSubInterval) - 1);
    }
	values.clear();
	values.push_back(start);
    for (int i=1; i<numSubInterval; i++)
	{
        start += delta;
		values.push_back(start);
        delta *= multiplier;
    }
	values.push_back(end);
}

void CUtilities::GetWorldPointAtFixedPlane(vtkRenderWindowInteractor *interactor, vtkRenderer *renderer, int fixed, float value, float point[3])
{
	// fixed 
	//   0 => X
	//   1 => Y
	//   2 => Z
	//
	ASSERT(0 <= fixed && fixed < 3);

	int i;

	// get the position of the mouse cursor in screen/window coordinates
	// (pixel)
	int* pos = interactor->GetEventPosition();
#ifdef _DEBUG
// COMMENT: {5/5/2006 5:10:56 PM}	TRACE("pos=%d,%d\n", pos[0], pos[1]);
// COMMENT: {5/5/2006 5:10:56 PM}	int *winSize = renderer->GetRenderWindow()->GetSize();
// COMMENT: {5/5/2006 5:10:56 PM}	if (!::GetCapture())
// COMMENT: {5/5/2006 5:10:56 PM}	{
// COMMENT: {5/5/2006 5:10:56 PM}		ASSERT(0 <= pos[0] && pos[0] <= winSize[0]);
// COMMENT: {5/5/2006 5:10:56 PM}		ASSERT(0 <= pos[1] && pos[1] <= winSize[1]);
// COMMENT: {5/5/2006 5:10:56 PM}	}
// COMMENT: {5/5/2006 5:10:56 PM}	TRACE("z(%d,%d)=%g\n", winSize[0]/2, winSize[1]/2, renderer->GetZ(winSize[0]/2, winSize[0]/2));
#endif

	// get the focal point in world coordinates
	//
	vtkCamera *camera = renderer->GetActiveCamera();	
	vtkFloatingPointType cameraFP[4];
	camera->GetFocalPoint((vtkFloatingPointType*)cameraFP); cameraFP[3] = 1.0;
#ifdef _DEBUG
// COMMENT: {5/5/2006 5:11:01 PM}	TRACE("fp=%g,%g,%g\n", cameraFP[0], cameraFP[1], cameraFP[2]);
#endif

	// Convert the focal point coordinates to display (or screen) coordinates.
	//
	renderer->SetWorldPoint(cameraFP);
	renderer->WorldToDisplay();
	vtkFloatingPointType *displayCoords = renderer->GetDisplayPoint();
#ifdef _DEBUG
// COMMENT: {5/5/2006 5:11:05 PM}	float dCoord[3];
// COMMENT: {5/5/2006 5:11:05 PM}	dCoord[0] = displayCoords[0];
// COMMENT: {5/5/2006 5:11:05 PM}	dCoord[1] = displayCoords[1];
// COMMENT: {5/5/2006 5:11:05 PM}	dCoord[2] = displayCoords[2];
// COMMENT: {5/5/2006 5:11:05 PM}	TRACE("displayCoords=%g,%g,%g\n", displayCoords[0], displayCoords[1], displayCoords[2]);
#endif

	// Convert the selection point into world coordinates.
	//
	renderer->SetDisplayPoint(pos[0], pos[1], displayCoords[2]);
	renderer->DisplayToWorld();
	vtkFloatingPointType *worldCoords = renderer->GetWorldPoint();
	if ( worldCoords[3] == 0.0 )
	{
		ASSERT(FALSE);
		return;
	}
#ifdef _DEBUG
// COMMENT: {5/5/2006 4:56:04 PM}// COMMENT: {5/5/2006 4:35:39 PM}	TRACE("worldCoords(%g)=%g,%g,%g\n", displayCoords[2], worldCoords[0], worldCoords[1], worldCoords[2]);
// COMMENT: {5/5/2006 4:56:04 PM}// COMMENT: {5/5/2006 4:35:39 PM}	renderer->SetDisplayPoint(pos[0], pos[1], 0.0);
// COMMENT: {5/5/2006 4:56:04 PM}// COMMENT: {5/5/2006 4:35:39 PM}	renderer->DisplayToWorld();
// COMMENT: {5/5/2006 4:56:04 PM}// COMMENT: {5/5/2006 4:35:39 PM}	worldCoords = renderer->GetWorldPoint();
// COMMENT: {5/5/2006 4:56:04 PM}// COMMENT: {5/5/2006 4:35:39 PM}	TRACE("worldCoords(0.0)=%g,%g,%g\n", worldCoords[0], worldCoords[1], worldCoords[2]);
// COMMENT: {5/5/2006 4:56:04 PM}	renderer->SetDisplayPoint(dCoord[0], dCoord[1], dCoord[2]);
// COMMENT: {5/5/2006 4:56:04 PM}	renderer->DisplayToWorld();
// COMMENT: {5/5/2006 4:56:04 PM}	worldCoords = renderer->GetWorldPoint();
// COMMENT: {5/5/2006 4:56:04 PM}	TRACE("worldCoords(%g,%g,%g)=%g,%g,%g\n", dCoord[0], dCoord[1], dCoord[2], worldCoords[0], worldCoords[1], worldCoords[2]);
#endif
	vtkFloatingPointType pickPosition[3];
	for (i = 0; i < 3; ++i)
	{
		pickPosition[i] = worldCoords[i] / worldCoords[3];
	}

	if ( camera->GetParallelProjection() )
	{
		double* cameraDOP = camera->GetDirectionOfProjection();
		double t = (value - pickPosition[fixed]) / cameraDOP[fixed];
		for (i = 0; i < 3; ++i)
		{
			if ( i != fixed )
			{
				point[i] = pickPosition[i] + t * cameraDOP[i];
			}
		}
	}
	else
	{
		double *cameraPos = camera->GetPosition();
		double t = (value - cameraPos[fixed]) / ( pickPosition[fixed] - cameraPos[fixed] );
		for (i = 0; i < 3; ++i)
		{
			if ( i != fixed )
			{
				point[i] = cameraPos[i] + t * ( pickPosition[i] - cameraPos[i] );
			}
		}
	}
	point[fixed] = value;
}

void CUtilities::GetWorldPointAtFixedPlane(vtkRenderWindowInteractor *interactor, vtkRenderer *renderer, int fixed, double value, double point[3])
{
	// fixed 
	//   0 => X
	//   1 => Y
	//   2 => Z
	//
	ASSERT(0 <= fixed && fixed < 3);

	int i;

	// get the position of the mouse cursor in screen/window coordinates
	// (pixel)
	int* pos = interactor->GetEventPosition();
#ifdef _DEBUG
// COMMENT: {5/5/2006 5:10:56 PM}	TRACE("pos=%d,%d\n", pos[0], pos[1]);
// COMMENT: {5/5/2006 5:10:56 PM}	int *winSize = renderer->GetRenderWindow()->GetSize();
// COMMENT: {5/5/2006 5:10:56 PM}	if (!::GetCapture())
// COMMENT: {5/5/2006 5:10:56 PM}	{
// COMMENT: {5/5/2006 5:10:56 PM}		ASSERT(0 <= pos[0] && pos[0] <= winSize[0]);
// COMMENT: {5/5/2006 5:10:56 PM}		ASSERT(0 <= pos[1] && pos[1] <= winSize[1]);
// COMMENT: {5/5/2006 5:10:56 PM}	}
// COMMENT: {5/5/2006 5:10:56 PM}	TRACE("z(%d,%d)=%g\n", winSize[0]/2, winSize[1]/2, renderer->GetZ(winSize[0]/2, winSize[0]/2));
#endif

	// get the focal point in world coordinates
	//
	vtkCamera *camera = renderer->GetActiveCamera();	
	double cameraFP[4];
	camera->GetFocalPoint(cameraFP); cameraFP[3] = 1.0;
#ifdef _DEBUG
// COMMENT: {5/5/2006 5:11:01 PM}	TRACE("fp=%g,%g,%g\n", cameraFP[0], cameraFP[1], cameraFP[2]);
#endif

	// Convert the focal point coordinates to display (or screen) coordinates.
	//
	renderer->SetWorldPoint(cameraFP[0], cameraFP[1], cameraFP[2], cameraFP[3]);
	renderer->WorldToDisplay();
// COMMENT: {8/26/2008 9:41:57 PM}	vtkFloatingPointType *displayCoords = renderer->GetDisplayPoint();
	double displayCoords[3];
	renderer->GetDisplayPoint(displayCoords);
#ifdef _DEBUG
// COMMENT: {5/5/2006 5:11:05 PM}	float dCoord[3];
// COMMENT: {5/5/2006 5:11:05 PM}	dCoord[0] = displayCoords[0];
// COMMENT: {5/5/2006 5:11:05 PM}	dCoord[1] = displayCoords[1];
// COMMENT: {5/5/2006 5:11:05 PM}	dCoord[2] = displayCoords[2];
// COMMENT: {5/5/2006 5:11:05 PM}	TRACE("displayCoords=%g,%g,%g\n", displayCoords[0], displayCoords[1], displayCoords[2]);
#endif

	// Convert the selection point into world coordinates.
	//
	renderer->SetDisplayPoint(pos[0], pos[1], displayCoords[2]);
	renderer->DisplayToWorld();
// COMMENT: {8/26/2008 9:43:19 PM}	vtkFloatingPointType *worldCoords = renderer->GetWorldPoint();
	double worldCoords[4];
	renderer->GetWorldPoint(worldCoords);
	if ( worldCoords[3] == 0.0 )
	{
		ASSERT(FALSE);
		return;
	}
#ifdef _DEBUG
// COMMENT: {5/5/2006 4:56:04 PM}// COMMENT: {5/5/2006 4:35:39 PM}	TRACE("worldCoords(%g)=%g,%g,%g\n", displayCoords[2], worldCoords[0], worldCoords[1], worldCoords[2]);
// COMMENT: {5/5/2006 4:56:04 PM}// COMMENT: {5/5/2006 4:35:39 PM}	renderer->SetDisplayPoint(pos[0], pos[1], 0.0);
// COMMENT: {5/5/2006 4:56:04 PM}// COMMENT: {5/5/2006 4:35:39 PM}	renderer->DisplayToWorld();
// COMMENT: {5/5/2006 4:56:04 PM}// COMMENT: {5/5/2006 4:35:39 PM}	worldCoords = renderer->GetWorldPoint();
// COMMENT: {5/5/2006 4:56:04 PM}// COMMENT: {5/5/2006 4:35:39 PM}	TRACE("worldCoords(0.0)=%g,%g,%g\n", worldCoords[0], worldCoords[1], worldCoords[2]);
// COMMENT: {5/5/2006 4:56:04 PM}	renderer->SetDisplayPoint(dCoord[0], dCoord[1], dCoord[2]);
// COMMENT: {5/5/2006 4:56:04 PM}	renderer->DisplayToWorld();
// COMMENT: {5/5/2006 4:56:04 PM}	worldCoords = renderer->GetWorldPoint();
// COMMENT: {5/5/2006 4:56:04 PM}	TRACE("worldCoords(%g,%g,%g)=%g,%g,%g\n", dCoord[0], dCoord[1], dCoord[2], worldCoords[0], worldCoords[1], worldCoords[2]);
#endif
	double pickPosition[3];
	for (i = 0; i < 3; ++i)
	{
		pickPosition[i] = worldCoords[i] / worldCoords[3];
	}

	if ( camera->GetParallelProjection() )
	{
		double* cameraDOP = camera->GetDirectionOfProjection();
		double t = (value - pickPosition[fixed]) / cameraDOP[fixed];
		for (i = 0; i < 3; ++i)
		{
			if ( i != fixed )
			{
				point[i] = pickPosition[i] + t * cameraDOP[i];
			}
		}
	}
	else
	{
		double *cameraPos = camera->GetPosition();
		double t = (value - cameraPos[fixed]) / ( pickPosition[fixed] - cameraPos[fixed] );
		for (i = 0; i < 3; ++i)
		{
			if ( i != fixed )
			{
				point[i] = cameraPos[i] + t * ( pickPosition[i] - cameraPos[i] );
			}
		}
	}
	point[fixed] = value;
}


void CUtilities::VTK_dump(vtkObject* obj)
{
#if defined(_DEBUG)
	std::ostringstream oss;
	obj->PrintSelf(oss, 4);
	TRACE("%s\n", oss.str().c_str());
#endif
}
