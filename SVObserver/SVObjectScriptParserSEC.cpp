//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectScriptParser
//* .File Name       : $Workfile:   SVObjectScriptParserSEC.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 15:24:38  $
//******************************************************************************

#include "stdafx.h"
#include "SVObjectScriptParserSEC.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVGlobal.h"
#include "SVTool.h"
#include "SVObserver.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVUserMessage.h"
#include "SVBarCodeAnalyzerClass.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace NS_ObjectScriptParserSEC
{

// Converts GUID into String {00000000-0000-0000-000000000000}
// If successfully it returns a TCHAR String which must be freed after using.
// Otherwise it returns NULL.
LPTSTR SVStringFromGUID( const GUID& RGUID )
{
	TCHAR szGUID[ 256 ];
	_stprintf( szGUID, _T( "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}" ),
			   RGUID.Data1, 
			   RGUID.Data2, 
			   RGUID.Data3,
			   RGUID.Data4[ 0 ], 
			   RGUID.Data4[ 1 ], 
			   RGUID.Data4[ 2 ], 
			   RGUID.Data4[ 3 ],
			   RGUID.Data4[ 4 ], 
			   RGUID.Data4[ 5 ], 
			   RGUID.Data4[ 6 ], 
			   RGUID.Data4[ 7 ]
			 );

	return _strdup( szGUID );
}

// Converts GUID into String {00000000-0000-0000-000000000000}
// CString& RString receives GUID String.
void SVStringFromGUID( CString& RString, const GUID& RGUID )
{
	RString.Format( _T( "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}" ),
					RGUID.Data1, 
					RGUID.Data2, 
					RGUID.Data3,
					RGUID.Data4[ 0 ], 
					RGUID.Data4[ 1 ], 
					RGUID.Data4[ 2 ], 
					RGUID.Data4[ 3 ],
					RGUID.Data4[ 4 ], 
					RGUID.Data4[ 5 ], 
					RGUID.Data4[ 6 ], 
					RGUID.Data4[ 7 ]
				  );
}



#define SV_PARSE_MESSAGE( XFORMATSTRING, XCOMMANDSTRING )	\
		{	\
			CString strMessage, strString;	\
			strString = ( XCOMMANDSTRING );	\
			int in = strString.Find( TCHAR( ';' ) );	\
			if( in > 0 )	strString = strString.Left( in + 1 );	\
			strMessage.Format( ( XFORMATSTRING ), strString );	\
			AfxMessageBox( strMessage );	\
		}

}// end namespace NS_ObjectScriptParserSEC


SVObjectScriptOperandStructSEC::SVObjectScriptOperandStructSEC( void* PVal, int Tpe, int Sze )
{
	PValue	= PVal;
	Type	= Tpe;
	ValueSize = Sze;
};


SVObjectScriptOperandStructSEC::SVObjectScriptOperandStructSEC( const SVObjectScriptOperandStructSEC& R2 )
{
	ValueSize	= R2.ValueSize;
	Type		= R2.Type;
	PValue		= NULL;

	//  Make a copy of the data
	if( ValueSize && R2.PValue )
	{
		void* newValue = malloc( ValueSize );
		memcpy( newValue, R2.PValue, ValueSize );

		PValue = newValue;
	}
}

SVObjectScriptOperandStructSEC& SVObjectScriptOperandStructSEC::operator=( const SVObjectScriptOperandStructSEC& RExp )
{
	ValueSize	= RExp.ValueSize;
	Type		= RExp.Type;

	//  Make a copy of the data
	if( ValueSize && RExp.PValue )
	{
		void* newValue = malloc( ValueSize );
		memcpy( newValue, RExp.PValue, ValueSize );

		if( PValue && RExp.PValue != PValue )
		{
			free( PValue );
			PValue = NULL;
		}
		PValue = newValue;
	}
	return *this;
}

void SVObjectScriptOperandStructSEC::CleanUp()
{
	if( PValue )
		free( PValue );
	
	PValue		= NULL;
	Type		= 0;
	ValueSize	= 0;
}

