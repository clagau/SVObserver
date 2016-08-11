//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStaticValueObjectClassImpl
//* .File Name       : $Workfile:   SVStaticValueObjectClassImpl.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   19 Nov 2013 09:24:20  $
//******************************************************************************


template <typename T>
SVStaticValueObjectClassImpl<T>::~SVStaticValueObjectClassImpl()
{
}


template <typename T>
void SVStaticValueObjectClassImpl<T>::Init()
{
	m_pBuckets = new SVArrayValueHolder<array_type, value_type>;
}

template <typename T>
const SVStaticValueObjectClassImpl<T>& SVStaticValueObjectClassImpl<T>::operator = (const SVStaticValueObjectClassImpl<T>& rhs)
{
	if ( this != &rhs )
	{
		if ( SetObjectDepth( 1 ) )
		{
			if ( S_OK == CreateBuckets() )
			{
				if ( 1 == rhs.ArraySize() )
				{
					m_ScalarBuckets = rhs.m_ScalarBuckets;
				}
				else
				{
					Buckets() = Buckets(rhs);
				}

				DefaultValue() = DefaultValue(rhs);

				m_iLastSetIndex = rhs.m_iLastSetIndex;
			}
		}
	}

	return *this;
}


template <typename T>
HRESULT SVStaticValueObjectClassImpl<T>::SetDefaultValue(const T value, bool bResetAll)
{
	//ASSERT((!pBArray && m_iNumberOfBuckets <= 0) || (pBArray && m_iNumberOfBuckets > 0));
	
	DefaultValue() = value;
	if ( bResetAll )
	{
		if ( ArraySize() == 1 )
		{
			ScalarBucket(0) = value;
		}
		else
		{
			for (size_t j=0; j < Buckets()[0].size(); ++j)
			{
				Buckets()[0][j] = value;
			}
		}
	}
	return S_OK;
}

template <typename T>
HRESULT SVStaticValueObjectClassImpl<T>::GetDefaultValue(T& rValue) const
{
	rValue = DefaultValue();
	return IsCreated() ? S_OK : S_FALSE;
}

template <typename T>
BOOL SVStaticValueObjectClassImpl<T>::SetObjectDepth(int iNewObjectDepth)
{
	BOOL l_Status = true;

	l_Status &= SVValueObjectClass::SetObjectDepth(1);

	return l_Status;
}

template <typename T>
BOOL SVStaticValueObjectClassImpl<T>::SetObjectDepthWithIndex(int iNewObjectDepth, int NewLastSetIndex)
{
	BOOL l_Status = true;

	l_Status &= SVValueObjectClass::SetObjectDepthWithIndex(1, 0);

	return l_Status;
}

template <typename T>
HRESULT SVStaticValueObjectClassImpl<T>::CreateBuckets()
{
	HRESULT l_hrOk = S_OK;

	m_objectDepth = 1;

	if ( m_iNumberOfBuckets != 1 && m_objectDepth > 0)
	{
		if ( ArraySize() == 1 )
		{
			BucketsNoAssert().resize(0);	// avoid ASSERT in Buckets()
			m_ScalarBuckets.resize( 1, DefaultValue() );
		}
		else
		{
			m_ScalarBuckets.resize(0);

			array_type l_DefaultArray(ArraySize(), DefaultValue());
			Buckets().resize( 1, l_DefaultArray );
		}
	}

	m_iNumberOfBuckets = 1;

	l_hrOk = SVValueObjectClass::CreateBuckets();

	return l_hrOk;
}

template <typename T>
HRESULT SVStaticValueObjectClassImpl<T>::SetArraySize(int iSize)
{
#ifdef _DEBUG
	int iOldSize = ArraySize();
#endif
	HRESULT hr = SVValueObjectClass::SetArraySize(iSize);

	if ( ArraySize() == 1 )
	{
		BucketsNoAssert().resize(0);	// avoid ASSERT in Buckets()
		if ( iSize == 0 )
			m_ScalarBuckets.resize(0);
		m_ScalarBuckets.resize( 1, DefaultValue() );
	}
	else
	{
		m_ScalarBuckets.resize(0);
		if ( Buckets().size() == 0 )
			Buckets().resize( 1 );
		array_type& rArray = Buckets()[0];
		if ( iSize == 0 )
			rArray.clear();	// minimum array size is 1. If iSize == 0, clear the array then resize back to 1 below with the default value.
		rArray.resize( ArraySize(), DefaultValue() );
	}

	return hr;
}


