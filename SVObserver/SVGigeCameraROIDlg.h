//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraROIDlg
//* .File Name       : $Workfile:   SVGigeCameraROIDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   02 Jul 2014 13:06:46  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVOMFCLibrary/SVDeviceParam.h"
#include "SVOMFCLibrary/SVDeviceParams.h"
#include "SVOMFCLibrary/SVCustomDeviceParam.h"
#include "SVOMFCLibrary/SVLongValueDeviceParam.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVAcquisitionClass.h"
//TODO: MZA(10.Nov 2014): Move this files to SVOGui project and then remove folder from include and Namespace add-on add PictureDisplay declaration.
#include "SVOGui/PictureDisplay.h"
#pragma endregion Includes

class ISVCameraDeviceImageFormatUpdater
{
public:
#pragma region Construction
	virtual ~ISVCameraDeviceImageFormatUpdater() {}
#pragma endregion Construction

#pragma region Public Methods
	virtual void SetXOffset(long xOffset)=0;
	virtual void SetYOffset(long yOffset)=0;
	virtual void SetWidth(long width)=0;
	virtual void SetHeight(long height)=0;
	virtual void Update(SVAcquisitionClassPtr pAcqDevice)=0;
#pragma endregion Public Methods
};

class SVGigeCameraROIDlg : public CDialog
{
#pragma region Construction
public:
	SVGigeCameraROIDlg(ISVCameraDeviceImageFormatUpdater& rUpdater, CWnd* pParent = NULL);
	virtual ~SVGigeCameraROIDlg();
#pragma endregion Construction

#pragma region Public Methods
public:
	void SetFormat( SVCameraFormat* pFormat );
	void SetFormatImage( const SVImageInfoClass& rInfo );
	void SetAcquisitionDevice( SVAcquisitionClassPtr pDevice );
	const SVLongValueDeviceParam* GetVerticalBinningParam() const;
	void SetVerticalBinningParam(SVLongValueDeviceParam* pParam);
	const SVLongValueDeviceParam* GetHorizontalBinningParam() const;
	void SetHorizontalBinningParam(SVLongValueDeviceParam* pParam);
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
#pragma region Virtual
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVGigeCameraROIDlg)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL
#pragma endregion Virtual
	// Generated message map functions
	//{{AFX_MSG(SVGigeCameraROIDlg)
	afx_msg void OnDeltaPosSpinHeight(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaPosSpinWidth(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaPosSpinLeft(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaPosSpinTop(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaPosSpinBinningVert(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaPosSpinBinningHoriz(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTakePicture();
	afx_msg void OnChangeROI();
	afx_msg void OnChangeBinningVert();
	afx_msg void OnChangeBinningHoriz();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	DECLARE_EVENTSINK_MAP()

	//************************************
	// Method:    ObjectChangedDialogimage
	// Description: Event-methods, called if overlay-object is changed. Set the new values to the mask properties.
	// Access:    public
	//************************************
	void ObjectChangedExDialogImage(long Tab, long Handle, VARIANT* ParameterList, VARIANT* ParameterValue);
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	void RestoreBinning();
	void SetGraphicROI();
	void SetupVerticalBinning();
	void SetupHorizontalBinning();
	void ShowBinningGroup();
	void UpdateImageDisplay();
	void UpdateDeviceBinningParams();
	void Normalize(CRect &p_roRect);
	void UpdateSpinTopRange();
	void UpdateSpinLeftRange();
	void UpdateSpinWidthRange();
	void UpdateSpinHeightRange();
	long GetScaledMaxWidth() const;
	long GetScaledMaxHeight() const;
	double GetScaledValue(long value, double scaleFactor) const;
	void setImages();
#pragma endregion Private Methods

#pragma region Member Variables
private:
	struct Binning
	{
		bool bExist;
		long minValue;
		long maxValue;
		long value;
		Binning() : bExist(false), minValue(1), maxValue(1), value(1) {}
	};

	//{{AFX_DATA(SVGigeCameraROIDlg)
	enum { IDD = IDD_GIGE_CAMERA_ROI_DLG };
	SvOg::PictureDisplay m_Image;
	CSpinButtonCtrl m_SpinHeight;
	CSpinButtonCtrl m_SpinLeft;
	CSpinButtonCtrl m_SpinWidth;
	CSpinButtonCtrl m_SpinTop;
	CSpinButtonCtrl m_SpinVerticalBinning;
	CEdit m_VerticalBinningEdit;
	CStatic m_VerticalBinningLabel;
	CSpinButtonCtrl m_SpinHorizontalBinning;
	CEdit m_HorizontalBinningEdit;
	CStatic m_HorizontalBinningLabel;
	CButton m_HQModeCheckBox;
	CStatic m_BinningGroupBox;

	int m_iHeight;
	int m_iLeft;
	int m_iTop;
	int m_iWidth;
	//}}AFX_DATA

	SVAcquisitionClassPtr m_pDevice;
	SVCameraFormat* m_pFormat;
	SVImageInfoClass m_ImageInfo;
	SVSmartHandlePointer m_pImageHandle;

	Binning m_verticalBinning;
	Binning m_horizontalBinning;
	SVLongValueDeviceParam* m_verticalBinningParam;
	SVLongValueDeviceParam* m_horizontalBinningParam;
	long m_verticalBinningOriginalValue;
	long m_horizontalBinningOriginalValue;
	ISVCameraDeviceImageFormatUpdater& m_rImageFormatUpdater;
	long m_handleToOverlay;
	static const long m_invalidHandle = -1;
#pragma endregion Member Variables
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVGigeCameraROIDlg.h_v  $
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
 *    Rev 1.2   29 Apr 2014 19:01:16   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Updated to follow coding standards in preparation for upcoming changes related to SVPictureDisplay ActiveX.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   11 Jun 2013 15:26:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 017.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 May 2013 18:26:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  828
 * SCR Title:  Camera Binning
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to fix acquisition image size issue when disabling binning.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 May 2013 19:46:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 016.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 May 2013 09:51:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  828
 * SCR Title:  Camera Binning
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
