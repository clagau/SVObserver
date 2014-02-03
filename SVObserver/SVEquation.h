//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEquation.h
//* .File Name       : $Workfile:   SVEquation.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   01 Feb 2014 10:32:28  $
//******************************************************************************

#ifndef SVEQUATIONCLASS_H
#define SVEQUATIONCLASS_H

#include "SVObjectLibrary/SVObjectScriptUsage.h"
#include "SVEquationLibrary/SVEquationBase.h"
#include "SVEquationLibrary/SVEquationLex.h"
#include "SVEquationLibrary/SVEquationYacc.h"
#include "SVTaskObject.h"

/**
@SVObjectName Equation Buffer Size Enumeration

@SVObjectOverview This enumeration defines the Equation Buffer Size.

@SVObjectOperations This enumeration has no operations.

*/
enum SVEquationBufferSizeEnum
{
	SV_EQUATION_BUFFER_SIZE = 4096
};

class SVIPDoc;

/**
@SVObjectName Equation Symbol Type Enumeration

@SVObjectOverview This enumeration defines the Unknown, PPQ and Toolset symbol type values.

@SVObjectOperations This enumeration has no operations.

*/
enum SVEquationSymbolTypeEnum
{
	SV_UNKNOWN_SYMBOL_TYPE,
	SV_PPQ_SYMBOL_TYPE,
	SV_TOOLSET_SYMBOL_TYPE
};

/**
@SVObjectName Equation Symbol Data

@SVObjectOverview This object holds the symbol connection data for the equation object.

@SVObjectOperations This object has no operations.

*/
struct SVEquationSymbolStruct
{
	SVEquationSymbolTypeEnum Type;
	SVInObjectInfoStruct	InObjectInfo;
	CString	Name;
	BOOL IsValid;

	SVEquationSymbolStruct()
	{
		Type = SV_UNKNOWN_SYMBOL_TYPE;
		IsValid = FALSE;
	};
};

/**
@SVObjectName Equation Symbol Table

@SVObjectOverview This object holds the group of symbols associated with a particular equation string.  It also provides the interface into the application to validate and make a connection to the desired symbol.

@SVObjectOperations This object maintains operators to handle container management, data insertion, data extraction, and symbol management.

*/
class SVEquationSymbolTableClass : 
	public SVVector< SVEquationSymbolStruct* >
{
public:
	SVEquationSymbolTableClass();
	~SVEquationSymbolTableClass();

	int FindSymbol( LPCTSTR name );					// Find PPQ or ToolSet Symbol
	int AddSymbol( LPCTSTR name, SVObjectClass* pRequestor );	// Add PPQ or ToolSet Symbol

	SVInputInfoListClass& GetToolSetSymbolTable();	// Get the ToolSet Symbol table

	HRESULT GetData(int iSymbolIndex, double& value, long lBufferIndex );		// Get the Data Value
	HRESULT GetData(int iSymbolIndex, int index, double& value, long lBufferIndex );		// Get the Data Value
	HRESULT GetData(int iSymbolIndex, std::vector<double>& values, long lBufferIndex );		// Get the Data Value

	void ClearAll();
	void SetAvailableLists( SVInputInfoListClass* PAvailPPQSymbols, SVOutputInfoListClass* PAvailToolSetSymbols );
	
protected:
	int findPPQSymbol( LPCTSTR name );					// Find PPQ Symbol
	int findToolSetSymbol( LPCTSTR name );				// Find ToolSet Symbol

	int addPPQSymbol( LPCTSTR name, int index );		// Add PPQ Symbol
	int addToolSetSymbol( LPCTSTR name, int index, SVObjectClass* pRequestor );	// Add ToolSet Symbol

protected:
	SVInputInfoListClass toolsetSymbolTable;		// The symbol table for the ToolSet Variables in the equation
	SVOutputInfoListClass* pAvailToolSetSymbols;	// List of Available ToolSet symbols
	SVInputInfoListClass* pAvailPPQSymbols;			// List of Available PPQ symbols
};

/**
@SVObjectName Equation Parsing Data

@SVObjectOverview This object holds the parsing data for the equation object.

@SVObjectOperations This object has simple operators to duplicate itself, set the equation string, get the equation string.

*/
struct SVEquationStruct 
{
	SVEquationStruct ()
	{
		EquationBuffer.Empty();
		StartPos = 0;
		EndPos = 0;
	};

