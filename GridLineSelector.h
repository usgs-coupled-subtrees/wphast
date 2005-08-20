#pragma once
//#include <vtkInteractorObserver.h>
#include <vtk3DWidget.h>

class CGridLineSelector : public vtk3DWidget
{
public:
	// Description:
	// Instantiate the object.
	//static CGridLineSelector *New();

	vtkTypeRevisionMacro(CGridLineSelector, vtk3DWidget);
	//void PrintSelf(ostream& os, vtkIndent indent);

// COMMENT: {8/19/2005 6:39:40 PM}	// Description:
// COMMENT: {8/19/2005 6:39:40 PM}	// Methods that satisfy the superclass' API.
// COMMENT: {8/19/2005 6:39:40 PM}	virtual void SetEnabled(int);
// COMMENT: {8/19/2005 6:39:40 PM}	virtual void PlaceWidget(double bounds[6]);
// COMMENT: {8/19/2005 6:39:40 PM}	void PlaceWidget()
// COMMENT: {8/19/2005 6:39:40 PM}	{this->Superclass::PlaceWidget();}
// COMMENT: {8/19/2005 6:39:40 PM}	void PlaceWidget(double xmin, double xmax, double ymin, double ymax, 
// COMMENT: {8/19/2005 6:39:40 PM}		double zmin, double zmax)
// COMMENT: {8/19/2005 6:39:40 PM}	{this->Superclass::PlaceWidget(xmin,xmax,ymin,ymax,zmin,zmax);}


protected:
	CGridLineSelector(void);
	virtual ~CGridLineSelector(void);

private:
	CGridLineSelector(const CGridLineSelector&);  // Not implemented.
	void operator=(const CGridLineSelector&);     // Not implemented.

};
