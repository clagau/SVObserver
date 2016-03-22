// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVWatershedFilterDlg
// * .File Name       : $Workfile:   SVWaterShedFilterDlg.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   15 May 2014 15:01:46  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <boost/assign/list_of.hpp>
#include "SVMatroxLibrary\SVMatroxEnums.h"
#include "SVWatershedFilterDlg.h"
#include "SVObjectLibrary\SVClsids.h"
#include "ObjectInterfaces\TextDefineSvOi.h"
#pragma region Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace SvOi;
enum
{
	OffVarRadioEnum_Off = 0,
	OffVarRadioEnum_Minimum,
	OffVarRadioEnum_Varation
} OffVarRadioEnum;

namespace Seidenader
{
	namespace SVOGui
	{
		static const std::string ControlFlagTag("ControlFlag");
		static const std::string MinVariationTag("MinVariation");
		static const std::string UseMarkerTag("UseMarker");

		SVWatershedFilterDlg::SVWatershedFilterDlg(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, const SVGUID& rFilterID, CWnd* pParent): 
				CDialog(SVWatershedFilterDlg::IDD, pParent)
				,m_filterID(rFilterID)
				,m_rInspectionID(rInspectionID)
				,m_rTaskObjectID(rTaskObjectID)
				, SvOg::ImageController(rInspectionID, rTaskObjectID)
				, m_Values(SvOg::BoundValues(rInspectionID, rFilterID, boost::assign::map_list_of
				(ControlFlagTag, SVWatershedFilterControlFlagGuid)
				(MinVariationTag, SVWatershedFilterMinVariationGuid)
				(UseMarkerTag, SVWatershedFilterUseMarkerGuid)))
	{

		//{{AFX_DATA_INIT(SVWatershedFilterDlg)
		m_bUseBasin = FALSE;
		m_bSkipLastLevel = FALSE;
		m_lMinVariation = 0;
		m_bUseWatershed = FALSE;
		m_iEightWatershedLines = -1;
		m_iMinFillBasin = -1;
		m_iRegularWatershedLines = -1;
		m_iVariationType = -1;
		m_bUseMarker = FALSE;
		//}}AFX_DATA_INIT
	}

	SVWatershedFilterDlg::~SVWatershedFilterDlg()
	{
	}

	HRESULT SVWatershedFilterDlg::SetInspectionData()
	{
		HRESULT l_hrOk = S_OK;

		UpdateData( TRUE ); // get data from dialog

		long lControlFlag = 0;
		lControlFlag |= m_bSkipLastLevel ? SVImageSkipLastLevel : 0;
		lControlFlag |= m_bUseBasin ? SVImageWSBasin : 0;
		lControlFlag |= m_bUseWatershed ? SVImageWSWatershed : 0;

		lControlFlag |= m_iEightWatershedLines ? SVImage8Connected : 0;
		lControlFlag |= m_iMinFillBasin ? SVImageWSMaximaFill  : 0;
		lControlFlag |= m_iRegularWatershedLines ? SVImageWSStraight : 0;

		m_Values.Set<long>(ControlFlagTag, lControlFlag);
		m_Values.Set<long>(MinVariationTag, m_lMinVariation);
		m_Values.Set<bool>(UseMarkerTag, TRUE == m_bUseMarker);
		m_Values.Commit(true);

		UpdateData( FALSE );

		return l_hrOk;
	}

	void SVWatershedFilterDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(SVWatershedFilterDlg)
		DDX_Control(pDX, IDC_SOURCE_IMAGE_COMBO, m_SVSourceImageCombo);
		DDX_Check(pDX, IDC_BASIN_CHECK, m_bUseBasin);
		DDX_Check(pDX, IDC_SKIP_LAST_CHECK, m_bSkipLastLevel);
		DDX_Text(pDX, IDC_VARIATION_EDIT, m_lMinVariation);
		DDX_Check(pDX, IDC_WATERSHED_CHECK, m_bUseWatershed);
		DDX_Radio(pDX, IDC_4CON_LINES_RADIO, m_iEightWatershedLines);
		DDX_Radio(pDX, IDC_MIN_FILL_RADIO, m_iMinFillBasin);
		DDX_Radio(pDX, IDC_REGULAR_RADIO, m_iRegularWatershedLines);
		DDX_Radio(pDX, IDC_OFF_VAR_RADIO, m_iVariationType);
		DDX_Check(pDX, IDC_MARKER_CHECK, m_bUseMarker);
		//}}AFX_DATA_MAP
	}