	SVEquationStruct& operator=( const SVEquationStruct &REquationStruct)
	{
		EquationBuffer = REquationStruct.EquationBuffer;
		StartPos = REquationStruct.StartPos;
		EndPos = REquationStruct.EndPos;
		return (*this);
	};

	void GetEquationText(CString &text)
	{
		text = EquationBuffer;	
	};

	void GetEquationText(SVString &text)
	{
		text = EquationBuffer;	
	};

	void SetEquationText(const CString text)
	{
		EquationBuffer = text;
	};

	void SetEquationText(const SVString text)
	{
		EquationBuffer = text.ToString();
	};

	CString EquationBuffer;
	int StartPos;
	int EndPos;
};


/**
@SVObjectName Equation Test Result

@SVObjectOverview This object holds the result information of the test operation.

@SVObjectOperations This object has no operations.

*/
class SVEquationTestResult
{
public:
	bool bPassed;
	int iPositionFailed;

	SVEquationTestResult()
	{
		bPassed = false;
		iPositionFailed=0;
	}
};

/**
@SVObjectName Equation

@SVObjectOverview This object is the interface between the LEX and YACC parsers and the application.  This object is also responsible for executing the list of operations necessary to get the result of the supplied equation string.

@SVObjectOperations The operations will be used to interface into the main application data and notate which application elements the LEX and YACC will need to access.  It will allow the LEX and YACC parsers to notify this object when an application variable is being used.  This object manages the verification and validation of that connection.  This object also overrides the required operators to integrate this object into the processing stream.

The Add Symbol operator notifies the equation class to register the specified item as an input into the equation.

The Get Property Value operator returns the value of the specified token by its symbol index.

The Get Subscripted Property Value operator returns the value at a particular subscript of the specified token by its symbol index.  If a default value is provided, it will be returned if the subscript is invalid.

The Get Array Values operator fills the provided array with the values of the specified token by its symbol index.

*/
class SVEquationClass : 
	public SVTaskObjectClass,
	public SVEquationBase
{
	SV_DECLARE_CLASS( SVEquationClass );

public:
	SVEquationClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVEQUATION );
	virtual ~SVEquationClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );

	SVEquationStruct* GetEquationStruct();
	
	double GetYACCResult();

	// For Conditional
	BOOL NeedsConditionalPPQData();

	BOOL HasCondition();

	void GetEquationText(CString &text);
	void SetEquationText(CString &text);

	virtual int AddSymbol( LPCTSTR name );

	virtual BOOL DisconnectToolSetSymbol( SVInObjectInfoStruct* pInObjectInfo );

	virtual SVEquationTestResult Test();

	virtual BOOL OnValidate();

	virtual double GetPropertyValue( int iSymbolIndex );
	virtual double GetSubscriptedPropertyValue( int iSymbolIndex, int iIndex, double dDefault );
	virtual double GetSubscriptedPropertyValue( int iSymbolIndex, int iIndex );
	virtual HRESULT GetArrayValues( int iSymbolIndex, std::vector< double >& values );

	BOOL IsEnabled();

	BOOL IsDataValid();

#ifdef USE_OBJECT_SCRIPT
	virtual void GetObjectScript( CString& RStrScript, CString& RStrAliasTable, int Indent = 0 );
#endif
	virtual void Persist(SVObjectWriter& rWriter);
	virtual HRESULT GetObjectValue( const SVString& p_rValueName, VARIANT& p_rVariantValue ) const;
	virtual HRESULT SetObjectValue( const SVString& p_rValueName, const _variant_t& p_rVariantValue );
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* PDataObject );

	virtual HRESULT ResetObject();

	SVBoolValueObjectClass enabled;			// enabled flag TRUE=Enabled

protected:
	void init();

	BOOL buildDynamicInputList();

	virtual BOOL renameToolSetSymbol(SVObjectClass* pObject, LPCTSTR orgName);

	virtual BOOL onRun( SVRunStatusClass& RRunStatus );

	virtual LONG_PTR processMessage( DWORD DwMessageID, LONG_PTR DwMessageValue, LONG_PTR DwMessageContext );
	
