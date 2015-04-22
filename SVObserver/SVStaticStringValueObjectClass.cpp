//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStaticStringValueObjectClass
//* .File Name       : $Workfile:   SVStaticStringValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   19 Nov 2013 09:24:20  $
//******************************************************************************

#include "stdafx.h"
#include "SVStaticStringValueObjectClass.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVOMFCLibrary/SVOMFCLibraryGlobals.h"
#include "SVGlobal.h"

namespace	// only for this file
{
	const CString DEFAULT_TAG_SAVE(_T(".Default"));
	const CString BUCKET_TAG_SAVE(_T(".Array"));	// for backwards compatibility
	const CString ARRAY_TAG_SAVE(_T(".Array_Elements"));	// new style; one bucket, all array values

	const CString DEFAULT_TAG_LOAD(_T("Default"));
	const CString BUCKET_TAG_LOAD(_T("Array"));	// for backwards compatibility
	const CString ARRAY_TAG_LOAD(_T("Array_Elements"));	// new style; one bucket, all array values
}	// end file scope namespace

SV_IMPLEMENT_CLASS(SVStaticStringValueObjectClass, SVStaticStringValueObjectClassGuid);

SVStaticStringValueObjectClass::SVStaticStringValueObjectClass(LPCTSTR ObjectName)
				         : base(ObjectName) 
{
	LocalInitialize();
}

SVStaticStringValueObjectClass::SVStaticStringValueObjectClass(	SVObjectClass* POwner, 	int StringResourceID )
   : base(POwner, StringResourceID) 
{
	LocalInitialize();
}

SVStaticStringValueObjectClass::SVStaticStringValueObjectClass( const SVStaticStringValueObjectClass& rhs)
	: base()
{
	LocalInitialize();
	*this = rhs;
}

const SVStaticStringValueObjectClass& SVStaticStringValueObjectClass::operator = (const SVStaticStringValueObjectClass& rhs)
{
	base::operator = (rhs);
	return *this;
}

SVStaticStringValueObjectClass::~SVStaticStringValueObjectClass()
{
	// Delete all object's data arrays...
	SetObjectDepth(0);
	CreateBuckets();
}

BOOL SVStaticStringValueObjectClass::CreateObject( SVObjectLevelCreateStruct* pCreateStructure )
{
	BOOL bCreate = base::CreateObject( pCreateStructure );
	SetBits( ObjectAttributesAllowedRef(), SV_CH_CONDITIONAL, false);	// Strings not allowed as conditional history conditionals
	return bCreate;
}

#ifdef USE_OBJECT_SCRIPT
void SVStaticStringValueObjectClass::GetObjectScript(CString& rstrScript, CString& rstrAliasTable, int iIndent)
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
	
	// Check for DoubleQuotes in variable
	CString tmp1 = DefaultValue();
	::SVAddEscapeSpecialCharacters(tmp1, true);
	
	tmp.Format(_T("STRING \"%s\""), tmp1);
	
	// Name is delimited by single quotes - SEJ july 23,1999
	script += strIndent + objectTag + DEFAULT_TAG_SAVE + nameStrDelimiter + _T(" = ") + tmp + _T(";");
	
	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	
	// Alias Name is delimited by single quotes - SEJ july 23,1999
	script += strIndent + objectTag + ARRAY_TAG_SAVE + nameStrDelimiter + _T(" = STRING ") + _T("[ ");
	
	// for all elements in the array (objectDepth)
	for (int i = 0; i < m_iArraySize; i++)
	{
		if (i)
			script += _T(", ");
		
		tmp1 = Element(m_iLastSetIndex, i); 
		::SVAddEscapeSpecialCharacters(tmp1, true);
		tmp.Format(_T("\"%s\""), tmp1);
		script +=  tmp;
	}
	script += _T(" ]");
	
	// Add termination...
	script += _T(";");
	
	script.FreeExtra();
	
	rstrScript += script;
}
#endif

void SVStaticStringValueObjectClass::Persist(SVObjectWriter& rWriter)
{
	rWriter.StartElement(GetObjectName()); // use internal name for node name

	// Get the ClassID, ObjectID, Attrributes
	SVValueObjectClass::Persist(rWriter);

	// Get the Data Values (Member Info, Values)
	// Check for DoubleQuotes in variable
	CString tmp = DefaultValue();
	::SVAddEscapeSpecialCharacters(tmp, true);
	
	_variant_t value;
	value.SetString(tmp);
	rWriter.WriteAttribute(scDefaultTag, value);
	value.Clear();

	rWriter.StartElement(scArrayElementsTag);

	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	SVVariantList list;
	
	// for all elements in the array (objectDepth)
	for (int i = 0; i < m_iArraySize; i++)
	{
		tmp = Element(0, i); 
		::SVAddEscapeSpecialCharacters(tmp, true);
		value.SetString(tmp);
		list.push_back(value);
		value.Clear();
	}
	rWriter.WriteAttribute(scElementTag, list);
	rWriter.EndElement();

	rWriter.EndElement();
}

