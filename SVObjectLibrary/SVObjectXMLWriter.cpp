//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectXMLWriter
//* .File Name       : $Workfile:   SVObjectXMLWriter.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   19 Dec 2014 02:47:46  $
//******************************************************************************

#include "stdafx.h"
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <boost/foreach.hpp>
//Moved to precompiled header: #include <boost/assign/list_of.hpp>
#include "SVObjectXMLWriter.h"
#include "SVUtilityLibrary/SVStringConversions.h"
#include "SVXMLLibrary/SVXMLLibraryGlobals.h"
#include "SVUtilityLibrary/SVSafeArray.h"

#define Stringtize(s) (#s)

static SVString scNodeTag(_T("NODE"));
static SVString scDataTag(_T("DATA"));
static SVString scNameTag(_T("Name"));
static SVString scTypeTag(_T("Type"));

static SVString scSchemaTag(_T("Schema"));
static SVString scElementTypeTag(_T("ElementType"));
static SVString scAttributeTypeTag(_T("AttributeType"));
static SVString scElementTag(_T("element"));
static SVString scAttributeTag(_T("attribute"));

typedef std::map<VARTYPE, std::string> VariantTypeToStringLookup;

static VariantTypeToStringLookup var_types = boost::assign::map_list_of<>
(VT_EMPTY, Stringtize(VT_EMPTY))	// nothing
(VT_NULL, Stringtize(VT_NULL))		// SQL style Null
(VT_I2, Stringtize(VT_I2))			// 2 byte signed int
(VT_I4, Stringtize(VT_I4))			// 4 byte signed int
(VT_INT, Stringtize(VT_INT))			// 4 byte signed int
(VT_R4, Stringtize(VT_R4))			// 4 byte real
(VT_R8, Stringtize(VT_R8))			// 8 byte real
// not allowed (VT_CY, Stringtize(VT_CY))		// currency
// not allowed (VT_DATE, Stringtize(VT_DATE))	// date
(VT_BSTR, Stringtize(VT_BSTR))	// OLE Automation string
// not allowed (VT_DISPATCH, Stringtize(VT_DISPATCH)) // IDispatch *
// not allowed (VT_ERROR, Stringtize(VT_ERROR))	// SCODE
(VT_BOOL, Stringtize(VT_BOOL))	// True=-1, False=0
// not allowed (VT_VARIANT, Stringtize(VT_VARIANT)) // VARIANT *
// not allowed (VT_UNKNOWN, Stringtize(VT_UNKNOWN)) // IUnknown *
// not allowed (VT_DECIMAL, Stringtize(VT_DECIMAL)) // 16 byte fixed point
// not allowed (VT_RECORD, Stringtize(VT_RECORD)) // user defined type
(VT_I1, Stringtize(VT_I1))			// signed char
(VT_UI1, Stringtize(VT_UI1))		// unsigned char
(VT_UI2, Stringtize(VT_UI2))		// unsigned short
(VT_UI4, Stringtize(VT_UI4))		// unsigned long
(VT_UINT, Stringtize(VT_UI4))		// unsigned long
(VT_I8, Stringtize(VT_I8))			// signed 64-bit int
(VT_UI8, Stringtize(VT_UI8))		// unsigned 64-bit int
;

static std::string BSTRToUTF8(BSTR value)
{
	return SVStringConversions::to_utf8(_bstr_t(value));
}

static std::string VariantTypeToString(VARTYPE vt)
{
	return var_types[vt];
}

static std::string VariantToString(_variant_t value)
{
	SVString strValue;

	switch (value.vt)
	{
	case VT_EMPTY:
		break;

	case VT_NULL:
		break;

	case VT_I2:
		strValue = SvUl_SF::Format("%d", value.iVal);
		break;

	case VT_INT:
	case VT_I4:
		strValue = SvUl_SF::Format("%ld", value.lVal);
		break;

	case VT_R4:
		strValue = SvUl_SF::Format("%f", value.fltVal);
		break;

	case VT_R8:
		strValue = SvUl_SF::Format("%lf", value.dblVal);
		break;

	case VT_BSTR:
		strValue = BSTRToUTF8(value.bstrVal);
		break;

	case VT_BOOL:
		strValue = SvUl_SF::Format("%s", (value.boolVal == VARIANT_TRUE) ? "TRUE" : "FALSE");
		break;

	case VT_I1:
		strValue = SvUl_SF::Format("%02X", value.cVal);
		break;

	case VT_UI1:
		strValue = SvUl_SF::Format("%02X", value.bVal);
		break;

	case VT_UI2:
		strValue = SvUl_SF::Format("%u", value.uiVal);
		break;

	case VT_UINT:
	case VT_UI4:
		strValue = SvUl_SF::Format("%lu", value.ulVal);
		break;

	case VT_I8:
		strValue = SvUl_SF::Format("%I64d", value.llVal);
		break;

	case VT_UI8:
		strValue = SvUl_SF::Format("%I64u", value.ullVal);
		break;

	default:
		break;
	}
	return std::string(strValue.c_str());
}

