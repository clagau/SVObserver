//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPatSelectModelPageClass
//* .File Name       : $Workfile:   SVPatSelectModelPageClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   21 Aug 2014 10:40:44  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVPatSelectModelPageClass.h"
#include "SVPatAnalyzeSetupDlgSheet.h"
#include "CameraLibrary\SVGraphix.h"
#include "SVUtilityLibrary/SVStringConversions.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVStatusLibrary\GlobalPath.h"
#include "SVOGui/DisplayHelper.h"
#include "SVOGui/GuiValueHelper.h"
#include "SVOGui\SVColor.h"
#include "GuiCommands/CreateModel.h"
#include "SVMessage/SVMessage.h"
#include "SVMatroxLibrary/SVMatroxPatternInterface.h"
#include "SVImageLibrary/SVExtentPointStruct.h"
#include "SVImageLibrary/SVImageExtentClass.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const int SVMinModelWidth = 4;
static const int SVMinModelHeight = 4;
static const int SVMinModelWidthWithCircular = 25;
static const int SVMinModelHeightWithCircular = 25;
static const int ToolImageTab = 0;
static const int ModelImageTab = 1;
static const int DontCareImageTab = 2;

static const std::string ModelWidthTag("Model Width");
static const std::string ModelHeightTag("Model Height");
static const std::string ModelCenterXTag("Model CenterX");
static const std::string ModelCenterYTag("Model CenterY");
static const std::string ModelImageFileTag("Model ImageFileName");
static const std::string UseCircularOverscanTag("Use Circular Overscan");
static const std::string UseDontCareTag("Use Dont Care Area");
static const std::string DontCareImageFileTag("Dont Care ImageFileName");
static const std::string ResultXTag("ResultX");
static const std::string ResultYTag("ResultY");
static const std::string ResultAngleTag("ResultAngle");
static const std::string ResultSizeTag("ResultSize");
#pragma endregion Declarations

#pragma region Constructor
SVPatModelPageClass::SVPatModelPageClass(const SVGUID& rInspectionID, const SVGUID& rAnalyzerID)
: CPropertyPage(SVPatModelPageClass::IDD)
, m_rInspectionID(rInspectionID)
, m_rAnalyzerID(rAnalyzerID)
, m_strModelName( _T("") )
, m_strDontCareName( _T("") )
, m_sourceImageWidth( 100 )
, m_sourceImageHeight( 100 )
, m_handleToModelOverlayObject( -1 )
, m_handleToCircleOverlayObject( -1 )
, m_handleToSquareOverlayObject( -1 )
, m_handleToCircleOverlayObject2( -1 )
, m_handleToSquareOverlayObject2( -1 )
, m_handleToModelCenterOverlay(-1)
, m_nXPos( 0 )
, m_nYPos( 0 )
, m_lModelWidth( m_sourceImageWidth/2 )
, m_lModelHeight( m_sourceImageHeight/2 )
, m_AnalyzerImageGUID( SV_GUID_NULL )
, SvOg::ImageController(rInspectionID, rAnalyzerID)
, m_values(SvOg::BoundValues(rInspectionID, rAnalyzerID, boost::assign::map_list_of
(ModelWidthTag, SVpatModelWidthObjectGuid)
(ModelHeightTag, SVpatModelHeightObjectGuid)
(ModelCenterXTag, SVpatModelCenterXObjectGuid)
(ModelCenterYTag, SVpatModelCenterYObjectGuid)
(UseCircularOverscanTag, SVpatCircularOverscanObjectGuid)
(ModelImageFileTag, SVpatModelImageFileGuid)
(UseDontCareTag, SVpatDontCareObjectGuid)
(DontCareImageFileTag, SVpatDontCareImageFileGuid)
(ResultXTag, SVpatResultXObjectGuid)
(ResultYTag, SVpatResultYObjectGuid)
(ResultAngleTag, SVpatResultAngleObjectGuid)
(ResultSizeTag, SVpatResultNumFoundOccurancesObjectGuid)))
{
}

SVPatModelPageClass::~SVPatModelPageClass()
{
}
#pragma endregion Constructor

#pragma region Protected Methods
#pragma region AFX Virtual Methods
void SVPatModelPageClass::OnCancel()
{
}

void SVPatModelPageClass::OnOK()
{
	m_bAllowExit = true;

	try
	{
		ValidateModelParameters(true);
		// Should we check if model needs created here?
		CPropertyPage::OnOK();
	}
	catch ( const SvStl::MessageContainer& rSvE )
	{
		//Now that we have caught the exception we would like to display it
		SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
		Msg.setMessage( rSvE.getMessage() );
		INT_PTR result = Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Pattern_Invalid_ShouldLeave, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10184, SV_GUID_NULL, MB_YESNO ); 
		if (IDYES == result)
		{
			m_bAllowExit = false;
		}
		else
		{
			CPropertyPage::OnOK();
		}
	}
}

