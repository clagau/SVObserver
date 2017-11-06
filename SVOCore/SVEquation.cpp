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
#include "SVObjectLibrary/SVClsIds.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVUtilityLibrary/SVSafeArray.h"
#include "SVValueObjectLibrary/BasicValueObject.h"

#include "SVObjectLibrary/SVToolsetScriptTags.h"
#include "SVObjectLibrary/GlobalConst.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVUtilityLibrary/SVStringConversions.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations


const TCHAR* const DIOInput = _T("DIO.Input");
const TCHAR* const Remote_Input = _T("Remote Input");

SVEquationSymbolTableClass::SVEquationSymbolTableClass()
{
	m_ToolSetName = SvUl_SF::LoadSVString( IDS_CLASSNAME_SVTOOLSET );
	m_ToolSetName +=  "."; 
	m_DIOInputName = DIOInput;
	m_RemoteInputName = Remote_Input;
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
		SVObjectManagerClass::Instance().DisconnectObjectInput(pInObjectInfo->GetInputObjectInfo().m_UniqueObjectID, pInObjectInfo);
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

	if( 0 == (ObjectReference.ObjectAttributesAllowed() & SvDef::SV_SELECTABLE_FOR_EQUATION) )
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
	pSymbolStruct->InObjectInfo.SetInputObjectType( ObjectReference.getObject()->GetObjectOutputInfo().m_ObjectTypeInfo);
	pSymbolStruct->InObjectInfo.SetInputObject( ObjectReference.getObject()->GetObjectOutputInfo().m_UniqueObjectID );
	// Try to Connect at this point
	bool rc = SVObjectManagerClass::Instance().ConnectObjectInput(ObjectReference.getObject()->GetObjectOutputInfo().m_UniqueObjectID, &pSymbolStruct->InObjectInfo);
	assert(rc);
	if( rc )
	{
		pSymbolStruct->IsValid = true;
	}

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
HRESULT SVEquationSymbolTableClass::GetData( int SymbolIndex, double& rValue, int Index )
{
	if( SymbolIndex >= 0 && SymbolIndex < GetSize() )
	{
		SVEquationSymbolStruct* pSymbolStruct = GetAt( SymbolIndex );
		bool isValidObject( false );

		if( SV_TOOLSET_SYMBOL_TYPE == pSymbolStruct->Type )
		{
			isValidObject = pSymbolStruct->InObjectInfo.IsConnected() && nullptr != pSymbolStruct->InObjectInfo.GetInputObjectInfo().m_pObject;
			}
		else	
		{
			if(nullptr != pSymbolStruct->InObjectInfo.GetInputObjectInfo().m_pObject)
			{
				isValidObject = (nullptr != pSymbolStruct->InObjectInfo.GetInputObjectInfo().m_pObject);
			}
			else
			{
				return E_FAIL;
			}
		}
		if( isValidObject )
		{
			HRESULT hr = pSymbolStruct->InObjectInfo.GetInputObjectInfo().m_pObject->getValue( rValue, Index );
			return hr;
		}
		}
	return E_FAIL;
}

HRESULT SVEquationSymbolTableClass::GetData(int SymbolIndex, std::vector<double>& rValues )
{
	HRESULT Result ( E_FAIL );

	if( SymbolIndex >= 0 && SymbolIndex < GetSize() )
	{
		SVEquationSymbolStruct* pSymbolStruct = GetAt( SymbolIndex );
		bool isValidObject( false );

		if( SV_TOOLSET_SYMBOL_TYPE == pSymbolStruct->Type )
		{
			isValidObject = pSymbolStruct->InObjectInfo.IsConnected() && nullptr != pSymbolStruct->InObjectInfo.GetInputObjectInfo().m_pObject;
		}
		else if( SV_INPUT_SYMBOL_TYPE == pSymbolStruct->Type )
		{
			isValidObject = nullptr != pSymbolStruct->InObjectInfo.GetInputObjectInfo().m_pObject;
		}

		if( isValidObject )
		{
			Result = pSymbolStruct->InObjectInfo.GetInputObjectInfo().m_pObject->getValues( rValues );
		}
	}

	return Result;
}

SV_IMPLEMENT_CLASS( SVEquationClass, SVEquationClassGuid );

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
SVEquationClass::SVEquationClass( SVObjectClass* POwner, int StringResourceID )
:SVTaskObjectClass( POwner, StringResourceID )
{
	// Give SVEquationLexClass and SVEquationYaccClass a pointer to us
	m_Lex.pEquation = this;
	m_Yacc.pEquation = this;

	init();
}

////////////////////////////////////////////////////////////////////////////////
// .Description : Initialization of newly Instantiated Object
////////////////////////////////////////////////////////////////////////////////
void SVEquationClass::init()
{
	m_bUseOverlays = false;

	// Identify our output type
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVEquationObjectType;

	// Identify our input type needs - this is a bit different here
	// Since out inputs are dynamic via the script specified
	// So the input will be identified when the script is created.
	
	// SetObjectDepth() already called in SVObjectClass Ctor

	// Register Embedded Objects
	RegisterEmbeddedObject( &enabled, SVEquationEnabledObjectGuid, IDS_OBJECTNAME_ENABLED, false, SvOi::SVResetItemNone );

	// Set Embedded defaults
	enabled.SetDefaultValue( BOOL(true), true );

	// Set default inputs and outputs
	addDefaultInputObjects();

	// Set local defaults
	m_isDataValid = true;
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
bool SVEquationClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVTaskObjectClass::CreateObject(rCreateStructure) && nullptr != GetInspection();

	// Set / Reset Printable Flag
	enabled.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );

	SVObjectClass *owner = enabled.GetOwner();
	if (nullptr != owner)
	{
		SVString conditionalString = SvUl_SF::LoadSVString( IDS_CLASSNAME_SVCONDITIONAL );

		if( owner->GetName() == conditionalString )
		{
			// Set / Reset Remotely Setable Flag, if owner is conditional class.
			enabled.SetObjectAttributesAllowed( SvDef::SV_REMOTELY_SETABLE | SvDef::SV_SETABLE_ONLINE, SvOi::SetAttributeType::AddAttribute );
		}
	}

	m_isCreated = bOk;

	return bOk;
}

