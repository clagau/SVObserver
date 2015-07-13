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
#include "ToolSizeAdjustTask.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVFormulaEditorSheet.h"
#include "EQAdjustSizePositionX.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "EQAdjustSize.h"
#include "SVStatusLibrary\ExceptionManager.h"
#include "SVGuiExtentUpdater.h"
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

	SVToolAdjustmentDialogSizePage::SVToolAdjustmentDialogSizePage(SVToolAdjustmentDialogSheetClass* Parent)
	: CPropertyPage(SVToolAdjustmentDialogSizePage::IDD),
	m_pParentDialog(Parent),
	m_pTool(nullptr)
{
	
	for( int i =0 ; i < ToolSizeAdjustTask::TSValuesCount; i++)
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
	
	DDX_Control(pDX, IDC_COMBO_TA_SIZE_MODE_WIDTH, m_ComboBox[ToolSizeAdjustTask::TSWidth]); // .m_ComboSizeMode
	DDX_Control(pDX, IDC_COMBO_TA_SIZE_MODE_HEIGHT, m_ComboBox[ToolSizeAdjustTask::TSHeight]);//m_ComboPositionMode);
	DDX_Control(pDX, IDC_COMBO_TA_SIZE_MODE_POSITIONX, m_ComboBox[ToolSizeAdjustTask::TSPositionX]); 
	DDX_Control(pDX, IDC_COMBO_TA_SIZE_MODE_POSITIONY, m_ComboBox[ToolSizeAdjustTask::TSPositionY]);
	
	DDX_Control(pDX, IDC_BUTTON_TA_WIDTH_FORMULA, m_Button[ToolSizeAdjustTask::TSWidth]);
	DDX_Control(pDX, IDC_BUTTON_TA_HEIGHT, m_Button[ToolSizeAdjustTask::TSHeight]);
	DDX_Control(pDX, IDC_BUTTON_POSITION_FORMULA_X, m_Button[ToolSizeAdjustTask::TSPositionX]);
	DDX_Control(pDX, IDC_BUTTON_POSITION_Y, m_Button[ToolSizeAdjustTask::TSPositionY]);

	DDX_Control(pDX, IDC_WIDTH_EDIT, m_EditCtrl[ToolSizeAdjustTask::TSWidth]);
	DDX_Control(pDX, IDC_HEIGHT_EDIT, m_EditCtrl[ToolSizeAdjustTask::TSHeight]);
	DDX_Control(pDX, IDC_TRANSLATIONX_EDIT, m_EditCtrl[ToolSizeAdjustTask::TSPositionX]);
	DDX_Control(pDX, IDC_TRANSLATIONY_EDIT, m_EditCtrl[ToolSizeAdjustTask::TSPositionY]);
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
		SetTaskObject( m_pTool );
		// Get ToolSizeAdjustTask 
		SVObjectTypeInfoStruct ToolSizeAdjustTaskInfo;
		//ToolSizeAdjustTaskInfo.EmbeddedID = ToolSizeAdjustTaskGuid;
		ToolSizeAdjustTaskInfo.ObjectType = SVToolSizeAdjustTaskType;
		
		
		m_pToolSizeAdjustTask = reinterpret_cast<ToolSizeAdjustTask*>(::SVSendMessage( m_pTool, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&ToolSizeAdjustTaskInfo) ));
		if( m_pToolSizeAdjustTask )
		{
			for( int vType  = ToolSizeAdjustTask::TSPositionX ; vType <  ToolSizeAdjustTask::TSValuesCount; ++vType)
			{
				CString csMode;
				m_pToolSizeAdjustTask->m_InputModes[vType].GetEnumTypes(csMode);
				m_ComboBox[vType].SetEnumTypes(csMode);
			}
		}
		
		// Get the Evaluate Objects..
		
		SVObjectTypeInfoStruct evaluateObjectInfo;
		evaluateObjectInfo.ObjectType = SVEquationObjectType;

		evaluateObjectInfo.SubType = EQSizePositionXType;		
		m_pEQAdjustSize[ToolSizeAdjustTask::TSPositionX] =  
		 reinterpret_cast<EQAdjustSize*>(::SVSendMessage(m_pTool, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&evaluateObjectInfo)));
		
		evaluateObjectInfo.SubType = EQSizePositionYType;
		m_pEQAdjustSize[ToolSizeAdjustTask::TSPositionY]  = reinterpret_cast<EQAdjustSize*>(::SVSendMessage(m_pTool, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&evaluateObjectInfo)));
		
		evaluateObjectInfo.SubType = EQSizeWidthType;		
		m_pEQAdjustSize[ToolSizeAdjustTask::TSWidth] = reinterpret_cast<EQAdjustSize*>(::SVSendMessage(m_pTool, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&evaluateObjectInfo)));
		
		evaluateObjectInfo.SubType = EQSizeHeightType;		
		m_pEQAdjustSize[ToolSizeAdjustTask::TSHeight] = reinterpret_cast<EQAdjustSize*>(::SVSendMessage(m_pTool, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&evaluateObjectInfo)));
		
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
		UpdateData( TRUE ); // get data from dialog
		
		EAutoSize AutoSizeEnable = m_pTool->GetAutoSizeEnabled();

		for( int vType  = ToolSizeAdjustTask::TSPositionX ; vType < ToolSizeAdjustTask::TSValuesCount ; ++vType)
		{
			int sel = m_ComboBox[vType].GetCurSel();
			if( sel >= 0 )
			{
				long Value = ( long ) m_ComboBox[vType].GetItemData( sel );
				
				
				
				if((vType == ToolSizeAdjustTask::TSPositionX || vType == ToolSizeAdjustTask::TSPositionY) )
				{
					if((AutoSizeEnable & EnablePosition) == 0)
					{
						Value = ToolSizeAdjustTask::TSNone;
					}

				}
				else if((AutoSizeEnable & EnableSize) == 0)
				{
					Value = ToolSizeAdjustTask::TSNone;
				}

				
				
				hresult = AddInputRequest( &(m_pToolSizeAdjustTask->m_InputModes[vType]), Value );
				if(S_OK == hresult) 
				{
					BOOL bEnabled =  (Value == ToolSizeAdjustTask::TSFormula);
					EQAdjustSize*  pEQ =  GetEvaluateObject( (ToolSizeAdjustTask::TSValues) vType);
					if(nullptr != pEQ)
					{
						//pEQ->enabled.SetValue(1,bEnabled);
						hresult = AddInputRequest( &(pEQ->enabled), bEnabled );
					}

				}
			}
			else
			{
				hresult = SvOi::Err_16029_InvalidSelection;
			}
			if( S_OK  !=  hresult)
			{
				break;
			}

		}

		if( hresult == S_OK )
		{
			hresult = AddInputRequestMarker();
		}

		if( hresult == S_OK )
		{
			hresult = RunOnce( m_pTool );
		}

		UpdateData( FALSE );
	}

	return hresult;
}




