#pragma once
#include <list>

#include <vtkAssembly.h>
#include <vtkPropAssembly.h>
#include <vtkIdType.h>

#include "River.h"          // CRiver
#include "tree.h"           // CTreeCtrlNode

class CUnits;
class CPropertyTreeControlBar;

#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif

class CRiverActor : public vtkAssembly
///class CRiverActor : public vtkPropAssembly
{
public:
	vtkTypeRevisionMacro(CRiverActor,vtkAssembly);
	///vtkTypeRevisionMacro(CRiverActor,vtkPropAssembly);
	static CRiverActor *New();
	void PrintSelf(ostream& os, vtkIndent indent);

	void SetRiver(const CRiver &river, const CUnits &units);
	CRiver GetRiver(void)const;

	vtkIdType InsertNextPoint(double x, double y, double z);
	void InsertPoint(vtkIdType id, double x, double y, double z);

	void SetUnits(const CUnits &units);
	void SetScale(float x, float y, float z);
	void SetScale(double x, double y, double z);

	void SetRadius(vtkFloatingPointType radius);
	vtkFloatingPointType GetRadius(void)const;

	void ScaleFromBounds(vtkFloatingPointType bounds[6]);

	void SetZ(double z);
	double GetZ(void)const;

	void UpdatePoints(void);
	void ClearPoints(void);
	void SelectPoint(int index);
	vtkActor* GetPoint(int index);
	void ClearSelection(void);

	void SetEnabled(int enabling);

	// CPropertyTreeControlBar operations
	//
	void Add(CPropertyTreeControlBar *pTree);
	void UnAdd(CPropertyTreeControlBar *pTree);

	void Remove(CPropertyTreeControlBar *pTree);
	void UnRemove(CPropertyTreeControlBar *pTree);

	void Update(CTreeCtrlNode node);

	enum EventIds {
		StartMovePointEvent = vtkCommand::UserEvent + 500,
		MovingPointEvent,
		EndMovePointEvent
	};

	void MovePoint(vtkIdType id, double x, double y);

	vtkIdType GetCurrentPointId(void)const;
	double*   GetCurrentPointPosition(void);
	void      GetCurrentPointPosition(double x[3])const;

	LPCTSTR GetSerialName(void)const;
	void Serialize(bool bStoring, hid_t loc_id, const CUnits &units);

	void SetVisibility(int visibility);
	int GetVisibility(void);

	// Description:
	// This method is used to associate the widget with the render window
	// interactor.  Observers of the appropriate events invoked in the render
	// window interactor are set up as a result of this method invocation.
	// The SetInteractor() method must be invoked prior to enabling the
	// vtkInteractorObserver.
	virtual void SetInteractor(vtkRenderWindowInteractor* iren);
	vtkGetObjectMacro(Interactor, vtkRenderWindowInteractor);

	static CRiverActor* StartNewRiver(vtkRenderWindowInteractor* pRenderWindowInteractor);

protected:
	CRiverActor(void);
	~CRiverActor(void);

	enum RiverState
	{
		None,
		MovingPoint,
		CreatingRiver,
	} State;


	// Description:
	// Main process event method
	static void ProcessEvents(vtkObject* object, unsigned long event, void* clientdata, void* calldata);

	// Properties used to control the appearance of selected objects and
	// the manipulator in general.
	vtkProperty *HandleProperty;
	vtkProperty *SelectedHandleProperty;
	vtkProperty *EnabledHandleProperty;
	void CreateDefaultProperties(void);

	int HighlightHandle(vtkProp *prop);

	// ProcessEvents() dispatches to these methods.
	void OnMouseMove(void);
	void OnLeftButtonDown(void);
	void OnLeftButtonUp(void);

	//
	void Update(void);
	double m_WorldPointXYPlane[4];
	double WorldSIPoint[4];
	double WorldScaledUnitPoint[4];

	// Used to process events
	vtkCallbackCommand* EventCallbackCommand;

	// Used to associate observers with the interactor
	vtkRenderWindowInteractor *Interactor;

	// Internal ivars for processing events
	vtkRenderer *CurrentRenderer;

	std::string          m_serialName;
	CRiver               m_river;
	vtkFloatingPointType m_Radius;
	double               m_Z;
	int                  Enabled;

	vtkLineSource     *ConnectingLineSource;
	vtkPolyDataMapper *ConnectingMapper;
	vtkActor          *ConnectingActor;

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

	CTreeCtrlNode       m_node;

	// 3D Cursor
	vtkCursor3D                    *m_pCursor3D;
	vtkPolyDataMapper              *m_pCursor3DMapper;
	vtkActor                       *m_pCursor3DActor;
};
