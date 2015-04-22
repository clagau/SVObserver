//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDoubleValueObjectClass
//* .File Name       : $Workfile:   SVDoubleValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   01 Oct 2013 14:12:22  $
//******************************************************************************

#include "stdafx.h"
#include "SVDoubleValueObjectClass.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
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

SV_IMPLEMENT_CLASS(SVDoubleValueObjectClass, SVDoubleValueObjectClassGuid);

SVDoubleValueObjectClass::SVDoubleValueObjectClass(LPCTSTR ObjectName)
				         : base(ObjectName) 
{
	LocalInitialize();
}

SVDoubleValueObjectClass::SVDoubleValueObjectClass(SVObjectClass* POwner, int StringResourceID)
				         : base(POwner, StringResourceID) 
{
	LocalInitialize();
}

SVDoubleValueObjectClass::SVDoubleValueObjectClass( const SVDoubleValueObjectClass& rhs )
	: base()
{
	LocalInitialize();
	*this = rhs;
}

const SVDoubleValueObjectClass& SVDoubleValueObjectClass::operator = (const SVDoubleValueObjectClass& rhs)
{
	base::operator = (rhs);
	return *this;
}

SVDoubleValueObjectClass::~SVDoubleValueObjectClass()
{
	// Delete all object's data arrays...
	SetObjectDepth(0);
	CreateBuckets();
}

#ifdef USE_OBJECT_SCRIPT
void SVDoubleValueObjectClass::GetObjectScript(CString& rstrScript, CString& rstrAliasTable, int iIndent)
{
	CString script;
	
	// preallocate 4K
	script.GetBuffer(4096);  
	script.ReleaseBuffer(); 
	
	// Get the Heading (Class Info)
	SVValueObjectClass::GetObjectScript(script, rstrAliasTable, iIndent);
	
	CString nameStrDelimiter = _T("'");
	
	// Generate indent...
	CString strIndent = _T("\n");
	if (iIndent)
	{
		CString tabsStrIndent(_T('\t'), iIndent);
		strIndent += tabsStrIndent;
	}
	
	// Name is delimited by single quotes - SEJ july 23,1999
	// NOTE: Don' use object name here, if object name and name
	//		 are not identically SetObjectValue(...) could be sent
	//		 to wrong object while script parsing. RO_22Feb2000
	CString objectTag = nameStrDelimiter + _T("_object_ID_") + GetName();
	// GetObjectName();
	
	// Get the Data Values (Member Info, Values)
	CString tmp;
	tmp.Format("DOUBLE %lf", DefaultValue());
	
	// Name is delimited by single quotes - SEJ july 23,1999
	script += strIndent + objectTag + DEFAULT_TAG_SAVE + nameStrDelimiter + _T(" = ") + tmp + _T(";");
	
	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	
	// Name is delimited by single quotes - SEJ july 23,1999
	script += strIndent + objectTag + ARRAY_TAG_SAVE + nameStrDelimiter + _T(" = DOUBLE ") + _T("[ ");
	
	// for all elements in the array (objectDepth)
	for (int i = 0; i < m_iArraySize; i++)
	{
		if (i)
			script += _T(", ");
		
		tmp.Format("%f", Element(m_iLastSetIndex, i));
		script +=  tmp;
	}
	script += _T(" ]");
	
	// Add termination...
	script += _T(";");
	
	script.FreeExtra();
	
	rstrScript += script;
}
#endif

void SVDoubleValueObjectClass::Persist(SVObjectWriter& rWriter)
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
		value.dblVal = Element(m_iLastSetIndex, i);
		list.push_back(value);
	}
	rWriter.WriteAttribute(scElementTag, list);
	rWriter.EndElement();

	rWriter.EndElement();
}

// This override provides the ability to correctly load script data from the legacy SVDoubleVectorObjectClass
HRESULT SVDoubleValueObjectClass::SetObjectValue( SVObjectAttributeClass* pDataObject )
{
	HRESULT hr = S_FALSE;
	
	SvCl::SVObjectArrayClassTemplate<value_type> svArray;	// for default values

	if ( m_bLegacyVectorObjectCompatibility )
	{
		if ( SVObjectAttributeClassHelper::GetAttributeData(pDataObject, "bDefault", svArray) )
		{
			if ( svArray.GetSize() > 0 )
				DefaultValue() = svArray[ svArray.GetSize()-1 ];
			hr = S_OK;
			return hr;
		}
		else if (    SVObjectAttributeClassHelper::GetAttributeData(pDataObject, "pArray", svArray)
		          || SVObjectAttributeClassHelper::GetAttributeData(pDataObject, BUCKET_TAG_LOAD, svArray) )
		{
			SetArraySize( static_cast<int>(svArray.size()) );
			hr = SetArrayValues(1, svArray.begin(), svArray.end());
			return hr;
		}
	}
	hr = base::SetObjectValue( pDataObject );
	return hr;
}

