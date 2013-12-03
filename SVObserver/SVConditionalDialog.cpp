//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVConditionalDialogClass
//* .File Name       : $Workfile:   SVConditionalDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 12:16:26  $
//******************************************************************************


#include "stdafx.h"
#include "SVConditionalDialog.h"

#include "SVObjectLibrary/SVObjectManagerClass.h"

#include "SVEquation.h"
#include "SVTaskObject.h"

#include "SVIPDoc.h"
#include "SVTool.h"
#include "SVToolSet.h"
#include "SVToolsetOutputSelectionDialog.h"
#include "SVInspectionProcess.h"
#include "SVConditional.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Kludge
#define OBM_DNARROW         32752

/////////////////////////////////////////////////////////////////////////////
// SVConditionalDialogClass dialog


SVConditionalDialogClass::SVConditionalDialogClass(  )
	: CPropertyPage( SVConditionalDialogClass::IDD )
{
	//{{AFX_DATA_INIT(SVConditionalDialogClass)
	StrConstantValue = _T("");
	ConditionDisabled = FALSE;
	Disabled = FALSE;
	ToolsetOutputVariable = _T("");
	//}}AFX_DATA_INIT

	pToolSet		= NULL;
	pTool			= NULL;
	pEquation		= NULL;
	pEquationStruct = NULL;
	numChars = 0;
	ppqComboExtent = 0;

}

HRESULT SVConditionalDialogClass::SetInspectionData()
{
	HRESULT l_hrOk = S_FALSE;

	UpdateData( TRUE );

	SVBoolValueObjectClass *l_psvEnable = NULL;

	if( pTool != NULL )
	{
		l_psvEnable = &( pTool->enabled );
	}
	else if( pToolSet != NULL )
	{
		l_psvEnable = &( pToolSet->enabled );
	}

	// Enable/Disable Tool or ToolSet
	if( l_psvEnable != NULL )
	{
		l_hrOk = AddInputRequest( l_psvEnable, ! Disabled );
	}

	if( l_hrOk == S_OK )
	{
		l_hrOk = AddInputRequest( &( pEquation->enabled ), ! ConditionDisabled );
	}

	if( l_hrOk == S_OK )
	{
		l_hrOk = AddInputRequestMarker();
	}

	if( l_hrOk == S_OK )
	{
		l_hrOk = RunOnce( pTool );
	}

	return l_hrOk;
}

void SVConditionalDialogClass::SetTaskObjectList( 
	SVTaskObjectListClass* pObject,
	LPCTSTR pszToolType      // For disable control caption
)
{
	SVConditionalClass* pCondition = NULL;
	SVObjectClass*		pAncestor = NULL;

	m_pIPDoc = NULL;

	SVObjectTypeInfoStruct info;
	info.ObjectType = SVEquationObjectType;
	info.SubType = SVConditionalObjectType;

	csToolTypeText = pszToolType;        // For disable control caption

	if( SV_IS_KIND_OF( pObject, SVToolSetClass ) )
	{
		pAncestor = pToolSet = ( SVToolSetClass* )pObject; 
		
		// *** // ***
		// get a pointer to the Document
		pInspection = dynamic_cast< SVInspectionProcess* >( pObject->GetAncestor( SVInspectionObjectType ) );
		// *** // ***

		if( pInspection != NULL )
		{
			m_pIPDoc = SVObjectManagerClass::Instance().GetIPDoc( pInspection->GetUniqueObjectID() );
		}

		// Set State of Disable ToolSet CheckBox
		Disabled = !pToolSet->IsEnabled();
		
		// Set the pointer to the Equation Class Object 
		pCondition = (SVConditionalClass*) ::SVSendMessage( pToolSet, SVM_GETFIRST_OBJECT | SVM_NOTIFY_ONLY_FRIENDS, NULL, (DWORD) &info );

		// Set the pointer to the Conditional Class Object 
		setEquation( pCondition );
	}
	else if( SV_IS_KIND_OF( pObject, SVToolClass ) )
	{
		pTool = (SVToolClass*)pObject; 

		pAncestor = pTool->GetAncestor( SVToolSetObjectType );

		// *** // ***
		// get a pointer to the Document
		pInspection = dynamic_cast< SVInspectionProcess* >( pObject->GetAncestor( SVInspectionObjectType ) );
		// *** // ***

		if( pInspection != NULL )
		{
			m_pIPDoc = SVObjectManagerClass::Instance().GetIPDoc( pInspection->GetUniqueObjectID() );
		}

		// Set State of Disable Tool CheckBox
		Disabled = !pTool->IsEnabled();

		// get the Equation object
		pCondition = (SVConditionalClass*) ::SVSendMessage( pTool, SVM_GETFIRST_OBJECT | SVM_NOTIFY_ONLY_FRIENDS, NULL, (DWORD)& info );

		// Set the pointer to the Conditional Class Object 
		setEquation( pCondition );

		if( pAncestor == NULL )
		{
			pAncestor = pToolSet = pInspection->GetToolSet();
		}
	}
}

