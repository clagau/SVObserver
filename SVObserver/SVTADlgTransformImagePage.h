//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgTransformImagePage
//* .File Name       : $Workfile:   SVTADlgTransformImagePage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:24:02  $
//******************************************************************************

#ifndef SVTADLGTRANSFORMIMAGEPAGE_H
#define SVTADLGTRANSFORMIMAGEPAGE_H

#include "SVToolAdjustmentDialogImagePageClass.h"

class SVToolAdjustmentDialogTransformImagePageClass : public SVToolAdjustmentDialogImagePageClass
{
	//{{AFX_VIRTUAL(SVToolAdjustmentDialogTransformImagePageClass)
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV
	//}}AFX_VIRTUAL

	//{{AFX_MSG(SVToolAdjustmentDialogTransformImagePageClass)
protected:
	afx_msg void OnSelchangeCombo1();
	afx_msg void OnUseExtentsOnly();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	//{{AFX_DATA(SVToolAdjustmentDialogTransformImagePageClass)
protected:
	enum { IDD = IDD_TA_TRANSFORM_IMAGE_DIALOG };
	CButton	m_useExtentsOnlyCheckBox;
	BOOL	m_useExtentsOnly;
	//}}AFX_DATA

public:
	SVToolAdjustmentDialogTransformImagePageClass( SVToolAdjustmentDialogSheetClass* Parent, int id = IDD );
	virtual ~SVToolAdjustmentDialogTransformImagePageClass();

	virtual HRESULT SetInspectionData();

protected:
	SVBoolValueObjectClass* pUseExtentsOnly;

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTADlgTransformImagePage.h_v  $
 * 
 *    Rev 1.0   24 Apr 2013 11:24:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   05 Dec 2012 12:02:00   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changes made for adding the the new Shift Tool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Aug 2005 08:25:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 16:21:26   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   21 Feb 2000 09:32:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  104
 * SCR Title:  New Build Reference Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin.
 * New Dialog Class for selecting the source image for the
 * Transformation Tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
