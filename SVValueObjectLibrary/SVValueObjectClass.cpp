//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVValueObjectClass
//* .File Name       : $Workfile:   SVValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   15 May 2014 15:08:10  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <assert.h>
#include "SVValueObjectClass.h"
#include "SVMessage/SVMessage.h"
#include "SVUtilityLibrary/SVSafeArray.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVStatusLibrary/MessageManager.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// The commented out code below is for Factory Creatable classes
//SV_IMPLEMENT_CLASS(SVValueObjectClass, SVValueObjectClassGuid);

SVValueObjectClass::SVValueObjectClass( LPCSTR ObjectName )
: SVObjectClass(ObjectName)
{
	Initialize();
}

SVValueObjectClass::SVValueObjectClass(SVObjectClass* POwner, int StringResourceID)
: SVObjectClass(POwner, StringResourceID) 
{
	Initialize();
}

SVValueObjectClass::~SVValueObjectClass()
{
	SVObjectManagerClass::Instance().UnregisterSubObject(GetUniqueObjectID());
	SetObjectDepth(-1);
	if ( m_pBuckets )
	{
		delete m_pBuckets;
	}
}

void SVValueObjectClass::InitializeBuckets()
{
	m_objectDepth = 2;
	m_iArraySize = 1;

	CreateBuckets();

	if( 1 < m_iNumberOfBuckets )
	{
		m_iLastSetIndex = 1;
	}
}

void SVValueObjectClass::CreateBuckets( )
{
	m_aiResultSize.resize(m_iNumberOfBuckets);
	std::fill(m_aiResultSize.begin(), m_aiResultSize.end(), m_iArraySize);
}

BOOL SVValueObjectClass::CreateObject( SVObjectLevelCreateStruct* pCreateStructure )
{
	BOOL l_bOk = SVObjectClass::CreateObject( pCreateStructure );

	SVObjectManagerClass::Instance().RegisterSubObject(GetUniqueObjectID());
	ObjectAttributesAllowedRef() |= SV_DD_VALUE;	// derived classes need to reset this

	if ( m_iNumberOfBuckets < 2 )
	{
		l_bOk = l_bOk && SetObjectDepth( 2 );
	}

	CreateBuckets();

	if( l_bOk )
	{
		m_iLastSetIndex = 1;
	}

	return l_bOk;
}

BOOL SVValueObjectClass::CloseObject()
{
	SVObjectManagerClass::Instance().UnregisterSubObject(GetUniqueObjectID());
	return SVObjectClass::CloseObject();
}

bool SVValueObjectClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	CreateBuckets();
	return __super::ResetObject(pErrorMessages);
}

bool SVValueObjectClass::ResetAlways() const
{
	return m_bResetAlways;
}

SVResetItemEnum SVValueObjectClass::ResetItem() const
{
	return m_eResetItem;
}

HRESULT SVValueObjectClass::SetResetOptions( bool p_bResetAlways, SVResetItemEnum p_eResetItem )
{
	m_bResetAlways = p_bResetAlways;
	m_eResetItem = p_eResetItem;

	return S_OK;
}

HRESULT SVValueObjectClass::SetObjectValue(SVObjectAttributeClass* pDataObject)
{
	return SVObjectClass::SetObjectValue(pDataObject);
}

bool SVValueObjectClass::IsArray() const
{
	return m_iArraySize > 1;
}

int SVValueObjectClass::GetArraySize() const
{
	return m_iArraySize;
}

HRESULT SVValueObjectClass::SetArraySize(int iSize)
{
	HRESULT hr = S_OK;

	m_iArraySize = std::max(iSize, 1);	// minimum one array element

	if( m_iArraySize <= 1 )
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

	// default number of results == array size
	m_aiResultSize.resize(m_iNumberOfBuckets);
	std::fill(m_aiResultSize.begin(), m_aiResultSize.end(), iSize);	// allow the result size to be 0

	m_ObjectAttributesSet.resize( m_iArraySize, m_DefaultObjectAttributesSet );

	return hr;
}

HRESULT SVValueObjectClass::GetResultSize(int iBucket, int& riResultSize) const
{
	HRESULT hr = S_FALSE;

	if ( iBucket >= 0 && iBucket < m_iNumberOfBuckets )
	{
		assert( m_aiResultSize.size() == m_iNumberOfBuckets );
		riResultSize = GetResultSize(iBucket);
		hr = S_OK;
	}

	return hr;
}

int SVValueObjectClass::GetResultSize() const
{
	int iResultSize = 0;
	GetResultSize(m_iLastSetIndex, iResultSize);
	return iResultSize;
}

// protected, only to be used by derived classes
int SVValueObjectClass::GetResultSize(int iBucket) const
{
	return m_aiResultSize[iBucket];
}

