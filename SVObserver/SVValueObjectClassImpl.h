//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVValueObjectClassImpl
//* .File Name       : $Workfile:   SVValueObjectClassImpl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Oct 2013 08:24:44  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <assert.h>
#include "SVTimerLibrary/SVClock.h"
#include "SVUtilityLibrary/SVSAFEARRAY.h"
#include "UseTypeExceptMatch.h"
#include "SVValueObjectClass.h"
#pragma endregion Includes

// implement default behavior for most of the types
// derived classes should override the virtuals needed for type-specific behavior.
template <typename T>
class SVValueObjectClassImpl : public SVValueObjectClass
{
public:
	virtual ~SVValueObjectClassImpl(); // See C++ Coding standards page 90 avoid memory leaks. public virtual destructor.
	typedef SVValueObjectClassImpl<T>  this_type;
	typedef typename T                          value_type;
	typedef std::vector <value_type>   array_type;
	typedef std::vector <value_type>   bucket_type_scalar;
	typedef std::vector <array_type>   bucket_type_array;
	typedef bucket_type_array          bucket_type;
	typedef value_type*                bucket_pointer_type;

	array_type m_ScalarBuckets;
	value_type m_DefaultValue;



	const this_type& operator = (const this_type& rhs);
	void swap( SVValueObjectClassImpl<T>& rhs );	// provide an efficient swap; derived classes with extra data should redefine.

	virtual BOOL      SetObjectDepth ( int iNewObjectDepth );
	virtual BOOL SetObjectDepthWithIndex( int NewObjectDepth, int NewLastSetIndex );
	virtual HRESULT SetArraySize   ( int iSize );
	virtual HRESULT CopyValue      ( int src, int dest );
	
	HRESULT SetDefaultValue( T value, bool bResetAll );
	HRESULT GetDefaultValue( T& rValue ) const;

	template <typename ELEMENT_TYPE>	// enable any allowable (implicit) conversions if using derived class interface
	HRESULT GetValues(std::vector<ELEMENT_TYPE>& raValues) const {return GetArrayValuesImpl(GetLastSetIndex(), raValues);}

	template <typename ELEMENT_TYPE>	// enable any allowable (implicit) conversions if using derived class interface
	HRESULT GetValues(int iBucket, std::vector<ELEMENT_TYPE>& raValues) const {return GetArrayValuesImpl(iBucket, raValues);}

	// implements copying the specified vector to the value object
	template <typename ELEMENT_TYPE>
	HRESULT SetArrayValues(int iBucket, std::vector<ELEMENT_TYPE>& raValues)
	{
		return SetArrayValues(iBucket, raValues.begin(), raValues.end());
	}

