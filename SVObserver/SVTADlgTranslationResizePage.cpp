//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Jim Brown
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include <afxctl.h>
#include <afxdialogex.h>
#include <limits>
#include "SVOResource\resource.h"
#include "SVMessage\SVMessage.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVStatusLibrary\MessageManagerResource.h"
#include "SVImageLibrary\SVImageBufferHandleImage.h"
#include "PropertyTree\PROPTREE.H"
#include "SVTADlgTranslationResizePage.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVGuiExtentUpdater.h"
#include "ResizeTool.h"
#pragma endregion Includes

#pragma region Properry Tree Items Enum
enum {	IDC_INPUTLISTTREE = 99,
		IDC_INPUTLISTTREE_ROOT,
		IDC_INPUTLISTTREE_SCALEFACTOR,
		IDC_INPUTLISTTREE_HEIGHTSCALEFACTOR,
		IDC_INPUTLISTTREE_WIDTHSCALEFACTOR,
		IDC_INPUTLISTTREE_INPUTIMAGE,
		IDC_INPUTLISTTREE_ROIHEIGHT,
		IDC_INPUTLISTTREE_ROIWIDTH,
		IDC_INPUTLISTTREE_OUTPUTIMAGE,
		IDC_INPUTLISTTREE_OUTPUTHEIGHT,
		IDC_INPUTLISTTREE_OUTPUTWIDTH,
		IDC_INPUTLISTTREE_OTHER,
		IDC_INPUTLISTTREE_INTERPOLATIONMODE,
		IDC_INPUTLISTTREE_OVERSCAN,
		IDC_INPUTLISTTREE_PERFORMANCE
};
#pragma endregion Properry Tree Items Enum

SVTADlgTranslationResizePage::SVTADlgTranslationResizePage(SVToolAdjustmentDialogSheetClass* Parent, int id)
: CPropertyPage(id)
, m_pTool(nullptr)
, m_ParentDialog(Parent)
, m_HeightScaleFactor(1.0)
, m_WidthScaleFactor(1.0)
, m_SourceHeight(0)
, m_SourceWidth(0)
, m_OutputHeight(0)
, m_OutputWidth(0)
, m_SourceTabHandle(0)
, m_ROITabHandle(0)
, m_OutputTabHandle(0)
{
}

SVTADlgTranslationResizePage::~SVTADlgTranslationResizePage()
{
}

void SVTADlgTranslationResizePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(SVTADlgTranslationResizePage)
	DDX_Control(pDX, IDC_DIALOGIMAGE, m_DialogImage);
	//}}AFX_DATA_MAP
}


BOOL SVTADlgTranslationResizePage::OnKillActive()
{
	HRESULT hr = ExitTabValidation();

	if (SUCCEEDED (hr))
	{
		if (false == CPropertyPage::OnKillActive())
		{
			hr = SVMSG_SVO_5077_COULDNOTVALIDATE;
		}
	}

	bool br = SUCCEEDED (hr); // returning false should make it stay on the 
							  // current tab.

	return br;
}


/////////////////////////////////////////////////////////////////////////////
// SVTADlgTranslationResizePage message handlers

BOOL SVTADlgTranslationResizePage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	HRESULT hr = S_OK;

	if (nullptr == m_ParentDialog)
	{
		//	This should not be able to happen.  I'm not using an ASSERT 
		//	because nobody cares about a couple milliseconds in the GUI.
		hr = SVMSG_SVO_5001_NULLPARENT;
	}
	else
	{
		m_pTool = static_cast <ResizeTool*> (m_ParentDialog->GetTool ());

		if (nullptr == m_pTool)
		{
			hr = SVMSG_SVO_5008_NULLTOOL;
		}
		else
		{
			// This sets the base class 
			//   SVTaskObjectValueInterface::m_psvTaskObject (Tool).  This is 
			//   necessary for performing a RunOnce later.
			SetTaskObject( m_pTool );
			
			m_pTool->BackupInspectionParameters ();

			hr = SetupResizePropertyTree();
			if (S_OK == hr )
			{
				hr = SetupResizeImageControl();
			}
		}
	}

	if (S_OK != hr)
	{
		SvStl::MessageMgrDisplayAndNotify Exception(  SvStl::LogAndDisplay );
		Exception.setMessage( hr, nullptr, StdMessageParams, 5015);
	}

	UpdateData(FALSE); // dialog being initialized.

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL SVTADlgTranslationResizePage::OnSetActive()
{
	UpdatePropertyTreeData();
	UpdateImages();
	return CPropertyPage::OnSetActive();
}

