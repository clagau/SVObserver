//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectScriptParser
//* .File Name       : $Workfile:   SVObjectScriptParserSVX.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   19 Dec 2014 04:14:46  $
//******************************************************************************

#include "stdafx.h"
#include "SVObjectScriptParserSVX.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVGlobal.h"
#include "SVTool.h"
#include "SVObserver.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "ObjectInterfaces/SVUserMessage.h"
#include "SVBarCodeAnalyzerClass.h"
#include "SVOMFCLibrary/SVFunctionProfiler.h"
#include "SVObjectLibrary/GlobalConst.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef _DEBUG
//#define SVOBJECTSCRIPTPARSER_DEBUG
#endif

//namespace NS_ObjectScriptParserSVX
//{
const TCHAR* const TSTR_RIGHT_BRACE = _T( "}" );

// Converts GUID into String {00000000-0000-0000-000000000000}
// If successfully it returns a TCHAR String which must be freed after using.
// Otherwise it returns NULL.
LPTSTR SVStringFromGUID( const GUID& RGUID )
{
//	static SVFunctionProfiler profile(_T("SVStringFromGUID(GUID)"));
//	SVFunctionProfilerLocal profiler(profile);
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
//	static SVFunctionProfiler profile(_T("SVStringFromGUID(CString, GUID)"));
//	SVFunctionProfilerLocal profiler(profile);
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

//}// end NS_ObjectScriptParserSVX

#define SV_PARSE_MESSAGE( XFORMATSTRING, XCOMMANDSTRING )	\
		{	\
			CString strMessage, strString;	\
			strString = ( XCOMMANDSTRING );	\
			int in = strString.Find( TCHAR( ';' ) );	\
			if( in > 0 )	strString = strString.Left( in + 1 );	\
			strMessage.Format( ( XFORMATSTRING ), strString );	\
			AfxMessageBox( strMessage );	\
		}




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



SVObjectScriptAliasListClass::SVObjectScriptAliasListClass()
{

}

SVObjectScriptAliasListClass::~SVObjectScriptAliasListClass()
{
	//static SVFunctionProfiler profile(_T("SVObjectScriptAliasListClass::~SVObjectScriptAliasListClass"));
	//SVFunctionProfilerLocal profiler(profile);
	CleanUp();
}

SVObjectScriptAliasListClass::SVObjectScriptAliasListClass(const SVObjectScriptAliasListClass& rhs)
{
	//static SVFunctionProfiler profile(_T("SVObjectScriptAliasListClass::SVObjectScriptAliasListClass copy constructor"));
	//SVFunctionProfilerLocal profiler(profile);
	m_mapAlias = rhs.m_mapAlias;
}

void SVObjectScriptAliasListClass::CleanUp()
{
	//static SVSizeProfiler sizeprofile(_T("SIZE SVObjectScriptAliasListClass::CleanUp"));
	//sizeprofile.Add(m_mapAlias.size());
	//static SVFunctionProfiler profile(_T("SVObjectScriptAliasListClass::CleanUp"));
	//SVFunctionProfilerLocal profiler(profile);
	m_mapAlias.clear();
	//RemoveAll();
}

SVObjectScriptParserSVXClass::SVObjectScriptParserSVXClass(unsigned long parserHandle, SVSharedPtr<CString> pScript, const GUID& OwnerGuid, SVObjectClass* pOwnerObject, CWnd* pWnd)
: SVObjectScriptParserBase(parserHandle, OwnerGuid, pOwnerObject, pWnd)
, m_pParseString( pScript )
{
	m_KeywordTable.Add( SVObjectScriptKeywordStruct( _T( "alias" ), ALIAS ) );
	m_KeywordTable.Add( SVObjectScriptKeywordStruct( _T( "class$" ), CLASS ) );
	m_KeywordTable.Add( SVObjectScriptKeywordStruct( _T( "object$" ), OBJECT ) );
	m_KeywordTable.Add( SVObjectScriptKeywordStruct( _T( "embedded$" ), EMBEDDED ) );

	m_DataTypeKeywordTable.Add( SVObjectScriptDataTypeKeywordStruct( _T( "BOOL" ), SV_BOOL_Type ) );
	m_DataTypeKeywordTable.Add( SVObjectScriptDataTypeKeywordStruct( _T( "BYTE" ), SV_BYTE_Type ) );
	m_DataTypeKeywordTable.Add( SVObjectScriptDataTypeKeywordStruct( _T( "DOUBLE" ), SV_DOUBLE_Type ) );
	m_DataTypeKeywordTable.Add( SVObjectScriptDataTypeKeywordStruct( _T( "DWORD" ), SV_DWORD_Type ) );
	m_DataTypeKeywordTable.Add( SVObjectScriptDataTypeKeywordStruct( _T( "LONG" ), SV_LONG_Type ) );
	m_DataTypeKeywordTable.Add( SVObjectScriptDataTypeKeywordStruct( _T( "POINT" ), SV_POINT_Type ) );
	m_DataTypeKeywordTable.Add( SVObjectScriptDataTypeKeywordStruct( _T( "STRING" ), SV_STRING_Type ) );
	m_DataTypeKeywordTable.Add( SVObjectScriptDataTypeKeywordStruct( _T( "DPOINT" ), SV_DPOINT_Type ) );
	m_DataTypeKeywordTable.Add( SVObjectScriptDataTypeKeywordStruct( _T( "VARIANT" ), SV_VARIANT_Type ) );
}

SVObjectScriptParserSVXClass::~SVObjectScriptParserSVXClass()
{
	m_KeywordTable.RemoveAll();
	m_DataTypeKeywordTable.RemoveAll();

	// Cleanup
	if ( !m_pParseString.empty() )
	{
		m_pParseString.clear();
	}
}

DWORD SVObjectScriptParserSVXClass::TStrToDWORD( LPCTSTR TStrInput )
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

double SVObjectScriptParserSVXClass::TStrToDouble( LPCTSTR tstrInput )
{
	double DoubleResult = 0;
	
	_stscanf( tstrInput, "%lf", &DoubleResult );

	return DoubleResult;
}

// Cut leading and successor whitespaces away...
CString SVObjectScriptParserSVXClass::ExtractToken( LPCTSTR TStrTokenString, int TokenStringLength )
{
	//static SVFunctionProfiler profile(_T("SVObjectScriptParserSVXClass::Extract"));
	//SVFunctionProfilerLocal profiler(profile);
	CString strReturn;

	if( TStrTokenString )
	{
		LPTSTR tsz = strReturn.GetBufferSetLength(TokenStringLength+1);
		if( tsz )
		{
			memcpy( tsz, TStrTokenString, TokenStringLength );
			strReturn.ReleaseBuffer( TokenStringLength );
		}
	}

	return strReturn;
}


BOOL SVObjectScriptParserSVXClass::CheckAliasTable( const SVObjectScriptAliasListClass& rCurrentAliasTable, LPCTSTR TStrTokenString )
{
	//static SVFunctionProfiler profile(_T("SVObjectScriptParserSVXClass::CheckAliasTable(2)"));
	//SVFunctionProfilerLocal profiler(profile);
	if( TStrTokenString )
	{
		CString sToken(TStrTokenString);
		SVObjectScriptAliasListClass::MapType::const_iterator iter;
		iter = rCurrentAliasTable.m_mapAlias.find(sToken);
		if ( iter != rCurrentAliasTable.m_mapAlias.end() )
		{
			//rOperand = iter->second.SubstituteOperand;
			return TRUE;
		}
		/*
		// Check alias table...
		for( int i = 0; i < rCurrentAliasTable.GetSize(); ++ i )
		{
			SVObjectScriptAliasStruct& aliasEntry = rCurrentAliasTable.GetAt( i );
			if( ! _tcscmp( TStrTokenString, aliasEntry.TStrAlias ) )
			{
#ifdef SVOBJECTSCRIPTPARSER_DEBUG
				TRACE( "Alias Found: %.80s\n", TStrTokenString );
#endif
				profiler.End();
				return TRUE;
			}
		}
		*/
	}
#ifdef SVOBJECTSCRIPTPARSER_DEBUG
	TRACE( "Alias Not Found: %.80s\n", TStrTokenString );
#endif
	return FALSE;
}

BOOL SVObjectScriptParserSVXClass::CheckAliasTable( const SVObjectScriptAliasListClass& rCurrentAliasTable, LPCTSTR TStrTokenString, SVObjectScriptOperandStruct& rOperand)
{
//	static SVFunctionProfiler profile(_T("SVObjectScriptParserSVXClass::CheckAliasTable(3)"));
//	SVFunctionProfilerLocal profiler(profile);

	if( TStrTokenString )
	{
		CString sToken(TStrTokenString);
		SVObjectScriptAliasListClass::MapType::const_iterator iter;
		iter = rCurrentAliasTable.m_mapAlias.find(sToken);
		if ( iter != rCurrentAliasTable.m_mapAlias.end() )
		{
			rOperand = iter->second.SubstituteOperand;
			return TRUE;
		}
		/*
		// Check alias table...
		for( int i = 0; i < rCurrentAliasTable.GetSize(); ++ i )
		{
			SVObjectScriptAliasStruct& aliasEntry = rCurrentAliasTable.GetAt( i );

			if( ! _tcscmp( TStrTokenString, aliasEntry.TStrAlias ) )
			{
				rOperand = aliasEntry.SubstituteOperand;

#ifdef SVOBJECTSCRIPTPARSER_DEBUG
				TRACE( "Alias Found: %.80s\n", TStrTokenString );
#endif
				profiler.End();
				return TRUE;
			}
		}
		*/
	}
#ifdef SVOBJECTSCRIPTPARSER_DEBUG
	TRACE( "Alias Not Found: %.80s\n", TStrTokenString );
#endif

	return FALSE;
}

ParserOperandTypeEnum SVObjectScriptParserSVXClass::CheckKeywordTable( LPCTSTR tstrExpression )
{
	//static SVFunctionProfiler profile(_T("SVObjectScriptParserSVXClass::CheckKeywordTable"));
	//SVFunctionProfilerLocal profiler(profile);
	// Returns iKeyIndex!!!
	if( tstrExpression )
	{
		// Check keyword table...
		for( int i = 0; i < m_KeywordTable.GetSize(); ++ i )
		{
			SVObjectScriptKeywordStruct& keywordEntry = m_KeywordTable.GetAt( i );
			if( ! _tcscmp( tstrExpression, keywordEntry.tstrKeyword ) )
			{

#ifdef SVOBJECTSCRIPTPARSER_DEBUG
				TRACE( "Keyword Found: %.80s\n", tstrExpression );
#endif
				return keywordEntry.eKeyIndex;
			}
		}
	}

#ifdef SVOBJECTSCRIPTPARSER_DEBUG
	TRACE( "Keyword Not Found: %.80s\n", tstrExpression );
#endif

	return NOP;
}

void SVObjectScriptParserSVXClass::CleanUpExpressionStack( SVExpressionStack& rExpressionStack )
{
//	static SVFunctionProfiler profile(_T("SVObjectScriptParserSVXClass::CleanUpExpressionStack"));
//	SVFunctionProfilerLocal profiler(profile);
	/*
	for( int i = 0; i < rExpressionStack.GetSize(); ++ i )
	{
		if( rExpressionStack.GetAt( i ) )
		{
			free( rExpressionStack.GetAt( i ) );
			rExpressionStack.SetAt( i, NULL );
		}
	}
	*/

	rExpressionStack.RemoveAll();
}

// Extract GUID Token...
//  GUID-Token: e.g. { 0x2ddd26a9, 0x79a7, 0x11d3, { 0xa8, 0x90, 0x0, 0x12, 0x0, 0x0, 0x5, 0xaa } }
//				or	 {2DDD26A9-79A7-11D3-A890-0012000005AA}
//
// Note: Converts { 0x2ddd26a9, 0x79a7, 0x11d3, { 0xa8, 0x90, 0x0, 0x12, 0x0, 0x0, 0x5, 0xaa } }
//		 to       {2DDD26A9-79A7-11D3-A890-0012000005AA}
CString SVObjectScriptParserSVXClass::ExtractGUIDToken( LPCTSTR TStrTokenString, int& rCount )
{
	//static SVFunctionProfiler profile(_T("SVObjectScriptParserSVXClass::ExtractGUIDToken"));
	//SVFunctionProfilerLocal profiler(profile);
	CString strReturn;
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
				rCount = static_cast<int>(tStrFinal - TStrTokenString + 1);

				if( bCType )
				{
					// Convert to "{2DDD26A9-" Type...
					
					TCHAR szResult[ _MAX_PATH ];
					_tcscpy( szResult, _T( "{" ) );

					TCHAR szTemp[ _MAX_PATH ];
					_tcsncpy( szTemp, TStrTokenString, rCount );
					szTemp[ rCount ] = TCHAR( '\0' );

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
					strReturn = szResult;
				}
				else
				{
					// Duplicate token...
					TCHAR* tsz = strReturn.GetBufferSetLength( rCount + 1 );
					_tcsncpy( tsz, TStrTokenString, rCount );
					tsz[ rCount ] = TCHAR( '\0' );
					strReturn.ReleaseBuffer( rCount );
				}
			}
		}
	}

	return strReturn;
}

