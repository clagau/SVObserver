//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVValueObject
//* .File Name       : $Workfile:   SVValueObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 15:08:08  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <set>
//Moved to precompiled header: #include <assert.h>
//Moved to precompiled header: #include <memory>
#include "ObjectInterfaces/IValueObject.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVOResource/resource.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvOi
{
class IInspectionProcess;
}

namespace SvVol //<SvValueObjectlibrary
{

#pragma region Declarations
enum OutputFormat
{
	OutputFormat_int,
	OutputFormat_hex,
};
#pragma endregion Declarations

template <typename T>
class SVValueObjectClass : public SVObjectClass, public SvOi::IValueObject
{
protected:
	using ValueType = typename T;
	using ValueVector = std::vector<ValueType>;

#pragma region Constructor
	explicit SVValueObjectClass( LPCTSTR ObjectName );
	explicit SVValueObjectClass( SVObjectClass* pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVVALUEOBJECT );

	virtual ~SVValueObjectClass();

	SVValueObjectClass<T>& operator= (const  SVValueObjectClass<T>& rRhs);
#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure );
	virtual bool CloseObject() override;
	virtual HRESULT SetArraySize(int32_t iSize);
	virtual HRESULT SetOutputFormat(OutputFormat ) { return E_NOTIMPL; };
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* pDataObject );
	virtual HRESULT SetValue( const T& rValue, int Index = -1 );
	virtual HRESULT GetValue(T&  rValue, int Index = -1) const;
	virtual HRESULT SetDefaultValue( const T& rValue, bool bResetAll = true );
	const ValueType& GetDefaultValue() const { return m_DefaultValue; };

	virtual HRESULT SetArrayValues(const ValueVector& rValues);
	virtual HRESULT GetArrayValues(ValueVector& rValues) const;

	void SetResultSize(int32_t  ResultSize);
	void SetTypeName( LPCTSTR TypeName ) { m_TypeName = TypeName; }
	bool CompareWithCurrentValue( const std::string& rCompare ) const;

	static void setMaxTextSize(int32_t maxSize) { m_maxTextSize = maxSize; }
	static int32_t getMaxTextSize() { return m_maxTextSize; }

#pragma region virtual method (IObjectClass/IValueObject)
	//! For these methods see IObject documentation
	virtual UINT SetObjectAttributesAllowed(UINT Attributes, SvOi::SetAttributeType Type) override;
	virtual HRESULT getValue(double& rValue, int Index = -1) const override;
	virtual HRESULT getValues(std::vector<double>& rValues) const override;
	virtual void Persist(SvOi::IObjectWriter& rWriter) override;
	//! For these methods see IValueObject documentation
	virtual HRESULT setDefaultValue(const _variant_t& rValue) override { return SetDefaultValue(Variant2ValueType(rValue), false); }
	virtual _variant_t getDefaultValue() const override { return ValueType2Variant(&m_DefaultValue); };
	virtual HRESULT setValue(const _variant_t& rValue, int Index = -1,bool fixArrasize = false) override;
	virtual HRESULT getValue(_variant_t& rValue, int Index = -1) const override;
	virtual HRESULT getValues(std::vector<_variant_t>&  rValues) const override;
	virtual HRESULT setValue(const std::string& rValueString, int Index = -1) override;
	virtual HRESULT getValue(std::string& rValueString, int Index = -1, const std::string& rFormatString = _T("")) const;
	virtual void setResetOptions(bool p_bResetAlways, SvOi::SVResetItemEnum p_eResetItem) override;
	virtual void validateValue(const _variant_t& rValue) const override;
	virtual std::string getTypeName() const override { return m_TypeName; };
	virtual bool isArray() const override { return 1 < m_ArraySize; };
	virtual int32_t getArraySize() const override { return m_ArraySize; };
	virtual int32_t getResultSize() const override { return m_ResultSize; };
	virtual SvOi::SVResetItemEnum getResetItem() const override { return m_eResetItem; };
	virtual bool ResetAlways() const override {	return m_ResetAlways; };
	virtual int32_t getByteSize(bool useResultSize, bool memBlockData) const override;
	virtual DWORD GetType() const override { return ValueType2Variant(valuePtr()).vt; };
	virtual void setSaveValueFlag(bool shouldSaveValue) override { m_shouldSaveValue = shouldSaveValue; };
	virtual void setTrData(int32_t memOffset, int32_t memSize, int32_t pos) override;
	virtual int32_t getTrPos() const override { return m_trPos; }
	virtual int32_t getMemOffset() const override { return m_memOffset; }
	///Note that setMemBlockPointer has specialized versions for DoubleSortValue, SVStringValue and SVVariantValue 
	virtual void setMemBlockPointer(uint8_t* pMemBlockBase) override;
	///Only specialized versions, namely DoubleSortValue, SVStringValue and SVVariantValue need an implementation
	virtual void updateMemBlockData() override {}
	virtual bool isIndirectValue() const override { return false; };
	virtual void setStandardFormatString() = 0;
	virtual std::string getFixedWidthFormatString(uint32_t totalWidth, uint32_t decimals) override;


#pragma endregion virtual method (IObjectClass/IValueObject)
	
