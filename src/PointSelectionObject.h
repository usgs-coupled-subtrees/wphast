#pragma once

// CPointSelectionObject command target

class CPointSelectionObject : public CObject
{
	DECLARE_DYNAMIC(CPointSelectionObject)
public:
	CPointSelectionObject(int n);
	virtual ~CPointSelectionObject();

	int GetPoint();
	void SetPoint(int n);
protected:
	int Point;
};


