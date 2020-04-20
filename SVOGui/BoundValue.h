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
#include "SVProtoBuf/SVO-Enum.h"
#include "Definitions/ObjectDefines.h"
#pragma endregion Includes


namespace SvOg
{
enum class PostAction : unsigned int
{
	doNothing = 0,
	doRunOnce = 1 << 0,
	doReset = 1 << 1,
};

inline constexpr PostAction operator|(PostAction lhs, PostAction rhs)
{
	return static_cast<PostAction>(static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs));
}

inline constexpr bool operator&(PostAction lhs, PostAction  rhs)
{
	return (static_cast<unsigned int>(lhs) & static_cast<unsigned int>(rhs)) != 0;
}

class BoundValue
{
#pragma region Constructor
public:
	BoundValue() {};
	BoundValue(uint32_t instanceID, const variant_t& rValue, const variant_t& rDefaultValue)
		: m_instanceID(instanceID), m_Value(rValue), m_DefaultValue(rDefaultValue)
	{
	}

	~BoundValue() {}
#pragma endregion Constructor

#pragma region Public Methods
public:
	const variant_t& GetDefaultValue() const { return m_DefaultValue; }
	const variant_t& GetValue() const { return m_Value; }
	int GetArrayIndex() const { return m_ArrayIndex; }

	void SetDefaultValue(const variant_t& rDefaultValue)
	{
		bool isArray = VT_ARRAY == (m_DefaultValue.vt & VT_ARRAY);
		//_variant does not allow compare for arrays 
		if (isArray || rDefaultValue != m_DefaultValue)
		{
			m_DefaultValue = rDefaultValue;
			m_bDefaultModified = true;
		}
	}

	void SetValue(const variant_t& rValue, int ArrayIndex = -1)
	{
		bool isArray = VT_ARRAY == (m_Value.vt & VT_ARRAY);
		//_variant does not allow compare for arrays 
		if (isArray || rValue != m_Value)
		{
			m_Value = rValue;
			m_ArrayIndex = ArrayIndex;
			m_bModified = true;
		}
	}

	uint32_t GetObjectID() const { return m_instanceID; }

	bool isModified() const { return m_bModified; }
	bool isDefaultModified() const { return m_bDefaultModified; }
	//Need to make this const to be able to clear the flag in other const methods
	void ClearModified() const { m_bModified = false; }
	void ClearArrayIndex() const { m_ArrayIndex = -1; }
	void ClearDefaultModified() const { m_bModified = false; }
#pragma endregion Public Methods

#pragma region Member Variables
private:
	uint32_t m_instanceID{ SvDef::InvalidObjectId };
	variant_t m_DefaultValue;
	variant_t m_Value;
	mutable bool m_bModified {false};
	mutable bool m_bDefaultModified {false};
	mutable int m_ArrayIndex {-1};
#pragma endregion Member Variables
};

class BoundValues
{
public:
	typedef std::map<SvPb::EmbeddedIdEnum, BoundValue> Container;
	typedef Container::iterator iterator;
	typedef Container::const_iterator const_iterator;
	typedef Container::value_type value_type;
	typedef Container::key_type key_type;
	typedef Container::mapped_type mapped_type;

#pragma region Constructor
public:
	BoundValues(uint32_t inspectionID, uint32_t taskID, bool ReadOnly = false)
		: m_inspectionID(inspectionID), m_TaskID(taskID), m_ReadOnly {ReadOnly}
	{
	}
	~BoundValues() {}

	BoundValues() = delete;
#pragma endregion Constructor

#pragma region Public Methods
public:
	auto insert(const key_type& rKey, const mapped_type& rMapped) { return m_values.insert(value_type(rKey, rMapped)); }
	mapped_type& operator[](const key_type& rKey) { return m_values[rKey]; }

	void SetTaskID(uint32_t taskID) { m_TaskID = taskID; };
	uint32_t GetTaskID() const { return m_TaskID; };
	void SetInspectionID(uint32_t inspectionID) { m_inspectionID = inspectionID; };
	uint32_t GetInspectionID() const { return m_inspectionID; }

	variant_t GetDefaultValue(SvPb::EmbeddedIdEnum embeddedID) const
	{
		Container::const_iterator it = m_values.find(embeddedID);
		if (m_values.end() != it)
		{
			return it->second.GetDefaultValue();
		}
		return variant_t();
	}

	variant_t GetValue(SvPb::EmbeddedIdEnum embeddedID) const
	{
		Container::const_iterator it = m_values.find(embeddedID);
		if (m_values.end() != it)
		{
			return it->second.GetValue();
		}
		return variant_t();
	}

	bool SetDefaultValue(SvPb::EmbeddedIdEnum embeddedID, const variant_t& rDefaultValue)
	{
		if (!m_ReadOnly)
		{
			Container::iterator it = m_values.find(embeddedID);
			if (m_values.end() != it)
			{
				it->second.SetDefaultValue(rDefaultValue);
				return true;
			}
		}
		return false;
	}

	bool SetValue(SvPb::EmbeddedIdEnum embeddedID, const variant_t& rValue, int ArrayIndex = -1)
	{
		if (!m_ReadOnly)
		{
			Container::iterator it = m_values.find(embeddedID);
			if (m_values.end() != it)
			{
				it->second.SetValue(rValue, ArrayIndex);
				return true;
			}
		}
		return false;
	}

	uint32_t GetObjectID(SvPb::EmbeddedIdEnum embeddedID) const
	{
		uint32_t objectID = SvDef::InvalidObjectId;
		Container::const_iterator it = m_values.find(embeddedID);
		if (it != m_values.end())
		{
			objectID = it->second.GetObjectID();
		}
		return objectID;
	}

	void clear()
	{
		m_values.clear();
	}

	iterator begin() { return m_values.begin(); }
	iterator end() { return m_values.end(); }
	const_iterator begin() const { return m_values.begin(); }
	const_iterator end() const { return m_values.end(); }
#pragma endregion Public Methods

#pragma region Member Variables
private:
	Container m_values;
	uint32_t m_inspectionID{ SvDef::InvalidObjectId };
	uint32_t m_TaskID { SvDef::InvalidObjectId };
	bool m_ReadOnly {false};
#pragma endregion Member Variables
};
} //namespace SvOg
