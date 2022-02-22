//******************************************************************************
//* COPYRIGHT (c) 2005 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMaskShapeEditorDlg
//* .File Name       : $Workfile:   SVMaskShapeEditorDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   26 Jun 2014 17:52:12  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <colordlg.h>
#include "SVMaskShapeEditorDlg.h"
#include "Definitions/Color.h"
#include "SVOGui/DisplayHelper.h"
#include "SVRPropertyTree/SVRPropTreeItemCombo.h"
#include "SVRPropertyTree/SVRPropTreeItemEdit.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVOGui/LinkedValueSelectorDialog.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVMaskShapeEditorDlg* SVMaskShapeEditorDlg::m_pThis = nullptr;
const UINT UISetRGB = RegisterWindowMessage(SETRGBSTRING);

BEGIN_MESSAGE_MAP(SVMaskShapeEditorDlg, CDialog)
	//{{AFX_MSG_MAP(SVMaskShapeEditorDlg)
	ON_BN_CLICKED(IDC_BTN_FILL_COLOR_MORE, OnBtnFillColorMore)
	ON_WM_MOUSEMOVE()
	ON_WM_KILLFOCUS()
	ON_CBN_SELCHANGE(IDC_COMBO_SHAPE, OnSelChangeComboShape)
	ON_CBN_SELCHANGE(IDC_COMBO_MASK_OPERATOR, OnSelChangeComboMaskOperator)
	ON_CBN_SELCHANGE(IDC_COMBO_MASK_AREA, OnSelChangeComboMaskArea)
	ON_CBN_SELCHANGE(IDC_COMBO_FILL_OPTIONS, OnSelChangeComboFillOptions)
	ON_WM_ACTIVATE()
	ON_EN_CHANGE(IDC_EDIT_FILL_COLOR, OnChangeEditFillColor)
	ON_BN_CLICKED(IDC_CHECK_AUTO_RESIZE, OnCheckAutoResize)
	ON_BN_CLICKED(IDC_CHECK_CONT_RECALC, OnCheckContRecalc)
	ON_NOTIFY(PTN_ITEMCHANGED, IDC_PROPERTIES, OnItemChanged)
	ON_NOTIFY(PTN_QUERY_SHOW_BUTTON, IDC_PROPERTIES, OnItemQueryShowButton)
	ON_NOTIFY(PTN_ITEMBUTTONCLICK, IDC_PROPERTIES, OnItemButtonClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(SVMaskShapeEditorDlg, CDialog)
	ON_EVENT(SVMaskShapeEditorDlg, IDC_DIALOGIMAGE, 6, SVMaskShapeEditorDlg::MouseMovedImDialogImage, VTS_I4 VTS_I4 VTS_I4)
	ON_EVENT(SVMaskShapeEditorDlg, IDC_DIALOGIMAGE, 8, SVMaskShapeEditorDlg::ObjectChangedExDialogImage, VTS_I4 VTS_I4 VTS_PVARIANT VTS_PVARIANT)
END_EVENTSINK_MAP()
#pragma endregion Declarations

#pragma region Constructor
SVMaskShapeEditorDlg::SVMaskShapeEditorDlg(uint32_t inspectionId, uint32_t taskObjectId, uint32_t maskOperatorId, uint32_t shapeMaskHelperId, CWnd* pParent /*=nullptr*/)
: CDialog(SVMaskShapeEditorDlg::IDD, pParent)
, m_InspectionID{ inspectionId }
, m_TaskObjectID{ taskObjectId }
, m_Values{ SvOg::BoundValues{ inspectionId, maskOperatorId } }
, m_ShapeHelperValues{ SvOg::BoundValues{ inspectionId, shapeMaskHelperId } }
, m_sFillColor( _T( "" ) )
, m_sCoordinates( _T( "" ) )
, m_bAutoResize( FALSE )
, m_isInit( false )
, m_currentTabNumber( 2 ) // BRW - Why is this 2?
, m_eShapeType(SvOp::SVShapeMaskHelperClass::SVMaskShapeTypeInvalid )
, m_maskController {inspectionId, taskObjectId, maskOperatorId}
{
	m_pThis = this;
		
	for (int i = 0; i < m_numberOfTabs; i++)
	{
		m_handleToActiveObjects[i] = -1;
	}
}

SVMaskShapeEditorDlg::~SVMaskShapeEditorDlg()
{
}
#pragma endregion Constructor

#pragma region Public Methods

void SVMaskShapeEditorDlg::CheckPoint()
{
	m_ValuesSaved = m_Values.getCurrentValues();
	m_ShapeHelperValuesSaved = m_ShapeHelperValues.getCurrentValues();
}

void SVMaskShapeEditorDlg::Revert()
{
	m_Values.setValues(m_ValuesSaved);
	m_Values.Commit();
	m_ShapeHelperValues.setValues(m_ShapeHelperValuesSaved);
	m_ShapeHelperValues.Commit();
}

long SVMaskShapeEditorDlg::getSelectedTab() // BRW - This method should be const.
{
	if (m_isInit)
	{
		m_dialogImage.GetSelectedTab(&m_currentTabNumber);
	}
	return m_currentTabNumber;
}

void SVMaskShapeEditorDlg::setSelectedTab(long tabNumber)
{ 
	m_currentTabNumber = tabNumber;
	if (m_isInit)
	{
		m_dialogImage.SelectTab(m_currentTabNumber);
	}
}
#pragma endregion Public Methods

#pragma region Protected Methods
void SVMaskShapeEditorDlg::SetInspectionData()
{
	m_ShapeHelperValues.Commit();

	//This needs to be called due to SetInspectionData being called by UpdateMask which has values set but not committed
	m_Values.Set<bool>(SvPb::ContRecalcEId, m_bContRecalc ? true : false);
	m_Values.Commit(SvOg::PostAction::doRunOnce | SvOg::PostAction::doReset);

	m_ShapeHelperValues.Init();
	RefreshProperties();
}

#pragma region AFX Methods
void SVMaskShapeEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVMaskShapeEditorDlg)
	DDX_Control(pDX, IDC_COMBO_SHAPE, m_cbMaskShape);
	DDX_Control(pDX, IDC_COMBO_MASK_OPERATOR, m_cbMaskOperator);
	DDX_Control(pDX, IDC_COMBO_MASK_AREA, m_cbMaskArea);
	DDX_Control(pDX, IDC_COMBO_FILL_OPTIONS, m_cbFillOptions);
	DDX_Text(pDX, IDC_EDIT_FILL_COLOR, m_sFillColor);
	DDX_Text(pDX, IDC_COORDINATES_STATIC, m_sCoordinates);
	DDX_Control(pDX, IDC_DIALOGIMAGE, m_dialogImage);
	DDX_Check(pDX, IDC_CHECK_CONT_RECALC, m_bContRecalc);
	DDX_Check(pDX, IDC_CHECK_AUTO_RESIZE, m_bAutoResize);
	//}}AFX_DATA_MAP
}

