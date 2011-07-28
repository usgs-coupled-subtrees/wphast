#pragma once
#include "sizecbar/sdbarcfvs7.h"
#include "IObserver.h"
#include "gridctrl/ModGridCtrlEx.h"
#include "srcWedgeSource.h"
#include "srcinput/Data_source.h"
#include <vector>

class vtkObject;
class CWPhastView;
class vtkProp3D;
class vtkCubeSource;
class CUnits;
class Data_source;
class Prism;
class CZoneActor;

struct coord
{
	double c[3];
	coord(double x, double y, double z)   {c[0]=x; c[1]=y; c[2]=z;};
	coord(double x, double y)             {c[0]=x; c[1]=y; c[2]=0.0;};
	coord(void)                           {c[0]=c[1]=c[2]=0.0;};
};

class CBoxPropertiesDialogBar :	public CSizingDialogBarCFVS7, public IObserver
{
    DECLARE_DYNAMIC(CBoxPropertiesDialogBar)
public:
	CBoxPropertiesDialogBar(void);
	virtual ~CBoxPropertiesDialogBar(void);

	virtual void Update(IObserver* pSender = 0, LPARAM lHint = 0L, CObject* pHint = 0, vtkObject* pObject = 0);

	void Set(CWPhastView* pView, vtkProp3D* pProp3D, const CUnits& units);
	vtkProp3D* GetProp3D(void);

	typedef enum tagBP_TYPE {
		BP_NONE,
		BP_POS_PLUS_LENGTH,
		BP_MIN_MAX,
		BP_WELL,
		BP_RIVER,
		BP_DRAIN,
		BP_WEDGE,
		BP_PRISM,
	} BP_TYPE;

	typedef enum tagPRISM_PART {
		PRISM_TOP       = 0,
		PRISM_PERIMETER = 1,
		PRISM_BOTTOM    = 2,
	} PRISM_PART;

	virtual BOOL Create(UINT nIDTemplate, LPCTSTR lpszWindowName,
		CWnd* pParentWnd, UINT nID, DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP);

	void HideZoneControls(void);
	void ShowBoxControls(void);
	void SizeZoneControls(int cx, int cy);

	void HideWellControls(void);
	void ShowWellControls(void);
	void SizeWellControls(int cx, int cy);

	void ShowRiverControls(void);
	void HideRiverControls(void);
	void SizeRiverControls(int cx, int cy);

	void ShowWedgeControls(void);
	void HideWedgeControls(void);
	void SizeWedgeControls(int cx, int cy);

	void ShowPrismControls(void);
	void HidePrismControls(void);
	void SizePrismControls(int cx, int cy);

	void ShowCheckUseMap(void);
	void HideCheckUseMap(void);

	void ShowApply();
	void HideApply();

	void UpdateApply();

	void UpdateUnits(const CUnits& units);

	void UpdatePrism(CZoneActor *pZoneActor, bool bForceUpdate = FALSE);
	void EnablePointsGrid(BOOL bEnable);
	void ApplyNewPrism(CZoneActor *pZoneActor);
	void UpdatePrismControls(void);

	void EnableNone(BOOL bEnable);
	void EnableConstant(BOOL bEnable);
	void EnableArcraster(BOOL bEnable);
	void EnableShape(BOOL bEnable);
	void EnableXYZ(BOOL bEnable);
	void EnablePoints(BOOL bEnable);
	void EnablePrismRadios(BOOL bEnable);

protected:
	CWPhastView   *m_pView;
	vtkProp3D     *m_pProp3D;
	vtkCubeSource *m_CubeSource;
	//CTabCtrl m_wndMethodTab;
	CModGridCtrlEx   m_wndRiverGrid;
	CModGridCtrlEx   m_wndPointsGrid;	
	//CGridCtrl   m_wndRiverGrid;
	//CBrush         m_HollowBrush;
	HBRUSH         m_hBrush;
	BOOL           m_bThemeActive;
	CTabCtrl       m_wndTab;
	CComboBox      m_wndShapeCombo;

