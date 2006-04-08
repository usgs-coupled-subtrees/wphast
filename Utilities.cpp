#include "StdAfx.h"
#include "Utilities.h"

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
