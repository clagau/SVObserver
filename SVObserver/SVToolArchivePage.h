//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentArchivePage
//* .File Name       : $Workfile:   SVToolArchivePage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   28 Aug 2014 07:25:48  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOGui\ISVPropertyPageDialog.h"
#include "SVOutputInfoListTreeCtrl.h"
#include "SVMFCControls\SVEditNumbers.h"
#include "SVImageListClass.h"
#include "SVArchiveTool.h"
#include "SVGlobal.h"
#pragma endregion Includes

class SVToolAdjustmentDialogSheetClass;

class SVToolAdjustmentArchivePage : public CPropertyPage, public SvOg::ISVPropertyPageDialog
{
public:
	SVToolAdjustmentArchivePage( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* Parent );
	virtual ~SVToolAdjustmentArchivePage();

#pragma region Public Methods
	void BuildImageList();

	// ISVPropertyPageDialog
	bool QueryAllowExit();
#pragma endregion Public Methods
	
#pragma region Public Members
	SVToolAdjustmentDialogSheetClass* m_pParentDialog;
#pragma endregion Public Members

#pragma region Protected Methods
protected:
	//{{AFX_VIRTUAL(SVToolAdjustmentArchivePage)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

	//{{AFX_DATA(SVToolAdjustmentArchivePage)
	enum { IDD = IDD_TA_ARCHIVE_PAGE };
	CStatic	m_wndAvailableArchiveImageMemory;
	CStatic	m_wndTxtAvailableArchiveImageMemory;
	CComboBox	m_cbMode;
	SvMc::CEditNumbers	m_editMaxImages;
	CEdit	m_editImageFilesRoot;
	SVOutputInfoListTreeCtrlClass m_treeResultsList;
	SVOutputInfoListTreeCtrlClass m_treeImagesList;
	CEdit	m_editArchiveFileName;
	BOOL	m_checkAppendArchive;
	BOOL	m_checkStopAtMaxImages;
	BOOL	m_bUseColumnHeaders;
	int		m_iModeIndex;
	CString	m_strAvailableArchiveImageMemory;
	//}}AFX_DATA
	SVArchiveTool* m_pTool;
	SVImageListClass m_imageListAll;
	SVArchiveMethodEnum m_eSelectedArchiveMethod;
	long m_lImagesToArchive;
	__int64 m_lTotalArchiveImageMemoryAvailable;
	__int64 m_lInitialArchiveImageMemoryUsage;
	__int64 m_lInitialArchiveImageMemoryUsageExcludingThisTool;
	__int64 m_lInitialToolImageMemoryUsage;
	__int64 m_lToolImageMemoryUsage;

	typedef std::map <SVImageClass*, long> MapSelectedImageType;
	MapSelectedImageType m_mapSelectedImageMemUsage;
	MapSelectedImageType m_mapInitialSelectedImageMemUsage;

	__int64 CalculateToolMemoryUsage();
	__int64 CalculateFreeMem();

	// Generated message map functions

	//{{AFX_MSG(SVToolAdjustmentArchivePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowse();
	afx_msg void OnBrowse2();
	afx_msg void OnSelchangeModeCombo();
	afx_msg void OnChangeEditMaxImages();
	afx_msg void OnBnClickedHeaderCheck();
	afx_msg void OnBnClickedHeaderBtn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	bool GetSelectedHeaderNamePairs( StringPairVect& HeaderPairs );
	bool StoreHeaderValuesToTool( StringPairVect& HeaderPairs );

	bool CanSelectObjectCallback( SVObjectReference refObject, bool bCurrentState, int );
	void OnClickResultsTreeCtrl( int Par1 );
	void UpdateHeaderBtn();
#pragma endregion Protected Methods

#pragma region Private Members
private:
	CString m_sMaxImageNumber;
	bool m_bInit;
#pragma endregion Private Members
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVToolArchivePage.h_v  $
 * 
 *    Rev 1.4   28 Aug 2014 07:25:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  916
 * SCR Title:  Fix issue with available memory calculation with Archive Tool (SV0-350)
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added member variable m_bInit.  When the dialog is being initialized do not display error messages in method  OnChangeEditMaxImages
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   12 Aug 2014 06:47:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  872
 * SCR Title:  Add Archive Tool Headers to Archive File
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added click callback to update header button.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   23 Jul 2014 11:37:26   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  916
 * SCR Title:  Fix issue with available memory calculation with Archive Tool (SV0-350)
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added CString m_sMaxImageNumber
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   11 Nov 2013 07:37:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  872
 * SCR Title:  Add Archive Tool Headers to Archive File
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added header support functions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:41:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   11 Apr 2006 15:14:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  559
 * SCR Title:  Fix Archive Tool dialog bug
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed image selection bug - memory calculation
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   25 Jan 2006 12:40:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   modified to match change in SVOutputInfoListTreeCtrl
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   08 Aug 2005 14:00:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   renamed SVToolArchive to SVArchiveTool
 * implemented usage tracking
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   03 Aug 2005 14:56:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   change ArchivePageOK to QueryAllowExit
 * create paths on exit
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   20 May 2005 12:29:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added archive mode combo
 * archive member variable name changed
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   22 Apr 2003 16:46:58   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:50:58   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   Nov 17 1999 07:25:14   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added check for unique results archive file paths.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   Oct 26 1999 15:04:38   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Limit Archived images to a user selectable number.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   Oct 22 1999 14:23:58   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Implement the capability to append to results archive, and include a time stamp for first record archived on each 'run'.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   Oct 14 1999 11:29:16   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Image archive changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   Oct 13 1999 14:37:48   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changes required to archive images into .bmp files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   Sep 16 1999 15:32:50   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Fix class name again.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   Sep 16 1999 14:33:00   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Class Name changed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Sep 15 1999 15:58:54   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added a Archive configuration 'page' to the tool adjust property 'sheet'.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/