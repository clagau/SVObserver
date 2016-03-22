//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectScriptParser
//* .File Name       : $Workfile:   SVObjectScriptParserSVX.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   19 Dec 2014 04:14:46  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <map>
#include "SVObjectScriptParser.h"
#include "SVObjectLibrary/SVObjectScriptEnums.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVOMFCLibrary/SVFunctionProfiler.h"

//namespace NS_ObjectScriptParserSVX
//{

	enum ParserOperandTypeEnum
	{
		NOP                         = 0x00000000,		// Command
		OBJECT_DEFINE               = 0x00001000,		// Command
		OBJECT_REDEFINE             = 0x00002000,		// Command
		ALIAS                       = 0x00004000,		// Evaluate type, Command and Keyword		
		CLASS                       = 0x00000001,		// Evaluate type, Expression type ( GUID* ) and Keyword
		OBJECT                      = 0x00000002,		// Evaluate type, Expression type ( GUID* ) and Keyword
		EMBEDDED                    = 0x00000004,		// Evaluate type, Expression type ( GUID* ) and Keyword
		EMBEDDED_OBJECT             = 0x00000010,		// Expression type ( GUID* )
		OBJECT_OWNER                = 0x00000020,		// Expression type ( GUID* )
		OBJECT_INPUT                = 0x00000040,		// Expression type ( GUID* )
		NAME                        = 0x00000100,		// Evaluate type, Expression type ( LPTSTR )
		STRING                      = 0x00000200,		// Expression type ( LPTSTR )
		MEMBER_ACCESS               = 0x10000000,		// Evaluate type
		MEMBER_NAME                 = 0x20000000,		// Expression type
		MEMBER_VALUE                = 0x40000000,		// Expression type
		PARSE                       = 0x00100000,		// Parser state
		READ_STRING                 = 0x00200000,		// Parser state
		READ_NAME_STRING            = 0x00400000,		// Parser state
		READ_GUID_STRING            = 0x00800000,		// Parser state
		PROCESS_COMMAND             = 0x01000000,		// Parser state
		PARSER_ERROR                = 0xffffffff,		// Parser state
		PARSE_END                   = 0x80000000		// Parser state
	};// end enum ParserOperandTypeEnum


struct SVObjectScriptDataTypeKeywordStruct
{
	SVObjectScriptDataTypeKeywordStruct( LPCTSTR tstrKeyString = _T( "" ), SVObjectScriptDataObjectTypeEnum eType = SV_UNKNOWN_Type )
	{
		tstrKeyword = tstrKeyString;
		eKeyType    = eType;
	};

	CString tstrKeyword;
	SVObjectScriptDataObjectTypeEnum eKeyType;
};

struct SVObjectScriptKeywordStruct
{
	SVObjectScriptKeywordStruct( LPCTSTR tstrKeyString = _T( "" ), ParserOperandTypeEnum eKeyAction = NOP )
	{
		tstrKeyword = tstrKeyString;
		eKeyIndex	= eKeyAction;
	};

	LPCTSTR tstrKeyword;
	ParserOperandTypeEnum eKeyIndex;
};


struct SVObjectScriptOperandStruct
{
	inline SVObjectScriptOperandStruct( void* pVal = NULL, ParserOperandTypeEnum eType = NOP, int iSize = 0 );
	inline SVObjectScriptOperandStruct( const SVObjectScriptOperandStruct& rhs );
	
	inline ~SVObjectScriptOperandStruct();

	inline SVObjectScriptOperandStruct& operator=( const SVObjectScriptOperandStruct& rExp );

	inline void SetValue( void* pValue, int iSize, ParserOperandTypeEnum eType_ );
	inline void SetValue( void* pValue, int iSize );

	inline void CleanUp();

	inline int    ValueSize() {return m_iValueSize;} // rvalue
	inline void*  Value() {return m_pValue;}        // non-const rvalue
	inline ParserOperandTypeEnum&   Type() {return m_eType;}           // lvalue
	inline bool&  Dirty() {return m_bDirty;}        // lvalue

protected:
	int                    m_iValueSize;
	void*                  m_pValue;
	ParserOperandTypeEnum  m_eType;
	bool                   m_bDirty;
};

inline SVObjectScriptOperandStruct::SVObjectScriptOperandStruct( void* pVal, ParserOperandTypeEnum eType, int iSize )
{
	//static SVFunctionProfiler profile(_T("SVObjectScriptOperandStruct::SVObjectScriptOperandStruct(void, int, int)"));
	//SVFunctionProfilerLocal profiler(profile);
	m_pValue	= pVal;
	m_eType	= eType;
	m_iValueSize = iSize;
	m_bDirty = true;
};