HRESULT SVTADlgTranslationResizePage::AddScaleFactors(SVRPropertyItem* pRoot)
{
	HRESULT hr = S_OK;
	SVRPropertyItem* pGroupItem = m_Tree.InsertItem(new SVRPropertyItem(), pRoot);
	if (nullptr == pGroupItem)
	{
		hr = SVMSG_SVO_5003_COULDNOTINSERTGROUPITEM;
	}
	else
	{
		pGroupItem->SetCtrlID (IDC_INPUTLISTTREE_SCALEFACTOR);
		pGroupItem->SetCanShrink(false);
		pGroupItem->SetLabelText(_T("Scale Factor"));
		pGroupItem->SetInfoText(_T(""));
		pGroupItem->Expand();
		pGroupItem->SetBold(true);
		pGroupItem->SetBackColor(::GetSysColor(COLOR_INACTIVECAPTION));
		pGroupItem->SetForeColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		pGroupItem->SetCanHighlight(false);

		SVRPropertyItemEdit* pEdit = static_cast <SVRPropertyItemEdit*> (m_Tree.InsertItem(new SVRPropertyItemEdit(), pGroupItem));
		if (nullptr == pEdit)
		{
			hr = SVMSG_SVO_5009_COULDNOTINSERTWIDTH;
		}
		else
		{
			pEdit->SetCtrlID( IDC_INPUTLISTTREE_WIDTHSCALEFACTOR );
			pEdit->SetLabelText( _T("Width"));
			pEdit->SetInfoText( _T("Width scale factor modifies the width of the image. < 1 will reduce.  > 1 will expand. Range: > 0 - 1000."));
			pEdit = static_cast <SVRPropertyItemEdit*> (m_Tree.InsertItem(new SVRPropertyItemEdit(), pGroupItem));
			if (!pEdit)
			{
				hr = SVMSG_SVO_5006_COULDNOTINSERTHEIGHT;
			}
			else
			{
				pEdit->SetCtrlID( IDC_INPUTLISTTREE_HEIGHTSCALEFACTOR );
				pEdit->SetLabelText( _T("Height"));
				pEdit->SetInfoText( _T("Height scale factor modifies the height of the image. < 1 will reduce.  > 1 will expand. Range: > 0 - 1000."));
			}
		}
	}
	return hr;
}

HRESULT SVTADlgTranslationResizePage::AddInputImageInfo(SVRPropertyItem* pRoot)
{
	HRESULT hr = S_OK;
	SVRPropertyItem* pGroupItem = m_Tree.InsertItem(new SVRPropertyItem(), pRoot);
	if (nullptr == pGroupItem)
	{
		hr = SVMSG_SVO_5010_COULDNOTINSERTINPUTIMAGEGROUP;
	}
	else
	{
		pGroupItem->SetCtrlID (IDC_INPUTLISTTREE_INPUTIMAGE);
		pGroupItem->SetCanShrink(false);
		pGroupItem->SetLabelText(_T("Input Image ROI Size"));
		pGroupItem->SetInfoText(_T(""));
		pGroupItem->Expand();
		pGroupItem->SetBold(true);
		pGroupItem->SetBackColor(::GetSysColor(COLOR_INACTIVECAPTION));
		pGroupItem->SetForeColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		pGroupItem->SetCanHighlight(false);

		SVRPropertyItemStatic* staticItem = static_cast <SVRPropertyItemStatic*> (m_Tree.InsertItem(new SVRPropertyItemStatic(), pGroupItem));
		if (nullptr == staticItem)
		{
			hr = SVMSG_SVO_5012_COULDNOTINSERTWIDTH;
		}
		else
		{
			staticItem->SetCtrlID( IDC_INPUTLISTTREE_ROIWIDTH );
			staticItem->SetLabelText( _T("Width"));
			staticItem->SetInfoText( _T("Width of input ROI (read only)"));
			staticItem = static_cast <SVRPropertyItemStatic*> (m_Tree.InsertItem(new SVRPropertyItemStatic(), pGroupItem));
			if (nullptr == staticItem)
			{
				hr = SVMSG_SVO_5011_COULDNOTINSERTHEIGHT;
			}
			else
			{
				staticItem->SetCtrlID( IDC_INPUTLISTTREE_ROIHEIGHT );
				staticItem->SetLabelText(_T("Height"));
				staticItem->SetInfoText(_T("Height of input ROI (read only)"));
			}
		}
	}
	return hr;
}

HRESULT SVTADlgTranslationResizePage::AddOutputImageInfo(SVRPropertyItem* pRoot)
{
	HRESULT hr = S_OK;
	SVRPropertyItem* pGroupItem = m_Tree.InsertItem(new SVRPropertyItem(), pRoot);
	if (nullptr == pGroupItem)
	{
		hr = SVMSG_SVO_5013_COULDNOTINSERTOUTPUTIMAGEGROUP;
	}
	else
	{
		pGroupItem->SetCtrlID (IDC_INPUTLISTTREE_OUTPUTIMAGE);
		pGroupItem->SetCanShrink(false);
		pGroupItem->SetLabelText(_T("Output Image Size"));
		pGroupItem->SetInfoText(_T(""));
		pGroupItem->Expand();
		pGroupItem->SetBold(true);
		pGroupItem->SetBackColor(::GetSysColor(COLOR_INACTIVECAPTION));
		pGroupItem->SetForeColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		pGroupItem->SetCanHighlight(false);

		SVRPropertyItemStatic* staticItem = static_cast <SVRPropertyItemStatic*> (m_Tree.InsertItem(new SVRPropertyItemStatic(), pGroupItem));
		if (nullptr == staticItem)
		{
			hr = SVMSG_SVO_5015_COULDNOTINSERTWIDTH;
		}
		else
		{
			staticItem->SetCtrlID(IDC_INPUTLISTTREE_OUTPUTWIDTH);
			staticItem->SetLabelText( _T("Width"));
			staticItem->SetInfoText( _T("Ouput Image width after the Width Scale Factor is applied. (read only)"));
			staticItem->SetItemValue( _T("400") );

			staticItem = static_cast <SVRPropertyItemStatic*> (m_Tree.InsertItem(new SVRPropertyItemStatic(), pGroupItem));
			if (nullptr == staticItem)
			{
				hr = SVMSG_SVO_5014_COULDNOTINSERTHEIGHT;
			}
			else
			{
				staticItem->SetCtrlID(IDC_INPUTLISTTREE_OUTPUTHEIGHT);
				staticItem->SetLabelText( _T("Height"));
				staticItem->SetInfoText( _T("Ouput Image height after the Height Scale Factor is applied. (read only)"));
				staticItem->SetItemValue( _T("300") );
			}
		}
	}
	return hr;
}

