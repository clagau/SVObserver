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
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVRunControlLibrary/SVRunControlLibrary.h"
#include "SVPatternAnalyzerClass.h"
#include "SVPatAnalyzeSetupDlgSheet.h"
#include "SVTool.h"
#include "SVImageProcessingClass.h"
#include "SVImageClass.h"
#include "SVOMFCLibrary\SVGraphix.h"
#include "SVOMFCLibrary\DisplayHelper.h"
#include "SVImageLibrary\MatroxImageData.h"
#include "SVOGui\SVColor.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace Seidenader::SVOMFCLibrary;

static const int SVMinModelWidth = 4;
static const int SVMinModelHeight = 4;
static const int SVMinModelWidthWithCircular = 25;
static const int SVMinModelHeightWithCircular = 25;

IMPLEMENT_DYNCREATE(SVPatModelPageClass, CPropertyPage)
#pragma endregion Declarations

#pragma region Constructor
SVPatModelPageClass::SVPatModelPageClass()
: CPropertyPage(SVPatModelPageClass::IDD)
{
	//{{AFX_DATA_INIT(SVPatModelPageClass)
	m_strModelName = _T(""); // @TODO:  Move assignments to the constructor initialization list.
	//}}AFX_DATA_INIT
	m_nMaxX = 100;
	m_nMaxY = 100;

	m_sourceImageWidth = 100;
	m_sourceImageHeight = 100;
	m_handleToModelOverlayObject = -1;
	m_handleToCircleOverlayObject = -1;
	m_handleToSquareOverlayObject = -1;
	m_handleToCircleOverlayObject2 = -1;
	m_handleToSquareOverlayObject2 = -1;

	m_nXPos = 0;
	m_nYPos = 0;
	m_lModelWidth = m_sourceImageWidth/2;
	m_lModelHeight = m_sourceImageHeight/2;
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
	UINT nMsgID = 0;
	if (ValidateModelParameters(nMsgID))
	{
		// Should we check if model needs created here?
		CPropertyPage::OnOK();
	}
	else
	{
		AfxMessageBox(nMsgID);
	}
	m_bAllowExit = true;
	if (!m_pPatAnalyzer->IsValidSize())
	{
		if (AfxMessageBox("Model is larger than the ROI of the Window Tool. Do you want to change Model (Yes) or leave dialog (No)?",MB_YESNO ) == IDYES)
		{
			m_bAllowExit = false;
			return;
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
	UINT nMsgID = 0;
	if (!ValidateModelParameters(nMsgID))
	{
		AfxMessageBox(nMsgID);
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

	m_pSheet = static_cast< SVPatAnalyzeSetupDlgSheet* >( GetParent() );

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
	DDX_Control(pDX, IDC_DIALOGIMAGE, m_dialogImage);
	//}}AFX_DATA_MAP
}
#pragma endregion AFX Virtual Methods

#pragma region AFX MSG
void SVPatModelPageClass::OnFileButton()
{
	if ( GetModelFile( TRUE ) ) // @TODO:  Explain the "TRUE".
	{
		ProcessOnKillFocus( IDC_PAT1_FILE_NAME );
	}
}

void SVPatModelPageClass::OnCreateModel()
{
	UpdateData();

	if ( m_pPatAnalyzer != nullptr && GetModelFile( FALSE ) ) // @TODO:  Explain the "FALSE".
	{
		m_pPatAnalyzer->SetModelExtents(static_cast<long>(m_nXPos), static_cast<long>(m_nYPos), m_lModelWidth, m_lModelHeight);
		m_pPatAnalyzer->SetCircularOverscan((m_bCircularOverscan) ? true : false);

		// SEJ - need to check if region is large enough to accommodate Overscan

		if( m_pPatAnalyzer->UpdateModelFromInputImage() )
		{
			SVMatroxBufferInterface::SVStatusCode l_Code;

			// Set the Search parameters
			BOOL bOk = m_pPatAnalyzer->SetSearchParameters() && !( m_pPatAnalyzer->m_patBufferHandlePtr.empty() );
			if (bOk)
			{
				SVFileNameClass svFileName( m_strModelName );

				CString strExt = svFileName.GetExtension();

				// Now save the Model Image buffer to a file
				SVMatroxFileTypeEnum FileFormatID = SVFileMIL; // Set as default.
				if ( strExt.CompareNoCase(_T(".bmp")) == 0)
				{
					FileFormatID = SVFileBitmap;
				}
				else if(strExt.CompareNoCase(_T(".tif")) == 0)
				{
					FileFormatID = SVFileTiff;
				}

				SVMatroxString l_strFileName = m_strModelName;

				SVImageBufferHandleImage l_MilHandle;
				m_pPatAnalyzer->m_patBufferHandlePtr->GetData( l_MilHandle );

				l_Code = SVMatroxBufferInterface::Export( l_MilHandle.GetBuffer(),
															l_strFileName,
															FileFormatID );

				if (l_Code == SVMEE_STATUS_OK)
				{
					
					UINT nMsgID = RestoreModelFromFile();
					if (nMsgID)
					{
						AfxMessageBox(nMsgID);
					}
				}
				else
				{
					AfxMessageBox(IDS_PAT_ALLOC_MODEL_FAILED);
				}
			}
			else
			{
				AfxMessageBox(IDS_PAT_ALLOC_MODEL_FAILED);
			}
		}
		else
		{
			AfxMessageBox(IDS_PAT_ALLOC_MODEL_FAILED);
		}
	}
}

// Kill focus of File name edit control
void SVPatModelPageClass::OnKillFileName()
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

void SVPatModelPageClass::OnCircularOverscanClicked()
{
	// Get Previous State
	BOOL bOverscanState = m_bCircularOverscan;
	UpdateData(true);

	// Check State Change
	if (bOverscanState != m_bCircularOverscan)
	{
		m_pPatAnalyzer->SetCircularOverscan((m_bCircularOverscan) ? true : false);
		// Set Search Parameters (not current ones, but mostly what we started with before invocation of this setup dialog
		m_pPatAnalyzer->SetSearchParameters();
	}
	setOverlay();
}

void SVPatModelPageClass::OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	LPNMPROPTREE pNMPropTree = reinterpret_cast< LPNMPROPTREE >(pNotifyStruct);
	*plResult = S_OK;

	if ( pNMPropTree->pItem )
	{
		SVRPropertyItem* pItem = pNMPropTree->pItem;

		CString sValue;
		long lNewValue;
		pItem->GetItemValue( sValue );
		lNewValue = atol( sValue );

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
				sValue = AsString( m_lModelWidth );
				pItem->SetItemValue( sValue );
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
				sValue = AsString( m_lModelHeight );
				pItem->SetItemValue( sValue );
			}
			break;
		case ModelOriginX_Property:
			if (lNewValue >= 0 && lNewValue <= m_sourceImageWidth-m_lModelWidth)
			{
				m_nXPos = lNewValue;
			}
			else
			{
				sValue = AsString( m_nXPos );
				pItem->SetItemValue( sValue );
			}
			break;
		case ModelOriginY_Property:
			if (lNewValue >= 0 && lNewValue <= m_sourceImageHeight-m_lModelHeight)
			{
				m_nYPos = lNewValue;
			}
			else
			{
				sValue = AsString( m_nYPos );
				pItem->SetItemValue( sValue );
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
	ON_EN_KILLFOCUS(IDC_PAT1_FILE_NAME, OnKillFileName)
	ON_NOTIFY(PTN_ITEMCHANGED, IDC_PROPERTIES, OnItemChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(SVPatModelPageClass, CDialog)
	ON_EVENT(SVPatModelPageClass, IDC_DIALOGIMAGE, 8, SVPatModelPageClass::ObjectChangedExDialogImage, VTS_I4 VTS_I4 VTS_PVARIANT VTS_PVARIANT)
END_EVENTSINK_MAP()
#pragma endregion AFX MSG

void SVPatModelPageClass::ObjectChangedExDialogImage(long Tab, long Handle, VARIANT* ParameterList, VARIANT* ParameterValue)
{
	////////////////////////////////////////////////////////
	// SET SHAPE PROPERTIES
	VariantParamMap ParaMap;
	int count = DisplayHelper::FillParameterMap(ParaMap, ParameterList, ParameterValue);

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
	setOverlay();
}
#pragma endregion Protected Methods

#pragma region Private Methods
bool SVPatModelPageClass::ValidateModelParameters(UINT& nMsgID)
{
	UpdateData(true);

	bool bRetVal = ValidateModelWidth(nMsgID);
	if (bRetVal)
	{
		bRetVal = ValidateModelHeight(nMsgID);
	}

	if (bRetVal)
	{
		bRetVal = ValidateModelFilename(nMsgID);
	}

	return bRetVal;
}

bool SVPatModelPageClass::ValidateModelWidth(UINT& nMsgID)
{
	nMsgID = 0;
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
		nMsgID = IDS_PAT_MODEL_SIZE_ERROR;
	}
	return bRetVal;
}

bool SVPatModelPageClass::ValidateModelHeight(UINT& nMsgID)
{
	nMsgID = 0;
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
		nMsgID = IDS_PAT_MODEL_SIZE_ERROR;
	}
	return bRetVal;
}

bool SVPatModelPageClass::ValidateModelFilename(UINT& nMsgID) // @TODO:  Add actual validation to this method.
{
	nMsgID = 0;
	UpdateData(true);

	// Should we have a model file name ?
	bool bRetVal = true;
	if ( !m_strModelName.IsEmpty() )
	{
		// verify that the file exists
	}
	return bRetVal; // Currently, this is always returning true!
}

/////////////////////////////////////////////////////////////////////////////
// SVPatModelPageClass message handlers
void SVPatModelPageClass::InitializeData()
{
	CString	strValue;

	SVImageExtentClass l_svExtents;
	RECT l_oRect;

	m_pPatAnalyzer->GetTool()->GetImageExtent( l_svExtents );
	l_svExtents.GetOutputRectangle( l_oRect );

	// Get Source Image Extents
	m_sourceImageWidth = l_oRect.right - l_oRect.left;
	m_sourceImageHeight = l_oRect.bottom - l_oRect.top;
	
	long xPos;
	long yPos;
	m_pPatAnalyzer->GetModelExtents(xPos, yPos, m_lModelWidth, m_lModelHeight);

	if(l_oRect.bottom < m_lModelHeight)
	{
		m_lModelHeight = l_oRect.bottom - 1;
	}

	if(l_oRect.right < m_lModelWidth)
	{
		m_lModelWidth = l_oRect.right - 1;
	}

	// Initialize the Slider for X origin
	m_nXPos = static_cast<int>(xPos);
	m_nMaxX = static_cast< int >( l_oRect.right - m_lModelWidth );

	if(m_nXPos > m_nMaxX)
	{
		m_nXPos = m_nMaxX;
	}

	// Initialize the Slider for Y origin
	m_nYPos = static_cast<int>(yPos);
	m_nMaxY = static_cast< int >( l_oRect.bottom - m_lModelHeight );

	if(m_nYPos > m_nMaxY)
	{
		m_nYPos = m_nMaxY;
	}

	m_pPatAnalyzer->GetModelImageFileName( m_strModelName );
	setCircularOverscanCheckboxState();
	UpdateData(false);
}

BOOL SVPatModelPageClass::ProcessOnKillFocus(UINT nId) //@TODO:  Change c-style casts in this method to _cast.
{
	// @TODO:  Move these method calls outside the "if" line.
	if (GetActiveWindow() != (CWnd *)m_pSheet ||
		m_pSheet->GetActiveIndex() != 0)
	{
		return TRUE;
	}

	UINT nMsgID = 0;

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
				nMsgID = RestoreModelFromFile();
			}
			break;
		}
		default:
		{
			break; // Do nothing.
		}
	}

	if (nMsgID)
	{
		AfxMessageBox(nMsgID);
		GetDlgItem(nId)->SetFocus();
		((CEdit *)GetDlgItem(nId))->SetSel(0, -1);
		return FALSE;
	}
	return TRUE;
}

