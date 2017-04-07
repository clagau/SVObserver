//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#include "SVVirtualCamera.h"
#include "SVAcquisitionClass.h"
#pragma endregion Includes

class SVLightReference;

typedef SVVector<SVLightReference*> SVLightReferencePtrVector;

class SVLightReferenceDialogPropertySheetClass : public CPropertySheet
{
public:
	SVLightReferenceDialogPropertySheetClass(UINT nIDCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);
	SVLightReferenceDialogPropertySheetClass(LPCTSTR pszCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);

	virtual ~SVLightReferenceDialogPropertySheetClass();

	bool CreatePages( SVVirtualCameraPtrSet& setCameras, SVLightReferencePtrVector& apLRA );
	void DestroyAllPages();

	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(SVLightReferenceDialogPropertySheetClass)
	//}}AFX_VIRTUAL
	virtual BOOL OnInitDialog() override;

protected:
	int miNumPages;
	SVLightReferencePtrVector mapLRA;

protected:
	DECLARE_DYNAMIC(SVLightReferenceDialogPropertySheetClass)

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

	SVVirtualCamera*       mpCamera;  // for band information
	SVAcquisitionClassPtr  mpDevice;  // for max and min values
	SVLightReference*      mpLR;      // for switching bands (channels) within a property page
	int miAttributeType;              // brightness, contrast, etc.
	CString msAttributeName;
	
	int miCurrentBand;


	//{{AFX_DATA(SVLightReferenceDialogPropertyPageClass)
	enum { IDD = IDD_LIGHTREFERENCE_PAGE };
	CSliderCtrl	mValueSlider;
	long	mlValue;
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
	DWORD CurrentType() {return miAttributeType;}
	CString CurrentName() {return msAttributeName;}

	SVAcquisitionClass* mpAcquisition;

	int miNumBands;
	int miBandSize;
	int miCamBand;
	

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