HRESULT SVTADlgTranslationResizePage::AddInterpolationMode(SVRPropertyItem* pGroupItem)
{
	HRESULT hr = S_OK;
	SVRPropertyItemCombo* comboItem = static_cast <SVRPropertyItemCombo*> (m_Tree.InsertItem(new SVRPropertyItemCombo(), pGroupItem));
	if (nullptr == comboItem)
	{
		hr = SVMSG_SVO_5035_COULDNOTINSERTINTERPOLATIONMODEITEM;
	}
	else
	{
		comboItem->SetCtrlID( IDC_INPUTLISTTREE_INTERPOLATIONMODE );
		comboItem->SetLabelText( _T("Interpolation Mode"));
		comboItem->SetInfoText( _T("Pulldown menu options: Auto (enlarge is Bilinear, reduce is Average), Bicubic, Bilinear, Nearest neighbor."));

		comboItem->CreateComboBox();

		SVEnumerateValueObjectClass* interpolationMode = m_pTool->getInterpolationMode ();
		CString	enumeratorName;

		interpolationMode->GetEnumeratorName(SVInterpolationModeOptions::InterpolationModeAuto, enumeratorName);
		long insertedIndex = comboItem->AddString(enumeratorName);
		comboItem->SetItemData(insertedIndex, SVInterpolationModeOptions::InterpolationModeAuto);

		// Average will only work with values less than 1.  
		// Auto will already use Average for values less than 1.
		interpolationMode->GetEnumeratorName(SVInterpolationModeOptions::InterpolationModeBicubic, enumeratorName);
		insertedIndex = comboItem->AddString(enumeratorName);
		comboItem->SetItemData(insertedIndex, SVInterpolationModeOptions::InterpolationModeBicubic);

		interpolationMode->GetEnumeratorName(SVInterpolationModeOptions::InterpolationModeBilinear, enumeratorName);
		insertedIndex = comboItem->AddString(enumeratorName);
		comboItem->SetItemData(insertedIndex, SVInterpolationModeOptions::InterpolationModeBilinear);

		interpolationMode->GetEnumeratorName(SVInterpolationModeOptions::InterpolationModeNearestNeighbor, enumeratorName);
		insertedIndex = comboItem->AddString(enumeratorName);
		comboItem->SetItemData(insertedIndex, SVInterpolationModeOptions::InterpolationModeNearestNeighbor);

		comboItem->SetItemValue(SVInterpolationModeOptions::InterpolationModeAuto); // Currently selected value.
	}
	return hr;
}

HRESULT SVTADlgTranslationResizePage::AddOverScan(SVRPropertyItem* pGroupItem)
{
	HRESULT hr = S_OK;
	SVRPropertyItemCombo* comboItem = static_cast <SVRPropertyItemCombo*> (m_Tree.InsertItem(new SVRPropertyItemCombo(), pGroupItem));
	if (nullptr == comboItem)
	{
		hr = SVMSG_SVO_5036_COULDNOTINSERTOVERSCANITEM;
	}
	else
	{
		comboItem->SetCtrlID( IDC_INPUTLISTTREE_OVERSCAN );
		comboItem->SetLabelText( _T("Overscan"));
		comboItem->SetInfoText( _T("Pulldown menu options: Enabled - use pixels from outside ROI for interpolation (if available), Disabled."));

		comboItem->CreateComboBox();

		SVEnumerateValueObjectClass* overscan = m_pTool->getOverscan ();
		CString	enumeratorName;

		overscan->GetEnumeratorName(SVOverscanOptions::OverscanEnable, enumeratorName);
		long insertedIndex = comboItem->AddString(enumeratorName);
		comboItem->SetItemData (insertedIndex, SVOverscanOptions::OverscanEnable);

		overscan->GetEnumeratorName(SVOverscanOptions::OverscanDisable, enumeratorName);
		insertedIndex = comboItem->AddString(enumeratorName);
		comboItem->SetItemData (insertedIndex, SVOverscanOptions::OverscanDisable);

		comboItem->SetItemValue (SVOverscanOptions::OverscanEnable); // Currently selected value.
	}
	return hr;
}

HRESULT SVTADlgTranslationResizePage::AddPerformance(SVRPropertyItem* pGroupItem)
{
	HRESULT hr = S_OK;
	SVRPropertyItemCombo* comboItem = static_cast <SVRPropertyItemCombo*> (m_Tree.InsertItem(new SVRPropertyItemCombo(), pGroupItem));
	if (nullptr == comboItem)
	{
		hr = SVMSG_SVO_5037_COULDNOTINSERTPERFORMANCEITEM;
	}
	else
	{
		comboItem->SetCtrlID( IDC_INPUTLISTTREE_PERFORMANCE	);
		comboItem->SetLabelText( _T("Performance"));
		comboItem->SetInfoText( _T("Pulldown menu options: Fast, Precise."));

		comboItem->CreateComboBox();

		SVEnumerateValueObjectClass* performance = m_pTool->getPerformance ();
		CString	enumeratorName;

		performance->GetEnumeratorName(SVPerformanceOptions::PerformanceFast, enumeratorName);
		long insertedIndex = comboItem->AddString(enumeratorName);
		comboItem->SetItemData (insertedIndex, SVPerformanceOptions::PerformanceFast);

		performance->GetEnumeratorName(SVPerformanceOptions::PerformancePresice, enumeratorName);
		insertedIndex = comboItem->AddString(enumeratorName);
		comboItem->SetItemData (insertedIndex, SVPerformanceOptions::PerformancePresice);

		comboItem->SetItemValue (SVPerformanceOptions::PerformancePresice); // Currently selected value.
	}
	return hr;
}

