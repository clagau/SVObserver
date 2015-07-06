//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVVariantValueObjectClass
//* .File Name       : $Workfile:   SVVariantValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   22 Apr 2014 07:05:58  $
//******************************************************************************

#include "stdafx.h"
#include "SVVariantValueObjectClass.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVGlobal.h"
#include "SVObjectLibrary/GlobalConst.h"

namespace	// only for this file
{
	const CString DEFAULT_TAG_SAVE(_T(".Default"));
	const CString BUCKET_TAG_SAVE(_T(".Array"));	// for backwards compatibility
	const CString ARRAY_TAG_SAVE(_T(".Array_Elements"));	// new style; one bucket, all array values

	const CString DEFAULT_TAG_LOAD(_T("Default"));
	const CString BUCKET_TAG_LOAD(_T("Array"));	// for backwards compatibility
	const CString ARRAY_TAG_LOAD(_T("Array_Elements"));	// new style; one bucket, all array values
}	// end file scope namespace

SV_IMPLEMENT_CLASS(SVVariantValueObjectClass, SVVariantValueObjectClassGuid);

SVVariantValueObjectClass::SVVariantValueObjectClass(LPCTSTR ObjectName)
				       : base(ObjectName) 
{
	LocalInitialize();
}


SVVariantValueObjectClass::SVVariantValueObjectClass(SVObjectClass* POwner, int StringResourceID)
				       : base(POwner, StringResourceID) 
{
	LocalInitialize();
}


SVVariantValueObjectClass::SVVariantValueObjectClass( const SVVariantValueObjectClass& rhs )
	: base()
{
	LocalInitialize();
	*this = rhs;
}

const SVVariantValueObjectClass& SVVariantValueObjectClass::operator = (const SVVariantValueObjectClass& rhs )
{
	base::operator = (rhs);
	return *this;
}

SVVariantValueObjectClass::~SVVariantValueObjectClass()
{
	// Delete all object's data arrays...
	SetObjectDepth(0);
	CreateBuckets();
}

#ifdef USE_OBJECT_SCRIPT
void SVVariantValueObjectClass::GetObjectScript(CString& rStrScript, CString& rStrAliasTable, int iIndent)
{
	CString script;
	
	// preallocate 4K
	script.GetBuffer(4096);  
	script.ReleaseBuffer(); 
	
	// Get the Heading (Class Info)
	SVValueObjectClass::GetObjectScript(script, rStrAliasTable, iIndent);
	
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
	CString strObjectTag = nameStrDelimiter + _T("_object_ID_") + GetName();
	// GetObjectName();
	
	// Get the Data Values (Member Info, Values)
	CString sTemp;
	CString sDefaultValue = ToString(DefaultValue(), true);
	sTemp.Format("VARIANT %s", sDefaultValue );
	
	// Name is delimited by single quotes - SEJ july 23,1999
	script += strIndent + strObjectTag + DEFAULT_TAG_SAVE + nameStrDelimiter + _T(" = ") + sTemp + _T(";");
	
	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	
	// Name is delimited by single quotes - SEJ july 23,1999
	script += strIndent + strObjectTag + ARRAY_TAG_SAVE + _T("'") + _T(" = VARIANT ") + _T("[ ");
	
	// for all elements in the array (m_iNumberOfBuckets)
	for (int i = 0; i < m_iArraySize; i++)
	{
		if (i)
			script += _T(", ");
		
		sTemp = ToString(Element(m_iLastSetIndex, i), true);
		script +=  sTemp;
	}
	script += _T(" ]");
	
	// Add termination...
	script += _T(";");
	
	script.FreeExtra();
	
	rStrScript += script;
}
#endif

void SVVariantValueObjectClass::Persist(SVObjectWriter& rWriter)
{
	rWriter.StartElement(GetObjectName()); // use internal name for node name

	SVValueObjectClass::Persist(rWriter);
	
	// Get the Data Values (Member Info, Values)
	_variant_t value(DefaultValue());
	rWriter.WriteAttribute( scDefaultTag, value  );
	
	rWriter.StartElement(scArrayElementsTag);

	// Object Depth is implicit (it's the count of the values)
	SVVariantList list;

	// for all elements in the array (m_iNumberOfBuckets)
	for (int i = 0; i < m_iArraySize; i++)
	{
		value = Element(m_iLastSetIndex, i);

		// The parser does not like reading in empty safe array.
		// Therefore if an empty array is detected then set the variant type to VT_EMPTY.
		// 
		if( (value.vt & VT_ARRAY) == VT_ARRAY)
		{
			bool bEmpty = true;
			unsigned int dim = ::SafeArrayGetDim(value.parray);
			long lBound = 0;
			long uBound = 0;
			if( dim > 0 )
			{
				HRESULT hr = ::SafeArrayGetLBound( value.parray,1,&lBound);
				hr = ::SafeArrayGetUBound( value.parray, 1, &uBound);
				long lSize = uBound-lBound+1;
				if(hr == S_OK && lSize > 0)
				{
					bEmpty = false;
				}
			}
			if( bEmpty)
			{
				value.Clear();
			}
		}
		list.push_back( value );
	}
	rWriter.WriteAttribute( scElementTag, list );
	rWriter.EndElement();

	rWriter.EndElement();
}

