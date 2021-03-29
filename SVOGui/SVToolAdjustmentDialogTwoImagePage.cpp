//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogTwoImagePage
//* .File Name       : $Workfile:   SVToolAdjustmentDialogTwoImagePage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   26 Jun 2014 18:29:28  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVToolAdjustmentDialogTwoImagePage.h"
#include "SVMatroxLibrary\SVMatroxSimpleEnums.h"
#include "Definitions/ImageOperatorEnums.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	constexpr const char* FirstImageTag = _T("First Image");
	constexpr const char* SecondImageTag = _T("Second Image");
	constexpr const char* ResultImageTag = _T("Result Image");
	constexpr const char* NoImageTag = _T("(No Image Available)"); // maybe move this to the control class?
	static const int NumberOfImagesRequired = 2;

	enum ImageTabsEnum
	{
		FirstImageTab = 0,
		SecondImageTab = 1,
		ResultImageTab = 2
	};

	BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogTwoImagePageClass, CPropertyPage)
		//{{AFX_MSG_MAP(SVToolAdjustmentDialogTwoImagePageClass)
		ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
		ON_CBN_SELCHANGE(IDC_COMBO2, OnSelChangeCombo2)
		ON_CBN_SELCHANGE(IDC_OPERATOR_COMBO, OnSelchangeOperatorCombo)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	#pragma region Constructor
	SVToolAdjustmentDialogTwoImagePageClass::SVToolAdjustmentDialogTwoImagePageClass(uint32_t inspectionId, uint32_t taskObjectId) 
	: CPropertyPage(SVToolAdjustmentDialogTwoImagePageClass::IDD)
	, SvOg::ImageController(inspectionId, taskObjectId, SvPb::SVImageMonoType, false)
	, m_InspectionID(inspectionId)
	, m_TaskObjectID(taskObjectId)
	, m_values{ SvOg::BoundValues{ inspectionId, taskObjectId } }
	{
	}

	SVToolAdjustmentDialogTwoImagePageClass::~SVToolAdjustmentDialogTwoImagePageClass()
	{
	}
	#pragma endregion Constructor

	HRESULT SVToolAdjustmentDialogTwoImagePageClass::SetInspectionData()
	{
		HRESULT hr = S_OK;

		UpdateData(true); // get data from dialog

		long lOperator = static_cast<long>(m_operatorCtrl.GetItemData(m_operatorCtrl.GetCurSel()));
		m_values.Set<long>(SvPb::ArithmeticOperatorEId, lOperator);
		m_values.Commit();
		
		if (SvDef::SVImageOperatorDoubleHeight == lOperator || SvDef::SVImageOperatorFlipVertical == lOperator || SvDef::SVImageOperatorFlipHorizontal == lOperator)
		{
			m_secondAvailableSourceImageListBoxCtl.EnableWindow(false);
			//hide second image tab, because in this mode is only on image aloud.
			m_firstImageCtrl.ShowTab(SecondImageTab, false);
			m_secondImageCtrl.ShowTab(SecondImageTab, false);
		}// end if
		else
		{
			m_secondAvailableSourceImageListBoxCtl.EnableWindow(true);
			//show second image tab
			m_firstImageCtrl.ShowTab(SecondImageTab, true);
			m_secondImageCtrl.ShowTab(SecondImageTab, true);
		}// end else

		UpdateData(false);
		return hr;
	}

	void SVToolAdjustmentDialogTwoImagePageClass::refresh()
	{
		SetInspectionData();
		setImages();
	}

	#pragma region Protected Methods
	void SVToolAdjustmentDialogTwoImagePageClass::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(SVToolAdjustmentDialogTwoImagePageClass)
		DDX_Control(pDX, IDC_OPERATOR_COMBO, m_operatorCtrl);
		DDX_Control(pDX, IDC_SECOND_IMAGE, m_secondImageCtrl);
		DDX_Control(pDX, IDC_FIRST_IMAGE, m_firstImageCtrl);
		DDX_Control(pDX, IDC_COMBO1, m_firstAvailableSourceImageListBoxCtl);
		DDX_Control(pDX, IDC_COMBO2, m_secondAvailableSourceImageListBoxCtl);
		//}}AFX_DATA_MAP
	}

	BOOL SVToolAdjustmentDialogTwoImagePageClass::OnInitDialog() 
	{
		CPropertyPage::OnInitDialog();
	
		Init();
		m_values.Init();

		const SvUl::NameObjectIdList& rAvailableImageList = GetAvailableImageList();
		RetreiveCurrentlySelectedImageNames();
		RetreiveResultImageNames();
	
		m_firstAvailableSourceImageListBoxCtl.Init(rAvailableImageList, m_firstImageName, NoImageTag);
		m_secondAvailableSourceImageListBoxCtl.Init(rAvailableImageList, m_secondImageName, NoImageTag);

		m_firstImageCtrl.AddTab(FirstImageTag); 
		m_firstImageCtrl.AddTab(SecondImageTag); 
		m_firstImageCtrl.AddTab(ResultImageTag); 
		m_secondImageCtrl.AddTab(FirstImageTag); 
		m_secondImageCtrl.AddTab(SecondImageTag); 
		m_secondImageCtrl.AddTab(ResultImageTag); 
		m_secondImageCtrl.SelectTab(1);
		setImages();
			
		// Fill Arithmetic Combo...
		m_operatorCtrl.SetItemData( m_operatorCtrl.AddString( _T( "AND" ) ), static_cast<DWORD_PTR>(SVImageAnd) );
		m_operatorCtrl.SetItemData( m_operatorCtrl.AddString( _T( "OR" ) ),  static_cast<DWORD_PTR>(SVImageOr) );	
		m_operatorCtrl.SetItemData( m_operatorCtrl.AddString( _T( "XOR" ) ), static_cast<DWORD_PTR>(SVImageXOr) );	
		m_operatorCtrl.SetItemData( m_operatorCtrl.AddString( _T( "NOR" ) ), static_cast<DWORD_PTR>(SVImageNor) );	
		m_operatorCtrl.SetItemData( m_operatorCtrl.AddString( _T( "XNOR" ) ), static_cast<DWORD_PTR>(SVImageXNor) );	
		m_operatorCtrl.SetItemData( m_operatorCtrl.AddString( _T( "NAND" ) ), static_cast<DWORD_PTR>(SVImageNand) );	
		m_operatorCtrl.SetItemData( m_operatorCtrl.AddString( _T( "ADD" ) ), static_cast<DWORD_PTR>(SVImageAddSaturation) );	
		m_operatorCtrl.SetItemData( m_operatorCtrl.AddString( _T( "SUB" ) ), static_cast<DWORD_PTR>(SVImageSubSaturation) );	
		m_operatorCtrl.SetItemData( m_operatorCtrl.AddString( _T( "MULT" ) ), static_cast<DWORD_PTR>(SVImageMulSaturation) );
		m_operatorCtrl.SetItemData( m_operatorCtrl.AddString( _T( "DIV" ) ), static_cast<DWORD_PTR>(SVImageDiv) );
		m_operatorCtrl.SetItemData( m_operatorCtrl.AddString( _T( "MIN" ) ), static_cast<DWORD_PTR>(SVImageMin) );	
		m_operatorCtrl.SetItemData( m_operatorCtrl.AddString( _T( "MAX" ) ), static_cast<DWORD_PTR>(SVImageMax) );	
		m_operatorCtrl.SetItemData( m_operatorCtrl.AddString( _T( "PASS" ) ), static_cast<DWORD_PTR>(SVImagePass) );	
			
		// Special Image Operator... ( not defined by MIL ! )
		m_operatorCtrl.SetItemData( m_operatorCtrl.AddString( _T( "AVERAGE" ) ), static_cast<DWORD_PTR>(SvDef::SVImageOperatorAverage) );	
		m_operatorCtrl.SetItemData( m_operatorCtrl.AddString( _T( "DOUBLE HEIGHT" ) ), static_cast<DWORD_PTR>(SvDef::SVImageOperatorDoubleHeight) );	
		m_operatorCtrl.SetItemData( m_operatorCtrl.AddString( _T( "FLIP VERTICAL" ) ), static_cast<DWORD_PTR>(SvDef::SVImageOperatorFlipVertical) );	
		m_operatorCtrl.SetItemData( m_operatorCtrl.AddString( _T( "FLIP HORIZONTAL" ) ), static_cast<DWORD_PTR>(SvDef::SVImageOperatorFlipHorizontal) );

		// Get Current Arithmetic Operator...
	
		long lOperator = m_values.Get<long>(SvPb::ArithmeticOperatorEId);

		for (int i = 0;i < m_operatorCtrl.GetCount(); ++ i)
		{
			if (lOperator == static_cast<long>(m_operatorCtrl.GetItemData(i)))
			{
				m_operatorCtrl.SetCurSel(i); // Set Selected Operator...
				break;
			}
		}
		UpdateData(false); // set data to dialog
		return true;
	}

	void SVToolAdjustmentDialogTwoImagePageClass::OnSelchangeCombo1()
	{
		UpdateData(true); // get data from dialog

		CString Name;
		m_firstAvailableSourceImageListBoxCtl.GetLBText(m_firstAvailableSourceImageListBoxCtl.GetCurSel(), Name);
		if (!Name.IsEmpty() && Name != NoImageTag)
		{
			m_firstImageName = Name;
			ConnectToImage(m_firstInputName, m_firstImageName);
			refresh();
		}
	}

	void SVToolAdjustmentDialogTwoImagePageClass::OnSelChangeCombo2()
	{
		UpdateData(true); // get data from dialog

		CString Name;
		m_secondAvailableSourceImageListBoxCtl.GetLBText(m_secondAvailableSourceImageListBoxCtl.GetCurSel(), Name);
		if (!Name.IsEmpty() && Name != NoImageTag)
		{
			m_secondImageName = Name;
			ConnectToImage(m_secondInputName, m_secondImageName);
			refresh();
		}
	}

	void SVToolAdjustmentDialogTwoImagePageClass::OnSelchangeOperatorCombo() 
	{
		refresh();
	}
	#pragma endregion Protected Methods

	#pragma region Private Methods
	void SVToolAdjustmentDialogTwoImagePageClass::setImages()
	{
		RetreiveCurrentlySelectedImageNames();

		IPictureDisp* pFirstImage = GetImage(m_firstImageName);
		IPictureDisp* pSecondImage = GetImage(m_secondImageName);
		m_firstImageCtrl.setImage(pFirstImage, FirstImageTab);
		m_firstImageCtrl.setImage(pSecondImage, SecondImageTab);
		m_secondImageCtrl.setImage(pFirstImage, FirstImageTab);
		m_secondImageCtrl.setImage(pSecondImage, SecondImageTab);
	
		IPictureDisp* pResultImage = GetImage(m_resultImageID);
		if (pResultImage)
		{
			m_firstImageCtrl.setImage(pResultImage, ResultImageTab);
			m_secondImageCtrl.setImage(pResultImage, ResultImageTab);
			//display result image tab
			m_firstImageCtrl.ShowTab(ResultImageTab, true);
			m_secondImageCtrl.ShowTab(ResultImageTab, true);
		}
		else
		{
			//hide result image tab
			m_firstImageCtrl.ShowTab(ResultImageTab, false);
			m_secondImageCtrl.ShowTab(ResultImageTab, false);
		}

		m_firstImageCtrl.Refresh();
		m_secondImageCtrl.Refresh();
	}

	void SVToolAdjustmentDialogTwoImagePageClass::RetreiveCurrentlySelectedImageNames()
	{
		const SvUl::InputNameObjectIdPairList& rImageList = GetInputImageList(SvDef::InvalidObjectId, NumberOfImagesRequired);

		// This requires that the input name sorts in descending natural order
		// and that the images we are concerned with are first in the list
		if (rImageList.size() && rImageList.size() > 1)
		{
			SvUl::InputNameObjectIdPairList::const_iterator it = rImageList.begin();
			m_firstInputName = it->first;
			m_firstImageName = it->second.first;
			++it;
			m_secondInputName = it->first;
			m_secondImageName = it->second.first;
		}
	}

	void SVToolAdjustmentDialogTwoImagePageClass::RetreiveResultImageNames()
	{
		const SvUl::NameObjectIdList& rImageList = GetResultImages();

		SvUl::NameObjectIdList::const_iterator it = rImageList.begin();
		if (it != rImageList.end())
		{
			m_resultImageName = it->first;
			m_resultImageID = it->second;
		}
	}
	#pragma endregion Private Methods
} //namespace SvOg

