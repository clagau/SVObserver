//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVStatusLibrary/MessageManager.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvVol
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS( SVEnumerateValueObjectClass, SvPb::EnumerateValueClassId);

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
	SvCl::SVObjectStdStringArrayClass SVStringArray;
	bool bOk(pDataObject->GetAttributeData(_T("Enumeration"), SVStringArray));

	if (bOk)
	{
		for( int i = 0; i < static_cast<int> (SVStringArray.size()); i++ )
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
		m_enumVector.clear();
		TCHAR* szList = _tcsdup( szEnumList );
		if( szList )
		{
			TCHAR  sep[] = _T( "," );
			TCHAR* pEnumToken;
			long enumValue = 0L;

			bRetVal = true;

			// Extract enumerations...
			pEnumToken = _tcstok( szList, sep );   
			do   
			{
				// Get Enum Identifier...
				std::string EnumString  = pEnumToken;
				size_t Pos = EnumString.find( TCHAR( '=' ) );
				if( std::string::npos != Pos )
				{
					// Extract Value...
					enumValue = atol( SvUl::Mid( EnumString, Pos + 1 ).c_str() );
					// Extract Identifier...
					std::string strIdent = SvUl::Left( EnumString, Pos );
					EnumString = strIdent;
				}
				
				// Trim identifier..
				EnumString = SvUl::Trim(EnumString.c_str());

				// Check if identifier is valid and unique...
				long lDummy = 0L;
				if ( !EnumString.empty() && !GetEnumerator( EnumString.c_str(), lDummy ) )	// If this fails enumerator was unknown!
				{
					m_enumVector.push_back(SvDef::NameValuePair{ EnumString, enumValue });
					enumValue++;
				}
				else
				{
					// Something was wrong...
					bRetVal = false;
				}

			} while(nullptr != (pEnumToken = _tcstok(nullptr, sep)));

			// Free allocated resources...
			free( szList );
		}

		std::sort(m_enumVector.begin(), m_enumVector.end(), [](const SvDef::NameValuePair& rLhs, const SvDef::NameValuePair& rRhs) -> bool
		{
			return rLhs.second < rRhs.second;
		});
	}
	return bRetVal;
}

bool SVEnumerateValueObjectClass::SetEnumTypes(const SvDef::NameValueVector& rEnumVector)
{
	//! Note the enums are not sorted
	m_enumVector = rEnumVector;
	return true;
}

bool SVEnumerateValueObjectClass::SetEnumTypes( int StringResourceID )
{
	std::string EnumList;
	EnumList = SvUl::LoadStdString( StringResourceID );
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
bool SVEnumerateValueObjectClass::GetEnumerator( LPCTSTR szEnumerator, long& rValue ) const
{
	bool bRetVal = false;
	if( szEnumerator )
	{
		// Check if enumerator is defined...
		for( auto const& rEntry : m_enumVector)
		{
			if( 0 == SvUl::CompareNoCase(rEntry.first, szEnumerator) )
			{
				// Found it...
				rValue = rEntry.second;
				bRetVal = true;
				break;
			}
		}

		if ( !bRetVal )	// look through values
		{
			long lEnumeratorValue = -98765432;
			if ( ToNumber( szEnumerator, lEnumeratorValue ) )
			{
				for (auto const& rEntry : m_enumVector)
				{
					if ( rEntry.second == lEnumeratorValue )
					{
						// Found it...
						rValue = lEnumeratorValue;
						bRetVal = true;
						break;
					}
				}
			}
		}
	}// end if( szEnumerator )

	return bRetVal;
}

/*static*/ bool SVEnumerateValueObjectClass::ToNumber(const std::string& rString, long& rValue)
{
	bool bConverted = false;

	std::string strTemp = SvUl::MakeLower(rString.c_str());
	std::string Digits = SvUl::ValidateString( rString, _T("0123456789-. xabcdef") );
	if ( (strTemp == Digits) && std::string::npos != Digits.find_first_of( _T("0123456789") ) )	// MUST have at least one digit and no non-hex alphabetic stuff or other spurrious chars
	{
		if ( std::string::npos != Digits.find( 'x' ) )	// HEX
		{
			TCHAR* p = nullptr;
			rValue = _tcstol(Digits.c_str(), &p, 16);	// base 16
			bConverted = true;
		}
		else	// assume base 10
		{
			std::string Decimal = SvUl::ValidateString( Digits, _T("0123456789-. ") );
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
bool SVEnumerateValueObjectClass::GetEnumeratorName( long lValue, std::string& rEnumerator ) const
{
	bool Result = false;
	// Check if enumerator is defined...
	for( auto const& rEntry : m_enumVector)
	{
		if( rEntry.second == lValue )
		{
			// Found it...
			rEnumerator = rEntry.first;
			Result = true;
			break;
		}
	}
	if (!Result)
	{
		// Return undefined value as string...
		rEnumerator = std::format(_T("<id={:d}>"), lValue);
	}
	return Result;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : GetEnumTypes
// -----------------------------------------------------------------------------
// .Description : Returns a string with all defined enumerations and their values
////////////////////////////////////////////////////////////////////////////////
bool SVEnumerateValueObjectClass::GetEnumTypes( std::string& rEnumList ) const
{
	bool bRetVal = true;
	// Get Enumeration types...
	rEnumList.clear();
	for (auto const& rEntry : m_enumVector)
	{
		if(!rEnumList.empty())
		{
			// Add comma, but after first element...
			rEnumList += _T(",");
		}

		rEnumList += rEntry.first + std::format("={:d}", rEntry.second );
	}
	return bRetVal;
}

long SVEnumerateValueObjectClass::ConvertString2Type( const std::string& rValue ) const
{
	long Result( 0L );

	if (!GetEnumerator( rValue.c_str(), Result ))
	{
		SvDef::StringVector msgList;
		msgList.push_back( rValue );
		msgList.push_back(GetName());
		SvStl::MessageManager Exception(SvStl::MsgType::Log );
		Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ValueObject_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
		Exception.Throw();
	}
	return Result;
}

std::string SVEnumerateValueObjectClass::ConvertType2String( const long& rValue ) const
{
	std::string Result;

	GetEnumeratorName( rValue, Result );

	return Result;
}

void SVEnumerateValueObjectClass::WriteValues(SvOi::IObjectWriter& rWriter) const
{
	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	std::vector<_variant_t> list;

	// for all elements in the array
	for (int32_t i = 0; i < getArraySize(); i++)
	{
		//Make sure this is not a derived virtual method which is called
		_variant_t Value;
		Value.ChangeType(VT_I4);
		SVEnumerateValueObjectClass::GetValue(Value.lVal, i);
		list.push_back(Value);
	}
	rWriter.WriteAttribute(scElementTag, list);
}

void SVEnumerateValueObjectClass::WriteDefaultValues(SvOi::IObjectWriter& rWriter) const
{
	_variant_t Value(GetDefaultValue());
	rWriter.WriteAttribute(scDefaultTag, Value);
}

void SVEnumerateValueObjectClass::LocalInitialize()
{
	m_ObjectTypeInfo.m_SubType = SvPb::SVEnumValueObjectType;

	SetTypeName( _T("Enumeration") );

	init();
}

} //namespace SvVol
