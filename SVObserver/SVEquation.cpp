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
#include "SVInfoStructs.h"
#include "SVInspectionProcess.h"
#include "SVPPQObject.h"
#include "SVToolSet.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVObjectLibrary\GlobalConst.h"
#include "RootObject.h"
#include "TextDefinesSvO.h"
#include "SVStatusLibrary\MessageManager.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVUtilityLibrary/SVStringConversions.h"
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
	m_ToolSetName = SvUl_SF::LoadSVString( IDS_CLASSNAME_SVTOOLSET );
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
		SVObjectManagerClass::Instance().DisconnectObjectInput(pInObjectInfo->GetInputObjectInfo().UniqueObjectID, pInObjectInfo);
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
		if( !_tcscmp(name, (*it)->Name.c_str() ) )
		{
			return static_cast<int> (it - begin());
		}	
	}
	return -1; 
}

void SVEquationSymbolTableClass::Init(SVObjectClass* pRequestor)
{
	SVObjectClass* pInspection( nullptr );
	SVObjectAppClass* pAppClass = dynamic_cast<SVObjectAppClass*>(pRequestor);
	if( nullptr != pAppClass )
	{
		pInspection = pAppClass->GetInspection();
	}
	if( nullptr != pInspection )
	{
		m_InspectionName = pInspection->GetName();
		m_InspectionName += "."; 
	}	
}

int SVEquationSymbolTableClass::AddSymbol(LPCTSTR name, SVObjectClass* pRequestor )
{
	// Strip off Double Quotes
	SVString SymbolName = SvUl_SF::Trim( SVString(name), _T("\"" ) );

	SVEquationSymbolTypeEnum Type = SV_INPUT_SYMBOL_TYPE; 
	
	int symbolIndex = FindSymbol( SymbolName.c_str() ); 
	if(symbolIndex != -1)
	{
		///it is already in the vector
		return symbolIndex;
	}
	
	SVString LookUpName;
	
	if( SvUl_SF::Left(SymbolName, m_ToolSetName.size()) == m_ToolSetName )
	{
		Type = SV_TOOLSET_SYMBOL_TYPE;
		LookUpName = m_InspectionName;
		LookUpName += SymbolName;
	}
	else if ( SvUl_SF::Left(SymbolName, m_DIOInputName.size()) == m_DIOInputName
			||  SvUl_SF::Left(SymbolName, m_RemoteInputName.size()) == m_RemoteInputName )
	{
		LookUpName = m_InspectionName;
		LookUpName += SymbolName;
	}
	else
	{
		LookUpName = SymbolName;
	}
		
	SVObjectReference ObjectReference;

	HRESULT hr = SVObjectManagerClass::Instance().GetObjectByDottedName( LookUpName.c_str(), ObjectReference );
	if( S_OK != hr )
	{
		return -1;
	}

	if( FALSE == (ObjectReference.ObjectAttributesAllowed() & SV_SELECTABLE_FOR_EQUATION) )
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
	bool rc = SVObjectManagerClass::Instance().ConnectObjectInput(ObjectReference->GetObjectOutputInfo().UniqueObjectID, &pSymbolStruct->InObjectInfo);
	if( rc )
	{
		pSymbolStruct->IsValid = TRUE;
	}
	ASSERT(rc);
	
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
			{
				return S_FALSE;
			}
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
// .Description : Initialization of newly Instantiated Object
////////////////////////////////////////////////////////////////////////////////
void SVEquationClass::init()
{
	m_bUseOverlays = false;

	// Identify our output type
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVEquationObjectType;

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
		bOk = nullptr != GetInspection();
	}

	// Set / Reset Printable Flag
	enabled.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

	SVObjectClass *owner = enabled.GetOwner();
	if (nullptr != owner)
	{
		SVString conditionalString = SvUl_SF::LoadSVString( IDS_CLASSNAME_SVCONDITIONAL );

		if( owner->GetName() == conditionalString )
		{
			// Set / Reset Remotely Setable Flag, if owner is conditional class.
			enabled.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
		}
	}

	m_isCreated = bOk;

	return bOk;
}

