//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectScriptParser
//* .File Name       : $Workfile:   SVObjectScriptParserSEC.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:41:30  $
//******************************************************************************

#ifndef SVOBJECTSCRIPTPARSERSEC_H
#define SVOBJECTSCRIPTPARSERSEC_H

#include "SVObjectScriptParser.h"
#include "SVObjectLibrary/SVObjectScriptEnums.h"
#include "SVObjectLibrary/SVObjectClass.h"

// it was decided to rename all helper classes instead of partitioning into a namespace
//namespace NS_ObjectScriptParserSEC
//{

struct SVObjectScriptDataTypeKeywordStructSEC
{
	SVObjectScriptDataTypeKeywordStructSEC( LPCTSTR TStrKeyString = _T( "" ), SVObjectScriptDataObjectTypeEnum Type = SV_UNKNOWN_Type )
	{
		TStrKeyword = TStrKeyString;
		KeyType		= Type;
	};

	CString TStrKeyword;
	SVObjectScriptDataObjectTypeEnum KeyType;
};

struct SVObjectScriptKeywordStructSEC
{
	SVObjectScriptKeywordStructSEC( LPCTSTR TStrKeyString = _T( "" ), int KeyAction = 0 )
	{
		TStrKeyword = TStrKeyString;
		KeyIndex	= KeyAction;
	};

	LPCTSTR TStrKeyword;
	int KeyIndex;
};


struct SVObjectScriptOperandStructSEC
{
	SVObjectScriptOperandStructSEC( void* PVal = NULL, int Tpe = 0, int Sze = 0 );
	SVObjectScriptOperandStructSEC( const SVObjectScriptOperandStructSEC& R2 );
	
	~SVObjectScriptOperandStructSEC();

	SVObjectScriptOperandStructSEC& operator=( const SVObjectScriptOperandStructSEC& RExp );

	void CleanUp();

	int		ValueSize;
	void*	PValue;
	int		Type;
};


typedef SVVector< SVObjectScriptOperandStructSEC > SVObjectScriptOperandListSEC;



struct SVObjectScriptAliasStructSEC
{
	SVObjectScriptAliasStructSEC( LPCTSTR TStrAliasName, const SVObjectScriptOperandStructSEC& RSub )
	{
		TStrAlias			= TStrAliasName;
		SubstituteOperand	= RSub;
	};

	SVObjectScriptAliasStructSEC( const SVObjectScriptAliasStructSEC& R2 )
	{
		TStrAlias			= R2.TStrAlias;
		SubstituteOperand	= R2.SubstituteOperand;
	};

	SVObjectScriptAliasStructSEC()
	{
		//TStrAlias  = NULL;
	};

	SVObjectScriptAliasStructSEC& operator=( const SVObjectScriptAliasStructSEC& R2 )
	{
		TStrAlias			= R2.TStrAlias;
		SubstituteOperand	= R2.SubstituteOperand;
		
		return( *this );
	};


	//LPTSTR						TStrAlias;
	CString						TStrAlias;
	SVObjectScriptOperandStructSEC SubstituteOperand;
};

typedef SVVector< LPTSTR >	SVExpressionStackSEC;

class SVObjectScriptAliasListClassSEC : public SVVector< SVObjectScriptAliasStructSEC >
{
public:
	SVObjectScriptAliasListClassSEC();
	void CleanUp();
};

typedef SVVector< SVObjectScriptKeywordStructSEC > SVKeywordTableSEC;
typedef SVVector< SVObjectScriptDataTypeKeywordStructSEC > SVDataTypeKeywordTableSEC;

class SVObjectScriptParserSECClass : public SVObjectScriptParserBase
{
	friend class SVObjectScriptParserClass;

public:
	SVObjectScriptParserSECClass(unsigned long parserHandle, CString* pScript, const GUID& OwnerGuid, SVObjectClass* pOwnerObject, CWnd* pWnd);
	virtual ~SVObjectScriptParserSECClass();

	virtual size_t GetTotal() const;
	virtual HRESULT DoParse();

	LPTSTR							ExtractToken( LPCTSTR TStrTokenString, int TokenStringLength );
	LPTSTR							ExtractGUIDToken( LPCTSTR TStrTokenString, int& RCount );
	
