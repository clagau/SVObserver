// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
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
#include "ValuesAccessor.h"
#include "GuiController.h"
#include "ImageController.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVOGui
	{
		class SVWatershedFilterDlg : public CDialog, protected SvOg::ImageController
		{
			// Construction
		public:
			SVWatershedFilterDlg(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, const SVGUID& rFilterID, CWnd* pParent = nullptr);   // standard constructor
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
			std::vector<SVString*>    m_AvailableImages;

			const SVGUID& m_rInspectionID;
			const SVGUID& m_rTaskObjectID;
			const SVGUID& m_filterID;
			typedef SvOg::ValuesAccessor<SvOg::BoundValues> ValueCommand;
			typedef SvOg::GuiController<ValueCommand, ValueCommand::value_type> Controller;
			Controller m_Values;
		};
	}  //end namespace SVOGUI
}  //end namespace Seidenader

namespace SvOg = Seidenader::SVOGui;

