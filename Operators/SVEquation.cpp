//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/SVToolsetScriptTags.h"
#include "SVObjectLibrary/ObjectUtilities.h"
#include "InspectionEngine/RunStatus.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVProtoBuf/SVO-Enum.h"
#include <regex>
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma endregion Declarations


SVEquationSymbolTableClass::SVEquationSymbolTableClass()
{
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
	// Disconnect the dynamic Inputs
	auto symbolTable = m_toolsetSymbolTable;
	m_toolsetSymbolTable.clear();
	for (const auto& rRef : symbolTable)
	{
		auto* pObject = SVObjectManagerClass::Instance().GetObject(rRef.getObjectId());
		if (pObject)
		{
			pObject->disconnectObject(m_ownerId);
		}
	}

	// Empty the Combined Symbol table
	for (auto& pItem : m_SVEquationSymbolPtrVector)
	{
		delete pItem;
	}
	m_SVEquationSymbolPtrVector.clear();
}



/////////////////////////////////////////////////////////////////
// Find a Symbol based on name
/////////////////////////////////////////////////////////////////
int SVEquationSymbolTableClass::FindSymbol(LPCTSTR name)
{
	SVEquationSymbolPtrVector::const_iterator Iter(m_SVEquationSymbolPtrVector.begin());
	for (; m_SVEquationSymbolPtrVector.end() != Iter; ++Iter)
	{
		if (!_tcscmp(name, (*Iter)->Name.c_str()))
		{
			return static_cast<int> (Iter - m_SVEquationSymbolPtrVector.begin());
		}
	}
	return -1;
}

void SVEquationSymbolTableClass::Init(SVObjectClass* pRequestor, bool useOwnerIdForDep)
{
	SVObjectClass* pInspection(nullptr);
	SvOl::SVObjectAppClass* pAppClass = dynamic_cast<SvOl::SVObjectAppClass*>(pRequestor);
	if (nullptr != pAppClass)
	{
		m_ownerId = useOwnerIdForDep ? pAppClass->GetParentID() : pAppClass->getObjectId();
		pInspection = pAppClass->GetInspection();
	}
	if (nullptr != pInspection)
	{
		m_InspectionName = pInspection->GetName();
		m_InspectionName += ".";
	}
}

int SVEquationSymbolTableClass::AddSymbol(LPCTSTR name)
{
	// Strip off Double Quotes
	std::string SymbolName = SvUl::Trim(name, _T("\""));

	SVEquationSymbolTypeEnum Type = SV_INPUT_SYMBOL_TYPE;

	int symbolIndex = FindSymbol(SymbolName.c_str());
	if (symbolIndex != -1)
	{
		///it is already in the vector
		return symbolIndex;
	}

	std::string LookUpName;

	if (SymbolName._Starts_with(m_ToolSetName))
	{
		Type = SV_TOOLSET_SYMBOL_TYPE;
		LookUpName = m_InspectionName;
		LookUpName += SymbolName;
	}
	else if (SvUl::Left(SymbolName, strlen(SvDef::DioInputStr)) == SvDef::DioInputStr
		|| SvUl::Left(SymbolName, strlen(SvDef::RemoteInputStr)) == SvDef::RemoteInputStr)
	{
		LookUpName = m_InspectionName;
		LookUpName += SymbolName;
	}
	else
	{
		LookUpName = SymbolName;
	}

	SVObjectReference ObjectReference;

	HRESULT hr = SVObjectManagerClass::Instance().GetObjectByDottedName(LookUpName.c_str(), ObjectReference);
	if (S_OK != hr)
	{
		return -1;
	}

	if (0 == (ObjectReference.ObjectAttributesAllowed() & SvPb::useable))
	{
		return -1;
	}

	if (nullptr == ObjectReference.getObject())
	{   //check if object can return a double. If not it is an invalid object and cannot be used.
		return -1; 
	}

	double testValue = 0;
	//check if object can return a double. If not it is an invalid object and cannot be used.
	HRESULT isDoubleOk = ObjectReference.getObject()->getValue(testValue);
	if (S_OK != isDoubleOk && SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE != isDoubleOk && E_BOUNDS != isDoubleOk)
	{	//check if object can return a double. If not it is an invalid object and cannot be used.
		return -1;
	}

	// Add to combined symbol table if not already there
	// Objects will be deletet in ClearAll() called by the ~SVEquationSymbolTableClass:;
	SVEquationSymbolStruct* pSymbolStruct = new SVEquationSymbolStruct();
	pSymbolStruct->Type = Type;
	pSymbolStruct->Name = name;
	// Set the variable to be used
	pSymbolStruct->m_ref = ObjectReference;
	// Try to Connect at this point
	ObjectReference.getObject()->connectObject(m_ownerId);
	pSymbolStruct->IsValid = true;
	m_SVEquationSymbolPtrVector.push_back(pSymbolStruct);
	symbolIndex = static_cast<int> (m_SVEquationSymbolPtrVector.size() - 1);
	// add it to the top
	m_toolsetSymbolTable.push_back(ObjectReference);
	auto pOwner = SVObjectManagerClass::Instance().GetObject(m_ownerId);
	if (nullptr != pOwner)
	{
		return pOwner->checkIfValidDependency(ObjectReference.getObject()) ? symbolIndex : -1;
	}
	else
	{
		return -1;
	}
}


