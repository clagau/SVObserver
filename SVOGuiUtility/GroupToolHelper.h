//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file GroupToolHelper.h
/// This is the class contains structs and functions for Group Tool GUIs (e.g. input tab)
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVProtoBuf/InspectionCommands.h"
#include "LinkedValue.h"
#include "Definitions/StringTypeDef.h"
#include "SVStatusLibrary/MessageData.h"
#include "SVStatusLibrary/MessageContainer.h"
#pragma endregion Includes

namespace SvOgu
{
class ValueController;




struct GroupInputResultData
{
	std::string m_name {"GroupData"};
	SvPb::EmbeddedIdEnum m_oldEmbeddedId = SvPb::NoEmbeddedId;
	SvPb::LinkedValueTypeEnum m_type = SvPb::LinkedValueTypeEnum::TypeDecimal;
	LinkedValueData m_data {SvPb::LinkedSelectedOption::DirectValue, {0.}, {0.}, {}, {0.}};
	SvDef::StringPairVector m_dependencies;
	SvStl::MessageData m_errorData;
	
	bool isOk() const;
	bool setValue(const std::string& newStr);

};

class StateHelper
{
public:
	static const std::array< std::pair<std::string, int>, 3>& getStates();
	static int toState(const std::string& name);
	static  std::string toString(int state);
	static  std::string toString(const _variant_t& var);
private:
	static const std::array< std::pair<std::string, int>,3> m_states;


};

const std::string c_inputName {"Input"};
const std::string c_resultName {"Result"};



///  Send the value to the business logic. It can throw an exception.
/// \param ipId [int]
/// \param taskId [in]
/// \param rDataList [in]
/// \param displayMessageBox [in]
/// \returns SvStl::MessageContainerVector
SvStl::MessageContainerVector sendValuesToInspection(uint32_t ipId, uint32_t taskId, const std::vector<GroupInputResultData>& rDataList, bool displayMessageBox);

bool addEntry(uint32_t ipId, uint32_t taskId, const std::string& firstNameTry, std::vector<GroupInputResultData>& rDataList);
void loadDataList(std::vector<GroupInputResultData>& rDataList, ValueController& rValueCtrl, SvPb::EmbeddedIdEnum valueEId, SvPb::EmbeddedIdEnum typeEId, const SvStl::MessageContainerVector& rErrorMessages);

/// Convert a text to the correct variant depend of the type. If convert failed it throw an exception.
variant_t convertTextToVariant(SvPb::LinkedValueTypeEnum type, const std::string& text);
/// Fit an variant to the correct variant depend of the type. If convert failed it throw an exception.
variant_t fitVariantToType(SvPb::LinkedValueTypeEnum type, const variant_t& value);

void checkAndCorrectTypes(SvOgu::GroupInputResultData& rData);

SvStl::MessageContainerVector checkParameterNames(uint32_t ipId, uint32_t taskId, const std::vector<GroupInputResultData>& rDataList);
SvStl::MessageContainerVector checkParameterNames(uint32_t ipId, uint32_t taskId, const std::vector<std::string>& rDataList);
}