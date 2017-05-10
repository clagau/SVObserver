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
#include "SVMessage/ErrorNumbers.h"
#include "ObjectInterfaces/IValueObject.h"
#include "ObjectInterfaces/TextDefineSvOi.h"
#include "SVMessage/SVMessage.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVOResource/resource.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVUtilityLibrary/SVSharedPtr.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVUtilityLibrary/SVSafeArray.h"
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
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* pCreateStructure ) override;
	virtual BOOL CloseObject() override;
	virtual bool ResetObject( SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
	virtual BOOL SetObjectDepth ( int iNewObjectDepth ) override;
	virtual BOOL SetObjectDepthWithIndex( int NewObjectDepth, int NewLastSetIndex ) override;
	virtual HRESULT SetArraySize(int iSize);
	virtual HRESULT SetOutputFormat(OutputFormat outputFormat) { return E_NOTIMPL; };
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* pDataObject );
	virtual HRESULT SetValue( const T& rValue, int Bucket = -1, int Index = -1 );
	virtual HRESULT GetValue( T&  rValue, int Bucket = -1, int Index = -1 ) const;
	virtual HRESULT SetDefaultValue( const T& rValue, bool bResetAll );
	const ValueType& GetDefaultValue() const { return m_DefaultValue; };

	HRESULT SetArrayValues( typename ValueVector::const_iterator BeginIter, typename ValueVector::const_iterator EndIter, int Bucket = -1 );
	HRESULT SetArrayValues( const ValueVector& rValues, int Bucket = -1 );
	virtual HRESULT GetArrayValues( ValueVector& rValues, int Bucket = -1 ) const;

	HRESULT SetResultSize(int iBucket, int  riResultSize);
	HRESULT SetTypeName( LPCTSTR TypeName );
	bool CompareWithCurrentValue( const SVString& rCompare ) const;
	void setStatic( bool isStatic ) { m_isStatic = isStatic; };
	
#pragma region virtual method
	virtual const UINT& SetObjectAttributesSet( UINT Attributes, SvOi::SetAttributeType Type, int iIndex=0 ) override;

	//! Gets the value for Value object NOTE this comes from IObjectClass
	//! \param rValue [out] The reference to write the value to
	//! \param Bucket [in] The corresponding bucket index to get, if required
	//! \param Index [in] The corresponding array index to write to, if required
	//! \returns S_OK if succeeded
	virtual HRESULT getValue(double& rValue, int Bucket = -1, int Index = -1) const override;

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
	//! \param Bucket [in] The corresponding bucket index to write to, if required
	//! \param Index [in] The corresponding array index to write to, if required
	//! \returns S_OK if succeeded
	virtual HRESULT setValue( const _variant_t& rValue, int Bucket = -1, int Index = -1 ) override;

	//! Gets the value for Value object
	//! \param rValue [out] The reference to write the value to
	//! \param Bucket [in] The corresponding bucket index to get, if required
	//! \param Index [in] The corresponding array index to write to, if required
	//! \returns S_OK if succeeded
	virtual HRESULT getValue( _variant_t& rValue, int Bucket = -1, int Index = -1 ) const override;

	//! Gets the values for Value object
	//! \param rValue [out] The reference to _variant_t vector to store the values
	//! \param Bucket [in] The corresponding bucket index to get, if required
	//! \returns S_OK if succeeded
	virtual HRESULT getValues( std::vector<_variant_t>&  rValues, int Bucket = -1 ) const override;

	//! Sets the value for Value object
	//! \param rValue [in] The value to set the Value object to
	//! \param Bucket [in] The corresponding bucket index to write to, if required
	//! \param Index [in] The corresponding array index to write to, if required
	//! \returns S_OK if succeeded
	virtual HRESULT setValue( const SVString& rValue, int Bucket = -1, int Index = -1 ) override;

	//! Gets the value for Value object
	//! \param rValue [out] The reference to write the value to
	//! \param Bucket [in] The corresponding bucket index to get, if required
	//! \param Index [in] The corresponding array index to write to, if required
	//! \returns S_OK if succeeded
	virtual HRESULT getValue( SVString& rValue, int Bucket = -1, int Index = -1 ) const override;

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
	virtual SVString getTypeName() const override { return m_TypeName; };
	
	//! Checks if the value object is an array
	//! \returns true if an array
	virtual bool isArray() const override { return 1 < m_ArraySize; };
	
	//! Gets the size of the value object array
	//! \returns size 0 if not an array
	virtual int getArraySize() const override { return m_ArraySize; };
	
	//! Gets the result size of the value object
	//! \returns size
	virtual int getResultSize( int Bucket = -1 ) const override;

	//! Gets the reset item type
	//! \returns the reset item enum type
	virtual SvOi::SVResetItemEnum getResetItem() const override { return m_eResetItem; };

	//! Gets the reset always value object flag
	//! \returns true if reset always
	virtual bool ResetAlways() const override {	return m_ResetAlways; };

	//! Copies the last set value to the destination bucket
	//! \returns the result of copying
	virtual HRESULT CopyLastSetValue( int DestBucket ) override { return CopyValue( m_LastSetIndex, DestBucket ); }