BOOL SVPatModelPageClass::OnApply()
{
	OnOK();

	if (m_bAllowExit)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL SVPatModelPageClass::OnSetActive()
{
	BOOL l_bOk = CPropertyPage::OnSetActive();

	if ( l_bOk )
	{
		InitializeData();
	}

	return l_bOk;
}

BOOL SVPatModelPageClass::OnKillActive()
{
	UpdateData(true);

	// Do Settings Validation
	try
	{
		ValidateModelParameters();
	}
	catch ( const SvStl::MessageContainer& rSvE )
	{
		//Now that we have caught the exception we would like to display it
		SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
		Msg.setMessage( rSvE.getMessage() );
		return FALSE;
	}

// No need for further data Validation. Just return TRUE
//	return CPropertyPage::OnKillActive();

	return TRUE;
}

BOOL SVPatModelPageClass::OnInitDialog()
{
	BOOL bOK = TRUE;

	CPropertyPage::OnInitDialog();

	Init(); //init of ImageController
	
	///////////////////////////////////////////////////////////////////////
	// Create Property box
	CRect rect;
	// PTS_NOTIFY - SVRPropTree will send notification messages to the parent window
	DWORD dwStyle = WS_CHILD|WS_VISIBLE|PTS_NOTIFY;

	GetDlgItem(IDC_PROPERTIES)->GetWindowRect(rect);
	ScreenToClient(rect);
	GetDlgItem(IDC_PROPERTIES)->DestroyWindow();
	// Create SVRPropTree control
	m_Tree.EnableListBoxStyle();
	m_Tree.Create(dwStyle, rect, this, IDC_PROPERTIES);
	m_Tree.SetColumn( static_cast<long>(rect.Width() * 65. / 100.) ); // @TODO:  Explain the numbers.
	m_Tree.ShowInfoText( false );

	m_dialogImage.AddTab(_T("Tool Image")); 
	m_dialogImage.AddTab(_T("Model Image")); 
	m_dialogImage.AddTab(_T("Don't Care Image"));

	InitializeData();
	BuildPropertyList();
	setImages();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVPatModelPageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVPatModelPageClass)
	DDX_Control(pDX, IDC_PAT_CIRCULAR_OVERSCAN, m_CircularOverscanCheckbox);
	DDX_Check(pDX, IDC_PAT_CIRCULAR_OVERSCAN, m_bCircularOverscan);
	DDX_Text(pDX, IDC_PAT1_FILE_NAME, m_strModelName);
	DDX_Check(pDX, IDC_PAT_USE_DONT_CARE, m_bDontCare);
	DDX_Text(pDX, IDC_PAT_DONT_CARE_FILE_NAME, m_strDontCareName);
	DDX_Text(pDX, IDC_PAT_MODELCENTERX, m_CenterX);
	DDX_Text(pDX, IDC_PAT_MODELCENTERY, m_CenterY);
	DDX_Control(pDX, IDC_DIALOGIMAGE, m_dialogImage);
	//}}AFX_DATA_MAP
}
#pragma endregion AFX Virtual Methods

#pragma region AFX MSG
void SVPatModelPageClass::OnFileButton()
{
	if ( GetModelFile( true, m_strModelName ) ) // true is mode for selected file.
	{
		m_strOldModelName.Empty(); // mark it as update needed
		ProcessOnKillFocus( IDC_PAT1_FILE_NAME );
	}
}

void SVPatModelPageClass::OnFileDontCareButton()
{
	if (GetModelFile(true, m_strDontCareName)) // true is mode for selected file.
	{
		ProcessOnKillFocus(IDC_PAT_DONT_CARE_FILE_NAME);
	}
}

void SVPatModelPageClass::OnCreateModel()
{
	UpdateData();

	SetValuesToAnalyzer();

	if ( GetModelFile( false, m_strModelName ) ) // false parameter is mode for save file.
	{
		SVSharedPtr<GuiCmd::CreateModel> commandPtr = new GuiCmd::CreateModel(m_rAnalyzerID, m_nXPos, m_nYPos, m_lModelWidth, m_lModelHeight, SVString(m_strModelName));
		SVObjectSynchronousCommandTemplate<SVSharedPtr<GuiCmd::CreateModel>> cmd(m_rInspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			SvStl::MessageContainerVector ErrorMessages;
			RestoreImagesFromFile(&ErrorMessages);
			if (!ErrorMessages.empty())
			{
				SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
				Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_PatAllocModelFailed, SvStl::SourceFileParams(StdMessageParams), 0, m_rAnalyzerID );
			}
		}
		else
		{
			SvStl::MessageContainerVector ErrorMessages = commandPtr->getErrorMessages();
			if (!ErrorMessages.empty())
			{
				SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
				Msg.setMessage( ErrorMessages[0].getMessage() );
			}
		}
	}
}

// Kill focus of File name edit control
void SVPatModelPageClass::OnKillModelFileName()
{
	if(GetFocus() && (GetFocus()->GetDlgCtrlID() == IDC_PAT1_FILE_BUTTON))
	{
		return;
	}

	// copy current file to old
	m_strOldModelName = m_strModelName;
	UpdateData(true);

	ProcessOnKillFocus(IDC_PAT1_FILE_NAME);
}

void SVPatModelPageClass::OnKillDontCareFileName()
{
	if (GetFocus() && (GetFocus()->GetDlgCtrlID() == IDC_PAT_DONT_CARE_FILE_BUTTON || GetFocus()->GetDlgCtrlID() == IDC_PAT_USE_DONT_CARE))
	{
		return;
	}

	UpdateData(true);

	ProcessOnKillFocus(IDC_PAT_DONT_CARE_FILE_NAME);
}

void SVPatModelPageClass::OnUseDontCareClicked()
{
	UpdateData(true);
	if (m_bDontCare)
	{
		ProcessOnKillFocus(IDC_PAT_DONT_CARE_FILE_NAME);
	}
	GetDlgItem(IDC_PAT_DONT_CARE_FILE_BUTTON)->EnableWindow(m_bDontCare);
	GetDlgItem(IDC_PAT_DONT_CARE_FILE_NAME)->EnableWindow(m_bDontCare);
	
	setImages();
}

