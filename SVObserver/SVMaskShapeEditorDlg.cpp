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
//Moved to precompiled header: #include <colordlg.h>
#include "SVMaskShapeEditorDlg.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "ObjectInterfaces/IObjectManager.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObserver.h"
#include "SVIPDoc.h"
#include "SVLibrary/SVWinUtility.h"	// for SVYieldPaintMessages
#include "Operators/SVMaskShape.h"
#include "Operators/SVUserMaskOperatorClass.h"
#include "SVOGui/DisplayHelper.h"
#include "Definitions/Color.h"
#include "SVUtilityLibrary/StringHelper.h"
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
SVMaskShapeEditorDlg::SVMaskShapeEditorDlg(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, const SVGUID& rMaskOperatorID, const SVGUID& rShapeMaskHelperID, CWnd* pParent /*=nullptr*/)
: CDialog(SVMaskShapeEditorDlg::IDD, pParent)
, m_rInspectionID{ rInspectionID }
, m_rTaskObjectID{ rTaskObjectID }
, m_Values{ SvOg::BoundValues{ rInspectionID, rMaskOperatorID } }
, m_ShapeHelperValues{ SvOg::BoundValues{ rInspectionID, rShapeMaskHelperID } }
, m_sFillColor( _T( "" ) )
, m_sCoordinates( _T( "" ) )
, m_bAutoResize( FALSE )
, m_pMask( nullptr )
, m_isInit( false )
, m_currentTabNumber( 2 ) // BRW - Why is this 2?
, m_eShapeType(SvOp::SVShapeMaskHelperClass::SVMaskShapeTypeInvalid )
, m_maskController {rInspectionID, rTaskObjectID, rMaskOperatorID}
{
	m_pThis = this;

	m_pMask = dynamic_cast<SvOp::SVUserMaskOperatorClass*> (SvOi::getObject(rMaskOperatorID));
	
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

void SVMaskShapeEditorDlg::CheckPoint()
{
	GetCancelData(m_cancelData);
}

void SVMaskShapeEditorDlg::Revert()
{
	for (auto const& rEntry : m_cancelData)
	{
		SVObjectClass* pObject = rEntry.first.m_ObjectRef.getObject();
		if(nullptr != pObject)
		{
			SVGUID EmbeddedID = pObject->GetEmbeddedID();
			SVObjectClass* pParent = pObject->GetParent();
			if(nullptr != pParent)
			{
				if (m_Values.GetTaskID() == pParent->GetUniqueObjectID())
				{
					m_Values.Set<_variant_t>(EmbeddedID, rEntry.second);
				}
				else if (m_ShapeHelperValues.GetTaskID() == pParent->GetUniqueObjectID())
				{
					m_ShapeHelperValues.Set<_variant_t>(EmbeddedID, rEntry.second);
				}
			}
		}
	}

	m_Values.Commit();
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

SvOp::SVMaskShape* SVMaskShapeEditorDlg::GetCurrentShape()	// holds the properties and does the rendering // BRW - This method should be const.
{
	return m_mapShapes[m_eShapeType];
}
#pragma endregion Public Methods

#pragma region Protected Methods
HRESULT SVMaskShapeEditorDlg::GetCancelData(SvIe::SVInputRequestStructMap& rMap)
{
	ASSERT( m_pMask );
	return m_pMask->GetCancelData( rMap );
}

HRESULT SVMaskShapeEditorDlg::SetInspectionData(bool bResetObject/* = false*/)
{
	SvOp::SVMaskShape::MapType mapProperties;
	GetCurrentShape()->GetProperties(mapProperties);

	SvIe::SVInputRequestStructMap mapData;

	for (auto const& rEntry : mapProperties)
	{
		GUID guid = rEntry.first;
		SVObjectClass* pObject = m_pMask->GetShapeHelper()->GetEmbeddedValueObject( guid );
		if ( nullptr != dynamic_cast<SvOi::IValueObject*> (pObject) )
		{
			mapData[ SVObjectReference( pObject ) ] = _variant_t( rEntry.second.value );
		}
	}

	for (auto const& rEntry : mapData)
	{
		SVGUID EmbeddedID = rEntry.first.m_ObjectRef.getObject()->GetEmbeddedID();
		m_ShapeHelperValues.Set<long>(EmbeddedID, static_cast<long> (rEntry.second));
	}

	m_ShapeHelperValues.Commit();

	//This needs to be called due to SetInspectionData being called by UpdateMask which has values set but not commited
	SvOg::PostAction commitAction{SvOg::PostAction::doRunOnce};
	commitAction = commitAction | (bResetObject ? SvOg::PostAction::doReset : SvOg::PostAction::doNothing);
	m_Values.Commit(commitAction);

	m_ShapeHelperValues.Init();

	return S_OK;
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
	DDX_Check(pDX, IDC_CHECK_AUTO_RESIZE, m_bAutoResize);
	//}}AFX_DATA_MAP
}

BOOL SVMaskShapeEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// must set these before calling DoModal
	assert( nullptr != m_pMask );

	m_maskController.Init();
	m_Values.Init();
	m_ShapeHelperValues.Init();

	bool l_bAutoResize = m_pMask->GetShapeHelper()->IsAutoResize();
	m_bAutoResize = l_bAutoResize;


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

	FillComboBox(m_pMask->getCancelData().evoCurrentMaskOperator, &m_cbMaskOperator);
	FillComboBox(m_pMask->getCancelData().evoFillArea, &m_cbFillOptions);	// Fill Area

	SvOp::SVShapeMaskHelperClass* pShapeMaskHelper = m_pMask->GetShapeHelper();

	assert( nullptr != pShapeMaskHelper );

	if (nullptr !=  pShapeMaskHelper )
	{
		FillComboBox(pShapeMaskHelper->getCancelData().evoMaskArea, &m_cbMaskArea);
		FillComboBox(pShapeMaskHelper->getCancelData().evoShapeType, &m_cbMaskShape);
	}

	long lColor=0;
	m_pMask->getCancelData().lvoFillColor.GetValue(lColor);
	m_sFillColor = SvUl::AsString(lColor).c_str();

	m_dialogImage.AddTab(_T("Tool Input"));
	m_dialogImage.AddTab(_T("Mask"));
	m_dialogImage.AddTab(_T("Tool Result"));
	m_dialogImage.SelectTab(m_currentTabNumber);

 	m_mapShapes[SvOp::SVShapeMaskHelperClass::SVMaskShapeTypeRectangle] = new SvOp::SVMaskShapeRectangle;
	m_mapShapes[SvOp::SVShapeMaskHelperClass::SVMaskShapeTypeOval] = new SvOp::SVMaskShapeOval;
	m_mapShapes[SvOp::SVShapeMaskHelperClass::SVMaskShapeTypeDoughnut] = new SvOp::SVMaskShapeDoughnut;
	m_mapShapes[SvOp::SVShapeMaskHelperClass::SVMaskShapeTypeSymmetricTrapezoid] = new SvOp::SVMaskShapeSymmetricTrapezoid;

	long lShapeType;
	pShapeMaskHelper->getCancelData().evoShapeType.GetValue(lShapeType);
	SvOp::SVShapeMaskHelperClass::ShapeTypeEnum shapeType = static_cast <SvOp::SVShapeMaskHelperClass::ShapeTypeEnum> (lShapeType);
	assert( shapeType != SvOp::SVShapeMaskHelperClass::SVMaskShapeTypeInvalid );
	setShapeType( shapeType );

	// set image info for all shapes
	ShapeMap::iterator itShapeMapIter;
	for ( itShapeMapIter = m_mapShapes.begin(); itShapeMapIter != m_mapShapes.end(); ++itShapeMapIter )
	{
		itShapeMapIter->second->SetImageInfo(m_pMask->getMaskBufferInfo());
	} // end for loop

	////////////////////////////////////////////////////////
	// SET SHAPE PROPERTIES
	SvOp::SVMaskShape* pShape = m_mapShapes[m_eShapeType];
	if (nullptr != pShape)
	{
		SvOp::SVMaskShape::MapType mapProperties;
		m_pMask->GetShapeHelper()->GetProperties(mapProperties);
		pShape->SetProperties(mapProperties);

		SvOp::SVMaskFillPropertiesStruct svFillProperties;
		m_pMask->GetFillProperties(svFillProperties);
		pShape->SetFillProperties(svFillProperties);

		pShape->SetAutoResize(l_bAutoResize);
	}

	////////////////////////////////////////////////////////

	if(nullptr != m_pMask->getImageBuffer())
	{
		GetCurrentShape()->Refresh();	// renders based on new properties
		GetCurrentShape()->Draw(m_pMask->getImageBuffer()->GetBuffer() );
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
	SvOp::SVShapeMaskHelperClass* pShapeMaskHelper = m_pMask->GetShapeHelper();

	int iSel = m_cbMaskShape.GetCurSel();
	long lValue = static_cast<long>( m_cbMaskShape.GetItemData( iSel ) );
	m_ShapeHelperValues.Set<long>(pShapeMaskHelper->getCancelData().evoShapeType.GetEmbeddedID(), lValue);

	iSel = m_cbMaskArea.GetCurSel();
	lValue = static_cast<long>( m_cbMaskArea.GetItemData( iSel ) );
	m_ShapeHelperValues.Set<long>(pShapeMaskHelper->getCancelData().evoMaskArea.GetEmbeddedID(), lValue);

	lValue = static_cast<long> (m_eShapeType);
	m_ShapeHelperValues.Set<long>(pShapeMaskHelper->getCancelData().evoShapeType.GetEmbeddedID(), lValue);

	iSel = m_cbMaskOperator.GetCurSel();
	lValue = static_cast<long>( m_cbMaskOperator.GetItemData( iSel ) );
	m_Values.Set<long>(m_pMask->getCancelData().evoCurrentMaskOperator.GetEmbeddedID(), lValue);

	iSel = m_cbFillOptions.GetCurSel();
	lValue = static_cast<long>( m_cbFillOptions.GetItemData( iSel ) );
	m_Values.Set<long>(m_pMask->getCancelData().evoFillArea.GetEmbeddedID(), lValue);

	lValue = atol(m_sFillColor);
	m_Values.Set<long>(m_pMask->getCancelData().lvoFillColor.GetEmbeddedID(), lValue);

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

		m_Values.Set<long>(m_pMask->getCancelData().lvoFillColor.GetEmbeddedID(), FillColor);
		UpdateMask();	// need to reset object for shape change
	}
	else
	{
		m_Values.Set<long>(m_pMask->getCancelData().lvoFillColor.GetEmbeddedID(), lOriginalColor);
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
		SvOp::SVMaskShape* pOldShape = GetCurrentShape();
		SvOp::SVMaskShape::MapType mapOldProperties;
		pOldShape->GetProperties( mapOldProperties );

		DWORD_PTR dwType = m_cbMaskShape.GetItemData(iSel);
		setShapeType( static_cast<SvOp::SVShapeMaskHelperClass::ShapeTypeEnum> (dwType) );	// change current shape

		GetCurrentShape()->SetProperties( mapOldProperties ); // reuse common properties; any that don't apply will be ignored
		GetCurrentShape()->SetAutoResize( TRUE == m_bAutoResize );

		m_ShapeHelperValues.Set<long>(m_pMask->GetShapeHelper()->getCancelData().evoShapeType.GetEmbeddedID(), static_cast<long> (m_eShapeType));

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
		m_Values.Set<long>(m_pMask->getCancelData().evoCurrentMaskOperator.GetEmbeddedID(), lValue);

		UpdateMask(true);
	}
}

void SVMaskShapeEditorDlg::OnSelChangeComboMaskArea()
{
	UpdateData();
	int iSel = m_cbMaskArea.GetCurSel();
	if ( iSel != CB_ERR )
	{
		SvOp::SVShapeMaskHelperClass* pShapeMaskHelper = m_pMask->GetShapeHelper();

		long lValue = (long) m_cbMaskArea.GetItemData( iSel );
		m_ShapeHelperValues.Set<long>(pShapeMaskHelper->getCancelData().evoMaskArea.GetEmbeddedID(), lValue);

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
		m_Values.Set<long>(m_pMask->getCancelData().evoFillArea.GetEmbeddedID(), lValue);

		UpdateMask();
	}
}

void SVMaskShapeEditorDlg::OnChangeEditFillColor()
{
	UpdateData();
	// stuff into value object
	m_Values.Set<long>(m_pMask->getCancelData().lvoFillColor.GetEmbeddedID(), atol(m_sFillColor));
	UpdateMask();	// need to reset object for shape change
}

void SVMaskShapeEditorDlg::OnCheckAutoResize()
{
	UpdateData();

	m_ShapeHelperValues.Set<bool>(m_pMask->GetShapeHelper()->getCancelData().bvoAutoResize.GetEmbeddedID(), m_bAutoResize ? true : false);
	GetCurrentShape()->SetAutoResize( m_bAutoResize ? true : false ); // reuse common properties; any that don't apply will be ignored

	UpdateMask(true);
	RefreshProperties();
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

		std::string sValue;
		long lNewValue;
		SVRPropertyItemCombo* pCombo = dynamic_cast <SVRPropertyItemCombo*> (pItem);
		if ( pCombo )
		{
			pCombo->GetItemValue( lNewValue );
		}
		else
		{
			pItem->GetItemValue( sValue );
			lNewValue = atol( sValue.c_str() );
		}

		SvOp::SVMaskShape::MapType mapProperties;
		GetCurrentShape()->GetProperties(mapProperties);

		long lOldValue = mapProperties[guidProperty].value;

		if (lOldValue != lNewValue)
		{
			mapProperties[guidProperty] = static_cast<long>(lNewValue);

			GetCurrentShape()->SetProperties(mapProperties);

			GetCurrentShape()->GetProperties(mapProperties);
			long lValue = mapProperties[guidProperty].value;

			SVObjectClass* pObject = m_pMask->GetShapeHelper()->GetEmbeddedValueObject(guidProperty);
			assert(pObject);
			if (nullptr != dynamic_cast<SvOi::IValueObject*> (pObject))
			{
				m_ShapeHelperValues.Set<long>(pObject->GetEmbeddedID(), lValue);
			}

			if (pCombo)
			{
				pCombo->SetItemValue((unsigned long)lValue);
			}
			else
			{
				pItem->SetItemValue(SvUl::AsString(lValue).c_str());
			}

			pItem->OnRefresh();

			// redraw shape
			UpdateMask();
		}
	}// end if ( pNMPropTree->pItem )
}
#pragma endregion AFX Methods

