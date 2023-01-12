//******************************************************************************
//* COPYRIGHT (c) 2012 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVObjectXMLWriter.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVUtilityLibrary/SVSafeArray.h"
#include "SVXMLEncryptionClass.h"
#include "SVConfigurationTags.h"


#define Stringtize(s) (#s)

static std::string scNodeTag(_T("NODE"));
static std::string scDataTag(_T("DATA"));
static std::string scNameTag(_T("Name"));
static std::string scTypeTag(_T("Type"));

static std::string scSchemaTag(_T("Schema"));
static std::string scElementTypeTag(_T("ElementType"));
static std::string scAttributeTypeTag(_T("AttributeType"));
static std::string scElementTag(_T("element"));
static std::string scAttributeTag(_T("attribute"));

const TCHAR* const	cRevisionHistory = _T("RevisionHistory");
const WCHAR* const	cRevision = L"Revision";
const WCHAR* const	cFormat = L"Format";
const WCHAR* const	cFormatVersion = L"FormatVersion";

typedef std::map<VARENUM, std::string> VariantTypeToStringLookup;

static const VariantTypeToStringLookup cVarTypes
{
	{VT_EMPTY, Stringtize(VT_EMPTY)},	// nothing
	{VT_NULL, Stringtize(VT_NULL)},		// SQL style Null
	{VT_I2, Stringtize(VT_I2)},			// 2 byte signed int
	{VT_I4, Stringtize(VT_I4)},			// 4 byte signed int
	{VT_INT, Stringtize(VT_INT)},			// 4 byte signed int
	{VT_R4, Stringtize(VT_R4)},			// 4 byte real
	{VT_R8, Stringtize(VT_R8)},			// 8 byte real
	// not allowed {VT_CY, Stringtize(VT_CY)}		// currency
	// not allowed {VT_DATE, Stringtize(VT_DATE)}	// date
	{VT_BSTR, Stringtize(VT_BSTR)},	// OLE Automation string
	// not allowed {VT_DISPATCH, Stringtize(VT_DISPATCH)} // IDispatch *
	// not allowed {VT_ERROR, Stringtize(VT_ERROR)}	// SCODE
	{VT_BOOL, Stringtize(VT_BOOL)},	// True=-1, False=0
	// not allowed {VT_VARIANT, Stringtize(VT_VARIANT)} // VARIANT *
	// not allowed {VT_UNKNOWN, Stringtize(VT_UNKNOWN)} // IUnknown *
	// not allowed {VT_DECIMAL, Stringtize(VT_DECIMAL)} // 16 byte fixed point
	// not allowed {VT_RECORD, Stringtize(VT_RECORD)} // user defined type
	{VT_I1, Stringtize(VT_I1)},			// signed char
	{VT_UI1, Stringtize(VT_UI1)},		// unsigned char
	{VT_UI2, Stringtize(VT_UI2)},		// unsigned short
	{VT_UI4, Stringtize(VT_UI4)},		// unsigned long
	{VT_UINT, Stringtize(VT_UI4)},		// unsigned long
	{VT_I8, Stringtize(VT_I8)},			// signed 64-bit int
	{VT_UI8, Stringtize(VT_UI8)}		// unsigned 64-bit int
};

namespace  SvXml
{
	std::string to_utf8(const wchar_t* buffer, int len)
	{
		int nChars = ::WideCharToMultiByte(
			CP_UTF8,
			0,
			buffer,
			len,
			NULL,
			0,
			NULL,
			NULL);
		if (nChars == 0) return "";

		std::string newbuffer;
		newbuffer.resize(nChars) ;
		::WideCharToMultiByte(
			CP_UTF8,
			0,
			buffer,
			len,
			const_cast< char* >(newbuffer.c_str()),
			nChars,
			NULL,
			NULL); 

		return newbuffer;
	}

	std::string to_utf8(const std::wstring& str)
	{
		return to_utf8(str.c_str(), (int)str.size());
	}

	static std::string BSTRToUTF8(BSTR value)
	{
		return SvUl::to_utf8(_bstr_t(value));
	}

	static std::string VariantTypeToString(VARTYPE vt)
	{
		return cVarTypes.at(static_cast<VARENUM> (vt));
	}

