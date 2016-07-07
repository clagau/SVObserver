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
#pragma region Includes
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
#include "ObjectInterfaces/ErrorNumbers.h"
#include "TextDefinesSvO.h"
#include "SVOMFCLibrary/SVDeviceParams.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//namespace NS_ObjectScriptParserSVX
//{
const TCHAR* const TSTR_RIGHT_BRACE = _T( "}" );
//}// end NS_ObjectScriptParserSVX

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVObjectScriptAliasListClass::SVObjectScriptAliasListClass()
{
}

SVObjectScriptAliasListClass::~SVObjectScriptAliasListClass()
{
	CleanUp();
}

SVObjectScriptAliasListClass::SVObjectScriptAliasListClass(const SVObjectScriptAliasListClass& rhs)
{
	m_mapAlias = rhs.m_mapAlias;
}

void SVObjectScriptAliasListClass::CleanUp()
{
	m_mapAlias.clear();
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

// Cut leading and successor whitespaces away...
CString SVObjectScriptParserSVXClass::ExtractToken( LPCTSTR TStrTokenString, int TokenStringLength )
{
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
	if( TStrTokenString )
	{
		CString sToken(TStrTokenString);
		SVObjectScriptAliasListClass::MapType::const_iterator iter;
		iter = rCurrentAliasTable.m_mapAlias.find(sToken);
		if ( iter != rCurrentAliasTable.m_mapAlias.end() )
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL SVObjectScriptParserSVXClass::CheckAliasTable( const SVObjectScriptAliasListClass& rCurrentAliasTable, LPCTSTR TStrTokenString, SVObjectScriptOperandStruct& rOperand)
{
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
	}
	return FALSE;
}

ParserOperandTypeEnum SVObjectScriptParserSVXClass::CheckKeywordTable( LPCTSTR tstrExpression )
{
	// Returns iKeyIndex!!!
	if( tstrExpression )
	{
		// Check keyword table...
		for( int i = 0; i < m_KeywordTable.GetSize(); ++ i )
		{
			SVObjectScriptKeywordStruct& keywordEntry = m_KeywordTable.GetAt( i );
			if( ! _tcscmp( tstrExpression, keywordEntry.tstrKeyword ) )
			{
				return keywordEntry.eKeyIndex;
			}
		}
	}
	return NOP;
}

void SVObjectScriptParserSVXClass::CleanUpExpressionStack( SVExpressionStack& rExpressionStack )
{
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
						szToken = _tcstok( nullptr, szSep );
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
	LPCTSTR ptr = rExpressionStack.GetAt( riIndex );
	if( ptr[0] == TCHAR( '{' ) && ptr[1] != TCHAR( '\0' ) )
	{
		// Convert the GUID String Expression to a Guid...
		AfxGetClassIDFromString( rExpressionStack.GetAt( riIndex ), &rUniqueID );
		return TRUE;
	}

#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
	TRACE( "ReadGUIDExpression: Failed\n" );
#endif
	return FALSE;
}


BOOL SVObjectScriptParserSVXClass::EvaluateOperandExpression( int OperandType, const SVObjectInfoStruct& rOwnerInfo, SVObjectScriptAliasListClass& rCurrentAliasTable, SVObjectScriptOperandStruct& rOperand, SVExpressionStack& rExpressionStack, int& riIndex, SVObjectScriptAliasListClass& rAliasTableForOptimization )
{
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
				SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
				Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_NameMustNotContainKeyword, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10073 );
				return FALSE;
			}

			if( bExpressionNext )
			{
				rOperand.CleanUp();

#ifndef SMALLMEMORYMANAGER
				rOperand.SetValue(_tcsdup( rExpressionStack.GetAt( riIndex ) ), static_cast<int>(_tcslen( rExpressionStack.GetAt( riIndex ) ) + 1));
#else
				int iLen = _tcslen( rExpressionStack.GetAt( riIndex ) ) + 1;
				char* pNewMem = (char*)SmallMemoryMalloc(iLen);
				_tcscpy(pNewMem, rExpressionStack.GetAt( riIndex ));
				rOperand.SetValue(pNewMem, iLen);
#endif
				if( rOperand.Value() )
				{
					rOperand.Type() = NAME;
					// Success...
					return TRUE;
				}
				
				SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
				Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_OutOfMemory, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10074 );
				return FALSE;
			}

			SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_NameExpressionExpected, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10075 );
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
						SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
						Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_OwnerNotExist, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10076 );
						return FALSE;
					}

					// No alias allowed for member access operands...
					SVObjectScriptAliasListClass emptyAliasTable;
					// Try to evaluate operand...
					OperandType &= (~ALIAS);					
					return EvaluateOperandExpression( OperandType, ownerInfo, emptyAliasTable, rOperand, rExpressionStack, ++riIndex, emptyAliasTable );
				}
			}
			SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_UnexpectedMember, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10077 );
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