void SVMaskShapeEditorDlg::ObjectChangedExDialogImage(long Tab, long Handle, VARIANT* ParameterList, VARIANT* ParameterValue)
{
	////////////////////////////////////////////////////////
	// SET SHAPE PROPERTIES
	VariantParamMap ParaMap;
	int count = SvOg::DisplayHelper::FillParameterMap(ParaMap, ParameterList, ParameterValue);

	SvOp::SVMaskShape* pShape = m_mapShapes[ m_eShapeType ];
	SvOp::SVMaskShape::MapType mapProperties;
	m_pMask->GetShapeHelper()->GetProperties( mapProperties );

	if( m_eShapeType != SvOp::SVShapeMaskHelperClass::SVMaskShapeTypeInvalid )
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

	if( m_eShapeType == SvOp::SVShapeMaskHelperClass::SVMaskShapeTypeSymmetricTrapezoid )
	{
		mapProperties[ SVShapeMaskPropertyOffsetGuid ].value = ParaMap[ CDSVPictureDisplay::P_Offset ].lVal;

		switch( ParaMap[ CDSVPictureDisplay::P_SubType ].lVal )
		{
		case CDSVPictureDisplay::VerticalAxisTop:
			mapProperties[ SVShapeMaskPropertySymmetryOrientationGuid ].value = SvOp::SVMaskShapeSymmetricTrapezoid::VerticalAxisTop;
			break;

		case CDSVPictureDisplay::VerticalAxisBottom:
			mapProperties[ SVShapeMaskPropertySymmetryOrientationGuid ].value = SvOp::SVMaskShapeSymmetricTrapezoid::VerticalAxisBottom;
			break;

		case CDSVPictureDisplay::HorizontalAxisLeft:
			mapProperties[ SVShapeMaskPropertySymmetryOrientationGuid ].value = SvOp::SVMaskShapeSymmetricTrapezoid::HorizontalAxisLeft;
			break;

		case CDSVPictureDisplay::HorizontalAxisRight:
			mapProperties[ SVShapeMaskPropertySymmetryOrientationGuid ].value = SvOp::SVMaskShapeSymmetricTrapezoid::HorizontalAxisRight;
			break;

		default:
			break;
		}
	}

	if( m_eShapeType == SvOp::SVShapeMaskHelperClass::SVMaskShapeTypeDoughnut )
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
	m_sCoordinates.Format( _T("%ld, %ld"), X, Y );
	UpdateData(FALSE);
}

