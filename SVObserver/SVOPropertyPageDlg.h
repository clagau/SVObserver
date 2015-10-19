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
	CSVOPropertyPageDlg(CWnd* pParent = NULL);   // standard constructor

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
    void TriggerHideItems();
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


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOPropertyPageDlg.h_v  $
 * 
 *    Rev 1.2   11 Jun 2013 15:37:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 017.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   21 May 2013 10:03:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  836
 * SCR Title:  Fix loading SEC bug
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Moved SVObserverEnums.h to SCConfigurationLibrary.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 May 2013 19:46:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 016.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   10 May 2013 10:58:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  828
 * SCR Title:  Camera Binning
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Renamed Format7Dlg to AdjustCameraImageFormat
 * Added ShowFormat7Dlg function
 * Added ShowGigeROIDlg function
 * Added UpdateGigeCameraBinningParameters function
 * Added ApplyGigeImageFormatChange function
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:11:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   01 Apr 2013 16:09:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  820
 * SCR Title:  Baumer Camera Trigger Debounce
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added SetupCameraDeviceParam method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   21 Jan 2013 11:33:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Conditional output for PPQ
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   02 Aug 2011 07:26:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  734
 * SCR Title:  Clarify OK Button on Camera Properties Dialog on Digital Systems
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed Text on OK button to Reset when files are present but not changed. When dialog is used for Digital or GigE Camera files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   16 Dec 2009 14:05:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   30 Jul 2009 12:14:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   17 Jun 2009 08:53:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added IsGigeSystem method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   03 Oct 2008 17:35:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Changed Frequency to Period for the Software Timer Trigger
 * Added check for Minimum Period for the Software Timer Trigger
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   16 Sep 2008 14:55:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added enums for general and advanced properties for FileAcquisition.
 * Added methods for FileAcqusition setup.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   10 Sep 2008 17:08:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to support the general and advanced properties of Software Triggering
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10.1.0   14 Jul 2009 14:58:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use new data manager objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   22 Jan 2007 09:29:52   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  587
 * SCR Title:  Add Timeout Threshold to Extended Time Delay Mode
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added new property Inspection Timeout Threshold that will get turned on if the PPQ Mode is Extended Time Delay.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   24 Oct 2005 09:40:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changes to support adding a flag to the Inspection that will enable Auxiliary Extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   23 Jul 2004 08:55:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  435
 * SCR Title:  Add Bulb Mode Operation to the Digital SVIM for Point Gray Dragonfly Cameras
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated OnItemChanged and OnInitDialog methods to use the new 1394 camera validation method CameraAdvancedHideItems to show/hide parameters that are not valid when certain trigger modes are used.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   12 Nov 2003 13:46:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  396
 * SCR Title:  Add New Product Types for 1394 SVIM
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed Board -> Product
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   07 Aug 2003 12:48:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   modified code to pop up Format 7 dialog
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   05 Aug 2003 13:51:12   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  373
 * SCR Title:  Add a new disable method that doesn't copy forward all result values
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes to the saving/loading/editing of configurations to add a new property for the inspection's disable method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   28 Jul 2003 10:51:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added support for Format7 dialog
 * added support for conditional "..." button
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   08 Jul 2003 10:28:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322, 320
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added SetConfigAssistantDlg method and updated SetBoardType, OnItemChanged and OnInitDialog methods for using the new 1394 cameras and new Trigger interfaces.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Apr 2003 13:27:14   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Feb 2003 13:27:18   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  316
 * SCR Title:  Add option to extent acquisition image source buffers to the length of the PPQ
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added code for the new PPQ property. 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:39:20   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
