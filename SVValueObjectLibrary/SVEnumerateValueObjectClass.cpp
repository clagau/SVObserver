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
: SVValueObjectClass<long>( ObjectName )
{
	LocalInitialize();
}

SVEnumerateValueObjectClass::SVEnumerateValueObjectClass( SVObjectClass* pOwner, int StringResourceID )
: SVValueObjectClass<long>( pOwner, StringResourceID )
{
	LocalInitialize();
}
 
const SVEnumerateValueObjectClass& SVEnumerateValueObjectClass::operator = (const SVEnumerateValueObjectClass& rhs)
{
	SVValueObjectClass<long>::operator = (rhs);
	return *this;
}

SVEnumerateValueObjectClass::~SVEnumerateValueObjectClass()
{
}

HRESULT SVEnumerateValueObjectClass::SetObjectValue( SVObjectAttributeClass* pDataObject )
{
	HRESULT Result( E_FAIL );
	bool bOk( false );
	
	SvCl::SVObjectSVStringArrayClass SVStringArray;

	if ( bOk = pDataObject->GetAttributeData( _T("Enumeration"), SVStringArray ) )
	{
		for( int i = 0; i < SVStringArray.GetSize(); i++ )
		{
			SetEnumTypes( SVStringArray[i].c_str() );
		}
	}
	else
	{
		Result = __super::SetObjectValue( pDataObject );
		return Result;
	}

	Result = bOk ? S_OK : E_FAIL;
	return Result;
}

HRESULT SVEnumerateValueObjectClass::SetDefaultValue( LPCTSTR Value, bool bResetAll )
{
	long lValue = 0;
	GetEnumerator( Value, lValue );
	return __super::SetDefaultValue( lValue, bResetAll );
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
bool SVEnumerateValueObjectClass::SetEnumTypes( LPCTSTR szEnumList )
{
	bool bRetVal = false;
	if( szEnumList )
	{
		TCHAR* szList = _tcsdup( szEnumList );
		if( szList )
		{
			TCHAR  sep[] = _T( "," );
			TCHAR* szEnumToken;
			long enumValue = 0L;

			bRetVal = true;

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
					bRetVal = false;
				}

			} while( szEnumToken = _tcstok( nullptr, sep ) );

			// Free allocated resources...
			free( szList );
		}
	}
	return bRetVal;
}

bool SVEnumerateValueObjectClass::SetEnumTypes( const SVEnumerateVector& rVec )
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

bool SVEnumerateValueObjectClass::GetEnumTypes( SVEnumerateVector& rVec ) const
{
	rVec.clear();

	for ( int i=0; i < m_enumStringTable.GetSize(); i++ )
	{
		rVec.push_back( SVEnumeratePair( m_enumStringTable.GetAt(i), m_enumValueTable.GetAt(i) ) );
	}
	return true;
}

bool SVEnumerateValueObjectClass::SetEnumTypes( int StringResourceID )
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
//				: false.
////////////////////////////////////////////////////////////////////////////////
bool SVEnumerateValueObjectClass::GetEnumerator( LPCTSTR szEnumerator, long& lValue ) const
{
	bool bRetVal = false;
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
//				: returns false.
////////////////////////////////////////////////////////////////////////////////
bool SVEnumerateValueObjectClass::GetEnumeratorName( long lValue, SVString& rEnumerator ) const
{
	bool bRetVal = false;
	// Check if enumerator is defined...
	for( int i = 0; i < m_enumValueTable.GetSize(); ++ i )
	{
		long lEnumVal = m_enumValueTable[ i ];
		if( lEnumVal == lValue )
		{
			// Found it...
			rEnumerator = m_enumStringTable.GetAt( i );
			bRetVal = true;
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
bool SVEnumerateValueObjectClass::GetEnumTypes( SVString& rEnumList ) const
{
	bool bRetVal = true;
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
//				: If there is no next enumerator it fails and returns false.
//				:
//				: Use Iterator which is returned by GetFirstEnumType(...),
//				:	to iterate through list of enum types.
////////////////////////////////////////////////////////////////////////////////
bool SVEnumerateValueObjectClass::GetNextEnumType( int& RIterator, SVString& RStrEnum, long& REnumValue ) const
{
	bool bRetVal = false;
	if( RIterator >= 0 && RIterator < m_enumStringTable.GetSize() )
	{
		RStrEnum	= m_enumStringTable.GetAt( RIterator );
		REnumValue	= m_enumValueTable.GetAt( RIterator );

		// Prepare next iterator...
		++RIterator;
		bRetVal = true;
	}
	return bRetVal;
}

HRESULT SVEnumerateValueObjectClass::GetVariantValue(_variant_t& rValue, int Index, int Bucket) const
{
	long Value;
	HRESULT Result = GetValue(Value, Index, Bucket);
	if( S_OK == Result )
	{
		rValue.SetString( ConvertType2String( Value ).c_str() );
	}
	else
	{
		rValue.Clear();
	}

	return Result;
}

long SVEnumerateValueObjectClass::ConvertString2Type( const SVString& rValue ) const
{
	long Result( 0L );

	if (!GetEnumerator( rValue.c_str(), Result ))
	{
		SVStringVector msgList;
		msgList.push_back( rValue );
		msgList.push_back(GetName());
		SvStl::MessageMgrStd Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ValueObject_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
		Exception.Throw();
	}
	return Result;
}

SVString SVEnumerateValueObjectClass::ConvertType2String( const long& rValue ) const
{
	SVString Result;

	GetEnumeratorName( rValue, Result );

	return Result;
}

void SVEnumerateValueObjectClass::WriteValues(SVObjectWriter& rWriter)
{
	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	SVVariantList list;

	// for all elements in the array
	for (int i = 0; i < getArraySize(); i++)
	{
		//Make sure this is not a derived virtual method which is called
		_variant_t Value;
		Value.ChangeType(VT_I4);
		SVEnumerateValueObjectClass::GetValue(Value.lVal, i);
		list.push_back(Value);
	}
	rWriter.WriteAttribute(scElementTag, list);
}

void SVEnumerateValueObjectClass::WriteDefaultValues(SVObjectWriter& rWriter)
{
	_variant_t Value(GetDefaultValue());
	rWriter.WriteAttribute(scDefaultTag, Value);
}

void SVEnumerateValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SVEnumValueObjectType;

	SetTypeName( _T("Enumeration") );

	InitializeBuckets();
}

