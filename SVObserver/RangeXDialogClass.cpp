//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRangeXDialog D
//* .File Name       : $Workfile:   RangeXDialogClass.cpp  $
//* .Description     : RangeXDialogClass this dialog is used instead of RangeDialogclass when indirect values for the rangeobjects are allowed
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   26 Jan 2015 11:17:46  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include <Windows.h>
#include "SVObserver.h"
#include "RangeXDialogClass.h"
#include "afxdialogex.h"
#include "SVRange.h"
#include "svresult.h"
#include "ObjectNameHelper.h"
#include "SVUtilityLibrary/NameSelectionList.h"
#include "SVInspectionProcess.h"
#include "SVToolSet.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "SVObjectLibrary/GlobalConst.h"
#include "SVTool.h"
#include "FormulaController.h"
#include "SVPPQObject.h"
#pragma endregion Includes

#pragma region Declarations
using namespace Seidenader::SVObjectLibrary;
using namespace Seidenader::ObjectSelectorLibrary;
#pragma endregion Declarations

IMPLEMENT_DYNAMIC(RangeXDialogClass, CDialog)

RangeXDialogClass::RangeXDialogClass(SVRangeClass* range, CWnd* parent /*=NULL*/)
: CDialog(RangeXDialogClass::IDD, parent)
, m_RangeClassHelper(range)
, m_LastSelected(ER_COUNT)
{
}

RangeXDialogClass::~RangeXDialogClass()
{
}

void RangeXDialogClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_FAILHIGH, m_EditFailHigh);
	DDX_Control(pDX, IDC_EDIT_WARNHIGH, m_EditWarnHigh);
	DDX_Control(pDX, IDC_EDIT_WARNLOW, m_EditWarnLow);
	DDX_Control(pDX, IDC_EDIT_FAILLOW, m_EditFailLow);
	DDX_Control(pDX, IDC_BUTTON_FAILHIGH, m_ButtonFailHigh);
	DDX_Control(pDX, IDC_BUTTON_WARNHIGH, m_ButtonWarnHigh);
	DDX_Control(pDX, IDC_BUTTON_WARNLOW, m_ButtonWarnLow);
	DDX_Control(pDX, IDC_BUTTON_FAILLOW, m_ButtonFailLow);
}

BEGIN_MESSAGE_MAP(RangeXDialogClass, CDialog)
	ON_BN_CLICKED(IDOK, &RangeXDialogClass::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &RangeXDialogClass::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_FAILHIGH, &RangeXDialogClass::OnBnClickedFailHighIndirect)
	ON_BN_CLICKED(IDC_BUTTON_WARNHIGH, &RangeXDialogClass::OnBnClickedWarnlHighIndirect)
	ON_BN_CLICKED(IDC_BUTTON_WARNLOW, &RangeXDialogClass::OnBnClickedWarnLowIndirect)
	ON_BN_CLICKED(IDC_BUTTON_FAILLOW, &RangeXDialogClass::OnBnClickedFailedLowIndirect)
END_MESSAGE_MAP()

void RangeXDialogClass::OnBnClickedOk()
{
	CString errorMsg;
	bool bOK = GetDlgData();

	if(bOK)
	{
		HRESULT hres = m_RangeClassHelper.CheckInternalData(errorMsg);
		if( S_OK != hres)
		{
			bOK = false;
			AfxMessageBox(errorMsg, MB_OK | MB_ICONERROR);
		}
	}

	if(bOK)
	{
		m_RangeClassHelper.SetInspectionData();
		CDialog::OnOK();
	}
}

void RangeXDialogClass::OnBnClickedCancel()
{
	CDialog::OnCancel();
}

