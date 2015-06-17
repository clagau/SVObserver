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

#include "stdafx.h"
#include "SVEnumerateValueObjectClass.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVOMFCLibrary/StringMunge.h"
#include "SVOMFCLibrary/SVOMFCLibraryGlobals.h" // for SV_FALSE;

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
	SetBits( ObjectAttributesAllowedRef(), SV_CH_CONDITIONAL, false);	// Enums not allowed as conditional history conditionals
	return bCreate;
}

#ifdef USE_OBJECT_SCRIPT
void SVEnumerateValueObjectClass::GetObjectScript( CString& RStrScript, CString& rStrAliasTable, int Indent )
{
	CString script;

	// preallocate 4K
    script.GetBuffer(4096);  
	script.ReleaseBuffer(); 
	
	// Get the Heading (Class Info)
	SVValueObjectClass::GetObjectScript( script, rStrAliasTable, Indent );

	CString nameStrDelimiter = _T( "'" );

	// Generate indent...
	CString strIndent = _T( "\n" );
	if( Indent )
	{
		CString tabsStrIndent(_T( '\t' ), Indent);
		strIndent += tabsStrIndent;
	}

	// Name is delimited by single quotes - SEJ july 23,1999
	//
	// NOTE: Don' use object name here, if object name and name
	//		 are not identically SetObjectValue(...) could be sent
	//		 to wrong object while script parsing.
	CString objectTag = nameStrDelimiter + _T( "_object_ID_" ) + GetName(); //GetObjectName();

	// Get the Data Values (Member Info, Values)
	CString tmp;
	tmp.Format( "LONG %d", DefaultValue());

	// Name is delimited by single quotes - SEJ july 23,1999
	script += strIndent + objectTag + _T(".Default" ) + nameStrDelimiter + _T( " = " ) + tmp + _T( ";" );

	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)

	// Name is delimited by single quotes - SEJ july 23,1999
	script += strIndent + objectTag + _T(".Array_Elements" ) + nameStrDelimiter + _T( " = LONG " ) + _T( "[ " );
	
	// for all elements in the array (m_iArraySize)
	for( int i = 0; i < m_iArraySize; i++ )
	{
		if( i > 0 )
			script += _T( ", " );

		tmp.Format( "%d", Element(m_iLastSetIndex, i) );
		script +=  tmp;
	}
	script += _T( " ]" );

	// Add termination...
	script += _T( ";" );

	// Get Enumeration types...
	CString strEnum;
	//
	// NOTE:
	// Can be scripted, but normally the owner object which embeddes this enumerator object
	// should set up the types everytime he is constructed.
	//
	// GetEnumTypes( strEnum );
	if( ! strEnum.IsEmpty() )
	{
		// Add enumeration types, if any...
		script += strIndent + objectTag + _T(".Enumeration" ) + nameStrDelimiter + _T( " = STRING [ \"" ) + strEnum + _T( "\" ];" );
	}
	
	script.FreeExtra();

	RStrScript += script;
}
#endif

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

					if( l_SafeArray.GetElement( l_Index, l_Value ) == S_OK && l_Value.vt == VT_BSTR )
					{
						l_Enums[ i ].first = static_cast< LPCTSTR >( _bstr_t( l_Value ) );
					}

					l_Index[ 0 ] = i;
					l_Index[ 1 ] = 1;

					if( l_SafeArray.GetElement( l_Index, l_Value ) == S_OK )
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

HRESULT SVEnumerateValueObjectClass::SetValueAt( int iBucket, int iIndex, CString strValue )
{
	long lValue = 0;

	if (GetEnumerator( strValue, lValue ))
		return base::SetValueAt( iBucket, iIndex, lValue );

	//ASSERT(FALSE);
	return S_FALSE;
}

HRESULT SVEnumerateValueObjectClass::SetValueAt( int iBucket, int iIndex, double value )
{
	return SetValueAt(iBucket, iIndex, static_cast <int> (value) );
}

HRESULT SVEnumerateValueObjectClass::SetDefaultValue( CString StrValue, bool bResetAll )
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
	//if ( hr == S_OK )
	{
		GetEnumeratorName( lValue, rstrValue );
	}
	return hr;
}

