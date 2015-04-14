//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogMaskPageClass
//* .File Name       : $Workfile:   SVToolAdjustmentDialogMaskPageClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   26 Jun 2014 18:29:28  $
//******************************************************************************

#ifndef SVTOOLADJUSTMENTDIALOGMASKPAGECLASS_H
#define SVTOOLADJUSTMENTDIALOGMASKPAGECLASS_H

#include "SVEnumerateCombo.h"
#include "SVImageListClass.h"
#include "SVObjectLibrary/SVInputInfoListClass.h"
#include "SVTaskObjectInterfaceClass.h"
#include "SVMFCControls/SVMaskEditor.h"
//TODO: MZA(10.Nov 2014): Move this files to SVOGui project and then remove folder from include and Namespace add-on add PictureDisplay declaration.
#include "SVOGui/PictureDisplay.h"
#include "SVView.h"

class SVToolAdjustmentDialogSheetClass;
class SVUnaryImageOperatorListClass;
class SVUserMaskOperatorClass;

class SVToolAdjustmentDialogMaskPageClass : public CPropertyPage, public SVTaskObjectInterfaceClass
{
public:
	SVToolAdjustmentDialogMaskPageClass( SVToolAdjustmentDialogSheetClass* Parent );
	virtual ~SVToolAdjustmentDialogMaskPageClass();

	virtual HRESULT SetInspectionData();

protected:

	enum MaskTypeEnum
	{
		MASK_TYPE_STATIC = 0,
		MASK_TYPE_IMAGE = 1,
		MASK_TYPE_SHAPE = 2,
	};

	void initMask();

