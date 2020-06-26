//******************************************************************************
//* COPYRIGHT (c) 1999 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMaskEditorDialog
//* .File Name       : $Workfile:   SVMaskEditorDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   03 Jun 2014 13:22:48  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"

#include "SVMaskEditor.h"
#include "SVMaskEditorDialog.h"
#include "SVLibrary\SVGraphix.h"
#include "SVMatroxLibrary\SVMatroxBufferInterface.h"
#include "SVMatroxLibrary\SVMatroxDisplayInterface.h"
#include "SVMatroxLibrary\SVMatroxImageInterface.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVMaskEditorDialogClass dialog


SVMaskEditorDialogClass::SVMaskEditorDialogClass(CWnd* pParent /*=nullptr*/)
	: CDialog(SVMaskEditorDialogClass::IDD, pParent)
{
	EnableAutomation();

	//{{AFX_DATA_INIT(SVMaskEditorDialogClass)
	StrZoom = _T("");
	//}}AFX_DATA_INIT

	HImageBitmap = nullptr;

	currentZoomX = 0L;
	currentZoomY = 0L;
	currentPanX = 0L;
	currentPanY = 0L;

	currentDrawObjectSelection = SVDrawObjectPoint;
	pCurrentDrawObject = nullptr;
	bNewDrawObject = TRUE;
	bDrawActive = FALSE;

	// Default, should be overwritten...
	m_lMilArithmeticOperation = SVImageAnd;
}


void SVMaskEditorDialogClass::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CDialog::OnFinalRelease();
}

void SVMaskEditorDialogClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVMaskEditorDialogClass)
	DDX_Control(pDX, IDC_ARITHMETIC_COMBO, ArithmeticComboCtl);
	DDX_Text(pDX, IDC_ZOOM_VALUES_STATIC, StrZoom);
	DDV_MaxChars(pDX, StrZoom, 25);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVMaskEditorDialogClass, CDialog)
	//{{AFX_MSG_MAP(SVMaskEditorDialogClass)
	ON_BN_CLICKED(IDC_UNDO_BUTTON, OnUndoButton)
	ON_BN_CLICKED(IDC_REDO_BUTTON, OnRedoButton)
	ON_BN_CLICKED(IDC_ZOOM_IN_BUTTON, OnZoomInButton)
	ON_BN_CLICKED(IDC_ZOOM_OUT_BUTTON, OnZoomOutButton)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_PEN_RADIO, OnPenRadio)
	ON_BN_CLICKED(IDC_POLYGON_RADIO, OnPolygonRadio)
	ON_BN_CLICKED(IDC_RECT_RADIO, OnRectRadio)
	ON_BN_CLICKED(IDC_ELLIPSE_RADIO, OnEllipseRadio)
	ON_BN_CLICKED(IDC_FILLED_ELLIPSE_RADIO, OnFilledEllipseRadio)
	ON_BN_CLICKED(IDC_FILLED_RECT_RADIO, OnFilledRectRadio)
	ON_BN_CLICKED(IDC_POLYLINE_RADIO, OnPolylineRadio)
	ON_BN_CLICKED(IDC_FILL_RADIO, OnFillRadio)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_CBN_SELCHANGE(IDC_ARITHMETIC_COMBO, OnSelchangeArithmeticCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(SVMaskEditorDialogClass, CDialog)
	//{{AFX_DISPATCH_MAP(SVMaskEditorDialogClass)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_ISVMaskEditorDialogClass to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {BB21D9EF-E6F9-11D2-A77B-00106F000C7D}
constexpr IID IID_ISVMaskEditorDialogClass =
{0xbb21d9ef, 0xe6f9, 0x11d2, { 0xa7, 0x7b, 0x0, 0x10, 0x6f, 0x0, 0xc, 0x7d }};

BEGIN_INTERFACE_MAP(SVMaskEditorDialogClass, CDialog)
	INTERFACE_PART(SVMaskEditorDialogClass, IID_ISVMaskEditorDialogClass, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVMaskEditorDialogClass message handlers

BOOL SVMaskEditorDialogClass::OnInitDialog()
{
	CDialog::OnInitDialog();

	HRESULT MatroxCode;

	if (HImageBitmap)
	{
		while (1)
		{
			// Try to get a mil image buffer handle...
			if ((milImageBuffer = SVBitmapToMilBuffer(HImageBitmap)).empty())
			{
				break;
			}
			// Try to get a mil display handle...
			MatroxCode = SVMatroxDisplayInterface::CreateDisplay(milDisplay);
			if (S_OK != MatroxCode)
			{
				break;
			}

			// Try to get a mil display buffer handle...
			long width;
			long height;

			MatroxCode = SVMatroxBufferInterface::Get(milImageBuffer, SVSizeX, width);
			MatroxCode = SVMatroxBufferInterface::Get(milImageBuffer, SVSizeY, height);

			MatroxCode = SVMatroxDisplayInterface::Create(milDisplayBuffer, milImageBuffer);
			if (S_OK != MatroxCode)
			{
				break;
			}

			// Try to get a mil mask buffer handle...
			MatroxCode = SVMatroxBufferInterface::Create(milMaskBuffer, milImageBuffer);
			if (S_OK != MatroxCode)
			{
				break;
			}

			// Set mil display features...
			MatroxCode = SVMatroxDisplayInterface::Set(milDisplay, SVDispWindowZoom, (long)SVValueEnable);
			MatroxCode = SVMatroxDisplayInterface::Set(milDisplay, SVDispUpdate, (long)SVValueEnable);
			MatroxCode = SVMatroxDisplayInterface::Set(milDisplay, SVDispWindowMove, (long)SVValueEnable);
			MatroxCode = SVMatroxDisplayInterface::Set(milDisplay, SVDispWindowResize, (long)SVValueEnable);

			// Create Image Window...
			CRect rect;
			CWnd* pWnd = GetDlgItem(IDC_IMAGE_STATIC);
			if (pWnd)
			{
				pWnd->GetClientRect(&rect);
				pWnd->ClientToScreen(&rect);
				ScreenToClient(&rect);
				pWnd->ShowWindow(SW_HIDE);
			}
			else
				rect = CRect(0, 0, 0, 0);


			DisplayWndCtl.PMother = this;
			DisplayWndCtl.Create(nullptr, _T("SVMaskEditorImageWindow"), WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, 0);


			// Select Display...
			MatroxCode = SVMatroxDisplayInterface::SelectWindow(milDisplay, milDisplayBuffer, DisplayWndCtl.m_hWnd);

			// Update zoom factors and string...
			zoom(0);


			// Set Default Mask size to image size...
			// NOTE: Mask size can differ from image size, using the default overlay mode ( e.g. stretch, etc. )
			CRect graRect(0, 0, static_cast<int>(width), static_cast<int>(height));
			GraphixObject.SetGraphixRect(graRect);

			// Update mask and perform first masking...
			prepareMaskBuffer();
			evaluateMask();
			enableButtons();


			// Load Button Icons...
			CButton* pRadio = dynamic_cast<CButton*> (GetDlgItem(IDC_PEN_RADIO));
			HICON hIcon;
			int iconSize = 16;
			if (nullptr != pRadio)
			{
				hIcon = (HICON) ::LoadImage(AfxGetInstanceHandle(),
					MAKEINTRESOURCE(IDI_PEN_ICON),
					IMAGE_ICON,
					iconSize,
					iconSize,
					LR_DEFAULTCOLOR);
				pRadio->SetIcon(hIcon);
				// Default Radio...
				pRadio->SetCheck(1);
			}

			pRadio = dynamic_cast<CButton*> (GetDlgItem(IDC_POLYGON_RADIO));
			if (nullptr != pRadio)
			{
				hIcon = (HICON) ::LoadImage(AfxGetInstanceHandle(),
					MAKEINTRESOURCE(IDI_POLYGON_ICON),
					IMAGE_ICON,
					iconSize,
					iconSize,
					LR_DEFAULTCOLOR);
				pRadio->SetIcon(hIcon);
			}

			pRadio = dynamic_cast<CButton*> (GetDlgItem(IDC_RECT_RADIO));
			if (nullptr != pRadio)
			{
				hIcon = (HICON) ::LoadImage(AfxGetInstanceHandle(),
					MAKEINTRESOURCE(IDI_RECT_ICON),
					IMAGE_ICON,
					iconSize,
					iconSize,
					LR_DEFAULTCOLOR);
				pRadio->SetIcon(hIcon);
			}

			pRadio = dynamic_cast<CButton*> (GetDlgItem(IDC_ELLIPSE_RADIO));
			if (nullptr != pRadio)
			{
				hIcon = (HICON) ::LoadImage(AfxGetInstanceHandle(),
					MAKEINTRESOURCE(IDI_ELLIPSE_ICON),
					IMAGE_ICON,
					iconSize,
					iconSize,
					LR_DEFAULTCOLOR);
				pRadio->SetIcon(hIcon);
			}

			pRadio = dynamic_cast<CButton*> (GetDlgItem(IDC_POLYLINE_RADIO));
			if (nullptr != pRadio)
			{
				hIcon = (HICON) ::LoadImage(AfxGetInstanceHandle(),
					MAKEINTRESOURCE(IDI_POLYLINE_ICON),
					IMAGE_ICON,
					iconSize,
					iconSize,
					LR_DEFAULTCOLOR);
				pRadio->SetIcon(hIcon);
			}

			pRadio = dynamic_cast<CButton*> (GetDlgItem(IDC_FILLED_RECT_RADIO));
			if (nullptr != pRadio)
			{
				hIcon = (HICON) ::LoadImage(AfxGetInstanceHandle(),
					MAKEINTRESOURCE(IDI_FILLED_RECT_ICON),
					IMAGE_ICON,
					iconSize,
					iconSize,
					LR_DEFAULTCOLOR);
				pRadio->SetIcon(hIcon);
			}

			pRadio = dynamic_cast<CButton*> (GetDlgItem(IDC_FILLED_ELLIPSE_RADIO));
			if (nullptr != pRadio)
			{
				hIcon = (HICON) ::LoadImage(AfxGetInstanceHandle(),
					MAKEINTRESOURCE(IDI_FILLED_ELLIPSE_ICON),
					IMAGE_ICON,
					iconSize,
					iconSize,
					LR_DEFAULTCOLOR);
				pRadio->SetIcon(hIcon);
			}

			pRadio = dynamic_cast<CButton*> (GetDlgItem(IDC_FILL_RADIO));
			if (nullptr != pRadio)
			{
				hIcon = (HICON) ::LoadImage(AfxGetInstanceHandle(),
					MAKEINTRESOURCE(IDI_FILL_ICON),
					IMAGE_ICON,
					iconSize,
					iconSize,
					LR_DEFAULTCOLOR);
				pRadio->SetIcon(hIcon);
			}

			// Fill Arithmetic Combo...
			ArithmeticComboCtl.SetItemData(ArithmeticComboCtl.AddString(_T("AND")), SVImageAnd);
			ArithmeticComboCtl.SetItemData(ArithmeticComboCtl.AddString(_T("OR")), SVImageOr);
			ArithmeticComboCtl.SetItemData(ArithmeticComboCtl.AddString(_T("XOR")), SVImageXOr);
			ArithmeticComboCtl.SetItemData(ArithmeticComboCtl.AddString(_T("NOR")), SVImageNor);
			ArithmeticComboCtl.SetItemData(ArithmeticComboCtl.AddString(_T("XNOR")), SVImageXNor);
			ArithmeticComboCtl.SetItemData(ArithmeticComboCtl.AddString(_T("NAND")), SVImageNand);
			ArithmeticComboCtl.SetItemData(ArithmeticComboCtl.AddString(_T("ADD")), SVImageAddSaturation);
			ArithmeticComboCtl.SetItemData(ArithmeticComboCtl.AddString(_T("SUB")), SVImageSubSaturation);
			ArithmeticComboCtl.SetItemData(ArithmeticComboCtl.AddString(_T("MULT")), SVImageMulSaturation);
			ArithmeticComboCtl.SetItemData(ArithmeticComboCtl.AddString(_T("DIV")), SVImageDiv);
			ArithmeticComboCtl.SetItemData(ArithmeticComboCtl.AddString(_T("MIN")), SVImageMin);
			ArithmeticComboCtl.SetItemData(ArithmeticComboCtl.AddString(_T("MAX")), SVImageMax);
			ArithmeticComboCtl.SetItemData(ArithmeticComboCtl.AddString(_T("PASS")), SVImagePass);
			// Set Default Operator...
			for (int i = 0; i < ArithmeticComboCtl.GetCount(); ++i)
			{
				if (m_lMilArithmeticOperation == static_cast<long>(ArithmeticComboCtl.GetItemData(i)))
				{
					ArithmeticComboCtl.SetCurSel(i);
					break;
				}
			}

			// Send data to dialog...
			UpdateData(FALSE);

			return TRUE;
		}


	}

	OnCancel();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void SVMaskEditorDialogClass::OnCancel()
{
	freeMilResources();
	CDialog::OnCancel();
}

void SVMaskEditorDialogClass::OnOK()
{
	freeMilResources();
	CDialog::OnOK();
}


void SVMaskEditorDialogClass::freeMilResources()
{
	if (!milDisplayBuffer.empty())
	{
		SVMatroxDisplayInterface::Deselect(milDisplay);
		//This is required to remove the buffer from the SVMatroxResourceMonitor
		SVMatroxDisplayInterface::Destroy(milDisplayBuffer);
	}

	// Free mil display...
	SVMatroxDisplayInterface::DestroyDisplay(milDisplay);
}


void SVMaskEditorDialogClass::prepareMaskBuffer()
{
	// Get DC...
	SVMatroxBufferInterface::Set(milMaskBuffer, SVBufWindowDCAlloc, (long long)SVValueDefault);
	LONGLONG Handle(0);
	SVMatroxBufferInterface::Get(milMaskBuffer, SVWindowDC, Handle);
	HDC dc = reinterpret_cast<HDC> (Handle);
	if (dc)
	{
		RECT rect;
		rect.left = 0;
		rect.top = 0;
		long l_Temp = 0;
		SVMatroxBufferInterface::Get(milMaskBuffer, SVSizeX, l_Temp);
		rect.right = static_cast<LONG>(l_Temp);
		SVMatroxBufferInterface::Get(milMaskBuffer, SVSizeY, l_Temp);
		rect.bottom = static_cast<LONG>(l_Temp);

		GraphixObject.Draw(dc, rect);

		SVMatroxBufferInterface::Set(milMaskBuffer, SVBufWindowDCFree, static_cast<long long>(SVValueDefault));
	}
}

// Processes the Arithmetic Operation which is specified by m_lMilArithmeticOperation
// using the original image defined by Input:HImageBitmap and 
// prepared Mask Buffer defined by Input:Mask
void SVMaskEditorDialogClass::evaluateMask()
{
	SVMatroxImageInterface::Arithmetic(milDisplayBuffer, milImageBuffer, milMaskBuffer, static_cast<SVImageOperationTypeEnum>(m_lMilArithmeticOperation));
}

void SVMaskEditorDialogClass::zoom(int ZoomOperand)
{
	// Update zoom factors...
	double l_dZoomX = 0.0;
	double l_dZoomY = 0.0;
	SVMatroxDisplayInterface::Get(milDisplay, SVDispZoomFactorX, l_dZoomX);
	SVMatroxDisplayInterface::Get(milDisplay, SVDispZoomFactorY, l_dZoomY);

	currentZoomX = static_cast<long>(l_dZoomX);
	currentZoomY = static_cast<long>(l_dZoomY);

	currentZoomX += ZoomOperand;
	currentZoomY += ZoomOperand;

	if (currentZoomX < -16)
		currentZoomX = -16;

	if (currentZoomY < -16)
		currentZoomY = -16;

	if (currentZoomX > 16)
		currentZoomX = 16;

	if (currentZoomY > 16)
		currentZoomY = 16;

	// Zoom factor must never be 0!
	if (currentZoomX == 0)
		currentZoomX = (ZoomOperand < 0) ? -1 : 1;

	if (currentZoomY == 0)
		currentZoomY = (ZoomOperand < 0) ? -1 : 1;

	// Invalidate complete display area...
	DisplayWndCtl.Invalidate();

	if (!milDisplayBuffer.empty())
	{
		// Check/Calculate for scroll bars...
		CRect rect;
		DisplayWndCtl.GetClientRect(&rect);
		long bufWidth;
		long bufHeight;
		SVMatroxBufferInterface::Get(milDisplayBuffer, SVSizeX, bufWidth);
		SVMatroxBufferInterface::Get(milDisplayBuffer, SVSizeX, bufHeight);

		BOOL bHorz = (currentZoomX >= 0) ? ((bufWidth * currentZoomX) > rect.Width()) : ((bufWidth / (-currentZoomX)) > rect.Width());
		BOOL bVert = (currentZoomY >= 0) ? ((bufHeight * currentZoomY) > rect.Height()) : ((bufHeight / (-currentZoomY)) > rect.Height());

		// Zoom...
		SVMatroxDisplayInterface::Zoom(milDisplay, currentZoomX, currentZoomY);

		// Invalidate complete display area...
		evaluateMask();

		DisplayWndCtl.ClientToScreen(&rect);
		ScreenToClient(&rect);
		RedrawWindow(&rect);
		// Force to redraw...
		UpdateWindow();


		// Update Scroll Bars...
		SCROLLINFO sInfo;
		sInfo.cbSize = sizeof(SCROLLINFO);
		sInfo.fMask = SIF_PAGE | SIF_RANGE;
		sInfo.nPage = 10;
		sInfo.nMin = 0;
		// Update horizontal scroll bar...
		DisplayWndCtl.ShowScrollBar(SB_HORZ, bHorz);
		if (bHorz)
		{
			sInfo.nMax = static_cast<int>(bufWidth);
			DisplayWndCtl.SetScrollInfo(SB_HORZ, &sInfo);
		}
		else
		{
			// Reset x pan...
			if (currentPanX != 0)
				PanX(0);
		}

		// Update vertical scroll bar...
		DisplayWndCtl.ShowScrollBar(SB_VERT, bVert);
		if (bVert)
		{
			sInfo.nMax = static_cast<int>(bufHeight);
			DisplayWndCtl.SetScrollInfo(SB_VERT, &sInfo);
		}
		else
		{
			// Reset y pan...
			if (currentPanY != 0)
				PanY(0);
		}


		// Update zoom value string...
		StrZoom.Format(_T("Zoom X/Y: %ld/%ld"), currentZoomX, currentZoomY);
	}
}

void SVMaskEditorDialogClass::PanY(int OffsetY)
{
	SVMatroxDisplayInterface::Get(milDisplay, SVDispPanX, currentPanX);
	// Update pan values...
	currentPanY = OffsetY;

	DisplayWndCtl.SetScrollPos(SB_VERT, currentPanY);

	// Pan...
	SVMatroxDisplayInterface::Pan(milDisplay, currentPanX, currentPanY);

	// Invalidate complete display area...
	evaluateMask();

	CRect rect;
	DisplayWndCtl.GetClientRect(&rect);
	DisplayWndCtl.ClientToScreen(&rect);
	ScreenToClient(&rect);
	RedrawWindow(&rect);
	// Force to redraw...
	UpdateWindow();

}


void SVMaskEditorDialogClass::PanX(int OffsetX)
{
	SVMatroxDisplayInterface::Get(milDisplay, SVDispPanY, currentPanY);
	// Update pan values...
	currentPanX = OffsetX;

	DisplayWndCtl.SetScrollPos(SB_HORZ, currentPanX);

	// Pan...
	SVMatroxDisplayInterface::Pan(milDisplay, currentPanX, currentPanY);

	// Invalidate complete display area...
	evaluateMask();

	CRect rect;
	DisplayWndCtl.GetClientRect(&rect);
	DisplayWndCtl.ClientToScreen(&rect);
	ScreenToClient(&rect);
	RedrawWindow(&rect);
	// Force to redraw...
	UpdateWindow();

}

SVGraphixDrawObjectClass* SVMaskEditorDialogClass::getDrawObject()
{
	if (!bNewDrawObject)
	{
		ASSERT(pCurrentDrawObject);
		return pCurrentDrawObject;
	}

	// Try to create new graphix draw object and insert
	// the new created object into to graphix canvas...
	pCurrentDrawObject = GraphixObject.GetNewDrawObject(currentDrawObjectSelection);

	if (pCurrentDrawObject)
	{
		bNewDrawObject = FALSE;
		pCurrentDrawObject->SetDrawingColor(RGB(0, 0, 0));
		pCurrentDrawObject->SetBrushStyle(TRUE);
		pCurrentDrawObject->SetPenStyle(PS_SOLID);
	}

	return pCurrentDrawObject;
}

void SVMaskEditorDialogClass::enableButtons()
{
	GetDlgItem(IDC_REDO_BUTTON)->EnableWindow(GraphixObject.canRedoSteps());
	GetDlgItem(IDC_UNDO_BUTTON)->EnableWindow(GraphixObject.canUndoSteps());
}

void SVMaskEditorDialogClass::ScalePoint(CPoint& RPoint, BOOL BDown)
{
	int zX = currentZoomX;
	int zY = currentZoomY;

	if (BDown)
	{
		zX = -zX;
		zY = -zY;
	}

	if (zX)
		RPoint.x = (zX >= 0) ? (RPoint.x * zX) : (RPoint.x / (-zX));

	if (zY)
		RPoint.y = (zY >= 0) ? (RPoint.y * zY) : (RPoint.y / (-zY));

}

void SVMaskEditorDialogClass::OnUndoButton()
{
	GraphixObject.undoOnStep();
	prepareMaskBuffer();
	evaluateMask();
	enableButtons();
}

void SVMaskEditorDialogClass::OnRedoButton()
{
	GraphixObject.redoOnStep();
	prepareMaskBuffer();
	evaluateMask();
	enableButtons();
}

void SVMaskEditorDialogClass::OnZoomInButton()
{
	// Zoom in...
	zoom(+1);

	// Send data to dialog...
	UpdateData(FALSE);
}

void SVMaskEditorDialogClass::OnZoomOutButton()
{
	// Zoom out...
	zoom(-1);

	// Send data to dialog...
	UpdateData(FALSE);
}

void SVMaskEditorDialogClass::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	UpdateData(FALSE);
}

void SVMaskEditorDialogClass::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);

	UpdateData(FALSE);
}

