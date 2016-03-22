//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEquation.cpp
//* .File Name       : $Workfile:   SVEquation.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.9  $
//* .Check In Date   : $Date:   19 Dec 2014 03:59:32  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <float.h>
#include "SVEquation.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVUtilityLibrary/SVSAFEARRAY.h"
#include "SVGlobal.h"
#include "SVInfoStructs.h"
#include "SVInspectionProcess.h"
#include "SVPPQObject.h"
#include "SVToolSet.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVObjectLibrary\GlobalConst.h"
#include "RootObject.h"
#include "TextDefinesSvO.h"
#include "SVStatusLibrary\MessageManagerResource.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations


SVEquationSymbolTableClass::SVEquationSymbolTableClass()
{
	m_ToolSetName.LoadString( IDS_CLASSNAME_SVTOOLSET );
	m_ToolSetName +=  "."; 
	m_DIOInputName = SvO::DIOInput;
	m_RemoteInputName = SvO::Remote_Input;
}

SVEquationSymbolTableClass::~SVEquationSymbolTableClass()
{

	ClearAll();
}

/////////////////////////////////////////////////////////////////
// Clear the Symbol Tables
/////////////////////////////////////////////////////////////////
void SVEquationSymbolTableClass::ClearAll()
{
	// Disconnect the dynmaic Inputs
	for( int i = m_toolsetSymbolTable.GetSize() - 1; i >= 0; i-- )
	{
		SVInObjectInfoStruct* pInObjectInfo = m_toolsetSymbolTable.GetAt( i );
		::SVSendMessage(pInObjectInfo->GetInputObjectInfo().UniqueObjectID,
										SVM_DISCONNECT_OBJECT_INPUT, 
										reinterpret_cast<DWORD_PTR>(pInObjectInfo), NULL );
	}
	// Empty the ToolSet Symbol table 
	m_toolsetSymbolTable.RemoveAll();

	// Empty the Combined Symbol table
	for( int n = GetSize() - 1;n >= 0; n-- )
	{
		SVEquationSymbolStruct* pSymbol = GetAt( n );
		RemoveAt( n );
		if( pSymbol)
			delete pSymbol;
	}
	//RemoveAll();
}



/////////////////////////////////////////////////////////////////
// Find a Symbol based on name
/////////////////////////////////////////////////////////////////
int SVEquationSymbolTableClass::FindSymbol( LPCTSTR name )
{
	for( const_iterator it = begin(); it != end(); it++ )
	{
		if( !_tcscmp(name, (*it)->Name ) )
		{
			return static_cast<int> (it - begin());
		}	
	}
	return -1; 
}

void SVEquationSymbolTableClass::Init(SVObjectClass* pRequestor)
{
	
	SVInspectionProcess *pInspection(nullptr);
	SVObjectAppClass* pAppClass = dynamic_cast<SVObjectAppClass*>(pRequestor);
	if(pAppClass != nullptr)
	{
		pInspection = 	pAppClass->GetInspection();
	}
	if(pInspection)
	{
		m_InspectionName = pInspection->GetName();
		m_InspectionName += "."; 
		
	}	
}


