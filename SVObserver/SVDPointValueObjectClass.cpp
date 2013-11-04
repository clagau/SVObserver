//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDPointValueObjectClass
//* .File Name       : $Workfile:   SVDPointValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   01 Aug 2013 11:21:42  $
//******************************************************************************

#include "stdafx.h"
#include "SVDPointValueObjectClass.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVOMFCLibrary/StringMunge.h"
#include "SVOMFCLibrary/SVOMFCLibraryGlobals.h"

namespace	// only for this file
{
	const CString DEFAULT_TAG_SAVE(_T(".Default"));
	const CString BUCKET_TAG_SAVE(_T(".Array"));	// for backwards compatibility
	const CString ARRAY_TAG_SAVE(_T(".Array_Elements"));	// new style; one bucket, all array values

	const CString DEFAULT_TAG_LOAD(_T("Default"));
	const CString BUCKET_TAG_LOAD(_T("Array"));	// for backwards compatibility
	const CString ARRAY_TAG_LOAD(_T("Array_Elements"));	// new style; one bucket, all array values
}	// end file scope namespace

SV_IMPLEMENT_CLASS( SVDPointValueObjectClass, SVDPointValueObjectClassGuid );

SVDPointValueObjectClass::SVDPointValueObjectClass( LPCTSTR ObjectName )
				        : base( ObjectName ) 
{
	LocalInitialize();
}

SVDPointValueObjectClass::SVDPointValueObjectClass( SVObjectClass* POwner, int StringResourceID )
				        : base( POwner, StringResourceID ) 
{
	LocalInitialize();
}

const SVDPointValueObjectClass& SVDPointValueObjectClass::operator = (const SVDPointValueObjectClass& rhs)
{
	base::operator = (rhs);
	return *this;
}
  
SVDPointValueObjectClass::~SVDPointValueObjectClass()
{
	// Delete all object's data arrays...
	SetObjectDepth( 0 );
	ResetObject();
}

BOOL SVDPointValueObjectClass::CreateObject( SVObjectLevelCreateStruct* pCreateStructure )
{
	BOOL bCreate = base::CreateObject( pCreateStructure );
	SetBits( ObjectAttributesAllowedRef(), SV_CH_CONDITIONAL, false);	// DPoints not allowed as conditional history conditionals
	return bCreate;
}

#ifdef USE_OBJECT_SCRIPT
void SVDPointValueObjectClass::GetObjectScript( CString& rstrScript, CString& rstrAliasTable, int iIndent )
{
	CString script;

	// preallocate 4K
    script.GetBuffer(4096);  
	script.ReleaseBuffer(); 
	
	// Get the Heading (Class Info)
	SVValueObjectClass::GetObjectScript( script, rstrAliasTable, iIndent );

	CString nameStrDelimiter = _T( "'" );

	// Generate indent...
	CString strIndent = _T( "\n" );
	if( iIndent )
	{
		CString tabsStrIndent(_T( '\t' ), iIndent);
		strIndent += tabsStrIndent;
	}

	// Name is delimited by single quotes - SEJ july 23,1999
	// NOTE: Don' use object name here, if object name and name
	//		 are not identically SetObjectValue(...) could be sent
	//		 to wrong object while script parsing. RO_22Feb2000
	CString objectTag = nameStrDelimiter + _T( "_object_ID_" ) + GetName(); //GetObjectName();
	
	// Get the Data Values (Member Info, Values)
	CString tmp;
	tmp.Format( "DPOINT %s %lf, %lf %s", _T( "[ " ), DefaultValue().x, DefaultValue().y, _T( " ]" ) );
	
	// Name is delimited by single quotes - SEJ july 23,1999
	script += strIndent + objectTag + _T(".Default" ) + nameStrDelimiter + _T( " = " ) + tmp + _T( ";" );

	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)

	// Name is delimited by single quotes - SEJ july 23,1999
	script += strIndent + objectTag + _T(".Array_Elements" ) + nameStrDelimiter + _T( " = DPOINT " ) + _T( "[ " );
	
	// for all elements in the array (m_iArraySize)
	for( int i = 0; i < m_iArraySize; i++ )
	{
		if( i > 0 )
			script += _T( ", " );

		tmp.Format( "%lf, %lf", Element(m_iLastSetIndex, i).x, Element(m_iLastSetIndex, i).y );
		script +=  tmp;
	}
	script += _T( " ]" );

	// Add termination...
	script += _T( ";" );
	
	script.FreeExtra();

	rstrScript += script;
}
#endif

