//******************************************************************************
//* COPYRIGHT (c) 2006 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCameraFilePropertyPageDlg
//* .File Name       : $Workfile:   SVCameraFilePropertyPageDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   15 May 2014 09:20:42  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "resource.h"
#include <math.h>
#pragma warning (disable : 4786)
#include "SVCameraFilePropertyPageDlg.h"
#include "SVTestAcquisitionClass.h"
#include "SVImageTest.h"
//#include "SVOMFCLibrary/SVFileNameClass.h"
//#include "SVCameraFormat7Dlg.h"
#include "SVOMFCLibrary/SVDeviceParams.h"
#include "SVOMFCLibrary/SVBoolValueDeviceParam.h"
#include "SVOMFCLibrary/SVi64ValueDeviceParam.h"
#include "SVOMFCLibrary/SVLongValueDeviceParam.h"
#include "SVOMFCLibrary/SVStringValueDeviceParam.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"

#include "SV1394CameraFileLibrary/SV1394CameraParamValidateClass.h"
#include "SVGigeCameraFileLibrary/SVGigeCameraParamValidateClass.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum { IDC_PROPERTYTREE = 100 };

BEGIN_MESSAGE_MAP(SVCameraFilePropertyPageDlg, CDialog)
	//{{AFX_MSG_MAP(SVCameraFilePropertyPageDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
    ON_NOTIFY(PTN_ITEMCHANGED, IDC_PROPERTYTREE, OnItemChanged)
    ON_NOTIFY(PTN_QUERY_SHOW_BUTTON, IDC_PROPERTYTREE, OnItemQueryShowButton)
    ON_NOTIFY(PTN_ITEMBUTTONCLICK, IDC_PROPERTYTREE, OnItemButtonClick)
END_MESSAGE_MAP()

SVCameraFilePropertyPageDlg::SVCameraFilePropertyPageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SVCameraFilePropertyPageDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVCameraFilePropertyPageDlg)
	//}}AFX_DATA_INIT

	m_pAcquisition = NULL;
}

void SVCameraFilePropertyPageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVCameraFilePropertyPageDlg)
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// SVCameraFilePropertyPageDlg message handlers

BOOL SVCameraFilePropertyPageDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	DWORD dwStyle;
	CRect rc;
	
	// PTS_NOTIFY - SVRPropTree will send notification messages to the parent window
	dwStyle = WS_CHILD | WS_VISIBLE | PTS_NOTIFY;
	
	// Init the control's size to cover the entire client area
	GetDlgItem(IDC_PROPERTY_TREE)->GetWindowRect(rc);
	ScreenToClient(rc);
	
	// Create SVRPropTree control
	m_Tree.Create(dwStyle, rc, this, IDC_PROPERTYTREE);
	
	// Create a root item (root items should always be SVRPropertyItem object since they
	// can not have properties
	SVRPropertyItem*        pRoot = NULL;
	SVRPropertyItemFile*    pFile = NULL;
	SVRPropertyItemCombo*   pCombo = NULL;
	SVRPropertyItemEdit*    pEdit = NULL;
	bool                    bResult = false;
	
//	SVFileNameClass FileName;
	
//	FileName.SetFileType(SV_CAMERA_FILE_TYPE);
	CString	rKey;
	