int SVEquationSymbolTableClass::AddSymbol(LPCTSTR name, SVObjectClass* pRequestor )
{
	
	// Strip off Double Quotes
	CString csName = name;
	//csName.Remove( _T( '\"' ) );
	csName.Trim(_T("\"" ) );

	SVEquationSymbolTypeEnum Type = SV_INPUT_SYMBOL_TYPE; 
	
	
	int symbolIndex = FindSymbol( csName ); 
	if(symbolIndex != -1)
	{
		///it is already in the vector
		return symbolIndex;
	}
	
	CString strLookUpName;
	
	if(0== csName.Left(m_ToolSetName.GetLength()).Compare(m_ToolSetName))
	{
		Type = SV_TOOLSET_SYMBOL_TYPE;
		strLookUpName = m_InspectionName;
		strLookUpName += csName;
	}
	else if ( 0== csName.Left(m_DIOInputName.GetLength()).Compare(m_DIOInputName) 
			||  0== csName.Left(m_RemoteInputName.GetLength()).Compare(m_RemoteInputName) )
	{
		strLookUpName = m_InspectionName;
		strLookUpName += csName;
	}
	else
	{
		strLookUpName = csName;
	}

	
	
	SVObjectReference ObjectReference;

	HRESULT hr = SVObjectManagerClass::Instance().GetObjectByDottedName( strLookUpName.GetString(), ObjectReference );
	if(hr != S_OK)
	{
		return -1;
	}

	if(FALSE == (ObjectReference.ObjectAttributesAllowed() & SV_SELECTABLE_FOR_EQUATION ))
	{
		return -1;
	}

	// Add to combined symbol table if not already there
	// Objects will be deletet in ClearAll() called by the ~SVEquationSymbolTableClass:;
	SVEquationSymbolStruct* pSymbolStruct = new SVEquationSymbolStruct();	
	pSymbolStruct->Type = Type;
	pSymbolStruct->Name = name;
	// Set who wants to use the variable
	pSymbolStruct->InObjectInfo.SetObject( pRequestor->GetObjectInfo() );

	// Set the variable to be used
	pSymbolStruct->InObjectInfo.SetInputObjectType( ObjectReference->GetObjectOutputInfo().ObjectTypeInfo);
	pSymbolStruct->InObjectInfo.SetInputObject( ObjectReference->GetObjectOutputInfo().UniqueObjectID );

	// Try to Connect at this point
	DWORD_PTR rc = ::SVSendMessage(ObjectReference->GetObjectOutputInfo().UniqueObjectID, SVM_CONNECT_OBJECT_INPUT, reinterpret_cast<DWORD_PTR>(&pSymbolStruct->InObjectInfo), NULL);
	if( rc == SVMR_SUCCESS )
	{
		pSymbolStruct->IsValid = TRUE;
	}
		
	ASSERT(rc == SVMR_SUCCESS);
	
	symbolIndex = Add( pSymbolStruct );
	// add it to the top
	m_toolsetSymbolTable.Add( &pSymbolStruct->InObjectInfo );
	return symbolIndex;
}
	

/////////////////////////////////////////////////////////////////
// Get the ToolSet Symbol table
/////////////////////////////////////////////////////////////////
SVInputInfoListClass& SVEquationSymbolTableClass::GetToolSetSymbolTable()
{
	return m_toolsetSymbolTable;
}

/////////////////////////////////////////////////////////////////
// Get the data from either a Input Symbol or a ToolSet Symbol
/////////////////////////////////////////////////////////////////
HRESULT SVEquationSymbolTableClass::GetData( int iSymbolIndex, double& value, long lBufferIndex )
{
	if( iSymbolIndex >= 0 && iSymbolIndex < GetSize() )
	{
		SVEquationSymbolStruct* pSymbolStruct = GetAt( iSymbolIndex );

		if( pSymbolStruct->Type == SV_TOOLSET_SYMBOL_TYPE )
		{
			// Get the Data
			if( pSymbolStruct->InObjectInfo.IsConnected() && pSymbolStruct->InObjectInfo.GetInputObjectInfo().PObject )
			{
				SVValueObjectClass* pValueObject = static_cast <SVValueObjectClass*>(pSymbolStruct->InObjectInfo.GetInputObjectInfo().PObject);
				HRESULT hr = pValueObject->GetValue( value );
				return hr;
			}
		}

		else	
		{
			// Get the Data
			SVObjectClass *object = pSymbolStruct->InObjectInfo.GetInputObjectInfo().PObject;
			if( SVValueObjectClass* pValueObject = dynamic_cast<SVValueObjectClass*> (object) )
			{
				return pValueObject->GetValue( value );
			}
			else if( BasicValueObject* pBasicValueObject = dynamic_cast<BasicValueObject*> (object) )
			{
				return pBasicValueObject->getValue( value );
			}
			else
				return S_FALSE;
		}

	}
	return S_FALSE;
}

HRESULT SVEquationSymbolTableClass::GetData( int iSymbolIndex, int iIndex, double& value, long lBufferIndex )
{
	if( iSymbolIndex >= 0 && iSymbolIndex < GetSize() )
	{
		SVEquationSymbolStruct* pSymbolStruct = GetAt( iSymbolIndex );

		if( pSymbolStruct->Type == SV_INPUT_SYMBOL_TYPE )
		{
			// Get the Data
			if( SVValueObjectClass* pValueObject = dynamic_cast<SVValueObjectClass*> (pSymbolStruct->InObjectInfo.GetInputObjectInfo().PObject) )
			{
				return pValueObject->GetValue( pValueObject->GetLastSetIndex(), iIndex, value );
			}
			else
				return S_FALSE;
		}
		else if( pSymbolStruct->Type == SV_TOOLSET_SYMBOL_TYPE )
		{
			// Get the Data
			if( pSymbolStruct->InObjectInfo.IsConnected() && pSymbolStruct->InObjectInfo.GetInputObjectInfo().PObject )
			{
				SVValueObjectClass* pValueObject = static_cast<SVValueObjectClass*>(pSymbolStruct->InObjectInfo.GetInputObjectInfo().PObject);
				return pValueObject->GetValue( pValueObject->GetLastSetIndex(), iIndex, value );
			}
		}
	}
	return S_FALSE;
}