HRESULT SVTADlgTranslationResizePage::AddOtherInfo(SVRPropertyItem* pRoot)
{
	HRESULT hr = S_OK;
	// Add Other group
	SVRPropertyItem* pGroupItem = m_Tree.InsertItem(new SVRPropertyItem(), pRoot);
	if (nullptr == pGroupItem)
	{
		hr = SVMSG_SVO_5038_COULDNOTINSERTOTHERITEM;
	}
	else
	{
		pGroupItem->SetCtrlID (IDC_INPUTLISTTREE_OTHER);
		pGroupItem->SetCanShrink(false);//(true); // Comboboxes in expand/shrink groups do not work correctly when collapsing
		pGroupItem->SetLabelText(_T("Other"));
		pGroupItem->SetInfoText(_T(""));
		pGroupItem->Expand(); // so all the children are visible
		pGroupItem->SetBold(true);
		pGroupItem->SetBackColor(::GetSysColor(COLOR_INACTIVECAPTION));
		pGroupItem->SetForeColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		pGroupItem->SetCanHighlight(false);

		hr = AddInterpolationMode(pGroupItem);
		if (S_OK == hr)
		{
			hr = AddOverScan(pGroupItem);
		
			if (S_OK == hr)
			{
				hr = AddPerformance(pGroupItem);
			}
		}
	}
	return hr;
}

HRESULT SVTADlgTranslationResizePage::SetupResizePropertyTree ()
{
	HRESULT	hr = S_OK;

	DWORD dwStyle;
	CRect rc;

	// PTS_NOTIFY - SVRPropTree will send notification messages to the parent window
	dwStyle = WS_CHILD|WS_VISIBLE|PTS_NOTIFY;

	// Init the control's size to cover the entire client area
	// Use dummy control for editor placement of Tree/list control.
	CWnd*	dummyControl = GetDlgItem(IDC_PROPERTY_TREE);
	if (nullptr == dummyControl)
	{
		hr = SVMSG_SVO_5002_NULLPROPERTYTREE_AREA;
	}
	else
	{
		dummyControl->GetWindowRect(rc);
		ScreenToClient(rc);

		// Create SVRPropTree control
		if (FALSE == m_Tree.Create(dwStyle, rc, this, IDC_INPUTLISTTREE))
		{
			hr = SVMSG_SVO_5004_COULDNOTCREATE;
		}
	}

	if (S_OK == hr)
	{
		m_Tree.SetColumn(150);

		SVRPropertyItem* pRoot = m_Tree.InsertItem(new SVRPropertyItem());
		if (nullptr == pRoot)
		{
			hr = SVMSG_SVO_5005_COULDNOTCREATE;
		}
		else
		{
			pRoot->SetCanShrink(false);
			pRoot->SetLabelText(_T(""));
			pRoot->SetInfoText(_T(""));
			pRoot->SetCtrlID (IDC_INPUTLISTTREE_ROOT);

			hr = AddScaleFactors(pRoot);
		}
		if (S_OK == hr)
		{
			hr = AddInputImageInfo(pRoot);
		
			if (S_OK == hr)
			{
				hr = AddOutputImageInfo(pRoot);
			
				if (S_OK == hr)
				{
					hr = AddOtherInfo(pRoot);
				}
			}
		}
		if (S_OK == hr)
		{
			hr = UpdatePropertyTreeData ();
		
			if (S_OK == hr)
			{
				if (nullptr == m_ParentDialog)
				{
					//			This should not be able to happen.  I'm not using an ASSERT 
					//			because nobody cares about a couple milliseconds in the GUI.
					hr = SVMSG_SVO_5007_NULLPARENT;
				}
				else
				{
					pRoot->Expand( true );	// needed for redrawing
				}
			}
		}
	}
	return hr;
}

void SVTADlgTranslationResizePage::UpdateScaleFactors(double newWidthScaleFactor, double newHeightScaleFactor)
{
	SVRPropertyItemEdit* editItem = static_cast <SVRPropertyItemEdit*> (m_Tree.FindItem(IDC_INPUTLISTTREE_HEIGHTSCALEFACTOR));

	double oldHeightScaleFactor = 0.0;
	editItem->GetItemValue(oldHeightScaleFactor);
	if (newHeightScaleFactor != oldHeightScaleFactor) // reduces flicker.
	{
		editItem->SetItemValue( newHeightScaleFactor );
		m_HeightScaleFactor = newHeightScaleFactor;
	}

	editItem = static_cast <SVRPropertyItemEdit*> (m_Tree.FindItem(IDC_INPUTLISTTREE_WIDTHSCALEFACTOR));
	double oldWidthScaleFactor = 0.0;
	editItem->GetItemValue(oldWidthScaleFactor);
	if (newWidthScaleFactor != oldWidthScaleFactor) // reduces flicker.
	{
		editItem->SetItemValue( newWidthScaleFactor ); 
		m_WidthScaleFactor = newWidthScaleFactor;
	}
}

