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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageTest\SVCameraFilePropertyPageDlg.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 11:08:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   17 Jun 2009 08:27:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to support Gige System
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   05 Mar 2008 09:39:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  622
 * SCR Title:  Update SVImageTest Application to Acquire on Multiple Cameras Simultaneously
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Added functionality for processing multiple camera acquisitions at the same time.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   29 Nov 2006 10:43:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  584
 * SCR Title:  Revise the SVImageTest utility program to load 1394 camera files
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/