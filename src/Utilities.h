#pragma once

#include <vector>

#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif

class CUtilities
{
public:
	static double GetDlgItemDouble(CWnd* pWnd, UINT nID);

	static void Subdivide(std::vector<double> &values, double start, double end, int numSubInterval, double multiplier);

	static double ParseDouble(LPCSTR lpszString);

	static void GetWorldPointAtFixedPlane(vtkRenderWindowInteractor *interactor, vtkRenderer *renderer, int fixed, vtkFloatingPointType value, vtkFloatingPointType point[3]);

	static void VTK_dump(vtkObject* obj);

public:
	class ParseException
	{
	};
};