BOOL SVEquationClass::HasCondition()
{
	return !equationStruct.EquationBuffer.empty();
}

double SVEquationClass::GetYACCResult() const
{
	return yacc.equationResult;
}

const SVString& SVEquationClass::GetEquationText() const
{
	return equationStruct.GetEquationText();
}

void SVEquationClass::SetEquationText(const SVString& rText)
{
	equationStruct.SetEquationText( rText );
	if( rText.empty() )
	{
		symbols.ClearAll();
		yacc.equationResult = 0.0;
	}
}

HRESULT SVEquationClass::GetObjectValue( const SVString& p_rValueName, VARIANT& p_rVariantValue ) const
{
	HRESULT hr = S_OK;

	if( p_rValueName == _T("EquationBuffer") )
	{
		_variant_t l_TempVariant;

		l_TempVariant.Attach( p_rVariantValue );

		l_TempVariant.SetString( equationStruct.GetEquationText().c_str() );

		l_TempVariant.Detach();
	}
	else
	{
		hr = SVTaskObjectClass::GetObjectValue( p_rValueName, p_rVariantValue );
	}

	return hr;
}

void SVEquationClass::Persist( SVObjectWriter& rWriter )
{
	SVTaskObjectClass::Persist(rWriter);
	
	// Get the Data Values (Member Info, Values)
	SVString Temp = equationStruct.EquationBuffer;
	
	SvUl::AddEscapeSpecialCharacters( Temp, true );

	_variant_t value;
	value.SetString(Temp.c_str());
	rWriter.WriteAttribute(scEquationBufferTag, value);
	value.Clear();
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
HRESULT SVEquationClass::SetObjectValue( SVObjectAttributeClass* pDataObject )
{
	HRESULT hr = S_FALSE;
	BOOL bOk = FALSE;

	SvCl::SVObjectSVStringArrayClass AttributeList;

	if ( ( bOk = pDataObject->GetAttributeData( _T("EquationBuffer"), AttributeList ) ) )
	{
		for( int i = 0;i < AttributeList.GetSize();i++ )
		{
			equationStruct.EquationBuffer = AttributeList.GetAt( i );

			SvUl::RemoveEscapedSpecialCharacters( equationStruct.EquationBuffer, true );
		}
	}
	else
	{
		hr = SVTaskObjectClass::SetObjectValue( pDataObject );
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
SvOi::EquationTestResult SVEquationClass::Test( bool DisplayErrorMessage, SvStl::MessageContainerVector *pErrorMessages )
{
	SvOi::EquationTestResult ret;
	m_lCurrentRunIndex = -1;
	isDataValid = TRUE;
	errContainer.clearMessage();

	if( HasCondition() && IsEnabled() )
	{
		
		SVOutputInfoListClass arInputAvailList;
		
		SVOutputInfoListClass toolSetPickList;
		
	
		// Clear the symbol Tables
		symbols.ClearAll();
		symbols.Init(this);
		

		SVString equationText = GetEquationText();
		equationText += "\n";
		// Note Buffer must be terminated with a new line
		// Build Lexical Stack
		SvOi::EquationTestResult result = lexicalScan( equationText.c_str() );
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
				SVString fullObjectName = GetCompleteObjectNameToObjectType( nullptr, SVInspectionObjectType );
				SVStringVector msgList;
				msgList.push_back(fullObjectName);
				if( S_OK != yacc.m_StatusCode )
				{
					errContainer.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_TooManyVariables, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10046, GetUniqueObjectID() );
				}
				else
				{
					ret.iPositionFailed = yacc.lex_stack[yacc.sIndex-1].position+1;
					msgList.push_back(SvUl_SF::Format(_T("%d"), ret.iPositionFailed));
					errContainer.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_EquationParserError, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10047, GetUniqueObjectID() );
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
		
		m_isObjectValid.SetValue( 1, ret.bPassed );
	}
	// return true if no equation or disabled
	if( !HasCondition() || !IsEnabled() )
	{
		ret.bPassed = true;
	}
	if( !ret.bPassed )
	{
		if (nullptr != pErrorMessages)
		{
			pErrorMessages->push_back(errContainer);
		}
		
		if (DisplayErrorMessage)
		{
			SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
			Msg.setMessage( errContainer.getMessage() ); 
		}
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
SvOi::EquationTestResult SVEquationClass::lexicalScan(LPCTSTR inBuffer)
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
	b = lex.yy_scan_string(inBuffer);

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
		SVString fullObjectName = GetCompleteObjectNameToObjectType( nullptr, SVInspectionObjectType );
		ret.bPassed = false;
		ret.iPositionFailed = static_cast< int >( lex.position + 1 );
		SVStringVector msgList;
		msgList.push_back(fullObjectName);
		msgList.push_back(SvUl_SF::Format(_T("%d"), ret.iPositionFailed));
		errContainer.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_EquationParserError, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10198, GetUniqueObjectID() );

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

bool SVEquationClass::DisconnectObjectInput( SVInObjectInfoStruct* pInObjectInfo )
{
	// Update ToolSet Symbol table - Gets called when one of our inputs goes away
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
					pSymbolInputObjectInfo->SetInputObject( nullptr );
					break;
				}
			}
		}
	}
	return __super::DisconnectObjectInput(pInObjectInfo);
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

	if ( S_OK != hr )
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

	if ( S_OK != hr )
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

	if ( S_OK != hr )
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
bool SVEquationClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	// call Test()...( Rebuilds symbol table !!! )
	if( HasCondition() && IsEnabled() )
	{
		//In reset object call test without displaying error messages is done using the returned result
		Result = Test( FALSE, pErrorMessages ).bPassed && Result;
	}

	return Result;
}

