//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVValueObject
//* .File Name       : $Workfile:   SVValueObjectClass.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 16:17:06  $
//******************************************************************************

#pragma region Constructor
template <typename T>
SVValueObjectClass<T>::SVValueObjectClass( LPCSTR ObjectName )
: SVObjectClass(ObjectName)
, m_pBucket( nullptr )
, m_pBucketArray( nullptr )
, m_shouldSaveValue(true)
, m_shouldSaveDefaultValue(false)
{
	Initialize();
}

template <typename T>
SVValueObjectClass<T>::SVValueObjectClass(SVObjectClass* POwner, int StringResourceID)
: SVObjectClass(POwner, StringResourceID) 
, m_pBucket( nullptr )
, m_pBucketArray( nullptr )
, m_shouldSaveValue(true)
, m_shouldSaveDefaultValue(false)
{
	Initialize();
}

template <typename T>
SVValueObjectClass<T>::~SVValueObjectClass()
{
	SVObjectManagerClass::Instance().UnregisterSubObject(GetUniqueObjectID());
	SetObjectDepth(-1);
}

template <typename T>
const SVValueObjectClass<T>& SVValueObjectClass<T>::operator= (const SVValueObjectClass<T>& rRhs)
{
	if ( this != &rRhs )
	{
		SetObjectDepth( rRhs.m_NumberOfBuckets );
		SetArraySize( rRhs.getArraySize() );
		CreateBuckets();

		if( !m_isBucketized )
		{
			if ( 1 == m_ArraySize )
			{
				m_Value = rRhs.m_Value;
			}
			else
			{
				m_ValueArray = rRhs.m_ValueArray;
			}
		}
		else
		{
			if ( 1 == m_ArraySize )
			{
				if(nullptr != m_pBucket.get() && nullptr != rRhs.m_pBucket.get())
				{
					*m_pBucket = *rRhs.m_pBucket;
				}
			}
			else
			{
				if(nullptr != m_pBucketArray.get() && nullptr != rRhs.m_pBucketArray.get())
				{
					*m_pBucketArray = *rRhs.m_pBucketArray;
				}
			}
		}
		m_DefaultValue = rRhs.GetDefaultValue();
	}

	return *this;
}
#pragma endregion Constructor

#pragma region Public Methods
template <typename T>
bool SVValueObjectClass<T>::CreateObject( SVObjectLevelCreateStruct* pCreateStructure )
{
	bool l_bOk = SVObjectClass::CreateObject( pCreateStructure );

	SVObjectManagerClass::Instance().RegisterSubObject(GetUniqueObjectID());
	SetObjectAttributesAllowed( SvOi::SV_DD_VALUE, SvOi::SetAttributeType::AddAttribute );	// derived classes need to reset this

	if ( m_NumberOfBuckets < 2 )
	{
		SetObjectDepth( 2 );
	}

	//A ValueObject with SvOi::SV_PUBLISHABLE set needs to be bucketized
	const UINT cAttributes = SvOi::SV_PUBLISHABLE;
	if( 0 != ( ObjectAttributesSet() & cAttributes )  )
	{
		setBucketized(true);
	}

	CreateBuckets();

	return l_bOk;
}

template <typename T>
bool SVValueObjectClass<T>::CloseObject()
{
	SVObjectManagerClass::Instance().UnregisterSubObject(GetUniqueObjectID());
	return SVObjectClass::CloseObject();
}

template <typename T>
bool SVValueObjectClass<T>::ResetObject( SvStl::MessageContainerVector *pErrorMessages=nullptr )
{
	CreateBuckets();
	return __super::ResetObject( pErrorMessages );
}

template <typename T>
void SVValueObjectClass<T>::SetObjectDepthWithIndex(int iNewObjectDepth, int NewLastSetIndex)
{
	if( 0 < iNewObjectDepth )
	{
		CopyValue(NewLastSetIndex);
	}

	SVObjectClass::SetObjectDepthWithIndex(iNewObjectDepth, NewLastSetIndex);
}

