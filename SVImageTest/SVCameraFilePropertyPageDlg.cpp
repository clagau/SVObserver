//******************************************************************************
//* COPYRIGHT (c) 2006 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
//Moved to precompiled header: #include <math.h>
#pragma warning (disable : 4786)
#include "SVCameraFilePropertyPageDlg.h"
#include "SVImageTestApp.h"
#include "SVTestAcquisitionClass.h"
#include "CameraLibrary/SVCustomDeviceParam.h"
#include "CameraLibrary/SVDeviceParams.h"
#include "CameraLibrary/SVDeviceParamCollection.h"
#include "CameraLibrary/SVBoolValueDeviceParam.h"
#include "CameraLibrary/SVi64ValueDeviceParam.h"
#include "CameraLibrary/SVLongValueDeviceParam.h"
#include "CameraLibrary/SVStringValueDeviceParam.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVGigeCameraFileLibrary/SVGigeCameraParamValidateClass.h"
#include "SVRPropertyTree/SVRPropTreeItemCombo.h"
#include "SVRPropertyTree/SVRPropTreeItemEdit.h"
#include "SVRPropertyTree/SVRPropTreeItemFile.h"
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

SVCameraFilePropertyPageDlg::SVCameraFilePropertyPageDlg(CWnd* pParent /*=nullptr*/)
: CDialog(SVCameraFilePropertyPageDlg::IDD, pParent)
, m_pAcquisition(nullptr)
{
	//{{AFX_DATA_INIT(SVCameraFilePropertyPageDlg)
	//}}AFX_DATA_INIT
}

