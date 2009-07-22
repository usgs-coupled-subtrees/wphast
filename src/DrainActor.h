#pragma once

#include "PointConnectorActor.h"
#include "River.h"
#include "Drain.h"

class CPropertyTreeControlBar;
class CWPhastDoc;
class CGridKeyword;

class CDrainActor : public CPointConnectorActor
{
public:
	vtkTypeRevisionMacro(CDrainActor, CPointConnectorActor);
	static CDrainActor *New();

// COMMENT: {10/17/2008 2:57:28 PM}	void SetRiver(const CRiver &river, const CUnits &units);
// COMMENT: {10/17/2008 2:57:28 PM}	CRiver GetRiver(void)const;
// COMMENT: {10/17/2008 2:57:28 PM}
// COMMENT: {10/17/2008 2:57:28 PM}	void SetData(const CRiver &river, const CUnits &units)  { this->SetRiver(river, units); };
// COMMENT: {10/17/2008 2:57:28 PM}	CRiver GetData(void)const                               { return this->GetRiver(); };

	void SetDrain(const CDrain &drain, const CUnits &units, const CGridKeyword &gridKeyword);
	CDrain GetDrain(void)const;

	void SetData(const CDrain &drain, const CUnits &units, const CGridKeyword &gridKeyword)  { this->SetDrain(drain, units, gridKeyword); };
	CDrain GetData(void)const                               { return this->GetDrain(); };

	virtual void Update(CTreeCtrlNode node);
	virtual void MovePoint(vtkIdType id, double x, double y);
	virtual PHAST_Transform::COORDINATE_SYSTEM GetCoordinateSystem(void)const;

	virtual void Add(CPropertyTreeControlBar *pTree, HTREEITEM hInsertAfter = TVI_LAST);

	static void SetStaticColor(COLORREF cr);

	static const char szHeading[];

	static CDrainActor* StartNewDrain(vtkRenderWindowInteractor* pRenderWindowInteractor);

	CRiverPoint* GetRiverPoint(int index);
	void InsertPoint(vtkIdType id, CRiverPoint riverPoint);
	virtual vtkIdType InsertNextPoint(double x, double y, double z);
	virtual void InsertPoint(vtkIdType id, double x, double y, double z);
	virtual void DeletePoint(vtkIdType id);

	void Serialize(bool bStoring, hid_t loc_id, const CWPhastDoc* pWPhastDoc);

	void Edit(int point);

protected:
	CDrainActor(void);
	~CDrainActor(void);

protected:
// COMMENT: {10/17/2008 3:02:24 PM}	CRiver m_river;
	CDrain Drain;
	static vtkProperty *s_HandleProperty;
	static vtkProperty *s_ConnectorProperty;
	static vtkFloatingPointType s_color[3];

	virtual vtkProperty *GetHandleProperty(void)      { return s_HandleProperty;    };
	virtual vtkProperty *GetConnectorProperty(void)   { return s_ConnectorProperty; };

	friend std::ostream& operator<< (std::ostream &os, const CDrainActor &a);

private:
	CDrainActor(const CDrainActor&);  // Not implemented.
	void operator=(const CDrainActor&);  // Not implemented.

public:
	class StaticInit
	{
	public:
		StaticInit()
		{
			if (CDrainActor::s_HandleProperty == 0)
			{
				CDrainActor::s_HandleProperty = vtkProperty::New();
				CDrainActor::s_HandleProperty->SetColor(CDrainActor::s_color);
				CDrainActor::s_HandleProperty->SetOpacity(0.7);

				CDrainActor::s_ConnectorProperty = vtkProperty::New();
				CDrainActor::s_ConnectorProperty->SetColor(CDrainActor::s_color);
				CDrainActor::s_ConnectorProperty->SetOpacity(0.3);
			}
		}
		~StaticInit()
		{
			if (CDrainActor::s_HandleProperty != 0)
			{
				CDrainActor::s_HandleProperty->Delete();
				CDrainActor::s_HandleProperty = 0;
			}
			if (CDrainActor::s_ConnectorProperty != 0)
			{
				CDrainActor::s_ConnectorProperty->Delete();
				CDrainActor::s_ConnectorProperty = 0;
			}
		}
	};
	static StaticInit sinit;
};