#ifndef SMALLMEMORYMANAGER
				rOperand.SetValue(malloc( sizeof( GUID ) ), sizeof(GUID) );
#else
				rOperand.SetValue(SmallMemoryMalloc( sizeof( GUID ) ), sizeof(GUID) );
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

					SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
					Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_CannotReadGuid, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10078 );
					return FALSE;
				}

				SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
				Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_OutOfMemory, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10079 );
				return FALSE;
			}
			else
			{
				SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
				Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_ExpressionExpected, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10080 );
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
				if( SV_GUID_NULL != rOwnerInfo.UniqueObjectID )
				{
					pObject = reinterpret_cast<SVObjectClass*>(::SVSendMessage( rOwnerInfo.UniqueObjectID, SVM_GET_OBJECT_BY_NAME, reinterpret_cast<DWORD_PTR>( static_cast<LPCTSTR>( rExpressionStack.GetAt( riIndex ))), 0 ));
				}
				if( nullptr != pObject )
				{
					rOperand.CleanUp();
				
#ifndef SMALLMEMORYMANAGER
					rOperand.SetValue( malloc( sizeof( GUID ) ), sizeof(GUID) );
#else
					rOperand.SetValue( SmallMemoryMalloc( sizeof( GUID ) ), sizeof(GUID) );
#endif
					if( rOperand.Value() )
					{
						memcpy( rOperand.Value(), &pObject->GetUniqueObjectID(), sizeof( GUID ) );
						rOperand.Type() = OBJECT;

						bExpressionNext = FALSE;
						continue;
					}
					
					rOperand.CleanUp();

					SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
					Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_OutOfMemory, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10081 );
					return FALSE;
				}
				else
				{
					// Not Known
				
					// Check for Unknown member access
					if( ( OperandType & ( MEMBER_ACCESS | ALIAS) ) == MEMBER_ACCESS )
					{
						rOperand.CleanUp();

#ifndef SMALLMEMORYMANAGER
						rOperand.SetValue(_tcsdup( rExpressionStack.GetAt( riIndex ) ), static_cast<int>(_tcslen( rExpressionStack.GetAt( riIndex ) ) + 1));
#else
						int iLen = _tcslen( rExpressionStack.GetAt( riIndex ) ) + 1;
						char* pNewMem = (char*)SmallMemoryMalloc(iLen);
						_tcscpy(pNewMem, rExpressionStack.GetAt( riIndex ));
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
				SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
				Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_ExpressionExpected, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10082 );
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
#ifndef SMALLMEMORYMANAGER
						operand2.SetValue( malloc( sizeof( GUID ) ), sizeof(GUID) );
#else
						operand2.SetValue( SmallMemoryMalloc( sizeof( GUID ) ), sizeof(GUID) );
#endif
						(*(GUID*)operand2.Value()) = SVBarCodeResultClassGuid;
						operand2.Type() = CLASS;

						rCurrentAliasTable.m_mapAlias[strName] = SVObjectScriptAliasStruct( strName, operand2 );
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
						bExpressionNext = FALSE;
						continue;
					}
					rOperand.CleanUp();

					SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
					Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_OutOfMemory, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10083 );

					return FALSE;
				}
				else // must be member name ???
				{
					int x = 777;
				}
			}
			else
			{
				SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
				Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_ExpressionExpected, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10084 );
				return FALSE;
			}
		}

		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_UnknownExpression, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10085 );
		return FALSE;
	}
	
	SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
	Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_UnexpectedEndOfCommand, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10086 );
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

