//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPerspectiveWarpPage
//* .File Name       : $Workfile:   SVPerspectiveWarpPage  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:22:22  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "ValuesAccessor.h"
#include "DataController.h"
#include "SVMFCControls/SVEnumerateCombo.h"
#pragma endregion Includes

namespace SvOg
{
	class SVPerspectiveWarpPage : public CPropertyPage
	{
	// Construction
	public:
		SVPerspectiveWarpPage( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, int id = IDD );
		virtual ~SVPerspectiveWarpPage();

		HRESULT SetInspectionData();

		enum { IDD = IDD_TA_PERSPECTIVE_TOOL_DIALOG };
		SvMc::SVEnumerateComboClass	m_FunctionCombo;
		SvMc::SVEnumerateComboClass	m_cbInterpolation;
		protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	protected:
		virtual BOOL OnInitDialog() override;
		afx_msg void OnSelchangePerspectiveUnwarpTypeCombo();
		afx_msg void OnSelchangeInterpolationModeCombo(); 
		afx_msg void OnDestroy();
		DECLARE_MESSAGE_MAP()

	private:
		typedef SvOg::ValuesAccessor<SvOg::BoundValues> ValueCommand;
		typedef SvOg::DataController<ValueCommand, ValueCommand::value_type> Controller;

		const SVGUID& m_rInspectionID;
		const SVGUID& m_rTaskObjectID;
		Controller m_Values;
		long m_lLastWarpType;
	};
} //namespace SvOg
