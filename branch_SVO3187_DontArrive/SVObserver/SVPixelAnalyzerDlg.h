//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPixelAnalyzerDlg
//* .File Name       : $Workfile:   SVPixelAnalyzerDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:23:08  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOGui/DataController.h"
#pragma endregion Includes


namespace SvAo
{
class PixelAnalyzer;
}

class SVPixelAnalyzerDlg : public CDialog
{
// Construction
public:
	SVPixelAnalyzerDlg(uint32_t inspectionId, uint32_t taskObjectId, CWnd* pParent = nullptr);
	virtual ~SVPixelAnalyzerDlg();
	void SetInspectionData();

	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	virtual void OnOK() override;
	virtual BOOL OnInitDialog() override;
	afx_msg void OnWhiteRad();
	afx_msg void OnBlackRad();
	afx_msg void OnOtherRad();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeGrayscaleEdt();
	virtual void OnCancel() override;
	afx_msg void OnPixelSetRange();
	DECLARE_MESSAGE_MAP()

/*- SetBlack () -------------------------------------------------------------------------*/
/*- Actions associated with the black radio button. This also forces the radio button    */
/*- state. ------------------------------------------------------------------------------*/
    void SetBlack();

/*- SetWhite () -------------------------------------------------------------------------*/
/*- Actions associated with the white radio button. This also forces the radio button    */
/*- state. ------------------------------------------------------------------------------*/
    void SetWhite();

/*- SetOther () -------------------------------------------------------------------------*/
/*- Actions associated with the other radio button. This also forces the radio button    */
/*- state. ------------------------------------------------------------------------------*/
    void SetOther(long lGrayscale);

    void SetGrayscale(long lGrayscale);

	SvAo::PixelAnalyzer*   m_pAnalyzer{ nullptr };
	unsigned long           msvulMaxGrayscale{ 0L };
	unsigned long           msvulMinGrayscale{ 0L };

protected:
	enum { IDD = IDD_PIXEL_ANALYZER_DIALOG };
	CSliderCtrl	m_GrayscaleSld;
	CButton	m_OtherRad;
	CButton	m_WhiteRad;
	CButton	m_BlackRad;
	CEdit	m_GrayScaleEdt;
	long	m_GrayscaleToCount{ 0L };
	CString	m_HighGrayscaleValue;
	CString	m_LowGrayscaleValue;
	int		m_GrayscaleSliderValue{ 0 };

private:
	const uint32_t m_InspectionID;
	const uint32_t m_TaskObjectID;
	SvOg::ValueController m_Values;
};
