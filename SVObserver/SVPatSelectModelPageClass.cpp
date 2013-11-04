//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPatSelectModelPageClass
//* .File Name       : $Workfile:   SVPatSelectModelPageClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:19:58  $
//******************************************************************************

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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int SVMinModelWidth = 4;
static const int SVMinModelHeight = 4;

/////////////////////////////////////////////////////////////////////////////
// helper to generate point set for circle
void Get8Points(SVDrawObjectClass& circle, const POINT& centerPos, const POINT& pos)
{
	circle.AddPoint(CPoint(centerPos.x + pos.x, centerPos.y + pos.y));
	circle.AddPoint(CPoint(centerPos.x + pos.x, centerPos.y - pos.y));
	circle.AddPoint(CPoint(centerPos.x - pos.x, centerPos.y + pos.y));
	circle.AddPoint(CPoint(centerPos.x - pos.x, centerPos.y - pos.y));
	circle.AddPoint(CPoint(centerPos.x + pos.y, centerPos.y + pos.x));
	circle.AddPoint(CPoint(centerPos.x + pos.y, centerPos.y - pos.x));
	circle.AddPoint(CPoint(centerPos.x - pos.y, centerPos.y + pos.x));
	circle.AddPoint(CPoint(centerPos.x - pos.y, centerPos.y - pos.x));
}

/////////////////////////////////////////////////////////////////////////////
// helper to generate point set for circle
void GenerateCirclePoints(SVDrawObjectClass& circle, long radius, const POINT& centerPos)
{
	CPoint pos(0, radius);

	long d = 3 - (2 * radius);		// init decision variable

	while (pos.x < pos.y)
	{
		Get8Points(circle, centerPos, pos);

		if (d < 0)
		{
			d += (4 * pos.x) + 6; // update decision variable
		}
		else
		{
			d += 4 * (pos.x - pos.y) + 10; // update decision variable
			pos.y--;	// Decrement Y
		}
		pos.x++;		// Increment X
	}

	if (pos.x == pos.y)
	{
		Get8Points(circle, centerPos, pos);
	}
}

/////////////////////////////////////////////////////////////////////////////
// SVPatModelPageClass property page

IMPLEMENT_DYNCREATE(SVPatModelPageClass, CPropertyPage)

SVPatModelPageClass::SVPatModelPageClass() 
: CPropertyPage(SVPatModelPageClass::IDD)
{
	//{{AFX_DATA_INIT(SVPatModelPageClass)
	m_nXPos = 0;
	m_nYPos = 0;
	m_lModelWidth = SVMinModelWidth;
	m_lModelHeight = SVMinModelHeight;
	m_strModelName = _T("");
	//}}AFX_DATA_INIT

	m_nMaxX = 100;
	m_nMaxY = 100;

	m_sourceImageWidth = 100;
	m_sourceImageHeight = 100;
}

SVPatModelPageClass::~SVPatModelPageClass()
{
}

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

bool SVPatModelPageClass::ValidateModelFilename(UINT& nMsgID)
{
	nMsgID = 0;
	UpdateData(true);

	// Should we have a model file name ?
	bool bRetVal = true;
	if ( !m_strModelName.IsEmpty() )
	{
		// verify that the file exists
	}
	return bRetVal;
}

void SVPatModelPageClass::ClampModelPositionX()
{
	UpdateData(true);

	if (m_nXPos > m_nMaxX)
	{
		m_nXPos = m_nMaxX;
	}

	m_XSlider.SetPos(m_nXPos);

	UpdateData(false);
}

void SVPatModelPageClass::ClampModelPositionY()
{
	UpdateData(true);
	
	if (m_nYPos > m_nMaxY)
	{
		m_nYPos = m_nMaxY;
	}

	m_YSlider.SetPos(m_nYPos);

	UpdateData(false);
}

void SVPatModelPageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVPatModelPageClass)
	DDX_Control(pDX, IDC_PAT1_PICTURE, m_svToolImage);
	DDX_Control(pDX, IDC_PAT1_MODEL, m_svModelImage);
	DDX_Control(pDX, IDC_PAT_ORIGIN_Y, m_YSlider);
	DDX_Control(pDX, IDC_PAT_ORIGIN_X, m_XSlider);
	DDX_Control(pDX, IDC_PAT_MDL_WIDTH, m_ModelWidthEditCtrl);
	DDX_Control(pDX, IDC_PAT_MDL_HEIGHT, m_ModelHeightEditCtrl);
	DDX_Control(pDX, IDC_PAT_X_POSITION, m_ModelPositionXEditCtrl);
	DDX_Control(pDX, IDC_PAT_Y_POSITION, m_ModelPositionYEditCtrl);
	DDX_Control(pDX, IDC_PAT_CIRCULAR_OVERSCAN, m_CircularOverscanCheckbox);
	DDX_Check(pDX, IDC_PAT_CIRCULAR_OVERSCAN, m_bCircularOverscan);
	DDX_Text(pDX, IDC_PAT_X_POSITION, m_nXPos);
	DDX_Text(pDX, IDC_PAT_Y_POSITION, m_nYPos);
	DDX_Text(pDX, IDC_PAT_MDL_WIDTH, m_lModelWidth);
	DDX_Text(pDX, IDC_PAT_MDL_HEIGHT, m_lModelHeight);
	DDX_Text(pDX, IDC_PAT1_FILE_NAME, m_strModelName);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SVPatModelPageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVPatModelPageClass)
	ON_WM_HSCROLL()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_PAT1_FILE_BUTTON, OnFileButton)
	ON_BN_CLICKED(IDC_PAT1_CREATE_MODEL, OnCreateModel)
	ON_BN_CLICKED(IDC_PAT_CIRCULAR_OVERSCAN, OnCircularOverscanClicked)
	ON_EN_KILLFOCUS(IDC_PAT_X_POSITION, OnKillXPosition)
	ON_EN_KILLFOCUS(IDC_PAT_Y_POSITION, OnKillYPosition)
	ON_EN_KILLFOCUS(IDC_PAT_MDL_HEIGHT, OnKillModelHeight)
	ON_EN_KILLFOCUS(IDC_PAT_MDL_WIDTH, OnKillModelWidth)
	ON_EN_KILLFOCUS(IDC_PAT1_FILE_NAME, OnKillFileName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVPatModelPageClass message handlers

BOOL SVPatModelPageClass::OnInitDialog() 
{
	BOOL bOK = TRUE;

	CPropertyPage::OnInitDialog();
	
	m_pSheet = (SVPatAnalyzeSetupDlgSheet *)GetParent();

	if ( m_pPatAnalyzer != NULL )
	{
		m_svToolImage.init( m_pPatAnalyzer->getInputImage() );
		m_svToolImage.refresh();
	}

	InitializeData(); 
	InitModelImage();
	
	SetControlTextLength(); // Set limits for edit control Text
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

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
	
	// Update the text on the Dialog with the limits
	CString text;
	text.Format(_T("Model Width (%d - %d Pixels)"), SVMinModelWidth, m_sourceImageWidth);
	CWnd* pWnd = GetDlgItem(IDC_PAT_MDL_WIDTH_TXT);
	pWnd->SetWindowText(text);

	text.Format(_T("Model Height (%d - %d Pixels)"), SVMinModelHeight, m_sourceImageHeight);
	pWnd = GetDlgItem(IDC_PAT_MDL_HEIGHT_TXT);
	pWnd->SetWindowText(text);

	long xPos;
	long yPos;
	m_pPatAnalyzer->GetModelExtents(xPos, yPos, m_lModelWidth, m_lModelHeight);
		
	if(l_oRect.bottom < m_lModelHeight)
		m_lModelHeight = l_oRect.bottom - 1;

	if(l_oRect.right < m_lModelWidth)
		m_lModelWidth = l_oRect.right - 1;

	// Initialize the Slider for X origin
	m_nXPos = static_cast<int>(xPos);
	m_nMaxX = (int)(l_oRect.right - m_lModelWidth);
	
	if(m_nXPos > m_nMaxX)
		m_nXPos = m_nMaxX;

// Initialize the Slider for Y origin
	m_nYPos = static_cast<int>(yPos);
	m_nMaxY = (int)(l_oRect.bottom - m_lModelHeight);
	
	if(m_nYPos > m_nMaxY)
		m_nYPos = m_nMaxY;

	m_XSlider.SetRange(0, m_nMaxX, TRUE);
	m_XSlider.SetPos(m_nXPos);
	m_XSlider.SetPageSize(10);
	strValue.Format("%d", m_nMaxX);
	SetDlgItemText(IDC_PAT_MAX_X, strValue);

	m_YSlider.SetRange(0, m_nMaxY, TRUE);
	m_YSlider.SetPos(m_nYPos);
	m_YSlider.SetPageSize(10);
	strValue.Format("%d", m_nMaxY);
	SetDlgItemText(IDC_PAT_MAX_Y, strValue);

	m_pPatAnalyzer->GetModelImageFileName( m_strModelName );

	UpdateData(false);
}

void SVPatModelPageClass::SetControlTextLength() 
{
	m_ModelWidthEditCtrl.SetLimitText(4);
	m_ModelHeightEditCtrl.SetLimitText(4);
	m_ModelPositionXEditCtrl.SetLimitText(4);
	m_ModelPositionYEditCtrl.SetLimitText(4);
}

void SVPatModelPageClass::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CSliderCtrl *pSlider = (CSliderCtrl *) pScrollBar;
	int iNewPos = pSlider->GetPos();

	switch (nSBCode)
	{
		case SB_LEFT :
		  iNewPos = pSlider->GetRangeMin();
		  break;

		case SB_ENDSCROLL :
		  break;

		case SB_LINELEFT :
		  iNewPos--;
		  break;

		case SB_LINERIGHT :
		  iNewPos++;
		  break;

		case SB_PAGELEFT :
		  iNewPos -= 10;
		  break;

		case SB_PAGERIGHT :
		  iNewPos += 10;
		  break;

		case SB_RIGHT :
		  iNewPos = pSlider->GetRangeMax();
		  break;

		case SB_THUMBPOSITION :
		  iNewPos = nPos;
		  break;

		case SB_THUMBTRACK :
		  iNewPos = nPos;
		  break;
	}
	if (iNewPos < pSlider->GetRangeMin())
		iNewPos = pSlider->GetRangeMin();

	if (iNewPos > pSlider->GetRangeMax())
		iNewPos = pSlider->GetRangeMax();
	
	switch (pSlider->GetDlgCtrlID())
	{
		case IDC_PAT_ORIGIN_X:
		{
			if(m_nXPos != iNewPos)
			{
				m_nXPos = iNewPos;
				UpdateData(false);
				DrawModelPosition();
			}
			break;
		}

		case IDC_PAT_ORIGIN_Y:
		{
			if(m_nYPos != iNewPos)
			{
				m_nYPos = iNewPos;
				UpdateData(false);
				DrawModelPosition();
			}
			break;
		}
	}

	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

void SVPatModelPageClass::OnFileButton() 
{
	if ( GetModelFile( true ) )
	{
		ProcessOnKillfocus( IDC_PAT1_FILE_NAME );
	}
}

void SVPatModelPageClass::OnKillXPosition() 
{
	if(ProcessOnKillfocus(IDC_PAT_X_POSITION))
	{
		DrawModelPosition();
	}
}

void SVPatModelPageClass::OnKillYPosition() 
{
	if(ProcessOnKillfocus(IDC_PAT_Y_POSITION))
	{
		DrawModelPosition();
	}
}

void SVPatModelPageClass::OnKillModelHeight() 
{
	if(ProcessOnKillfocus(IDC_PAT_MDL_HEIGHT))
	{
		DrawModelPosition();
	}
}

void SVPatModelPageClass::OnKillModelWidth() 
{
	if(ProcessOnKillfocus(IDC_PAT_MDL_WIDTH))
	{
		DrawModelPosition();
	}
}

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
}