inline SVObjectScriptOperandStruct::SVObjectScriptOperandStruct( const SVObjectScriptOperandStruct& rhs )
{
	//static SVFunctionProfiler profile(_T("SVObjectScriptOperandStruct::SVObjectScriptOperandStruct copy constructor"));
	//SVFunctionProfilerLocal profiler(profile);
	m_iValueSize	= rhs.m_iValueSize;
	m_eType		= rhs.m_eType;
	m_bDirty = true;

	//  Make a copy of the data
	if( m_iValueSize && rhs.m_pValue )
	{
		#ifndef SMALLMEMORYMANAGER
			void* newValue = malloc( m_iValueSize );
		#else
			void* newValue = SmallMemoryMalloc( m_iValueSize );
		#endif
		memcpy( newValue, rhs.m_pValue, m_iValueSize );

		m_pValue = newValue;
	}
	else
	{
		m_pValue = NULL;
	}
}

inline SVObjectScriptOperandStruct& SVObjectScriptOperandStruct::operator=( const SVObjectScriptOperandStruct& rExp )
{
	//static SVFunctionProfiler profile(_T("SVObjectScriptOperandStruct::operator ="));
	//static SVFunctionProfiler profileCopyOnly(_T("SVObjectScriptOperandStruct::operator = copy only "));
	//static SVFunctionProfiler profileReallocate(_T("SVObjectScriptOperandStruct::operator = reallocate"));
	//static SVFunctionProfiler profileMemoryManagement(_T("SVObjectScriptOperandStruct::operator = memory management"));
	//static SVFunctionProfiler profileMemcpy(_T("SVObjectScriptOperandStruct::operator = memcpy"));
	//SVFunctionProfilerLocal profiler(profile);
	m_bDirty = true;
	if ( m_iValueSize == rExp.m_iValueSize )// no need to free and reallocate
	{
		//SVFunctionProfilerLocal profiler(profileCopyOnly);
		//m_iValueSize	= rExp.m_iValueSize;
		m_eType		= rExp.m_eType;
		//  Make a copy of the data
		if( m_iValueSize && rExp.m_pValue && m_pValue )
		{
			memcpy( m_pValue, rExp.m_pValue, m_iValueSize );
		}// end if( ValueSize && rExp.PValue )
	}
	else
	{
		//SVFunctionProfilerLocal profiler(profileReallocate);
		m_iValueSize	= rExp.m_iValueSize;
		m_eType		= rExp.m_eType;

		//  Make a copy of the data
		if( m_iValueSize && rExp.m_pValue )
		{
			void* newValue;
			{// begin block
				//SVFunctionProfilerLocal profiler(profileMemoryManagement);
				#ifndef SMALLMEMORYMANAGER
					newValue = malloc( m_iValueSize );
				#else
					newValue = SmallMemoryMalloc( m_iValueSize );
				#endif

				if( m_pValue && rExp.m_pValue != m_pValue )
				{
					#ifndef SMALLMEMORYMANAGER
						free( m_pValue );
					#else
						SmallMemoryFree( m_pValue , m_iValueSize);
					#endif

					m_pValue = NULL;
				}
			}// end block

			{// begin block
				//SVFunctionProfilerLocal profiler(profileMemcpy);
				memcpy( newValue, rExp.m_pValue, m_iValueSize );
			}// end block

			m_pValue = newValue;
		}// end if( ValueSize && rExp.PValue )
	}

	return *this;
}

inline void SVObjectScriptOperandStruct::CleanUp()
{
	//static SVSizeProfiler sizeprofile(_T("SVObjectScriptOperandStruct::CleanUp"));
	//sizeprofile.Add(ValueSize);
	//static SVFunctionProfiler profile(_T("SVObjectScriptOperandStruct::CleanUp"));
	//SVFunctionProfilerLocal profiler(profile);
	if( m_pValue )
#ifndef SMALLMEMORYMANAGER
		free( m_pValue );
#else
		SmallMemoryFree( m_pValue, m_iValueSize);
#endif
	
	m_pValue     = NULL;
	m_eType      = NOP;
	m_iValueSize = 0;
	m_bDirty     = true;
}

inline void SVObjectScriptOperandStruct::SetValue( void* pValue_, int iSize_, ParserOperandTypeEnum eType_ )
{
	//static SVFunctionProfiler profile(_T("SVObjectScriptOperandStruct::SetValue(3)"));
	//SVFunctionProfilerLocal profiler(profile);
	CleanUp();
	m_pValue = pValue_;
	m_iValueSize = iSize_;
	m_eType = eType_;
	m_bDirty = true;
}

inline void SVObjectScriptOperandStruct::SetValue( void* pValue_, int iSize_ )
{
	//static SVFunctionProfiler profile(_T("SVObjectScriptOperandStruct::SetValue(2)"));
	//SVFunctionProfilerLocal profiler(profile);
	CleanUp();
	m_pValue = pValue_;
	m_iValueSize = iSize_;
	m_bDirty = true;
}

