#pragma once

#include <vector>

class CUtilities
{
public:
	static double GetDlgItemDouble(CWnd* pWnd, UINT nID);

	static void Subdivide(std::vector<double> &values, double start, double end, int numSubInterval, double multiplier);

	static double ParseDouble(LPCSTR lpszString);

	static void GetWorldPointAtFixedPlane(vtkRenderWindowInteractor *interactor, vtkRenderer *renderer, int fixed, float value, float point[3]);
	static void GetWorldPointAtFixedPlane(vtkRenderWindowInteractor *interactor, vtkRenderer *renderer, int fixed, double value, double point[3]);

	static void VTK_dump(vtkObject* obj);

public:
	class ParseException
	{
	};
};