HRESULT SVVariantValueObjectClass::SetObjectValue(SVObjectAttributeClass* pDataObject)
{
	HRESULT hr = S_FALSE;
	BOOL bOk = FALSE;
	SvCl::SVObjectVariantArrayClass svArray;
	bucket_type l_Buckets(BucketsNoAssert());
	array_type l_Array;
	
	if ( bOk = pDataObject->GetAttributeData(DEFAULT_TAG_LOAD, svArray) )
	{
		for (int i = 0; i < svArray.GetSize(); i++)
		{
			DefaultValue() = svArray[i];
		}
	}
	else if ( bOk = pDataObject->GetAttributeData(BUCKET_TAG_LOAD, l_Buckets, DefaultValue() ) )
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
	// new-style: store all array elements:
	else if ( bOk = pDataObject->GetArrayData(ARRAY_TAG_LOAD, l_Array, DefaultValue() ) )
	{
		for ( size_t i = 0; i < l_Array.size(); i++ )
		{
			_variant_t& l_rvt = l_Array[ i ];
			if( l_rvt.vt == VT_BSTR )
			{
				CString strTmp ( l_rvt.bstrVal );
				if ( SVRemoveEscapedSpecialCharacters(strTmp, true) )
				{
					l_rvt = strTmp;
				}
			}
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
	}
	else if ( bOk = pDataObject->GetAttributeData("m_vtDefault", svArray) )
	{
//		for (int i = 0; i < svArray.GetSize(); i++)
		{
			//m_vtDefault = svArray[i];
			if ( svArray.GetSize() > 0 )
				DefaultValue() = svArray[svArray.GetSize()-1];
		}
	}
	else if ( bOk = pDataObject->GetAttributeData("m_pavtArray", l_Buckets, DefaultValue() ) )
	{
		for( size_t i = 0 ; i < l_Buckets.size(); i++ )
		{
			_variant_t& l_rvt = l_Buckets[i][0];
			if( l_rvt.vt == VT_BSTR )
			{
				CString strTmp ( l_rvt.bstrVal );
				if ( SVRemoveEscapedSpecialCharacters( strTmp, true ) )
				{
					l_rvt = strTmp;
				}
			}
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
	}
	else
	{
		hr = base::SetObjectValue(pDataObject);
		return hr;
	}

	hr = bOk ? S_OK : S_FALSE;
	return hr;
}

HRESULT SVVariantValueObjectClass::SetValueAt( int iBucket, int iIndex, const VARIANT& rvtValue )
{
	return base::SetValueAt(iBucket, iIndex, _variant_t(rvtValue));
}

HRESULT SVVariantValueObjectClass::SetValueAt(int iBucket, int iIndex, const BOOL bValue)
{
	HRESULT hr = ValidateIndexes(iBucket, iIndex);

	if ( hr == S_OK || hr == SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE )
	{
		Element(iBucket, iIndex).Clear();
		Element(iBucket, iIndex).ChangeType(VT_BOOL);
		Element(iBucket, iIndex) = (short) bValue;
		m_iLastSetIndex  = iBucket;

		hr = S_OK;
	}
	
	return hr;
}

HRESULT SVVariantValueObjectClass::SetValueAt( int iBucket, int iIndex, const double dValue )
{
	return base::SetValueAt( iBucket, iIndex, _variant_t(dValue) );
}

HRESULT SVVariantValueObjectClass::SetValueAt( int iBucket, int iIndex, const BYTE byValue )
{
	return base::SetValueAt( iBucket, iIndex, _variant_t(byValue) );
}

HRESULT SVVariantValueObjectClass::SetValueAt( int iBucket, int iIndex, const long lValue )
{
	return base::SetValueAt( iBucket, iIndex, _variant_t(lValue) );
}

HRESULT SVVariantValueObjectClass::SetValueAt( int iBucket, int iIndex, const DWORD dwValue )
{
	HRESULT hr = ValidateIndexes(iBucket, iIndex);

	if ( hr == S_OK || hr == SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE )
	{
		Element(iBucket, iIndex).Clear();
		Element(iBucket, iIndex).ChangeType(VT_UI4);
		Element(iBucket, iIndex).ulVal = dwValue;
		m_iLastSetIndex  = iBucket;
		
		hr = S_OK;
	}
	return hr;
}

HRESULT SVVariantValueObjectClass::SetValueAt( int iBucket, int iIndex, CString strValue )
{
	return base::SetValueAt( iBucket, iIndex, _variant_t(strValue) );
}

VARTYPE SVVariantValueObjectClass::GetDefaultType( ) const
{
	return DefaultValue().vt;
}


BOOL SVVariantValueObjectClass::SetType( int vt )
{
	if ( DefaultValue().vt != vt )
	{
		DefaultValue().Clear();
		DefaultValue().ChangeType( vt );
	}
	if ( ArraySize() == 1 )
	{
		for (int i = 0; i < m_iNumberOfBuckets; ++ i)
		{
			if ( ScalarBucket(i).vt != vt )
			{
				ScalarBucket(i).Clear();
				ScalarBucket(i).ChangeType( vt );
			}
		}
	}
	else
	{
		for (int i = 0; i < m_iNumberOfBuckets; i++)
		{
			for (int j=0; j < ArraySize(); j++)
			{
				if ( Element(i, j).vt != vt )
				{
					Element(i, j).Clear();
					Element(i, j).ChangeType( vt );
				}
			}
		}
	}
	return TRUE;
}

HRESULT SVVariantValueObjectClass::SetValueKeepType( int iBucket, int iIndex, LPCTSTR strValue )
{
	HRESULT hr = S_OK;

	_variant_t vtTemp;
	vtTemp = strValue;

	if( DefaultValue().vt != VT_EMPTY )
	{
		hr = ::VariantChangeType( &vtTemp, &vtTemp, 0, DefaultValue().vt );
	}

	if( hr == S_OK )
	{
		hr = base::SetValueAt( iBucket, iIndex, vtTemp );
	}

	return hr;
}

HRESULT SVVariantValueObjectClass::SetDefaultValue(const VARIANT& rvtValue, bool bResetAll)
{
	//ASSERT((!m_iNumberOfBuckets <= 0) || (m_iNumberOfBuckets > 0));
	
	HRESULT hr = ::VariantCopy( &(DefaultValue()), const_cast <VARIANT*>( &rvtValue) );
	if ( bResetAll )
	{
		if ( ArraySize() == 1 )
		{
			for (int i = 0; i < m_iNumberOfBuckets; ++ i)
			{
				ScalarBucket(i) = rvtValue;
			}
		}
		else
		{
			for (int i = 0; i < m_iNumberOfBuckets; ++ i)
			{
				for (size_t j=0; j < Buckets()[i].size(); ++j)
				{
					Buckets()[i][j] = rvtValue;
				}
			}
		}
	}
	return hr;
}

HRESULT SVVariantValueObjectClass::GetDefaultValue(_variant_t& rvtValue)
{
	HRESULT hr = ::VariantCopy( &rvtValue, &(DefaultValue()) );
	return IsCreated() ? hr : S_FALSE;
}

HRESULT SVVariantValueObjectClass::GetValueAt( int iBucket, int iIndex, VARIANT& rvtValue) const
{
	HRESULT hr = ValidateIndexes(iBucket,iIndex);

	if ( hr == S_OK )
	{
		hr = ::VariantCopy( &rvtValue, const_cast <_variant_t*> (&(Element(iBucket, iIndex))) );
		isObjectValid = TRUE;
	}
	else if ( hr == SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE )
	{
		rvtValue = DefaultValue();
		isObjectValid = TRUE;
	}
	else	//	BAD INDEX
	{
		rvtValue = DefaultValue();
		isObjectValid = FALSE;
	}

	return hr;
}

HRESULT SVVariantValueObjectClass::GetValueAt( int iBucket, int iIndex, BOOL& rbValue) const
{
	HRESULT hr = ValidateIndexes(iBucket,iIndex);

	if ( hr == S_OK )
	{
		isObjectValid = TRUE;
		_variant_t vtTemp = Element(iBucket, iIndex);
		switch ( vtTemp.vt )
		{
			case VT_BSTR:
			{
				CString sTemp(vtTemp.bstrVal);
				if ( sTemp.CompareNoCase(_T("TRUE")) == 0 )
				{
					rbValue = TRUE;
					hr = S_OK;
				}
				else if ( sTemp.CompareNoCase(_T("FALSE")) == 0 )
				{
					rbValue = FALSE;
					hr = S_OK;
				}
				else if ( sTemp.CompareNoCase(_T("1")) == 0 )
				{
					rbValue = TRUE;
					hr = S_OK;
				}
				else if ( sTemp.CompareNoCase(_T("0")) == 0 )
				{
					rbValue = FALSE;
					hr = S_OK;
				}
				else
				{
					rbValue = FALSE;
				}
			}
			break;

			default:
			{
				hr = ::VariantChangeType(&vtTemp, &vtTemp, 0, VT_BOOL);
				//vtTemp.ChangeType(VT_BOOL);
				if ( hr == S_OK )
				{
					rbValue = vtTemp.boolVal;
				}
				else
				{
					if ( DefaultValue().vt == VT_BOOL )
						rbValue = DefaultValue().boolVal;
					else
						rbValue = FALSE;
				}
			}
		}// end switch ( vtTemp.vt )
	}// end if (iBucket >= 0 && iBucket < m_iNumberOfBuckets)
	else
	{
		if ( DefaultValue().vt == VT_BOOL )
			rbValue = DefaultValue().boolVal;
		else
			rbValue = FALSE;
		isObjectValid = (hr == SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE);
	}

	return hr;
}


HRESULT SVVariantValueObjectClass::GetValueAt( int iBucket, int iIndex, CString& rstrValue) const
{
	HRESULT hr = ValidateIndexes(iBucket,iIndex);

	_variant_t vtTemp;
	if ( hr == S_OK )
	{
		vtTemp = Element(iBucket, iIndex);
		isObjectValid = TRUE;
	}
	else
	{
		vtTemp = DefaultValue();
		isObjectValid = (hr == SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE);
	}

	switch ( vtTemp.vt )
	{
		case VT_BSTR:
		{
			rstrValue = vtTemp.bstrVal;
			hr = S_OK;
		}
		break;

		default:
		{
			VARTYPE vtOrig = vtTemp.vt;
			hr = ::VariantChangeType(&vtTemp, &vtTemp, VARIANT_ALPHABOOL, VT_BSTR);
			if ( hr == S_OK )
			{
				rstrValue = vtTemp.bstrVal;
				if( vtOrig == VT_BOOL )
				{
					rstrValue.MakeUpper();
				}
			}
			else
			{
				rstrValue.Empty();
			}
		}
	}// end switch ( vtTemp.vt )

	return hr;
}

HRESULT SVVariantValueObjectClass::GetValueAt( int iBucket, int iIndex, double& rdValue) const
{
	HRESULT hr = ValidateIndexes(iBucket, iIndex);

	_variant_t vtTemp;
	if ( hr == S_OK )
	{
		vtTemp = Element(iBucket, iIndex);
		isObjectValid = TRUE;
	}
	else
	{
		vtTemp = DefaultValue();
		isObjectValid = (hr == SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE);
	}

	HRESULT hrChange = ::VariantChangeType(&vtTemp, &vtTemp, 0, VT_R8);
	if ( hrChange == S_OK )
	{
		rdValue = vtTemp.dblVal;
	}
	else
	{
		rdValue = 0;
		if ( hr == S_OK )
			hr = hrChange;
	}

	return hr;
}

HRESULT SVVariantValueObjectClass::GetValueAt( int iBucket, int iIndex, long& rlValue ) const
{
	HRESULT hr = ValidateIndexes(iBucket, iIndex);

	_variant_t vtTemp;
	if ( hr == S_OK )
	{
		vtTemp = Element(iBucket, iIndex);
		isObjectValid = TRUE;
	}
	else
	{
		vtTemp = DefaultValue();
		isObjectValid = (hr == SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE);
	}

	HRESULT hrChange = ::VariantChangeType(&vtTemp, &vtTemp, 0, VT_I4);
	if ( hrChange == S_OK )
	{
		rlValue = vtTemp.lVal;
	}
	else
	{
		rlValue = 0;

		if ( hr == S_OK )
			hr = hrChange;
	}

	return hr;
}

HRESULT SVVariantValueObjectClass::GetValueAt( int iBucket, int iIndex, DWORD& rdwValue ) const
{
	HRESULT hr = ValidateIndexes(iBucket, iIndex);

	_variant_t vtTemp;
	if ( hr == S_OK )
	{
		vtTemp = Element(iBucket, iIndex);
		isObjectValid = TRUE;
	}
	else
	{
		vtTemp = DefaultValue();
		isObjectValid = (hr == SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE);
	}

	HRESULT hrChange = ::VariantChangeType(&vtTemp, &vtTemp, 0, VT_UI4);
	if ( hrChange == S_OK )
	{
		rdwValue = vtTemp.ulVal;
	}
	else
	{
		rdwValue = 0;
		if ( hr == S_OK )
			hr = hrChange;
	}
	return hr;
}


HRESULT SVVariantValueObjectClass::GetValueAt( int iBucket, int iIndex, BYTE& rbyValue ) const
{
	HRESULT hr = ValidateIndexes(iBucket, iIndex);

	_variant_t vtTemp;
	if ( hr == S_OK )
	{
		vtTemp = Element(iBucket, iIndex);
		isObjectValid = TRUE;
	}
	else
	{
		vtTemp = DefaultValue();
		isObjectValid = (hr == SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE);
	}

	HRESULT hrChange = ::VariantChangeType(&vtTemp, &vtTemp, 0, VT_UI1);
	if ( hrChange == S_OK )
	{
		rbyValue = vtTemp.bVal;
	}
	else
	{
		rbyValue = 0;
		if ( hr == S_OK )
			hr = hrChange;
	}

	return hr;
}

CString SVVariantValueObjectClass::ToString(const VARIANT& rvt, bool bScript )
{
	CString sTemp;
	_variant_t vt = rvt;


	switch ( vt.vt )
	{
		case VT_EMPTY:
		{
			if ( bScript )
			{
				sTemp.Format(_T("%d, "), vt.vt);
				sTemp += _T("0");
			}
			break;
		}

		case VT_BSTR:
		{
			if ( bScript )
			{
				CString s(rvt.bstrVal);
				::SVAddEscapeSpecialCharacters( s, true );
				CString sTemp1;
				sTemp1.Format(_T("\"%s\""), s);
				sTemp.Format(_T("%d, %s"), vt.vt, sTemp1);
			}
			else
			{
				sTemp += rvt.bstrVal;
			}
		}
		break;

		default:
		{
			if( (vt.vt & VT_ARRAY) == VT_ARRAY)
			{
				if( bScript)
				{
					sTemp.Format(_T("%d, 0"), VT_EMPTY);
				}
			}
			else
			{
				VARTYPE l_OldType = vt.vt;
				HRESULT hr = ::VariantChangeTypeEx(&vt, &vt, SvOl::LCID_USA, VARIANT_ALPHABOOL, VT_BSTR);	// use United States locale
				if ( hr == S_OK )
				{
					if( bScript)
					{
						sTemp.Format(_T("%d, "), l_OldType);
					}

					sTemp += vt.bstrVal;
					if( rvt.vt == VT_BOOL )
					{
						sTemp.MakeUpper();
					}
				}
				else
				{
					if( bScript)
					{
						sTemp.Format(_T("%d, 0"), VT_EMPTY);
					}
				}
			}
		}
	}// end switch ( rvt.vt )

	return sTemp;
}

void SVVariantValueObjectClass::LocalInitialize()
{
	outObjectInfo.ObjectTypeInfo.ObjectType = SVVariantValueObjectType;
	DefaultValue().Clear();

	m_strTypeName = "Variant";

	InitializeBuckets();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVVariantValueObjectClass.cpp_v  $
 * 
 *    Rev 1.2   22 Apr 2014 07:05:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  901
 * SCR Title:  Fix configuration loading problem with variant value object
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified the Persist function to save empty variant value objects that were of type VT_ARRAY with a type of VT_EMPTY.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Oct 2013 08:39:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 16:19:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   04 Sep 2012 15:20:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add new scripting and object creation/update funcitonality to the object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   05 Mar 2012 15:38:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  756
 * SCR Title:  Fix Variant Array Object Script Issue
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added logic to the ToString function to handle the default case that will take care of variant arrays.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   01 Sep 2011 10:55:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  737
 * SCR Title:  Fix serialization bug associated with control characters in strings
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified SVRemoveEscapeCharacters and SVAddEscapeCharacters.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   30 Mar 2011 16:36:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix performance problems with the extent changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   18 Mar 2011 07:46:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix performance issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   27 Jan 2011 12:04:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   05 Nov 2010 11:13:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove redundent data objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   03 Nov 2010 09:56:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update source so that if the default value of the variant value object is not set, it uses the incoming type for the string conversion.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   28 Oct 2010 14:27:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Dec 2009 13:31:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Apr 2008 14:59:22   JSpila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source to split the value objects into individual files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