#ifndef SMALLMEMORYMANAGER
				operand.SetValue( malloc( sizeof( GUID ) ), sizeof(GUID) );
#else
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
	BOOL rc = FALSE;

	// Check current index...
	if( riIndex >= rExpressionStack.GetSize() )
		return rc;

	// Check for value(s) begin...
	dataObject.SetType( CheckDataTypeKeywordTable( rExpressionStack.GetAt( riIndex ) ) );
	// Should we have specified a type
	if( dataObject.GetSVObjectScriptDataObjectTypeEnum() == SV_UNKNOWN_Type )
		return rc;

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
				ASSERT( S_OK == hr );
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
LPCTSTR SVObjectScriptParserSVXClass::Parse( SVObjectClass* pOwner, LPCTSTR tstrParseString, SVObjectScriptAliasListClass* pExternalAliasTable /*= nullptr*/ )
{
	ParserOperandTypeEnum currentParseState = PARSE;
	int currentTokenIndex = 0;
	LPTSTR tStrParse = (LPTSTR)tstrParseString;

	if( tStrParse )
	{
		SVExpressionStack	expressionStack;

		SVObjectInfoStruct localOwnerInfo;
		localOwnerInfo = pOwner;
		SVObjectScriptAliasListClass localAliasTable;

		int parseLength = static_cast<int>(_tcslen( tStrParse ));

		while( currentParseState != PARSER_ERROR && currentParseState != PARSE_END && 
				currentTokenIndex < parseLength )
		{
			SVObjectScriptAliasListClass mergeAliasTable;

			UpdateProgress( currentTokenIndex, parseLength );

			if( currentParseState == PARSE )
			{
				// localAliasTable not used here...
				ParseStatements( tStrParse, parseLength, currentTokenIndex, expressionStack, localOwnerInfo, localAliasTable, currentParseState );
			}
			
			UpdateProgress( currentTokenIndex, parseLength );

			if( currentParseState == PROCESS_COMMAND )
			{
				if( expressionStack.GetSize() < 1 )
				{
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
				SVObjectScriptAliasListClass copyOfLocalAliasTable;
				bool bAlwaysCopyAliasTable = false;

				SVObjectScriptAliasListClass* pCurrentAliasTable = nullptr;
				if( pExternalAliasTable )
				{
					pCurrentAliasTable = pExternalAliasTable;
				}
				else
				{
					pCurrentAliasTable = &copyOfLocalAliasTable;
					bAlwaysCopyAliasTable = true;
				}

				
				{	// block
					SVObjectScriptAliasListClass::MapType::iterator iterLocal;
					for ( iterLocal = localAliasTable.m_mapAlias.begin(); iterLocal != localAliasTable.m_mapAlias.end(); ++iterLocal)
					{
						if (bAlwaysCopyAliasTable || iterLocal->second.SubstituteOperand.Dirty())
						{
							SVObjectScriptAliasListClass::MapType::iterator iterCurrent = pCurrentAliasTable->m_mapAlias.find(iterLocal->first);
							if ( iterCurrent == pCurrentAliasTable->m_mapAlias.end() )
							{
								pCurrentAliasTable->m_mapAlias[iterLocal->first] = iterLocal->second;
							}
							else
							{
								SVObjectScriptAliasStruct& rLocal = iterLocal->second;
								SVObjectScriptAliasStruct& rCurrent = iterCurrent->second;
								iterCurrent->second = iterLocal->second;
							}
							iterLocal->second.SubstituteOperand.Dirty() = false;
						}// end if (iterLocal->second.SubstituteOperand.Dirty())
						else
						{
							int i=0;
							i++;
						}
					}// end for ( iterLocal = localAliasTable.m_mapAlias.begin(); iterLocal != localAliasTable.m_mapAlias.end(); iterLocal++)
				}// end block
				
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
					// currentAliasTable not used here
					if( !ProcessAlias( expressionStack, localOwnerInfo, *pCurrentAliasTable, localAliasTable ) )
					{
						ASSERT(FALSE);
						currentParseState = PARSER_ERROR;
					}
					else
						currentParseState = PARSE;

					CleanUpExpressionStack( expressionStack );
					continue;				
				} // Check for alias command...

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
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
							TRACE("Failed to Create Object-{%08X}\n",*(DWORD*)firstOperand.Value());
#endif
						}
						else
						{
							if( bStepIn )
							{
								// step in...
								LPCTSTR tStrNewParsPos = Parse( pObject, &tStrParse[ currentTokenIndex ] , pCurrentAliasTable );
								if( ! tStrNewParsPos  )
								{
									ASSERT(FALSE);
									SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
									SVStringArray msgList;
									msgList.push_back(pObject->GetName());
									Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_Error_ScriptParseFailed, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10030 ); 
									
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
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
							TRACE("Failed to Set Object.Member value\n");
#endif
						}
						else
							currentParseState = PARSE;

						CleanUpExpressionStack( expressionStack );
						continue;				
					} // Check for Object.Member Assignment
				
				} // Check first operand... EvaluateOperandExpression

				// unknown command...
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
				TRACE("Unknown Command\n");
#endif
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
	return nullptr;
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

	SVInputInfoListClass inputInfoList;

	::SVSendMessage(pObject, SVM_GET_INPUT_INTERFACE, reinterpret_cast<DWORD_PTR>(&inputInfoList), 0 );

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
		if( operand.ValueSize() == sizeof( GUID ) && SV_GUID_NULL != *( ( GUID* ) operand.Value() ) )
		{
			SVGUID inputGuid = *( ( GUID* ) operand.Value() );

			SVInObjectInfoStruct* pInInfo = nullptr;

			if( l_lOperandSize == l_lInfoSize )
			{
				pInInfo = inputInfoList.GetAt( i );
			}
			else
			{
				if( nullptr != l_psvTool && i + l_lOffset < l_lInfoSize )
				{
					do
					{
						pInInfo = inputInfoList.GetAt( i + l_lOffset );

						if( TheSVObserverApp.getLoadingVersion() < 0x00044B00 && pInInfo == l_pAuxInfo )
						{
							pInInfo = nullptr;

							l_lOffset++;
						}
					}
					while( nullptr == pInInfo && i + l_lOffset < l_lInfoSize );
				}
				else
				{
					l_bOk = false;
				}
			}

			if( nullptr != pInInfo )
			{
				pInInfo->SetInputObject( inputGuid );
			}
			else
			{
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
				TRACE("Could not find - Object Input is not Valid\n");
#endif

				l_bOk = false;
			}
		}
		else // Invalid GUID operand
		{
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
			TRACE("Object Input is not a Valid GUID type\n");
#endif

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
								}
								else
								{
									
								}
							}
							else
							{
								// error
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
								TRACE("ExtractToken (1) Failed \n");
#endif
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
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
							TRACE("ExtractToken (2) Failed \n");
#endif
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
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
							TRACE("ExtractToken (3) Failed \n");
#endif
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
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
								TRACE("ExtractToken (4) Failed \n");
#endif
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
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
							TRACE("ExtractToken (6) Failed \n");
#endif
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
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
								TRACE("ExtractToken (7) Failed \n");
#endif
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

				return true;
			}
		}
	}
	// Error, unexpected end of parse string...
	SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
	Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_UnexpectedEndOfParse, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10088 );
	return false;
}

