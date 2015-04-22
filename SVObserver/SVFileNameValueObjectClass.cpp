//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileNameValueObjectClass
//* .File Name       : $Workfile:   SVFileNameValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   01 Oct 2013 14:19:28  $
//******************************************************************************

#include "stdafx.h"
#include "SVFileNameValueObjectClass.h"
#include "SVFileNameManagerClass.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVOMFCLibrary/SVOMFCLibraryGlobals.h" // for SV_FALSE;
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

SV_IMPLEMENT_CLASS(SVFileNameValueObjectClass, SVFileNameValueObjectClassGuid);

SVFileNameValueObjectClass::SVFileNameValueObjectClass(LPCTSTR ObjectName)
                          : base(ObjectName) 
{
	LocalInitialize();
}


SVFileNameValueObjectClass::SVFileNameValueObjectClass(	SVObjectClass* POwner, 	int StringResourceID )
                          : base(POwner, StringResourceID) 
{
	LocalInitialize();
}


SVFileNameValueObjectClass::SVFileNameValueObjectClass( const SVFileNameValueObjectClass& rhs)
                          : base()
{
	LocalInitialize();
	*this = rhs;
}

const SVFileNameValueObjectClass& SVFileNameValueObjectClass::operator = ( const SVFileNameValueObjectClass& rhs )
{
	base::operator = (rhs);
	return *this;
}


SVFileNameValueObjectClass::~SVFileNameValueObjectClass()
{
	// Delete all object's data arrays...
	SetObjectDepth(0);
	CreateBuckets();
	
	SVFileNameManagerClass svFileManager;
	svFileManager.RemoveItem(&m_svFileName);
}

BOOL SVFileNameValueObjectClass::CreateObject( SVObjectLevelCreateStruct* pCreateStructure )
{
	BOOL bCreate = base::CreateObject( pCreateStructure );
	SetBits( ObjectAttributesAllowedRef(), SV_CH_CONDITIONAL, false);	// File names not allowed as conditional history conditionals
	return bCreate;
}

#ifdef USE_OBJECT_SCRIPT
void SVFileNameValueObjectClass::GetObjectScript(CString& rstrScript, CString& rstrAliasTable, int iIndent)
{
	SVFileNameManagerClass svFileManager;
	svFileManager.SaveItem(&m_svFileName);
	
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
	::SVAddEscapeSpecialCharacters(tmp1, false);
	
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
		::SVAddEscapeSpecialCharacters(tmp1, false);
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

void SVFileNameValueObjectClass::Persist(SVObjectWriter& rWriter)
{
	SVFileNameManagerClass svFileManager;
	svFileManager.SaveItem(&m_svFileName);
	
	rWriter.StartElement(GetObjectName()); // use internal name for node name

	// Get the Heading (Class Info)
	SVValueObjectClass::Persist( rWriter );

	// Get the Data Values (Member Info, Values)
	CString tmp;
	tmp.Format( _T("%s"), DefaultValue() );
	
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
		tmp.Format( _T("%s"), Element(m_iLastSetIndex, i) );
		value.SetString(tmp);
		list.push_back(value);
		value.Clear();
	}
	rWriter.WriteAttribute(scElementTag, list);
	rWriter.EndElement();

	rWriter.EndElement();
}

HRESULT SVFileNameValueObjectClass::SetObjectValue( const SVString& p_rValueName, const _variant_t& p_rVariantValue )
{
	HRESULT hr = S_OK;

	CString  csTemp;

	if( p_rValueName == _T( "Default" ) )
	{
		if( p_rVariantValue.vt == VT_BSTR )
		{
			DefaultValue() = static_cast< LPCTSTR >( _bstr_t( p_rVariantValue ) );

			m_svFileName.SetDefaultFullFileName( DefaultValue() );
		}
		else
		{
			hr = E_FAIL;
		}
	}
	// new-style: store all array elements:
	else if( p_rValueName == _T( "Array_Elements" ) )
	{
		if ( ArraySize() == 1 && p_rVariantValue.vt == VT_BSTR )
		{
			ScalarBucket(1) = static_cast< LPCTSTR >( _bstr_t( p_rVariantValue ) );

			m_iLastSetIndex = 1;

			SVFileNameManagerClass SVFileManager;

			m_svFileName.SetFullFileName(ScalarBucket(1));
			SVFileManager.LoadItem(&m_svFileName);
		}
		else
		{
			hr = E_FAIL;
		}
	}
	else
	{
		hr = base::SetObjectValue( p_rValueName, p_rVariantValue );
	}

	return hr;
}

