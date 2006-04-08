#pragma once

#include <vector>

class CUtilities
{
public:
	static double GetDlgItemDouble(CWnd* pWnd, UINT nID);

	static void Subdivide(std::vector<double> &values, double start, double end, int numSubInterval, double multiplier);

	static double ParseDouble(LPCSTR lpszString);


public:
	class ParseException
	{
	};
};