BOOL RangeXDialogClass::OnInitDialog()
{
	__super::OnInitDialog();

	// Put the Down Arrow on the Button
	m_downArrowBitmap.LoadOEMBitmap( OBM_DNARROW );

	//(HBITMAP) is a call to the overloaded function operator HBITMAP and no c style cast
	m_ButtonFailHigh.SetBitmap( ( HBITMAP )m_downArrowBitmap );
	m_ButtonWarnHigh.SetBitmap( ( HBITMAP )m_downArrowBitmap );
	m_ButtonWarnLow.SetBitmap( ( HBITMAP )m_downArrowBitmap );
	m_ButtonFailLow.SetBitmap( ( HBITMAP )m_downArrowBitmap );

	m_RangeClassHelper.SetRangeTaskObject();
	m_RangeClassHelper.GetAllInspectionData();

	SetDlgData();

	CString title = m_RangeClassHelper.GetOwnerName();
	SetWindowText( title );

	// Ensure the title is readable
	CRect rect;
	CSize size;
	GetWindowRect(&rect);
	CDC* pDC = GetDC();
	if(nullptr != pDC )
	{
		size = pDC->GetTextExtent(title);
		ReleaseDC(pDC);
	}

	if (size.cx > rect.Width())
	{
		int borderWidth = GetSystemMetrics(SM_CXDLGFRAME);
		//make the windows with 24 pixel wider then the text.
		int width = size.cx + (borderWidth * 2) + 24;
		SetWindowPos(&CWnd::wndTop, 0, 0, width, rect.Height(), SWP_NOMOVE | SWP_NOZORDER);
	}

	return TRUE;
}

void RangeXDialogClass::SetDlgData()
{
	CString csText;

	if(m_RangeClassHelper.m_FailHighIndirect.GetLength() > 0)
	{
		m_EditFailHigh.SetWindowText(m_RangeClassHelper.m_FailHighIndirect);
	}
	else
	{
		csText.Format(_T("%lf"),m_RangeClassHelper.m_FailHigh );
		m_EditFailHigh.SetWindowText(csText);
	}

	if(m_RangeClassHelper.m_WarnHighIndirect.GetLength() > 0)
	{
		m_EditWarnHigh.SetWindowText(m_RangeClassHelper.m_WarnHighIndirect);
	}
	else
	{
		csText.Format(_T("%lf"),m_RangeClassHelper.m_WarnHigh );
		m_EditWarnHigh.SetWindowText(csText);
	}

	if(m_RangeClassHelper.m_FailLowIndirect.GetLength() > 0)
	{
		m_EditFailLow.SetWindowText(m_RangeClassHelper.m_FailLowIndirect);
	}
	else
	{
		csText.Format(_T("%lf"),m_RangeClassHelper.m_FailLow );
		m_EditFailLow.SetWindowText(csText);
	}

	if(m_RangeClassHelper.m_WarnLowIndirect.GetLength() > 0)
	{
		m_EditWarnLow.SetWindowText(m_RangeClassHelper.m_WarnLowIndirect);
	}
	else
	{
		csText.Format(_T("%lf"),m_RangeClassHelper.m_WarnLow );
		m_EditWarnLow.SetWindowText(csText);
	}
}

