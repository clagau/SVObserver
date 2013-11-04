//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMaskShapeEditorDlg
//* .File Name       : $Workfile:   SVMaskShapeEditorDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   13 Aug 2013 10:24:58  $
//******************************************************************************

#include "stdafx.h"
#include <colordlg.h>	// for custom color dlg resource #defines
#include "SVMaskShapeEditorDlg.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVMaskShape.h"
#include "SVMaskShapeFigureEditors.h"
#include "SVObserver.h"
#include "SVTool.h"
#include "SVIPDoc.h"
#include "SVOMFCLibrary/stringmunge.h"
#include "SVLibrary/SVWinUtility.h"	// for SVYieldPaintMessages
#include "SVUserMaskOperatorClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVMaskShapeEditorDlg dialog

SVMaskShapeEditorDlg* SVMaskShapeEditorDlg::m_pThis = NULL;
const CString FIT_TO_WINDOW(_T("Fit to Window"));

BEGIN_MESSAGE_MAP(SVMaskShapeEditorDlg, CDialog)
	//{{AFX_MSG_MAP(SVMaskShapeEditorDlg)
	ON_NOTIFY(TCN_SELCHANGE, IDC_IMAGE_TAB, OnSelchangeImageTab)
	ON_BN_CLICKED(IDC_BTN_FILL_COLOR_MORE, OnBtnFillColorMore)
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_ZOOM_IN_BUTTON, OnZoomInButton)
	ON_BN_CLICKED(IDC_ZOOM_OUT_BUTTON, OnZoomOutButton)
	ON_WM_KILLFOCUS()
	ON_CBN_SELCHANGE(IDC_COMBO_SHAPE, OnSelchangeComboShape)
	ON_CBN_SELCHANGE(IDC_COMBO_MASK_OPERATOR, OnSelchangeComboMaskOperator)
	ON_CBN_SELCHANGE(IDC_COMBO_MASK_AREA, OnSelchangeComboMaskArea)
	ON_CBN_SELCHANGE(IDC_COMBO_FILL_OPTIONS, OnSelchangeComboFillOptions)
	ON_CBN_SELCHANGE(IDC_COMBO_ZOOM_SCALE, OnSelchangeComboZoomScale)
	ON_WM_ACTIVATE()
	ON_CBN_EDITCHANGE(IDC_COMBO_ZOOM_SCALE, OnEditchangeComboZoomScale)
	ON_EN_CHANGE(IDC_EDIT_FILL_COLOR, OnChangeEditFillColor)
	ON_BN_CLICKED(IDC_CHECK_AUTO_RESIZE, OnCheckAutoResize)
	//}}AFX_MSG_MAP
    ON_NOTIFY(PTN_ITEMCHANGED, IDC_PROPERTIES, OnItemChanged)
END_MESSAGE_MAP()


SVMaskShapeEditorDlg::SVMaskShapeEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SVMaskShapeEditorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVMaskShapeEditorDlg)
	m_sFillColor = _T("");
	m_sCoordinates = _T("");
	m_bAutoResize = FALSE;
	//}}AFX_DATA_INIT
	m_pTool = NULL;
	m_pMask = NULL;
	m_ePage = VIEW_MASK;
	m_pThis = this;
	m_dZoom = 0;
}


void SVMaskShapeEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVMaskShapeEditorDlg)
	DDX_Control(pDX, IDC_COMBO_ZOOM_SCALE, m_cbZoom);
	DDX_Control(pDX, IDC_IMAGE_TAB, m_tabImage);
	DDX_Control(pDX, IDC_COMBO_SHAPE, m_cbMaskShape);
	DDX_Control(pDX, IDC_COMBO_MASK_OPERATOR, m_cbMaskOperator);
	DDX_Control(pDX, IDC_COMBO_MASK_AREA, m_cbMaskArea);
	DDX_Control(pDX, IDC_COMBO_FILL_OPTIONS, m_cbFillOptions);
	DDX_Text(pDX, IDC_EDIT_FILL_COLOR, m_sFillColor);
	DDX_Text(pDX, IDC_COORDINATES_STATIC, m_sCoordinates);
	DDX_Control(pDX, IDC_IMAGE_STATIC, m_dialogImage);
	DDX_Check(pDX, IDC_CHECK_AUTO_RESIZE, m_bAutoResize);
	//}}AFX_DATA_MAP
}