///////////////////////////////////////////////////////////////
// Extract the Name String Token
///////////////////////////////////////////////////////////////
bool SVObjectScriptParserSVXClass::GetNameStringToken( SVExpressionStack& rExpressionStack, LPCTSTR tStatement, int statementLen, int& tokenIndex )
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
	SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
	Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_UnexpectedEndOfName, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10089 );
	return false;
}

///////////////////////////////////////////////////////////////
// Extract the Guid String Token
///////////////////////////////////////////////////////////////
bool SVObjectScriptParserSVXClass::GetGuidStringToken( SVExpressionStack& rExpressionStack, LPCTSTR tStatement, int statementlen, int& tokenIndex )
{
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
			tokenIndex += count;
			return true;
		}
		else
		{
		}

		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_EmptyToken, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10090 );
		return false;
	}

	SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
	Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_GuidCannotEvaluated, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10091 );
	return false;
}

///////////////////////////////////////////////////////////////
// Process the Alias Command
///////////////////////////////////////////////////////////////
bool SVObjectScriptParserSVXClass::ProcessAlias( SVExpressionStack& rExpressionStack, SVObjectInfoStruct& rLocalOwnerInfo, SVObjectScriptAliasListClass& rCurrentAliasTable, SVObjectScriptAliasListClass& rLocalAliasTable )
{
	bool rc = false;

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
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_NameOperandExpected, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10092 );
		return rc;
	}

	// must be operator =
	if( ++index >= numberOfExpressions || _tcscmp( rExpressionStack.GetAt( index ), _T( "=" ) ) )
	{
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_OperatorExpected, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10093 );
		return rc;
	}

	// operand2
	// must be a embedded_expression|class_expression|object_expression
	if( ! EvaluateOperandExpression( ALIAS | OBJECT | CLASS | EMBEDDED, rLocalOwnerInfo, rLocalAliasTable, operand2, rExpressionStack, ++index, rLocalAliasTable ) ||
		! operand2.Value() 
	  )
	{
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_OperandExpected, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10094 );
		return rc;
	}

	// Check for operator ; 
	if(	++index >= numberOfExpressions || _tcscmp( rExpressionStack.GetAt( index ), _T( ";" ) ) )
	{
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_CommandTerminationExpected, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10095 );
		return rc;
	}

	// Now we must be at the end of the stack...
	if(	index != rExpressionStack.GetUpperBound() )
	{
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_TooManyExpressions, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10096 );
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
		operand1.SetValue(_tcsdup( strName ), static_cast<int>(_tcslen( strName ) + 1));