void SVMaskEditorDialogClass::OnPenRadio()
{
	currentDrawObjectSelection = SVDrawObjectPoint;
	bNewDrawObject = TRUE;
}

void SVMaskEditorDialogClass::OnPolygonRadio()
{
	currentDrawObjectSelection = SVDrawObjectPolygon;
	bNewDrawObject = TRUE;
}

void SVMaskEditorDialogClass::OnRectRadio()
{
	currentDrawObjectSelection = SVDrawObjectRect;
	bNewDrawObject = TRUE;
}

void SVMaskEditorDialogClass::OnEllipseRadio()
{
	currentDrawObjectSelection = SVDrawObjectEllipse;
	bNewDrawObject = TRUE;
}

void SVMaskEditorDialogClass::OnFilledEllipseRadio()
{
	currentDrawObjectSelection = SVDrawObjectFilledEllipse;
	bNewDrawObject = TRUE;
}

void SVMaskEditorDialogClass::OnFilledRectRadio()
{
	currentDrawObjectSelection = SVDrawObjectFilledRect;
	bNewDrawObject = TRUE;
}

void SVMaskEditorDialogClass::OnPolylineRadio()
{
	currentDrawObjectSelection = SVDrawObjectPolyline;
	bNewDrawObject = TRUE;
}

