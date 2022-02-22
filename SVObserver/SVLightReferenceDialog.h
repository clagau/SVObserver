//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLightReferenceDialogPropertySheetClass
//* .File Name       : $Workfile:   SVLightReferenceDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:47:04  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "InspectionEngine/SVVirtualCamera.h"
#pragma endregion Includes

class SVLightReference;

typedef std::vector<SVLightReference> SVLightReferenceVector;

class SVLightReferenceDialogPropertySheetClass : public CPropertySheet
{
public:
	explicit SVLightReferenceDialogPropertySheetClass(UINT nIDCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);
	explicit SVLightReferenceDialogPropertySheetClass(LPCTSTR pszCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);

	virtual ~SVLightReferenceDialogPropertySheetClass();

	bool CreatePages(SvIe::SVVirtualCameraPtrVector& rCameraVector, SVLightReferenceVector& rLRA );
	void DestroyAllPages();

	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(SVLightReferenceDialogPropertySheetClass)
	//}}AFX_VIRTUAL
	virtual BOOL OnInitDialog() override;

protected:
	//{{AFX_MSG(SVLightReferenceDialogPropertySheetClass)
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

class SVLightReferenceDialogPropertyPageClass : public CPropertyPage
{
public:
	SVLightReferenceDialogPropertyPageClass( LPCTSTR lpszTitle = nullptr );
	virtual ~SVLightReferenceDialogPropertyPageClass();

	SvIe::SVVirtualCamera*       m_pCamera;  // for band information
	SvIe::SVAcquisitionClassPtr  m_pDevice;  // for max and min values
	SVLightReference*      m_pLR;      // for switching bands (channels) within a property page
	int m_AttributeType;              // brightness, contrast, etc.
	CString m_AttributeName;
	
	int miCurrentBand;


	//{{AFX_DATA(SVLightReferenceDialogPropertyPageClass)
	enum { IDD = IDD_LIGHTREFERENCE_PAGE };
	CSliderCtrl	m_ValueSlider;
	long	m_Value;
	//}}AFX_DATA

	// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(SVLightReferenceDialogPropertyPageClass)
	public:
	virtual void OnOK() override;
	virtual BOOL OnSetActive() override;
	virtual void OnReset() override;
	virtual BOOL OnKillActive() override;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

protected:
	void UpdateLightReference();
	DWORD CurrentValue();
	void SetCurrentValue(DWORD dw);
	DWORD CurrentType() {return m_AttributeType;}
	CString CurrentName() {return m_AttributeName;}

	int m_NumBands;
	int m_BandSize;
	int m_CamBand;
	

	DECLARE_DYNCREATE(SVLightReferenceDialogPropertyPageClass)

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(SVLightReferenceDialogPropertyPageClass)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChannel0();
	afx_msg void OnChannel1();
	afx_msg void OnChannel2();
	afx_msg void OnChannel3();
	afx_msg void OnChangeValue();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

