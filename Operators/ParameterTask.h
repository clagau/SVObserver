//*****************************************************************************
/// \copyright (c) 2020,2020 by Seidenader Maschinenbau GmbH
/// \file ParameterTask.h
/// All Rights Reserved 
//*****************************************************************************
/// Class to contains parameters (LinkedValue and TypeParameter per Parameter), e.g. for Group Tool-Inputs and -Results.
//******************************************************************************
#pragma once


#pragma region Includes
#include "InspectionEngine/SVTaskObject.h"
#include "SVValueObjectLibrary/LinkedValue.h"
#include "SVValueObjectLibrary/SVLongValueObjectClass.h"
#pragma endregion Includes

namespace SvVol
{
	class SVEnumerateValueObjectClass;
}

namespace SvOp
{

	class ParameterTask : public SvIe::SVTaskObjectClass
	{
#pragma region Constructor
	public:
		ParameterTask(SvPb::EmbeddedIdEnum startEmbeddedIdValue, SvPb::EmbeddedIdEnum startEmbeddedIdLinked, SvPb::EmbeddedIdEnum startEmbeddedIdType,
			SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_PARAMETER_TASK);
		virtual ~ParameterTask();
#pragma endregion Constructor

#pragma region Public Methods
	public:
		virtual bool CreateObject(const SVObjectLevelCreateStruct& rCreateStructure) override;
		virtual bool ResetObject(SvStl::MessageContainerVector* pErrorMessages = nullptr) override;

		SvPb::InspectionCmdResponse setAndSortEmbeddedValues(SvPb::SetAndSortEmbeddedValueRequest request);

#pragma endregion Public Methods

	protected:
		//virtual bool onRun(RunStatus& rRunStatus, SvStl::MessageContainerVector* pErrorMessages = nullptr) override;

#pragma region Private Methods
	private:
		void init();
		void BuildEmbeddedObjectList();
		void registerParameter();
		bool checkObject(const std::string& name, const SvOi::IObjectClass* pObject, SvPb::InputTypeEnum type, SvStl::MessageContainerVector* pErrorMessages);
		bool checkValue(SvStl::MessageContainerVector* pErrorMessages);
#pragma endregion Private Methods

#pragma region Member Variables
	private:
		SvVol::SVLongValueObjectClass m_NumberOfObjects;
		std::vector<SvVol::LinkedValue*> m_objects;
		std::vector<SvVol::SVEnumerateValueObjectClass*>  m_TypeObjects;
		SvPb::EmbeddedIdEnum m_startEmbeddedIdValue;
		SvPb::EmbeddedIdEnum m_startEmbeddedIdLinked;
		SvPb::EmbeddedIdEnum m_startEmbeddedIdType;
#pragma endregion Member Variables
	};

	class InputParameterTask : public ParameterTask
	{
		SV_DECLARE_CLASS

		InputParameterTask(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_INPUT_PARAMETER_TASK);
		virtual ~InputParameterTask();
	};

	class ResultParameterTask : public ParameterTask
	{
		SV_DECLARE_CLASS

		ResultParameterTask(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_RESULT_PARAMETER_TASK);
		virtual ~ResultParameterTask();
	};
} //namespace SvOp
