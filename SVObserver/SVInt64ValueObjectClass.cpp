//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInt64ValueObjectClass
//* .File Name       : $Workfile:   SVInt64ValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   30 Jul 2013 12:24:32  $
//******************************************************************************

#include "stdafx.h"
#include "SVInt64ValueObjectClass.h"
#include "SVObjectLibrary/SVToolsetScriptTags.h"
#include "SVOMFCLibrary/StringMunge.h"

namespace	// only for this file
{
	const CString DEFAULT_TAG_SAVE(_T(".Default"));
	const CString BUCKET_TAG_SAVE(_T(".Array"));	// for backwards compatibility
	const CString ARRAY_TAG_SAVE(_T(".Array_Elements"));	// new style; one bucket, all array values

	const CString DEFAULT_TAG_LOAD(_T("Default"));
	const CString BUCKET_TAG_LOAD(_T("Array"));	// for backwards compatibility
	const CString ARRAY_TAG_LOAD(_T("Array_Elements"));	// new style; one bucket, all array values
}	// end file scope namespace

SV_IMPLEMENT_CLASS(SVInt64ValueObjectClass, SVInt64ValueObjectClassGuid);

SVInt64ValueObjectClass::SVInt64ValueObjectClass(LPCTSTR ObjectName)
						: base(ObjectName)
{
	LocalInitialize();
}

SVInt64ValueObjectClass::SVInt64ValueObjectClass(SVObjectClass* pOwner, int StringResourceID)
						: base(pOwner, StringResourceID)
{
	LocalInitialize();
}

SVInt64ValueObjectClass::~SVInt64ValueObjectClass()
{
	SetObjectDepth( 0 );
	ResetObject();
}

const SVInt64ValueObjectClass& SVInt64ValueObjectClass::operator = (const SVInt64ValueObjectClass& rhs)
{
	base::operator = (rhs);
	return *this;
}

void SVInt64ValueObjectClass::Persist(SVObjectWriter& rWriter)
{
	rWriter.StartElement(GetObjectName()); // use internal name for node name

	// Get the Heading (Class Info)
	SVValueObjectClass::Persist( rWriter );

	// Get the Data Values (Member Info, Values)
	_variant_t value;
	value.ChangeType(VT_I8);
	value.llVal = DefaultValue();

	rWriter.WriteAttribute(scDefaultTag, value);

	rWriter.StartElement(scArrayElementsTag);
	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	SVVariantList list;	

	// for all elements in the array (m_iArraySize)
	for( int i = 0; i < m_iArraySize; i++ )
	{
		value.llVal = Element(m_iLastSetIndex, i);
		list.push_back(value);
	}
	rWriter.WriteAttribute(scElementTag, list);
	rWriter.EndElement();

	rWriter.EndElement();
}
HRESULT SVInt64ValueObjectClass::SetValueAt(int iBucket, int iIndex, const int iValue)
{
    return base::SetValueAt(iBucket, iIndex, static_cast <__int64> (iValue) );
}

HRESULT SVInt64ValueObjectClass::SetValueAt(int iBucket, int iIndex, CString strValue)
{
	CString strDigits( strValue );
	StringMunge::KeepChars( &strDigits, _T("0123456789- ") );
	if ( strDigits == strValue )
	{
		__int64 iValue = _atoi64( strDigits );
		return base::SetValueAt(iBucket, iIndex, iValue);
	}
	ASSERT( FALSE );
	return S_FALSE;
}

HRESULT SVInt64ValueObjectClass::SetValueAt( int iBucket, int iIndex, double value )
{
	return base::SetValueAt(iBucket, iIndex, static_cast <__int64> (value) );
}

HRESULT SVInt64ValueObjectClass::GetValueAt(int iBucket, int iIndex, double& rdValue) const
{
	__int64 value=0;

	HRESULT hr = base::GetValueAt(iBucket, iIndex, value);
	rdValue = (double) value;

	return hr;
}

HRESULT SVInt64ValueObjectClass::GetValueAt(int iBucket, int iIndex, CString& rstrValue) const
{
	__int64 value=0;

	HRESULT hr = base::GetValueAt(iBucket, iIndex, value);
	rstrValue.Format("%d",value);

	return hr;
}

HRESULT SVInt64ValueObjectClass::GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const
{
	__int64 l_i64Value=0;
	_variant_t l_Temp;
	l_Temp.Attach( rValue );
	HRESULT hr = base::GetValueAt( iBucket, iIndex, l_i64Value );
	if( hr == S_OK )
	{
		l_Temp = l_i64Value ;
	}
	else
	{
		l_Temp.Clear();
	}
	rValue = l_Temp.Detach();

	return hr;

}

void SVInt64ValueObjectClass::LocalInitialize()
{
	outObjectInfo.ObjectTypeInfo.ObjectType = SVInt64ValueObjectType;
	DefaultValue() = 0;
	m_sLegacyScriptDefaultName = "Default";
	m_sLegacyScriptArrayName = "Array";
	m_strTypeName = "Integer64";

	InitializeBuckets();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVInt64ValueObjectClass.cpp_v  $
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
 *    Rev 1.0   23 Apr 2013 11:04:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   04 Sep 2012 15:17:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add new scripting and object creation/update funcitonality to the object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   16 Dec 2009 11:08:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   18 Apr 2008 14:56:08   JSpila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source to split the value objects into individual files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