void SVPatModelPageClass::OnCircularOverscanClicked()
{
	UpdateData(true);
	setOverlay();
}

void SVPatModelPageClass::OnKillModelCenter()
{
	UpdateData(true);
	setModelCenterOverlay();
}

void SVPatModelPageClass::OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	LPNMPROPTREE pNMPropTree = reinterpret_cast< LPNMPROPTREE >(pNotifyStruct);
	*plResult = S_OK;

	if ( pNMPropTree->pItem )
	{
		SVRPropertyItem* pItem = pNMPropTree->pItem;

		SVString Value;
		long lNewValue;
		pItem->GetItemValue( Value );
		lNewValue = atol( Value.c_str() );

		switch (pItem->GetCtrlID())
		{
		case ModelWidth_Property:
			if (lNewValue >= SVMinModelWidth && lNewValue <= m_sourceImageWidth-m_nXPos)
			{
				m_lModelWidth = lNewValue;
				setCircularOverscanCheckboxState();
			}
			else
			{
				Value = SvUl::AsString( m_lModelWidth );
				pItem->SetItemValue( Value.c_str() );
			}
			break;
		case ModelHeight_Property:
			if (lNewValue >= SVMinModelHeight && lNewValue <= m_sourceImageHeight-m_nYPos)
			{
				m_lModelHeight = lNewValue;
				setCircularOverscanCheckboxState();
			}
			else
			{
				Value = SvUl::AsString( m_lModelHeight );
				pItem->SetItemValue( Value.c_str() );
			}
			break;
		case ModelOriginX_Property:
			if (lNewValue >= 0 && lNewValue <= m_sourceImageWidth-m_lModelWidth)
			{
				m_nXPos = lNewValue;
			}
			else
			{
				Value = SvUl::AsString( m_nXPos );
				pItem->SetItemValue( Value.c_str() );
			}
			break;
		case ModelOriginY_Property:
			if (lNewValue >= 0 && lNewValue <= m_sourceImageHeight-m_lModelHeight)
			{
				m_nYPos = lNewValue;
			}
			else
			{
				Value = SvUl::AsString( m_nYPos );
				pItem->SetItemValue( Value.c_str() );
			}	
			break;
		default:
			break;
		}
		pItem->OnRefresh();
		setOverlay();
	}// end if ( pNMPropTree->pItem )
}

