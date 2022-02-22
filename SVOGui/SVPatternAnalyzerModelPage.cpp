//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVPatternAnalyzerModelPage.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVStatusLibrary\GlobalPath.h"
#include "DisplayHelper.h"
#include "GuiValueHelper.h"
#include "Definitions/Color.h"
#include "Definitions/TextDefineSvDef.h"
#include "SVMessage/SVMessage.h"
#include "SVUtilityLibrary/SVPoint.h"
#include "SVImageLibrary/SVImageExtentClass.h"
#include "Definitions/GlobalConst.h"
#include "SVFileSystemLibrary/SVFileNameClass.h"
#include "SVRPropertyTree/SVRPropTreeItemEdit.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
constexpr int SVMinModelWidthWithCircular = 25;
constexpr int SVMinModelHeightWithCircular = 25;
constexpr int ToolImageTab = 0;
constexpr int ModelImageTab = 1;
constexpr int DontCareImageTab = 2;
#pragma endregion Declarations

namespace SvOg
{
	#pragma region Constructor
	SVPatternAnalyzerModelPage::SVPatternAnalyzerModelPage(uint32_t inspectionID, uint32_t analyzerID)
	: CPropertyPage(SVPatternAnalyzerModelPage::IDD)
	, m_InspectionID(inspectionID)
	, m_AnalyzerID(analyzerID)
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
	, SvOg::ImageController(inspectionID, analyzerID)
	, m_values{ SvOg::BoundValues{ inspectionID, analyzerID } }
	{
	}

	SVPatternAnalyzerModelPage::~SVPatternAnalyzerModelPage()
	{
	}
	#pragma endregion Constructor

	#pragma region Protected Methods
	#pragma region AFX Virtual Methods
	void SVPatternAnalyzerModelPage::OnCancel()
	{
	}