// ISVCancel
bool SVMaskShapeEditorDlg::CanCancel()
{
	return m_pMask->CanCancel();
}

// ISVCancel
HRESULT SVMaskShapeEditorDlg::GetCancelData(SVCancelData*& rpData)
{
	//ASSERT( m_pMask );
	//return m_pMask->GetCancelData( rpData );
	ASSERT( FALSE );
	return E_NOTIMPL;
}

// ISVCancel
HRESULT SVMaskShapeEditorDlg::SetCancelData(SVCancelData* pData)
{
	//ASSERT( m_pMask );
	//return m_pMask->SetCancelData( pData );
	ASSERT( FALSE );
	return E_NOTIMPL;
}

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
		if ( pValueObject )
		{
			SVValueObjectReference ref( pValueObject );
			mapData[ ref ] = AsString( iter->second.value );
		}
	}

	AddInputRequest( mapData );
	RunOnce( m_pMask->GetTool() );
	

	return S_OK;
}



SVMaskShape* SVMaskShapeEditorDlg::GetCurrentShape()	// holds the properties and does the rendering
{
	return m_mapShapes[m_eShapeType].pShape;
}

SVMaskShapeFigureEditor* SVMaskShapeEditorDlg::GetCurrentFigureEditor()  // handles GUI feedback ( mouse move, cursor changes, etc. )
{
	return m_mapShapes[m_eShapeType].pFigureEditor;
}


/////////////////////////////////////////////////////////////////////////////
// SVMaskShapeEditorDlg message handlers