bool RangeXDialogClass::GetDlgData()
{
	bool res = true;
	CString errormsg, csText;
	ERange Eerror = ER_COUNT;

	if(res)
	{
		m_EditFailHigh.GetWindowText(csText);
		if( S_OK != m_RangeClassHelper.SetInternalData(ER_FailHigh, csText, errormsg))
		{
			res = false;
			m_EditFailHigh.SetFocus();
			AfxMessageBox(errormsg,MB_OK | MB_ICONERROR);
			Eerror = ER_FailHigh;
			csText = m_RangeClassHelper.GetStringFromRange(ER_FailHigh);
			m_EditFailHigh.SetWindowText(csText);
		}
	}

	if(res)
	{
		m_EditWarnHigh.GetWindowText(csText);
		if(S_OK != m_RangeClassHelper.SetInternalData(ER_WarnHigh, csText, errormsg) )
		{
			m_EditWarnHigh.SetFocus();
			res = false;
			Eerror = ER_WarnHigh;

			AfxMessageBox(errormsg, MB_OK | MB_ICONERROR);
			csText = m_RangeClassHelper.GetStringFromRange(ER_WarnHigh);
			m_EditWarnHigh.SetWindowText(csText);
		}
	}

	if(res)
	{
		m_EditFailLow.GetWindowText(csText);
		if(S_OK != m_RangeClassHelper.SetInternalData(ER_FailLow, csText, errormsg) )
		{
			m_EditFailLow.SetFocus();
			res = false;
			Eerror = ER_FailLow;
			AfxMessageBox(errormsg, MB_OK | MB_ICONERROR);
			csText = m_RangeClassHelper.GetStringFromRange(ER_FailLow);
			m_EditFailLow.SetWindowText(csText);
		}
	}

	if(res)
	{
		m_EditWarnLow.GetWindowText(csText);
		if(S_OK != m_RangeClassHelper.SetInternalData(ER_WarnLow, csText, errormsg) )
		{
			m_EditWarnLow.SetFocus();
			res = false;
			Eerror = ER_WarnLow;
			AfxMessageBox(errormsg, MB_OK | MB_ICONERROR);
			csText = m_RangeClassHelper.GetStringFromRange(ER_WarnLow);
			m_EditWarnLow.SetWindowText(csText);
		}
	}

	return res;
}

bool RangeXDialogClass::ShowObjectSelector(CString& name)
{
	if (nullptr == m_RangeClassHelper.m_pRange)
	{
		return false;
	}

	SVInspectionProcess* pInspectionProcess = m_RangeClassHelper.m_pRange->GetInspection();
	if(nullptr == pInspectionProcess)
	{
		return false; // @TODO:  Better to return a unique error code.
	}

	SVTaskObjectListClass* pTaskObjectList = dynamic_cast<SVTaskObjectListClass*>(pInspectionProcess->GetToolSet());
	if(nullptr == pTaskObjectList)
	{
		return false; // @TODO:  Better to return a unique error code.
	}

	bool result = false;
	SVToolClass* pTool = m_RangeClassHelper.m_pRange->GetTool();
	CString csToolCompleteName;
	if(pTool)
	{
		csToolCompleteName = pTool->GetCompleteObjectName();
		csToolCompleteName += _T(".");
	}

	SVStringArray nameArray;
	typedef std::insert_iterator<SVStringArray> Inserter;
	SVString InspectionName;
	SVString PPQName = FqnPPQVariables; 
	
	InspectionName = pInspectionProcess->GetName();
	SVPPQObject* pPPQ = pInspectionProcess->GetPPQ();
	if( nullptr != pPPQ )
	{
		PPQName = pPPQ->GetName();
	}
	

	ObjectTreeGenerator::Instance().setLocationFilter( ObjectTreeGenerator::FilterInput, InspectionName, SVString( _T("") ) );
	ObjectTreeGenerator::Instance().setLocationFilter( ObjectTreeGenerator::FilterOutput, InspectionName, SVString( _T("") ) );

	SVString InspectionNameDot = InspectionName + SVString( _T("."));
	SVString PPQNameDot = PPQName + SVString( _T("."));
	
	ObjectTreeGenerator::Instance().setLocationFilter( ObjectTreeGenerator::FilterOutput, PPQName, SVString( _T("")  ));
	ObjectTreeGenerator::Instance().setSelectorType( ObjectTreeGenerator::SelectorTypeEnum::TypeSingleObject );

	// Insert Tool Set Objects
	ObjectNameHelper::BuildObjectNameList(pTaskObjectList, Inserter(nameArray, nameArray.begin()), csToolCompleteName);
	ObjectTreeGenerator::Instance().insertTreeObjects( nameArray );

	if(name.GetLength() > 0)
	{
		SVStringSet nameSet;
		nameSet.insert(name);
		ObjectTreeGenerator::Instance().setCheckItems(nameSet);
	}

	CString Title = m_RangeClassHelper.GetOwnerName();
	Title += _T(": ");
	Title += RangeClassHelper::ERange2String(m_LastSelected);

	CString mainTabTitle;
	mainTabTitle.LoadString( IDS_SELECT_TOOLSET_OUTPUT );
	CString FilterTab;
	FilterTab.LoadString( IDS_FILTER );

	INT_PTR Result = ObjectTreeGenerator::Instance().showDialog( Title, mainTabTitle, FilterTab, this );

	if( IDOK == Result )
	{
		name = ObjectTreeGenerator::Instance().getSingleObjectResult().getLocation().c_str(); // @TODO:  Should we check the return values of getSingleObjectResult and getLocation?
		result = true;
	}

	return result;
}

