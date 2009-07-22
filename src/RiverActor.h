#pragma once

#include "PointConnectorActor.h"
#include "River.h"

class CPropertyTreeControlBar;
class CWPhastDoc;
class CGridKeyword;

class CRiverActor : public CPointConnectorActor
{
public:
	vtkTypeRevisionMacro(CRiverActor, CPointConnectorActor);
	static CRiverActor *New();

	void SetRiver(const CRiver &river, const CUnits &units, const CGridKeyword &gridKeyword);
	CRiver GetRiver(void)const;

	void SetData(const CRiver &river, const CUnits &units, const CGridKeyword &gridKeyword)  { this->SetRiver(river, units, gridKeyword); };
	CRiver GetData(void)const                               { return this->GetRiver(); };

	virtual void Update(CTreeCtrlNode node);
	virtual void MovePoint(vtkIdType id, double x, double y);
	PHAST_Transform::COORDINATE_SYSTEM GetCoordinateSystem(void)const;

	virtual void Add(CPropertyTreeControlBar *pTree, HTREEITEM hInsertAfter = TVI_LAST);

	static const char szHeading[];

	static void SetStaticColor(COLORREF cr);

	static CRiverActor* StartNewRiver(vtkRenderWindowInteractor* pRenderWindowInteractor);

	CRiverPoint* GetRiverPoint(int index);
	void InsertPoint(vtkIdType id, CRiverPoint riverPoint);
	virtual vtkIdType InsertNextPoint(double x, double y, double z);
	virtual void InsertPoint(vtkIdType id, double x, double y, double z);
	virtual void DeletePoint(vtkIdType id);

	void Serialize(bool bStoring, hid_t loc_id, const CWPhastDoc* pWPhastDoc);

	void Edit(int point);

protected:
	CRiverActor(void);
	~CRiverActor(void);

protected:
	CRiver m_river;
	static vtkProperty *s_HandleProperty;
	static vtkProperty *s_ConnectorProperty;
	static vtkFloatingPointType s_color[3];

	virtual vtkProperty *GetHandleProperty(void)      { return s_HandleProperty;    };
	virtual vtkProperty *GetConnectorProperty(void)   { return s_ConnectorProperty; };

	friend std::ostream& operator<< (std::ostream &os, const CRiverActor &a);

private:
	CRiverActor(const CRiverActor&);  // Not implemented.
	void operator=(const CRiverActor&);  // Not implemented.

public:
	class StaticInit
	{
	public:
		StaticInit()
		{
			if (CRiverActor::s_HandleProperty == 0)
			{
				CRiverActor::s_HandleProperty = vtkProperty::New();
				CRiverActor::s_HandleProperty->SetColor(CRiverActor::s_color);
				CRiverActor::s_HandleProperty->SetOpacity(0.7);

				CRiverActor::s_ConnectorProperty = vtkProperty::New();
				CRiverActor::s_ConnectorProperty->SetColor(CRiverActor::s_color);
				CRiverActor::s_ConnectorProperty->SetOpacity(0.3);
			}
		}
		~StaticInit()
		{
			if (CRiverActor::s_HandleProperty != 0)
			{
				CRiverActor::s_HandleProperty->Delete();
				CRiverActor::s_HandleProperty = 0;
			}
			if (CRiverActor::s_ConnectorProperty != 0)
			{
				CRiverActor::s_ConnectorProperty->Delete();
				CRiverActor::s_ConnectorProperty = 0;
			}
		}
	};
	static StaticInit sinit;
};