//	SetTitle(GetCameraDisplayName());
	pRoot = m_Tree.InsertItem(new SVRPropertyItem());
	if (pRoot)
	{
		pRoot->SetCanShrink(false);
		pRoot->SetLabelText(_T("Camera Advanced"));
		pRoot->SetInfoText(_T("Advanced camera properties"));
		
		// load options based on camera file
		const SVDeviceParamCollection& rCameraFileParams = GetCameraFileParams();
		const SVDeviceParamCollection& rCameraDeviceParams = GetCameraDeviceParams();
		
		// use sorted access
		SVDeviceParamIndexer index( rCameraFileParams.mapParameters );
		SVDeviceParamIndexer::iterator iter;
		for (iter = index.begin(); iter != index.end(); ++iter)
		{
			const SVDeviceParamWrapper& rCamFileParam = rCameraFileParams.Parameter( *iter );
			const SVDeviceParamWrapper& rCamDeviceParam = rCameraDeviceParams.Parameter( *iter );

			if ( !rCamFileParam.IsValid() || !rCamDeviceParam.IsValid() )
				continue;
			
			const int iDetailLevel = 0;
			if ( rCamFileParam->DetailLevel() > iDetailLevel )
				continue;
			
			switch ( rCamFileParam->DataType() )
			{
				case DeviceDataTypeLong:
				case DeviceDataTypeComplex:
				case DeviceDataTypeString:
					break;

				default:
				{
					pCombo = (SVRPropertyItemCombo*)m_Tree.InsertItem(new SVRPropertyItemCombo(), pRoot);
					if (!pCombo)
						break;
					
					pCombo->SetCtrlID( PROP_CAMERA_FILE_BASE + rCamFileParam->Type() );
					CString sLabel = rCamFileParam->VisualName();
					
					if ( sLabel == _T("") )
						sLabel = rCamFileParam->Name();

					pCombo->SetLabelText( sLabel );
					pCombo->SetInfoText( CString(rCamFileParam->Description()) );
					pCombo->CreateComboBox();
				}
				break;
			}
				
			switch ( rCamFileParam->DataType() )
			{
				case DeviceDataTypeLong:
				{
					const SVLongValueDeviceParam* pCamFileParam = rCamFileParam.DerivedValue(pCamFileParam);
					const SVLongValueDeviceParam* pCamDeviceParam = rCamDeviceParam.DerivedValue(pCamDeviceParam);
					if ( pCamFileParam->info.options.size() > 0 )
					{
						pCombo = (SVRPropertyItemCombo*)m_Tree.InsertItem(new SVRPropertyItemCombo(), pRoot);
						if (!pCombo)
							break;
						
						pCombo->SetCtrlID( PROP_CAMERA_FILE_BASE + rCamFileParam->Type() );
						CString sLabel = rCamFileParam->VisualName();
						if ( sLabel == _T("") )
							sLabel = rCamFileParam->Name();
						pCombo->SetLabelText( sLabel );
						pCombo->SetInfoText( CString(rCamFileParam->Description()) );
						pCombo->CreateComboBox();
						
						SVLongValueDeviceParam::OptionsType::const_iterator iterOption;
						for (iterOption = pCamFileParam->info.options.begin(); iterOption != pCamFileParam->info.options.end(); ++iterOption)
						{
							CString sText = iterOption->strDescription.c_str();
							int iPos;
							iPos = pCombo->AddString( sText );
							pCombo->SetItemData( iPos, iterOption->value );
						}
						if ( pCamFileParam->info.options.size() == 1 )
						{
							pCombo->ReadOnly( true );
						}
						
						pCombo->SetItemValue( pCamDeviceParam->lValue );
					}
					else
					{
						// do a spin control bounded by pParam->info.min / info.max;
						pEdit = (SVRPropertyItemEdit*)m_Tree.InsertItem(new SVRPropertyItemEdit(), pRoot);
						if (!pEdit)
							break;
						
						pEdit->SetCtrlID( PROP_CAMERA_FILE_BASE + rCamFileParam->Type() );
						CString sLabel = rCamFileParam->VisualName();
						if ( sLabel == _T("") )
							sLabel = rCamFileParam->Name();

						sLabel = sLabel + " (" + pCamDeviceParam->info.sUnits + ")";
						pEdit->SetLabelText( sLabel );
						CString sDescription = CString(rCamFileParam->Description());
						sDescription += CString(_T("   Min = ")) + AsString(pCamFileParam->GetScaledMin() ) + CString(_T(", Max = ")) + AsString( pCamFileParam->GetScaledMax() ) + CString(_T("; default = ")) + AsString((long )ceil( pCamFileParam->lValue * pCamFileParam->info.multiplier) );
						pEdit->SetInfoText( sDescription );
						
						pEdit->SetItemValue( pCamDeviceParam->GetScaledValue() );
					}
					break;
				}

				case DeviceDataTypei64:
				{
					const SVi64ValueDeviceParam* pCamFileParam = rCamFileParam.DerivedValue(pCamFileParam);
					break;
				}

				case DeviceDataTypeBool:
				{
					const SVBoolValueDeviceParam* pCamFileParam = rCamFileParam.DerivedValue(pCamFileParam);
					const SVBoolValueDeviceParam* pCamDeviceParam = rCamDeviceParam.DerivedValue(pCamDeviceParam);
					SVBoolValueDeviceParam::OptionsType::const_iterator iterOption;
					for (iterOption = pCamFileParam->info.options.begin(); iterOption != pCamFileParam->info.options.end(); ++iterOption)
					{
						CString sText = iterOption->strDescription.c_str();
						int iPos;
						iPos = pCombo->AddString( sText );
						pCombo->SetItemData( iPos, (LPARAM) iterOption->value );
					}
					if ( pCamFileParam->info.options.size() == 1 )
					{
						pCombo->ReadOnly( true );
					}
					
					pCombo->SetItemValue( pCamDeviceParam->bValue );
					
					break;
				}

				case DeviceDataTypeString:
				{
					switch ( rCamFileParam->Type() )
					{
						case DeviceParamCameraFormats:
						{
							const SVCameraFormatsDeviceParam* pCamFileParam = rCamFileParam.DerivedValue(pCamFileParam);
							const SVCameraFormatsDeviceParam* pCamDeviceParam = rCamDeviceParam.DerivedValue(pCamDeviceParam);
							if ( pCamFileParam->options.size() > 0 )
							{
								pCombo = (SVRPropertyItemCombo*) m_Tree.InsertItem(new SVRPropertyItemCombo(),pRoot);
								if (!pCombo)
									break;
								
								pCombo->SetCtrlID( PROP_CAMERA_FILE_BASE + rCamFileParam->Type() );
								CString sLabel = rCamFileParam->VisualName();
								
								if ( sLabel == _T("") )
									sLabel = rCamFileParam->Name();

								pCombo->SetLabelText( sLabel );
								pCombo->SetInfoText( CString(rCamFileParam->Description()) );
								pCombo->SetButtonText("ROI");
								pCombo->CreateComboBox();
/* SEJ - 								
								// special case to deal with color / mono issues EB 20031107
								bool bColorSystem =    m_pAssistant->GetProductType() == SVIM_PRODUCT_D1_COLOR
									|| m_pAssistant->GetProductType() == SVIM_PRODUCT_D2_COLOR
									|| m_pAssistant->GetProductType() == SVIM_PRODUCT_D3_COLOR
									|| m_pAssistant->GetProductType() == SVIM_PRODUCT_X1_COLOR
									|| m_pAssistant->GetProductType() == SVIM_PRODUCT_X2_GD1A_COLOR;
									|| m_pAssistant->GetProductType() == SVIM_PRODUCT_X2_GD2A_COLOR;
									|| m_pAssistant->GetProductType() == SVIM_PRODUCT_X2_GD4A_COLOR;
									|| m_pAssistant->GetProductType() == SVIM_PRODUCT_X2_GD8A_COLOR;
*/								
								bool bColorSystem = false; // SEJ - no color for now

								SVCameraFormatsDeviceParam::OptionsType::const_iterator iterOption;
								const SVCameraFormat* pFormat=NULL;
								for (iterOption = pCamFileParam->options.begin(); iterOption != pCamFileParam->options.end(); ++iterOption)
								{
									if ( iterOption->second.bColor == bColorSystem )	// if camera format matches product
									{
										CString sText = iterOption->second.strDescription.c_str();
										int iPos;
										iPos = pCombo->AddString( sText );
										pCombo->SetItemData( iPos, reinterpret_cast<DWORD_PTR>(&(iterOption->second)) );
										if ( pCamDeviceParam->strValue == iterOption->second.m_strName )
										{
											pFormat = &(iterOption->second);
										}
									}
								}

								if ( pCamFileParam->options.size() == 1 )
								{
									pCombo->ReadOnly( true );
								}
								
								pCombo->SetItemValue( reinterpret_cast<DWORD_PTR>(pFormat) );
							}
						}
						break;
							
						default:
						{
							const SVStringValueDeviceParam* pCamFileParam = rCamFileParam.DerivedValue(pCamFileParam);
							const SVStringValueDeviceParam* pCamDeviceParam = rCamDeviceParam.DerivedValue(pCamDeviceParam);
							if ( pCamFileParam->info.options.size() > 0 )
							{
								pCombo = (SVRPropertyItemCombo*)m_Tree.InsertItem(new SVRPropertyItemCombo(),pRoot);
								if (!pCombo)
									break;
								
								pCombo->SetCtrlID( PROP_CAMERA_FILE_BASE + rCamFileParam->Type() );
								CString sLabel = rCamFileParam->VisualName();
								
								if ( sLabel == _T("") )
									sLabel = rCamFileParam->Name();

								pCombo->SetLabelText( sLabel );
								pCombo->SetInfoText( CString(rCamFileParam->Description()) );
								pCombo->CreateComboBox();
								
								SVStringValueDeviceParam::OptionsType::const_iterator iterOption;
								int iOption=0;
								int iOptionIndex=0;
								for (iterOption = pCamFileParam->info.options.begin(); iterOption != pCamFileParam->info.options.end(); ++iterOption)
								{
									CString sText = iterOption->strDescription.c_str();
									int iPos;
									iPos = pCombo->AddString( sText );
									++iOption;

									if ( iterOption->value == pCamDeviceParam->strValue )
										iOptionIndex = iOption;
									
									pCombo->SetItemData( iPos, iOption );
								}
								if ( pCamFileParam->info.options.size() == 1 )
								{
									pCombo->ReadOnly( true );
								}
								
								pCombo->SetItemValue( iOptionIndex );
							}
						}
						break;
					}
				}
				case DeviceDataTypeComplex:
				{
					// put a "..." button to pull up custom dialog (e.g. LUT or LR)
					break;
				}
			
				default:
					break;
			}
		}
	}

	if (IsGigeSystem())
	{
		SVGigeCameraParamValidateClass l_svValidate;
			
		l_svValidate.UpdateParams( GetCameraFileParamsNonConst(), GetCameraDeviceParamsNonConst() );
	}
	else
	{
		SV1394CameraParamValidateClass l_svValidate;
				
		l_svValidate.UpdateParams( GetCameraFileParamsNonConst(), GetCameraDeviceParamsNonConst() );
	}
	CameraAdvancedHideItems();