#else
		int iLen = _tcslen( strName ) + 1;
		char* pNewMem = (char*)SmallMemoryMalloc(iLen);
		_tcscpy(pNewMem, strName);
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

	return rc;
}

///////////////////////////////////////////////////////////////
// Process Define Object - Constructs Dynamic Objects
///////////////////////////////////////////////////////////////
SVObjectClass* SVObjectScriptParserSVXClass::ProcessDefineObject( SVExpressionStack& rExpressionStack, SVObjectInfoStruct& rLocalOwnerInfo, SVObjectScriptAliasListClass& rAliasTable, SVObjectScriptOperandStruct& rFirstOperand, BOOL& rbStepIn, SVObjectScriptAliasListClass& rAliasTableForOptimization )
{
	SVObjectClass* pObject = nullptr;

	int index = 0;

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
			SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_ClassOperandExpected, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10097 );
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
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_NameOperandExpected, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10098 );
		return pObject;
	}

	// must be operator =
	if( ++index >= rExpressionStack.GetSize() || _tcscmp( rExpressionStack.GetAt( index ), _T( "=" ) ) )
	{
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_OperatorExpected, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10099 );
		return pObject;
	}

	// objectOperand
	// must be a object_expression
	if( ! EvaluateOperandExpression( ALIAS | OBJECT, rLocalOwnerInfo, rAliasTable, objectOperand, rExpressionStack, ++index, rAliasTableForOptimization ) ||
		! objectOperand.Value()
	  )
	{
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_OperatorOperand, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10100 );
		return pObject;
	}

	if( ++index >= rExpressionStack.GetSize() )
	{
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_UnexpectedEndOfCommand, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10101 );
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
			SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_ObjectOperandExpected, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10102 );
			return pObject;
		}
		objectOwnerOperand.Type() = OBJECT_OWNER;

		if( ++index >= rExpressionStack.GetSize() )
		{
			SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_UnexpectedEndOfCommand, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10103 );
			return pObject;
		}
	}
	else
	{
		// No explicit object owner given, use local owner...
#ifndef SMALLMEMORYMANAGER
		objectOwnerOperand.SetValue( malloc( sizeof( GUID ) ), sizeof(GUID) );
#else
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
			SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_OutOfMemory, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10104 );
			return pObject;
		}
	}

	// Check for input object list...
	// (...)
	if(	! _tcscmp( rExpressionStack.GetAt( index ), _T( "(" ) ) )
	{
		if( ! ReadOperandList( rLocalOwnerInfo, rAliasTable, inputOperandList, rExpressionStack, index ) )
		{
			SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_UnableToReadInputList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10105 );
			return pObject;
		}

		if( ++index >= rExpressionStack.GetSize() )
		{
			SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_UnexpectedEndOfCommand, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10106 );
			return pObject;
		}
	}

	// Check for operator ; or operator {
	if(	_tcscmp( rExpressionStack.GetAt( index ), _T( ";" ) ) && 
		_tcscmp( rExpressionStack.GetAt( index ), _T( "{" ) ) 
	  )
	{
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_CommandTerminationOrObjectMemberExpected, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10107 );
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
			SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_TooManyExpressions, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10108 );
			return pObject;
		}
	}

	// Process command...
	SVObjectInfoStruct ownerObjectInfo;
	ownerObjectInfo.UniqueObjectID = *( ( GUID* ) objectOwnerOperand.Value() );
	ownerObjectInfo.PObject = SVObjectManagerClass::Instance().GetObject( ownerObjectInfo.UniqueObjectID );

	if( ! ownerObjectInfo.PObject )
	{
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_NoValidObjectOwner, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10109 );
		return pObject;
	}

	// Check for embedded...
	if( embeddedOperand.Value() || embeddedOperand.Type() == EMBEDDED )
	{
		if( embeddedOperand.Value() && embeddedOperand.Type() == EMBEDDED )
		{
			// Send to Owner of Embedded Object
			// Try to overwrite object...
			if( pObject = reinterpret_cast<SVObjectClass*>( ::SVSendMessage( ownerObjectInfo.PObject, 
			                                                  SVM_OVERWRITE_OBJECT, 
			                                                 reinterpret_cast<DWORD_PTR>(objectOperand.Value()), 
			                                                 reinterpret_cast<DWORD_PTR>(embeddedOperand.Value()) )) )
			{
			}

			ASSERT( nullptr != pObject );
		}
		else
		{
			SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_WrongEmbeddedValues, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10110 );
			return pObject;
		}
	}
	else
	{
		// Construct new object...
		SVObjectManagerClass::Instance().ConstructObject( *( ( GUID* ) classOperand.Value() ), pObject );

		if( pObject && objectOwnerOperand.Value() )
		{
			// Try to replace or add object...
			if( ::SVSendMessage( ownerObjectInfo.PObject, SVM_REPLACE_OBJECT, reinterpret_cast<DWORD_PTR>(objectOperand.Value()), reinterpret_cast<DWORD_PTR>(pObject) ) != SVMR_SUCCESS )
			{
				ASSERT( FALSE );
#if defined (TRACE_THEM_ALL) || defined (TRACE_SVXPARSER)
				TRACE( "ReplaceObject %.80s\n", rLocalOwnerInfo.PObject->GetName() );
#endif

				delete pObject;

#if defined (TRACE_THEM_ALL) || defined (TRACE_SVXPARSER)
				TRACE( "after delete, SVM_REPLACE_OBJECT was not successfully" );
#endif
			}
			else
			{
				ReattachInputs( pObject, inputOperandList );
			}
		}
		else
		{
			SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_CannotConstructObject, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10111 );
			return pObject;
		
		}
	}

	/////////////////////////////////////////////////////////////////////
	// Both Embedded and Dynamic Objects come to this point
	/////////////////////////////////////////////////////////////////////
	if( pObject )
	{
		pObject->SetName( ( LPTSTR ) nameOperand.Value() );
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
	bool rc = false;

	int index = 2;

	SVObjectAttributeClass dataObject;

	// must be member name
	dataObject.SetName( ( LPCTSTR )rFirstOperand.Value() );

	// Get Member's Owner (embeddedObject)
	SVObjectScriptOperandStruct objectOperand, objectOwnerOperand;


	// Set Embedded Object's Owner
#ifndef SMALLMEMORYMANAGER
	objectOwnerOperand.SetValue( malloc( sizeof( GUID ) ), sizeof(GUID) );
#else
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
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_ObjectRequired, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10112 );
		return rc;
	}
	
	// objectOperand
	// must be a object_expression
	if( !EvaluateOperandExpression( ALIAS | OBJECT, rLocalOwnerInfo, rAliasTable, objectOperand, rExpressionStack, i, rAliasTableForOptimization ) ||
		! objectOperand.Value()
	  )
	{
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_OperatorOperand, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10113 );
		return rc;
	}
	
	// Check operands and operators...

	// must be operator =
	if( ++index >= rExpressionStack.GetSize() || _tcscmp( rExpressionStack.GetAt( index ), _T( "=" ) ) )
	{
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_OperatorExpected, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10114 );
		return rc;
	}
	// skip past operator =
	
	if( ++index >= rExpressionStack.GetSize() )
	{
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_UnexpectedEndOfCommand, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10115 );
		return rc;
	}

	// must be DataType and Array [ ] of decimal, hexadecimal, or floating point numbers
	// OR DataType and single decimal, hexadecimal, floating point number

	if( ! ReadValues( dataObject, rExpressionStack, index ) )
	{
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_ValuesExpected, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10116 );
		return rc;
	}

	// Check for operator ;
	if(	_tcscmp( rExpressionStack.GetAt( index ), _T( ";" ) ) )
	{
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_CommandTerminationOrObjectMemberExpected, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10117 );
		return rc;
	}
	
	// Process command...
	SVObjectInfoStruct objectInfo, ownerObjectInfo;

	objectInfo.UniqueObjectID = *( ( GUID* ) objectOperand.Value() );
	objectInfo.PObject = SVObjectManagerClass::Instance().GetObject( objectInfo.UniqueObjectID );

	if( ! objectInfo.PObject )
	{
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_NoValidObject, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10118 );
		return rc;
	}
	
	ownerObjectInfo.UniqueObjectID = *( ( GUID* ) objectOwnerOperand.Value() );
	ownerObjectInfo.PObject = SVObjectManagerClass::Instance().GetObject( ownerObjectInfo.UniqueObjectID );

	if( ! ownerObjectInfo.PObject )
	{
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_NoValidObjectOwner, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10119 );
		return rc;
	}
	
	/////////////////////////////////////////////////////////////////////
	// If an Object and it's Owner were Found
	/////////////////////////////////////////////////////////////////////
	if( ownerObjectInfo.PObject && objectInfo.PObject )
	{
		// try to set the object's member value...
		// Note:: Send this message to the Object's Owner
		if( ::SVSendMessage( ownerObjectInfo.PObject, SVM_SET_OBJECT_VALUE, reinterpret_cast<DWORD_PTR>(&objectInfo.UniqueObjectID), reinterpret_cast<DWORD_PTR>(&dataObject) ) != SVMR_SUCCESS )
		{
			SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ParseError_SetObjectMemberFailed, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10120 );
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
	return (nullptr != pScript) ? S_OK : E_FAIL;
}

