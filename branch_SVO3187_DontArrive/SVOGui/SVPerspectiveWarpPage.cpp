//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPerspectiveWarpPage
//* .File Name       : $Workfile:   SVPerspectiveWarpPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 12:31:44  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVPerspectiveWarpPage.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	BEGIN_MESSAGE_MAP(SVPerspectiveWarpPage, CPropertyPage)
		//{{AFX_MSG_MAP(SVPerspectiveWarpPage)
		ON_CBN_SELCHANGE(IDC_PERSPECTIVE_UNWARP_TYPE_COMBO, OnSelchangePerspectiveUnwarpTypeCombo)
		ON_CBN_SELCHANGE(IDC_INTERPOLATION_MODE_COMBO, OnSelchangeInterpolationModeCombo)
		ON_WM_DESTROY()
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	SVPerspectiveWarpPage::SVPerspectiveWarpPage( uint32_t inspectionId, uint32_t taskObjectId, int ID ) 
	: CPropertyPage( ID )
	, m_InspectionID{ inspectionId }
	, m_TaskObjectID{ taskObjectId }
	, m_values{ SvOg::BoundValues{ inspectionId, taskObjectId } }
	{
	}

	SVPerspectiveWarpPage::~SVPerspectiveWarpPage()
	{
	}

	void SVPerspectiveWarpPage::SetInspectionData()
	{
		UpdateData(true); // get data from dialog

		int CurrentSelection = m_FunctionCombo.GetCurSel();
		if( 0 <= CurrentSelection)
		{
			long Value = static_cast<long> (m_FunctionCombo.GetItemData(CurrentSelection));
			m_values.Set<long>(SvPb::WarpTypeEId, Value);
		}

		CurrentSelection = m_cbInterpolation.GetCurSel();
		if( CurrentSelection >= 0 )
		{
			long Value = static_cast<long> (m_cbInterpolation.GetItemData(CurrentSelection));
			m_values.Set<long>(SvPb::OutputInterpolationModeEId, Value);
		}

		m_values.Commit();
	}


	void SVPerspectiveWarpPage::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(SVPerspectiveWarpPage)
		DDX_Control(pDX, IDC_PERSPECTIVE_UNWARP_TYPE_COMBO, m_FunctionCombo);
		DDX_Control(pDX, IDC_INTERPOLATION_MODE_COMBO, m_cbInterpolation);
		//}}AFX_DATA_MAP
	}


	BOOL SVPerspectiveWarpPage::OnInitDialog() 
	{
		CPropertyPage::OnInitDialog();

		m_values.Init();

		const SvOi::NameValueVector& rWarpTypeList = m_values.GetEnumTypes(SvPb::WarpTypeEId);
		m_FunctionCombo.SetEnumTypes(rWarpTypeList);
		m_lLastWarpType = m_values.Get<long>(SvPb::WarpTypeEId);
		m_FunctionCombo.SetCurSelItemData(m_lLastWarpType);

		const SvOi::NameValueVector& rInterpolationModeList = m_values.GetEnumTypes(SvPb::OutputInterpolationModeEId);
		m_cbInterpolation.SetEnumTypes(rInterpolationModeList);
		long CurrentSelection = m_values.Get<long>(SvPb::OutputInterpolationModeEId);
		m_cbInterpolation.SetCurSelItemData(CurrentSelection);

		UpdateData(false);

		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX Property Pages should return FALSE
	}

	void SVPerspectiveWarpPage::OnSelchangePerspectiveUnwarpTypeCombo() 
	{
		SetInspectionData();
	}


	void SVPerspectiveWarpPage::OnDestroy() 
	{
		//@TODO[gra][8.00][25.01.2018]: This should be changed when it is possible to change Image extents through Inspection commands
		//Warp Type enum is declared in SVPerspectiveTool we do not want 
		enum WarpType
		{
			WarpTypeInvalid = 0,
			WarpTypeHorizontal = 1,
			WarpTypeVertical = 2,
		};

		long lType = m_values.Get<long>(SvPb::WarpTypeEId);
		if( lType != m_lLastWarpType )
		{
			WarpType eWarpType = static_cast<WarpType> (lType);
 			if (eWarpType == WarpTypeVertical)
			{
				setExtentImageProperty(SvPb::SVExtentPropertyTranslationOffsetY, 10);
				setExtentImageProperty(SvPb::SVExtentPropertyTranslationOffsetX, 0);
			}
			else if (eWarpType == WarpTypeHorizontal)
			{
				setExtentImageProperty(SvPb::SVExtentPropertyTranslationOffsetX, 10);
				setExtentImageProperty(SvPb::SVExtentPropertyTranslationOffsetY, 0);
			}
			else
			{ 
				//nothing to do
			}
		}
		CPropertyPage::OnDestroy();
	}

	bool SVPerspectiveWarpPage::setExtentImageProperty(SvPb::SVExtentPropertyEnum eProperty, double value)
	{
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_setextentparameterrequest();
		pRequest->set_objectid(m_TaskObjectID);
		pRequest->mutable_setproperty()->set_propertyflag(eProperty);
		pRequest->mutable_setproperty()->set_value(value);

		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);

		if ((S_OK != hr) || (false == responseCmd.has_setextentparameterresponse()))
		{
			return false;
		}

		return true;
	}

	void SVPerspectiveWarpPage::OnSelchangeInterpolationModeCombo() 
	{
		SetInspectionData();
	}
} //namespace SvOg