BOOL SVMaskShapeEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_maskController.Init();
	m_Values.Init();
	m_ShapeHelperValues.Init();

	CheckPoint();

	m_bContRecalc = m_Values.Get<bool>(SvPb::ContRecalcEId);
	m_bAutoResize = m_ShapeHelperValues.Get<bool>(SvPb::ShapeMaskAutoResizeEId);


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
	m_Tree.SetColumn( static_cast<long>(rect.Width() * 45. / 100.) );
	m_Tree.ShowInfoText( false );

	/////////////////////////////////////////////////////////////////////////
	// FILL COMBO BOXES

	FillComboBox(m_Values, SvPb::MaskOperatorEId, m_cbMaskOperator);
	FillComboBox(m_Values, SvPb::MaskFillAreaEId, m_cbFillOptions);	// Fill Area

	FillComboBox(m_ShapeHelperValues, SvPb::ShapeMaskMaskAreaEId, m_cbMaskArea);
	FillComboBox(m_ShapeHelperValues, SvPb::ShapeMaskTypeEId, m_cbMaskShape);

	m_sFillColor = SvUl::AsString(m_Values.Get<long>(SvPb::MaskFillColorEId)).c_str();

	m_dialogImage.AddTab(_T("Tool Input"));
	m_dialogImage.AddTab(_T("Mask"));
	m_dialogImage.AddTab(_T("Tool Result"));
	m_dialogImage.SelectTab(m_currentTabNumber);

	SvOp::SVShapeMaskHelperClass::ShapeTypeEnum shapeType = static_cast <SvOp::SVShapeMaskHelperClass::ShapeTypeEnum> (m_ShapeHelperValues.Get<long>(SvPb::ShapeMaskTypeEId));
	assert( shapeType != SvOp::SVShapeMaskHelperClass::SVMaskShapeTypeInvalid );
	setShapeType( shapeType );

	setImages();
	resetShapeOverlay();
	BuildPropertyList();
	m_isInit = true;
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void SVMaskShapeEditorDlg::OnOK()
{
	//!! store combo box values to value objects
	UpdateData();

	int iSel = m_cbMaskShape.GetCurSel();
	long lValue = static_cast<long>( m_cbMaskShape.GetItemData( iSel ) );
	m_ShapeHelperValues.Set<long>(SvPb::ShapeMaskTypeEId, lValue);

	iSel = m_cbMaskArea.GetCurSel();
	lValue = static_cast<long>( m_cbMaskArea.GetItemData( iSel ) );
	m_ShapeHelperValues.Set<long>(SvPb::ShapeMaskMaskAreaEId, lValue);

	lValue = static_cast<long> (m_eShapeType);
	m_ShapeHelperValues.Set<long>(SvPb::ShapeMaskTypeEId, lValue);

	iSel = m_cbMaskOperator.GetCurSel();
	lValue = static_cast<long>( m_cbMaskOperator.GetItemData( iSel ) );
	m_Values.Set<long>(SvPb::MaskOperatorEId, lValue);

	iSel = m_cbFillOptions.GetCurSel();
	lValue = static_cast<long>( m_cbFillOptions.GetItemData( iSel ) );
	m_Values.Set<long>(SvPb::MaskFillAreaEId, lValue);

	lValue = atol(m_sFillColor);
	m_Values.Set<long>(SvPb::MaskFillColorEId, lValue);

	UpdateMask();

	CDialog::OnOK();
}

void SVMaskShapeEditorDlg::OnBtnFillColorMore()
{
	UpdateData();
	CColorDialog dlg;
	dlg.m_cc.Flags |= CC_FULLOPEN;

	dlg.m_cc.Flags |= CC_ENABLETEMPLATE;
	dlg.m_cc.lpTemplateName = MAKEINTRESOURCE(IDD_CHOOSEGRAYCOLOR);
	dlg.m_cc.hInstance = (HWND)::AfxGetResourceHandle();

	dlg.m_cc.Flags |= CC_ENABLEHOOK;
	dlg.m_cc.lpfnHook = ColorDlgHookFn;

	dlg.m_cc.lCustData = reinterpret_cast <LPARAM> (this);

	long lOriginalColor = atol(m_sFillColor);

	if ( dlg.DoModal() == IDOK )
	{
		COLORREF rgb = dlg.GetColor();
		long FillColor = static_cast<long>( GetRValue(rgb) );
		m_sFillColor = SvUl::AsString(FillColor).c_str();
		UpdateData(false);

		m_Values.Set<long>(SvPb::MaskFillColorEId, FillColor);
		UpdateMask();	// need to reset object for shape change
	}
	else
	{
		m_Values.Set<long>(SvPb::MaskFillColorEId, lOriginalColor);
		UpdateMask();	// need to reset object for shape change
	}
}