HRESULT SVEnumerateValueObjectClass::GetValueAt( int iBucket, int iIndex, double& rdValue ) const
{
	long lValue=0;
	HRESULT hr = base::GetValueAt( iBucket, iIndex, lValue );
	//if ( hr == S_OK )
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
	if( hr == S_OK )
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




////////////////////////////////////////////////////////////////////////////////
// .Title       : SetEnumTypes
// -----------------------------------------------------------------------------
// .Description : Pass Enumeration String e.g. "NULL=0,ONE=1,BigNumber=99999"
//				: Use 'comma' sign as separator and 'assign' sign to set a 
//				: special value. If no value is defined for an enumeration
//				: standard enumeration defines an incremental number.
//				: ( e.g. "NULL,ONE,TWO,BigNumber=99999,BiggerNumber" ) 
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :21.02.2000 RO			First Implementation
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

			} while( szEnumToken = _tcstok( NULL, sep ) );

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
	return TRUE;
}

BOOL SVEnumerateValueObjectClass::GetEnumTypes( SVEnumerateVector& rVec ) const
{
	rVec.clear();

	for ( int i=0; i < m_enumStringTable.GetSize(); i++ )
	{
		rVec.push_back( SVEnumeratePair( m_enumStringTable.GetAt(i), m_enumValueTable.GetAt(i) ) );
	}
	return TRUE;
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
// .History
//	 Date		Author		Comment                                       
//  :21.02.2000 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVEnumerateValueObjectClass::GetEnumerator( LPCTSTR szEnumerator, long& lValue ) const
{
	BOOL bRetVal = FALSE;
	if( szEnumerator )
	{
		// Check if enumerator is defined...
		for( int i = 0; i < m_enumStringTable.GetSize(); ++ i )
		{
			if( m_enumStringTable.GetAt( i ).CompareNoCase( szEnumerator ) == 0 )
			{
				// Found it...
				lValue = m_enumValueTable[ i ];
				bRetVal = TRUE;
			}
		}

		if ( bRetVal == FALSE )	// look through values
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
						bRetVal = TRUE;
						break;
					}
				}
			}
		}
	}// end if( szEnumerator )

	return bRetVal;
}

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
			TCHAR* p = NULL;
			p_rlValue = _tcstol(strMunge, &p, 16);	// base 16
			bConverted = true;
		}
		else	// assume base 10
		{
			CString strDec( strMunge );
			StringMunge::KeepChars( &strDec, _T("0123456789-. ") );
			if ( strDec == strMunge )	// if no abcdef
			{
				TCHAR* p = NULL;
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
// .History
//	 Date		Author		Comment                                       
//  :21.02.2000 RO			First Implementation
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
// .History
//	 Date		Author		Comment                                       
//  :22.02.2000 RO			First Implementation
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
// .History
//	 Date		Author		Comment                                       
//  :22.02.2000 RO			First Implementation
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
// .History
//	 Date		Author		Comment                                       
//  :22.02.2000 RO			First Implementation
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
	outObjectInfo.ObjectTypeInfo.ObjectType = SVEnumValueObjectType;

	//DefaultValue() = 0;
	SetBits( ObjectAttributesAllowedRef(), SV_CH_CONDITIONAL, false);	// Enums not allowed as conditional history conditionals

	m_strTypeName = "Enumeration";

	InitializeBuckets();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVEnumerateValueObjectClass.cpp_v  $
 * 
 *    Rev 1.2   01 Oct 2013 14:12:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Jul 2013 12:28:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  824
 * SCR Title:  Add Remote Access to SVObserver Application Configuration Variables
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified GetValueAt( ...VARIANT&) to use _variant_t to manage the clearing of the passed in variant.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:26:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   21 Jan 2013 10:58:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated missing SetObjectValue and GetObjectValue methods based on 6.01 branch changes for the shift tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   04 Sep 2012 15:03:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add new scripting and object creation/update funcitonality to the object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   28 Oct 2010 13:46:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Dec 2009 10:21:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Apr 2008 14:41:16   JSpila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source to split the value objects into individual files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