template <typename T>
HRESULT SVValueObjectClass<T>::SetArraySize(int iSize)
{
	HRESULT hr = S_OK;

	m_ArraySize = (std::max)(iSize, 1);	// minimum one array element //parentheses around std::max avoid problems if a max macro has beed defined somewhere (e.g. in windows.h)

	if( m_ArraySize <= 1 )
	{
		if( SVString::npos != m_TypeName.find( _T(" Array") ) )
		{
			m_TypeName = SvUl_SF::Left( m_TypeName, m_TypeName.size() - 6 );
		}
	}
	else
	{
		if( SVString::npos == m_TypeName.find( _T(" Array") ) )
		{
			m_TypeName += _T(" Array") ;
		}
	}

	m_ResultSize = iSize;

	m_ObjectAttributesSet.resize( m_ArraySize, m_DefaultObjectAttributesSet );

	if( !m_isBucketized )
	{
		if ( 1 == m_ArraySize )
		{
			if (iSize == 0)
			{
				m_Value = GetDefaultValue();
			}
			m_ValueArray.clear();
		}
		else
		{
			if ( iSize == 0 )
			{
				m_ValueArray.clear();	// minimum array size is 1. If iSize == 0, clear the array then resize back to 1 below with the default value.
			}
			m_ValueArray.resize( m_ArraySize, GetDefaultValue() );
		}
	}
	else
	{
		if ( 1 == m_ArraySize )
		{
			if(nullptr != m_pBucket.get())
			{
				std::fill( m_pBucket->begin(), m_pBucket->end(), GetDefaultValue() );
			}
		}
		else
		{
			//When bucketized the standard array needs to increase in size too
			m_ValueArray.resize(m_ArraySize, GetDefaultValue());
			if(nullptr != m_pBucketArray.get())
			{
				for ( int i=0; i < m_NumberOfBuckets; i++ )
				{
					ValueVector& rBucket = m_pBucketArray->at( i );
					rBucket.resize( m_ArraySize, GetDefaultValue() );
				}
			}
		}

	}

	return hr;
}

template <typename T>
HRESULT SVValueObjectClass<T>::SetObjectValue(SVObjectAttributeClass* pDataObject)
{
	HRESULT Result( E_FAIL );

	SvCl::SVObjectArrayClassTemplate<T> ObjectArray;	// for default values
	SvCl::SVObjectLongArrayClass LongArray;
	BucketVector BucketArray;
	ValueVector ValueArray;

	if (pDataObject->GetAttributeData(scDefaultTag, ObjectArray))
	{
		if (0 < ObjectArray.GetSize())
		{
			DefaultValue() = ObjectArray[ObjectArray.GetSize() - 1];
		}
	}
	//  BUCKET_TAG_LOAD; get buckets, not array; for backward compatibility;
	else if (pDataObject->GetAttributeData(SvOi::cBucketTag, BucketArray, DefaultValue()))
	{

		if( !m_isBucketized )
		{
			if ( 1 == m_ArraySize )
			{
				// In configurations the value are placed in bucket 1
				 m_Value = BucketArray[1][0];
			}
			else
			{
				// In configurations the values are placed in bucket 1
				std::swap( m_ValueArray, BucketArray[1] );
			}
		}
		else
		{
			if ( 1 == m_ArraySize )
			{
				if(nullptr != m_pBucket.get())
				{
					m_pBucket->at(0) = BucketArray[1][0];
					m_pBucket->at(1) = BucketArray[1][0];
				}
			}
			else
			{
				if(nullptr != m_pBucketArray.get())
				{
					std::swap( *m_pBucketArray, BucketArray );
				}
			}
		}
		}
	// new-style: store all array elements:
	else if ( pDataObject->GetArrayData( SvOi::cArrayTag, ValueArray, DefaultValue() ) )
	{
		SetArraySize( static_cast< int >( ValueArray.size() ) );
		if( !m_isBucketized )
		{
			if ( 1 == m_ArraySize )
			{
				m_Value = ValueArray[0];
			}
			else
			{
				std::swap( m_ValueArray, ValueArray );
			}
		}
	}
	else if (pDataObject->GetAttributeData(scArraySizeTag, LongArray))
	{
		if (0 < LongArray.size())
		{
			SetArraySize(LongArray[0]);
		}
	}
	else
	{
		Result = SVObjectClass::SetObjectValue(pDataObject);
		return Result;
	}

	Result = S_OK;	// always OK if we get here

	return Result;
}