namespace	// file local
{
	static int ID_BASE = 1000;
}
BOOL SVMaskShapeEditorDlg::OnInitDialog() 
{
	// must set these before calling DoModal
	ASSERT( m_pTool != NULL );
	ASSERT( m_pMask != NULL );

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

	const double dZoom[] = { 0, 100, 125, 150, 200, 300, 400 };
	const int iZoomSize = sizeof(dZoom) / sizeof(double);
	m_vecZoomLevels.resize( iZoomSize );
	std::copy( dZoom, dZoom + iZoomSize, m_vecZoomLevels.begin() );

	m_cbZoom.SetItemData( m_cbZoom.AddString(FIT_TO_WINDOW), static_cast<LPARAM>(m_vecZoomLevels[0]) );
	std::vector<double>::iterator iter;
	for ( iter = m_vecZoomLevels.begin()+1; iter != m_vecZoomLevels.end(); ++iter )
	{
		m_cbZoom.SetItemData( m_cbZoom.AddString(AsString((long)*iter)+_T("%")), static_cast<LPARAM>(*iter) );
	}
	m_cbZoom.SetCurSel(0);
	m_dZoom = m_vecZoomLevels[0];


	long lColor=0;
	m_pMask->m_Data.lvoFillColor.GetValue(lColor);
	m_sFillColor = AsString(lColor);


	m_tabImage.GetWindowRect(&rect);
	m_tabImage.SetMinTabWidth( rect.Width() / 3 - 5 );

	TCITEM tcitem;
	tcitem.mask = TCIF_PARAM;

	m_tabImage.InsertItem(0, _T("Tool Input"));
	tcitem.lParam = VIEW_SOURCE;
	m_tabImage.SetItem(VIEW_SOURCE, &tcitem);

	m_tabImage.InsertItem(1, _T("Mask"));
	tcitem.lParam = VIEW_MASK;
	m_tabImage.SetItem(VIEW_MASK, &tcitem);

	m_tabImage.InsertItem(2, _T("Tool Result"));
	tcitem.lParam = VIEW_RESULT;
	m_tabImage.SetItem(VIEW_RESULT, &tcitem);

	m_tabImage.SetCurSel( m_ePage );


	m_mapShapes[ SVShapeMaskHelperClass::SVMaskShapeTypeRectangle ] = ShapePair( new SVMaskShapeRectangle, new SVMaskShapeRectangleFigureEditor );
	m_mapShapes[ SVShapeMaskHelperClass::SVMaskShapeTypeOval ] = ShapePair( new SVMaskShapeOval, new SVMaskShapeOvalFigureEditor );
	m_mapShapes[ SVShapeMaskHelperClass::SVMaskShapeTypeDoughnut ] = ShapePair( new SVMaskShapeDoughnut, new SVMaskShapeDoughnutFigureEditor );
	m_mapShapes[ SVShapeMaskHelperClass::SVMaskShapeTypeSymmetricTrapezoid ] = ShapePair( new SVMaskShapeSymmetricTrapezoid, new SVMaskShapeSymmetricTrapezoidFigureEditor );

	long lShapeType;
	pShapeMaskHelper->m_Data.evoShapeType.GetValue(lShapeType);
	m_eShapeType = static_cast <SVShapeMaskHelperClass::ShapeTypeEnum> (lShapeType);
	ASSERT( m_eShapeType != SVShapeMaskHelperClass::SVMaskShapeTypeInvalid );

	if ( m_eShapeType == SVShapeMaskHelperClass::SVMaskShapeTypeInvalid )
		m_eShapeType = SVShapeMaskHelperClass::SVMaskShapeTypeRectangle;

	m_dialogImage.SetFigureEditor( GetCurrentFigureEditor() );


	// set image info for all shapes
	ShapeMap::iterator itShapeMapIter;
	for ( itShapeMapIter = m_mapShapes.begin(); itShapeMapIter != m_mapShapes.end(); ++itShapeMapIter )
	{
		itShapeMapIter->second.pShape->SetImageInfo(m_pMask->m_MaskBufferInfo);
		//itShapeMapIter->second.pShape->SetFillProperties( svFillProperties );
	} // end for loop
	
	////////////////////////////////////////////////////////
	// SET SHAPE PROPERTIES
	SVMaskShape* l_pShape = m_mapShapes[ m_eShapeType ].pShape;
	SVMaskShape::MapType mapProperties;
	m_pMask->GetShapeHelper()->GetProperties( mapProperties );
	l_pShape->SetProperties( mapProperties );

	SVMaskFillPropertiesStruct svFillProperties;
	m_pMask->GetFillProperties( svFillProperties );
	l_pShape->SetFillProperties( svFillProperties );
	
	l_pShape->SetAutoResize( l_bAutoResize );

	////////////////////////////////////////////////////////

	if( !( m_pMask->m_MaskBufferHandlePtr.empty() ) )
	{
		SVImageBufferHandleImage l_MilHandle;
		m_pMask->m_MaskBufferHandlePtr->GetData( l_MilHandle );

		GetCurrentShape()->Refresh();	// renders based on new properties
		GetCurrentShape()->Draw( l_MilHandle.GetBuffer() );
	}

	DisplayImagePage();


	BuildPropertyList();


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
	lValue = (long) m_cbMaskShape.GetItemData( iSel );
	//pShapeMaskHelper->m_Data.evoShapeType.SetValue(1, lValue);
	AddInputRequest( SVValueObjectReference( &pShapeMaskHelper->m_Data.evoShapeType ), AsString(lValue) );
	
	iSel = m_cbMaskOperator.GetCurSel();
	lValue = (long) m_cbMaskOperator.GetItemData( iSel );
	//m_pMask->m_Data.evoCurrentMaskOperator.SetValue(1, lValue);
	AddInputRequest( SVValueObjectReference( &m_pMask->m_Data.evoCurrentMaskOperator ), AsString(lValue) );
	
	iSel = m_cbMaskArea.GetCurSel();
	lValue = (long) m_cbMaskArea.GetItemData( iSel );
	//pShapeMaskHelper->m_Data.evoMaskArea.SetValue(1, lValue);
	AddInputRequest( SVValueObjectReference( &pShapeMaskHelper->m_Data.evoMaskArea ), AsString(lValue) );
	
	iSel = m_cbFillOptions.GetCurSel();
	lValue = (long) m_cbFillOptions.GetItemData( iSel );
	//m_pMask->m_Data.evoFillArea.SetValue(1, lValue);
	AddInputRequest( SVValueObjectReference( &m_pMask->m_Data.evoFillArea ), AsString(lValue) );

	lValue = atol(m_sFillColor);
	//m_pMask->m_Data.lvoFillColor.SetValue(1, lValue);
	AddInputRequest( SVValueObjectReference( &m_pMask->m_Data.lvoFillColor ), AsString(lValue) );


	////////////////////////////////////////////////////////
	// SET SHAPE PROPERTIES
	//SVMaskShape* l_pShape = GetCurrentShape();
	//SVMaskShape::MapType mapProperties;
	//l_pShape->GetProperties( mapProperties );

	AddInputRequest( &m_pMask->GetShapeHelper()->m_Data.evoShapeType, AsString(m_eShapeType) );
	//m_pMask->GetShapeHelper()->SetShape( m_eShapeType );
	//m_pMask->GetShapeHelper()->SetProperties( mapProperties );
	//SetProperties( mapProperties );
	SetInspectionData();

	UpdateMask(true);
	////////////////////////////////////////////////////////

	
	CDialog::OnOK();
}

