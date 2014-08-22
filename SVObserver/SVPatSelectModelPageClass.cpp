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
#include "SVObserver.h"
#include "SVPatternAnalyzerClass.h"
#include "SVPatAnalyzeSetupDlgSheet.h"
#include "SVTool.h"
#include "SVImageProcessingClass.h"
#include "SVImageClass.h"
#include "SVOMFCLibrary\SVGraphix.h"
#include "SVOMFCLibrary\DisplayHelper.h"
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
	if ( m_pPatAnalyzer != NULL )
	{
		m_dialogImage.setImage( m_pPatAnalyzer->getInputImage(), 0 );
		m_dialogImage.setImage( m_pPatAnalyzer->m_patBufferHandlePtr, 1 );
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVPatSelectModelPageClass.cpp_v  $
 * 
 *    Rev 1.3   21 Aug 2014 10:40:44   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  912
 * SCR Title:  Fix issue with Pattern Analyzer if pattern is larger than the ROI of tool
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added OnApply and a check to see if the user wants to exit if model is larger then ROI
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   17 Jul 2014 06:36:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  912
 * SCR Title:  Fix issue with Pattern Analyzer if pattern is larger than the ROI of tool
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   chnaged OnOK to validate size of Model to ROI, if larger display a message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   26 Jun 2014 18:09:46   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   use SVPictureDisplay-control
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:19:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   10 Feb 2011 15:09:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   27 Jan 2011 11:50:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   30 Aug 2010 13:51:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  700
 * SCR Title:  Remove String Buffer Problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update code to remove or fix uses of CString GetBuffer and ReleaseBuffer methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   03 Sep 2009 10:38:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   12 Jan 2009 18:17:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  644
 * SCR Title:  Pattern Analyzer Enhancements and Bug Fixes
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised OnCreateModel and RestoreModelFromFile to save and restore offsets posiitions for drawing figures.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   31 Dec 2008 11:23:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  644
 * SCR Title:  Pattern Analyzer Enhancements and Bug Fixes
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to support Circular Overscan.
 * Revised Field validation logic.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   24 Jul 2007 15:44:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   17 Feb 2005 15:15:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   10 Feb 2004 12:05:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  370
 * SCR Title:  Display buffers have reached maxium number of available
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated OnPaint and OnSetActive methods to fix display update problems.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   19 Jan 2004 10:45:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  370
 * SCR Title:  Display buffers have reached maxium number of available
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed all non-statndard paint functionality in replace of SVDlgImageClass functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   29 Oct 2003 14:04:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  393
 * SCR Title:  Create Model for Pattern Analyzer will save even if you press cancel
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed return type of GetModelFile to return FALSE if the user pressed cancel.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   08 Jul 2003 10:15:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated OnPaint, InitModelImage, and OnInitDialog methods to handle issues with different version of MIL.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   22 Apr 2003 13:52:22   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   06 Mar 2003 13:17:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  328
 * SCR Title:  Upgrade SVObserver to MIL 7.1
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated to work with MIL 7.1
 * MvgaDispXXX no longer works. Create a copy of the DIB instead.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   30 Jan 2003 15:36:34   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated OnPaint, InitModelImage, OnInitDialog, destructor and OnCreateModel methods to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   20 Nov 2002 10:34:36   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Cosmetic changes (member variable names)
 * Use SVImageInfoClass instead of direct MIL handle
 * Destructor, OnInitDialog, InitModelImage
 * Call SVImageProcessingClass method instead of MIL method
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   19 Jul 2001 21:10:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  217
 * SCR Title:  Fix Dr. Watson errors in Patch Match Tool
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated method to get rid of unnecessary compare.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/