void SVToolAdjustmentDialogSizePage::Refresh( bool bSave /*= true*/ )
{
	CWnd* pWnd = NULL;
	if( nullptr != m_pTool &&  nullptr !=  m_pToolSizeAdjustTask )
	{
		if( bSave )
		{
			SetInspectionData();
		}
		
		EAutoSize AutoSizeEnable = m_pTool->GetAutoSizeEnabled();
		

		///Hide not allowed Controls
		int show = (m_pToolSizeAdjustTask->m_AllowAdjustSize == false)?  SW_HIDE :SW_SHOW;
		
		bool bEnable = (m_pToolSizeAdjustTask->m_AllowAdjustSize == false)?  false :true;
		bEnable = (AutoSizeEnable & EnableSize) == EnableSize  ?  bEnable : false;
		m_ComboBox[ToolSizeAdjustTask::TSWidth].ShowWindow(show);
		m_EditCtrl[ToolSizeAdjustTask::TSWidth].ShowWindow(show);
		m_ComboBox[ToolSizeAdjustTask::TSHeight].ShowWindow(show);
		m_EditCtrl[ToolSizeAdjustTask::TSHeight].ShowWindow(show);
		
		m_ComboBox[ToolSizeAdjustTask::TSWidth].EnableWindow(bEnable);
		m_ComboBox[ToolSizeAdjustTask::TSHeight].EnableWindow(bEnable);
		

		show = (m_pToolSizeAdjustTask->m_AllowAdjustPosition== false)?  SW_HIDE :SW_SHOW;
		bEnable = (m_pToolSizeAdjustTask->m_AllowAdjustPosition == false)?  false :true;
		bEnable = (AutoSizeEnable & EnablePosition) ==  EnablePosition ?  bEnable : false;

		m_ComboBox[ToolSizeAdjustTask::TSPositionX].ShowWindow(show);
		m_EditCtrl[ToolSizeAdjustTask::TSPositionX].ShowWindow(show);
		m_ComboBox[ToolSizeAdjustTask::TSPositionY].ShowWindow(show);
		m_EditCtrl[ToolSizeAdjustTask::TSPositionY].ShowWindow(show);

		m_ComboBox[ToolSizeAdjustTask::TSPositionX].EnableWindow(bEnable);
		m_ComboBox[ToolSizeAdjustTask::TSPositionY].EnableWindow(bEnable);



		// refresh  combo settings...
		for( int vType  = ToolSizeAdjustTask::TSPositionX ; vType <  ToolSizeAdjustTask::TSValuesCount; ++vType)
		{
			long SelMode(0),nShow(SW_HIDE) ;
			CString csResult;
			if(m_pToolSizeAdjustTask->m_InputModes[vType].GetValue( SelMode ) == S_OK )
			{
				
				if((vType == ToolSizeAdjustTask::TSPositionX || vType == ToolSizeAdjustTask::TSPositionY) )
				{
					if((AutoSizeEnable & EnablePosition) ==0)
					{
						SelMode = ToolSizeAdjustTask::TSNone;
					}
					
				}
				else if((AutoSizeEnable & EnableSize) == 0)
				{
					SelMode = ToolSizeAdjustTask::TSNone;
				} 

				
				//m_ComboBox[vType].SetCurSel(Sel);
				m_ComboBox[vType].SetCurSelItemData(SelMode);

				if(SelMode == ToolSizeAdjustTask::TSFormula)
				{
						nShow = SW_SHOW ;
						if(nullptr != m_pEQAdjustSize[vType]  && (m_pEQAdjustSize[vType])->IsEnabled() && (m_pEQAdjustSize[vType])->HasCondition())
						{
							long lResult = static_cast< long >( (m_pEQAdjustSize[vType])->GetYACCResult() );
							csResult.Format( _T("%ld") , lResult );
						}
				}
				else if(SelMode == ToolSizeAdjustTask::TSNone)
				{
					long lvalue;
					if(m_pToolSizeAdjustTask->GetExtentValues(ToolSizeAdjustTask::TSValues(vType),lvalue)== S_OK )
					{
						csResult.Format( _T("%ld") , lvalue );
					}
				}
				else if(SelMode == ToolSizeAdjustTask::TSFullSize)
				{
					long lvalue(0);
					if(ToolSizeAdjustTask::TSValues(vType) == ToolSizeAdjustTask::TSHeight ||ToolSizeAdjustTask::TSValues(vType) == ToolSizeAdjustTask::TSWidth )
					{
						if(m_pToolSizeAdjustTask->GetParentExtentOutputValues(ToolSizeAdjustTask::TSValues(vType),lvalue)== S_OK )
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
			UpdateData( FALSE ); // set data to dialog
	
}

void SVToolAdjustmentDialogSizePage::OnBnClickedButtonFormula(ToolSizeAdjustTask::TSValues  mode)
{

	if(mode < 0 || mode >= ToolSizeAdjustTask::TSValuesCount)
	{
		ASSERT(FALSE);
		return;
	}
	if (m_pEQAdjustSize[mode])
	{
		CString strCaption = m_pEQAdjustSize[mode]->GetName();
		strCaption += _T(" Formula");
		SVFormulaEditorSheetClass dlg(strCaption);
		dlg.SetTaskObject(m_pEQAdjustSize[mode]);
		dlg.DoModal();
		Refresh(true);
	}
}

void SVToolAdjustmentDialogSizePage::OnSelchangeCombo(ToolSizeAdjustTask::TSValues  mode)
{
	/// full size must be selected in both or none of the combo boxes 
	//int sel =  m_ComboBox[mode].GetCurSel();
	//DWORD_PTR selMode = m_ComboBox[mode].GetItemData(sel);
	DWORD_PTR selMode = m_ComboBox[mode].GetCurSelItemData();
	if(selMode == ToolSizeAdjustTask::TSFullSize)
	{
		///set all other to full size 
		for( int vType  = ToolSizeAdjustTask::TSPositionX ; vType <  ToolSizeAdjustTask::TSValuesCount; ++vType)
		{
			if(vType == mode)
			{
				continue;	
			}
			///m_ComboBox[vType].SetCurSel(ToolSizeAdjustTask::TSFullSize);
			m_ComboBox[vType].SetCurSelItemData(ToolSizeAdjustTask::TSFullSize);
		}
	}
	else 
	{
		///Then  all must be not be full sized
		for( int vType  = ToolSizeAdjustTask::TSPositionX ; vType <  ToolSizeAdjustTask::TSValuesCount; ++vType)
		{
			if(vType == mode)
			{
				continue;	
			}
			if  ( m_ComboBox[vType].GetCurSelItemData() == ToolSizeAdjustTask::TSFullSize)
			{
				m_ComboBox[vType].SetCurSelItemData(ToolSizeAdjustTask::TSNone);
			}
		}
	}
	Refresh(true);
}

void SVToolAdjustmentDialogSizePage::OnCbnSelchangeComboWidthMode()
{
	
	OnSelchangeCombo(ToolSizeAdjustTask::TSWidth);
}


void SVToolAdjustmentDialogSizePage::OnCbnSelchangeComboHeightMode()
{
	OnSelchangeCombo(ToolSizeAdjustTask::TSHeight);
}

void SVToolAdjustmentDialogSizePage::OnCbnSelchangeComboPositionX()
{

	OnSelchangeCombo(ToolSizeAdjustTask::TSPositionX);
}
void SVToolAdjustmentDialogSizePage::OnCbnSelchangeComboPositionY()
{

	OnSelchangeCombo(ToolSizeAdjustTask::TSPositionY);
}



void SVToolAdjustmentDialogSizePage::OnBnClickedButtonTaWidthFormula()
{
	return OnBnClickedButtonFormula(ToolSizeAdjustTask::TSWidth);

}


void SVToolAdjustmentDialogSizePage::OnBnClickedButtonPositionY()
{
	return OnBnClickedButtonFormula(ToolSizeAdjustTask::TSPositionY);
}


void SVToolAdjustmentDialogSizePage::OnBnClickedButtonPositionFormulaX()
{
	return OnBnClickedButtonFormula(ToolSizeAdjustTask::TSPositionX);
}


void SVToolAdjustmentDialogSizePage::OnBnClickedButtonTaHeight()
{
	return OnBnClickedButtonFormula(ToolSizeAdjustTask::TSHeight);
}

EQAdjustSize* SVToolAdjustmentDialogSizePage::GetEvaluateObject(ToolSizeAdjustTask::TSValues typ)
{
	
	if(typ < 0 || typ >= ToolSizeAdjustTask::TSValuesCount)
	{
		ASSERT(FALSE);
		return nullptr;
	}

	return m_pEQAdjustSize[typ];
	
}

bool SVToolAdjustmentDialogSizePage::QueryAllowExit()
{
	/// no empty formulas 
	for( int vType  = ToolSizeAdjustTask::TSPositionX ; vType < ToolSizeAdjustTask::TSValuesCount ; ++vType)
	{
		int sel = m_ComboBox[vType].GetCurSel();
		if( sel >= 0 )
		{
			long Value = ( long ) m_ComboBox[vType].GetItemData( sel );
			if(Value == ToolSizeAdjustTask::TSFormula)
			{
				EQAdjustSize*  pEQ =  GetEvaluateObject( (ToolSizeAdjustTask::TSValues) vType);
				if(nullptr != pEQ)
				{
					CString csEqText;
					pEQ->GetEquationText(csEqText);
					if(csEqText.IsEmpty())
					{
						SvStl::ExceptionMgr1 Exception(SvStl::ExpTypeEnum::LogAndDisplay);
						Exception.setMessage( SVMSG_SVO_64_EMPTY_FORMULAS_ARE_NOT_ALLOWED, nullptr, StdExceptionParams, SvOi::Err_16038_EmptyFormula );
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
	if (QueryAllowExit() == false)
	{
		return 0;
	}
	if(nullptr != m_pTool)
	{
		hr  = SVGuiExtentUpdater::ForwardSizeAndPosition(m_pTool, false);
	}
	
	return CPropertyPage::OnKillActive();
}