template <typename T>
__forceinline HRESULT SVValueObjectClass<T>::SetValue( const T& rValue, int Index )
{
	HRESULT Result = ValidateIndex( Index );
	
	if ( S_OK == Result || SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == Result )
	{
			if ( 1 == m_ArraySize )
			{
				m_Value = rValue;
			}
			else
			{
				m_ValueArray[Index] = rValue;
			}
		}

#if defined (TRACE_THEM_ALL) || defined (TRACE_VALUE_OBJECT)
	SVString DebugString = SvUl_SF::Format(_T("SetValue, %s, %s, %d, %d\r\n"), GetName(), ConvertType2String(rValue).c_str(), Index, Bucket);
	::OutputDebugString(DebugString.c_str());
#endif
	return Result;
}

template <typename T>
__forceinline HRESULT SVValueObjectClass<T>::GetValue(T& rValue, int Index, int Bucket) const
{
	//! When Value Object is an array and index is -1 then use first index
	if (isArray() && 0 > Index)
	{
		Index = 0;
	}
	HRESULT Result = ValidateIndex( Index );

	// is the index valid?
	if ( S_OK == Result )
	{
		m_isObjectValid = true;
		if(!m_isBucketized || -1 == Bucket)
		{
			if ( 1 == m_ArraySize )
			{
				rValue = m_Value;
			}
			else
			{
				rValue = m_ValueArray[Index];
			}
		}
		else
		{
			if ( 1 == m_ArraySize )
			{
				assert(nullptr != m_pBucket.get());
				if(nullptr != m_pBucket.get())
				{
					rValue = m_pBucket->at(Bucket);
				}
				else
				{
					Result = E_POINTER;
				}
			}
			else
			{
				assert(nullptr != m_pBucketArray.get());
				if(nullptr != m_pBucketArray.get())
				{
					rValue = m_pBucketArray->at(Bucket)[Index];
				}
				else
				{
					Result = E_POINTER;
				}
			}
		}
	}
	else if ( SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == Result )
	{
		// inside the allocated array size but bucket does
		// not contain a value for the current run so use
		// the default value.
		rValue = m_DefaultValue;
		m_isObjectValid = true;
	}
	else
	{
		// the selected index is not in the allocated array 
		rValue = m_DefaultValue;
		m_isObjectValid = false;
	}
#if defined (TRACE_THEM_ALL) || defined (TRACE_VALUE_OBJECT)
	SVString DebugString = SvUl_SF::Format(_T("GetValue, %s, %s, %d, %d\r\n"), GetName(), ConvertType2String(rValue).c_str(), Index, Bucket);
	::OutputDebugString(DebugString.c_str());
#endif
	return Result;
}

template <typename T>
HRESULT SVValueObjectClass<T>::SetDefaultValue(const T& rValue, bool bResetAll)
{
	HRESULT Result( S_OK );

	m_DefaultValue = rValue;
	if ( bResetAll )
	{
		if( !m_isBucketized )
		{
			if ( 1 == m_ArraySize )
			{
				m_Value = rValue;
			}
			else
			{
				std::fill( m_ValueArray.begin(), m_ValueArray.end(), rValue );
			}
		}
		else
		{
			if ( 1 == m_ArraySize )
			{
				m_Value = rValue;
				if(nullptr != m_pBucket.get())
				{
					std::fill( m_pBucket->begin(), m_pBucket->end(), rValue );
				}
				else
				{
					Result = E_POINTER;
				}
			}
			else
			{
				std::fill(m_ValueArray.begin(), m_ValueArray.end(), rValue);
				if(nullptr != m_pBucketArray.get() && static_cast<int> (m_pBucketArray->size()) == m_NumberOfBuckets )
				{
					std::fill( m_pBucketArray->at(0).begin(), m_pBucketArray->at(m_NumberOfBuckets-1).end(), rValue );
				}
				else
				{
					Result = E_POINTER;
				}
			}
		}
	}
	return Result;
}

