//******************************************************************************
//! COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
//! All Rights Reserved
//******************************************************************************
//! Interface class for the SVValueObject class
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <utility>
//Moved to precompiled header: #include <comdef.h>

#pragma endregion Includes

namespace SvOi
{
	enum SVResetItemEnum //! used with Embedded Object registration.
	{
		//!  Used in cases where changing the Tool value can have an impact on 
		//! 	dependent Tools. In most cases this would be overkill.	
		SVResetItemIP = 0,

		//!  Generally the default.  
		SVResetItemTool = 1,

		//!  An optimization IF you are editing a sub-task object (friend or child)
		//!  and you can get away with only parent task for some duration.  Usually  
		//!  requires "manual" Tool level reset when accepting sub-task values. 
		SVResetItemOwner = 2,

		//	An SVResetItemNone example are Tool Results, which do not require reseting the 
		//!  Tool, Owner, or IP when their values change.
		SVResetItemNone = 3,
		SVResetItemSize,
	};

	class IValueObject
	{
	public:
		virtual ~IValueObject() {}

		//! Sets the default value for the Value object
		//! \param rValue [in] The value to set the Default Value object to
		//! \returns S_OK if succeeded
		virtual HRESULT setDefaultValue(const _variant_t& rValue) = 0;

		//! Gets the default value for the Value object
		//! \returns the default value as a variant
		virtual _variant_t getDefaultValue() const = 0;

		//! Sets the value for Value object
		//! \param rValue [in] The value to set the Value object to
		//! \param Index [in] The corresponding array index to write to, if required
		//! \returns S_OK if succeeded
		//!\param fixArrasize [in] if this true the arraysisize is unchanged in every case
		virtual HRESULT setValue(const _variant_t& rValue, int Index = -1, bool fixArrasize = false) = 0;

		//! Gets the value for Value object
		//! \param rValue [out] The reference to write the value to
		//! \param Index [in] The corresponding array index to write to, if required
		//! \returns S_OK if succeeded
		virtual HRESULT getValue(_variant_t& rValue, int Index = -1) const = 0;

		//! Gets the values for Value object
		//! \param rValue [out] The reference to _variant_t vector to store the values
		//! \returns S_OK if succeeded
		virtual HRESULT getValues(std::vector<_variant_t>& rValues) const = 0;

		//! Sets the value for Value object as determined from rValueString
		//! \param rValueString [in] The value to set the Value object to
		//! \param Index [in] The corresponding array index to write to, if required
		//! \returns S_OK if succeeded
		virtual HRESULT setValue(const std::string& rValueString, int Index = -1) = 0;

		//! Writes the contained value into rValueString (using rFormatString if different from _T(""))
		//! \param rValueString [out] The reference to write the value to
		//! \param Index [in] The corresponding array index to write to, if required
		//! \param rFormatString [in] the formatString be used
		//! \returns S_OK if succeeded
		virtual HRESULT getValue(std::string& rValueString, int Index = -1, const std::string& rFormatString = _T("")) const = 0;

		//! Set the reset options for the value object
		//! \param bResetAlways [in] 
		//! \param eResetItem [in] Which item to reset
		virtual void setResetOptions(bool bResetAlways, SVResetItemEnum eResetItem) = 0;

		//! Validate the value. If value invalid an exception message will be thrown.
		//! \param rValue [in] The value to validate
		virtual void validateValue(const _variant_t& rValue, const _variant_t& rDefaultValue) const = 0;

		//! Gets the type name for the Value object
		//! \returns the type name
		virtual std::string getTypeName() const = 0;

		//! Checks if the value object is an array
		//! \returns true if an array
		virtual bool isArray() const = 0;

		//! Gets the size of the value object array
		//! \returns size 0 if not an array
		virtual int32_t getArraySize() const = 0;

		//! Gets the result size of the value object
		//! \returns size
		virtual int32_t getResultSize() const = 0;

		//! Gets the reset item type
		//! \returns the reset item enum type
		virtual SVResetItemEnum getResetItem() const = 0;

		//! Gets the reset always value object flag
		//! \returns true if reset always
		virtual bool ResetAlways() const = 0;

		//! Returns the value object byte size requirement
		//! \param useResultSize true for result bytes otherwise array bytes
		//! \param memBlockData true for block data memory size
		//! \returns the number of bytes for the data
		virtual int32_t getByteSize(bool useResultSize, bool memBlockData) const = 0;

		//! Returns the variant type of the value object
		//! \returns the VT type
		virtual DWORD GetType() const = 0;

		//! Sets the save flag for the value object
		//! \param shouldSaveValue [in] flag determining if value saved
		virtual void setSaveValueFlag(bool shouldSaveValue) = 0;

		//! Sets the Trigger Record data
		//! \param memOffset, the memory offset from the memory block base
		//! \param memSize, the memory size reserved
		//! \param pos, TR position
		virtual void setTrData(int32_t memOffset, int32_t memSize, int32_t pos) = 0;

		//! Gets the Trigger Record position
		//! \returns the position
		virtual int32_t getTrPos() const = 0;

		//! Gets the memory block offset is -1 if not set
		//! \returns the offset
		virtual int32_t getMemOffset() const = 0;

		//! Sets the memory block pointer
		//! \param pMemBlockBase, this is the memory block base pointer
		virtual void setMemBlockPointer(uint8_t* pMemBlockBase) = 0;

		//! Update the memory block data
		virtual void updateMemBlockData() = 0;

		//! is true for linkedvalue with indirect value set
		virtual bool isIndirectValue() const = 0;

		//! Sets a 'standard' format string to be used when the value of the object value needs to be output
		virtual void setStandardFormatString() = 0;

		// Creates a format string yielding a fixed length output string to be used when the value of the object value needs to be output
		virtual std::string getFixedWidthFormatString(uint32_t totalWidth, uint32_t decimals) = 0;

	};

	struct SetValueStruct 
	{
		SetValueStruct(IValueObject* pValueObject, _variant_t Value, _variant_t defaultValue, int Index) :
			m_pValueObject{ pValueObject }, m_Value{ Value }, m_DefaultValue{ defaultValue }, m_ArrayIndex{ Index } {}

		IValueObject* m_pValueObject{ nullptr };
		_variant_t m_Value;
		_variant_t m_DefaultValue;
		int m_ArrayIndex{ -1 };
	};

	typedef std::set<IValueObject*> IValueObjectPtrSet;
	typedef std::vector<SetValueStruct> SetValueStructVector;

} //namespace SvOi
