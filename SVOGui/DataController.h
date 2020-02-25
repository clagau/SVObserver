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
#pragma endregion Includes

namespace SvOg
{
	template <typename Command, typename Model>
	class DataController : public Command
	{
	#pragma region Constructor
	public:
		DataController(const Model& rModel) : m_Data(rModel) {}
		virtual ~DataController() {}

		DataController() = delete;
		DataController& operator=(const DataController&) = delete;
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

		HRESULT Commit(PostAction doAction = PostAction::doRunOnce, bool shouldDisplayErrors = false)
		{
			try
			{
				SetValues(m_Data, doAction);

				//If a runonce is done then read values as they may have changed
				if(doAction & PostAction::doRunOnce)
				{
					GetValues(m_Data);
				}
			}
			catch (const SvStl::MessageContainerVector& rSvE)
			{
				if (shouldDisplayErrors && 0<rSvE.size())
				{
					SvStl::MessageMgrStd e(SvStl::MsgType::Log | SvStl::MsgType::Display);
					e.setMessage(rSvE[0].getMessage());
				}
				return E_FAIL;
			}
			catch (const SvStl::MessageContainer& rSvE)
			{
				if (shouldDisplayErrors)
				{
					SvStl::MessageMgrStd e(SvStl::MsgType::Log | SvStl::MsgType::Display);
					e.setMessage(rSvE.getMessage());
				}
				return E_FAIL;
			}
			return S_OK;

		}

		void SetTaskID(const GUID& rTaskID) { return m_Data.SetTaskID(rTaskID); }
		const GUID& GetTaskID() const { return m_Data.GetTaskID(); }
		void SetInspectionID(const GUID& rInspectionID) { return m_Data.SetInspectionID(rInspectionID); }
		const GUID& GetInspectionID() const { return m_Data.GetInspectionID(); }
		GUID GetObjectID(const GUID& rEmbeddedID) const { return m_Data.GetObjectID(rEmbeddedID); }
	#pragma endregion Public Methods

	#pragma region Member Variables
	private:
		Model m_Data;
	#pragma endregion Member Variables
	};
} //namespace SvOg
