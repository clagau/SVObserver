//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogLinearSpecialPageClass
//* .File Name       : $Workfile:   SVToolAdjustmentDialogLinearSpecialPageClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:36:38  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVToolAdjustmentDialogLinearSpecialPage.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogLinearSpecialPage, CPropertyPage)
		ON_CBN_SELCHANGE(IDC_COMBO_PROFILE_ORIENTATION, OnSelchangeComboProfileOrientation)
		ON_BN_CLICKED(IDC_CHECK_ROTATION, OnCheckRotation)
	END_MESSAGE_MAP()

	constexpr long cRotationAngle_0	= 0;
	constexpr long cRotationAngle_90 = 90;

	SVToolAdjustmentDialogLinearSpecialPage::SVToolAdjustmentDialogLinearSpecialPage( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID)
	: CPropertyPage(SVToolAdjustmentDialogLinearSpecialPage::IDD)
	, m_Values{ SvOg::BoundValues{ rInspectionID, rTaskObjectID } }
	, m_InspectionID{ rInspectionID }
	, m_TaskObjectID{ rTaskObjectID }
	{
	}

	SVToolAdjustmentDialogLinearSpecialPage::~SVToolAdjustmentDialogLinearSpecialPage()
	{
	}

	HRESULT SVToolAdjustmentDialogLinearSpecialPage::SetInspectionData()
	{
		HRESULT Result{ E_FAIL };

		UpdateData(true); // get data from dialog

		m_Values.Set<bool>(SvPb::LinearToolUseRotationEId, m_ctlUseRotation.GetCheck() ? true : false);
		int iCurSel = m_ctlProfileOrientation.GetCurSel();
		if(0 <= iCurSel)
		{
			long lValue = static_cast<long> (m_ctlProfileOrientation.GetItemData(iCurSel));
			m_Values.Set<long>(SvPb::ProfileOrientationEId, lValue);
		}
		Result = m_Values.Commit(SvOg::PostAction::doReset | SvOg::PostAction::doRunOnce);

		return Result;
	}

	void SVToolAdjustmentDialogLinearSpecialPage::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		DDX_Control(pDX, IDC_CHECK_ROTATION, m_ctlUseRotation);
		DDX_Control(pDX, IDC_COMBO_PROFILE_ORIENTATION, m_ctlProfileOrientation);
	}

	BOOL SVToolAdjustmentDialogLinearSpecialPage::OnInitDialog() 
	{
		CPropertyPage::OnInitDialog();

		m_Values.Init();

		bool bRotate = m_Values.Get<bool>(SvPb::LinearToolUseRotationEId);

		const SvOi::NameValueVector& rProfileOrientationList = m_Values.GetEnumTypes(SvPb::ProfileOrientationEId);
		m_ctlProfileOrientation.SetEnumTypes(rProfileOrientationList);

		m_ctlUseRotation.SetCheck(bRotate ? BST_CHECKED : BST_UNCHECKED);
		
		if (bRotate)
		{
			m_ctlProfileOrientation.EnableWindow(false);
			m_ctlProfileOrientation.SetCurSelItemData(0);
		}
		else
		{	
			long CurrentSelection = m_Values.Get<long>(SvPb::ProfileOrientationEId);
			m_ctlProfileOrientation.SetCurSelItemData(CurrentSelection);
		}
	
		UpdateData(false);
	
		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX Property Pages should return FALSE
	}

	void SVToolAdjustmentDialogLinearSpecialPage::OnSelchangeComboProfileOrientation() 
	{
		SetInspectionData();
	}

	void SVToolAdjustmentDialogLinearSpecialPage::OnCheckRotation() 
	{
		BOOL bUpdateRotation = TRUE;

		double dRotationAngle {0.0};
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_getextentparameterrequest();
		SvPb::SetGuidInProtoBytes(pRequest->mutable_objectid(), m_TaskObjectID);

		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
		if (S_OK == hr && responseCmd.has_getextentparameterresponse())
		{
			auto extentParameter = responseCmd.getextentparameterresponse().parameters();
			auto valuePair = std::find_if(extentParameter.begin(), extentParameter.end(), [](const auto value) { return value.type() == SvPb::SVExtentPropertyRotationAngle; });
			if (extentParameter.end() != valuePair)
			{
				dRotationAngle = valuePair->value();
			}
		}
		bool bIsRotated = (0.0 != dRotationAngle) ? true : false;

		BOOL bValue = m_ctlUseRotation.GetCheck();

		long Angle = static_cast<long> (m_ctlProfileOrientation.GetCurSelItemData());

		if (bValue)
		{
			if (cRotationAngle_90 == Angle)
			{
				SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
				INT_PTR result = Msg.setMessage( SVMSG_SVO_94_GENERAL_Informational, SvStl::Tid_LinearSpecial_OrientationVertical, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10213, GUID_NULL, MB_YESNO );
				bUpdateRotation = (IDYES == result);

				if(!bUpdateRotation)
				{
					bValue = FALSE;
				}
			}
		}
		else
		{
			if (cRotationAngle_0 == Angle)
			{
				if (bIsRotated)
				{
					SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
					INT_PTR result = Msg.setMessage( SVMSG_SVO_94_GENERAL_Informational, SvStl::Tid_LinearSpecial_IsRotated, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10214, GUID_NULL, MB_YESNO );
					bUpdateRotation = (IDYES == result);
					if(!bUpdateRotation)
					{
						bValue = TRUE;
					}
				}
			}
		}

		if( bUpdateRotation )
		{
			SetInspectionData();
		}

		m_ctlUseRotation.SetCheck( bValue );

		if (bValue)
		{
			m_ctlProfileOrientation.EnableWindow(false);
			m_ctlProfileOrientation.SetCurSelItemData(0);
		}
		else
		{
			m_ctlProfileOrientation.EnableWindow(true);
			long CurrentSelection = m_Values.Get<long>(SvPb::ProfileOrientationEId);
			m_ctlProfileOrientation.SetCurSelItemData(CurrentSelection);
		}
	}
} //namespace SvOg