/* SEJ -			
	if (m_eProduct == SVIM_PRODUCT_RGB_MONO)
	{
		pCombo = (SVRPropertyItemCombo*)m_Tree.InsertItem(new SVRPropertyItemCombo(),pRoot);
		if (pCombo)
		{  
			CString sBands;
			int iBandPos;
			pCombo->SetCtrlID(PROP_ADV_CAMERA_BANDS);
			pCombo->SetLabelText(_T("Band #"));
			pCombo->SetInfoText(_T("Enter the band that the camera will use."));
			pCombo->CreateComboBox();
			for (int iC = 0; iC < 3; iC++)
			{
				sBands.Empty();
				sBands.Format("%d",iC);
				iBandPos = pCombo->AddString(sBands);
				pCombo->SetItemData(iBandPos,iC);
			}
			pCombo->SetItemValue(m_pCameraObj->GetBandNumber());
		}
	}
*/
	m_Tree.SetColumn( (long) ( rc.Width() * 0.30 ) );
	pRoot->Select(true);
	m_Tree.SetFocus();
	pRoot->Expand();
	
	return FALSE;  // return TRUE unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void SVCameraFilePropertyPageDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if (IsIconic())
    {
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
    }
	else
    {
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR SVCameraFilePropertyPageDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void SVCameraFilePropertyPageDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// resize the control to always fit the dialog
	if (::IsWindow(m_Tree.GetSafeHwnd()))
		m_Tree.SetWindowPos(NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOZORDER);	
}