void SVMaskShapeEditorDlg::OnKillFocus(CWnd* pNewWnd)
{
	CDialog::OnKillFocus(pNewWnd);

	//save selected tab for later request
	m_dialogImage.GetSelectedTab(&m_currentTabNumber);
	m_isInit = false;
}

void SVMaskShapeEditorDlg::OnSelChangeComboShape()
{
	int iSel = m_cbMaskShape.GetCurSel();
	if ( iSel != CB_ERR )
	{
		DWORD_PTR dwType = m_cbMaskShape.GetItemData(iSel);
		setShapeType( static_cast<SvOp::SVShapeMaskHelperClass::ShapeTypeEnum> (dwType) );	// change current shape

		m_ShapeHelperValues.Set<long>(SvPb::ShapeMaskTypeEId, static_cast<long> (m_eShapeType));

		UpdateMask();	// need to reset object for shape change

		setImages();
		resetShapeOverlay();
		BuildPropertyList();
	};
}

void SVMaskShapeEditorDlg::OnSelChangeComboMaskOperator() 
{
	UpdateData();
	int iSel = m_cbMaskOperator.GetCurSel();
	if ( iSel != CB_ERR )
	{
		long lValue = static_cast<long>( m_cbMaskOperator.GetItemData( iSel ) );
		m_Values.Set<long>(SvPb::MaskOperatorEId, lValue);

		UpdateMask();
	}
}

void SVMaskShapeEditorDlg::OnSelChangeComboMaskArea()
{
	UpdateData();
	int iSel = m_cbMaskArea.GetCurSel();
	if ( iSel != CB_ERR )
	{
		long lValue = (long) m_cbMaskArea.GetItemData( iSel );
		m_ShapeHelperValues.Set<long>(SvPb::ShapeMaskMaskAreaEId, lValue);

		UpdateMask();
	}
}

void SVMaskShapeEditorDlg::OnSelChangeComboFillOptions() 
{
	UpdateData();
	int iSel = m_cbFillOptions.GetCurSel();
	if ( iSel != CB_ERR )
	{
		long lValue = static_cast<long>( m_cbFillOptions.GetItemData( iSel ) );
		m_Values.Set<long>(SvPb::MaskFillAreaEId, lValue);

		UpdateMask();
	}
}

void SVMaskShapeEditorDlg::OnChangeEditFillColor()
{
	UpdateData();
	// stuff into value object
	m_Values.Set<long>(SvPb::MaskFillColorEId, atol(m_sFillColor));
	UpdateMask();	// need to reset object for shape change
}

void SVMaskShapeEditorDlg::OnCheckAutoResize()
{
	UpdateData();

	m_ShapeHelperValues.Set<bool>(SvPb::ShapeMaskAutoResizeEId, m_bAutoResize ? true : false);

	UpdateMask();
	RefreshProperties();
}

void SVMaskShapeEditorDlg::OnCheckContRecalc()
{
	UpdateData();
}

void SVMaskShapeEditorDlg::OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	LPNMPROPTREE pNMPropTree = reinterpret_cast< LPNMPROPTREE >(pNotifyStruct);
	*plResult = S_OK;

	if ( pNMPropTree->pItem )
	{
		SVRPropertyItem* pItem = pNMPropTree->pItem;

		// do validation
		int iPropertyID = pItem->GetCtrlID();
		auto embeddedId = GetPropertyEmbeddedId(iPropertyID);

		SVRPropertyItemCombo* pCombo = dynamic_cast <SVRPropertyItemCombo*> (pItem);
		if ( pCombo )
		{
			long lNewValue;
			pCombo->GetItemValue( lNewValue );
			m_ShapeHelperValues.Set<long>(embeddedId, lNewValue);
		}
		else
		{
			auto data = m_ShapeHelperValues.Get<SvOg::LinkedValueData>(embeddedId);
			if (SvPb::LinkedSelectedOption::DirectValue == data.m_selectedOption)
			{
				_variant_t newVal;
				pItem->GetItemValue(newVal);

				std::string Name;
				pItem->GetItemValue(Name);

				bool isValid = (S_OK == ::VariantChangeTypeEx(&data.m_directValue, &newVal, SvDef::LCID_USA, VARIANT_ALPHABOOL, data.m_defaultValue.vt));
				if (false == isValid)
				{
					SvDef::StringVector msgList;
					msgList.push_back(pItem->GetLabelText());
					SvStl::MessageContainer msgContainer;
					msgContainer.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_LinkedValue_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
					SvStl::MessageManager Msg(SvStl::MsgType::Display);
					Msg.setMessage(msgContainer.getMessage());
					*plResult = -1;
					return;
				}

				data.m_Value = data.m_directValue;
				m_ShapeHelperValues.Set<SvOg::LinkedValueData>(embeddedId, data);
			}
		}

		
		bool fail = (S_OK != m_ShapeHelperValues.Commit(SvOg::PostAction::doNothing, true));
		if (fail)
		{
			*plResult = E_FAIL;
			m_ShapeHelperValues.Init();
		}

		pItem->OnRefresh();

		// redraw shape
		UpdateMask();
	}// end if ( pNMPropTree->pItem )
}

