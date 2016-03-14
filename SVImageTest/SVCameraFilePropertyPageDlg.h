//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCameraFilePropertyPageDlg 
//* .File Name       : $Workfile:   SVCameraFilePropertyPageDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 11:08:10  $
//******************************************************************************

#ifndef INCL_SVCAMERAFILEPROPERTYPAGEDLG_H
#define INCL_SVCAMERAFILEPROPERTYPAGEDLG_H
//defines for the properties for each dlg
enum PROPERTY_PAGE_PROPERTY
{
	//defines for advanced properties
	PROP_ADV_CAMERA_DIG                 = 201,
	PROP_ADV_CAMERA_BANDS,
	PROP_CAMERA_FILE_BASE               = 10000
};

#include "SVOMFCLibrary/SVDeviceParamCollection.h"
#include "PropertyTree/PropTree.h"

class SVTestAcquisitionClass;

class SVCameraFilePropertyPageDlg : public CDialog
{
public:
	SVCameraFilePropertyPageDlg(CWnd* pParent = NULL);   // standard constructor
	
	SVTestAcquisitionClass* m_pAcquisition;

//	HRESULT Format7Dlg( LPCTSTR sSelectedFormat, SVDeviceParamWrapper& rw );

// Dialog Data
	//{{AFX_DATA(SVCameraFilePropertyPageDlg)
	enum { IDD = IDD_CAMERAFILE_PROPERTIES_DIALOG };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVCameraFilePropertyPageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	HICON m_hIcon;
	SVRPropTree	m_Tree;
    
	// Generated message map functions
	//{{AFX_MSG(SVCameraFilePropertyPageDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnClose();
	//}}AFX_MSG
	afx_msg void OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult);
    afx_msg void OnItemQueryShowButton(NMHDR* pNotifyStruct, LRESULT* plResult);
    afx_msg void OnItemButtonClick(NMHDR* pNotifyStruct, LRESULT* plResult);

	DECLARE_MESSAGE_MAP()

private:
    void CameraAdvancedHideItems();
    void SetTitle(CString sName);

	const SVDeviceParamCollection& GetCameraFileParams();
	SVDeviceParamCollection& GetCameraFileParamsNonConst();
	const SVDeviceParamCollection& GetCameraDeviceParams();
	SVDeviceParamCollection& GetCameraDeviceParamsNonConst();

	bool IsGigeSystem() const;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif 