template <typename T>
HRESULT SVValueObjectClass<T>::SetArrayValues( typename ValueVector::const_iterator BeginIter, typename ValueVector::const_iterator EndIter)
{
	HRESULT Result(E_FAIL);

		int Size = static_cast<int> ( std::distance(BeginIter, EndIter));
		assert( Size <= m_ArraySize );
		if ( Size <= m_ArraySize )
		{
		SetResultSize(Size);
			if ( Size > 0 )
			{
					if ( 1 == m_ArraySize )
					{
						m_Value = *BeginIter;
					}
					else
					{
						std::copy( BeginIter, EndIter, m_ValueArray.begin() );
					}
				}
			Result = S_OK;
		}
		else
		{
			Result = SVMSG_SVO_33_OBJECT_INDEX_INVALID;
		}
	return Result;
}

template <typename T>
HRESULT SVValueObjectClass<T>::SetArrayValues( const ValueVector& rValues)
{
	return SetArrayValues( rValues.begin(), rValues.end() );
}

template <typename T>
HRESULT SVValueObjectClass<T>::GetArrayValues( ValueVector& rValues, int Bucket /*= -1*/ ) const
{
	HRESULT Result = S_OK;
	if (0 == m_ResultSize)
	{
		return Result;
	}
	assert( m_ResultSize <= m_ArraySize );
	rValues.resize( m_ResultSize );
	if( !m_isBucketized || -1 == Bucket )
	{
		if ( 1 == m_ArraySize )
		{
			rValues[0] = m_Value;
		}
		else
		{
			std::copy(m_ValueArray.begin(), m_ValueArray.begin() + m_ResultSize, rValues.begin() );
		}
	}
	else
	{
		if ( 1 == m_ArraySize )
		{
			if(nullptr != m_pBucket.get())
			{
				rValues[0] = m_pBucket->at( Bucket );
			}
			else
			{
				Result = E_POINTER;
			}
		}
		else
		{
			if(nullptr != m_pBucketArray.get())
			{
				std::copy(m_pBucketArray->at( Bucket ).begin(), m_pBucketArray->at( Bucket ).begin() + m_ResultSize, rValues.begin() );
			}
			else
			{
				Result = E_POINTER;
			}
		}
	}
	return Result;
}

template <typename T>
HRESULT SVValueObjectClass<T>::SetTypeName( LPCTSTR TypeName )
{
	m_TypeName = TypeName;	
	return S_OK;
}

template <typename T>
bool SVValueObjectClass<T>::CompareWithCurrentValue( const SVString& rCompare ) const
{
	bool Result( false );

	SVString CurrentValue;
	if( S_OK == getValue( CurrentValue ) )
	{
		Result = (CurrentValue == rCompare);
	}

	return Result;
}

#pragma region  virtual method
template <typename T>
UINT SVValueObjectClass<T>::SetObjectAttributesSet( UINT Attributes, SvOi::SetAttributeType Type, int Index )
{
	UINT newAttribute = __super::SetObjectAttributesSet( Attributes, Type, Index );

	const UINT cBucketizedAttributes = SvOi::SV_PUBLISHABLE;
	if (0 != (cBucketizedAttributes & Attributes))
	{
		if (0 != (cBucketizedAttributes & newAttribute))
		{
			setBucketized(true);
		}
		else if (SvOi::SetAttributeType::RemoveAttribute == Type)
		{
			//! Set this only if attributes are being removed as other objects may be set to bucketized for other reasons
			setBucketized(false);
		}
		//! We need to reset the object to create or delete the buckets
		ResetObject();
	}
	return newAttribute;
}

template <typename T>
HRESULT SVValueObjectClass<T>::getValue(double& rValue, int Index /*= -1*/, int Bucket /*= -1*/) const
{
	HRESULT Result(E_FAIL);

	ValueType Value;
	Result = GetValue(Value, Index, Bucket);
	rValue = ValueType2Double(Value);

	return Result;
}