static XMLElementPtr Element(const SVString& name, xml::writer& writer)
{
	XMLElementPtr pNode(new xml::element(name.c_str(), writer));
	return pNode;
}

static void Attribute(const SVString& name, const SVString& value, XMLElementPtr pNode)
{
	pNode->attr(name.c_str(), value.c_str());
}

static void Attribute(const wchar_t* name, const SVString& value, XMLElementPtr pNode)
{
	pNode->attr(SvUl_SF::createSVString(name).c_str(), value.c_str());
}

static void Attribute(const wchar_t* name, const _variant_t& value, XMLElementPtr pNode)
{
	pNode->attr(SvUl_SF::createSVString(name).c_str(), SvUl_SF::createSVString(value).c_str());
}

SVObjectXMLWriter::SVObjectXMLWriter(std::ostream& os)
{
	m_pWriter = XMLWriterPtr(new xml::writer(os));
}

SVObjectXMLWriter::~SVObjectXMLWriter()
{
	m_pWriter.reset();
}

void SVObjectXMLWriter::WriteAttribute(LPCTSTR rName, const variant_t& value)
{
	if (0 == (VT_ARRAY & value.vt))
	{
		const std::string& varTypeStr = VariantTypeToString(value.vt);
		const std::string& valueStr = VariantToString(value);

		if (!varTypeStr.empty())
		{
			xml::element data(scDataTag.c_str(), *m_pWriter);
			data.attr(scNameTag.c_str(), rName);
			data.attr(scTypeTag.c_str(), varTypeStr.c_str());
			data.contents(valueStr.c_str());
		}
		else
		{
			// throw an exception
		}
	}
	else
	{
		StartElement(rName);
		ElementAttribute("Type", "VT_ARRAY");
		SVSAFEARRAY arrayValue = value;
		for (int i=0; i < arrayValue.size(); ++i)
		{
			SVString attName = SvUl_SF::Format("DataIndex_%d", i+1);
			_variant_t lVal;
			arrayValue.GetElement( i, lVal );
			WriteAttribute(attName.c_str(), lVal);
		}		
		EndElement();
	}
}

void SVObjectXMLWriter::WriteAttribute(LPCTSTR rName, const SVVariantList& rValues)
{
	BOOST_FOREACH(_variant_t value, rValues)
	{
		WriteAttribute(rName, value);
	}
}

void SVObjectXMLWriter::StartElement(LPCTSTR rName)
{
	XMLElementPtr pNode(new xml::element(scNodeTag.c_str(), *m_pWriter));
	m_elements.push_front(pNode);
	pNode->attr(scNameTag.c_str(), rName);
}

void SVObjectXMLWriter::EndElement()
{
	m_elements.pop_front();
}

void SVObjectXMLWriter::EndAllElements()
{
	//Order is important that is why we need to do them one by one
	while( 0 < m_elements.size() )
	{
		m_elements.pop_front();
	}
}

void SVObjectXMLWriter::ElementAttribute(LPCTSTR rAttrName, const variant_t& value)
{
	m_elements[0]->attr(rAttrName, VariantToString(value));
}

void SVObjectXMLWriter::WriteRootElement(LPCTSTR rName)
{
	XMLElementPtr pNode(new xml::element(rName, *m_pWriter));
	m_elements.push_front(pNode);
}

