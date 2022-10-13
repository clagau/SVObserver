// ******************************************************************************
// * COPYRIGHT (c) 2004 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVWatershedFilterDlg
// * .File Name       : $Workfile:   SVWaterShedFilterDlg.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 16:31:00  $
// ******************************************************************************
#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVOGuiUtility/DataController.h"
#include "SVOGuiUtility/ImageController.h"

#pragma endregion Includes

namespace SvOg
{
	class SVWatershedFilterDlg : public CDialog, protected SvOgu::ImageController
	{
		// Construction
	public:
		SVWatershedFilterDlg(uint32_t inspectionId, uint32_t taskObjectId, uint32_t rFilterId, CWnd* pParent = nullptr);   // standard constructor
		virtual ~SVWatershedFilterDlg();

		HRESULT SetInspectionData();

		// Dialog Data
		//{{AFX_DATA(SVWatershedFilterDlg)
		enum { IDD = IDD_WATERSHED_FILTER };
		CComboBox	m_SVSourceImageCombo;
		BOOL	m_bUseBasin;
		BOOL	m_bSkipLastLevel;
		long	m_lMinVariation;
		BOOL	m_bUseWatershed;
		int		m_iEightWatershedLines;
		int		m_iMinFillBasin;
		int		m_iRegularWatershedLines;
		int		m_iVariationType;
		BOOL	m_bUseMarker;
		//}}AFX_DATA

		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(SVWatershedFilterDlg)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
		//}}AFX_VIRTUAL

	protected:
		// Generated message map functions
		//{{AFX_MSG(SVWatershedFilterDlg)
		virtual BOOL OnInitDialog() override;
		virtual void OnOK() override;
		virtual void OnCancel() override;
		afx_msg void OnSETVarRadio();
		afx_msg void OnSelchangeSourceImageCombo();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	private:
		std::vector<std::string*>    m_AvailableImages;

		const uint32_t m_InspectionID;
		const uint32_t m_TaskObjectID;
		const uint32_t m_filterID;
		SvOgu::ValueController m_values;
	};
} //namespace SvOg