void SVMaskShapeEditorDlg::OnItemQueryShowButton(NMHDR*, LRESULT* plResult)
{
	*plResult = TRUE;	// always show button
}

void SVMaskShapeEditorDlg::OnItemButtonClick(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	*plResult = TRUE;

	LPNMPROPTREE pNMPropTree = (LPNMPROPTREE)pNotifyStruct;
	SVRPropertyItemEdit* pEditItem = dynamic_cast<SVRPropertyItemEdit*>(pNMPropTree->pItem);
	if (pEditItem)
	{
		int iPropertyID = pEditItem->GetCtrlID();
		auto embeddedId = GetPropertyEmbeddedId(iPropertyID);

		SvOg::ObjectSelectorData objSelectorData {m_TaskObjectID};
		objSelectorData.m_attribute = SvPb::archivable;
		objSelectorData.m_searchArea = {SvPb::SearchArea::globalConstantItems, SvPb::SearchArea::cameraObject, SvPb::SearchArea::ppqItems, SvPb::SearchArea::toolsetItems};

		auto data = m_ShapeHelperValues.Get<SvOg::LinkedValueData>(embeddedId);

		SvOg::LinkedValueSelectorDialog dlg(m_InspectionID, m_ShapeHelperValues.GetObjectID(embeddedId),
			m_ShapeHelperValues.GetName(embeddedId),
			data,
			data.m_defaultValue.vt,
			objSelectorData);
		if (IDOK == dlg.DoModal())
		{
			m_ShapeHelperValues.Set(embeddedId, dlg.getData());
			setValueColumn(embeddedId, *pEditItem);
		}
	}// end if ( pNMPropTree->pItem )
}
#pragma endregion AFX Methods

void SVMaskShapeEditorDlg::ObjectChangedExDialogImage(long , long , VARIANT* ParameterList, VARIANT* ParameterValue)
{
	////////////////////////////////////////////////////////
	// SET SHAPE PROPERTIES
	VariantParamMap ParaMap;
	SvOg::DisplayHelper::FillParameterMap(ParaMap, ParameterList, ParameterValue);
	bool isResizeable = 0 != (static_cast<int>(ParaMap[CDSVPictureDisplay::P_AllowEdit]) & static_cast<int>(CDSVPictureDisplay::AllowResize));
	bool isMoveable = 0 != (static_cast<int>(ParaMap[CDSVPictureDisplay::P_AllowEdit]) & static_cast<int>(CDSVPictureDisplay::AllowMove));

	if( m_eShapeType != SvOp::SVShapeMaskHelperClass::SVMaskShapeTypeInvalid )
	{
		long width = ParaMap[ CDSVPictureDisplay::P_X2 ].lVal - ParaMap[ CDSVPictureDisplay::P_X1 ].lVal;
		long Height = ParaMap[ CDSVPictureDisplay::P_Y2 ].lVal - ParaMap[ CDSVPictureDisplay::P_Y1 ].lVal;
		
		if (isResizeable)
		{
			m_ShapeHelperValues.Set(SvPb::WidthEId, width);
			m_ShapeHelperValues.Set(SvPb::HeightEId, Height);
		}
		if (isMoveable)
		{
			long CenterX = ParaMap[CDSVPictureDisplay::P_X1].lVal + width / 2;
			long CenterY = ParaMap[CDSVPictureDisplay::P_Y1].lVal + Height / 2;
			m_ShapeHelperValues.Set(SvPb::CenterXEId, CenterX);
			m_ShapeHelperValues.Set(SvPb::CenterYEId, CenterY);
		}
	}

	if( m_eShapeType == SvOp::SVShapeMaskHelperClass::SVMaskShapeTypeSymmetricTrapezoid )
	{
		if (isResizeable)
		{
			m_ShapeHelperValues.Set(SvPb::ShapeMaskPropertyOffsetEId, ParaMap[CDSVPictureDisplay::P_Offset].lVal);
		}

		switch( ParaMap[ CDSVPictureDisplay::P_SubType ].lVal )
		{
		case CDSVPictureDisplay::VerticalAxisTop:
			m_ShapeHelperValues.Set<long>(SvPb::ShapeMaskPropertySymmetryOrientationEId, SvOp::SVMaskShapeSymmetricTrapezoid::VerticalAxisTop);
			break;

		case CDSVPictureDisplay::VerticalAxisBottom:
			m_ShapeHelperValues.Set<long>(SvPb::ShapeMaskPropertySymmetryOrientationEId, SvOp::SVMaskShapeSymmetricTrapezoid::VerticalAxisBottom);
			break;

		case CDSVPictureDisplay::HorizontalAxisLeft:
			m_ShapeHelperValues.Set<long>(SvPb::ShapeMaskPropertySymmetryOrientationEId, SvOp::SVMaskShapeSymmetricTrapezoid::HorizontalAxisLeft);
			break;

		case CDSVPictureDisplay::HorizontalAxisRight:
			m_ShapeHelperValues.Set<long>(SvPb::ShapeMaskPropertySymmetryOrientationEId, SvOp::SVMaskShapeSymmetricTrapezoid::HorizontalAxisRight);
			break;

		default:
			break;
		}
	}

	if( m_eShapeType == SvOp::SVShapeMaskHelperClass::SVMaskShapeTypeDoughnut && isResizeable)
	{
		m_ShapeHelperValues.Set(SvPb::ShapeMaskPropertySideThicknessEId, ParaMap[CDSVPictureDisplay::P_SideThickness].lVal);
		m_ShapeHelperValues.Set(SvPb::ShapeMaskPropertyTopBottomThicknessEId, ParaMap[CDSVPictureDisplay::P_TopThickness].lVal);
	}

	m_ShapeHelperValues.Commit(SvOg::PostAction::doReset);
	RefreshProperties();
	UpdateMask();
}

