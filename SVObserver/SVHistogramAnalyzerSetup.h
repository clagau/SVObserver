//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVHistogramAnalyzerSetup
//* .File Name       : $Workfile:   SVHistogramAnalyzerSetup.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:46:44  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVTaskObjectValueInterface.h"
#include "SVMFCControls\SVHistogram.h"
#include "afxwin.h"
#include "afxcmn.h"
#pragma region Includes

class SVHistogramAnalyzerClass;

class SVSquare: public CStatic
{
	DECLARE_DYNAMIC(SVSquare)
public:
	SVSquare():CStatic(), m_color(SvMc::color::Aqua)
	{
		RegisterWindowClass();
	}
	virtual ~SVSquare() {}
	void SetColor(COLORREF color) { m_color = color; }
	afx_msg void OnPaint();
protected:
	DECLARE_MESSAGE_MAP()
private:
	BOOL RegisterWindowClass();
	COLORREF m_color;
};

// SVHistogramAnalyzerSetup dialog

class SVHistogramAnalyzerSetupClass : public CDialog, public SVTaskObjectValueInterface
{
//	DECLARE_DYNAMIC(SVHistogramAnalyzerSetupClass)
public:
	SVHistogramAnalyzerClass* m_pAnalyzer;

	HRESULT SetInspectionData();
	void Refresh();

protected:
	
private:
	void SetResultRange(const GUID & resultGuid);

public:
	SVHistogramAnalyzerSetupClass(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVHistogramAnalyzerSetupClass();

// Dialog Data
	enum { IDD = IDD_HISTOGRAM_ANALYZER_SETUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	SvMc::SVHistogram m_histogram;

	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedRadiolin();
	afx_msg void OnBnClickedRadiolog();
	afx_msg void OnBnClickedResetbutton();
	afx_msg void OnBnClickedAccumulate();
	afx_msg void OnBnClickedRadiodynamic();
	afx_msg void OnBnClickedRadiofixed();
	afx_msg void OnBnClickedValleyRange();
	afx_msg void OnBnClickedSecondpeakRange();
	afx_msg void OnBnClickedFirstpeakRange();
	afx_msg void OnEnChangeDistThreshold();
	afx_msg void OnEnChangeDefaultPeak();
	afx_msg void OnEnChangeHeightThreshold();
	afx_msg void OnEnChangeLowclip();
	afx_msg void OnEnChangeHighclip();
	afx_msg void OnEnChangeFixededit();
	afx_msg void OnEnChangeLowValley();
	afx_msg void OnEnChangeHighValley();
	afx_msg void OnEnChangeDefaultValley();
	afx_msg void OnEnKillfocusDistThreshold();
	afx_msg void OnEnKillfocusHeightThreshold();
	afx_msg void OnEnKillfocusLowclip();
	afx_msg void OnEnKillfocusHighclip();
	afx_msg void OnEnKillfocusDefaultPeak();
	afx_msg void OnEnKillfocusFixededit();
	afx_msg void OnEnKillfocusLowValley();
	afx_msg void OnEnKillfocusHighValley();
	afx_msg void OnEnKillfocusDefaultValley();
	afx_msg void OnClose();

	bool setHistogram(const SvUl::SVHistogramBase &);
	static const LPCTSTR c_zerosz;
	static const LPCTSTR c_255sz;
	static const int c_zero = 0;
	static const int c_255  = 255;
	virtual BOOL OnInitDialog() override;
	SVSquare m_firstSquare;
	SVSquare m_secondSquare;
	SVSquare m_valleySquare;
};