void SVPatModelPageClass::DrawCircularOverscanFigure(SVDlgImageGraphClass& rImage, const CRect& outerRect, const CRect& innerRect, COLORREF color)
{
	if (m_bCircularOverscan)
	{
		SVDrawObjectClass l_svROIFigure;

		SVDrawObjectClass circle;
		circle.SetDrawPen( TRUE, PS_SOLID, 1, color );
		circle.SetDrawSinglePoints(true);

		int radius = min(outerRect.Width(), outerRect.Height()) / 2;
		
		if (radius)
		{
			CPoint centerPos(outerRect.left + (outerRect.Width() / 2), 
							outerRect.top + (outerRect.Height() / 2));
		
			GenerateCirclePoints(circle, radius, centerPos);

			//
			rImage.AddPoints( circle, SVGraphScale );
		}

		l_svROIFigure.SetDrawSinglePoints(false);
		l_svROIFigure.SetDrawPen( TRUE, PS_SOLID, 1, color );

		// Draw Bounding Rect
		l_svROIFigure.SetPointAtGrow( 4, CPoint( innerRect.left, innerRect.top ) );		
		l_svROIFigure.SetPointAtGrow( 0, CPoint( innerRect.left, innerRect.top ) );
		l_svROIFigure.SetPointAtGrow( 1, CPoint( innerRect.right, innerRect.top ) );
		l_svROIFigure.SetPointAtGrow( 2, CPoint( innerRect.right, innerRect.bottom ) );
		l_svROIFigure.SetPointAtGrow( 3, CPoint( innerRect.left, innerRect.bottom ) );

		rImage.AddPoints( l_svROIFigure, SVGraphScale );
	}
}

