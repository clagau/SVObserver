// ******************************************************************************
// * COPYRIGHT (c) 2006 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCameraPropDlgClass.h
// * .File Name       : $Workfile:   SVCameraPropDlgClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 11:08:28  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "resource.h"
#include "SVMFCControls\SVListCtrl.h"
#pragma endregion Includes

namespace SvTrig
{
	class SVDigitizerLoadLibraryClass;
}//namespace SvTrig


class SVCameraPropDlgClass : public CDialog
{
public:
	SVCameraPropDlgClass(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVCameraPropDlgClass();

	//{{AFX_DATA(SVCameraPropDlgClass)
	enum { IDD = IDD_CAMERA_PROP };
	SvMc::SVListCtrl	m_svListCtrl;
	//}}AFX_DATA

	SvTrig::SVDigitizerLoadLibraryClass *m_psvDigitizers;
	unsigned long m_digitizerHandle;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVCameraPropDlgClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	// Generated message map functions
	//{{AFX_MSG(SVCameraPropDlgClass)
	virtual BOOL OnInitDialog() override;
	virtual void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

