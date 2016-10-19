//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file DoubleSortValueObject.cpp
/// All Rights Reserved 
//*****************************************************************************
/// This class is a doubleValueObject, but the order of the array will be defined by a map.
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "DoubleSortValueObject.h"
#include "SVObjectLibrary\SVClsids.h"
#include "SVObjectLibrary\SVObjectLibrary.h"
#include "SVStatusLibrary\MessageManager.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS(DoubleSortValueObject, DoubleSortValueObjectGuid);
#pragma endregion Declarations

#pragma region Constructor
DoubleSortValueObject::DoubleSortValueObject(LPCTSTR ObjectName)
	: SVDoubleValueObjectClass(ObjectName) 
{
	LocalInitialize();
}

DoubleSortValueObject::DoubleSortValueObject(SVObjectClass* POwner, int StringResourceID)
	: SVDoubleValueObjectClass(POwner, StringResourceID) 
{
	LocalInitialize();
}

DoubleSortValueObject::DoubleSortValueObject( const DoubleSortValueObject& rhs )
	: SVDoubleValueObjectClass()
{
	LocalInitialize();
	*this = rhs;
}

const DoubleSortValueObject& DoubleSortValueObject::operator = (const DoubleSortValueObject& rhs)
{
	base::operator = (rhs);
	return *this;
}

DoubleSortValueObject::~DoubleSortValueObject()
{
	// Delete all object's data arrays...
	SetObjectDepth(0);
	CreateBuckets();
}
#pragma endregion Constructor

#pragma region Public Methods
const ValueObjectSortContainer& DoubleSortValueObject::getSortContainer(int iBucket) const 
{
	ASSERT(iBucket >= 0 && iBucket < m_iNumberOfBuckets);
	if (iBucket < 0 || iBucket > m_iNumberOfBuckets)
	{
		iBucket = 0;
	}
	
	return m_sortContainerArray[iBucket];
}

HRESULT DoubleSortValueObject::setSortContainer(int iBucket, const ValueObjectSortContainer& sortMap) 
{
	HRESULT result(E_FAIL);
	if (iBucket >= 0 && iBucket < m_iNumberOfBuckets && m_isCreated)
	{
		m_sortContainerArray[iBucket] = sortMap;
		result = S_OK;
	}
	return result;
}

HRESULT DoubleSortValueObject::CopyValue(int iSourceBucket, int iDestBucket)
{
	if ((iSourceBucket >= 0 && iSourceBucket < m_iNumberOfBuckets) && (iDestBucket >= 0 && iDestBucket < m_iNumberOfBuckets))
	{
		m_sortContainerArray[iDestBucket] = m_sortContainerArray[iSourceBucket];
		return __super::CopyValue(iSourceBucket, iDestBucket);
	}

	return S_FALSE;
}
#pragma endregion Public Methods

#pragma region Protected Methods
HRESULT DoubleSortValueObject::CreateBuckets( )
{
	HRESULT result = SVDoubleValueObjectClass::CreateBuckets();
	m_sortContainerArray.resize(m_iNumberOfBuckets);
	return result;
}

HRESULT DoubleSortValueObject::SetValueAt( int iBucket, int iIndex, const VARIANT& rvtValue )
{
	if (0 <= iBucket && iBucket < m_iNumberOfBuckets && 0 <= iIndex && m_sortContainerArray[iBucket].size()>iIndex)
	{
		return SVDoubleValueObjectClass::SetValueAt(iBucket, m_sortContainerArray[iBucket][iIndex], rvtValue);
	}
	return E_FAIL;
}

HRESULT DoubleSortValueObject::SetValueAt(int iBucket, int iIndex, const int value)
{
	if (0 <= iBucket && iBucket < m_iNumberOfBuckets && 0 <= iIndex && m_sortContainerArray[iBucket].size()>iIndex)
	{
		return SVDoubleValueObjectClass::SetValueAt(iBucket, m_sortContainerArray[iBucket][iIndex], value);
	}
	return E_FAIL;
}

HRESULT DoubleSortValueObject::SetValueAt(int iBucket, int iIndex, const long value)
{
	if (0 <= iBucket && iBucket < m_iNumberOfBuckets && 0 <= iIndex && m_sortContainerArray[iBucket].size()>iIndex)
	{
		return SVDoubleValueObjectClass::SetValueAt(iBucket, m_sortContainerArray[iBucket][iIndex], value);
	}
	return E_FAIL;
}

HRESULT DoubleSortValueObject::SetValueAt(int iBucket, int iIndex, const double value)
{
	if (0 <= iBucket && iBucket < m_iNumberOfBuckets && 0 <= iIndex && m_sortContainerArray[iBucket].size()>iIndex)
	{
		return SVDoubleValueObjectClass::SetValueAt(iBucket, m_sortContainerArray[iBucket][iIndex], value);
	}
	return E_FAIL;
}

HRESULT DoubleSortValueObject::SetValueAt( int iBucket, int iIndex, const CString& value )
{
	try
	{
		ValidateValue( iBucket, iIndex, SVString(value) );
		return SVDoubleValueObjectClass::SetValueAt(iBucket, m_sortContainerArray[iBucket][iIndex], value);
	}
	catch(const SvStl::MessageContainer&)
	{
		ASSERT(FALSE);
		return E_FAIL;
	}
}