void SVPatModelPageClass::DrawModelPosition()
{
	m_svToolImage.ClearPoints();

	CRect outerRect(m_nXPos, m_nYPos, m_nXPos + m_lModelWidth, m_nYPos + m_lModelHeight);
	CRect innerRect = SVMatroxPatternInterface::CalculateOverscanInnerRect(CPoint(outerRect.left, outerRect.top), CSize(m_lModelWidth, m_lModelHeight));

	SVDrawObjectClass l_svROIFigure;

	COLORREF color = SV_DEFAULT_GOOD_COLOR;
	l_svROIFigure.SetDrawSinglePoints(false);
	l_svROIFigure.SetDrawPen( TRUE, PS_SOLID, 1, color );

	l_svROIFigure.SetPointAtGrow( 4, CPoint( outerRect.left, outerRect.top ) );		
	l_svROIFigure.SetPointAtGrow( 0, CPoint( outerRect.left, outerRect.top ) );
	l_svROIFigure.SetPointAtGrow( 1, CPoint( outerRect.right, outerRect.top ) );
	l_svROIFigure.SetPointAtGrow( 2, CPoint( outerRect.right, outerRect.bottom ) );
	l_svROIFigure.SetPointAtGrow( 3, CPoint( outerRect.left, outerRect.bottom ) );

	m_svToolImage.AddPoints( l_svROIFigure, SVGraphScale );

	if (m_bCircularOverscan)
	{
		DrawCircularOverscanFigure(m_svToolImage, outerRect, innerRect, color);
	}

	m_svToolImage.refresh();
}

