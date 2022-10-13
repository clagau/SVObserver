//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file ReconstructFilterDlg.h
/// All Rights Reserved
//*****************************************************************************
/// The dialog class for the filter Reconstruct Filter
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVOGuiUtility/DataController.h"
#include "SVOGuiUtility/ImageController.h"
#include "SVMFCControls/SVEnumerateCombo.h"
#include "SVOGuiUtility/PictureDisplay.h"
#pragma region Includes

#pragma region Declarations
#pragma endregion Declarations

namespace SvOg
{
class ReconstructFilterDlg : public CDialog
{
	// Construction
public:
	ReconstructFilterDlg(uint32_t inspectionID, uint32_t taskObjectId, uint32_t filterID, CWnd* pParent = nullptr);   // standard constructor
	virtual ~ReconstructFilterDlg();

	HRESULT SetInspectionData();

	// Dialog Data
	//{{AFX_DATA(ReconstructFilterDlg)
	enum { IDD = IDD_RECONSTRUCT_FILTER };
	BOOL	m_bGrayScale;
	SvMc::SVEnumerateCombo m_cbBlobColor;
	CComboBox	m_SeedImageCombo;
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
	afx_msg void OnSelchangeImageCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void setImages();

private:
	const uint32_t m_InspectionID;
	const uint32_t m_filterID;
	SvOgu::ValueController m_values;
	std::string m_seedImageName;
	SvOgu::ImageController m_imageController;
	SvOgu::PictureDisplay m_pictureDisplay;
};
} //namespace SvOg

