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
#include "SVUtilityLibrary/SVSharedPtr.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVUtilityLibrary/SVSafeArray.h"
#include "Definitions/GlobalConst.h"
#pragma endregion Includes

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
	typedef std::vector<ValueType>		Bucket;
	typedef std::vector<ValueVector>	BucketVector;
	typedef std::unique_ptr<Bucket>		BucketPtr;
	typedef std::unique_ptr<BucketVector> BucketVectorPtr;

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
	virtual bool ResetObject( SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
	virtual void SetObjectDepthWithIndex( int NewObjectDepth, int NewLastSetIndex ) override;
	virtual HRESULT SetArraySize(int iSize);
	virtual HRESULT SetOutputFormat(OutputFormat outputFormat) { return E_NOTIMPL; };
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* pDataObject );
	virtual HRESULT SetValue( const T& rValue, int Index = -1 );
	virtual HRESULT GetValue(T&  rValue, int Index = -1, int Bucket = -1) const;
	virtual HRESULT SetDefaultValue( const T& rValue, bool bResetAll = true );
	const ValueType& GetDefaultValue() const { return m_DefaultValue; };

	HRESULT SetArrayValues(typename ValueVector::const_iterator BeginIter, typename ValueVector::const_iterator EndIter);
	HRESULT SetArrayValues(const ValueVector& rValues);
	virtual HRESULT GetArrayValues(ValueVector& rValues, int Bucket = -1) const;

	HRESULT SetResultSize(int  ResultSize) { m_ResultSize = (ResultSize <= m_ArraySize) ? ResultSize : 0; return S_OK; };
	HRESULT SetTypeName( LPCTSTR TypeName );
	bool CompareWithCurrentValue( const std::string& rCompare ) const;
	void setStatic( bool isStatic ) { m_isStatic = isStatic; };
	

#pragma region virtual method
	virtual UINT SetObjectAttributesSet( UINT Attributes, SvOi::SetAttributeType Type, int iIndex=0 ) override;

	//! Gets the value for Value object NOTE this comes from IObjectClass
	//! \param rValue [out] The reference to write the value to
	//! \param Index [in] The corresponding array index to write to, if required
	//! \param Bucket [in] The corresponding bucket index to get, if required
	//! \returns S_OK if succeeded
	virtual HRESULT getValue(double& rValue, int Index = -1, int Bucket = -1) const override;

	//! Gets the values for Value object NOTE this comes from IObjectClass
	//! \param rValue [out] The reference to double vector to store the values
	//! \param Bucket [in] The corresponding bucket index to get, if required
	//! \returns S_OK if succeeded
	virtual HRESULT getValues(std::vector<double>& rValues, int Bucket = -1) const override;

	//! Gets the default value for the Value object
	//! \returns the default value as a variant
	virtual _variant_t getDefaultValue() const override { return ValueType2Variant(m_DefaultValue); };

	//! Sets the value for Value object
	//! \param rValue [in] The value to set the Value object to
	//! \param Index [in] The corresponding array index to write to, if required
	//! \returns S_OK if succeeded
	virtual HRESULT setValue(const _variant_t& rValue, int Index = -1) override;

	//! Gets the value for Value object
	//! \param rValue [out] The reference to write the value to
	//! \param Index [in] The corresponding array index to write to, if required
	//! \param Bucket [in] The corresponding bucket index to get, if required
	//! \returns S_OK if succeeded
	virtual HRESULT getValue(_variant_t& rValue, int Index = -1, int Bucket = -1) const override;

	//! Gets the values for Value object
	//! \param rValue [out] The reference to _variant_t vector to store the values
	//! \param Bucket [in] The corresponding bucket index to get, if required
	//! \returns S_OK if succeeded
	virtual HRESULT getValues(std::vector<_variant_t>&  rValues, int Bucket = -1) const override;

	//! Sets the value for Value object
	//! \param rValue [in] The value to set the Value object to
	//! \param Index [in] The corresponding array index to write to, if required
	//! \returns S_OK if succeeded
	virtual HRESULT setValue( const std::string& rValue, int Index = -1 ) override;

	//! Gets the value for Value object
	//! \param rValue [out] The reference to write the value to
	//! \param Index [in] The corresponding array index to write to, if required
	//! \param Bucket [in] The corresponding bucket index to get, if required
	//! \returns S_OK if succeeded
	virtual HRESULT getValue(std::string& rValue, int Index = -1, int Bucket = -1) const override;

	//! Set the value object bucketized flag
	//! \param Bucketized [in] true if bucketized (Only set if not a static ValueObject
	virtual void setBucketized( bool isBucketized ) override { m_isBucketized = m_isStatic ? false : isBucketized; };

	//! Set the reset options for the value object
	//! \param bResetAlways [in] 
	//! \param eResetItem [in] Which item to reset
	virtual void setResetOptions( bool p_bResetAlways, SvOi::SVResetItemEnum p_eResetItem ) override;
	
	//! Validate the value. If value invalid an exception message will be thrown.
	//! \param rValue [in] The value to validate
	virtual void validateValue( const _variant_t& rValue ) const override;

	//! Gets the type name for the Value object
	//! \returns the type name
	virtual std::string getTypeName() const override { return m_TypeName; };
	
	//! Checks if the value object is an array
	//! \returns true if an array
	virtual bool isArray() const override { return 1 < m_ArraySize; };
	
	//! Gets the size of the value object array
	//! \returns size 0 if not an array
	virtual int getArraySize() const override { return m_ArraySize; };
	
	//! Gets the result size of the value object
	//! \returns size
	virtual int getResultSize() const override { return m_ResultSize; };

	//! Gets the reset item type
	//! \returns the reset item enum type
	virtual SvOi::SVResetItemEnum getResetItem() const override { return m_eResetItem; };

	//! Gets the reset always value object flag
	//! \returns true if reset always
	virtual bool ResetAlways() const override {	return m_ResetAlways; };

	//! Copies the last set value to the destination bucket
	//! \returns the result of copying
	virtual HRESULT CopyValue( int DestBucket ) override;

	//! Returns the value object byte size
	//! \returns the number of bytes for the data
	virtual DWORD GetByteSize() const override;

	//! Returns the variant type of the value object
	//! \returns the VT type
	virtual DWORD GetType() const override { return ValueType2Variant(m_Value).vt; };

	//! Copies the value object to the memory block
	//! \param pMemoryBlock [in] Pointer to the byte address of the memory block
	//! \param MemByteSize [in] The memory block byte size
	//! \param Index [in] The index of the array (-1 if no array)
	//! \returns S_OK if successful
	virtual HRESULT CopyToMemoryBlock(BYTE* pMemoryBlock, DWORD MemByteSize, int Index = -1) const override;

	virtual void Persist(SVObjectWriter& rWriter) override;

	virtual void setSaveValueFlag(bool shouldSaveValue) override { m_shouldSaveValue = shouldSaveValue; } ;
