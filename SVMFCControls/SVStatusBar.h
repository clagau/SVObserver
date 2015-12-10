//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStatusBar
//* .File Name       : $Workfile:   SVStatusBar.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:13:48  $
//******************************************************************************

#ifndef SVSTATUSBAR_H
#define SVSTATUSBAR_H

/*
static UINT indicators[] =
{
	ID_SEPARATOR,
	ID_INDICATOR_INFO,
	ID_INDICATOR_MODE,
	ID_INDICATOR_USER,
	ID_INDICATOR_FREE_MEMORY,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};
*/

static UINT indicators[] =
{
	ID_SEPARATOR,
	ID_INDICATOR_INFO,
	ID_INDICATOR_MODE,
	ID_INDICATOR_FBWF, ///< for file based write filter information
	ID_INDICATOR_USER,
	ID_INDICATOR_GIGE_LICENSE_ERROR,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

class SVStatusBar : public CStatusBar  
{
public:
	SVStatusBar();
	virtual ~SVStatusBar();
	// sets information for the file based write filter
	void SetFbwfInfo(UINT PaneIndex,bool FbwfAvailable, bool FbwfActive, bool FbwfChanging);
	virtual void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );
	COLORREF m_lModeBKColor;
	COLORREF m_lModeForeColor;
	COLORREF m_lErrorBKColor;
	COLORREF m_lErrorForeColor;
	COLORREF m_lFbwfBKColor;
	COLORREF m_lFbwfForeColor;
	CString m_strModeText;
	CString m_strErrorText;
	CString m_strFbwfText; ///< for file based write filter information

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVStatusBar.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:13:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   27 Feb 2013 11:26:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added case for ID_INDICATOR_GIGE_LICENSE_ERROR to display all license errors in the status bar
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   05 Jul 2011 12:35:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  723
 * SCR Title:  Remove Free Memory Check from Status Area
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove the memory check process and status bar display.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   27 Oct 2010 09:32:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix include issues and comment issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   23 Jul 2010 09:35:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  697
 * SCR Title:  Add Free Memory Information to Status Bar
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated status bar functionality to better size the data so that it does not scroll off the screen.  Updated version number to 5.32 Beta 026.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 Jul 2010 12:25:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  697
 * SCR Title:  Add Free Memory Information to Status Bar
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes to status bar to add free memory status.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Aug 2005 11:48:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed bug with status bar text
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   05 Aug 2005 09:07:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Implement Mode on StatusBar and Add color indicators.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
