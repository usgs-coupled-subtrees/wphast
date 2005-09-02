#pragma once

#include "GridKeyword.h"
#include "Units.h"
#include "afxcmn.h"

class CWPhastDoc;
class CGridActor;
class CGridElementsSelector;

// CGridRefinePage dialog

class CGridRefinePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CGridRefinePage)

public:
	CGridRefinePage();
	virtual ~CGridRefinePage();

	void SetUnits(const CUnits& units);
	CUnits GetUnits()const;

	void SetProperties(const CGridKeyword& gridKeyword);
	void GetProperties(CGridKeyword& gridKeyword)const;

	void SetDocument(CWPhastDoc *document);
	CWPhastDoc* GetDocument()const;

	void SetActor(CGridActor *actor);
	CGridActor* GetActor()const;

	void SetWidget(CGridElementsSelector *widget);
	CGridElementsSelector* GetWidget()const;

	void SetMin(int min[3]);
	void GetMin(int min[3])const;

	void SetMax(int max[3]);
	void GetMax(int max[3])const;

	void UpdateSpinners();

// Dialog Data
	enum { IDD = IDD_GRID_REFINE };

protected:
	bool  NeedAction;
	
	CWPhastDoc            *Document;
	CGridActor            *Actor;
	CGridElementsSelector *Widget;

	CGridKeyword       GridKeyword;
	CGridKeyword       OriginalGridKeyword;
	CUnits             Units;

	int MinOneBased[3];
	int MaxOneBased[3];
	int Parts[3];

	CSpinButtonCtrl spinMin[3];
	CSpinButtonCtrl spinMax[3];
	CSpinButtonCtrl spinParts[3];

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	afx_msg void OnEnChangeEditMinI();
	afx_msg void OnEnChangeEditMaxI();
	afx_msg void OnEnChangeEditPartsI();

	afx_msg void OnEnChangeEditMinJ();
	afx_msg void OnEnChangeEditMaxJ();
	afx_msg void OnEnChangeEditPartsJ();

	afx_msg void OnEnChangeEditMinK();
	afx_msg void OnEnChangeEditMaxK();
	afx_msg void OnEnChangeEditPartsK();

	virtual BOOL OnApply();
	virtual void OnCancel();
protected:
	virtual void PostNcDestroy();
public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	afx_msg void OnBnClickedCheckX();
	afx_msg void OnBnClickedCheckY();
	afx_msg void OnBnClickedCheckZ();
};
