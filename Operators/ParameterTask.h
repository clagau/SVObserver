//*****************************************************************************
/// \copyright (c) 2020,2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file ParameterTask.h
/// All Rights Reserved 
//*****************************************************************************
/// Class to contains parameters (LinkedValue and TypeParameter per Parameter), e.g. for Group Tool-Inputs and -Results.
//******************************************************************************
#pragma once


#pragma region Includes
#include "InspectionEngine/SVTaskObject.h"
#include "ObjectInterfaces/IParameterTask.h"
#include "SVValueObjectLibrary/LinkedValue.h"
#include "SVValueObjectLibrary/SVLongValueObjectClass.h"
#pragma endregion Includes

namespace SvIe
{
class RunStatus;
}

namespace SvVol
{
	class SVEnumerateValueObjectClass;
}

namespace SvOp
{

	class ParameterTask : public SvIe::SVTaskObjectClass, public SvOi::IParameterTask
	{
#pragma region Constructor
	public:
		ParameterTask(SvPb::EmbeddedIdEnum startEmbeddedIdValue, SvPb::EmbeddedIdEnum startEmbeddedIdType, LPCTSTR objectTypeEnumString,
			SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_PARAMETER_TASK);
		virtual ~ParameterTask();
#pragma endregion Constructor

#pragma region Public Methods
	public:
		virtual bool CreateObject(const SVObjectLevelCreateStruct& rCreateStructure) override;
		virtual bool CloseObject() override;
		virtual bool ResetObject(SvStl::MessageContainerVector* pErrorMessages = nullptr) override;

		SvPb::InspectionCmdResponse setAndSortEmbeddedValues(SvPb::SetAndSortEmbeddedValueRequest request);

		SVObjectClass* OverwriteEmbeddedObject(uint32_t uniqueID, SvPb::EmbeddedIdEnum embeddedID);

		void setToDefault();

		virtual SvPb::InspectionCmdResponse addParameterAndUseIt(const SvPb::AddParameterAndUseItRequest& rRequest) override;
		virtual SvPb::InspectionCmdResponse checkParameterNames(const SvPb::CheckParameterNamesRequest& rRequest) override;

		void fixAndAddAllObjects(std::back_insert_iterator<std::vector<SvPb::FixedInputData>> inserter);
#pragma endregion Public Methods

	protected:

#pragma region Private Methods
	private:
		void init();
		void BuildEmbeddedObjectList();
		void registerParameter();
		bool checkObject(const std::string& name, const SvOi::IObjectClass* pObject, SvPb::LinkedValueTypeEnum type, SvStl::MessageContainerVector* pErrorMessages, uint32_t objectIdForErrorMessage);
		bool checkValue(SvStl::MessageContainerVector* pErrorMessages);

		void setObject(int index);
		bool checkValueAndGetDefault(const SvPb::EmbeddedValueStruct& rRequestValue, SvVol::LinkedValue& rValueObject, variant_t& rDefaultValue, SvPb::MessageContainerVector* pErrorMessage);
		void setEmptyOldEmbeddedIdToFreeId(google::protobuf::RepeatedPtrField< SvPb::EmbeddedValueStruct >& rEmbeddedList, long newFreePos);
		void swapElements(google::protobuf::RepeatedPtrField< SvPb::EmbeddedValueStruct >& rValues, int pos, SvPb::MessageContainerVector& rMessageVec);
		bool setElement(const SvPb::EmbeddedValueStruct& rValue, int pos, const variant_t& rDefault, SvPb::MessageContainerVector* pErrorMessage);
#pragma endregion Private Methods

#pragma region Member Variables
	protected:
		std::vector<std::unique_ptr<SvVol::LinkedValue>> m_objects;

	private:
		SvVol::SVLongValueObjectClass m_NumberOfObjects;
		std::vector< std::unique_ptr<SvVol::SVEnumerateValueObjectClass>>  m_TypeObjects;
		SvPb::EmbeddedIdEnum m_startEmbeddedIdValue;
		SvPb::EmbeddedIdEnum m_startEmbeddedIdType;
		LPCTSTR m_objectTypeEnumString;
#pragma endregion Member Variables
	};

	class InputParameterTask : public ParameterTask
	{
		SV_DECLARE_CLASS

		InputParameterTask(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_INPUT_PARAMETER_TASK);
		virtual ~InputParameterTask();

		virtual uint32_t getFirstClosedParent(uint32_t stopSearchAtObjectId) const override;
	};

	class ResultParameterTask : public ParameterTask
	{
		SV_DECLARE_CLASS

		ResultParameterTask(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_RESULT_PARAMETER_TASK);
		virtual ~ResultParameterTask();

		bool calcFormulaPost(SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector* pErrorMessages);
	};
} //namespace SvOp
