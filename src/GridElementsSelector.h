#pragma once
//#include <vtkInteractorObserver.h>
#include <vtk3DWidget.h>
#include <vtkBoxWidget.h>

#include "GridKeyword.h"

class CGridActor;
class CWPhastDoc;

#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif

class CGridElementsSelector : public vtk3DWidget
{
public:
	// Description:
	// Instantiate the object.
	static CGridElementsSelector *New();

	vtkTypeRevisionMacro(CGridElementsSelector, vtk3DWidget);
	//void PrintSelf(ostream& os, vtkIndent indent);

	// Description:
	// Methods that satisfy the superclass' API.
	virtual void SetEnabled(int);
	virtual void PlaceWidget(vtkFloatingPointType bounds[6]);
	void PlaceWidget()
	{this->Superclass::PlaceWidget();}
	void PlaceWidget(vtkFloatingPointType xmin, vtkFloatingPointType xmax, vtkFloatingPointType ymin, vtkFloatingPointType ymax, 
		vtkFloatingPointType zmin, vtkFloatingPointType zmax)
	{this->Superclass::PlaceWidget(xmin,xmax,ymin,ymax,zmin,zmax);}

	//
	vtkSetMacro(GridActor,CGridActor*);
	vtkGetMacro(GridActor,CGridActor*);

	//
	vtkSetMacro(SelectElementIntersection, int);
	vtkGetMacro(SelectElementIntersection, int);

	//
	vtkSetMacro(Document, CWPhastDoc*);
	vtkGetMacro(Document, CWPhastDoc*);

	void SetIBounds(int imin, int imax, int jmin, int jmax, int kmin, int kmax);
	void SetIBounds(int ibounds[6]);

	void GetIBounds(int &imin, int &imax, int &jmin, int &jmax, int &kmin, int &kmax);
	void GetIBounds(int ibounds[6]);

	//vtkSetVector3Macro(Max, int);
	//void SetMax(int i, int j, int k);
	//void SetMax(int max[3]) { this->SetMax(max[0], max[0], max[0]); }

	vtkGetVector3Macro(Min, int);
// COMMENT: {8/25/2005 8:21:42 PM}	virtual void GetMin(int &_arg1, int &_arg2, int &_arg3)
// COMMENT: {8/25/2005 8:21:42 PM}	{
// COMMENT: {8/25/2005 8:21:42 PM}		_arg1 = this->Min[0];
// COMMENT: {8/25/2005 8:21:42 PM}		_arg2 = this->Min[1];
// COMMENT: {8/25/2005 8:21:42 PM}		_arg3 = this->Min[2];
// COMMENT: {8/25/2005 8:21:42 PM}	};
// COMMENT: {8/25/2005 8:21:42 PM}	virtual void GetMin(int _arg[3])
// COMMENT: {8/25/2005 8:21:42 PM}	{
// COMMENT: {8/25/2005 8:21:42 PM}		this->GetMin(_arg[0], _arg[1], _arg[2]);
// COMMENT: {8/25/2005 8:21:42 PM}	}
	vtkGetVector3Macro(Max, int);

	//vtkSetVector3Macro(Min, int);
	//vtkGetVector3Macro(Min, int);

	//void SetMax(int max[3]);
	//void GetMax(int max[3])const;

	void UpdateGrid(void);


protected:
	CGridElementsSelector(void);
	//virtual ~CGridElementsSelector(void);
	~CGridElementsSelector(void);

	int State;
	enum WidgetState
	{
		Start=0,
		Selecting,
	};

	//
	CGridActor  *GridActor;

	// 
	CWPhastDoc  *Document;

	// element selection
	vtkCubeSource     *Source;
	vtkPolyDataMapper *Mapper;
	vtkActor          *Actor;

	// wireframe outline
	vtkActor             *OutlineActor;
	vtkPolyDataMapper    *OutlineMapper;
	vtkOutlineSource     *OutlineSource;
	vtkFloatingPointType  StartPoint[3];

	int FixedPlane;  // [0-5]
	int FixedCoord;  // [0-2]

	int Min[3];
	int Max[3];

	CGridKeyword          GridKeyword;

	int SelectElementIntersection;


	// Handles the events
	static void ProcessEvents(vtkObject* object, 
		unsigned long event,
		void* clientdata, 
		void* calldata);

	// ProcessEvents() dispatches to these methods.
	void OnMouseMove();
	void OnLeftButtonDown();
	void OnLeftButtonUp();
// COMMENT: {8/22/2005 3:46:36 PM}	void OnMiddleButtonDown();
// COMMENT: {8/22/2005 3:46:36 PM}	void OnMiddleButtonUp();
// COMMENT: {8/22/2005 3:46:36 PM}	void OnRightButtonDown();
// COMMENT: {8/22/2005 3:46:36 PM}	void OnRightButtonUp();

private:
	CGridElementsSelector(const CGridElementsSelector&);  // Not implemented.
	void operator=(const CGridElementsSelector&);     // Not implemented.

};
