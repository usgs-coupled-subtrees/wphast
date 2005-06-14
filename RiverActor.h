#pragma once
#include <list>

#include <vtkAssembly.h>
#include <vtkIdType.h>

#include "River.h"

class CUnits;

class CRiverActor : public vtkAssembly
{
public:
	vtkTypeRevisionMacro(CRiverActor,vtkAssembly);
	static CRiverActor *New();
	void PrintSelf(ostream& os, vtkIndent indent);

	void SetRiver(const CRiver &river, const CUnits &units);
	CRiver GetRiver(void)const;

	vtkIdType InsertNextPoint(double x, double y, double z);
	void InsertPoint(vtkIdType id, double x, double y, double z);

	void SetUnits(const CUnits &units);
	void SetScale(float x, float y, float z);
	void SetScale(double x, double y, double z);
	void SetRadius(float radius);
	void SetZ(double z);

	void UpdatePoints(void);
	void ClearPoints(void);

	void SetEnabled(int enabling);

	// Description:
	// This method is used to associate the widget with the render window
	// interactor.  Observers of the appropriate events invoked in the render
	// window interactor are set up as a result of this method invocation.
	// The SetInteractor() method must be invoked prior to enabling the
	// vtkInteractorObserver.
	virtual void SetInteractor(vtkRenderWindowInteractor* iren);
	vtkGetObjectMacro(Interactor, vtkRenderWindowInteractor);

protected:
	CRiverActor(void);
	~CRiverActor(void);

	// Description:
	// Main process event method
	static void ProcessEvents(vtkObject* object, unsigned long event, void* clientdata, void* calldata);

	// Properties used to control the appearance of selected objects and
	// the manipulator in general.
	vtkProperty *HandleProperty;
	vtkProperty *SelectedHandleProperty;
	void CreateDefaultProperties(void);

	int HighlightHandle(vtkProp *prop);

	// ProcessEvents() dispatches to these methods.
	void OnMouseMove(void);
	void OnLeftButtonDown(void);
	void OnLeftButtonUp(void);

	//
	void Update(void);
	double m_WorldPointXYPlane[4];

	// Used to process events
	vtkCallbackCommand* EventCallbackCommand;

	// Used to associate observers with the interactor
	vtkRenderWindowInteractor *Interactor;

	// Internal ivars for processing events
	vtkRenderer *CurrentRenderer;

	std::string         m_serialName;
	CRiver              m_river;
	float               m_fRadius;
	int                 Enabled;

private:
	CRiverActor(const CRiverActor&);  // Not implemented.
	void operator=(const CRiverActor&);  // Not implemented.

protected:
	std::list<vtkSphereSource*>   m_listSphereSource;
	std::list<vtkPolyDataMapper*> m_listPolyDataMapper;
	std::list<vtkActor*>          m_listActor;

	std::list<vtkLineSource*>     m_listLineSource;
	std::list<vtkPolyDataMapper*> m_listLinePolyDataMapper;
	std::list<vtkTubeFilter*>     m_listTubeFilter;
	std::list<vtkActor*>          m_listLineActor;

	vtkPoints     *m_pPoints;
	vtkTransform  *m_pTransformUnits;
	vtkTransform  *m_pTransformScale;

	// Do the picking
	vtkCellPicker   *m_pCellPicker;
	vtkActor        *CurrentHandle;
	vtkSphereSource *CurrentSource;
	vtkIdType        CurrentId;
};
