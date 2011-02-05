#pragma once
#include "Action.h"

#include <string>

#include "WPhastDoc.h"
#include "srcWedgeSource.h"
#include "srcinput/Prism.h"
#include <vtkProperty.h>

template<typename T>
class CZoneCreateAction : public CAction
{
public:

	CZoneCreateAction(CWPhastDoc* pDoc, Polyhedron* polyh, const double origin[3], double angle, const char* desc, CTreeCtrlNode nodeInsertAfter = CTreeCtrlNode(TVI_LAST, NULL));
	CZoneCreateAction(CWPhastDoc* pDoc, CString name, Polyhedron* polyh, const double origin[3], double angle, const char* desc, CTreeCtrlNode nodeInsertAfter = CTreeCtrlNode(TVI_LAST, NULL));

// COMMENT: {1/18/2011 5:46:58 PM}	CZoneCreateAction(CWPhastDoc* pDoc, const char* name, Polyhedron* polyh, const char* desc, CTreeCtrlNode nodeInsertAfter = CTreeCtrlNode(TVI_LAST, NULL));
// COMMENT: {1/18/2011 5:46:58 PM}	CZoneCreateAction(CWPhastDoc* pDoc, Polyhedron* polyh, const char* desc, CTreeCtrlNode nodeInsertAfter = CTreeCtrlNode(TVI_LAST, NULL));

	~CZoneCreateAction(void);

	virtual void Execute();
	virtual void UnExecute();

	T* GetZoneActor();

protected:
	void Create(CWPhastDoc* pDoc, CString name, Polyhedron* polyh, const char* desc, CTreeCtrlNode nodeInsertAfter, const double origin[3], double angle);

protected:
	CWPhastDoc *m_pDoc;
	T* m_pZoneActor;

	HTREEITEM m_hInsertAfter;
	DWORD_PTR m_dwInsertAfter;
	CTreeCtrlNode m_nodeParent;
};

template<typename T>
CZoneCreateAction<T>::CZoneCreateAction(CWPhastDoc* pDoc, Polyhedron* polyh, const double origin[3], double angle, const char* desc, CTreeCtrlNode nodeInsertAfter)
: m_pDoc(pDoc)
, m_hInsertAfter(0)
, m_dwInsertAfter(0)
, m_pZoneActor(0)
{
	// set name
	//
	CString name;
	ASSERT(polyh && ::AfxIsValidAddress(polyh, sizeof(Polyhedron)));
	switch (polyh->get_type())
	{
	case Polyhedron::CUBE:
		name = pDoc->GetNextZoneName();
		break;
	case Polyhedron::WEDGE:
		name = pDoc->GetNextWedgeName();
		break;
	case Polyhedron::PRISM:
		name = pDoc->GetNextPrismName();
		break;
	default:
		ASSERT(FALSE);
		name = "Unknown Polyhedron type";
	}
	this->Create(pDoc, name, polyh, desc, nodeInsertAfter, origin, angle);
}

template<typename T>
CZoneCreateAction<T>::CZoneCreateAction(CWPhastDoc* pDoc, CString name, Polyhedron* polyh, const double origin[3], double angle, const char* desc, CTreeCtrlNode nodeInsertAfter)
: m_pDoc(pDoc)
, m_hInsertAfter(0)
, m_dwInsertAfter(0)
, m_pZoneActor(0)
{
	ASSERT(polyh && ::AfxIsValidAddress(polyh, sizeof(Polyhedron)));
	this->Create(pDoc, name, polyh, desc, nodeInsertAfter, origin, angle);
}