HRESULT SVEquationSymbolTableClass::GetData(int iSymbolIndex, std::vector<double>& values, long lBufferIndex )
{
	if( iSymbolIndex >= 0 && iSymbolIndex < GetSize() )
	{
		SVEquationSymbolStruct* pSymbolStruct = GetAt( iSymbolIndex );

		if( pSymbolStruct->Type == SV_INPUT_SYMBOL_TYPE )
		{
			// Get the Data
			if( SVValueObjectClass* pValueObject = dynamic_cast<SVValueObjectClass*> (pSymbolStruct->InObjectInfo.GetInputObjectInfo().PObject) )
			{
				return pValueObject->GetValues( pValueObject->GetLastSetIndex(), values );
			}
			else
				return S_FALSE;
		}
		else if( pSymbolStruct->Type == SV_TOOLSET_SYMBOL_TYPE )
		{
			// Get the Data
			if( pSymbolStruct->InObjectInfo.IsConnected() && pSymbolStruct->InObjectInfo.GetInputObjectInfo().PObject )
			{
				SVValueObjectClass* pValueObject = static_cast<SVValueObjectClass*>(pSymbolStruct->InObjectInfo.GetInputObjectInfo().PObject);
				return pValueObject->GetValues( pValueObject->GetLastSetIndex(), values );
			}
		}
	}
	return S_FALSE;
}

//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVEquationClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

