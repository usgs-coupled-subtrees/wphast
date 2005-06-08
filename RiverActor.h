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


protected:
	CRiverActor(void);
	~CRiverActor(void);
	///void AddPart(vtkProp3D *pProp3D);

	std::string         m_serialName;
	CRiver              m_river;
	float               m_fRadius;

private:
	CRiverActor(const CRiverActor&);  // Not implemented.
	void operator=(const CRiverActor&);  // Not implemented.

protected:
	std::list<vtkSphereSource*>   m_listSphereSource;
	std::list<vtkPolyDataMapper*> m_listPolyDataMapper;
	std::list<vtkActor*>          m_listActor;

	vtkPoints     *m_pPoints;
	vtkTransform  *m_pTransformUnits;
	vtkTransform  *m_pTransformScale;
	vtkCellPicker *m_pCellPicker;

};