/////////////////////////////////////////////////////////////////
// Get the ToolSet Symbol table
/////////////////////////////////////////////////////////////////
std::vector<SVObjectReference>& SVEquationSymbolTableClass::GetToolSetSymbolTable()
{
	return m_toolsetSymbolTable;
}

/////////////////////////////////////////////////////////////////
// Get the data from either a Input Symbol or a ToolSet Symbol
/////////////////////////////////////////////////////////////////
HRESULT SVEquationSymbolTableClass::GetData(int SymbolIndex, double& rValue, int Index)
{
	if (SymbolIndex >= 0 && SymbolIndex < static_cast<int> (m_SVEquationSymbolPtrVector.size()))
	{
		SVEquationSymbolStruct* pSymbolStruct = m_SVEquationSymbolPtrVector[SymbolIndex];
		bool isValidObject(false);

		if (SV_TOOLSET_SYMBOL_TYPE == pSymbolStruct->Type)
		{
			isValidObject = nullptr != pSymbolStruct->m_ref.getObject();
		}
		else
		{
			if (nullptr != pSymbolStruct->m_ref.getObject())
			{
				isValidObject = (nullptr != pSymbolStruct->m_ref.getObject());
			}
			else
			{
				return E_FAIL;
			}
		}
		if (isValidObject)
		{
			HRESULT hr = pSymbolStruct->m_ref.getObject()->getValue(rValue, Index);
			return hr;
		}
	}
	return E_FAIL;
}

HRESULT SVEquationSymbolTableClass::GetData(int SymbolIndex, std::vector<double>& rValues)
{
	HRESULT Result(E_FAIL);

	if (SymbolIndex >= 0 && SymbolIndex < static_cast<int> (m_SVEquationSymbolPtrVector.size()))
	{
		SVEquationSymbolStruct* pSymbolStruct = m_SVEquationSymbolPtrVector[SymbolIndex];
		bool isValidObject(false);

		if (SV_TOOLSET_SYMBOL_TYPE == pSymbolStruct->Type)
		{
			isValidObject = nullptr != pSymbolStruct->m_ref.getObject();
		}
		else if (SV_INPUT_SYMBOL_TYPE == pSymbolStruct->Type)
		{
			isValidObject = (nullptr != pSymbolStruct->m_ref.getObject());
		}

		if (isValidObject)
		{
			Result = pSymbolStruct->m_ref.getObject()->getValues(rValues);
		}
	}

	return Result;
}