template <typename T>
__forceinline HRESULT SVStaticValueObjectClassImpl<T>::CopyValue(int iSourceBucket, int iDestBucket)
{
	HRESULT hr = S_OK;
	// The Static value object will only have one bucket
	// No copy needed...
	return hr;
}

template <typename T>
__forceinline HRESULT SVStaticValueObjectClassImpl<T>::SetValueAt( int iBucket, int iIndex, const T value )
{
	// Static Value Object has only 1 bucket.. Always access bucket 0
	HRESULT hr = m_iNumberOfBuckets == 1 ? S_OK : SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE;

	if ( S_OK == hr || SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == hr )
	{
		m_iLastSetIndex  = 0;

		if ( 1 == ArraySize() )
		{
			m_ScalarBuckets[0] = value;
		}
		else
		{
			Buckets()[0][iIndex] = value;
		}
	}
	
	return hr;
}

template <typename T>
HRESULT SVStaticValueObjectClassImpl<T>::GetValueAt( int iBucket, int iIndex, T& rValue ) const
{
	HRESULT hr = m_iNumberOfBuckets == 1 ? S_OK : SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE;

	// is the index valid?
	if ( S_OK == hr )
	{
		if ( false == m_isObjectValid )	// optimization? or not? Is reading faster than writing to cache/memory?
		{
			m_isObjectValid = true;
		}
		if ( 1 == ArraySize() )
		{
			rValue = m_ScalarBuckets[0];
			return hr;
		}
		else
		{
			rValue = Buckets()[0][iIndex];
		}
	}
	else if ( SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == hr )
	{
		rValue = DefaultValue();
		m_isObjectValid = true;
	}
	else	// BAD INDEX
	{
		rValue = DefaultValue();
		m_isObjectValid = false;
	}
	return hr;
}

template <typename T>
HRESULT SVStaticValueObjectClassImpl<T>::GetObjectValue( const SVString& p_rValueName, VARIANT& p_rVariantValue ) const
{
	HRESULT hr = S_OK;

	if( p_rValueName == _T( "Default" ) )
	{
		_variant_t l_TempVariant;

		l_TempVariant.Attach( p_rVariantValue );

		l_TempVariant = DefaultValue();

		l_TempVariant.Detach();
	}
	else if( p_rValueName == _T( "Array_Elements" ) )
	{
		_variant_t l_TempVariant;

		l_TempVariant.Attach( p_rVariantValue );

		if( ArraySize() == 1 )
		{
			l_TempVariant = ScalarBucket( 0 );
		}
		else if( 1 < ArraySize() )
		{
			SVSAFEARRAY l_SafeArray;

			for( int i = 0; i < m_iArraySize; i++ )
			{
				_variant_t l_Value = Element( 0, i );

				l_SafeArray.Add( l_Value );
			}

			l_TempVariant = l_SafeArray;
		}
		else
		{
			l_TempVariant.Clear();

			hr = E_FAIL;
		}

		l_TempVariant.Detach();
	}
	else
	{
		hr = SVValueObjectClass::GetObjectValue( p_rValueName, p_rVariantValue );
	}

	return hr;
}

