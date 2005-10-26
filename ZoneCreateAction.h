#pragma once
#include "Action.h"

#include <string>

#include "WPhastDoc.h"
#include <vtkProperty.h>

#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif

template<typename T>
class CZoneCreateAction : public CAction
{
public:
	CZoneCreateAction(CWPhastDoc* pDoc, const char* name, float xMin, float xMax, float yMin, float yMax, float zMin, float zMax, CTreeCtrlNode nodeInsertAfter = CTreeCtrlNode(TVI_LAST, NULL));
	~CZoneCreateAction(void);

	virtual void Execute();
	virtual void UnExecute();

	T* GetZoneActor();

protected:
	CWPhastDoc *m_pDoc;
	std::string m_sName;
	float m_xMin;
	float m_xMax;
	float m_yMin;
	float m_yMax;
	float m_zMin;
	float m_zMax;
	T* m_pZoneActor;
	HTREEITEM m_hInsertAfter;
	DWORD_PTR m_dwInsertAfter;
	CTreeCtrlNode m_nodeParent;
};

template<typename T>
CZoneCreateAction<T>::CZoneCreateAction(CWPhastDoc* pDoc, const char* name, float xMin, float xMax, float yMin, float yMax, float zMin, float zMax, CTreeCtrlNode nodeInsertAfter)
: m_pDoc(pDoc)
, m_sName(name)
, m_xMin(xMin)
, m_xMax(xMax)
, m_yMin(yMin)
, m_yMax(yMax)
, m_zMin(zMin)
, m_zMax(zMax)
, m_hInsertAfter(0)
, m_dwInsertAfter(0)
, m_pZoneActor(0)
{
	this->m_pZoneActor = T::New();
	ASSERT(this->m_pZoneActor->IsA("CZoneActor"));

	// size
	// TODO : this may need to be inside the Execute method
	this->m_pZoneActor->SetBounds(
		this->m_xMin,
		this->m_xMax,
		this->m_yMin,
		this->m_yMax,
		this->m_zMin,
		this->m_zMax,
		this->m_pDoc->GetUnits()
		);

	// set name
	//
	this->m_pZoneActor->SetName(this->m_sName.c_str());

	// create random color
	// ::rand() returns 0 - RAND_MAX
	vtkFloatingPointType color[3];
	color[0] = (double)::rand() / (double)RAND_MAX;
	color[1] = (double)::rand() / (double)RAND_MAX;
	color[2] = (double)::rand() / (double)RAND_MAX;

	// set color and transparency
	//
	this->m_pZoneActor->GetProperty()->SetColor(color);
	this->m_pZoneActor->GetProperty()->SetOpacity(0.3);

	if (nodeInsertAfter == TVI_LAST)
	{
		this->m_hInsertAfter = TVI_LAST;
	}
	else if (nodeInsertAfter == TVI_FIRST)
	{
		this->m_hInsertAfter = TVI_FIRST;
	}
	else
	{
		this->m_nodeParent = nodeInsertAfter.GetParent();
		this->m_dwInsertAfter = nodeInsertAfter.GetData();
		ASSERT(this->m_nodeParent && this->m_dwInsertAfter);
	}
}

template<typename T>
CZoneCreateAction<T>::~CZoneCreateAction(void)
{
	this->m_pZoneActor->Delete();
}

template<typename T>
T* CZoneCreateAction<T>::GetZoneActor(void)
{
	return this->m_pZoneActor;
}

template<typename T>
void CZoneCreateAction<T>::Execute()
{
	ASSERT_VALID(this->m_pDoc);
	HTREEITEM hInsertAfter = TVI_LAST;
	if (this->m_nodeParent)
	{
		// search for hInsertAfter (by data)
		CTreeCtrlNode node = this->m_nodeParent.GetChild();
		while (node)
		{
			if (node.GetData() == this->m_dwInsertAfter)
			{
				hInsertAfter = node;
				break;
			}
			node = node.GetNextSibling();
		}
		ASSERT(node);
	}
	else
	{
		hInsertAfter = this->m_hInsertAfter;
	}
	this->m_pDoc->Add(this->m_pZoneActor, hInsertAfter);
}

template<typename T>
void CZoneCreateAction<T>::UnExecute()
{
	ASSERT_VALID(this->m_pDoc);
	this->m_pDoc->UnAdd(this->m_pZoneActor);
}

