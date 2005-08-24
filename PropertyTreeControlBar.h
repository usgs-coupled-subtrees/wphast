#pragma once
#include "sizecbar/scbarcfvs7.h"
#include "IObserver.h"


#include "Tree.h"
//#include "structs.h"
class vtkObject;

class CUnits;
class CFlowOnly;
class CSteadyFlow;
class CFreeSurface;
class CNewModel;
class CTimeControl2;
class CPrintFreq;
class CZoneActor;
class CWPhastDoc;

////class CZoneLODActor;
class vtkProp3D;
class CGridActor;

class CModelessPropertySheet;
class CGridPropertyPage2;

//class vtkPropCollection;

//// axes
//class vtkAxes;
//class vtkTubeFilter;
//class vtkPolyDataMapper;
//class vtkActor;

#ifndef baseCPropertyTreeControlBar
#define baseCPropertyTreeControlBar CSizingControlBarCFVS7
#endif

class CPropertyTreeControlBar : public baseCPropertyTreeControlBar, public IObserver
{
    DECLARE_DYNAMIC(CPropertyTreeControlBar)
public:
	CPropertyTreeControlBar(void);
	virtual ~CPropertyTreeControlBar(void);

	// virtual void Serialize(CArchive& ar);
	virtual void DeleteContents();
	virtual void Update(IObserver* pSender = 0, LPARAM lHint = 0L, CObject* pHint = 0, vtkObject* pObject = 0);

	// vtkPropCollection* GetPropCollection() const;
	// void SetGrid(grid* x, grid* y, grid* z);
	// float* GetGridBounds();
	// void SetScale(float x, float y, float z);
protected:

	CTreeCtrlEx     m_wndTree;
	CImageList     *m_pImageList;


	CTreeCtrlNode   m_nodeBC;
	CTreeCtrlNode   m_nodeGrid;
	CTreeCtrlNode   m_nodeIC;	
	CTreeCtrlNode   m_nodeMedia;
	CTreeCtrlNode   m_nodeUnits;
	CTreeCtrlNode   m_nodeFlowOnly;
	CTreeCtrlNode   m_nodeTimeControl;
	CTreeCtrlNode   m_nodeTimeControl2;
	CTreeCtrlNode   m_nodeSP1;
	CTreeCtrlNode   m_nodePF;
	CTreeCtrlNode   m_nodeFreeSurface;
	CTreeCtrlNode   m_nodeSteadyFlow;
	CTreeCtrlNode   m_nodeWells;
	CTreeCtrlNode   m_nodeRivers;
	
	bool            m_bSelectingProp;
	int             m_nNextZone;

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	// Tree control notifications
	afx_msg void OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMClk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMDblClk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyDown(NMHDR* pNMHDR, LRESULT* pResult);

public:
	CWPhastDoc* GetDocument(void)const;

	void AddZone(CZoneActor* pZone);
	void RemoveZone(CZoneActor* pZone);
	CString GetNextZoneName(void)const;
	int GetNextWellNumber(void)const;


// COMMENT: {4/8/2005 6:50:23 PM}	CTreeCtrlNode AddStressPeriod(const CTimeControl& timeControl);
// COMMENT: {4/8/2005 6:50:23 PM}	void RemoveStressPeriod(int nStressPeriod);

// COMMENT: {4/8/2005 6:50:29 PM}	int GetStressPeriodCount(void);
// COMMENT: {4/8/2005 6:50:29 PM}	CTreeCtrlNode GetStressPeriodNode(int nStressPeriod);

	void SetUnits(CUnits *pUnits);
	void SetFlowOnly(CFlowOnly *pFlowOnly);
	void SetFreeSurface(CFreeSurface *pFreeSurface);
	void SetSteadyFlow(CSteadyFlow *pSteadyFlow);

	void SetTimeControl2(CTimeControl2* pTimeControl2);
	CTimeControl2* GetTimeControl2(void);

	void SetPrintFrequency(CPrintFreq* pPrintFreq);
// COMMENT: {4/8/2005 7:06:02 PM}	void SetPrintFrequency(const CPrintFreq& printFreq, int nStressPeriod = 1);
// COMMENT: {4/8/2005 7:06:02 PM}	CPrintFreq* GetPrintFrequency(int nStressPeriod);
	void SetPrintFrequency(const CPrintFreq& printFreq);
	CPrintFreq* GetPrintFrequency(void);

	void SetModel(CNewModel* pModel);

	CTreeCtrl* GetTreeCtrl(void)       {return &m_wndTree;}
	CTreeCtrlEx* GetTreeCtrlEx(void)   {return &m_wndTree;}

	CTreeCtrlNode GetGridNode(void)         {return m_nodeGrid;}
	CTreeCtrlNode GetMediaNode(void)        {return m_nodeMedia;}
	CTreeCtrlNode GetUnitsNode(void)        {return m_nodeUnits;}
	CTreeCtrlNode GetICNode(void)           {return m_nodeIC;}
	CTreeCtrlNode GetWellsNode(void)const   {return m_nodeWells;}
	CTreeCtrlNode GetRiversNode(void)const  {return m_nodeRivers;}
// COMMENT: {4/8/2005 7:01:33 PM}	CTreeCtrlNode GetBCNode(int nStressPeriod = 1);
// COMMENT: {4/8/2005 7:01:33 PM}	CTreeCtrlNode GetTimeControlNode(int nStressPeriod = 1);
// COMMENT: {4/8/2005 7:01:33 PM}	CTreeCtrlNode GetPrintFrequencyNode(int nStressPeriod = 1);
	CTreeCtrlNode GetBCNode(void);
	CTreeCtrlNode GetTimeControlNode(void);
	CTreeCtrlNode GetTimeControl2Node(void);
	CTreeCtrlNode GetPrintFrequencyNode(void);

	void SetNodeCheck(CTreeCtrlNode node, UINT nCheckState);
	void SetBCCheck(UINT nCheckState);
	void SetICCheck(UINT nCheckState);
	void SetMediaCheck(UINT nCheckState);

	UINT GetNodeCheck(CTreeCtrlNode node)const;
	UINT GetBCCheck(void);
	UINT GetICCheck(void);
	UINT GetMediaCheck(void);


	BOOL SelectWithoutNotification(HTREEITEM htItem);

#ifdef WPHAST_DEPRECATED
// COMMENT: {10/23/2003 8:08:41 PM}	HTREEITEM SelectProp(vtkProp3D* pProp);
#endif
	void SelectGridNode(void);
	void SetGridActor(CGridActor* pGridActor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};
