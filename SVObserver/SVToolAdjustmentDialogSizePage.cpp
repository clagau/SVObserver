//******************************************************************************
/// COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Property Page: Position and Size Page for The ToolAdjustmen Dialog
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVToolAdjustmentDialogSizePage.h"
#include "SVObserver.h"
#include "afxdialogex.h"
#include "Operators/ToolSizeAdjustTask.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SvOGui/SVFormulaEditorSheet.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "Operators/EQAdjustSize.h"
#include "Tools/SVTool.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVGuiExtentUpdater.h"
#include "TextDefinesSvO.h"
#include "SVOGui/ValuesAccessor.h"
#include "SVOGui/DataController.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

// SVToolAdjustmentDialogSizePage dialog

IMPLEMENT_DYNAMIC(SVToolAdjustmentDialogSizePage, CPropertyPage)

SVToolAdjustmentDialogSizePage::SVToolAdjustmentDialogSizePage(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* Parent)
: CPropertyPage(SVToolAdjustmentDialogSizePage::IDD)
, m_pParentDialog(Parent)
, m_pTool(nullptr)
{
	for( int i =0 ; i < SvOp::ToolSizeAdjustTask::TSValuesCount; i++)
	{
		m_pEQAdjustSize[i] = nullptr;
	}
}

SVToolAdjustmentDialogSizePage::~SVToolAdjustmentDialogSizePage()
{
}

void SVToolAdjustmentDialogSizePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	
	DDX_Control(pDX, IDC_COMBO_TA_SIZE_MODE_WIDTH, m_ComboBox[SvOp::ToolSizeAdjustTask::TSWidth]);
	DDX_Control(pDX, IDC_COMBO_TA_SIZE_MODE_HEIGHT, m_ComboBox[SvOp::ToolSizeAdjustTask::TSHeight]);
	DDX_Control(pDX, IDC_COMBO_TA_SIZE_MODE_POSITIONX, m_ComboBox[SvOp::ToolSizeAdjustTask::TSPositionX]);
	DDX_Control(pDX, IDC_COMBO_TA_SIZE_MODE_POSITIONY, m_ComboBox[SvOp::ToolSizeAdjustTask::TSPositionY]);
	
	DDX_Control(pDX, IDC_BUTTON_TA_WIDTH_FORMULA, m_Button[SvOp::ToolSizeAdjustTask::TSWidth]);
	DDX_Control(pDX, IDC_BUTTON_TA_HEIGHT, m_Button[SvOp::ToolSizeAdjustTask::TSHeight]);
	DDX_Control(pDX, IDC_BUTTON_POSITION_FORMULA_X, m_Button[SvOp::ToolSizeAdjustTask::TSPositionX]);
	DDX_Control(pDX, IDC_BUTTON_POSITION_Y, m_Button[SvOp::ToolSizeAdjustTask::TSPositionY]);

	DDX_Control(pDX, IDC_WIDTH_EDIT, m_EditCtrl[SvOp::ToolSizeAdjustTask::TSWidth]);
	DDX_Control(pDX, IDC_HEIGHT_EDIT, m_EditCtrl[SvOp::ToolSizeAdjustTask::TSHeight]);
	DDX_Control(pDX, IDC_TRANSLATIONX_EDIT, m_EditCtrl[SvOp::ToolSizeAdjustTask::TSPositionX]);
	DDX_Control(pDX, IDC_TRANSLATIONY_EDIT, m_EditCtrl[SvOp::ToolSizeAdjustTask::TSPositionY]);
}

BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogSizePage, CPropertyPage)
	ON_CBN_SELCHANGE(IDC_COMBO_TA_SIZE_MODE_POSITIONY, &SVToolAdjustmentDialogSizePage::OnCbnSelchangeComboPositionY)
	ON_CBN_SELCHANGE(IDC_COMBO_TA_SIZE_MODE_POSITIONX, &SVToolAdjustmentDialogSizePage::OnCbnSelchangeComboPositionX)
	ON_CBN_SELCHANGE(IDC_COMBO_TA_SIZE_MODE_WIDTH, &SVToolAdjustmentDialogSizePage::OnCbnSelchangeComboWidthMode)
	ON_CBN_SELCHANGE(IDC_COMBO_TA_SIZE_MODE_HEIGHT, &SVToolAdjustmentDialogSizePage::OnCbnSelchangeComboHeightMode)
	ON_BN_CLICKED(IDC_BUTTON_TA_WIDTH_FORMULA, &SVToolAdjustmentDialogSizePage::OnBnClickedButtonTaWidthFormula)
	ON_BN_CLICKED(IDC_BUTTON_POSITION_Y, &SVToolAdjustmentDialogSizePage::OnBnClickedButtonPositionY)
	ON_BN_CLICKED(IDC_BUTTON_POSITION_FORMULA_X, &SVToolAdjustmentDialogSizePage::OnBnClickedButtonPositionFormulaX)
	ON_BN_CLICKED(IDC_BUTTON_TA_HEIGHT, &SVToolAdjustmentDialogSizePage::OnBnClickedButtonTaHeight)