void SVTADlgTranslationResizePage::UpdateInputImageInfo(long newInputROIWidth, long newInputROIHeight)
{
	SVRPropertyItemStatic* staticItem = static_cast <SVRPropertyItemStatic*> (m_Tree.FindItem(IDC_INPUTLISTTREE_ROIHEIGHT));
	
	CString	stringValue;
	staticItem->GetItemValue(stringValue);
	long oldInputROIHeight = atol (stringValue);
	if (newInputROIHeight != oldInputROIHeight)
	{
		stringValue.Format ("%d", newInputROIHeight);
		staticItem->SetItemValue( stringValue );
	}

	staticItem = static_cast <SVRPropertyItemStatic*> (m_Tree.FindItem(IDC_INPUTLISTTREE_ROIWIDTH));

	staticItem->GetItemValue(stringValue);
	long oldInputROIWidth = atol (stringValue);
	if (newInputROIWidth != oldInputROIWidth)
	{
		stringValue.Format ("%d", newInputROIWidth);
		staticItem->SetItemValue( stringValue );
	}
}

void SVTADlgTranslationResizePage::UpdateOutputImageInfo(long newOutputWidth, long newOutputHeight)
{
	SVRPropertyItemStatic* staticItem = static_cast <SVRPropertyItemStatic*> (m_Tree.FindItem(IDC_INPUTLISTTREE_OUTPUTHEIGHT));

	CString	stringValue;
	staticItem->GetItemValue(stringValue);
	long oldOutputHeight = atol (stringValue);
	if (newOutputHeight != oldOutputHeight)
	{
		stringValue.Format ("%d", newOutputHeight);
		staticItem->SetItemValue( stringValue );
	}

	staticItem = static_cast <SVRPropertyItemStatic*> (m_Tree.FindItem(IDC_INPUTLISTTREE_OUTPUTWIDTH));

	staticItem->GetItemValue(stringValue);
	long oldOutputWidth = atol (stringValue);
	if (newOutputWidth != oldOutputWidth)
	{
		stringValue.Format ("%d", newOutputWidth);
		staticItem->SetItemValue( stringValue );
	}
}

void SVTADlgTranslationResizePage::UpdateOtherInfo()
{
	SVRPropertyItemCombo* comboItem = static_cast <SVRPropertyItemCombo*> (m_Tree.FindItem(IDC_INPUTLISTTREE_INTERPOLATIONMODE));
	SVEnumerateValueObjectClass* interpolationMode = m_pTool->getInterpolationMode ();
	long oldInterpolationValue;
	long newInterpolationValue;
	interpolationMode->GetValue (newInterpolationValue);
	comboItem->GetItemValue(oldInterpolationValue);
	if (newInterpolationValue != oldInterpolationValue)
	{
		comboItem->SetItemValue( newInterpolationValue );
	}

	comboItem = static_cast <SVRPropertyItemCombo*> (m_Tree.FindItem(IDC_INPUTLISTTREE_OVERSCAN));
	SVEnumerateValueObjectClass* overscan = m_pTool->getOverscan ();
	long oldOverscanValue;
	long newOverscanValue;
	overscan->GetValue (newOverscanValue);
	comboItem->GetItemValue(oldOverscanValue);
	if (newOverscanValue != oldOverscanValue)
	{
		comboItem->SetItemValue( newOverscanValue );
	}

	comboItem = static_cast <SVRPropertyItemCombo*> (m_Tree.FindItem(IDC_INPUTLISTTREE_PERFORMANCE));
	SVEnumerateValueObjectClass* performance = m_pTool->getPerformance ();
	long oldPerformanceValue;
	long newPerformanceValue;
	performance->GetValue (newPerformanceValue);
	comboItem->GetItemValue(oldPerformanceValue);
	if (newPerformanceValue != oldPerformanceValue)
	{
		comboItem->SetItemValue( newPerformanceValue );
	}
}

HRESULT SVTADlgTranslationResizePage::UpdatePropertyTreeData ()
{
	SVImageExtentClass toolImageExtents;
	HRESULT hr = m_pTool->GetImageExtent(toolImageExtents);
	if (S_OK == hr)
	{
		double newHeightScaleFactor = 0.0;
		toolImageExtents.GetExtentProperty(SVExtentPropertyHeightScaleFactor, newHeightScaleFactor);
		double newWidthScaleFactor = 0.0;
		toolImageExtents.GetExtentProperty(SVExtentPropertyWidthScaleFactor, newWidthScaleFactor);
		UpdateScaleFactors(newWidthScaleFactor, newHeightScaleFactor);

		long newInputROIWidth(0);
		long newInputROIHeight(0);
		toolImageExtents.GetExtentProperty(SVExtentPropertyWidth, newInputROIWidth);
		toolImageExtents.GetExtentProperty(SVExtentPropertyHeight, newInputROIHeight);
		UpdateInputImageInfo(newInputROIWidth, newInputROIHeight);

		long newOutputWidth(0);
		long newOutputHeight(0);
		toolImageExtents.GetExtentProperty(SVExtentPropertyOutputWidth, newOutputWidth);
		toolImageExtents.GetExtentProperty(SVExtentPropertyOutputHeight , newOutputHeight);
		UpdateOutputImageInfo(newOutputWidth, newOutputHeight);

		UpdateOtherInfo();
	}
	return hr;
}