HRESULT DoubleSortValueObject::GetValueAt( int iBucket, int iIndex, double& rValue ) const
{
	if (0 <= iBucket && iBucket < m_iNumberOfBuckets && 0 <= iIndex && m_sortContainerArray[iBucket].size()>iIndex)
	{
		return SVDoubleValueObjectClass::GetValueAt(iBucket, m_sortContainerArray[iBucket][iIndex], rValue);
	}
	return E_FAIL;
}

HRESULT DoubleSortValueObject::GetValueAt( int iBucket, int iIndex, long& rValue ) const
{
	if (0 <= iBucket && iBucket < m_iNumberOfBuckets && 0 <= iIndex && m_sortContainerArray[iBucket].size()>iIndex)
	{
		return SVDoubleValueObjectClass::GetValueAt(iBucket, m_sortContainerArray[iBucket][iIndex], rValue);
	}
	return E_FAIL;
}

HRESULT DoubleSortValueObject::GetValueAt( int iBucket, int iIndex, DWORD& rValue ) const
{
	if (0 <= iBucket && iBucket < m_iNumberOfBuckets && 0 <= iIndex && m_sortContainerArray[iBucket].size()>iIndex)
	{
		return SVDoubleValueObjectClass::GetValueAt(iBucket, m_sortContainerArray[iBucket][iIndex], rValue);
	}
	return E_FAIL;
}

HRESULT DoubleSortValueObject::GetValueAt( int iBucket, int iIndex, CString& rValue) const
{
	if (0 <= iBucket && iBucket < m_iNumberOfBuckets && 0 <= iIndex && m_sortContainerArray[iBucket].size()>iIndex)
	{
		return SVDoubleValueObjectClass::GetValueAt(iBucket, m_sortContainerArray[iBucket][iIndex], rValue);
	}
	return E_FAIL;
}

HRESULT DoubleSortValueObject::GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const
{
	if (0 <= iBucket && iBucket < m_iNumberOfBuckets && 0 <= iIndex && m_sortContainerArray[iBucket].size()>iIndex)
	{
		return SVDoubleValueObjectClass::GetValueAt(iBucket, m_sortContainerArray[iBucket][iIndex], rValue);
	}
	return E_FAIL;
}

HRESULT DoubleSortValueObject::GetArrayValues(int iBucket, std::vector<double>& raValues) const
{
	HRESULT hrOk = E_FAIL;
	ASSERT( iBucket >= 0 && iBucket < m_iNumberOfBuckets );
	if ( iBucket >= 0 && iBucket < m_iNumberOfBuckets )
	{
		int iResultSize = GetResultSize(iBucket);
		ASSERT( iResultSize <= ArraySize() );
		raValues.resize( iResultSize );
		double value = 0;
		for (int i=0; i<iResultSize; i++)
		{
			//must be get once by once, because values can be disorder and not in a row.
			GetValueAt( iBucket, i, value );
			raValues[i] = value;
		}
		hrOk = S_OK;
	}
	return hrOk;
}

HRESULT DoubleSortValueObject::GetArrayValuesAsVariant( int iBucket, VARIANT&  rValue ) const
{
	HRESULT hrOk = E_FAIL;
	_variant_t variantValue;
	variantValue.Attach( rValue );

	if ( iBucket >= 0 && iBucket < m_iNumberOfBuckets )
	{
		SVSAFEARRAY l_SafeArray;

		hrOk = S_OK;
		int iResultSize = GetResultSize(iBucket);
		for( int i = 0; i < iResultSize; ++i )
		{
			_variant_t l_Variant;
			GetValue( iBucket, i, l_Variant );
			HRESULT l_TempStatus = l_SafeArray.Add( l_Variant );
			if( S_OK == hrOk )
			{
				hrOk = l_TempStatus;
			}
		}

		if( S_OK == hrOk )
		{
			variantValue = l_SafeArray;
		}
		else
		{
			variantValue.Clear();
		}
	}
	else
	{
		variantValue.Clear();
	}

	rValue = variantValue.Detach();

	return hrOk;
}

HRESULT DoubleSortValueObject::GetArrayValuesAsVariantVector( int iBucket, std::vector< _variant_t >&  rValues ) const
{
	HRESULT hrOk = E_FAIL;
	ASSERT( iBucket >= 0 && iBucket < m_iNumberOfBuckets );
	if ( iBucket >= 0 && iBucket < m_iNumberOfBuckets )
	{
		int iResultSize = GetResultSize(iBucket);
		ASSERT( iResultSize <= ArraySize() );
		rValues.resize( iResultSize );
		_variant_t value = 0;
		for (int i=0; i<iResultSize; i++)
		{
			//must be get once by once, because values can be disorder and not in a row.
			GetValueAt( iBucket, i, value );
			rValues[i] = value;
		}
		hrOk = S_OK;
	}
	return hrOk;
}

void DoubleSortValueObject::ValidateValue( int iBucket, int iIndex, const SVString& rValue ) const
{
	if (0 > iBucket || iBucket >= m_iNumberOfBuckets || 0 > iIndex && m_sortContainerArray[iBucket].size()<=iIndex)
	{
		SVStringArray msgList;
		msgList.push_back(GetName());
		SvStl::MessageMgrStd Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_LinkedValue_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
		Exception.Throw();
	}
	base::ValidateValue( iBucket, iIndex, rValue );
}
#pragma endregion Protected Methods

#pragma region Private Methods
void DoubleSortValueObject::LocalInitialize()
{
	m_outObjectInfo.ObjectTypeInfo.ObjectType = DoubleSortValueObjectType;
	m_strTypeName = "Decimal";
	m_bLegacyVectorObjectCompatibility = false;
}
#pragma endregion Private Methods
