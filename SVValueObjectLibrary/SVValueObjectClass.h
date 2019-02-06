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
#include "SVStatusLibrary/ErrorNumbers.h"
#include "ObjectInterfaces/IValueObject.h"
#include "Definitions/TextDefineSVDef.h"
#include "SVMessage/SVMessage.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/SVToolsetScriptTags.h"
#include "SVOResource/resource.h"
#include "SVStatusLibrary/MessageManager.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "Definitions/GlobalConst.h"
#pragma endregion Includes

namespace SvVol
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
	typedef typename T					ValueType;
	typedef std::vector<ValueType>		ValueVector;

#pragma region Constructor
	SVValueObjectClass( LPCTSTR ObjectName );
	SVValueObjectClass( SVObjectClass* pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVVALUEOBJECT );

	virtual ~SVValueObjectClass();

	const  SVValueObjectClass<T>& operator= (const  SVValueObjectClass<T>& rRhs);
#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure );
	virtual bool CloseObject() override;
	virtual HRESULT SetArraySize(int iSize);
	virtual HRESULT SetOutputFormat(OutputFormat outputFormat) { return E_NOTIMPL; };
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* pDataObject );
	virtual HRESULT SetValue( const T& rValue, int Index = -1 );
	virtual HRESULT GetValue(T&  rValue, int Index = -1) const;
	virtual HRESULT SetDefaultValue( const T& rValue, bool bResetAll = true );
	const ValueType& GetDefaultValue() const { return m_DefaultValue; };

	HRESULT SetArrayValues(typename ValueVector::const_iterator BeginIter, typename ValueVector::const_iterator EndIter);
	HRESULT SetArrayValues(const ValueVector& rValues);
	virtual HRESULT GetArrayValues(ValueVector& rValues) const;

	HRESULT SetResultSize(int  ResultSize) { m_ResultSize = (ResultSize <= m_ArraySize) ? ResultSize : 0; return S_OK; };
	HRESULT SetTypeName( LPCTSTR TypeName );
	bool CompareWithCurrentValue( const std::string& rCompare ) const;
	
#pragma region virtual method (IObjectClass/IValueObject)
	//! For these methods see IObject documentation
	virtual HRESULT getValue(double& rValue, int Index = -1) const override;
	virtual HRESULT getValues(std::vector<double>& rValues) const override;
	virtual void Persist(SvOi::IObjectWriter& rWriter) override;
	//! For these methods see IValueObject documentation
	virtual HRESULT setDefaultValue(const _variant_t& rValue) override { return SetDefaultValue(Variant2ValueType(rValue), false); }
	virtual _variant_t getDefaultValue() const override { return ValueType2Variant(m_DefaultValue); };
	virtual HRESULT setValue(const _variant_t& rValue, int Index = -1) override;
	virtual HRESULT getValue(_variant_t& rValue, int Index = -1) const override;
	virtual HRESULT getValues(std::vector<_variant_t>&  rValues) const override;
	virtual HRESULT setValue(const std::string& rValue, int Index = -1) override;
	virtual HRESULT getValue(std::string& rValue, int Index = -1) const override;
	virtual void setResetOptions(bool p_bResetAlways, SvOi::SVResetItemEnum p_eResetItem) override;
	virtual void validateValue(const _variant_t& rValue) const override;
	virtual std::string getTypeName() const override { return m_TypeName; };
	virtual bool isArray() const override { return 1 < m_ArraySize; };
	virtual int getArraySize() const override { return m_ArraySize; };
	virtual int getResultSize() const override { return m_ResultSize; };
	virtual SvOi::SVResetItemEnum getResetItem() const override { return m_eResetItem; };
	virtual bool ResetAlways() const override {	return m_ResetAlways; };
	virtual DWORD GetByteSize() const override;
	virtual DWORD GetType() const override { return ValueType2Variant(m_Value).vt; };
	virtual HRESULT CopyToMemoryBlock(BYTE* pMemoryBlock, DWORD MemByteSize, int Index = -1) const override;
	virtual void setSaveValueFlag(bool shouldSaveValue) override { m_shouldSaveValue = shouldSaveValue; };
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

	virtual double ValueType2Double(const T& rValue) const = 0;
	virtual _variant_t ValueType2Variant(const T& rValue) const = 0;
	virtual T Variant2ValueType(const _variant_t& rValue) const = 0;

	void init();

	//! Convert String to template type !!can throw Exception!!
	//! If value invalid an exception message will be thrown.
	//! \param rValue [in] String to convert
	/// \returns the Value converted!
	virtual T ConvertString2Type( const std::string& rValue ) const = 0;

	//! Convert template type to String
	//! \param rValue [in] Type to convert
	/// \returns the std::string
	virtual std::string ConvertType2String( const T& rValue ) const = 0;

	HRESULT ValidateIndex(int ArrayIndex) const;

	//! Validates the parameters required for the CopyToMemoryBlock method
	//! \param pMemoryBlock [in] Pointer to the byte address of the memory block
	//! \param MemByteSize [in] The memory block byte size
	//! \param Index [in] The index of the array (-1 if no array)
	/// \returns S_OK on success
	HRESULT ValidateMemoryBlockParameters(BYTE* pMemoryBlock, DWORD MemByteSize, int Index) const;

	ValueType& DefaultValue() { return m_DefaultValue; };
	ValueType& Value() { return m_Value; };
	const ValueType& Value() const { return m_Value; };
	ValueVector& ValueArray() { return m_ValueArray; };

	ValueType* getValuePointer(int Index);

	std::string FormatOutput(const T& rValue) const;
	void setOutputFormat( LPCTSTR OutFormat ) { m_OutFormat = OutFormat; };
	LPCTSTR getOutputFormat() const { return m_OutFormat.c_str(); };
	bool isLegacyVectorObjectCompatibility() const { return m_LegacyVectorObjectCompatibility; };

	void swap( SVValueObjectClass& rRhs );

	/// Write the Values of this object to the IObjectWriter
	/// \param rWriter [in,out] The IObjectWriter
	virtual void WriteValues(SvOi::IObjectWriter& rWriter) = 0;

	/// Write the Default-Values of this object to the IObjectWriter
	/// \param rWriter [in,out] The IObjectWriter
	virtual void WriteDefaultValues(SvOi::IObjectWriter& rWriter) = 0;

	/// !!can throw Exception!!
	T convertVariantValue(const _variant_t& rValue) const;
	/// !!can throw Exception!!
	ValueVector variant2VectorType(const _variant_t& rValue) const;
	/// Uses move semantics
	_variant_t vectorType2SafeArray() const;
#pragma endregion Protected Methods

#pragma region Member Variables
private:
	std::string m_TypeName;					//The data type name
	bool m_shouldSaveValue;					//If true, the value will be saved in configuration file, else it will not be saved and after loading the configuration it is default value.
	bool m_shouldSaveDefaultValue;			//If true, the default value will be saved in configuration file, else it will not be saved and after loading the configuration it is default of the default value.
	bool m_ResetAlways;
	bool m_LegacyVectorObjectCompatibility;
	std::string m_OutFormat;					//This is used to format the value object to a string

	SvOi::SVResetItemEnum m_eResetItem;

	int m_ArraySize;
	int m_ResultSize;

	ValueType m_DefaultValue;
	ValueType m_Value;
	ValueVector m_ValueArray;
#pragma endregion Member Variables
};

} //namespace SvVol

#pragma region Inline
#include "SVValueObjectClass.inl"
#pragma endregion Inline