void SVObjectXMLWriter::WriteSchema()
{
/*
	<Schema xmlns="urn:schemas-microsoft-com:xml-data" name="SVR00001" xmlns:dt="urn:schemas-microsoft-com:datatypes">
    <AttributeType name="Name" dt:type="string"/>
    <AttributeType name="Value" dt:type="string"/>
    <AttributeType name="Type" dt:type="string"/>
    <ElementType name="DATA" content="mixed" model="closed">
      <attribute type="Name" required="yes"/>
      <attribute type="Type" required="yes"/>
    </ElementType>
    <ElementType name="NODE" content="mixed" model="closed">
      <attribute type="Name" required="yes"/>
      <attribute type="Type" required="no"/>
      <element type="NODE" minOccurs="0" maxOccurs="*"/>
      <element type="DATA" minOccurs="0" maxOccurs="1"/>
    </ElementType>
    <AttributeType name="Format" dt:type="string"/>
    <AttributeType name="FormatVersion" dt:type="string"/>
    <AttributeType name="Revision" dt:type="string"/>
    <ElementType name="Revision" content="mixed" model="closed">
      <attribute type="Format" required="yes"/>
      <attribute type="FormatVersion" required="yes"/>
      <attribute type="Revision" required="yes"/>
      <element type="DATA" minOccurs="0" maxOccurs="1"/>
    </ElementType>
    <ElementType name="RevisionHistory" content="mixed" model="closed">
      <element type="Revision" minOccurs="1" maxOccurs="*"/>
    <element type="DATA" minOccurs="0" maxOccurs="1"/>
    </ElementType><AttributeType name="IsActive" dt:type="string"/>
    <ElementType name="Encryption" content="mixed" model="closed">
      <attribute type="IsActive" required="yes"/>
      <element type="DATA" minOccurs="0" maxOccurs="1"/>
    </ElementType>
  </Schema>
*/
	XMLElementPtr pSchemaNode = Element(scSchemaTag, *m_pWriter);
	Attribute("xmlns", "urn:schemas-microsoft-com:xml-data", pSchemaNode);
	Attribute("name", "SVR00001", pSchemaNode);
	Attribute("xmlns:dt", "urn:schemas-microsoft-com:datatypes", pSchemaNode);

	XMLElementPtr pNode = Element(scAttributeTypeTag, *m_pWriter);
	Attribute("name", "Name", pNode);
	Attribute("dt:type", "string", pNode);
	pNode.reset();
    
	pNode = Element(scAttributeTypeTag, *m_pWriter);
	Attribute("name", "Value", pNode);
	Attribute("dt:type", "string", pNode);
    pNode.reset();

	pNode = Element(scAttributeTypeTag, *m_pWriter);
	Attribute("name", "Type", pNode);
	Attribute("dt:type", "string", pNode);
    pNode.reset();

	pNode = Element(scElementTypeTag, *m_pWriter);
	Attribute("name", "DATA", pNode);
	Attribute("content", "mixed", pNode);
	Attribute("model", "closed", pNode);

    XMLElementPtr pAttrNode = Element(scAttributeTag, *m_pWriter);
	Attribute("type", "Name", pAttrNode);
	Attribute("required", "yes", pAttrNode);
    pAttrNode.reset();

	pAttrNode = Element(scAttributeTag, *m_pWriter);
	Attribute("type", "Type", pAttrNode);
	Attribute("required", "yes", pAttrNode);
	pAttrNode.reset(); 
    pNode.reset();

	pNode = Element(scElementTypeTag, *m_pWriter);
	Attribute("name", "NODE", pNode);
	Attribute("content", "mixed", pNode);
	Attribute("model", "closed", pNode);

	pAttrNode = Element(scAttributeTag, *m_pWriter);
	Attribute("type", "Name", pAttrNode);
	Attribute("required", "yes", pAttrNode);
	pAttrNode.reset();

	pAttrNode = Element(scAttributeTag, *m_pWriter);
	Attribute("type", "Type", pAttrNode);
	Attribute("required", "no", pAttrNode);
	pAttrNode.reset();

	XMLElementPtr pElemNode = Element(scElementTag, *m_pWriter);
	Attribute("type", "NODE", pElemNode);
	Attribute("minOccurs", "0", pElemNode);
	Attribute("maxOccurs", "*", pElemNode);
    pElemNode.reset();

	pElemNode = Element(scElementTag, *m_pWriter);
	Attribute("type", "DATA", pElemNode);
	Attribute("minOccurs", "0", pElemNode);
	Attribute("maxOccurs", "1", pElemNode);
	pElemNode.reset();
	pNode.reset();

	pNode = Element(scAttributeTypeTag, *m_pWriter);
	Attribute("name", "Format", pNode);
	Attribute("dt:type", "string", pNode);
	pNode.reset();
    
	pNode = Element(scAttributeTypeTag, *m_pWriter);
	Attribute("name", "FormatVersion", pNode);
	Attribute("dt:type", "string", pNode);
	pNode.reset();
    
	pNode = Element(scAttributeTypeTag, *m_pWriter);
	Attribute("name", "Revision", pNode);
	Attribute("dt:type", "string", pNode);
	pNode.reset();

	pNode = Element(scElementTypeTag, *m_pWriter);
	Attribute("name", "Revision", pNode);
	Attribute("content", "mixed", pNode);
	Attribute("model", "closed", pNode);
    
	pAttrNode = Element(scAttributeTag, *m_pWriter);
	Attribute("type", "Format", pAttrNode);
	Attribute("required", "yes", pAttrNode);
	pAttrNode.reset();

	pAttrNode = Element(scAttributeTag, *m_pWriter);
	Attribute("type", "FormatVersion", pAttrNode);
	Attribute("required", "yes", pAttrNode);
	pAttrNode.reset();

	pAttrNode = Element(scAttributeTag, *m_pWriter);
	Attribute("type", "Revision", pAttrNode);
	Attribute("required", "yes", pAttrNode);
	pAttrNode.reset();

	pElemNode = Element(scElementTag, *m_pWriter);
	Attribute("type", "DATA", pElemNode);
	Attribute("minOccurs", "0", pElemNode);
	Attribute("maxOccurs", "1", pElemNode);
	pElemNode.reset();
	pNode.reset();
		
    pNode = Element(scElementTypeTag, *m_pWriter);
	Attribute("name", "RevisionHistory", pNode);
	Attribute("content", "mixed", pNode);
	Attribute("model", "closed", pNode);
    
	pElemNode = Element(scElementTag, *m_pWriter);
	Attribute("type", "Revision", pElemNode);
	Attribute("minOccurs", "1", pElemNode);
	Attribute("maxOccurs", "*", pElemNode);
	pElemNode.reset();

	pElemNode = Element(scElementTag, *m_pWriter);
	Attribute("type", "DATA", pElemNode);
	Attribute("minOccurs", "0", pElemNode);
	Attribute("maxOccurs", "1", pElemNode);
	pElemNode.reset();
	pNode.reset();

	pNode = Element(scAttributeTypeTag, *m_pWriter);
	Attribute("name", "IsActive", pNode);
	Attribute("dt:type", "string", pNode);
	pNode.reset();
	
	pNode = Element(scElementTypeTag, *m_pWriter);
	Attribute("name", "Encryption", pNode);
	Attribute("content", "mixed", pNode);
	Attribute("model", "closed", pNode);

	pAttrNode = Element(scAttributeTag, *m_pWriter);
	Attribute("type", "IsActive", pAttrNode);
	Attribute("required", "yes", pAttrNode);
	pAttrNode.reset();

	pElemNode = Element(scElementTag, *m_pWriter);
	Attribute("type", "DATA", pElemNode);
	Attribute("minOccurs", "0", pElemNode);
	Attribute("maxOccurs", "1", pElemNode);
	pElemNode.reset();
	pNode.reset();
 
	pSchemaNode.reset();
}

