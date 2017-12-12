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
#include "CameraLibrary/SVDeviceParam.h"
#include "CameraLibrary/SVDeviceParams.h"
#include "CameraLibrary/SVCustomDeviceParam.h"
#include "CameraLibrary/SVLongValueDeviceParam.h"
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
	SVGigeCameraROIDlg(ISVCameraDeviceImageFormatUpdater& rUpdater, CWnd* pParent = nullptr);
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
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	virtual BOOL OnInitDialog() override;
	virtual void OnOK() override;
	virtual void OnCancel() override;
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
	SVImageBufferHandlePtr m_pImageHandle;

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

