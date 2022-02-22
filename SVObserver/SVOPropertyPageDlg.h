//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVOLibrary/SVObserverEnums.h"
#include "SVRPropertyTree/SVRPropTree.h"
#include "SVOCameraObj.h"
#include "SVOInspectionObj.h"
#include "SVOPPQObj.h"
#include "Triggering/SVOTriggerObj.h"
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
	PROP_PPQ_MAX_PROCESSING_OFFSET,
	PROP_CAMERA_COLOR,

	//defines for advanced properties
	PROP_ADV_CAMERA_DIG                 = 201,
	PROP_ADV_CAMERA_BANDS,
	PROP_ADV_TRIGGER_PERIOD,
	PROP_ADV_TRIGGER_START_OBJECT_ID,
	PROP_ADV_TRIGGER_PER_OBJECT_ID,
	PROP_ADV_TRIGGER_OBJECT_ID_COUNT,
	PROP_ADV_FILECAMERA_IMAGESIZEEDITMODE,
	PROP_ADV_FILECAMERA_IMAGEWIDTH,
	PROP_ADV_FILECAMERA_IMAGEHEIGHT,

	PROP_CAMERA_FILE_BASE               = 10000
};


/////////////////////////////////////////////////////////////////////////////
// SVOPropertyPageDlg dialog

namespace SvTrig
{
	class SVOTriggerObj;
} //namespace SvTrig
class SVOInspectionObj;
class SVOPPQObj;
class SVOConfigAssistantDlg;

class SVOPropertyPageDlg : public CDialog
{
// Construction
public:
	SVOPropertyPageDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVOPropertyPageDlg();

	SVOCameraObj& getCameraObject() { return m_CameraObj; };
    SvTrig::SVOTriggerObj& getTriggerObject() { return m_TriggerObj; };
    SVOInspectionObj& getInspectObject() { return m_InspectionObj; };
    SVOPPQObj& getPPQObject() { return m_PPQObj; };

    void SetDlgPage(int nID);

    void SetProductType(SVIMProductEnum eType);

	void SetConfigAssistantDlg(SVOConfigAssistantDlg* pAssistant);

	HRESULT AdjustCameraImageFormat(LPCTSTR sSelectedFormat, SVDeviceParamWrapper& rw);

// Dialog Data
	//{{AFX_DATA(SVOPropertyPageDlg)
	enum { IDD = IDD_DLG_PROPERTIES_PAGE };
//	SVPropertyListBox	m_PropertyListBox;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVOPropertyPageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON		m_hIcon;
	SVRPropTree	m_Tree;
    
	// Generated message map functions
	//{{AFX_MSG(SVOPropertyPageDlg)
	virtual BOOL OnInitDialog() override;
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
    SvTrig::SVOTriggerObj m_TriggerObj;
    SVOInspectionObj m_InspectionObj;
    SVOPPQObj m_PPQObj;
	SVOConfigAssistantDlg* m_pAssistant;

	CMapStringToPtr	*m_pMap;
	int				m_nID;
    SVIMProductEnum m_eProduct;
	std::string m_InitialCameraFileName;

    void CameraAdvancedHideItems();
    void PPQHideItems();
    void SetTitle( LPCTSTR Name );

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

	void checkAndSetMaxProcessingOffset(int iLen);
};