BOOL SVCameraFilePropertyPageDlg::OnEraseBkgnd(CDC* pdc) 
{
    CDialog::OnEraseBkgnd(pdc);
	// don't bother erasing the background since our control will always
	// cover the entire client area
	return TRUE;
}

void SVCameraFilePropertyPageDlg::OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	LPNMPROPTREE pNMPropTree = (LPNMPROPTREE) pNotifyStruct;
	*plResult = S_OK;

	if ( pNMPropTree->pItem )
	{
		SVRPropertyItem* pItem = pNMPropTree->pItem;
		if ( pItem->GetCtrlID() >= PROP_CAMERA_FILE_BASE )
		{
			SVDeviceParamEnum e = (SVDeviceParamEnum) (pItem->GetCtrlID() - PROP_CAMERA_FILE_BASE);

			SVDeviceParamCollection& params = GetCameraDeviceParamsNonConst();
			const SVDeviceParamCollection& l_CamFileParams = GetCameraFileParams();
			SVDeviceParamWrapper& w = params.GetParameter(e);
			const SVDeviceParamWrapper& wCF = l_CamFileParams.Parameter( e );
			SVDeviceParam* pDeviceParam = w;
			ASSERT( pDeviceParam );
			long lValue;

			switch ( pDeviceParam->DataType() )
			{
				case DeviceDataTypeLong:
				{
					SVLongValueDeviceParam* pParam = w.DerivedValue( pParam );
					const SVLongValueDeviceParam* pCFParam = wCF.DerivedValue( pCFParam );
					pItem->GetItemValue(lValue);

					if ( (pCFParam->info.max == 0 && pCFParam->info.min == 0) ||
						(lValue >= pCFParam->GetScaledMin() && lValue <= pCFParam->GetScaledMax() ) )
					{
						pParam->SetScaledValue(lValue);
						long lRecalcValue = pParam->GetScaledValue();
						
						if( lRecalcValue != lValue)
						{
							pItem->SetItemValue(lRecalcValue);
						}
					}
					else
					{
						MessageBeep(MB_ICONASTERISK);
						*plResult = S_FALSE;
					}
					break;
				}

				case DeviceDataTypei64:
				{
					SVi64ValueDeviceParam* pParam = w.DerivedValue( pParam );
					break;
				}

				case DeviceDataTypeBool:
				{
					SVBoolValueDeviceParam* pParam = w.DerivedValue( pParam );
					pItem->GetItemValue(lValue);
					*pParam = lValue != 0;
					break;
				}
				
				case DeviceDataTypeString:
				{
					switch ( pDeviceParam->Type() )
					{
						case DeviceParamCameraFormats:
						{
							SVCameraFormatsDeviceParam* pParam = w.DerivedValue( pParam );
							pItem->GetItemValue(lValue);
							const SVCameraFormat* pFormat = (const SVCameraFormat*) lValue;
							if ( pFormat )
							{
								pParam->strValue = pFormat->m_strName;
							}
						}
						break;

						default:
						{
							SVStringValueDeviceParam* pParam = w.DerivedValue( pParam );
						}
						break;
					}
				}

				case DeviceDataTypeComplex:
				{
					// put a "..." button to pull up custom dialog (e.g. LUT or LR)
					break;
				}

				default:
					break;
			}

			if (IsGigeSystem())
			{
				SVGigeCameraParamValidateClass l_svValidate;

				if ( l_svValidate.ShouldUpdateParams( e ) )
				{
					l_svValidate.UpdateParams( GetCameraFileParamsNonConst(), GetCameraDeviceParamsNonConst() );

					CameraAdvancedHideItems();
				}
			}
			else
			{
				SV1394CameraParamValidateClass l_svValidate;

				if ( l_svValidate.ShouldUpdateParams( e ) )
				{
					l_svValidate.UpdateParams( GetCameraFileParamsNonConst(), GetCameraDeviceParamsNonConst() );

					CameraAdvancedHideItems();
				}
			}

		}
		else
		{
			switch (pNMPropTree->pItem->GetCtrlID())
			{
/*
				case PROP_AD_FILE_NAME:
					{
						CString sFileName;
						m_Tree.FindItem(PROP_AD_FILE_NAME)->GetItemValue(sFileName);
						sFileName.TrimLeft();
						sFileName.TrimRight();
						m_pCameraObj->SetCameraFile(sFileName);
						m_pCameraObj->SetCameraFileChanged();
					}
					break;
*/
				case PROP_ADV_CAMERA_DIG:
					{
/* SEJ
						int iBands;
						long iDig;
						m_Tree.FindItem(PROP_ADV_CAMERA_DIG)->GetItemValue(iDig);
						
						SetDigNumber(iDig); 

						if (m_eProduct == SVIM_PRODUCT_RGB_MONO)
						{
							m_Tree.FindItem(PROP_ADV_CAMERA_BANDS)->GetItemValue(iBands);
							SetBandNumber(iBands);
						}
*/
					}
					break;
				
				default:
					break;
			}
		}
	}
}

