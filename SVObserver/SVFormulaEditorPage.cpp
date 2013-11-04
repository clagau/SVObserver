//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFormulaEditorPageClass
//* .File Name       : $Workfile:   SVFormulaEditorPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   13 May 2013 16:29:24  $
//******************************************************************************

#include "stdafx.h"
#include "SVIPDoc.h"
#include "SVInspectionProcess.h"
#include "SVTool.h"
#include "SVToolSet.h"
#include "SVFormulaEditorPage.h"
#include "SVMathContainer.h"
#include "SVEquation.h"
#include "SVToolsetOutputSelectionDialog.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constant types
enum
{
	SV_FORMULA_DECIMAL_CONSTANT_TYPE = 0,
	SV_FORMULA_HEXADECIMAL_CONSTANT_TYPE = 1,
	SV_FORMULA_BINARY_CONSTANT_TYPE = 2,

	// Kludge
	OBM_DNARROW         = 32752,
};

/////////////////////////////////////////////////////////////////////////////
// SVFormulaEditorPageClass dialog

BEGIN_MESSAGE_MAP(SVFormulaEditorPageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVFormulaEditorPageClass)
	ON_BN_CLICKED(IDC_ADD_PPQ_VARIABLE_BUTTON, OnAddPPQVariableButton)
	ON_BN_CLICKED(IDC_ADD_LOCAL_VARIABLE_BUTTON, OnAddLocalVariableButton)
	ON_BN_CLICKED(IDC_ADD_CONSTANT_BUTTON, OnAddConstantButton)
	ON_CBN_DROPDOWN(IDC_PPQ_VARIABLE_COMBO, OnDropdownPpqVariableCombo)
	ON_BN_CLICKED(IDC_LOCAL_VARIABLE_SELECT, OnLocalVariableSelect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


SVFormulaEditorPageClass::SVFormulaEditorPageClass( SVTaskObjectClass* pTaskObject )
	: CPropertyPage( SVFormulaEditorPageClass::IDD )
{
	//{{AFX_DATA_INIT(SVFormulaEditorPageClass)
	m_StrConstantValue = _T("");
	m_constantType = 0;
	m_strToolsetOutputVariable = _T("");
	//}}AFX_DATA_INIT

	m_pToolSet        = NULL;
	m_pTool			= NULL;
	m_pMathContainer  = NULL;

	m_pEquation		= NULL;
	m_pEquationStruct = NULL;
	m_numChars = 0;
	m_ppqComboExtent = 0;

	SetTaskObject( pTaskObject );
}

void SVFormulaEditorPageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVFormulaEditorPageClass)
	DDX_Control(pDX, IDC_LOCAL_VARIABLE_SELECT, m_ToolsetOutputSelectButton);
	DDX_Control(pDX, IDC_CONSTANT_EDIT, m_ConstantEditCtrl);
	DDX_Control(pDX, IDC_ADD_PPQ_VARIABLE_BUTTON, m_AddPPQVariableCtrl);
	DDX_Control(pDX, IDC_ADD_LOCAL_VARIABLE_BUTTON, m_AddLocalVariableCtrl);
	DDX_Control(pDX, IDC_ADD_CONSTANT_BUTTON, m_AddConstantCtrl);
	DDX_Control(pDX, IDC_PPQ_VARIABLE_COMBO, m_PPQVariableComboCtrl);
	DDX_Control(pDX, IDC_MATHCOND_RICHEDIT, m_MathRichEditCtrl);
	DDX_Text(pDX, IDC_CONSTANT_EDIT, m_StrConstantValue);
	DDX_Radio(pDX, IDC_DECIMAL, m_constantType);
	DDX_Text(pDX, IDC_LOCAL_VARIABLE_EDIT_CTRL, m_strToolsetOutputVariable);
	//}}AFX_DATA_MAP
}

