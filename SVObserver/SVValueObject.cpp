//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVValueObject
//* .File Name       : $Workfile:   SVValueObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 16:16:14  $
//******************************************************************************

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

#include "stdafx.h"
#include "SVValueObject.h"

//******************************************************************************
//* DEFINITIONS OF MODULE-LOCAL VARIABLES:
//******************************************************************************

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

/*
namespace	// only for this file
{
	const CString DEFAULT_TAG_SAVE(_T(".Default"));
	const CString BUCKET_TAG_SAVE(_T(".Array"));	// for backwards compatibility
	const CString ARRAY_TAG_SAVE(_T(".Array_Elements"));	// new style; one bucket, all array values

	const CString DEFAULT_TAG_LOAD(_T("Default"));
	const CString BUCKET_TAG_LOAD(_T("Array"));	// for backwards compatibility
	const CString ARRAY_TAG_LOAD(_T("Array_Elements"));	// new style; one bucket, all array values
}	// end file scope namespace
//*/

//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVValueObjectClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

//SV_IMPLEMENT_CLASS(SVValueObjectClass, SVValueObjectClassGuid);


SVValueObjectClass::SVValueObjectClass( LPCSTR ObjectName )
				   :SVObjectAppClass(ObjectName) 
{
	Initialize();
}


SVValueObjectClass::SVValueObjectClass(SVObjectClass* POwner, int StringResourceID)
				   :SVObjectAppClass(POwner, StringResourceID) 
{
	Initialize();
}


SVValueObjectClass::~SVValueObjectClass()
{
	SetObjectDepth(-1);
	if ( m_pBuckets )
		delete m_pBuckets;
}

void SVValueObjectClass::Initialize()
{
	m_iLastSetIndex = -1;
	m_iNumberOfBuckets = 0;
	m_iArraySize = 1;
	m_pBuckets = NULL;
	m_bLegacyVectorObjectCompatibility = false;
	m_bResetAlways = false;
	m_eResetItem = SVResetItemIP;

	ObjectAttributesAllowedRef() = SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES;// | SV_CH_VALUE | SV_CH_CONDITIONAL;	// These are included in the DEFAULT
}

void SVValueObjectClass::InitializeBuckets()
{
	objectDepth = 2;
	m_iArraySize = 1;

	CreateBuckets();

	if( 1 < m_iNumberOfBuckets )
	{
		m_iLastSetIndex = 1;
	}
}

SVHRESULT SVValueObjectClass::CreateBuckets( )
{
	m_aiResultSize.resize(m_iNumberOfBuckets);
	std::fill(m_aiResultSize.begin(), m_aiResultSize.end(), m_iArraySize);

	return S_OK;
}

BOOL SVValueObjectClass::CreateObject( SVObjectLevelCreateStruct* pCreateStructure )
{
	BOOL l_bOk = SVObjectAppClass::CreateObject( pCreateStructure );
	
	ObjectAttributesAllowedRef() |= SV_CH_VALUE | SV_CH_CONDITIONAL | SV_DD_VALUE;	// derived classes need to reset this

	if ( m_iNumberOfBuckets < 2 )
	{
		l_bOk = l_bOk && SetObjectDepth( 2 );
	}

	l_bOk = l_bOk && (CreateBuckets() == S_OK);

	if( l_bOk )
	{
		m_iLastSetIndex = 1;
	}

	return l_bOk;
}

HRESULT SVValueObjectClass::ResetObject()
{
	HRESULT l_hrOk = CreateBuckets();

	HRESULT hr = SVObjectAppClass::ResetObject();
	if ( l_hrOk == S_OK )
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

SVHRESULT SVValueObjectClass::SetObjectValue(SVObjectAttributeClass* pDataObject)
{
	return SVObjectAppClass::SetObjectValue(pDataObject);
}

void SVValueObjectClass::GetObjectScript(CString& rstrScript, CString& rstrAliasTable, int iIndent)
{
	// Get the Heading (Class Info)
	SVObjectAppClass::GetObjectScript(rstrScript, rstrAliasTable, iIndent);
}

DWORD SVValueObjectClass::processMessage(DWORD DwMessageID, DWORD DwMessageValue, DWORD DwMessageContext)
{
	DWORD DwResult = SVMR_NOT_PROCESSED;
	
	// Try to process message by yourself...
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;
	
	switch (dwPureMessageID)
	{
		case SVMSGID_RESET_ALL_OBJECTS:
		{
			if ( ResetObject() != S_OK )
			{
				ASSERT(FALSE);
				DwResult = SVMR_NO_SUCCESS;
			}
			else
			{
				DwResult = SVMR_SUCCESS;
			}
			break;
		}

		case SVMSGID_COPY_RESULT_DATA_FORWARD:
			HRESULT hrRet = CopyLastSetValue( DwMessageValue );

			return (hrRet == S_OK) ? SVMR_SUCCESS : SVMR_NO_SUCCESS;
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

SVHRESULT SVValueObjectClass::SetArraySize(int iSize)
{
	SVHRESULT hr = S_OK;

	m_iArraySize = max(iSize,1);	// minimum one array element

	// default number of results == array size
	m_aiResultSize.resize(m_iNumberOfBuckets);
	std::fill(m_aiResultSize.begin(), m_aiResultSize.end(), iSize);	// allow the result size to be 0

	m_auObjectAttributesSet.resize( m_iArraySize, m_uDefaultObjectAttributesSet );

	return hr;
}

SVHRESULT SVValueObjectClass::GetResultSize(int iBucket, int& riResultSize) const
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
	int iResultSize=0;
	GetResultSize(m_iLastSetIndex, iResultSize);
	return iResultSize;
}

// protected, only to be used by derived classes
int SVValueObjectClass::GetResultSize(int iBucket) const
{
	return m_aiResultSize[iBucket];
}

SVHRESULT SVValueObjectClass::SetResultSize(int iBucket, int riResultSize)
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

SVHRESULT SVValueObjectClass::CompareWithCurrentValueImpl( const CString& rstrCompare ) const
{
	// default implementation
	SVHRESULT hr = S_FALSE;
	CString strCurrentValue;
	SVHRESULT hrGet = GetValue( strCurrentValue );
	if ( hrGet == S_OK )
	{
		hr = ( (strCurrentValue == rstrCompare) ? S_OK : S_FALSE );
	}
	return hr;
}

SVHRESULT SVValueObjectClass::GetValue( int iBucket, int iIndex, SVScalarValue& rValue ) const
{
	rValue.strName = GetCompleteObjectName();
	//rValue.strType = ;
	SVHRESULT hr = GetValue( iBucket, iIndex, rValue.strValue );

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