template <typename T>
HRESULT SVValueObjectClass<T>::getValues(std::vector<double>&  rValues, int Bucket = -1) const
{
	HRESULT Result(S_OK);
	
	int ResultSize = getResultSize();
	assert(ResultSize <= m_ArraySize);
	rValues.resize(ResultSize);
	for (int i = 0; i < ResultSize && S_OK == Result; i++)
	{
		ValueType Value;
		Result = GetValue(Value, i, Bucket);
		rValues[i] = ValueType2Double(Value);
	}

	return Result;
}

template <typename T>
HRESULT SVValueObjectClass<T>::setValue( const _variant_t& rValue, int Index /*= -1*/  )
{
	HRESULT hr = S_OK;
	if (!isArray() || 0 == (VT_ARRAY & rValue.vt) || nullptr == rValue.parray)
	{
		hr = SetVariantValue( rValue, Index ); 
	}
	else
	{
		SVSAFEARRAY safeArray( rValue );

		if (safeArray.size() > 0)
		{
			//fit array size to safeArray-size
			if( m_ArraySize != safeArray.size() )
			{
				hr = SetArraySize(static_cast<int>(safeArray.size()));
			}

			//set all value to array
			for (int i=0; i < static_cast<int> (safeArray.size()); i++)
			{
				_variant_t tmpVar;
				HRESULT tempHr = safeArray.GetElement( i, tmpVar );
				if (S_OK == hr)
				{
					hr = tempHr;
				}

				tempHr = SetVariantValue( tmpVar, i ); 
				if (S_OK == hr)
				{
					hr = tempHr;
				}
			}
		}
		else
		{
			hr = SvStl::Err_10029_ValueObject_Parameter_WrongSize;
		}
	}

	return hr;
}

template <typename T>
HRESULT SVValueObjectClass<T>::getValue(_variant_t& rValue, int Index /*= -1*/, int Bucket /*= -1*/) const
{ 
	return GetVariantValue(rValue, Index, Bucket);
}

template <typename T>
HRESULT SVValueObjectClass<T>::getValues( std::vector<_variant_t>&  rValues, int Bucket = -1 ) const
{
	HRESULT Result = E_FAIL;

	if (0 == m_ResultSize)
	{
		return S_OK;
	}

	assert( m_ResultSize <= m_ArraySize );
	rValues.resize( m_ResultSize );
	if( 1 == m_ArraySize )
	{
		ValueType Value;
		Result = GetValue(Value, -1, Bucket);
		if (S_OK == Result)
		{
			rValues[0] = ValueType2Variant(Value);
		}
	}
	else
	{
		ValueVector ValueArray;
		Result = GetArrayValues(ValueArray, Bucket);
		if (S_OK == Result)
		{
			ValueVector::const_iterator FromIter(ValueArray.begin());
			std::vector<_variant_t>::iterator ToIter(rValues.begin());

			while (ToIter != rValues.end() && FromIter != ValueArray.end() &&
				FromIter != ValueArray.begin() + m_ResultSize)
			{
				*ToIter = ValueType2Variant(*FromIter);

				++FromIter;
				++ToIter;
			}
		}
	}
	return Result;
}

template <typename T>
HRESULT SVValueObjectClass<T>::setValue( const SVString& rValue, int Index /*= -1*/ )
{
	HRESULT Result( E_FAIL );

	try
	{
		T Value = ConvertString2Type( rValue );
		Result = SetValue( Value, Index );
	}
	catch ( const SvStl::MessageContainer& )
	{
		Result = E_FAIL;
	}

	return Result;
}

template <typename T>
HRESULT SVValueObjectClass<T>::getValue(SVString& rValue, int Index /*= -1*/, int Bucket /*= -1*/) const
{
	HRESULT Result( E_FAIL );

	//Whole array
	if( isArray() && -1 == Index )
	{
		ValueVector Values;
		Result = GetArrayValues( Values, Bucket );
		if( S_OK == Result )
		{
			ValueVector::const_iterator Iter( Values.begin() );
			for( ; Values.end() != Iter; ++Iter )
			{
				if( !rValue.empty() )
				{
					rValue += _T("; ");
				}
				rValue += ConvertType2String( *Iter );
			}
		}
	}
	else
	{
		ValueType Value;
		Result = GetValue(Value, Index, Bucket);
		if( S_OK == Result || SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == Result)
		{
			rValue = ConvertType2String( Value );
		}
	}
	return Result;
}