void SVMaskEditorDialogClass::OnFillRadio()
{
	currentDrawObjectSelection = SVDrawObjectFill;
	bNewDrawObject = TRUE;
}

void SVMaskEditorDialogClass::OnSelchangeArithmeticCombo()
{
	m_lMilArithmeticOperation = (long)ArithmeticComboCtl.GetItemData(ArithmeticComboCtl.GetCurSel());
	// Update mask and perform masking...
	prepareMaskBuffer();
	evaluateMask();
}


void SVMaskEditorDialogClass::CanvasLButtonDown(UINT , CPoint point)
{
	CRect rect;
	DisplayWndCtl.GetClientRect(&rect);

	// Check point...
	if (rect.PtInRect(point))
	{
		// Regard Zoom Factors...
		ScalePoint(point, TRUE);

		// Add in scroll position.
		CPoint ptPan = CPoint(currentPanX, currentPanY);
		//		ScalePoint(ptPan, TRUE);

		StrZoom.Format(_T("X: %d, Y: %d"), point.x, point.y);

		// Regard Overlay Mode...
		GraphixObject.ScalePoint(point);
		GraphixObject.ScalePoint(ptPan);

		point += ptPan;

		switch (currentDrawObjectSelection)
		{
			case SVDrawObjectFill:
			case SVDrawObjectPoint:
			{
				if (GraphixObject.CheckPoint(point))
				{
					SVGraphixDrawObjectClass* pPoint = getDrawObject();
					if (pPoint)
						pPoint->AddPoint(point);

					bDrawActive = FALSE;
				}
				bNewDrawObject = TRUE;
			}
			break;

			case SVDrawObjectFilledRect:
			case SVDrawObjectRect:
			case SVDrawObjectFilledEllipse:
			case SVDrawObjectEllipse:
			{
				if (GraphixObject.CheckPoint(point))
				{
					SVGraphixDrawObjectClass* pRect = getDrawObject();
					if (pRect)
					{
						pRect->AddPoint(point);
						bDrawActive = TRUE;
						pRect->SetBrushStyle(FALSE);
					}
				}
				else
					bNewDrawObject = TRUE;
			}
			break;

			case SVDrawObjectPolyline:
			case SVDrawObjectPolygon:
			{
				if (GraphixObject.CheckPoint(point))
				{
					SVGraphixDrawObjectClass* pPoly = getDrawObject();
					if (pPoly)
					{
						pPoly->AddPoint(point);
						bDrawActive = TRUE;
						pPoly->SetBrushStyle(FALSE);
					}
				}
			}
			break;

		};

		// Update mask and perform masking...
		prepareMaskBuffer();
		evaluateMask();

	}

	enableButtons();
	UpdateData(FALSE);
}