SVObjectScriptOperandStructSEC::~SVObjectScriptOperandStructSEC()
{
	CleanUp();
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



SVObjectScriptAliasListClassSEC::SVObjectScriptAliasListClassSEC()
{

}

void SVObjectScriptAliasListClassSEC::CleanUp()
{
	RemoveAll();
}

SVObjectScriptParserSECClass::SVObjectScriptParserSECClass(unsigned long parserHandle, CString* pScript, const GUID& ownerGuid, SVObjectClass* pOwnerObject, CWnd* pWnd) 
: SVObjectScriptParserBase(parserHandle, ownerGuid, pOwnerObject, pWnd)
, m_pParseString(pScript)
{
	keywordTable.Add( SVObjectScriptKeywordStructSEC( _T( "alias" ), ALIAS ) );
	keywordTable.Add( SVObjectScriptKeywordStructSEC( _T( "class$" ), CLASS ) );
	keywordTable.Add( SVObjectScriptKeywordStructSEC( _T( "object$" ), OBJECT ) );
	keywordTable.Add( SVObjectScriptKeywordStructSEC( _T( "embedded$" ), EMBEDDED ) );

	dataTypeKeywordTable.Add( SVObjectScriptDataTypeKeywordStructSEC( _T( "BOOL" ), SV_BOOL_Type ) );
	dataTypeKeywordTable.Add( SVObjectScriptDataTypeKeywordStructSEC( _T( "BYTE" ), SV_BYTE_Type ) );
	dataTypeKeywordTable.Add( SVObjectScriptDataTypeKeywordStructSEC( _T( "DOUBLE" ), SV_DOUBLE_Type ) );
	dataTypeKeywordTable.Add( SVObjectScriptDataTypeKeywordStructSEC( _T( "DWORD" ), SV_DWORD_Type ) );
	dataTypeKeywordTable.Add( SVObjectScriptDataTypeKeywordStructSEC( _T( "LONG" ), SV_LONG_Type ) );
	dataTypeKeywordTable.Add( SVObjectScriptDataTypeKeywordStructSEC( _T( "POINT" ), SV_POINT_Type ) );
	dataTypeKeywordTable.Add( SVObjectScriptDataTypeKeywordStructSEC( _T( "STRING" ), SV_STRING_Type ) );
	dataTypeKeywordTable.Add( SVObjectScriptDataTypeKeywordStructSEC( _T( "DPOINT" ), SV_DPOINT_Type ) );
	dataTypeKeywordTable.Add( SVObjectScriptDataTypeKeywordStructSEC( _T( "VARIANT" ), SV_VARIANT_Type ) );
}

SVObjectScriptParserSECClass::~SVObjectScriptParserSECClass()
{
	keywordTable.RemoveAll();
	dataTypeKeywordTable.RemoveAll();

	// Cleanup
	if( m_pParseString )
	{
		delete m_pParseString ;
		m_pParseString  = NULL;
	}
}

DWORD SVObjectScriptParserSECClass::tStrToDWORD( LPTSTR TStrInput )
{
	DWORD DwResult = 0;
	BOOL bIsHex = FALSE;
	for(; TStrInput; ++TStrInput )
	{
		switch( *TStrInput )
		{
			case TCHAR( '\0' ):
				return DwResult;

			case TCHAR( '\t' ):
				if( DwResult | bIsHex )
					return DwResult;
				continue;

			case TCHAR( ' ' ):
				if( DwResult | bIsHex )
					return DwResult;
				continue;
		
			case TCHAR( 'x' ):
			case TCHAR( 'X' ):
				if( DwResult | bIsHex )
					return DwResult;
				bIsHex = TRUE;
				continue;
		
			case TCHAR( '0' ):
				if( bIsHex )
					DwResult *= 16;
				else
					DwResult *= 10;
				DwResult += 0;
				continue;

			case TCHAR( '1' ):
				if( bIsHex )
					DwResult *= 16;
				else
					DwResult *= 10;
				DwResult += 1;
				continue;

			case TCHAR( '2' ):
				if( bIsHex )
					DwResult *= 16;
				else
					DwResult *= 10;
				DwResult += 2;
				continue;

			case TCHAR( '3' ):
				if( bIsHex )
					DwResult *= 16;
				else
					DwResult *= 10;
				DwResult += 3;
				continue;

			case TCHAR( '4' ):
				if( bIsHex )
					DwResult *= 16;
				else
					DwResult *= 10;
				DwResult += 4;
				continue;

			case TCHAR( '5' ):
				if( bIsHex )
					DwResult *= 16;
				else
					DwResult *= 10;
				DwResult += 5;
				continue;

			case TCHAR( '6' ):
				if( bIsHex )
					DwResult *= 16;
				else
					DwResult *= 10;
				DwResult += 6;
				continue;

			case TCHAR( '7' ):
				if( bIsHex )
					DwResult *= 16;
				else
					DwResult *= 10;
				DwResult += 7;
				continue;

			case TCHAR( '8' ):
				if( bIsHex )
					DwResult *= 16;
				else
					DwResult *= 10;
				DwResult += 8;
				continue;

			case TCHAR( '9' ):
				if( bIsHex )
					DwResult *= 16;
				else
					DwResult *= 10;
				DwResult += 9;
				continue;

			case TCHAR( 'a' ):
			case TCHAR( 'A' ):
				if( bIsHex )
					DwResult *= 16;
				else
					return DwResult;
				DwResult += 10;
				continue;

			case TCHAR( 'b' ):
			case TCHAR( 'B' ):
				if( bIsHex )
					DwResult *= 16;
				else
					return DwResult;
				DwResult += 11;
				continue;

			case TCHAR( 'c' ):
			case TCHAR( 'C' ):
				if( bIsHex )
					DwResult *= 16;
				else
					return DwResult;
				DwResult += 12;
				continue;

			case TCHAR( 'd' ):
			case TCHAR( 'D' ):
				if( bIsHex )
					DwResult *= 16;
				else
					return DwResult;
				DwResult += 13;
				continue;
		
			case TCHAR( 'e' ):
			case TCHAR( 'E' ):
				if( bIsHex )
					DwResult *= 16;
				else
					return DwResult;
				DwResult += 14;
				continue;
		
			case TCHAR( 'f' ):
			case TCHAR( 'F' ):
				if( bIsHex )
					DwResult *= 16;
				else
					return DwResult;
				DwResult += 15;
				continue;

			default:
				return DwResult;
		}
	}
	return DwResult;
}

double SVObjectScriptParserSECClass::tStrToDouble( LPTSTR TStrInput )
{
	double DoubleResult = 0;
	
	_stscanf( TStrInput, "%lf", &DoubleResult );

	return DoubleResult;
}

// Cut leading and successor whitespaces away...
LPTSTR SVObjectScriptParserSECClass::ExtractToken( LPCTSTR TStrTokenString, int TokenStringLength )
{
	LPTSTR tStrReturn = NULL;

	if( TStrTokenString )
	{
		tStrReturn = ( LPTSTR )malloc( TokenStringLength + 1 );
		if( tStrReturn )
		{
			memcpy( tStrReturn, TStrTokenString, TokenStringLength );
			tStrReturn[TokenStringLength] = TCHAR( '\0' );
		}
	}
	return tStrReturn;
}


BOOL SVObjectScriptParserSECClass::CheckAliasTable( const SVObjectScriptAliasListClassSEC& RCurrentAliasTable, LPCTSTR TStrTokenString )
{
	if( TStrTokenString )
	{
		// Check alias table...
		for( int i = 0; i < RCurrentAliasTable.GetSize(); ++ i )
		{
			SVObjectScriptAliasStructSEC& aliasEntry = RCurrentAliasTable.GetAt( i );
			if( ! _tcscmp( TStrTokenString, aliasEntry.TStrAlias ) )
			{
#ifdef SVOBJECTSCRIPTPARSER_DEBUG
				TRACE( "Alias Found: %.80s\n", TStrTokenString );
#endif
				return TRUE;
			}
		}
	}
#ifdef SVOBJECTSCRIPTPARSER_DEBUG
	TRACE( "Alias Not Found: %.80s\n", TStrTokenString );
#endif
	return FALSE;
}

BOOL SVObjectScriptParserSECClass::CheckAliasTable( const SVObjectScriptAliasListClassSEC& RCurrentAliasTable, LPCTSTR TStrTokenString, SVObjectScriptOperandStructSEC& ROperand)
{
	if( TStrTokenString )
	{
		// Check alias table...
		for( int i = 0; i < RCurrentAliasTable.GetSize(); ++ i )
		{
			SVObjectScriptAliasStructSEC& aliasEntry = RCurrentAliasTable.GetAt( i );

			if( ! _tcscmp( TStrTokenString, aliasEntry.TStrAlias ) )
			{
				ROperand = aliasEntry.SubstituteOperand;

#ifdef SVOBJECTSCRIPTPARSER_DEBUG
				TRACE( "Alias Found: %.80s\n", TStrTokenString );
#endif
				return TRUE;
			}
		}
	}
#ifdef SVOBJECTSCRIPTPARSER_DEBUG
	TRACE( "Alias Not Found: %.80s\n", TStrTokenString );
#endif

	return FALSE;
}

int SVObjectScriptParserSECClass::CheckKeywordTable( LPCTSTR TStrExpression )
{
	// Returns KeyIndex!!!
	if( TStrExpression )
	{
		// Check keyword table...
		for( int i = 0; i < keywordTable.GetSize(); ++ i )
		{
			SVObjectScriptKeywordStructSEC& keywordEntry = keywordTable.GetAt( i );
			if( ! _tcscmp( TStrExpression, keywordEntry.TStrKeyword ) )
			{

#ifdef SVOBJECTSCRIPTPARSER_DEBUG
				TRACE( "Keyword Found: %.80s\n", TStrExpression );
#endif
				return keywordEntry.KeyIndex;
			}
		}
	}

#ifdef SVOBJECTSCRIPTPARSER_DEBUG
	TRACE( "Keyword Not Found: %.80s\n", TStrExpression );
#endif

	return NOP;
}

void SVObjectScriptParserSECClass::CleanUpExpressionStack( SVExpressionStackSEC& RExpressionStack )
{
	for( int i = 0; i < RExpressionStack.GetSize(); ++ i )
	{
		if( RExpressionStack.GetAt( i ) )
		{
			free( RExpressionStack.GetAt( i ) );
			RExpressionStack.SetAt( i, NULL );
		}
	}

	RExpressionStack.RemoveAll();
}

// Extract GUID Token...
//  GUID-Token: e.g. { 0x2ddd26a9, 0x79a7, 0x11d3, { 0xa8, 0x90, 0x0, 0x12, 0x0, 0x0, 0x5, 0xaa } }
//				or	 {2DDD26A9-79A7-11D3-A890-0012000005AA}
//
// Note: Converts { 0x2ddd26a9, 0x79a7, 0x11d3, { 0xa8, 0x90, 0x0, 0x12, 0x0, 0x0, 0x5, 0xaa } }
//		 to       {2DDD26A9-79A7-11D3-A890-0012000005AA}
LPTSTR SVObjectScriptParserSECClass::ExtractGUIDToken( LPCTSTR TStrTokenString, int& RCount )
{
	LPTSTR tStrReturn = NULL;
	if( TStrTokenString && *TStrTokenString == TCHAR( '{' ) )
	{
		// find final }...
		LPCTSTR tStrFinal = _tcschr( TStrTokenString, TCHAR( '}' ) );
		if( tStrFinal )
		{
			BOOL bCType = FALSE;
			// Check, if "{ 0x2ddd26a9," Type or "{2DDD26A9-" Type...
			LPCTSTR tStrComma = _tcschr( TStrTokenString, TCHAR( ',' ) );
			if( tStrComma && tStrComma < tStrFinal )
			{
				// Assume "{ 0x2ddd26a9," Type !!!
				bCType = TRUE;
				tStrFinal = _tcschr( tStrFinal + 1, TCHAR( '}' ) );
			}

			if( tStrFinal )
			{
				RCount = static_cast<int>(tStrFinal - TStrTokenString + 1);

				if( bCType )
				{
					// Convert to "{2DDD26A9-" Type...
					
					TCHAR szResult[ _MAX_PATH ];
					_tcscpy( szResult, _T( "{" ) );

					TCHAR szTemp[ _MAX_PATH ];
					_tcsncpy( szTemp, TStrTokenString, RCount );
					szTemp[ RCount ] = TCHAR( '\0' );

					LPTSTR szToken;
					TCHAR szSep[] = _T( ",{}xX " );
					TCHAR szFill[] = _T( "00000000000000000000" );
					int dataCount = 0;
					szToken = _tcstok( szTemp, szSep );
					while( szToken )
					{
						// Make upper case...
						_tcsupr( szToken );
						int len = static_cast<int>(_tcslen( szToken ));
						switch( dataCount )
						{
							case 1:
							{
								if( len < 8 )
									_tcsncat( szResult, szFill, 8 - len );

								_tcscat( szResult, szToken );

								_tcscat( szResult, _T( "-" ) );
							}
							break;

							case 3:
							case 5:
							{
								if( len < 4 )
									_tcsncat( szResult, szFill, 4 - len );

								_tcscat( szResult, szToken );

								_tcscat( szResult, _T( "-" ) );
							}
							break;

							case 7:
							case 11:
							case 13:
							case 15:
							case 17:
							case 19:
							case 21:
							{
								if( len < 2 )
									_tcsncat( szResult, szFill, 2 - len );

								_tcscat( szResult, szToken );
							}
							break;

							case 9:
							{
								if( len < 2 )
									_tcsncat( szResult, szFill, 2 - len );

								_tcscat( szResult, szToken );

								_tcscat( szResult, _T( "-" ) );
							}
							break;
						}

						++dataCount;
						szToken = _tcstok( NULL, szSep );
					}

					_tcscat( szResult, _T( "}" ) );
					tStrReturn = _tcsdup( szResult );
				}
				else
				{
					// Duplicate token...
					if( tStrReturn = ( LPTSTR ) malloc( sizeof( TCHAR ) * ( RCount + 1 ) ) )
					{
						_tcsncpy( tStrReturn, TStrTokenString, RCount );
						tStrReturn[ RCount ] = TCHAR( '\0' );
					}
				}
			}
		}
	}
	return tStrReturn;
}

// Expectes $ as one Expression and GUID as one Expression on stack ( generated using ExtractGUIDToken!!! )
// NOTE: GUID Expression must be a {2DDD26A9-79A7-11D3-A890-0012000005AA} Type 
//		 ( refer to StringFromGUID2() Win-SDK Function )
BOOL SVObjectScriptParserSECClass::ReadGUIDExpression( SVExpressionStackSEC& RExpressionStack, GUID& RUniqueID, int& RIndex )
{

#ifdef SVOBJECTSCRIPTPARSER_DEBUG
	TRACE( "ReadGUIDExpression: " );

	TRACE( "%.80s", RExpressionStack.GetAt( RIndex ) );
#endif

	LPCTSTR ptr = RExpressionStack.GetAt( RIndex );
	if( ptr[0] == TCHAR( '{' ) && ptr[1] != TCHAR( '\0' ) )
	{
		// Convert the GUID String Expression to a Guid...
		AfxGetClassIDFromString( RExpressionStack.GetAt( RIndex ), &RUniqueID );
		return TRUE;
	}

	TRACE( "ReadGUIDExpression: Failed\n" );
	return FALSE;
}


BOOL SVObjectScriptParserSECClass::EvaluateOperandExpression( int OperandType, const SVObjectInfoStruct& ROwnerInfo, const SVObjectScriptAliasListClassSEC& RCurrentAliasTable, SVObjectScriptOperandStructSEC& ROperand, SVExpressionStackSEC& RExpressionStack, int& RIndex )
{
	// Operands are identified by
	// name_expression
	// object_expression		...could use local owner to identify
	// object_expression . object_expression [ . object_expression [...] ]

	BOOL bExpressionNext = TRUE;
	for( ; RIndex < RExpressionStack.GetSize(); ++ RIndex )
	{
		int type = NOP;
		// only look for keyword when appropriate
		if( RIndex == 0 || RIndex == 3)
			type = CheckKeywordTable( RExpressionStack.GetAt( RIndex ) );

		// Check for name_expression...
		if( ( OperandType & NAME ) == NAME )
		{	
			if( type != NOP )
			{
				AfxMessageBox( "Parser Error:\nName expression expected, valid names must not contain keywords!" );
				return FALSE;
			}

			if( bExpressionNext )
			{
				ROperand.CleanUp();

				ROperand.PValue = _tcsdup( RExpressionStack.GetAt( RIndex ) );
				if( ROperand.PValue )
				{
					ROperand.ValueSize = static_cast<int>(_tcslen( RExpressionStack.GetAt( RIndex ) ) + 1);
					ROperand.Type = NAME;
					// Success...
					return TRUE;
				}
				
				AfxMessageBox( "Parser Error:\nOut of memory!" );
				return FALSE;
			}

			AfxMessageBox( "Parser Error:\nName expression expected!" );
			return FALSE;
		}

		// Check for access operator . 
		if( ( OperandType & MEMBER_ACCESS ) == MEMBER_ACCESS && ! _tcscmp( RExpressionStack.GetAt( RIndex ), _T( "." ) ) )
		{
			if( ! bExpressionNext )
			{
				SVObjectInfoStruct ownerInfo;
				if( ROperand.PValue && ( ROperand.Type == OBJECT || ROperand.Type == OBJECT_OWNER || ROperand.Type == EMBEDDED_OBJECT || ROperand.Type == OBJECT_INPUT ) )
				{
					ownerInfo.UniqueObjectID	= *( ( GUID* ) ROperand.PValue );
					ownerInfo.PObject			= SVObjectManagerClass::Instance().GetObject( ownerInfo.UniqueObjectID );
					// Complete info, if object available...
					if( ownerInfo.PObject )
						ownerInfo = ownerInfo.PObject->GetObjectInfo();
					else
					{
						AfxMessageBox( "Parser Error:\nOwner doesn't exist, cannot access member!" );
						return FALSE;
					}

					// No alias allowed for member access operands...
					SVObjectScriptAliasListClassSEC emptyAliasTable;
					// Try to evaluate operand...

#ifdef SVOBJECTSCRIPTPARSER_DEBUG
					TRACE( "EvaluateOperandExpression, Bingo member access\n" );
#endif

					OperandType &= (~ALIAS);					
					return EvaluateOperandExpression( OperandType, ownerInfo, emptyAliasTable, ROperand, RExpressionStack, ++ RIndex );
				}
			}
			AfxMessageBox( "Parser Error:\nUnexpected member access operator!" );
			return FALSE;
		}
		else
			if( ! bExpressionNext )
			{
				// Success...
				--RIndex;
				return TRUE;	
			}

			
		// Check for first part of object_expression|class_expression|embedded_expression...
		if( type != NOP &&
			( ( OperandType & OBJECT )		== type ||
			  ( OperandType & EMBEDDED )	== type ||
			  ( OperandType & CLASS )		== type
			)
		  )
		{	
			if( bExpressionNext )
			{
				ROperand.CleanUp();

				ROperand.ValueSize = sizeof( GUID );
				ROperand.PValue = malloc( sizeof( GUID ) );
				if( ROperand.PValue )
				{
					ROperand.Type = type;
					if( ReadGUIDExpression( RExpressionStack, *( ( GUID* ) ROperand.PValue ), ++RIndex ) )
					{
						if( type == CLASS || type == EMBEDDED )
						{
							// Success...
							return TRUE;
						}

						bExpressionNext = FALSE;
						continue;
					}
					ROperand.CleanUp();

					AfxMessageBox( "Parser Error:\nCannot read GUID!" );
					return FALSE;
				}

				AfxMessageBox( "Parser Error:\nOut of memory!" );
				return FALSE;
			}
			else
			{
				AfxMessageBox( "Parser Error:\nExpression expected!" );
				return FALSE;
			}
		}

		// Check for known object...
		if( type == NOP )
		{	
			if( bExpressionNext )
			{
				// ...using owner info...
				SVObjectClass* pObject = NULL;
				if( ROwnerInfo.UniqueObjectID != SVInvalidGUID )
					pObject = ( SVObjectClass* ) ::SVSendMessage( ROwnerInfo.UniqueObjectID, SVM_GET_OBJECT_BY_NAME, ( DWORD ) RExpressionStack.GetAt( RIndex ), NULL );
				if( SV_IS_KIND_OF( pObject, SVObjectClass ) )
				{
					ROperand.CleanUp();
				
					ROperand.ValueSize = sizeof( GUID );
					ROperand.PValue = malloc( sizeof( GUID ) );
					if( ROperand.PValue )
					{
						memcpy( ROperand.PValue, &pObject->GetUniqueObjectID(), sizeof( GUID ) );
						ROperand.Type = OBJECT;

#ifdef SVOBJECTSCRIPTPARSER_DEBUG
						TRACE( "EvaluateOperandExpression, Bingo using owner info\n" );
#endif
						bExpressionNext = FALSE;
						continue;
					}
					
					ROperand.CleanUp();

					AfxMessageBox( "Parser Error:\nOut of memory!" );
					return FALSE;
				}
				else
				{
					// Not Known
#ifdef SVOBJECTSCRIPTPARSER_DEBUG
					TRACE("Unknown Object\n");
#endif
					
					// Check for Unknown member access
					if( ( OperandType & ( MEMBER_ACCESS | ALIAS) ) == MEMBER_ACCESS )
					{
#ifdef SVOBJECTSCRIPTPARSER_DEBUG
						TRACE("Assuming Member Name\n");
#endif

						ROperand.CleanUp();

						ROperand.PValue = _tcsdup( RExpressionStack.GetAt( RIndex ) );
						if( ROperand.PValue )
						{
							ROperand.ValueSize = static_cast<int>(_tcslen( RExpressionStack.GetAt( RIndex ) ) + 1);
							ROperand.Type = MEMBER_NAME;

							// Success...
							return TRUE;
						}
					}
				}
			}
			else
			{
				AfxMessageBox( "Parser Error:\nExpression expected!" );
				return FALSE;
			}
		}

		// Check for alias...
		if( ( OperandType & ALIAS ) == ALIAS && type == NOP )
		{	
			if( bExpressionNext )
			{
				// ...using alias table...
				SVObjectScriptOperandStructSEC AliasExp;

				// *** // ***
				// The old code used the string for "Long Result" for Bar Code Result
				// This messes up the Alias table. Replace it to remove the issue.
				CString strName;
				LPSTR strAlias;
				BOOL bAlreadyChecked;
				BOOL bOk;

				bAlreadyChecked = FALSE;
				strName.LoadString( IDS_CLASSNAME_RESULT_LONG );
				if( _tcscmp( (LPTSTR) RExpressionStack.GetAt( RIndex ), strName ) == 0 &&
					SV_IS_KIND_OF( ROwnerInfo.PObject, SVBarCodeAnalyzerClass )  )
				{
					strName.LoadString( IDS_CLASSNAME_SVBARCODEANALYZERESULT );
					strAlias = _tcsdup( strName );
					if( CheckAliasTable( RCurrentAliasTable, strAlias, AliasExp ) )
					{
						RExpressionStack.SetAt( RIndex, strAlias );
						bAlreadyChecked = TRUE;
						bOk				= TRUE;
					}// end if
					else
					{
						SVObjectScriptOperandStructSEC operand2;
						operand2.ValueSize = sizeof( GUID );
						operand2.PValue = malloc( sizeof( GUID ) );
						(*(GUID*)operand2.PValue) = SVBarCodeResultClassGuid;
						operand2.Type = CLASS;

						((SVObjectScriptAliasListClassSEC*)&RCurrentAliasTable)->Add( SVObjectScriptAliasStructSEC( strName, operand2 ) );

						bOk = CheckAliasTable( RCurrentAliasTable, strAlias, AliasExp );
						bAlreadyChecked = TRUE;
					}// end if
				}// end if

				if( !bAlreadyChecked )
					bOk = CheckAliasTable( RCurrentAliasTable, RExpressionStack.GetAt( RIndex ), AliasExp );

				if(  bOk && AliasExp.PValue && 
					( ( OperandType & OBJECT )	 == AliasExp.Type ||
					  ( OperandType & EMBEDDED ) == AliasExp.Type ||
					  ( OperandType & CLASS )	 == AliasExp.Type
					)
				  )
				{
					// substitute operand...
					ROperand = AliasExp;
						
					if( ROperand.PValue )
					{
#ifdef SVOBJECTSCRIPTPARSER_DEBUG
						TRACE( "EvaluateOperandExpression, Bingo using alias table\n" );
#endif

						bExpressionNext = FALSE;
						continue;
					}
					ROperand.CleanUp();

					AfxMessageBox( "Parser Error:\nOut of memory!" );
					return FALSE;
				}
				else // must be member name ???
				{
					int x = 777;
				}
			}
			else
			{
				AfxMessageBox( "Parser Error:\nExpression expected!" );
				return FALSE;
			}
		}

		AfxMessageBox( "Parser Error:\nUnknown expression!" );
		return FALSE;
	}
	
	AfxMessageBox( "Parser Error:\nUnexpected end of command!" );
	return FALSE;
}


BOOL SVObjectScriptParserSECClass::ReadOperandList( const SVObjectInfoStruct& RLocalOwnerInfo, const SVObjectScriptAliasListClassSEC& RCurrentAliasTable, SVObjectScriptOperandListSEC& ROperandList, SVExpressionStackSEC& RExpressionStack, int& RIndex )
{
	// Input object operands are identified by
	// object_expression
	// object_expression . object_expression [ . object_expression [...] ]

	int evaluateType = MEMBER_ACCESS | OBJECT;

	// Operand lists are identified by
	// ( Operand1, Operand2, ..., Operandn ) or ()

	// Check current index...
	if( RIndex >= RExpressionStack.GetSize() )
		return FALSE;

	// Check for list begin...
	if( _tcscmp( RExpressionStack.GetAt( RIndex ), _T( "(" ) ) )
		return FALSE;

	// Check current index...
	if( ++RIndex >= RExpressionStack.GetSize() )
		return FALSE;

	// Check for empty list...
	if( ! _tcscmp( RExpressionStack.GetAt( RIndex ), _T( ")" ) ) )
		return TRUE;

	// Read operand...
	SVObjectScriptOperandStructSEC operand;

	// Read operand list...
	BOOL bOperandNext = TRUE;
	for( ; RIndex < RExpressionStack.GetSize() && _tcscmp( RExpressionStack.GetAt( RIndex ), _T( ")" ) ); ++ RIndex )
	{
		if( bOperandNext )
		{
			// Check for object$ keyword
			if( !_tcscmp( RExpressionStack.GetAt( RIndex++ ), _T( "object$" ) ) )
			{
				operand.CleanUp();

				operand.ValueSize = sizeof( GUID );
				operand.PValue = malloc( sizeof( GUID ) );
				if( operand.PValue )
				{
					operand.Type = OBJECT_INPUT;
			
					// check for guid
					if( ReadGUIDExpression( RExpressionStack, *( ( GUID* )operand.PValue ), RIndex ) )
					{
						ROperandList.Add( operand );
						bOperandNext = FALSE;
					}
				}
			}
			else
			{
				operand.CleanUp();
				return FALSE;
			}
		}
		else
		{
			// Read separator ,
			if( _tcscmp( RExpressionStack.GetAt( RIndex ), _T( "," ) ) )
				return FALSE;

			bOperandNext = TRUE;
		}
	}

	return ! bOperandNext;
}

BOOL SVObjectScriptParserSECClass::ReadValues( SVObjectAttributeClass& dataObject, SVExpressionStackSEC& RExpressionStack, int& RIndex )
{
	BOOL rc = FALSE;

	// Check current index...
	if( RIndex >= RExpressionStack.GetSize() )
		return rc;

	// Check for value(s) begin...
	dataObject.SetType( CheckDataTypeKeywordTable( RExpressionStack.GetAt( RIndex ) ) );
	// Should we have specified a type
	if( dataObject.GetSVObjectScriptDataObjectTypeEnum() == SV_UNKNOWN_Type )
		return rc;

	//ASSERT( dataObject.GetSVObjectScriptDataObjectTypeEnum() != SV_VARIANT_Type );	// DEBUG break on Variant

	// Check current index...
	if( ++RIndex >= RExpressionStack.GetSize() )
		return rc;

	// Read value(s)...
	
	if(  !_tcscmp( RExpressionStack.GetAt( RIndex ), _T( "[" ) ) )
	{
		RIndex++;

		for( ; RIndex < RExpressionStack.GetSize() && _tcscmp( RExpressionStack.GetAt( RIndex ), _T( ";" ) ); ++ RIndex )
		{
			if (   dataObject.GetSVObjectScriptDataObjectTypeEnum() == SV_STRING_Type
				|| dataObject.GetSVObjectScriptDataObjectTypeEnum() == SV_VARIANT_Type)	//????
			{
				if( _tcscmp( RExpressionStack.GetAt( RIndex ), _T( "," ) ) &&
					  _tcscmp( RExpressionStack.GetAt( RIndex ), _T( "]" ) ) )
				{
					// it's our string !
					if( ! ExtractValue( dataObject, RExpressionStack, RIndex ) )
					{
						rc = FALSE;
						break;
					}
					rc = TRUE;
				}
			}

			else 
			{
				if( CheckNumber( RExpressionStack.GetAt( RIndex ) ) )
				{
					if( ! ExtractValue( dataObject, RExpressionStack, RIndex ) )
					{
						rc = FALSE;
						break;
					}

					rc = TRUE;
				}
				else
				{
					// Check for end array or separator
					// Read separator ,
					if( _tcscmp( RExpressionStack.GetAt( RIndex ), _T( "," ) ) &&
						  _tcscmp( RExpressionStack.GetAt( RIndex ), _T( "]" ) ) )
					{
						rc = FALSE;
						break;
					}
				}
			}
		}
	}
	else // single value
	{
		for( ; RIndex < RExpressionStack.GetSize() && _tcscmp( RExpressionStack.GetAt( RIndex ), _T( ";" ) ); ++ RIndex )
		{
			if( CheckValue( dataObject, RExpressionStack.GetAt( RIndex ) ) )
			{
				if(! ExtractValue( dataObject, RExpressionStack, RIndex ))
				{
					rc = FALSE;
					break;
				}
				rc = TRUE;
			}
			else
			{
				break;
			}
		}
	}
	return rc;
}

BOOL SVObjectScriptParserSECClass::ExtractValue( SVObjectAttributeClass& dataObject, 
                                              SVExpressionStackSEC& RExpressionStack, 
                                              int& RIndex )
{
	BOOL bOk = FALSE;

	// convert String to Number
	switch ( dataObject.GetSVObjectScriptDataObjectTypeEnum() )
	{
		case SV_BOOL_Type:
		{
			BOOL boolValue;

			_stscanf( RExpressionStack.GetAt( RIndex ), "%d", &boolValue );

			bOk = dataObject.AddData( boolValue );

			break;
		}

		case SV_DWORD_Type:
		{
			DWORD dwordValue;

			_stscanf( RExpressionStack.GetAt( RIndex ), "%x", &dwordValue );

			bOk = dataObject.AddData( dwordValue );

			break;
		}

		case SV_LONG_Type:
		{
			long longValue;

			_stscanf( RExpressionStack.GetAt( RIndex ), "%d", &longValue );

			bOk = dataObject.AddData( longValue );

			break;
		}

		case SV_DOUBLE_Type:
		{
			double doubleValue;
			LPCTSTR doubleStr = RExpressionStack.GetAt( RIndex );

			if ( _tcsstr( doubleStr, _T( "#INF" ) ) )
			{
				if(doubleStr[0] == _T( '-' ) )
					doubleValue = -HUGE_VAL;
				else
					doubleValue = HUGE_VAL;
			}
			else if ( _tcsstr( doubleStr, _T( "#IND" ) ) )
			{
				__int64 *val = (__int64 * )&doubleValue;

				if(doubleStr[0] == _T( '-' ) )
					*val = 0xfff8000000000000;
				else
					*val = 0x7ff8000000000000;
			}
			// Set it to NAN
			// We don't check for NAN explicitly due to
			// conflicts in the documentation!
			else if ( _tcsstr( doubleStr, _T( "#" ) ) )
			{
				__int64 *val = (__int64 * )&doubleValue;

				if(doubleStr[0] == _T( '-' ) )
					*val = 0xffffffffffffffff;
				else
					*val = 0x7fffffffffffffff;
			}
			else
			{
				_stscanf( doubleStr, "%lf", &doubleValue );
			}

			bOk = dataObject.AddData( doubleValue );

			break;
		}// end case SV_DOUBLE_Type:

		case SV_POINT_Type:
		{
			POINT pointValue;
			_stscanf( RExpressionStack.GetAt( RIndex++ ), "%d", &pointValue.x );
			// Next expression on stack is a comma!!!
			RIndex++;
			_stscanf( RExpressionStack.GetAt( RIndex ), "%d", &pointValue.y );

			bOk = dataObject.AddData( pointValue );

			break;
		}

		case SV_DPOINT_Type:
		{
			SVDPointClass pointValue;
			_stscanf( RExpressionStack.GetAt( RIndex++ ), "%lf", &pointValue.x );
			// Next expression on stack is a comma!!!
			RIndex++;
			_stscanf( RExpressionStack.GetAt( RIndex ), "%lf", &pointValue.y );

			bOk = dataObject.AddData( pointValue );

			break;
		}

		case SV_BYTE_Type:
		{
			BYTE byteValue;
			int value = 0;

			// Bytes are scripted in HEX !!! 
			// BugFix, RO_23Mar2000

			// BugFix, SEJ 19May2000
			// Only SVByteVectorObject BYTEs are scripted in HEX !!! 
			// SVByteValueObject values are scripted in decimal as well as
			// the bDefault variable of the SVByteVectorObject!!!
			LPTSTR expPtr = RExpressionStack.GetAt( RIndex );
			
			if( expPtr )
			{
				// check for hex prefix
				if( _tcsstr( expPtr, _T( "0x" ) ) )
				{
					_stscanf( expPtr, "%x", &value );
				}
				else
				{
					// it's decimal
					_stscanf( expPtr, "%d", &value );
				}
			}

			byteValue = (BYTE)value;

			bOk = dataObject.AddData( byteValue );

			break;
		}// end case SV_BYTE_Type:

		case SV_STRING_Type:
		{
			CString stringValue;

			stringValue = RExpressionStack.GetAt( RIndex );

			bOk = dataObject.AddData( stringValue );

			break;
		}

		case SV_VARIANT_Type:
		{
			_variant_t v;

			long lvt;

			_stscanf( RExpressionStack.GetAt( RIndex++ ), "%d", &lvt );

			VARTYPE l_VarType = static_cast<VARTYPE>(lvt);

			if ( l_VarType == VT_EMPTY )
			{
				RIndex++;	// skip comma
			}
			else
			{
				// parse value
				RIndex++;	// skip comma
				CString strValue;
				strValue = RExpressionStack.GetAt( RIndex );
				_variant_t vStringValue;
				vStringValue.SetString( strValue );
				HRESULT hr = ::VariantChangeTypeEx(&v, &vStringValue, 0x0409, VARIANT_ALPHABOOL, l_VarType);		// use United States locale
				ASSERT( hr == S_OK );
			}

			bOk = dataObject.AddData( v );
			break;
		}// end case SV_VARIANT_Type:

		default:	// Unknown data type
		{
			break;
		}
	}// end switch ( dataObject.GetSVObjectScriptDataObjectTypeEnum() )

	return bOk;
}

BOOL SVObjectScriptParserSECClass::CheckValue( SVObjectAttributeClass& dataObject, LPCTSTR tokenStr )
{
	BOOL rc = (    dataObject.GetSVObjectScriptDataObjectTypeEnum() == SV_STRING_Type
				|| dataObject.GetSVObjectScriptDataObjectTypeEnum() == SV_VARIANT_Type );

	if( !rc )
	 return CheckNumber( tokenStr );
		
	return rc;
}

/* Do not use - Delimiters have been removed already !!! */
BOOL SVObjectScriptParserSECClass::CheckString( LPCTSTR tokenStr )
{
	int len = static_cast<int>(_tcslen( tokenStr ));
	
	// check for double quote at beginning
	// check for double quote at end
	if( len > 1 )
	{
		if( tokenStr[0] == TCHAR( '"' ) && tokenStr[len - 1] == TCHAR( '"' ) )
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL SVObjectScriptParserSECClass::CheckNumber( LPCTSTR tokenStr )
{
	int len = static_cast<int>(_tcslen( tokenStr ));
	BOOL decimalPoint = FALSE;
	BOOL minusSign = FALSE;
	
	CString infinityStr = _T( "#INF" );
	CString quietNanStr = _T( "#IND" );

	// We don't check for NAN explicitly due to
	// conflicts in the documentation!
	CString nanStr = _T( "#" ); //_T( "#NAN" );

	// check for hex 0x at beginning
	if( len > 2 )
	{
		if( tokenStr[0] == TCHAR( '0' ) && tokenStr[1] == TCHAR( 'x' ) )
		{
			// hexadecimal number
			for (int i = 2;i < len;i++ )
			{
				if (!_istdigit( tokenStr[i] ) && !_istxdigit( tokenStr[i] ) )
					return FALSE;
			}
			return TRUE;
		}
	}

	// Decimal/floating point numbers
	for (int i = 0;i < len;i++ )
	{
		if (!_istdigit( tokenStr[i] ) )
		{
			if( tokenStr[i] == TCHAR( '.' )  && !decimalPoint )
			{
				decimalPoint = TRUE;
				continue;
			}
			// check for minus sign
			if( tokenStr[i] == TCHAR( '-' ) && !minusSign && !decimalPoint )
			{
				minusSign = TRUE;
				continue;
			}

			if( decimalPoint )
			{
				// Check for #INF , #IND, or # ( NAN or QNAN ? )
				if( len -  i >= nanStr.GetLength() &&
					( !_tcsncmp( &tokenStr[i], infinityStr, infinityStr.GetLength() ) ||
					  !_tcsncmp( &tokenStr[i], quietNanStr, quietNanStr.GetLength() ) ||
					  !_tcsncmp( &tokenStr[i], nanStr, nanStr.GetLength() ) ) )
					return TRUE;
			}
			return FALSE;
		}
	}
	return TRUE;
}

SVObjectScriptDataObjectTypeEnum SVObjectScriptParserSECClass::CheckDataTypeKeywordTable( LPCTSTR TStrExpression )
{
	// Check keyword table...
	if( TStrExpression )
	{
		for( int i = 0; i < dataTypeKeywordTable.GetSize(); ++ i )
		{
			SVObjectScriptDataTypeKeywordStructSEC& dataTypeKeywordEntry = dataTypeKeywordTable.GetAt( i );
			if( ! _tcscmp( TStrExpression, dataTypeKeywordEntry.TStrKeyword ) )
			{
				return dataTypeKeywordEntry.KeyType;
			}
		}
	}
	return SV_UNKNOWN_Type;
}

BOOL SVObjectScriptParserSECClass::FindMembers( LPTSTR tokenStr, int tokenStrLen, SVExpressionStackSEC& RExpressionStack )
{
	BOOL found = FALSE;
	CString tmp;

	tmp.GetBuffer(512);
	tmp.ReleaseBuffer();

	// Decimal/floating point numbers
	for (int i = 0;i < tokenStrLen;i++ )
	{
		if( tokenStr[i] == TCHAR( '.' ) )
		{
			found = TRUE;
			// get expression to the left of the period
			// push name expression onto expression stack
			RExpressionStack.Add( _tcsdup( tmp ) );

			tmp.Empty();

			tmp.GetBuffer(512);
			tmp.ReleaseBuffer();

			// push period (separator) onto stack
			RExpressionStack.Add( _tcsdup( _T( "." ) ) );
		}
		else 
		{
			tmp += tokenStr[i];
		}
	}
	if( found ) // don't forget the last name (rhs)
	{
		RExpressionStack.Add( _tcsdup( tmp ) );
	}
	return found;
}

////////////////////////////////////////////////////
// The main parse method
////////////////////////////////////////////////////
LPCTSTR SVObjectScriptParserSECClass::Parse( SVObjectClass* POwner, LPCTSTR TStrParseString, SVObjectScriptAliasListClassSEC* PExternalAliasTable /*= NULL*/ )
{
	int currentParseState = PARSE;
	int currentTokenIndex = 0;
	LPTSTR tStrParse = (LPTSTR)TStrParseString;

	if( tStrParse )
	{
		SVExpressionStackSEC	expressionStack;

		// Force allocation of memory...
		// RDS DOES NOTHING
		// expressionStack.SetSize( 50 );
		// expressionStack.SetSize( 0 );

		SVObjectInfoStruct localOwnerInfo;
		localOwnerInfo = POwner;
		SVObjectScriptAliasListClassSEC localAliasTable;

		int parseLength = static_cast<int>(_tcslen( tStrParse ));

		while( currentParseState != PARSER_ERROR && currentParseState != PARSE_END && 
				currentTokenIndex < parseLength )
		{
			UpdateProgress( currentTokenIndex, parseLength );

			if( currentParseState == PARSE )
			{
				parseStatements( tStrParse, parseLength, currentTokenIndex, expressionStack, localOwnerInfo, localAliasTable, currentParseState );
			}
			
			UpdateProgress( currentTokenIndex, parseLength );

			if( currentParseState == PROCESS_COMMAND )
			{
				if( expressionStack.GetSize() < 1 )
				{
#ifdef SVOBJECTSCRIPTPARSER_DEBUG
					TRACE("PROCESS_COMMAND = Empty Expression stack\n");
#endif
					currentParseState = PARSER_ERROR;
					continue;
				}

				if( ! _tcscmp( expressionStack.GetAt( 0 ), _T( "}" ) ) )
				{
					// step out...
					currentParseState = PARSE_END;
					break;
				}

				////////////////////////////////////////////////
				// Now we have an expression list
				////////////////////////////////////////////////

				////////////////////////////////////////////////
				// Build current alias table...
				////////////////////////////////////////////////
				SVObjectScriptAliasListClassSEC currentAliasTable;
				currentAliasTable.Copy( localAliasTable );
				if( PExternalAliasTable )
					currentAliasTable.Append( *PExternalAliasTable );

				///////////////////////////////////////////////////////////////////////////////////////////
				// Expressions:
				// class_expression			class$GUID
				// object_expression		object$GUID
				// embedded_expression		embedded$GUID
				// name_expression			name	
				// string_expression		"Name: \"Hans\""
				// value					decimal | hex | double | string

				// Known Commands:
				// Alias:
				// alias name_Expression = embedded_expression|class_expression|object_expression;
				// Object_Define:
				// [ embedded_expression ] class_expression name_Expression = object_expression[@object_expression][([object_expression [,object_expression[,...]]])] [;]|[{ ... }];
				// Object_Redefine:
				// object_expression[@object_expression][([object_expression [,object_expression[,...]]])] [;]|[{ ... }][;]
				// name_Expression[@object_expression][([object_expression [,object_expression[,...]]])] [;]|[{ ... }][;]
				// Assignment:
				// object_expression = value;
				// name_Expression = value;
				// Assignment to trivial members:
				// name_Expression.trivial_member = value[[value,value,...]];
				// object_expression.trivial_member = value;

				// Check for alias command...
				if( CheckKeywordTable( expressionStack.GetAt( 0 ) ) == ALIAS )
				{
					if( !processAlias( expressionStack, localOwnerInfo, currentAliasTable, localAliasTable ) )
						currentParseState = PARSER_ERROR;
					else
						currentParseState = PARSE;

					CleanUpExpressionStack( expressionStack );
					continue;				
				} // Check for alias command...

				// Check other commands...

				// Check first operand...
				SVObjectScriptOperandStructSEC firstOperand;
				int index = 0;
				if( EvaluateOperandExpression( ALIAS | OBJECT | CLASS | EMBEDDED | MEMBER_ACCESS, localOwnerInfo, currentAliasTable, firstOperand, expressionStack, index ) && 
					firstOperand.PValue
				  )
				{
					// Check for Object_Define command...
					// must be ...
					// alias for embedded_expression|class_expression or
					// embedded_expression|class_expression
					if( firstOperand.Type == CLASS || firstOperand.Type == EMBEDDED )
					{
						BOOL bStepIn = FALSE;
						SVObjectClass* pObject;
						pObject = processDefineObject( expressionStack, localOwnerInfo, currentAliasTable, firstOperand, bStepIn );
						if(! pObject )
						{
							currentParseState = PARSER_ERROR;
							TRACE("Failed to Create Object-{%08X}\n",*(DWORD*)firstOperand.PValue);
						}
						else
						{
							if( bStepIn )
							{
								// step in...
								LPCTSTR tStrNewParsPos = Parse( pObject, &tStrParse[ currentTokenIndex ] , &currentAliasTable );
								if( ! tStrNewParsPos  )
								{
									SV_FORMAT_MESSAGE( "%s parse was not successful", pObject->GetName() );
									currentParseState = PARSER_ERROR;
								}
								else
								{
									currentTokenIndex = static_cast<int>(tStrNewParsPos - tStrParse);

									currentParseState = PARSE;
								}
							}
							else
								currentParseState = PARSE;
						}
						CleanUpExpressionStack( expressionStack );
						continue;				
					} // Check for Object_Define command...

					// Assignment to trivial members:
					// must be ...
					// name_Expression.trivial_member = value[[value,value,...]];
					// object_expression.trivial_member = value;
					if( firstOperand.Type == MEMBER_NAME )
					{
						if( !processMemberAssignment( expressionStack, localOwnerInfo, currentAliasTable, firstOperand ) )
						{
							currentParseState = PARSER_ERROR;
							TRACE("Failed to Set Object.Member value\n");
						}
						else
							currentParseState = PARSE;

						CleanUpExpressionStack( expressionStack );
						continue;				
					} // Check for Object.Member Assignment
				
				} // Check first operand...


				// unknown command...
				TRACE("Unknown Command\n");
				currentParseState = PARSER_ERROR;

			} // PROCESS_COMMAND
	
		}	// while

		// Clean up local resources...
		CleanUpExpressionStack( expressionStack );
		localAliasTable.CleanUp();

		UpdateProgress( currentTokenIndex, parseLength );
	}

	if( currentParseState == PARSE_END )
	{
		// Success
		return TStrParseString + currentTokenIndex;
	}
	return NULL;
}

void SVObjectScriptParserSECClass::RemoveExternalDependencies( CString& script )
{
	int startIndex = 0;
	int pos;
	int len;
	CString keyWord = _T( "object$" );

	SVVector< CString > objectGuidStrList;
	SVVector< SVObjectScriptInputObjectTokenStructSEC > inputObjectGuidList;
	SVVector< CString > scriptStack;

	CString tmpStr;
	CString guidStr;
	CString endGuidIdentifier = _T( " } }" );

	while( ( pos = script.Find( keyWord, startIndex ) ) != -1 )
	{
		// add this portion of the script to the stack
		len = pos - startIndex;
		tmpStr = script.Mid( startIndex, len );
		scriptStack.Add( tmpStr );

		// skip white space and find an opening parenthese
		// sets the location of the first non space character
		int loc = pos - 1;
		int endGuidPos;

		// skip white space and find an opening parenthese
		if( findCharacters( script, loc, _T( " " ), true ) )
			loc--;

		// Identify input Object GUID or just Object GUID
		// Build a list of requested Object GUIDs
		if( script[loc] == _T( '=' ) )
		{
			// it's a define

			// Add the object$ to the script stack
			scriptStack.Add( keyWord );

			// Build the list of known Object GUIDs
			
			// Find the ending braces
			loc = static_cast<int>(pos + _tcsclen( keyWord ));
			if( ( endGuidPos = script.Find( endGuidIdentifier, loc ) ) != -1 )
			{
				endGuidPos += endGuidIdentifier.GetLength();
				len = endGuidPos - loc;
			
				guidStr = script.Mid( loc, len );
				objectGuidStrList.Add( guidStr );

				// Add the Guid to the Script Stack
				scriptStack.Add( guidStr );
			}
		}
		else if( script[loc] == _T( '(' ) || script[loc] == _T( ',' ) )
		{
			// it's a reference to an object GUID (input)

			// Add the object$ to the script stack
			scriptStack.Add( keyWord );
			
			// Find the ending braces
			loc = static_cast<int>(pos + _tcsclen( keyWord ));
			if( ( endGuidPos = script.Find( endGuidIdentifier, loc ) ) != -1 )
			{
				endGuidPos += endGuidIdentifier.GetLength();

				len = endGuidPos - loc;

				guidStr = script.Mid( loc, len );
				
				// Add the Guid to the Script Stack
				int index = scriptStack.Add( guidStr );

				// Save it's Guid and Location in the Script Stack
				SVObjectScriptInputObjectTokenStructSEC token;
				token.GuidStr = guidStr;
				token.Index = index;
				inputObjectGuidList.Add( token );
			}
		}
		else
		{
			// really bad !!!
			AfxMessageBox("Critical Error removing external dependencies");
		}
		startIndex = endGuidPos;
	}
	// get the remaining portion of the script
	len = script.GetLength() - startIndex;
	tmpStr = script.Mid( startIndex, len );
	scriptStack.Add( tmpStr );

	// Resolve Input Object GUIDs to known GUIDs
	for( int i = 0;i < inputObjectGuidList.GetSize(); i++ )
	{
		bool found = false;

		SVObjectScriptInputObjectTokenStructSEC& inputObjectToken = inputObjectGuidList.GetAt(i);
		for( int j = 0;j < objectGuidStrList.GetSize(); j++ )
		{
			guidStr = objectGuidStrList.GetAt( j );
			if( !guidStr.Compare( inputObjectToken.GuidStr ) )
			{
				found = true;
			}
		}
		if( !found )
		{
			// Set the inputObjectGUID to SVInvalidGuid
			// Build the Invalid Guid String
			TCHAR* tGuidStr;
			SV_GUID_TO_NEW_STRING( SVInvalidGUID, tGuidStr );
		
			guidStr = tGuidStr;
			scriptStack.SetAt( inputObjectToken.Index, guidStr );
			free( tGuidStr );
		}
	}
	
	// re-build the script
	if( scriptStack.GetSize() )
	{
		script.Empty();

		for( int i = 0;i < scriptStack.GetSize(); i++ )
		{
			script += scriptStack.GetAt( i );
		}
	}
}


// returns when one of the specified characters is not found
// Sets loc to the location of the last encountered character
bool SVObjectScriptParserSECClass::findCharacters(CString& str, int& loc, LPCTSTR characters, bool reverse )
{
	return findCharacters((LPCTSTR)str, str.GetLength(), loc, characters, reverse );
}

// returns when one of the specified characters is not found
// Sets loc to the location of the last encountered character
bool SVObjectScriptParserSECClass::findCharacters(LPCTSTR tStr, int tStrLen, int& loc, LPCTSTR characters, bool reverse )
{
	bool found = false;
	
	int numChars = static_cast<int>(_tcsclen( characters ));

	if( reverse )
	{
		for( int i = loc; i >= 0; i-- )
		{
			bool rc = false;

			for( int n = 0; n < numChars; n++ )
			{
				if( tStr[i] == characters[n] )
				{
					rc = true;
					found = true;
					loc = i;
					break;
				}
			}
			if ( !rc )
				break;
		}
	}
	else
	{
		for( int i = loc; i < tStrLen; i++ )
		{
			bool rc = false;

			for( int n = 0; n < numChars; n++ )
			{
				if( tStr[i] == characters[n] )
				{
					rc = true;
					found = true;
					loc = i;
					break;
				}
			}
			if ( !rc )
				break;
		}
	}
	
	return found;
}

bool SVObjectScriptParserSECClass::reAttachInputs( SVObjectClass* pObject, SVObjectScriptOperandListSEC& RInputOperandList )
{
	bool l_bOk = true;

	//static SVFunctionProfiler profile(_T("SVObjectScriptParserSVXClass::reAttachInputs"));
	//SVFunctionProfilerLocal profiler(profile);
	SVInputInfoListClass inputInfoList;

	::SVSendMessage(pObject, SVM_GET_INPUT_INTERFACE, ( DWORD )&inputInfoList, NULL );

	// Input List and requiredInputList must be the same size
	// and In the same Order !!!
	long l_lOperandSize = RInputOperandList.GetSize();
	long l_lInfoSize = inputInfoList.GetSize();

	long l_lOffset = 0;

	SVToolClass* l_psvTool = dynamic_cast<SVToolClass*>( pObject );

	SVInObjectInfoStruct* l_pAuxInfo = &( l_psvTool->m_AuxSourceImageObjectInfo );

	// reattach inputs
	for( int i = 0; l_bOk && i < l_lOperandSize; i++ )
	{
		SVObjectScriptOperandStructSEC& operand = RInputOperandList.GetAt( i );

		// Check if valid GUID type
		if( operand.ValueSize == sizeof( GUID ) && *( ( GUID* ) operand.PValue ) != SVInvalidGUID )
		{
			SVGUID inputGuid = *( ( GUID* ) operand.PValue );

			SVInObjectInfoStruct* pInInfo = NULL;

			if( l_lOperandSize == l_lInfoSize )
			{
				pInInfo = inputInfoList.GetAt( i );
			}
			else
			{
				if( l_psvTool != NULL && i + l_lOffset < l_lInfoSize )
				{
					do
					{
						pInInfo = inputInfoList.GetAt( i + l_lOffset );

						if( TheSVObserverApp.DwCurrentLoadingVersion < 0x00044B00 && pInInfo == l_pAuxInfo )
						{
							pInInfo = NULL;

							l_lOffset++;
						}
					}
					while( pInInfo == NULL && i + l_lOffset < l_lInfoSize );
				}
				else
				{
					l_bOk = false;
				}
			}

			if( pInInfo != NULL )
			{
				pInInfo->SetInputObject( inputGuid );
			}
			else
			{
				TRACE("Could not find - Object Input is not Valid\n");

				l_bOk = false;
			}
		}
		else // Invalid GUID operand
		{
			TRACE("Object Input is not a Valid GUID type\n");

			l_bOk = false;
		}
	}

	return l_bOk;
}


///////////////////////////////////////////////////////////////
// Find and parse Statements - delimited by new line
///////////////////////////////////////////////////////////////
bool SVObjectScriptParserSECClass::parseStatements( LPCTSTR ParseStr, int parseLength, int& currentIndex, SVExpressionStackSEC& RExpressionStack, SVObjectInfoStruct& RLocalOwnerInfo, SVObjectScriptAliasListClassSEC& RAliasTable, int& RParseState )
{
	int statementLen = 0;
	int tokenIndex = 0;
	int tokenSize = 0;

	LPCTSTR tStatement;
	LPCTSTR tStrCurrentToken;

	// Get a Statement
	tStatement = &ParseStr[currentIndex];
	statementLen = parseLength - currentIndex;

	if( tStatement && statementLen )
	{
		TCHAR currentChar;
		tStrCurrentToken = tStatement;

		while( RParseState == PARSE && tokenIndex < statementLen )
		{
			// Find a token
			// look for keywords, name strings, strings and delimiters
			currentChar = tStatement[tokenIndex++];
			tokenSize++;

			if( currentChar != TCHAR( '\0' ) )
			{
				// Check separators...
				switch( currentChar )
				{
					case TCHAR( ';' ):	// Process Command
					case TCHAR( '{' ):	// Process Command
					{
						if( tokenSize > 1 )
						{
							// Put current token on the stack...
							LPTSTR tStrTok = ExtractToken( tStrCurrentToken, tokenSize-1 );
							if( tStrTok )
							{
								// If there was an expression, put it on the stack...
								if( _tcslen( tStrTok ) > 0 )
								{
									RExpressionStack.Add( tStrTok );

#ifdef SVOBJECTSCRIPTPARSER_DEBUG
									TRACE( "Add Expression: %.80s\n", tStrTok );
#endif
								}
								else
									free( tStrTok );
							}
							else
							{
								// error
								TRACE("ExtractToken (1) Failed \n");
								RParseState = PARSER_ERROR;
							}
						}
						// Put current separator on the stack...
						TCHAR tStrSep[ 2 ];
							
						tStrSep[ 0 ] = currentChar;
						tStrSep[ 1 ] = TCHAR( '\0' );

						RExpressionStack.Add( _tcsdup( tStrSep ) );

						// Stop parsing and start processing command, if you know
						//	this is a complete command terminated by ; ( semicolon )
						//	this is a object definition and we are entering the
						//		object's local definition block { .... };
						int index;
							
						if( currentChar == TCHAR( ';' ) ||
							currentChar == TCHAR( '{' ) ||
							( index = static_cast<int>(RExpressionStack.GetUpperBound()) ) >= 0 && 
							RExpressionStack.GetAt( index ) && 
							*RExpressionStack.GetAt( index ) == TCHAR( ')' ) 
						  )
						{
							// Process Command And Parse Object Content
							RParseState = PROCESS_COMMAND;
						}
						// point to next possible token
						tStrCurrentToken = &tStatement[tokenIndex];
						tokenSize = 0;
					}
					break;

					case TCHAR( '\"' ):	// Read String SubstituteOperand
					{
						RParseState = READ_STRING;

						if( getStringToken( RExpressionStack, tStatement, statementLen, tokenIndex ) )
						{
							RParseState = PARSE;
						}
						else
						{
							// error
							TRACE("ExtractToken (2) Failed \n");
							RParseState = PARSER_ERROR;
						}
						// point to next possible token
						tStrCurrentToken = &tStatement[tokenIndex];
						tokenSize = 0;
					}
					break;

					case TCHAR( '\'' ): // start delimiter for names
					{
						RParseState = READ_NAME_STRING;

						if( getNameStringToken( RExpressionStack, tStatement, statementLen, tokenIndex ) )
						{
							RParseState = PARSE;
						}
						else
						{
							// error
							TRACE("ExtractToken (3) Failed \n");
							RParseState = PARSER_ERROR;
						}
						// point to next possible token
						tStrCurrentToken = &tStatement[tokenIndex];
						tokenSize = 0;
					}
					break;
						
					case TCHAR( ' ' ):	
					{
						// Check for keyword and alias expression...
						// If keyword or alias expression, put the current token on the stack,
						// otherwise skip
						if( tokenSize > 1 )
						{
							LPTSTR tStrTok = ExtractToken( tStrCurrentToken, tokenSize-1 );
							if( tStrTok )
							{
								// If there was an expression, ckeck it...
								if( _tcslen( tStrTok ) > 0 )
								{
									RExpressionStack.Add( tStrTok );
								}
								else // empty string
									free( tStrTok );
							}
							else
							{
								// error
								TRACE("ExtractToken (4) Failed \n");
								RParseState = PARSER_ERROR;
							}
						}
						// point to next possible token
						tStrCurrentToken = &tStatement[tokenIndex];
						tokenSize = 0;
					}
					break;

					case TCHAR( '$' ):	// Read Guid 
					{
						// Get prior token ( class , object, embedded )

						// Get Current Token
						LPTSTR tStrTok = ExtractToken( tStrCurrentToken, tokenSize );
						if( tStrTok )
						{
							// If there was an expression, ckeck it...
							if( _tcslen( tStrTok ) > 0 )
							{
								// If there was an expression, put it on the stack...
								RExpressionStack.Add( tStrTok );

#ifdef SVOBJECTSCRIPTPARSER_DEBUG
								TRACE( "Add Expression: %.80s\n", tStrTok );
#endif

								if( getGuidStringToken( RExpressionStack, tStatement, statementLen, tokenIndex ) )
								{
									RParseState = PARSE;
								}
								else
								{
									RParseState = PARSER_ERROR;
								}
							}
							else // empty string
								free( tStrTok );
						}
						else
						{
							// error
							TRACE("ExtractToken (6) Failed \n");
							RParseState = PARSER_ERROR;
						}
						// point to next possible token
						tStrCurrentToken = &tStatement[tokenIndex];
						tokenSize = 0;
					}
					break;

					case TCHAR( '}' ):	// Parse Object Content Or Evaluate Bracket Content ( GUID )
					case TCHAR( '(' ):	// Read Input Objects Or Evaluate Bracket Content
					case TCHAR( ',' ):	// Parameter List
					case TCHAR( ')' ):	// Read Input Objects Or Evaluate Bracket Content
					case TCHAR( '=' ):	// Init Or Assign
					case TCHAR( '@' ):	// Read Owner
					case TCHAR( '[' ):	// Array of Values - starting delimiter
					case TCHAR( ']' ):	// Array of Values - ending delimiter
					{
						// pick up any residual
						if( tokenSize > 1 )
						{
							LPTSTR tStrTok = ExtractToken( tStrCurrentToken, tokenSize - 1 );
							if( tStrTok )
							{
								// If there was an expression, ckeck it...
								if( _tcslen( tStrTok ) > 0 )
								{
									RExpressionStack.Add( tStrTok );
								}
								else // empty string
									free( tStrTok );
							}
							else
							{
								// error
								TRACE("ExtractToken (7) Failed \n");
								RParseState = PARSER_ERROR;
							}
						}
						// Put current separator on the stack...
						TCHAR tStrSep[ 2 ];

						tStrSep[ 0 ] = currentChar;
						tStrSep[ 1 ] = TCHAR( '\0' );

						RExpressionStack.Add( _tcsdup( tStrSep ) );
							
						// point to next possible token
						tStrCurrentToken = &tStatement[tokenIndex];
						tokenSize = 0;
					}
					break;

					case TCHAR( '\t' ):
					case TCHAR( '\n' ):
					{
						// point to next possible token
						tStrCurrentToken = &tStatement[tokenIndex];
						tokenSize = 0;
					}
					break;

					default:
					break;
				}
			}
		}
	}
	if( RParseState == PARSER_ERROR )
		return false;

	currentIndex += tokenIndex;

	return true;
}

///////////////////////////////////////////////////////////////
// Extract the String Token
///////////////////////////////////////////////////////////////
bool SVObjectScriptParserSECClass::getStringToken( SVExpressionStackSEC& RExpressionStack, LPCTSTR tStatement, int statementLen, int& tokenIndex )
{
	LPCTSTR tStrCurrentToken = &tStatement[tokenIndex];
	int startPos = tokenIndex;
	TCHAR currentChar;

	while( tokenIndex < statementLen )
	{
		currentChar = tStatement[tokenIndex++];
		if( currentChar != TCHAR( '\0' ) )
		{
			// If it's an escape character
			if( currentChar == TCHAR( '\\' ) )
			{
				// advance past next character
				++tokenIndex;
			}
			else if( currentChar == TCHAR( '\"' ) )
			{
				// End of string expression... put expression on the stack...
				
				// Add the token to the stack
				// Note: The Delimiters are not saved as part of the Token
				int tokenSize = (tokenIndex - startPos) - 1;

				LPTSTR tStrToken = ( LPTSTR )malloc( tokenSize + 1 );
					
				// Empty String is allowed
				if( tokenSize )
					memcpy( tStrToken, tStrCurrentToken, tokenSize );

				tStrToken[tokenSize] = TCHAR( '\0' );
				RExpressionStack.Add( tStrToken );

#ifdef SVOBJECTSCRIPTPARSER_DEBUG
				TRACE( "ParserState: READ_STRING, CurrentToken: %.80s\n", tStrToken );
#endif

				return true;
			}
		}
	}
	// Error, unexpected end of parse string...
	AfxMessageBox( "ParserState: READ_STRING\nParser Error, unexpected end of parse string while reading string..." );
	return false;
}

///////////////////////////////////////////////////////////////
// Extract the Name String Token
///////////////////////////////////////////////////////////////
bool SVObjectScriptParserSECClass::getNameStringToken( SVExpressionStackSEC& RExpressionStack, LPCTSTR tStatement, int statementLen, int& tokenIndex )
{
	LPTSTR tStrCurrentToken = ( LPTSTR )&tStatement[tokenIndex];
	int startPos = tokenIndex;
	TCHAR currentChar;

	while( tokenIndex < statementLen )
	{
		currentChar = tStatement[tokenIndex++];
		if( currentChar != TCHAR( '\0' ) )
		{
			// If it's an escape character
			if( currentChar == TCHAR( '\\' ) )
			{
				// advance past next character
				++tokenIndex;
			}
			else if( currentChar == TCHAR( '\'' ) )
			{
				// End of name string expression... put expression on the stack...

#ifdef SVOBJECTSCRIPTPARSER_DEBUG				
				TRACE( "ParserState: READ_NAME_STRING, CurrentToken: %.80s\n", tStrCurrentToken );
#endif

				// Note: The Delimiters are not saved as part of the Token
				int tokenSize = (tokenIndex - startPos) - 1;

				// Kludge to parse out member names (Name.Name.Member)
				if (!FindMembers( tStrCurrentToken, tokenSize, RExpressionStack ) )
				{
					// Add the token to the stack
					if( tokenSize )
					{
						LPTSTR tStrToken = ( LPTSTR )malloc( tokenSize + 1 );
						memcpy( tStrToken, tStrCurrentToken, tokenSize );
						tStrToken[tokenSize] = TCHAR( '\0' );
						RExpressionStack.Add( tStrToken );
					}
				}
				return true;
			}
		}
	}	
	// Error, unexpected end of parse string...
	AfxMessageBox( "ParserState: READ_NAME_STRING\nParser Error, unexpected end of name string while parsing ..." );
	return false;
}

///////////////////////////////////////////////////////////////
// Extract the Guid String Token
///////////////////////////////////////////////////////////////
bool SVObjectScriptParserSECClass::getGuidStringToken( SVExpressionStackSEC& RExpressionStack, LPCTSTR tStatement, int statementlen, int& tokenIndex )
{
	//	get complete GUID token and put it on the expression stack
	//  GUID-Token: e.g. { 0x2ddd26a9, 0x79a7, 0x11d3, { 0xa8, 0x90, 0x0, 0x12, 0x0, 0x0, 0x5, 0xaa } }

	// Read complete GUID token and put token on the stack...
	int count = 0;
	
	LPTSTR tStrTok = ExtractGUIDToken( &tStatement[ tokenIndex ], count );
	if( tStrTok )
	{
		// If there was an expression, put it on the stack...
		if( _tcslen( tStrTok ) > 0 )
		{
			RExpressionStack.Add( tStrTok );

#ifdef SVOBJECTSCRIPTPARSER_DEBUG
			TRACE( "Add Expression: %.80s\n", tStrTok );
#endif

			tokenIndex += count;
			return true;
		}
		else
			free( tStrTok );

		AfxMessageBox( "ParserState: READ_GUID_STRING\nParser Error, empty token found..." );
		return false;
	}

	AfxMessageBox( "ParserState: READ_GUID_STRING\nParser Error, GUID expression cannot be evaluated..." );
	return false;
}

///////////////////////////////////////////////////////////////
// Process the Alias Command
///////////////////////////////////////////////////////////////
bool SVObjectScriptParserSECClass::processAlias( SVExpressionStackSEC& RExpressionStack, SVObjectInfoStruct& RLocalOwnerInfo, SVObjectScriptAliasListClassSEC& RCurrentAliasTable, SVObjectScriptAliasListClassSEC& RAliasTable )
{
	bool rc = false;

#ifdef SVOBJECTSCRIPTPARSER_DEBUG
	TRACE( "ALIAS\n" );
#endif

	SVObjectScriptOperandStructSEC operand1, operand2;

	// Check operands and operators...
	int numberOfExpressions = RExpressionStack.GetSize();
	int index = 1;

	// operand1
	// must be a name_expression
	if( ! EvaluateOperandExpression( NAME, RLocalOwnerInfo, RCurrentAliasTable, operand1, RExpressionStack, index ) ||
		! operand1.PValue 
	  )
	{
		AfxMessageBox( "Parser Error:\nName operand expected!" );
		return rc;
	}

	// must be operator =
	if( ++index >= numberOfExpressions || _tcscmp( RExpressionStack.GetAt( index ), _T( "=" ) ) )
	{
		AfxMessageBox( "Parser Error:\nOperator = expected!" );
		return rc;
	}

	// operand2
	// must be a embedded_expression|class_expression|object_expression
	if( ! EvaluateOperandExpression( ALIAS | OBJECT | CLASS | EMBEDDED, RLocalOwnerInfo, RAliasTable, operand2, RExpressionStack, ++ index ) ||
		! operand2.PValue 
	  )
	{
		AfxMessageBox( "Parser Error:\nObject operand, class operand or embedded operand expected!" );
		return rc;
	}

	// Check for operator ; 
	if(	++index >= numberOfExpressions || _tcscmp( RExpressionStack.GetAt( index ), _T( ";" ) ) )
	{
		AfxMessageBox( "Parser Error:\nCommand termination expected!" );
		return rc;
	}

	// Now we must be at the end of the stack...
	if(	index != RExpressionStack.GetUpperBound() )
	{
		AfxMessageBox( "Parser Error:\nToo many expressions found!" );
		return rc;
	}

	// Process command...
	rc = true;

	// *** // ***
	// The old code used the string for "Long Result" for Bar Code Result
	// This messes up the Alias table. Replace it to remove the issue.
	CString strName;
	strName.LoadString( IDS_CLASSNAME_RESULT_LONG );

	if( _tcscmp( (LPTSTR) operand1.PValue, strName ) == 0 &&
		*(GUID*)( operand2.PValue ) == SVBarCodeResultClassGuid )
	{
		free( (LPTSTR) operand1.PValue );
		strName.LoadString( IDS_CLASSNAME_SVBARCODEANALYZERESULT );
		operand1.PValue = _tcsdup( strName );
	}// end if
	// *** // ***
	
	RAliasTable.Add( SVObjectScriptAliasStructSEC( ( LPTSTR ) operand1.PValue, operand2 ) );

#ifdef SVOBJECTSCRIPTPARSER_DEBUG
	TRACE("Adding Alias: %.80s\n", ( LPTSTR ) operand1.PValue );
#endif

	return rc;
}

///////////////////////////////////////////////////////////////
// Process Define Object - Constructs Dynamic Objects
///////////////////////////////////////////////////////////////
SVObjectClass* SVObjectScriptParserSECClass::processDefineObject( SVExpressionStackSEC& RExpressionStack, SVObjectInfoStruct& RLocalOwnerInfo, SVObjectScriptAliasListClassSEC& RAliasTable, SVObjectScriptOperandStructSEC& RFirstOperand, BOOL& RBStepIn )
{
	SVObjectClass* pObject = NULL;

	int index = 0;

#ifdef SVOBJECTSCRIPTPARSER_DEBUG
	TRACE( "OBJECT_DEFINE\n" );
#endif

	SVObjectScriptOperandStructSEC embeddedOperand, classOperand, nameOperand, objectOperand, objectOwnerOperand;
	SVObjectScriptOperandListSEC inputOperandList;
	RBStepIn = FALSE;
	
	// Check operands and operators...
	if( RFirstOperand.Type == EMBEDDED )
	{
		embeddedOperand = RFirstOperand;
		// Try to read class operand...
		if( ! EvaluateOperandExpression( ALIAS | CLASS, RLocalOwnerInfo, RAliasTable, classOperand, RExpressionStack, ++ index ) ||
			! classOperand.PValue
		  )
		{
			AfxMessageBox( "Parser Error:\nClass operand expected!" );
			return pObject;
		}
	}
	else
		classOperand = RFirstOperand;

	// nameOperand
	// must be a name_Expression
	if( ! EvaluateOperandExpression( NAME, RLocalOwnerInfo, RAliasTable, nameOperand, RExpressionStack, ++ index ) ||
		! nameOperand.PValue
	  )
	{
		AfxMessageBox( "Parser Error:\nName operand expected!" );
		return pObject;
	}

	// must be operator =
	if( ++index >= RExpressionStack.GetSize() || _tcscmp( RExpressionStack.GetAt( index ), _T( "=" ) ) )
	{
		AfxMessageBox( "Parser Error:\nOperator = expected!" );
		return pObject;
	}

	// objectOperand
	// must be a object_expression
	if( ! EvaluateOperandExpression( ALIAS | OBJECT, RLocalOwnerInfo, RAliasTable, objectOperand, RExpressionStack, ++ index ) ||
		! objectOperand.PValue
	  )
	{
		AfxMessageBox( "Parser Error:\nOperator operand expected!" );
//		SV_PARSE_MESSAGE( "Parser Error:\nObject operand expected!\n\n*%s*", tStrParseEnd );
		return pObject;
	}

	if( ++index >= RExpressionStack.GetSize() )
	{
		AfxMessageBox( "Parser Error:\nUnexpected end of command found!" );
		return pObject;
	}

	// could be operator @
	if(	! _tcscmp( RExpressionStack.GetAt( index ), _T( "@" ) ) )
	{
		// Try to read owner...
		if( ! EvaluateOperandExpression( ALIAS | OBJECT, RLocalOwnerInfo, RAliasTable, objectOwnerOperand, RExpressionStack, ++ index ) ||
			! objectOwnerOperand.PValue
		  )
		{
			AfxMessageBox( "Parser Error:\nObject operand expected!" );
			return pObject;
		}
		objectOwnerOperand.Type = OBJECT_OWNER;

#ifdef SVOBJECTSCRIPTPARSER_DEBUG
		TRACE( "OBJECT_DEFINE, @\n" );
#endif

		if( ++index >= RExpressionStack.GetSize() )
		{
			AfxMessageBox( "Parser Error:\nUnexpected end of command found!" );
			return pObject;
		}
	}
	else
	{
		// No explicit object owner given, use local owner...
		objectOwnerOperand.ValueSize = sizeof( GUID );
		objectOwnerOperand.PValue = malloc( sizeof( GUID ) );
		if( objectOwnerOperand.PValue )
		{
			// substitute operand...
			memcpy( objectOwnerOperand.PValue, &RLocalOwnerInfo.UniqueObjectID, sizeof( GUID ) );
			objectOwnerOperand.Type = OBJECT_OWNER;
		}
		else
		{
			AfxMessageBox( "Parser Error:\nOut of memory!" );
			return pObject;
		}
	}

	// Check for input object list...
	// (...)
	if(	! _tcscmp( RExpressionStack.GetAt( index ), _T( "(" ) ) )
	{
		if( ! ReadOperandList( RLocalOwnerInfo, RAliasTable, inputOperandList, RExpressionStack, index ) )
		{
			AfxMessageBox( "Parser Error:\nUnable to read input list!" );
			return pObject;
		}

#ifdef SVOBJECTSCRIPTPARSER_DEBUG
		TRACE( "OBJECT_DEFINE, (...)\n" );
#endif

		if( ++index >= RExpressionStack.GetSize() )
		{
			AfxMessageBox( "Parser Error:\nUnexpected end of command found!" );
			return pObject;
		}
	}

	// Check for operator ; or operator {
	if(	_tcscmp( RExpressionStack.GetAt( index ), _T( ";" ) ) && 
		_tcscmp( RExpressionStack.GetAt( index ), _T( "{" ) ) 
	  )
	{
		AfxMessageBox( "Parser Error:\nCommand termination or object member definition expected!" );
		return pObject;
	}

	// Check for object member definition block...
	if( ! _tcscmp( RExpressionStack.GetAt( index ), _T( "{" ) ) )
	{
		RBStepIn = TRUE;
	}
	else
	{
		// Now we must be at the end of the stack...
		if(	index != RExpressionStack.GetUpperBound() )
		{
			AfxMessageBox( "Parser Error:\nToo many expressions found!" );
			return pObject;
		}
	}

	// Process command...
#ifdef SVOBJECTSCRIPTPARSER_DEBUG
	TRACE( "OBJECT_DEFINE, Process command\n" );
#endif

	SVObjectInfoStruct ownerObjectInfo;
	ownerObjectInfo.UniqueObjectID = *( ( GUID* ) objectOwnerOperand.PValue );
	ownerObjectInfo.PObject = SVObjectManagerClass::Instance().GetObject( ownerObjectInfo.UniqueObjectID );

	if( ! ownerObjectInfo.PObject )
	{
		AfxMessageBox( "Parser Error:\n No valid object owner given!" );
		return pObject;
	}

	// Check for embedded...
	if( embeddedOperand.PValue || embeddedOperand.Type == EMBEDDED )
	{
		if( embeddedOperand.PValue && embeddedOperand.Type == EMBEDDED )
		{
#ifdef SVOBJECTSCRIPTPARSER_DEBUG
			TRACE( "OBJECT_DEFINE, embedded SVM_OVERWRITE_OBJECT\n" ); 
#endif

			// Send to Owner of Embedded Object
			// Try to overwrite object...
			if( pObject = ( SVObjectClass* ) ::SVSendMessage( ownerObjectInfo.PObject, 
			                                                  SVM_OVERWRITE_OBJECT, 
																												( DWORD ) objectOperand.PValue, 
																												( DWORD ) embeddedOperand.PValue ) )
			{
#ifdef SVOBJECTSCRIPTPARSER_DEBUG
				TRACE( "OverWriteObject %.80s\n", pObject->GetName() );
#endif
			}
			else
			{
				int x = 6;
			}
		}
		else
		{
			AfxMessageBox( "Parser Error:\n Wrong embedded values!" );
			return pObject;
		}
	}
	else
	{
		// Construct new object...
		SVObjectManagerClass::Instance().ConstructObject( *( ( GUID* ) classOperand.PValue ), pObject );

#ifdef SVOBJECTSCRIPTPARSER_DEBUG
		TRACE( "OBJECT_DEFINE, new object SVM_REPLACE_OBJECT\n" );
#endif

		if( pObject && objectOwnerOperand.PValue )
		{
			// Try to replace or add object...
			if( ::SVSendMessage( ownerObjectInfo.PObject, SVM_REPLACE_OBJECT, ( DWORD ) objectOperand.PValue, ( DWORD ) pObject ) != SVMR_SUCCESS )
			{
				TRACE( "ReplaceObject %.80s\n", RLocalOwnerInfo.PObject->GetName() );

				delete pObject;

				TRACE( "after delete, SVM_REPLACE_OBJECT was not successfully" );
			}
			else
			{
				reAttachInputs( pObject, inputOperandList );
			}
		}
		else
		{
			AfxMessageBox( "Parser Error:\n Cannot construct new object!" );
			return pObject;
		
		}
	}

	/////////////////////////////////////////////////////////////////////
	// Both Embedded and Dynamic Objects come to this point
	/////////////////////////////////////////////////////////////////////
	if( pObject )
	{
		pObject->SetName( ( LPTSTR ) nameOperand.PValue );

#ifdef SVOBJECTSCRIPTPARSER_DEBUG
		TRACE( "%.80s\n",pObject->GetName() );
#endif
		
		//rc = true;				
	}
	else
	{
		int x = 5;
	}
	return pObject;
}

/////////////////////////////////////////////////////////////
// Process Member Assignment
/////////////////////////////////////////////////////////////
bool SVObjectScriptParserSECClass::processMemberAssignment( SVExpressionStackSEC& RExpressionStack, SVObjectInfoStruct& RLocalOwnerInfo, SVObjectScriptAliasListClassSEC& RAliasTable, SVObjectScriptOperandStructSEC& RFirstOperand )
{
	bool rc = false;

	int index = 2;

#ifdef SVOBJECTSCRIPTPARSER_DEBUG
	TRACE( "Member ASSIGNMENT\n" );
#endif
						
	SVObjectAttributeClass dataObject;

	// must be member name
	dataObject.SetName( ( LPCTSTR )RFirstOperand.PValue );

	// Get Member's Owner (embeddedObject)
	SVObjectScriptOperandStructSEC objectOperand, objectOwnerOperand;


	// Set Embedded Object's Owner
	objectOwnerOperand.ValueSize = sizeof( GUID );
	objectOwnerOperand.PValue = malloc( sizeof( GUID ) );
	if( objectOwnerOperand.PValue )
	{
		// substitute operand...
		memcpy( objectOwnerOperand.PValue, &RLocalOwnerInfo.UniqueObjectID, sizeof( GUID ) );
		objectOwnerOperand.Type = OBJECT_OWNER;
	}

	// Position expression stack to the object just before the member
	// Backup 2 (object.member) since we are positioned at the member
	int i = index - 2;
	if( i < 0 )
	{
		AfxMessageBox( "Parser Error:\nObject required for Member!" );
		return rc;
	}
	
	// objectOperand
	// must be a object_expression
	if( ! EvaluateOperandExpression( ALIAS | OBJECT, RLocalOwnerInfo, RAliasTable, objectOperand, RExpressionStack, i ) ||
		! objectOperand.PValue
	  )
	{
		AfxMessageBox( "Parser Error:\nOperator operand expected!" );
		//SV_PARSE_MESSAGE( "Parser Error:\nObject operand expected!\n\n*%s*", tStrParseEnd );
		return rc;
	}
	
	// Check operands and operators...

	// must be operator =
	if( ++index >= RExpressionStack.GetSize() || _tcscmp( RExpressionStack.GetAt( index ), _T( "=" ) ) )
	{
		AfxMessageBox( "Parser Error:\nOperator = expected!" );
		return rc;
	}
	// skip past operator =
	
	if( ++index >= RExpressionStack.GetSize() )
	{
		AfxMessageBox( "Parser Error:\nUnexpected end of command found!" );
		return rc;
	}

	// must be DataType and Array [ ] of decimal, hexadecimal, or floating point numbers
	// OR DataType and single decimal, hexadecimal, floating point number

	if( ! ReadValues( dataObject, RExpressionStack, index ) )
	{
		AfxMessageBox( "Parser Error:\nValues expected!" );
		return rc;
	}

	// Check for operator ;
	if(	_tcscmp( RExpressionStack.GetAt( index ), _T( ";" ) ) )
	{
		AfxMessageBox( "Parser Error:\nCommand termination or object member definition expected!" );
		return rc;
	}
	
	// Process command...
#ifdef SVOBJECTSCRIPTPARSER_DEBUG
	TRACE( "SET_OBJECT_VALUE, Process command\n" );
#endif

	SVObjectInfoStruct objectInfo, ownerObjectInfo;

	objectInfo.UniqueObjectID = *( ( GUID* ) objectOperand.PValue );
	objectInfo.PObject = SVObjectManagerClass::Instance().GetObject( objectInfo.UniqueObjectID );

	if( ! objectInfo.PObject )
	{
		AfxMessageBox( "Parser Error:\n No valid object given!" );
		return rc;
	}
	
	ownerObjectInfo.UniqueObjectID = *( ( GUID* ) objectOwnerOperand.PValue );
	ownerObjectInfo.PObject = SVObjectManagerClass::Instance().GetObject( ownerObjectInfo.UniqueObjectID );

	if( ! ownerObjectInfo.PObject )
	{
		AfxMessageBox( "Parser Error:\n No valid object owner given!" );
		return rc;
	}
	
	/////////////////////////////////////////////////////////////////////
	// If an Object and it's Owner were Found
	/////////////////////////////////////////////////////////////////////
	if( ownerObjectInfo.PObject && objectInfo.PObject )
	{
#ifdef SVOBJECTSCRIPTPARSER_DEBUG
		TRACE( "%s %s\n",ownerObjectInfo.PObject->GetName(), objectInfo.PObject->GetName()  );
#endif

		// try to set the object's member value...
		// Note:: Send this message to the Object's Owner
		if( ::SVSendMessage( ownerObjectInfo.PObject, SVM_SET_OBJECT_VALUE, ( DWORD )&objectInfo.UniqueObjectID, ( DWORD ) &dataObject ) != SVMR_SUCCESS )
		{
			AfxMessageBox( "Parser Error:\n Set Object Member Value failed!" );
			return rc;
		}
		rc = true;
	}
	return rc;
}

size_t SVObjectScriptParserSECClass::GetTotal() const
{ 
	return m_pParseString->GetLength();
}

HRESULT SVObjectScriptParserSECClass::DoParse()
{
	LPCTSTR pScript = Parse( m_pOwnerObject, *m_pParseString );
	return (pScript != NULL) ? S_OK : E_FAIL;
}


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVObjectScriptParserSEC.cpp_v  $
 * 
 *    Rev 1.1   01 Oct 2013 15:24:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 12:41:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   04 Sep 2012 15:50:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use new progress dialog functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   19 Jun 2012 13:10:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
 * Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   27 Jan 2011 11:41:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   08 Dec 2010 13:15:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   08 Nov 2010 14:46:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   05 Nov 2010 11:08:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove redundent data objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   29 Jun 2010 14:02:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to change object validity test and casting changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   01 Jun 2010 10:59:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   16 Dec 2009 12:08:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   31 Oct 2007 08:00:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Coding error in ExtractValue - changed VariantClear to VariantInit to initialize variant.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   21 Jun 2007 14:34:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   19 Jan 2006 10:36:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methodology to get the input info objects and ignor the Aux Extent input info.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   17 Feb 2005 14:52:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Feb 2005 14:51:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 Aug 2004 12:11:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  416
 * SCR Title:  Add Variant Value Object
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   fixed parser to treat floating point as US locale
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   12 Jul 2004 12:35:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  428
 * SCR Title:  Improve Load Configuration time
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   22 Apr 2003 11:49:34   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   20 Nov 2002 09:40:18   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Changed TheObjectManager -> TheSVObjectManager
   Changed TheClassRegister -> TheSVClassRegister
   EvaluateOperandExpression
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   08 Nov 2001 15:06:36   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  255
   SCR Title:  Add Sub-pixel resolution to Profile and Line Gage Tool Analyzers
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated constructor and ExactValue methods to load sub-pixel results.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