// Expectes $ as one Expression and GUID as one Expression on stack ( generated using ExtractGUIDToken!!! )
// NOTE: GUID Expression must be a {2DDD26A9-79A7-11D3-A890-0012000005AA} Type 
//		 ( refer to StringFromGUID2() Win-SDK Function )
BOOL SVObjectScriptParserSVXClass::ReadGUIDExpression( SVExpressionStack& rExpressionStack, GUID& rUniqueID, int& riIndex )
{
	//static SVFunctionProfiler profile(_T("SVObjectScriptParserSVXClass::ReadGUIDExpression"));
	//SVFunctionProfilerLocal profiler(profile);

#ifdef SVOBJECTSCRIPTPARSER_DEBUG
	TRACE( "ReadGUIDExpression: " );

	TRACE( "%.80s", rExpressionStack.GetAt( riIndex ) );
#endif

	LPCTSTR ptr = rExpressionStack.GetAt( riIndex );
	if( ptr[0] == TCHAR( '{' ) && ptr[1] != TCHAR( '\0' ) )
	{
		// Convert the GUID String Expression to a Guid...
		AfxGetClassIDFromString( rExpressionStack.GetAt( riIndex ), &rUniqueID );
		return TRUE;
	}

	TRACE( "ReadGUIDExpression: Failed\n" );
	return FALSE;
}


