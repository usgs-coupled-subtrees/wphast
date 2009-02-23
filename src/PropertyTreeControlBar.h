#pragma once
#include "sizecbar/scbarcfvs7.h"
#include "IObserver.h"

#include "Tree.h"
#include "PropTreeOleDropTarget.h"

class vtkObject;

class CUnits;
class CFlowOnly;
class CSteadyFlow;
class CFreeSurface;
class CSolutionMethod;
class CNewModel;
class CTimeControl2;
class CPrintInput;
class CPrintFreq;
class CZoneActor;
class CWPhastDoc;
class CGridActor;
class CModelessPropertySheet;


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
	CTreeCtrlNode   m_nodeICHead;
	CTreeCtrlNode   m_nodeICChem;
	CTreeCtrlNode   m_nodeMedia;
	CTreeCtrlNode   m_nodeUnits;
	CTreeCtrlNode   m_nodeFlowOnly;
// COMMENT: {6/22/2007 4:26:13 PM}	CTreeCtrlNode   m_nodeTimeControl;
	CTreeCtrlNode   m_nodeTimeControl2;
	CTreeCtrlNode   m_nodeSP1;
	CTreeCtrlNode   m_nodePrintInput;
	CTreeCtrlNode   m_nodePF;
	CTreeCtrlNode   m_nodeFreeSurface;
	CTreeCtrlNode   m_nodeSolutionMethod;
	CTreeCtrlNode   m_nodeSteadyFlow;
	CTreeCtrlNode   m_nodeWells;
	CTreeCtrlNode   m_nodeRivers;
	
	bool            m_bSelectingProp;

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	// Tree control notifications
	afx_msg void OnSelChanging(NMHDR* pNMHDR, LRESULT* pResult);	
	afx_msg void OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMClk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMDblClk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyDown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult);

public:
	CWPhastDoc* GetDocument(void)const;

	void AddZone(CZoneActor* pZone, HTREEITEM hInsertAfter = TVI_LAST);
	void RemoveZone(CZoneActor* pZone);

	void SetUnits(CUnits *pUnits);
	void SetFlowOnly(CFlowOnly *pFlowOnly);
	void SetFreeSurface(CFreeSurface *pFreeSurface);
	void SetSteadyFlow(CSteadyFlow *pSteadyFlow);
	void SetSolutionMethod(CSolutionMethod *pSolutionMethod);

	void SetTimeControl2(CTimeControl2* pTimeControl2);
	CTimeControl2* GetTimeControl2(void);

	void SetPrintFrequency(CPrintFreq* pPrintFreq);
	void SetPrintFrequency(const CPrintFreq& printFreq);
	CPrintFreq* GetPrintFrequency(void);

	void SetPrintInput(CPrintInput* pPrintInput);

	void SetModel(CNewModel* pModel);

	CTreeCtrl* GetTreeCtrl(void)       {return &m_wndTree;}
	CTreeCtrlEx* GetTreeCtrlEx(void)   {return &m_wndTree;}

	CTreeCtrlNode GetGridNode(void)           {return m_nodeGrid;}
	CTreeCtrlNode GetMediaNode(void)          {return m_nodeMedia;}
	CTreeCtrlNode GetUnitsNode(void)          {return m_nodeUnits;}
	CTreeCtrlNode GetICNode(void)             {return m_nodeIC;}
	CTreeCtrlNode GetICHeadNode(void)         {return m_nodeICHead;}
	CTreeCtrlNode GetICChemNode(void)         {return m_nodeICChem;}
	CTreeCtrlNode GetWellsNode(void)          {return m_nodeWells;}
	CTreeCtrlNode GetRiversNode(void)         {return m_nodeRivers;}
	CTreeCtrlNode GetBCNode(void)             {return m_nodeBC;}
// COMMENT: {6/22/2007 5:09:02 PM}	CTreeCtrlNode GetTimeControlNode(void)    {return m_nodeTimeControl;}
	CTreeCtrlNode GetTimeControl2Node(void)   {return m_nodeTimeControl2;}
	CTreeCtrlNode GetPrintInitialNode(void)   {return m_nodePrintInput;}
	CTreeCtrlNode GetPrintFrequencyNode(void) {return m_nodePF;}

	void SetNodeCheck(CTreeCtrlNode node, UINT nCheckState);
	void SetBCCheck(UINT nCheckState);
	void SetICCheck(UINT nCheckState);
	void SetMediaCheck(UINT nCheckState);

	UINT GetNodeCheck(CTreeCtrlNode node)const;
	UINT GetBCCheck(void);
	UINT GetICCheck(void);
	UINT GetMediaCheck(void);


	BOOL SelectWithoutNotification(HTREEITEM htItem);
	void ClearSelection(void);

	void SelectGridNode(void);
	void SetGridActor(CGridActor* pGridActor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);

// COMMENT: {6/22/2007 3:06:41 PM}	void EditSelection(LRESULT* pResult, BOOL bJustCheckIfEditable = FALSE);

protected:
	friend CPropTreeOleDropTarget;
	CPropTreeOleDropTarget m_OleDropTarget;
	CTreeCtrlNode m_dragNode;

	CLIPFORMAT m_cfPID;

	bool IsNodeEditable(CTreeCtrlNode editNode, bool bDoEdit);
	bool IsNodeDraggable(CTreeCtrlNode dragNode, COleDataSource &oleDataSource);
	bool IsNodeCopyable(CTreeCtrlNode copyNode, COleDataSource *pOleDataSource);
	bool IsNodePasteable(CTreeCtrlNode pasteNode, bool bDoPaste);
	bool IsNodePasteableWell(CTreeCtrlNode pasteNode, bool bDoPaste);
	bool IsNodePasteableRiver(CTreeCtrlNode pasteNode, bool bDoPaste);	

	template<typename ZT, typename DT>
	bool IsNodePasteable(CTreeCtrlNode headNode, CTreeCtrlNode pasteNode, bool bDoPaste);


	CLIPFORMAT GetZoneClipFormat(void)const;
	CLIPFORMAT GetNativeClipFormat(void)const;

	virtual DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	virtual void OnDragLeave(CWnd* pWnd);
public:
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditPaste(CCmdUI *pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditCut(CCmdUI *pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditClear(CCmdUI *pCmdUI);
	afx_msg void OnEditClear();
	afx_msg void OnUpdateEditProperties(CCmdUI *pCmdUI);
	afx_msg void OnEditProperties();
};