template <typename T>
void SVValueObjectClass<T>::setResetOptions( bool bResetAlways, SvOi::SVResetItemEnum eResetItem )
{
	m_ResetAlways = bResetAlways;
	m_eResetItem = eResetItem;
}

template <typename T>
void SVValueObjectClass<T>::validateValue( const _variant_t& rValue ) const
{
	if (!isArray() || 0 == (VT_ARRAY & rValue.vt) || nullptr == rValue.parray)
	{
		ConvertString2Type( SvUl_SF::createSVString(rValue) ); 
	}
	else
	{
		SVSAFEARRAY safeArray( rValue );

		if (safeArray.size() > 0)
		{
			//set all value to array
			for (int i=0; i<safeArray.size(); i++)
			{
				_variant_t tmpVar;
				HRESULT tempHr = safeArray.GetElement( i, tmpVar );
				if (S_OK != tempHr)
				{
					SVStringVector msgList;
					msgList.push_back(SvUl_SF::Format(_T("%d"), tempHr));
					SvStl::MessageMgrStd Exception( SvStl::LogOnly );
					Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING , SvStl::Tid_ValidateValue_InvalidElementInVariantArray, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
					Exception.Throw();
				}

				ConvertString2Type( SvUl_SF::createSVString(tmpVar) ); 
			}
		}
		else
		{
			SvStl::MessageMgrStd Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ValidateValue_ArraySizeInvalid, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10029_ValueObject_Parameter_WrongSize, GetUniqueObjectID() );
			Exception.Throw();
		}
	}
}

template <typename T>
void SVValueObjectClass<T>::Persist(SVObjectWriter& rWriter)
{
	rWriter.StartElement(GetObjectName()); // use internal name for node name

	// Get the Heading (Class Info)
	__super::Persist(rWriter);

	if (shouldSaveDefaultValue())
	{
		WriteDefaultValues(rWriter);
	}

	// Get the Data Values (Member Info, Values)
	if (shouldSaveValue())
	{
		rWriter.StartElement(scArrayElementsTag);
		WriteValues(rWriter);
		rWriter.EndElement();
	}
	else
	{
		int size = getArraySize();
		if (1 < size)
		{
			_variant_t varSize = size;
			varSize.ChangeType(VT_I4);
			rWriter.WriteAttribute(scArraySizeTag, varSize);
		}
	}

	rWriter.EndElement();
}
#pragma endregion virtual method
#pragma endregion Public Methods

#pragma region Protected Methods
template <typename T>
void SVValueObjectClass<T>::Initialize()
{
	m_NumberOfBuckets = 0;
	m_ArraySize = 1;
	m_LegacyVectorObjectCompatibility = false;
	m_ResetAlways = false;
	m_eResetItem = SvOi::SVResetItemIP;
	m_isBucketized = false;
	m_isStatic = false;
	m_ResultSize = 1;

	SetObjectAttributesAllowed( SvOi::SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES, SvOi::SetAttributeType::OverwriteAttribute );
}

template <typename T>
HRESULT SVValueObjectClass<T>::SetVariantValue( const _variant_t& rValue, int Index )
{
	if (VT_BSTR == rValue.vt)
	{
		SVString Value = SvUl_SF::createSVString(rValue);
		return setValue(Value, Index);
	}
	//!For type safety check that the VT type is either the default value main value or when not set yet (VT_EMPTY)
	else if (ValueType2Variant(m_DefaultValue).vt == rValue.vt || ValueType2Variant(m_Value).vt == rValue.vt || ValueType2Variant(m_Value).vt == VT_EMPTY)
	{
		return SetValue( Variant2ValueType(rValue), Index );
	}

	assert(false);
	SvStl::MessageMgrStd Exception(SvStl::LogOnly);
	Exception.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_WrongType, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());

	return E_FAIL;
}