//******************************************************************************
// Adjustments
//******************************************************************************
SV_IMPLEMENT_CLASS( SVEquationClass, SVEquationClassGuid );

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
SVEquationClass::SVEquationClass( SVObjectClass* POwner, int StringResourceID )
:SVTaskObjectClass( POwner, StringResourceID )
{
	// Give SVEquationLexClass and SVEquationYaccClass a pointer to us
	lex.pEquation = this;
	yacc.pEquation = this;

	init();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Initialization of class SVEquationClass
// -----------------------------------------------------------------------------
// .Description : Initialization of newly Instantiated Object
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :10.08.1999 SEJ			First Implementation
////////////////////////////////////////////////////////////////////////////////
void SVEquationClass::init()
{
	m_bUseOverlays = false;

	// Identify our output type
	outObjectInfo.ObjectTypeInfo.ObjectType = SVEquationObjectType;

	// Identify our input type needs - this is a bit different here
	// Since out inputs are dynamic via the script specified
	// So the input will be identified when the script is created.
	
	// SetObjectDepth() already called in SVObjectClass Ctor

	// Register Embedded Objects
	RegisterEmbeddedObject( &enabled, SVEquationEnabledObjectGuid, IDS_OBJECTNAME_ENABLED, false, SVResetItemNone );

	// Set Embedded defaults
	enabled.SetDefaultValue( TRUE, TRUE );

	// Set default inputs and outputs
	addDefaultInputObjects();

	// Set local defaults
	isDataValid = TRUE;

	m_lCurrentRunIndex = -1;
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
SVEquationClass::~SVEquationClass()
{
}



////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
BOOL SVEquationClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;

	if( SVTaskObjectClass::CreateObject( PCreateStructure ) )
	{
		bOk = GetInspection() != NULL;
	}

	// Set / Reset Printable Flag
	enabled.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

	SVObjectClass *owner = enabled.GetOwner();
	if (nullptr != owner)
	{
		SVString ownerName(owner->GetName());
		CString conditionalString;
		conditionalString.LoadString( IDS_CLASSNAME_SVCONDITIONAL );

		if (0 == ownerName.compare(conditionalString))
		{
			// Set / Reset Remotely Setable Flag, if owner is conditional class.
			enabled.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
		}
	}

	isCreated = bOk;

	return bOk;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : NeedsConditionalPPQData
// -----------------------------------------------------------------------------
// .Description : 
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	:None
// .Return Value
//	:BOOL
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :26.08.1999 SEJ			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVEquationClass::NeedsConditionalPPQData()
{
	if( HasCondition() && IsEnabled() )
		return TRUE;

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : HasCondition
// -----------------------------------------------------------------------------
// .Description : 
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	:None
// .Return Value
//	:BOOL
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :30.06.1999 SEJ			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVEquationClass::HasCondition()
{
	return !equationStruct.EquationBuffer.IsEmpty();
}

double SVEquationClass::GetYACCResult() const
{
	return yacc.equationResult;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : GetEquationText
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :25.05.1999 SEJ			First Implementation
////////////////////////////////////////////////////////////////////////////////
void SVEquationClass::GetEquationText(CString& text) const
{
	text = equationStruct.EquationBuffer;
}

void SVEquationClass::GetEquationText(SVString& text) const
{
	text = equationStruct.EquationBuffer.GetString();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : SetEquationText
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :25.05.1999 SEJ			First Implementation
////////////////////////////////////////////////////////////////////////////////
void SVEquationClass::SetEquationText(const CString& text)
{
	equationStruct.EquationBuffer = text;
	if (text.IsEmpty())
	{
		symbols.ClearAll();
		yacc.equationResult = 0.0;
	}
}

void SVEquationClass::SetEquationText(const SVString& text)
{
	SetEquationText(CString(text.c_str()));
}

#ifdef USE_OBJECT_SCRIPT
////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
void SVEquationClass::GetObjectScript( CString& RStrScript, CString& RStrAliasTable, int Indent )
{
	CString script;

	// preallocate 16K
    script.GetBuffer(16384);  
	script.ReleaseBuffer(); 
	
	// Get the Heading (Class Info)
	SVTaskObjectClass::GetObjectScript( script, RStrAliasTable, Indent );

	CString nameStrDelimiter = _T( "'" );

	// Generate indent...
	CString strIndent = _T( "\n" );

	if( Indent )
	{
		CString tabsStrIndent(_T( '\t' ), Indent);
		strIndent += tabsStrIndent;
	}

	// Overwrite task object termination termination...
	int last = script.ReverseFind( TCHAR( '}' ) );
	if( last >= 0 )
		script = script.Left( last );

	// Name is delimited by single quotes
	CString objectTag = nameStrDelimiter + _T( "_object_ID_" ) + GetName();

	// Get the Data Values (Member Info, Values)
	CString tmp, tmp1;
	tmp1 = equationStruct.EquationBuffer;
	
	::SVAddEscapeSpecialCharacters( tmp1, false );

	tmp.Format( _T( "STRING \"%s\"" ), tmp1 );

	// Name is delimited by single quotes - SEJ july 23,1999
	script += strIndent + objectTag + _T(".EquationBuffer" ) + nameStrDelimiter + _T( " = " ) + tmp + _T( ";" );

	// Add termination...
	script += strIndent + _T( "};" );

	script.FreeExtra();
	
	RStrScript += script;
}
#endif
HRESULT SVEquationClass::GetObjectValue( const SVString& p_rValueName, VARIANT& p_rVariantValue ) const
{
	HRESULT hr = S_OK;

	if( p_rValueName == _T( "EquationBuffer" ) )
	{
		_variant_t l_TempVariant;

		l_TempVariant.Attach( p_rVariantValue );

		l_TempVariant = static_cast< LPCTSTR >( equationStruct.EquationBuffer );

		l_TempVariant.Detach();
	}
	else
	{
		hr = SVTaskObjectClass::GetObjectValue( p_rValueName, p_rVariantValue );
	}

	return hr;
}

HRESULT SVEquationClass::SetObjectValue( const SVString& p_rValueName, const _variant_t& p_rVariantValue )
{
	HRESULT hr = S_OK;

	if( p_rValueName == _T( "EquationBuffer" ) )
	{
		if( p_rVariantValue.vt == VT_BSTR )
		{
			equationStruct.EquationBuffer = static_cast< LPCTSTR >( _bstr_t( p_rVariantValue ) );
		}
		else
		{
			hr = E_FAIL;
		}
	}
	else
	{
		hr = SVTaskObjectClass::SetObjectValue( p_rValueName, p_rVariantValue );
	}

	return hr;
}

void SVEquationClass::Persist( SVObjectWriter& rWriter )
{
	SVTaskObjectClass::Persist(rWriter);
	
	// Get the Data Values (Member Info, Values)
	CString tmp = equationStruct.EquationBuffer;
	
	::SVAddEscapeSpecialCharacters( tmp, true );

	_variant_t value;
	value.SetString(tmp);
	rWriter.WriteAttribute(scEquationBufferTag, value);
	value.Clear();
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
HRESULT SVEquationClass::SetObjectValue( SVObjectAttributeClass* PDataObject )
{
	HRESULT hr = S_FALSE;
	BOOL bOk = FALSE;

	SvCl::SVObjectCStringArrayClass svCStringArray;

	if ( ( bOk = PDataObject->GetAttributeData( "EquationBuffer", svCStringArray ) ) )
	{
		for( int i = 0;i < svCStringArray.GetSize();i++ )
		{
			equationStruct.EquationBuffer = svCStringArray.GetAt( i );

			::SVRemoveEscapedSpecialCharacters( equationStruct.EquationBuffer, true );
		}
	}
	else
	{
		hr = SVTaskObjectClass::SetObjectValue( PDataObject );
		return hr;
	}

	hr = bOk ? S_OK : S_FALSE;
	return hr;
}


////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
BOOL SVEquationClass::IsEnabled()
{
	BOOL bEnabled;
	enabled.GetValue( bEnabled );
		
	return bEnabled;
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
SvOi::EquationTestResult SVEquationClass::Test( bool DisplayErrorMessage )
{
	SvOi::EquationTestResult ret;
	m_lCurrentRunIndex = -1;
	isDataValid = TRUE;
	errStr.Empty();

	if( HasCondition() && IsEnabled() )
	{
		
		SVOutputInfoListClass arInputAvailList;
		
		SVOutputInfoListClass toolSetPickList;
		
	
		// Clear the symbol Tables
		symbols.ClearAll();
		symbols.Init(this);
		

		CString equationText;
		GetEquationText( equationText );
		equationText += "\n";
		// Note Buffer must be terminated with a new line
		LPTSTR pEquation = equationText.GetBuffer( equationText.GetLength() + 1 );
		// Build Lexical Stack
		SvOi::EquationTestResult result = lexicalScan( pEquation );
		if( result.bPassed )
		{
			// Parse the equation
			yacc.sIndex = 0;
			yacc.yacc_err = 0;

			try
			{
				// Get the default control word.   
				int cw = _controlfp( 0,0 );
			
				// This one turns off exception generation
				int newCw = cw | (EM_OVERFLOW|EM_UNDERFLOW|EM_INEXACT|EM_ZERODIVIDE|EM_DENORMAL);

				// Set the exception masks OFF, turn exceptions on.
				//int newCw = cw & (~(EM_OVERFLOW|EM_UNDERFLOW|EM_INEXACT|EM_ZERODIVIDE|EM_DENORMAL));

				// Set the control word.   
				_controlfp( newCw, MCW_EM );

				yacc.yyparse();

				// Reset it back
				_controlfp( cw, MCW_EM );
			}
		
			catch( ... )
			{
				_clearfp();

				isDataValid = FALSE;
			}

			//yacc.yyparse();
	
			if (yacc.yacc_err)
			{
				CString yaccErrorStr,fullObjectName;
				fullObjectName = GetCompleteObjectNameToObjectType( NULL, SVInspectionObjectType );
				if( yacc.m_StatusCode != S_OK )
				{
					yaccErrorStr.LoadString( IDS_TOO_MANY_VARIABLES );
					errStr.Format( _T( "%s \n%s" ), fullObjectName, yaccErrorStr );
				}
				else
				{
					ret.iPositionFailed = yacc.lex_stack[yacc.sIndex-1].position+1;
					yaccErrorStr.Format( IDS_EQUATION_PARSER_ERROR, ret.iPositionFailed );
					errStr.Format(_T( "%s \n%s" ), fullObjectName, yaccErrorStr );
				}
				isDataValid = FALSE;
			}
			else
			{
				// Check for DataValid from the Variables - not at this Point
				ret.bPassed = true;
			}
		}
		else
		{
			ret = result;
		}
		equationText.ReleaseBuffer();
		
		
		isObjectValid.SetValue( 1, ret.bPassed );
	}
	// return true if no equation or disabled
	if( !HasCondition() || !IsEnabled() )
		ret.bPassed = true;

	if( DisplayErrorMessage && !ret.bPassed )
	{
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, errStr, StdMessageParams, SvOi::Err_10046 ); 
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
SvOi::EquationTestResult SVEquationClass::lexicalScan(LPSTR inBuf)
{
	SvOi::EquationTestResult ret;
	ret.bPassed = true;

	// Initialize the lexical scanner
	lex.lex_err = 0;
	lex.syntax_check = 1;
	lex.currentPos = 0;
	lex.position = 0;
	
	// Initialize the parser
	yacc.yacc_err = 0;
	yacc.sIndex = 0;

	// For lexical scanner
	struct yy_buffer_state* b;
	int yychar;
	int sIndex = 0;
	// Scan the input buffer
	b = lex.yy_scan_string(inBuf);

	// Find the Tokens and build the token stack for YACC
	while ((yychar = lex.yylex()) > 0)
	{
		yacc.lex_stack[sIndex].position = static_cast< int >( lex.position );
		yacc.lex_stack[sIndex].token = yychar;
		if( yychar == SV_IDENTIFIER )
			yacc.lex_stack[sIndex].value.index = lex.yylval.index;
		else
			yacc.lex_stack[sIndex].value.val = lex.yylval.val;
		sIndex++;
	}
		
	yacc.lex_stack[sIndex].position = static_cast< int >( lex.position );
	yacc.lex_stack[sIndex].token = yychar;
	if( yychar == SV_IDENTIFIER )
		yacc.lex_stack[sIndex].value.index = lex.yylval.index;
	else
		yacc.lex_stack[sIndex].value.val = lex.yylval.val;
	sIndex++;

	yacc.numTokens = sIndex;

	if (lex.lex_err)
	{
		CString lexErrorStr,fullObjectName;
		fullObjectName = GetCompleteObjectNameToObjectType( NULL, SVInspectionObjectType );
		ret.bPassed = false;
		ret.iPositionFailed = static_cast< int >( lex.position + 1 );
		lexErrorStr.Format( IDS_EQUATION_PARSER_ERROR, ret.iPositionFailed );
		errStr.Format(_T( "%s \n%s" ), fullObjectName, lexErrorStr );

		isDataValid = FALSE;
	}
	// Release the buffer
	lex.yy_delete_buffer(b);

	return ret;
}

////////////////////////////////////////////////////////////////////////////////
// Rebuild Symbol table - Gets Called from lexer during scan of equation
////////////////////////////////////////////////////////////////////////////////
int SVEquationClass::AddSymbol( LPCTSTR name )
{
	int index = symbols.AddSymbol( name, this );

	if( -1 == index )
	{
		isDataValid = FALSE;
	}

	return index;
}

////////////////////////////////////////////////////////////////////////////////
// Update ToolSet Symbol table - Gets called when one of our inputs goes away
////////////////////////////////////////////////////////////////////////////////
BOOL SVEquationClass::DisconnectToolSetSymbol( SVInObjectInfoStruct* pInObjectInfo )
{
	if( pInObjectInfo )
	{
		SVInputInfoListClass& toolSetSymbols = symbols.GetToolSetSymbolTable();

		for( int i = 0;i < toolSetSymbols.GetSize(); i++ )
		{
			SVInObjectInfoStruct* pSymbolInputObjectInfo = toolSetSymbols.GetAt( i );

			if( pSymbolInputObjectInfo )
			{
				if( pInObjectInfo->GetInputObjectInfo().UniqueObjectID == pSymbolInputObjectInfo->GetInputObjectInfo().UniqueObjectID )
				{
					pSymbolInputObjectInfo->SetInputObject( NULL );
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

BOOL SVEquationClass::renameToolSetSymbol( const SVObjectClass* pObject, LPCTSTR orginalName )
{
	bool Result( false );

	if( nullptr != pObject )
	{
		SVString newPrefix;
		SVString oldPrefix;

		if( const SVInspectionProcess* pInspection = dynamic_cast<const SVInspectionProcess*> (pObject) )
		{
			newPrefix = _T( "." ) + pInspection->GetCompleteObjectNameToObjectType( NULL, SVInspectionObjectType ) + _T( "." );
		}// end if
		else if( const BasicValueObject* pBasicValueObject = dynamic_cast<const BasicValueObject*> (pObject) )
		{
			newPrefix = _T( "\"" ) + pBasicValueObject->GetCompleteObjectNameToObjectType( NULL, SVRootObjectType ) + _T( "\"" );
		}
		else
		{
			newPrefix = _T( "\"" ) + pObject->GetCompleteObjectNameToObjectType( NULL, SVToolSetObjectType ) + _T( "." );
		}// end else
		oldPrefix = newPrefix;
		SvUl_SF::searchAndReplace( oldPrefix, pObject->GetName(), orginalName );

		CString equationBuff;
		GetEquationText( equationBuff );

		// Replace all occurences
		if( equationBuff.Replace( oldPrefix.c_str(), newPrefix.c_str() ) )
		{
			SetEquationText( equationBuff );

			Result = true;
		}
	}
	return Result;
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
BOOL SVEquationClass::buildDynamicInputList()
{
	BOOL retVal = TRUE;

	// for all toolset Variables
	SVInObjectInfoStruct* pInObjectInfo;

	SVInputInfoListClass& dynaInputList = symbols.GetToolSetSymbolTable();

	for( int i = 0; i < dynaInputList.GetSize() ;i++ )
	{
		pInObjectInfo = dynaInputList.GetAt( i );

		// set the owner
		pInObjectInfo->SetObject( GetObjectInfo() );

		SVString l_Name = SvUl_SF::Format( _T( "Equation%d" ), i );

		// add to the owner list of inputs
		RegisterInputObject( pInObjectInfo, l_Name );

		// if not connected - try to connect it
		if( !pInObjectInfo->IsConnected() )
		{
			// Connect to the Input
			retVal = ::SVSendMessage( pInObjectInfo->GetInputObjectInfo().UniqueObjectID, SVM_CONNECT_OBJECT_INPUT, reinterpret_cast<DWORD_PTR>(pInObjectInfo), NULL ) && retVal;
		}
	}
	return retVal;
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
BOOL SVEquationClass::OnValidate()
{
	BOOL retVal = isDataValid;
	
	// validate our outputs
	retVal = SVTaskObjectClass::OnValidate() && retVal;

	if( !retVal )
		SetInvalid();

	return retVal;
}


////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
// If Conditional is disabled equation.Run() returns always TRUE.
// Otherwise the return value depends on the Conditional equation result!
BOOL SVEquationClass::onRun( SVRunStatusClass& RRunStatus )
{
	BOOL retVal = SVTaskObjectClass::onRun( RRunStatus );

	m_lCurrentRunIndex = RRunStatus.m_lResultDataIndex;

	if ( HasCondition() && IsEnabled() )
	{
		/////////////////////////////////////////////////////
		// The lexical Stack must be valid at this point
		// And the yacc parsing must not have any errors
		/////////////////////////////////////////////////////
		isDataValid = TRUE;

		yacc.sIndex = 0;
		yacc.yacc_err = 0;

		try
		{
			// Get the default control word.   
			int cw = _controlfp( 0,0 );
			
			// This one turns off exception generation
			int newCw = cw | (EM_OVERFLOW|EM_UNDERFLOW|EM_INEXACT|EM_ZERODIVIDE|EM_DENORMAL);

			// Set the exception masks OFF, turn exceptions on.
			//int newCw = cw & (~(EM_OVERFLOW|EM_UNDERFLOW|EM_INEXACT|EM_ZERODIVIDE|EM_DENORMAL));

			// Set the control word.   
			_controlfp( newCw, MCW_EM );

			yacc.yyparse();

			// Reset it back
			_controlfp( cw, MCW_EM );
		}
		
		catch( ... )
		{
			_clearfp();

			isDataValid = FALSE;
		}

		//yacc.yyparse();

		/////////////////////////////////////////////////////
		// Check for Valid Data
		/////////////////////////////////////////////////////
		if( !isDataValid || yacc.yacc_err )
		{
			retVal = FALSE;
			SetInvalid();
			RRunStatus.SetInvalid();
		}
	}
	if( retVal )
		RRunStatus.SetPassed();
	else
		RRunStatus.SetFailed();

	return retVal;
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
double SVEquationClass::GetPropertyValue( int iSymbolIndex )
{
	HRESULT hr = S_FALSE;
	double value = 0.0;

	if( iSymbolIndex != -1 )
	{
		if( m_lCurrentRunIndex != -1 )
			hr = symbols.GetData( iSymbolIndex, value, m_lCurrentRunIndex );
		else
			hr = symbols.GetData( iSymbolIndex, value, 1 );
	}// end if

	if ( hr != S_OK && hr != SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE)
	{
		isDataValid = FALSE;
	}
	return value;
}

HRESULT SVEquationClass::GetArrayValues( int iSymbolIndex, std::vector< double >& values )
{
	HRESULT hr = S_FALSE;
	double value = 0.0;

	if( iSymbolIndex != -1 )
	{
		if( m_lCurrentRunIndex != -1 )
			hr = symbols.GetData( iSymbolIndex, values, m_lCurrentRunIndex );
		else
			hr = symbols.GetData( iSymbolIndex, values, 1 );
	}// end if

	if ( hr != S_OK )
	{
		isDataValid = FALSE;
	}

	return hr;
}

double SVEquationClass::GetSubscriptedPropertyValue( int iSymbolIndex, int iIndex, double dDefault )	// user specifies a default
{
	HRESULT hr = S_FALSE;
	double value = dDefault;

	if( iSymbolIndex != -1 )
	{
		if( m_lCurrentRunIndex != -1 )
			hr = symbols.GetData( iSymbolIndex, iIndex, value, m_lCurrentRunIndex );
		else
			hr = symbols.GetData( iSymbolIndex, iIndex, value, 1 );
	}// end if

	if ( hr != S_OK )
	{
		value = dDefault;
	}

	return value;
}

double SVEquationClass::GetSubscriptedPropertyValue( int iSymbolIndex, int iIndex )	// user doesn't specify a default; use internal default of the value object
{
	HRESULT hr = S_FALSE;
	const double dDefault = 0.0;
	double value = dDefault;

	if( iSymbolIndex != -1 )
	{
		if( m_lCurrentRunIndex != -1 )
			hr = symbols.GetData( iSymbolIndex, iIndex, value, m_lCurrentRunIndex );
		else
			hr = symbols.GetData( iSymbolIndex, iIndex, value, 1 );
	}// end if

	if ( hr == S_FALSE )	// all other return codes use what value is set to (value object default value)
	{
		value = dDefault;
	}

	return value;
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
DWORD_PTR SVEquationClass::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = NULL;
	// Try to process message by yourself...
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;
	switch( dwPureMessageID )
	{
		// is sent in SVIPDoc::Validate() ( old PrepareForRunning() )
	case SVMSGID_RESET_ALL_OBJECTS:
		{
			HRESULT ResetStatus = ResetObject();
			if( ResetStatus != S_OK )
			{
				BOOL SilentReset = static_cast<BOOL> (DwMessageValue);

				if( !SilentReset && !errStr.IsEmpty() )
				{
					SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
					Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, errStr, StdMessageParams, SvOi::Err_10047 ); 
				}
				DwResult = SVMR_NO_SUCCESS;
			}
			else
			{
				DwResult = SVMR_SUCCESS;
			}
			break;
		}

		// This Message occurs for two scenarios
		// 1. Some Object is using our outputs and they are no longer needed.
		// 2. We are using some Object's outputs and the ouputs are no longer available
	case SVMSGID_DISCONNECT_OBJECT_INPUT:
		{
			// ...use second message parameter ( DwMessageValue ) as pointer to InObjectInfo ( SVInObjectInfoStruct* )
			// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
			SVInObjectInfoStruct* pInObjectInfo = ( SVInObjectInfoStruct* ) DwMessageValue;
			if( DisconnectToolSetSymbol( pInObjectInfo ) )
				DwResult = SVMR_SUCCESS;
		}
		break;

		// handle renaming of toolset variables
	case SVMSGID_OBJECT_RENAMED:
		{
			SVObjectClass* pObject = reinterpret_cast <SVObjectClass*> (DwMessageValue); // Object with new name
			LPCTSTR orgName = ( LPCTSTR )DwMessageContext;

			if( renameToolSetSymbol(pObject, orgName ) )
				DwResult = SVMR_SUCCESS;
		}
		break;
	}

	return( SVTaskObjectClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}

HRESULT SVEquationClass::ResetObject()
{
	HRESULT l_hrOk = SVTaskObjectClass::ResetObject();

	
	
	// call Test()...( Rebuilds symbol table !!! )
	if( HasCondition() && IsEnabled() )
	{
		//In reset object call test without displaying error messages is done using the returned result
		if( ! Test( FALSE ).bPassed )
		{
			l_hrOk = S_FALSE;
		}
	}

	return l_hrOk;
}


