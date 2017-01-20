//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVValueObjectClassImpl
//* .File Name       : $Workfile:   SVValueObjectClassImpl.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Oct 2013 08:24:40  $
//******************************************************************************

#pragma region Includes
//Moved to precompiled header: #include <assert.h>
//Moved to precompiled header: #include <string>
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

template <typename T>
SVValueObjectClassImpl<T>::~SVValueObjectClassImpl()
{
}

template <typename T>
void SVValueObjectClassImpl<T>::Init()
{
	m_pBuckets = new SVArrayValueHolder<array_type, value_type>;
}

template <typename T>
const SVValueObjectClassImpl<T>& SVValueObjectClassImpl<T>::operator = (const SVValueObjectClassImpl<T>& rhs)
{
	if ( this != &rhs )
	{
		if ( SetObjectDepth( rhs.m_iNumberOfBuckets ) )
		{
			if ( S_OK == CreateBuckets() )
			{
				if ( 1 == rhs.ArraySize() )
				{
					m_ScalarBuckets = rhs.m_ScalarBuckets;
				}
				else
				{
// Jim 10.02.15 - allow the copy of empty vector.
					BucketNoAssert() = rhs.Buckets();
				}
				DefaultValue() = rhs.DefaultValue();

				m_iLastSetIndex = rhs.m_iLastSetIndex;
			}
		}
	}
	return *this;
}

template <typename T>
HRESULT SVValueObjectClassImpl<T>::SetDefaultValue(const T& value, bool bResetAll)
{
	DefaultValue() = value;
	if ( bResetAll )
	{
		if ( ArraySize() == 1 )
		{
			for (int i = 0; i < m_iNumberOfBuckets; ++ i)
			{
				ScalarBucket(i) = value;
			}
		}
		else
		{
			for (int i = 0; i < m_iNumberOfBuckets; ++ i)
			{
				for (size_t j=0; j < Buckets()[i].size(); ++j)
				{
					Buckets()[i][j] = value;
				}
			}
		}
	}
	return S_OK;
}

template <typename T>
HRESULT SVValueObjectClassImpl<T>::GetDefaultValue(T& rValue) const
{
	rValue = DefaultValue();
	return IsCreated() ? S_OK : S_FALSE;
}

template <typename T>
BOOL SVValueObjectClassImpl<T>::SetObjectDepth(int iNewObjectDepth)
{
	BOOL l_Status = true;

	if( iNewObjectDepth < 1 || iNewObjectDepth <= m_iLastSetIndex )
	{
		m_iLastSetIndex = -1;
	}

	l_Status &= SVValueObjectClass::SetObjectDepth(iNewObjectDepth);

	return l_Status;
}

template <typename T>
BOOL SVValueObjectClassImpl<T>::SetObjectDepthWithIndex(int iNewObjectDepth, int NewLastSetIndex)
{
	BOOL l_Status = true;

	if( 0 < iNewObjectDepth )
	{
		if( iNewObjectDepth <= m_iLastSetIndex )
		{
			l_Status = CopyValue( m_iLastSetIndex, NewLastSetIndex );
		}
	}
	else
	{
		m_iLastSetIndex = -1;
	}

	l_Status &= SVValueObjectClass::SetObjectDepthWithIndex(iNewObjectDepth, NewLastSetIndex);

	return l_Status;
}

template <typename T>
HRESULT SVValueObjectClassImpl<T>::CreateBuckets()
{
	HRESULT l_hrOk = S_OK;

	if( m_objectDepth < 2 )
	{
		m_objectDepth = 2;
	}

	if ( (m_iNumberOfBuckets != m_objectDepth) && (m_objectDepth > 0))
	{
		if ( ArraySize() == 1 )
		{
			BucketNoAssert().resize(0);	// avoid assert in Buckets()
			m_ScalarBuckets.resize( m_objectDepth, DefaultValue() );
		}
		else
		{
			m_ScalarBuckets.resize(0);

			array_type l_DefaultArray(ArraySize(), DefaultValue());
			Buckets().resize( m_objectDepth, l_DefaultArray );
		}
	}

	m_iNumberOfBuckets = m_objectDepth;

	l_hrOk = SVValueObjectClass::CreateBuckets();

	return l_hrOk;
}