	BOOL							CheckAliasTable( const SVObjectScriptAliasListClassSEC& RCurrentAliasTable, LPCTSTR TStrTokenString );
	BOOL							CheckAliasTable( const SVObjectScriptAliasListClassSEC& RCurrentAliasTable, LPCTSTR TStrTokenString, SVObjectScriptOperandStructSEC& ROperand );

	int								CheckKeywordTable( LPCTSTR TStrExpression );
	void							CleanUpExpressionStack( SVExpressionStackSEC& RExpressionStack );
	BOOL							ReadGUIDExpression( SVExpressionStackSEC& RExpressionStack, GUID& RUniqueID, int& RIndex );
	BOOL							EvaluateOperandExpression( int OperandType, const SVObjectInfoStruct& ROwnerInfo, const SVObjectScriptAliasListClassSEC& RCurrentAliasTable, SVObjectScriptOperandStructSEC& ROperand, SVVector< LPTSTR >& RExpressionStack, int& RIndex );
	BOOL							ReadOperandList( const SVObjectInfoStruct& RLocalOwnerInfo, const SVObjectScriptAliasListClassSEC& RCurrentAliasTable, SVObjectScriptOperandListSEC& ROperandList, SVVector< LPTSTR >& RExpressionStack, int& RIndex );
	BOOL							ReadValues( SVObjectAttributeClass& dataObject, SVExpressionStackSEC& RExpressionStack, int& RIndex );

	BOOL							ExtractValue( SVObjectAttributeClass& dataObject, 
	                                SVExpressionStackSEC& RExpressionStack, 
																	int& RIndex );

	BOOL							CheckValue( SVObjectAttributeClass& dataObject, LPCTSTR tokenStr );
	BOOL							CheckNumber( LPCTSTR tokenStr );
	BOOL							CheckString( LPCTSTR tokenStr );
	SVObjectScriptDataObjectTypeEnum CheckDataTypeKeywordTable( LPCTSTR TStrExpression );
	BOOL							FindMembers( LPTSTR tokenStr, int tokenStrLen, SVExpressionStackSEC& RExpressionStack );
	LPCTSTR							Parse( SVObjectClass* POwner, LPCTSTR TStrParseString, SVObjectScriptAliasListClassSEC* PExternalAliasTable = NULL );
	void							RemoveExternalDependencies( CString& script );

protected:
	DWORD tStrToDWORD( LPTSTR TStrInput );
	double tStrToDouble( LPTSTR TStrInput );
	bool findCharacters( CString& str, int& loc, LPCTSTR characters, bool reverse=false );
	bool findCharacters( LPCTSTR pStr, int tStrLen, int& loc, LPCTSTR characters, bool reverse=false );
	bool reAttachInputs( SVObjectClass* pObject, SVObjectScriptOperandListSEC& RInputOperandList );

	bool parseStatements( LPCTSTR ParseStr, int parseLength, int& currentIndex, SVExpressionStackSEC& RExpressionStack, SVObjectInfoStruct& RLocalOwnerInfo, SVObjectScriptAliasListClassSEC& RAliasTable, int& RParseState );

	bool getStringToken( SVExpressionStackSEC& RExpressionStack, LPCTSTR tStatement, int statementLen, int& tokenIndex );
	bool getNameStringToken( SVExpressionStackSEC& RExpressionStack, LPCTSTR tStatement, int statementLen, int& tokenIndex );
	bool getGuidStringToken( SVExpressionStackSEC& RExpressionStack, LPCTSTR tStatement, int statementlen, int& tokenIndex );

	bool processAlias( SVExpressionStackSEC& RExpressionStack, SVObjectInfoStruct& RLocalOwnerInfo, SVObjectScriptAliasListClassSEC& RCurrentAliasTable, SVObjectScriptAliasListClassSEC& RAliasTable );
	SVObjectClass* processDefineObject( SVExpressionStackSEC& RExpressionStack, SVObjectInfoStruct& RLocalOwnerInfo, SVObjectScriptAliasListClassSEC& RAliasTable, SVObjectScriptOperandStructSEC& RFirstOperand, BOOL& RBStepIn );
	bool processMemberAssignment( SVExpressionStackSEC& RExpressionStack, SVObjectInfoStruct& RLocalOwnerInfo, SVObjectScriptAliasListClassSEC& RAliasTable, SVObjectScriptOperandStructSEC& RFirstOperand );
	