	static std::string VariantToString(_variant_t value)
	{
		std::string strValue;

		switch (value.vt)
		{
		case VT_EMPTY:
			break;

		case VT_NULL:
			break;

		case VT_I2:
			strValue = SvUl::Format("%d", value.iVal);
			break;

		case VT_INT:
		case VT_I4:
			strValue = SvUl::Format("%ld", value.lVal);
			break;

		case VT_R4:
			strValue = SvUl::Format("%f", value.fltVal);
			break;

		case VT_R8:
			strValue = SvUl::Format("%lf", value.dblVal);
			break;

		case VT_BSTR:
			strValue = BSTRToUTF8(value.bstrVal);
			break;

		case VT_BOOL:
			strValue = SvUl::Format("%s", (value.boolVal == VARIANT_TRUE) ? "TRUE" : "FALSE");
			break;

		case VT_I1:
			strValue = SvUl::Format("%02X", value.cVal);
			break;

		case VT_UI1:
			strValue = SvUl::Format("%02X", value.bVal);
			break;

		case VT_UI2:
			strValue = SvUl::Format("%u", value.uiVal);
			break;

		case VT_UINT:
		case VT_UI4:
			strValue = SvUl::Format("%lu", value.ulVal);
			break;

		case VT_I8:
			strValue = SvUl::Format("%I64d", value.llVal);
			break;

		case VT_UI8:
			strValue = SvUl::Format("%I64u", value.ullVal);
			break;

		default:
			break;
		}
		return std::string(strValue.c_str());
	}

	static XMLElementPtr Element(const std::string& name, xml::writer& writer)
	{
		XMLElementPtr pNode(new xml::element(name.c_str(), writer));
		return pNode;
	}

	static void Attribute(const std::string& name, const std::string& value, XMLElementPtr pNode)
	{
		pNode->attr(name.c_str(), value.c_str());
	}

	static void Attribute(const wchar_t* name, const std::string& value, XMLElementPtr pNode)
	{
		pNode->attr(SvUl::createStdString(name).c_str(), value.c_str());
	}

	static void Attribute(const wchar_t* name, const _variant_t& value, XMLElementPtr pNode)
	{
		pNode->attr(SvUl::createStdString(name).c_str(), SvUl::createStdString(value).c_str());
	}

