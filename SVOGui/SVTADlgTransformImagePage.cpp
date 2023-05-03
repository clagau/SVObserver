//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgTransformImagePage
//* .File Name       : $Workfile:   SVTADlgTransformImagePage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 13:07:14  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVTADlgTransformImagePage.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	BEGIN_MESSAGE_MAP(SVTADlgTransformImagePage, CPropertyPage)
		//{{AFX_MSG_MAP(SVTADlgTransformImagePage)
		ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
		ON_BN_CLICKED(IDC_USE_EXTENTS_ONLY, OnUseExtentsOnly)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	SVTADlgTransformImagePage::SVTADlgTransformImagePage(uint32_t inspectionId, uint32_t toolId, uint32_t taskObjectId, SvPb::SVObjectSubTypeEnum SubType /*= SvDef::SVImageMonoType*/, int id /*= IDD*/)
	: SVToolAdjustmentDialogImagePageClass(inspectionId, toolId, SubType, id)
		, m_values{ SvOgu::BoundValues{ inspectionId, taskObjectId } }
	{
		//{{AFX_DATA_INIT(SVTADlgTransformImagePage)
		m_useExtentsOnly = FALSE;
		//}}AFX_DATA_INIT
	}

	SVTADlgTransformImagePage::~SVTADlgTransformImagePage()
	{
	}

	HRESULT SVTADlgTransformImagePage::SetInspectionData()
	{
		HRESULT hr = S_OK;

		UpdateData(true); // get data from dialog

		m_values.Set<bool>(SvPb::UseExtentsOnlyEId, m_useExtentsOnly ? true : false);
		hr = m_values.Commit();

		UpdateData(false);
	
		return hr;
	}

	void SVTADlgTransformImagePage::CheckSourceImage()
	{
		auto imageData = getImageController().GetInputData(SvPb::ImageInputEId);
		// Check if Main Image Type
		const SvPb::SVImageTypeEnum& rImageType = getImageController().GetImageType(imageData.connected_objectid());
		if (SvPb::SVImageTypeMain == rImageType)
		{
			m_useExtentsOnly = false;
			// use Extents Only does not Apply
			m_useExtentsOnlyCheckBox.EnableWindow(false);
		}
		else
		{
			m_useExtentsOnlyCheckBox.EnableWindow(true);
		}
	}

	void SVTADlgTransformImagePage::DoDataExchange(CDataExchange* pDX)
	{
		SVToolAdjustmentDialogImagePageClass::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(SVTADlgTransformImagePage)
		DDX_Control(pDX, IDC_USE_EXTENTS_ONLY, m_useExtentsOnlyCheckBox);
		DDX_Check(pDX, IDC_USE_EXTENTS_ONLY, m_useExtentsOnly);
		//}}AFX_DATA_MAP
	}
	BOOL SVTADlgTransformImagePage::OnInitDialog() 
	{
	
		BOOL rc = SVToolAdjustmentDialogImagePageClass::OnInitDialog();
		if (rc)
		{
			m_values.Init();
			m_useExtentsOnly = m_values.Get<bool>(SvPb::UseExtentsOnlyEId);
			CheckSourceImage();
			UpdateData(false);
		}
		else
		{
			// Not valid call...
			if (GetParent())
			{
				GetParent()->SendMessage(WM_CLOSE);
			}
			else
			{
				SendMessage(WM_CLOSE);
			}
		}
		return true;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
	}

	void SVTADlgTransformImagePage::OnSelchangeCombo1() 
	{
		SVToolAdjustmentDialogImagePageClass::OnSelchangeCombo1();
		CheckSourceImage();
	}

	void SVTADlgTransformImagePage::OnUseExtentsOnly() 
	{
		SetInspectionData();
	}
} //namespace SvOg