HRESULT SVTADlgTranslationResizePage::SetInspectionData()
{
	HRESULT	hr = S_OK;

	bool	extentChanged =			false;
	bool	embeddedChanged =		false;

	double	oldWidthScaleFactor =	0.0;
	double	oldHeightScaleFactor = 0.0;
	long	oldInterpolationModeValue = 0;
	long	oldOverscanValue = 0;
	long	oldPerformanceValue = 0;

	SVRPropertyItemEdit*	editItem = nullptr;
	SVRPropertyItemCombo*	comboItem = nullptr;

	SVImageExtentClass toolImageExtents;

	// Retrieve current values --------------------------------------------------
	hr = m_pTool->GetImageExtent(toolImageExtents);

	if (SUCCEEDED (hr))
	{
		toolImageExtents.GetExtentProperty (SVExtentPropertyHeightScaleFactor, oldHeightScaleFactor);
		toolImageExtents.GetExtentProperty (SVExtentPropertyWidthScaleFactor, oldWidthScaleFactor);

		SVEnumerateValueObjectClass* interpolationMode = m_pTool->getInterpolationMode ();
		interpolationMode->GetValue (oldInterpolationModeValue);

		SVEnumerateValueObjectClass* overscan = m_pTool->getOverscan ();
		overscan->GetValue (oldOverscanValue);

		SVEnumerateValueObjectClass* performance = m_pTool->getPerformance ();
		performance->GetValue (oldPerformanceValue);
	}

	// Validate new values --------------------------------------------------

	double	newHeightScaleFactor =	0.0;
	if (SUCCEEDED (hr))
	{
		editItem = static_cast <SVRPropertyItemEdit*> (m_Tree.FindItem(IDC_INPUTLISTTREE_HEIGHTSCALEFACTOR));
		editItem->GetItemValue(newHeightScaleFactor);
		hr = m_pTool->ValidateScaleFactor (newHeightScaleFactor);
		if (!SUCCEEDED (hr))
		{
			newHeightScaleFactor = oldHeightScaleFactor;
			editItem->SetItemValue(newHeightScaleFactor);
		}
	}

	double	newWidthScaleFactor =	0.0;
	if (SUCCEEDED (hr))
	{
		editItem = static_cast <SVRPropertyItemEdit*> (m_Tree.FindItem(IDC_INPUTLISTTREE_WIDTHSCALEFACTOR));
		editItem->GetItemValue(newWidthScaleFactor);
		hr = m_pTool->ValidateScaleFactor (newWidthScaleFactor);
		if (!SUCCEEDED (hr))
		{
			// Expected codes include...
			//  SVMSG_SVO_5061_SFOUTSIDERANGE
			newWidthScaleFactor = oldWidthScaleFactor;
			editItem->SetItemValue(newWidthScaleFactor);
		}
	}

	SVInterpolationModeOptions::SVInterpolationModeOptionsEnum newInterpolationValue = 
		SVInterpolationModeOptions::InterpolationModeInitialize;
	
	if (SUCCEEDED (hr))
	{
		SVRPropertyItemCombo* comboItem = static_cast <SVRPropertyItemCombo*> (m_Tree.FindItem(IDC_INPUTLISTTREE_INTERPOLATIONMODE));
		comboItem->GetItemValue(*(reinterpret_cast <long*> (&newInterpolationValue)));

		hr = m_pTool->ValidateInterpolation(newInterpolationValue);
		if (!SUCCEEDED (hr))
		{
			comboItem->SetItemValue(oldInterpolationModeValue);
		}
	}

	SVOverscanOptions::SVOverscanOptionsEnum newOverscanValue = 
		SVOverscanOptions::OverscanInitialize;
	if (SUCCEEDED (hr))
	{
		comboItem = static_cast <SVRPropertyItemCombo*> (m_Tree.FindItem(IDC_INPUTLISTTREE_OVERSCAN));
		comboItem->GetItemValue(*(reinterpret_cast <long*> (&newOverscanValue)));

		hr = m_pTool->ValidateOverscan(newOverscanValue);
		if (!SUCCEEDED (hr))
		{
			comboItem->SetItemValue(oldOverscanValue);
		}

	}

	SVPerformanceOptions::SVPerformanceOptionsEnum newPerformanceValue = 
		SVPerformanceOptions::PerformanceInitialize;
	if (SUCCEEDED (hr))
	{
		comboItem = static_cast <SVRPropertyItemCombo*> (m_Tree.FindItem(IDC_INPUTLISTTREE_PERFORMANCE));
		comboItem->GetItemValue(*(reinterpret_cast <long*> (&newPerformanceValue)));
		hr = m_pTool->ValidatePerformance(newPerformanceValue);
		if (!SUCCEEDED (hr))
		{
			comboItem->SetItemValue(oldPerformanceValue);
		}
	}


	// Set new values -------------------------------------------------------
	// Want to do as much error checking as possible before calling 
	// AddInputRequest(), which can not be easily backed out.  Restoring from
	// an error prior to this point is easier than restoring from an error 
	// after this point.
	if (SUCCEEDED (hr) && (newHeightScaleFactor != oldHeightScaleFactor)) // reduces flicker.
	{
		toolImageExtents.SetExtentProperty (SVExtentPropertyHeightScaleFactor, newHeightScaleFactor);
		extentChanged = true;
	}

	if (SUCCEEDED (hr) && (newWidthScaleFactor != oldWidthScaleFactor)) // reduces flicker.
	{
			toolImageExtents.SetExtentProperty (SVExtentPropertyWidthScaleFactor, newWidthScaleFactor);
			extentChanged = true;
	}

	if (SUCCEEDED (hr) && (static_cast <long> (newInterpolationValue) != oldInterpolationModeValue))
	{
		SVValueObjectClass*	interpolationModeVO = m_pTool->getInterpolationMode();
		AddInputRequest(interpolationModeVO, newInterpolationValue);
		embeddedChanged = true;
	}

	if (SUCCEEDED (hr) && (newOverscanValue != oldOverscanValue))
	{
		SVValueObjectClass*	overscanVO = m_pTool->getOverscan();
		AddInputRequest(overscanVO, newOverscanValue);
		embeddedChanged = true;
	}

	if (SUCCEEDED (hr) && (newPerformanceValue != oldPerformanceValue))
	{
		SVValueObjectClass*	performanceVO = m_pTool->getPerformance();
		AddInputRequest(performanceVO, newPerformanceValue);
		embeddedChanged = true;
	}

	if (SUCCEEDED (hr))
	{
		if (true == extentChanged)
		{
			HRESULT hr = SVGuiExtentUpdater::SetImageExtent(m_pTool, toolImageExtents);
		}

		if (true == embeddedChanged)
		{
			HRESULT hr = AddInputRequestMarker();
		}

		if ((true == extentChanged) || (true == embeddedChanged))
		{
			hr = RunOnce(m_pTool);
			if (S_FALSE == hr)
			{
				hr = SVMSG_SVO_5066_CATCHRUNONCESFALSE;
			}

			if (SUCCEEDED (hr))
			{
				hr = UpdateImages();
			}

			if (SUCCEEDED (hr))
			{
				m_pTool->BackupInspectionParameters();
			}
		}
	}

	return hr;
}


