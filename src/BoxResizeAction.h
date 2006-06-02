#pragma once
#include "Action.h"

class CWPhastView;
class vtkProp3D;
class vtkCubeSource;
class CZoneLODActor;

class CBoxResizeAction : public CAction
{
public:
	CBoxResizeAction(CWPhastView* pView, vtkProp3D* pProp3D, float xLength, float yLength, float zLength, float xCenter, float yCenter, float zCenter);
	virtual ~CBoxResizeAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	void Store();

	CWPhastView   *m_pView;
	vtkProp3D     *m_pProp3D;
	vtkCubeSource *m_CubeSource;

	// original values
	float m_OriginalXLength;
	float m_OriginalYLength;
	float m_OriginalZLength;
	////float m_OriginalPosition[3];
	float m_OriginalCenter[3];

	// new values
	float m_XLength;
	float m_YLength;
	float m_ZLength;
	////float m_Position[3];
	float m_Center[3];
};