	BP_TYPE m_nType;
	Wedge::WEDGE_ORIENTATION m_nOrientation;
	PRISM_PART m_nPrismPart;
	Data_source::DATA_SOURCE_TYPE m_nDataSourceType;

	// Data_source data
	//
	Data_source *m_pds[3];
	double m_dConstant[3];
	CString m_sArcraster[3];
	CString m_sShapefile[3];
	int m_nShapeAttribute[3];
	CString m_sXYZ[3];
	std::list<Point> m_listPoint[3];
	PHAST_Transform::COORDINATE_SYSTEM m_coordinate_system;

	// river data
	//
	std::vector< std::pair<double, double> > m_vectorPoints;

	float m_X;
	float m_Y;
	float m_Z;

	float m_XLength;
	float m_YLength;
	float m_ZLength;

	float m_XMin;
	float m_XMax;
	float m_YMin;
	float m_YMax;
	float m_ZMin;
	float m_ZMax;

	// wells
	float m_XWell;
	float m_YWell;
	PHAST_Transform::COORDINATE_SYSTEM m_xy_coordinate_system_user;

	CString m_strHorizontalUnits;
	CString m_strVerticalUnits;
	CString m_strMapHorizontalUnits;
	CString m_strMapVerticalUnits;

	bool m_bNeedsUpdate;
	// float m_Position[3];
	virtual void DoDataExchange(CDataExchange* pDX);
	void DoDataExchangePrism(CDataExchange *pDX, Data_source *pData_source);

	template<class T>
	void DoDataExchangePoints(CDataExchange *pDX, T &type);

public:
	void Enable(bool bEnable);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void UpdateBackgroundBrush();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnEnKillfocusEdit();
	afx_msg void OnEnChange(); 
	afx_msg void OnApply();
	afx_msg void OnBnClickedArcraster();
	afx_msg void OnBnClickedShape();
	afx_msg void OnBnClickedXYZ();
	afx_msg void OnCbnSelChangeShape();
	afx_msg void OnEnKillfocusConstant();
	afx_msg void OnEnChangeConstant();	
	afx_msg void OnBnClickedUseMap();
	void OnUseMapBox(void);	
	void OnUseMapWell(void);
	void OnUseMapDrain(void);
	void OnUseMapRiver(void);

	void OnChangeCoorSys(void);
	void OnChangeCoorSysBox(void);
	void OnChangeCoorSysWedge(void);
	void OnChangeCoorSysWell(void);
	void OnChangeCoorSysDrain(void);
	void OnChangeCoorSysRiver(void);
	void OnChangeCoorSysPrism(void);

	void OnBnClickedAddPoint(void);
	void OnBnClickedDeletePoint(void);

protected:
	void TestPointsGrid(NMHDR *pNotifyStruct, LRESULT *result, BOOL bShowErrors);

public:
	afx_msg void OnUpdateEditCut(CCmdUI *pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditPaste(CCmdUI *pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditClear(CCmdUI *pCmdUI);
	afx_msg void OnEditClear();
	afx_msg void OnUpdateEditUndo(CCmdUI *pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditRedo(CCmdUI *pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnEndLabelEditGrid(NMHDR *pNotifyStruct, LRESULT *result);
	afx_msg void OnEndLabelEditPointsGrid(NMHDR *pNotifyStruct, LRESULT *result);
	afx_msg void OnBeginLabelEditPointsGrid(NMHDR *pNotifyStruct, LRESULT *result);


	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTcnSelchangingTab(NMHDR *pNMHDR, LRESULT *pResult);

	//
	///afx_msg void OnEnKillfocusYMAXEdit();
	afx_msg void OnEnKillfocusRange(UINT nID);

	// update wedge controls
	afx_msg void OnBnClickedUpdateWedge();

	// update prism controls
	afx_msg void OnBnClickedData_source();

	//
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
