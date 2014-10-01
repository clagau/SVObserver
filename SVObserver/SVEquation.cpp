//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEquation.cpp
//* .File Name       : $Workfile:   SVEquation.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.7  $
//* .Check In Date   : $Date:   30 Sep 2014 15:37:34  $
//******************************************************************************

#include "stdafx.h"
#include <float.h>
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

SVEquationSymbolTableClass::SVEquationSymbolTableClass()
{
	SetAvailableLists( NULL, NULL );
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
// Set pointers to Available Lists
/////////////////////////////////////////////////////////////////
void SVEquationSymbolTableClass::SetAvailableLists(SVInputInfoListClass* PAvailInputSymbols, SVOutputInfoListClass* PAvailToolSetSymbols )
{
	m_pAvailInputSymbols = PAvailInputSymbols;
	m_pAvailToolSetSymbols = PAvailToolSetSymbols;

	ASSERT( m_pAvailToolSetSymbols == NULL || 0 < m_pAvailToolSetSymbols->GetSize() );
}

/////////////////////////////////////////////////////////////////
// Find a Symbol based on name
/////////////////////////////////////////////////////////////////
int SVEquationSymbolTableClass::FindSymbol( LPCTSTR name )
{
	int index = -1;
	int l_iCount = GetSize();

	for( int i = 0; i < l_iCount; i++ )
	{
		SVEquationSymbolStruct* pSymbolStruct = GetAt( i );
		if( pSymbolStruct->Type == SV_INPUT_SYMBOL_TYPE )
		{
			if( !_tcscmp(name, pSymbolStruct->Name ) )
			{
				index = i;
				break;
			}
		}
		else if ( pSymbolStruct->Type == SV_TOOLSET_SYMBOL_TYPE )
		{
			if( !_tcscmp(name, pSymbolStruct->Name ) )
			{
				index = i;
				break;
			}
		}
	}
	return index;
}

/////////////////////////////////////////////////////////////////
// Add a input (e.g. PPQ) or ToolSet Symbol based on name
/////////////////////////////////////////////////////////////////
int SVEquationSymbolTableClass::AddSymbol( LPCTSTR name, SVObjectClass* pRequestor )
{
	// Strip off Double Quotes
	CString nameStr = name;
	nameStr.Remove( _T( '\"' ) );

	// Find the name in the Input Available list (e.g. PPQ parameter or modes)
	int index = findInputSymbol( nameStr );

	if( index != -1 ) // found in Input Available List
	{
		index = addInputSymbol( nameStr, index );
	}
	else
	{
		// Find the name in the ToolSet Available list
		index = findToolSetSymbol( nameStr );

		if( index != -1 ) // found in Available ToolSet OutputList
		{
			index = addToolSetSymbol( nameStr, index, pRequestor );
		}
	}
	return index;
}

/////////////////////////////////////////////////////////////////
// Add a Input Symbol
/////////////////////////////////////////////////////////////////
int SVEquationSymbolTableClass::addInputSymbol( LPCTSTR name, int index )
{
	SVEquationSymbolStruct* pSymbolStruct;
	int symbolIndex = -1;

	if( m_pAvailInputSymbols )
	{
		SVInObjectInfoStruct* pObjInfo = m_pAvailInputSymbols->GetAt( index );
		if( pObjInfo )
		{
			pSymbolStruct = new SVEquationSymbolStruct();
			pSymbolStruct->Type = SV_INPUT_SYMBOL_TYPE;
			pSymbolStruct->Name = name;
			pSymbolStruct->IsValid = TRUE;
			pSymbolStruct->InObjectInfo.SetInputObject( pObjInfo->GetInputObjectInfo().PObject );

			// Add to combined symbol table if not already there
			symbolIndex = FindSymbol( name );

			if( symbolIndex == -1 )
				symbolIndex = Add( pSymbolStruct );

		}// end if

	}// end if
	return symbolIndex;
}

/////////////////////////////////////////////////////////////////
// Add a ToolSet Symbol
/////////////////////////////////////////////////////////////////
int SVEquationSymbolTableClass::addToolSetSymbol( LPCTSTR name, int index, SVObjectClass* pRequestor )
{
	SVEquationSymbolStruct* pSymbolStruct;
	int symbolIndex = -1;

	if( m_pAvailToolSetSymbols )
	{
		SVOutObjectInfoStruct* pOutObjectInfo = m_pAvailToolSetSymbols->GetAt( index );
		if( pOutObjectInfo )
		{
			// Add to combined symbol table if not already there
			symbolIndex = FindSymbol( name );

			if( symbolIndex == -1 )
			{

				pSymbolStruct = new SVEquationSymbolStruct();

				pSymbolStruct->Type = SV_TOOLSET_SYMBOL_TYPE;
				pSymbolStruct->Name = name;

				// Set who wants to use the variable
				pSymbolStruct->InObjectInfo.SetObject( pRequestor->GetObjectInfo() );

				// Set the variable to be used
				pSymbolStruct->InObjectInfo.SetInputObjectType( pOutObjectInfo->ObjectTypeInfo );
				pSymbolStruct->InObjectInfo.SetInputObject( pOutObjectInfo->UniqueObjectID );
			
				// Try to Connect at this point
				DWORD_PTR rc = ::SVSendMessage(pOutObjectInfo->UniqueObjectID, SVM_CONNECT_OBJECT_INPUT, reinterpret_cast<DWORD_PTR>(&pSymbolStruct->InObjectInfo), NULL);
				if( rc == SVMR_SUCCESS )
				{
					pSymbolStruct->IsValid = TRUE;
				}
				else
				{
					ASSERT(0);
				}
				symbolIndex = Add( pSymbolStruct );
				// add it to the top
				m_toolsetSymbolTable.Add( &pSymbolStruct->InObjectInfo );
			}
		}
	}
	return symbolIndex;
}

/////////////////////////////////////////////////////////////////
// Find a Input Symbol
/////////////////////////////////////////////////////////////////
int SVEquationSymbolTableClass::findInputSymbol( LPCTSTR name )
{
	int index = -1;
	SVInObjectInfoStruct *pObjInfo;

	if( m_pAvailInputSymbols )
	{
		// Find DataLinkInfo in Available List by name
		for( int i = m_pAvailInputSymbols->GetSize() - 1; i >= 0; i-- )
		{
			pObjInfo = m_pAvailInputSymbols->GetAt( i );

			//search for short name (used by PPQVariable)
			SVString temp = pObjInfo->GetOneBasedInputObjectShortName();
			if( temp == name )
			{
				index = i;
				break;
			}

			//search for name in the complete object name
			temp = pObjInfo->GetInputObjectInfo().GetObjectReference().GetCompleteObjectName();
			size_t position = temp.find( name );
			if( SVString::npos != position && position + strlen(name) >= temp.size() )
			{
				index = i;
				break;
			}
		}
	}
	return index;
}

/////////////////////////////////////////////////////////////////
// Find a Local Symbol
/////////////////////////////////////////////////////////////////
int SVEquationSymbolTableClass::findToolSetSymbol( LPCTSTR name )
{
	int index = -1;

	if( m_pAvailToolSetSymbols )
	{
		// Find the name in the ToolSet Available Output List
		int l_iCount = m_pAvailToolSetSymbols->GetSize();

		for( int i = l_iCount - 1; i >= 0; i-- )
		{
			SVOutObjectInfoStruct* pOutObjectInfo = m_pAvailToolSetSymbols->GetAt( i );
			if( pOutObjectInfo->PObject ) 
			{
				SVObjectClass* pObject = pOutObjectInfo->PObject;

				// Get the 'dotted' name including the Object Type name selected for the 'root'.
				CString csCompleteName = pObject->GetCompleteObjectNameToObjectType( NULL, SVToolSetObjectType );
				if( !_tcscmp( csCompleteName, name ) )
				{
					index = i;
					break;
				}
			}
		}
	}

	return index;
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

		if( pSymbolStruct->Type == SV_INPUT_SYMBOL_TYPE )
		{
			// Get the Data
			SVObjectClass *object = pSymbolStruct->InObjectInfo.GetInputObjectInfo().PObject;
			if( SV_IS_KIND_OF( object, SVValueObjectClass ) )
			{
				return ( static_cast <SVValueObjectClass*>(object) )->GetValue( value );
			}
			else if( SV_IS_KIND_OF( object, BasicValueObject ) )
			{
				return static_cast <BasicValueObject*>(object)->getValue( value );
			}
			else
				return S_FALSE;
		}
		else if( pSymbolStruct->Type == SV_TOOLSET_SYMBOL_TYPE )
		{
			// Get the Data
			if( pSymbolStruct->InObjectInfo.IsConnected() && pSymbolStruct->InObjectInfo.GetInputObjectInfo().PObject )
			{
				SVValueObjectClass* pValueObject = static_cast <SVValueObjectClass*>(pSymbolStruct->InObjectInfo.GetInputObjectInfo().PObject);
				return pValueObject->GetValue( value );
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
			if( SV_IS_KIND_OF( pSymbolStruct->InObjectInfo.GetInputObjectInfo().PObject, SVValueObjectClass ) )
			{
				SVValueObjectClass* pValueObject = static_cast<SVValueObjectClass*>(pSymbolStruct->InObjectInfo.GetInputObjectInfo().PObject);
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
			if( SV_IS_KIND_OF( pSymbolStruct->InObjectInfo.GetInputObjectInfo().PObject, SVValueObjectClass ) )
			{
				SVValueObjectClass* pValueObject = static_cast<SVValueObjectClass*>(pSymbolStruct->InObjectInfo.GetInputObjectInfo().PObject);
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

		if (0 == ownerName.Compare(conditionalString))
		{
			// Set / Reset Remotely Setable Flag, if owner is conditional class.
			enabled.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE;
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

////////////////////////////////////////////////////////////////////////////////
// .Title       : GetEquationStruct
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
SVEquationStruct* SVEquationClass::GetEquationStruct()
{
	return &equationStruct;
}

double SVEquationClass::GetYACCResult()
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
void SVEquationClass::GetEquationText(CString &text)
{
	text = equationStruct.EquationBuffer;
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
void SVEquationClass::SetEquationText(CString &text)
{
	equationStruct.EquationBuffer = text;
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

	SVObjectCStringArrayClass svCStringArray;

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
SVEquationTestResult SVEquationClass::Test()
{
	SVEquationTestResult ret;
	m_lCurrentRunIndex = -1;

	if( HasCondition() && IsEnabled() )
	{
		// *** // ***
		SVInputInfoListClass arInputAvailList;
		addPPQVariableToList(arInputAvailList);
		addOldPPQDigitizerVariableToList( arInputAvailList );
		addPPQ_XParameterToList(arInputAvailList);
		addEnvironmentModeParameterToList(arInputAvailList);

		// Get Available ToolSet Variables
		// Get Known ToolSet Outputs ( selectable for equation )
		SVTaskObjectClass* pToolSet = GetInspection()->GetToolSet();
		SVOutputInfoListClass l_OutputList;
		pToolSet->GetOutputList( l_OutputList );
		SVOutputInfoListClass toolSetPickList;
		l_OutputList.GetAllowedAttributesList( SV_SELECTABLE_FOR_EQUATION, &toolSetPickList );
	
		// Clear the symbol Tables
		symbols.ClearAll();
		symbols.SetAvailableLists( &arInputAvailList, &toolSetPickList );

		CString equationText;
		GetEquationText( equationText );
		equationText += "\n";
		// Note Buffer must be terminated with a new line
		LPTSTR pEquation = equationText.GetBuffer( equationText.GetLength() + 1 );
		// Build Lexical Stack
		SVEquationTestResult result = lexicalScan( pEquation );
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
				CString yaccErrorStr,tmp,fullObjectName;
				fullObjectName = GetCompleteObjectNameToObjectType( NULL, SVInspectionObjectType );
				if( yacc.m_StatusCode != S_OK )
				{
					yaccErrorStr.LoadString( IDS_TOO_MANY_VARIABLES );

					tmp.Format( _T( "%s \n%s" ), fullObjectName, yaccErrorStr );
				}
				else
				{
					ret.iPositionFailed = yacc.lex_stack[yacc.sIndex-1].position+1;
					yaccErrorStr.Format( _T( "Syntax Error at position %d\n" ), ret.iPositionFailed );
					tmp.Format(_T( "%s \n%s" ), fullObjectName, yaccErrorStr );
				}
				// Show Error
				ASSERT( FALSE );
				AfxMessageBox( tmp );
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
		
		// Clear the pointers to the Available Lists
		symbols.SetAvailableLists( NULL, NULL );

		// Free the memory for the input variables list
		long lSize = arInputAvailList.GetSize();
		for( long l = 0; l < lSize; l++ )
		{
			delete arInputAvailList[l];
		}// end for
		arInputAvailList.RemoveAll();
		
		isObjectValid.SetValue( 1, ret.bPassed );
	}
	// return true if no equation or disabled
	if( !HasCondition() || !IsEnabled() )
		ret.bPassed = true;

	return ret;
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
SVEquationTestResult SVEquationClass::lexicalScan(LPSTR inBuf)
{
	SVEquationTestResult ret;
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
		CString lexErrorStr,tmp,fullObjectName;
		fullObjectName = GetCompleteObjectNameToObjectType( NULL, SVInspectionObjectType );
		ret.bPassed = false;
		ret.iPositionFailed = static_cast< int >( lex.position + 1 );
		lexErrorStr.Format( _T( "Syntax Error at position %d\n" ), ret.iPositionFailed );
		tmp.Format(_T( "%s \n%s" ), fullObjectName, lexErrorStr );
		ASSERT( FALSE );
		AfxMessageBox( tmp );
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

	ASSERT( -1 < index );

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

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
BOOL SVEquationClass::renameToolSetSymbol(SVObjectClass* pObject, LPCTSTR orgName)
{
	CString newPrefix;
	CString oldPrefix;

	if( SVInspectionProcess* l_pInspection = dynamic_cast< SVInspectionProcess* >( pObject ) )
	{
		newPrefix = _T( "." ) + l_pInspection->GetCompleteObjectNameToObjectType( NULL, SVInspectionObjectType ) + _T( "." );
		oldPrefix = newPrefix;
		oldPrefix.Replace( l_pInspection->GetName(), orgName );
	}// end if
	else
	{
		newPrefix = _T( "\"" ) + pObject->GetCompleteObjectNameToObjectType( NULL, SVToolSetObjectType ) + _T( "." );
		oldPrefix = newPrefix;
		oldPrefix.Replace( pObject->GetName(), orgName );
	}// end else

	CString equationBuff;
	GetEquationText( equationBuff );

	// Replace all occurences
	if( equationBuff.Replace( oldPrefix, newPrefix ) )
	{
		SetEquationText( equationBuff );

		return TRUE;
	}
	return FALSE;
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

		SVString l_Name;

		l_Name.Format( _T( "Equation%d" ), i );

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
	BOOL retVal = TRUE;
	
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

	if ( hr != S_OK )
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
			HRESULT l_ResetStatus = ResetObject();
			if( l_ResetStatus != S_OK )
			{
				ASSERT( SUCCEEDED( l_ResetStatus ) );

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
		if( ! Test().bPassed )
		{
			CString tmp,fullObjectName;
			fullObjectName = GetCompleteObjectNameToObjectType( NULL, SVInspectionObjectType );
			tmp.LoadString(IDS_EQUATION_PARSER_ERROR);
			errStr.Format(_T( "%s \n %s" ), fullObjectName, tmp );

			l_hrOk = S_FALSE;
		}
	}

	return l_hrOk;
}

void SVEquationClass::addOldPPQDigitizerVariableToList( SVInputInfoListClass &arInputAvailList )
{
	SVPPQObject* pPPQ = GetInspection()->GetPPQ();

	// Check for old variables that were shared across the PPQ/Digitizer
	CString strEquation;
	GetEquationText( strEquation );
	CString strTemp = strEquation;
	CString strTempName = "";
	CString strName;
	strTemp.MakeUpper();

	int iWhere1 = -1;
	int iWhere2 = -1;
	int iWhere3 = -1;
	int iWhere4 = -1;
	int iWhere5 = -1;
	int iWhere6 = -1;
	bool bDone = false;
	while( !bDone )
	{
		bool bFound = false;

		iWhere1 = strTemp.Find( "VIPER QUAD", iWhere1 + 1 );
		if( !bFound && iWhere1 != -1 )
		{
			iWhere2 = strTemp.Find( ". ", iWhere1 );
			if( iWhere2 != -1 )
			{
				strName     = strEquation.Mid( iWhere1, iWhere2 - iWhere1 + 2 );
				strTempName = strEquation.Mid( iWhere1 + 17, iWhere2 - iWhere1 - 17 );

				bFound = TRUE;
			}// end if

		}// end if

		iWhere3 = strTemp.Find( "VIPER DUAL", iWhere3 + 1 );
		if( !bFound && iWhere3 != -1 )
		{
			iWhere4 = strTemp.Find( ". ", iWhere3 );
			if( iWhere4 != -1 )
			{
				strName     = strEquation.Mid( iWhere3, iWhere4 - iWhere3 + 2 );
				strTempName = strEquation.Mid( iWhere3 + 17, iWhere4 - iWhere3 - 17 );

				bFound = TRUE;
			}// end if

		}// end if

		iWhere5 = strTemp.Find( "VIPER RGB", iWhere5 + 1 );
		if( !bFound && iWhere5 != -1 )
		{
			iWhere6 = strTemp.Find( ". ", iWhere5 );
			if( iWhere6 != -1 )
			{
				strName     = strEquation.Mid( iWhere5, iWhere6 - iWhere5 + 2 );
				strTempName = strEquation.Mid( iWhere5 + 16, iWhere6 - iWhere5 - 16 );

				bFound = TRUE;
			}// end if
		}// end if

		if( bFound )
		{
			long lSize;
			// Right now an inspection can have only one PPQ
			pPPQ->GetInspectionCount( lSize );
			SVObjectClass* pObject = nullptr;
			SVInspectionProcess* pInspect = nullptr;
			for( long l = 0; l < lSize; l++ )
			{
				pPPQ->GetInspection( l, pInspect );
				pObject = reinterpret_cast<SVObjectClass *>(::SVSendMessage( pInspect->GetToolSet(), 
					( SVM_GET_OBJECT_BY_NAME | SVM_PARENT_TO_CHILD | SVM_NOTIFY_FRIENDS ) & ~SVM_NOTIFY_ONLY_THIS, 
					reinterpret_cast<DWORD_PTR>( static_cast<LPCSTR>( strTempName )), NULL ) );

				if( pObject != NULL )
					break;
			}// end for

			SVInObjectInfoStruct* pInInfoStruct = new SVInObjectInfoStruct;
			if( nullptr == pObject )
			{
				pInInfoStruct->SetInputObject( static_cast< LPCTSTR >( strName ) );
			}
			else
			{
				pInInfoStruct->SetInputObject( pObject );
			}

			arInputAvailList.Add( pInInfoStruct );
			continue;
		}// end if

		bDone = true;
	}// end while
}

void SVEquationClass::addPPQVariableToList( SVInputInfoListClass &arInputAvailList )
{
	SVIOEntryStruct pIOEntry;
	SVInspectionProcess *pInspection = GetInspection();
	for( size_t i = 0; pInspection && i < pInspection->m_PPQInputs.size(); i++ )
	{
		pIOEntry = pInspection->m_PPQInputs[i];

		if( !pIOEntry.m_IOEntryPtr->m_Enabled )
			continue;

		SVInObjectInfoStruct *pInInfoStruct = new SVInObjectInfoStruct;

		pInInfoStruct->SetInputObject( pIOEntry.m_IOEntryPtr->m_pValueObject );

		arInputAvailList.Add( pInInfoStruct );
	}// end for
}

void SVEquationClass::addPPQ_XParameterToList( SVInputInfoListClass &arInputAvailList )
{
	//add PPQ_X parameter to the equation selection list
	SVPPQObject *ppq = GetInspection()->GetPPQ();
	SVString PPQName = ppq->GetName();
	SVObjectReferenceVector ObjectList;
	SVObjectManagerClass::Instance().getTreeList( PPQName, ObjectList, SV_SELECTABLE_FOR_EQUATION );
	for(SVObjectReferenceVector::const_iterator iter = ObjectList.begin(); iter != ObjectList.end(); ++iter) 
	{
		SVInObjectInfoStruct *pInInfoStruct = new SVInObjectInfoStruct;
		pInInfoStruct->SetInputObject( iter->Object() );
		arInputAvailList.Add( pInInfoStruct );
	}
}

void SVEquationClass::addEnvironmentModeParameterToList( SVInputInfoListClass &arInputAvailList )
{
	SVObjectReferenceVector ObjectList;
	SVObjectManagerClass::Instance().getTreeList( Seidenader::SVObjectLibrary::FqnEnvironmentMode, ObjectList, SV_SELECTABLE_FOR_EQUATION );
	for(SVObjectReferenceVector::const_iterator iter = ObjectList.begin(); iter != ObjectList.end(); ++iter) 
	{
		SVInObjectInfoStruct *pInInfoStruct = new SVInObjectInfoStruct;
		pInInfoStruct->SetInputObject( iter->Object() );
		arInputAvailList.Add( pInInfoStruct );
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVEquation.cpp_v  $
 * 
 *    Rev 1.7   30 Sep 2014 15:37:34   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  925
 * SCR Title:  Add PPQ Items and SVObserver Modes to Equation Editor Object Selector
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed parameter name for SetAvailableLists method from PAvailPPQSymbols to PAvailInputSymbols.
 * Changed SV_PPQ_SYMBOL_TYPE to SV_INPUT_SYMBOL_TYPE in these methods:
 * FindSymbol,
 * addInputSymbol,
 * and GetData (multiple versions).
 * Changed member variable name from toolsetSymbolTable to m_toolsetSymbolTable in these methods:
 * ClearAll,
 * addToolSetSymbol,
 * and GetToolSetSymbolTable.
 * Changed member variable name from pAvailToolSetSymbols to m_pAvailableToolSetSymbols in these methods:
 * SetAvailableLists,
 * addToolSetSymbol,
 * and findToolSetSymbol.
 * Changed member variable name from pAvailPPQSymbols to m_pAvailInputSymbols in these methods:
 * SetAvailableLists,
 * addInputSymbol,
 * and findInputSymbol.
 * Changed method name from addPPQSymbol to addInputSymbol.
 * Changed method name from findPPQSymbol to findInputSymbol.
 * Added new methods:
 * addOldPPQDigitizerVariableToList,
 * addPPQVariableToList,
 * addPPQ_XParameterToList,
 * and addEnvironmentModeParameterToList.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   26 Sep 2014 12:48:44   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  946
 * SCR Title:  Make Conditional Page's Disable Equation Setting Writable from SVRC and SIAC
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed CreateObject method to set/reset Remotely Setable Flag, if owner is conditional class.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   15 May 2014 11:21:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised processMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   28 Feb 2014 11:28:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  890
 * SCR Title:  Fix SVObserver Memory Leaks
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   prevent leak of toolSet Symbol when already in list.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   01 Feb 2014 10:32:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed sendmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   01 Oct 2013 14:12:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 May 2013 12:16:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
      Rev 3.37   07 May 2013 11:02:48   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  832
   SCR Title:  Allow line breaks (cariage return line feed) in equations
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Changed flag to enable saving and loading control characters in equation text.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.36   02 May 2013 11:02:58   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  810
   SCR Title:  Remove support for Legacy Tools
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Updated process message method to fix formatting issues and updated the reset object section to only assert when the HRESULT is a failed status.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
 *    Rev 1.0   23 Apr 2013 10:27:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.35   21 Jan 2013 10:59:00   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  801
   SCR Title:  Add new Shift Tool to SVObserver
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Updated missing SetObjectValue and GetObjectValue methods based on 6.01 branch changes for the shift tool.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.34   10 Jan 2013 16:13:24   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  801
   SCR Title:  Add new Shift Tool to SVObserver
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Added missing log entries.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.33   07 Dec 2012 10:35:30   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  801
   SCR Title:  Add new Shift Tool to SVObserver
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     added new virtual methods for GetObjectValue and SetObjectValue
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.32   03 Dec 2012 09:42:54   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  800
   SCR Title:  Fix Blob Array Result index string
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Change symbol lookup to one based.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.31   04 Sep 2012 15:00:58   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  602
   SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to add new scripting and object creation/update funcitonality to the object.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.30.1.1   05 Dec 2012 11:53:06   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  801
   SCR Title:  Add new Shift Tool to SVObserver
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     added new virtual methods for GetObjectValue and SetObjectValue
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.30.1.0   30 Nov 2012 08:37:56   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  800
   SCR Title:  Fix Blob Array Result index string
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Change symbol lookup to one based.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.30   18 Jul 2012 13:30:58   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.29   12 Jul 2012 14:37:54   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.28   02 Jul 2012 16:52:04   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to promote new display functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.27   19 Jun 2012 12:43:30   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  744
   SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
   Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.26   01 Sep 2011 10:55:10   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  737
   SCR Title:  Fix serialization bug associated with control characters in strings
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Modified SVRemoveEscapeCharacters and SVAddEscapeCharacters.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.25   18 Apr 2011 09:29:32   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to move the shared data for the IO Entry data to a different class to fix issues with PPQ and Inspection shared data.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.24   27 Jan 2011 11:06:48   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix issues with Matrox Buffer Management.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.23   08 Dec 2010 08:05:42   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to include changes in notification functionality using the Observer Design Pattern.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.22   28 Oct 2010 13:44:38   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  704
   SCR Title:  Upgrade SVObserver Version for 5.33 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.21   01 Jun 2010 14:19:24   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to improve ability to track performance.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.20   16 Dec 2009 10:21:46   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  677
   SCR Title:  Fix problem in camera notify thread
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fix issues with includes and comments.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.19   20 Jun 2007 16:32:58   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  598
   SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
   Checked in by:  jSpila;  Joseph Spila
   Change Description:  
     These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
   
   Also updated Equation class to use the new SVEquationLibrary and override the functionality in SVEquationBase.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   20 Mar 2006 13:57:08   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  547
   SCR Title:  Fix Image overlay data
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Fixed overlays issue
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   21 Oct 2005 14:02:52   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed un-necessary setting of the IsConnected flag.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   20 Oct 2005 10:15:12   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  450
   SCR Title:  Add asynchronous functionality to the archive tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     fixed potential crash caused by C-style casts
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   08 Aug 2005 15:56:08   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated objects and dialogs to use the ActiveX Interface to update inspection data.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   29 Jul 2005 12:32:32   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   19 Jul 2005 14:30:20   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     added syntax for accessing an array element without specifying a default value (uses value object default value)
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   22 Jun 2005 08:53:32   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     implemented array value object equation interface
   added position information to test result
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   17 Feb 2005 14:03:46   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Update class with new methodologies based on new base object, extents and reset structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   16 Feb 2005 14:06:42   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  440
   SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     updated header files
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   02 Jun 2003 15:06:46   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  352
   SCR Title:  Update SVObserver to Version 4.30 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Checked in some preliminary beta code for the new methode of disabling tools and toolsets.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   22 Apr 2003 09:25:22   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   29 Jan 2003 15:42:36   joe
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated processMessage method to update image depth on a create all objects message.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   09 Dec 2002 19:19:26   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  295
   SCR Title:  Remove Result Data from Configuration Printout
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   19 Nov 2002 13:55:06   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  272
   SCR Title:  Outputs do not always get set correctly.
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     relates to how the equation classes connect and display information for Input/Output from the PPQ
   
   /////////////////////////////////////////////////////////////////////////////////////
*/