	void SetLegacyVectorObjectCompatibility() { m_LegacyVectorObjectCompatibility = true; }

	/// getter and setter for ShouldSaveValue-Flag
	bool shouldSaveValue() { return m_shouldSaveValue; };

	/// getter and setter for ShouldSaveDefaultValue-Flag
	bool shouldSaveDefaultValue() { return m_shouldSaveDefaultValue; };
	void setSaveDefaultValueFlag(bool isValueSaved) { m_shouldSaveDefaultValue = isValueSaved; };
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	void Initialize();
	void init();

	virtual ValueType* reserveLocalMemory();
	virtual void clearMemoryBlockPointer() {m_pValue = (-1 == m_memOffset) ? m_pValue : nullptr; }

	///Note these virtual functions cannot be defined here, only in the defined classes until
	///if constexpr is available due to different types
	virtual double ValueType2Double(const ValueType& rValue) const = 0;
	virtual _variant_t ValueType2Variant(const ValueType* pValue) const = 0;
	virtual ValueType Variant2ValueType(const _variant_t& rValue) const = 0;


	//! Convert String to template type !!can throw Exception!!
	//! If value invalid an exception message will be thrown.
	//! \param rValue [in] String to convert
	/// \returns the Value converted!
	virtual T ConvertString2Type( const std::string& rValue ) const = 0;

	//! Convert template type to String
	//! \param rValue [in] Type to convert
	/// \returns the std::string
	virtual std::string ConvertType2String( const T& rValue ) const = 0;

	virtual HRESULT ValidateIndex(int ArrayIndex) const;

	ValueType& DefaultValue() { return m_DefaultValue; };
	ValueType* valuePtr() { return m_pValue; }
	const ValueType* valuePtr() const { return m_pValue; }

	std::string FormatOutput(const T& rValue, const std::string& rFormatString=_T("")) const;
	void setOutputFormat( LPCTSTR OutFormat ) { m_OutFormat = OutFormat; };
	LPCTSTR getOutputFormat() const { return m_OutFormat.c_str(); };
	bool isLegacyVectorObjectCompatibility() const { return m_LegacyVectorObjectCompatibility; };

	/// Write the Values of this object to the IObjectWriter
	/// \param rWriter [in,out] The IObjectWriter
	virtual void WriteValues(SvOi::IObjectWriter& rWriter) = 0;

	/// Write the Default-Values of this object to the IObjectWriter
	/// \param rWriter [in,out] The IObjectWriter
	virtual void WriteDefaultValues(SvOi::IObjectWriter& rWriter) = 0;

	/// !!can throw Exception!!
	T convertVariantValue(const _variant_t& ) const;
	/// !!can throw Exception!!
	ValueVector variant2VectorType(const _variant_t& rValue) const;

	_variant_t vectorType2SafeArray(long arraySize) const;

	void setResultSizePointer(int32_t* pResultSize);
	int32_t getMemSizeReserved() const { return m_memSizeReserved; }
	void setHasChanged(bool hasChanged) const { m_hasChanged = hasChanged; }
	bool hasChanged() const { return m_hasChanged; }
#pragma endregion Protected Methods

#pragma region Member Variables
private:
	std::string m_TypeName;					//The data type name
	bool m_shouldSaveValue{true};			//If true, the value will be saved in configuration file, else it will not be saved and after loading the configuration it is default value.
	bool m_shouldSaveDefaultValue{false};	//If true, the default value will be saved in configuration file, else it will not be saved and after loading the configuration it is default of the default value.
	bool m_ResetAlways{false};
	bool m_LegacyVectorObjectCompatibility{false};
	std::string m_OutFormat;				//This is used to format the value object to a string

	SvOi::SVResetItemEnum m_eResetItem;

	//NOTE! Never access these variables directly as getResultSize and getArraySize are overloaded
	//>Use int_32_t to make sure it stays constant in size for Shared Memory
	int32_t m_ArraySize {0L};
	int32_t m_ResultSize {0L};
	int32_t* m_pResultSize{nullptr};

	ValueType m_DefaultValue;
	///This value object pointer is always a pointer to the template type, for SVStringValueObjectClass and SVVariantValueObjectClass
	///the memory block pointer is stored in the derived class
	ValueType* m_pValue{nullptr};

	std::vector<uint8_t> m_valueData;	///Local memory when value object not part of inspection or during startup

	int32_t m_memOffset {-1L};		///The memory offset in the inspection data
	int32_t m_memSizeReserved{0L};	///The block memory size reserved
	int32_t m_trPos {-1L};			///The trigger record position
	mutable bool m_hasChanged{true};///Flag if value has changed since last update

	static int32_t m_maxTextSize;		///This is a global size for all text values
#pragma endregion Member Variables
};

} //namespace SvVol

#pragma region Inline
#include "SVValueObjectClass.inl"
#pragma endregion Inline