bool	SVTADlgTranslationResizePage::QueryAllowExit()
{
	HRESULT hr = ExitTabValidation();
	
	bool br = SUCCEEDED (hr); // returning false should make it stay on the 
							  // current tab.
	return br;
}


HRESULT	SVTADlgTranslationResizePage::ExitTabValidation ()
{
	HRESULT		hr =		S_OK;
	HRESULT		hrTemp =	S_OK;
	BOOL		br =		FALSE;
	LRESULT		lr =		S_OK; // not used.
	bool		alreadyDisplayed = false;

	SVString	tString;

	SVRPropertyItem* item = m_Tree.GetFocusedItem ();

	// if changes in the property are pending, CommitChanges() will fire off 
	// OnItemChanged(). Because OnItemChanged() will in turn go into 
	// RunOnce() and ResetObject() logic, we will attempt to retrieve 
	// we will only be able to track results from the Run Error Code.
	m_pTool->ClearRunError();

	// It is possible for no item to be in focus if the table was not clicked 
	// on.  In which case nothing has changed.
	if (nullptr != item)
	{
		item->CommitChanges();  
	}

	hr = m_pTool->GetRunErrorCode();
	tString = m_pTool->GetRunErrorData();
	
	// above validates the parameters from the translation tab.
	// below validates the whole tool.

	if (SUCCEEDED (hr))
	{
		br = m_pTool->Validate();
		hr = m_pTool->GetValidationErrorCode();
		if ((TRUE == br) != SUCCEEDED (hr))
		{
			hr = SVMSG_SVO_5078_INCONSISTENTDATA;
		}
		else
			if (!SUCCEEDED (hr))
			{
				tString = m_pTool->GetValidationErrorData();
			}
	}
	else
	{
		// anything that failed during the commit should have already been 
		// displayed.
		alreadyDisplayed = true;
	}

	if (SUCCEEDED (hr))
	{

		hr = RunOnce(m_pTool);
		hrTemp = m_pTool->GetRunErrorCode();
		if (hr == hrTemp)
		{
			tString = m_pTool->GetRunErrorData();
		}
	}

	if (!SUCCEEDED (hr))
	{
		DisplayRunError(hr, tString, 5068);
	}

	return hr;
}


HRESULT		SVTADlgTranslationResizePage::DisplayRunError (HRESULT errorCode, SVString errorString, unsigned long programCode)
{
	HRESULT hr = S_OK;

	HRESULT runError = m_pTool->GetRunErrorCode ();
	bool	displayed = m_pTool->GetRunDisplayed ();

	if (errorCode == runError)
	{
		if (true == displayed)
		{
			// do nothing, this error message has already been displayed.
		}
		else
		{
			// this is the Run Error and should be displayed.
			SvStl::MessageMgrDisplayAndNotify Exception(  SvStl::LogAndDisplay );
			Exception.setMessage( errorCode, errorString.c_str(), StdMessageParams, programCode);
			hr = m_pTool->SetRunDisplayed(true);
		}
	}
	else
	{
		// this is not the Run Error and should always be displayed.
		SvStl::MessageMgrDisplayAndNotify Exception(  SvStl::LogAndDisplay );
		Exception.setMessage( errorCode, errorString.c_str(), StdMessageParams, programCode);
	}

	return hr;
}


HRESULT SVTADlgTranslationResizePage::SetupResizeImageControl()
{
	HRESULT	hr = S_OK;

	if (S_OK != m_DialogImage.AddTab(_T("Source ROI"), &m_ROITabHandle))
	{
		hr = SVMSG_SVO_5016_COULDNOTADDTAB;
	}
	else
	{
		if (S_OK != m_DialogImage.AddTab(_T("Result"), &m_OutputTabHandle))
		{
			hr = SVMSG_SVO_5017_COULDNOTADDTAB;
		}
		else
		{
			hr = UpdateImages();
		}
	}
	return hr;
}

