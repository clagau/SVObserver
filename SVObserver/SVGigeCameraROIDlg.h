//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraROIDlg
//* .File Name       : $Workfile:   SVGigeCameraROIDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   11 Jun 2013 15:26:06  $
//******************************************************************************

#ifndef SVGIGECAMERAROIDLG_H
#define SVGIGECAMERAROIDLG_H

#include "resource.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVOMFCLibrary/SVDeviceParam.h"
#include "SVOMFCLibrary/SVDeviceParams.h"
#include "SVOMFCLibrary/SVCustomDeviceParam.h"
#include "SVOMFCLibrary/SVLongValueDeviceParam.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVAcquisitionClass.h"
#include "SVDlgImageGraphROI.h"

class ISVCameraDeviceImageFormatUpdater
{
public:
	virtual ~ISVCameraDeviceImageFormatUpdater() {}
	virtual void SetXOffset(long xOffset)=0;
	virtual void SetYOffset(long yOffset)=0;
	virtual void SetWidth(long width)=0;
	virtual void SetHeight(long height)=0;
	virtual void Update(SVAcquisitionClassPtr pAcqDevice)=0;
};

class SVGigeCameraROIDlg : public CDialog
{
private:
	struct Binning
	{
		bool bExist;
		long minValue;
		long maxValue;
		long value;
		Binning() : bExist(false), minValue(1), maxValue(1), value(1) {}
	};

public:
	SVGigeCameraROIDlg(ISVCameraDeviceImageFormatUpdater& rUpdater, CWnd* pParent = NULL);
	virtual ~SVGigeCameraROIDlg();

	//{{AFX_DATA(SVGigeCameraROIDlg)
	enum { IDD = IDD_GIGE_CAMERA_ROI_DLG };
	SVDlgImageGraphROI	m_Image;
	CSpinButtonCtrl	m_SpinHeight;
	CSpinButtonCtrl	m_SpinLeft;
	CSpinButtonCtrl	m_SpinWidth;
	CSpinButtonCtrl	m_SpinTop;
	CSpinButtonCtrl	m_SpinVerticalBinning;
	CEdit m_VerticalBinningEdit;
	CStatic m_VerticalBinningLabel;
	CSpinButtonCtrl	m_SpinHorizontalBinning;
	CEdit m_HorizontalBinningEdit;
	CStatic m_HorizontalBinningLabel;
	CButton m_HQModeCheckBox;
	CStatic m_BinningGroupBox;

	int		m_iHeight;
	int		m_iLeft;
	int		m_iTop;
	int		m_iWidth;
	//}}AFX_DATA

	void SetFormat( SVCameraFormat* pFormat );

	void SetFormatImage( const SVImageInfoClass& rInfo );
	void SetAcquisitionDevice( SVAcquisitionClassPtr pDevice );

	const SVLongValueDeviceParam* GetVerticalBinningParam() const;
	void SetVerticalBinningParam(SVLongValueDeviceParam* pParam);

	const SVLongValueDeviceParam* GetHorizontalBinningParam() const;
	void SetHorizontalBinningParam(SVLongValueDeviceParam* pParam);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVGigeCameraROIDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	// Generated message map functions
	//{{AFX_MSG(SVGigeCameraROIDlg)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposSpinHeight(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinLeft(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinTop(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinWidth(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinBinningVert(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinBinningHoriz(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTakePicture();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnChangeROI();
	afx_msg void OnChangeBinningVert();
	afx_msg void OnChangeBinningHoriz();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void SetGraphicROI();
	void Normalize(CRect &p_roRect);

	SVSmartHandlePointer GetImageHandle() const;
	void SetupVerticalBinning();
	void SetupHorizontalBinning();
	void ShowBinningGroup();
	void UpdateDeviceBinningParams();
	void UpdateSpinTopRange();
	void UpdateSpinLeftRange();
	void UpdateSpinWidthRange();
	void UpdateSpinHeightRange();

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

private:
	void RestoreBinning();
	void UpdateImageDisplay();
	long GetScaledMaxWidth() const;
	long GetScaledMaxHeight() const;
	double GetScaledValue(long value, double scaleFactor) const;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVGigeCameraROIDlg.h_v  $
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