HRESULT SVMaskShapeEditorDlg::UpdateMask(bool bResetObject)
{
	GetCurrentShape()->Refresh();	// renders based on new properties
	SetInspectionData();
	if ( bResetObject )
	{
		m_dialogImage.UpdateImageInfo( &(m_pMask->m_MaskBufferInfo),  m_pMask->m_MaskBufferHandlePtr );
	}

	m_dialogImage.refresh(); // refresh display
	return S_OK;
}


int SVMaskShapeEditorDlg::GetPropertyID(GUID guidProperty )
{
	int id = m_mapPropertyIds[ guidProperty ];
	if ( id == 0 )
	{
		do
		{
			id = (rand() % 15000) + ID_BASE;
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
			return iter->first;
	}
	return SVInvalidGUID;
}


void SVMaskShapeEditorDlg::OnSelchangeImageTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CTabCtrl* pTab = dynamic_cast<CTabCtrl*> ( CWnd::FromHandle( pNMHDR->hwndFrom ) );
	ASSERT( pTab );
	if ( pTab )
	{
		int iSel = pTab->GetCurSel();
		TCITEM tcitem;
		tcitem.mask = TCIF_PARAM;
		pTab->GetItem(iSel, &tcitem);
		m_ePage = static_cast<ViewImageEnum> (tcitem.lParam);
		DisplayImagePage();
	}

	*pResult = 0;
}

void SVMaskShapeEditorDlg::DisplayImagePage()
{
	switch ( m_ePage )
	{
		case VIEW_RESULT:
		{
			// Get the Image for the Dialog
			// Get the Image for this tool
			SVImageInfoClass* pImageInfo = reinterpret_cast < SVImageInfoClass*> (::SVSendMessage( m_pTool, SVM_GETFIRST_IMAGE_INFO, NULL, NULL ) );
			if( pImageInfo )
			{
				SVImageClass* l_pImage = NULL;

				pImageInfo->GetOwnerImage( l_pImage );

				m_dialogImage.UpdateImageInfo( l_pImage );
				m_dialogImage.refresh();
			}
			break;
		}// end case VIEW_RESULT:

		case VIEW_MASK:
		{
			m_dialogImage.UpdateImageInfo( &(m_pMask->m_MaskBufferInfo),  m_pMask->m_MaskBufferHandlePtr );
			m_dialogImage.refresh();
			break;
		}// end case VIEW_MASK:

		case VIEW_SOURCE:
		{
			m_dialogImage.UpdateImageInfo( m_pMask->getReferenceImage(), true );
			m_dialogImage.refresh();
			break;
		}// end case VIEW_SOURCE:

	}// end switch ( m_ePage )
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
		long earlGrayTea = (long) GetRValue(rgb);// + GetGValue(rgb) + GetBValue(rgb)) / 3.0 );
		m_sFillColor = AsString(earlGrayTea);
		UpdateData(FALSE);

		// stuff into value object
		AddInputRequest( SVValueObjectReference( &m_pMask->m_Data.lvoFillColor ), m_sFillColor );
		//m_pMask->m_Data.lvoFillColor.SetValue(1, earlGrayTea);

		UpdateMask();	// need to reset object for shape change
	}
	else
	{
		//m_pMask->m_Data.lvoFillColor.SetValue(1, lOriginalVal);
		AddInputRequest( SVValueObjectReference( &m_pMask->m_Data.lvoFillColor ), AsString(lOriginalVal) );
		UpdateMask();	// need to reset object for shape change
	}
}