template <typename T>
HRESULT SVValueObjectClassImpl<T>::SetArraySize(int iSize)
{
#ifdef _DEBUG
	int iOldSize = ArraySize();
#endif
	HRESULT hr = SVValueObjectClass::SetArraySize(iSize);

	if ( ArraySize() == 1 )
	{
		BucketNoAssert().resize(0);	// avoid assert in Buckets()
		if ( iSize == 0 )
			m_ScalarBuckets.resize(0);
		m_ScalarBuckets.resize( m_iNumberOfBuckets, DefaultValue() );
	}
	else
	{
		m_ScalarBuckets.resize(0);
		if ( Buckets().size() == 0 )
			Buckets().resize( m_iNumberOfBuckets );
		for ( int i=0; i < m_iNumberOfBuckets; i++ )
		{
			array_type& rArray = Buckets()[i];
			if ( iSize == 0 )
				rArray.clear();	// minimum array size is 1. If iSize == 0, clear the array then resize back to 1 below with the default value.
			rArray.resize( ArraySize(), DefaultValue() );
		}
	}

	return hr;
}

template <typename T>
__forceinline HRESULT SVValueObjectClassImpl<T>::CopyValue(int iSourceBucket, int iDestBucket)
{
	HRESULT hr = S_FALSE;

	if ((iSourceBucket >= 0 && iSourceBucket < m_iNumberOfBuckets) && (iDestBucket >= 0 && iDestBucket < m_iNumberOfBuckets))
	{
		if ( ArraySize() == 1 )
		{
			ScalarBucket(iDestBucket) = ScalarBucket(iSourceBucket);
		}
		else
		{
			Buckets()[iDestBucket] = Buckets()[iSourceBucket];
			m_aiResultSize[iDestBucket] = m_aiResultSize[iSourceBucket];
		}
		m_iLastSetIndex = iDestBucket;
		
		hr = S_OK;
	}

	return hr;
}

template <typename T>
__forceinline HRESULT SVValueObjectClassImpl<T>::SetValueAt( int iBucket, int iIndex, const T value )
{
	HRESULT hr = ValidateIndexes(iBucket, iIndex);

	if ( S_OK == hr || SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == hr )
	{
		m_iLastSetIndex  = iBucket;

		if ( 1 == ArraySize() )
		{
			m_ScalarBuckets[iBucket] = value;
		}
		else
		{
			Buckets()[iBucket][iIndex] = value;
		}
	}
	
	return hr;
}

template <typename T>
HRESULT SVValueObjectClassImpl<T>::GetValueAt( int iBucket, int iIndex, T& rValue ) const
{
	HRESULT hr = ValidateIndexes(iBucket, iIndex);

	// is the index valid?
	if ( S_OK == hr )
	{
		if ( !m_isObjectValid )	// optimization? or not? Is reading faster than writing to cache/memory?
		{
			m_isObjectValid = true;
		}
		if ( 1 == ArraySize() )
		{
			rValue = m_ScalarBuckets[iBucket];
			return hr;
		}
		else
		{
			rValue = Buckets()[iBucket][iIndex];
		}
	}
	else if ( SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == hr )
	{
		// inside the allocated array size but bucket does
		// not contain a value for the current run so use
		// the default value.
		rValue = DefaultValue();
		m_isObjectValid = true;
	}
	else	// BAD INDEX
	{
		// the selected index is not in the allocated array 
		// size. ie (-1, 1001).  use the default value.		 
		rValue = DefaultValue();
		m_isObjectValid = false;
	}
	return hr;
}