private:
	SVEquationTestResult lexicalScan( LPSTR buffer );		// perform lexical scan

protected:
	SVEquationLexClass lex;					// scanner class
	SVEquationYaccClass yacc;				// parser class

	CString errStr;							// for errorEvent

	BOOL isDataValid;						// the variable(s) returned Valid Data
	
	SVEquationStruct equationStruct;		// the Equation Structure
	
	SVEquationSymbolTableClass symbols;		// all symbols ( PPQ and local )
	long m_lCurrentRunIndex;				// pointer to the run status for the current run
											// it is needed to get the result data index for PPQ inputs
};

#endif	// SVEQUATIONCLASS_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVEquation.h_v  $
 * 
 *    Rev 1.2   01 Feb 2014 10:32:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed sendmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   14 Jan 2014 12:09:22   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  877
 * SCR Title:  Add undo-button to formula and conditional pages
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed parameter for SetEquationText from CString & to const CString.
 * Added SVString versions of SetEquationText and GetEquationText.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:27:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   10 Jan 2013 16:13:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added missing log entries.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   07 Dec 2012 10:35:30   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new virtual methods for GetObjectValue and SetObjectValue
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   04 Sep 2012 15:00:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add new scripting and object creation/update funcitonality to the object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14.1.0   05 Dec 2012 11:53:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new virtual methods for GetObjectValue and SetObjectValue
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   08 Nov 2010 14:35:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   28 Oct 2010 13:44:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   20 Jun 2007 16:32:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * Also updated Equation class to use the new SVEquationLibrary and override the functionality in SVEquationBase.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   08 Aug 2005 15:56:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   19 Jul 2005 14:30:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added syntax for accessing an array element without specifying a default value (uses value object default value)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   22 Jun 2005 08:53:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented array value object equation interface
 * added position information to test result
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   17 Feb 2005 14:03:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   02 Jun 2003 15:06:48   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Checked in some preliminary beta code for the new methode of disabling tools and toolsets.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   22 Apr 2003 09:25:26   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   19 Nov 2002 13:55:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   relates to how the equation classes connect and display information for Input/Output from the PPQ
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   04 Apr 2002 09:53:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  286
 * SCR Title:  Upgrade SVObserver to Version 3.37 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code syntax to allow rational rose to reverse engineer properly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   31 Oct 2001 16:01:06   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  252
 * SCR Title:  System hang on .5 dual when accessing Math Tool after going online
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed members of SVEquationStruct from long to int
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   Apr 17 2001 16:13:32   steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen S. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   01 Mar 2000 16:14:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  106
 * SCR Title:  Math Tool - List processing
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed dead function isEquationValid.
 * Added check for Yacc Error message string (list processing)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 11:29:50   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   08 Nov 1999 09:49:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added method to get YACC resultant value.
 * ( for use in displaying test result) 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   18 Oct 1999 16:04:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to show complete name when displaying parser 
 * error.
 * Added method and handler to handle renaming of tools
 * (updates equation buffer)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   07 Oct 1999 19:58:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised parser error messages to indicate which equation
 * owned by what tool has a problem.
 * Added handler for disconnect of inputs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   Sep 27 1999 19:31:40   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  35
 * SCR Title:  PPQ mode to wait for input data
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Bugfix: Rebuilding Symbol Table now only if going to Run Mode or Test Mode or casused by user presses Validate Button.
 * Changed to used PQdataLineInfoPtrList.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   21 Sep 1999 10:42:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Corrected problem with using Toolset variables.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   15 Sep 1999 17:13:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed Run() method.
 * Revised onRun Method to call base class first.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   13 Sep 1999 19:33:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Put Run method back in. (SVTaskObject framework
 * does not call validate for friends)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   10 Sep 1999 17:46:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to use toolset variables as well as PPQ variables
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   02 Sep 1999 11:50:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Run logic
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   27 Aug 1999 08:51:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  Steve Jones
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   Aug 23 1999 10:02:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  22
 * SCR Title:  Port Multiple Position Queue code from program version 2.4.2
 * Checked in by:  Steve Jones
 * Change Description:  
 *   Revised for new PPQ Architecture.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