void SVMaskShapeEditorDlg::OnChangeEditFillColor() 
{
	UpdateData();
	long lColor = atol( m_sFillColor );
	// stuff into value object
	//m_pMask->m_Data.lvoFillColor.SetValue(1, lColor);
	AddInputRequest( SVValueObjectReference( &m_pMask->m_Data.lvoFillColor ), m_sFillColor );
	UpdateMask();	// need to reset object for shape change
}

UINT_PTR CALLBACK SVMaskShapeEditorDlg::ColorDlgHookFn( HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	int iReturnCode = 0;	// by default allow color dlg to process message

	TRACE("ColorDlgHookFn - MSG: %08X, WPARAM: %08X, LPARAM: %08X\n", uiMsg, wParam, lParam);

	CWnd* pwndTmp = CWnd::FromHandle(hdlg);
	ASSERT(pwndTmp);
	CColorDialog* pDlg = dynamic_cast <CColorDialog*> (pwndTmp);
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
				//(dynamic_cast<CColorDialog*>(pwndTmp))->SetCurrentColor(RGB(pColor->lCustData,pColor->lCustData,pColor->lCustData));
				UINT uiSetRGB;
				uiSetRGB = RegisterWindowMessage(SETRGBSTRING);
				::SendMessage( hdlg, uiSetRGB, 0, (LPARAM) RGB(lColor, lColor, lColor) ); 
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
				long earlGrayTea = atol( sText );
				//m_pThis->m_pMask->m_Data.lvoFillColor.SetValue(1, earlGrayTea);
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

void SVMaskShapeEditorDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	if ( point.x == 0 && point.y == 0 )
	{
		// this is a reflected message from the image
		CPoint ptMouse = m_dialogImage.GetMouseCoordinates();
		m_sCoordinates.Format( _T("%d, %d"), ptMouse.x, ptMouse.y );
		UpdateData(FALSE);

		if ( GetCurrentFigureEditor()->IsCaptured() )
		{
			SVRPropertyItem* pItem = m_Tree.GetFocusedItem();
			if ( pItem )
			{
				pItem->CommitChanges();
				pItem->OnCommit();
				pItem->Select(false);
				m_Tree.SetFocusedItem(NULL);
			}

			RefreshProperties();	// property box display


			// redraw shape
			UpdateMask();
		}
	}
	else
	{
		CDialog::OnMouseMove(nFlags, point);
	}
}

void SVMaskShapeEditorDlg::FillComboBox(SVEnumerateValueObjectClass& p_rValueObject, CComboBox* p_pCombo)
{
	ASSERT( p_pCombo != NULL );

	if ( p_pCombo )
	{
		int i=0;
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
			if( lCurrent == ( long ) p_pCombo->GetItemData( i ) )
			{
				p_pCombo->SetCurSel( i );
				break;
			}
		}
	}
}

void SVMaskShapeEditorDlg::OnZoomInButton() 
{
	GetDlgItem(IDC_ZOOM_IN_BUTTON)->SendMessage( WM_KILLFOCUS, NULL, 0 );
	std::vector<double>::iterator iter;
	for ( iter = m_vecZoomLevels.begin(); iter != m_vecZoomLevels.end(); ++iter)
	{
		if ( *iter / 100.0 > m_dZoom )
		{
			m_dZoom = *iter /100.0;
			m_dialogImage.SetZoom( m_dZoom );
			m_cbZoom.SetCurSel(iter - m_vecZoomLevels.begin());
			break;
		}
	}
}

