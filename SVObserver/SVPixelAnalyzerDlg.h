//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#include "SVOGui/ValuesAccessor.h"
#include "SVOGui/DataController.h"
#pragma endregion Includes

class SVPixelAnalyzerClass;

class SVPixelAnalyzerDlg : public CDialog
{
// Construction
public:
	SVPixelAnalyzerDlg(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, CWnd* pParent = nullptr);
	virtual ~SVPixelAnalyzerDlg();
	HRESULT SetInspectionData();

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
    unsigned long           SetBlack();

/*- SetWhite () -------------------------------------------------------------------------*/
/*- Actions associated with the white radio button. This also forces the radio button    */
/*- state. ------------------------------------------------------------------------------*/
    unsigned long           SetWhite();

/*- SetOther () -------------------------------------------------------------------------*/
/*- Actions associated with the other radio button. This also forces the radio button    */
/*- state. ------------------------------------------------------------------------------*/
    unsigned long           SetOther(long alGrayscale);

    unsigned long           SetGrayscale(long alGrayscale);

	SVPixelAnalyzerClass*   m_pAnalyzer{ nullptr };
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
	const SVGUID& m_rInspectionID;
	const SVGUID& m_rTaskObjectID;
	typedef SvOg::ValuesAccessor<SvOg::BoundValues> ValueCommand;
	typedef SvOg::DataController<ValueCommand, ValueCommand::value_type> Controller;
	Controller m_Values;
};
