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

#include <assert.h>
#include "SVTimerLibrary/SVClock.h"
#include "SVUtilityLibrary/SVSAFEARRAY.h"

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
			if ( CreateBuckets() == S_OK )
			{
				if ( rhs.ArraySize() == 1 )
					m_ScalarBuckets = rhs.m_ScalarBuckets;
				else
					Buckets() = Buckets(rhs);

				DefaultValue() = DefaultValue(rhs);

				m_iLastSetIndex = rhs.m_iLastSetIndex;
			}
		}
	}

	return *this;
}


template <typename T>
HRESULT SVValueObjectClassImpl<T>::SetDefaultValue(const T value, bool bResetAll)
{
	//ASSERT((!pBArray && m_iNumberOfBuckets <= 0) || (pBArray && m_iNumberOfBuckets > 0));
	
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

	if( objectDepth < 2 )
	{
		objectDepth = 2;
	}

	if ( m_iNumberOfBuckets != objectDepth && objectDepth > 0)
	{
		if ( ArraySize() == 1 )
		{
			BucketsNoAssert().resize(0);	// avoid ASSERT in Buckets()
			m_ScalarBuckets.resize( objectDepth, DefaultValue() );
		}
		else
		{
			m_ScalarBuckets.resize(0);

			array_type l_DefaultArray(ArraySize(), DefaultValue());
			Buckets().resize( objectDepth, l_DefaultArray );
		}
	}

	m_iNumberOfBuckets = objectDepth;

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
		BucketsNoAssert().resize(0);	// avoid ASSERT in Buckets()
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

	if ( hr == S_OK || hr == SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE )
	{
		m_iLastSetIndex  = iBucket;

		if ( ArraySize() == 1 )
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
	if ( hr == S_OK )
	{
		if ( isObjectValid == FALSE )	// optimization? or not? Is reading faster than writing to cache/memory?
			isObjectValid = TRUE;
		if ( ArraySize() == 1 )
		{
			rValue = m_ScalarBuckets[iBucket];
			return hr;
		}
		else
			rValue = Buckets()[iBucket][iIndex];
	}
	else if ( hr == SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE )
	{
		rValue = DefaultValue();
		isObjectValid = TRUE;
	}
	else	// BAD INDEX
	{
		rValue = DefaultValue();
		isObjectValid = FALSE;
	}

	return hr;
}

template <typename T>
HRESULT SVValueObjectClassImpl<T>::GetObjectValue( const SVString& p_rValueName, VARIANT& p_rVariantValue ) const
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
			l_TempVariant = ScalarBucket( m_iLastSetIndex );
		}
		else if( 1 < ArraySize() )
		{
			SVSAFEARRAY l_SafeArray;

			for( int i = 0; i < m_iArraySize; i++ )
			{
				_variant_t l_Value = Element( m_iLastSetIndex, i );

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
HRESULT SVValueObjectClassImpl<T>::SetObjectValue( const SVString& p_rValueName, const _variant_t& p_rVariantValue )
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

				if( hr == S_OK )
				{
					ScalarBucket(1) = l_Value;

					m_iLastSetIndex = 1;
				}
			}
			else
			{
				Array(1).resize( l_SafeArray.size() );

				for( size_t i = 0; i < l_SafeArray.size(); i++ )
				{
					_variant_t l_Value;

					if( l_SafeArray.GetElement( i, l_Value ) == S_OK )
					{
						Array(1)[ i ] = l_Value;
					}
					else
					{
						Array(1)[ i ] = DefaultValue();
					}
				}

				m_iLastSetIndex = 1;
			}
		}
		else if ( ArraySize() == 1 )
		{
			ScalarBucket(1) = p_rVariantValue;

			m_iLastSetIndex = 1;
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
HRESULT SVValueObjectClassImpl<T>::SetObjectValue(SVObjectAttributeClass* pDataObject)
{
//	ASSERT( !m_sLegacyScriptDefaultName.IsEmpty() );
//	ASSERT( !m_sLegacyScriptArrayName.IsEmpty() );

	HRESULT hr = S_FALSE;
	
	SVObjectArrayClassTemplate<T> svArray;	// for default values
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
	else if ( SVObjectAttributeClassHelper::GetArrayData(pDataObject, "Array_Elements", l_Array, DefaultValue() ) )
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
	else if ( SVObjectAttributeClassHelper::GetAttributeData(pDataObject, m_sLegacyScriptDefaultName, svArray) )
	{
		if ( svArray.GetSize() > 0 )
			DefaultValue() = svArray[ svArray.GetSize()-1 ];
	}
	//else if ( bOk = pDataObject->GetAttributeData("pBArray", Buckets() ) )
	else if ( SVObjectAttributeClassHelper::GetAttributeData(pDataObject, m_sLegacyScriptArrayName, l_Buckets, DefaultValue() ) )
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
			for( int i = 0; i < static_cast< int >( Array( iBucket ).size() ); ++i )
			{
				_variant_t l_Variant;

				GetValue( iBucket, i, l_Variant );

				HRESULT l_TempStatus = l_SafeArray.Add( l_Variant );

				if( hrOk == S_OK )
				{
					hrOk = l_TempStatus;
				}
			}
		}

		if( hrOk == S_OK )
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
	ASSERT( iBucket >= 0 && iBucket < m_iNumberOfBuckets );
	if ( iBucket >= 0 && iBucket < m_iNumberOfBuckets )
	{
		int iResultSize = GetResultSize(iBucket);
		ASSERT( iResultSize <= ArraySize() );
		raValues.resize( iResultSize );
		if ( ArraySize() == 1 )
			raValues[0] = ScalarBucket(iBucket);
		else
		{
			array_type::const_iterator l_FromIter( Array(iBucket).begin() );

			std::vector<_variant_t>::iterator l_ToIter( raValues.begin() );

			while( l_ToIter != raValues.end() &&
				l_FromIter != Array(iBucket).end() && 
				l_FromIter != Array(iBucket).begin() + iResultSize )
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
inline void SVValueObjectClassImpl<T>::swap( SVValueObjectClassImpl<T>& rhs )
{
	ASSERT( typeid( *this ) == typeid( rhs ) );
	using std::swap;

	if ( this != &rhs )
	{
		swap( m_DefaultValue, rhs.m_DefaultValue );
		//swap( m_ScalarBuckets, rhs.m_ScalarBuckets );// not sure if VC6 implements redirection to vector member function
		m_ScalarBuckets.swap( rhs.m_ScalarBuckets );

		SVValueObjectClass::swap( rhs );
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVValueObjectClassImpl.inl_v  $
 * 
 *    Rev 1.1   02 Oct 2013 08:24:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 16:17:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   18 Feb 2013 13:08:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Value Object classes to include new GetValues methods that take VARIANT as the value parameter.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   21 Jan 2013 10:58:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated missing SetObjectValue and GetObjectValue methods based on 6.01 branch changes for the shift tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   08 Jan 2013 12:35:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  805
 * SCR Title:  Fix Value Object Indexing Problem when Shortening PPQ
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new virtual method to classes to fix adjusting Object Depth of Value Object and Last Set Index.  Also, updated code with new methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   14 Feb 2012 13:02:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated target windows version to XP (_WIN32_WINNT 0x0501).
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   30 Mar 2011 16:36:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix performance problems with the extent changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   21 Mar 2011 12:20:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated system to fix performance issues with extent checks and updates.  Added additional methods to determine if object has extents and moved extent updated to  associated children.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   18 Mar 2011 07:46:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix performance issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Dec 2010 13:48:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   28 Oct 2010 14:27:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Apr 2008 14:59:16   JSpila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source to split the value objects into individual files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