void SVObjectXMLWriter::WriteStartOfBase()
{
	//<NODE xmlns="x-schema:#SVR00001" Name="Base" Type="SV_BASENODE">
	_variant_t xmlnsValue;
	_variant_t value;
	xmlnsValue.SetString("x-schema:#SVR00001");
	value.SetString("SV_BASENODE");
	StartElement("Base");
	ElementAttribute("xmlns", xmlnsValue);
	ElementAttribute("Type", value);
}

void SVObjectXMLWriter::WriteRevisionHistory(const _variant_t formatVersionValue, const _variant_t revisionValue)
{
	SVString revisionHistoryString(g_csRevisionHistory);
	XMLElementPtr pRevisionHistoryNode = Element(revisionHistoryString, *m_pWriter);
	Attribute("xmlns", "x-schema:#SVR00001", pRevisionHistoryNode);

	SVString revisionString = SvUl_SF::createSVString(g_wcsRevision);
	XMLElementPtr pRevisionNode = Element(revisionString, *m_pWriter);
	Attribute(g_wcsFormat, SVString("SVObserver"), pRevisionNode);
	Attribute(g_wcsFormatVersion, formatVersionValue, pRevisionNode);
	Attribute(g_wcsRevisionAtt, revisionValue, pRevisionNode);
	pRevisionNode.reset();

	pRevisionHistoryNode.reset();
}

void SVObjectXMLWriter::setNewLine( bool NewLine )
{
	if( nullptr != m_pWriter )
	{
		m_pWriter->setNewLine( NewLine );
	}
}

