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
#include "SVInspectionProcess.h"
#include "SVPPQObject.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "InspectionEngine/SVVirtualCamera.h"
#include "SVObjectLibrary/SVObjectClass.h"
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

SVToolAdjustmentDialogAcquisitionSourcePageClass::SVToolAdjustmentDialogAcquisitionSourcePageClass(uint32_t inspectionID, uint32_t taskObjectID, SVToolAdjustmentDialogSheetClass* pSheet )
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

	if( m_pSheet && ( m_pTaskObject = m_pSheet->GetTaskObject() ) && SvPb::SVToolAcquisitionObjectType == m_pTaskObject->GetObjectSubType())
	{
		// Try to get main image of the current acquisition tool...
		SvDef::SVObjectTypeInfoStruct info;
		info.m_ObjectType = SvPb::SVImageObjectType;
		m_pMainImage = dynamic_cast<SvIe::SVCameraImageTemplate*> (m_pTaskObject->getFirstObject(info));
		if( nullptr != m_pMainImage)
		{
			SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*> ( m_pTaskObject->GetAncestorInterface(SvPb::SVInspectionObjectType));

			if( nullptr != pInspection && nullptr != pInspection->GetPPQ())
			{
				SvUl::NameObjectIdList CameraObjectIDList;
				SvIe::SVVirtualCameraPtrVector cameraVector = pInspection->GetPPQ()->GetVirtualCameras(true);

				for (auto const* pCamera : cameraVector)
				{
					if( nullptr != pCamera && nullptr != pCamera->GetAcquisitionDevice())
					{
						CameraObjectIDList.push_back(SvUl::NameObjectIdPair(pCamera->GetCompleteName(), pCamera->getObjectId()));
					}
				}

				m_CameraListBox.Init( CameraObjectIDList, m_pMainImage->GetCamera()->GetCompleteName(), std::string());
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
	uint32_t SelectedCameraId = m_CameraListBox.getSelectedValue();
	if (nullptr != m_pTaskObject && nullptr != m_pMainImage && SvDef::InvalidObjectId != SelectedCameraId)
	{
		// Set new digitizer of main image...
		m_pMainImage->UpdateCameraImage(SelectedCameraId);

		// Reset all objects...
		m_pTaskObject->resetAllObjects();
	}
}


