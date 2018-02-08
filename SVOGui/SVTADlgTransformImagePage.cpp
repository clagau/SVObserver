//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#include "SVObjectLibrary\SVClsids.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogTransformImagePageClass, CPropertyPage)
		//{{AFX_MSG_MAP(SVToolAdjustmentDialogTransformImagePageClass)
		ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
		ON_BN_CLICKED(IDC_USE_EXTENTS_ONLY, OnUseExtentsOnly)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	SVToolAdjustmentDialogTransformImagePageClass::SVToolAdjustmentDialogTransformImagePageClass(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, int id) 
	: SVToolAdjustmentDialogImagePageClass(rInspectionID, rTaskObjectID, id)
		, m_Values{ SvOg::BoundValues{ rInspectionID, rTaskObjectID } }
	{
		//{{AFX_DATA_INIT(SVToolAdjustmentDialogTransformImagePageClass)
		m_useExtentsOnly = FALSE;
		//}}AFX_DATA_INIT
	}

	SVToolAdjustmentDialogTransformImagePageClass::~SVToolAdjustmentDialogTransformImagePageClass()
	{
	}

	HRESULT SVToolAdjustmentDialogTransformImagePageClass::SetInspectionData()
	{
		HRESULT hr = S_OK;

		UpdateData(true); // get data from dialog

		SetData();
		hr = m_Values.Commit();
		GetData();

		UpdateData(false);
	
		return hr;
	}

	void SVToolAdjustmentDialogTransformImagePageClass::GetData()
	{
		m_useExtentsOnly = m_Values.Get<bool>(SVUseExtentsOnlyObjectGuid);
	}

	void SVToolAdjustmentDialogTransformImagePageClass::SetData()
	{
		m_Values.Set<bool>(SVUseExtentsOnlyObjectGuid, m_useExtentsOnly ? true : false);
	}
		
	void SVToolAdjustmentDialogTransformImagePageClass::CheckSourceImage()
	{
		const SvUl::InputNameGuidPairList& images = GetConnectedImageList();
		if (images.size())
		{
			// Check if Main Image Type
			const SvDef::SVImageTypeEnum& rImageType = GetImageType(images.begin()->second.second.ToGUID());
			if (SvDef::SVImageTypeMain == rImageType)
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
	}

	void SVToolAdjustmentDialogTransformImagePageClass::DoDataExchange(CDataExchange* pDX)
	{
		SVToolAdjustmentDialogImagePageClass::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(SVToolAdjustmentDialogTransformImagePageClass)
		DDX_Control(pDX, IDC_USE_EXTENTS_ONLY, m_useExtentsOnlyCheckBox);
		DDX_Check(pDX, IDC_USE_EXTENTS_ONLY, m_useExtentsOnly);
		//}}AFX_DATA_MAP
	}
	BOOL SVToolAdjustmentDialogTransformImagePageClass::OnInitDialog() 
	{
	
		BOOL rc = SVToolAdjustmentDialogImagePageClass::OnInitDialog();
		if (rc)
		{
			m_Values.Init();
			GetData();
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

	void SVToolAdjustmentDialogTransformImagePageClass::OnSelchangeCombo1() 
	{
		SVToolAdjustmentDialogImagePageClass::OnSelchangeCombo1();
		CheckSourceImage();
	}

	void SVToolAdjustmentDialogTransformImagePageClass::OnUseExtentsOnly() 
	{
		SetInspectionData();
	}
} //namespace SvOg