END_MESSAGE_MAP()

// SVToolAdjustmentDialogSizePage message handlers
BOOL SVToolAdjustmentDialogSizePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	if( nullptr != m_pParentDialog )
	{
		m_pTool = m_pParentDialog->GetTool();
	}
	if(  nullptr != m_pTool )
	{
		// Get ToolSizeAdjustTask 
		SvDef::SVObjectTypeInfoStruct ToolSizeAdjustTaskInfo;
		ToolSizeAdjustTaskInfo.ObjectType = SvPb::SVToolSizeAdjustTaskType;
		
		m_pToolSizeAdjustTask = dynamic_cast<SvOp::ToolSizeAdjustTask*>(m_pTool->getFirstObject(ToolSizeAdjustTaskInfo));
		if( m_pToolSizeAdjustTask )
		{
			for( int vType  = SvOp::ToolSizeAdjustTask::TSPositionX ; vType <  SvOp::ToolSizeAdjustTask::TSValuesCount; ++vType)
			{
				SvVol::SVEnumerateValueObjectClass* pValue = m_pToolSizeAdjustTask->GetInputMode(static_cast<SvOp::ToolSizeAdjustTask::TSValues>(vType));
				if (pValue)
				{
					m_ComboBox[vType].SetEnumTypes(pValue->GetEnumVector());
					if (false == m_pToolSizeAdjustTask->IsFullSizeAllowed())
					{
						///Remove Fullsize from combobox
						int index = m_ComboBox[vType].FindString(-1, SvDef::SizeAdjustTextFullSize);
						if (index >= 0)
						{
							m_ComboBox[vType].DeleteString(index);
						}
					}
				}
			}
		}
		
		// Get the Evaluate Objects..
		SvDef::SVObjectTypeInfoStruct evaluateObjectInfo;
		evaluateObjectInfo.ObjectType = SvPb::SVEquationObjectType;

		evaluateObjectInfo.SubType = SvPb::EQSizePositionXType;
		m_pEQAdjustSize[SvOp::ToolSizeAdjustTask::TSPositionX] = dynamic_cast<SvOp::EQAdjustSize*>(m_pTool->getFirstObject(evaluateObjectInfo));
		
		evaluateObjectInfo.SubType = SvPb::EQSizePositionYType;
		m_pEQAdjustSize[SvOp::ToolSizeAdjustTask::TSPositionY]  = dynamic_cast<SvOp::EQAdjustSize*>(m_pTool->getFirstObject(evaluateObjectInfo));
		
		evaluateObjectInfo.SubType = SvPb::EQSizeWidthType;
		m_pEQAdjustSize[SvOp::ToolSizeAdjustTask::TSWidth] = dynamic_cast<SvOp::EQAdjustSize*>(m_pTool->getFirstObject(evaluateObjectInfo));
		
		evaluateObjectInfo.SubType = SvPb::EQSizeHeightType;
		m_pEQAdjustSize[SvOp::ToolSizeAdjustTask::TSHeight] = dynamic_cast<SvOp::EQAdjustSize*>(m_pTool->getFirstObject(evaluateObjectInfo));
		
		Refresh( false );
		UpdateData( FALSE );
	}
	else
	{
		// Not valid call...
		if( GetParent())
		{
			GetParent()->SendMessage( WM_CLOSE );
		}
		else
		{
			SendMessage( WM_CLOSE );
		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
}

HRESULT SVToolAdjustmentDialogSizePage::SetInspectionData()
{
	HRESULT hresult = S_OK;

	if( nullptr !=  m_pToolSizeAdjustTask && nullptr !=  m_pTool)
	{
		UpdateData(true); // get data from dialog
		
		SvTo::EAutoSize AutoSizeEnable = m_pTool->GetAutoSizeEnabled();

		//@TODO[gra][8.00][15.01.2018]: The data controller should be used like the rest of SVOGui
		typedef SvOg::DataController<SvOg::ValuesAccessor, SvOg::ValuesAccessor::value_type> Controller;
		Controller Values{ SvOg::BoundValues{ m_pToolSizeAdjustTask->GetInspection()->GetUniqueObjectID(), m_pToolSizeAdjustTask->GetUniqueObjectID() } };
		Values.Init();

		for( int vType  = SvOp::ToolSizeAdjustTask::TSPositionX; vType < SvOp::ToolSizeAdjustTask::TSValuesCount; ++vType)
		{
			int sel = m_ComboBox[vType].GetCurSel();
			if( sel >= 0 )
			{
				long Value = ( long ) m_ComboBox[vType].GetItemData( sel );
				
				if((vType == SvOp::ToolSizeAdjustTask::TSPositionX || vType == SvOp::ToolSizeAdjustTask::TSPositionY) )
				{
					if((AutoSizeEnable & SvTo::EnablePosition) == 0)
					{
						Value = SvOp::ToolSizeAdjustTask::TSNone;
					}
				}
				else if((AutoSizeEnable & SvTo::EnableSize) == 0)
				{
					Value = SvOp::ToolSizeAdjustTask::TSNone;
				}
				
				SvVol::SVEnumerateValueObjectClass* pValue = m_pToolSizeAdjustTask->GetInputMode(static_cast<SvOp::ToolSizeAdjustTask::TSValues>(vType));
				if (nullptr != pValue)
				{
					Values.Set<long>(pValue->GetEmbeddedID(), Value);
				}
				bool bEnabled = (Value == SvOp::ToolSizeAdjustTask::TSFormula);
				SvOp::EQAdjustSize* pEquation = GetEvaluateObject( (SvOp::ToolSizeAdjustTask::TSValues) vType);
				if (nullptr != pEquation)
				{
					//@TODO[gra][8.00][15.01.2018]: The data controller should be used like the rest of SVOGui
					Controller EquationValues{ SvOg::BoundValues{ pEquation->GetInspection()->GetUniqueObjectID(), pEquation->GetUniqueObjectID() } };
					EquationValues.Init();
					EquationValues.Set<bool>(pEquation->enabled.GetEmbeddedID(), bEnabled);
					EquationValues.Commit(SvOg::PostAction::doNothing);
				}
			}
			else
			{
				hresult = SvStl::Err_16029_InvalidSelection;
			}
			if ( S_OK != hresult)
			{
				break;
			}
		}

		Values.Commit();
	}
	return hresult;
}

void SVToolAdjustmentDialogSizePage::Refresh( bool bSave /*= true*/ )
{
	CWnd* pWnd = nullptr;
	if( nullptr != m_pTool && nullptr !=  m_pToolSizeAdjustTask )
	{
		if( bSave )
		{
			SetInspectionData();
		}
		
		SvTo::EAutoSize AutoSizeEnable = m_pTool->GetAutoSizeEnabled();

		///Hide not allowed Controls
		int show = (m_pToolSizeAdjustTask->IsAdjustSizeAllowed() == false) ?  SW_HIDE :SW_SHOW;
		
		bool bEnable = (false == m_pToolSizeAdjustTask->IsAdjustSizeAllowed()) ?  false :true;
		bEnable = (AutoSizeEnable & SvTo::EnableSize) == SvTo::EnableSize ?  bEnable : false;
		m_ComboBox[SvOp::ToolSizeAdjustTask::TSWidth].ShowWindow(show);
		m_EditCtrl[SvOp::ToolSizeAdjustTask::TSWidth].ShowWindow(show);
		m_ComboBox[SvOp::ToolSizeAdjustTask::TSHeight].ShowWindow(show);
		m_EditCtrl[SvOp::ToolSizeAdjustTask::TSHeight].ShowWindow(show);
		
		m_ComboBox[SvOp::ToolSizeAdjustTask::TSWidth].EnableWindow(bEnable);
		m_ComboBox[SvOp::ToolSizeAdjustTask::TSHeight].EnableWindow(bEnable);
		
		show = (false == m_pToolSizeAdjustTask->IsAdjustPositionAllowed()) ?  SW_HIDE :SW_SHOW;
		bEnable = (false == m_pToolSizeAdjustTask->IsAdjustPositionAllowed()) ?  false :true;
		bEnable = (AutoSizeEnable & SvTo::EnablePosition) == SvTo::EnablePosition ? bEnable : false;

		m_ComboBox[SvOp::ToolSizeAdjustTask::TSPositionX].ShowWindow(show);
		m_EditCtrl[SvOp::ToolSizeAdjustTask::TSPositionX].ShowWindow(show);
		m_ComboBox[SvOp::ToolSizeAdjustTask::TSPositionY].ShowWindow(show);
		m_EditCtrl[SvOp::ToolSizeAdjustTask::TSPositionY].ShowWindow(show);

		m_ComboBox[SvOp::ToolSizeAdjustTask::TSPositionX].EnableWindow(bEnable);
		m_ComboBox[SvOp::ToolSizeAdjustTask::TSPositionY].EnableWindow(bEnable);

		// refresh  combo settings...
		for( int vType = SvOp::ToolSizeAdjustTask::TSPositionX; vType < SvOp::ToolSizeAdjustTask::TSValuesCount; ++vType)
		{
			long SelMode(0), nShow(SW_HIDE);
			CString csResult;
			SvVol::SVEnumerateValueObjectClass* pValue = m_pToolSizeAdjustTask->GetInputMode(static_cast<SvOp::ToolSizeAdjustTask::TSValues>(vType));
			if (pValue)
			{
				if (S_OK == pValue->GetValue( SelMode ))
				{
					if (SvOp::ToolSizeAdjustTask::TSPositionX == vType || SvOp::ToolSizeAdjustTask::TSPositionY == vType)
					{
						if (0 == (AutoSizeEnable & SvTo::EnablePosition))
						{
							SelMode = SvOp::ToolSizeAdjustTask::TSNone;
						}
					}
					else if (0 == (AutoSizeEnable & SvTo::EnableSize))
					{
						SelMode = SvOp::ToolSizeAdjustTask::TSNone;
					} 

					m_ComboBox[vType].SetCurSelItemData(SelMode);

					if (SvOp::ToolSizeAdjustTask::TSFormula == SelMode)
					{
						nShow = SW_SHOW;
						if (nullptr != m_pEQAdjustSize[vType]  && (m_pEQAdjustSize[vType])->IsEnabled() && (m_pEQAdjustSize[vType])->HasCondition())
						{
							long lResult = static_cast< long >( (m_pEQAdjustSize[vType])->GetYACCResult() );
							csResult.Format( _T("%ld") , lResult );
						}
					}
					else if (SvOp::ToolSizeAdjustTask::TSNone == SelMode)
					{
						long lvalue;
						if (S_OK == m_pToolSizeAdjustTask->GetExtentValues(SvOp::ToolSizeAdjustTask::TSValues(vType),lvalue))
						{
							csResult.Format( _T("%ld") , lvalue );
						}
					}
					else if(SelMode == SvOp::ToolSizeAdjustTask::TSFullSize)
					{
						long lvalue(0);
						if (SvOp::ToolSizeAdjustTask::TSHeight == SvOp::ToolSizeAdjustTask::TSValues(vType) || SvOp::ToolSizeAdjustTask::TSWidth == SvOp::ToolSizeAdjustTask::TSValues(vType))
						{
							if (S_OK == m_pToolSizeAdjustTask->GetParentExtentOutputValues(SvOp::ToolSizeAdjustTask::TSValues(vType), lvalue) )
							{
								csResult.Format( _T("%ld") , lvalue );
							}
						}
						else
						{
							csResult.Format( _T("%ld") , 0 );
						}
					}
					m_EditCtrl[vType].SetWindowText(csResult);
					m_Button[vType].ShowWindow(nShow);
				}
			}
		}
	}
	UpdateData( false ); // set data to dialog
}

void SVToolAdjustmentDialogSizePage::OnBnClickedButtonFormula(SvOp::ToolSizeAdjustTask::TSValues mode)
{
	if (mode < 0 || mode >= SvOp::ToolSizeAdjustTask::TSValuesCount)
	{
		ASSERT(false);
		return;
	}
	if (m_pEQAdjustSize[mode])
	{
		CString strCaption = m_pEQAdjustSize[mode]->GetName();
		strCaption += _T(" Formula");

		SvDef::SVObjectTypeInfoStruct info(m_pEQAdjustSize[mode]->GetObjectType(), m_pEQAdjustSize[mode]->GetObjectSubType());
		const GUID& rInspectionID = m_pParentDialog->GetInspectionID();
		const GUID& rObjectID = m_pEQAdjustSize[mode]->GetUniqueObjectID();

		SvOg::SVFormulaEditorSheetClass dlg(rInspectionID, rObjectID, info, strCaption);
		dlg.DoModal();
		Refresh(true);
	}
}

void SVToolAdjustmentDialogSizePage::OnSelchangeCombo(SvOp::ToolSizeAdjustTask::TSValues mode)
{
	/// full size must be selected in both or none of the combo boxes 
	DWORD_PTR selMode = m_ComboBox[mode].GetCurSelItemData();
	if (SvOp::ToolSizeAdjustTask::TSFullSize == selMode)
	{
		///set all other to full size 
		for( int vType = SvOp::ToolSizeAdjustTask::TSPositionX; vType < SvOp::ToolSizeAdjustTask::TSValuesCount; ++vType)
		{
			if (vType == mode)
			{
				continue;	
			}
			m_ComboBox[vType].SetCurSelItemData(SvOp::ToolSizeAdjustTask::TSFullSize);
		}
	}
	else 
	{
		///Then all must be not be full sized
		for( int vType = SvOp::ToolSizeAdjustTask::TSPositionX; vType < SvOp::ToolSizeAdjustTask::TSValuesCount; ++vType)
		{
			if (vType == mode)
			{
				continue;	
			}
			if ( SvOp::ToolSizeAdjustTask::TSFullSize == m_ComboBox[vType].GetCurSelItemData() )
			{
				m_ComboBox[vType].SetCurSelItemData(SvOp::ToolSizeAdjustTask::TSNone);
			}
		}
	}
	Refresh(true);
}

void SVToolAdjustmentDialogSizePage::OnCbnSelchangeComboWidthMode()
{
	OnSelchangeCombo(SvOp::ToolSizeAdjustTask::TSWidth);
}

void SVToolAdjustmentDialogSizePage::OnCbnSelchangeComboHeightMode()
{
	OnSelchangeCombo(SvOp::ToolSizeAdjustTask::TSHeight);
}

void SVToolAdjustmentDialogSizePage::OnCbnSelchangeComboPositionX()
{
	OnSelchangeCombo(SvOp::ToolSizeAdjustTask::TSPositionX);
}

void SVToolAdjustmentDialogSizePage::OnCbnSelchangeComboPositionY()
{
	OnSelchangeCombo(SvOp::ToolSizeAdjustTask::TSPositionY);
}

void SVToolAdjustmentDialogSizePage::OnBnClickedButtonTaWidthFormula()
{
	return OnBnClickedButtonFormula(SvOp::ToolSizeAdjustTask::TSWidth);
}

void SVToolAdjustmentDialogSizePage::OnBnClickedButtonPositionY()
{
	return OnBnClickedButtonFormula(SvOp::ToolSizeAdjustTask::TSPositionY);
}

void SVToolAdjustmentDialogSizePage::OnBnClickedButtonPositionFormulaX()
{
	return OnBnClickedButtonFormula(SvOp::ToolSizeAdjustTask::TSPositionX);
}

void SVToolAdjustmentDialogSizePage::OnBnClickedButtonTaHeight()
{
	return OnBnClickedButtonFormula(SvOp::ToolSizeAdjustTask::TSHeight);
}

SvOp::EQAdjustSize* SVToolAdjustmentDialogSizePage::GetEvaluateObject(SvOp::ToolSizeAdjustTask::TSValues typ)
{
	if (typ < 0 || typ >= SvOp::ToolSizeAdjustTask::TSValuesCount)
	{
		ASSERT(false);
		return nullptr;
	}
	return m_pEQAdjustSize[typ];
}

bool SVToolAdjustmentDialogSizePage::QueryAllowExit()
{
	/// no empty formulas 
	for( int vType  = SvOp::ToolSizeAdjustTask::TSPositionX; vType < SvOp::ToolSizeAdjustTask::TSValuesCount; ++vType)
	{
		int sel = m_ComboBox[vType].GetCurSel();
		if( sel >= 0 )
		{
			long Value = static_cast<long>(m_ComboBox[vType].GetItemData( sel ));
			if (SvOp::ToolSizeAdjustTask::TSFormula == Value)
			{
				SvOp::EQAdjustSize* pEQ = GetEvaluateObject( static_cast<SvOp::ToolSizeAdjustTask::TSValues>(vType));
				if (nullptr != pEQ)
				{
					if( pEQ->GetEquationText().empty() )
					{
						SvStl::MessageMgrStd Exception(SvStl::MsgType::Log | SvStl::MsgType::Display );
						Exception.setMessage( SVMSG_SVO_64_EMPTY_FORMULAS_ARE_NOT_ALLOWED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16038_EmptyFormula );
						return false;
					}
				}
			}
		}
	}
	return true;
}

BOOL SVToolAdjustmentDialogSizePage::OnSetActive()
{
	Refresh(false);
	return CPropertyPage::OnSetActive();
}

BOOL SVToolAdjustmentDialogSizePage::OnKillActive() 
{
	HRESULT hr = E_FAIL;
	if (false == QueryAllowExit())
	{
		return false;
	}
	if (nullptr != m_pTool)
	{
		hr  = SVGuiExtentUpdater::ForwardSizeAndPosition(m_pTool, ResetMode_Tool);
	}
	return CPropertyPage::OnKillActive();
}