void SVCameraFilePropertyPageDlg::OnItemQueryShowButton(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	*plResult = FALSE;

	LPNMPROPTREE pNMPropTree = (LPNMPROPTREE) pNotifyStruct;

	if ( pNMPropTree->pItem )
	{
		SVRPropertyItem* pItem = pNMPropTree->pItem;
		if ( pItem->GetCtrlID() >= PROP_CAMERA_FILE_BASE )
		{
			SVDeviceParamEnum e = (SVDeviceParamEnum) (pItem->GetCtrlID() - PROP_CAMERA_FILE_BASE);
			const SVDeviceParamCollection& params = GetCameraFileParams();
			const SVDeviceParamWrapper& w = params.Parameter(e);
			const SVDeviceParam* pDeviceParam = w;

			switch ( pDeviceParam->Type() )
			{
				case DeviceParamCameraFormats:
				{
					long lValue;
					pItem->GetItemValue(lValue);
					const SVCameraFormat* pFormat = (const SVCameraFormat*) lValue;
					if ( pFormat )
					{
						if ( pFormat->bVariableROI )
						{
							*plResult = true;
						}
					}
				}
			}
		}
	}
}

void SVCameraFilePropertyPageDlg::OnItemButtonClick(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	*plResult = TRUE;

	LPNMPROPTREE pNMPropTree = (LPNMPROPTREE) pNotifyStruct;

	if ( pNMPropTree->pItem )
	{
		SVRPropertyItem* pItem = pNMPropTree->pItem;
		if ( pItem->GetCtrlID() >= PROP_CAMERA_FILE_BASE )
		{
			SVDeviceParamEnum e = (SVDeviceParamEnum) (pItem->GetCtrlID() - PROP_CAMERA_FILE_BASE);
			SVDeviceParamCollection& rCamDeviceParams = GetCameraDeviceParamsNonConst();
			ASSERT( rCamDeviceParams.ParameterExists(e) );
			SVDeviceParamWrapper& rw = rCamDeviceParams.GetParameter(e);
			SVDeviceParam* pDeviceParam = rw;
			switch ( pDeviceParam->Type() )
			{
				case DeviceParamCameraFormats:
				{
					long lValue;
					pItem->GetItemValue(lValue);
					const SVCameraFormat* pSelectedFormat = (const SVCameraFormat*) lValue;
					if ( pSelectedFormat )
					{
						if ( pSelectedFormat->bVariableROI )
						{
//SEJ							Format7Dlg( pSelectedFormat->m_strName, rw );
						}
					}
				}
			}
		}
	}
}