void SVMaskShapeEditorDlg::MouseMovedImDialogImage(long , long X, long Y)
{
	m_sCoordinates.Format( _T("%ld, %ld"), X, Y );
	UpdateData(FALSE);
}

UINT_PTR CALLBACK SVMaskShapeEditorDlg::ColorDlgHookFn( HWND hdlg, UINT uiMsg, WPARAM, LPARAM lParam )
{
	int iReturnCode = 0;	// by default allow color dlg to process message

#if defined (TRACE_THEM_ALL) || defined (TRACE_OTHER)
	TRACE("ColorDlgHookFn - MSG: %08X, LPARAM: %08X\n", uiMsg, lParam);
#endif

	CWnd* pwndTmp = CWnd::FromHandle(hdlg);
	assert(pwndTmp);
	CColorDialog* pDlg = dynamic_cast< CColorDialog* >( pwndTmp );
	switch (uiMsg)
	{
	case WM_INITDIALOG:
		{
			if ( pwndTmp )
			{
				pwndTmp->GetDlgItem(COLOR_RAINBOW)->ShowWindow(SW_HIDE);
				pwndTmp->GetDlgItem(COLOR_BOX1)->ShowWindow(SW_HIDE);
				pwndTmp->GetDlgItem(COLOR_CUSTOM1)->ShowWindow(SW_HIDE);
				pwndTmp->GetDlgItem(COLOR_SOLID)->ShowWindow(SW_HIDE);

				CHOOSECOLOR* pColor = reinterpret_cast <CHOOSECOLOR*> (lParam);
				SVMaskShapeEditorDlg* pThis = reinterpret_cast <SVMaskShapeEditorDlg*> (pColor->lCustData);
				long lColor = atoi(pThis->m_sFillColor);
				
				::SendMessage( hdlg, UISetRGB, 0, static_cast< LPARAM >( RGB(lColor, lColor, lColor) ));
			}
			iReturnCode = 1;	// don't have color dlg handle this again (it has already handled it)
			break;
		}

	case WM_PAINT:
		{
			CWnd* pWndRed = pDlg->GetDlgItem(COLOR_RED);
			assert( pWndRed );
			if ( pWndRed )
			{
				CString sText;
				pWndRed->GetWindowText( sText );
				m_pThis->GetValues().Set<long>(SvPb::MaskFillColorEId, atol(sText));
				m_pThis->UpdateMask();
			}
			break;
		}

	default:
		{
			break;
		}
	}

	return iReturnCode;
}
#pragma endregion Protected Methods

#pragma region Private Methods
unsigned int SVMaskShapeEditorDlg::GetPropertyID(SvPb::EmbeddedIdEnum embeddedId)
{
	auto iter = std::find_if(m_propertyIds.begin(), m_propertyIds.end(), [embeddedId](const auto& rPair) { return embeddedId == rPair; });
	if (m_propertyIds.end() == iter)
	{
		m_propertyIds.push_back(embeddedId);
		//The new id is pos+1 in the list. 
		return static_cast<unsigned int>(m_propertyIds.size());
	}

	return static_cast<unsigned int>(std::distance(m_propertyIds.begin(), iter) + 1);
}

SvPb::EmbeddedIdEnum SVMaskShapeEditorDlg::GetPropertyEmbeddedId(int iPropertyID)
{
	if (0 < iPropertyID && iPropertyID <= m_propertyIds.size())
	{
		return m_propertyIds[iPropertyID-1];
	}

	return SvPb::NoEmbeddedId;
}

