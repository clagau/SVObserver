//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : TADlgTransformImagePage
//* .File Name       : $Workfile:   TADlgTransformImagePage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 13:07:14  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TADlgTransformImagePage.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	BEGIN_MESSAGE_MAP(TADlgTransformImagePage, CPropertyPage)
		//{{AFX_MSG_MAP(TADlgTransformImagePage)
		ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
		ON_BN_CLICKED(IDC_USE_EXTENTS_ONLY, OnUseExtentsOnly)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	TADlgTransformImagePage::TADlgTransformImagePage(uint32_t inspectionId, uint32_t toolId, uint32_t taskObjectId, SvPb::SVObjectSubTypeEnum SubType /*= SvDef::SVImageMonoType*/, int id /*= IDD*/)
	: TADlgImagePageClass(inspectionId, toolId, SubType, id)
		, m_values{ SvOgu::BoundValues{ inspectionId, taskObjectId } }
	{
		//{{AFX_DATA_INIT(TADlgTransformImagePage)
		m_useExtentsOnly = FALSE;
		//}}AFX_DATA_INIT
	}

	TADlgTransformImagePage::~TADlgTransformImagePage()
	{
	}

	HRESULT TADlgTransformImagePage::SetInspectionData()
	{
		HRESULT hr = S_OK;

		UpdateData(true); // get data from dialog

		m_values.Set<bool>(SvPb::UseExtentsOnlyEId, m_useExtentsOnly ? true : false);
		hr = m_values.Commit();

		UpdateData(false);
	
		return hr;
	}

	void TADlgTransformImagePage::CheckSourceImage()
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

	void TADlgTransformImagePage::DoDataExchange(CDataExchange* pDX)
	{
		TADlgImagePageClass::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(TADlgTransformImagePage)
		DDX_Control(pDX, IDC_USE_EXTENTS_ONLY, m_useExtentsOnlyCheckBox);
		DDX_Check(pDX, IDC_USE_EXTENTS_ONLY, m_useExtentsOnly);
		//}}AFX_DATA_MAP
	}
	BOOL TADlgTransformImagePage::OnInitDialog() 
	{
	
		BOOL rc = TADlgImagePageClass::OnInitDialog();
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

	void TADlgTransformImagePage::OnSelchangeCombo1() 
	{
		TADlgImagePageClass::OnSelchangeCombo1();
		CheckSourceImage();
	}

	void TADlgTransformImagePage::OnUseExtentsOnly() 
	{
		SetInspectionData();
	}
} //namespace SvOg

