//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEquation.h
//* .File Name       : $Workfile:   SVEquation.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   19 Dec 2014 03:59:32  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "ObjectInterfaces/EquationTestResult.h"
#include "ObjectInterfaces/IEquation.h"
#include "SVEquationLibrary/SVEquationBase.h"
#include "SVEquationLibrary/SVEquationLex.h"
#include "SVEquationLibrary/SVEquationYacc.h"
#include "SVTaskObject.h"
#include "SVStatusLibrary/MessageContainer.h"
#pragma endregion Includes

/**
@SVObjectName Equation Symbol Type Enumeration

@SVObjectOverview This enumeration defines the Unknown, Input and Toolset symbol type values.

@SVObjectOperations This enumeration has no operations.

*/
enum SVEquationSymbolTypeEnum
{
	SV_UNKNOWN_SYMBOL_TYPE,
	SV_INPUT_SYMBOL_TYPE,
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

	int FindSymbol( LPCTSTR name );					// Find Input or ToolSet Symbol
	
	//************************************
	//! This adds a symbol into the list of used symbol
	//! \param name [in] symbolname 
	//! \param pRequestor [in]  who wants to use the variable
	//! \returns int the index of the used symbols list
	//************************************
	int AddSymbol(LPCTSTR name, SVObjectClass* pRequestor );
	
	SVInputInfoListClass& GetToolSetSymbolTable();	// Get the ToolSet Symbol table

	HRESULT GetData(int iSymbolIndex, double& value, long lBufferIndex );		// Get the Data Value
	HRESULT GetData(int iSymbolIndex, int index, double& value, long lBufferIndex );		// Get the Data Value
	HRESULT GetData(int iSymbolIndex, std::vector<double>& values, long lBufferIndex );		// Get the Data Value

	void ClearAll();
	//************************************
	//! init the inspectionName 
	//! \param pRequestor [in] the tool who wants to use the variable
	//! \returns void
	//************************************
	void Init(SVObjectClass* pRequestor);

	
protected:
	SVInputInfoListClass m_toolsetSymbolTable;		// The symbol table for the ToolSet Variables in the equation
	CString m_InspectionName;

	CString  m_ToolSetName;
	CString m_DIOInputName;
	CString  m_RemoteInputName;
	

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
class SVEquationClass : public SVTaskObjectClass, public SVEquationBase, public SvOi::IEquation
{
	SV_DECLARE_CLASS( SVEquationClass );

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

		void GetEquationText(CString &text) const
		{
			text = EquationBuffer;	
		};

		void SetEquationText(const CString& text)
		{
			EquationBuffer = text;
		};

		CString EquationBuffer;
		int StartPos;
		int EndPos;
	};

public:
	SVEquationClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVEQUATION );
	virtual ~SVEquationClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	
	BOOL HasCondition();

	void GetEquationText(CString& text) const;
	void SetEquationText(const CString& text);

#pragma region IEquation
	virtual void GetEquationText(SVString& text) const override;
	virtual void SetEquationText(const SVString& text) override;
	virtual SvOi::EquationTestResult Test( bool DisplayErrorMessage = true ) override;
	virtual double GetYACCResult() const override;
#pragma endregion IEquation

	virtual int AddSymbol( LPCTSTR name );

	virtual BOOL DisconnectToolSetSymbol( SVInObjectInfoStruct* pInObjectInfo );

	virtual BOOL OnValidate();

	virtual double GetPropertyValue( int iSymbolIndex );
	virtual double GetSubscriptedPropertyValue( int iSymbolIndex, int iIndex, double dDefault );
	virtual double GetSubscriptedPropertyValue( int iSymbolIndex, int iIndex );
	virtual HRESULT GetArrayValues( int iSymbolIndex, std::vector< double >& values );

	BOOL IsEnabled();

	virtual void Persist(SVObjectWriter& rWriter);
	virtual HRESULT GetObjectValue( const SVString& p_rValueName, VARIANT& p_rVariantValue ) const;
	virtual HRESULT SetObjectValue( const SVString& p_rValueName, const _variant_t& p_rVariantValue );
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* PDataObject );

	virtual HRESULT ResetObject();

	SVBoolValueObjectClass enabled;			// enabled flag TRUE=Enabled

protected:
	void init();

	virtual BOOL renameToolSetSymbol( const SVObjectClass* pObject, LPCTSTR originalName );

	virtual BOOL onRun( SVRunStatusClass& RRunStatus );

	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );
	
private:
	SvOi::EquationTestResult lexicalScan( LPCTSTR inBuffer );		// perform lexical scan

	protected:
	SVEquationLexClass lex;					// scanner class
	SVEquationYaccClass yacc;				// parser class

	SvStl::MessageContainer errContainer;							// for errorEvent

	BOOL isDataValid;						// the variable(s) returned Valid Data
	
	SVEquationStruct equationStruct;		// the Equation Structure
	
	SVEquationSymbolTableClass symbols;		// all symbols ( input and local )
	long m_lCurrentRunIndex;				// pointer to the run status for the current run
											// it is needed to get the result data index for PPQ inputs
};