// @TODO:  The next 4 methods are very similar.  Consider refactoring to call a common method.  Otherwise, clean up the camelcase and result checking in all the methods.
void RangeXDialogClass::OnBnClickedFailHighIndirect()
{
	m_LastSelected = ER_FailHigh;
	CString csText;
	m_EditFailHigh.GetWindowText(csText); // @TODO:  Should check to see if GetWindowText worked before using the value it returned.
	if (ShowObjectSelector(csText))
	{
		m_EditFailHigh.SetWindowText(csText);
	}
}

void RangeXDialogClass::OnBnClickedWarnlHighIndirect()
{
	m_LastSelected = ER_WarnHigh;
	CString csText;
	m_EditWarnHigh.GetWindowText(csText);
	if (ShowObjectSelector(csText) )
	{
		m_EditWarnHigh.SetWindowText(csText);
	}
}

void RangeXDialogClass::OnBnClickedWarnLowIndirect()
{
	m_LastSelected = ER_WarnLow;
	CString csText;
	m_EditWarnLow.GetWindowText(csText);
	if (ShowObjectSelector(csText) )
	{
		m_EditWarnLow.SetWindowText(csText);
	}
}

void RangeXDialogClass::OnBnClickedFailedLowIndirect()
{
	m_LastSelected = ER_FailLow;
	CString csText;
	m_EditFailLow.GetWindowText(csText);
	if (ShowObjectSelector(csText) )
	{
		m_EditFailLow.SetWindowText(csText);
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\RangeXDialogClass.cpp_v  $
 * 
 *    Rev 1.6   26 Jan 2015 11:17:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  979
 * SCR Title:  Provide additional options to input the feature range for the blob analyzer.
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Change window text to Select Toolset Output.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   19 Jan 2015 11:41:20   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  979
 * SCR Title:  Provide additional options to input the feature range for the blob analyzer.
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Changed method ShowObjectSelector to remove PPQ node.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 Jan 2015 08:26:44   mEichengruen
 * Project:  SVObserver
 * Change Request (SCR) nbr:  979
 * SCR Title:  Provide additional options to input the feature range for the blob analyzer.
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Change Name for Inputs in Objectselector
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   13 Jan 2015 13:12:10   mEichengruen
 * Project:  SVObserver
 * Change Request (SCR) nbr:  979
 * SCR Title:  Provide additional options to input the feature range for the blob analyzer.
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Range Indirect name String without inspection Name 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   12 Jan 2015 17:31:18   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  979
 * SCR Title:  Provide additional options to input the feature range for the blob analyzer.
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Changed method ShowObjectSelector to add "PPQ Variables" at the top of the list.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Jan 2015 11:59:22   mEichengruen
 * Project:  SVObserver
 * Change Request (SCR) nbr:  979
 * SCR Title:  Provide additional options to input the feature range for the blob analyzer.
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Fix bug when using  last valid values by wrong user input.
 * 
 * Allow PPQ Variables for indirect range values 
 * 
 * Dont show Inspection_1 in Selector for Range Values
 * Show PPQ Varaiables in Selector for Range Values 
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 Dec 2014 13:53:42   mEichengruen
 * Project:  SVObserver
 * Change Request (SCR) nbr:  979
 * SCR Title:  Provide additional options to input the feature range for the blob analyzer.
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Initial Check In for SCR 979 Indirect Range Objects 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/