void SVMaskEditorDialogClass::CanvasLButtonUp(UINT , CPoint point)
{
	// Regard Zoom Factors...
	ScalePoint(point, TRUE);

	// Add in scroll position.
	CPoint ptPan = CPoint(currentPanX, currentPanY);
	//ScalePoint(ptPan, TRUE);

	StrZoom.Format(_T("X: %d, Y: %d"), point.x, point.y);

	// Regard Overlay Mode...
	GraphixObject.ScalePoint(point);
	GraphixObject.ScalePoint(ptPan);

	point += ptPan;

	switch (currentDrawObjectSelection)
	{
		case SVDrawObjectRect:
		case SVDrawObjectFilledEllipse:
		case SVDrawObjectEllipse:
		case SVDrawObjectFilledRect:
		{
			SVGraphixDrawObjectClass* pRect = getDrawObject();
			if (pRect)
			{
				if (pRect->GetPointCount() > 1)
				{
					pRect->ReplacePoint(point);
				}
				else
					pRect->AddPoint(point);

				pRect->SetBrushStyle(TRUE);
			}
			bNewDrawObject = TRUE;
			bDrawActive = FALSE;
		}
		break;

		case SVDrawObjectPolyline:
		case SVDrawObjectPolygon:
		{
			if (GraphixObject.CheckPoint(point))
			{
				SVGraphixDrawObjectClass* pPoly = getDrawObject();
				if (pPoly)
				{
					pPoly->AddPoint(point);
					bDrawActive = TRUE;
					bNewDrawObject = FALSE;
				}
				else
				{
					bNewDrawObject = TRUE;
					bDrawActive = FALSE;
				}
			}
		}
		break;

	};

	// Update mask and perform masking...
	prepareMaskBuffer();
	evaluateMask();
	enableButtons();

	UpdateData(FALSE);
}