SVCameraFilePropertyPageDlg::~SVCameraFilePropertyPageDlg()
{
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
	SVRPropertyItem*        pRoot = nullptr;
	
	
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
			{
				continue;
			}
			
			const int iDetailLevel = 0;
			if ( rCamFileParam->DetailLevel() > iDetailLevel )
			{
				continue;
			}
			
			const SVDeviceParam* pCamFileParam = rCamFileParam.DerivedValue(pCamFileParam);
			const SVDeviceParam* pCamDeviceParam = rCamDeviceParam.DerivedValue(pCamDeviceParam);

			switch ( rCamFileParam->DataType() )
			{
				case DeviceDataTypeBool:
				case DeviceDataTypeLong:
				case DeviceDataTypeComplex:
				case DeviceDataTypeString:
					SetupCameraDeviceParam(pRoot, pCamDeviceParam, pCamFileParam);
					break;

				case DeviceDataTypeCustom:
				{
					// swap the Container for the real parameters
					const SVCustomDeviceParam* pCustomFileParam = rCamFileParam.DerivedValue(pCustomFileParam);
					const SVCustomDeviceParam* pCustomDeviceParam = rCamDeviceParam.DerivedValue(pCustomDeviceParam);
					if (pCustomFileParam && pCustomDeviceParam)
					{
						const SVDeviceParamWrapper& rFileParam = pCustomFileParam->GetHeldParam();
						const SVDeviceParamWrapper& rDeviceParam = pCustomDeviceParam->GetHeldParam();

						const SVDeviceParam* pFileParam = rFileParam.DerivedValue(pFileParam);
						const SVDeviceParam* pDeviceParam = rDeviceParam.DerivedValue(pDeviceParam);
						SetupCameraDeviceParam(pRoot, pDeviceParam, pFileParam);
					}
				}
				break;

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
	
	CameraAdvancedHideItems();

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
	{
		m_Tree.SetWindowPos(nullptr, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOZORDER);
	}
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
			Log_Assert( pDeviceParam );
			long lValue;

			// If Custom get real DeviceParam
			if (pDeviceParam && pDeviceParam->DataType() == DeviceDataTypeCustom)
			{
				// swap the Container for the real parameters
				SVCustomDeviceParam* pCustomParam = w.DerivedValue(pCustomParam);

				pDeviceParam = pCustomParam->GetHeldParam();
			}
			switch ( pDeviceParam->DataType() )
			{
				case DeviceDataTypeLong:
				{
					SVLongValueDeviceParam* pParam = dynamic_cast<SVLongValueDeviceParam*>(pDeviceParam);
					const SVLongValueDeviceParam* pCFParam = wCF.DerivedValue(pCFParam);
					if (pParam && pCFParam)
					{
						pItem->GetItemValue(lValue);
						if ((pCFParam->info.max == 0 && pCFParam->info.min == 0)
							|| (lValue >= pCFParam->GetScaledMin() && lValue <= pCFParam->GetScaledMax()))
						{
							pParam->SetScaledValue(lValue);
							long lRecalcValue = pParam->GetScaledValue();
							if (lRecalcValue != lValue)
							{
								pItem->SetItemValue(lRecalcValue);
							}
						}
						else
						{
							MessageBeep(MB_ICONASTERISK);
							*plResult = S_FALSE;
						}
					}
					break;
				}

				case DeviceDataTypei64:
				{
					// this does nothing so we can't edit I64 values....
					break;
				}

				case DeviceDataTypeBool:
				{
					SVBoolValueDeviceParam* pParam = dynamic_cast<SVBoolValueDeviceParam*>(pDeviceParam);
					if (pParam)
					{
						pItem->GetItemValue(lValue);
						*pParam = lValue != 0;
					}
					break;
				}
				
				case DeviceDataTypeString:
				{
					switch ( pDeviceParam->Type() )
					{
						case DeviceParamCameraFormats:
						{
							SVCameraFormatsDeviceParam* pParam = dynamic_cast<SVCameraFormatsDeviceParam*>(pDeviceParam);
							_variant_t Value;
							pItem->GetItemValue( Value );
							const SVCameraFormat* pFormat = reinterpret_cast< const SVCameraFormat*> (Value.llVal);
							if ( pFormat )
							{
								pParam->strValue = pFormat->m_strName;
							}
						}
						break;

						default:
						{
							SVStringValueDeviceParam* pParam = dynamic_cast<SVStringValueDeviceParam*>(pDeviceParam);
							if (pParam)
							{
								SVRPropertyItemCombo* l_pCombo = dynamic_cast<SVRPropertyItemCombo*>(pItem);

								if (nullptr != l_pCombo)
								{
									int l_Index = l_pCombo->GetCurSel();
									if (-1 < l_Index)
									{
										CString Description;
										l_pCombo->GetLBText(l_Index, Description);

										SVStringValueDeviceParam::OptionsType::const_iterator iterOption;
										for (iterOption = pParam->info.options.begin(); iterOption != pParam->info.options.end(); ++iterOption)
										{
											if (Description.GetString() == iterOption->m_Description)
											{
												pParam->strValue = iterOption->m_Value;
												break;
											}
										}
									}
								}
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

			if (IsGigeSystem())
			{
				SVGigeCameraParamValidateClass l_svValidate;

				if ( l_svValidate.ShouldUpdateParams( e ) )
				{
					l_svValidate.UpdateParams( GetCameraFileParamsNonConst(), GetCameraDeviceParamsNonConst() );

					CameraAdvancedHideItems();
				}
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
					_variant_t Value;
					pItem->GetItemValue( Value );
					const SVCameraFormat* pFormat = reinterpret_cast< const SVCameraFormat*> (Value.llVal);
					if ( pFormat )
					{
						if ( pFormat->m_bVariableROI )
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
			Log_Assert( rCamDeviceParams.ParameterExists(e) );
			SVDeviceParamWrapper& rw = rCamDeviceParams.GetParameter(e);
			SVDeviceParam* pDeviceParam = rw;
			switch ( pDeviceParam->Type() )
			{
				case DeviceParamCameraFormats:
				{
					_variant_t Value;
					pItem->GetItemValue( Value );
					const SVCameraFormat* pSelectedFormat = reinterpret_cast< const SVCameraFormat*> (Value.llVal);
					if ( pSelectedFormat )
					{
						if ( pSelectedFormat->m_bVariableROI )
						{
							// Show ROI adjustment Dialog here
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
	// Traverse 2 generations or is this a copy/paste error?
	if ( nullptr != l_pItem )
	{
		l_pItem = l_pItem->GetChild();
	}
	if ( nullptr != l_pItem )
	{
		l_pItem = l_pItem->GetChild();
	}

	while ( nullptr != l_pItem )
	{
		SVDeviceParamEnum l_eParamId = (SVDeviceParamEnum) (l_pItem->GetCtrlID() - PROP_CAMERA_FILE_BASE);

		const SVDeviceParamCollection& l_CamFileParams = GetCameraFileParams();

		const SVDeviceParam *l_pDeviceParam = l_CamFileParams.Parameter( l_eParamId );

		if ( nullptr != l_pDeviceParam )
		{
			l_pItem->HideItem( 0 < l_pDeviceParam->DetailLevel() );
		}
		l_pItem = l_pItem->GetSibling();
	} 
}

void SVCameraFilePropertyPageDlg::SetTitle(std::string Name)
{
	std::string Tmp = _T("Property Settings  -  ") + Name;
	SetWindowText(Tmp.c_str());
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
	SVImageTestApp* pApp = reinterpret_cast<SVImageTestApp *>(AfxGetApp());
	if (pApp)
	{
		bRetVal = pApp->IsGigeSystem(); 
	}
	return bRetVal;
}

void SVCameraFilePropertyPageDlg::SetupCameraDeviceParam(SVRPropertyItem* pRoot, const SVDeviceParam* pDeviceParam, const SVDeviceParam* pFileParam)
{
	if (pDeviceParam && pFileParam)
	{
		switch (pFileParam->DataType())
		{
			case DeviceDataTypeLong:
			{
				const SVLongValueDeviceParam* pCamFileParam = dynamic_cast<const SVLongValueDeviceParam*>(pFileParam);
				const SVLongValueDeviceParam* pCamDeviceParam = dynamic_cast<const SVLongValueDeviceParam*>(pDeviceParam);
				if (pCamFileParam->info.options.size() > 0)
				{
					SVRPropertyItemCombo* pCombo = (SVRPropertyItemCombo*)m_Tree.InsertItem(new SVRPropertyItemCombo(), pRoot);
					if (pCombo)
					{
						pCombo->SetCtrlID(static_cast<UINT> (PROP_CAMERA_FILE_BASE) + static_cast<UINT>(pCamFileParam->Type()));
						std::string Label = pCamFileParam->VisualName();
						if (Label.empty())
						{
							Label = pCamFileParam->Name();
						}
						pCombo->SetLabelText(Label.c_str());
						pCombo->SetInfoText(pCamFileParam->Description());
						pCombo->CreateComboBox();

						SVLongValueDeviceParam::OptionsType::const_iterator iterOption;
						for (iterOption = pCamFileParam->info.options.begin(); iterOption != pCamFileParam->info.options.end(); ++iterOption)
						{
							int iPos = pCombo->AddString(iterOption->m_Description.c_str());
							pCombo->SetItemData(iPos, iterOption->m_Value);
						}
						if (pCamFileParam->info.options.size() == 1)
						{
							pCombo->ReadOnly(true);
						}
						pCombo->SetItemValue(pCamDeviceParam->lValue);
					}
				}
				else
				{
					// do a spin control bounded by pParam->info.min / info.max;
					SVRPropertyItemEdit* pEdit = (SVRPropertyItemEdit*)m_Tree.InsertItem(new SVRPropertyItemEdit(), pRoot);
					if (pEdit)
					{
						pEdit->SetCtrlID(static_cast<UINT>(PROP_CAMERA_FILE_BASE) + static_cast<UINT>(pCamFileParam->Type()));

						std::string Label = pCamFileParam->VisualName();
						if (Label.empty())
						{
							Label = pCamFileParam->Name();
						}
						Label = std::format(_T("{} ({})"), Label.c_str(), pCamDeviceParam->info.sUnits.c_str());
						pEdit->SetLabelText(Label.c_str());
						std::string Description = std::format(_T("{}   Min = {}, Max = {}; default = {}"), pCamFileParam->Description(), pCamFileParam->GetScaledMin(), pCamFileParam->GetScaledMax(), static_cast<long> (ceil(pCamFileParam->lValue * pCamFileParam->info.multiplier)));
						pEdit->SetInfoText(Description.c_str());

						pEdit->SetItemValue(pCamDeviceParam->GetScaledValue());
					}
				}
				break;
			}
			case DeviceDataTypeBool:
			{
				const SVBoolValueDeviceParam* pCamFileParam = dynamic_cast<const SVBoolValueDeviceParam*>(pFileParam);
				const SVBoolValueDeviceParam* pCamDeviceParam = dynamic_cast<const SVBoolValueDeviceParam*>(pDeviceParam);

				SVRPropertyItemCombo* pCombo = (SVRPropertyItemCombo*)m_Tree.InsertItem(new SVRPropertyItemCombo(), pRoot);
				if (pCombo)
				{
					pCombo->SetCtrlID(static_cast<UINT>(PROP_CAMERA_FILE_BASE) + static_cast<UINT>(pCamFileParam->Type()));
					std::string Label = pCamFileParam->VisualName();
					if (Label.empty())
					{
						Label = pCamFileParam->Name();
					}
					pCombo->SetLabelText(Label.c_str());
					pCombo->SetInfoText(pCamFileParam->Description());
					pCombo->CreateComboBox();

					SVBoolValueDeviceParam::OptionsType::const_iterator iterOption;
					for (iterOption = pCamFileParam->info.options.begin(); iterOption != pCamFileParam->info.options.end(); ++iterOption)
					{
						int iPos = pCombo->AddString(iterOption->m_Description.c_str());
						pCombo->SetItemData(iPos, (LPARAM)iterOption->m_Value);
					}
					if (pCamFileParam->info.options.size() == 1)
					{
						pCombo->ReadOnly(true);
					}
					pCombo->SetItemValue(pCamDeviceParam->bValue);
				}
				break;
			}
			case DeviceDataTypeString:
			{
				switch (pFileParam->Type())
				{
					case DeviceParamCameraFormats:
					{
						const SVCameraFormatsDeviceParam* pCamFileParam = dynamic_cast<const SVCameraFormatsDeviceParam*>(pFileParam);
						const SVCameraFormatsDeviceParam* pCamDeviceParam = dynamic_cast<const SVCameraFormatsDeviceParam*>(pDeviceParam);
						if (pCamFileParam->options.size() > 0)
						{
							SVRPropertyItemCombo* pCombo = (SVRPropertyItemCombo*)m_Tree.InsertItem(new SVRPropertyItemCombo(), pRoot);
							if (pCombo)
							{
								pCombo->SetCtrlID(static_cast<UINT>(PROP_CAMERA_FILE_BASE) + static_cast<UINT>(pCamFileParam->Type()));
								std::string Label = pCamFileParam->VisualName();
								if (Label.empty())
								{
									Label = pCamFileParam->Name();
								}
								pCombo->SetLabelText(Label.c_str());
								pCombo->SetInfoText(pCamFileParam->Description());
								pCombo->SetButtonText("ROI");
								pCombo->CreateComboBox();

								SVCameraFormatsDeviceParam::OptionsType::const_iterator iterOption;
								const SVCameraFormat* pFormat = nullptr;
								for (iterOption = pCamFileParam->options.begin(); iterOption != pCamFileParam->options.end(); ++iterOption)
								{
									if (false == iterOption->second.m_bColor)
									{
										int iPos = pCombo->AddString(iterOption->second.m_strDescription.c_str());
										pCombo->SetItemData(iPos, reinterpret_cast<DWORD_PTR>(&(iterOption->second)));
										if (pCamDeviceParam->strValue == iterOption->second.m_strName)
										{
											pFormat = &(iterOption->second);
										}
									}
								}
								if (pCamFileParam->options.size() == 1)
								{
									pCombo->ReadOnly(true);
								}
								pCombo->SetItemValue(reinterpret_cast<DWORD_PTR>(pFormat));
							}
						}// end if ( pCamFileParam->options.size() > 0 )
					}// end case DeviceParamCameraFormats:
					break;

					default:
					{
						const SVStringValueDeviceParam* pCamFileParam = dynamic_cast<const SVStringValueDeviceParam*>(pFileParam);
						const SVStringValueDeviceParam* pCamDeviceParam = dynamic_cast<const SVStringValueDeviceParam*>(pDeviceParam);
						if (pCamFileParam->info.options.size() > 0)
						{
							SVRPropertyItemCombo* pCombo = (SVRPropertyItemCombo*)m_Tree.InsertItem(new SVRPropertyItemCombo(), pRoot);
							if (pCombo)
							{
								pCombo->SetCtrlID(static_cast<UINT>(PROP_CAMERA_FILE_BASE) + static_cast<UINT>(pCamFileParam->Type()));
								std::string Label = pCamFileParam->VisualName();
								if (Label.empty())
								{
									Label = pCamFileParam->Name();
								}
								pCombo->SetLabelText(Label.c_str());
								pCombo->SetInfoText(pCamFileParam->Description());
								pCombo->CreateComboBox();

								SVStringValueDeviceParam::OptionsType::const_iterator iterOption;
								int iOption = 0;
								int iOptionIndex = 0;
								for (iterOption = pCamFileParam->info.options.begin(); iterOption != pCamFileParam->info.options.end(); ++iterOption)
								{
									int iPos;
									iPos = pCombo->AddString(iterOption->m_Description.c_str());
									++iOption;
									if (iterOption->m_Value == pCamDeviceParam->strValue)
										iOptionIndex = iOption;
									pCombo->SetItemData(iPos, iOption);
								}
								if (pCamFileParam->info.options.size() == 1)
								{
									pCombo->ReadOnly(true);
								}
								pCombo->SetItemValue(iOptionIndex);
							}
						}
					}// end default:
					break;
				}// end switch ( pFileParam->Type() )
			}
			case DeviceDataTypeComplex:
			{
				// put a "..." button to pull up custom dialog (e.g. LUT or LR)
				break;
			}
			default:
				break;
		}// end switch ( w->DataType() )
	}
}