#pragma endregion virtual method
	
	const int&  GetLastSetIndex()               const    { return m_LastSetIndex; }
	void SetLegacyVectorObjectCompatibility() { m_LegacyVectorObjectCompatibility = true; }

	bool isBucketized() { return m_isBucketized; };
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	void Initialize();

	HRESULT SetVariantValue( const _variant_t& rValue, int iBucket = -1, int iIndex = -1 );
	virtual HRESULT GetVariantValue( _variant_t& rValue, int Bucket = -1, int Index = -1 ) const;

	virtual HRESULT CopyValue( int SourceBucket, int DestBucket );

	virtual double ValueType2Double(const T& rValue) const = 0;
	virtual _variant_t ValueType2Variant( const T& rValue ) const = 0;
	virtual T Variant2ValueType( const _variant_t& rValue ) const = 0;

	virtual void CreateBuckets();
	void InitializeBuckets();

	/// Validate the value. If value invalid an exception message will be thrown.
	/// \param iBucket [in] 
	/// \param iIndex [in]
	/// \param rValue [in]
	virtual void ValidateValue( int Bucket, int Index, const SVString& rValue ) const;

	//! Convert String to template type 
	//! If value invalid an exception message will be thrown.
	//! \param rValue [in] String to convert
	/// \returns the Value converted or throws an exception!
	virtual T ConvertString2Type( const SVString& rValue ) const = 0;

	//! Convert template type to String
	//! \param rValue [in] Type to convert
	/// \returns the SVString
	virtual SVString ConvertType2String( const T& rValue ) const = 0;

	HRESULT ValidateIndexes( int Bucket, int ArrayIndex ) const;

	ValueType& DefaultValue() { return m_DefaultValue; };
	ValueType& Value() { return m_Value; };
	ValueVector& ValueArray() { return m_ValueArray; };

	ValueType* getValuePointer( int Bucket, int Index );
	BucketPtr& getBucket() { return m_pBucket; };
	BucketVectorPtr& getBucketArray() { return m_pBucketArray; };

	SVString FormatOutput(const T& rValue) const;
	void setOutputFormat( LPCTSTR OutFormat ) { m_OutFormat = OutFormat; };
	LPCTSTR getOutputFormat() const { return m_OutFormat.c_str(); };
	const int& getNumberOfBuckets() const { return m_NumberOfBuckets; };
	bool isLegacyVectorObjectCompatibility() const { return m_LegacyVectorObjectCompatibility; };
	void setLastSetIndex( const int& rLastSetIndex ) { m_LastSetIndex = rLastSetIndex; };

	void swap( SVValueObjectClass& rRhs );

#pragma endregion Protected Methods

#pragma region Member Variables
protected:
	//// one set per template type
	static SVString m_sLegacyScriptDefaultName;
	static SVString m_sLegacyScriptArrayName;
private:
	SVString m_TypeName;					//The data type name
	bool m_isBucketized;					//This is set to make the value object bucketized
	bool m_isStatic;						//If this is set then the value object cannot be bucketized (For values that don't change during run)
	bool m_ResetAlways;
	bool m_LegacyVectorObjectCompatibility;
	SVString m_OutFormat;					//This is used to format the value object to a string

	SvOi::SVResetItemEnum m_eResetItem;

	int m_LastSetIndex;
	int m_NumberOfBuckets;
	int m_ArraySize;
	std::vector<int> m_ResultSize;			//The result size for each bucket

	ValueType m_DefaultValue;
	ValueType m_Value;
	ValueVector m_ValueArray;

	BucketPtr m_pBucket;					//Bucket shared pointer
	BucketVectorPtr m_pBucketArray;			//Bucket array pointer
#pragma endregion Member Variables
};

template <typename T>
SVString SVValueObjectClass<T>::m_sLegacyScriptDefaultName;
template <typename T>
SVString SVValueObjectClass<T>::m_sLegacyScriptArrayName;

#include "SVValueObjectClass.inl"