template <typename T>
HRESULT SVStaticValueObjectClassImpl<T>::SetObjectValue( const SVString& p_rValueName, const _variant_t& p_rVariantValue )
{
	HRESULT hr = S_OK;

	if( p_rValueName == _T( "Default" ) )
	{
		DefaultValue() = p_rVariantValue;
	}
	// new-style: store all array elements:
	else if( p_rValueName == _T( "Array_Elements" ) )
	{
		if( ( p_rVariantValue.vt & VT_ARRAY ) == VT_ARRAY )
		{
			SVSAFEARRAY l_SafeArray( p_rVariantValue );

			SetArraySize( static_cast< int >( l_SafeArray.size() ) );

			if ( ArraySize() == 1 )
			{
				_variant_t l_Value;

				hr = l_SafeArray.GetElement( 0, l_Value );

				if( S_OK == hr )
				{
					ScalarBucket(0) = l_Value;

					m_iLastSetIndex = 0;
				}
			}
			else
			{
				Array(0).resize( l_SafeArray.size() );

				for( size_t i = 0; i < l_SafeArray.size(); i++ )
				{
					_variant_t l_Value;

					if( S_OK == l_SafeArray.GetElement( i, l_Value ) )
					{
						Array(0)[ i ] = l_Value;
					}
					else
					{
						Array(0)[ i ] = DefaultValue();
					}
				}

				m_iLastSetIndex = 0;
			}
		}
		else if ( ArraySize() == 1 )
		{
			ScalarBucket(0) = p_rVariantValue;

			m_iLastSetIndex = 0;
		}
		else
		{
			hr = E_FAIL;
		}
	}
	else
	{
		hr = SVValueObjectClass::SetObjectValue( p_rValueName, p_rVariantValue );
	}

	return hr;
}

template <typename T>
HRESULT SVStaticValueObjectClassImpl<T>::SetObjectValue(SVObjectAttributeClass* pDataObject)
{
	HRESULT hr = S_FALSE;
	
	SvCl::SVObjectArrayClassTemplate<T> svArray;	// for default values
	bucket_type l_Buckets(BucketsNoAssert());
	array_type l_Array;

	if ( SVObjectAttributeClassHelper::GetAttributeData(pDataObject, "Default", svArray) )
	{
		if ( svArray.GetSize() > 0 )
			DefaultValue() = svArray[ svArray.GetSize()-1 ];
	}
	else if ( SVObjectAttributeClassHelper::GetAttributeData(pDataObject, "Array", l_Buckets, DefaultValue() ) )//  BUCKET_TAG_LOAD; get buckets, not array; for backward compatibility;
	{
		if ( ArraySize() == 1 )
		{
			ScalarBucket(0)=l_Buckets[0][0];
			//ScalarBucket(1)=l_Buckets[1][0];
		}
		else
		{
			std::swap(Buckets(), l_Buckets);
		}
		if ( 0 < m_iNumberOfBuckets )
		{
			m_iLastSetIndex = 0;
		}
	}
	// new-style: store all array elements:
	else if ( SVObjectAttributeClassHelper::GetArrayData(pDataObject, "Array_Elements", l_Array, DefaultValue() ) )
	{
		SetArraySize( static_cast< int >( l_Array.size() ) );
		if ( ArraySize() == 1 )
		{
			ScalarBucket(0)=l_Array[0];
		}
		else
		{
			std::swap(Array(0), l_Array);
		}
		m_iLastSetIndex = 0;
	}
	else if ( SVObjectAttributeClassHelper::GetAttributeData(pDataObject, m_sLegacyScriptDefaultName, svArray) )
	{
		if ( svArray.GetSize() > 0 )
			DefaultValue() = svArray[ svArray.GetSize()-1 ];
	}
	else if ( SVObjectAttributeClassHelper::GetAttributeData(pDataObject, m_sLegacyScriptArrayName, l_Buckets, DefaultValue() ) )
	{
		if ( ArraySize() == 1 )
		{
			ScalarBucket(0)=l_Buckets[0][0];
		}
		else
		{
			std::swap(Buckets(), l_Buckets);
		}
		if ( 0 < m_iNumberOfBuckets )
		{
			m_iLastSetIndex = 0;
		}
	}
	else
	{
		hr = SVValueObjectClass::SetObjectValue(pDataObject);

		return hr;
	}
	
	hr = S_OK;	// always OK if we get here

	return hr;
}

template <typename T>
HRESULT SVStaticValueObjectClassImpl<T>::GetArrayValues(std::vector<T>& raValues) const
{
	return GetArrayValuesImpl(0, raValues);
}

