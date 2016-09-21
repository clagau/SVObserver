//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEnumerateValueObjectClass
//* .File Name       : $Workfile:   SVEnumerateValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   01 Oct 2013 14:12:24  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVEnumerateValueObjectClass.h"
#include "SVObjectLibrary\SVClsids.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVOMFCLibrary/StringMunge.h"
#include "SVStatusLibrary/MessageManager.h"
#pragma endregion Includes

namespace	// only for this file
{
	const CString DEFAULT_TAG_SAVE(_T(".Default"));
	const CString BUCKET_TAG_SAVE(_T(".Array"));	// for backwards compatibility
	const CString ARRAY_TAG_SAVE(_T(".Array_Elements"));	// new style; one bucket, all array values

	const CString DEFAULT_TAG_LOAD(_T("Default"));
	const CString BUCKET_TAG_LOAD(_T("Array"));	// for backwards compatibility
	const CString ARRAY_TAG_LOAD(_T("Array_Elements"));	// new style; one bucket, all array values
}	// end file scope namespace

SV_IMPLEMENT_CLASS( SVEnumerateValueObjectClass, SVEnumerateValueObjectClassGuid );

SVEnumerateValueObjectClass::SVEnumerateValueObjectClass( LPCTSTR ObjectName )
							              : base( ObjectName ) 
{
	LocalInitialize();
}

SVEnumerateValueObjectClass::SVEnumerateValueObjectClass( SVObjectClass* POwner, int StringResourceID )
							: base( POwner, StringResourceID ) 
{
	LocalInitialize();
}
 
const SVEnumerateValueObjectClass& SVEnumerateValueObjectClass::operator = (const SVEnumerateValueObjectClass& rhs)
{
	base::operator = (rhs);
	return *this;
}

SVEnumerateValueObjectClass::~SVEnumerateValueObjectClass()
{
	// Delete all object's data arrays...
	SetObjectDepth( 0 );
	ResetObject();
}

BOOL SVEnumerateValueObjectClass::CreateObject( SVObjectLevelCreateStruct* pCreateStructure )
{
	BOOL bCreate = base::CreateObject( pCreateStructure );
	return bCreate;
}

void SVEnumerateValueObjectClass::Persist(SVObjectWriter& rWriter)
{
	rWriter.StartElement(GetObjectName()); // use internal name for node name

	// Get the Heading (Class Info)
	SVValueObjectClass::Persist( rWriter );

	// Get the Data Values (Member Info, Values)
	_variant_t value(DefaultValue());
	
	rWriter.WriteAttribute(scDefaultTag, value);

	rWriter.StartElement(scArrayElementsTag);

	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	SVVariantList list;	

	// for all elements in the array (m_iArraySize)
	for( int i = 0; i < m_iArraySize; i++ )
	{
		value.lVal = Element(m_iLastSetIndex, i);
		list.push_back(value);
	}
	rWriter.WriteAttribute(scElementTag, list);
	rWriter.EndElement();

	rWriter.EndElement();
}

HRESULT SVEnumerateValueObjectClass::GetObjectValue( const SVString& p_rValueName, VARIANT& p_rVariantValue ) const
{
	HRESULT hr = S_OK;

	if( p_rValueName == _T( "Enumeration" ) )
	{
		_variant_t l_TempVariant;

		l_TempVariant.Attach( p_rVariantValue );

		SVEnumerateVector l_Enums;

		GetEnumTypes( l_Enums );

		if( !( l_Enums.empty() ) )
		{
			SVSAFEARRAY::SVBounds l_Bounds;

			l_Bounds.resize( 2 );

			// First Dimension number of objects in list..
			l_Bounds[ 0 ].lLbound = 0;
			l_Bounds[ 0 ].cElements = static_cast< ULONG >( l_Enums.size() );

			// Second Dimension is the Enumeration Value
			// 0 = Name, 1 = Enum Value
			l_Bounds[ 1 ].lLbound = 0;
			l_Bounds[ 1 ].cElements = 2;

			size_t i = 0;
			SVSAFEARRAY l_Temp( VT_VARIANT, l_Bounds );
			SVSAFEARRAY::SVIndex l_Index( 2 );

			for( int i = 0; i < static_cast< int >( l_Enums.size() ); ++i )
			{
				_variant_t l_Name = l_Enums[i].first;
				_variant_t l_Value = l_Enums[i].second;

				l_Index[ 0 ] = i;
				l_Index[ 1 ] = 0;
				
				l_Temp.PutElement( l_Index, l_Name );

				l_Index[ 0 ] = i;
				l_Index[ 1 ] = 1;
				
				l_Temp.PutElement( l_Index, l_Value );
			}

			l_TempVariant = l_Temp;
		}
		else
		{
			l_TempVariant.Clear();
		}

		l_TempVariant.Detach();
	}
	else
	{
		hr = base::GetObjectValue( p_rValueName, p_rVariantValue );
	}

	return hr;
}