void SVMaskEditorDialogClass::CanvasLButtonDblClk(UINT , CPoint point)
{
	// Regard Zoom Factors...
	ScalePoint(point, TRUE);

	// Add in scroll position.
	CPoint ptPan = CPoint(currentPanX, currentPanY);
	ScalePoint(ptPan, TRUE);

	StrZoom.Format(_T("X: %d, Y: %d"), point.x, point.y);

	// Regard Overlay Mode...
	GraphixObject.ScalePoint(point);
	GraphixObject.ScalePoint(ptPan);

	point += ptPan;

	switch (currentDrawObjectSelection)
	{
		case SVDrawObjectPolygon:
		case SVDrawObjectPolyline:
		{
			// Last point...
			if (GraphixObject.CheckPoint(point))
			{
				SVGraphixDrawObjectClass* pPoly = getDrawObject();
				if (pPoly)
				{
					if (pPoly->GetPointCount() > 1)
					{
						pPoly->ReplacePoint(point);
					}
					else
						pPoly->AddPoint(point);

					pPoly->SetBrushStyle(TRUE);
					bNewDrawObject = TRUE;
					bDrawActive = FALSE;
				}
			}
		}
		break;

	};

	// Update mask and perform masking...
	prepareMaskBuffer();
	evaluateMask();
	enableButtons();

	UpdateData(FALSE);
}