UINT_PTR CALLBACK SVMaskShapeEditorDlg::ColorDlgHookFn( HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	int iReturnCode = 0;	// by default allow color dlg to process message

#if defined (TRACE_THEM_ALL) || defined (TRACE_OTHER)
	TRACE("ColorDlgHookFn - MSG: %08X, WPARAM: %08X, LPARAM: %08X\n", uiMsg, wParam, lParam);
#endif

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
				
				::SendMessage( hdlg, UISetRGB, 0, static_cast< LPARAM >( RGB(lColor, lColor, lColor) ));
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
				m_pThis->GetValues().Set<long>(m_pThis->m_pMask->getCancelData().lvoFillColor.GetEmbeddedID(), atol(sText));
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
		} while ( GUID_NULL != GetPropertyGuid(id) );

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
	return GUID_NULL;
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

	SvOp::SVMaskShape::MapType mapProperties;
	GetCurrentShape()->GetProperties( mapProperties );

	SvOp::SVMaskShape::SortedMapViewType::const_iterator iter;
	SvOp::SVMaskShape::SortedMapViewType viewOrderedProperties(SvOp::SVMaskShape::GetTempSortedPropertyMapView(mapProperties));

	for( iter = viewOrderedProperties.begin(); iter != viewOrderedProperties.end(); ++iter )
	{
		const SvOp::SVMaskShape::MapType::key_type& key = (*iter)->first;
		const SvOp::SVMaskShapeProperty& rData = (*iter)->second;

		SVObjectClass* pValueObject = m_pMask->GetShapeHelper()->GetEmbeddedValueObject( key );
		ASSERT( pValueObject );
		if( nullptr != pValueObject )
		{
			std::string Name = pValueObject->GetName();

			SVRPropertyItem* pItem = nullptr;

			if (SvVol::SVEnumerateValueObjectClass* pEnum = dynamic_cast <SvVol::SVEnumerateValueObjectClass*> ( pValueObject ) )
			{
				SVRPropertyItemCombo* pCombo = dynamic_cast <SVRPropertyItemCombo*> (m_Tree.InsertItem(new SVRPropertyItemCombo, pRoot) );
				pItem = pCombo;

				pCombo->EnableButton( false );
				pCombo->CreateComboBox( WS_CHILD | WS_VSCROLL | CBS_DROPDOWNLIST );

				for (auto const& rEntry : pEnum->GetEnumVector())
				{
					int iPos = pCombo->AddString( rEntry.first.c_str() );
					pCombo->SetItemData(iPos, static_cast<DWORD_PTR> (rEntry.second));
				}
				pCombo->SetItemValue( rData.value );
			}
			else
			{
				SVRPropertyItemEdit* pEdit = dynamic_cast <SVRPropertyItemEdit*> ( m_Tree.InsertItem(new SVRPropertyItemEdit(), pRoot) );
				pItem = pEdit;

				long lValue = rData.value;
				pEdit->SetItemValue( SvUl::AsString(lValue).c_str() );
			}

			ASSERT( pItem );

			pItem->SetCtrlID( GetPropertyID(key) );
			pItem->SetBold( false );
			pItem->SetHeight(16); //@TODO:  Document the reason for the use of the magic number 16.
			pItem->SetLabelText( Name.c_str() );

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
	SvOp::SVMaskShape::MapType mapProperties;
	GetCurrentShape()->GetProperties( mapProperties );

	SVRPropertyItem* pRoot = m_Tree.GetRootItem()->GetChild();
	ASSERT( pRoot );
	SVRPropertyItem* pChild = pRoot->GetChild();

	while ( pChild )
	{
		GUID guidProperty = GetPropertyGuid( pChild->GetCtrlID() );
		if ( SVRPropertyItemCombo* pCombo = dynamic_cast <SVRPropertyItemCombo*> (pChild) )
		{
			unsigned long ulValue = mapProperties[ guidProperty ].value;
			pCombo->SetItemValue( ulValue );
		}
		else
		{
			pChild->SetItemValue( SvUl::AsString( mapProperties[ guidProperty ].value ).c_str() );
		}

		if ( m_bAutoResize && ! mapProperties[ guidProperty ].bAvailableWithAutoResize )
		{
			pChild->SetItemValue( CString(_T("---")) );
			// grey out property
			pChild->SetCanHighlight( false );
			pChild->SetForeColor( SvDef::LightGray );
			pChild->ReadOnly( true );
		}
		else
		{
			// enable property
			pChild->SetCanHighlight( true );
			pChild->SetForeColor( SvDef::Black );
			pChild->ReadOnly( false );
		}

		pChild = pChild->GetSibling();
	}

	m_Tree.RedrawWindow();

	return S_OK;
}

HRESULT SVMaskShapeEditorDlg::UpdateMask(bool bResetObject)
{
	SetInspectionData(bResetObject);
	GetCurrentShape()->Refresh();
	setImages();

	resetShapeOverlay();
	return S_OK;
}

void SVMaskShapeEditorDlg::FillComboBox(const SvVol::SVEnumerateValueObjectClass& rValueObject, CComboBox* pCombo)
{
	assert( nullptr != pCombo );

	if (nullptr != pCombo)
	{
		for (auto const& rEntry : rValueObject.GetEnumVector())
		{
			pCombo->SetItemData( pCombo->AddString(rEntry.first.c_str()), static_cast<DWORD_PTR> (rEntry.second) );
		}

		// Set Current Value...
		long lCurrent;
		rValueObject.GetValue(lCurrent);

		for(int i=0; i < pCombo->GetCount(); ++i )
		{
			if( lCurrent == static_cast<long> (pCombo->GetItemData(i)) )
			{
				pCombo->SetCurSel(i);
				break;
			}
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

		CRect rect = m_mapShapes[ m_eShapeType ]->GetRect();

		LongParamMap ParMap;
		ParMap[ CDSVPictureDisplay::P_X1 ] = rect.left; //@TODO:  Consider making a method that takes a rect as a parameter, and moving these lines to that method.
		ParMap[ CDSVPictureDisplay::P_Y1 ] = rect.top;
		ParMap[ CDSVPictureDisplay::P_X2 ] = rect.right;
		ParMap[ CDSVPictureDisplay::P_Y2 ] = rect.bottom;
		ParMap[ CDSVPictureDisplay::P_Color ] = SvDef::Green;
		ParMap[ CDSVPictureDisplay::P_SelectedColor ] = SvDef::Green;
		ParMap[ CDSVPictureDisplay::P_AllowEdit ] = m_bAutoResize ? CDSVPictureDisplay::AllowNone : CDSVPictureDisplay::AllowAll;

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
	SvOp::SVMaskShape* pShape = m_mapShapes[ m_eShapeType ];
	SvOp::SVMaskShape::MapType mapProperties;
	m_pMask->GetShapeHelper()->GetProperties( mapProperties );
	pShape->SetProperties( mapProperties );
	SvOp::SVMaskFillPropertiesStruct svFillProperties;
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
	ParMap[ CDSVPictureDisplay::P_Color ] = SvDef::Green;
	ParMap[ CDSVPictureDisplay::P_SelectedColor ] = SvDef::Green;
	ParMap[ CDSVPictureDisplay::P_AllowEdit ] = m_bAutoResize ? CDSVPictureDisplay::AllowNone : CDSVPictureDisplay::AllowAll;

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

		switch( mapProperties[ SVShapeMaskPropertySymmetryOrientationGuid].value)
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
		ParMap[ CDSVPictureDisplay::P_Offset ] = mapProperties[ SVShapeMaskPropertyOffsetGuid ].value;
		break;
	case SvOp::SVShapeMaskHelperClass::SVMaskShapeTypeDoughnut:
		ParMap[ CDSVPictureDisplay::P_Type ] = CDSVPictureDisplay::DoughnutROI;
		ParMap[ CDSVPictureDisplay::P_SideThickness ] = mapProperties[ SVShapeMaskPropertySideThicknessGuid ].value;
		ParMap[ CDSVPictureDisplay::P_TopThickness ] = mapProperties[ SVShapeMaskPropertyTopBottomThicknessGuid ].value;
		break;
	default:
		break; // Do nothing.
	}

	COleSafeArray saPar, saVal;
	SvOg::DisplayHelper::CreateSafeArrayFromMap( ParMap, saPar, saVal );
	for (int i = 0; i < m_numberOfTabs; i++)
	{
		m_dialogImage.EditOverlay( i, m_handleToActiveObjects[i], static_cast< LPVARIANT >( saPar ), static_cast< LPVARIANT >( saVal ) );
	}
	saVal.Destroy();
	saPar.Destroy();
}
#pragma endregion Private Methods

