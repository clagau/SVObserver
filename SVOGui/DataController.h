//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the DataController class for use by the GUI.
//******************************************************************************
#pragma once

#pragma region Includes
#include "BoundValue.h"
#include "ObjectInterfaces\NameValueVector.h"
#include "SVStatusLibrary\MessageContainer.h"
#include "SVStatusLibrary\MessageManager.h"
#include "ValuesAccessor.h"
#pragma endregion Includes

namespace SvOg
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

		template<typename DataType>
		DataType GetDefault(SvPb::EmbeddedIdEnum embeddedID) const
		{
			return static_cast<DataType>(m_Data.GetDefaultValue(embeddedID));
		}

		template<typename DataType>
		DataType Get(SvPb::EmbeddedIdEnum embeddedID) const
		{
			return static_cast<DataType>(m_Data.GetValue(embeddedID));
		}

		uint32_t GetAllowedAttributeFlags(SvPb::EmbeddedIdEnum embeddedID) const
		{
			return GetAllowedAttribute(m_Data.GetInspectionID(), m_Data.GetObjectID(embeddedID));
		}

		template<typename DataType>
		bool SetDefault(SvPb::EmbeddedIdEnum embeddedID, const DataType& rValue)
		{
			_variant_t Value(rValue);
			return m_Data.SetDefaultValue(embeddedID, Value);
		}

		template<typename DataType>
		bool Set(SvPb::EmbeddedIdEnum embeddedID, const DataType& rValue, int ArrayIndex = -1)
		{
			_variant_t Value(rValue);
			return m_Data.SetValue(embeddedID, Value, ArrayIndex);
		}

		SvOi::NameValueVector GetEnumTypes(SvPb::EmbeddedIdEnum embeddedID) const
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
#pragma endregion Public Methods

#pragma region Member Variables
	private:
		BoundValues m_Data;
#pragma endregion Member Variables
	};


} //namespace SvOg