// If an error occurs, return the Error message Id, otherwise return 0;
UINT SVPatModelPageClass::RestoreModelFromFile()
{
	UINT nMsgId = 0;
	UpdateData( true );

	// set analyzer values
	// Set circular overscan State
	m_pPatAnalyzer->SetCircularOverscan((m_bCircularOverscan) ? true : false);

	if ( m_pPatAnalyzer->RestorePattern( m_strModelName, &nMsgId ) )
	{
		// save the offsets
		POINT pos = { m_nXPos, m_nYPos };

		InitializeData();

		// restore the offsets
		m_nXPos = pos.x;
		m_nYPos = pos.y;
		RefreshProperties();
		UpdateData(false);
	}
	else
	{
		return nMsgId;
	}

	setImages();
	return 0;
}

BOOL SVPatModelPageClass::GetModelFile(BOOL bMode)
{
	BOOL bOk = FALSE;

	SVFileNameClass svfncFileName;

	svfncFileName.SetFileType( SV_PATTERN_MODEL_FILE_TYPE );

	//
	// Try to read the current image file path name from registry...
	//
	CString csPath = AfxGetApp()->GetProfileString(	_T( "Settings" ), 
		                                              _T( "SVCFilePath" ), 
		                                              _T("C:\\RUN") );   // Default

	svfncFileName.SetDefaultPathName( csPath );

	UpdateData( TRUE );

	svfncFileName.SetFullFileName( m_strModelName );
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
										svfncFileName.GetPathName() );

		m_strModelName = svfncFileName.GetFullFileName();
		m_strOldModelName.Empty(); // mark it as update needed

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

	CString text;
	SVRPropertyItemEdit* pItem = dynamic_cast <SVRPropertyItemEdit*> ( m_Tree.InsertItem(new SVRPropertyItemEdit(), pRoot) );
	long lValue = m_nXPos;
	CString sValue = AsString(lValue);
	pItem->SetItemValue( sValue );
	pItem->SetCtrlID( ModelOriginX_Property );
	pItem->SetBold( false );
	pItem->SetHeight(16);
	pItem->SetLabelText( "X-Position" );
	pItem->OnRefresh();

	pItem = dynamic_cast <SVRPropertyItemEdit*> ( m_Tree.InsertItem(new SVRPropertyItemEdit(), pRoot) );
	lValue = m_nYPos;
	sValue = AsString(lValue);
	pItem->SetItemValue( sValue );
	pItem->SetCtrlID( ModelOriginY_Property );
	pItem->SetBold( false );
	pItem->SetHeight(16);
	pItem->SetLabelText( "Y-Position" );
	pItem->OnRefresh();

	pItem = dynamic_cast <SVRPropertyItemEdit*> ( m_Tree.InsertItem(new SVRPropertyItemEdit(), pRoot) );
	lValue = m_lModelWidth;
	sValue = AsString(lValue);
	pItem->SetItemValue( sValue );
	pItem->SetCtrlID( ModelWidth_Property );
	pItem->SetBold( false );
	pItem->SetHeight(16);
	text.Format(_T("Width (%d - %d)"),  SVMinModelWidth, m_sourceImageWidth);
	pItem->SetLabelText( text );
	pItem->OnRefresh();

	pItem = dynamic_cast <SVRPropertyItemEdit*> ( m_Tree.InsertItem(new SVRPropertyItemEdit(), pRoot) );
	lValue = m_lModelHeight;
	sValue = AsString(lValue);
	pItem->SetItemValue( sValue );
	pItem->SetCtrlID( ModelHeight_Property );
	pItem->SetBold( false );
	pItem->SetHeight(16);
	text.Format(_T("Height (%d - %d)"), SVMinModelHeight, m_sourceImageHeight);
	pItem->SetLabelText( text );
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

	CString sValue("");
	while ( nullptr != pChild )
	{
		switch (pChild->GetCtrlID())
		{
		case ModelWidth_Property:
			sValue = AsString( m_lModelWidth );
			pChild->SetItemValue( sValue );
			break;
		case ModelHeight_Property:
			sValue = AsString( m_lModelHeight );
			pChild->SetItemValue( sValue );
			break;
		case ModelOriginX_Property:
			sValue = AsString( m_nXPos );
			pChild->SetItemValue( sValue );
			break;
		case ModelOriginY_Property:
			sValue = AsString( m_nYPos );
			pChild->SetItemValue( sValue );
			break;
		}
		pChild = pChild->GetSibling();
	}

	m_Tree.RedrawWindow();
	return S_OK;
}

