//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolSetListCtrl
//* .File Name       : $Workfile:   SVToolSetListCtrl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   13 May 2013 16:29:46  $
//******************************************************************************

#ifndef INCL_SVTOOLSETLISTCTRL_H
#define INCL_SVTOOLSETLISTCTRL_H

#include "SVTaskObjectListCtrl.h"
#include "SVUtilityLibrary/SVGUID.h"

/////////////////////////////////////////////////////////////////////////////
// SVTaskObjectTreeCtrlClass window

class SVToolSetListCtrlClass : public SVTaskObjectListCtrlClass
{
	DECLARE_DYNCREATE(SVToolSetListCtrlClass)

public:
	//{{AFX_MSG(SVToolSetListCtrlClass)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVToolSetListCtrlClass)
	//}}AFX_VIRTUAL

public:
	SVToolSetListCtrlClass();

	virtual ~SVToolSetListCtrlClass();

	virtual void Rebuild();
	virtual void RebuildImages();

	SVGUID GetCurrentSelectionID();

	int GetCurrentSelection( int StartAt = -1 /* start searching at top */ );
	int SetCurrentSelection( int nIndex = -1 /* to deselect */ );
	int SetCurrentSelection( const SVGUID& p_rObjectID );
	void SaveScrollPos();
	void RestoreScrollPos();

protected:
	void CreateImageLists();

	CImageList m_oStateImageList;

	int m_iNone;
	int m_iInvalid;
	int m_iDisabled;
	int m_iPass;
	int m_iFail;
	int m_iWarn;
	int m_iUnknown;
	int m_iTopIndex;

public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif 

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVToolSetListCtrl.h_v  $
 * 
 *    Rev 1.1   13 May 2013 16:29:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 015.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   08 May 2013 10:49:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  834
 * SCR Title:  Modify tool set list to maintain position when tools are deleted
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added functions to support selected tool with keyboard and scroll bars.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:47:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   02 Jul 2012 17:50:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   16 Aug 2005 15:25:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  506
 * SCR Title:  Update Tool Set List Tab to include an indicator when a tool is invalid
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added functionality to paint tool status Icons in front of the tool names.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   22 Apr 2003 16:56:04   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   20 Nov 2002 14:42:18   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed OLE Automation
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   Feb 01 2000 10:13:44   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  90
 * SCR Title:  Save  Operator Tool Move Priviledge in Configuration
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added method to allow software selection of 'current selected' tool in list control.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:54:22   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   27 Aug 1999 15:04:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  Steve Jones
 * Change Description:  
 *   Revised to translate list control index to tTskObjectList index
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   27 Aug 1999 08:57:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  Steve Jones
 * Change Description:  
 *   New Class derived from SVTaskObjectListCtrlClass.
 * Provides functionality relevant to the displaying of the tool list.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