void SVMaskShapeEditorDlg::OnZoomOutButton() 
{
	GetDlgItem(IDC_ZOOM_OUT_BUTTON)->SendMessage( WM_KILLFOCUS, NULL, 0 );
	if ( m_dZoom == m_vecZoomLevels[1]/100.0 )
	{
		m_dZoom = m_vecZoomLevels[0];
		m_dialogImage.SetZoom( m_dZoom );
		m_cbZoom.SetCurSel(0);
	}
	else
	{
		std::vector<double>::reverse_iterator iter;
		for ( iter = m_vecZoomLevels.rbegin(); iter != m_vecZoomLevels.rend(); ++iter)
		{
			if ( *iter / 100.0 < m_dZoom )
			{
				m_dZoom = *iter / 100.0;
				m_dialogImage.SetZoom( m_dZoom );
				m_cbZoom.SetCurSel(iter.base() - m_vecZoomLevels.begin() - 1);
				break;
			}
		}
	}
}

void SVMaskShapeEditorDlg::OnSelchangeComboZoomScale() 
{
	UpdateData();
	CString sText;
	m_cbZoom.GetWindowText( sText );
	int iSel = m_cbZoom.GetCurSel();
	if ( iSel != CB_ERR )
	{
		m_cbZoom.GetLBText( iSel, sText );
		if ( sText == FIT_TO_WINDOW )
		{
			m_dZoom = 0;
			m_dialogImage.SetZoom( 0 );
		}
		else
		{
			StringMunge::KeepChars(&sText, _T("0123456789"));
			double dPercent = atof(sText);
			m_dZoom = dPercent / 100.0;
			m_dialogImage.SetZoom( m_dZoom );
		}
	}
}


void SVMaskShapeEditorDlg::OnEditchangeComboZoomScale() 
{
	UpdateData();
	CString sText;
	m_cbZoom.GetWindowText( sText );
	StringMunge::KeepChars(&sText, _T("0123456789"));
	double dPercent = atof(sText);
	m_dZoom = dPercent / 100.0;
	m_dialogImage.SetZoom( m_dZoom );
	
}


void SVMaskShapeEditorDlg::OnKillFocus(CWnd* pNewWnd) 
{
	CDialog::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here
	
}

void SVMaskShapeEditorDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);
	
	GetDlgItem(IDC_ZOOM_IN_BUTTON)->SendMessage( WM_KILLFOCUS, NULL, 0 );
	GetDlgItem(IDC_ZOOM_OUT_BUTTON)->SendMessage( WM_KILLFOCUS, NULL, 0 );
}

void SVMaskShapeEditorDlg::OnSelchangeComboShape() 
{
	int iSel = m_cbMaskShape.GetCurSel();
	if ( iSel != CB_ERR )
	{
		SVMaskShape* pOldShape = GetCurrentShape();
		SVMaskShape::MapType mapOldProperties;
		pOldShape->GetProperties( mapOldProperties );

		DWORD dwType = m_cbMaskShape.GetItemData(iSel);
		m_eShapeType = static_cast<SVShapeMaskHelperClass::ShapeTypeEnum> (dwType);	// change current shape

		bool bMaintainSizePosition = true;
		if ( bMaintainSizePosition )
			GetCurrentShape()->SetProperties( mapOldProperties ); // reuse common properties; any that don't apply will be ignored
	
		bool bAutoResize = m_bAutoResize ? true : false;
		GetCurrentShape()->SetAutoResize( bAutoResize );

		//SVMaskShape::MapType mapNewProperties;
		//GetCurrentShape()->GetProperties( mapNewProperties );

		AddInputRequest( &m_pMask->GetShapeHelper()->m_Data.evoShapeType, AsString(m_eShapeType) );
		//m_pMask->GetShapeHelper()->SetShape( m_eShapeType );
		//m_pMask->GetShapeHelper()->SetProperties( mapNewProperties );
		//SetProperties( mapNewProperties );
		SetInspectionData();

		UpdateMask(true);	// need to reset object for shape change

		m_dialogImage.SetFigureEditor( GetCurrentFigureEditor() );

		DisplayImagePage();
		BuildPropertyList();
	};

}


