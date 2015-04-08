//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVView
//* .File Name       : $Workfile:   SVView.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 16:20:28  $
//******************************************************************************

#pragma once

class SVImageClass;
class SVImageListClass;
class SVClassInfoStructListClass;

////////////////////////////////////////////////////////////////////////////////
// .Title       : Class SVToolWindow 
////////////////////////////////////////////////////////////////////////////////
class SVToolWindow : public CWnd
{
public:
	SVToolWindow();
	virtual ~SVToolWindow();

protected:
	//{{AFX_MSG(SVToolWindow)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(SVToolWindow)
	//}}AFX_VIRTUAL

};

////////////////////////////////////////////////////////////////////////////////
// .Title       : Class SVAvailableAnalyzerListComboBoxClass 
////////////////////////////////////////////////////////////////////////////////
class SVAvailableAnalyzerListComboBoxClass : public CComboBox
{
public:
	SVAvailableAnalyzerListComboBoxClass();
	virtual ~SVAvailableAnalyzerListComboBoxClass();

	int init( SVClassInfoStructListClass* pClassInfoList, int currentSelection = -1 );
	void add (LPCTSTR ClassName);
	void remove (LPCTSTR ClassName);

protected:
	//{{AFX_MSG(SVAvailableAnalyzerListComboBoxClass)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(SVAvailableAnalyzerListComboBoxClass)
	//}}AFX_VIRTUAL

};


////////////////////////////////////////////////////////////////////////////////
// .Title       : Class SVAvailableSourceImageListComboBoxClass 
////////////////////////////////////////////////////////////////////////////////
class SVAvailableSourceImageListComboBoxClass : public CComboBox
{
public:
	SVAvailableSourceImageListComboBoxClass();
	virtual ~SVAvailableSourceImageListComboBoxClass();

	int init( SVImageListClass* pImageList, SVImageClass* pCurrentSelectedImage );

protected:
	//{{AFX_MSG(SVAvailableSourceImageListComboBoxClass)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(SVAvailableSourceImageListComboBoxClass)
	//}}AFX_VIRTUAL

};

////////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// DevStudio inserts additional declarations immediate in front of the preceding line
////////////////////////////////////////////////////////////////////////////////


//** EOF **
//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVView.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 16:20:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   05 Mar 2013 13:40:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added the add & remove method, to add/remove an analyzer name to the combo box
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   21 Jun 2007 15:07:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   12 Jul 2004 12:05:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   cleaned up code
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   22 Apr 2003 17:30:14   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   31 Jan 2003 09:37:34   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated init method to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   04 Apr 2002 10:33:54   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  286
 * SCR Title:  Upgrade SVObserver to Version 3.37 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed unused SVCameraWindowClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   23 Mar 2000 16:53:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  118
 * SCR Title:  Integration of LUT Operator
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed SVDlgImageClass (now in SVDlgImage.cpp/h)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