BEGIN_MESSAGE_MAP(SVPatModelPageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVPatModelPageClass)
	ON_WM_HSCROLL()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_PAT1_FILE_BUTTON, OnFileButton)
	ON_BN_CLICKED(IDC_PAT1_CREATE_MODEL, OnCreateModel)
	ON_BN_CLICKED(IDC_PAT_CIRCULAR_OVERSCAN, OnCircularOverscanClicked)
	ON_BN_CLICKED(IDC_PAT_DONT_CARE_FILE_BUTTON, OnFileDontCareButton)
	ON_BN_CLICKED(IDC_PAT_USE_DONT_CARE, OnUseDontCareClicked)
	ON_EN_KILLFOCUS(IDC_PAT1_FILE_NAME, OnKillModelFileName)
	ON_EN_KILLFOCUS(IDC_PAT_DONT_CARE_FILE_NAME, OnKillDontCareFileName)
	ON_EN_KILLFOCUS(IDC_PAT_MODELCENTERX, OnKillModelCenter)
	ON_EN_KILLFOCUS(IDC_PAT_MODELCENTERY, OnKillModelCenter)
	ON_NOTIFY(PTN_ITEMCHANGED, IDC_PROPERTIES, OnItemChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(SVPatModelPageClass, CDialog)
	ON_EVENT(SVPatModelPageClass, IDC_DIALOGIMAGE, 8, SVPatModelPageClass::ObjectChangedExDialogImage, VTS_I4 VTS_I4 VTS_PVARIANT VTS_PVARIANT)
	ON_EVENT(SVPatModelPageClass, IDC_DIALOGIMAGE, 9, SVPatModelPageClass::TabChangeDialogImage, VTS_I4)
END_EVENTSINK_MAP()
#pragma endregion AFX MSG

void SVPatModelPageClass::ObjectChangedExDialogImage(long Tab, long Handle, VARIANT* ParameterList, VARIANT* ParameterValue)
{
	if (ToolImageTab == Tab && m_handleToModelOverlayObject == Handle)
	{
		////////////////////////////////////////////////////////
		// SET SHAPE PROPERTIES
		VariantParamMap ParaMap;
		int count = SvOg::DisplayHelper::FillParameterMap(ParaMap, ParameterList, ParameterValue);

		if( ParaMap.end() != ParaMap.find(CDSVPictureDisplay::P_X1) && VT_I4 == ParaMap[CDSVPictureDisplay::P_X1].vt &&
			ParaMap.end() != ParaMap.find(CDSVPictureDisplay::P_X2) && VT_I4 == ParaMap[CDSVPictureDisplay::P_X2].vt &&
			ParaMap.end() != ParaMap.find(CDSVPictureDisplay::P_Y1) && VT_I4 == ParaMap[CDSVPictureDisplay::P_Y1].vt &&
			ParaMap.end() != ParaMap.find(CDSVPictureDisplay::P_Y2) && VT_I4 == ParaMap[CDSVPictureDisplay::P_Y2].vt)
		{
			m_lModelWidth = ParaMap[CDSVPictureDisplay::P_X2].lVal - ParaMap[CDSVPictureDisplay::P_X1].lVal;
			m_lModelHeight = ParaMap[CDSVPictureDisplay::P_Y2].lVal - ParaMap[CDSVPictureDisplay::P_Y1].lVal;
			m_nXPos = ParaMap[CDSVPictureDisplay::P_X1].lVal;
			m_nYPos = ParaMap[CDSVPictureDisplay::P_Y1].lVal;

			setCircularOverscanCheckboxState();
		}

		RefreshProperties();
	}
	else if (ModelImageTab == Tab && m_handleToModelCenterOverlay == Handle)
	{
		VariantParamMap ParaMap;
		int count = SvOg::DisplayHelper::FillParameterMap(ParaMap, ParameterList, ParameterValue);

		if( ParaMap.end() != ParaMap.find(CDSVPictureDisplay::P_ARRAY_XY) && VT_ARRAY == (ParaMap[CDSVPictureDisplay::P_ARRAY_XY].vt & VT_ARRAY) 
			&& (VT_I4 == (ParaMap[CDSVPictureDisplay::P_ARRAY_XY].vt & VT_I4) ))
		{
			const _variant_t& ValueArray = ParaMap[CDSVPictureDisplay::P_ARRAY_XY];
			long length = ValueArray.parray->rgsabound[0].cElements;
			if (6 <= length)
			{
				void* data = nullptr;
				SafeArrayAccessData(ValueArray.parray, &data);
				long* dataLong = reinterpret_cast< long* >( data );
				m_CenterX = dataLong[4];
				m_CenterY = dataLong[5];
				SafeArrayAccessData(ValueArray.parray, &data);
			}
		}
		UpdateData(false);
	}
	setOverlay();
}

void SVPatModelPageClass::TabChangeDialogImage(long Tab)
{
	m_dialogImage.SetBoundaryCheck(ModelImageTab != Tab);
}

#pragma endregion Protected Methods

#pragma region Private Methods
void SVPatModelPageClass::ValidateModelParameters(bool shouldReset)
{
	UpdateData(true);

	ValidateModelWidth();
	ValidateModelHeight();
	ValidateModelFilename();

	SvStl::MessageContainerVector ErrorMessages;
	SetValuesToAnalyzer(&ErrorMessages, shouldReset);
	if (!ErrorMessages.empty())
	{
		throw ErrorMessages[0];
	}
}

void SVPatModelPageClass::ValidateModelWidth()
{
	UpdateData(true);

	long lMaxPixels = m_sourceImageWidth;
	long minWidth = SVMinModelWidth;

	// Check for Circular Overscan as the Minimum applies to the Inner Rectangle Width
	if (m_bCircularOverscan)
	{
		// Get the Outer Rectangle
		CRect outerRect = SVMatroxPatternInterface::CalculateOverscanOuterRect(CPoint(SVMinModelWidth << 1, SVMinModelHeight << 1), CSize(SVMinModelWidth, SVMinModelHeight));

		minWidth = outerRect.Width();
	}
	
	bool bRetVal = (m_lModelWidth >= minWidth && m_lModelWidth <= lMaxPixels);
	
	if (!bRetVal)
	{
		SvStl::MessageContainer Msg( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_PatModelSizeErr, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10245 );
		throw Msg;
	}
}

void SVPatModelPageClass::ValidateModelHeight()
{
	UpdateData(true);

	long lMaxPixels = m_sourceImageHeight;
	long minHeight = SVMinModelHeight;
	
	// Check for Circular Overscan as the Minimum applies to the Inner Rectangle height
	if (m_bCircularOverscan)
	{
		// Get the Outer Rectangle
		CRect outerRect = SVMatroxPatternInterface::CalculateOverscanOuterRect(CPoint(SVMinModelWidth << 1, SVMinModelHeight << 1), CSize(SVMinModelWidth, SVMinModelHeight));

		minHeight = outerRect.Height();
	}

	bool bRetVal = (m_lModelHeight >= minHeight && m_lModelHeight <= lMaxPixels);
	if (!bRetVal)
	{
		SvStl::MessageContainer Msg( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_PatModelSizeErr, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10244 );
		throw Msg;
	}
}

void SVPatModelPageClass::ValidateModelFilename() // @TODO:  Add actual validation to this method.
{
	UpdateData(true);

	// Should we have a model file name ?
	if ( !m_strModelName.IsEmpty() )
	{
		// verify that the file exists
	}
}

/////////////////////////////////////////////////////////////////////////////
// SVPatModelPageClass message handlers
void SVPatModelPageClass::InitializeData()
{
	const SvUl::InputNameGuidPairList& rImageList = GetConnectedImageList();
	const SvUl::InputNameGuidPairList::const_iterator Iter = rImageList.find(SvOi::ImageAnalyzerImageName); 
	if (rImageList.cend() != Iter)
	{
		m_AnalyzerImageGUID = Iter->second.second;
		GetImage(m_AnalyzerImageGUID, m_sourceImageWidth, m_sourceImageHeight);
	}	
	
	// Model Selection values
	m_values.Init();
	m_lModelWidth = m_values.Get<long>(ModelWidthTag);
	m_lModelHeight = m_values.Get<long>(ModelHeightTag);
	m_CenterX = m_values.Get<long>(ModelCenterXTag);
	m_CenterY = m_values.Get<long>(ModelCenterYTag);
	m_strModelName = m_values.Get<CString>(ModelImageFileTag); 
	m_bCircularOverscan = m_values.Get<bool>(UseCircularOverscanTag);
	m_strDontCareName = m_values.Get<CString>(DontCareImageFileTag); 
	m_bDontCare = m_values.Get<bool>(UseDontCareTag);
	int resultSize = m_values.Get<bool>(ResultSizeTag);
	m_nXPos = 0;
	m_nYPos = 0;
	if (0 < resultSize)
	{
		double dPosX = 0;
		double dPoxY = 0;
		std::vector<double> ResultXArray = SvOg::ConvertVariantSafeArrayToVector<double>(m_values.Get<_variant_t>(ResultXTag));
		if (0 < ResultXArray.size())
		{
			dPosX = ResultXArray[0];
		}

		std::vector<double> ResultYArray = SvOg::ConvertVariantSafeArrayToVector<double>(m_values.Get<_variant_t>(ResultYTag));
		if (0 < ResultYArray.size())
		{
			dPoxY = ResultYArray[0];
		}

		double angle = 0;
		std::vector<double> ResultAngleArray = SvOg::ConvertVariantSafeArrayToVector<double>(m_values.Get<_variant_t>(ResultAngleTag));
		if (0 < ResultAngleArray.size())
		{
			angle = ResultAngleArray[0];
			SVExtentPointStruct moveVector = SVRotatePoint(SVExtentPointStruct(0, 0), SVExtentPointStruct(m_CenterX, m_CenterY), -angle);
			m_nXPos = static_cast<int>(dPosX - moveVector.m_dPositionX);
			m_nYPos = static_cast<int>(dPoxY - moveVector.m_dPositionY);
		}
	}

	if(m_sourceImageHeight < m_lModelHeight)
	{
		m_lModelHeight = m_sourceImageHeight - 1;
	}

	if(m_sourceImageWidth < m_lModelWidth)
	{
		m_lModelWidth = m_sourceImageWidth - 1;
	}

	// Initialize the Slider for X origin
	int nMaxX = m_sourceImageWidth - m_lModelWidth;

	if(m_nXPos > nMaxX)
	{
		m_nXPos = nMaxX;
	}

	// Initialize the Slider for Y origin
	int nMaxY = m_sourceImageHeight - m_lModelHeight;

	if(m_nYPos > nMaxY)
	{
		m_nYPos = nMaxY;
	}

	UpdateData( false );
	setCircularOverscanCheckboxState();
}

BOOL SVPatModelPageClass::ProcessOnKillFocus(UINT nId)
{
	CPropertySheet* pPropSheet = dynamic_cast<CPropertySheet*>(GetParent());
	int activeIndex = (nullptr != pPropSheet) ? pPropSheet->GetActiveIndex() : 0;
	if (GetActiveWindow() != GetParent() || 0 != activeIndex)
	{
		return true;
	}

	switch (nId)
	{
		case IDC_PAT1_FILE_NAME:
		{
			// if no model name - don't create
			if (m_strModelName.IsEmpty())
			{
				break;
			}

			// check that name has changed
			if (m_strModelName != m_strOldModelName)
			{
				// Extract Model from the file
				SvStl::MessageContainerVector ErrorMessages;
				if (!RestoreImagesFromFile(&ErrorMessages))
				{
					if (!ErrorMessages.empty())
					{
						SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
						Msg.setMessage( ErrorMessages[0].getMessage() );
					}
					
					GetDlgItem(nId)->SetFocus();
					CWnd* tmp = (GetDlgItem(nId));
					((CEdit*)GetDlgItem(nId))->SetSel(0, -1);
					return false;
				}
			}
			break;
		}
		case IDC_PAT_DONT_CARE_FILE_NAME:
		{
			// if no model name - don't create
			if (m_strDontCareName.IsEmpty())
			{
				break;
			}
			SvStl::MessageContainerVector ErrorMessages;
			if (!RestoreImagesFromFile(&ErrorMessages))
			{
				if (!ErrorMessages.empty())
				{
					SvStl::MessageMgrStd Msg(SvStl::LogAndDisplay);
					Msg.setMessage(ErrorMessages[0].getMessage());
				}

				GetDlgItem(nId)->SetFocus();
				((CEdit*)GetDlgItem(nId))->SetSel(0, -1);
				return false;
			}
			break;
		}
		default:
		{
			break; // Do nothing.
		}
	}

	return true;
}

// If an error occurs, return the Error message Id, otherwise return 0;
bool SVPatModelPageClass::RestoreImagesFromFile(SvStl::MessageContainerVector *pErrorMessages)
{
	UpdateData( true );

	// set analyzer values
	if ( S_OK == SetValuesToAnalyzer(pErrorMessages, true) )
	{
		InitializeData();
		RefreshProperties();
		UpdateData(false);
	}
	else
	{
		return false;
	}

	setImages();
	return true;
}

bool SVPatModelPageClass::GetModelFile(bool bMode, CString& rFileName)
{
	bool bOk = false;

	SVFileNameClass svfncFileName;

	svfncFileName.SetFileType( SV_PATTERN_MODEL_FILE_TYPE );

	//
	// Try to read the current image file path name from registry...
	//
	SVString Path = AfxGetApp()->GetProfileString(	_T( "Settings" ), 
		                                              _T( "SVCFilePath" ), 
		                                              SvStl::GlobalPath::Inst().GetRunPath().c_str() );   // Default

	svfncFileName.SetDefaultPathName( Path );

	UpdateData( TRUE );

	svfncFileName.SetFullFileName(rFileName);
	if ( bMode )
	{
		bOk = (TRUE == svfncFileName.SelectFile());
	}
	else
	{
		bOk = (TRUE == svfncFileName.SaveFile());
	}

	if ( bOk )
	{
		AfxGetApp()->WriteProfileString( _T( "Settings" ),
		                                 _T( "SVCFilePath" ),
										svfncFileName.GetPathName().c_str() );

		rFileName = svfncFileName.GetFullFileName().c_str();
		
		UpdateData( false );
	}

	return bOk;
}

HRESULT SVPatModelPageClass::BuildPropertyList()
{
	m_Tree.DeleteAllItems();

	SVRPropertyItem* pRoot = m_Tree.InsertItem(new SVRPropertyItem());
	ASSERT( pRoot );
	pRoot->SetCanShrink(false);
	pRoot->SetInfoText(_T(""));
	pRoot->HideItem();
	pRoot->SetHeight(2);

	SVString Text;
	SVRPropertyItemEdit* pItem = dynamic_cast <SVRPropertyItemEdit*> ( m_Tree.InsertItem(new SVRPropertyItemEdit(), pRoot) );
	long lValue = m_nXPos;
	pItem->SetItemValue( SvUl::AsString(lValue).c_str() );
	pItem->SetCtrlID( ModelOriginX_Property );
	pItem->SetBold( false );
	pItem->SetHeight(16);
	pItem->SetLabelText( "X-Position" );
	pItem->OnRefresh();

	pItem = dynamic_cast <SVRPropertyItemEdit*> ( m_Tree.InsertItem(new SVRPropertyItemEdit(), pRoot) );
	lValue = m_nYPos;
	pItem->SetItemValue( SvUl::AsString(lValue).c_str() );
	pItem->SetCtrlID( ModelOriginY_Property );
	pItem->SetBold( false );
	pItem->SetHeight(16);
	pItem->SetLabelText( "Y-Position" );
	pItem->OnRefresh();

	pItem = dynamic_cast <SVRPropertyItemEdit*> ( m_Tree.InsertItem(new SVRPropertyItemEdit(), pRoot) );
	lValue = m_lModelWidth;
	pItem->SetItemValue( SvUl::AsString(lValue).c_str() );
	pItem->SetCtrlID( ModelWidth_Property );
	pItem->SetBold( false );
	pItem->SetHeight(16);
	Text= SvUl_SF::Format(_T("Width (%d - %d)"),  SVMinModelWidth, m_sourceImageWidth);
	pItem->SetLabelText( Text.c_str() );
	pItem->OnRefresh();

	pItem = dynamic_cast <SVRPropertyItemEdit*> ( m_Tree.InsertItem(new SVRPropertyItemEdit(), pRoot) );
	lValue = m_lModelHeight;
	pItem->SetItemValue( SvUl::AsString(lValue).c_str() );
	pItem->SetCtrlID( ModelHeight_Property );
	pItem->SetBold( false );
	pItem->SetHeight(16);
	Text= SvUl_SF::Format(_T("Height (%d - %d)"), SVMinModelHeight, m_sourceImageHeight);
	pItem->SetLabelText( Text.c_str() );
	pItem->OnRefresh();

	SVRPropertyItem* pChild = pRoot->GetChild();
	while ( pChild )
	{
		pChild->Expand( TRUE );
		pChild = pChild->GetSibling();
	}
	pRoot->Expand( true );	// needed for redrawing

	RefreshProperties();	// redraw with appropriate bAvailableWithAutoResize status

	return S_OK;
}

HRESULT SVPatModelPageClass::RefreshProperties()
{
	SVRPropertyItem* pRoot = m_Tree.GetRootItem()->GetChild();
	ASSERT( pRoot );
	SVRPropertyItem* pChild = pRoot->GetChild();

	while ( nullptr != pChild )
	{
		switch (pChild->GetCtrlID())
		{
		case ModelWidth_Property:
			pChild->SetItemValue( SvUl::AsString( m_lModelWidth ).c_str() );
			break;
		case ModelHeight_Property:
			pChild->SetItemValue( SvUl::AsString( m_lModelHeight ).c_str() );
			break;
		case ModelOriginX_Property:
			pChild->SetItemValue( SvUl::AsString( m_nXPos ).c_str() );
			break;
		case ModelOriginY_Property:
			pChild->SetItemValue( SvUl::AsString( m_nYPos ).c_str() );
			break;
		}
		pChild = pChild->GetSibling();
	}

	m_Tree.RedrawWindow();
	return S_OK;
}

void SVPatModelPageClass::setImages()
{
	if ( SV_GUID_NULL != m_AnalyzerImageGUID )
	{
		IPictureDisp* pFirstImage = GetImage(m_AnalyzerImageGUID);
		m_dialogImage.setImage( pFirstImage, ToolImageTab );
	}

	IPictureDisp* pSecondImage = GetImage(SvOi::PatternModelImageName);
	m_dialogImage.setImage( pSecondImage, ModelImageTab );

	if (m_bDontCare)
	{
		IPictureDisp* pThirdImage = GetImage(SvOi::PatternDontCareImageName);
		m_dialogImage.setImage(pThirdImage, DontCareImageTab);
		m_dialogImage.ShowTab(DontCareImageTab, true);
	}
	else
	{
		m_dialogImage.ShowTab(DontCareImageTab, false);
	}

	setOverlay();
	m_dialogImage.Refresh();
}

void SVPatModelPageClass::setOverlay()
{
	LongParamMap Parmap;
	Parmap[ CDSVPictureDisplay::P_Type] = CDSVPictureDisplay::RectangleROI;
	Parmap[CDSVPictureDisplay::P_X1] = m_nXPos;
	Parmap[CDSVPictureDisplay::P_Y1] = m_nYPos;
	Parmap[CDSVPictureDisplay::P_X2] = m_nXPos + m_lModelWidth;
	Parmap[CDSVPictureDisplay::P_Y2] = m_nYPos + m_lModelHeight;
	Parmap[CDSVPictureDisplay::P_Color] = SVColor::Green;
	Parmap[CDSVPictureDisplay::P_SelectedColor] = SVColor::Green;
	Parmap[CDSVPictureDisplay::P_AllowEdit] = CDSVPictureDisplay::AllowResizeAndMove;

	if ( -1 < m_handleToModelOverlayObject )
	{
		m_dialogImage.EditOverlay(ToolImageTab, m_handleToModelOverlayObject, Parmap);
	}
	else
	{
		m_dialogImage.AddOverlay(ToolImageTab, Parmap, &m_handleToModelOverlayObject);
	}
	m_dialogImage.SetEditAllow(ToolImageTab, m_handleToModelOverlayObject, 7);

	if (m_bCircularOverscan)
	{
		setCircularToolOverlay();
		setCircularModelOverlay();
	}
	else
	{
		if (m_handleToCircleOverlayObject != -1)
		{
			m_dialogImage.RemoveOverlay(ToolImageTab, m_handleToCircleOverlayObject);
			m_handleToCircleOverlayObject = -1;
		}
		if (m_handleToCircleOverlayObject2 != -1)
		{
			m_dialogImage.RemoveOverlay(ModelImageTab, m_handleToCircleOverlayObject2);
			m_handleToCircleOverlayObject2 = -1;
		}
		if (m_handleToSquareOverlayObject != -1)
		{
			m_dialogImage.RemoveOverlay(ToolImageTab, m_handleToSquareOverlayObject);
			m_handleToSquareOverlayObject = -1;
		}
		if (m_handleToSquareOverlayObject2 != -1)
		{
			m_dialogImage.RemoveOverlay(ModelImageTab, m_handleToSquareOverlayObject2);
			m_handleToSquareOverlayObject2 = -1;
		}
	}

	setModelCenterOverlay();
}

void SVPatModelPageClass::setCircularToolOverlay()
{
	LongParamMap Parmap;
	Parmap[CDSVPictureDisplay::P_Color] = SVColor::Green;
	Parmap[CDSVPictureDisplay::P_SelectedColor] = SVColor::Green;
	Parmap[CDSVPictureDisplay::P_AllowEdit] = CDSVPictureDisplay::AllowNone;
	CRect innerRect = SVMatroxPatternInterface::CalculateOverscanInnerRect(CPoint(m_nXPos, m_nYPos), CSize(m_lModelWidth, m_lModelHeight));
	CRect outerRect = SVMatroxPatternInterface::CalculateOverscanOuterRect(CPoint(innerRect.left, innerRect.top), innerRect.Size() );
	//Circle overlay
	Parmap[CDSVPictureDisplay::P_Type] = CDSVPictureDisplay::EllipseROI;
	Parmap[CDSVPictureDisplay::P_X1] = outerRect.left;
	Parmap[CDSVPictureDisplay::P_Y1] = outerRect.top;
	Parmap[CDSVPictureDisplay::P_X2] = outerRect.right;
	Parmap[CDSVPictureDisplay::P_Y2] = outerRect.bottom;
	if ( -1 < m_handleToCircleOverlayObject )
	{
		m_dialogImage.EditOverlay(ToolImageTab, m_handleToCircleOverlayObject, Parmap);
	}
	else
	{
		m_dialogImage.AddOverlay(ToolImageTab, Parmap, &m_handleToCircleOverlayObject);
	}

	//Square overlay
	Parmap[CDSVPictureDisplay::P_Type] = CDSVPictureDisplay::RectangleROI;
	Parmap[CDSVPictureDisplay::P_X1] = innerRect.left;
	Parmap[CDSVPictureDisplay::P_Y1] = innerRect.top;
	Parmap[CDSVPictureDisplay::P_X2] = innerRect.right;
	Parmap[CDSVPictureDisplay::P_Y2] = innerRect.bottom;
	if ( -1 < m_handleToSquareOverlayObject )
	{
		m_dialogImage.EditOverlay(ToolImageTab, m_handleToSquareOverlayObject, Parmap);
	}
	else
	{
		m_dialogImage.AddOverlay(ToolImageTab, Parmap, &m_handleToSquareOverlayObject);
	}
}

void SVPatModelPageClass::setCircularModelOverlay()
{
	long modelWidth;
	long modelHeight;
	//Overlay for the model
	IPictureDisp* pModelImage = GetImage(SvOi::PatternModelImageName, modelWidth, modelHeight);
	if (nullptr != pModelImage)
	{
		CRect innerRect = SVMatroxPatternInterface::CalculateOverscanInnerRect(CPoint(0, 0), CSize(modelWidth, modelHeight));
		CRect outerRect = SVMatroxPatternInterface::CalculateOverscanOuterRect(CPoint(innerRect.left, innerRect.top), innerRect.Size() );
		LongParamMap Parmap;
		Parmap[CDSVPictureDisplay::P_Color] = SVColor::Green;
		Parmap[CDSVPictureDisplay::P_SelectedColor] = SVColor::Green;
		Parmap[CDSVPictureDisplay::P_AllowEdit] = CDSVPictureDisplay::AllowNone;
		//Circle overlay
		Parmap[CDSVPictureDisplay::P_Type] = CDSVPictureDisplay::EllipseROI;
		Parmap[CDSVPictureDisplay::P_X1] = outerRect.left;
		Parmap[CDSVPictureDisplay::P_Y1] = outerRect.top;
		Parmap[CDSVPictureDisplay::P_X2] = outerRect.right;
		Parmap[CDSVPictureDisplay::P_Y2] = outerRect.bottom;

		if ( -1 < m_handleToCircleOverlayObject2 )
		{
			m_dialogImage.EditOverlay(ModelImageTab, m_handleToCircleOverlayObject2, Parmap);
		}
		else
		{
			m_dialogImage.AddOverlay(ModelImageTab, Parmap, &m_handleToCircleOverlayObject2);
		}

		//Square overlay
		Parmap[CDSVPictureDisplay::P_Type] = CDSVPictureDisplay::RectangleROI;
		Parmap[CDSVPictureDisplay::P_X1] = innerRect.left;
		Parmap[CDSVPictureDisplay::P_Y1] = innerRect.top;
		Parmap[CDSVPictureDisplay::P_X2] = innerRect.right;
		Parmap[CDSVPictureDisplay::P_Y2] = innerRect.bottom;
		if ( -1 < m_handleToSquareOverlayObject2 )
		{
			m_dialogImage.EditOverlay(ModelImageTab, m_handleToSquareOverlayObject2, Parmap);
		}
		else
		{
			m_dialogImage.AddOverlay(ModelImageTab, Parmap, &m_handleToSquareOverlayObject2);
		}
	}  //if (nullptr != pModelImage)
}

void SVPatModelPageClass::setModelCenterOverlay()
{
	static const int pointCount = 10;
	int points[pointCount];
	points[0] = m_CenterX;
	points[1] = 0;
	points[2] = m_CenterX;
	points[3] = m_lModelHeight;
	points[4] = m_CenterX;
	points[5] = m_CenterY;
	points[6] = 0;
	points[7] = m_CenterY;
	points[8] = m_lModelWidth;
	points[9] = m_CenterY;


	COleSafeArray arraySafe;
	arraySafe.CreateOneDim(VT_I4, pointCount, points);
	std::map<long,_variant_t> ParMap;
	ParMap[CDSVPictureDisplay::P_Type ] = static_cast<long>(CDSVPictureDisplay::GraphROI);
	ParMap[CDSVPictureDisplay::P_SubType_X ] = static_cast<long>(CDSVPictureDisplay::ImageArea_NoScale);
	ParMap[CDSVPictureDisplay::P_SubType_Y ] = static_cast<long>(CDSVPictureDisplay::ImageArea_NoScale);
	ParMap[CDSVPictureDisplay::P_Color] = SVColor::LightMagenta;
	ParMap[CDSVPictureDisplay::P_SelectedColor] = SVColor::LightMagenta;
	ParMap[CDSVPictureDisplay::P_AllowEdit] = CDSVPictureDisplay::AllowMove;
	ParMap[CDSVPictureDisplay::P_ARRAY_XY] = arraySafe;

	if ( -1 < m_handleToModelCenterOverlay )
	{
		m_dialogImage.EditOverlay(ModelImageTab, m_handleToModelCenterOverlay, ParMap);
	}
	else
	{
		m_dialogImage.AddOverlay(ModelImageTab, ParMap, &m_handleToModelCenterOverlay);
	}
}

void SVPatModelPageClass::setCircularOverscanCheckboxState()
{
	bool shouldEnable = (SVMinModelWidthWithCircular <= m_lModelWidth && SVMinModelHeightWithCircular <= m_lModelHeight);

	m_CircularOverscanCheckbox.EnableWindow(shouldEnable);
	if (!shouldEnable)
	{
		m_CircularOverscanCheckbox.SetCheck(0);
		OnCircularOverscanClicked();
	}
	UpdateData(FALSE);
}

HRESULT SVPatModelPageClass::SetValuesToAnalyzer(SvStl::MessageContainerVector *pErrorMessages, bool shouldResetTask)
{
	m_values.Set<CString>(ModelImageFileTag, m_strModelName);
	m_values.Set<bool>(UseCircularOverscanTag, m_bCircularOverscan ? true : false);
	m_values.Set<long>(ModelCenterXTag, m_CenterX);
	m_values.Set<long>(ModelCenterYTag, m_CenterY);
	m_values.Set<CString>(DontCareImageFileTag, m_strDontCareName);
	m_values.Set<bool>(UseDontCareTag, m_bDontCare ? true : false);
	HRESULT result = m_values.Commit(shouldResetTask);
	if (S_OK != result && nullptr != pErrorMessages)
	{
		*pErrorMessages = m_values.getCommitErrorList();
	}
	return result;
}

#pragma endregion Private Methods