inline SVObjectScriptOperandStruct::~SVObjectScriptOperandStruct()
{
	//static SVFunctionProfiler profile(_T("SVObjectScriptOperandStruct::~SVObjectScriptOperandStruct"));
	//SVFunctionProfilerLocal profiler(profile);
	if( m_pValue )
#ifndef SMALLMEMORYMANAGER
		free( m_pValue );
#else
		SmallMemoryFree( m_pValue, m_iValueSize);
#endif
	// don't bother setting to null
}



typedef SVVector< SVObjectScriptOperandStruct > SVObjectScriptOperandList;



struct SVObjectScriptAliasStruct
{
	SVObjectScriptAliasStruct( LPCTSTR tstrAliasName, const SVObjectScriptOperandStruct& rSub )
	{
		tstrAlias			= tstrAliasName;
		SubstituteOperand	= rSub;
	};

	SVObjectScriptAliasStruct( const SVObjectScriptAliasStruct& rhs ) : tstrAlias(rhs.tstrAlias), SubstituteOperand(rhs.SubstituteOperand)
	{
	};

	SVObjectScriptAliasStruct()
	{
	};

	SVObjectScriptAliasStruct& operator = ( const SVObjectScriptAliasStruct& rhs )
	{
		tstrAlias			= rhs.tstrAlias;
		SubstituteOperand	= rhs.SubstituteOperand;
		
		return( *this );
	};


	CString						tstrAlias;
	SVObjectScriptOperandStruct SubstituteOperand;
};

//typedef SVArrayClass< LPTSTR, LPTSTR >	SVExpressionStack;	// should be a cstring
//typedef SVArrayClass< CString, CString >	SVExpressionStack;
typedef CStringArray SVExpressionStack;

//}// temporary end namespace NS_ObjectScriptParserSVX

/*
// these can't belong in the namespace scope
char* std::allocator<NS_ObjectScriptParserSVX::SVObjectScriptAliasStruct>::_Charalloc(size_t _N)
{
	//_N = 36;//sizeof(SVObjectScriptAliasStruct);
	#ifndef SMALLMEMORYMANAGER
		return (char*) malloc( _N );
	#else
		return (char*) SmallMemoryMalloc( 40 );
	#endif
}

void std::allocator<NS_ObjectScriptParserSVX::SVObjectScriptAliasStruct>::deallocate(void* _P, size_t _N)
{
	//_N = 36;//sizeof(SVObjectScriptAliasStruct);
	#ifndef SMALLMEMORYMANAGER
		free( _P );
	#else
		SmallMemoryFree( _P, 40 );
	#endif
}
//*/

//namespace NS_ObjectScriptParserSVX	// continue namespace
//{


class SVObjectScriptAliasListClass
{
public:
	SVObjectScriptAliasListClass();
	~SVObjectScriptAliasListClass();
	SVObjectScriptAliasListClass(const SVObjectScriptAliasListClass& rhs);
	void CleanUp();
public:
	typedef std::map<CString, SVObjectScriptAliasStruct> MapType;
	MapType m_mapAlias;
};

typedef SVVector< SVObjectScriptKeywordStruct > SVKeywordTable;
typedef SVVector< SVObjectScriptDataTypeKeywordStruct > SVDataTypeKeywordTable;

class SVObjectScriptParserSVXClass : public SVObjectScriptParserBase
{
public:
	SVObjectScriptParserSVXClass(unsigned long parserHandle,SVSharedPtr<CString> pScript, const GUID& OwnerGuid, SVObjectClass* pOwnerObject, CWnd* pWnd);
	virtual ~SVObjectScriptParserSVXClass();

	virtual size_t GetTotal() const;
	virtual HRESULT DoParse();
	
protected:
	LPCTSTR Parse( SVObjectClass* POwner, LPCTSTR TStrParseString, SVObjectScriptAliasListClass* PExternalAliasTable = NULL );

	CString ExtractToken( LPCTSTR TStrTokenString, int TokenStringLength );
	CString ExtractGUIDToken( LPCTSTR TStrTokenString, int& rCount );
	
	BOOL CheckAliasTable( const SVObjectScriptAliasListClass& rCurrentAliasTable, LPCTSTR TStrTokenString );
	BOOL CheckAliasTable( const SVObjectScriptAliasListClass& rCurrentAliasTable, LPCTSTR TStrTokenString, SVObjectScriptOperandStruct& rOperand );