#pragma endregion virtual method
	
	void SetLegacyVectorObjectCompatibility() { m_LegacyVectorObjectCompatibility = true; }

	bool isBucketized() const { return m_isBucketized; };

	/// getter and setter for ShouldSaveValue-Flag
	bool shouldSaveValue() { return m_shouldSaveValue; };

	/// getter and setter for ShouldSaveDefaultValue-Flag
	bool shouldSaveDefaultValue() { return m_shouldSaveDefaultValue; };
	void setSaveDefaultValueFlag(bool isValueSaved) { m_shouldSaveDefaultValue = isValueSaved; };
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	void Initialize();

	HRESULT SetVariantValue(const _variant_t& rValue, int iIndex = -1);
	virtual HRESULT GetVariantValue(_variant_t& rValue, int Index = -1, int Bucket = -1) const;

	virtual double ValueType2Double(const T& rValue) const = 0;
	virtual _variant_t ValueType2Variant(const T& rValue) const = 0;
	virtual T Variant2ValueType(const _variant_t& rValue) const = 0;

	virtual void CreateBuckets();
	void InitializeBuckets();

	//! Convert String to template type 
	//! If value invalid an exception message will be thrown.
	//! \param rValue [in] String to convert
	/// \returns the Value converted or throws an exception!
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

	ValueType* getValuePointer( int Index, int Bucket );
	BucketPtr& getBucket() { return m_pBucket; };
	BucketVectorPtr& getBucketArray() { return m_pBucketArray; };

	std::string FormatOutput(const T& rValue) const;
	void setOutputFormat( LPCTSTR OutFormat ) { m_OutFormat = OutFormat; };
	LPCTSTR getOutputFormat() const { return m_OutFormat.c_str(); };
	bool isLegacyVectorObjectCompatibility() const { return m_LegacyVectorObjectCompatibility; };

	void swap( SVValueObjectClass& rRhs );

	/// Write the Values of this object to the SVObjectWriter
	/// \param rWriter [in,out] The SVObjectWriter
	virtual void WriteValues(SVObjectWriter &rWriter) = 0;

	/// Write the Default-Values of this object to the SVObjectWriter
	/// \param rWriter [in,out] The SVObjectWriter
	virtual void WriteDefaultValues(SVObjectWriter &rWriter) = 0;
#pragma endregion Protected Methods

#pragma region Member Variables
private:
	std::string m_TypeName;					//The data type name
	bool m_isBucketized;					//This is set to make the value object bucketized
	bool m_isStatic;						//If this is set then the value object cannot be bucketized (For values that don't change during run)
	bool m_shouldSaveValue;					//If true, the value will be saved in configuration file, else it will not be saved and after loading the configuration it is default value.
	bool m_shouldSaveDefaultValue;			//If true, the default value will be saved in configuration file, else it will not be saved and after loading the configuration it is default of the default value.
	bool m_ResetAlways;
	bool m_LegacyVectorObjectCompatibility;
	std::string m_OutFormat;					//This is used to format the value object to a string

	SvOi::SVResetItemEnum m_eResetItem;

	int m_NumberOfBuckets;
	int m_ArraySize;
	int m_ResultSize;

	ValueType m_DefaultValue;
	ValueType m_Value;
	ValueVector m_ValueArray;

	BucketPtr m_pBucket;					//Bucket unique pointer
	BucketVectorPtr m_pBucketArray;			//Bucket array unique pointer
#pragma endregion Member Variables
};

#include "SVValueObjectClass.inl"