HRESULT SVDoubleValueObjectClass::SetValueAt( int iBucket, int iIndex, const VARIANT& rvtValue )
{
	if ( rvtValue.vt == VT_R8 )
		return base::SetValueAt(iBucket, iIndex, rvtValue.dblVal);
	ASSERT(FALSE);
	return S_FALSE;
}

HRESULT SVDoubleValueObjectClass::SetValueAt(int iBucket, int iIndex, const int value)
{
    return base::SetValueAt(iBucket, iIndex, (double) value);
}

HRESULT SVDoubleValueObjectClass::SetValueAt(int iBucket, int iIndex, const long value)
{
    return base::SetValueAt(iBucket, iIndex, (double) value);
}

HRESULT SVDoubleValueObjectClass::SetValueAt( int iBucket, int iIndex, CString strValue )
{
	CString strDigits (strValue);
	StringMunge::KeepChars( &strDigits, _T("-0123456789. ") );
	if ( strDigits == strValue )
	{
		double dValue = atof(strValue);
		return base::SetValueAt(iBucket, iIndex, dValue );
	}
	ASSERT(FALSE);
	return S_FALSE;
}

HRESULT SVDoubleValueObjectClass::GetValueAt( int iBucket, int iIndex, long& rlValue ) const
{
	double dValue=0.0;

	HRESULT hr = base::GetValueAt( iBucket, iIndex, dValue );
	rlValue = static_cast<long>(dValue);

	return hr;
}

HRESULT SVDoubleValueObjectClass::GetValueAt( int iBucket, int iIndex, DWORD& rdwValue ) const
{
	double dValue=0.0;

	HRESULT hr = base::GetValueAt( iBucket, iIndex, dValue );
	rdwValue = static_cast<unsigned long>(dValue);

	return hr;
}

HRESULT SVDoubleValueObjectClass::GetValueAt( int iBucket, int iIndex, CString& rstrValue) const
{
	double value = 0.0;

	HRESULT hr = base::GetValueAt(iBucket, iIndex, value);
	rstrValue.Format(_T("%lf"), value);

	return hr;
}

HRESULT SVDoubleValueObjectClass::GetValueAt( int iBucket, int iIndex, VARIANT& rvtValue ) const
{
	double l_dValue=0;
	_variant_t l_Temp;
	l_Temp.Attach( rvtValue );
	HRESULT hr = base::GetValueAt( iBucket, iIndex, l_dValue );
	if( hr == S_OK )
	{
		l_Temp = l_dValue;
	}
	else
	{
		l_Temp.Clear();
	}
	rvtValue = l_Temp.Detach();
	return hr;
}

void SVDoubleValueObjectClass::LocalInitialize()
{
	outObjectInfo.ObjectTypeInfo.ObjectType = SVDoubleValueObjectType;
	DefaultValue() = 0.0;
	if ( m_sLegacyScriptDefaultName.IsEmpty() )
	{
		m_sLegacyScriptDefaultName = "dDefault";
		m_sLegacyScriptArrayName = "pDArray";
	}
	m_strTypeName = "Decimal";
	InitializeBuckets();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVDoubleValueObjectClass.cpp_v  $
 * 
 *    Rev 1.2   01 Oct 2013 14:12:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Jul 2013 12:28:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  824
 * SCR Title:  Add Remote Access to SVObserver Application Configuration Variables
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified GetValueAt( ...VARIANT&) to use _variant_t to manage the clearing of the passed in variant.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:20:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   04 Sep 2012 15:00:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add new scripting and object creation/update funcitonality to the object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Nov 2010 16:09:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   28 Oct 2010 13:44:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Dec 2009 09:01:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix the include and comment issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Apr 2008 14:40:16   JSpila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source to split the value objects into individual files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