void SVPatModelPageClass::setImages()
{
	if ( nullptr != m_pPatAnalyzer )
	{
		m_dialogImage.setImage( m_pPatAnalyzer->getInputImage(), 0 );
		MatroxImageData data(m_pPatAnalyzer->m_patBufferHandlePtr);
		m_dialogImage.setImage( &data, 1 );
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
	Parmap[CDSVPictureDisplay::P_AllowEdit] = 7;

	if ( -1 < m_handleToModelOverlayObject )
	{
		m_dialogImage.EditOverlay(0, m_handleToModelOverlayObject, Parmap);
	}
	else
	{
		m_dialogImage.AddOverlay(0, Parmap, &m_handleToModelOverlayObject);
	}
	m_dialogImage.SetEditAllow(0, m_handleToModelOverlayObject, 7);

	if (m_bCircularOverscan)
	{
		setCircularToolOverlay();
		setCircularModelOverlay();
	}
	else
	{
		if (m_handleToCircleOverlayObject != -1)
		{
			m_dialogImage.RemoveOverlay(0, m_handleToCircleOverlayObject);
			m_handleToCircleOverlayObject = -1;
		}
		if (m_handleToCircleOverlayObject2 != -1)
		{
			m_dialogImage.RemoveOverlay(1, m_handleToCircleOverlayObject2);
			m_handleToCircleOverlayObject2 = -1;
		}
		if (m_handleToSquareOverlayObject != -1)
		{
			m_dialogImage.RemoveOverlay(0, m_handleToSquareOverlayObject);
			m_handleToSquareOverlayObject = -1;
		}
		if (m_handleToSquareOverlayObject2 != -1)
		{
			m_dialogImage.RemoveOverlay(1, m_handleToSquareOverlayObject2);
			m_handleToSquareOverlayObject2 = -1;
		}
	}
}

void SVPatModelPageClass::setCircularToolOverlay()
{
	LongParamMap Parmap;
	Parmap[CDSVPictureDisplay::P_Color] = SVColor::Green;
	Parmap[CDSVPictureDisplay::P_SelectedColor] = SVColor::Green;
	Parmap[CDSVPictureDisplay::P_AllowEdit] = 0;
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
		m_dialogImage.EditOverlay(0, m_handleToCircleOverlayObject, Parmap);
	}
	else
	{
		m_dialogImage.AddOverlay(0, Parmap, &m_handleToCircleOverlayObject);
	}

	//Square overlay
	Parmap[CDSVPictureDisplay::P_Type] = CDSVPictureDisplay::RectangleROI;
	Parmap[CDSVPictureDisplay::P_X1] = innerRect.left;
	Parmap[CDSVPictureDisplay::P_Y1] = innerRect.top;
	Parmap[CDSVPictureDisplay::P_X2] = innerRect.right;
	Parmap[CDSVPictureDisplay::P_Y2] = innerRect.bottom;
	if ( -1 < m_handleToSquareOverlayObject )
	{
		m_dialogImage.EditOverlay(0, m_handleToSquareOverlayObject, Parmap);
	}
	else
	{
		m_dialogImage.AddOverlay(0, Parmap, &m_handleToSquareOverlayObject);
	}
}