void SVConditionalDialogClass::setEquation( SVConditionalClass* PEquation )
{
	pEquation = PEquation;
	pEquationStruct = pEquation->GetEquationStruct();
	ConditionDisabled = !pEquation->IsEnabled();
}

void SVConditionalDialogClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVConditionalDialogClass)
	DDX_Control(pDX, ID_TEST, ValidateCtrl);
	DDX_Control(pDX, IDC_CONSTANT_EDIT, ConstantEditCtrl);
	DDX_Control(pDX, IDC_ADD_PPQ_VARIABLE_BUTTON, AddPPQVariableCtrl);
	DDX_Control(pDX, IDC_ADD_LOCAL_VARIABLE_BUTTON, AddLocalVariableCtrl);
	DDX_Control(pDX, IDC_ADD_CONSTANT_BUTTON, AddConstantCtrl);
	DDX_Control(pDX, IDC_DISABLE_CONDITION, DisableConditionCtrl);
	DDX_Control(pDX, IDC_PPQ_VARIABLE_COMBO, PPQVariableComboCtrl);
	DDX_Text(pDX, IDC_LOCAL_VARIABLE_EDIT_CTRL, ToolsetOutputVariable);
	DDX_Control(pDX, IDC_LOCAL_VARIABLE_SELECT, ToolsetOutputSelectButton);
	DDX_Control(pDX, IDC_MATHCOND_RICHEDIT, MathRichEditCtrl);
	DDX_Text(pDX, IDC_CONSTANT_EDIT, StrConstantValue);
	DDX_Check(pDX, IDC_DISABLE_CONDITION, ConditionDisabled);
	DDX_Check(pDX, IDC_DISABLE, Disabled);
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
BOOL SVConditionalDialogClass::createCursorToolbar()
{
	CRect rect,trect;
	CSize size(45,35);
	int numButtons = 5;

	GetDlgItem( IDC_TOOLBAR_STATIC )->GetWindowRect( &rect );

	// Common Cursor movements toolbar
	trect.left   = rect.left + 10;
	trect.right  = trect.left + ( ( size.cx + ( SV_TOOLBAR_BORDER_W * 2 ) ) * numButtons );
	trect.top    = rect.bottom - ( size.cy + ( SV_TOOLBAR_BORDER_H * 2 ) ) - 10;
	trect.bottom = trect.top + size.cy + ( SV_TOOLBAR_BORDER_H * 2 );
	ScreenToClient( &trect );
	BOOL rc = cursorBar.Create(	WS_CHILD | WS_VISIBLE | CCS_NODIVIDER | CBRS_SIZE_DYNAMIC,
								trect, this, IDR_MATH_CURSOR_TOOLBAR, ID_LARROWBUTTON, numButtons, size );

	return rc;
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
BOOL SVConditionalDialogClass::createOperatorToolbars()
{
	CRect	rect;
	CRect	trect;
	CSize	size( 28, 28 );	// Button size x, y
	BOOL BRetVal = TRUE;
	GetDlgItem( IDC_TOOLBAR_STATIC )->GetWindowRect( &rect );

	// Logical Tool Bar...
	int numButtons = 4;
	trect.left   = rect.left + 10;
	trect.right  = trect.left + ( ( size.cx + ( SV_TOOLBAR_BORDER_W * 2 ) ) * numButtons );
	trect.top    = rect.top + 10;
	trect.bottom = trect.top + size.cy + ( SV_TOOLBAR_BORDER_H * 2 );
	ScreenToClient( &trect );
	BRetVal = logicalOperatorBar.Create( WS_CHILD | WS_VISIBLE | CCS_NODIVIDER | CBRS_SIZE_DYNAMIC,
										 trect, this, IDR_MATH_LOGICAL_TOOLBAR, ID_ANDBUTTON, numButtons, size );

	// Conditional Tool Bar...
	numButtons = 6;
	trect.right  = trect.left + ( ( size.cx + ( SV_TOOLBAR_BORDER_W * 2 ) ) * numButtons );
	trect.top    = trect.bottom;
	trect.bottom = trect.top + size.cy + ( SV_TOOLBAR_BORDER_H * 2 );
	BRetVal = conditionalOperatorBar.Create( WS_CHILD | WS_VISIBLE | CCS_NODIVIDER | CBRS_SIZE_DYNAMIC,
										     trect, this, IDR_MATH_CONDITIONAL_TOOLBAR, ID_LESSBUTTON, numButtons, size 
									       ) && BRetVal;

	return BRetVal;
}


/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void SVConditionalDialogClass::insertIntoRichEdit( LPCTSTR TStrValue )
{
	int nStart, nEnd;

	MathRichEditCtrl.GetSel( nStart, nEnd );

	MathRichEditCtrl.ReplaceSel( TStrValue );
	
	//Update the text Count
	numChars += lstrlen(TStrValue);
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void SVConditionalDialogClass::backspaceRichEdit( long Pos )
{
	int nStart, nEnd;

	MathRichEditCtrl.GetSel( nStart, nEnd );

	MathRichEditCtrl.SetSel( nEnd - Pos, nEnd - Pos );
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void SVConditionalDialogClass::advanceRichEdit( long Pos )
{
	int nStart, nEnd;

	MathRichEditCtrl.GetSel( nStart, nEnd );

	MathRichEditCtrl.SetSel( nEnd + Pos, nEnd + Pos );
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void SVConditionalDialogClass::deleteRichEdit( long Pos )
{
	int nStart, nEnd;

	MathRichEditCtrl.GetSel(nStart, nEnd);

	// Delete Selection
	if (nStart != nEnd)
	{
		MathRichEditCtrl.ReplaceSel("");
		numChars -= (nEnd - nStart);
	}
	else
	{
		if (nEnd == numChars) // Backspace
		{
			MathRichEditCtrl.SetSel(nEnd-1,nEnd);
		}
		else // Delete a character
		{
			MathRichEditCtrl.SetSel(nEnd,nEnd+1);
		}
		MathRichEditCtrl.ReplaceSel("");
		numChars--;
	}
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void SVConditionalDialogClass::GetEquationText()
{
	ASSERT( pEquationStruct );
	if( pEquationStruct )
	{
		// Get text from the RichEdit and place into EquationhStruct
		CString equationText;
		MathRichEditCtrl.GetWindowText( equationText );
		pEquationStruct->SetEquationText( equationText );

		if( pEquation->GetTool() != NULL )
		{
			::SVSendMessage( pEquation->GetTool(), SVM_RESET_ALL_OBJECTS, NULL, NULL );
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void SVConditionalDialogClass::setEquationText()
{
	ASSERT( pEquationStruct );
	if( pEquationStruct )
	{
		// Get text from EquationStruct and place into RichEdit
		CString equationText;
		pEquationStruct->GetEquationText( equationText );
		MathRichEditCtrl.SetWindowText( equationText );
		numChars = lstrlen(equationText);
	}
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void SVConditionalDialogClass::EnableControls()
{
	BOOL state = !Disabled;

	DisableConditionCtrl.EnableWindow( state );
	
	state = ( !Disabled && !ConditionDisabled );

	// Disable Controls
	ValidateCtrl.EnableWindow( state );

	if (state)
	{
		// Disable if no PPQ Inputs
		if (PPQVariableComboCtrl.GetCount() <= 0)
		{
			PPQVariableComboCtrl.EnableWindow( false );
			AddPPQVariableCtrl.EnableWindow( false );
		}
		else
		{
			PPQVariableComboCtrl.EnableWindow( true );
			AddPPQVariableCtrl.EnableWindow( true );
		}
	}
	else
	{
		PPQVariableComboCtrl.EnableWindow( state );
		AddPPQVariableCtrl.EnableWindow( state );
	}
	
	ToolsetOutputSelectButton.EnableWindow( state );
	AddLocalVariableCtrl.EnableWindow( state );
	
	ConstantEditCtrl.EnableWindow( state );
	AddConstantCtrl.EnableWindow( state );

	MathRichEditCtrl.EnableWindow( state );

	logicalOperatorBar.EnableWindow( state );
	conditionalOperatorBar.EnableWindow( state );
	cursorBar.EnableWindow( state );
}

BEGIN_MESSAGE_MAP(SVConditionalDialogClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVConditionalDialogClass)
	ON_BN_CLICKED(IDC_ADD_PPQ_VARIABLE_BUTTON, OnAddPPQVariableButton)
	ON_BN_CLICKED(IDC_ADD_LOCAL_VARIABLE_BUTTON, OnAddLocalVariableButton)
	ON_BN_CLICKED(IDC_ADD_CONSTANT_BUTTON, OnAddConstantButton)
	ON_BN_CLICKED(IDC_DISABLE_CONDITION, OnDisableCondition)
	ON_BN_CLICKED(IDC_DISABLE, OnDisable)
	ON_BN_CLICKED(ID_TEST, OnValidate)
	ON_CBN_DROPDOWN(IDC_PPQ_VARIABLE_COMBO, OnDropdownPpqVariableCombo)
	ON_BN_CLICKED(IDC_LOCAL_VARIABLE_SELECT, OnLocalVariableSelect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVConditionalDialogClass message handlers

void SVConditionalDialogClass::OnOK() 
{
	MathRichEditCtrl.GetSel(pEquationStruct->StartPos,pEquationStruct->EndPos);
	GetEquationText();

	SetInspectionData();

	if( Disabled || ConditionDisabled || pEquation->Test().bPassed )
	{
		m_pIPDoc->SetModifiedFlag();

		CPropertyPage::OnOK();
	}
}


void SVConditionalDialogClass::OnAddPPQVariableButton() 
{
	CString text;

	// Get Pointer to SVOutObjectInfo
	int curSel = PPQVariableComboCtrl.GetCurSel();
	if( curSel == CB_ERR )
	{
		AfxMessageBox("The item you are trying to add is not a valid selection\nPlease select an item from the list");
		return;
	}

	// Get Current Selection in combobox
	PPQVariableComboCtrl.GetWindowText( text );
	if( !text.IsEmpty() )
	{
		// Variables are delimited by qouble qoutes
		CString variable = _T( "\"" ) + text + _T( "\"" );
		insertIntoRichEdit( ( LPCTSTR )variable );
	}
}

void SVConditionalDialogClass::OnAddLocalVariableButton() 
{
	UpdateData( TRUE );

	// Get Current Variable name
	if( !ToolsetOutputVariable.IsEmpty() )
	{
		// Variables are delimited by qouble qoutes
		CString variable = _T( "\"" ) + ToolsetOutputVariable + _T( "\"" );
		insertIntoRichEdit( ( LPCTSTR )variable );
	}
}

void SVConditionalDialogClass::OnAddConstantButton() 
{
	UpdateData( TRUE );

	if ( ! StrConstantValue.IsEmpty () )
	{
		insertIntoRichEdit((LPCTSTR)StrConstantValue);
	}
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
BOOL SVConditionalDialogClass::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch (LOWORD(wParam))
	{
		case ID_LESSBUTTON:
			insertIntoRichEdit(" LT ");
			break;
		
		case ID_GREATERBUTTON:
			insertIntoRichEdit(" GT ");
			break;
		
		case ID_LESSEQUALBUTTON:
			insertIntoRichEdit(" LTE ");
			break;
		
		case ID_GREATEREQUALBUTTON:
			insertIntoRichEdit(" GTE ");
			break;
		
		case ID_EQUALBUTTON:
			insertIntoRichEdit(" EQ ");
			break;
		
		case ID_NOTEQUALBUTTON:
			insertIntoRichEdit(" NE ");
			break;

		case ID_ANDBUTTON:
			insertIntoRichEdit(" AND ");
			break;
		
		case ID_ORBUTTON:
			insertIntoRichEdit(" OR ");
			break;
		
		case ID_NOTBUTTON:
			insertIntoRichEdit(" NOT ");
			break;
		
		case ID_XORBUTTON:
			insertIntoRichEdit(" XOR ");
			break;

		case ID_LARROWBUTTON:
			backspaceRichEdit(1);
			break;

		case ID_RARROWBUTTON:
			advanceRichEdit(1);
			break;

		case ID_DELETEBUTTON:
			deleteRichEdit(1);
			break;

		case ID_LPARENBUTTON:
			insertIntoRichEdit("(");
			break;

		case ID_RPARENBUTTON:
			insertIntoRichEdit(")");
			break;
	}

	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL SVConditionalDialogClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	SetTaskObject( pEquation );

							// Put the Down Arrow on the Button
	downArrowBitmap.LoadOEMBitmap( OBM_DNARROW );
	ToolsetOutputSelectButton.SetBitmap( ( HBITMAP )downArrowBitmap );

	if( pToolSet || pTool )
	{
		CString s;
		s = _T("Disable ") + csToolTypeText;
		GetDlgItem(IDC_DISABLE)->SetWindowText((LPCTSTR)s);

		// Get the Conditional class

		ASSERT( pEquationStruct );
	
		if( pEquationStruct )
		{
			MathRichEditCtrl.LimitText( SV_EQUATION_BUFFER_SIZE );

			createCursorToolbar();
			createOperatorToolbars();

			setEquationText();

			// Set CombmoBox Horizontal Extent
			ppqComboExtent = 0;

			// Clear ListBox Extents
			PPQVariableComboCtrl.SetHorizontalExtent( ppqComboExtent );

			// *** // ***
			// populate PPQ Variables ComboBox
			for( size_t i = 0; i < pInspection->m_PPQInputs.size(); i++ )
			{	
				SVValueObjectClass *pValueObject;
				
				pValueObject = pInspection->m_PPQInputs[i].m_IOEntryPtr->m_pValueObject;

				if( !pInspection->m_PPQInputs[i].m_IOEntryPtr->m_Enabled )
					continue;

				// get the width of the string
				int strWidth = GetComboBoxStringExtent( PPQVariableComboCtrl, pValueObject->GetName()  );
				if( strWidth > ppqComboExtent )
					ppqComboExtent = strWidth;

				PPQVariableComboCtrl.AddString( pValueObject->GetName() );
			}// end for
			// *** // ***

			// Getpointer to Tool and Toolset
			if( pTool )
			{
				SVObjectClass* pToolOwner = pTool->GetOwner();
				if( SV_IS_KIND_OF( pToolOwner, SVToolSetClass ) )
					pToolSet = ( SVToolSetClass* )pTool->GetOwner();
			}
			UpdateData( FALSE );
			
			// Enable/Disable Controls
			EnableControls();

			return TRUE;  // return TRUE unless you set the focus to a control
						  // EXCEPTION: OCX Property Pages should return FALSE

		}
	}
	if( GetParent() )
		GetParent()->SendMessage( WM_CLOSE );
	else
		SendMessage( WM_CLOSE );

	return FALSE;
}

void SVConditionalDialogClass::OnDisableCondition() 
{
	UpdateData( TRUE ); // Update the variables
	
	// Enable/Disable Controls	
	EnableControls();

	SetInspectionData();
}

void SVConditionalDialogClass::OnDisable() 
{
	UpdateData( TRUE ); // Update the variables

	// Enable/Disable Controls
	EnableControls();

	SetInspectionData();
}

BOOL SVConditionalDialogClass::ValidateEquation() 
{
	if( GetSafeHwnd() )
	{
		MathRichEditCtrl.GetSel(pEquationStruct->StartPos,pEquationStruct->EndPos);
		GetEquationText();

		SetInspectionData();

		return pEquation->Test().bPassed;
	}
	return TRUE;
}

void SVConditionalDialogClass::OnValidate() 
{
	MathRichEditCtrl.GetSel(pEquationStruct->StartPos,pEquationStruct->EndPos);
	GetEquationText();

	UpdateData( TRUE ); // Update the variables
	
	SetInspectionData();

	if( pEquation->Test().bPassed )
	{
		CString tmp;
        //
        // A nonzero result is TRUE,  zero result is FALSE.
        //
		//BOOL value = (int)pEquation->GetYACCResult(); 
        BOOL value;
        value = pEquation->GetYACCResult() ? TRUE : FALSE;
		
		CString boolStr = ( ( value ) ? _T( "TRUE" ) : _T( "FALSE" ) );

		tmp.Format("Conditional Equation Validated Successfully\n Condition = %s", boolStr); 

		AfxMessageBox(tmp);
	}
	//else // Something is wrong
}


int SVConditionalDialogClass::GetComboBoxStringExtent( CComboBox& RComboBox, LPCTSTR szStr ) 
{ 
     CDC         *pDC; 
     pDC = RComboBox.GetDC(); 
  
    CSize size = pDC->GetOutputTextExtent( szStr ); 
    
    RComboBox.ReleaseDC( pDC );

    return (int)(size.cx * 0.80);
} 

void SVConditionalDialogClass::OnDropdownPpqVariableCombo() 
{
	// This does not work cause the combobox list does not have a horizontal scrollbar
	//PPQVariableComboCtrl.SetHorizontalExtent( ppqComboExtent );

	// Instead - set the width of the drop down list to 
	// the size of tyhe widest string (in pixels)
	int width = PPQVariableComboCtrl.GetDroppedWidth();
	if( width < ppqComboExtent )
		PPQVariableComboCtrl.SetDroppedWidth( ppqComboExtent );
}

void SVConditionalDialogClass::OnLocalVariableSelect() 
{
	SVToolsetOutputSelectionDialog dlg;
	
	dlg.PTaskObjectList = pToolSet;
	dlg.uAttributesDesired = SV_SELECTABLE_FOR_EQUATION;

	INT_PTR nResult = dlg.DoModal();
	if( nResult == IDOK )
	{
		// Get Toolset Output Name
		// Update ToolsetOutputVariable
		ToolsetOutputVariable = dlg.m_sSelectedOutputName;
		UpdateData( FALSE );
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVConditionalDialog.cpp_v  $
 * 
 *    Rev 1.1   01 Oct 2013 12:16:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 09:59:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   02 Jul 2012 16:39:48   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to promote new display functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   18 Apr 2011 09:22:12   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to move the shared data for the IO Entry data to a different class to fix issues with PPQ and Inspection shared data.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   08 Dec 2010 07:45:20   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to include changes in notification functionality using the Observer Design Pattern.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   05 Feb 2009 16:17:48   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  649
   SCR Title:  Update Equation Dialog to disable the PPQ Variable Edit/Selection Field
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised to disable Combobox and add button for PPQ variables if there are no PPQ variables to select.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   15 Sep 2005 07:37:12   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Added code to for the owning tool of the equation to reset all of its object to make sure sub-tool parts get updated properly.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   08 Aug 2005 15:33:34   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated objects and dialogs to use the ActiveX Interface to update inspection data.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   22 Jun 2005 08:09:44   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     bPassed on Equation Test
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   17 Feb 2005 13:28:04   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Update class with new methodologies based on new base object, extents and reset structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   12 Jul 2004 12:29:46   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  429
   SCR Title:  Fix Select Object tree control
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     changed SVToolsetOutputSelectionDialog member variable name
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   17 Apr 2003 17:16:50   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   29 Jan 2003 15:07:08   joe
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated include file information.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   19 Nov 2002 11:07:30   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Removed COM support
               SetTaskObjectList
                           Removed hack
               OnInitDialog
                           Changed source of combo box values
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   09 Nov 2001 13:22:08   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  255
   SCR Title:  Add Sub-pixel resolution to Profile and Line Gage Tool Analyzers
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Changed calls to SVToolBar::Create in functions createCursorToolbar & createOperatorToolbars
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   31 Oct 2001 16:19:38   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  252
   SCR Title:  System hang on .5 dual when accessing Math Tool after going online
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Removed rich edit control calls in the constructor and OnInitDialog. Changed local parameters for GetSel/SetSel from long to int.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