template <typename T>
HRESULT SVValueObjectClass<T>::GetVariantValue(_variant_t& rValue, int Index, int Bucket) const
{
	HRESULT Result(E_FAIL);

	//! If index is -1 and it is an array then get the whole array
	if( -1 == Index && isArray() )
	{
		std::vector<_variant_t> AllValues;
		Result = getValues( AllValues, Bucket );
		if( S_OK == Result )
		{
			SVSAFEARRAY SafeArray;

			SafeArray.assign( AllValues.begin(), AllValues.end() );

			rValue = SafeArray;
		}
	}
	else
	{
		ValueType Value;
		Result = GetValue(Value, Index, Bucket);
		if( S_OK == Result )
		{
			rValue = ValueType2Variant( Value );
		}
		else
		{
			rValue.Clear();
		}
	}

	return Result;
}

template <typename T>
__forceinline HRESULT SVValueObjectClass<T>::CopyValue( int DestBucket )
{
	HRESULT Result( E_FAIL );

	if (m_isBucketized)
	{
		if (DestBucket >= 0 && DestBucket < m_NumberOfBuckets)
		{
			if ( 1 == m_ArraySize )
			{
				assert(nullptr != m_pBucket.get());
				if(nullptr != m_pBucket.get())
				{
					m_pBucket->at(DestBucket) = m_Value;
				}
				else
				{
					Result = E_POINTER;
				}
			}
			else
			{
				assert(nullptr != m_pBucketArray.get());
				if(nullptr != m_pBucketArray.get())
				{
					m_pBucketArray->at(DestBucket) = m_ValueArray;
				}
				else
				{
					Result = E_POINTER;
				}
			}

			Result = S_OK;
		}
	}
	else
	{
		//If not bucketized does not need to be copied
		Result = S_OK;
	}

	return Result;
}

template <typename T>
DWORD SVValueObjectClass<T>::GetByteSize() const
{
	DWORD Result(0);

	if (VT_BSTR == ValueType2Variant(m_Value).vt)
	{
		Result = cMaxStringSize;
	}
	else
	{
		Result = sizeof(m_Value);
	}
	return Result;
}

template <typename T>
HRESULT SVValueObjectClass<T>::CopyToMemoryBlock(BYTE* pMemoryBlock, DWORD MemByteSize, int Index /* = -1*/) const
{
	HRESULT Result = ValidateMemoryBlockParameters(pMemoryBlock, MemByteSize, Index);
	
	if (S_OK == Result)
	{
		if (isArray())
		{
			memcpy(pMemoryBlock, &m_ValueArray[Index], GetByteSize());
		}
		else
		{
			memcpy(pMemoryBlock, &m_Value, GetByteSize());
		}
	}
	else if (GetByteSize() == MemByteSize)
	{
		memcpy(pMemoryBlock, &m_DefaultValue, GetByteSize());
	}
	
	return Result;
}


template <typename T>
void SVValueObjectClass<T>::CreateBuckets( )
{
	m_objectDepth = ( m_objectDepth < 2 ) ? 2 : m_objectDepth;
	m_NumberOfBuckets = m_objectDepth;

	if( isBucketized() )
	{
		if( !isArray() )
		{
			if(nullptr == m_pBucket.get())
			{
				SVObjectManagerClass::Instance().createBucket( m_pBucket, m_DefaultValue, m_NumberOfBuckets );
				m_pBucket->at(1) = m_Value;
			}
			else if( static_cast<int> (m_pBucket->size()) != m_NumberOfBuckets )
			{
				m_pBucket->resize( m_NumberOfBuckets, m_DefaultValue );
			}

		}
		else
		{
			ValueVector DefaultArray;
			DefaultArray.resize( m_ArraySize, m_DefaultValue ); 
			if(nullptr == m_pBucketArray.get())
			{
				SVObjectManagerClass::Instance().createBucket( m_pBucketArray, DefaultArray, m_NumberOfBuckets );
				//Now set the array size for each bucket
				BucketVector::iterator Iter = m_pBucketArray->begin();
				for( ; m_pBucketArray->end() != Iter; ++Iter )
				{
					Iter->resize( m_ArraySize );
				}
				m_pBucketArray->at(1) = m_ValueArray;
			}
			else if( static_cast<int> (m_pBucketArray->size()) != m_NumberOfBuckets )
			{
				m_pBucketArray->resize( m_NumberOfBuckets, DefaultArray );
			}
		}
	}
	else
	{
		m_pBucket.reset();
		m_pBucketArray.reset();
	}
}