void SVMaskEditorDialogClass::CanvasMouseMove(UINT, CPoint point)
{
	// Regard Zoom Factors...
	ScalePoint(point, TRUE);

	// Add in scroll position.
	CPoint ptPan = CPoint(currentPanX, currentPanY);
	//	ScalePoint(ptPan, TRUE);

	StrZoom.Format(_T("X: %d, Y: %d"), point.x, point.y);

	// Regard Overlay Mode...
	GraphixObject.ScalePoint(point);
	GraphixObject.ScalePoint(ptPan);

	point += ptPan;

	// Check for cursor...
	if (GraphixObject.CheckPoint(point))
	{
		AfxGetApp()->LoadCursor(IDC_CROSS);
	}
	else
		AfxGetApp()->LoadCursor(IDC_ARROW);


	if (bDrawActive)
	{
		switch (currentDrawObjectSelection)
		{
			case SVDrawObjectRect:
			case SVDrawObjectFilledEllipse:
			case SVDrawObjectEllipse:
			case SVDrawObjectFilledRect:
			{
				SVGraphixDrawObjectClass* pRect = getDrawObject();
				if (pRect)
				{
					if (pRect->GetPointCount() > 1)
					{
						pRect->ReplacePoint(point);
					}
					else
						pRect->AddPoint(point);

					bNewDrawObject = FALSE;
					bDrawActive = TRUE;
				}
				else
				{
					bNewDrawObject = TRUE;
					bDrawActive = FALSE;
				}
			}
			break;

			case SVDrawObjectPolyline:
			case SVDrawObjectPolygon:
			{
				SVGraphixDrawObjectClass* pPoly = getDrawObject();
				if (pPoly)
				{
					if (pPoly->GetPointCount() > 1)
					{
						pPoly->ReplacePoint(point);
					}
					else
					{
						pPoly->AddPoint(point);
					}

					bNewDrawObject = FALSE;
					bDrawActive = TRUE;
				}
				else
				{
					bNewDrawObject = TRUE;
					bDrawActive = FALSE;
				}
			}
			break;


		};

		// Update mask and perform masking...
		prepareMaskBuffer();
		evaluateMask();
	}

	UpdateData(FALSE);
}