	ParserOperandTypeEnum  CheckKeywordTable( LPCTSTR TStrExpression );
	void CleanUpExpressionStack( SVExpressionStack& rExpressionStack );
	BOOL ReadGUIDExpression( SVExpressionStack& rExpressionStack, GUID& rUniqueID, int& rIndex );
	BOOL EvaluateOperandExpression( int OperandType, const SVObjectInfoStruct& rOwnerInfo, SVObjectScriptAliasListClass& rCurrentAliasTable, SVObjectScriptOperandStruct& rOperand, SVExpressionStack& rExpressionStack, int& rIndex, SVObjectScriptAliasListClass& rAliasTableForOptimization );
	BOOL ReadOperandList( const SVObjectInfoStruct& rLocalOwnerInfo, const SVObjectScriptAliasListClass& rCurrentAliasTable, SVObjectScriptOperandList& rOperandList, SVExpressionStack& rExpressionStack, int& rIndex );
	BOOL ReadValues( SVObjectAttributeClass& dataObject, SVExpressionStack& rExpressionStack, int& rIndex );

	BOOL ExtractValue( SVObjectAttributeClass& dataObject, 
	                   SVExpressionStack& rExpressionStack, 
	                   int& rIndex );

	BOOL CheckValue( SVObjectAttributeClass& dataObject, LPCTSTR tokenStr );
	BOOL CheckNumber( LPCTSTR tokenStr );
	BOOL CheckString( LPCTSTR tokenStr );
	BOOL FindMembers( LPTSTR tokenStr, int tokenStrLen, SVExpressionStack& rExpressionStack );
	void RemoveExternalDependencies( CString& script );
	SVObjectScriptDataObjectTypeEnum CheckDataTypeKeywordTable( LPCTSTR TStrExpression );

	DWORD TStrToDWORD( LPCTSTR TStrInput );
	double TStrToDouble( LPCTSTR TStrInput );
	bool FindCharacters( CString& str, int& loc, LPCTSTR characters, bool reverse=false );
	bool FindCharacters( LPCTSTR pStr, int tStrLen, int& loc, LPCTSTR characters, bool reverse=false );
	bool ReattachInputs( SVObjectClass* pObject, SVObjectScriptOperandList& rInputOperandList );

	bool ParseStatements( LPCTSTR ParseStr, int parseLength, int& currentIndex, SVExpressionStack& rExpressionStack, SVObjectInfoStruct& rLocalOwnerInfo, SVObjectScriptAliasListClass& rAliasTable, ParserOperandTypeEnum& reParseState );

	bool GetStringToken( SVExpressionStack& rExpressionStack, LPCTSTR tStatement, int statementLen, int& tokenIndex );
	bool GetNameStringToken( SVExpressionStack& rExpressionStack, LPCTSTR tStatement, int statementLen, int& tokenIndex );
	bool GetGuidStringToken( SVExpressionStack& rExpressionStack, LPCTSTR tStatement, int statementlen, int& tokenIndex );

	bool ProcessAlias( SVExpressionStack& rExpressionStack, SVObjectInfoStruct& rLocalOwnerInfo, SVObjectScriptAliasListClass& rCurrentAliasTable, SVObjectScriptAliasListClass& rLocalAliasTable );
	SVObjectClass* ProcessDefineObject( SVExpressionStack& rExpressionStack, SVObjectInfoStruct& rLocalOwnerInfo, SVObjectScriptAliasListClass& rAliasTable, SVObjectScriptOperandStruct& rFirstOperand, BOOL& rbStepIn, SVObjectScriptAliasListClass& rAliasTableForOptimization );
	bool ProcessMemberAssignment( SVExpressionStack& rExpressionStack, SVObjectInfoStruct& rLocalOwnerInfo, SVObjectScriptAliasListClass& rAliasTable, SVObjectScriptOperandStruct& rFirstOperand, SVObjectScriptAliasListClass& rAliasTableForOptimization  );
	

	SVKeywordTable			m_KeywordTable;
	SVDataTypeKeywordTable	m_DataTypeKeywordTable;

	SVSharedPtr<CString> m_pParseString; // 
};	// end SVObjectScriptParserSVXClass

struct SVObjectScriptInputObjectTokenStruct
{
	SVObjectScriptInputObjectTokenStruct( CString& rGuidStr, int index ) : strGuid(rGuidStr), iIndex(index)
	{
	};

	SVObjectScriptInputObjectTokenStruct( const SVObjectScriptInputObjectTokenStruct& rhs )
	{
		strGuid	= rhs.strGuid;
		iIndex	= rhs.iIndex;
	};

	SVObjectScriptInputObjectTokenStruct()
	{
	};

	SVObjectScriptInputObjectTokenStruct& operator=( const SVObjectScriptInputObjectTokenStruct& rhs )
	{
		strGuid	= rhs.strGuid;
		iIndex	= rhs.iIndex;
		
		return( *this );
	};

	CString strGuid;	// the input object's Guid as a String
	int iIndex;			// index to access the script stack
};