void SVMaskShapeEditorDlg::OnSelchangeComboMaskOperator() 
{
	UpdateData();
	int iSel = m_cbMaskOperator.GetCurSel();
	if ( iSel != CB_ERR )
	{
		long lValue = (long) m_cbMaskOperator.GetItemData( iSel );
		//m_pMask->m_Data.evoCurrentMaskOperator.SetValue(1, lValue);
		AddInputRequest( SVValueObjectReference( &m_pMask->m_Data.evoCurrentMaskOperator ), AsString(lValue) );

		UpdateMask(true);
		DisplayImagePage();
	}
}

void SVMaskShapeEditorDlg::OnSelchangeComboMaskArea() 
{
	UpdateData();
	int iSel = m_cbMaskArea.GetCurSel();
	if ( iSel != CB_ERR )
	{
		SVShapeMaskHelperClass* pShapeMaskHelper = dynamic_cast <SVShapeMaskHelperClass*> 
												   ( SVObjectManagerClass::Instance().GetObject(m_pMask->m_guidShapeHelper) );

		long lValue = (long) m_cbMaskArea.GetItemData( iSel );
		//pShapeMaskHelper->m_Data.evoMaskArea.SetValue(1, lValue);
		AddInputRequest( SVValueObjectReference( &pShapeMaskHelper->m_Data.evoMaskArea ), AsString(lValue) );

		UpdateMask();
		DisplayImagePage();
	}
}

void SVMaskShapeEditorDlg::OnSelchangeComboFillOptions() 
{
	UpdateData();
	int iSel = m_cbFillOptions.GetCurSel();
	if ( iSel != CB_ERR )
	{
		long lValue = (long) m_cbFillOptions.GetItemData( iSel );
		//m_pMask->m_Data.evoFillArea.SetValue(1, lValue);
		AddInputRequest( SVValueObjectReference( &m_pMask->m_Data.evoFillArea ), AsString(lValue) );

		UpdateMask();
	}
}

void SVMaskShapeEditorDlg::OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	LPNMPROPTREE pNMPropTree = (LPNMPROPTREE) pNotifyStruct;
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
				mapProperties[guidProperty] = lNewValue;

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
					pCombo->SetItemValue( (unsigned long) lValue );
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

HRESULT SVMaskShapeEditorDlg::BuildPropertyList()
{
	//m_Tree.LockWindowUpdate();	// doesn't make any visible difference

	m_Tree.DeleteAllItems();

    SVRPropertyItem* pRoot = m_Tree.InsertItem(new SVRPropertyItem());
	ASSERT( pRoot );
	pRoot->SetCanShrink(false);
    pRoot->SetInfoText(_T(""));
	pRoot->HideItem();
	pRoot->SetHeight(2);

	SVMaskShape::MapType mapProperties;
	GetCurrentShape()->GetProperties( mapProperties );

	//SVMaskShape::MapType::const_iterator iter;
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
			pItem->SetHeight(16);
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

	//m_Tree.UnlockWindowUpdate();
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
			pChild->SetItemValue( sValue );

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




void SVMaskShapeEditorDlg::OnCheckAutoResize() 
{
	UpdateData();
	bool bAutoResize = m_bAutoResize ? true : false;
	AddInputRequest( SVValueObjectReference( &m_pMask->GetShapeHelper()->m_Data.bvoAutoResize ), AsString(bAutoResize) );
	GetCurrentShape()->SetAutoResize( bAutoResize ); // reuse common properties; any that don't apply will be ignored

	UpdateMask(true);
	RefreshProperties();
	DisplayImagePage();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVMaskShapeEditorDlg.cpp_v  $
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