	SVObjectXMLWriter::SVObjectXMLWriter(std::ostream& os)
	{
		m_pWriter = XMLWriterPtr(new xml::writer(os));
		m_pEncryption = nullptr;
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
				if(m_pEncryption && m_pEncryption->GetIsEncrypted()== TRUE)
				{
							_bstr_t Name(rName);
							_bstr_t Type( varTypeStr.c_str());
							_bstr_t content(valueStr.c_str());
							_bstr_t EnName;
							_bstr_t EnType;
							_bstr_t EnContent;
						
						
							if(m_pEncryption->m_lEncryptionMethod == -1)
							{
								m_pEncryption->EncryptString(1,Name, EnName);
								m_pEncryption->EncryptString(1,Type, EnType);
								m_pEncryption->EncryptString(1,content, EnContent);
							}
							else
							{
									long encryption = m_pEncryption->m_lEncryptionMethod;
									m_pEncryption->EncryptNameAttribute(Name, EnName);
									m_pEncryption->EncryptString(encryption, Type, EnType);
									m_pEncryption->EncryptString(encryption, content, EnContent);
							}
						
							std::wstring encryptedName = static_cast<LPWSTR> (EnName);
							std::wstring encyptedType = static_cast<LPWSTR> (EnType);
							std::wstring encryptedContent =  static_cast<LPWSTR> (EnContent);
						
							xml::element data(scDataTag.c_str(), *m_pWriter);
							data.attr(scNameTag.c_str(), to_utf8(encryptedName.c_str()));
							data.attr(scTypeTag.c_str(), to_utf8(encyptedType.c_str()));
							data.contents( to_utf8(encryptedContent.c_str()));
				}
				else
				{
					xml::element data(scDataTag.c_str(), *m_pWriter);
					data.attr(scNameTag.c_str(), rName);
					data.attr(scTypeTag.c_str(), varTypeStr.c_str());
					data.contents(valueStr.c_str());
				}
			
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
			SvUl::SVSAFEARRAY arrayValue{ value };
			for (int i=0; i < arrayValue.size(); ++i)
			{
				std::string attName = SvUl::Format("DataIndex_%d", i+1);
				_variant_t lVal;
				arrayValue.GetElement( i, lVal );
				WriteAttribute(attName.c_str(), lVal);
			}		
			EndElement();
		}
	}

	void SVObjectXMLWriter::WriteAttribute(LPCTSTR rName, const std::vector<_variant_t>& rValues)
	{
		for(auto const& rValue : rValues)
		{
			WriteAttribute(rName, rValue);
		}
	}

	void SVObjectXMLWriter::StartElement(LPCTSTR rName)
	{
		XMLElementPtr pNode(new xml::element(scNodeTag.c_str(), *m_pWriter));
		m_elements.push_front(pNode);
	
		if(m_pEncryption && m_pEncryption->GetIsEncrypted()== TRUE)
		{
			_bstr_t Name(rName);
			_bstr_t EnName;

			if (m_pEncryption->m_lEncryptionMethod == -1)
			{
				m_pEncryption->EncryptString(1, Name, EnName);
			}
			else
			{
				m_pEncryption->EncryptNameAttribute(Name, EnName);
			}
			std::wstring encryptedName = static_cast<LPWSTR> (EnName);

			pNode->attr(scNameTag.c_str(), to_utf8(encryptedName.c_str()));
		}
	
		else 
		{
			pNode->attr(scNameTag.c_str(), rName);
		}
	
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


	void SVObjectXMLWriter::SetEncryption(SVXMLEncryptionClass*  pEncryption)
	{

		m_pEncryption = pEncryption;

	}

	void SVObjectXMLWriter::WriteEncryption()
	{
	
		long isEncrypted(FALSE);
		if(m_pEncryption)
		{
			m_pEncryption->GetIsEncrypted(&isEncrypted);
		}
		///<Encryption xmlns="x-schema:#SVR00001" IsActive="TRUE">
		std::string EncryptionString( _T("Encryption"));
		XMLElementPtr pEncryptionNode = Element(EncryptionString, *m_pWriter);
		Attribute("xmlns", "x-schema:#SVR00001", pEncryptionNode);
		Attribute("IsActive", (isEncrypted == TRUE)?  "TRUE": "FALSE", pEncryptionNode);
	
		if(isEncrypted)
		{
			m_pEncryption->SetEncryptionMethod(-1);
		
			_variant_t content( m_pEncryption->GetNameSeed()); 
			WriteAttribute( _T("NameSeed"), content);
			m_pEncryption->SetEncryptionMethod(1);
		}
		pEncryptionNode.reset();

	}

	void SVObjectXMLWriter::WriteStartOfBase()
	{
		//<NODE xmlns="x-schema:#SVR00001" Name="Base" Type="SV_BASENODE">
		_variant_t xmlnsValue;
		_variant_t value;
		xmlnsValue.SetString(SvXml::SvrNameSpace);
	
	
		if(m_pEncryption && m_pEncryption->GetIsEncrypted()== TRUE) 
		{
			_bstr_t basenode(SvXml::SV_BaseNode);
			_bstr_t Enbasenode;
			m_pEncryption->EncryptString (2, basenode, Enbasenode);
			value.SetString( (LPCTSTR) Enbasenode);
		}
		else
		{
			value.SetString(SvXml::SV_BaseNode);
		}
	
	
		StartElement(SvXml::BaseTag);
		ElementAttribute(SvXml::XmlNameSpace, xmlnsValue);
		ElementAttribute(SvXml::TypeTag, value);
	}

	void SVObjectXMLWriter::WriteShortEvirmonment(DWORD version)
	{
		StartElement(SvXml::CTAG_ENVIRONMENT);
		WriteAttribute(SvXml::CTAG_VERSION_NUMBER, version);
		EndElement();
	}

	void SVObjectXMLWriter::WriteRevisionHistory(const _variant_t formatVersionValue, const _variant_t revisionValue)
	{
		std::string revisionHistoryString(cRevisionHistory);
		XMLElementPtr pRevisionHistoryNode = Element(revisionHistoryString, *m_pWriter);
		Attribute("xmlns", "x-schema:#SVR00001", pRevisionHistoryNode);

		std::string revisionString = SvUl::createStdString(cRevision);
		XMLElementPtr pRevisionNode = Element(revisionString, *m_pWriter);
		Attribute(cFormat, std::string("SVObserver"), pRevisionNode);
		Attribute(cFormatVersion, formatVersionValue, pRevisionNode);
		Attribute(cRevision, revisionValue, pRevisionNode);
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


	void SVObjectXMLWriter::setHeader(LPCTSTR header)
	{
		if( nullptr != m_pWriter )
		{
			m_pWriter->setHeader( header );
		}
	}

} //namespace SvXml
