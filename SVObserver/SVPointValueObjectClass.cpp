//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPointValueObjectClass
//* .File Name       : $Workfile:   SVPointValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   01 Aug 2013 11:21:42  $
//******************************************************************************

#include "stdafx.h"
#include "SVPointValueObjectClass.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVOMFCLibrary/SVOMFCLibraryGlobals.h" // for SV_FALSE;
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

SV_IMPLEMENT_CLASS(SVPointValueObjectClass, SVPointValueObjectClassGuid);

SVPointValueObjectClass::SVPointValueObjectClass(LPCTSTR ObjectName)
				        : base(ObjectName) 
{
	LocalInitialize();
}


SVPointValueObjectClass::SVPointValueObjectClass(SVObjectClass* POwner, int StringResourceID)
				        : base(POwner, StringResourceID) 
{
	LocalInitialize();
}

SVPointValueObjectClass::SVPointValueObjectClass( const SVPointValueObjectClass& rhs )
	: base()
{
	LocalInitialize();
	*this = rhs;
}

const SVPointValueObjectClass& SVPointValueObjectClass::operator = (const SVPointValueObjectClass& rhs)
{
	base::operator = (rhs);
	return *this;
}

SVPointValueObjectClass::~SVPointValueObjectClass()
{
	// Delete all object's data arrays...
	SetObjectDepth(0);
	CreateBuckets();
}

BOOL SVPointValueObjectClass::CreateObject( SVObjectLevelCreateStruct* pCreateStructure )
{
	BOOL bCreate = base::CreateObject( pCreateStructure );
	SetBits( ObjectAttributesAllowedRef(), SV_CH_CONDITIONAL, false);	// Points not allowed as conditional history conditionals
	return bCreate;
}

#ifdef USE_OBJECT_SCRIPT
void SVPointValueObjectClass::GetObjectScript(CString& rstrScript, CString& rstrAliasTable, int iIndent)
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
	tmp.Format("POINT %s %d, %d %s", _T("[ "), DefaultValue().x(), DefaultValue().y(), _T(" ]"));
	
	// Name is delimited by single quotes - SEJ july 23,1999
	script += strIndent + objectTag + DEFAULT_TAG_SAVE + nameStrDelimiter + _T(" = ") + tmp + _T(";");
	
	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	
	// Name is delimited by single quotes - SEJ july 23,1999
	script += strIndent + objectTag + ARRAY_TAG_SAVE + nameStrDelimiter + _T(" = POINT ") + _T("[ ");
	
	// for all elements in the array (objectDepth)
	for (int i = 0; i < m_iArraySize; i++)
	{
		if (i)
			script += _T(", ");
		
		tmp.Format("%d, %d", Element(m_iLastSetIndex, i).x(), Element(m_iLastSetIndex, i).y());
		script +=  tmp;
	}
	script += _T(" ]");
	
	// Add termination...
	script += _T(";");
	
	script.FreeExtra();
	
	rstrScript += script;
}
#endif

void SVPointValueObjectClass::Persist(SVObjectWriter& rWriter)
{
	rWriter.StartElement(GetObjectName()); // use internal name for node name

	// Get the Heading (Class Info)
	SVValueObjectClass::Persist(rWriter);
	
	// Get the Data Values (Member Info, Values)
	CString tmp;
	tmp.Format(_T("%d, %d"), DefaultValue().x(), DefaultValue().y());
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
		tmp.Format(_T("%d, %d"), Element(m_iLastSetIndex, i).x(), Element(m_iLastSetIndex, i).y());
		value.SetString(tmp);
		list.push_back(value);
		value.Clear();
	}
	rWriter.WriteAttribute(scElementTag, list);
	rWriter.EndElement();

	rWriter.EndElement();
}

HRESULT SVPointValueObjectClass::GetDefaultValue(POINT& rPoint)
{
	SVPOINT value;
	HRESULT hrOk = base::GetDefaultValue(value);
	rPoint = value;
	return hrOk;
}

HRESULT SVPointValueObjectClass::SetValueAt( int iBucket, int iIndex, const POINT value )
{
	return base::SetValueAt(iBucket, iIndex, SVPOINT(value));
}

HRESULT SVPointValueObjectClass::SetValueAt( int iBucket, int iIndex, double value )
{
	ASSERT(FALSE);
	return S_FALSE;
}

HRESULT SVPointValueObjectClass::SetValueAt( int iBucket, int iIndex, CString strValue )
{
	CString strLegalChars (strValue);
	StringMunge::KeepChars( &strLegalChars, _T("0123456789()-, ") );	// only integers
	if ( strLegalChars == strValue )
	{
		StringMunge::StripChars( &strLegalChars, _T("()") );
		int iComma = strLegalChars.Find(_T(','));
		if ( iComma > 0 && iComma < strLegalChars.GetLength() - 1 )
		{
			CString sX = strLegalChars.Left( iComma );
			CString sY = strLegalChars.Mid( iComma + 1 );
			SVPOINT ptValue (atol(sX), atol(sY));
			return base::SetValueAt(iBucket, iIndex, ptValue );
		}
	}
	ASSERT(FALSE);
	return S_FALSE;
}

HRESULT SVPointValueObjectClass::GetValueAt( int iBucket, int iIndex, POINT& rPoint) const
{
	SVPOINT value;

	HRESULT hr = base::GetValueAt(iBucket, iIndex, value);
	rPoint = value;

	return hr;
}

HRESULT SVPointValueObjectClass::GetValueAt( int iBucket, int iIndex, CString& rstrValue) const
{
	SVPOINT value;
	
	HRESULT hr = base::GetValueAt(iBucket, iIndex, value);
	rstrValue.Format(_T("( %d, %d)"), value.x(), value.y());

	return hr;
}

HRESULT SVPointValueObjectClass::GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const
{
	SVPOINT l_Point;
	_variant_t l_Temp;
	l_Temp.Attach( rValue );

	HRESULT hr = base::GetValueAt( iBucket, iIndex, l_Point );
	if( hr == S_OK )
	{
		CString l_strTmp;
		l_strTmp.Format(  _T( "( %d, %d )"),l_Point.x(), l_Point.y());
		l_Temp = l_strTmp;
	}
	else
	{
		l_Temp.Clear();
	}
	rValue = l_Temp.Detach();

	return hr;
}

HRESULT SVPointValueObjectClass::GetValueAt( int iBucket, int iIndex, double& rValue ) const
{
	ASSERT(FALSE);
	return S_FALSE;
}

void SVPointValueObjectClass::LocalInitialize()
{
	outObjectInfo.ObjectTypeInfo.ObjectType = SVPointValueObjectType;
	//DefaultValue().x = 0;
	//DefaultValue().y = 0;
	if ( m_sLegacyScriptDefaultName.IsEmpty() )
	{
		m_sLegacyScriptDefaultName = "defaultPoint";
		m_sLegacyScriptArrayName = "pArray";
	}	
	ObjectAttributesAllowedRef() = SV_VIEWABLE | SV_ARCHIVABLE | SV_EMBEDABLE | SV_PRINTABLE | SV_CH_VALUE | SV_DD_VALUE;

	m_strTypeName = "Point";
	InitializeBuckets();
}