HRESULT  SVStaticStringValueObjectClass::SetObjectValue(SVObjectAttributeClass* pDataObject)
{
	HRESULT hr = S_FALSE;
	BOOL bOk = FALSE;
	
	CString  csTemp;
	
	//@WARNING [MZA, 21.04.15] Why do we use here not the class SVObjectCStringArrayClass?
	SvCl::SVObjectArrayClassTemplate<CString> svArray;
	bucket_type l_Buckets(BucketsNoAssert());
	array_type l_Array;
	
	if ( bOk = pDataObject->GetAttributeData(DEFAULT_TAG_LOAD, svArray) )
	{
		for (int i = 0; i < svArray.GetSize(); i++)
		{
			DefaultValue() = svArray[i];
			
			// Remove any escapes
			SVRemoveEscapedSpecialCharacters(DefaultValue(), true);
		}
	}
	else if ( bOk = pDataObject->GetAttributeData(BUCKET_TAG_LOAD, l_Buckets, DefaultValue() ) )
	{
		// Remove any escapes
		SVRemoveEscapedSpecialCharacters(l_Buckets[0][0], true);

		if ( ArraySize() == 1 )
		{
			ScalarBucket(0)=l_Buckets[0][0];
		}
		else
		{
			std::swap(Buckets(), l_Buckets);
		}
		if ( 0 < m_iNumberOfBuckets )
		{
			m_iLastSetIndex = 0;
		}
	}
	// new-style: store all array elements:
	else if ( bOk = pDataObject->GetArrayData(ARRAY_TAG_LOAD, l_Array, DefaultValue() ) )
	{
		for (size_t i = 0; i < l_Array.size(); i++)
		{
			// Remove any escapes
			SVRemoveEscapedSpecialCharacters(l_Array[i], true);
		}

		SetArraySize( static_cast<int>(l_Array.size()) );
		if ( ArraySize() == 1 )
		{
			ScalarBucket(0)=l_Array[0];
		}
		else
		{
			std::swap(Array(0), l_Array);
		}
		m_iLastSetIndex = 0;
	}
	else if ((bOk = pDataObject->GetAttributeData("StrDefault", svArray)))
	{
		for (int i = 0; i < svArray.GetSize(); i++)
		{
			DefaultValue() = svArray[i];
			
			// Remove any escapes
			SVRemoveEscapedSpecialCharacters(DefaultValue(), true);
		}
	}
	else if ( bOk = pDataObject->GetAttributeData("StrArray", l_Buckets, DefaultValue() ) )
	{
		// Remove any escapes
		SVRemoveEscapedSpecialCharacters(l_Buckets[0][0], true);

		if ( ArraySize() == 1 )
		{
			ScalarBucket(0)=l_Buckets[0][0];
		}
		else
		{
			std::swap(Buckets(), l_Buckets);
		}
		if ( 0 < m_iNumberOfBuckets )
		{
			m_iLastSetIndex = 0;
		}
	}
	else
	{
		hr = base::SetObjectValue(pDataObject);
		return hr;
	}

	hr = bOk ? S_OK : S_FALSE;
	return hr;
}

HRESULT SVStaticStringValueObjectClass::SetValueAt( int iBucket, int iIndex, const VARIANT& rvtValue )
{
	if ( rvtValue.vt == VT_BSTR )
		return base::SetValueAt(iBucket, iIndex, CString(rvtValue.bstrVal));
	ASSERT(FALSE);
	return S_FALSE;
}

HRESULT SVStaticStringValueObjectClass::GetValueAt( int iBucket, int iIndex, long& rlValue) const
{
	CString strValue;

	HRESULT hr =  base::GetValueAt( iBucket, iIndex, strValue );
	rlValue = atoi( strValue );

	return hr;
}

HRESULT SVStaticStringValueObjectClass::GetValueAt( int iBucket, int iIndex, double& rdValue) const
{
	CString strValue;

	HRESULT hr = base::GetValueAt( iBucket, iIndex, strValue );
	rdValue = atof( strValue );

	return hr;
}

HRESULT SVStaticStringValueObjectClass::GetValueAt( int iBucket, int iIndex, VARIANT& rvtValue ) const
{
	CString strVal;

	_variant_t l_Temp;
	l_Temp.Attach( rvtValue );
	HRESULT hr = base::GetValueAt( iBucket, iIndex, strVal );
	if( hr == S_OK )
	{
		l_Temp = strVal ;
	}
	else
	{
		l_Temp.Clear();
	}
	rvtValue = l_Temp.Detach();

	return hr;
}

void SVStaticStringValueObjectClass::LocalInitialize()
{
	outObjectInfo.ObjectTypeInfo.ObjectType = SVStringValueObjectType;
	//StrDefault = _T("");
	
	ObjectAttributesAllowedRef() = SV_VIEWABLE | SV_PUBLISHABLE | SV_ARCHIVABLE | SV_EMBEDABLE | SV_PRINTABLE | SV_CH_VALUE | SV_DD_VALUE;
	m_strTypeName = "Text";

	InitializeBuckets();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVStaticStringValueObjectClass.cpp_v  $
 * 
 *    Rev 1.1   19 Nov 2013 09:24:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  872
 * SCR Title:  Add Archive Tool Headers to Archive File
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified last set index to always be zero.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Nov 2013 09:32:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  872
 * SCR Title:  Add Archive Tool Headers to Archive File
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial check-in static value object to support Archive tool headers and tool comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   02 Oct 2013 08:17:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Jul 2013 12:29:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  824
 * SCR Title:  Add Remote Access to SVObserver Application Configuration Variables
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified GetValueAt( ...VARIANT&) to use _variant_t to manage the clearing of the passed in variant.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:15:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   04 Sep 2012 15:20:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add new scripting and object creation/update funcitonality to the object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   01 Sep 2011 10:55:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  737
 * SCR Title:  Fix serialization bug associated with control characters in strings
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified SVRemoveEscapeCharacters and SVAddEscapeCharacters.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   27 Jan 2011 11:58:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   28 Oct 2010 14:11:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Dec 2009 12:54:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Apr 2008 12:42:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  623
 * SCR Title:  Upgrade SVObserver to 5.00
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added the Special character 0x1d or  Octal 35 to the specialCharacters in the SetObjectValue and GetObjectScript functions to allow barcodes with multiple lines to save without causing an issue.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Apr 2008 14:46:00   JSpila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source to split the value objects into individual files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