HRESULT SVFileNameValueObjectClass::SetObjectValue(SVObjectAttributeClass* pDataObject)
{
	HRESULT hr  = S_FALSE;
	BOOL	  bOk = FALSE;
	
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
			
			SVRemoveEscapedSpecialCharacters(DefaultValue(), false);
		}
		
		GetDefaultValue(csTemp);
		SetDefaultValue(csTemp, FALSE);
	}
	else if ( bOk = pDataObject->GetAttributeData(BUCKET_TAG_LOAD, l_Buckets, DefaultValue() ) )
	{
		for (size_t i = 0; i < l_Buckets.size(); i++)
		{
			SVRemoveEscapedSpecialCharacters(l_Buckets[i][0], false);
		}

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

		GetValue(m_iLastSetIndex, csTemp);
		SetValue(m_iLastSetIndex, csTemp);
	}
	// new-style: store all array elements:
	else if ( bOk = pDataObject->GetArrayData(ARRAY_TAG_LOAD, l_Array, DefaultValue() ) )
	{
		for (size_t i = 0; i < l_Array.size(); i++)
		{
			// Remove any escapes
			SVRemoveEscapedSpecialCharacters(l_Array[i], false);
		}

		SetArraySize( static_cast<int>(l_Array.size()) );
		if ( ArraySize() == 1 )
		{
			ScalarBucket(1)=l_Array[0];
		}
		else
		{
			std::swap(Array(1), l_Array);
		}
		m_iLastSetIndex = 1;

		CString strTemp;
		GetValue(m_iLastSetIndex, strTemp);
		SetValue(m_iLastSetIndex, strTemp);
	}
	else if ( bOk = pDataObject->GetAttributeData("StrDefault", svArray) )
	{
		for (int i = 0; i < svArray.GetSize(); i++)
		{
			DefaultValue() = svArray[i];
			
			// Remove any escapes
			SVRemoveEscapedSpecialCharacters(DefaultValue(), false);
		}
		
		GetDefaultValue(csTemp);
		SetDefaultValue(csTemp, FALSE);
	}
	else if ( bOk = pDataObject->GetAttributeData("StrArray", l_Buckets, DefaultValue() ) )
	{
		for (size_t i = 0; i < l_Buckets.size(); i++)
		{
			// Remove any escapes
			SVRemoveEscapedSpecialCharacters(l_Buckets[i][0], false);
		}
		
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
		
		GetValue(m_iLastSetIndex, csTemp);
		SetValue(m_iLastSetIndex, csTemp);
	}
	else
	{
		hr = base::SetObjectValue(pDataObject);
		return hr;
	}
	
	hr = bOk ? S_OK : S_FALSE;
	return hr;
}

HRESULT SVFileNameValueObjectClass::SetValueAt(int iBucket, int iIndex, CString strValue)
{
	HRESULT hr = ValidateIndexes(iBucket, iIndex);

	if ( hr == S_OK || hr == SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE )
	{
		SVFileNameManagerClass SVFileManager;

		m_svFileName.SetFullFileName(strValue);
		SVFileManager.LoadItem(&m_svFileName);
		hr = base::SetValueAt(iBucket, iIndex, m_svFileName.GetFullFileName());
	}
	
	return hr;
}

HRESULT SVFileNameValueObjectClass::SetValueAt( int iBucket, int iIndex, const VARIANT& rvtValue )
{
	if ( rvtValue.vt == VT_BSTR )
		return SetValueAt(iBucket, iIndex, CString(rvtValue.bstrVal) );
	ASSERT(FALSE);
	return S_FALSE;
}


HRESULT SVFileNameValueObjectClass::SetDefaultValue(CString strValue, bool bResetAll)
{
	m_svFileName.SetDefaultFullFileName(strValue);
	
	return base::SetDefaultValue(strValue, bResetAll);
}


HRESULT SVFileNameValueObjectClass::GetValueAt( int iBucket, int iIndex, VARIANT& rvtValue ) const
{
	CString strVal;
	_variant_t l_Temp;
	l_Temp.Attach( rvtValue );
	HRESULT hr = GetValueAt( iBucket, iIndex, strVal );
	if( hr == S_OK )
	{
		l_Temp = strVal;
	}
	else
	{
		l_Temp.Clear();
	}
	rvtValue = l_Temp.Detach();

	return hr;
}

HRESULT SVFileNameValueObjectClass::GetValueAt( int iBucket, int iIndex, double& rValue ) const
{
	ASSERT(FALSE);
	return S_FALSE;
}

void SVFileNameValueObjectClass::LocalInitialize()
{
	outObjectInfo.ObjectTypeInfo.ObjectType = SVStringValueObjectType;
	//StrDefault = _T("");
	
	ObjectAttributesAllowedRef() = SV_VIEWABLE | SV_PUBLISHABLE | SV_ARCHIVABLE | SV_EMBEDABLE | SV_PRINTABLE | SV_CH_VALUE | SV_DD_VALUE;

	m_strTypeName = "FileName";
	
	InitializeBuckets();

	SVFileNameManagerClass svFileManager;
	svFileManager.AddItem(&m_svFileName);
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVFileNameValueObjectClass.cpp_v  $
 * 
 *    Rev 1.2   01 Oct 2013 14:19:28   tbair
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
 *    Rev 1.0   23 Apr 2013 10:38:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   21 Jan 2013 10:59:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated missing SetObjectValue and GetObjectValue methods based on 6.01 branch changes for the shift tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   04 Sep 2012 15:00:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add new scripting and object creation/update funcitonality to the object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   01 Sep 2011 10:55:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  737
 * SCR Title:  Fix serialization bug associated with control characters in strings
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified SVRemoveEscapeCharacters and SVAddEscapeCharacters.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   27 Jan 2011 11:06:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   28 Oct 2010 13:44:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Dec 2009 10:24:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Apr 2008 14:41:18   JSpila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source to split the value objects into individual files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