	void SVPatternAnalyzerModelPage::OnOK()
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
			SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
			Msg.setMessage( rSvE.getMessage() );
			INT_PTR result = Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Pattern_Invalid_ShouldLeave, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10184, SvDef::InvalidObjectId, MB_YESNO);
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

	BOOL SVPatternAnalyzerModelPage::OnApply()
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

	BOOL SVPatternAnalyzerModelPage::OnSetActive()
	{
		BOOL l_bOk = CPropertyPage::OnSetActive();

		if ( l_bOk )
		{
			InitializeData();
		}

		return l_bOk;
	}

	BOOL SVPatternAnalyzerModelPage::OnKillActive()
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
			SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
			Msg.setMessage( rSvE.getMessage() );
			return FALSE;
		}

	// No need for further data Validation. Just return TRUE
	//	return CPropertyPage::OnKillActive();

		return TRUE;
	}

	BOOL SVPatternAnalyzerModelPage::OnInitDialog()
	{
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

	void SVPatternAnalyzerModelPage::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(SVPatternAnalyzerModelPage)
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
	void SVPatternAnalyzerModelPage::OnFileButton()
	{
		if ( GetModelFile( true, m_strModelName ) ) // true is mode for selected file.
		{
			m_strOldModelName.Empty(); // mark it as update needed
			ProcessOnKillFocus( IDC_PAT1_FILE_NAME );
		}
	}

	void SVPatternAnalyzerModelPage::OnFileDontCareButton()
	{
		if (GetModelFile(true, m_strDontCareName)) // true is mode for selected file.
		{
			ProcessOnKillFocus(IDC_PAT_DONT_CARE_FILE_NAME);
		}
	}

	void SVPatternAnalyzerModelPage::OnCreateModel()
	{
		UpdateData();

		SetValuesToAnalyzer();

		if ( GetModelFile( false, m_strModelName ) ) // false parameter is mode for save file.
		{
			SvPb::InspectionCmdRequest requestCmd;
			SvPb::InspectionCmdResponse responseCmd;
			auto* pRequest= requestCmd.mutable_createmodelrequest();
			pRequest->set_patternanalyzerid(m_AnalyzerID);
			pRequest->set_posx(m_nXPos);
			pRequest->set_posy(m_nYPos);
			pRequest->set_modelwidth(m_lModelWidth);
			pRequest->set_modelheight(m_lModelHeight);
			pRequest->set_filename(m_strModelName);

			HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
			if (S_OK == hr)
			{
				SvStl::MessageContainerVector ErrorMessages;
				RestoreImagesFromFile(&ErrorMessages);
				if (!ErrorMessages.empty())
				{
					SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
					Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_PatAllocModelFailed, SvStl::SourceFileParams(StdMessageParams), 0, m_AnalyzerID );
				}
			}
			else if (responseCmd.has_standardresponse())
			{
				SvStl::MessageContainerVector ErrorMessages = SvPb::convertProtobufToMessageVector(responseCmd.standardresponse().errormessages());
				if (!ErrorMessages.empty())
				{
					SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
					Msg.setMessage( ErrorMessages[0].getMessage() );
				}
			}
		}
	}

	// Kill focus of File name edit control
	void SVPatternAnalyzerModelPage::OnKillModelFileName()
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

	void SVPatternAnalyzerModelPage::OnKillDontCareFileName()
	{
		if (GetFocus() && (GetFocus()->GetDlgCtrlID() == IDC_PAT_DONT_CARE_FILE_BUTTON || GetFocus()->GetDlgCtrlID() == IDC_PAT_USE_DONT_CARE))
		{
			return;
		}

		UpdateData(true);

		ProcessOnKillFocus(IDC_PAT_DONT_CARE_FILE_NAME);
	}

	void SVPatternAnalyzerModelPage::OnUseDontCareClicked()
	{
		UpdateData(true);
		if (m_bDontCare)
		{
			ProcessOnKillFocus(IDC_PAT_DONT_CARE_FILE_NAME);
		}
		SetEnableStateDontCareControls();
	
		setImages();
		setCircularOverscanCheckboxState();
	}

	void SVPatternAnalyzerModelPage::OnCircularOverscanClicked()
	{
		UpdateData(true);
		if (m_bCircularOverscan)
		{
			m_bDontCare = false;
		}
		SetEnableStateDontCareControls();
		setOverlay();
	}

	void SVPatternAnalyzerModelPage::OnKillModelCenter()
	{
		UpdateData(true);
		setModelCenterOverlay();
	}

	void SVPatternAnalyzerModelPage::OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult)
	{
		LPNMPROPTREE pNMPropTree = reinterpret_cast< LPNMPROPTREE >(pNotifyStruct);
		*plResult = S_OK;

		if ( pNMPropTree->pItem )
		{
			SVRPropertyItem* pItem = pNMPropTree->pItem;

			std::string Value;
			long lNewValue;
			pItem->GetItemValue( Value );
			lNewValue = atol( Value.c_str() );

			switch (pItem->GetCtrlID())
			{
			case ModelWidth_Property:
				if (lNewValue >= SvDef::cMinPatternModelNewSize && lNewValue <= m_sourceImageWidth-m_nXPos)
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
				if (lNewValue >= SvDef::cMinPatternModelNewSize && lNewValue <= m_sourceImageHeight-m_nYPos)
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

	BEGIN_MESSAGE_MAP(SVPatternAnalyzerModelPage, CPropertyPage)
		//{{AFX_MSG_MAP(SVPatternAnalyzerModelPage)
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

	BEGIN_EVENTSINK_MAP(SVPatternAnalyzerModelPage, CPropertyPage)
		ON_EVENT(SVPatternAnalyzerModelPage, IDC_DIALOGIMAGE, 8, SVPatternAnalyzerModelPage::ObjectChangedExDialogImage, VTS_I4 VTS_I4 VTS_PVARIANT VTS_PVARIANT)
		ON_EVENT(SVPatternAnalyzerModelPage, IDC_DIALOGIMAGE, 9, SVPatternAnalyzerModelPage::TabChangeDialogImage, VTS_I4)
	END_EVENTSINK_MAP()
	#pragma endregion AFX MSG

	void SVPatternAnalyzerModelPage::ObjectChangedExDialogImage(long Tab, long Handle, VARIANT* ParameterList, VARIANT* ParameterValue)
	{
		if (ToolImageTab == Tab && m_handleToModelOverlayObject == Handle)
		{
			////////////////////////////////////////////////////////
			// SET SHAPE PROPERTIES
			VariantParamMap ParaMap;
			SvOg::DisplayHelper::FillParameterMap(ParaMap, ParameterList, ParameterValue);

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
			SvOg::DisplayHelper::FillParameterMap(ParaMap, ParameterList, ParameterValue);

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

	void SVPatternAnalyzerModelPage::TabChangeDialogImage(long Tab)
	{
		m_dialogImage.SetBoundaryCheck(ModelImageTab != Tab);
	}

	#pragma endregion Protected Methods

	#pragma region Private Methods
	void SVPatternAnalyzerModelPage::ValidateModelParameters(bool shouldReset)
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

	void SVPatternAnalyzerModelPage::ValidateModelWidth()
	{
		UpdateData(true);

		long lMaxPixels = m_sourceImageWidth;
		long minWidth = SvDef::cMinPatternModelLoadSize;

		// Check for Circular Overscan as the Minimum applies to the Inner Rectangle Width
		if (m_bCircularOverscan)
		{
			// Get the Outer Rectangle
			CRect outerRect = computeOverscanRect(CPoint(minWidth << 1, minWidth << 1), CSize(minWidth, minWidth), SvPb::OverscanRectKindEnum::OuterRect);
			
			minWidth = outerRect.Width();
		}
	
		bool bRetVal = (m_lModelWidth >= minWidth && m_lModelWidth <= lMaxPixels);
	
		if (!bRetVal)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_PatModelSizeErr, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10245 );
			throw Msg;
		}
	}

	void SVPatternAnalyzerModelPage::ValidateModelHeight()
	{
		UpdateData(true);

		long lMaxPixels = m_sourceImageHeight;
		long minHeight = SvDef::cMinPatternModelLoadSize;
	
		// Check for Circular Overscan as the Minimum applies to the Inner Rectangle height
		if (m_bCircularOverscan)
		{
			// Get the Outer Rectangle
			CRect outerRect = computeOverscanRect(CPoint(minHeight << 1, minHeight << 1), CSize(minHeight, minHeight), SvPb::OverscanRectKindEnum::OuterRect);
			
			minHeight = outerRect.Height();
		}

		bool bRetVal = (m_lModelHeight >= minHeight && m_lModelHeight <= lMaxPixels);
		if (!bRetVal)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_PatModelSizeErr, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10244 );
			throw Msg;
		}
	}

	void SVPatternAnalyzerModelPage::ValidateModelFilename() // @TODO:  Add actual validation to this method.
	{
		UpdateData(true);

		// Should we have a model file name ?
		if ( !m_strModelName.IsEmpty() )
		{
			// verify that the file exists
		}
	}

	/////////////////////////////////////////////////////////////////////////////
	// SVPatternAnalyzerModelPage message handlers
	void SVPatternAnalyzerModelPage::InitializeData()
	{
		const SvUl::InputNameObjectIdPairList& rImageList = GetInputImageList();
		const SvUl::InputNameObjectIdPairList::const_iterator Iter = rImageList.find(SvDef::ImageAnalyzerImageName);
		if (rImageList.cend() != Iter)
		{
			m_AnalyzerImageID = Iter->second.second;
			GetImage(m_AnalyzerImageID, m_sourceImageWidth, m_sourceImageHeight);
		}	
	
		// Model Selection values
		m_values.Init();
		m_lModelWidth = m_values.Get<long>(SvPb::PatModelWidthEId);
		m_lModelHeight = m_values.Get<long>(SvPb::PatModelHeightEId);
		m_CenterX = m_values.Get<long>(SvPb::PatModelCenterXEId);
		m_CenterY = m_values.Get<long>(SvPb::PatModelCenterYEId);
		m_strModelName = m_values.Get<CString>(SvPb::PatModelImageFileEId);
		m_bCircularOverscan = m_values.Get<bool>(SvPb::PatCircularOverscanEId);
		m_strDontCareName = m_values.Get<CString>(SvPb::PatDontCareImageFileEId);
		m_bDontCare = m_values.Get<bool>(SvPb::PatDontCareEId);
		long resultSize = m_values.Get<long>(SvPb::PatResultNumFoundOccurancesEId);
		m_nXPos = 0;
		m_nYPos = 0;
		if (0 < resultSize)
		{
			double dPosX = 0;
			double dPoxY = 0;
			std::vector<double> ResultXArray = ConvertVariantSafeArrayToVector<double>(m_values.Get<_variant_t>(SvPb::PatResultXEId));
			if (0 < ResultXArray.size())
			{
				dPosX = ResultXArray[0];
			}

			std::vector<double> ResultYArray = ConvertVariantSafeArrayToVector<double>(m_values.Get<_variant_t>(SvPb::PatResultYEId));
			if (0 < ResultYArray.size())
			{
				dPoxY = ResultYArray[0];
			}

			std::vector<double> ResultAngleArray = ConvertVariantSafeArrayToVector<double>(m_values.Get<_variant_t>(SvPb::PatResultAngleEId));
			if (0 < ResultAngleArray.size())
			{
				double angle = ResultAngleArray[0];
				SVPoint<double> moveVector = SVRotatePoint(SVPoint<double>(0, 0), SVPoint<double>(m_CenterX, m_CenterY), -angle);
				m_nXPos = static_cast<int>(dPosX - moveVector.m_x);
				m_nYPos = static_cast<int>(dPoxY - moveVector.m_y);
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
		SetEnableStateDontCareControls();
	}

	BOOL SVPatternAnalyzerModelPage::ProcessOnKillFocus(UINT nId)
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
							SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
							Msg.setMessage( ErrorMessages[0].getMessage() );
						}
						///Note here we must use static_cast as the CWnd* value is a temporary and could then cast to nullptr!
						CEdit* pEdit = static_cast<CEdit*> (GetDlgItem(nId));
						if(nullptr != pEdit)
						{
							pEdit->SetFocus();
							pEdit->SetSel(0, -1);
						}
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
						SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
						Msg.setMessage(ErrorMessages[0].getMessage());
					}

					///Note here we must use static_cast as the CWnd* value is a temporary and could then cast to nullptr!
					CEdit* pEdit = static_cast<CEdit*> (GetDlgItem(nId));
					if (nullptr != pEdit)
					{
						pEdit->SetFocus();
						pEdit->SetSel(0, -1);
					}
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
	bool SVPatternAnalyzerModelPage::RestoreImagesFromFile(SvStl::MessageContainerVector *pErrorMessages)
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

	bool SVPatternAnalyzerModelPage::GetModelFile(bool bMode, CString& rFileName)
	{
		bool bOk = false;

		SVFileNameClass svfncFileName;

		svfncFileName.SetFileType( SV_PATTERN_MODEL_FILE_TYPE );

		//
		// Try to read the current image file path name from registry...
		//
		std::string Path = AfxGetApp()->GetProfileString(_T("Settings"), _T("SVCFilePath"), SvStl::GlobalPath::Inst().GetRunPath().c_str() ).GetString();   // Default

		svfncFileName.SetDefaultPathName( Path );

		UpdateData( TRUE );

		svfncFileName.SetFullFileName(rFileName);
		if ( bMode )
		{
			bOk = svfncFileName.SelectFile();
		}
		else
		{
			bOk = svfncFileName.SaveFile();
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

	HRESULT SVPatternAnalyzerModelPage::BuildPropertyList()
	{
		m_Tree.DeleteAllItems();

		SVRPropertyItem* pRoot = m_Tree.InsertItem(new SVRPropertyItem());
		assert( pRoot );
		pRoot->SetCanShrink(false);
		pRoot->SetInfoText(_T(""));
		pRoot->HideItem();
		pRoot->SetHeight(2);

		std::string Text;
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
		Text= SvUl::Format(_T("Width (%d - %d)"), SvDef::cMinPatternModelNewSize, m_sourceImageWidth);
		pItem->SetLabelText( Text.c_str() );
		pItem->OnRefresh();

		pItem = dynamic_cast <SVRPropertyItemEdit*> ( m_Tree.InsertItem(new SVRPropertyItemEdit(), pRoot) );
		lValue = m_lModelHeight;
		pItem->SetItemValue( SvUl::AsString(lValue).c_str() );
		pItem->SetCtrlID( ModelHeight_Property );
		pItem->SetBold( false );
		pItem->SetHeight(16);
		Text= SvUl::Format(_T("Height (%d - %d)"), SvDef::cMinPatternModelNewSize, m_sourceImageHeight);
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

	HRESULT SVPatternAnalyzerModelPage::RefreshProperties()
	{
		SVRPropertyItem* pRoot = m_Tree.GetRootItem()->GetChild();
		assert( pRoot );
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

	void SVPatternAnalyzerModelPage::setImages()
	{
		if (SvDef::InvalidObjectId != m_AnalyzerImageID)
		{
			IPictureDisp* pFirstImage = GetImage(m_AnalyzerImageID);
			m_dialogImage.setImage( pFirstImage, ToolImageTab );
		}

		IPictureDisp* pSecondImage = GetImage(SvDef::PatternModelImageName);
		m_dialogImage.setImage( pSecondImage, ModelImageTab );

		if (m_bDontCare)
		{
			IPictureDisp* pThirdImage = GetImage(SvDef::PatternDontCareImageName);
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

	void SVPatternAnalyzerModelPage::setOverlay()
	{
		LongParamMap Parmap;
		Parmap[CDSVPictureDisplay::P_Type] = CDSVPictureDisplay::RectangleROI;
		Parmap[CDSVPictureDisplay::P_X1] = m_nXPos;
		Parmap[CDSVPictureDisplay::P_Y1] = m_nYPos;
		Parmap[CDSVPictureDisplay::P_X2] = m_nXPos + m_lModelWidth;
		Parmap[CDSVPictureDisplay::P_Y2] = m_nYPos + m_lModelHeight;
		Parmap[CDSVPictureDisplay::P_Color] = SvDef::Green;
		Parmap[CDSVPictureDisplay::P_SelectedColor] = SvDef::Green;
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

	void SVPatternAnalyzerModelPage::setCircularToolOverlay()
	{
		LongParamMap Parmap;
		Parmap[CDSVPictureDisplay::P_Color] = SvDef::Green;
		Parmap[CDSVPictureDisplay::P_SelectedColor] = SvDef::Green;
		Parmap[CDSVPictureDisplay::P_AllowEdit] = CDSVPictureDisplay::AllowNone;
		CRect innerRect = computeOverscanRect(CPoint(m_nXPos, m_nYPos), CSize(m_lModelWidth, m_lModelHeight), SvPb::OverscanRectKindEnum::InnerRect);
		CRect outerRect = computeOverscanRect(CPoint(innerRect.left, innerRect.top), innerRect.Size(), SvPb::OverscanRectKindEnum::OuterRect);

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

	void SVPatternAnalyzerModelPage::setCircularModelOverlay()
	{
		long modelWidth;
		long modelHeight;
		//Overlay for the model
		IPictureDisp* pModelImage = GetImage(SvDef::PatternModelImageName, modelWidth, modelHeight);
		if (nullptr != pModelImage)
		{
			CRect innerRect = computeOverscanRect(CPoint(0, 0), CSize(modelWidth, modelHeight), SvPb::OverscanRectKindEnum::InnerRect);
			CRect outerRect = computeOverscanRect(CPoint(innerRect.left, innerRect.top), innerRect.Size(), SvPb::OverscanRectKindEnum::OuterRect);
			LongParamMap Parmap;
			Parmap[CDSVPictureDisplay::P_Color] = SvDef::Green;
			Parmap[CDSVPictureDisplay::P_SelectedColor] = SvDef::Green;
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

	void SVPatternAnalyzerModelPage::setModelCenterOverlay()
	{
		constexpr int pointCount = 10;
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
		ParMap[CDSVPictureDisplay::P_Color] = SvDef::LightMagenta;
		ParMap[CDSVPictureDisplay::P_SelectedColor] = SvDef::LightMagenta;
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

	void SVPatternAnalyzerModelPage::setCircularOverscanCheckboxState()
	{
		bool shouldEnable = (SVMinModelWidthWithCircular <= m_lModelWidth && SVMinModelHeightWithCircular <= m_lModelHeight);
		shouldEnable = shouldEnable && !m_bDontCare;

		m_CircularOverscanCheckbox.EnableWindow(shouldEnable);
		if (!shouldEnable)
		{
			m_CircularOverscanCheckbox.SetCheck(0);
			OnCircularOverscanClicked();
		}
		UpdateData(FALSE);
	}

	HRESULT SVPatternAnalyzerModelPage::SetValuesToAnalyzer(SvStl::MessageContainerVector *pErrorMessages, bool shouldResetTask)
	{
		m_values.Set<CString>(SvPb::PatModelImageFileEId, m_strModelName);
		m_values.Set<bool>(SvPb::PatCircularOverscanEId, m_bCircularOverscan ? true : false);
		m_values.Set<long>(SvPb::PatModelCenterXEId, m_CenterX);
		m_values.Set<long>(SvPb::PatModelCenterYEId, m_CenterY);
		m_values.Set<CString>(SvPb::PatDontCareImageFileEId, m_strDontCareName);
		m_values.Set<bool>(SvPb::PatDontCareEId, m_bDontCare ? true : false);

		SvOg::PostAction commitAction {SvOg::PostAction::doRunOnce};
		commitAction = commitAction | (shouldResetTask ? SvOg::PostAction::doReset : SvOg::PostAction::doNothing);
		HRESULT result = m_values.Commit(commitAction);
		if (S_OK != result && nullptr != pErrorMessages)
		{
			*pErrorMessages = m_values.getFailedMessageList();
		}
		return result;
	}

	void SVPatternAnalyzerModelPage::SetEnableStateDontCareControls()
	{
		GetDlgItem(IDC_PAT_USE_DONT_CARE)->EnableWindow(!m_bCircularOverscan);
		GetDlgItem(IDC_PAT_DONT_CARE_FILE_BUTTON)->EnableWindow(!m_bCircularOverscan && m_bDontCare);
		GetDlgItem(IDC_PAT_DONT_CARE_FILE_NAME)->EnableWindow(!m_bCircularOverscan && m_bDontCare);
	}

	CRect SVPatternAnalyzerModelPage::computeOverscanRect(const POINT& point, const SIZE& size, SvPb::OverscanRectKindEnum rectKind)
	{
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_computeoverscanrectrequest();
		pRequest->mutable_point()->set_x(point.x);
		pRequest->mutable_point()->set_y(point.y);
		pRequest->mutable_size()->set_sizex(size.cx);
		pRequest->mutable_size()->set_sizey(size.cy);
		pRequest->set_rectkind(rectKind);

		CRect overscanRect; 
		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);

		if (S_OK == hr && responseCmd.has_computeoverscanrectresponse())
		{
			overscanRect = CRect(responseCmd.computeoverscanrectresponse().overscanrect().left(),
				responseCmd.computeoverscanrectresponse().overscanrect().top(),
				responseCmd.computeoverscanrectresponse().overscanrect().right(),
				responseCmd.computeoverscanrectresponse().overscanrect().bottom());

		}
		
		return overscanRect;
	}

	#pragma endregion Private Methods
} //namespace SvOg
