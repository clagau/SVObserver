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

#include "SVObjectAttributeClassHelper.h"	// solely for compile speed & dependency separation (SetObjectValue)
#include "SVLoki/TypeManip.h"	// Loki
#include "SVMessage/SVMessage.h"


inline HRESULT SVValueObjectClass::ValidateIndexes(int iBucket, int iArrayIndex) const
{
	if (iBucket >= 0 && iBucket < m_iNumberOfBuckets)
	{
		if ( iArrayIndex >= 0 && iArrayIndex < m_iArraySize )
		{
			if ( 1 == m_iArraySize || iArrayIndex < m_aiResultSize[iBucket] )
			{
				return S_OK;
			}
			else	// ARRAY INDEX OUT OF BOUNDS (out of result range)
			{
				return SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE;
			}
		}
		else	// BAD ARRAY INDEX
		{
			return SVMSG_SVO_33_OBJECT_INDEX_INVALID;
		}
	}
	else	// BAD BUCKET
	{
		ASSERT( iBucket == -1 );	// -1 is a "normal" invalid index
		return SVMSG_SVO_33_OBJECT_INDEX_INVALID;
	}
}


// protected member function; only derived class should call this
inline void SVValueObjectClass::swap( SVValueObjectClass& rhs )
{
	ASSERT( typeid( *this ) == typeid( rhs ) );

	if ( this != &rhs )
	{
		std::swap( m_iNumberOfBuckets, rhs.m_iNumberOfBuckets );
		std::swap( m_iLastSetIndex, rhs.m_iLastSetIndex );
		std::swap( m_iArraySize, rhs.m_iArraySize );
		//swap( m_aiResultSize, rhs.m_aiResultSize );	// not sure if VC6 implements redirection to vector member function
		m_aiResultSize.swap( rhs.m_aiResultSize );

		std::swap( m_pBuckets, rhs.m_pBuckets );// assumes rhs was allocated in the same memory space!

		std::swap( m_bLegacyVectorObjectCompatibility, rhs.m_bLegacyVectorObjectCompatibility );

		std::swap( m_bResetAlways, m_bResetAlways );
		std::swap( m_eResetItem, m_eResetItem );
	}
}