BOOL SVPatModelPageClass::ProcessOnKillfocus(UINT nId) 
{
	if (GetActiveWindow() != (CWnd *)m_pSheet ||
		m_pSheet->GetActiveIndex() != 0)
		return TRUE;
	
	UINT nMsgID = 0;
	
	switch (nId)
	{
		case IDC_PAT_MDL_WIDTH:
		{
			if (ValidateModelWidth(nMsgID))
			{
				AdjustSliderX();
			}
			break;
		}

		case IDC_PAT_MDL_HEIGHT:
		{
			if (ValidateModelHeight(nMsgID))
			{
				AdjustSliderY();
			}
			break;
		}

		case IDC_PAT_X_POSITION:
		{
			ClampModelPositionX();
			break;
		}

		case IDC_PAT_Y_POSITION:
		{
			ClampModelPositionY();
			break;
		}

		case IDC_PAT1_FILE_NAME:
		{
			// if no model name - don't create
			if (m_strModelName.IsEmpty())
				break;

			// check that name has changed
			if (m_strModelName != m_strOldModelName)
			{
				// Extract Model from the file			
				nMsgID = RestoreModelFromFile();
			}
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

void SVPatModelPageClass::AdjustSliderY() 
{
	SVImageExtentClass l_svExtents;
	RECT l_oRect;

	m_pPatAnalyzer->GetTool()->GetImageExtent( l_svExtents );
	l_svExtents.GetOutputRectangle( l_oRect );

	if ( m_pPatAnalyzer != NULL )
	{
		CString	strMax;
		GetDlgItemText(IDC_PAT_MDL_HEIGHT, strMax);
		m_nMaxY = (int)(l_oRect.bottom - m_lModelHeight);
		
		if (m_nMaxY < 1)
		{
			m_nMaxY = 1;
			strMax.Format("%d", (int)(l_oRect.bottom) - 1);
			SetDlgItemText(IDC_PAT_MDL_HEIGHT, strMax);
		}
		
		m_lModelHeight = atol(strMax.Left(10));

		if (m_nYPos > m_nMaxY)
		{
			m_nYPos = m_nMaxY;
			strMax.Format("%d", m_nMaxY);
			SetDlgItemText(IDC_PAT_Y_POSITION, strMax);
		}
		m_YSlider.SetRange(0, m_nMaxY, TRUE);
		m_YSlider.SetPos(m_nYPos);
		strMax.Format("%d", m_nMaxY);
		SetDlgItemText(IDC_PAT_MAX_Y, strMax);
	}
}

void SVPatModelPageClass::AdjustSliderX() 
{
	SVImageExtentClass l_svExtents;
	RECT l_oRect;

	m_pPatAnalyzer->GetTool()->GetImageExtent( l_svExtents );
	l_svExtents.GetOutputRectangle( l_oRect );

// Re-size the X-axis slide Bar.
	if ( m_pPatAnalyzer != NULL )
	{
		CString		strMax;
		GetDlgItemText(IDC_PAT_MDL_WIDTH, strMax);
		m_nMaxX = (int)(l_oRect.right - m_lModelWidth);
		
		if (m_nMaxX < 1)
		{
			m_nMaxX = 1;
			strMax.Format("%d", (int)(l_oRect.right) - 1);
			SetDlgItemText(IDC_PAT_MDL_WIDTH, strMax);
		}

		m_lModelWidth = atol(strMax.Left(10));
		
		if (m_nXPos > m_nMaxX)
		{
			m_nXPos = m_nMaxX;
			strMax.Format("%d", m_nMaxX);
			SetDlgItemText(IDC_PAT_X_POSITION, strMax);
		}
		m_XSlider.SetRange(0, m_nMaxX, TRUE);
		m_XSlider.SetPos(m_nXPos);
		strMax.Format("%d", m_nMaxX);
		SetDlgItemText(IDC_PAT_MAX_X, strMax);
	}
}

void SVPatModelPageClass::InitModelImage() 
{
	if ( m_pPatAnalyzer != NULL )
	{
		m_svModelImage.init( &( m_pPatAnalyzer->m_patBuffer ), m_pPatAnalyzer->m_patBufferHandlePtr );

		m_svModelImage.ClearPoints();

		if (m_bCircularOverscan)
		{
			CRect outerRect(0, 0, m_lModelWidth, m_lModelHeight);
			CRect innerRect = SVMatroxPatternInterface::CalculateOverscanInnerRect(CPoint(outerRect.left, outerRect.top), CSize(m_lModelWidth, m_lModelHeight));

			COLORREF color = SV_DEFAULT_GOOD_COLOR;

			// draw rectangle on center of image
			DrawCircularOverscanFigure(m_svModelImage, outerRect, innerRect, color);
		}

		m_svModelImage.refresh();
	}
}

void SVPatModelPageClass::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	DrawModelPosition();
}

void SVPatModelPageClass::OnCreateModel() 
{
	UpdateData();

	if ( m_pPatAnalyzer != NULL && GetModelFile( FALSE ) )
	{
		m_pPatAnalyzer->SetModelExtents(static_cast<long>(m_nXPos), static_cast<long>(m_nYPos), m_lModelWidth, m_lModelHeight);
		m_pPatAnalyzer->SetCircularOverscan((m_bCircularOverscan) ? true : false);

		// SEJ - need to check if region is large enough to accomodate Overscan

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

		Invalidate();
//		UpdateWindow();
	}
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
		InitModelImage();

		// restore the offsets
		m_nXPos = pos.x;
		m_nYPos = pos.y;
		UpdateData(false);
	}
	else
		return nMsgId;

	Invalidate();
//	UpdateWindow();

	return ((UINT)0);
}

BOOL SVPatModelPageClass::GetModelFile(BOOL	bMode)
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

BOOL SVPatModelPageClass::OnSetActive() 
{
	BOOL l_bOk = CPropertyPage::OnSetActive();

	if ( l_bOk )
	{
		InitializeData();
		InitModelImage();
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

// Kill focus of File name edit control
void SVPatModelPageClass::OnKillFileName() 
{
	if(GetFocus() && (GetFocus()->GetDlgCtrlID() == IDC_PAT1_FILE_BUTTON))
		return;

	// copy current file to old
	m_strOldModelName = m_strModelName;
	UpdateData(true);

	ProcessOnKillfocus(IDC_PAT1_FILE_NAME);
}

void SVPatModelPageClass::OnCircularOverscanClicked()
{
	// Get Previous State
	BOOL bOverscanState = m_bCircularOverscan;
	UpdateData(true);

	// Check State Change
	if (bOverscanState != m_bCircularOverscan)
	{
		m_pPatAnalyzer->SetModelExtents(static_cast<long>(m_nXPos), static_cast<long>(m_nYPos), m_lModelWidth, m_lModelHeight);

		m_pPatAnalyzer->SetCircularOverscan((m_bCircularOverscan) ? true : false);
		
		// Recreate pattern
		m_pPatAnalyzer->UpdateModelFromInputImage();
		
		// Set Search Parameters (not current ones, but mostly what we started with before invocation of this setup dialog
		m_pPatAnalyzer->SetSearchParameters();

		// Refresh the Model View
		InitModelImage();

		// cause a repaint
		Invalidate();
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPatSelectModelPageClass.cpp_v  $
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