void SVCameraFilePropertyPageDlg::OnClose() 
{
	CDialog::OnClose();
}

void SVCameraFilePropertyPageDlg::CameraAdvancedHideItems()
{
	SVRPropertyItem* l_pItem = m_Tree.GetRootItem();

	if ( l_pItem != NULL )
	{
		l_pItem = l_pItem->GetChild();
	}

	if ( l_pItem != NULL )
	{
		l_pItem = l_pItem->GetChild();
	}

	while ( l_pItem != NULL )
	{
		SVDeviceParamEnum l_eParamId = (SVDeviceParamEnum) (l_pItem->GetCtrlID() - PROP_CAMERA_FILE_BASE);

		const SVDeviceParamCollection& l_CamFileParams = GetCameraFileParams();

		const SVDeviceParam *l_pDeviceParam = l_CamFileParams.Parameter( l_eParamId );

		if ( l_pDeviceParam != NULL )
		{
			l_pItem->HideItem( 0 < l_pDeviceParam->DetailLevel() );
		}
		l_pItem = l_pItem->GetSibling();
	} 
}

void SVCameraFilePropertyPageDlg::SetTitle(CString sName)
{
    CString sTmp = _T("Property Settings  -  ") + sName;
    SetWindowText(sTmp);
}

const SVDeviceParamCollection& SVCameraFilePropertyPageDlg::GetCameraFileParams()
{
	return m_pAcquisition->m_CameraFileDeviceParams;
}

SVDeviceParamCollection& SVCameraFilePropertyPageDlg::GetCameraFileParamsNonConst()
{
	return m_pAcquisition->m_CameraFileDeviceParams;
}

const SVDeviceParamCollection& SVCameraFilePropertyPageDlg::GetCameraDeviceParams()
{
	return m_pAcquisition->m_DeviceParams;
}

SVDeviceParamCollection& SVCameraFilePropertyPageDlg::GetCameraDeviceParamsNonConst()
{
	return m_pAcquisition->m_DeviceParams;
}

bool SVCameraFilePropertyPageDlg::IsGigeSystem() const
{
	bool bRetVal = false;
	CSVImageTestApp* pApp = reinterpret_cast<CSVImageTestApp *>(AfxGetApp());
	if (pApp)
	{
		bRetVal = pApp->IsGigeSystem(); 
	}
	return bRetVal;
}

