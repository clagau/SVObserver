//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
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
#include <colordlg.h>	// for custom color dlg resource #defines
#include "SVMaskShapeEditorDlg.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVMaskShape.h"
#include "SVObserver.h"
#include "SVTool.h"
#include "SVIPDoc.h"
#include "SVOMFCLibrary/stringmunge.h"
#include "SVLibrary/SVWinUtility.h"	// for SVYieldPaintMessages
#include "SVUserMaskOperatorClass.h"
#include "SVOMFCLibrary/DisplayHelper.h"
#include "SVImageLibrary/MatroxImageData.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace Seidenader::SVOMFCLibrary;

/////////////////////////////////////////////////////////////////////////////
// SVMaskShapeEditorDlg dialog

SVMaskShapeEditorDlg* SVMaskShapeEditorDlg::m_pThis = nullptr;

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
	ON_NOTIFY(PTN_ITEMCHANGED, IDC_PROPERTIES, OnItemChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(SVMaskShapeEditorDlg, CDialog)
	ON_EVENT(SVMaskShapeEditorDlg, IDC_DIALOGIMAGE, 6, SVMaskShapeEditorDlg::MouseMovedImDialogImage, VTS_I4 VTS_I4 VTS_I4)
	ON_EVENT(SVMaskShapeEditorDlg, IDC_DIALOGIMAGE, 8, SVMaskShapeEditorDlg::ObjectChangedExDialogImage, VTS_I4 VTS_I4 VTS_PVARIANT VTS_PVARIANT)
END_EVENTSINK_MAP()
/////////////////////////////////////////////////////////////////////////////
// SVMaskShapeEditorDlg message handlers
namespace	// file local
{
	static int ID_BASE = 1000;
}
#pragma endregion Declarations

#pragma region Constructor
SVMaskShapeEditorDlg::SVMaskShapeEditorDlg( SVToolClass* pTool, SVUserMaskOperatorClass* pMask, CWnd* pParent /*=NULL*/)
: CDialog(SVMaskShapeEditorDlg::IDD, pParent)
, m_sFillColor( _T( "" ) )
, m_sCoordinates( _T( "" ) )
, m_bAutoResize( FALSE )
, m_pTool( pTool )
, m_pMask( pMask )
, m_isInit( false )
, m_currentTabNumber( 2 ) // BRW - Why is this 2?
, m_eShapeType( SVShapeMaskHelperClass::SVMaskShapeTypeInvalid )
{
	m_pThis = this;

	for (int i = 0; i < m_numberOfTabs; i++)
	{
		m_handleToActiveObjects[i] = -1;
	}
}

SVMaskShapeEditorDlg::~SVMaskShapeEditorDlg()
{
	ShapeMap::iterator itShapeMapIter;
	for ( itShapeMapIter = m_mapShapes.begin(); itShapeMapIter != m_mapShapes.end(); ++itShapeMapIter )
	{
		if (nullptr != itShapeMapIter->second)
		{
			delete itShapeMapIter->second;
			itShapeMapIter->second = nullptr;
		}
	} // end for loop
}
#pragma endregion Constructor

#pragma region Public Methods
HRESULT SVMaskShapeEditorDlg::GetCancelData(SVInputRequestStructMap& rMap)
{
	ASSERT( m_pMask );
	return m_pMask->GetCancelData( rMap );
}

HRESULT SVMaskShapeEditorDlg::SetInspectionData()
{
	SVMaskShape::MapType mapProperties;
	GetCurrentShape()->GetProperties(mapProperties);

	SVInputRequestStructMap mapData;

	SVMaskShape::MapType::iterator iter;
	for ( iter = mapProperties.begin(); iter != mapProperties.end(); ++iter )
	{
		GUID guid = iter->first;
		SVValueObjectClass* pValueObject = m_pMask->GetShapeHelper()->GetEmbeddedValueObject( guid );
		if ( pValueObject != nullptr )
		{
			SVValueObjectReference ref( pValueObject );
			mapData[ ref ] = AsString( iter->second.value );
		}
	}

	AddInputRequest( mapData );
	RunOnce( m_pMask->GetTool() );

	return S_OK;
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

SVMaskShape* SVMaskShapeEditorDlg::GetCurrentShape()	// holds the properties and does the rendering // BRW - This method should be const.
{
	return m_mapShapes[m_eShapeType];
}
#pragma endregion Public Methods

#pragma region Protected Methods
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
	DDX_Check(pDX, IDC_CHECK_AUTO_RESIZE, m_bAutoResize);
	//}}AFX_DATA_MAP
}

BOOL SVMaskShapeEditorDlg::OnInitDialog()
{
	// must set these before calling DoModal
	ASSERT( m_pTool != nullptr );
	ASSERT( m_pMask != nullptr );

	// SVTaskObjectInterfaceClass requirement
	SetTaskObject( m_pTool );

	bool l_bAutoResize = m_pMask->GetShapeHelper()->IsAutoResize();
	m_bAutoResize = l_bAutoResize;

	CDialog::OnInitDialog();

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

	FillComboBox(m_pMask->m_Data.evoCurrentMaskOperator, &m_cbMaskOperator);
	FillComboBox(m_pMask->m_Data.evoFillArea, &m_cbFillOptions);	// Fill Area

	SVShapeMaskHelperClass* pShapeMaskHelper = dynamic_cast <SVShapeMaskHelperClass*> 
		( SVObjectManagerClass::Instance().GetObject(m_pMask->m_guidShapeHelper) );

	ASSERT( pShapeMaskHelper != NULL );

	if ( pShapeMaskHelper )
	{
		FillComboBox(pShapeMaskHelper->m_Data.evoMaskArea, &m_cbMaskArea);
		FillComboBox(pShapeMaskHelper->m_Data.evoShapeType, &m_cbMaskShape);
	}

	long lColor=0;
	m_pMask->m_Data.lvoFillColor.GetValue(lColor);
	m_sFillColor = AsString(lColor);

	m_dialogImage.AddTab(_T("Tool Input"));
	m_dialogImage.AddTab(_T("Mask"));
	m_dialogImage.AddTab(_T("Tool Result"));
	m_dialogImage.SelectTab(m_currentTabNumber);

 	m_mapShapes[ SVShapeMaskHelperClass::SVMaskShapeTypeRectangle ] = new SVMaskShapeRectangle;
	m_mapShapes[ SVShapeMaskHelperClass::SVMaskShapeTypeOval ] = new SVMaskShapeOval;
	m_mapShapes[ SVShapeMaskHelperClass::SVMaskShapeTypeDoughnut ] = new SVMaskShapeDoughnut;
	m_mapShapes[ SVShapeMaskHelperClass::SVMaskShapeTypeSymmetricTrapezoid ] = new SVMaskShapeSymmetricTrapezoid;

	long lShapeType;
	pShapeMaskHelper->m_Data.evoShapeType.GetValue(lShapeType);
	SVShapeMaskHelperClass::ShapeTypeEnum shapeType = static_cast <SVShapeMaskHelperClass::ShapeTypeEnum> (lShapeType);
	ASSERT( shapeType != SVShapeMaskHelperClass::SVMaskShapeTypeInvalid );
	setShapeType( shapeType );

	// set image info for all shapes
	ShapeMap::iterator itShapeMapIter;
	for ( itShapeMapIter = m_mapShapes.begin(); itShapeMapIter != m_mapShapes.end(); ++itShapeMapIter )
	{
		itShapeMapIter->second->SetImageInfo(m_pMask->m_MaskBufferInfo);
	} // end for loop

	////////////////////////////////////////////////////////
	// SET SHAPE PROPERTIES
	SVMaskShape* pShape = m_mapShapes[ m_eShapeType ];
	SVMaskShape::MapType mapProperties;
	m_pMask->GetShapeHelper()->GetProperties( mapProperties );
	pShape->SetProperties( mapProperties );

	SVMaskFillPropertiesStruct svFillProperties;
	m_pMask->GetFillProperties( svFillProperties );
	pShape->SetFillProperties( svFillProperties );

	pShape->SetAutoResize( l_bAutoResize );

	////////////////////////////////////////////////////////

	if( !( m_pMask->m_MaskBufferHandlePtr.empty() ) )
	{
		SVImageBufferHandleImage l_MilHandle;
		m_pMask->m_MaskBufferHandlePtr->GetData( l_MilHandle );

		GetCurrentShape()->Refresh();	// renders based on new properties
		GetCurrentShape()->Draw( l_MilHandle.GetBuffer() );
	}

	setImages();
	resetShapeOverlay();
	BuildPropertyList();
	m_isInit = true;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void SVMaskShapeEditorDlg::OnOK()
{
	//!! store combo box values to value objects
	UpdateData();
	int iSel = CB_ERR;
	long lValue = 0;
	SVShapeMaskHelperClass* pShapeMaskHelper = dynamic_cast <SVShapeMaskHelperClass*> 
		( SVObjectManagerClass::Instance().GetObject(m_pMask->m_guidShapeHelper) );

	iSel = m_cbMaskShape.GetCurSel();
	lValue = static_cast<long>( m_cbMaskShape.GetItemData( iSel ) );
	AddInputRequest( SVValueObjectReference( &pShapeMaskHelper->m_Data.evoShapeType ), AsString(lValue) );

	iSel = m_cbMaskOperator.GetCurSel();
	lValue = static_cast<long>( m_cbMaskOperator.GetItemData( iSel ) );
	AddInputRequest( SVValueObjectReference( &m_pMask->m_Data.evoCurrentMaskOperator ), AsString(lValue) );

	iSel = m_cbMaskArea.GetCurSel();
	lValue = static_cast<long>( m_cbMaskArea.GetItemData( iSel ) );
	AddInputRequest( SVValueObjectReference( &pShapeMaskHelper->m_Data.evoMaskArea ), AsString(lValue) );

	iSel = m_cbFillOptions.GetCurSel();
	lValue = static_cast<long>( m_cbFillOptions.GetItemData( iSel ) );
	AddInputRequest( SVValueObjectReference( &m_pMask->m_Data.evoFillArea ), AsString(lValue) );

	lValue = atol(m_sFillColor);
	AddInputRequest( SVValueObjectReference( &m_pMask->m_Data.lvoFillColor ), AsString(lValue) );

	AddInputRequest( &m_pMask->GetShapeHelper()->m_Data.evoShapeType, AsString(m_eShapeType) );
	SetInspectionData();

	UpdateMask(true);

	CDialog::OnOK();
}

void SVMaskShapeEditorDlg::OnBtnFillColorMore()
{
	UpdateData();
	CColorDialog dlg;
	dlg.m_cc.Flags |= CC_FULLOPEN;

	dlg.m_cc.Flags |= CC_ENABLETEMPLATE;
	dlg.m_cc.lpTemplateName = MAKEINTRESOURCE(IDD_CHOOSEGRAYCOLOR);
	dlg.m_cc.hInstance = (HWND)AfxGetApp()->m_hInstance;

	dlg.m_cc.Flags |= CC_ENABLEHOOK;
	dlg.m_cc.lpfnHook = ColorDlgHookFn;

	dlg.m_cc.lCustData = reinterpret_cast <LPARAM> (this);

	long lOriginalVal = atol(m_sFillColor);

	if ( dlg.DoModal() == IDOK )
	{
		COLORREF rgb = dlg.GetColor();
		long earlGrayTea = static_cast<long>( GetRValue(rgb) );
		m_sFillColor = AsString(earlGrayTea);
		UpdateData(FALSE);

		// stuff into value object
		AddInputRequest( SVValueObjectReference( &m_pMask->m_Data.lvoFillColor ), m_sFillColor );
		UpdateMask();	// need to reset object for shape change
	}
	else
	{
		AddInputRequest( SVValueObjectReference( &m_pMask->m_Data.lvoFillColor ), AsString(lOriginalVal) );
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
		SVMaskShape* pOldShape = GetCurrentShape();
		SVMaskShape::MapType mapOldProperties;
		pOldShape->GetProperties( mapOldProperties );

		DWORD_PTR dwType = m_cbMaskShape.GetItemData(iSel);
		setShapeType( static_cast<SVShapeMaskHelperClass::ShapeTypeEnum> (dwType) );	// change current shape

		GetCurrentShape()->SetProperties( mapOldProperties ); // reuse common properties; any that don't apply will be ignored
		GetCurrentShape()->SetAutoResize( TRUE == m_bAutoResize );
		AddInputRequest( &m_pMask->GetShapeHelper()->m_Data.evoShapeType, AsString(m_eShapeType) );
		SetInspectionData();

		UpdateMask(true);	// need to reset object for shape change

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
		AddInputRequest( SVValueObjectReference( &m_pMask->m_Data.evoCurrentMaskOperator ), AsString(lValue) );

		UpdateMask(true);
		setImages();
	}
}

void SVMaskShapeEditorDlg::OnSelChangeComboMaskArea()
{
	UpdateData();
	int iSel = m_cbMaskArea.GetCurSel();
	if ( iSel != CB_ERR )
	{
		SVShapeMaskHelperClass* pShapeMaskHelper = dynamic_cast <SVShapeMaskHelperClass*> 
			( SVObjectManagerClass::Instance().GetObject(m_pMask->m_guidShapeHelper) );

		long lValue = (long) m_cbMaskArea.GetItemData( iSel );
		AddInputRequest( SVValueObjectReference( &pShapeMaskHelper->m_Data.evoMaskArea ), AsString(lValue) );

		UpdateMask();
		setImages();
	}
}

void SVMaskShapeEditorDlg::OnSelChangeComboFillOptions() 
{
	UpdateData();
	int iSel = m_cbFillOptions.GetCurSel();
	if ( iSel != CB_ERR )
	{
		long lValue = static_cast<long>( m_cbFillOptions.GetItemData( iSel ) );
		AddInputRequest( SVValueObjectReference( &m_pMask->m_Data.evoFillArea ), AsString(lValue) );

		UpdateMask();
	}
}

void SVMaskShapeEditorDlg::OnChangeEditFillColor()
{
	UpdateData();
	// stuff into value object
	AddInputRequest( SVValueObjectReference( &m_pMask->m_Data.lvoFillColor ), m_sFillColor );
	UpdateMask();	// need to reset object for shape change
}

void SVMaskShapeEditorDlg::OnCheckAutoResize()
{
	UpdateData();
	bool bAutoResize = m_bAutoResize ? true : false;
	AddInputRequest( SVValueObjectReference( &m_pMask->GetShapeHelper()->m_Data.bvoAutoResize ), AsString(bAutoResize) );
	GetCurrentShape()->SetAutoResize( bAutoResize ); // reuse common properties; any that don't apply will be ignored

	UpdateMask(true);
	RefreshProperties();
	setImages();
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
		GUID guidProperty = GetPropertyGuid( iPropertyID );

		CString sValue;
		long lNewValue;
		SVRPropertyItemCombo* pCombo = dynamic_cast <SVRPropertyItemCombo*> (pItem);
		if ( pCombo )
		{
			pCombo->GetItemValue( lNewValue );
		}
		else
		{
			pItem->GetItemValue( sValue );
			lNewValue = atol( sValue );
		}
		bool bValidated = true;

		if ( !bValidated )
		{
			*plResult = S_FALSE;
		}
		else
		{
			SVMaskShape::MapType mapProperties;
			GetCurrentShape()->GetProperties(mapProperties);

			long lOldValue = mapProperties[guidProperty].value;

			if ( lOldValue != lNewValue )
			{
				mapProperties[guidProperty] = static_cast< long >( lNewValue );

				GetCurrentShape()->SetProperties( mapProperties );

				GetCurrentShape()->GetProperties(mapProperties);
				long lValue = mapProperties[guidProperty].value;

				SVValueObjectClass* pValueObject = m_pMask->GetShapeHelper()->GetEmbeddedValueObject( guidProperty );
				ASSERT( pValueObject );
				if ( pValueObject )
				{
					AddInputRequest( SVValueObjectReference( pValueObject ), AsString(lValue) );
				}

				if ( pCombo )
				{
					pCombo->SetItemValue( (unsigned long) lValue );
				}
				else
				{
					sValue = AsString(lValue);
					pItem->SetItemValue( sValue );
				}

				pItem->OnRefresh();

				// redraw shape
				UpdateMask();
			}
		}
	}// end if ( pNMPropTree->pItem )
}
#pragma endregion AFX Methods

void SVMaskShapeEditorDlg::ObjectChangedExDialogImage(long Tab, long Handle, VARIANT* ParameterList, VARIANT* ParameterValue)
{
	////////////////////////////////////////////////////////
	// SET SHAPE PROPERTIES
	VariantParamMap ParaMap;
	int count = DisplayHelper::FillParameterMap(ParaMap, ParameterList, ParameterValue);

	SVMaskShape* pShape = m_mapShapes[ m_eShapeType ];
	SVMaskShape::MapType mapProperties;
	m_pMask->GetShapeHelper()->GetProperties( mapProperties );

	if( m_eShapeType !=  SVShapeMaskHelperClass::SVMaskShapeTypeInvalid )
	{
		long width = ParaMap[ CDSVPictureDisplay::P_X2 ].lVal - ParaMap[ CDSVPictureDisplay::P_X1 ].lVal;
		long Height = ParaMap[ CDSVPictureDisplay::P_Y2 ].lVal - ParaMap[ CDSVPictureDisplay::P_Y1 ].lVal;
		long CenterX = ParaMap[ CDSVPictureDisplay::P_X1 ].lVal + width / 2;
		long CenterY = ParaMap[ CDSVPictureDisplay::P_Y1 ].lVal + Height / 2;

		mapProperties[ SVShapeMaskPropertyWidthGuid ].value = width;
		mapProperties[ SVShapeMaskPropertyHeightGuid ].value = Height;
		mapProperties[ SVShapeMaskPropertyCenterXGuid ].value = CenterX;
		mapProperties[ SVShapeMaskPropertyCenterYGuid ].value = CenterY;
	}

	if( m_eShapeType == SVShapeMaskHelperClass::SVMaskShapeTypeSymmetricTrapezoid )
	{
		mapProperties[ SVShapeMaskPropertyOffsetGuid ].value = ParaMap[ CDSVPictureDisplay::P_Offset ].lVal;

		switch( ParaMap[ CDSVPictureDisplay::P_SubType ].lVal )
		{
		case CDSVPictureDisplay::VerticalAxisTop:
			mapProperties[ SVShapeMaskPropertySymmetryOrientationGuid ].value = SVMaskShapeSymmetricTrapezoid::VerticalAxisTop;
			break;

		case CDSVPictureDisplay::VerticalAxisBottom:
			mapProperties[ SVShapeMaskPropertySymmetryOrientationGuid ].value = SVMaskShapeSymmetricTrapezoid::VerticalAxisBottom;
			break;

		case CDSVPictureDisplay::HorizontalAxisLeft:
			mapProperties[ SVShapeMaskPropertySymmetryOrientationGuid ].value = SVMaskShapeSymmetricTrapezoid::HorizontalAxisLeft;
			break;

		case CDSVPictureDisplay::HorizontalAxisRight:
			mapProperties[ SVShapeMaskPropertySymmetryOrientationGuid ].value = SVMaskShapeSymmetricTrapezoid::HorizontalAxisRight;
			break;

		default:
			break;
		}
	}

	if( m_eShapeType == SVShapeMaskHelperClass::SVMaskShapeTypeDoughnut )
	{
		mapProperties[ SVShapeMaskPropertySideThicknessGuid ].value = ParaMap[ CDSVPictureDisplay::P_SideThickness ].lVal;
		mapProperties[ SVShapeMaskPropertyTopBottomThicknessGuid ].value = ParaMap[ CDSVPictureDisplay::P_TopThickness ].lVal;
	}

	m_pMask->GetShapeHelper()->SetProperties( mapProperties );
	pShape->SetProperties( mapProperties );
	m_pMask->ResetObject();
	RefreshProperties();
	UpdateMask(true);
}

void SVMaskShapeEditorDlg::MouseMovedImDialogImage(long Tab, long X, long Y)
{
	m_sCoordinates.Format( _T("%d, %d"), X, Y );
	UpdateData(FALSE);
}

UINT_PTR CALLBACK SVMaskShapeEditorDlg::ColorDlgHookFn( HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	int iReturnCode = 0;	// by default allow color dlg to process message

	TRACE("ColorDlgHookFn - MSG: %08X, WPARAM: %08X, LPARAM: %08X\n", uiMsg, wParam, lParam);

	CWnd* pwndTmp = CWnd::FromHandle(hdlg);
	ASSERT(pwndTmp);
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
				UINT uiSetRGB;
				uiSetRGB = RegisterWindowMessage(SETRGBSTRING);
				::SendMessage( hdlg, uiSetRGB, 0, static_cast< LPARAM >( RGB(lColor, lColor, lColor) ));
			}
			iReturnCode = 1;	// don't have color dlg handle this again (it has already handled it)
			break;
		}

	case WM_PAINT:
		{
			CWnd* pWndRed = pDlg->GetDlgItem(COLOR_RED);
			ASSERT( pWndRed );
			if ( pWndRed )
			{
				CString sText;
				pWndRed->GetWindowText( sText );
				m_pThis->AddInputRequest( SVValueObjectReference( &m_pThis->m_pMask->m_Data.lvoFillColor ), sText );
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
int SVMaskShapeEditorDlg::GetPropertyID(GUID guidProperty )
{
	int id = m_mapPropertyIds[ guidProperty ];
	if ( id == 0 )
	{
		do
		{
			id = (rand() % 15000) + ID_BASE; //@TODO:  Explain the use of rand() and the 15000.
		} while ( GetPropertyGuid(id) != SVInvalidGUID );

		m_mapPropertyIds[ guidProperty ] = id;
	}
	return id;
}

GUID SVMaskShapeEditorDlg::GetPropertyGuid(int iPropertyID )
{
	std::map<GUID, int>::iterator iter;
	for ( iter = m_mapPropertyIds.begin(); iter != m_mapPropertyIds.end(); ++iter )
	{
		if ( iter->second == iPropertyID )
		{
			return iter->first;
		}
	}
	return SVInvalidGUID;
}

HRESULT SVMaskShapeEditorDlg::BuildPropertyList()
{
	m_Tree.DeleteAllItems();

	SVRPropertyItem* pRoot = m_Tree.InsertItem(new SVRPropertyItem());
	ASSERT( pRoot );
	pRoot->SetCanShrink(false);
	pRoot->SetInfoText(_T(""));
	pRoot->HideItem();
	pRoot->SetHeight(2); //@TODO:  Document the reason for the use of the magic number 2.

	SVMaskShape::MapType mapProperties;
	GetCurrentShape()->GetProperties( mapProperties );

	SVMaskShape::SortedMapViewType::const_iterator iter;
	SVMaskShape::SortedMapViewType viewOrderedProperties( SVMaskShape::GetTempSortedPropertyMapView(mapProperties) );

	for( iter = viewOrderedProperties.begin(); iter != viewOrderedProperties.end(); ++iter )
	{
		const SVMaskShape::MapType::key_type& key = (*iter)->first;
		const SVMaskShape::MapType::referent_type& data = (*iter)->second;

		SVValueObjectClass* pValueObject = m_pMask->GetShapeHelper()->GetEmbeddedValueObject( key );
		ASSERT( pValueObject );
		if ( pValueObject )
		{
			CString sName = pValueObject->GetName();

			SVRPropertyItem* pItem = NULL;

			if ( SVEnumerateValueObjectClass* pEnum = dynamic_cast <SVEnumerateValueObjectClass*> ( pValueObject ) )
			{
				SVRPropertyItemCombo* pCombo = dynamic_cast <SVRPropertyItemCombo*> (m_Tree.InsertItem(new SVRPropertyItemCombo, pRoot) );
				pItem = pCombo;

				pCombo->EnableButton( false );
				pCombo->CreateComboBox( WS_CHILD | WS_VSCROLL | CBS_DROPDOWNLIST );

				SVEnumerateVector vecEnums;
				pEnum->GetEnumTypes( vecEnums );
				SVEnumerateVector::const_iterator iter;
				for ( iter = vecEnums.begin(); iter != vecEnums.end(); ++iter )
				{
					int iPos = pCombo->AddString( iter->first );
					pCombo->SetItemData( iPos, iter->second );
				}
				pCombo->SetItemValue( data.value );
			}
			else
			{
				SVRPropertyItemEdit* pEdit = dynamic_cast <SVRPropertyItemEdit*> ( m_Tree.InsertItem(new SVRPropertyItemEdit(), pRoot) );
				pItem = pEdit;

				long lValue = data.value;
				CString sValue = AsString(lValue);
				pEdit->SetItemValue( sValue );
			}

			ASSERT( pItem );

			pItem->SetCtrlID( GetPropertyID(key) );
			pItem->SetBold( false );
			pItem->SetHeight(16); //@TODO:  Document the reason for the use of the magic number 16.
			pItem->SetLabelText( sName );

			pItem->OnRefresh();
		}
	}//end for( iter = map.begin(); iter != map.end(); iter++ )

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

HRESULT SVMaskShapeEditorDlg::RefreshProperties()
{
	SVMaskShape::MapType mapProperties;
	GetCurrentShape()->GetProperties( mapProperties );

	SVRPropertyItem* pRoot = m_Tree.GetRootItem()->GetChild();
	ASSERT( pRoot );
	SVRPropertyItem* pChild = pRoot->GetChild();

	while ( pChild )
	{
		GUID guidProperty = GetPropertyGuid( pChild->GetCtrlID() );
		CString sValue = AsString( mapProperties[ guidProperty ].value );
		if ( SVRPropertyItemCombo* pCombo = dynamic_cast <SVRPropertyItemCombo*> (pChild) )
		{
			unsigned long ulValue = mapProperties[ guidProperty ].value;
			pCombo->SetItemValue( ulValue );
		}
		else
		{
			pChild->SetItemValue( sValue );
		}

		if ( m_bAutoResize && ! mapProperties[ guidProperty ].bAvailableWithAutoResize )
		{
			pChild->SetItemValue( CString(_T("---")) );
			// grey out property
			pChild->SetCanHighlight( false );
			pChild->SetForeColor( SVColor::LightGray );
			pChild->ReadOnly( true );
		}
		else
		{
			// enable property
			pChild->SetCanHighlight( true );
			pChild->SetForeColor( SVColor::Black );
			pChild->ReadOnly( false );
		}

		pChild = pChild->GetSibling();
	}

	m_Tree.RedrawWindow();

	return S_OK;
}

HRESULT SVMaskShapeEditorDlg::UpdateMask(bool bResetObject)
{
	GetCurrentShape()->Refresh(); // renders based on new properties
	SetInspectionData();

	setImages();

	resetShapeOverlay();
	return S_OK;
}

void SVMaskShapeEditorDlg::FillComboBox(SVEnumerateValueObjectClass& p_rValueObject, CComboBox* p_pCombo)
{
	ASSERT( p_pCombo != NULL );

	if ( p_pCombo )
	{
		int i;
		SVEnumerateVector vec;
		p_rValueObject.GetEnumTypes( vec );
		for ( i = 0; i < static_cast<int>(vec.size()); i++ )
		{
			p_pCombo->SetItemData( p_pCombo->AddString(vec[i].first), vec[i].second );
		}

		// Set Current Value...
		long lCurrent;
		p_rValueObject.GetValue(lCurrent);

		for( i = 0; i < p_pCombo->GetCount(); ++i )
		{
			if( lCurrent == static_cast<long>( p_pCombo->GetItemData( i )) )
			{
				p_pCombo->SetCurSel( i );
				break;
			}
		}
	}
}

void SVMaskShapeEditorDlg::setImages()
{
	int tabIndex = 0;
	// Get the Image for this tool and set to first tab
	m_dialogImage.setImageFromParent( m_pMask->getReferenceImage(), tabIndex );
	tabIndex++;
	// Set second tab to Mask
	MatroxImageData data(m_pMask->m_MaskBufferHandlePtr);
	m_dialogImage.setImage( &data, tabIndex );
	tabIndex++;
	// Set third tab to source image
	SVImageInfoClass* pImageInfo = reinterpret_cast < SVImageInfoClass*> (::SVSendMessage( m_pTool, SVM_GETFIRST_IMAGE_INFO, NULL, NULL ) );
	if( nullptr != pImageInfo )
	{
		SVImageClass* pImage = nullptr;
		pImageInfo->GetOwnerImage( pImage );
		m_dialogImage.setImage( pImage, tabIndex );
	}
	tabIndex++;

	m_dialogImage.Refresh();
}

void SVMaskShapeEditorDlg::setShapeType(SVShapeMaskHelperClass::ShapeTypeEnum shapeType)
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
		if ( SVShapeMaskHelperClass::SVMaskShapeTypeInvalid != shapeType )
		{
			m_eShapeType = shapeType;
		}
		else
		{
			m_eShapeType = SVShapeMaskHelperClass::SVMaskShapeTypeRectangle;
		}

		CRect rect = m_mapShapes[ m_eShapeType ]->GetRect();

		LongParamMap ParMap;
		ParMap[ CDSVPictureDisplay::P_X1 ] = rect.left; //@TODO:  Consider making a method that takes a rect as a parameter, and moving these lines to that method.
		ParMap[ CDSVPictureDisplay::P_Y1 ] = rect.top;
		ParMap[ CDSVPictureDisplay::P_X2 ] = rect.right;
		ParMap[ CDSVPictureDisplay::P_Y2 ] = rect.bottom;
		ParMap[ CDSVPictureDisplay::P_Color ] = SVColor::Green;
		ParMap[ CDSVPictureDisplay::P_SelectedColor ] = SVColor::Green;
		ParMap[ CDSVPictureDisplay::P_AllowEdit ] = m_bAutoResize ? CDSVPictureDisplay::AllowNone : CDSVPictureDisplay::AllowAll;

		//add new shape types
		switch (m_eShapeType)
		{
		case SVShapeMaskHelperClass::SVMaskShapeTypeRectangle:
			ParMap[ CDSVPictureDisplay::P_Type ] = CDSVPictureDisplay::RectangleROI;
			break;
		case SVShapeMaskHelperClass::SVMaskShapeTypeOval:
			ParMap[ CDSVPictureDisplay::P_Type ] = CDSVPictureDisplay::EllipseROI;
			break;
		case SVShapeMaskHelperClass::SVMaskShapeTypeSymmetricTrapezoid:
			ParMap[ CDSVPictureDisplay::P_Type ] = CDSVPictureDisplay::TrapezoidROI;
			ParMap[ CDSVPictureDisplay::P_SubType ] = CDSVPictureDisplay::VerticalAxisTop;
			ParMap[ CDSVPictureDisplay::P_Offset ] = 10;
			break;
		case SVShapeMaskHelperClass::SVMaskShapeTypeDoughnut:
			ParMap[ CDSVPictureDisplay::P_Type ] = CDSVPictureDisplay::DoughnutROI;
			ParMap[ CDSVPictureDisplay::P_SideThickness ] = 10;
			ParMap[ CDSVPictureDisplay::P_TopThickness ] = 20;
			break;
		default:
			ParMap[ CDSVPictureDisplay::P_Type ] = CDSVPictureDisplay::InvalidROIType; 
			break; // Do nothing.
		}

		COleSafeArray saPar, saVal;
		DisplayHelper::CreateSafeArrayFromMap( ParMap, saPar, saVal );

		for (int i = 0; i < m_numberOfTabs; i++)
		{
			m_dialogImage.AddOverlay( i, static_cast< LPVARIANT >( saPar ), static_cast< LPVARIANT >( saVal ), &(m_handleToActiveObjects[ i ] ) );
			m_dialogImage.SetEditAllow(i, m_handleToActiveObjects[i], 7);
		}
		saPar.Destroy();
		saVal.Destroy();
	}
}

void SVMaskShapeEditorDlg::resetShapeOverlay()
{
	////////////////////////////////////////////////////////
	// SET SHAPE PROPERTIES
	SVMaskShape* pShape = m_mapShapes[ m_eShapeType ];
	SVMaskShape::MapType mapProperties;
	m_pMask->GetShapeHelper()->GetProperties( mapProperties );
	pShape->SetProperties( mapProperties );
	SVMaskFillPropertiesStruct svFillProperties;
	m_pMask->GetFillProperties( svFillProperties );
	pShape->SetFillProperties( svFillProperties );
	pShape->SetAutoResize( TRUE == m_bAutoResize );
	CRect rect = pShape->GetRect();

	//add new shape types
	std::map<long,long> ParMap;
	ParMap[ CDSVPictureDisplay::P_Type ] = 0;
	ParMap[ CDSVPictureDisplay::P_X1 ] = rect.left;
	ParMap[ CDSVPictureDisplay::P_Y1 ] = rect.top;
	ParMap[ CDSVPictureDisplay::P_X2 ] = rect.right;
	ParMap[ CDSVPictureDisplay::P_Y2 ] = rect.bottom;
	ParMap[ CDSVPictureDisplay::P_Color ] = SVColor::Green;
	ParMap[ CDSVPictureDisplay::P_SelectedColor ] = SVColor::Green;
	ParMap[ CDSVPictureDisplay::P_AllowEdit ] = m_bAutoResize ? CDSVPictureDisplay::AllowNone : CDSVPictureDisplay::AllowAll;

	//add new shape types
	switch (m_eShapeType)
	{
	case SVShapeMaskHelperClass::SVMaskShapeTypeRectangle:
		ParMap[ CDSVPictureDisplay::P_Type ] = CDSVPictureDisplay::RectangleROI;
		break;
	case SVShapeMaskHelperClass::SVMaskShapeTypeOval:
		ParMap[ CDSVPictureDisplay::P_Type ] = CDSVPictureDisplay::EllipseROI;
		break;
	case SVShapeMaskHelperClass::SVMaskShapeTypeSymmetricTrapezoid:
		ParMap[ CDSVPictureDisplay::P_Type ] = CDSVPictureDisplay::TrapezoidROI;

		switch( mapProperties[ SVShapeMaskPropertySymmetryOrientationGuid].value)
		{
		case SVMaskShapeSymmetricTrapezoid::VerticalAxisTop:
			ParMap[ CDSVPictureDisplay::P_SubType ] = CDSVPictureDisplay::VerticalAxisTop;
			break;
		case SVMaskShapeSymmetricTrapezoid::VerticalAxisBottom:
			ParMap[ CDSVPictureDisplay::P_SubType ] = CDSVPictureDisplay::VerticalAxisBottom;
			break;
		case SVMaskShapeSymmetricTrapezoid::HorizontalAxisLeft:
			ParMap[ CDSVPictureDisplay::P_SubType ] = CDSVPictureDisplay::HorizontalAxisLeft;
			break;
		case SVMaskShapeSymmetricTrapezoid::HorizontalAxisRight:
			ParMap[ CDSVPictureDisplay::P_SubType ] = CDSVPictureDisplay::HorizontalAxisRight;
			break;
		default:
			break; // Do nothing.
		}
		ParMap[ CDSVPictureDisplay::P_Offset ] = mapProperties[ SVShapeMaskPropertyOffsetGuid ].value;
		break;
	case SVShapeMaskHelperClass::SVMaskShapeTypeDoughnut:
		ParMap[ CDSVPictureDisplay::P_Type ] = CDSVPictureDisplay::DoughnutROI;
		ParMap[ CDSVPictureDisplay::P_SideThickness ] = mapProperties[ SVShapeMaskPropertySideThicknessGuid ].value;
		ParMap[ CDSVPictureDisplay::P_TopThickness ] = mapProperties[ SVShapeMaskPropertyTopBottomThicknessGuid ].value;
		break;
	default:
		break; // Do nothing.
	}

	COleSafeArray saPar, saVal;
	DisplayHelper::CreateSafeArrayFromMap( ParMap, saPar, saVal );
	for (int i = 0; i < m_numberOfTabs; i++)
	{
		m_dialogImage.EditOverlay( i, m_handleToActiveObjects[i], static_cast< LPVARIANT >( saPar ), static_cast< LPVARIANT >( saVal ) );
	}
	saVal.Destroy();
	saPar.Destroy();
}
#pragma endregion Private Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVMaskShapeEditorDlg.cpp_v  $
 * 
 *    Rev 1.4   26 Jun 2014 17:52:12   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   use SVPictureDisplay-control
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   05 Feb 2014 18:31:28   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed to follow guidelines more closely.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   01 Oct 2013 15:24:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 Aug 2013 10:24:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added include of SVUserMaskOperatorClass.h.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 12:27:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   04 Oct 2012 10:45:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   07 Apr 2011 16:36:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVDlgImageClass to change the method init to UpdateImageInfo to make it trackable throughout the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   10 Feb 2011 15:00:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   27 Jan 2011 11:33:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   08 Dec 2010 13:05:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   16 Dec 2009 12:00:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   30 Jul 2009 12:04:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   24 Jul 2007 14:27:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update to MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   21 Jun 2007 14:16:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   04 Apr 2006 07:23:06   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  554
 * SCR Title:  Add Shape Mask capability
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   corrected property display on init dialog
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   03 Apr 2006 15:58:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  554
 * SCR Title:  Add Shape Mask capability
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented AvailableWithAutoResize for shape properties
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   31 Mar 2006 09:20:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  554
 * SCR Title:  Add Shape Mask capability
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added Auto Resize capability to the Shape Mask
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   20 Mar 2006 13:31:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  549
 * SCR Title:  Fix Shape Mask Editor
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added GetFillProperties functionality
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   08 Sep 2005 15:38:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   fix display bugs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   24 Aug 2005 08:08:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   fixed tool update for properties box
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   17 Aug 2005 14:20:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   fixed dlg updating mask operator
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   17 Aug 2005 10:36:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   implemented Input Request Queue tool updating
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Aug 2005 07:25:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 Jul 2005 10:25:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Cosmetic changes for the Mask dialogs
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 May 2005 14:28:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/