void SVEquationClass::OnObjectRenamed(const SVObjectClass& rRenamedObject, const SVString& rOldName)
{
	SVString newPrefix;
	SVString oldPrefix;

	if( const SVInspectionProcess* pInspection = dynamic_cast<const SVInspectionProcess*> (&rRenamedObject) )
	{
		newPrefix = _T( "." ) + pInspection->GetCompleteObjectNameToObjectType( nullptr, SVInspectionObjectType ) + _T( "." );
	}// end if
	else if( const BasicValueObject* pBasicValueObject = dynamic_cast<const BasicValueObject*> (&rRenamedObject) )
	{
		newPrefix = _T( "\"" ) + pBasicValueObject->GetCompleteObjectNameToObjectType( nullptr, SVRootObjectType ) + _T( "\"" );
	}
	else if( const SVValueObjectClass* pValueObject = dynamic_cast<const SVValueObjectClass*> (&rRenamedObject) )
	{
		newPrefix = _T( "\"" ) + pValueObject->GetCompleteObjectNameToObjectType( nullptr, SVToolSetObjectType ) + _T( "\"" );
	}
	else
	{
		newPrefix = _T( "\"" ) + rRenamedObject.GetCompleteObjectNameToObjectType( nullptr, SVToolSetObjectType ) + _T( "." );
	}// end else
	oldPrefix = newPrefix;
	SvUl_SF::searchAndReplace( oldPrefix, rRenamedObject.GetName(), rOldName.c_str() );

	SVString equationBuff = GetEquationText();

	// Replace all occurences
	if( GetEquationText() != SvUl_SF::searchAndReplace( equationBuff, oldPrefix.c_str(), newPrefix.c_str() ) )
	{
		SetEquationText( equationBuff );
	}

	__super::OnObjectRenamed(rRenamedObject, rOldName);
}