	// Generated message map functions
	//{{AFX_MSG(SVToolAdjustmentDialogMaskPageClass)
	virtual BOOL OnInitDialog();
	afx_msg void OnEditStaticMaskButton();
	afx_msg void OnEditShapeMaskButton();
	afx_msg void OnExportMaskButton();
	afx_msg void OnImportMaskButton();
	afx_msg void OnActivateMask();
	afx_msg void OnSelchangeMaskOperatorCombo();
	afx_msg void OnRadioImageMask();
	afx_msg void OnRadioStaticMask();
	afx_msg void OnSelchangeComboImageMask();
	afx_msg void OnButtonFillColorMore();
	afx_msg void OnRadioShapeMask();
	afx_msg void OnSelchangeComboFillOptions();
	afx_msg void OnChangeEditFillColor();
	afx_msg void OnSelchangeDrawMaskCriteria();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(SVToolAdjustmentDialogMaskPageClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV
	virtual BOOL OnSetActive( );
	//}}AFX_VIRTUAL

	//{{AFX_DATA(SVToolAdjustmentDialogMaskPageClass)
	enum { IDD = IDD_TA_MASK };
	CButton	m_btnImportMask;
	CButton	m_btnExportMask;
	CButton	m_btnEditStaticMask;
	CButton	m_btnEditShapeMask;
	CEdit	m_ebFillColor;
	CComboBox	m_cbFillOptions;
	CButton	m_btnFillColorMore;
	SVAvailableSourceImageListComboBoxClass	m_cbAvailableSourceImageList;
	CComboBox           m_cbMaskOperator;
	SvOg::PictureDisplay      m_dialogImage;
	BOOL                m_bActivateMask;
	int                 m_iMaskType;
	CString	m_sFillColor;
	//}}AFX_DATA

	SvMc::CSVMaskEditor*                     m_pMaskEditorCtl;

	SVToolAdjustmentDialogSheetClass*  m_pParentDialog;
	SVToolClass*                       m_pTool;
	SVUnaryImageOperatorListClass*     m_pUnaryImageOperatorList;
	SVUserMaskOperatorClass*           m_pMask;

	SVImageListClass                   m_imageList;
	SVInObjectInfoStruct*              m_pImageInputInfo;
	SVImageClass*                      m_pCurrentSourceImage;
	SVEnumerateComboClass              m_DrawCriteriaCombo;

	SVFileNameClass                    m_svfnFileName;    // SES 15-Jan-2001

	static UINT_PTR CALLBACK ColorDlgHookFn( HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam );

private:
	void SetupImageMaskCombo();
	void ShowControls(int iMaskType);
	void setImages();

	void RefreshComboBox(SVValueObjectClass& p_rValueObject, CComboBox* p_pComboBox);
	
	static SVToolAdjustmentDialogMaskPageClass* m_pThis;
};


////////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// DevStudio inserts additional declarations immediate in front of the preceding line
////////////////////////////////////////////////////////////////////////////////


//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#endif	//	_SVTOOLADJUSTMENTDIALOGMASKPAGECLASS_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVToolAdjustmentDialogMaskPageClass.h_v  $
 * 
 *    Rev 1.1   26 Jun 2014 18:29:28   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   use SVPictureDisplay-control
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:37:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Dec 2009 13:20:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   19 Jan 2006 11:22:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update code to not process Aux Extent when a gage tool is in the toolset.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   25 Aug 2005 14:10:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed fill color update
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   09 Aug 2005 08:37:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   19 May 2005 15:02:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented Shape Mask
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   18 May 2005 14:10:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  484
 * SCR Title:  Display Mask on Source Image
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added support for mask draw criteria combo.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   16 Feb 2005 15:21:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   07 Aug 2003 12:03:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  380
 * SCR Title:  Add Image Mask option
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new methods for the new mask that is on the dialog
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   08 Jul 2003 10:15:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  378
 * SCR Title:  Implement new CustomFilter for WindowTools that will also be remotely settable
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new code and classes for the new custom filters objects and dialogs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   22 Apr 2003 16:46:40   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   31 Jan 2003 08:59:56   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated OnEditMaskButton and OnInitDialog methods to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   20 Nov 2002 13:59:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed OLE dispatch map
 * Removed OnFinalRelease
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   19 Apr 2001 20:24:46   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   19 Apr 2001 20:05:14   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   01 Feb 2001 11:53:32   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Steve;  Stephen S. Steffan
 * Change Description:  
 *   Modified files to use the SVFileNameClass and the SVFileNameManagerClass if applicable, if the object refers to a filename.  The SVFileNameManagerClass is used if the Object is a persistant file object, that is, if the file object is saved as part of the configuration data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   19 May 2000 16:05:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  145
 * SCR Title:  Half field images and rotation
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Moved SVToolAdjustmentDialogTwoImagePageClass to
 * it's own header and implementation file (
 * SVToolAdjustmentDialogTwoImagePage.h and 
 * SVToolAdjustmentDialogTwoImagePage.cpp
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   23 Mar 2000 16:48:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  118
 * SCR Title:  Integration of LUT Operator
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added new LUT Dialog page.
 * Revised to work with new In Place Image Operator List.
 * Revised to run Tool instead or OperatorList.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   29 Feb 2000 14:28:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  106
 * SCR Title:  Math Tool - List processing
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed Close Button on caption.
 * Added Handler for WM_SYSCOMMAND to restrict SC_CLOSE message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   21 Feb 2000 15:22:16   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  104
 * SCR Title:  New Build Reference Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised 	SVToolAdjustmentDialogImagePageClass
 * constructor to use the id as an argument.
 * Added include files for
 * SVToolImage.h
 * SVTransformationTool.h
 * SVTADlgTransformImagePage.h
 * SVTADlgTranslationPage.h
 * SVTADlgRotationPage.h
 * SVTADlgTransformationLearnPage.h
 * 
 * Revised SVToolAdjustmentDialogSheetClass::addPages()
 * to add property pages for editing the Transformation Tool.
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   07 Feb 2000 22:48:06   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  93
 * SCR Title:  Integrate Multiple Camera Image Input per IPD
 * Checked in by:  Robert;  Robert Massinger
 * Change Description:  
 *   Changed to handle new Page of the Acq. Tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:45:44   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   19 Oct 1999 17:42:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  48
 * SCR Title:  Threshold Page Formula
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Moved SVTADlgThresholdPageClass to it's own file(s)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   08 Oct 1999 16:29:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Corrected problem in SVToolAdjustmentDialogAnalyzerPageClass
 * which would crash when no analyzer selected and the
 * publish button was pressed.
 * Added call to SVToolAdjustmentDialogPassFailPageClass::
 * OnOK()
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   Sep 24 1999 13:58:46   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  4
 * SCR Title:  Add "Load image" option
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Mask Editor OCX access is corrected.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   Sep 23 1999 13:59:22   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  37
 * SCR Title:  Version 3.00 Beta 3 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Bug fixes in threshold edit control code for threshold values.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   Sep 23 1999 08:38:22   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   New variable for edit control.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   Sep 20 1999 15:35:24   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  35
 * SCR Title:  PPQ mode to wait for input data
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Image source Page Bug fix..
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   Sep 20 1999 09:51:12   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  35
 * SCR Title:  PPQ mode to wait for input data
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Fixes in image source page.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   15 Sep 1999 17:11:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised for SVUserMaskOperatorClass Embedded Objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   Sep 13 1999 14:01:08   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Ported TwoImagePage
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   30 Aug 1999 20:15:22   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  14
 * SCR Title:  Integrate luminance analyzer into window tool.
 * Checked in by:  Jim;  James A. Brown
 * Change Description:  
 *   Changed OnPublishButton() to use error handling.
 * 
 * Added 
 * SVErrorClass msvError member to SVToolAdjustmentDialogAnalyzerPageClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   25 Aug 1999 12:21:14   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert Massinger
 * Change Description:  
 *   Removed old Multiple Analyzer Page.
 * Adapted new object type enumerations into SVToolAdjustmentDialogSheet::addPage().
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
