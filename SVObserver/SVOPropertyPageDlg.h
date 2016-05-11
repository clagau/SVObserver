//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOPropertyPageDlg
//* .File Name       : $Workfile:   SVOPropertyPageDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   11 Jun 2013 15:37:30  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVConfigurationLibrary/SVObserverEnums.h"
#include "SVOMFCLibrary/SVDeviceParamCollection.h"
#include "PropertyTree/PropTree.h"
#include "SVOCameraObj.h"
#include "SVOInspectionObj.h"
#include "SVOPPQObj.h"
#include "SVOTriggerObj.h"
#pragma endregion Includes

// defines for the dlgs
enum PROPERTY_PAGE_DIALOG_TYPE
{
	VIRTUAL_CAMERA_DLG                  = 1,
	VIRTUAL_INSPECT_DLG,
	VIRTUAL_TRIGGER_DLG,
	VIRTUAL_PPQ_DLG,
	VIRTUAL_CAMERA_ADV,
	VIRTUAL_TRIGGER_ADV,
	VIRTUAL_INSPECT_ADV,
	VIRTUAL_PPQ_ADV,
};

enum
{
	PPQ_MODE_NEXT_TRG                   = 0,
	PPQ_MODE_TIME_DELAY                 = 1,
	PPQ_MODE_TIME_DATA_COMPLETE         = 2,
};

const CString PPQ_NEXT_TRG_STR                    ( _T("NextTrigger") );
const CString PPQ_TIME_DELAY_STR                  ( _T("TimeDelay") );
const CString PPQ_TIME_DATA_COMPLETE              ( _T("TimeDelayDataCompletion") );
const CString PPQ_EXTENDED_TIME_DELAY             ( _T("ExtendedTimeDelay") );

//defines for the properties for each dlg
enum PROPERTY_PAGE_PROPERTY
{
	PROP_AD_FILE_NAME                   = 101,
	PROP_TRG_TYPE,
	PROP_INS_TOOLSET_IMAGE,
	PROP_PPQ_MODE,
	PROP_PPQ_LENGTH,
	PROP_PPQ_OUTPUT_RESET_DELAY,
	PROP_PPQ_OUTPUT_DELAY_TIME,
	PROP_PPQ_MAINTAIN_SRC_IMG,
	PROP_INS_NEW_DISABLE_METHOD,
	PROP_INS_ENABLE_AUX_EXTENT,
	PROP_PPQ_INSPECTION_TIMEOUT,
	PROP_CAMERA_TYPE_FILE_OR_REAL,
	PROP_FILECAMERA_FILENAME,
	PROP_FILECAMERA_DIRECTORY,
	PROP_FILECAMERA_MODE,
	PROP_PPQ_CONDITIONAL_OUTPUT,
	PROP_CAMERA_COLOR,

	//defines for advanced properties
	PROP_ADV_CAMERA_DIG                 = 201,
	PROP_ADV_CAMERA_BANDS,
	PROP_ADV_TRIGGER_PERIOD,
	PROP_ADV_FILECAMERA_IMAGESIZEEDITMODE,
	PROP_ADV_FILECAMERA_IMAGEWIDTH,
	PROP_ADV_FILECAMERA_IMAGEHEIGHT,

	PROP_CAMERA_FILE_BASE               = 10000
};


/////////////////////////////////////////////////////////////////////////////
// CSVOPropertyPageDlg dialog

class SVOTriggerObj;
class SVOInspectionObj;
class SVOPPQObj;
class CSVOConfigAssistantDlg;

class CSVOPropertyPageDlg : public CDialog
{
// Construction
public:
	CSVOPropertyPageDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CSVOPropertyPageDlg();

	SVOCameraObj& getCameraObject() { return m_CameraObj; };
    SVOTriggerObj& getTriggerObject() { return m_TriggerObj; };
    SVOInspectionObj& getInspectObject() { return m_InspectionObj; };
    SVOPPQObj& getPPQObject() { return m_PPQObj; };

    void SetDlgPage(int nID);

    void SetProductType(SVIMProductEnum eType);

	void SetConfigAssistantDlg(CSVOConfigAssistantDlg* pAssistant);

	HRESULT AdjustCameraImageFormat(LPCTSTR sSelectedFormat, SVDeviceParamWrapper& rw);

// Dialog Data
	//{{AFX_DATA(CSVOPropertyPageDlg)
	enum { IDD = IDD_DLG_PROPERTIES_PAGE };
//	SVPropertyListBox	m_PropertyListBox;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVOPropertyPageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON		m_hIcon;
	SVRPropTree	m_Tree;
    
	// Generated message map functions
	//{{AFX_MSG(CSVOPropertyPageDlg)
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

private: //data members
    SVOCameraObj m_CameraObj;
    SVOTriggerObj m_TriggerObj;
    SVOInspectionObj m_InspectionObj;
    SVOPPQObj m_PPQObj;
	CSVOConfigAssistantDlg* m_pAssistant;

	CMapStringToPtr	*m_pMap;
	int				m_nID;
    SVIMProductEnum m_eProduct;
	CString m_InitialCameraFileName;

    void CameraAdvancedHideItems();
    void PPQHideItems();
    void SetTitle(CString sName);

	void SetupCamera();
	void SetupAdvancedCamera();
	void SetupFileCamera(SVRPropertyItem* pRoot);
	void SetupAdvancedFileCamera(SVRPropertyItem* pRoot);
	void SetupCameraDeviceParam(SVRPropertyItem* pRoot, const SVDeviceParam* pDeviceParam, const SVDeviceParam* pFileParam, bool ColorCamera);
	
	void ShowCameraProperties();
	void UpdateFileImageSize();
	bool ScanForImageSize(SIZE& size);
	void SetImageSizeEditAttributes();

	void SetupInspection();
	void SetupTrigger();
	void SetupAdvancedTrigger();
	void SetupPPQ();

	bool IsGigeSystem() const;
};