template <typename T>
void SVValueObjectClass<T>::InitializeBuckets()
{
	m_objectDepth = 2;
	m_ArraySize = 1;

	m_Value = m_DefaultValue;

	CreateBuckets();
	}

template <typename T>
inline HRESULT SVValueObjectClass<T>::ValidateIndex( int ArrayIndex ) const
{
	if( isArray() )
	{
		if( ArrayIndex >= 0 && ArrayIndex < m_ArraySize )
		{
			if ( 1 == m_ArraySize || ArrayIndex < m_ResultSize )
			{
				return S_OK;
			}
			else
			{
				return SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE;
			}
		}
		else
		{
			return SVMSG_SVO_33_OBJECT_INDEX_INVALID;
		}
	}

	return S_OK;
}

template <typename T>
inline HRESULT SVValueObjectClass<T>::ValidateMemoryBlockParameters(BYTE* pMemoryBlock, DWORD MemByteSize, int Index) const
{
	HRESULT Result = ValidateIndex(Index);
	
	if (S_OK == Result)
	{
		if (nullptr != pMemoryBlock)
		{
			if (MemByteSize != GetByteSize())
			{
				Result = E_FAIL;
			}
		}
		else
		{
			Result = E_POINTER;
		}
	}

	return Result;
}
template <typename T>
typename T* SVValueObjectClass<T>::getValuePointer( int Index, int Bucket)
{
	ValueType* pResult( nullptr );

	if( !m_isBucketized || -1 == Bucket )
	{
		if ( 1 == m_ArraySize )
		{
			pResult = &m_Value;
		}
		else
		{
			pResult = &m_ValueArray[Index];
		}
	}
	else
	{
		if ( 1 == m_ArraySize )
		{
			if(nullptr != m_pBucket.get())
			{
				pResult = &m_pBucket->at(Bucket);
			}
		}
		else
		{
			if(nullptr != m_pBucketArray.get())
			{
				pResult = &m_pBucketArray->at(Bucket)[Index];
			}
		}
	}

	return pResult;
}

template <typename T>
SVString SVValueObjectClass<T>::FormatOutput(const T& rValue) const
{
	SVString Result;
	if(!m_OutFormat.empty())
	{
		//This is faster than SvUl_SF::Format
		TCHAR Text[100];
		sprintf_s(Text, 100, m_OutFormat.c_str(), rValue);
		Result = Text;
	}
	return Result;
}

template <typename T>
inline void SVValueObjectClass<T>::swap( SVValueObjectClass& rRhs )
{
	assert( typeid( *this ) == typeid( rRhs ) );

	if ( this != &rRhs )
	{
		std::swap( m_DefaultValue, rRhs.m_DefaultValue );
		std::swap( m_Value, rRhs.m_Value );
		std::swap( m_ValueArray, rRhs.m_ArrayValues );
		std::swap( m_NumberOfBuckets, rRhs.m_NumberOfBuckets );
		std::swap( m_ArraySize, rRhs.m_ArraySize );
		std::swap( m_ResultSize, rRhs.m_ResultSize );
		std::swap( m_bLegacyVectorObjectCompatibility, rRhs.m_bLegacyVectorObjectCompatibility );
		std::swap( m_bResetAlways, m_bResetAlways );
		std::swap( m_eResetItem, m_eResetItem );
		if( !m_pBucket.empty() && !rRhs.m_pBucket.empty() )
		{
			std::swap( *m_pBucket, *rRhs.m_pBucket );
		}
		if( !m_pBucketArray.empty() && !rRhs.m_pBucketArray.empty() )
		{
			std::swap( *m_pBucketArray, *rRhs.m_pBucketArray );
		}
	}
}
#pragma endregion Protected Methods