HRESULT SVValueObjectClass::SetResultSize(int iBucket, int riResultSize)
{
	HRESULT hr = S_FALSE;

	assert( riResultSize <= m_iArraySize );

	if ( iBucket >= 0 && iBucket < m_iNumberOfBuckets )
	{
		assert( m_aiResultSize.size() == m_iNumberOfBuckets );
		m_aiResultSize[iBucket] = riResultSize;
		hr = S_OK;
	}

	return hr;
}

HRESULT SVValueObjectClass::CompareWithCurrentValueImpl( const SVString& rCompare ) const
{
	// default implementation
	HRESULT hr = S_FALSE;
	SVString CurrentValue;
	HRESULT hrGet = GetValue( CurrentValue );
	if ( S_OK == hrGet )
	{
		hr = ( (CurrentValue == rCompare) ? S_OK : S_FALSE );
	}
	return hr;
}

void SVValueObjectClass::ValidateValue( int iBucket, int iIndex, const SVString& rValue ) const
{
	HRESULT hr = ValidateIndexes(iBucket, iIndex);

	if ( S_OK != hr && SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE != hr ) //object index out of range will not throw
	{
		SvStl::MessageMgrStd Exception( SvStl::LogOnly );
		Exception.setMessage( hr, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
		Exception.Throw();
	}
}

HRESULT SVValueObjectClass::SetTypeName( LPCTSTR TypeName )
{
	m_TypeName = TypeName;	

	return S_OK;
}

void SVValueObjectClass::Initialize()
{
	m_iLastSetIndex = -1;
	m_iNumberOfBuckets = 0;
	m_iArraySize = 1;
	m_pBuckets = nullptr;
	m_bLegacyVectorObjectCompatibility = false;
	m_bResetAlways = false;
	m_eResetItem = SVResetItemIP;

	ObjectAttributesAllowedRef() = SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES;// | SV_CH_VALUE | SV_CH_CONDITIONAL;	// These are included in the DEFAULT
}

#pragma region IValueObject
HRESULT SVValueObjectClass::SetValue( const _variant_t& rValue )
{
	HRESULT hr = S_OK;
	if (!IsArray() || 0 == (VT_ARRAY & rValue.vt) || nullptr == rValue.parray)
	{
		hr = SetValue(m_iLastSetIndex, 0, SvUl_SF::createSVString(rValue).c_str()); 
	}
	else
	{
		SVSAFEARRAY safeArray( rValue );

		if (safeArray.size() > 0)
		{
			//fit array size to safeArray-size
			if (GetArraySize() != safeArray.size())
			{
				hr = SetArraySize(static_cast<int>(safeArray.size()));
			}

			//set all value to array
			for (int i=0; i<safeArray.size(); i++)
			{
				variant_t tmpVar;
				HRESULT tempHr = safeArray.GetElement( i, tmpVar );
				if (S_OK == hr)
				{
					hr = tempHr;
				}

				tempHr = SetValue(m_iLastSetIndex, i, SvUl_SF::createSVString(tmpVar).c_str()); 
				if (S_OK == hr)
				{
					hr = tempHr;
				}
			}
		}
		else
		{
			hr = SvOi::Err_10029_ValueObject_Parameter_WrongSize;
		}
	}

	return hr;
}

HRESULT SVValueObjectClass::GetValue( _variant_t& rValue ) const
{ 
	if (!IsArray())
	{
		return GetValue(*(rValue.GetAddress())); 
	}
	return GetValues(*(rValue.GetAddress())); 
}

void SVValueObjectClass::ValidateValue( const _variant_t& rValue ) const
{
	if (!IsArray() || 0 == (VT_ARRAY & rValue.vt) || nullptr == rValue.parray)
	{
		ValidateValue(m_iLastSetIndex, 0, SvUl_SF::createSVString(rValue).c_str()); 
	}
	else
	{
		SVSAFEARRAY safeArray( rValue );

		if (safeArray.size() > 0)
		{
			//set all value to array
			for (int i=0; i<safeArray.size(); i++)
			{
				variant_t tmpVar;
				HRESULT tempHr = safeArray.GetElement( i, tmpVar );
				if (S_OK != tempHr)
				{
					SVStringVector msgList;
					msgList.push_back(SvUl_SF::Format(_T("%d"), tempHr));
					SvStl::MessageMgrStd Exception( SvStl::LogOnly );
					Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING , SvOi::Tid_ValidateValue_InvalidElementInVariantArray, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
					Exception.Throw();
				}

				//check only if it is possible to set at zero index, because the array size is not changed yet.
				ValidateValue(m_iLastSetIndex, 0, SvUl_SF::createSVString(tmpVar).c_str()); 
			}
		}
		else
		{
			SvStl::MessageMgrStd Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_ValidateValue_ArraySizeInvalid, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10029_ValueObject_Parameter_WrongSize, GetUniqueObjectID() );
			Exception.Throw();
		}
	}
}
#pragma endregion IValueObject