HRESULT SVEnumerateValueObjectClass::SetObjectValue( const SVString& p_rValueName, const _variant_t& p_rVariantValue )
{
	HRESULT hr = S_OK;

	if( p_rValueName == _T( "Enumeration" ) )
	{
		if( ( p_rVariantValue.vt & VT_ARRAY ) == VT_ARRAY )
		{
			SVSAFEARRAY l_SafeArray( p_rVariantValue );

			if( l_SafeArray.GetDim() == 2 )
			{
				SVEnumerateVector l_Enums;

				l_Enums.resize( l_SafeArray.size() );

				for( int i = 0; i < static_cast< int >( l_SafeArray.size() ); i++ )
				{
					SVSAFEARRAY::SVIndex l_Index( 2 );
					_variant_t l_Value;

					l_Index[ 0 ] = i;
					l_Index[ 1 ] = 0;

					if( S_OK == l_SafeArray.GetElement( l_Index, l_Value ) && VT_BSTR == l_Value.vt )
					{
						l_Enums[ i ].first = static_cast< LPCTSTR >( _bstr_t( l_Value ) );
					}

					l_Index[ 0 ] = i;
					l_Index[ 1 ] = 1;

					if( S_OK == l_SafeArray.GetElement( l_Index, l_Value ) )
					{
						l_Enums[ i ].second = l_Value;
					}
					else
					{
						l_Enums[ i ].second = 0;
					}
				}

				SetEnumTypes( l_Enums );
			}
			else
			{
				hr = S_FALSE;
			}
		}
		else
		{
			hr = S_FALSE;
		}
	}
	else
	{
		hr = base::SetObjectValue( p_rValueName, p_rVariantValue );
	}

	return hr;
}

HRESULT SVEnumerateValueObjectClass::SetObjectValue( SVObjectAttributeClass* pDataObject )
{
	HRESULT hr = S_FALSE;
	BOOL bOk = FALSE;
	
	SvCl::SVObjectCStringArrayClass svCStringArray;
	SvCl::SVObjectLongArrayClass svLongArray;
	bucket_type l_Buckets(BucketsNoAssert());

	if ( bOk = pDataObject->GetAttributeData( "Enumeration", svCStringArray ) )
	{
		for( int i = 0; i < svCStringArray.GetSize(); i++ )
		{
			SetEnumTypes( svCStringArray[i] );
		}
	}
	else if ( bOk = pDataObject->GetAttributeData( "lDefault", svLongArray ) )
	{
		for( int i = 0; i < svLongArray.GetSize(); i++ )
		{
			DefaultValue() = svLongArray[i];
		}
	}
	else if ( bOk = pDataObject->GetAttributeData( "pLArray", l_Buckets, DefaultValue() ) )
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
		hr = base::SetObjectValue( pDataObject );
		return hr;
	}

	hr = bOk ? S_OK : S_FALSE;
	return hr;
}


HRESULT SVEnumerateValueObjectClass::SetValueAt( int iBucket, int iIndex, const DWORD dwValue )
{
	return base::SetValueAt( iBucket, iIndex, (long) dwValue );
}

HRESULT SVEnumerateValueObjectClass::SetValueAt( int iBucket, int iIndex, const int iValue )
{
	return base::SetValueAt( iBucket, iIndex, (long) iValue );
}

HRESULT SVEnumerateValueObjectClass::SetValueAt( int iBucket, int iIndex, const CString& strValue )
{
	long lValue = 0;

	if (GetEnumerator( strValue, lValue ))
	{
		return base::SetValueAt( iBucket, iIndex, lValue );
	}
	return S_FALSE;
}

HRESULT SVEnumerateValueObjectClass::SetValueAt( int iBucket, int iIndex, double value )
{
	return SetValueAt(iBucket, iIndex, static_cast <int> (value) );
}

