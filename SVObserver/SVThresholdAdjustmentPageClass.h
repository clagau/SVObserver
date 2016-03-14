//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVThresholdAdjustmentPageClass
//* .File Name       : $Workfile:   SVThresholdAdjustmentPageClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:31:50  $
//******************************************************************************

#ifndef _SVTHRESHOLDADJUSTMENTPAGECLASS_H
#define _SVTHRESHOLDADJUSTMENTPAGECLASS_H

#include "SVEdge.h"
#include "SVValueBaseNormalizerClass.h"
#include "SVValueObjectImpl.h"

#include "SVTaskObjectInterfaceClass.h"

class SVLineAnalyzerClass;
class SVTaskObjectClass;
class SVIPDoc;

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVThresholdAdjustmentPageClass 
// -----------------------------------------------------------------------------
// .Description : e.g. This base class capsules an Image and provides its ...
//              :
//              :
// -----------------------------------------------------------------------------
// .Export
//	 Public Method				Description
//  :
//  :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :10.06.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
class SVThresholdAdjustmentPageClass : public CPropertyPage, public SVTaskObjectInterfaceClass
{
	DECLARE_DYNCREATE(SVThresholdAdjustmentPageClass)

public:
// Dialogfelddaten
	//{{AFX_DATA(SVThresholdAdjustmentPageClass)
	enum { IDD = IDD_GR_THRESHOLD_ADJUST_DIALOG };
	CSliderCtrl	UpperSliderCtrl;
	CSliderCtrl	LowerSliderCtrl;
	CString	StrUpper;
	CString	StrLower;
	//}}AFX_DATA

protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(SVThresholdAdjustmentPageClass)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	SVThresholdAdjustmentPageClass( UINT nIDCaption = 0, int id = IDD );

public:
	~SVThresholdAdjustmentPageClass();

	virtual HRESULT SetInspectionData();

// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(SVThresholdAdjustmentPageClass)
	public:
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL
	
protected:
	void updateDisplay();

public:
//#ifdef _DEBUG
//	virtual void AssertValid() const;
//	virtual void Dump(CDumpContext& dc) const;
//#endif

public:
	//edge value objects
	SVBoolValueObjectClass		*m_pvoUseLowerThresholdSelectable;
	SVBoolValueObjectClass		*m_pvoUseLowerThresholdMaxMinusPercentDiff;
	SVBoolValueObjectClass		*m_pvoUseLowerThresholdMaxMinusOffset;
	SVBoolValueObjectClass		*m_pvoUseLowerThresholdMinPlusOffset;
	SVDWordValueObjectClass		*m_pvoLowerThresholdMaxMinusPercentDiff;
	SVDWordValueObjectClass		*m_pvoLowerThresholdMaxMinusOffset;
	SVDWordValueObjectClass		*m_pvoLowerThresholdMinPlusOffset;
	SVBoolValueObjectClass		*m_pvoUseUpperThresholdSelectable;
	SVBoolValueObjectClass		*m_pvoUseUpperThresholdMaxMinusPercentDiff;
	SVBoolValueObjectClass		*m_pvoUseUpperThresholdMaxMinusOffset;
	SVBoolValueObjectClass		*m_pvoUseUpperThresholdMinPlusOffset;
	SVDWordValueObjectClass		*m_pvoUpperThresholdMaxMinusPercentDiff;
	SVDWordValueObjectClass		*m_pvoUpperThresholdMaxMinusOffset;
	SVDWordValueObjectClass		*m_pvoUpperThresholdMinPlusOffset;

	SVDWordValueObjectClass		*m_pvoUpperThreshold;
	SVDWordValueObjectClass		*m_pvoLowerThreshold;

	SVIPDoc						*PCurrentDocument;
	SVLineAnalyzerClass			*PCurrentAnalyzer;
	SVValueBaseNormalizerClass	Normalizer;
protected:

	SVTaskObjectClass*  m_pAnalyzerOwner;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif	// _SVTHRESHOLDADJUSTMENTPAGECLASS_H