template <typename T>
HRESULT SVValueObjectClassImpl<T>::GetObjectValue( const SVString& rValueName, VARIANT& p_rVariantValue ) const
{
	HRESULT hr = S_OK;

	if( SvOi::cDefaultTag == rValueName )
	{
		_variant_t l_TempVariant;

		l_TempVariant.Attach( p_rVariantValue );

		l_TempVariant = ConvertToVariant( DefaultValue() );

		l_TempVariant.Detach();
	}
	else if( SvOi::cArrayTag == rValueName )
	{
		_variant_t l_TempVariant;

		l_TempVariant.Attach( p_rVariantValue );

		if( ArraySize() == 1 )
		{
			l_TempVariant = ConvertToVariant( ScalarBucket( m_iLastSetIndex ) );
		}
		else if( 1 < ArraySize() )
		{
			SVSAFEARRAY l_SafeArray;

			for( int i = 0; i < m_iArraySize; i++ )
			{
				_variant_t l_Value = ConvertToVariant( Element( m_iLastSetIndex, i ) );

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
		hr = SVValueObjectClass::GetObjectValue( rValueName, p_rVariantValue );
	}

	return hr;
}

template <typename T>
HRESULT SVValueObjectClassImpl<T>::SetObjectValue(SVObjectAttributeClass* pDataObject)
{
	HRESULT hr = S_FALSE;
	
	SvCl::SVObjectArrayClassTemplate<T> svArray;	// for default values
	bucket_type l_Buckets(BucketNoAssert());
	array_type l_Array;

	if ( SVObjectAttributeClassHelper::GetAttributeData(pDataObject, SvOi::cDefaultTag, svArray) )
	{
		if ( svArray.GetSize() > 0 )
			DefaultValue() = svArray[ svArray.GetSize()-1 ];
	}
	else if ( SVObjectAttributeClassHelper::GetAttributeData(pDataObject, SvOi::cBucketTag, l_Buckets, DefaultValue() ) )//  BUCKET_TAG_LOAD; get buckets, not array; for backward compatibility;
	{
		if ( ArraySize() == 1 )
		{
			ScalarBucket(0)=l_Buckets[0][0];
			ScalarBucket(1)=l_Buckets[1][0];
		}
		else
		{
			std::swap(Buckets(), l_Buckets);
		}
		if ( 1 < m_iNumberOfBuckets )
		{
			m_iLastSetIndex = 1;
		}
	}
	// new-style: store all array elements:
	else if ( SVObjectAttributeClassHelper::GetArrayData(pDataObject, SvOi::cArrayTag, l_Array, DefaultValue() ) )
	{
		SetArraySize( static_cast< int >( l_Array.size() ) );
		if ( ArraySize() == 1 )
		{
			ScalarBucket(1)=l_Array[0];
		}
		else
		{
			std::swap(Array(1), l_Array);
		}
		m_iLastSetIndex = 1;
	}
	else if ( SVObjectAttributeClassHelper::GetAttributeData(pDataObject, m_sLegacyScriptDefaultName.c_str(), svArray) )
	{
		if ( svArray.GetSize() > 0 )
			DefaultValue() = svArray[ svArray.GetSize()-1 ];
	}
	else if ( SVObjectAttributeClassHelper::GetAttributeData(pDataObject, m_sLegacyScriptArrayName.c_str(), l_Buckets, DefaultValue() ) )
	{
		if ( ArraySize() == 1 )
		{
			ScalarBucket(0)=l_Buckets[0][0];
			ScalarBucket(1)=l_Buckets[1][0];
		}
		else
		{
			std::swap(Buckets(), l_Buckets);
		}
		if ( 1 < m_iNumberOfBuckets )
		{
			m_iLastSetIndex = 1;
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
HRESULT SVValueObjectClassImpl<T>::GetArrayValues(std::vector<T>& raValues) const
{
	return GetArrayValuesImpl(m_iLastSetIndex, raValues);
}

template <typename T>
HRESULT SVValueObjectClassImpl<T>::GetArrayValues(int iBucket, std::vector<T>& raValues) const
{
	return GetArrayValuesImpl(iBucket, raValues);
}

template <typename T>
HRESULT SVValueObjectClassImpl<T>::GetArrayValues(int iBucket, DoubleVectorType& raValues) const
{
	// if the conversion is allowable, make the call, otherwise, don't.
	// the template Select keeps the compiler from compiling both paths which would generate a compile error when the conversion doesn't exist
	return Loki::Select< Loki::Conversion< value_type, DoubleType >::exists,
		conversion_exists, conversion_doesnt_exist>::Result().call(this, iBucket, raValues);
}

template <typename T>
HRESULT SVValueObjectClassImpl<T>::GetArrayValues(int iBucket, SVString& rValues) const
{
	HRESULT hrOk = S_FALSE;
	rValues = _T("");
	if ( iBucket >= 0 && iBucket < m_iNumberOfBuckets && 0 < GetResultSize(iBucket) )
	{
		hrOk = S_OK;

		GetValue( iBucket, 0, rValues );
		int resultSize = GetResultSize(iBucket);
		for( int i = 1; i < resultSize; ++i )
		{
			SVString valueString;
			GetValue( iBucket, i, valueString );
			rValues += _T(" ; ")+valueString;
		}
	}
	return hrOk;
}

template <typename T>
HRESULT SVValueObjectClassImpl<T>::GetArrayValuesAsVariant( int iBucket, VARIANT& rValue ) const
{
	HRESULT hrOk = S_FALSE;

	_variant_t l_Variant;

	l_Variant.Attach( rValue );

	if ( iBucket >= 0 && iBucket < m_iNumberOfBuckets )
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
			hrOk = S_OK;
			for( int i = 0; i < static_cast< int >( Array( iBucket ).size() ); ++i )
			{
				_variant_t l_Variant;

				GetValue( iBucket, i, l_Variant );

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
HRESULT SVValueObjectClassImpl<T>::GetArrayValuesAsVariantVector(int iBucket, std::vector<_variant_t>& raValues) const
{
	HRESULT hrOk = S_FALSE;
	assert( iBucket >= 0 && iBucket < m_iNumberOfBuckets );
	if ( iBucket >= 0 && iBucket < m_iNumberOfBuckets )
	{
		int iResultSize = GetResultSize(iBucket);
		assert( iResultSize <= ArraySize() );
		raValues.resize( iResultSize );
		if ( ArraySize() == 1 )
		{
			raValues[0] = ConvertToVariant( ScalarBucket(iBucket) );
		}
		else
		{
			array_type::const_iterator l_FromIter( Array(iBucket).begin() );

			std::vector<_variant_t>::iterator l_ToIter( raValues.begin() );

			while( l_ToIter != raValues.end() &&
				l_FromIter != Array(iBucket).end() && 
				l_FromIter != Array(iBucket).begin() + iResultSize )
			{
				*l_ToIter = ConvertToVariant( *l_FromIter );

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
inline void SVValueObjectClassImpl<T>::swap( SVValueObjectClassImpl<T>& rhs )
{
	assert( typeid( *this ) == typeid( rhs ) );

	if ( this != &rhs )
	{
		std::swap( m_DefaultValue, rhs.m_DefaultValue );
		m_ScalarBuckets.swap( rhs.m_ScalarBuckets );

		SVValueObjectClass::swap( rhs );
	}
}

// implements copying the specified data to the value object
// source data specified like STL ranges: e.g. myvector.begin(), myvector.end()
template <typename T>
template <class InIterator>
HRESULT SVValueObjectClassImpl<T>::SetArrayValues(int iBucket, InIterator begin, InIterator end)
{
	HRESULT hr = ValidateIndexes(iBucket,0);
	if ( S_OK == hr || SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == hr )
	{
		int iSize = static_cast< int >( std::distance(begin, end));
		assert( iSize <= ArraySize() );
		if ( iSize <= ArraySize() )
		{
			SetResultSize(iBucket, iSize);
			if ( iSize > 0 )
			{
				if ( 1 == ArraySize() )
				{
					ScalarBucket(iBucket) = *begin;
				}
				else
				{
					std::copy( begin, end, Array(iBucket).begin() );
				}
			}
			m_iLastSetIndex = iBucket;
			hr = S_OK;
		}
		else
		{
			hr = SVMSG_SVO_33_OBJECT_INDEX_INVALID;
		}
	}
	return hr;
}

// implements copying the specified array to a vector of any implicitly convertible type
template <typename T>
template <typename ELEMENT_TYPE>
HRESULT SVValueObjectClassImpl<T>::GetArrayValuesImpl(int iBucket, std::vector<ELEMENT_TYPE>& raValues) const
{
	HRESULT hrOk = S_FALSE;
	assert( iBucket >= 0 && iBucket < m_iNumberOfBuckets );
	if ( iBucket >= 0 && iBucket < m_iNumberOfBuckets )
	{
		int iResultSize = GetResultSize(iBucket);
		assert( iResultSize <= ArraySize() );
		raValues.resize( iResultSize );
		if ( 1 == ArraySize() )
		{
			raValues[0] = static_cast< ELEMENT_TYPE >(ScalarBucket(iBucket));
		}
		else
		{
			array_type::const_iterator l_FromIter( Array(iBucket).begin() );

			std::vector<ELEMENT_TYPE>::iterator l_ToIter( raValues.begin() );

			while( l_ToIter != raValues.end() &&
				l_FromIter != Array(iBucket).end() && 
				l_FromIter != Array(iBucket).begin() + iResultSize )
			{
				*l_ToIter = static_cast< ELEMENT_TYPE >( *l_FromIter );

				++l_FromIter;
				++l_ToIter;
			}
		}
		hrOk = S_OK;
	}
	return hrOk;
}

variant_t SVValueObjectClassImpl<SVString>::ConvertToVariant( const SVString& rValue ) const
{
	return _variant_t( rValue.c_str() );
}