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
#include "SVValueObjectClass.h"

#include "SVInspectionProcess.h"
#include "SVTool.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//SV_IMPLEMENT_CLASS(SVValueObjectClass, SVValueObjectClassGuid);

SVValueObjectClass::SVValueObjectClass( LPCSTR ObjectName )
: SVObjectAppClass(ObjectName) 
{
	Initialize();
}


SVValueObjectClass::SVValueObjectClass(SVObjectClass* POwner, int StringResourceID)
: SVObjectAppClass(POwner, StringResourceID) 
{
	Initialize();
}

SVValueObjectClass::~SVValueObjectClass()
{
	if( nullptr != GetInspection() )
	{
		GetInspection()->UnregisterSubObject( this );
	}

	if( nullptr != GetTool() )
	{
		GetTool()->UnregisterSubObject( this );
	}
	
	SetObjectDepth(-1);
	if ( m_pBuckets )
		delete m_pBuckets;
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

HRESULT SVValueObjectClass::CreateBuckets( )
{
	m_aiResultSize.resize(m_iNumberOfBuckets);
	std::fill(m_aiResultSize.begin(), m_aiResultSize.end(), m_iArraySize);

	return S_OK;
}

BOOL SVValueObjectClass::CreateObject( SVObjectLevelCreateStruct* pCreateStructure )
{
	BOOL l_bOk = SVObjectAppClass::CreateObject( pCreateStructure );

	if( nullptr != GetTool() )
	{
		GetTool()->RegisterSubObject( this );
	}
	
	if( nullptr != GetInspection() )
	{
		GetInspection()->RegisterSubObject( this );
	}
	
	ObjectAttributesAllowedRef() |= SV_CH_VALUE | SV_CH_CONDITIONAL | SV_DD_VALUE;	// derived classes need to reset this

	if ( m_iNumberOfBuckets < 2 )
	{
		l_bOk = l_bOk && SetObjectDepth( 2 );
	}

	l_bOk = l_bOk && ( S_OK == CreateBuckets());

	if( l_bOk )
	{
		m_iLastSetIndex = 1;
	}

	return l_bOk;
}

BOOL SVValueObjectClass::CloseObject()
{
	if( nullptr != GetInspection() )
	{
		GetInspection()->UnregisterSubObject( this );
	}

	if( nullptr != GetTool() )
	{
		GetTool()->UnregisterSubObject( this );
	}
	
	return SVObjectAppClass::CloseObject();
}

HRESULT SVValueObjectClass::ResetObject()
{
	HRESULT l_hrOk = CreateBuckets();

	HRESULT hr = SVObjectAppClass::ResetObject();
	if ( S_OK == l_hrOk )
	{
		l_hrOk = hr;
	}

	return l_hrOk;
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

HRESULT SVValueObjectClass::SetObjectValue( const SVString& p_rValueName, const _variant_t& p_rVariantValue )
{
	return SVObjectAppClass::SetObjectValue( p_rValueName, p_rVariantValue );
}

HRESULT SVValueObjectClass::SetObjectValue(SVObjectAttributeClass* pDataObject)
{
	return SVObjectAppClass::SetObjectValue(pDataObject);
}

DWORD_PTR SVValueObjectClass::processMessage(DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext)
{
	DWORD_PTR DwResult = SVMR_NOT_PROCESSED;

	// Try to process message by yourself...
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;

	switch (dwPureMessageID)
	{
	case SVMSGID_RESET_ALL_OBJECTS:
		{
			HRESULT l_ResetStatus = ResetObject();
			if( S_OK != l_ResetStatus )
			{
				ASSERT( SUCCEEDED( l_ResetStatus ) );

				DwResult = SVMR_NO_SUCCESS;
			}
			else
			{
				DwResult = SVMR_SUCCESS;
			}
			break;
		}

	case SVMSGID_COPY_RESULT_DATA_FORWARD:
		HRESULT hrRet = CopyLastSetValue( static_cast<int>(DwMessageValue) );

		return (S_OK == hrRet) ? SVMR_SUCCESS : SVMR_NO_SUCCESS;
		break;
	}

	return (SVObjectAppClass::processMessage(DwMessageID, DwMessageValue, DwMessageContext) | DwResult);
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

	m_iArraySize = std::max(iSize,1);	// minimum one array element

	if( m_iArraySize <= 1 )
	{
		if( m_strTypeName.Find( _T(" Array") ) != -1 )
		{
			m_strTypeName = m_strTypeName.Left( m_strTypeName.GetLength() - 6 );
		}
	}
	else
	{
		if( m_strTypeName.Find( _T(" Array") ) == -1 )
		{
			m_strTypeName += _T(" Array") ;
		}
	}

	// default number of results == array size
	m_aiResultSize.resize(m_iNumberOfBuckets);
	std::fill(m_aiResultSize.begin(), m_aiResultSize.end(), iSize);	// allow the result size to be 0

	m_auObjectAttributesSet.resize( m_iArraySize, m_uDefaultObjectAttributesSet );

	return hr;
}

HRESULT SVValueObjectClass::GetResultSize(int iBucket, int& riResultSize) const
{
	HRESULT hr = S_FALSE;

	if ( iBucket >= 0 && iBucket < m_iNumberOfBuckets )
	{
		ASSERT( m_aiResultSize.size() == m_iNumberOfBuckets );
		riResultSize = m_aiResultSize[iBucket];
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

	ASSERT( riResultSize <= m_iArraySize );

	if ( iBucket >= 0 && iBucket < m_iNumberOfBuckets )
	{
		ASSERT( m_aiResultSize.size() == m_iNumberOfBuckets );
		m_aiResultSize[iBucket] = riResultSize;
		hr = S_OK;
	}

	return hr;
}

HRESULT SVValueObjectClass::CompareWithCurrentValueImpl( const CString& rstrCompare ) const
{
	// default implementation
	HRESULT hr = S_FALSE;
	CString strCurrentValue;
	HRESULT hrGet = GetValue( strCurrentValue );
	if ( S_OK == hrGet )
	{
		hr = ( (strCurrentValue == rstrCompare) ? S_OK : S_FALSE );
	}
	return hr;
}

HRESULT SVValueObjectClass::GetValue( int iBucket, int iIndex, SVScalarValue& rValue ) const
{
	rValue.strName = GetCompleteObjectName();
	HRESULT hr = GetValue( iBucket, iIndex, rValue.strValue );

	return hr;
}

HRESULT SVValueObjectClass::GetTypeName( CString& p_rstrValue )const 
{
	p_rstrValue = m_strTypeName;	

	return S_OK;
}

HRESULT SVValueObjectClass::SetTypeName( LPCTSTR p_pstrValue )
{
	m_strTypeName = p_pstrValue;	

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
#pragma endregion IValueObject