	// implements copying the specified data to the value object
	// source data specified like STL ranges: e.g. myvector.begin(), myvector.end()
	template <class InIterator>
	HRESULT SetArrayValues(int iBucket, InIterator begin, InIterator end)//std::vector<ELEMENT_TYPE>& raValues)
	{
		HRESULT hr = ValidateIndexes(iBucket,0);
		if ( hr == S_OK || hr == SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE )
		{
			int iSize = static_cast< int >( std::distance(begin, end));
			ASSERT( iSize <= ArraySize() );
			if ( iSize <= ArraySize() )
			{
				SetResultSize(iBucket, iSize);
				if ( iSize > 0 )
				{
					if ( ArraySize() == 1 )
						ScalarBucket(iBucket) = *begin;
					else
						std::copy( begin, end, Array(iBucket).begin() );
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


protected:
	typedef SVValueObjectClassImpl<T> base;	// to be used only by derived class!

	SVValueObjectClassImpl() : SVValueObjectClass() {Init();}
	SVValueObjectClassImpl(LPCTSTR lpszObjectName) : SVValueObjectClass(lpszObjectName) {Init();}
	SVValueObjectClassImpl(SVObjectClass* pOwner, int StringResourceID) : SVValueObjectClass(pOwner, StringResourceID) {Init();}

	virtual HRESULT CreateBuckets();
	virtual HRESULT SetValueAt( int iBucket, int iIndex, T value );
	virtual HRESULT GetValueAt( int iBucket, int iIndex, T& rValue ) const;

	virtual HRESULT GetObjectValue( const SVString& p_rValueName, VARIANT& p_rVariantValue ) const;
	virtual HRESULT SetObjectValue( const SVString& p_rValueName, const _variant_t& p_rVariantValue );
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* pDataObject );

	virtual HRESULT GetArrayValues(std::vector<T>& raValues) const;	// allow copy of native type
	virtual HRESULT GetArrayValues(int iBucket, std::vector<T>& raValues) const;	// allow copy of native type
	// implement <double> for all value object types (that can convert to double) to support the equation array capability:
	typedef typename UseTypeExceptMatch<std::vector<double>, std::vector<T> >::type DoubleVectorType;
	typedef typename UseTypeExceptMatch<double, T>::type DoubleType;
	virtual HRESULT GetArrayValues(int iBucket, DoubleVectorType& raValues) const;	// allow copy to vector<double>
	virtual HRESULT GetArrayValuesAsVariant( int iBucket, VARIANT& rValue ) const;	// allow copy to VARIANT SAFEARRAY
	virtual HRESULT GetArrayValuesAsVariantVector(int iBucket, std::vector<_variant_t>& raValues) const;	// allow copy to vector<_variant_t>

	// implements copying the specified array to a vector of any implicitly convertible type
	template <typename ELEMENT_TYPE>
	HRESULT GetArrayValuesImpl(int iBucket, std::vector<ELEMENT_TYPE>& raValues) const
	{
		HRESULT hrOk = S_FALSE;
		ASSERT( iBucket >= 0 && iBucket < m_iNumberOfBuckets );
		if ( iBucket >= 0 && iBucket < m_iNumberOfBuckets )
		{
			int iResultSize = GetResultSize(iBucket);
			ASSERT( iResultSize <= ArraySize() );
			raValues.resize( iResultSize );
			if ( ArraySize() == 1 )
				raValues[0] = static_cast< ELEMENT_TYPE >(ScalarBucket(iBucket));
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

	// Comments on m_ScalarBuckets vs. m_pBuckets:
	// Because of cache hits, having a two-dimensional array for all value objects caused
	// approx. 20% slowdown per inspection on a large (100 tools) configuration.
	// therefore, since 99.9% of all value objects are single array element, the optimization
	// was put in to implement that case as a single dimentional array, the way the pre-array value objects worked.
	// In most cases, the derived classes can use the Element function which will work correctly regardless
	// of which mode is being used. However, in certain cases, the derived classes need to be aware of which mode
	// the object is operating in, and use the correct accessor function ( Buckets() and Array() vs. ScalarBucket() )

	__forceinline value_type& Element(int iBucket, int iArray)
		{if (ArraySize() == 1) return m_ScalarBuckets[iBucket]; else return (static_cast<SVArrayValueHolder<array_type>*> (m_pBuckets))->m_array[iBucket][iArray];}
	__forceinline const value_type& Element(int iBucket, int iArray) const
		{if (ArraySize() == 1) return m_ScalarBuckets[iBucket]; else return (static_cast<const SVArrayValueHolder<array_type>*> (m_pBuckets))->m_array[iBucket][iArray];}

	__forceinline value_type& ScalarBucket(int iBucket)
		{ASSERT(ArraySize() == 1); return m_ScalarBuckets[iBucket];}
	__forceinline const value_type& ScalarBucket(int iBucket) const
		{ASSERT(ArraySize() == 1); return m_ScalarBuckets[iBucket];}

	__forceinline array_type& Array(int iBucket)
		{ASSERT(ArraySize() != 1); return (static_cast<SVArrayValueHolder<array_type>*> (m_pBuckets))->m_array[iBucket];}
	__forceinline const array_type& Array(int iBucket) const
		{ASSERT(ArraySize() != 1); return (static_cast<const SVArrayValueHolder<array_type>*> (m_pBuckets))->m_array[iBucket];}

	__forceinline bucket_type& Buckets()
		{ASSERT(ArraySize() != 1); return (static_cast<SVArrayValueHolder<array_type>*> (m_pBuckets))->m_array;}
	__forceinline const bucket_type& Buckets() const
		{ASSERT(ArraySize() != 1); return (static_cast<const SVArrayValueHolder<array_type>*> (m_pBuckets))->m_array;}
	__forceinline bucket_type& BucketsNoAssert()
		{return (static_cast<SVArrayValueHolder<array_type>*> (m_pBuckets))->m_array;}
	__forceinline const bucket_type& BucketsNoAssert() const
		{return (static_cast<const SVArrayValueHolder<array_type>*> (m_pBuckets))->m_array;}
	static const bucket_type& Buckets(const this_type& rhs)
		{ASSERT(rhs.ArraySize() != 1); return (static_cast<const SVArrayValueHolder<array_type>*> (rhs.m_pBuckets))->m_array;}
	static const bucket_type& Buckets(const SVArrayValueHolderBase* pArrayHolder)
		{ASSERT(rhs.ArraySize() != 1); return (static_cast<const SVArrayValueHolder<array_type>*> (pArrayHolder))->m_array;}

	__forceinline value_type& DefaultValue()
		{return m_DefaultValue;}
	__forceinline const value_type& DefaultValue() const
		{return m_DefaultValue;}
	static const value_type& DefaultValue(const this_type& rhs)
		{return rhs.m_DefaultValue;}
	static const value_type& DefaultValue(const SVArrayValueHolderBase* pArrayHolder)
		{return rhs.m_DefaultValue;}

	// helper structs for GetArrayValues(int iBucket, std::vector< UseTypeExceptMatch<double, T>::result >& raValues)
	struct conversion_exists
	{
		HRESULT call(const this_type* pThis, int iBucket, std::vector<double>& raValues)
		{return pThis->GetArrayValuesImpl(iBucket, raValues);}
	};
	struct conversion_doesnt_exist
	{
		HRESULT call(const this_type* pThis, int iBucket, DoubleVectorType& raValues)
		{return S_FALSE;}
	};
	friend struct conversion_exists;
	friend struct conversion_doesnt_exist;

	// used by SetObjectValue; one set per template type
	static CString m_sLegacyScriptDefaultName;
	static CString m_sLegacyScriptArrayName;

private:
	void Init();
};// end SVValueObjectClassImpl

template <typename T>
CString SVValueObjectClassImpl<T>::m_sLegacyScriptDefaultName;
template <typename T>
CString SVValueObjectClassImpl<T>::m_sLegacyScriptArrayName;

// we don't want to use the default std::swap because it will invoke copy constructors and destructors.
// Make the efficient member function available to algorithms:
// according to C++ Templates: the complete guide( Vandevoorde/Josuttis ) section 11.4,
// these swap functions will succeed when passed a derived class

template < typename T >
inline void swap ( SVValueObjectClassImpl<T>& lhs, SVValueObjectClassImpl<T>& rhs )
{
	lhs.swap(rhs);
}

#include "SVValueObjectClassImpl.inl"

