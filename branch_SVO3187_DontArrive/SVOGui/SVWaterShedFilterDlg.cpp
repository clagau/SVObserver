// ******************************************************************************
// * COPYRIGHT (c) 2004 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVWatershedFilterDlg.h"
#include "SVMatroxLibrary/SVMatroxSimpleEnums.h"
#include "Definitions/TextDefineSVDef.h"
#pragma region Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum
{
	OffVarRadioEnum_Off = 0,
	OffVarRadioEnum_Minimum,
	OffVarRadioEnum_Varation
} OffVarRadioEnum;

namespace SvOg
{
	SVWatershedFilterDlg::SVWatershedFilterDlg(uint32_t inspectionId, uint32_t taskObjectId, uint32_t filterId, CWnd* pParent) :
		CDialog(SVWatershedFilterDlg::IDD, pParent)
		,m_filterID(filterId)
		,m_InspectionID(inspectionId)
		,m_TaskObjectID(taskObjectId)
		, SvOg::ImageController(inspectionId, taskObjectId)
		, m_values{ SvOg::BoundValues{ inspectionId, filterId } }
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

		m_values.Set<long>(SvPb::WatershedFilterControlFlagEId, lControlFlag);
		m_values.Set<long>(SvPb::WatershedFilterMinVariationEId, m_lMinVariation);
		m_values.Set<bool>(SvPb::WatershedFilterUseMarkerEId, m_bUseMarker ? true : false);
		m_values.Commit(SvOg::PostAction::doReset | SvOg::PostAction::doRunOnce);

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
		m_values.Init();
		long lControlFlag =  m_values.Get<long>(SvPb::WatershedFilterControlFlagEId);
		long lMinVariation =  m_values.Get<long>(SvPb::WatershedFilterMinVariationEId);;

		// Check Boxes
		m_bSkipLastLevel = (lControlFlag & SVImageSkipLastLevel) == SVImageSkipLastLevel;
		m_bUseBasin = (lControlFlag & SVImageWSBasin) == SVImageWSBasin;
		m_bUseWatershed = (lControlFlag & SVImageWSWatershed) == SVImageWSWatershed;
		m_bUseMarker = m_values.Get<bool>(SvPb::WatershedFilterUseMarkerEId);

		// Radios
		m_iEightWatershedLines = (lControlFlag & SVImage8Connected)  == SVImage8Connected ;
		m_iMinFillBasin = (lControlFlag & SVImageWSMaximaFill  )  == SVImageWSMaximaFill   ;
		m_iRegularWatershedLines = (lControlFlag & SVImageWSStraight  )  == SVImageWSStraight;

		m_iVariationType = lMinVariation < OffVarRadioEnum_Varation ? lMinVariation : OffVarRadioEnum_Varation ;

		// Edit Box
		m_lMinVariation = lMinVariation;

		GetDlgItem( IDC_VARIATION_EDIT )->EnableWindow( OffVarRadioEnum_Varation == m_iVariationType );

		const SvUl::NameObjectIdList& availImages = GetAvailableImageList();
		for (SvUl::NameObjectIdList::const_iterator it = availImages.begin(); availImages.end() != it; ++it)
		{
			m_SVSourceImageCombo.AddString(it->first.c_str());
		}
		
		const auto& connectedImageList = GetInputImageList(m_filterID);
		std::string currentMarkerImageName; 
		if (0 < connectedImageList.size() && connectedImageList.begin()->inputname() == SvDef::WatershedMarkerImageConnectionName)
		{
			currentMarkerImageName = connectedImageList.begin()->connected_objectdottedname();
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
			ConnectToImage(SvDef::WatershedMarkerImageConnectionName, std::string(currentImageName), m_filterID);
		}
	}
}  //namespace SvOg
