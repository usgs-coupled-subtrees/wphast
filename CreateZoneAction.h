#pragma once
#include "Action.h"

#include <string>

class CZoneLODActor;
class CWPhastDoc;

class CCreateZoneAction :
	public CAction
{
public:
	CCreateZoneAction(CWPhastDoc* pDoc, const char* name, float xMin, float xMax, float yMin, float yMax, float zMin, float zMax);
	virtual ~CCreateZoneAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	std::string m_name;
	CZoneLODActor *m_pZoneLODActor;
	CWPhastDoc *m_pDoc;
	float m_xMin;
	float m_xMax;
	float m_yMin;
	float m_yMax;
	float m_zMin;
	float m_zMax;
	float m_color[3];
};
