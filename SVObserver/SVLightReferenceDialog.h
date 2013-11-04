//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLightReferenceDialogPropertySheetClass
//* .File Name       : $Workfile:   SVLightReferenceDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:47:04  $
//******************************************************************************

#ifndef SVOBSERVERDIALOG_H
#define SVOBSERVERDIALOG_H

#include "SVVirtualCamera.h"
#include "SVAcquisitionClass.h"

class SVLightReference;

typedef SVVector< SVLightReference* > SVLightReferenceArray;

class SVLightReferenceDialogPropertySheetClass : public CPropertySheet
{
public:
	SVLightReferenceDialogPropertySheetClass(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	SVLightReferenceDialogPropertySheetClass(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

	virtual ~SVLightReferenceDialogPropertySheetClass();

	bool CreatePages( SVVirtualCameraPtrSet& setCameras, SVLightReferenceArray& apLRA );
	void DestroyAllPages();

	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(SVLightReferenceDialogPropertySheetClass)
	//}}AFX_VIRTUAL
	virtual BOOL OnInitDialog();

protected:
	int miNumPages;
	SVLightReferenceArray mapLRA;

protected:
	DECLARE_DYNAMIC(SVLightReferenceDialogPropertySheetClass)

	//{{AFX_MSG(SVLightReferenceDialogPropertySheetClass)
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};



class SVLightReferenceDialogPropertyPageClass : public CPropertyPage
{
public:
	SVLightReferenceDialogPropertyPageClass( LPCTSTR lpszTitle = NULL );
	~SVLightReferenceDialogPropertyPageClass();

	SVVirtualCamera*       mpCamera;  // for band information
	SVAcquisitionClassPtr  mpDevice;  // for max and min values
	SVLightReference*      mpLR;      // for switching bands (channels) within a property page
	int miAttributeType;              // brightness, contrast, etc.
	CString msAttributeName;
	
	int miCurrentBand;


	//{{AFX_DATA(SVLightReferenceDialogPropertyPageClass)
	enum { IDD = IDD_LIGHTREFERENCE_PAGE };
	CSliderCtrl	mValueSlider;
	long	mlValue;
	//}}AFX_DATA

	// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(SVLightReferenceDialogPropertyPageClass)
	public:
	virtual void OnOK();
	virtual BOOL OnSetActive();
	virtual void OnReset();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

protected:
	void UpdateLightReference();
	DWORD CurrentValue();
	void SetCurrentValue(DWORD dw);
	DWORD CurrentType() {return miAttributeType;}
	CString CurrentName() {return msAttributeName;}

	SVAcquisitionClass* mpAcquisition;

	int miNumBands;
	int miBandSize;
	int miCamBand;
	

	DECLARE_DYNCREATE(SVLightReferenceDialogPropertyPageClass)

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(SVLightReferenceDialogPropertyPageClass)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChannel0();
	afx_msg void OnChannel1();
	afx_msg void OnChannel2();
	afx_msg void OnChannel3();
	afx_msg void OnChangeValue();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}
// DevStudio inserts additional declarations immediate in front of the preceding line
////////////////////////////////////////////////////////////////////////////////

#endif	//	SVLIGHTREFERENCEDIALOG_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVLightReferenceDialog.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 11:47:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   28 Sep 2011 12:58:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to fix issues with camera image interface and methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   22 Mar 2011 07:57:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated static global classes to singletons to promote proper and defined construction and destruction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   08 Dec 2010 12:56:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   08 Nov 2010 14:40:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   03 Mar 2010 12:03:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to remove duplicate containers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   16 Sep 2008 14:24:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised CreatePages to check if LightReference is available
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   30 Jul 2004 11:45:22   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  441
 * SCR Title:  Fix Light Reference Dialog - White Balance
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   cleaned up code
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   01 Apr 2004 10:22:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  407
 * SCR Title:  Fix Multi-band Issues with Light Reference Dialog
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added method CurrentName()
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   17 Nov 2003 16:26:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed problem handling multi-banded cams
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   22 Oct 2003 09:34:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed to reference attributes by type, not index
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   08 Jul 2003 11:28:22   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed SVLightReferenceArray to SVLightReference
 * general cleanup
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   22 Apr 2003 10:39:08   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
