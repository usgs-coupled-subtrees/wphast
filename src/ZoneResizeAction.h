#pragma once
#include "Action.h"

class CWPhastView;
class CZoneActor;
class vtkCubeSource;

class CZoneResizeAction : public CAction
{
public:
#ifdef USE_GET_LENGTHS_CENTER
	CZoneResizeAction(CWPhastView* pView, CZoneActor* pZone, float xLength, float yLength, float zLength, float xCenter, float yCenter, float zCenter);
#endif
	CZoneResizeAction(CWPhastView* pView, CZoneActor* pZone, float xMin, float xMax, float yMin, float yMax, float zMin, float zMax);
	virtual ~CZoneResizeAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	void Store();

protected:
	CWPhastView   *m_pView;
	CZoneActor    *m_pZone;
	// vtkCubeSource *m_CubeSource;
	double m_OriginalBounds[6];
	double m_Bounds[6];

#ifdef USE_GET_LENGTHS_CENTER
	// original values
	float m_OriginalXLength;
	float m_OriginalYLength;
	float m_OriginalZLength;
	float m_OriginalCenter[3];

	// new values
	float m_XLength;
	float m_YLength;
	float m_ZLength;
	float m_Center[3];
#endif
};
