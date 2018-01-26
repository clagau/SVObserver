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
#include "SVObjectLibrary/SVClsids.h"
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

	SVToolAdjustmentDialogLinearSpecialPage::SVToolAdjustmentDialogLinearSpecialPage( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID)
	: CPropertyPage(SVToolAdjustmentDialogLinearSpecialPage::IDD)
	, m_Values{ SvOg::BoundValues{ rInspectionID, rTaskObjectID } }
	, m_rInspectionID{ rInspectionID }
	, m_rTaskObjectID{ rTaskObjectID }
	{
	}

	SVToolAdjustmentDialogLinearSpecialPage::~SVToolAdjustmentDialogLinearSpecialPage()
	{
	}

	HRESULT SVToolAdjustmentDialogLinearSpecialPage::SetInspectionData()
	{
		HRESULT Result{ E_FAIL };

		UpdateData(true); // get data from dialog

		m_Values.Set<bool>(SVLinearToolUseRotationGuid, m_ctlUseRotation.GetCheck() ? true : false);
		int iCurSel = m_ctlProfileOrientation.GetCurSel();
		if(0 <= iCurSel)
		{
			long lValue = static_cast<long> (m_ctlProfileOrientation.GetItemData(iCurSel));
			m_Values.Set<long>(SVProfileOrientationGuid, lValue);
		}
		Result = m_Values.Commit();

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

		bool bRotate = m_Values.Get<bool>(SVLinearToolUseRotationGuid);

		const SvOi::NameValueVector& rProfileOrientationList = m_Values.GetEnumTypes(SVProfileOrientationGuid);
		m_ctlProfileOrientation.SetEnumTypes(rProfileOrientationList);

		m_ctlUseRotation.SetCheck(bRotate ? BST_CHECKED : BST_UNCHECKED);
		
		if (bRotate)
		{
			m_ctlProfileOrientation.EnableWindow(false);
			m_ctlProfileOrientation.SetCurSelItemData(0);
		}
		else
		{	
			long CurrentSelection = m_Values.Get<long>(SVProfileOrientationGuid);
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
		BOOL l_bUpdateRotation = TRUE;

		bool bIsRotated = m_Values.Get<bool>(SVLinearToolUseRotationGuid);

		BOOL l_bValue = m_ctlUseRotation.GetCheck();
		
		if ( l_bValue ) //true
		{
			CString Text;
			//turning on Rotation.
			m_ctlProfileOrientation.GetWindowText(Text);
			if ( _T("Vertical") == Text  )
			{
				SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
				INT_PTR result = Msg.setMessage( SVMSG_SVO_94_GENERAL_Informational, SvStl::Tid_LinearSpecial_OrientationVertical, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10213, GUID_NULL, MB_YESNO );
				l_bUpdateRotation = (IDYES == result);

				if( ! l_bUpdateRotation )
				{
					l_bValue = FALSE;
				}
			}
		}
		else
		{
			CString Text;
			m_ctlProfileOrientation.GetWindowText(Text);

			if ( "Horizontal" == Text )
			{
				if ( bIsRotated )
				{
					SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
					INT_PTR result = Msg.setMessage( SVMSG_SVO_94_GENERAL_Informational, SvStl::Tid_LinearSpecial_IsRotated, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10214, GUID_NULL, MB_YESNO );
					l_bUpdateRotation = (IDYES == result);
					if( ! l_bUpdateRotation )
					{
						l_bValue = TRUE;
					}
				}
			}
		}

		if( l_bUpdateRotation )
		{
			SetInspectionData();
		}

		m_ctlUseRotation.SetCheck( l_bValue );

		if (l_bValue)
		{
			m_ctlProfileOrientation.EnableWindow(false);
			m_ctlProfileOrientation.SetCurSelItemData(0);
		}
		else
		{
			m_ctlProfileOrientation.EnableWindow(true);
			long CurrentSelection = m_Values.Get<long>(SVProfileOrientationGuid);
			m_ctlProfileOrientation.SetCurSelItemData(CurrentSelection);
		}
	}
} //namespace SvOg
