
#pragma once

class CPropertyTreeControlBar;

// CPropTreeOleDropTarget command target

class CPropTreeOleDropTarget : public COleDropTarget
{

public:
	CPropTreeOleDropTarget();
	virtual ~CPropTreeOleDropTarget();

	void SetHandler(CPropertyTreeControlBar* handler);

protected:
	CPropertyTreeControlBar* Handler;
	DECLARE_MESSAGE_MAP()
public:
	virtual DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	virtual void OnDragLeave(CWnd* pWnd);
};


