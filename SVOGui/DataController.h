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
		DataType GetDefault(const GUID& rEmbeddedID) const
		{
			return static_cast<DataType>(m_Data.GetDefaultValue(rEmbeddedID));
		}

		template<typename DataType>
		DataType Get(const GUID& rEmbeddedID) const
		{
			return static_cast<DataType>(m_Data.GetValue(rEmbeddedID));
		}

		template<typename DataType>
		bool SetDefault(const GUID& rEmbeddedID, const DataType& rValue)
		{
			_variant_t Value(rValue);
			return m_Data.SetDefaultValue(rEmbeddedID, Value);
		}

		template<typename DataType>
		bool Set(const GUID& rEmbeddedID, const DataType& rValue, int ArrayIndex = -1)
		{
			_variant_t Value(rValue);
			return m_Data.SetValue(rEmbeddedID, Value, ArrayIndex);
		}

		SvOi::NameValueVector GetEnumTypes(const GUID& rEmbeddedID) const
		{
			return GetEnums(m_Data.GetInspectionID(), m_Data.GetObjectID(rEmbeddedID));
		}

		std::string GetName(const GUID& rEmbeddedID) const
		{
			return GetObjectName(m_Data.GetInspectionID(), m_Data.GetObjectID(rEmbeddedID));
		}

		HRESULT Commit(PostAction doAction = doRunOnce)
		{
			return SetValues(m_Data, doAction);
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