	enum
	{
		NOP							=	0x00000000,		// Command
		OBJECT_DEFINE				=	0x00001000,		// Command
		OBJECT_REDEFINE				=	0x00002000,		// Command
		ALIAS						=	0x00004000,		// Evaluate type, Command and Keyword		
		CLASS						=	0x00000001,		// Evaluate type, Expression type ( GUID* ) and Keyword
		OBJECT						=	0x00000002,		// Evaluate type, Expression type ( GUID* ) and Keyword
		EMBEDDED					=	0x00000004,		// Evaluate type, Expression type ( GUID* ) and Keyword
		EMBEDDED_OBJECT				=	0x00000010,		// Expression type ( GUID* )
		OBJECT_OWNER				=	0x00000020,		// Expression type ( GUID* )
		OBJECT_INPUT				=	0x00000040,		// Expression type ( GUID* )
		NAME						=	0x00000100,		// Evaluate type, Expression type ( LPTSTR )
		STRING						=	0x00000200,		// Expression type ( LPTSTR )
		MEMBER_ACCESS				=	0x10000000,		// Evaluate type
		MEMBER_NAME					=	0x20000000,		// Expression type
		MEMBER_VALUE				=	0x40000000,		// Expression type
		PARSE						=	0x00100000,		// Parser state
		READ_STRING					=	0x00200000,		// Parser state
		READ_NAME_STRING			=	0x00400000,		// Parser state
		READ_GUID_STRING            =   0x00800000,		// Parser state
		PROCESS_COMMAND				=	0x01000000,		// Parser state
		PARSER_ERROR				=	0xffffffff,		// Parser state
		PARSE_END					=	0x80000000		// Parser state
	};


	SVKeywordTableSEC          keywordTable;
	SVDataTypeKeywordTableSEC  dataTypeKeywordTable;

	CString* m_pParseString; // 
};

struct SVObjectScriptInputObjectTokenStructSEC
{
	SVObjectScriptInputObjectTokenStructSEC( CString& RGuidStr, int index )
	{
		GuidStr	= RGuidStr;
		Index	= index;
	};

	SVObjectScriptInputObjectTokenStructSEC( const SVObjectScriptInputObjectTokenStructSEC& R2 )
	{
		GuidStr	= R2.GuidStr;
		Index	= R2.Index;
	};

	SVObjectScriptInputObjectTokenStructSEC()
	{
	};

	SVObjectScriptInputObjectTokenStructSEC& operator=( const SVObjectScriptInputObjectTokenStructSEC& R2 )
	{
		GuidStr	= R2.GuidStr;
		Index	= R2.Index;
		
		return( *this );
	};

	CString GuidStr;	// the input object's Guid as a String
	int Index;			// index to access the script stack
};

//******************************************************************************
//* EXTERN(S):
//******************************************************************************


//}// end namespace NS_ObjectScriptParserSEC

#endif 

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVObjectScriptParserSEC.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:41:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   04 Sep 2012 15:50:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use new progress dialog functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   08 Nov 2010 14:46:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Dec 2009 12:08:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Feb 2005 14:51:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
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
 * 
 *    Rev 3.3   22 Apr 2003 11:49:44   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   14 Jun 2001 07:39:30   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Updated to reflect changes missed in previous revision.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   19 Apr 2001 18:41:26   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   19 Apr 2001 15:09:56   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changes due to addition of the SVFileNameObjectClass and new GUID for the class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:20:54   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   18 Nov 1999 09:13:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Optimized and Added MultiThreading Support
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   01 Nov 1999 17:18:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to address performance issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   Oct 04 1999 09:56:22   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Object script tuning for faster performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   24 Sep 1999 19:26:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Corrected bug in SVObjectScriptOperandStructSEC 
 * (copy constructor and operator=).
 * Moved code for SVObjectScriptOperandStructSEC from .h to .cpp
 * Added Code to reattach inputs on restoration from script.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   28 Aug 1999 17:55:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Fixed bug in parsing STRING member assignments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
