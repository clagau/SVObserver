//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
//Moved to precompiled header: #include <vector>
#include "ObjectInterfaces/EquationTestResult.h"
#include "ObjectInterfaces/IEquation.h"
#include "ObjectInterfaces/IValueObject.h"
#include "SVEquationLibrary/SVEquationBase.h"
#include "SVEquationLibrary/SVEquationLex.h"
#include "SVEquationLibrary/SVEquationYacc.h"
#include "InspectionEngine/SVTaskObject.h"
#include "SVStatusLibrary/MessageContainer.h"
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#pragma endregion Includes

namespace SvOp
{

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
	SVObjectReference	m_ref;
	std::string Name;
	bool IsValid;

	SVEquationSymbolStruct()
	{
		Type = SV_UNKNOWN_SYMBOL_TYPE;
		IsValid = false;
	};
};

/**
@SVObjectName Equation Symbol Table

@SVObjectOverview This object holds the group of symbols associated with a particular equation string.  It also provides the interface into the application to validate and make a connection to the desired symbol.

@SVObjectOperations This object maintains operators to handle container management, data insertion, data extraction, and symbol management.

*/
class SVEquationSymbolTableClass
{
public:
	typedef std::vector<SVEquationSymbolStruct*> SVEquationSymbolPtrVector;
	SVEquationSymbolTableClass();
	~SVEquationSymbolTableClass();

	int FindSymbol( LPCTSTR name );					// Find Input or ToolSet Symbol
	
	//************************************
	//! This adds a symbol into the list of used symbol
	//! \param name [in] name 
	//! \returns int the index of the used symbols list
	//************************************
	int AddSymbol(LPCTSTR name);
	
	std::vector<SVObjectReference>& GetToolSetSymbolTable();	// Get the ToolSet Symbol table

	HRESULT GetData(int SymbolIndex, double& rValue, int Index = -1 );
	HRESULT GetData(int SymbolIndex, std::vector<double>& rValues );

	void ClearAll();
	//************************************
	//! init the inspectionName 
	//! \param pRequestor [in] the tool who wants to use the variable
	//! \returns void
	//************************************
	void Init(SVObjectClass* pRequestor);

private:
	SVEquationSymbolPtrVector m_SVEquationSymbolPtrVector;
	std::vector<SVObjectReference> m_toolsetSymbolTable;		// The symbol table for the ToolSet Variables in the equation
	std::string m_InspectionName;

	std::string  m_ToolSetName = SvUl::LoadedStrings::g_ToolSetName + ".";
	uint32_t m_ownerId = SvDef::InvalidObjectId;
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
class SVEquation : public SvIe::SVTaskObjectClass, public SVEquationBase, public SvOi::IEquation
{
	SV_DECLARE_CLASS

	/**
	@SVObjectName Equation Parsing Data

	@SVObjectOverview This object holds the parsing data for the equation object.

	@SVObjectOperations This object has simple operators to duplicate itself, set the equation string, get the equation string.

	*/
	struct SVEquationStruct 
	{
		SVEquationStruct() = default;
		SVEquationStruct(const SVEquationStruct&) = default;
		SVEquationStruct& operator=(const SVEquationStruct &REquationStruct) = default;

		const std::string& GetEquationText() const
		{
			return EquationBuffer;
		};

		void SetEquationText(const std::string& rText)
		{
			EquationBuffer = rText;
		};

		std::string EquationBuffer;
		int StartPos = 0;
		int EndPos = 0;
	};

public:
	SVEquation( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVEQUATION );
	virtual ~SVEquation();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	
	bool HasCondition() const;

#pragma region IEquation
	virtual const std::string& GetEquationText() const override;
	virtual void SetEquationText(const std::string& text) override;
	virtual SvOi::EquationTestResult Test( SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
	virtual double GetYACCResult() const override;
#pragma endregion IEquation

	/// Run equation and return the result
	/// \returns double
	double RunAndGetResult();

	virtual int AddSymbol( LPCTSTR name ) override;

	virtual void disconnectObjectInput(uint32_t objectId) override;

	virtual double GetPropertyValue( int iSymbolIndex ) override;
	virtual double GetSubscriptedPropertyValue( int SymbolIndex, int Index, double Default = 0.0 ) override;
	virtual HRESULT GetArrayValues( int iSymbolIndex, std::vector< double >& values ) override;

	bool IsEnabled() const;

	virtual void Persist(SvOi::IObjectWriter& rWriter) const override;
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* PDataObject ) override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

#pragma region Methods to replace processMessage
	virtual void OnObjectRenamed(const SVObjectClass& rRenamedObject, const std::string& rOldName) override;
#pragma endregion Methods to replace processMessage

protected:
	void init();

	virtual bool onRun( SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	double getResult() const { return m_Yacc.equationResult; };

	/// Parse the equation (calculated the result)
	void ParseYacc();

private:
	SvOi::EquationTestResult lexicalScan( LPCTSTR inBuffer );		// perform lexical scan


protected:
	SvVol::SVBoolValueObjectClass m_enabled;			// enabled flag TRUE=Enabled
	bool m_isDataValid;						// the variable(s) returned Valid Data

	SVEquationYaccClass m_Yacc;				// parser class

private:
	SVEquationLexClass m_Lex;					// scanner class
	SvStl::MessageContainer m_errContainer;							// for errorEvent	
	
	SVEquationStruct m_equationStruct;		// the Equation Structure
	
	SVEquationSymbolTableClass m_Symbols;		// all symbols ( input and local )
};

} //namespace SvOp
