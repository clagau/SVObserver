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
#include "SVStatusLibrary/MessageManager.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
				_variant_t l_Name = l_Enums[i].first.c_str();
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

HRESULT SVEnumerateValueObjectClass::SetObjectValue( SVObjectAttributeClass* pDataObject )
{
	HRESULT hr = S_FALSE;
	BOOL bOk = FALSE;
	
	SvCl::SVObjectSVStringArrayClass SVStringArray;
	SvCl::SVObjectLongArrayClass svLongArray;
	bucket_type l_Buckets(BucketNoAssert());

	if ( bOk = pDataObject->GetAttributeData( _T("Enumeration"), SVStringArray ) )
	{
		for( int i = 0; i < SVStringArray.GetSize(); i++ )
		{
			SetEnumTypes( SVStringArray[i].c_str() );
		}
	}
	else if ( bOk = pDataObject->GetAttributeData( _T("lDefault"), svLongArray ) )
	{
		for( int i = 0; i < svLongArray.GetSize(); i++ )
		{
			DefaultValue() = svLongArray[i];
		}
	}
	else if ( bOk = pDataObject->GetAttributeData( _T("pLArray"), l_Buckets, DefaultValue() ) )
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

HRESULT SVEnumerateValueObjectClass::SetValueAt( int iBucket, int iIndex, const SVString& rValue )
{
	long lValue = 0;

	if (GetEnumerator( rValue.c_str(), lValue ))
	{
		return base::SetValueAt( iBucket, iIndex, lValue );
	}
	return S_FALSE;
}

HRESULT SVEnumerateValueObjectClass::SetValueAt( int iBucket, int iIndex, double value )
{
	return SetValueAt(iBucket, iIndex, static_cast<int> (value) );
}

HRESULT SVEnumerateValueObjectClass::SetDefaultValue( LPCTSTR rValue, bool bResetAll )
{
	long lValue = 0;
	GetEnumerator( rValue, lValue );
	return base::SetDefaultValue( lValue, bResetAll );
}

HRESULT SVEnumerateValueObjectClass::GetValueAt( int iBucket, int iIndex, DWORD& rValue ) const
{
	long value=0;

	HRESULT hr = base::GetValueAt(iBucket, iIndex, value);
	rValue = value;
	
	return hr;
}

HRESULT SVEnumerateValueObjectClass::GetValueAt( int iBucket, int iIndex, SVString& rValue ) const
{
	long lValue=0;
	HRESULT hr = base::GetValueAt( iBucket, iIndex, lValue );
	//if ( S_OK == hr ) // @WARNING - log an error here ?
	{
		GetEnumeratorName( lValue, rValue );
	}
	return hr;
}

HRESULT SVEnumerateValueObjectClass::GetValueAt( int iBucket, int iIndex, double& rValue ) const
{
	long lValue=0;
	HRESULT hr = base::GetValueAt( iBucket, iIndex, lValue );
	//if ( S_OK == hr ) // @WARNING - log an error here ?
	{
		rValue = ( double ) lValue;
	}
	return hr;
}

HRESULT SVEnumerateValueObjectClass::GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const
{
	SVString sVal;
	_variant_t l_Temp;
	l_Temp.Attach( rValue );
	HRESULT hr = GetValueAt( iBucket, iIndex, sVal );
	if( S_OK == hr )
	{
		l_Temp = static_cast< LPCTSTR >( sVal.c_str() );
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
		SVStringVector msgList;
		msgList.push_back( rValue );
		msgList.push_back(GetName());
		SvStl::MessageMgrStd Exception( SvStl::LogOnly );
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
				SVString EnumString  = szEnumToken;
				size_t Pos = EnumString.find( TCHAR( '=' ) );
				if( SVString::npos != Pos )
				{
					// Extract Value...
					enumValue = atol( SvUl_SF::Mid( EnumString, Pos + 1 ).c_str() );
					// Extract Identifier...
					SVString strIdent = SvUl_SF::Left( EnumString, Pos );
					EnumString = strIdent;
				}
				
				// Trim identifier..
				EnumString = SvUl_SF::TrimLeft( EnumString );
				EnumString = SvUl_SF::TrimRight( EnumString );

				// Check if identifier is valid and unique...
				long lDummy = 0L;
				if ( !EnumString.empty() && !GetEnumerator( EnumString.c_str(), lDummy ) )	// If this fails enumerator was unknown!
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
					m_enumStringTable.InsertAt( i, EnumString );
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
		SVString sFirst = rVec[i].first;
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
	SVString EnumList;
	EnumList = SvUl_SF::LoadSVString( StringResourceID );
	return SetEnumTypes( EnumList.c_str() );
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
			if( 0 == SvUl_SF::CompareNoCase( m_enumStringTable.GetAt( i ), szEnumerator ) )
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
		list.push_back(std::make_pair(m_enumStringTable.GetAt(i).c_str(), m_enumValueTable.GetAt(i)));
	}
	return list;
}
#pragma endregion IVEnumerateValueObject

/*static*/ bool SVEnumerateValueObjectClass::ToNumber(const SVString& rString, long& rValue)
{
	bool bConverted = false;

	SVString strTemp( rString );
	SvUl_SF::MakeLower( strTemp );
	SVString Digits = SvUl_SF::ValidateString( rString, _T("0123456789-. xabcdef") );
	if ( (strTemp == Digits) && SVString::npos != Digits.find_first_of( _T("0123456789") ) )	// MUST have at least one digit and no non-hex alphabetic stuff or other spurrious chars
	{
		if ( SVString::npos != Digits.find( 'x' ) )	// HEX
		{
			TCHAR* p = nullptr;
			rValue = _tcstol(Digits.c_str(), &p, 16);	// base 16
			bConverted = true;
		}
		else	// assume base 10
		{
			SVString Decimal = SvUl_SF::ValidateString( Digits, _T("0123456789-. ") );
			if ( Decimal == Digits )	// if no abcdef
			{
				TCHAR* p = nullptr;
				rValue = _tcstol(Decimal.c_str(), &p, 10);	// base 10
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
BOOL SVEnumerateValueObjectClass::GetEnumeratorName( long lValue, SVString& rEnumerator ) const
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
			rEnumerator = SvUl_SF::Format( _T( "%d" ), lValue );
		}
	}
	return bRetVal;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : GetEnumTypes
// -----------------------------------------------------------------------------
// .Description : Returns a string with all defined enumerations and their values
////////////////////////////////////////////////////////////////////////////////
BOOL SVEnumerateValueObjectClass::GetEnumTypes( SVString& rEnumList ) const
{
	BOOL bRetVal = TRUE;
	// Get Enumeration types...
	for( int i = 0; i < m_enumStringTable.GetSize(); ++ i )
	{
		long lEnumValue = m_enumValueTable[ i ];
		SVString tmp = SvUl_SF::Format( "=%d", lEnumValue );

		if( i )
		{
			// Add comma, but after first element...
			rEnumList += _T( "," );
		}

		rEnumList += m_enumStringTable.GetAt( i ) + tmp;
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
BOOL SVEnumerateValueObjectClass::GetNextEnumType( int& RIterator, SVString& RStrEnum, long& REnumValue ) const
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

HRESULT SVEnumerateValueObjectClass::CompareWithCurrentValueImpl( const SVString& rCompare ) const
{
	HRESULT hr = S_FALSE;

	long lEnumValue = 0;
	BOOL bGetEnum = GetEnumerator( rCompare.c_str(), lEnumValue );
	if ( bGetEnum )
	{
		long lCurrentValue;
		SVString strCurrentValue;
		HRESULT hrGet = GetValue( strCurrentValue );
		BOOL bGetCurrentEnum = GetEnumerator( strCurrentValue.c_str(), lCurrentValue );
		if ( bGetCurrentEnum )
		{
			hr = ( (lEnumValue == lCurrentValue) ? S_OK : S_FALSE );
		}
	}

	return hr;
}

HRESULT SVEnumerateValueObjectClass::GetNormalizedValueImpl( const SVString& strValue, SVString& rstrNormalized ) const
{
	return S_FALSE;
}

void SVEnumerateValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVEnumValueObjectType;

	SetTypeName( _T("Enumeration") );

	InitializeBuckets();
}