void SVPatModelPageClass::setCircularModelOverlay()
{
	//Overlay for the model
	if (!m_pPatAnalyzer->m_patBufferHandlePtr->empty())
	{
		long modelWidth = abs(m_pPatAnalyzer->m_patBufferHandlePtr->GetBitmapInfo().GetWidth());
		long modelHeight = abs(m_pPatAnalyzer->m_patBufferHandlePtr->GetBitmapInfo().GetHeight());
		CRect innerRect = SVMatroxPatternInterface::CalculateOverscanInnerRect(CPoint(0, 0), CSize(modelWidth, modelHeight));
		CRect outerRect = SVMatroxPatternInterface::CalculateOverscanOuterRect(CPoint(innerRect.left, innerRect.top), innerRect.Size() );
		LongParamMap Parmap;
		Parmap[CDSVPictureDisplay::P_Color] = SVColor::Green;
		Parmap[CDSVPictureDisplay::P_SelectedColor] = SVColor::Green;
		Parmap[CDSVPictureDisplay::P_AllowEdit] = 0;
		//Circle overlay
		Parmap[CDSVPictureDisplay::P_Type] = CDSVPictureDisplay::EllipseROI;
		Parmap[CDSVPictureDisplay::P_X1] = outerRect.left;
		Parmap[CDSVPictureDisplay::P_Y1] = outerRect.top;
		Parmap[CDSVPictureDisplay::P_X2] = outerRect.right;
		Parmap[CDSVPictureDisplay::P_Y2] = outerRect.bottom;

		if ( -1 < m_handleToCircleOverlayObject2 )
		{
			m_dialogImage.EditOverlay(1, m_handleToCircleOverlayObject2, Parmap);
		}
		else
		{
			m_dialogImage.AddOverlay(1, Parmap, &m_handleToCircleOverlayObject2);
		}

		//Square overlay
		Parmap[CDSVPictureDisplay::P_Type] = CDSVPictureDisplay::RectangleROI;
		Parmap[CDSVPictureDisplay::P_X1] = innerRect.left;
		Parmap[CDSVPictureDisplay::P_Y1] = innerRect.top;
		Parmap[CDSVPictureDisplay::P_X2] = innerRect.right;
		Parmap[CDSVPictureDisplay::P_Y2] = innerRect.bottom;
		if ( -1 < m_handleToSquareOverlayObject2 )
		{
			m_dialogImage.EditOverlay(1, m_handleToSquareOverlayObject2, Parmap);
		}
		else
		{
			m_dialogImage.AddOverlay(1, Parmap, &m_handleToSquareOverlayObject2);
		}
	}  //if (!m_pPatAnalyzer->m_patBufferHandlePtr->empty())
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
#pragma endregion Private Methods

