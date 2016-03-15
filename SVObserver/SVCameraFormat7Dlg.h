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

