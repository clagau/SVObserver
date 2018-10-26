//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFormulaEditorPageClass
//* .File Name       : $Workfile:   SVFormulaEditorPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.13  $
//* .Check In Date   : $Date:   07 Jan 2015 18:09:34  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "Definitions/ObjectDefines.h"
#include "Definitions/GlobalConst.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "SVFormulaEditorPage.h"
#include "SVOResource/ConstGlobalSvOr.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVUtilityLibrary/LoadDll.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "Scintilla.h"
#include "SciLexer.h"
#include "TextDefinesSvOg.h"
#include "Definitions/TextDefineSVDef.h"
#include "TextDefinesSvOg.h"
#include "SVStatusLibrary/GlobalPath.h"
#pragma endregion Includes

#pragma region Declarations

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	// Constant types
	enum
	{
		SV_FORMULA_DECIMAL_CONSTANT_TYPE = 0,
		SV_FORMULA_HEXADECIMAL_CONSTANT_TYPE = 1,
		SV_FORMULA_BINARY_CONSTANT_TYPE = 2,

		SV_EQUATION_BUFFER_SIZE = 4096, //This enumeration defines the Equation Buffer Size.
	};

	const int c_offsetXForCursorToolbar = 340;
	const TCHAR* const cQuote = _T("\"");

	BEGIN_MESSAGE_MAP(SVFormulaEditorPageClass, CPropertyPage)
		//{{AFX_MSG_MAP(SVFormulaEditorPageClass)
		ON_BN_CLICKED(IDC_ADD_LOCAL_VARIABLE_BUTTON, OnAddLocalVariableButton)
		ON_BN_CLICKED(IDC_ADD_CONSTANT_BUTTON, OnAddConstantButton)
		ON_BN_CLICKED(IDC_LOCAL_VARIABLE_SELECT, OnLocalVariableSelect)
		ON_BN_CLICKED(IDC_DISABLE_EQUATION, OnDisable)
		ON_BN_CLICKED(IDC_DISABLE_TOOL, OnDisable)
		ON_NOTIFY(SCN_UPDATEUI, IDC_MATHCOND_EDITOR, OnEquationFieldChanged)
		ON_WM_HELPINFO()
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	SVFormulaEditorPageClass::SVFormulaEditorPageClass( SvOi::IFormulaControllerPtr controller, bool isDisableCheckboxesVisible, UINT captionID, UINT disableExtentionID )
	: CPropertyPage( SVFormulaEditorPageClass::IDD, captionID )
	, m_FormulaController( controller )
	, m_isConditionalPage( isDisableCheckboxesVisible )
	, m_disableExtentionID( disableExtentionID )
	, m_ConstantValue( _T( "" ) )
	, m_constantType( 0 )
	, m_ToolsetOutputVariable( _T( "" ) )
	, m_equationDisabled( false )
	, m_ownerDisabled( false )
	{
	}

	SVFormulaEditorPageClass::~SVFormulaEditorPageClass()
	{
		m_FormulaController.reset();
	}

	void SVFormulaEditorPageClass::SetDefaultInputs()
	{
		m_FormulaController->SetDefaultInputs();
	}

	std::string SVFormulaEditorPageClass::GetOwnerName() const
	{
		return m_FormulaController->GetOwnerName();
	}

	void SVFormulaEditorPageClass::DoDataExchange(CDataExchange* pDX)
	{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVFormulaEditorPageClass)
	DDX_Control(pDX, IDC_LOCAL_VARIABLE_SELECT, m_ToolsetOutputSelectButton);
	DDX_Control(pDX, IDC_CONSTANT_EDIT, m_ConstantEditCtrl);
	DDX_Control(pDX, IDC_ADD_LOCAL_VARIABLE_BUTTON, m_AddLocalVariableCtrl);
	DDX_Control(pDX, IDC_ADD_CONSTANT_BUTTON, m_AddConstantCtrl);
	DDX_Control(pDX, IDC_DECIMAL, m_decimalRadioButton);
	DDX_Control(pDX, IDC_HEXADECIMAL, m_hexadecimalRadioButton);
	DDX_Control(pDX, IDC_BINARY, m_binaryRadioButton);
	DDX_Control(pDX, IDC_DISABLE_EQUATION, m_DisableEquationCtrl);
	DDX_Control(pDX, IDC_DISABLE_TOOL, m_DisableToolCtrl);
	DDX_Text(pDX, IDC_CONSTANT_EDIT, m_ConstantValue);
	DDX_Radio(pDX, IDC_DECIMAL, m_constantType);
	DDX_Text(pDX, IDC_LOCAL_VARIABLE_EDIT_CTRL, m_ToolsetOutputVariable);
	DDX_Check(pDX, IDC_DISABLE_EQUATION, m_equationDisabled);
	DDX_Check(pDX, IDC_DISABLE_TOOL, m_ownerDisabled);
	//}}AFX_DATA_MAP
	}

	BOOL SVFormulaEditorPageClass::OnInitDialog()
	{
		CPropertyPage::OnInitDialog();

		HINSTANCE ScintillaInstance( nullptr );

		
		//Load Scintilla dll explicitly
		HRESULT hOK = SvUl::LoadDll::Instance().getDll(SvUl::ScintillaDll, ScintillaInstance);
		if (S_OK != hOK || nullptr == ScintillaInstance)
		{
			SvStl::MessageMgrStd Exception(SvStl::MsgType::Log | SvStl::MsgType::Display );
			Exception.setMessage( SVMSG_SVO_88_LOADING_SCINTILLA_DLL_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10028_LoadOfScintillaDllFailed );
		}
		else
		{
			createScintillaControl();
		}

		// Put the Down Arrow on the Button
		m_downArrowBitmap.LoadOEMBitmap( OBM_DNARROW );
		m_ToolsetOutputSelectButton.SetBitmap( ( HBITMAP )m_downArrowBitmap );

		createToolbars();

		std::string DisableText = SvUl::LoadStdString( IDS_DISABLE_STRING ) + _T(" ") + SvUl::LoadStdString( m_disableExtentionID );
		GetDlgItem(IDC_DISABLE_TOOL)->SetWindowText( DisableText.c_str() );

		setEquationText();

		if (!m_isConditionalPage)
		{
			m_DisableEquationCtrl.ShowWindow(SW_HIDE);
			m_DisableToolCtrl.ShowWindow(SW_HIDE);
		}
		else
		{
			m_DisableEquationCtrl.ShowWindow(SW_SHOW);
			m_DisableToolCtrl.ShowWindow(SW_SHOW);
		}

		enableControls();
		UpdateData( FALSE );

		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}

	void SVFormulaEditorPageClass::OnHelp()
	{
		if (m_isConditionalPage)
		{
			AfxGetApp()->HtmlHelp( IDD_CONDITIONAL_PAGE + SvOr::HELPFILE_SVORESOURCE_OFFSET );
		}
		else
		{
			AfxGetApp()->HtmlHelp( IDD_FORMULA_DIALOG + SvOr::HELPFILE_SVORESOURCE_OFFSET );
		}
	}

	BOOL SVFormulaEditorPageClass::OnHelpInfo(HELPINFO* pHelpInfo)
	{
		if (m_isConditionalPage)
		{
			pHelpInfo->iCtrlId = IDD_CONDITIONAL_PAGE + SvOr::HELPFILE_SVORESOURCE_OFFSET;
		}
		else
		{
			pHelpInfo->iCtrlId = IDD_FORMULA_DIALOG + SvOr::HELPFILE_SVORESOURCE_OFFSET;
		}

		AfxGetApp()->HtmlHelp( pHelpInfo->iCtrlId, HH_HELP_CONTEXT );
		return TRUE ;
	}

	/////////////////////////////////////////////////////////////////////////////
	//The function length breaks the programming guidelines, but it is not split because it would reduce the readability.
	bool SVFormulaEditorPageClass::createToolbars()
	{
		CRect	rect;
		CRect	trect;
		CSize	size( 28, 28 );
		int numButtons;
		bool retVal = true;

		// Get base rectangle
		GetDlgItem( IDC_TOOLBAR_STATIC )->GetWindowRect( &rect );
		ScreenToClient( &rect );

		// Logical Tool Bar...
		numButtons = 4;
		trect.left   = rect.left;
		trect.top    = rect.top + SV_TOOLBAR_BORDER_H;
		trect.right  = trect.left + ( ( size.cx + ( SV_TOOLBAR_BORDER_W * 2 ) ) * numButtons );
		trect.bottom = trect.top + size.cy + ( SV_TOOLBAR_BORDER_H * 2 );
		retVal = m_logicalOperatorBar.Create( WS_CHILD | WS_VISIBLE | CCS_NODIVIDER | CBRS_SIZE_DYNAMIC,
			trect, this, IDR_MATH_LOGICAL_TOOLBAR, ID_ANDBUTTON, numButtons, size ) && retVal;

		// Basic Arithmetic Tool Bar
		numButtons = 4;
		trect.left	 = trect.right;
		trect.right  = trect.left + ( ( size.cx + ( SV_TOOLBAR_BORDER_W * 2 ) ) * numButtons );
		trect.bottom = trect.top + size.cy + ( SV_TOOLBAR_BORDER_H * 2 );
		retVal = m_basicOperatorBar.Create( WS_CHILD | WS_VISIBLE | CCS_NODIVIDER | CBRS_SIZE_DYNAMIC,
			trect, this, IDR_MATH_BASIC_TOOLBAR, ID_PLUSBUTTON, numButtons, size) && retVal;

		// Trigonometric Operators
		numButtons = 6;
		trect.left = rect.left;
		trect.top    = trect.bottom + SV_TOOLBAR_BORDER_H;
		trect.right = trect.left + ( ( size.cx + ( SV_TOOLBAR_BORDER_W * 2 ) ) * numButtons );
		trect.bottom = trect.top + size.cy + ( SV_TOOLBAR_BORDER_H * 2 );
		retVal = m_trigOperatorBar.Create( WS_CHILD | WS_VISIBLE | CCS_NODIVIDER | CBRS_SIZE_DYNAMIC,
			trect, this, IDR_MATH_TRIGONOMETRIC_TOOLBAR, ID_SINBUTTON, numButtons, size ) && retVal;

		// Statistics Toolbar
		numButtons = 6;
		trect.left   = rect.left;	
		trect.top    = trect.bottom + SV_TOOLBAR_BORDER_H;
		trect.right  = trect.left + ( ( size.cx + ( SV_TOOLBAR_BORDER_W * 2 ) ) * numButtons );
		trect.bottom = trect.top + size.cy + ( SV_TOOLBAR_BORDER_H * 2 );
		retVal = m_statisticsOperatorBar.Create( WS_CHILD | WS_VISIBLE | CCS_NODIVIDER | CBRS_SIZE_DYNAMIC,
			trect, this, IDR_MATH_STATISTICS_TOOLBAR, ID_MINBUTTON, numButtons, size ) && retVal;

		// Miscellaneous operators
		numButtons = 6;
		trect.left = rect.left;
		trect.top = trect.bottom + SV_TOOLBAR_BORDER_H;
		trect.right = trect.left + ( ( size.cx + ( SV_TOOLBAR_BORDER_W * 2 ) ) * numButtons );
		trect.bottom = trect.top + size.cy + ( SV_TOOLBAR_BORDER_H * 2 );
		retVal = m_miscOperatorBar.Create( WS_CHILD | WS_VISIBLE | CCS_NODIVIDER | CBRS_SIZE_DYNAMIC,
			trect, this, IDR_MATH_MISC_TOOLBAR, ID_MODBUTTON, numButtons, size ) && retVal;

		// Conditional Tool Bar...
		numButtons = 6;
		trect.left   = rect.left;
		trect.top    = trect.bottom + SV_TOOLBAR_BORDER_H;
		trect.right  = trect.left + ( ( size.cx + ( SV_TOOLBAR_BORDER_W * 2 ) ) * numButtons );
		trect.bottom = trect.top + size.cy + ( SV_TOOLBAR_BORDER_H * 2 );
		retVal = m_conditionalOperatorBar.Create( WS_CHILD | WS_VISIBLE | CCS_NODIVIDER | CBRS_SIZE_DYNAMIC,
			trect, this, IDR_MATH_CONDITIONAL_TOOLBAR, ID_LESSBUTTON, numButtons,
			size ) && retVal;

		// Punctuation toolbar
		numButtons = 3;
		trect.left	 = trect.right;
		trect.right  = trect.left + ( ( size.cx + ( SV_TOOLBAR_BORDER_W * 2 ) ) * numButtons );
		trect.bottom = trect.top + size.cy + ( SV_TOOLBAR_BORDER_H * 2 );
		retVal = ( TRUE == m_punctuationBar.Create(	WS_CHILD | WS_VISIBLE | CCS_NODIVIDER | CBRS_SIZE_DYNAMIC,
			trect, this, IDR_MATH_PUNCTUATION_TOOLBAR, ID_FORMULA_LPARENBUTTON, numButtons, size ));

		// Common Cursor movements toolbar
		size = CSize( 37, 28 );
		numButtons = 3;
		trect.left   = rect.left + c_offsetXForCursorToolbar;
		trect.right  = trect.left + ( ( size.cx + ( SV_TOOLBAR_BORDER_W * 2 ) ) * numButtons );	
		retVal = m_cursorBar.Create( WS_CHILD | WS_VISIBLE | CCS_NODIVIDER | CBRS_SIZE_DYNAMIC,
			trect, this, IDR_FORMULA_CURSOR_TOOLBAR, ID_FORMULA_DELETEBUTTON, numButtons, size ) && retVal;

		// Validate toolbar
		size = CSize( 50, 28 );
		numButtons = 2;
		trect.right  = rect.right - 1;
		trect.left   = trect.right - ( ( size.cx + ( SV_TOOLBAR_BORDER_W * 2 ) ) * numButtons );
		retVal = m_validateBar.Create( WS_CHILD | WS_VISIBLE | CCS_NODIVIDER | CBRS_SIZE_DYNAMIC,
			trect, this, IDR_FORMULA_VALIDATE_TOOLBAR, ID_FORMULA_UNDO, numButtons, size ) && retVal;

		return retVal;
	}

	void SVFormulaEditorPageClass::insertIntoEditor( LPCTSTR tszValue )
	{
		if( nullptr != m_EditWnd.GetSafeHwnd() )
		{
			m_EditWnd.SendMessage( SCI_REPLACESEL, 0, reinterpret_cast<LPARAM>( tszValue ) );
		}

		enableUndoButton();
	}

	void SVFormulaEditorPageClass::backspaceInEditor( long Pos )
	{
		if( nullptr != m_EditWnd.GetSafeHwnd() )
		{
			int nStart =  static_cast<int>(m_EditWnd.SendMessage(SCI_GETCURRENTPOS));
			m_EditWnd.SendMessage(SCI_GOTOPOS, nStart - Pos);
		}

		enableUndoButton();
	}

	void SVFormulaEditorPageClass::advanceInEditor( long Pos )
	{
		if( nullptr != m_EditWnd.GetSafeHwnd() )
		{
			int nStart =  static_cast<int>(m_EditWnd.SendMessage(SCI_GETCURRENTPOS));
			m_EditWnd.SendMessage(SCI_GOTOPOS, nStart + Pos);
		}

		enableUndoButton();
	}

	void SVFormulaEditorPageClass::deleteInEditor( long Pos )
	{
		if( nullptr != m_EditWnd.GetSafeHwnd() )
		{
			int nStart =  static_cast<int>(m_EditWnd.SendMessage(SCI_GETSELECTIONSTART));
			int nEnd =  static_cast<int>(m_EditWnd.SendMessage(SCI_GETSELECTIONEND));

			if (nStart == nEnd)
			{  //add an one character selection
				int length =  static_cast<int>(m_EditWnd.SendMessage(SCI_GETLENGTH));
				if (nEnd == length) // Backspace
				{
					m_EditWnd.SendMessage(SCI_SETSEL, nEnd-1, nEnd);
				}
				else // Delete a character
				{
					m_EditWnd.SendMessage(SCI_SETSEL, nEnd, nEnd+1);
				}
			}

			// Delete Selection
			m_EditWnd.SendMessage( SCI_REPLACESEL, 0, reinterpret_cast<LPARAM>( _T("") ) );
		}

		enableUndoButton();
	}

	std::string SVFormulaEditorPageClass::getEquationText() const
	{
		std::vector<TCHAR> equationText;
		equationText.resize(SV_EQUATION_BUFFER_SIZE, 0);
		if( nullptr != m_EditWnd.GetSafeHwnd() )
		{
			m_EditWnd.SendMessage( SCI_GETTEXT, SV_EQUATION_BUFFER_SIZE, reinterpret_cast<LPARAM>( &equationText[0] ) );
		}
		return std::string(&equationText[0]);
	}

	void SVFormulaEditorPageClass::setEquationText()
	{
		// Get text from EquationStruct and place into Editor
		std::string equationText = m_FormulaController->GetEquationText();
		if( nullptr != m_EditWnd.GetSafeHwnd() )
		{
			m_EditWnd.SendMessage( SCI_SETTEXT, 0, reinterpret_cast<LPARAM>( equationText.c_str() ) );
		}

		bool ownerEnabled = false; 
		bool equationEnabled = false; 
		HRESULT result = m_FormulaController->IsOwnerAndEquationEnabled(ownerEnabled, equationEnabled);
		if (S_OK == result )
		{
			m_ownerDisabled = ownerEnabled ? false: true;
			m_equationDisabled = equationEnabled ? false: true;
		}
		enableUndoButton();
	}

	/////////////////////////////////////////////////////////////////////////////
	// SVFormulaEditorPageClass message handlers

	void SVFormulaEditorPageClass::OnLocalVariableSelect()
	{
		UpdateData( TRUE );

		std::string InspectionName = m_FormulaController->GetInspectionName();
	
		std::string Filter = SvUl::LoadStdString( IDS_CLASSNAME_ROOTOBJECT );

		SvOsl::ObjectTreeGenerator::Instance().setSelectorType( SvOsl::ObjectTreeGenerator::TypeSingleObject );
		SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterInput, InspectionName, std::string( _T("") ) );
		SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterInput, Filter, std::string( _T("") ) );
		SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterOutput, SvDef::FqnPPQVariables, std::string( _T("") ) );

		// Insert the Names of the objects selecteable for an Equation
		m_FormulaController->BuildSelectableItems();

		SvDef::StringSet Items;
		Items.insert( std::string(m_ToolsetOutputVariable) );
		SvOsl::ObjectTreeGenerator::Instance().setCheckItems( Items );

		std::string ToolsetOutput = SvUl::LoadStdString( IDS_SELECT_TOOLSET_OUTPUT );
		Filter = SvUl::LoadStdString( IDS_FILTER );
		std::string Title = SvUl::Format( _T("%s - %s"), ToolsetOutput.c_str(), InspectionName.c_str() );

		INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog( Title.c_str(), ToolsetOutput.c_str(), Filter.c_str(), this );

		if( IDOK == Result )
		{
			m_ToolsetOutputVariable = SvOsl::ObjectTreeGenerator::Instance().getSingleObjectResult().m_DisplayLocation.c_str();
			UpdateData( false );
		}
	}

	void SVFormulaEditorPageClass::OnAddLocalVariableButton() 
	{
		UpdateData( true );


		// Get Current Variable name
		if( !m_ToolsetOutputVariable.IsEmpty() )
		{
			if ( m_ToolsetOutputVariable.Right(1) == _T("]") )	// array
			{
				int iArrayIndexPos = m_ToolsetOutputVariable.ReverseFind( _T('[') );
				std::string RootName = m_ToolsetOutputVariable.Left( iArrayIndexPos );
				std::string Index = m_ToolsetOutputVariable.Mid( iArrayIndexPos );
				Index = Index.substr( 1, Index.size() - 2 );	// strip off [ ]
				std::string Name = cQuote + RootName + cQuote + _T("[ ") + Index + _T(" ]");
				insertIntoEditor( Name.c_str() );
			}
			else
			{
				// Variables are delimited by double qoutes
				std::string Name = cQuote + m_ToolsetOutputVariable + cQuote;
				insertIntoEditor( Name.c_str() );
			}
		}
	}

	void SVFormulaEditorPageClass::OnAddConstantButton() 
	{
		UpdateData( TRUE );

		if ( ! m_ConstantValue.IsEmpty () )
		{
			std::string TempString = m_ConstantValue;

			switch ( m_constantType )
			{
			case SV_FORMULA_BINARY_CONSTANT_TYPE:
				TempString += _T("b");
				break;

			case SV_FORMULA_HEXADECIMAL_CONSTANT_TYPE:
				TempString += _T("h");
				break;

			default:
				//Do nothing.
				break;
			}
			insertIntoEditor( TempString.c_str() );
		}
	}

	void SVFormulaEditorPageClass::OnDisable()
	{
		if (m_isConditionalPage)
		{
			UpdateData( TRUE );

			bool enableOwner = (m_ownerDisabled ? false : true);
			bool enableEquation = (m_equationDisabled ? false : true);
			m_FormulaController->SetOwnerAndEquationEnabled(enableOwner, enableEquation);
			enableControls();
		}
	}

	//The function length breaks the programming guidelines, but it is only one switch statement.
	//To change this would reduce the understanding of the functionality.
	BOOL SVFormulaEditorPageClass::OnCommand(WPARAM wParam, LPARAM lParam) 
	{
		// IDs must be sequential within a toolbar!
		switch (LOWORD(wParam))
		{
			// Condition operators
		case ID_LESSBUTTON:
			insertIntoEditor(" LT ");
			break;
		case ID_GREATERBUTTON:
			insertIntoEditor(" GT ");
			break;
		case ID_LESSEQUALBUTTON:
			insertIntoEditor(" LTE ");
			break;
		case ID_GREATEREQUALBUTTON:
			insertIntoEditor(" GTE ");
			break;
		case ID_EQUALBUTTON:
			insertIntoEditor(" EQ ");
			break;

			// Logical operators
		case ID_NOTEQUALBUTTON:
			insertIntoEditor(" NE ");
			break;
		case ID_ANDBUTTON:
			insertIntoEditor(" AND ");
			break;
		case ID_ORBUTTON:
			insertIntoEditor(" OR ");
			break;
		case ID_NOTBUTTON:
			insertIntoEditor(" NOT ");
			break;
		case ID_XORBUTTON:
			insertIntoEditor(" XOR ");
			break;

			// Arithmetic Operators
		case ID_PLUSBUTTON:
			insertIntoEditor(" + ");
			break;
		case ID_MINUSBUTTON:
			insertIntoEditor(" - ");
			break;
		case ID_TIMESBUTTON:
			insertIntoEditor(" * ");
			break;
		case ID_DIVIDEBUTTON:
			insertIntoEditor(" / ");
			break;

			// Trig operators
		case ID_SINBUTTON:
			insertIntoEditor(" SIN() ");
			backspaceInEditor(2);
			break;
		case ID_COSBUTTON:
			insertIntoEditor(" COS() ");
			backspaceInEditor(2);
			break;
		case ID_TANBUTTON:
			insertIntoEditor(" TAN() ");
			backspaceInEditor(2);
			break;
		case ID_ARCSINBUTTON:
			insertIntoEditor(" ASIN() ");
			backspaceInEditor(2);
			break;
		case ID_ARCCOSBUTTON:
			insertIntoEditor(" ACOS() ");
			backspaceInEditor(2);
			break;
		case ID_ARCTANBUTTON:
			insertIntoEditor(" ATAN() ");
			backspaceInEditor(2);
			break;

			// Misc Operators
		case ID_MODBUTTON:
			insertIntoEditor(" MOD(,) ");
			backspaceInEditor(3);
			break;
		case ID_ABSBUTTON:
			insertIntoEditor(" ABS() ");
			backspaceInEditor(2);
			break;
		case ID_SQRBUTTON:
			insertIntoEditor(" SQR() ");
			backspaceInEditor(2);
			break;
		case ID_SQRTBUTTON:
			insertIntoEditor(" SQRT() ");
			backspaceInEditor(2);
			break;
		case ID_TRUNCBUTTON:
			insertIntoEditor(" TRUNC() ");
			backspaceInEditor(2);
			break;
		case ID_POWBUTTON:
			insertIntoEditor(" POW(,) ");
			backspaceInEditor(3);
			break;

			// Statistics Operators
		case ID_MINBUTTON:
			insertIntoEditor(" MIN(,) ");
			backspaceInEditor(3);
			break;
		case ID_MAXBUTTON:
			insertIntoEditor(" MAX(,) ");
			backspaceInEditor(3);
			break;
		case ID_AVERAGEBUTTON:
			insertIntoEditor(" AVG(,) ");
			backspaceInEditor(3);
			break;
		case ID_MEDIANBUTTON:
			insertIntoEditor(" MED(,) ");
			backspaceInEditor(3);
			break;
		case ID_SUM_BUTTON:
			insertIntoEditor(" SUM(,) ");
			backspaceInEditor(3);
			break;
		case ID_STD_DEV_BUTTON:
			insertIntoEditor(" STDDEV(,) ");
			backspaceInEditor(3);
			break;

			// Punctuation Bar
		case ID_FORMULA_LPARENBUTTON:
			insertIntoEditor("(");
			break;
		case ID_FORMULA_RPARENBUTTON:
			insertIntoEditor(")");
			break;
		case ID_FORMULA_COMMABUTTON:
			insertIntoEditor(",");
			break;

			// Cursor Bar
		case ID_FORMULA_LARROWBUTTON:
			backspaceInEditor(1);
			break;
		case ID_FORMULA_RARROWBUTTON:
			advanceInEditor(1);
			break;
		case ID_FORMULA_DELETEBUTTON:
			deleteInEditor(1);
			break;
		case ID_FORMULA_TEST:
			onValidate();
			break;
		case ID_FORMULA_UNDO:
			onUndo();
			break;
		}// end switch (LOWORD(wParam))

		return CPropertyPage::OnCommand(wParam, lParam);
	}

	bool SVFormulaEditorPageClass::validateAndSetEquation() 
	{
		if( GetSafeHwnd() )
		{
			std::string equationText = getEquationText();

			UpdateData( TRUE ); // Update the variables

			double value = 0;
			SvStl::MessageContainerVector ErrorMessages;
			const int result = m_FormulaController->ValidateEquation( equationText, value, true, ErrorMessages );
			for (SvStl::MessageContainerVector::iterator iter = ErrorMessages.begin(); ErrorMessages.end() != iter; iter++ )
			{
				SvStl::MessageMgrStd message(SvStl::MsgType::Log | SvStl::MsgType::Display );
				message.setMessage(iter->getMessage());
			}
			enableUndoButton();
			//@Info (MZA): result also true if resetFailed, because the fail of reset of the object maybe happen by a other page.
			//By leaving the sheet it will reset objects and check if it works
			return result == SvOi::IFormulaController::validateSuccessful || result == SvOi::IFormulaController::resetFailed;
		}
		return true;
	}

	void SVFormulaEditorPageClass::onValidate() 
	{
		std::string equationText = getEquationText();

		UpdateData( TRUE ); // Update the variables
		double value = 0;
		SvStl::MessageContainerVector ErrorMessages;
		const int result = m_FormulaController->ValidateEquation( equationText, value, false, ErrorMessages );
		for (SvStl::MessageContainerVector::iterator iter = ErrorMessages.begin(); ErrorMessages.end() != iter; iter++ )
		{
			SvStl::MessageMgrStd message(SvStl::MsgType::Log | SvStl::MsgType::Display );
			message.setMessage(iter->getMessage());
		}
		if(SvOi::IFormulaController::validateSuccessful == result || SvOi::IFormulaController::resetFailed == result)
		{
			SvStl::MessageTextEnum id = SvStl::Tid_Empty;
			SvDef::StringVector msgList;
			if (m_isConditionalPage)
			{
				msgList.push_back(SvStl::MessageData::convertId2AddtionalText(( value ) ? SvStl::Tid_True : SvStl::Tid_False));
				id = SvStl::Tid_ConditionalValidated;
			}
			else
			{
				msgList.push_back(SvUl::Format(_T("%lf"), value));
				id = SvStl::Tid_FormulaValidated;
			}
			SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
			Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, id, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10223 );
		}
		else // Something is wrong
		{
			HandleValidateError(result);
		}
	}

	void SVFormulaEditorPageClass::onUndo()
	{
		setEquationText();
		enableUndoButton();
	}

	void SVFormulaEditorPageClass::OnEquationFieldChanged(NMHDR* pNotifyStruct, LRESULT* plResult)
	{
		if( nullptr != m_EditWnd.GetSafeHwnd() )
		{
			int length =  static_cast<int>(m_EditWnd.SendMessage(SCI_GETLENGTH));
			if(SV_EQUATION_BUFFER_SIZE<=length)
			{ //text too long, remove the chars they are too many
				int numberOfDelete = length-SV_EQUATION_BUFFER_SIZE+1;
				int pos = static_cast<int>(m_EditWnd.SendMessage(SCI_GETCURRENTPOS));
			
				// Delete characters
				m_EditWnd.SendMessage( SCI_DELETERANGE, pos-numberOfDelete, numberOfDelete );
			}
			CheckBrace();
		}

		enableUndoButton();

	}

	BOOL SVFormulaEditorPageClass::OnKillActive()
	{
		if( !validateAndSetEquation() )
		{
			// Equation must be valid or disabled
			SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
			Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_InvalidFormula, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10222 );
			return FALSE;
		}
		else
		{
			return CPropertyPage::OnKillActive();
		}
	}

	void SVFormulaEditorPageClass::HandleValidateError( int posFailed )
	{
		if( nullptr != m_EditWnd.GetSafeHwnd() )
		{
			m_EditWnd.SendMessage(SCI_SETSEL, posFailed+1, posFailed+1);
		}
	}

	void SVFormulaEditorPageClass::enableUndoButton()
	{
		std::vector<TCHAR> equationText;
		equationText.resize(SV_EQUATION_BUFFER_SIZE, 0);

		if( nullptr != m_EditWnd.GetSafeHwnd() )
		{
			m_EditWnd.SendMessage( SCI_GETTEXT, SV_EQUATION_BUFFER_SIZE, reinterpret_cast<LPARAM>( &equationText[0] ) );
		}

		if(&equationText[0] == m_FormulaController->GetEquationText() )
		{
			m_validateBar.EnableButton(ID_FORMULA_UNDO, false);
		}
		else
		{
			m_validateBar.EnableButton(ID_FORMULA_UNDO, true);
		}
	}

	void SVFormulaEditorPageClass::enableControls()
	{
		//all buttons enabled if it is not the conditional page
		bool state = !m_isConditionalPage; 

		if (m_isConditionalPage)
		{   //if conditional page, set state depends on if tool or equation is enabled or disabled
			state = !m_ownerDisabled;
			//disable equation checkbox if tool is disabled
			m_DisableEquationCtrl.EnableWindow( state );
			//if tool or equation disabled, all buttons should be disabled
			state = ( !m_ownerDisabled && !m_equationDisabled );
		}

		if (true == state)
		{
			m_validateBar.EnableWindow( true );
			m_validateBar.EnableButton(ID_FORMULA_TEST, true);
			enableUndoButton();
		}
		else
		{
			m_validateBar.EnableWindow( state );
		}

		m_ToolsetOutputSelectButton.EnableWindow( state );
		m_AddLocalVariableCtrl.EnableWindow( state );

		m_ConstantEditCtrl.EnableWindow( state );
		m_AddConstantCtrl.EnableWindow( state );
		m_decimalRadioButton.EnableWindow( state );
		m_hexadecimalRadioButton.EnableWindow( state );
		m_binaryRadioButton.EnableWindow( state );

		if( nullptr != m_EditWnd.GetSafeHwnd() )
		{
			m_EditWnd.EnableWindow( state );
		}

		m_logicalOperatorBar.EnableWindow( state );
		m_conditionalOperatorBar.EnableWindow( state );
		m_cursorBar.EnableWindow( state );
		m_trigOperatorBar.EnableWindow( state );
		m_miscOperatorBar.EnableWindow( state );
		m_basicOperatorBar.EnableWindow( state );
		m_punctuationBar.EnableWindow( state );
		m_statisticsOperatorBar.EnableWindow( state );
	}

	void SVFormulaEditorPageClass::createScintillaControl()
	{
		//This is not called when the Scintilla DLL could not be loaded
		m_EditWnd.Create(ScintillaControlName, "", WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_CLIPCHILDREN, CRect(327, 13, 665, 362), this, IDC_MATHCOND_EDITOR);

		// Use Default as Default for all styles
		m_EditWnd.SendMessage(SCI_STYLECLEARALL);

		// Set Lexer
		m_EditWnd.SendMessage( SCI_SETLEXER, SCLEX_SVO);

		m_EditWnd.SendMessage( SCI_SETWRAPMODE, SC_WRAP_WHITESPACE);
		m_EditWnd.SendMessage( SCI_SETHSCROLLBAR, false);
		m_EditWnd.SendMessage( SCI_SETVSCROLLBAR, true);

		// set end of line mode to CRLF
		m_EditWnd.SendMessage( SCI_CONVERTEOLS, 0);
		m_EditWnd.SendMessage( SCI_SETEOLMODE, 0);
	}

	bool SVFormulaEditorPageClass::IsBrace( int c )
	{
		switch (c)
		{
		case '(':
		case ')':
		case '[':
		case ']':
			return true;
		default:
			return false;
		}
	}

	void SVFormulaEditorPageClass::CheckBrace()
	{
		int curpos = -1;
		int bracePos1 = -1;
		int bracePos2 = -1;

		curpos =  static_cast<int>(m_EditWnd.SendMessage(SCI_GETCURRENTPOS));

		// check for bracket on right 
		if ( 0 <= curpos && IsBrace(static_cast<int>(m_EditWnd.SendMessage(SCI_GETCHARAT, curpos)) ) )
		{
			bracePos1 = curpos;
		}
		// no bracket on right, check left
		else if ( 0 < curpos && IsBrace(static_cast<int>(m_EditWnd.SendMessage(SCI_GETCHARAT, curpos - 1)) ) ) 
		{
			bracePos1 = curpos - 1 ; 
		}

		if (0 <= bracePos1 )
		{
			// Find the matching brace
			bracePos2 =  static_cast<int>(m_EditWnd.SendMessage(SCI_BRACEMATCH, bracePos1, 0));
			if (-1 == bracePos2 )
			{
				m_EditWnd.SendMessage(SCI_BRACEBADLIGHT, bracePos1);
			}
			else
			{
				m_EditWnd.SendMessage(SCI_BRACEHIGHLIGHT, bracePos1, bracePos2);
			}
		}
		else
		{
			m_EditWnd.SendMessage(SCI_BRACEHIGHLIGHT, -1, -1);
		}
	}

} //namespace SvOg

