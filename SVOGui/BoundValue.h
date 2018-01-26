//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Bound Value class for use by thye GUI.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <comdef.h>
//Moved to precompiled header: #include <guiddef.h>
#pragma endregion Includes

namespace SvOg
{
	enum PostAction
	{
		doNothing = 0,
		doRunOnce,
		doReset,
		doResetRunOnce
	};

	class BoundValue
	{
#pragma region Constructor
	public:
		BoundValue() {};
		BoundValue(const GUID& rEmbeddedID, const GUID& rInstanceID, const variant_t& rValue, const variant_t& rDefaultValue)
			: m_embeddedID(rEmbeddedID), m_instanceID(rInstanceID), m_Value(rValue), m_DefaultValue(rDefaultValue) {}
			
		~BoundValue() { }
	#pragma endregion Constructor

	#pragma region Public Methods
	public:
		const variant_t& GetDefaultValue() const { return m_DefaultValue; }
		const variant_t& GetValue() const { return m_Value; }
		int GetArrayIndex() const { return m_ArrayIndex; }

		void SetDefaultValue(const variant_t& rDefaultValue)
		{
			if (rDefaultValue != m_DefaultValue)
			{
				m_DefaultValue = rDefaultValue;
				m_bDefaultModified = true;
			}
		}

		void SetValue(const variant_t& rValue, int ArrayIndex = -1)
		{
			if (rValue != m_Value)
			{
				m_Value = rValue;
				m_ArrayIndex = ArrayIndex;
				m_bModified = true;
			}
		}

		const GUID& GetEmbeddedID() const { return m_embeddedID; }
		const GUID& GetObjectID() const { return m_instanceID; }

		bool isModified() const { return m_bModified; }
		bool isDefaultModified() const { return m_bDefaultModified; }
		//Need to make this const to be able to clear the flag in other const methods
		void ClearModified() const { m_bModified = false; }
		void ClearArrayIndex() const { m_ArrayIndex = -1; }
		void ClearDefaultModified() const { m_bModified = false; }
#pragma endregion Public Methods

	#pragma region Member Variables
	private:
		GUID m_embeddedID{ GUID_NULL };
		GUID m_instanceID{ GUID_NULL };
		variant_t m_DefaultValue;
		variant_t m_Value;
		mutable bool m_bModified{ false };
		mutable bool m_bDefaultModified{ false };
		mutable int m_ArrayIndex{ -1 };
#pragma endregion Member Variables
	};

	class BoundValues
	{
	public:
		typedef std::map<GUID, BoundValue> Container;
		typedef Container::iterator iterator;
		typedef Container::const_iterator const_iterator;
		typedef Container::value_type value_type;
		typedef Container::key_type key_type;
		typedef Container::mapped_type mapped_type;

	#pragma region Constructor
	public:
		BoundValues(const GUID& rInspectionID, const GUID& rTaskID, bool ReadOnly = false)
			: m_inspectionID(rInspectionID), m_TaskID(rTaskID), m_ReadOnly{ ReadOnly } {}
		~BoundValues() {}

		BoundValues() = delete;
	#pragma endregion Constructor

	#pragma region Public Methods
	public:
		Container::_Pairib insert(const key_type& rKey, const mapped_type& rMapped) { return m_values.insert(value_type(rKey, rMapped)); }
		mapped_type& operator[](const key_type& rKey) { return m_values[rKey]; }

		const GUID& GetTaskID() const { return m_TaskID; };
		const GUID& GetInspectionID() const { return m_inspectionID; }

		variant_t GetDefaultValue(const GUID& rEmbeddedID) const
		{
			Container::const_iterator it = m_values.find(rEmbeddedID);
			if (m_values.end() != it)
			{
				return it->second.GetDefaultValue();
			}
			return variant_t();
		}

		variant_t GetValue(const GUID& rEmbeddedID) const
		{
			Container::const_iterator it = m_values.find(rEmbeddedID);
			if (m_values.end() != it)
			{
				return it->second.GetValue();
			}
			return variant_t();
		}

		bool SetDefaultValue(const GUID& rEmbeddedID, const variant_t& rDefaultValue)
		{
			if (!m_ReadOnly)
			{
				Container::iterator it = m_values.find(rEmbeddedID);
				if (m_values.end() != it)
				{
					it->second.SetDefaultValue(rDefaultValue);
					return true;
				}
			}
			return false;
		}

		bool SetValue(const GUID& rEmbeddedID, const variant_t& rValue, int ArrayIndex = -1)
		{
			if (!m_ReadOnly)
			{
				Container::iterator it = m_values.find(rEmbeddedID);
				if (m_values.end() != it)
				{
					it->second.SetValue(rValue, ArrayIndex);
					return true;
				}
			}
			return false;
		}

		GUID GetObjectID(const GUID& rEmbeddedID) const
		{
			GUID objectID = GUID_NULL;
			Container::const_iterator it = m_values.find(rEmbeddedID);
			if (it != m_values.end())
			{
				objectID = it->second.GetObjectID();
			}
			return objectID;
		}

		iterator begin() { return m_values.begin(); }
		iterator end() { return m_values.end(); }
		const_iterator begin() const { return m_values.begin(); }
		const_iterator end() const { return m_values.end(); }
	#pragma endregion Public Methods

	#pragma region Member Variables
	private:
		Container m_values;
		GUID m_inspectionID{ GUID_NULL };
		GUID m_TaskID{ GUID_NULL };
		bool m_ReadOnly{ false };
	#pragma endregion Member Variables
	};
} //namespace SvOg
