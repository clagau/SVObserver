//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by K�rber Pharma Inspection GmbH. All Rights Reserved
// All Rights Reserved
//*****************************************************************************

// This is the DataController class for use by the GUI.
//******************************************************************************
#pragma once

#pragma region Includes
#include "BoundValue.h"
#include "Definitions\NameValueVector.h"
#include "SVStatusLibrary\MessageContainer.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVUtilityLibrary\SVSafeArray.h"
#include "LinkedValue.h"
#include "ValuesAccessor.h"
#pragma endregion Includes

namespace SvOgu
{

	class ValueController : public ValuesAccessor
	{
#pragma region Constructor
	public:
		explicit ValueController(const BoundValues& rBoundValue) : m_Data(rBoundValue) {}
		virtual ~ValueController() {}

		ValueController() = delete;
		ValueController& operator=(const ValueController&) = delete;
#pragma endregion Constructor

#pragma region Public Methods
	public:
		HRESULT Init()
		{
			return GetValues(m_Data);
		}

		std::vector<std::pair<SvPb::EmbeddedIdEnum, std::variant<_variant_t, LinkedValueData>>> getCurrentValues()
		{
			return m_Data.getCurrentValues();
		}

		template<typename DataType>
		DataType GetDefault(SvPb::EmbeddedIdEnum embeddedID) const
		{
			return static_cast<DataType>(m_Data.GetDefaultValue(embeddedID));
		}

		bool isLinkedValue(SvPb::EmbeddedIdEnum embeddedID) const
		{
			return m_Data.isLinkedValue(embeddedID);
		}

		template<typename DataType>
		DataType Get(SvPb::EmbeddedIdEnum embeddedID) const
		{
			return static_cast<DataType>(m_Data.GetValue(embeddedID));
		}

		template<>
		LinkedValueData Get(SvPb::EmbeddedIdEnum embeddedID) const
		{
			return m_Data.getLinkedData(embeddedID);
		}

		template<typename DataType>
		DataType GetMinValue(SvPb::EmbeddedIdEnum embeddedID) const
		{
			return static_cast<DataType>(m_Data.GetMinValue(embeddedID));
		}

		template<typename DataType>
		DataType GetMaxValue(SvPb::EmbeddedIdEnum embeddedID) const
		{
			return static_cast<DataType>(m_Data.GetMaxValue(embeddedID));
		}

		uint32_t GetAllowedAttributeFlags(SvPb::EmbeddedIdEnum embeddedID) const
		{
			return GetAllowedAttribute(m_Data.GetInspectionID(), m_Data.GetObjectID(embeddedID));
		}

		template<typename DataType>
		bool SetDefault(SvPb::EmbeddedIdEnum embeddedID, const DataType& rValue)
		{
			_variant_t Value(rValue);
			bool retVal = m_Data.SetDefaultValue(embeddedID, Value);
			if (false == retVal)
			{
				SvStl::MessageManager e(SvStl::MsgType::Display);
				e.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetOfValueFailed, {std::to_string(embeddedID)}, SvStl::SourceFileParams(StdMessageParams));
			}

			return retVal;
		}

		template<typename DataType>
		bool Set(SvPb::EmbeddedIdEnum embeddedID, const DataType& rValue, int ArrayIndex = -1)
		{
			_variant_t Value(rValue);
			bool retVal = m_Data.SetValue(embeddedID, Value, ArrayIndex);
			if (false == retVal)
			{
				SvStl::MessageManager e(SvStl::MsgType::Display);
				e.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetOfValueFailed, {std::to_string(embeddedID)}, SvStl::SourceFileParams(StdMessageParams));
			}
			
			return retVal;
		}

		template<>
		bool Set(SvPb::EmbeddedIdEnum embeddedID, const LinkedValueData& data, int /*ArrayIndex = -1*/)
		{
			bool retVal = m_Data.setLinkedData(embeddedID, data);
			if (false == retVal)
			{
				SvStl::MessageManager e(SvStl::MsgType::Display);
				e.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetOfValueFailed, {std::to_string(embeddedID)}, SvStl::SourceFileParams(StdMessageParams));
			}

			return retVal;
		}

		void setValues(const std::vector< std::pair<SvPb::EmbeddedIdEnum, std::variant<_variant_t, LinkedValueData>>>& values)
		{
			for (const auto& value : values)
			{
				if (0 == value.second.index())
				{
					m_Data.SetValue(value.first, std::get<0>(value.second));
				}
				else
				{
					m_Data.setLinkedData(value.first, std::get<1>(value.second));
				}
			}
		}

		SvDef::NameValueVector GetEnumTypes(SvPb::EmbeddedIdEnum embeddedID) const
		{
			return GetEnums(m_Data.GetInspectionID(), m_Data.GetObjectID(embeddedID));
		}

		std::string GetName(SvPb::EmbeddedIdEnum embeddedID) const
		{
			return GetObjectName(m_Data.GetInspectionID(), m_Data.GetObjectID(embeddedID));
		}

		HRESULT SetName(SvPb::EmbeddedIdEnum embeddedID, const std::string& name)
		{
			return setObjectName(m_Data.GetInspectionID(), m_Data.GetObjectID(embeddedID), name);
		}

		HRESULT Commit(PostAction doAction = PostAction::doRunOnce, bool shouldDisplayErrors = false)
		{
			try
			{
				SetValues(m_Data, doAction);

				//If a runonce is done then read values as they may have changed
				if (doAction & PostAction::doRunOnce)
				{
					GetValues(m_Data);
				}
			}
			catch (const SvStl::MessageContainerVector& rSvE)
			{
				if (shouldDisplayErrors && 0 < rSvE.size())
				{
					SvStl::MessageManager e(SvStl::MsgType::Log | SvStl::MsgType::Display);
					e.setMessage(rSvE[0].getMessage());
				}
				return E_FAIL;
			}
			catch (const SvStl::MessageContainer& rSvE)
			{
				if (shouldDisplayErrors)
				{
					SvStl::MessageManager e(SvStl::MsgType::Log | SvStl::MsgType::Display);
					e.setMessage(rSvE.getMessage());
				}
				return E_FAIL;
			}
			return S_OK;

		}

		void SetTaskID(uint32_t taskID) { return m_Data.SetTaskID(taskID); }
		uint32_t GetTaskID() const { return m_Data.GetTaskID(); }
		void SetInspectionID(uint32_t inspectionID) { return m_Data.SetInspectionID(inspectionID); }
		uint32_t GetInspectionID() const { return m_Data.GetInspectionID(); }
		uint32_t GetObjectID(uint32_t rEmbeddedID) const { return m_Data.GetObjectID((SvPb::EmbeddedIdEnum) rEmbeddedID); }
		bool resetTaskObject() 
		{ 
			try
			{
				ResetObject(m_Data.GetInspectionID(), m_Data.GetTaskID());
			}
			catch(  ... )
			{
				return false;
			}
			return true;

		}

		SvDef::StringVector getStringVector(SvPb::EmbeddedIdEnum embeddedID)
		{
			return SvUl::getVectorFromOneDim<std::string>(Get<_variant_t>(embeddedID));
		}

		void writeStringVector(SvDef::StringVector& strings, SvPb::EmbeddedIdEnum embeddedID)
		{
			_variant_t var = SvUl::getVariantFromStringVector(strings);

			Set<_variant_t>(embeddedID, var);
		}

#pragma endregion Public Methods

#pragma region Member Variables
	private:
		BoundValues m_Data;
#pragma endregion Member Variables
	};


} //namespace SvOgu