BOOL SVFormulaEditorPageClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	// Put the Down Arrow on the Button
	m_downArrowBitmap.LoadOEMBitmap( OBM_DNARROW );
	m_ToolsetOutputSelectButton.SetBitmap( ( HBITMAP )m_downArrowBitmap );

	if( m_pToolSet || m_pTool || m_pMathContainer )
	{
		ASSERT( m_pEquationStruct );
	
		if( m_pEquationStruct )
		{
			m_MathRichEditCtrl.LimitText( SV_EQUATION_BUFFER_SIZE );

			createCursorToolbar();
			createOperatorToolbars();

			setEquationText();

			// Set CombmoBox Horizontal Extent
			m_ppqComboExtent = 0;

			// Clear ListBox Extents
			m_PPQVariableComboCtrl.SetHorizontalExtent( m_ppqComboExtent );

			// *** // ***
			// populate PPQ Variables ComboBox
			for( size_t i = 0; i < m_pInspection->m_PPQInputs.size(); i++ )
			{	
				SVValueObjectClass* pValueObject;
				
				pValueObject = m_pInspection->m_PPQInputs[i].m_IOEntryPtr->m_pValueObject;

				if( !m_pInspection->m_PPQInputs[i].m_IOEntryPtr->m_Enabled )
					continue;

				// get the width of the string
				int strWidth = GetComboBoxStringExtent( m_PPQVariableComboCtrl, pValueObject->GetName() );
				if( strWidth > m_ppqComboExtent )
					m_ppqComboExtent = strWidth;

				m_PPQVariableComboCtrl.AddString( pValueObject->GetName() );
			}// end for
			// *** // ***

			// Disable if no PPQ Inputs
			if ( m_PPQVariableComboCtrl.GetCount() <= 0 )
			{
				m_PPQVariableComboCtrl.EnableWindow( false );
				m_AddPPQVariableCtrl.EnableWindow( false );
			}

			// Get Tool and Toolset Pointers
			if( m_pMathContainer )
			{
				// Try to get Tool....
				m_pTool = m_pMathContainer->GetTool();
			}
			if( m_pTool )
			{
				m_pToolSet = dynamic_cast < SVToolSetClass* > ( m_pTool->GetOwner() );
			}
			
			UpdateData( FALSE );
			
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

void SVFormulaEditorPageClass::OnOK() 
{
	m_MathRichEditCtrl.GetSel(m_pEquationStruct->StartPos, m_pEquationStruct->EndPos);
	BOOL l_bPassed = GetEquationText();

	UpdateData( TRUE );
	
	SVEquationTestResult result = m_pEquation->Test();
	if( result.bPassed && l_bPassed )
	{
		CPropertyPage::OnOK();
	}
	else // Equation is invalid
	{
		HandleValidateError( result );
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL SVFormulaEditorPageClass::createOperatorToolbars()
{
	CRect	rect;
	CRect	trect;
	CSize	size( 28, 28 );
	int numButtons;
	BOOL BRetVal = TRUE;

	// Get base rectangle
	GetDlgItem( IDC_TOOLBAR_STATIC )->GetWindowRect( &rect );
	ScreenToClient( &rect );

	// Logical Tool Bar...
	numButtons = 4;
	trect.left   = rect.left;
	trect.top    = rect.top + SV_TOOLBAR_BORDER_H;
	trect.right  = trect.left + ( ( size.cx + ( SV_TOOLBAR_BORDER_W * 2 ) ) * numButtons );
	trect.bottom = trect.top + size.cy + ( SV_TOOLBAR_BORDER_H * 2 );
	BRetVal = m_logicalOperatorBar.Create( WS_CHILD | WS_VISIBLE | CCS_NODIVIDER | CBRS_SIZE_DYNAMIC,
										 trect, this, IDR_MATH_LOGICAL_TOOLBAR, ID_ANDBUTTON, numButtons, size ) && BRetVal;

	// Basic Arithmetic Tool Bar
	numButtons = 4;
	trect.left	 = trect.right;
	trect.right  = trect.left + ( ( size.cx + ( SV_TOOLBAR_BORDER_W * 2 ) ) * numButtons );
	trect.bottom = trect.top + size.cy + ( SV_TOOLBAR_BORDER_H * 2 );
	BRetVal = m_basicOperatorBar.Create( WS_CHILD | WS_VISIBLE | CCS_NODIVIDER | CBRS_SIZE_DYNAMIC,
									trect, this, IDR_MATH_BASIC_TOOLBAR, ID_PLUSBUTTON, numButtons, size) && BRetVal;

	// Miscellaineous operators
	numButtons = 5;
	trect.left = rect.left;
	trect.top = trect.bottom + SV_TOOLBAR_BORDER_H;
	trect.right = trect.left + ( ( size.cx + ( SV_TOOLBAR_BORDER_W * 2 ) ) * numButtons );
	trect.bottom = trect.top + size.cy + ( SV_TOOLBAR_BORDER_H * 2 );
	BRetVal = m_miscOperatorBar.Create( WS_CHILD | WS_VISIBLE | CCS_NODIVIDER | CBRS_SIZE_DYNAMIC,
									trect, this, IDR_MATH_MISC_TOOLBAR, ID_MODBUTTON, numButtons, size ) && BRetVal;

	// Conditional Tool Bar...
	numButtons = 6;
	trect.left   = trect.right;
	trect.right  = trect.left + ( ( size.cx + ( SV_TOOLBAR_BORDER_W * 2 ) ) * numButtons );
	trect.bottom = trect.top + size.cy + ( SV_TOOLBAR_BORDER_H * 2 );
	BRetVal = m_conditionalOperatorBar.Create( WS_CHILD | WS_VISIBLE | CCS_NODIVIDER | CBRS_SIZE_DYNAMIC,
										     trect, this, IDR_MATH_CONDITIONAL_TOOLBAR, ID_LESSBUTTON, numButtons,
											 size ) && BRetVal;
	
	// Triginometric Operators
	numButtons = 6;
	trect.left = trect.right;
	trect.right = trect.left + ( ( size.cx + ( SV_TOOLBAR_BORDER_W * 2 ) ) * numButtons );
	trect.bottom = trect.top + size.cy + ( SV_TOOLBAR_BORDER_H * 2 );
	BRetVal = m_trigOperatorBar.Create( WS_CHILD | WS_VISIBLE | CCS_NODIVIDER | CBRS_SIZE_DYNAMIC,
									trect, this, IDR_MATH_TRIGONOMETRIC_TOOLBAR, ID_SINBUTTON, numButtons, size ) && BRetVal;
	
	// Statistics Tool Bar...
	numButtons = 6;
	trect.left   = rect.left;	
	trect.top    = trect.bottom + SV_TOOLBAR_BORDER_H;
	trect.right  = trect.left + ( ( size.cx + ( SV_TOOLBAR_BORDER_W * 2 ) ) * numButtons );
	trect.bottom = trect.top + size.cy + ( SV_TOOLBAR_BORDER_H * 2 );
	BRetVal = m_statisticsOperatorBar.Create( WS_CHILD | WS_VISIBLE | CCS_NODIVIDER | CBRS_SIZE_DYNAMIC,
										 trect, this, IDR_MATH_STATISTICS_TOOLBAR, ID_MINBUTTON, numButtons, size ) && BRetVal;
	
	return BRetVal;
}

/////////////////////////////////////////////////////////////////////////////
BOOL SVFormulaEditorPageClass::createCursorToolbar()
{
	CRect rect,trect;
	CSize size( 50, 28 );
	int numButtons = 1;

	// place under the rich edit control
	GetDlgItem( IDC_TOOLBAR_STATIC )->GetWindowRect( &rect );
	ScreenToClient( &rect );

	// Validate toolbar
	trect.right  = rect.right - 1;
	trect.left   = trect.right - ( ( size.cx + ( SV_TOOLBAR_BORDER_W * 2 ) ) * numButtons );
	trect.bottom = rect.bottom - ( SV_TOOLBAR_BORDER_H * 2 );
	trect.top    = trect.bottom - ( size.cy + ( SV_TOOLBAR_BORDER_H * 2 ) );
	
	BOOL bRetVal = m_validateBar.Create(	WS_CHILD | WS_VISIBLE | CCS_NODIVIDER | CBRS_SIZE_DYNAMIC,
								trect, this, IDR_FORMULA_VALIDATE_TOOLBAR, ID_FORMULA_TEST, numButtons, size );

	size = CSize(37,28);
	numButtons = 3;
	// Common Cursor movements toolbar
	trect.right  = trect.left;
	trect.left   = trect.right - ( ( size.cx + ( SV_TOOLBAR_BORDER_W * 2 ) ) * numButtons );
	
	bRetVal = m_cursorBar.Create(	WS_CHILD | WS_VISIBLE | CCS_NODIVIDER | CBRS_SIZE_DYNAMIC,
								trect, this, IDR_FORMULA_CURSOR_TOOLBAR, ID_FORMULA_LARROWBUTTON, numButtons, size );

	// Punctuation toolbar
	numButtons = 3;
	size = CSize( 22, 28 );

	trect.right = trect.left;
	trect.left = trect.right - ( ( size.cx + ( SV_TOOLBAR_BORDER_W * 2 ) ) * numButtons );
	bRetVal = m_punctuationBar.Create(	WS_CHILD | WS_VISIBLE | CCS_NODIVIDER | CBRS_SIZE_DYNAMIC,
								trect, this, IDR_MATH_PUNCTUATION_TOOLBAR, ID_FORMULA_LPARENBUTTON, numButtons, size );

	/*
	// Array toolbar
	numButtons = 2;
	size = CSize( 45, 28 );

	trect.right = trect.left;
	trect.left = trect.right - ( ( size.cx + ( SV_TOOLBAR_BORDER_W * 2 ) ) * numButtons );
	bRetVal = m_ArrayBar.Create(	WS_CHILD | WS_VISIBLE | CCS_NODIVIDER | CBRS_SIZE_DYNAMIC,
								trect, this, IDR_FORMULA_ARRAY_TOOLBAR, ID_FORMULA_ARRAY_BUTTON, numButtons, size );
	*/

	return bRetVal;
}

/////////////////////////////////////////////////////////////////////////////
void SVFormulaEditorPageClass::SetTaskObject( SVTaskObjectClass* pObject )
{
	SVObjectTypeInfoStruct info;
	info.ObjectType = SVEquationObjectType;
	info.SubType    = SVMathEquationObjectType;	// we are not looking for conditional equation !!!

	if( pObject != NULL )
	{
		m_pToolSet = pObject->GetInspection()->GetToolSet();
		m_pTool = pObject->GetTool();
		m_pInspection = pObject->GetInspection();

		SVEquationClass* pEquation = dynamic_cast <SVEquationClass*> (pObject);
		if ( pEquation == NULL )
		{
			pEquation = dynamic_cast < SVEquationClass* >
				( reinterpret_cast < SVObjectClass* > (::SVSendMessage( pObject, SVM_GETFIRST_OBJECT | SVM_NOTIFY_ONLY_FRIENDS, NULL, (DWORD) &info ) ));
		}

		m_pMathContainer = dynamic_cast < SVMathContainerClass* > ( pObject ); 

		// Set the pointer to the Equation Class Object 
		setEquation( pEquation );
	}


}

/////////////////////////////////////////////////////////////////////////////
void SVFormulaEditorPageClass::setEquation( SVEquationClass* p_pEquation )
{
	m_pEquation = p_pEquation;
	if( m_pEquation != NULL )
	{
		m_pEquationStruct = m_pEquation->GetEquationStruct();
	}
	else
	{
		m_pEquationStruct = NULL;
	}
}



/////////////////////////////////////////////////////////////////////////////
void SVFormulaEditorPageClass::insertIntoRichEdit( LPCTSTR tszValue )
{
	int nStart, nEnd;

	m_MathRichEditCtrl.GetSel( nStart, nEnd );

	m_MathRichEditCtrl.ReplaceSel( tszValue );
	
	//Update the text Count
	m_numChars += lstrlen(tszValue);
}

/////////////////////////////////////////////////////////////////////////////
void SVFormulaEditorPageClass::backspaceRichEdit( long Pos )
{
	int nStart, nEnd;

	m_MathRichEditCtrl.GetSel( nStart, nEnd );

	m_MathRichEditCtrl.SetSel( nEnd - Pos, nEnd - Pos );
}

/////////////////////////////////////////////////////////////////////////////
void SVFormulaEditorPageClass::advanceRichEdit( long Pos )
{
	int nStart, nEnd;

	m_MathRichEditCtrl.GetSel( nStart, nEnd );

	m_MathRichEditCtrl.SetSel( nEnd + Pos, nEnd + Pos );
}

/////////////////////////////////////////////////////////////////////////////
void SVFormulaEditorPageClass::deleteRichEdit( long Pos )
{
	int nStart, nEnd;

	m_MathRichEditCtrl.GetSel(nStart, nEnd);

	// Delete Selection
	if (nStart != nEnd)
	{
		m_MathRichEditCtrl.ReplaceSel("");
		m_numChars -= (nEnd - nStart);
	}
	else
	{
		if (nEnd == m_numChars) // Backspace
		{
			m_MathRichEditCtrl.SetSel(nEnd-1,nEnd);
		}
		else // Delete a character
		{
			m_MathRichEditCtrl.SetSel(nEnd,nEnd+1);
		}
		m_MathRichEditCtrl.ReplaceSel("");
		m_numChars--;
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL SVFormulaEditorPageClass::GetEquationText()
{
	BOOL l_bOK = TRUE;
	ASSERT( m_pEquationStruct );
	if( m_pEquationStruct )
	{
		// Get text from the RichEdit and place into EquationhStruct
		CString equationText;
		m_MathRichEditCtrl.GetWindowText( equationText );
		m_pEquationStruct->SetEquationText( equationText );

		if( m_pEquation->GetTool() != NULL )
		{
			DWORD l_dwRet = ::SVSendMessage( m_pEquation->GetTool(),SVM_RESET_ALL_OBJECTS & ~SVM_NOTIFY_FRIENDS , NULL, NULL); // Do not reset friends because they may be invalid.
			if( l_dwRet != SVMR_SUCCESS)
			{
				l_bOK = FALSE;
			}
		}
	}
	return l_bOK;
}

/////////////////////////////////////////////////////////////////////////////
void SVFormulaEditorPageClass::setEquationText()
{
	ASSERT( m_pEquationStruct );
	if( m_pEquationStruct )
	{
		// Get text from EquationStruct and place into RichEdit
		CString equationText;
		m_pEquationStruct->GetEquationText( equationText );
		m_MathRichEditCtrl.SetWindowText( equationText );
		m_numChars = lstrlen(equationText);
	}
}

/////////////////////////////////////////////////////////////////////////////
// SVFormulaEditorPageClass message handlers

void SVFormulaEditorPageClass::OnDropdownPpqVariableCombo() 
{
	// This does not work cause the combobox list does not have a horizontal scrollbar
	//PPQVariableComboCtrl.SetHorizontalExtent( ppqComboExtent );

	// Instead - set the width of the drop down list to 
	// the size of tyhe widest string (in pixels)
	int width = m_PPQVariableComboCtrl.GetDroppedWidth();
	if( width < m_ppqComboExtent )
		m_PPQVariableComboCtrl.SetDroppedWidth( m_ppqComboExtent );
}

void SVFormulaEditorPageClass::OnAddPPQVariableButton() 
{
	CString text;

	// Get Pointer to SVOutObjectInfo
	int curSel = m_PPQVariableComboCtrl.GetCurSel();
	if( curSel == CB_ERR )
	{
		AfxMessageBox("The item you are trying to add is not a valid selection\nPlease select an item from the list");
		return;
	}

	// Get Current Selection in combobox
	m_PPQVariableComboCtrl.GetWindowText( text );
	if( !text.IsEmpty() )
	{
		// Variables are delimited by qouble qoutes
		CString variable = _T( "\"" ) + text + _T( "\"" );
		insertIntoRichEdit( ( LPCTSTR )variable );
	}
}

void SVFormulaEditorPageClass::OnLocalVariableSelect() 
{
	UpdateData();

	SVInspectionProcess* pInspection = m_pTool->GetInspection();

	// use inspection to look up instead of TheSVObjectManager because
	// name does not have Inspection_1 prefix.
	SVValueObjectReference ref;
	SVObjectManagerClass::Instance().GetObjectByDottedName( static_cast< LPCTSTR >( m_strToolsetOutputVariable ), ref );

	m_pTool->UpdateTaskObjectOutputListAttributes( ref, SV_SELECTABLE_FOR_EQUATION );

	SVToolsetOutputSelectionDialog dlg;
	
	dlg.PTaskObjectList = m_pToolSet;
	dlg.uAttributesDesired = SV_SELECTABLE_FOR_EQUATION;
	dlg.m_treeOutputList.SetAllowWholeArray();

	int nResult = dlg.DoModal();
	if( nResult == IDOK )
	{
		// Get Toolset Output Name
		// Update ToolsetOutputVariable
		m_strToolsetOutputVariable = dlg.m_sSelectedOutputName;
		UpdateData( FALSE );
	}
}

void SVFormulaEditorPageClass::OnAddLocalVariableButton() 
{
	UpdateData( TRUE );

	const CString QUOTE = _T("\"");

	// Get Current Variable name
	if( !m_strToolsetOutputVariable.IsEmpty() )
	{
		if ( m_strToolsetOutputVariable.Right(1) == _T("]") )	// array
		{
			int iArrayIndexPos = m_strToolsetOutputVariable.ReverseFind( _T('[') );
			CString sRootName = m_strToolsetOutputVariable.Left( iArrayIndexPos );
			CString sIndex = m_strToolsetOutputVariable.Mid( iArrayIndexPos );
			sIndex = sIndex.Mid( 1, sIndex.GetLength() - 2 );	// strip off [ ]
			//CString sName = QUOTE + sRootName + QUOTE + _T("[ ") + sIndex + _T(" {-1} ]");
			CString sName = QUOTE + sRootName + QUOTE + _T("[ ") + sIndex + _T(" ]");
			insertIntoRichEdit( sName );
		}
		else
		{
			// Variables are delimited by double qoutes
			CString sName = QUOTE + m_strToolsetOutputVariable + QUOTE;
			insertIntoRichEdit( sName );
		}
	}
}

void SVFormulaEditorPageClass::OnAddConstantButton() 
{
	UpdateData( TRUE );

	if ( ! m_StrConstantValue.IsEmpty () )
	{
		CString TempString = m_StrConstantValue;
			
		switch ( m_constantType )
		{
			case SV_FORMULA_BINARY_CONSTANT_TYPE:
				TempString += "b";
				break;

			case SV_FORMULA_HEXADECIMAL_CONSTANT_TYPE:
				TempString += "h";
				break;
		}
		insertIntoRichEdit( ( LPCTSTR )TempString );
	}
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
BOOL SVFormulaEditorPageClass::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// IDs must be sequential within a toolbar!
	switch (LOWORD(wParam))
	{
// Condition operators
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
		
// Logical operators
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

// Arithmetic Operators
			case ID_PLUSBUTTON:
			insertIntoRichEdit(" + ");
			break;
		
		case ID_MINUSBUTTON:
			insertIntoRichEdit(" - ");
			break;
		
		case ID_TIMESBUTTON:
			insertIntoRichEdit(" * ");
			break;
		
		case ID_DIVIDEBUTTON:
			insertIntoRichEdit(" / ");
			break;

// Trig operators
		case ID_SINBUTTON:
			insertIntoRichEdit(" SIN() ");
			backspaceRichEdit(2);
			break;

		case ID_COSBUTTON:
			insertIntoRichEdit(" COS() ");
			backspaceRichEdit(2);
			break;

		case ID_TANBUTTON:
			insertIntoRichEdit(" TAN() ");
			backspaceRichEdit(2);
			break;

		case ID_ARCSINBUTTON:
			insertIntoRichEdit(" ASIN() ");
			backspaceRichEdit(2);
			break;

		case ID_ARCCOSBUTTON:
			insertIntoRichEdit(" ACOS() ");
			backspaceRichEdit(2);
			break;

		case ID_ARCTANBUTTON:
			insertIntoRichEdit(" ATAN() ");
			backspaceRichEdit(2);
			break;

// Misc Operators
		case ID_MODBUTTON:
			insertIntoRichEdit(" MOD(,) ");
			backspaceRichEdit(3);
			break;

		case ID_ABSBUTTON:
			insertIntoRichEdit(" ABS() ");
			backspaceRichEdit(2);
			break;

		case ID_SQRBUTTON:
			insertIntoRichEdit(" SQR() ");
			backspaceRichEdit(2);
			break;

		case ID_SQRTBUTTON:
			insertIntoRichEdit(" SQRT() ");
			backspaceRichEdit(2);
			break;

		case ID_TRUNCBUTTON:
			insertIntoRichEdit(" TRUNC() ");
			backspaceRichEdit(2);
			break;

// Statistics Operators
		case ID_MINBUTTON:
			insertIntoRichEdit(" MIN(,) ");
			backspaceRichEdit(3);
			break;

		case ID_MAXBUTTON:
			insertIntoRichEdit(" MAX(,) ");
			backspaceRichEdit(3);
			break;

		case ID_AVERAGEBUTTON:
			insertIntoRichEdit(" AVG(,) ");
			backspaceRichEdit(3);
			break;

		case ID_MEDIANBUTTON:
			insertIntoRichEdit(" MED(,) ");
			backspaceRichEdit(3);
			break;

		case ID_SUM_BUTTON:
			insertIntoRichEdit(" SUM(,) ");
			backspaceRichEdit(3);
			break;

		case ID_STD_DEV_BUTTON:
			insertIntoRichEdit(" STDDEV(,) ");
			backspaceRichEdit(3);
			break;

// Punctuation Bar
		case ID_FORMULA_LPARENBUTTON:
			insertIntoRichEdit("(");
			break;

		case ID_FORMULA_RPARENBUTTON:
			insertIntoRichEdit(")");
			break;

		case ID_FORMULA_COMMABUTTON:
			insertIntoRichEdit(",");
			break;

// Cursor Bar
		case ID_FORMULA_LARROWBUTTON:
			backspaceRichEdit(1);
			break;

		case ID_FORMULA_RARROWBUTTON:
			advanceRichEdit(1);
			break;

		case ID_FORMULA_DELETEBUTTON:
			deleteRichEdit(1);
			break;
		
		case ID_FORMULA_TEST:
			onValidate();
			break;

		case ID_FORMULA_ARRAY_BUTTON:
			insertIntoRichEdit(" [ ] ");
			break;

		case ID_FORMULA_ARRAY_INDEX_BUTTON:
			insertIntoRichEdit(" [ 1 { -1 } ] ");
			backspaceRichEdit(10);
			break;

}// end switch (LOWORD(wParam))

	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL SVFormulaEditorPageClass::ValidateEquation() 
{
	BOOL l_bValid = TRUE;
	if( GetSafeHwnd() )
	{
		m_MathRichEditCtrl.GetSel(m_pEquationStruct->StartPos, m_pEquationStruct->EndPos);
		l_bValid = GetEquationText();

		UpdateData( TRUE ); // Update the variables
	
		SVEquationTestResult result = m_pEquation->Test();
		return result.bPassed && l_bValid;
	}
	return l_bValid;
}

void SVFormulaEditorPageClass::onValidate() 
{
	m_MathRichEditCtrl.GetSel(m_pEquationStruct->StartPos, m_pEquationStruct->EndPos);
	GetEquationText();

	UpdateData( TRUE ); // Update the variables
	
	SVEquationTestResult result = m_pEquation->Test();
	if( result.bPassed )
	{
		CString tmp;
		double value = m_pEquation->GetYACCResult();

		tmp.Format("Formula Validated Successfully\n Value = %lf", value); 

		AfxMessageBox(tmp);
	}
	else // Something is wrong
	{
		HandleValidateError(result);
	}
}


int SVFormulaEditorPageClass::GetComboBoxStringExtent( CComboBox& rComboBox, LPCTSTR szStr ) 
{ 
	CDC* pDC; 
	pDC = rComboBox.GetDC(); 

	CSize size = pDC->GetOutputTextExtent( szStr ); 
	
	return (int)(size.cx * 0.80); 
} 

void SVFormulaEditorPageClass::HandleValidateError( SVEquationTestResult result )
{
	m_MathRichEditCtrl.SetSel( result.iPositionFailed, result.iPositionFailed );
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVFormulaEditorPage.cpp_v  $
 * 
 *    Rev 1.2   13 May 2013 16:29:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 015.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.24   08 May 2013 10:18:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  829
 * SCR Title:  Fix tool adjust dialogs to validate all math equations before exiting.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modify validation to only validate the equation for this formula - do not validate friends.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 May 2013 13:26:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 013.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.23   30 Apr 2013 11:27:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  829
 * SCR Title:  Fix tool adjust dialogs to validate all math equations before exiting.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified GetEquationText to return TRUE if the equation validates correctly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:39:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.22   02 Jul 2012 16:54:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.21   18 Apr 2011 09:29:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to move the shared data for the IO Entry data to a different class to fix issues with PPQ and Inspection shared data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   08 Dec 2010 08:05:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   16 Dec 2009 10:24:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   05 Feb 2009 16:17:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  649
 * SCR Title:  Update Equation Dialog to disable the PPQ Variable Edit/Selection Field
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to disable Combobox and add button for PPQ variables if there are no PPQ variables to select.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   15 Sep 2005 07:37:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added code to for the owning tool of the equation to reset all of its object to make sure sub-tool parts get updated properly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   01 Sep 2005 14:48:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed bug in SetTaskObject that caused pEquation to be null.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   31 Aug 2005 09:22:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed bugs with LUT Dlg Continous Reload and Formula
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   10 Aug 2005 15:58:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   set object picker option to allow whole array selection
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   10 Aug 2005 13:26:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed GUI nits
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   14 Jul 2005 12:37:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   corrected handling of array value object names
 * cleaned up formatting
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   22 Jun 2005 08:55:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented SUM, STDDEV, array value object
 * cleaned up code
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   17 Feb 2005 14:08:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   12 Jul 2004 12:30:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  429
 * SCR Title:  Fix Select Object tree control
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed SVToolsetOutputSelectionDialog member variable name
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   22 Apr 2003 09:45:30   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   29 Jan 2003 15:38:16   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update include file names.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   19 Nov 2002 13:55:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   relates to how the equation classes connect and display information for Input/Output from the PPQ
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   09 Nov 2001 13:23:32   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  255
 * SCR Title:  Add Sub-pixel resolution to Profile and Line Gage Tool Analyzers
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed calls to SVToolbarClass::Create in functions createCursorToolbar & createOperatorToolbars
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   31 Oct 2001 16:20:04   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  252
 * SCR Title:  System hang on .5 dual when accessing Math Tool after going online
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Removed rich edit control calls in the constructor and OnInitDialog. Changed local parameters for GetSel/SetSel from long to int.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   04 Oct 2001 14:53:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  229
 * SCR Title:  Upgrade SVObserver to Version 3.34 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modified source code to remove compiler warning message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   01 Mar 2000 17:17:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  106
 * SCR Title:  Math Tool - List processing
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added new toolbar for punctuaution.
 * Revised createCursorToolbar for revised cursor toolbar and
 * new punctuation toolbar.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   29 Feb 2000 14:32:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  106
 * SCR Title:  Math Tool - List processing
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised size of math toolbars.
 * Revised selection of toolset output from a combobox to
 * a tree control selection.
 * Added statistics toolbar and handling.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 11:39:18   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   08 Nov 1999 09:50:50   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to show result of validating the equation.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   21 Oct 1999 16:04:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised GetComboBoxStringExtent to return 80% of the 
 * string text extent size.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   19 Oct 1999 17:39:50   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  48
 * SCR Title:  Threshold Page Formula
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to use formulas for setting thresholds
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   05 Oct 1999 14:26:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Corrected Syntax for ArcSine,ArcCoSine and ArcTangent
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   Sep 16 1999 13:19:46   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changed Discarding of PQData...
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   14 Sep 1999 17:04:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added validation check for equations in OnOk method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   Sep 14 1999 15:27:12   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Introduced Math Container.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Sep 1999 20:04:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin. new class for editing math equations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