	BEGIN_MESSAGE_MAP(SVWatershedFilterDlg, CDialog)
		//{{AFX_MSG_MAP(SVWatershedFilterDlg)
		ON_BN_CLICKED(IDC_OFF_VAR_RADIO, OnSETVarRadio)
		ON_BN_CLICKED(IDC_MIN_VAR_RADIO, OnSETVarRadio)
		ON_BN_CLICKED(IDC_SET_VAR_RADIO, OnSETVarRadio)
		ON_CBN_SELCHANGE(IDC_SOURCE_IMAGE_COMBO, OnSelchangeSourceImageCombo)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// SVWatershedFilterDlg message handlers

	BOOL SVWatershedFilterDlg::OnInitDialog()
	{
		CDialog::OnInitDialog();
		Init(); //ImageController
		m_Values.Init();
		long lControlFlag =  m_Values.Get<long>(ControlFlagTag);
		long lMinVariation =  m_Values.Get<long>(MinVariationTag);;

		// Check Boxes
		m_bSkipLastLevel = (lControlFlag & SVImageSkipLastLevel) == SVImageSkipLastLevel;
		m_bUseBasin = (lControlFlag & SVImageWSBasin) == SVImageWSBasin;
		m_bUseWatershed = (lControlFlag & SVImageWSWatershed) == SVImageWSWatershed;
		m_bUseMarker = m_Values.Get<bool>(UseMarkerTag);

		// Radios
		m_iEightWatershedLines = (lControlFlag & SVImage8Connected)  == SVImage8Connected ;
		m_iMinFillBasin = (lControlFlag & SVImageWSMaximaFill  )  == SVImageWSMaximaFill   ;
		m_iRegularWatershedLines = (lControlFlag & SVImageWSStraight  )  == SVImageWSStraight;

		m_iVariationType = lMinVariation < OffVarRadioEnum_Varation ? lMinVariation : OffVarRadioEnum_Varation ;

		// Edit Box
		m_lMinVariation = lMinVariation;

		GetDlgItem( IDC_VARIATION_EDIT )->EnableWindow( OffVarRadioEnum_Varation == m_iVariationType );

		const SvUl::NameGuidList& availImages = GetAvailableImageList();
		for (SvUl::NameGuidList::const_iterator it = availImages.begin(); availImages.end() != it; ++it)
		{
			m_SVSourceImageCombo.AddString(it->first.c_str());
		}
		
		const SvUl::InputNameGuidPairList& connectedImageList = GetConnectedImageList(m_filterID);
		SVString currentMarkerImageName; 
		if (0 < connectedImageList.size() && connectedImageList.begin()->first == SvOi::WatershedMarkerImageConnectionName)
		{
			currentMarkerImageName = connectedImageList.begin()->second.first;
		}
		m_SVSourceImageCombo.SelectString(-1, currentMarkerImageName.c_str());

		UpdateData( FALSE ); // set data to dialog

		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}



	void SVWatershedFilterDlg::OnOK() 
	{
		SetInspectionData();

		CDialog::OnOK();
	}

	void SVWatershedFilterDlg::OnCancel() 
	{
		CDialog::OnCancel();
	}


	void SVWatershedFilterDlg::OnSETVarRadio() 
	{
		UpdateData();
		GetDlgItem( IDC_VARIATION_EDIT )->EnableWindow( OffVarRadioEnum_Varation == m_iVariationType );
		if( OffVarRadioEnum_Varation > m_iVariationType )
		{
			m_lMinVariation = m_iVariationType;
			UpdateData(FALSE);
		}
	}


	void SVWatershedFilterDlg::OnSelchangeSourceImageCombo() 
	{
		UpdateData( TRUE ); // get data from dialog

		int iCurSel = m_SVSourceImageCombo.GetCurSel();
		CString currentImageName;
		m_SVSourceImageCombo.GetLBText(iCurSel, currentImageName);

		if( !currentImageName.IsEmpty() )
		{
			ConnectToImage(SvOi::WatershedMarkerImageConnectionName, SVString(currentImageName), m_filterID);
		}
	}
	}  //end namespace SVOGUI
}  //end namespace Seidenader
