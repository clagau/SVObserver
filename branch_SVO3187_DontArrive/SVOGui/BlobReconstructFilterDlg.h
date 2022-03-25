//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file BlobReconstructFilterDlg.h
/// All Rights Reserved
//*****************************************************************************
/// The dialog class for the filter Blob Reconstruct Filters (e.g. EraseBorderBlobs)
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
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
	BlobReconstructFilterDlg(UINT caption, uint32_t inspectionId, uint32_t filterId, CWnd* pParent = nullptr);   // standard constructor
	virtual ~BlobReconstructFilterDlg();

	HRESULT SetInspectionData();

	// Dialog Data
	//{{AFX_DATA(BlobReconstructFilterDlg)
	enum { IDD = IDD_BLOBRECONSTRUCT_FILTER	};
	BOOL	m_bGrayScale;
	SvMc::SVEnumerateCombo m_cbBlobColor;
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
	const uint32_t m_InspectionID;
	const uint32_t m_filterID;
	const UINT m_captionId;
	ValueController m_values;
};
} //namespace SvOg

