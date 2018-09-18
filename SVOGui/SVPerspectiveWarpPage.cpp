//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#include "SVObjectLibrary/SVClsids.h"
#include "InspectionEngine/SVTool.h"
#include "ObjectInterfaces/IObjectManager.h"
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

	SVPerspectiveWarpPage::SVPerspectiveWarpPage( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, int ID ) 
	: CPropertyPage( ID )
	, m_rInspectionID{ rInspectionID }
	, m_rTaskObjectID{ rTaskObjectID }
	, m_Values{ SvOg::BoundValues{ rInspectionID, rTaskObjectID } }
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
			m_Values.Set<long>(SVWarpTypeObjectGuid, Value);
		}

		CurrentSelection = m_cbInterpolation.GetCurSel();
		if( CurrentSelection >= 0 )
		{
			long Value = static_cast<long> (m_cbInterpolation.GetItemData(CurrentSelection));
			m_Values.Set<long>(SVOutputInterpolationModeObjectGuid, Value);
		}

		m_Values.Commit();
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

		m_Values.Init();

		const SvOi::NameValueVector& rWarpTypeList = m_Values.GetEnumTypes(SVWarpTypeObjectGuid);
		m_FunctionCombo.SetEnumTypes(rWarpTypeList);
		m_lLastWarpType = m_Values.Get<long>(SVWarpTypeObjectGuid);
		m_FunctionCombo.SetCurSelItemData(m_lLastWarpType);

		const SvOi::NameValueVector& rInterpolationModeList = m_Values.GetEnumTypes(SVOutputInterpolationModeObjectGuid);
		m_cbInterpolation.SetEnumTypes(rInterpolationModeList);
		long CurrentSelection = m_Values.Get<long>(SVOutputInterpolationModeObjectGuid);
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

		long lType = m_Values.Get<long>(SVWarpTypeObjectGuid);
		if( lType != m_lLastWarpType )
		{
			SVToolClass* pTool = dynamic_cast<SVToolClass*> (SvOi::getObject(m_rInspectionID));
			if (nullptr != pTool)
			{
				SVImageExtentClass Extents;
				pTool->GetImageExtent(Extents);
				WarpType eWarpType = static_cast<WarpType> (lType);
				if (eWarpType == WarpTypeVertical)
				{
					Extents.SetExtentProperty(SvDef::SVExtentPropertyTranslationOffsetY, 10);
					Extents.SetExtentProperty(SvDef::SVExtentPropertyTranslationOffsetX, 0);
				}
				else
					if (eWarpType == WarpTypeHorizontal)
					{
						Extents.SetExtentProperty(SvDef::SVExtentPropertyTranslationOffsetX, 10);
						Extents.SetExtentProperty(SvDef::SVExtentPropertyTranslationOffsetY, 0);
					}
				pTool->SetImageExtent(Extents);
			}
		}
		CPropertyPage::OnDestroy();
	}

	void SVPerspectiveWarpPage::OnSelchangeInterpolationModeCombo() 
	{
		SetInspectionData();
	}
} //namespace SvOg
