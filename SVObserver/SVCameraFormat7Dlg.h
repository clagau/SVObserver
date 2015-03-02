// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCameraFormat7Dlg
// * .File Name       : $Workfile:   SVCameraFormat7Dlg.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.3  $
// * .Check In Date   : $Date:   02 Jul 2014 13:06:46  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
//TODO: MZA(10.Nov 2014): Move this files to SVOGui project and then remove folder from include and Namespace add-on add PictureDisplay declaration.
#include "SVOGui/PictureDisplay.h"
#include "SVOMFCLibrary/SVDeviceParams.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVAcquisitionClass.h"
#pragma endregion Includes

class SVCameraFormat7Dlg : public CDialog
{
#pragma region Construction
public:
	SVCameraFormat7Dlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~SVCameraFormat7Dlg();
#pragma endregion Construction

#pragma region Public Methods
public:
	void SetFormat( SVCameraFormat* pFormat );
	void SetFormat7Image( const SVImageInfoClass& rInfo );
	void SetAcquisitionDevice( SVAcquisitionClassPtr pDevice );
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
#pragma region Virtual
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVCameraFormat7Dlg)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL
#pragma endregion Virtual

	// Generated message map functions
	//{{AFX_MSG(SVCameraFormat7Dlg)
	afx_msg void OnDeltaPosSpinHeight(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaPosSpinWidth(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaPosSpinLeft(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaPosSpinTop(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTakePicture();
	afx_msg void OnChangeROI();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	DECLARE_EVENTSINK_MAP()

	//************************************
	// Method:    ObjectChangedExDialogImage
	// Description: Event-methods, called if overlay-object is changed. Set the new values to the ROI properties.
	// Parameter: long Tab: handle to the tab
	// Parameter: long Handle: handle to the overlay
	// Parameter: VARIANT * ParameterList: List of the parameter names
	// Parameter: VARIANT * ParameterValue: List of the parameter values
	// Returns:   void
	//************************************
	void ObjectChangedExDialogImage(long Tab, long Handle, VARIANT* ParameterList, VARIANT* ParameterValue);
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	void SetGraphicROI();
	void OnDeltaPosSpin( NMHDR* pNMHDR );
	void Normalize(CRect &p_roRect);
	void setImages();
#pragma endregion Private Methods

#pragma region Member Variables
	//{{AFX_DATA(SVCameraFormat7Dlg)
	enum { IDD = IDD_CAMERA_FORMAT7_DLG };
	SvOg::PictureDisplay m_Image;
	CSpinButtonCtrl m_SpinHeight;
	CSpinButtonCtrl m_SpinLeft;
	CSpinButtonCtrl m_SpinWidth;
	CSpinButtonCtrl m_SpinTop;
	int m_iHeight;
	int m_iLeft;
	int m_iTop;
	int m_iWidth;
	//}}AFX_DATA

	SVAcquisitionClassPtr m_pDevice;
	SVCameraFormat* m_pFormat;
	SVImageInfoClass m_ImageInfo;
	SVSmartHandlePointer m_pImageHandle;
	long m_handleToOverlay;
	static const long m_invalidHandle = -1;
#pragma endregion Member Variables
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVCameraFormat7Dlg.h_v  $
 * 
 *    Rev 1.3   02 Jul 2014 13:06:46   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  902
 * SCR Title:  Change Complex Dialog Image Displays to Use SVPictureDisplay ActiveX
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   use SVPictureDisplay-control
 * cleanup (e.g. use static_cast instead of c-style cast)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   29 Apr 2014 19:01:14   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Updated to follow coding standards in preparation for upcoming changes related to SVPictureDisplay ActiveX.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 May 2013 19:45:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 016.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   10 May 2013 11:27:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  828
 * SCR Title:  Camera Binning
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised SetFormat7Image method arguments to be const
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 09:49:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   22 Mar 2011 07:45:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated static global classes to singletons to promote proper and defined construction and destruction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   10 Feb 2011 14:33:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   07 Jul 2010 16:10:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to make Single Grab functionality work for IEEE1394 and GigE.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   15 Dec 2009 15:13:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   14 Jul 2009 12:06:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code based on data manager and acquisition changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   28 Mar 2006 14:35:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  550
 * SCR Title:  Fix Format7 problem when changing width of image using the Intek driver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update dialog to have an image buffer attached to it so the user can use the mouse to adjust the ROI size and position.  The acquired image will get copied into the dialog buffer.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   17 Feb 2005 13:20:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   12 Aug 2003 09:00:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   general "first iteration" code changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   07 Aug 2003 12:43:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   modified code to grab and display camera image
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   31 Jul 2003 09:48:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added #include "SVImagingDeviceParams.h"
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   28 Jul 2003 10:34:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   initial iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