bool SVEquationClass::HasCondition()
{
	return !m_equationStruct.EquationBuffer.empty();
}

double SVEquationClass::GetYACCResult() const
{
	return m_Yacc.equationResult;
}

const SVString& SVEquationClass::GetEquationText() const
{
	return m_equationStruct.GetEquationText();
}

void SVEquationClass::SetEquationText(const SVString& rText)
{
	m_equationStruct.SetEquationText( rText );
	if( rText.empty() )
	{
		m_Symbols.ClearAll();
		m_Yacc.equationResult = 0.0;
	}
}

HRESULT SVEquationClass::GetObjectValue( const SVString& rValueName, _variant_t& rValue ) const
{
	HRESULT hr = S_OK;

	if( _T("EquationBuffer") == rValueName )
	{
		rValue.SetString( m_equationStruct.GetEquationText().c_str() );
	}
	else
	{
		hr = SVTaskObjectClass::GetObjectValue( rValueName, rValue );
	}

	return hr;
}

void SVEquationClass::Persist( SVObjectWriter& rWriter )
{
	SVTaskObjectClass::Persist(rWriter);
	
	// Get the Data Values (Member Info, Values)
	SVString Temp = m_equationStruct.EquationBuffer;
	
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
	bool bOk = false;

	SvCl::SVObjectSVStringArrayClass AttributeList;

	if ( ( bOk = pDataObject->GetAttributeData( _T("EquationBuffer"), AttributeList ) ) )
	{
		for( int i = 0;i < AttributeList.GetSize();i++ )
		{
			m_equationStruct.EquationBuffer = AttributeList.GetAt( i );

			SvUl::RemoveEscapedSpecialCharacters( m_equationStruct.EquationBuffer, true );
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
bool SVEquationClass::IsEnabled()
{
	BOOL bEnabled;
	enabled.GetValue( bEnabled );
		
	return bEnabled ? true : false;
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
SvOi::EquationTestResult SVEquationClass::Test( SvStl::MessageContainerVector *pErrorMessages/*=nullptr */ )
{
	SvOi::EquationTestResult ret;
	m_isDataValid = true;
	errContainer.clearMessage();

	if( HasCondition() && IsEnabled() )
	{
		
		SVOutputInfoListClass arInputAvailList;
		
		SVOutputInfoListClass toolSetPickList;
		
	
		// Clear the symbol Tables
		m_Symbols.ClearAll();
		m_Symbols.Init(this);
		

		SVString equationText = GetEquationText();
		equationText += "\n";
		// Note Buffer must be terminated with a new line
		// Build Lexical Stack
		SvOi::EquationTestResult result = lexicalScan( equationText.c_str() );
		if( result.bPassed )
		{
			ParseYacc();
	
			if (m_Yacc.yacc_err)
			{
				SVString fullObjectName = GetCompleteObjectNameToObjectType( nullptr, SVInspectionObjectType );
				SVStringVector msgList;
				msgList.push_back(fullObjectName);
				if( S_OK != m_Yacc.m_StatusCode )
				{
					errContainer.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_TooManyVariables, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10046, GetUniqueObjectID() );
				}
				else
				{
					ret.iPositionFailed = m_Yacc.lex_stack[m_Yacc.sIndex-1].position+1;
					msgList.push_back(SvUl_SF::Format(_T("%d"), ret.iPositionFailed));
					errContainer.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_EquationParserError, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10047, GetUniqueObjectID() );
				}
				m_isDataValid = false;
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
		
		m_isObjectValid.SetValue(BOOL(ret.bPassed));
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
	m_Lex.lex_err = 0;
	m_Lex.syntax_check = 1;
	m_Lex.currentPos = 0;
	m_Lex.position = 0;
	
	// Initialize the parser
	m_Yacc.yacc_err = 0;
	m_Yacc.sIndex = 0;

	// For lexical scanner
	struct yy_buffer_state* b;
	int yychar;
	int sIndex = 0;
	// Scan the input buffer
	b = m_Lex.yy_scan_string(inBuffer);

	// Find the Tokens and build the token stack for YACC
	while ((yychar = m_Lex.yylex()) > 0)
	{
		m_Yacc.lex_stack[sIndex].position = static_cast< int >( m_Lex.position );
		m_Yacc.lex_stack[sIndex].token = yychar;
		if( yychar == SV_IDENTIFIER )
			m_Yacc.lex_stack[sIndex].value.index = m_Lex.yylval.index;
		else
			m_Yacc.lex_stack[sIndex].value.val = m_Lex.yylval.val;
		sIndex++;
	}
		
	m_Yacc.lex_stack[sIndex].position = static_cast< int >( m_Lex.position );
	m_Yacc.lex_stack[sIndex].token = yychar;
	if( yychar == SV_IDENTIFIER )
		m_Yacc.lex_stack[sIndex].value.index = m_Lex.yylval.index;
	else
		m_Yacc.lex_stack[sIndex].value.val = m_Lex.yylval.val;
	sIndex++;

	m_Yacc.numTokens = sIndex;

	if (m_Lex.lex_err)
	{
		SVString fullObjectName = GetCompleteObjectNameToObjectType( nullptr, SVInspectionObjectType );
		ret.bPassed = false;
		ret.iPositionFailed = static_cast< int >( m_Lex.position + 1 );
		SVStringVector msgList;
		msgList.push_back(fullObjectName);
		msgList.push_back(SvUl_SF::Format(_T("%d"), ret.iPositionFailed));
		errContainer.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_EquationParserError, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10198, GetUniqueObjectID() );

		m_isDataValid = false;
	}
	// Release the buffer
	m_Lex.yy_delete_buffer(b);

	return ret;
}

double SVEquationClass::RunAndGetResult()
{
	m_isDataValid = true;

	ParseYacc();

	if (!m_isDataValid || m_Yacc.yacc_err)
	{
		return 0;
	}
	return m_Yacc.equationResult;
}

////////////////////////////////////////////////////////////////////////////////
// Rebuild Symbol table - Gets Called from lexer during scan of equation
////////////////////////////////////////////////////////////////////////////////
int SVEquationClass::AddSymbol( LPCTSTR name )
{
	int index = m_Symbols.AddSymbol( name, this );

	if( -1 == index )
	{
		m_isDataValid = false;
	}

	return index;
}

bool SVEquationClass::DisconnectObjectInput( SVInObjectInfoStruct* pInObjectInfo )
{
	// Update ToolSet Symbol table - Gets called when one of our inputs goes away
	if( pInObjectInfo )
	{
		SVInputInfoListClass& toolSetSymbols = m_Symbols.GetToolSetSymbolTable();

		for( int i = 0;i < toolSetSymbols.GetSize(); i++ )
		{
			SVInObjectInfoStruct* pSymbolInputObjectInfo = toolSetSymbols.GetAt( i );

			if( pSymbolInputObjectInfo )
			{
				if( pInObjectInfo->GetInputObjectInfo().m_UniqueObjectID == pSymbolInputObjectInfo->GetInputObjectInfo().m_UniqueObjectID )
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
// If Conditional is disabled equation.Run() returns always true.
// Otherwise the return value depends on the Conditional equation result!
bool SVEquationClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	bool retVal = __super::onRun( rRunStatus, pErrorMessages );

	if ( HasCondition() && IsEnabled() )
	{
		/////////////////////////////////////////////////////
		// The lexical Stack must be valid at this point
		// And the yacc parsing must not have any errors
		/////////////////////////////////////////////////////
		m_isDataValid = true;

		ParseYacc();

		/////////////////////////////////////////////////////
		// Check for Valid Data
		/////////////////////////////////////////////////////
		if( !m_isDataValid || m_Yacc.yacc_err )
		{
			retVal = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidData, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
			SetInvalid();
			rRunStatus.SetInvalid();
		}
	}
	if( retVal )
		rRunStatus.SetPassed();
	else
		rRunStatus.SetFailed();

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
		hr = m_Symbols.GetData( iSymbolIndex, value );
	}// end if

	//E_BOUNDS is return if a Sort value object is used and no value is set yet. In this case it should not set to invalid, but return 0.0
	if ( S_OK != hr && E_BOUNDS != hr )
	{
		m_isDataValid = false;
	}
	return value;
}

HRESULT SVEquationClass::GetArrayValues( int iSymbolIndex, std::vector< double >& values )
{
	HRESULT hr = S_FALSE;
	double value = 0.0;

	if( iSymbolIndex != -1 )
	{
		hr = m_Symbols.GetData( iSymbolIndex, values );
	}// end if

	if ( S_OK != hr )
	{
		m_isDataValid = false;
	}

	return hr;
}

double SVEquationClass::GetSubscriptedPropertyValue( int SymbolIndex, int Index, double Default /*= 0.0*/ )
{
	HRESULT hr = S_FALSE;
	double Value = Default;

	if( SymbolIndex != -1 )
	{
		hr = m_Symbols.GetData( SymbolIndex, Value, Index );
	}

	if ( S_OK != hr )
	{
		Value = Default;
	}

	return Value;
}

bool SVEquationClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	// call Test()...( Rebuilds symbol table !!! )
	if( HasCondition() && IsEnabled() )
	{
		//In reset object call test without displaying error messages is done using the returned result
		Result = Test( pErrorMessages).bPassed && Result;
	}

	return Result;
}

void SVEquationClass::OnObjectRenamed(const SVObjectClass& rRenamedObject, const SVString& rOldName)
{
	SVString newPrefix;
	SVString oldPrefix;

	SVObjectTypeEnum type = rRenamedObject.GetObjectType();
	if (SVInspectionObjectType == type)
	{
		newPrefix = _T(".") + rRenamedObject.GetCompleteObjectNameToObjectType(nullptr, SVInspectionObjectType) + _T(".");
	}
	else if (SVBasicValueObjectType == type)
	{
		newPrefix = _T("\"") + rRenamedObject.GetCompleteObjectNameToObjectType(nullptr, SVRootObjectType) + _T("\"");
	}
	else if (nullptr != dynamic_cast<const SvOi::IValueObject*> (&rRenamedObject))
	{
		newPrefix = _T("\"") + rRenamedObject.GetCompleteObjectNameToObjectType(nullptr, SVToolSetObjectType) + _T("\"");
	}
	else
	{
		newPrefix = _T("\"") + rRenamedObject.GetCompleteObjectNameToObjectType(nullptr, SVToolSetObjectType) + _T(".");
	}


	oldPrefix = newPrefix;
	SvUl_SF::searchAndReplace( oldPrefix, rRenamedObject.GetName(), rOldName.c_str() );

	SVString equationBuff = GetEquationText();

	// Replace all occurrences
	if( GetEquationText() != SvUl_SF::searchAndReplace( equationBuff, oldPrefix.c_str(), newPrefix.c_str() ) )
	{
		SetEquationText( equationBuff );
	}

	__super::OnObjectRenamed(rRenamedObject, rOldName);
}

void SVEquationClass::ParseYacc()
{
	m_Yacc.sIndex = 0;
	m_Yacc.yacc_err = 0;

	try
	{
		// Get the default control word.   
		int cw = _controlfp(0, 0);

		// This one turns off exception generation
		int newCw = cw | (EM_OVERFLOW | EM_UNDERFLOW | EM_INEXACT | EM_ZERODIVIDE | EM_DENORMAL);

		// Set the exception masks OFF, turn exceptions on.
		//int newCw = cw & (~(EM_OVERFLOW|EM_UNDERFLOW|EM_INEXACT|EM_ZERODIVIDE|EM_DENORMAL));

		// Set the control word.   
		_controlfp(newCw, MCW_EM);

		m_Yacc.yyparse();

		// Reset it back
		_controlfp(cw, MCW_EM);
	}

	catch (...)
	{
		_clearfp();

		m_isDataValid = false;
	}
}