SV_IMPLEMENT_CLASS(SVEquation, SvPb::EquationClassId);

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
SVEquation::SVEquation(SVObjectClass* POwner, int StringResourceID, bool enableFlagRemoteSettable)
	:SVTaskObjectClass(POwner, StringResourceID)
{
	// Give SVEquationLexClass and SVEquationYaccClass a pointer to us
	m_Lex.setEquation(this);
	m_Yacc.setEquation(this);
	if (enableFlagRemoteSettable)
	{
		m_enabled.SetObjectAttributesAllowed(SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	}

	init();
}

////////////////////////////////////////////////////////////////////////////////
// .Description : Initialization of newly Instantiated Object
////////////////////////////////////////////////////////////////////////////////
void SVEquation::init()
{
	// Identify our output type
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVEquationObjectType;

	// Identify our input type needs - this is a bit different here
	// Since out inputs are dynamic via the script specified
	// So the input will be identified when the script is created.

	// Register Embedded Objects
	RegisterEmbeddedObject(&m_enabled, SvPb::EquationEnabledEId, IDS_OBJECTNAME_ENABLED, false, SvOi::SVResetItemNone, true);

	// Set Embedded defaults
	m_enabled.SetDefaultValue(BOOL(true), true);

	// Set local defaults
	m_isDataValid = true;
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
SVEquation::~SVEquation()
{
	m_Symbols.ClearAll();
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
bool SVEquation::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	bool bOk = SVTaskObjectClass::CreateObject(rCreateStructure) && nullptr != GetInspection();

	// Set / Reset Printable Flag
	m_enabled.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::AddAttribute);

	m_isCreated = bOk;

	//Test must be called early, because it make the dependencies of the equation to the other object. And this is e.g. need by the first resetObject from GroupTool to check if invalid dependencies available
	ResetObject();

	return bOk;
}

bool SVEquation::HasCondition() const
{
	return !m_equationStruct.EquationBuffer.empty();
}

double SVEquation::GetYACCResult() const
{
	return m_Yacc.equationResult;
}

void SVEquation::fillSelectorListForEquation(std::back_insert_iterator<std::vector<SvPb::TreeItem>> treeInserter) const
{
	auto* pInspection = GetInspectionInterface();
	auto* pStartObject = SvOi::getObject(getFirstClosedParent(SvDef::InvalidObjectId));
	SvOi::fillSelectorList(treeInserter, pInspection, pStartObject, SvPb::LinkedValueTypeEnum::TypeDecimal, true);
}

const std::string& SVEquation::GetEquationText() const
{
	return m_equationStruct.GetEquationText();
}

void SVEquation::SetEquationText(const std::string& rText)
{
	m_equationStruct.SetEquationText(rText);
	std::regex eSpace("[[:space:]]*");   
	if (rText.empty() || std::regex_match(rText, eSpace))
	{
		m_Symbols.ClearAll();
		m_Yacc.equationResult = 0.0;
	}
}

void SVEquation::Persist(SvOi::IObjectWriter& rWriter, bool /*closeObject = true*/) const
{
	SVTaskObjectClass::Persist(rWriter);

	// Get the Data Values (Member Info, Values)
	std::string Temp = m_equationStruct.EquationBuffer;

	SvUl::AddEscapeSpecialCharacters(Temp, true);

	_variant_t value;
	value.SetString(Temp.c_str());
	rWriter.WriteAttribute(scEquationBufferTag, value);
	value.Clear();
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
HRESULT SVEquation::SetObjectValue(SVObjectAttributeClass* pDataObject)
{
	SvCl::SVObjectStdStringArrayClass AttributeList;
	bool bOk{pDataObject->GetAttributeData(_T("EquationBuffer"), AttributeList)};
	if(bOk)
	{
		for (int i = 0; i < static_cast<int> (AttributeList.size()); i++)
		{
			m_equationStruct.EquationBuffer = AttributeList[i];

			SvUl::RemoveEscapedSpecialCharacters(m_equationStruct.EquationBuffer, true);
		}
		return S_OK;
	}
	else
	{
		return SVTaskObjectClass::SetObjectValue(pDataObject);
	}
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
bool SVEquation::IsEnabled() const
{
	BOOL bEnabled;
	m_enabled.GetValue(bEnabled);

	return bEnabled ? true : false;
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
SvOi::EquationTestResult SVEquation::Test(SvStl::MessageContainerVector *pErrorMessages/*=nullptr */)
{
	SvOi::EquationTestResult ret;
	m_isDataValid = true;
	m_errContainer.clearMessage();

	if (HasCondition() && IsEnabled())
	{
		// Clear the symbol Tables
		m_Symbols.ClearAll();
		m_Symbols.Init(this, m_useOwnerIdForDep);


		std::string equationText = GetEquationText();
		equationText += "\n";
		// Note Buffer must be terminated with a new line
		// Build Lexical Stack
		SvOi::EquationTestResult result = lexicalScan(equationText.c_str());
		if (result.bPassed)
		{
			ParseYacc();

			if (m_Yacc.yacc_err)
			{
				std::string fullObjectName = GetObjectNameToObjectType(SvPb::SVInspectionObjectType);
				SvDef::StringVector msgList;
				msgList.push_back(fullObjectName);
				if (S_OK != m_Yacc.m_StatusCode)
				{
					m_errContainer.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_TooManyVariables, msgList, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				}
				else
				{
					ret.iPositionFailed = m_Yacc.lex_stack[m_Yacc.sIndex - 1].position + 1;
					msgList.push_back(std::format(_T("{:d}"), ret.iPositionFailed));
					m_errContainer.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_EquationParserError, msgList, SvStl::SourceFileParams(StdMessageParams), getObjectId());
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
	}
	// return true if no equation or disabled
	if (!HasCondition() || !IsEnabled())
	{
		ret.bPassed = true;
	}
	if (!ret.bPassed)
	{
		if (nullptr != pErrorMessages)
		{
			pErrorMessages->push_back(m_errContainer);
		}
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
SvOi::EquationTestResult SVEquation::lexicalScan(LPCTSTR inBuffer)
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
		m_Yacc.lex_stack[sIndex].position = static_cast<int>(m_Lex.position);
		m_Yacc.lex_stack[sIndex].token = yychar;
		if (yychar == SV_IDENTIFIER)
			m_Yacc.lex_stack[sIndex].value.index = m_Lex.yylval.index;
		else
			m_Yacc.lex_stack[sIndex].value.val = m_Lex.yylval.val;
		sIndex++;
	}

	m_Yacc.lex_stack[sIndex].position = static_cast<int>(m_Lex.position);
	m_Yacc.lex_stack[sIndex].token = yychar;
	if (yychar == SV_IDENTIFIER)
		m_Yacc.lex_stack[sIndex].value.index = m_Lex.yylval.index;
	else
		m_Yacc.lex_stack[sIndex].value.val = m_Lex.yylval.val;
	sIndex++;

	m_Yacc.numTokens = sIndex;

	if (m_Lex.lex_err)
	{
		std::string fullObjectName = GetObjectNameToObjectType(SvPb::SVInspectionObjectType);
		ret.bPassed = false;
		ret.iPositionFailed = static_cast<int>(m_Lex.position + 1);
		SvDef::StringVector msgList;
		msgList.push_back(fullObjectName);
		msgList.push_back(std::format(_T("{:d}"), ret.iPositionFailed));
		m_errContainer.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_EquationParserError, msgList, SvStl::SourceFileParams(StdMessageParams), getObjectId());

		m_isDataValid = false;
	}
	// Release the buffer
	m_Lex.yy_delete_buffer(b);

	return ret;
}

double SVEquation::RunAndGetResult()
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
int SVEquation::AddSymbol(LPCTSTR name)
{
	int index = m_Symbols.AddSymbol(name);

	if (-1 == index)
	{
		m_isDataValid = false;
	}

	return index;
}

void SVEquation::disconnectObjectInput(uint32_t objectId)
{
	if (SvDef::InvalidObjectId != objectId)
	{
		// Update ToolSet Symbol table - Gets called when one of our inputs goes away
		auto& toolSetSymbols = m_Symbols.GetToolSetSymbolTable();
		for (auto& rRef : toolSetSymbols)
		{
			if (objectId == rRef.getObjectId())
			{
				rRef = {};
				__super::disconnectObjectInput(objectId);
				return;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
// If Conditional is disabled equation.Run() returns always true.
// Otherwise the return value depends on the Conditional equation result!
bool SVEquation::onRun(SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	bool retVal = __super::onRun(rRunStatus, pErrorMessages);

	if (HasCondition() && IsEnabled())
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
		if (!m_isDataValid || m_Yacc.yacc_err)
		{
			retVal = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidData, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				pErrorMessages->push_back(Msg);
			}
			rRunStatus.SetInvalid();
		}
	}
	if (retVal)
		rRunStatus.SetPassed();
	else
		rRunStatus.SetFailed();

	return retVal;
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
double SVEquation::GetPropertyValue(int iSymbolIndex)
{
	HRESULT hr = S_FALSE;
	double value = 0.0;

	if (iSymbolIndex != -1)
	{
		hr = m_Symbols.GetData(iSymbolIndex, value);
	}// end if

	//E_BOUNDS is return if a Sort value object is used and no value is set yet. In this case it should not set to invalid, but return 0.0
	if (S_OK != hr && E_BOUNDS != hr)
	{
		m_isDataValid = false;
	}
	return value;
}

HRESULT SVEquation::GetArrayValues(int iSymbolIndex, std::vector< double >& values)
{
	HRESULT hr = S_FALSE;

	if (iSymbolIndex != -1)
	{
		hr = m_Symbols.GetData(iSymbolIndex, values);
	}// end if

	if (S_OK != hr)
	{
		m_isDataValid = false;
	}

	return hr;
}

double SVEquation::GetSubscriptedPropertyValue(int SymbolIndex, int Index, double Default /*= 0.0*/)
{
	HRESULT hr = S_FALSE;
	double Value = Default;

	if (SymbolIndex != -1)
	{
		hr = m_Symbols.GetData(SymbolIndex, Value, Index);
	}

	if (S_OK != hr)
	{
		Value = Default;
	}

	return Value;
}

bool SVEquation::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	// call Test()...( Rebuilds symbol table !!! )
	if (Result && HasCondition() && IsEnabled())
	{
		//In reset object call test without displaying error messages is done using the returned result
		Result = Test(pErrorMessages).bPassed;
	}

	return Result;
}

void SVEquation::changeSource(const SVObjectReference& rOldObject, SVObjectClass& rNewObject)
{
	auto oldName = _T("\"") + rOldObject.GetObjectNameToObjectType(SvPb::SVToolSetObjectType) + _T("\"");
	auto newName = _T("\"") + rNewObject.GetObjectNameToObjectType(SvPb::SVToolSetObjectType, true) + _T("\"");
	std::string equationBuff = GetEquationText();

	// Replace all occurrences
	if (GetEquationText() != SvUl::searchAndReplace(equationBuff, oldName.c_str(), newName.c_str()))
	{
		SetEquationText(equationBuff);
	}
}

void SVEquation::OnObjectRenamed(const SVObjectClass& rRenamedObject, const std::string& rOldName)
{
	std::string equationBuff = SvOl::renameFormulaTextAfterRenameObject(GetEquationText(), rRenamedObject, rOldName);

	// Replace all occurrences
	if (GetEquationText() != equationBuff)
	{
		SetEquationText(equationBuff);
	}

	__super::OnObjectRenamed(rRenamedObject, rOldName);
}

void SVEquation::ParseYacc()
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

} //namespace SvOp