HRESULT SVEnumerateValueObjectClass::SetDefaultValue( LPCTSTR StrValue, bool bResetAll )
{
	long lValue = 0;
	GetEnumerator( StrValue, lValue );
	return base::SetDefaultValue( lValue, bResetAll );
}

HRESULT SVEnumerateValueObjectClass::GetValueAt( int iBucket, int iIndex, DWORD& rdwValue ) const
{
	long value=0;

	HRESULT hr = base::GetValueAt(iBucket, iIndex, value);
	rdwValue = value;
	
	return hr;
}

HRESULT SVEnumerateValueObjectClass::GetValueAt( int iBucket, int iIndex, CString& rstrValue ) const
{
	long lValue=0;
	HRESULT hr = base::GetValueAt( iBucket, iIndex, lValue );
	//if ( S_OK == hr ) // @WARNING - log an error here ?
	{
		GetEnumeratorName( lValue, rstrValue );
	}
	return hr;
}

HRESULT SVEnumerateValueObjectClass::GetValueAt( int iBucket, int iIndex, double& rdValue ) const
{
	long lValue=0;
	HRESULT hr = base::GetValueAt( iBucket, iIndex, lValue );
	//if ( S_OK == hr ) // @WARNING - log an error here ?
	{
		rdValue = ( double ) lValue;
	}
	return hr;
}

HRESULT SVEnumerateValueObjectClass::GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const
{
	CString sVal;
	_variant_t l_Temp;
	l_Temp.Attach( rValue );
	HRESULT hr = GetValueAt( iBucket, iIndex, sVal );
	if( S_OK == hr )
	{
		l_Temp = static_cast< LPCTSTR >( sVal );
	}
	else
	{
		l_Temp.Clear();
	}
	rValue = l_Temp.Detach();

	return hr;
}