// COMMENT: {1/18/2011 5:47:07 PM}template<typename T>
// COMMENT: {1/18/2011 5:47:07 PM}CZoneCreateAction<T>::CZoneCreateAction(CWPhastDoc* pDoc, const char* name, Polyhedron* polyh, const char* desc, CTreeCtrlNode nodeInsertAfter)
// COMMENT: {1/18/2011 5:47:07 PM}: m_pDoc(pDoc)
// COMMENT: {1/18/2011 5:47:07 PM}, m_hInsertAfter(0)
// COMMENT: {1/18/2011 5:47:07 PM}, m_dwInsertAfter(0)
// COMMENT: {1/18/2011 5:47:07 PM}, m_pZoneActor(0)
// COMMENT: {1/18/2011 5:47:07 PM}{
// COMMENT: {1/18/2011 5:47:07 PM}	double angle = 0.0;
// COMMENT: {1/18/2011 5:47:07 PM}	double origin[3] = { 0.0, 0.0, 0.0 };
// COMMENT: {1/18/2011 5:47:07 PM}	ASSERT(polyh && ::AfxIsValidAddress(polyh, sizeof(Polyhedron)));
// COMMENT: {1/18/2011 5:47:07 PM}	this->Create(pDoc, name, polyh, desc, nodeInsertAfter, origin, angle);
// COMMENT: {1/18/2011 5:47:07 PM}}
// COMMENT: {1/18/2011 5:47:07 PM}
// COMMENT: {1/18/2011 5:47:07 PM}template<typename T>
// COMMENT: {1/18/2011 5:47:07 PM}CZoneCreateAction<T>::CZoneCreateAction(CWPhastDoc* pDoc, Polyhedron* polyh, const char* desc, CTreeCtrlNode nodeInsertAfter)
// COMMENT: {1/18/2011 5:47:07 PM}: m_pDoc(pDoc)
// COMMENT: {1/18/2011 5:47:07 PM}, m_hInsertAfter(0)
// COMMENT: {1/18/2011 5:47:07 PM}, m_dwInsertAfter(0)
// COMMENT: {1/18/2011 5:47:07 PM}, m_pZoneActor(0)
// COMMENT: {1/18/2011 5:47:07 PM}{
// COMMENT: {1/18/2011 5:47:07 PM}	// set name
// COMMENT: {1/18/2011 5:47:07 PM}	//
// COMMENT: {1/18/2011 5:47:07 PM}	CString name;
// COMMENT: {1/18/2011 5:47:07 PM}	ASSERT(polyh && ::AfxIsValidAddress(polyh, sizeof(Polyhedron)));
// COMMENT: {1/18/2011 5:47:07 PM}	switch (polyh->get_type())
// COMMENT: {1/18/2011 5:47:07 PM}	{
// COMMENT: {1/18/2011 5:47:07 PM}	case Polyhedron::CUBE:
// COMMENT: {1/18/2011 5:47:07 PM}		name = pDoc->GetNextZoneName();
// COMMENT: {1/18/2011 5:47:07 PM}		break;
// COMMENT: {1/18/2011 5:47:07 PM}	case Polyhedron::WEDGE:
// COMMENT: {1/18/2011 5:47:07 PM}		name = pDoc->GetNextWedgeName();
// COMMENT: {1/18/2011 5:47:07 PM}		break;
// COMMENT: {1/18/2011 5:47:07 PM}	case Polyhedron::PRISM:
// COMMENT: {1/18/2011 5:47:07 PM}		name = pDoc->GetNextPrismName();
// COMMENT: {1/18/2011 5:47:07 PM}		break;
// COMMENT: {1/18/2011 5:47:07 PM}	default:
// COMMENT: {1/18/2011 5:47:07 PM}		ASSERT(FALSE);
// COMMENT: {1/18/2011 5:47:07 PM}		name = "Unknown Polyhedron type";
// COMMENT: {1/18/2011 5:47:07 PM}	}
// COMMENT: {1/18/2011 5:47:07 PM}
// COMMENT: {1/18/2011 5:47:07 PM}	double angle = 0.0;
// COMMENT: {1/18/2011 5:47:07 PM}	double origin[3] = { 0.0, 0.0, 0.0 };
// COMMENT: {1/18/2011 5:47:07 PM}	this->Create(pDoc, name, polyh, desc, nodeInsertAfter, origin, angle);
// COMMENT: {1/18/2011 5:47:07 PM}}

template<typename T>
//void CZoneCreateAction<T>::Create(CWPhastDoc* pDoc, const char* name, Polyhedron* polyh, const char* desc, CTreeCtrlNode nodeInsertAfter)
//void CZoneCreateAction<T>::Create(CWPhastDoc* pDoc, const char* name, Polyhedron* polyh, const char* desc, CTreeCtrlNode nodeInsertAfter, double origin[3], double angle)
void CZoneCreateAction<T>::Create(CWPhastDoc* pDoc, CString name, Polyhedron* polyh, const char* desc, CTreeCtrlNode nodeInsertAfter, const double origin[3], double angle)
{
	ASSERT(polyh && ::AfxIsValidAddress(polyh, sizeof(Polyhedron)));

	this->m_pZoneActor = T::New();
	ASSERT(this->m_pZoneActor->IsA("CZoneActor"));

	this->m_pZoneActor->SetPolyhedron(polyh, m_pDoc->GetUnits(), origin, angle);

	// set name
	//
	this->m_pZoneActor->SetName(name);

	// set desc
	//
	this->m_pZoneActor->SetDesc(desc);

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
