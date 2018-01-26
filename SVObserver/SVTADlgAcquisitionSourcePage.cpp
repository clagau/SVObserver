//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgAcquisitionSourcePage
//* .File Name       : $Workfile:   SVTADlgAcquisitionSourcePage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   21 May 2014 12:24:36  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVTADlgAcquisitionSourcePage.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVToolSet.h"
#include "InspectionEngine/SVTool.h"
#include "SVVirtualCamera.h"
#include "SVToolAcquisition.h"
#include "SVAcquisitionClass.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVIPDoc.h"
#include "SVInspectionProcess.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogAcquisitionSourcePageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVToolAdjustmentDialogAcquisitionSourcePageClass)
	ON_CBN_SELCHANGE(IDC_COMBO_CAMERAS, OnSelchangeCameraCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVToolAdjustmentDialogAcquisitionSourcePageClass::SVToolAdjustmentDialogAcquisitionSourcePageClass( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* pSheet ) 
: CPropertyPage(SVToolAdjustmentDialogAcquisitionSourcePageClass::IDD)
, m_pSheet(pSheet)
{
}

SVToolAdjustmentDialogAcquisitionSourcePageClass::~SVToolAdjustmentDialogAcquisitionSourcePageClass()
{
}

void SVToolAdjustmentDialogAcquisitionSourcePageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVToolAdjustmentDialogAcquisitionSourcePageClass)
	DDX_Control(pDX, IDC_COMBO_CAMERAS, m_CameraListBox);
	//}}AFX_DATA_MAP
}


BOOL SVToolAdjustmentDialogAcquisitionSourcePageClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	UpdateData( FALSE );

	if( m_pSheet && ( m_pTool = m_pSheet->GetTool() ) && SV_IS_KIND_OF( m_pTool, SVAcquisitionToolClass ) )
	{
		// Try to get main image of the current acquisition tool...
		SvDef::SVObjectTypeInfoStruct info;
		info.ObjectType = SvDef::SVImageObjectType;
		m_pMainImage = dynamic_cast<SVCameraImageTemplate*>(m_pTool->getFirstObject(info));
		if( nullptr != m_pMainImage)
		{
			SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*> ( m_pTool->GetAncestor(SvDef::SVInspectionObjectType));

			if( nullptr != pInspection )
			{
				SvUl::NameGuidList CameraGuidList;
				SVVirtualCameraPtrSet CameraList;

				pInspection->GetPPQCameras( CameraList );

				for (auto const& pEntry : CameraList )
				{
					if( nullptr != pEntry && nullptr != pEntry->mpsvDevice )
					{
						CameraGuidList.push_back(SvUl::NameGuidPair(pEntry->GetCompleteName(), pEntry->GetUniqueObjectID()));
					}
				}

				m_CameraListBox.Init( CameraGuidList, m_pMainImage->GetCamera()->GetCompleteName(), std::string());
			}

			// Success...
			return TRUE;  // return TRUE unless you set the focus to a control
						  // EXCEPTION: OCX Property Pages should return FALSE
		}
	}

	// Not valid call...try to close!
	if (GetParent())
	{
		GetParent()->SendMessage(WM_CLOSE);
	}
	else
	{
		SendMessage(WM_CLOSE);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVToolAdjustmentDialogAcquisitionSourcePageClass::OnSelchangeCameraCombo()
{
	SVGUID SelectedCameraGuid = m_CameraListBox.getSelectedGUID();
	if( nullptr != m_pTool && nullptr != m_pMainImage && GUID_NULL != SelectedCameraGuid )
	{
		// Set new digitizer of main image...
		m_pMainImage->UpdateCameraImage(SelectedCameraGuid);

		// Reset all objects...
		m_pTool->resetAllObjects();
	}
}


