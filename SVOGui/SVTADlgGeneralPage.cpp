//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgGeneralPage
//* .File Name       : $Workfile:   SVTADlgGeneralPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 12:50:40  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <boost/assign/list_of.hpp>
#include "SVTADlgGeneralPage.h"
#include "SVObjectLibrary\SVClsids.h"
#include "ObjectInterfaces\NameValueVector.h"
#include "SVObjectLibrary\SVObjectSynchronousCommandTemplate.h"
#include "SVShowDependentsDialog.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	SVToolAdjustmentDialogGeneralPageClass::SVToolAdjustmentDialogGeneralPageClass(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID) 
	: CPropertyPage(SVToolAdjustmentDialogGeneralPageClass::IDD)
	, m_bIsImageTool(false)
	, m_bAuxExtentsAvailable(false)
	, m_InspectionID(rInspectionID)
	, m_TaskObjectID(rTaskObjectID)
	, m_Values(SvOg::BoundValues(rInspectionID, rTaskObjectID))
	, m_AuxExtentsController(rInspectionID, rTaskObjectID)
	{
		//{{AFX_DATA_INIT(SVToolAdjustmentDialogGeneralPageClass)
		m_bUpdateAuxiliaryExtents = false;
		//}}AFX_DATA_INIT
	}

	SVToolAdjustmentDialogGeneralPageClass::~SVToolAdjustmentDialogGeneralPageClass()
	{
	}

	HRESULT SVToolAdjustmentDialogGeneralPageClass::SetInspectionData()
	{
		HRESULT hr = S_OK;

		UpdateData(true); // get data from dialog

		int CurrentSelection = m_drawToolCombo.GetCurSel();
		if (CB_ERR != CurrentSelection)
		{
			long Value = static_cast<long> (m_drawToolCombo.GetItemData(CurrentSelection));
			m_Values.Set<long>(SVConditionalToolDrawFlagObjectGuid, Value);
		}			
		m_AuxExtentsController.EnableAuxExtents(m_bUpdateAuxiliaryExtents ? true : false);
			
		hr = m_Values.Commit();
		if (S_OK == hr)
		{
			hr = m_AuxExtentsController.Commit();
		}
		UpdateData(false);

		return hr;
	}

	void SVToolAdjustmentDialogGeneralPageClass::refresh()
	{
		// Update dialog with freeze tool attributes...
		long CurrentSelection = m_Values.Get<long>(SVConditionalToolDrawFlagObjectGuid);
		m_drawToolCombo.SetCurSelItemData(CurrentSelection);
	
		// Check, if drawToolCombo must be disabled/enabled...
		// based on What?
		CWnd* pWnd = GetDlgItem(IDC_DRAW_TOOL_COMBO);
		if (pWnd)
		{
			pWnd->EnableWindow(true);
		}
		if (m_bIsImageTool)
		{
			GetDlgItem(IDC_SOURCE_IMAGE_COMBO)->EnableWindow(m_bUpdateAuxiliaryExtents);
		}
		UpdateData(false); // Send Data to Dialog...
	}

	void SVToolAdjustmentDialogGeneralPageClass::SetupAuxExtents()
	{
		m_bUpdateAuxiliaryExtents = m_AuxExtentsController.IsUpdateAuxExtentsEnabled();
		UpdateData(false);
	}

	void SVToolAdjustmentDialogGeneralPageClass::SetupDrawFlagComboBox()
	{
		const SvOi::NameValueVector& rDrawCriteria = m_Values.GetEnumTypes(SVConditionalToolDrawFlagObjectGuid);
		m_drawToolCombo.SetEnumTypes(rDrawCriteria);
		long CurrentSelection = m_Values.Get<long>(SVConditionalToolDrawFlagObjectGuid);
		m_drawToolCombo.SetCurSelItemData(CurrentSelection);
	}

	void SVToolAdjustmentDialogGeneralPageClass::SetImages()
	{
		m_AvailableSourceImageCombo.ResetContent();
		if (m_bAuxExtentsAvailable)
		{
			const SvUl::NameGuidList& availImages = m_AuxExtentsController.GetAvailableImageList();
			for (SvUl::NameGuidList::const_iterator it = availImages.begin();it != availImages.end();++it)
			{
				m_AvailableSourceImageCombo.AddString(it->first.c_str());
			}
			CString name = m_AuxExtentsController.GetAuxSourceImageName().c_str();
			if (!name.IsEmpty())
			{
				m_AvailableSourceImageCombo.SelectString(-1, name);
			}
		}
	}

	bool SVToolAdjustmentDialogGeneralPageClass::CheckAuxiliaryExtentsAvailable() const
	{
		bool bRetVal = m_AuxExtentsController.AreAuxiliaryExtentsAvailable();
		return bRetVal;
	}

	void SVToolAdjustmentDialogGeneralPageClass::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(SVToolAdjustmentDialogGeneralPageClass)
		DDX_Control(pDX, IDC_SOURCE_IMAGE_COMBO, m_AvailableSourceImageCombo);
		DDX_Control(pDX, IDC_DRAW_TOOL_COMBO, m_drawToolCombo);
		DDX_Check(pDX, IDC_ENABLE_AUXILIARY_EXTENTS, m_bUpdateAuxiliaryExtents);
		//}}AFX_DATA_MAP
	}

	BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogGeneralPageClass, CPropertyPage)
		//{{AFX_MSG_MAP(SVToolAdjustmentDialogGeneralPageClass)
		ON_CBN_SELCHANGE(IDC_DRAW_TOOL_COMBO, OnSelchangeDrawToolCombo)
		ON_BN_CLICKED(IDC_ENABLE_AUXILIARY_EXTENTS, OnUpdateAuxiliaryExtents)
		ON_CBN_SELCHANGE(IDC_SOURCE_IMAGE_COMBO, OnSelchangeSourceImageCombo)
		ON_BN_CLICKED(ID_SHOW_RELATIONS, OnShowRelations)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()


	/////////////////////////////////////////////////////////////////////////////
	// SVToolAdjustmentDialogGeneralPageClass message handlers

	BOOL SVToolAdjustmentDialogGeneralPageClass::OnInitDialog() 
	{
		CPropertyPage::OnInitDialog();

		m_AuxExtentsController.Init();
		m_Values.Init();

		SetupDrawFlagComboBox();
		m_bAuxExtentsAvailable = CheckAuxiliaryExtentsAvailable();

		// Get tool's Update Auxiliary Extents...
		if (!m_bAuxExtentsAvailable)
		{
			// Disable Auxiliary Extents
			GetDlgItem(IDC_ENABLE_AUXILIARY_EXTENTS)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_AUXILIARY_GROUP)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_SOURCE_IMAGE_TEXT)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_SOURCE_IMAGE_COMBO)->ShowWindow(SW_HIDE);
			m_bIsImageTool = false;
		}
		else
		{
			// Enable Auxiliary Extents
			GetDlgItem(IDC_ENABLE_AUXILIARY_EXTENTS)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_AUXILIARY_GROUP)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_SOURCE_IMAGE_TEXT)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_SOURCE_IMAGE_COMBO)->ShowWindow(SW_SHOW);
			m_bIsImageTool = true;
		
			SetupAuxExtents();

			GetDlgItem(IDC_SOURCE_IMAGE_COMBO)->EnableWindow(m_bUpdateAuxiliaryExtents);

			SetImages();
		}
		UpdateData(false);
		refresh();
		// Success...
		return true;  // return TRUE unless you set the focus to a control
						  // EXCEPTION: OCX Property Pages should return FALSE
	}

	void SVToolAdjustmentDialogGeneralPageClass::OnSelchangeDrawToolCombo() 
	{
		SetInspectionData();
		refresh();
	}

	void SVToolAdjustmentDialogGeneralPageClass::OnUpdateAuxiliaryExtents() 
	{
		SetInspectionData();

		const SvUl::NameGuidPair& source = m_AuxExtentsController.GetAuxSourceImage();
		if (!source.first.empty())
		{
			m_AuxExtentsController.SetAuxSourceImage(source.first);
		}

		GetDlgItem(IDC_SOURCE_IMAGE_COMBO)->EnableWindow(m_bUpdateAuxiliaryExtents);
		refresh();
	}

	void SVToolAdjustmentDialogGeneralPageClass::OnSelchangeSourceImageCombo() 
	{
		UpdateData( TRUE ); // get data from dialog

		int index = static_cast<int>(m_AvailableSourceImageCombo.GetCurSel());
		if (CB_ERR != index)
		{
			CString name;
			m_AvailableSourceImageCombo.GetLBText(index, name);
			if (!name.IsEmpty())
			{
				m_AuxExtentsController.SetAuxSourceImage( std::string(name.GetString() ));
			}
		}
		refresh();
	}
	
	void SVToolAdjustmentDialogGeneralPageClass::OnShowRelations() 
	{
		SVGuidSet DependencySet;
		DependencySet.insert(m_TaskObjectID);
		SVShowDependentsDialog Dlg( DependencySet, SvDef::SVToolObjectType, nullptr, SVShowDependentsDialog::Show );
		Dlg.DoModal();
	}

	BOOL SVToolAdjustmentDialogGeneralPageClass::OnSetActive() 
	{
		SetImages();
		refresh();

		return CPropertyPage::OnSetActive();
	}
} //namespace SvOg