HRESULT SVTADlgTranslationResizePage::UpdateImages()
{
	HRESULT hr = S_OK;

	SVSmartHandlePointer inputImageHandle;

	SVImageClass* inputImage = m_pTool->getInputImage();
	if (nullptr == inputImage)
	{
		// if (nullptr == inputImage) should not need checked here.
		// is already checked in SetupTempROIImage().  If this error is
		// occurring but that one did not, then something bad happened 
		// between there and here.
		hr = SVMSG_SVO_5039_COULDNOTGETINPUTIMAGE;
	}
	else
	{
		SVImageClass* logicalROIImage = m_pTool->getLogicalROIImage();
		if (nullptr == logicalROIImage)
		{
			// if (nullptr == logicalROIImage) should not need checked here.
			// is already checked in SetupTempROIImage().  If this error is
			// occurring but that one did not, then something bad happened 
			// between there and here.
			hr = SVMSG_SVO_5040_COULDNOTGETROIIMAGE;
		}
		else
		{
			SVImageClass* outputImage = m_pTool->getOutputImage();
			if (!outputImage)
			{
				hr = SVMSG_SVO_5041_COULDNOTGETOUTPUTIMAGE;
			}
			else
			{
				m_DialogImage.setImage(logicalROIImage, m_ROITabHandle);
				m_DialogImage.setImage(outputImage, m_OutputTabHandle);

				m_DialogImage.Refresh();
			}
		}
	}
	return hr;
}


void SVTADlgTranslationResizePage::OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	LPNMPROPTREE pNMPropTree = reinterpret_cast< LPNMPROPTREE >(pNotifyStruct);
	*plResult = S_OK;  // the OS (DefWindowProc) is not required to process this 
					   // message.

	HRESULT hr = ValidateCurrentTreeData (pNMPropTree->pItem);
}


HRESULT SVTADlgTranslationResizePage::ValidateCurrentTreeData (SVRPropertyItem* item)
{
//	bool bValid = true;

	SVString tString;
	HRESULT hrTemp = S_OK;

	// Some items are checked before the RunOnce, and then the RunOnce is 
	// bypassed.  So the clear must be explicitly called.
	m_pTool->ClearRunError();
	m_pTool->ClearValidationError();

	HRESULT	hr = SetInspectionData();

	if (!SUCCEEDED (hr))
	{
		// SetInspectionData() ends up calling RunOnce and ResetObjects.  
		// Error handling within these processes is not complete. The 
		// maintains members to help pass error data outside these 
		// operations.
		HRESULT hrTemp = m_pTool->GetRunErrorCode();

		if (((SVMSG_SVO_5067_IMAGEALLOCATIONFAILED == hr) ||
			 (SVMSG_SVO_5061_SFOUTSIDERANGE == hr)) &&
			(nullptr != item))
		{
			if (SVMSG_SVO_5067_IMAGEALLOCATIONFAILED == hr)
			{
				// more specific message for this context.
				hr = SVMSG_SVO_5070_IMAGEALLOCATIONFAILED;
			}

			tString = item->GetLabelText();
		}
		else
		if (hr == hrTemp)
		{
			tString = m_pTool->GetRunErrorData();
		}

		DisplayRunError(hr, tString.c_str(), SvOi::ProgCode_5067_ValidateCurrentTreeData);
	}

	hrTemp = hr;


	if (!SUCCEEDED (hr))
	{
		if (item)
		{
			// Attempt to restore to previous values.
			RestorePropertyTreeItemFromBackup (item);
		}

		// It is understood that we are loosing the previous error.
		hr = SetInspectionData ();
	}

	if (SUCCEEDED(hr))
	{
		hr = UpdatePropertyTreeData();

		if (SUCCEEDED (hr))
		{
			// if there was an error setting the properties the first time,
			// and there was no error setting the properties back to the 
			// original values, then preserve the first error (which was 
			// displayed).
			m_pTool->SetRunErrorCode (hrTemp);
			m_pTool->SetRunErrorData (tString);
			m_pTool->SetRunDisplayed(true);
		}
	}

	return hr;
}

HRESULT SVTADlgTranslationResizePage::RestorePropertyTreeItemFromBackup (SVRPropertyItem* pItem)
{
	HRESULT hr = S_OK;

	UINT controlID = pItem->GetCtrlID();

	double	oldWidthScaleFactor =	0.0;
	double	oldHeightScaleFactor = 0.0;
	long	oldInterpolationModeValue = 0;
	long	oldOverscanValue = 0;
	long	oldPerformanceValue = 0;

	SVRPropertyItemEdit* editItem = nullptr;

	m_pTool->GetBackupInspectionParameters (	&oldHeightScaleFactor,
		&oldWidthScaleFactor,
		&oldInterpolationModeValue,
		&oldOverscanValue,
		&oldPerformanceValue);

	switch (controlID)
	{
	case IDC_INPUTLISTTREE_WIDTHSCALEFACTOR:
		{
			pItem->SetItemValue(oldWidthScaleFactor);
			break;
		}
	case IDC_INPUTLISTTREE_HEIGHTSCALEFACTOR:
		{
			pItem->SetItemValue(oldHeightScaleFactor);
			break;
		}
	case IDC_INPUTLISTTREE_INTERPOLATIONMODE:
		{
			pItem->SetItemValue(oldInterpolationModeValue);
			break;
		}
	case IDC_INPUTLISTTREE_OVERSCAN:
		{
			pItem->SetItemValue(oldOverscanValue);
			break;
		}

	case IDC_INPUTLISTTREE_PERFORMANCE:
		{
			pItem->SetItemValue(oldPerformanceValue);
			break;
		}
	default:
		{
			//  ingore for now.
			break;
		}
	}

	return hr;
}


#pragma region Message Map
BEGIN_MESSAGE_MAP(SVTADlgTranslationResizePage, CPropertyPage)
	ON_NOTIFY(PTN_ITEMCHANGED, IDC_INPUTLISTTREE, OnItemChanged)
END_MESSAGE_MAP()
#pragma endregion Message Map