HRESULT SVMaskShapeEditorDlg::BuildPropertyList()
{
	m_Tree.DeleteAllItems();

	SVRPropertyItem* pRoot = m_Tree.InsertItem(new SVRPropertyItem());
	assert( pRoot );
	pRoot->SetCanShrink(false);
	pRoot->SetInfoText(_T(""));
	pRoot->HideItem();
	pRoot->SetHeight(2); //@TODO:  Document the reason for the use of the magic number 2.

	//first = embedded, second = true means LinkedValue, false not LinkedValue
	std::initializer_list<SvPb::EmbeddedIdEnum> baseEmbeddedIdList = {
	SvPb::CenterXEId,
	SvPb::CenterYEId,
	SvPb::WidthEId,
	SvPb::HeightEId };
	for (const auto& embeddedIdPair : baseEmbeddedIdList)//propertiesListMap[m_eShapeType])
	{
		setPropertyToList(embeddedIdPair, pRoot);
	}
	std::map<SvOp::SVShapeMaskHelperClass::ShapeTypeEnum, std::initializer_list<SvPb::EmbeddedIdEnum>> propertiesListMap =
	{
		{SvOp::SVShapeMaskHelperClass::SVMaskShapeTypeDoughnut, {SvPb::ShapeMaskPropertySideThicknessEId, SvPb::ShapeMaskPropertyTopBottomThicknessEId}},
		{SvOp::SVShapeMaskHelperClass::SVMaskShapeTypeSymmetricTrapezoid, {SvPb::ShapeMaskPropertyOffsetEId, SvPb::ShapeMaskPropertySymmetryOrientationEId}}
	};
	for (const auto& embeddedIdPair : propertiesListMap[m_eShapeType])
	{
		setPropertyToList(embeddedIdPair, pRoot);
	}

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

void SVMaskShapeEditorDlg::setPropertyToList(SvPb::EmbeddedIdEnum embeddedId, SVRPropertyItem* pRoot)
{
	std::string Name{ m_ShapeHelperValues.GetName(embeddedId) };
	if (false == Name.empty())
	{
		SVRPropertyItem* pItem = nullptr;
		if (m_ShapeHelperValues.isLinkedValue(embeddedId))
		{
			SVRPropertyItemEdit* pEdit = dynamic_cast <SVRPropertyItemEdit*> (m_Tree.InsertItem(new SVRPropertyItemEdit(), pRoot));
			pItem = pEdit;
			assert(pEdit);
			setValueColumn(embeddedId, *pEdit);
		}
		else
		{
			auto enumList = m_ShapeHelperValues.GetEnumTypes(embeddedId);
			if (0 < enumList.size())
			{
				SVRPropertyItemCombo* pCombo = dynamic_cast <SVRPropertyItemCombo*> (m_Tree.InsertItem(new SVRPropertyItemCombo, pRoot));
				pItem = pCombo;

				pCombo->EnableButton(false);
				pCombo->CreateComboBox(WS_CHILD | WS_VSCROLL | CBS_DROPDOWNLIST);

				for (auto const& rEntry : enumList)
				{
					int iPos = pCombo->AddString(rEntry.first.c_str());
					pCombo->SetItemData(iPos, static_cast<DWORD_PTR> (rEntry.second));
				}
				long value = m_ShapeHelperValues.Get<long>(embeddedId);
				pCombo->SetItemValue(value);
			}
			else
			{
				assert(false);
			}
		}

		assert(pItem);

		pItem->SetCtrlID(GetPropertyID(embeddedId));
		pItem->SetBold(false);
		pItem->SetHeight(16); //@TODO:  Document the reason for the use of the magic number 16.
		pItem->SetLabelText(Name.c_str());

		pItem->OnRefresh();
	}
}

HRESULT SVMaskShapeEditorDlg::RefreshProperties()
{
	SVRPropertyItem* pRoot = m_Tree.GetRootItem()->GetChild();
	assert( pRoot );
	SVRPropertyItem* pChild = pRoot->GetChild();

	while ( pChild )
	{
		auto embeddedId = GetPropertyEmbeddedId(pChild->GetCtrlID());
		const std::initializer_list<SvPb::EmbeddedIdEnum> availableWithAutoResize { SvPb::ShapeMaskPropertyOffsetEId, SvPb::ShapeMaskPropertySymmetryOrientationEId, SvPb::ShapeMaskPropertySideThicknessEId, SvPb::ShapeMaskPropertyTopBottomThicknessEId };
		if (m_bAutoResize && std::none_of(availableWithAutoResize.begin(), availableWithAutoResize.end(), [embeddedId](auto value) { return embeddedId == value; }))
		{
			pChild->SetItemValue( CString(_T("---")) );
			// grey out property
			pChild->SetCanHighlight( false );
			pChild->SetForeColor( SvDef::LightGray );
			pChild->SetBackColorReadOnly(false, SvDef::WhiteSmoke);
			pChild->ReadOnly( true );
		}
		else
		{
			// enable property
			pChild->SetCanHighlight( true );
			pChild->SetForeColor( SvDef::Black );
			if (SVRPropertyItemCombo* pCombo = dynamic_cast <SVRPropertyItemCombo*> (pChild))
			{
				pCombo->SetItemValue(m_ShapeHelperValues.Get<long>(embeddedId));
				pChild->ReadOnly(false);
			}
			else if (SVRPropertyItemEdit* pEdit = dynamic_cast <SVRPropertyItemEdit*> (pChild))
			{
				setValueColumn(embeddedId, *pEdit);
			}
			else
			{
				assert(false);
			}
		}

		pChild = pChild->GetSibling();
	}

	m_Tree.RedrawWindow();

	return S_OK;
}

void SVMaskShapeEditorDlg::UpdateMask()
{
	SetInspectionData();
	setImages();

	resetShapeOverlay();
}

void SVMaskShapeEditorDlg::FillComboBox(const SvOg::ValueController& rValueController, SvPb::EmbeddedIdEnum embeddedId, CComboBox& rCombo)
{
	for (auto const& rEntry : rValueController.GetEnumTypes(embeddedId))
	{
		rCombo.SetItemData(rCombo.AddString(rEntry.first.c_str()), static_cast<DWORD_PTR> (rEntry.second));
	}

	// Set Current Value...
	long lCurrent = rValueController.Get<long>(embeddedId);
	for (int i = 0; i < rCombo.GetCount(); ++i)
	{
		if (lCurrent == static_cast<long> (rCombo.GetItemData(i)))
		{
			rCombo.SetCurSel(i);
			break;
		}
	}
}

void SVMaskShapeEditorDlg::setImages()
{
	int tabIndex = 0;
	IPictureDisp* pSourceImage = m_maskController.GetReferenceImage();
	IPictureDisp* pMaskImage = m_maskController.GetMaskImage();
	IPictureDisp* pResultImage = m_maskController.GetResultImage();

	m_dialogImage.setImage(pSourceImage, tabIndex);
	tabIndex++;
	m_dialogImage.setImage(pMaskImage, tabIndex);
	tabIndex++;
	m_dialogImage.setImage(pResultImage, tabIndex);

	m_dialogImage.Refresh();
}

void SVMaskShapeEditorDlg::setShapeType(SvOp::SVShapeMaskHelperClass::ShapeTypeEnum shapeType)
{
	if (shapeType != m_eShapeType)
	{
		//delete old overlay-objects from activeX-control
		for (int i = 0; i < m_numberOfTabs; i++)
		{
			if ( -1 < m_handleToActiveObjects[i] )
			{
				m_dialogImage.RemoveOverlay(i, m_handleToActiveObjects[i]);
				m_handleToActiveObjects[i] = -1;
			}
		}
		//set type
		if (SvOp::SVShapeMaskHelperClass::SVMaskShapeTypeInvalid != shapeType )
		{
			m_eShapeType = shapeType;
		}
		else
		{
			m_eShapeType = SvOp::SVShapeMaskHelperClass::SVMaskShapeTypeRectangle;
		}

		CRect rect = getRect();

		LongParamMap ParMap;
		ParMap[ CDSVPictureDisplay::P_X1 ] = rect.left; //@TODO:  Consider making a method that takes a rect as a parameter, and moving these lines to that method.
		ParMap[ CDSVPictureDisplay::P_Y1 ] = rect.top;
		ParMap[ CDSVPictureDisplay::P_X2 ] = rect.right;
		ParMap[ CDSVPictureDisplay::P_Y2 ] = rect.bottom;
		ParMap[ CDSVPictureDisplay::P_Color ] = SvDef::Green;
		ParMap[ CDSVPictureDisplay::P_SelectedColor ] = SvDef::Green;
		ParMap[ CDSVPictureDisplay::P_AllowEdit ] = getDrawChangeType();

		//add new shape types
		switch (m_eShapeType)
		{
		case SvOp::SVShapeMaskHelperClass::SVMaskShapeTypeRectangle:
			ParMap[ CDSVPictureDisplay::P_Type ] = CDSVPictureDisplay::RectangleROI;
			break;
		case SvOp::SVShapeMaskHelperClass::SVMaskShapeTypeOval:
			ParMap[ CDSVPictureDisplay::P_Type ] = CDSVPictureDisplay::EllipseROI;
			break;
		case SvOp::SVShapeMaskHelperClass::SVMaskShapeTypeSymmetricTrapezoid:
			ParMap[ CDSVPictureDisplay::P_Type ] = CDSVPictureDisplay::TrapezoidROI;
			ParMap[ CDSVPictureDisplay::P_SubType ] = CDSVPictureDisplay::VerticalAxisTop;
			ParMap[ CDSVPictureDisplay::P_Offset ] = 10;
			break;
		case SvOp::SVShapeMaskHelperClass::SVMaskShapeTypeDoughnut:
			ParMap[ CDSVPictureDisplay::P_Type ] = CDSVPictureDisplay::DoughnutROI;
			ParMap[ CDSVPictureDisplay::P_SideThickness ] = 10;
			ParMap[ CDSVPictureDisplay::P_TopThickness ] = 20;
			break;
		default:
			ParMap[ CDSVPictureDisplay::P_Type ] = CDSVPictureDisplay::InvalidROIType; 
			break; // Do nothing.
		}

		COleSafeArray saPar, saVal;
		SvOg::DisplayHelper::CreateSafeArrayFromMap( ParMap, saPar, saVal );

		for (int i = 0; i < m_numberOfTabs; i++)
		{
			m_dialogImage.AddOverlay( i, static_cast< LPVARIANT >( saPar ), static_cast< LPVARIANT >( saVal ), &(m_handleToActiveObjects[ i ] ) );
			m_dialogImage.SetEditAllow(i, m_handleToActiveObjects[i], getDrawChangeType());
		}
		saPar.Destroy();
		saVal.Destroy();
	}
}

void SVMaskShapeEditorDlg::resetShapeOverlay()
{
	CRect rect = getRect();

	//add new shape types
	std::map<long,long> ParMap;
	ParMap[ CDSVPictureDisplay::P_Type ] = 0;
	ParMap[ CDSVPictureDisplay::P_X1 ] = rect.left;
	ParMap[ CDSVPictureDisplay::P_Y1 ] = rect.top;
	ParMap[ CDSVPictureDisplay::P_X2 ] = rect.right;
	ParMap[ CDSVPictureDisplay::P_Y2 ] = rect.bottom;
	ParMap[ CDSVPictureDisplay::P_Color ] = SvDef::Green;
	ParMap[ CDSVPictureDisplay::P_SelectedColor ] = SvDef::Green;
	ParMap[ CDSVPictureDisplay::P_AllowEdit ] = getDrawChangeType();

	//add new shape types
	switch (m_eShapeType)
	{
	case SvOp::SVShapeMaskHelperClass::SVMaskShapeTypeRectangle:
		ParMap[ CDSVPictureDisplay::P_Type ] = CDSVPictureDisplay::RectangleROI;
		break;
	case SvOp::SVShapeMaskHelperClass::SVMaskShapeTypeOval:
		ParMap[ CDSVPictureDisplay::P_Type ] = CDSVPictureDisplay::EllipseROI;
		break;
	case SvOp::SVShapeMaskHelperClass::SVMaskShapeTypeSymmetricTrapezoid:
		ParMap[ CDSVPictureDisplay::P_Type ] = CDSVPictureDisplay::TrapezoidROI;

		switch( m_ShapeHelperValues.Get<long>(SvPb::ShapeMaskPropertySymmetryOrientationEId) )
		{
		case SvOp::SVMaskShapeSymmetricTrapezoid::VerticalAxisTop:
			ParMap[ CDSVPictureDisplay::P_SubType ] = CDSVPictureDisplay::VerticalAxisTop;
			break;
		case SvOp::SVMaskShapeSymmetricTrapezoid::VerticalAxisBottom:
			ParMap[ CDSVPictureDisplay::P_SubType ] = CDSVPictureDisplay::VerticalAxisBottom;
			break;
		case SvOp::SVMaskShapeSymmetricTrapezoid::HorizontalAxisLeft:
			ParMap[ CDSVPictureDisplay::P_SubType ] = CDSVPictureDisplay::HorizontalAxisLeft;
			break;
		case SvOp::SVMaskShapeSymmetricTrapezoid::HorizontalAxisRight:
			ParMap[ CDSVPictureDisplay::P_SubType ] = CDSVPictureDisplay::HorizontalAxisRight;
			break;
		default:
			break; // Do nothing.
		}
		ParMap[ CDSVPictureDisplay::P_Offset ] = m_ShapeHelperValues.Get<SvOg::LinkedValueData>(SvPb::ShapeMaskPropertyOffsetEId).m_Value;
		break;
	case SvOp::SVShapeMaskHelperClass::SVMaskShapeTypeDoughnut:
		ParMap[ CDSVPictureDisplay::P_Type ] = CDSVPictureDisplay::DoughnutROI;
		ParMap[ CDSVPictureDisplay::P_SideThickness ] = m_ShapeHelperValues.Get<SvOg::LinkedValueData>(SvPb::ShapeMaskPropertySideThicknessEId).m_Value;
		ParMap[ CDSVPictureDisplay::P_TopThickness ] = m_ShapeHelperValues.Get<SvOg::LinkedValueData>(SvPb::ShapeMaskPropertyTopBottomThicknessEId).m_Value;
		break;
	default:
		break; // Do nothing.
	}

	COleSafeArray saPar, saVal;
	SvOg::DisplayHelper::CreateSafeArrayFromMap( ParMap, saPar, saVal );
	for (int i = 0; i < m_numberOfTabs; i++)
	{
		m_dialogImage.EditOverlay( i, m_handleToActiveObjects[i], static_cast< LPVARIANT >( saPar ), static_cast< LPVARIANT >( saVal ) );
		m_dialogImage.SetEditAllow(i, m_handleToActiveObjects[i], getDrawChangeType());
	}
	saVal.Destroy();
	saPar.Destroy();
}

RECT SVMaskShapeEditorDlg::getRect() const
{
	if (m_bAutoResize)
	{
		return m_maskController.GetResultRect();
	}
	else
	{
		long lCenterX = m_ShapeHelperValues.Get<SvOg::LinkedValueData>(SvPb::CenterXEId).m_Value;
		long lCenterY = m_ShapeHelperValues.Get<SvOg::LinkedValueData>(SvPb::CenterYEId).m_Value;
		long lWidth = m_ShapeHelperValues.Get<SvOg::LinkedValueData>(SvPb::WidthEId).m_Value;
		long lHeight = m_ShapeHelperValues.Get<SvOg::LinkedValueData>(SvPb::HeightEId).m_Value;
		long lHalfWidth = lWidth / 2;
		long lHalfHeight = lHeight / 2;

		long lTop = lCenterY - lHalfHeight;
		long lLeft = lCenterX - lHalfWidth;
		long lRight = lLeft + lWidth;
		long lBottom = lTop + lHeight;

		RECT rect{ lLeft, lTop, lRight, lBottom };
		return rect;
	}
}

CDSVPictureDisplay::AllowType SVMaskShapeEditorDlg::getDrawChangeType() const
{
	if (m_bAutoResize)
	{
		return CDSVPictureDisplay::AllowNone;
	}

	//If string empty it is a direct value and it is allowed to change it
	bool bMove = isChangable(SvPb::CenterXEId) && isChangable(SvPb::CenterYEId);

	bool bAll = bMove && isChangable(SvPb::WidthEId) && isChangable(SvPb::HeightEId);

	switch (m_eShapeType)
	{
	case SvOp::SVShapeMaskHelperClass::SVMaskShapeTypeDoughnut:
		bAll &= isChangable(SvPb::ShapeMaskPropertySideThicknessEId) &&	isChangable(SvPb::ShapeMaskPropertyTopBottomThicknessEId);
		break;
	case SvOp::SVShapeMaskHelperClass::SVMaskShapeTypeSymmetricTrapezoid:
		bAll &= isChangable(SvPb::ShapeMaskPropertyOffsetEId);
		break;
	default: //nothing to do
		break;
	}

	CDSVPictureDisplay::AllowType type = bAll ? CDSVPictureDisplay::AllowResizeAndMove : bMove ? CDSVPictureDisplay::AllowMove : CDSVPictureDisplay::AllowNone;
	return type;
}

void SVMaskShapeEditorDlg::setValueColumn(SvPb::EmbeddedIdEnum embeddedId, SVRPropertyItemEdit& rEdit)
{
	auto data = m_ShapeHelperValues.Get<SvOg::LinkedValueData>(embeddedId);
	CString valueString = SvUl::VariantToString(data.m_Value).c_str();

	rEdit.SetBackColorReadOnly(false, SvDef::WhiteSmoke);
	rEdit.SetItemValue(valueString);
	rEdit.ReadOnly(SvPb::LinkedSelectedOption::DirectValue != data.m_selectedOption, true);
	rEdit.OnRefresh();
	m_Tree.UpdatedItems();
}

bool SVMaskShapeEditorDlg::isChangable(SvPb::EmbeddedIdEnum embeddedId) const
{
	auto data = m_ShapeHelperValues.Get<SvOg::LinkedValueData>(embeddedId);
	return SvPb::LinkedSelectedOption::DirectValue == data.m_selectedOption;
}
#pragma endregion Private Methods

