//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Seidenader Maschinenbau GmbH
/// \file BlobReconstructFilterDlg.h
/// All Rights Reserved
//*****************************************************************************
/// The dialog class for the filter Blob Reconstruct Filters (e.g. EraseBorderBlobs)
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "ValuesAccessor.h"
#include "DataController.h"
#include "SVMFCControls/SVEnumerateCombo.h"
#pragma region Includes

#pragma region Declarations
#pragma endregion Declarations

namespace SvOg
{
class BlobReconstructFilterDlg : public CDialog
{
	// Construction
public:
	BlobReconstructFilterDlg(UINT caption, const SVGUID& rInspectionID, const SVGUID& rFilterID, CWnd* pParent = nullptr);   // standard constructor
	virtual ~BlobReconstructFilterDlg();

	HRESULT SetInspectionData();

	// Dialog Data
	//{{AFX_DATA(BlobReconstructFilterDlg)
	enum { IDD = IDD_BLOBRECONSTRUCT_FILTER	};
	BOOL	m_bGrayScale;
	SvMc::SVEnumerateComboClass m_cbBlobColor;
	//}}AFX_DATA


	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(BlobReconstructFilterDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
																 //}}AFX_VIRTUAL

protected:

	// Generated message map functions
	//{{AFX_MSG(BlobReconstructFilterDlg)
	virtual BOOL OnInitDialog() override;
	virtual void OnOK() override;
	virtual void OnCancel() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	const SVGUID& m_rInspectionID;
	const SVGUID& m_filterID;
	const UINT m_captionId;
	typedef SvOg::DataController<SvOg::ValuesAccessor, SvOg::ValuesAccessor::value_type> Controller;
	Controller m_Values;
};
} //namespace SvOg