void SVDPointValueObjectClass::Persist(SVObjectWriter& rWriter)
{
	rWriter.StartElement(GetObjectName()); // use internal name for node name

	// Get the Heading (Class Info)
	SVValueObjectClass::Persist( rWriter );

	// Get the Data Values (Member Info, Values)
	CString tmp;
	tmp.Format( _T( "%lf, %lf"), DefaultValue().x, DefaultValue().y );
	
	_variant_t value;
	value.SetString(tmp);
	rWriter.WriteAttribute(scDefaultTag, value);
	value.Clear();

	rWriter.StartElement(scArrayElementsTag);
	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	SVVariantList list;	

	// for all elements in the array (m_iArraySize)
	for( int i = 0; i < m_iArraySize; i++ )
	{
		tmp.Format( _T("%lf, %lf"), Element(m_iLastSetIndex, i).x, Element(m_iLastSetIndex, i).y );
		value.SetString(tmp);
		list.push_back(value);
		value.Clear();
	}
	rWriter.WriteAttribute(scElementTag, list);
	rWriter.EndElement();

	rWriter.EndElement();
}

HRESULT SVDPointValueObjectClass::SetObjectValue( SVObjectAttributeClass* PDataObject )
{
	HRESULT hr = S_FALSE;
	BOOL bOk = FALSE;

	SVObjectDPointArrayClass svPointArray;
	bucket_type l_Buckets(BucketsNoAssert());

	if ( ( bOk = PDataObject->GetAttributeData( "defaultPoint", svPointArray ) ) )
	{
		for( int i = 0; i < svPointArray.GetSize(); i++ )
		{
			DefaultValue() = svPointArray[i];
		}
	}
	else if ( bOk = PDataObject->GetAttributeData( "pArray", l_Buckets, DefaultValue() ) )
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
		hr = base::SetObjectValue( PDataObject );
		return hr;
	}

	hr = bOk ? S_OK : S_FALSE;
	return hr;
}

HRESULT SVDPointValueObjectClass::SetValueAt( int iBucket, int iIndex, const POINT Point )
{
	return base::SetValueAt(iBucket, iIndex, SVDPointClass(Point));
}

HRESULT SVDPointValueObjectClass::SetValueAt( int iBucket, int iIndex, const CPoint Point )
{
	return base::SetValueAt(iBucket, iIndex, SVDPointClass(Point));
}

HRESULT SVDPointValueObjectClass::SetDefaultValue( const POINT Point, bool bResetAll )
{
	return base::SetDefaultValue( SVDPointClass(Point), bResetAll );
}

HRESULT SVDPointValueObjectClass::GetDefaultValue( POINT& rPoint )
{
	DefaultValue().GetValue( rPoint );
	return IsCreated() ? S_OK : S_FALSE;
}

HRESULT SVDPointValueObjectClass::SetDefaultValue( const CPoint Point, bool bResetAll )
{
	return base::SetDefaultValue( SVDPointClass(Point), bResetAll );
}

HRESULT SVDPointValueObjectClass::GetDefaultValue( CPoint& rPoint )
{
	DefaultValue().GetValue( rPoint );
	return IsCreated() ? S_OK : S_FALSE;
}