template <typename T>
HRESULT SVStaticValueObjectClassImpl<T>::GetArrayValues(int iBucket, std::vector<T>& raValues) const
{
	return GetArrayValuesImpl(1, raValues);
}

template <typename T>
HRESULT SVStaticValueObjectClassImpl<T>::GetArrayValues(int iBucket, DoubleVectorType& raValues) const
{
	// if the conversion is allowable, make the call, otherwise, don't.
	// the template Select keeps the compiler from compiling both paths which would generate a compile error when the conversion doesn't exist
	return Loki::Select< Loki::Conversion< value_type, DoubleType >::exists,
		conversion_exists, conversion_doesnt_exist>::Result().call(this, 0, raValues);
}

template <typename T>
HRESULT SVStaticValueObjectClassImpl<T>::GetArrayValues(int iBucket, CString& rValues) const
{
	HRESULT hrOk = S_FALSE;
	rValues = _T("");
	if ( 0 < GetResultSize(0) )
	{
		hrOk = S_OK;

		GetValue( 1, 0, rValues );
		int resultSize = GetResultSize(0);
		for( int i = 1; i < resultSize; ++i )
		{
			CString valueString;
			GetValue( 1, i, valueString );
			rValues += _T(" ; ")+valueString;
		}
	}
	return hrOk;
}

template <typename T>
HRESULT SVStaticValueObjectClassImpl<T>::GetArrayValuesAsVariant( int iBucket, VARIANT& rValue ) const
{
	HRESULT hrOk = S_FALSE;

	_variant_t l_Variant;

	l_Variant.Attach( rValue );

	if ( 1 < m_iNumberOfBuckets )
	{
		SVSAFEARRAY l_SafeArray;

		if ( ArraySize() == 1 )
		{
			_variant_t l_Variant;

			GetValue( l_Variant );

			hrOk = l_SafeArray.Add( l_Variant );
		}
		else
		{
			for( int i = 0; i < static_cast< int >( Array( 0 ).size() ); ++i )
			{
				_variant_t l_Variant;

				GetValue( 1, i, l_Variant );

				HRESULT l_TempStatus = l_SafeArray.Add( l_Variant );

				if( S_OK == hrOk )
				{
					hrOk = l_TempStatus;
				}
			}
		}

		if( S_OK == hrOk )
		{
			l_Variant = l_SafeArray;
		}
		else
		{
			l_Variant.Clear();
		}
	}
	else
	{
		l_Variant.Clear();
	}

	rValue = l_Variant.Detach();

	return hrOk;
}

template <typename T>
HRESULT SVStaticValueObjectClassImpl<T>::GetArrayValuesAsVariantVector(int iBucket, std::vector<_variant_t>& raValues) const
{
	HRESULT hrOk = S_FALSE;
	ASSERT( 0 < m_iNumberOfBuckets );
	if ( 0 < m_iNumberOfBuckets )
	{
		int iResultSize = GetResultSize(1);
		ASSERT( iResultSize <= ArraySize() );
		raValues.resize( iResultSize );
		if ( ArraySize() == 1 )
			raValues[0] = ScalarBucket(0);
		else
		{
			array_type::const_iterator l_FromIter( Array(0).begin() );

			std::vector<_variant_t>::iterator l_ToIter( raValues.begin() );

			while( l_ToIter != raValues.end() &&
				l_FromIter != Array(0).end() && 
				l_FromIter != Array(0).begin() + iResultSize )
			{
				*l_ToIter = *l_FromIter;

				++l_FromIter;
				++l_ToIter;
			}
		}
		hrOk = S_OK;
	}
	return hrOk;
}

// provide an efficient swap; derived classes with extra data should redefine.
template <typename T>
inline void SVStaticValueObjectClassImpl<T>::swap( SVStaticValueObjectClassImpl<T>& rhs )
{
	ASSERT( typeid( *this ) == typeid( rhs ) );

	if ( this != &rhs )
	{
		std::swap( m_DefaultValue, rhs.m_DefaultValue );
		m_ScalarBuckets.swap( rhs.m_ScalarBuckets );

		SVValueObjectClass::swap( rhs );
	}
}