BOOL SVObjectScriptParserSVXClass::EvaluateOperandExpression( int OperandType, const SVObjectInfoStruct& rOwnerInfo, SVObjectScriptAliasListClass& rCurrentAliasTable, SVObjectScriptOperandStruct& rOperand, SVExpressionStack& rExpressionStack, int& riIndex, SVObjectScriptAliasListClass& rAliasTableForOptimization )
{
//	static SVFunctionProfiler profile(_T("SVObjectScriptParserSVXClass::EvaluateOperandExpression"));
//	SVFunctionProfilerLocal profiler(profile);
	// Operands are identified by
	// name_expression
	// object_expression		...could use local owner to identify
	// object_expression . object_expression [ . object_expression [...] ]

	BOOL bExpressionNext = TRUE;
	for( ; riIndex < rExpressionStack.GetSize(); ++ riIndex )
	{
		ParserOperandTypeEnum type = NOP;
		// only look for keyword when appropriate
		if( riIndex == 0 || riIndex == 3)
			type = CheckKeywordTable( rExpressionStack.GetAt( riIndex ) );

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
				rOperand.CleanUp();

#ifndef SMALLMEMORYMANAGER
				//rOperand.PValue = _tcsdup( rExpressionStack.GetAt( riIndex ) );
				//rOperand.ValueSize = _tcslen( rExpressionStack.GetAt( riIndex ) ) + 1;
				rOperand.SetValue(_tcsdup( rExpressionStack.GetAt( riIndex ) ), static_cast<int>(_tcslen( rExpressionStack.GetAt( riIndex ) ) + 1));
#else
				int iLen = _tcslen( rExpressionStack.GetAt( riIndex ) ) + 1;
				char* pNewMem = (char*)SmallMemoryMalloc(iLen);
				_tcscpy(pNewMem, rExpressionStack.GetAt( riIndex ));
				//rOperand.ValueSize = iLen;
				//rOperand.PValue = pNewMem;
				rOperand.SetValue(pNewMem, iLen);
#endif
				if( rOperand.Value() )
				{
					rOperand.Type() = NAME;
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
		if( ( OperandType & MEMBER_ACCESS ) == MEMBER_ACCESS && ! _tcscmp( rExpressionStack.GetAt( riIndex ), _T( "." ) ) )
		{
			if( ! bExpressionNext )
			{
				SVObjectInfoStruct ownerInfo;
				if( rOperand.Value() && ( rOperand.Type() == OBJECT || rOperand.Type() == OBJECT_OWNER || rOperand.Type() == EMBEDDED_OBJECT || rOperand.Type() == OBJECT_INPUT ) )
				{
					ownerInfo.UniqueObjectID	= *( ( GUID* ) rOperand.Value() );
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
					SVObjectScriptAliasListClass emptyAliasTable;
					// Try to evaluate operand...

#ifdef SVOBJECTSCRIPTPARSER_DEBUG
					TRACE( "EvaluateOperandExpression, Bingo member access\n" );
#endif

					OperandType &= (~ALIAS);					
					return EvaluateOperandExpression( OperandType, ownerInfo, emptyAliasTable, rOperand, rExpressionStack, ++riIndex, emptyAliasTable );
				}
			}
			AfxMessageBox( "Parser Error:\nUnexpected member access operator!" );
			return FALSE;
		}
		else
			if( ! bExpressionNext )
			{
				// Success...
				--riIndex;
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
				rOperand.CleanUp();

				//rOperand.ValueSize = sizeof( GUID );
#ifndef SMALLMEMORYMANAGER
				rOperand.SetValue(malloc( sizeof( GUID ) ), sizeof(GUID) );
				//rOperand.PValue = malloc( sizeof( GUID ) );
#else
				rOperand.SetValue(SmallMemoryMalloc( sizeof( GUID ) ), sizeof(GUID) );
				//rOperand.PValue = SmallMemoryMalloc( sizeof( GUID ) );
#endif
				if( rOperand.Value() )
				{
					rOperand.Type() = type;
					if( ReadGUIDExpression( rExpressionStack, *( ( GUID* ) rOperand.Value() ), ++riIndex ) )
					{
						if( type == CLASS || type == EMBEDDED )
						{
							// Success...
							return TRUE;
						}

						bExpressionNext = FALSE;
						continue;
					}
					rOperand.CleanUp();

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
				SVObjectClass* pObject = nullptr;
				if( rOwnerInfo.UniqueObjectID != SVInvalidGUID )
				{
					pObject = reinterpret_cast<SVObjectClass*>(::SVSendMessage( rOwnerInfo.UniqueObjectID, SVM_GET_OBJECT_BY_NAME, reinterpret_cast<DWORD_PTR>( static_cast<LPCTSTR>( rExpressionStack.GetAt( riIndex ))), NULL ));
				}
				if( nullptr != pObject )
				{
					rOperand.CleanUp();
				
					//rOperand.ValueSize = sizeof( GUID );
#ifndef SMALLMEMORYMANAGER
					rOperand.SetValue( malloc( sizeof( GUID ) ), sizeof(GUID) );
					//rOperand.PValue = malloc( sizeof( GUID ) );
#else
					rOperand.SetValue( SmallMemoryMalloc( sizeof( GUID ) ), sizeof(GUID) );
					//rOperand.PValue = SmallMemoryMalloc( sizeof( GUID ) );
#endif
					if( rOperand.Value() )
					{
						memcpy( rOperand.Value(), &pObject->GetUniqueObjectID(), sizeof( GUID ) );
						rOperand.Type() = OBJECT;

#ifdef SVOBJECTSCRIPTPARSER_DEBUG
						TRACE( "EvaluateOperandExpression, Bingo using owner info\n" );
#endif
						bExpressionNext = FALSE;
						continue;
					}
					
					rOperand.CleanUp();

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

						rOperand.CleanUp();

#ifndef SMALLMEMORYMANAGER
						//rOperand.PValue = _tcsdup( rExpressionStack.GetAt( riIndex ) );
						//rOperand.ValueSize = _tcslen( rExpressionStack.GetAt( riIndex ) ) + 1;
						rOperand.SetValue(_tcsdup( rExpressionStack.GetAt( riIndex ) ), static_cast<int>(_tcslen( rExpressionStack.GetAt( riIndex ) ) + 1));
#else
						int iLen = _tcslen( rExpressionStack.GetAt( riIndex ) ) + 1;
						char* pNewMem = (char*)SmallMemoryMalloc(iLen);
						_tcscpy(pNewMem, rExpressionStack.GetAt( riIndex ));
						//rOperand.ValueSize = iLen;
						//rOperand.PValue = pNewMem;
						rOperand.SetValue(pNewMem, iLen);

#endif
						if( rOperand.Value() )
						{
							rOperand.Type() = MEMBER_NAME;

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
				SVObjectScriptOperandStruct AliasExp;

				// *** // ***
				// The old code used the string for "Long Result" for Bar Code Result
				// This messes up the Alias table. Replace it to remove the issue.
				CString strName;
				LPSTR strAlias;
				BOOL bAlreadyChecked;
				BOOL bOk;

				bAlreadyChecked = FALSE;
				strName.LoadString( IDS_CLASSNAME_RESULT_LONG );
				if( rExpressionStack.GetAt( riIndex ) == strName &&
					SV_IS_KIND_OF( rOwnerInfo.PObject, SVBarCodeAnalyzerClass )  )
				{
					strName.LoadString( IDS_CLASSNAME_SVBARCODEANALYZERESULT );
					strAlias = _tcsdup( strName );
					if( CheckAliasTable( rCurrentAliasTable, strAlias, AliasExp ) )
					{
						rExpressionStack.SetAt( riIndex, strAlias );
						bAlreadyChecked = TRUE;
						bOk				= TRUE;
					}// end if
					else
					{
						SVObjectScriptOperandStruct operand2;
						//operand2.ValueSize = sizeof( GUID );
#ifndef SMALLMEMORYMANAGER
						//operand2.PValue = malloc( sizeof( GUID ) );
						operand2.SetValue( malloc( sizeof( GUID ) ), sizeof(GUID) );
#else
						//operand2.PValue = SmallMemoryMalloc( sizeof( GUID ) );
						operand2.SetValue( SmallMemoryMalloc( sizeof( GUID ) ), sizeof(GUID) );
#endif
						(*(GUID*)operand2.Value()) = SVBarCodeResultClassGuid;
						operand2.Type() = CLASS;

						rCurrentAliasTable.m_mapAlias[strName] = SVObjectScriptAliasStruct( strName, operand2 );
						//rCurrentAliasTable.Add( SVObjectScriptAliasStruct( strName, operand2 ) );
						if ( &rAliasTableForOptimization != &rCurrentAliasTable )
							rAliasTableForOptimization.m_mapAlias[strName].SubstituteOperand.Dirty() = true;

						bOk = CheckAliasTable( rCurrentAliasTable, strAlias, AliasExp );
						bAlreadyChecked = TRUE;
					}// end if
				}// end if

				if( !bAlreadyChecked )
					bOk = CheckAliasTable( rCurrentAliasTable, rExpressionStack.GetAt( riIndex ), AliasExp );

				if(  bOk && AliasExp.Value() && 
					( ( OperandType & OBJECT )	 == AliasExp.Type() ||
					  ( OperandType & EMBEDDED ) == AliasExp.Type() ||
					  ( OperandType & CLASS )	 == AliasExp.Type()
					)
				  )
				{
					// substitute operand...
					rOperand = AliasExp;
						
					if( rOperand.Value() )
					{
#ifdef SVOBJECTSCRIPTPARSER_DEBUG
						TRACE( "EvaluateOperandExpression, Bingo using alias table\n" );
#endif

						bExpressionNext = FALSE;
						continue;
					}
					rOperand.CleanUp();

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


BOOL SVObjectScriptParserSVXClass::ReadOperandList( const SVObjectInfoStruct& rLocalOwnerInfo, const SVObjectScriptAliasListClass& rCurrentAliasTable, SVObjectScriptOperandList& rOperandList, SVExpressionStack& rExpressionStack, int& riIndex )
{
	// Input object operands are identified by
	// object_expression
	// object_expression . object_expression [ . object_expression [...] ]

	int evaluateType = MEMBER_ACCESS | OBJECT;

	// Operand lists are identified by
	// ( Operand1, Operand2, ..., Operandn ) or ()

	// Check current index...
	if( riIndex >= rExpressionStack.GetSize() )
		return FALSE;

	// Check for list begin...
	if( _tcscmp( rExpressionStack.GetAt( riIndex ), _T( "(" ) ) )
		return FALSE;

	// Check current index...
	if( ++riIndex >= rExpressionStack.GetSize() )
		return FALSE;

	// Check for empty list...
	if( ! _tcscmp( rExpressionStack.GetAt( riIndex ), _T( ")" ) ) )
		return TRUE;

	// Read operand...
	SVObjectScriptOperandStruct operand;

	// Read operand list...
	BOOL bOperandNext = TRUE;
	for( ; riIndex < rExpressionStack.GetSize() && _tcscmp( rExpressionStack.GetAt( riIndex ), _T( ")" ) ); ++ riIndex )
	{
		if( bOperandNext )
		{
			// Check for object$ keyword
			if( !_tcscmp( rExpressionStack.GetAt( riIndex++ ), _T( "object$" ) ) )
			{
				operand.CleanUp();

				//operand.ValueSize = sizeof( GUID );
#ifndef SMALLMEMORYMANAGER
				//operand.PValue = malloc( sizeof( GUID ) );
				operand.SetValue( malloc( sizeof( GUID ) ), sizeof(GUID) );
#else
				//operand.PValue = SmallMemoryMalloc( sizeof( GUID ) );
				operand.SetValue( SmallMemoryMalloc( sizeof( GUID ) ), sizeof(GUID) );
#endif
				if( operand.Value() )
				{
					operand.Type() = OBJECT_INPUT;
			
					// check for guid
					if( ReadGUIDExpression( rExpressionStack, *( ( GUID* )operand.Value() ), riIndex ) )
					{
						rOperandList.Add( operand );
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
			if( _tcscmp( rExpressionStack.GetAt( riIndex ), _T( "," ) ) )
				return FALSE;

			bOperandNext = TRUE;
		}
	}

	return ! bOperandNext;
}

BOOL SVObjectScriptParserSVXClass::ReadValues( SVObjectAttributeClass& dataObject, SVExpressionStack& rExpressionStack, int& riIndex )
{
	//static SVFunctionProfiler profile(_T("SVObjectScriptParserSVXClass::ReadValues"));
	//SVFunctionProfilerLocal profiler(profile);
	BOOL rc = FALSE;

	// Check current index...
	if( riIndex >= rExpressionStack.GetSize() )
		return rc;

	// Check for value(s) begin...
	dataObject.SetType( CheckDataTypeKeywordTable( rExpressionStack.GetAt( riIndex ) ) );
	// Should we have specified a type
	if( dataObject.GetSVObjectScriptDataObjectTypeEnum() == SV_UNKNOWN_Type )
		return rc;

	//ASSERT( dataObject.GetSVObjectScriptDataObjectTypeEnum() != SV_VARIANT_Type );	// DEBUG break on Variant

	// Check current index...
	if( ++riIndex >= rExpressionStack.GetSize() )
		return rc;

	// Read value(s)...
	
	if(  !_tcscmp( rExpressionStack.GetAt( riIndex ), _T( "[" ) ) )
	{
		rc = TRUE;	// allow empty arrays to parse successfully. EB 2005 01 28
		riIndex++;

		for( ; riIndex < rExpressionStack.GetSize() && _tcscmp( rExpressionStack.GetAt( riIndex ), _T( ";" ) ); ++ riIndex )
		{
			if (   dataObject.GetSVObjectScriptDataObjectTypeEnum() == SV_STRING_Type
				|| dataObject.GetSVObjectScriptDataObjectTypeEnum() == SV_VARIANT_Type)	//????
			{
				if( _tcscmp( rExpressionStack.GetAt( riIndex ), _T( "," ) ) &&
					  _tcscmp( rExpressionStack.GetAt( riIndex ), _T( "]" ) ) )
				{
					// it's our string !
					if( ! ExtractValue( dataObject, rExpressionStack, riIndex ) )
					{
						rc = FALSE;
						break;
					}
					rc = TRUE;
				}
			}

			else 
			{
				if( CheckNumber( rExpressionStack.GetAt( riIndex ) ) )
				{
					if( ! ExtractValue( dataObject, rExpressionStack, riIndex ) )
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
					if( _tcscmp( rExpressionStack.GetAt( riIndex ), _T( "," ) ) &&
						  _tcscmp( rExpressionStack.GetAt( riIndex ), _T( "]" ) ) )
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
		for( ; riIndex < rExpressionStack.GetSize() && _tcscmp( rExpressionStack.GetAt( riIndex ), _T( ";" ) ); ++ riIndex )
		{
			if( CheckValue( dataObject, rExpressionStack.GetAt( riIndex ) ) )
			{
				if(! ExtractValue( dataObject, rExpressionStack, riIndex ))
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

BOOL SVObjectScriptParserSVXClass::ExtractValue( SVObjectAttributeClass& dataObject, 
	SVExpressionStack& rExpressionStack, 
	int& riIndex )
{
	BOOL bOk = FALSE;

	// convert String to Number
	switch ( dataObject.GetSVObjectScriptDataObjectTypeEnum() )
	{
		case SV_BOOL_Type:
		{
			BOOL boolValue;

			_stscanf( rExpressionStack.GetAt( riIndex ), "%d", &boolValue );

			bOk = dataObject.AddData( boolValue );

			break;
		}

		case SV_DWORD_Type:
		{
			DWORD dwordValue;

			_stscanf( rExpressionStack.GetAt( riIndex ), "%x", &dwordValue );

			bOk = dataObject.AddData( dwordValue );

			break;
		}

		case SV_LONG_Type:
		{
			long longValue;

			_stscanf( rExpressionStack.GetAt( riIndex ), "%d", &longValue );

			bOk = dataObject.AddData( longValue );

			break;
		}

		case SV_DOUBLE_Type:
		{
			double doubleValue;
			LPCTSTR doubleStr = rExpressionStack.GetAt( riIndex );

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
			_stscanf( rExpressionStack.GetAt( riIndex++ ), "%d", &pointValue.x );
			// Next expression on stack is a comma!!!
			riIndex++;
			_stscanf( rExpressionStack.GetAt( riIndex ), "%d", &pointValue.y );

			bOk = dataObject.AddData( pointValue );

			break;
		}

		case SV_DPOINT_Type:
		{
			SVDPointClass pointValue;
			_stscanf( rExpressionStack.GetAt( riIndex++ ), "%lf", &pointValue.x );
			// Next expression on stack is a comma!!!
			riIndex++;
			_stscanf( rExpressionStack.GetAt( riIndex ), "%lf", &pointValue.y );

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
			LPCTSTR lpszExp = rExpressionStack.GetAt( riIndex );
			
			if( lpszExp )
			{
				// check for hex prefix
				if( _tcsstr( lpszExp, _T( "0x" ) ) )
				{
					_stscanf( lpszExp, "%x", &value );
				}
				else
				{
					// it's decimal
					_stscanf( lpszExp, "%d", &value );
				}
			}

			byteValue = (BYTE) value;

			bOk = dataObject.AddData( byteValue );

			break;
		}// end case SV_BYTE_Type:

		case SV_STRING_Type:
		{
			CString stringValue;

			stringValue = rExpressionStack.GetAt( riIndex );

			bOk = dataObject.AddData( stringValue );

			break;
		}

		case SV_VARIANT_Type:
		{
			_variant_t v;

			long lvt;

			_stscanf( rExpressionStack.GetAt( riIndex++ ), "%d", &lvt );

			VARTYPE l_VarType = static_cast<VARTYPE>(lvt);

			if ( l_VarType == VT_EMPTY || (l_VarType & VT_ARRAY) == VT_ARRAY ) 
			{
				riIndex++;	// skip comma
			}
			else
			{
				// parse value
				riIndex++;	// skip comma
				CString strValue;
				strValue = rExpressionStack.GetAt( riIndex );
				_variant_t vStringValue;
				vStringValue.SetString( strValue );
				HRESULT hr = ::VariantChangeTypeEx(&v, &vStringValue, SvOl::LCID_USA, VARIANT_ALPHABOOL, l_VarType);		// use United States locale
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

BOOL SVObjectScriptParserSVXClass::CheckValue( SVObjectAttributeClass& dataObject, LPCTSTR tstrToken )
{
	BOOL rc = (    dataObject.GetSVObjectScriptDataObjectTypeEnum() == SV_STRING_Type
				|| dataObject.GetSVObjectScriptDataObjectTypeEnum() == SV_VARIANT_Type );

	if( !rc )
	 return CheckNumber( tstrToken );
		
	return rc;
}

/* Do not use - Delimiters have been removed already !!! */
BOOL SVObjectScriptParserSVXClass::CheckString( LPCTSTR tstrToken )
{
	//static SVFunctionProfiler profile(_T("SVObjectScriptParserSVXClass::CheckString"));
	//SVFunctionProfilerLocal profiler(profile);
	int len = static_cast<int>(_tcslen( tstrToken ));
	
	// check for double quote at beginning
	// check for double quote at end
	if( len > 1 )
	{
		if( tstrToken[0] == TCHAR( '"' ) && tstrToken[len - 1] == TCHAR( '"' ) )
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL SVObjectScriptParserSVXClass::CheckNumber( LPCTSTR tstrToken )
{
	int len = static_cast<int>(_tcslen( tstrToken ));
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
		if( tstrToken[0] == TCHAR( '0' ) && tstrToken[1] == TCHAR( 'x' ) )
		{
			// hexadecimal number
			for (int i = 2;i < len;i++ )
			{
				if (!_istdigit( tstrToken[i] ) && !_istxdigit( tstrToken[i] ) )
					return FALSE;
			}
			return TRUE;
		}
	}

	// Decimal/floating point numbers
	for (int i = 0;i < len;i++ )
	{
		if (!_istdigit( tstrToken[i] ) )
		{
			if( tstrToken[i] == TCHAR( '.' )  && !decimalPoint )
			{
				decimalPoint = TRUE;
				continue;
			}
			// check for minus sign
			if( tstrToken[i] == TCHAR( '-' ) && !minusSign && !decimalPoint )
			{
				minusSign = TRUE;
				continue;
			}

			if( decimalPoint )
			{
				// Check for #INF , #IND, or # ( NAN or QNAN ? )
				if( len -  i >= nanStr.GetLength() &&
					( !_tcsncmp( &tstrToken[i], infinityStr, infinityStr.GetLength() ) ||
					  !_tcsncmp( &tstrToken[i], quietNanStr, quietNanStr.GetLength() ) ||
					  !_tcsncmp( &tstrToken[i], nanStr, nanStr.GetLength() ) ) )
					return TRUE;
			}
			return FALSE;
		}
	}
	return TRUE;
}

SVObjectScriptDataObjectTypeEnum SVObjectScriptParserSVXClass::CheckDataTypeKeywordTable( LPCTSTR tstrExpression )
{
	// Check keyword table...
	if( tstrExpression )
	{
		for( int i = 0; i < m_DataTypeKeywordTable.GetSize(); ++ i )
		{
			SVObjectScriptDataTypeKeywordStruct& dataTypeKeywordEntry = m_DataTypeKeywordTable.GetAt( i );
			if( ! _tcscmp( tstrExpression, dataTypeKeywordEntry.tstrKeyword ) )
			{
				return dataTypeKeywordEntry.eKeyType;
			}
		}
	}
	return SV_UNKNOWN_Type;
}

BOOL SVObjectScriptParserSVXClass::FindMembers( LPTSTR tstrToken, int tokenStrLen, SVExpressionStack& rExpressionStack )
{
	//static SVFunctionProfiler profile(_T("SVObjectScriptParserSVXClass::FindMembers"));
	//SVFunctionProfilerLocal profiler(profile);
	BOOL found = FALSE;
	CString tmp;

	tmp.GetBuffer(512);
	tmp.ReleaseBuffer();

	// Decimal/floating point numbers
	for (int i = 0;i < tokenStrLen;i++ )
	{
		if( tstrToken[i] == TCHAR( '.' ) )
		{
			found = TRUE;
			// get expression to the left of the period
			// push name expression onto expression stack
			rExpressionStack.Add( tmp );

			tmp.Empty();

			tmp.GetBuffer(512);
			tmp.ReleaseBuffer();

			// push period (separator) onto stack
			rExpressionStack.Add( _T( "." ) );
		}
		else 
		{
			tmp += tstrToken[i];
		}
	}
	if( found ) // don't forget the last name (rhs)
	{
		rExpressionStack.Add( tmp );
	}
	return found;
}

////////////////////////////////////////////////////
// The main parse method
////////////////////////////////////////////////////
LPCTSTR SVObjectScriptParserSVXClass::Parse( SVObjectClass* pOwner, LPCTSTR tstrParseString, SVObjectScriptAliasListClass* pExternalAliasTable /*= NULL*/ )
{
//	static SVFunctionProfiler profile(_T("SVObjectScriptParserSVXClass::Parse"));
	//static SVFunctionProfiler profile1(_T("SVObjectScriptParserSVXClass::Parse1"));
	//static SVFunctionProfiler profile2(_T("SVObjectScriptParserSVXClass::Parse2"));
	//static SVFunctionProfiler profile3(_T("SVObjectScriptParserSVXClass::Parse3"));
	//static SVFunctionProfiler profile4(_T("SVObjectScriptParserSVXClass::Parse4"));
//	SVFunctionProfilerLocal profiler(profile);
	ParserOperandTypeEnum currentParseState = PARSE;
	int currentTokenIndex = 0;
	LPTSTR tStrParse = (LPTSTR)tstrParseString;

	if( tStrParse )
	{
		SVExpressionStack	expressionStack;
		//expressionStack.SetSize(0, 50);

		SVObjectInfoStruct localOwnerInfo;
		localOwnerInfo = pOwner;
		SVObjectScriptAliasListClass localAliasTable;

		int parseLength = static_cast<int>(_tcslen( tStrParse ));

		while( currentParseState != PARSER_ERROR && currentParseState != PARSE_END && 
				currentTokenIndex < parseLength )
		{
			SVObjectScriptAliasListClass mergeAliasTable;

			//SVFunctionProfilerLocal profiler4(profile4);
			//profile1.Begin();
			UpdateProgress( currentTokenIndex, parseLength );

			if( currentParseState == PARSE )
			{
				// localAliasTable not used here...
				ParseStatements( tStrParse, parseLength, currentTokenIndex, expressionStack, localOwnerInfo, localAliasTable, currentParseState );
			}
			
			UpdateProgress( currentTokenIndex, parseLength );

			//profile1.End();

			if( currentParseState == PROCESS_COMMAND )
			{
				if( expressionStack.GetSize() < 1 )
				{
#ifdef SVOBJECTSCRIPTPARSER_DEBUG
					TRACE("PROCESS_COMMAND = Empty Expression stack\n");
#endif
					ASSERT(FALSE);
					currentParseState = PARSER_ERROR;
					continue;
				}

				if( expressionStack.GetAt( 0 ) == TSTR_RIGHT_BRACE )
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
				//SVObjectScriptAliasListClass currentAliasTable(localAliasTable);
				//SVObjectScriptAliasListClass* pCurrentAliasTable = &currentAliasTable;
				SVObjectScriptAliasListClass copyOfLocalAliasTable;
				bool bAlwaysCopyAliasTable = false;

				SVObjectScriptAliasListClass* pCurrentAliasTable = NULL;
				//SVObjectScriptAliasListClass currentAliasTable;
				//currentAliasTable.Copy( localAliasTable );
				if( pExternalAliasTable )
				{
					pCurrentAliasTable = pExternalAliasTable;
					
					/*
					static SVFunctionProfiler profile(_T("SVObjectScriptParserSVXClass::Parse - copy alias table"));
					SVFunctionProfilerLocal profiler(profile);
					std::insert_iterator<SVObjectScriptAliasListClass::MapType > insert_iter(currentAliasTable.m_mapAlias, currentAliasTable.m_mapAlias.begin());
					std::copy( pExternalAliasTable->m_mapAlias.begin(), pExternalAliasTable->m_mapAlias.end(),
						 insert_iter);
					*/
					

					//currentAliasTable.Append( *pExternalAliasTable );
				}
				else
				{
					//copyOfLocalAliasTable = localAliasTable;
					pCurrentAliasTable = &copyOfLocalAliasTable;
					bAlwaysCopyAliasTable = true;
//					static SVSizeProfiler countprofileLocalAlias(_T("COUNT SVObjectScriptParserSVXClass::Parse - empty local alias table (root)"));
//					countprofileLocalAlias.Add(0);
				}

				
				{	// block for profiler
					//static SVSizeProfiler sizeprofileLocal(_T("SIZE SVObjectScriptParserSVXClass::Parse - local alias table"));
					//sizeprofileLocal.Add(localAliasTable.m_mapAlias.size());
					//static SVSizeProfiler sizeprofileGlobal(_T("SIZE SVObjectScriptParserSVXClass::Parse - global alias table"));
					//sizeprofileGlobal.Add(pCurrentAliasTable->m_mapAlias.size());
//					static SVFunctionProfiler profileCopyAlias(_T("SVObjectScriptParserSVXClass::Parse - copy alias table"));
//					static SVFunctionProfiler profileCopyAliasNew(_T("SVObjectScriptParserSVXClass::Parse - copy alias new"));
//					static SVFunctionProfiler profileCopyAliasOverwrite(_T("SVObjectScriptParserSVXClass::Parse - copy alias overwrite"));
//					SVFunctionProfilerLocal profiler(profileCopyAlias);


					SVObjectScriptAliasListClass::MapType::iterator iterLocal;
					for ( iterLocal = localAliasTable.m_mapAlias.begin(); iterLocal != localAliasTable.m_mapAlias.end(); ++iterLocal)
					{
						if (bAlwaysCopyAliasTable || iterLocal->second.SubstituteOperand.Dirty())
						{
							//pCurrentAliasTable->m_mapAlias[iterLocal->first] = iterLocal->second;	// slower than below
							SVObjectScriptAliasListClass::MapType::iterator iterCurrent = pCurrentAliasTable->m_mapAlias.find(iterLocal->first);
							if ( iterCurrent == pCurrentAliasTable->m_mapAlias.end() )
							{
								//SVFunctionProfilerLocal profiler(profileCopyAliasNew);
								pCurrentAliasTable->m_mapAlias[iterLocal->first] = iterLocal->second;
								//pCurrentAliasTable->m_mapAlias.insert(iterCurrent, *iterLocal);	// about the same speed
							}
							else
							{
								//SVFunctionProfilerLocal profiler(profileCopyAliasOverwrite);
								SVObjectScriptAliasStruct& rLocal = iterLocal->second;
								SVObjectScriptAliasStruct& rCurrent = iterCurrent->second;
								//ASSERT(FALSE);
								iterCurrent->second = iterLocal->second;
							}
							iterLocal->second.SubstituteOperand.Dirty() = false;
						}// end if (iterLocal->second.SubstituteOperand.Dirty())
						else
						{
							//TRACE("Not Dirty: %s\n", iterLocal->second.TStrAlias);
							int i=0;
							i++;
						}
					}// end for ( iterLocal = localAliasTable.m_mapAlias.begin(); iterLocal != localAliasTable.m_mapAlias.end(); iterLocal++)
					
					

					// doesn't work; insert does not overwrite
					//std::insert_iterator<SVObjectScriptAliasListClass::MapType > insert_iter(pCurrentAliasTable->m_mapAlias, pCurrentAliasTable->m_mapAlias.begin());
					//std::copy( localAliasTable.m_mapAlias.begin(), localAliasTable.m_mapAlias.end(),
					//	 insert_iter);
				}// end block for profiler
				
				
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

				//profile2.Begin();
				// Check for alias command...
				if( CheckKeywordTable( expressionStack.GetAt( 0 ) ) == ALIAS )
				{
					// currentAliasTable not used here
					if( !ProcessAlias( expressionStack, localOwnerInfo, *pCurrentAliasTable, localAliasTable ) )
					{
						ASSERT(FALSE);
						currentParseState = PARSER_ERROR;
					}
					else
						currentParseState = PARSE;

					CleanUpExpressionStack( expressionStack );
					//profile2.End();
					continue;				
				} // Check for alias command...
				//profile2.End();

				// Check other commands...

				// Check first operand...
				SVObjectScriptOperandStruct firstOperand;
				int index = 0;
				if( EvaluateOperandExpression( ALIAS | OBJECT | CLASS | EMBEDDED | MEMBER_ACCESS, localOwnerInfo, *pCurrentAliasTable, firstOperand, expressionStack, index, localAliasTable ) && 
					firstOperand.Value()
				  )
				{
					// Check for Object_Define command...
					// must be ...
					// alias for embedded_expression|class_expression or
					// embedded_expression|class_expression
					if( firstOperand.Type() == CLASS || firstOperand.Type() == EMBEDDED )
					{
						BOOL bStepIn = FALSE;
						SVObjectClass* pObject;
						pObject = ProcessDefineObject( expressionStack, localOwnerInfo, *pCurrentAliasTable, firstOperand, bStepIn, localAliasTable );
						if(! pObject )
						{
							ASSERT(FALSE);
							currentParseState = PARSER_ERROR;
							TRACE("Failed to Create Object-{%08X}\n",*(DWORD*)firstOperand.Value());
						}
						else
						{
							if( bStepIn )
							{
								// step in...
//								profile.End();
								//profile3.End();
								//profile4.End();
								LPCTSTR tStrNewParsPos = Parse( pObject, &tStrParse[ currentTokenIndex ] , pCurrentAliasTable );
//								profile.Begin();
								//profile4.Begin();
								//profile3.Begin();
								if( ! tStrNewParsPos  )
								{
									ASSERT(FALSE);
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
					if( firstOperand.Type() == MEMBER_NAME )
					{
						if( !ProcessMemberAssignment( expressionStack, localOwnerInfo, *pCurrentAliasTable, firstOperand, localAliasTable ) )
						{
							currentParseState = PARSER_ERROR;
							TRACE("Failed to Set Object.Member value\n");
						}
						else
							currentParseState = PARSE;

						CleanUpExpressionStack( expressionStack );
						continue;				
					} // Check for Object.Member Assignment
				
				} // Check first operand... EvaluateOperandExpression

				// unknown command...
				TRACE("Unknown Command\n");
				ASSERT(FALSE);
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
		return tstrParseString + currentTokenIndex;
	}
	return NULL;
}

void SVObjectScriptParserSVXClass::RemoveExternalDependencies( CString& script )
{
	int startIndex = 0;
	int pos;
	int len;
	CString keyWord = _T( "object$" );

	CStringArray objectGuidStrList;
	SVVector< SVObjectScriptInputObjectTokenStruct > inputObjectGuidList;
	CStringArray scriptStack;

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
		if( FindCharacters( script, loc, _T( " " ), true ) )
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
			loc = pos + static_cast<int>(_tcsclen( keyWord ));
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
			loc = pos + static_cast<int>(_tcsclen( keyWord ));
			if( ( endGuidPos = script.Find( endGuidIdentifier, loc ) ) != -1 )
			{
				endGuidPos += endGuidIdentifier.GetLength();

				len = endGuidPos - loc;

				guidStr = script.Mid( loc, len );
				
				// Add the Guid to the Script Stack
				int index = static_cast<int>(scriptStack.Add( guidStr ));

				// Save it's Guid and Location in the Script Stack
				SVObjectScriptInputObjectTokenStruct token;
				token.strGuid = guidStr;
				token.iIndex = index;
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

		SVObjectScriptInputObjectTokenStruct& inputObjectToken = inputObjectGuidList.GetAt(i);
		for( int j = 0;j < objectGuidStrList.GetSize(); j++ )
		{
			guidStr = objectGuidStrList.GetAt( j );
			if( !guidStr.Compare( inputObjectToken.strGuid ) )
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
			scriptStack.SetAt( inputObjectToken.iIndex, guidStr );
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
bool SVObjectScriptParserSVXClass::FindCharacters(CString& str, int& loc, LPCTSTR characters, bool reverse )
{
	return FindCharacters((LPCTSTR)str, str.GetLength(), loc, characters, reverse );
}

// returns when one of the specified characters is not found
// Sets loc to the location of the last encountered character
bool SVObjectScriptParserSVXClass::FindCharacters(LPCTSTR tStr, int tStrLen, int& loc, LPCTSTR characters, bool reverse )
{
	//static SVFunctionProfiler profile(_T("SVObjectScriptParserSVXClass::findCharacters(LPCTSTR)"));
	//SVFunctionProfilerLocal profiler(profile);
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

bool SVObjectScriptParserSVXClass::ReattachInputs( SVObjectClass* pObject, SVObjectScriptOperandList& RInputOperandList )
{
	bool l_bOk = true;

	//static SVFunctionProfiler profile(_T("SVObjectScriptParserSVXClass::reAttachInputs"));
	//SVFunctionProfilerLocal profiler(profile);
	SVInputInfoListClass inputInfoList;

	::SVSendMessage(pObject, SVM_GET_INPUT_INTERFACE, reinterpret_cast<DWORD_PTR>(&inputInfoList), NULL );

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
		SVObjectScriptOperandStruct& operand = RInputOperandList.GetAt( i );

		// Check if valid GUID type
		if( operand.ValueSize() == sizeof( GUID ) && *( ( GUID* ) operand.Value() ) != SVInvalidGUID )
		{
			SVGUID inputGuid = *( ( GUID* ) operand.Value() );

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

						if( TheSVObserverApp.getLoadingVersion() < 0x00044B00 && pInInfo == l_pAuxInfo )
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
bool SVObjectScriptParserSVXClass::ParseStatements( LPCTSTR ParseStr, int parseLength, int& currentIndex, SVExpressionStack& rExpressionStack, SVObjectInfoStruct& rLocalOwnerInfo, SVObjectScriptAliasListClass& rAliasTable, ParserOperandTypeEnum& rParseState )
{
	//static SVFunctionProfiler profile(_T("SVObjectScriptParserSVXClass::parseStatements"));
	//SVFunctionProfilerLocal profiler(profile);
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

		while( rParseState == PARSE && tokenIndex < statementLen )
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
							CString strTok = ExtractToken( tStrCurrentToken, tokenSize-1 );
							if( strTok.GetAllocLength() > 0 )
							{
								// If there was an expression, put it on the stack...
								if( !strTok.IsEmpty() )
								{
									rExpressionStack.Add( strTok );

#ifdef SVOBJECTSCRIPTPARSER_DEBUG
									TRACE( "Add Expression: %.80s\n", strTok );
#endif
								}
								else
								{
									
								}
							}
							else
							{
								// error
								TRACE("ExtractToken (1) Failed \n");
								ASSERT(FALSE);
								rParseState = PARSER_ERROR;
							}
						}
						// Put current separator on the stack...
						CString strSep;
						TCHAR* tsz = strSep.GetBufferSetLength(2);
						tsz[ 0 ] = currentChar;
						tsz[ 1 ] = TCHAR( '\0' );
						strSep.ReleaseBuffer(1);

						rExpressionStack.Add( strSep );

						// Stop parsing and start processing command, if you know
						//	this is a complete command terminated by ; ( semicolon )
						//	this is a object definition and we are entering the
						//		object's local definition block { .... };
						int index;
							
						if( currentChar == TCHAR( ';' ) ||
							currentChar == TCHAR( '{' ) ||
							( index = static_cast<int>(rExpressionStack.GetUpperBound()) ) >= 0 && 
							rExpressionStack.GetAt( index ) && 
							*rExpressionStack.GetAt( index ) == TCHAR( ')' ) 
						  )
						{
							// Process Command And Parse Object Content
							rParseState = PROCESS_COMMAND;
						}
						// point to next possible token
						tStrCurrentToken = &tStatement[tokenIndex];
						tokenSize = 0;
					}
					break;

					case TCHAR( '\"' ):	// Read String SubstituteOperand
					{
						rParseState = READ_STRING;

						if( GetStringToken( rExpressionStack, tStatement, statementLen, tokenIndex ) )
						{
							rParseState = PARSE;
						}
						else
						{
							// error
							TRACE("ExtractToken (2) Failed \n");
							ASSERT(FALSE);
							rParseState = PARSER_ERROR;
						}
						// point to next possible token
						tStrCurrentToken = &tStatement[tokenIndex];
						tokenSize = 0;
					}
					break;

					case TCHAR( '\'' ): // start delimiter for names
					{
						rParseState = READ_NAME_STRING;

						if( GetNameStringToken( rExpressionStack, tStatement, statementLen, tokenIndex ) )
						{
							rParseState = PARSE;
						}
						else
						{
							// error
							TRACE("ExtractToken (3) Failed \n");
							ASSERT(FALSE);
							rParseState = PARSER_ERROR;
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
							CString strTok = ExtractToken( tStrCurrentToken, tokenSize-1 );
							if( strTok.GetAllocLength() > 0 )
							{
								// If there was an expression, ckeck it...
								if( strTok.GetLength() > 0 )
								{
									rExpressionStack.Add( strTok );
								}
								else // empty string
								{
									
								}
							}
							else
							{
								// error
								TRACE("ExtractToken (4) Failed \n");
								ASSERT(FALSE);
								rParseState = PARSER_ERROR;
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
						CString strTok = ExtractToken( tStrCurrentToken, tokenSize );
						if( strTok.GetAllocLength() > 0 )
						{
							// If there was an expression, ckeck it...
							if( strTok.GetLength() > 0 )
							{
								// If there was an expression, put it on the stack...
								rExpressionStack.Add( strTok );

#ifdef SVOBJECTSCRIPTPARSER_DEBUG
								TRACE( "Add Expression: %.80s\n", strTok );
#endif

								if( GetGuidStringToken( rExpressionStack, tStatement, statementLen, tokenIndex ) )
								{
									rParseState = PARSE;
								}
								else
								{
									ASSERT(FALSE);
									rParseState = PARSER_ERROR;
								}
							}
							else // empty string
							{
								
							}
						}
						else
						{
							// error
							TRACE("ExtractToken (6) Failed \n");
							ASSERT(FALSE);
							rParseState = PARSER_ERROR;
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
							CString strTok = ExtractToken( tStrCurrentToken, tokenSize - 1 );
							if( strTok.GetAllocLength() > 0 )
							{
								// If there was an expression, ckeck it...
								if( strTok.GetLength() > 0 )
								{
									rExpressionStack.Add( strTok );
								}
								else // empty string
								{
									
								}
							}
							else
							{
								// error
								TRACE("ExtractToken (7) Failed \n");
								ASSERT(FALSE);
								rParseState = PARSER_ERROR;
							}
						}
						// Put current separator on the stack...
						CString strSep;
						TCHAR* tsz = strSep.GetBufferSetLength(2);
						tsz[ 0 ] = currentChar;
						tsz[ 1 ] = TCHAR( '\0' );
						strSep.ReleaseBuffer(1);

						rExpressionStack.Add( strSep );
							
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
	if( rParseState == PARSER_ERROR )
		return false;

	currentIndex += tokenIndex;

	return true;
}

///////////////////////////////////////////////////////////////
// Extract the String Token
///////////////////////////////////////////////////////////////
bool SVObjectScriptParserSVXClass::GetStringToken( SVExpressionStack& rExpressionStack, LPCTSTR tStatement, int statementLen, int& tokenIndex )
{
	//static SVFunctionProfiler profile(_T("SVObjectScriptParserSVXClass::getStringToken"));
	//SVFunctionProfilerLocal profiler(profile);
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

				CString strToken;
				LPTSTR tsz = strToken.GetBufferSetLength(tokenSize + 1);
					
				// Empty String is allowed
				if( tokenSize )
					memcpy( tsz, tStrCurrentToken, tokenSize );

				tsz[tokenSize] = TCHAR( '\0' );
				strToken.ReleaseBuffer( tokenSize );
				rExpressionStack.Add( strToken );

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
bool SVObjectScriptParserSVXClass::GetNameStringToken( SVExpressionStack& rExpressionStack, LPCTSTR tStatement, int statementLen, int& tokenIndex )
{
	//static SVFunctionProfiler profile(_T("SVObjectScriptParserSVXClass::getNameStringToken"));
	//SVFunctionProfilerLocal profiler(profile);
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
				if (!FindMembers( tStrCurrentToken, tokenSize, rExpressionStack ) )
				{
					// Add the token to the stack
					if( tokenSize )
					{
						CString strToken;
						LPTSTR tsz = strToken.GetBufferSetLength( tokenSize + 1 );
						memcpy( tsz, tStrCurrentToken, tokenSize );
						tsz[tokenSize] = TCHAR( '\0' );
						strToken.ReleaseBuffer( tokenSize );
						rExpressionStack.Add( strToken );
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
bool SVObjectScriptParserSVXClass::GetGuidStringToken( SVExpressionStack& rExpressionStack, LPCTSTR tStatement, int statementlen, int& tokenIndex )
{
	//static SVFunctionProfiler profile(_T("SVObjectScriptParserSVXClass::getGuidStringToken"));
	//SVFunctionProfilerLocal profiler(profile);
	//	get complete GUID token and put it on the expression stack
	//  GUID-Token: e.g. { 0x2ddd26a9, 0x79a7, 0x11d3, { 0xa8, 0x90, 0x0, 0x12, 0x0, 0x0, 0x5, 0xaa } }

	// Read complete GUID token and put token on the stack...
	int count = 0;
	
	CString strTok = ExtractGUIDToken( &tStatement[ tokenIndex ], count );
	if( strTok.GetAllocLength() > 0 )
	{
		// If there was an expression, put it on the stack...
		if( strTok.GetLength() > 0 )
		{
			rExpressionStack.Add( strTok );

#ifdef SVOBJECTSCRIPTPARSER_DEBUG
			TRACE( "Add Expression: %.80s\n", tStrTok );
#endif

			tokenIndex += count;
			return true;
		}
		else
		{
			
		}

		AfxMessageBox( "ParserState: READ_GUID_STRING\nParser Error, empty token found..." );
		return false;
	}

	AfxMessageBox( "ParserState: READ_GUID_STRING\nParser Error, GUID expression cannot be evaluated..." );
	return false;
}

///////////////////////////////////////////////////////////////
// Process the Alias Command
///////////////////////////////////////////////////////////////
bool SVObjectScriptParserSVXClass::ProcessAlias( SVExpressionStack& rExpressionStack, SVObjectInfoStruct& rLocalOwnerInfo, SVObjectScriptAliasListClass& rCurrentAliasTable, SVObjectScriptAliasListClass& rLocalAliasTable )
{
	//static SVFunctionProfiler profile(_T("SVObjectScriptParserSVXClass::processAlias"));
	//SVFunctionProfilerLocal profiler(profile);
	bool rc = false;

#ifdef SVOBJECTSCRIPTPARSER_DEBUG
	TRACE( "ALIAS\n" );
#endif

	SVObjectScriptOperandStruct operand1, operand2;

	// Check operands and operators...
	int numberOfExpressions = static_cast<int>(rExpressionStack.GetSize());
	int index = 1;

	// operand1
	// must be a name_expression
	if( ! EvaluateOperandExpression( NAME, rLocalOwnerInfo, rCurrentAliasTable, operand1, rExpressionStack, index, rLocalAliasTable ) ||
		! operand1.Value() 
	  )
	{
		AfxMessageBox( "Parser Error:\nName operand expected!" );
		return rc;
	}

	// must be operator =
	if( ++index >= numberOfExpressions || _tcscmp( rExpressionStack.GetAt( index ), _T( "=" ) ) )
	{
		AfxMessageBox( "Parser Error:\nOperator = expected!" );
		return rc;
	}

	// operand2
	// must be a embedded_expression|class_expression|object_expression
	if( ! EvaluateOperandExpression( ALIAS | OBJECT | CLASS | EMBEDDED, rLocalOwnerInfo, rLocalAliasTable, operand2, rExpressionStack, ++index, rLocalAliasTable ) ||
		! operand2.Value() 
	  )
	{
		AfxMessageBox( "Parser Error:\nObject operand, class operand or embedded operand expected!" );
		return rc;
	}

	// Check for operator ; 
	if(	++index >= numberOfExpressions || _tcscmp( rExpressionStack.GetAt( index ), _T( ";" ) ) )
	{
		AfxMessageBox( "Parser Error:\nCommand termination expected!" );
		return rc;
	}

	// Now we must be at the end of the stack...
	if(	index != rExpressionStack.GetUpperBound() )
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

	if( _tcscmp( (LPTSTR) operand1.Value(), strName ) == 0 &&
		*(GUID*)( operand2.Value() ) == SVBarCodeResultClassGuid )
	{
#ifndef SMALLMEMORYMANAGER
		free( (LPTSTR) operand1.Value() );
#else
		operand1.CleanUp();
#endif
		strName.LoadString( IDS_CLASSNAME_SVBARCODEANALYZERESULT );

#ifndef SMALLMEMORYMANAGER
		//operand1.PValue = _tcsdup( strName );
		operand1.SetValue(_tcsdup( strName ), static_cast<int>(_tcslen( strName ) + 1));
#else
		int iLen = _tcslen( strName ) + 1;
		char* pNewMem = (char*)SmallMemoryMalloc(iLen);
		_tcscpy(pNewMem, strName);
		//operand1.ValueSize = iLen;
		//operand1.PValue = pNewMem;
		operand1.SetValue(pNewMem, iLen);
#endif
	}// end if
	// *** // ***

#ifdef _DEBUG
	SVObjectScriptAliasListClass::MapType::iterator iter = rLocalAliasTable.m_mapAlias.find(CString(( LPTSTR ) operand1.Value()));
	if ( iter != rLocalAliasTable.m_mapAlias.end() )
	{
		int a=0;
		a++;
	}

#endif
	rLocalAliasTable.m_mapAlias[CString(( LPTSTR ) operand1.Value())] = SVObjectScriptAliasStruct( ( LPTSTR ) operand1.Value(), operand2 );
	//rAliasTable.Add( SVObjectScriptAliasStruct( ( LPTSTR ) operand1.PValue, operand2 ) );

#ifdef SVOBJECTSCRIPTPARSER_DEBUG
	TRACE("Adding Alias: %.80s\n", ( LPTSTR ) operand1.Value() );
#endif

	return rc;
}

///////////////////////////////////////////////////////////////
// Process Define Object - Constructs Dynamic Objects
///////////////////////////////////////////////////////////////
SVObjectClass* SVObjectScriptParserSVXClass::ProcessDefineObject( SVExpressionStack& rExpressionStack, SVObjectInfoStruct& rLocalOwnerInfo, SVObjectScriptAliasListClass& rAliasTable, SVObjectScriptOperandStruct& rFirstOperand, BOOL& rbStepIn, SVObjectScriptAliasListClass& rAliasTableForOptimization )
{
	//static SVFunctionProfiler profile(_T("SVObjectScriptParserSVXClass::processDefineObject"));
	//SVFunctionProfilerLocal profiler(profile);
	SVObjectClass* pObject = NULL;

	int index = 0;

#ifdef SVOBJECTSCRIPTPARSER_DEBUG
	TRACE( "OBJECT_DEFINE\n" );
#endif

	SVObjectScriptOperandStruct embeddedOperand, classOperand, nameOperand, objectOperand, objectOwnerOperand;
	SVObjectScriptOperandList inputOperandList;
	rbStepIn = FALSE;
	
	// Check operands and operators...
	if( rFirstOperand.Type() == EMBEDDED )
	{
		embeddedOperand = rFirstOperand;
		// Try to read class operand...
		if( ! EvaluateOperandExpression( ALIAS | CLASS, rLocalOwnerInfo, rAliasTable, classOperand, rExpressionStack, ++index, rAliasTableForOptimization ) ||
			! classOperand.Value()
		  )
		{
			AfxMessageBox( "Parser Error:\nClass operand expected!" );
			return pObject;
		}
	}
	else
		classOperand = rFirstOperand;

	// nameOperand
	// must be a name_Expression
	if( ! EvaluateOperandExpression( NAME, rLocalOwnerInfo, rAliasTable, nameOperand, rExpressionStack, ++index, rAliasTableForOptimization ) ||
		! nameOperand.Value()
	  )
	{
		AfxMessageBox( "Parser Error:\nName operand expected!" );
		return pObject;
	}

	// must be operator =
	if( ++index >= rExpressionStack.GetSize() || _tcscmp( rExpressionStack.GetAt( index ), _T( "=" ) ) )
	{
		AfxMessageBox( "Parser Error:\nOperator = expected!" );
		return pObject;
	}

	// objectOperand
	// must be a object_expression
	if( ! EvaluateOperandExpression( ALIAS | OBJECT, rLocalOwnerInfo, rAliasTable, objectOperand, rExpressionStack, ++index, rAliasTableForOptimization ) ||
		! objectOperand.Value()
	  )
	{
		AfxMessageBox( "Parser Error:\nOperator operand expected!" );
//		SV_PARSE_MESSAGE( "Parser Error:\nObject operand expected!\n\n*%s*", tStrParseEnd );
		return pObject;
	}

	if( ++index >= rExpressionStack.GetSize() )
	{
		AfxMessageBox( "Parser Error:\nUnexpected end of command found!" );
		return pObject;
	}

	// could be operator @
	if(	! _tcscmp( rExpressionStack.GetAt( index ), _T( "@" ) ) )
	{
		// Try to read owner...
		if( ! EvaluateOperandExpression( ALIAS | OBJECT, rLocalOwnerInfo, rAliasTable, objectOwnerOperand, rExpressionStack, ++index, rAliasTableForOptimization ) ||
			! objectOwnerOperand.Value()
		  )
		{
			AfxMessageBox( "Parser Error:\nObject operand expected!" );
			return pObject;
		}
		objectOwnerOperand.Type() = OBJECT_OWNER;

#ifdef SVOBJECTSCRIPTPARSER_DEBUG
		TRACE( "OBJECT_DEFINE, @\n" );
#endif

		if( ++index >= rExpressionStack.GetSize() )
		{
			AfxMessageBox( "Parser Error:\nUnexpected end of command found!" );
			return pObject;
		}
	}
	else
	{
		// No explicit object owner given, use local owner...
		//objectOwnerOperand.ValueSize = sizeof( GUID );
#ifndef SMALLMEMORYMANAGER
		//objectOwnerOperand.PValue = malloc( sizeof( GUID ) );
		objectOwnerOperand.SetValue( malloc( sizeof( GUID ) ), sizeof(GUID) );
#else
		//objectOwnerOperand.PValue = SmallMemoryMalloc( sizeof( GUID ) );
		objectOwnerOperand.SetValue( SmallMemoryMalloc( sizeof( GUID ) ), sizeof(GUID) );
#endif
		if( objectOwnerOperand.Value() )
		{
			// substitute operand...
			memcpy( objectOwnerOperand.Value(), &rLocalOwnerInfo.UniqueObjectID, sizeof( GUID ) );
			objectOwnerOperand.Type() = OBJECT_OWNER;
		}
		else
		{
			AfxMessageBox( "Parser Error:\nOut of memory!" );
			return pObject;
		}
	}

	// Check for input object list...
	// (...)
	if(	! _tcscmp( rExpressionStack.GetAt( index ), _T( "(" ) ) )
	{
		if( ! ReadOperandList( rLocalOwnerInfo, rAliasTable, inputOperandList, rExpressionStack, index ) )
		{
			AfxMessageBox( "Parser Error:\nUnable to read input list!" );
			return pObject;
		}

#ifdef SVOBJECTSCRIPTPARSER_DEBUG
		TRACE( "OBJECT_DEFINE, (...)\n" );
#endif

		if( ++index >= rExpressionStack.GetSize() )
		{
			AfxMessageBox( "Parser Error:\nUnexpected end of command found!" );
			return pObject;
		}
	}

	// Check for operator ; or operator {
	if(	_tcscmp( rExpressionStack.GetAt( index ), _T( ";" ) ) && 
		_tcscmp( rExpressionStack.GetAt( index ), _T( "{" ) ) 
	  )
	{
		AfxMessageBox( "Parser Error:\nCommand termination or object member definition expected!" );
		return pObject;
	}

	// Check for object member definition block...
	if( ! _tcscmp( rExpressionStack.GetAt( index ), _T( "{" ) ) )
	{
		rbStepIn = TRUE;
	}
	else
	{
		// Now we must be at the end of the stack...
		if(	index != rExpressionStack.GetUpperBound() )
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
	ownerObjectInfo.UniqueObjectID = *( ( GUID* ) objectOwnerOperand.Value() );
	ownerObjectInfo.PObject = SVObjectManagerClass::Instance().GetObject( ownerObjectInfo.UniqueObjectID );

	if( ! ownerObjectInfo.PObject )
	{
		AfxMessageBox( "Parser Error:\n No valid object owner given!" );
		return pObject;
	}

	// Check for embedded...
	if( embeddedOperand.Value() || embeddedOperand.Type() == EMBEDDED )
	{
		if( embeddedOperand.Value() && embeddedOperand.Type() == EMBEDDED )
		{
#ifdef SVOBJECTSCRIPTPARSER_DEBUG
			TRACE( "OBJECT_DEFINE, embedded SVM_OVERWRITE_OBJECT\n" ); 
#endif

			// Send to Owner of Embedded Object
			// Try to overwrite object...
			if( pObject = reinterpret_cast<SVObjectClass*>( ::SVSendMessage( ownerObjectInfo.PObject, 
			                                                  SVM_OVERWRITE_OBJECT, 
			                                                 reinterpret_cast<DWORD_PTR>(objectOperand.Value()), 
			                                                 reinterpret_cast<DWORD_PTR>(embeddedOperand.Value()) )) )
			{
#ifdef SVOBJECTSCRIPTPARSER_DEBUG
				TRACE( "OverWriteObject %.80s\n", pObject->GetName() );
#endif
			}

			ASSERT( pObject != NULL );
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
		SVObjectManagerClass::Instance().ConstructObject( *( ( GUID* ) classOperand.Value() ), pObject );

#ifdef SVOBJECTSCRIPTPARSER_DEBUG
		TRACE( "OBJECT_DEFINE, new object SVM_REPLACE_OBJECT\n" );
#endif

		if( pObject && objectOwnerOperand.Value() )
		{
			// Try to replace or add object...
			if( ::SVSendMessage( ownerObjectInfo.PObject, SVM_REPLACE_OBJECT, reinterpret_cast<DWORD_PTR>(objectOperand.Value()), reinterpret_cast<DWORD_PTR>(pObject) ) != SVMR_SUCCESS )
			{
				ASSERT( FALSE );
				TRACE( "ReplaceObject %.80s\n", rLocalOwnerInfo.PObject->GetName() );

				delete pObject;

				TRACE( "after delete, SVM_REPLACE_OBJECT was not successfully" );
			}
			else
			{
				ReattachInputs( pObject, inputOperandList );
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
		pObject->SetName( ( LPTSTR ) nameOperand.Value() );

#ifdef SVOBJECTSCRIPTPARSER_DEBUG
		TRACE( "%.80s\n",pObject->GetName() );
#endif
		
		//rc = true;				
	}
	else
	{
		int x = 5;
	}
	ASSERT(pObject);
	return pObject;
}

/////////////////////////////////////////////////////////////
// Process Member Assignment
/////////////////////////////////////////////////////////////
bool SVObjectScriptParserSVXClass::ProcessMemberAssignment( SVExpressionStack& rExpressionStack, SVObjectInfoStruct& rLocalOwnerInfo, SVObjectScriptAliasListClass& rAliasTable, SVObjectScriptOperandStruct& rFirstOperand, SVObjectScriptAliasListClass& rAliasTableForOptimization )
{
	//static SVFunctionProfiler profile(_T("SVObjectScriptParserSVXClass::processMemberAssignment"));
	//SVFunctionProfilerLocal profiler(profile);
	bool rc = false;

	int index = 2;

#ifdef SVOBJECTSCRIPTPARSER_DEBUG
	TRACE( "Member ASSIGNMENT\n" );
#endif
						
	SVObjectAttributeClass dataObject;

	// must be member name
	dataObject.SetName( ( LPCTSTR )rFirstOperand.Value() );

	// Get Member's Owner (embeddedObject)
	SVObjectScriptOperandStruct objectOperand, objectOwnerOperand;


	// Set Embedded Object's Owner
	//objectOwnerOperand.ValueSize = sizeof( GUID );
#ifndef SMALLMEMORYMANAGER
	//objectOwnerOperand.PValue = malloc( sizeof( GUID ) );
	objectOwnerOperand.SetValue( malloc( sizeof( GUID ) ), sizeof(GUID) );
#else
	//objectOwnerOperand.PValue = SmallMemoryMalloc( sizeof( GUID ) );
	objectOwnerOperand.SetValue( SmallMemoryMalloc( sizeof( GUID ) ), sizeof(GUID) );
#endif
	if( objectOwnerOperand.Value() )
	{
		// substitute operand...
		memcpy( objectOwnerOperand.Value(), &rLocalOwnerInfo.UniqueObjectID, sizeof( GUID ) );
		objectOwnerOperand.Type() = OBJECT_OWNER;
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
	if( !EvaluateOperandExpression( ALIAS | OBJECT, rLocalOwnerInfo, rAliasTable, objectOperand, rExpressionStack, i, rAliasTableForOptimization ) ||
		! objectOperand.Value()
	  )
	{
		AfxMessageBox( "Parser Error:\nOperator operand expected!" );
		//SV_PARSE_MESSAGE( "Parser Error:\nObject operand expected!\n\n*%s*", tStrParseEnd );
		return rc;
	}
	
	// Check operands and operators...

	// must be operator =
	if( ++index >= rExpressionStack.GetSize() || _tcscmp( rExpressionStack.GetAt( index ), _T( "=" ) ) )
	{
		AfxMessageBox( "Parser Error:\nOperator = expected!" );
		return rc;
	}
	// skip past operator =
	
	if( ++index >= rExpressionStack.GetSize() )
	{
		AfxMessageBox( "Parser Error:\nUnexpected end of command found!" );
		return rc;
	}

	// must be DataType and Array [ ] of decimal, hexadecimal, or floating point numbers
	// OR DataType and single decimal, hexadecimal, floating point number

	if( ! ReadValues( dataObject, rExpressionStack, index ) )
	{
		AfxMessageBox( "Parser Error:\nValues expected!" );
		return rc;
	}

	// Check for operator ;
	if(	_tcscmp( rExpressionStack.GetAt( index ), _T( ";" ) ) )
	{
		AfxMessageBox( "Parser Error:\nCommand termination or object member definition expected!" );
		return rc;
	}
	
	// Process command...
#ifdef SVOBJECTSCRIPTPARSER_DEBUG
	TRACE( "SET_OBJECT_VALUE, Process command\n" );
#endif

	SVObjectInfoStruct objectInfo, ownerObjectInfo;

	objectInfo.UniqueObjectID = *( ( GUID* ) objectOperand.Value() );
	objectInfo.PObject = SVObjectManagerClass::Instance().GetObject( objectInfo.UniqueObjectID );

	if( ! objectInfo.PObject )
	{
		AfxMessageBox( "Parser Error:\n No valid object given!" );
		return rc;
	}
	
	ownerObjectInfo.UniqueObjectID = *( ( GUID* ) objectOwnerOperand.Value() );
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
		if( ::SVSendMessage( ownerObjectInfo.PObject, SVM_SET_OBJECT_VALUE, reinterpret_cast<DWORD_PTR>(&objectInfo.UniqueObjectID), reinterpret_cast<DWORD_PTR>(&dataObject) ) != SVMR_SUCCESS )
		{
			AfxMessageBox( "Parser Error:\n Set Object Member Value failed!" );
			return rc;
		}
		rc = true;
	}
	return rc;
}

size_t SVObjectScriptParserSVXClass::GetTotal() const
{
	return m_pParseString->GetLength();
}

HRESULT SVObjectScriptParserSVXClass::DoParse()
{
	LPCTSTR pScript = Parse( m_pOwnerObject, *m_pParseString );
	return (pScript != NULL) ? S_OK : E_FAIL;
}

