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

#ifndef SVANALYZERHISTOGRAMSETUPCLASS_H
#define SVANALYZERHISTOGRAMSETUPCLASS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "SVTaskObjectInterfaceClass.h"
#include "svhistogram.h"
#include "afxwin.h"
#include "afxcmn.h"

class SVHistogramAnalyzerClass;

class SVSquare: public CStatic
{
	DECLARE_DYNAMIC(SVSquare)
public:
	SVSquare():CStatic(), m_color(color::Aqua)
	{
		RegisterWindowClass();
	}

	void SetColor(COLORREF color) { m_color = color; }
	afx_msg void OnPaint();
protected:
	DECLARE_MESSAGE_MAP()
private:
	BOOL RegisterWindowClass();
	COLORREF m_color;
};

// SVHistogramAnalyzerSetup dialog

class SVHistogramAnalyzerSetupClass : public CDialog, public SVTaskObjectInterfaceClass
{
//	DECLARE_DYNAMIC(SVHistogramAnalyzerSetupClass)
public:
	SVHistogramAnalyzerClass* m_pAnalyzer;

	virtual HRESULT SetInspectionData();
	void Refresh();

protected:
	SVErrorClass  msvError;

private:
	void SetResultRange(const GUID & resultGuid);

public:
	SVHistogramAnalyzerSetupClass(CWnd* pParent = NULL);   // standard constructor
	virtual ~SVHistogramAnalyzerSetupClass();

// Dialog Data
	enum { IDD = IDD_HISTOGRAM_ANALYZER_SETUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	SVHistogram m_histogram;

	//int m_highClipEdit;
	//int m_lowClipEdit;
	//INT m_scaleRadio;
	//double m_meanText;
	//double m_stdDevText;
	//double m_varianceText;
	//int m_minText;
	//int m_maxText;
	//int m_countText;
	//int m_grayscaleText;
	//int m_highPeak;
	//int m_lowPeak;
	//int m_valley;
	//int m_threshold;
	////int m_minHeight;
	//int m_defaultPeak;
	////BOOL m_useDefaultPeak;
	//BOOL m_accumulate;
	//int m_heightThreshold;

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

	//BOOL UpdateData(BOOL bSaveAndValidate = TRUE);

	bool setHistogram(const SVHistogramBase &);
	static const LPCTSTR c_zerosz;
	static const LPCTSTR c_255sz;
	static const int c_zero = 0;
	static const int c_255  = 255;
	virtual BOOL OnInitDialog();
	SVSquare m_firstSquare;
	SVSquare m_secondSquare;
	SVSquare m_valleySquare;
};
#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVHistogramAnalyzerSetup.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:46:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/