HRESULT SVDPointValueObjectClass::SetValueAt( int iBucket, int iIndex, CString strValue )
{
	CString strLegalChars (strValue);
	StringMunge::KeepChars( &strLegalChars, _T("0123456789()-., ") );	// floats
	if ( strLegalChars == strValue )
	{
		StringMunge::StripChars( &strLegalChars, _T("()") );
		int iComma = strLegalChars.Find(_T(','));
		if ( iComma > 0 && iComma < strLegalChars.GetLength() - 1 )
		{
			CString sX = strLegalChars.Left( iComma );
			CString sY = strLegalChars.Mid( iComma + 1 );
			SVDPointClass ptValue (atof(sX), atof(sY));
			return base::SetValueAt(iBucket, iIndex, ptValue );
		}
	}
	ASSERT(FALSE);
	return S_FALSE;
}

HRESULT SVDPointValueObjectClass::SetValueAt( int iBucket, int iIndex, double value )
{
	ASSERT(FALSE);
	return S_FALSE;
}

HRESULT SVDPointValueObjectClass::GetValueAt( int iBucket, int iIndex, POINT& rPoint ) const
{
	SVDPointClass value;

	HRESULT hr = base::GetValueAt(iBucket, iIndex, value);
	value.GetValue(rPoint);

	return hr;
}

HRESULT SVDPointValueObjectClass::GetValueAt( int nBucket, int iIndex, double& rstrValue ) const
{
	ASSERT(FALSE);
	return S_FALSE;
}

HRESULT SVDPointValueObjectClass::GetValueAt( int iBucket, int iIndex, CPoint& rPoint ) const
{
	SVDPointClass value;
	
	HRESULT hr = base::GetValueAt(iBucket, iIndex, value);
	value.GetValue(rPoint);

	return hr;
}

HRESULT SVDPointValueObjectClass::GetValueAt( int iBucket, int iIndex, CString& rstrValue ) const
{
	SVDPointClass value;

	HRESULT hr = base::GetValueAt( iBucket, iIndex, value );
	//if ( hr == S_OK )
	{
		rstrValue.Format( _T( "( %lf, %lf )" ), value.x, value.y );
	}

	return hr;
}

HRESULT SVDPointValueObjectClass::GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const
{
	SVDPointClass l_dPoint;

	_variant_t l_Temp;
	l_Temp.Attach( rValue );

	HRESULT hr = base::GetValueAt( iBucket, iIndex, l_dPoint );
	if( hr == S_OK )
	{
		CString l_strTmp;
		l_strTmp.Format(  _T( "( %lf, %lf )"),l_dPoint.x, l_dPoint.y);
		l_Temp = l_strTmp;
	}
	else
	{
		l_Temp.Clear();
	}
	rValue = l_Temp.Detach();

	return hr;
}

void SVDPointValueObjectClass::LocalInitialize()
{
	outObjectInfo.ObjectTypeInfo.ObjectType = SVDPointValueObjectType;
	//DefaultValue().x = 0.0;
	//DefaultValue().y = 0.0;

	ObjectAttributesAllowedRef() = SV_VIEWABLE | SV_ARCHIVABLE | SV_EMBEDABLE | SV_PRINTABLE | SV_CH_VALUE | SV_DD_VALUE;

	m_strTypeName = "Point";

	InitializeBuckets();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVDPointValueObjectClass.cpp_v  $
 * 
 *    Rev 1.2   01 Aug 2013 11:21:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  824
 * SCR Title:  Add Remote Access to SVObserver Application Configuration Variables
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed GetValueAt(...VARIANT&)  to use _variant_t for automatic cleanup of old variant.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Jul 2013 12:24:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  824
 * SCR Title:  Add Remote Access to SVObserver Application Configuration Variables
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Implemented the GetValueAt(... Variant&) function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:20:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   04 Sep 2012 15:00:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add new scripting and object creation/update funcitonality to the object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   18 Mar 2011 07:18:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix performance issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   28 Oct 2010 13:44:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   16 Dec 2009 09:01:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix the include and comment issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   18 Apr 2008 14:45:28   JSpila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source to split the value objects into individual files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