void SVEnumerateValueObjectClass::ValidateValue( int iBucket, int iIndex, const SVString& rValue ) const
{
	long lValue = 0;
	if (!GetEnumerator( rValue.c_str(), lValue ))
	{
		SVStringArray msgList;
		msgList.push_back(SVString(rValue));
		msgList.push_back(GetName());
		SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_ValueObject_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
		Exception.Throw();
	}
	base::ValidateValue( iBucket, iIndex, rValue );
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : SetEnumTypes
// -----------------------------------------------------------------------------
// .Description : Pass Enumeration String e.g. "NULL=0,ONE=1,BigNumber=99999"
//				: Use 'comma' sign as separator and 'assign' sign to set a 
//				: special value. If no value is defined for an enumeration
//				: standard enumeration defines an incremental number.
//				: ( e.g. "NULL,ONE,TWO,BigNumber=99999,BiggerNumber" ) 
////////////////////////////////////////////////////////////////////////////////
BOOL SVEnumerateValueObjectClass::SetEnumTypes( LPCTSTR szEnumList )
{
	BOOL bRetVal = FALSE;
	if( szEnumList )
	{
		TCHAR* szList = _tcsdup( szEnumList );
		if( szList )
		{
			TCHAR  sep[] = _T( "," );
			TCHAR* szEnumToken;
			long enumValue = 0L;

			bRetVal = TRUE;

			// Extract enumerations...
			szEnumToken = _tcstok( szList, sep );   
			do   
			{
				// Get Enum Identifier...
				CString strEnum  = szEnumToken;
				int ind = strEnum.Find( TCHAR( '=' ) );
				if( ind >= 0 )
				{
					// Extract Value...
					enumValue = atol( strEnum.Mid( ind + 1 ) );
					// Extract Identifier...
					CString strIdent = strEnum.Left( ind );
					strEnum = strIdent;
				}
				
				// Trim identifier..
				strEnum.TrimLeft();
				strEnum.TrimRight();

				// Check if identifier is valid and unique...
				long lDummy = 0L;
				if ( ! strEnum.IsEmpty() && ! GetEnumerator( strEnum, lDummy ) )	// If this fails enumerator was unknown!
				{
					int i = 0;
					// Get position in tables ( sorted by value )
					for(; i < m_enumValueTable.GetSize(); ++i )
					{
						long lValue = m_enumValueTable[ i ];
						if( lValue > enumValue )
							break;
					}
					// Insert enumeration at found position...
					m_enumStringTable.InsertAt( i, strEnum );
					m_enumValueTable.InsertAt( i, enumValue );

					// Prepare next enumeration value...
					++enumValue;
				}
				else
				{
					// Something was wrong...
					bRetVal = FALSE;
				}

			} while( szEnumToken = _tcstok( nullptr, sep ) );

			// Free allocated resources...
			free( szList );
		}
	}
	return bRetVal;
}

BOOL SVEnumerateValueObjectClass::SetEnumTypes( const SVEnumerateVector& rVec )
{
	for ( size_t i=0; i < rVec.size(); i++ )
	{
		CString sFirst = rVec[i].first;
		long lSecond = rVec[i].second;
		m_enumStringTable.Add( sFirst );
		m_enumValueTable.Add( lSecond );
	}
	return true;
}

BOOL SVEnumerateValueObjectClass::GetEnumTypes( SVEnumerateVector& rVec ) const
{
	rVec.clear();

	for ( int i=0; i < m_enumStringTable.GetSize(); i++ )
	{
		rVec.push_back( SVEnumeratePair( m_enumStringTable.GetAt(i), m_enumValueTable.GetAt(i) ) );
	}
	return true;
}

BOOL SVEnumerateValueObjectClass::SetEnumTypes( int StringResourceID )
{
	CString strEnumList;
	strEnumList.LoadString( StringResourceID );
	return SetEnumTypes( strEnumList );
}


////////////////////////////////////////////////////////////////////////////////
// .Title       : GetEnumerator
// -----------------------------------------------------------------------------
// .Description : Returns in lValue the value of a given Enumerator defined by 
//				: szEnumerator.
//				: If Enumerator is not defined, the function fails and returns
//				: FALSE.
////////////////////////////////////////////////////////////////////////////////
BOOL SVEnumerateValueObjectClass::GetEnumerator( LPCTSTR szEnumerator, long& lValue ) const
{
	BOOL bRetVal = false;
	if( szEnumerator )
	{
		// Check if enumerator is defined...
		for( int i = 0; i < m_enumStringTable.GetSize(); ++ i )
		{
			if( 0 == m_enumStringTable.GetAt( i ).CompareNoCase( szEnumerator ) )
			{
				// Found it...
				lValue = m_enumValueTable[ i ];
				bRetVal = true;
			}
		}

		if ( !bRetVal )	// look through values
		{
			long lEnumeratorValue = -98765432;
			if ( ToNumber( szEnumerator, lEnumeratorValue ) )
			{
				for( int i = 0; i < m_enumValueTable.GetSize(); ++i )
				{
					if ( m_enumValueTable.GetAt( i ) == lEnumeratorValue )
					{
						// Found it...
						lValue = lEnumeratorValue;
						bRetVal = true;
						break;
					}
				}
			}
		}
	}// end if( szEnumerator )

	return bRetVal;
}

#pragma region IVEnumerateValueObject
SvOi::NameValueList SVEnumerateValueObjectClass::GetEnumList() const
{
	SvOi::NameValueList list;
	for (int i = 0;i < m_enumStringTable.GetSize(); i++)
	{
		list.push_back(std::make_pair(m_enumStringTable.GetAt(i).GetString(), m_enumValueTable.GetAt(i)));
	}
	return list;
}
#pragma endregion IVEnumerateValueObject

/*static*/ bool SVEnumerateValueObjectClass::ToNumber(const CString& str, long& p_rlValue)
{
	bool bConverted = false;

	CString strTemp( str );
	strTemp.MakeLower();
	CString strMunge( strTemp );
	StringMunge::KeepChars( &strMunge, _T("0123456789-. xabcdef") );
	if ( (strTemp == strMunge) && strMunge.FindOneOf( _T("0123456789") ) != -1 )	// MUST have at least one digit and no non-hex alphabetic stuff or other spurrious chars
	{
		if ( strMunge.Find( _T('x') ) != -1 )	// HEX
		{
			TCHAR* p = nullptr;
			p_rlValue = _tcstol(strMunge, &p, 16);	// base 16
			bConverted = true;
		}
		else	// assume base 10
		{
			CString strDec( strMunge );
			StringMunge::KeepChars( &strDec, _T("0123456789-. ") );
			if ( strDec == strMunge )	// if no abcdef
			{
				TCHAR* p = nullptr;
				p_rlValue = _tcstol(strDec, &p, 10);	// base 10
				bConverted = true;
			}
		}
	}
	return bConverted;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : GetEnumeratorName
// -----------------------------------------------------------------------------
// .Description : Returns in rEnumerator the name of a given value defined by 
//				: lValue.
//				: If Enumerator is not defined for this value, the function 
//				: returns in rEnumerator the value converted to a String and
//				: returns FALSE.
////////////////////////////////////////////////////////////////////////////////
BOOL SVEnumerateValueObjectClass::GetEnumeratorName( long lValue, CString& rEnumerator ) const
{
	BOOL bRetVal = FALSE;
	// Check if enumerator is defined...
	for( int i = 0; i < m_enumValueTable.GetSize(); ++ i )
	{
		long lEnumVal = m_enumValueTable[ i ];
		if( lEnumVal == lValue )
		{
			// Found it...
			rEnumerator = m_enumStringTable.GetAt( i );
			bRetVal = TRUE;
		}

		if( ! bRetVal )
		{
			// Return undefined value as string...
			rEnumerator.Format( _T( "%d" ), lValue );
		}
	}
	return bRetVal;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : GetEnumTypes
// -----------------------------------------------------------------------------
// .Description : Returns a string with all defined enumerations and their values
////////////////////////////////////////////////////////////////////////////////
BOOL SVEnumerateValueObjectClass::GetEnumTypes( CString& rStrEnumList ) const
{
	BOOL bRetVal = TRUE;
	// Get Enumeration types...
	CString tmp;
	for( int i = 0; i < m_enumStringTable.GetSize(); ++ i )
	{
		long lEnumValue = m_enumValueTable[ i ];
		tmp.Format( "=%d", lEnumValue );

		if( i )
		{
			// Add comma, but after first element...
			rStrEnumList += _T( "," );
		}

		rStrEnumList += m_enumStringTable.GetAt( i ) + tmp;
	}
	return bRetVal;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : GetFirstEnumType
// -----------------------------------------------------------------------------
// .Description : Returns Iterator which can be used by GetNextEnumType(...)
//				: to iterate through all defined enumerators.
//				:
//				: The return value is -1, if there is nothing defined or the 
//				: func. failed.
////////////////////////////////////////////////////////////////////////////////
int SVEnumerateValueObjectClass::GetFirstEnumTypePos() const
{
	int iRetVal = -1;
	if( m_enumStringTable.GetSize() > 0 )
	{
		iRetVal = 0;
	}
	return iRetVal;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : GetNextEnumType
// -----------------------------------------------------------------------------
// .Description : Returns in RStrEnum the next defined enumeration identifier
//				: and in REnumValue its value.
//				:
//				: If there is no next enumerator it fails and returns FALSE.
//				:
//				: Use Iterator which is returned by GetFirstEnumType(...),
//				:	to iterate through list of enum types.
////////////////////////////////////////////////////////////////////////////////
BOOL SVEnumerateValueObjectClass::GetNextEnumType( int& RIterator, CString& RStrEnum, long& REnumValue ) const
{
	BOOL bRetVal = FALSE;
	if( RIterator >= 0 && RIterator < m_enumStringTable.GetSize() )
	{
		RStrEnum	= m_enumStringTable.GetAt( RIterator );
		REnumValue	= m_enumValueTable.GetAt( RIterator );

		// Prepare next iterator...
		++RIterator;
		bRetVal = TRUE;
	}
	return bRetVal;
}

HRESULT SVEnumerateValueObjectClass::CompareWithCurrentValueImpl( const CString& rstrCompare ) const
{
	HRESULT hr = S_FALSE;

	long lEnumValue = 0;
	BOOL bGetEnum = GetEnumerator( rstrCompare, lEnumValue );
	if ( bGetEnum )
	{
		long lCurrentValue;
		CString strCurrentValue;
		HRESULT hrGet = GetValue( strCurrentValue );
		BOOL bGetCurrentEnum = GetEnumerator( strCurrentValue, lCurrentValue );
		if ( bGetCurrentEnum )
		{
			hr = ( (lEnumValue == lCurrentValue) ? S_OK : S_FALSE );
		}
	}

	return hr;
}

HRESULT SVEnumerateValueObjectClass::GetNormalizedValueImpl( const CString& strValue, CString& rstrNormalized ) const
{
	return S_FALSE;
}

void SVEnumerateValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVEnumValueObjectType;

	m_strTypeName = "Enumeration";

	InitializeBuckets();
}

