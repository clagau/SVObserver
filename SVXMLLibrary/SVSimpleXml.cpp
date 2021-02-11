//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// class SVSimpleXml
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVSimpleXml.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVXMLLibrary/XMLWriter.h"
#include "ISaxElementHandler.h"
#include "SaxParser.h"
#pragma endregion Includes

namespace SvXml
{

namespace
{

struct SimpleXmlElementHandler : public SvXml::ISaxElementHandler
{
public:
	explicit SimpleXmlElementHandler(XmlElement& root) : m_root(root), m_curr(&root)
	{
	}

	HRESULT OnStartElement(const wchar_t*, int, const wchar_t* pwchName, int cchName, ISAXAttributes* pAttributes, int ) override
	{
		// TODO implement namespace handling
		m_curr = m_curr->addChild(std::wstring(pwchName, cchName));

		if (pAttributes)
		{
			int NoOfAttr(0);
			pAttributes->getLength(&NoOfAttr);
			for (int i = 0; i < NoOfAttr; ++i)
			{
				const wchar_t* pName(nullptr), * pValue(nullptr);
				int lName(0), lValue = 0;

				pAttributes->getLocalName(i, &pName, &lName);
				if (lName == 0)
				{
					///localname is empty for namespace attributes xmln dt:xmln
					pAttributes->getQName(i, &pName, &lName);
				}
				pAttributes->getValue(i, &pValue, &lValue);
				if (lName == 0)
				{
					continue;
				}

				XmlAttribute attr;
				attr.name = std::wstring(pName, lName);
				attr.value = std::wstring(pValue, lValue);
				m_curr->attributes.emplace_back(std::move(attr));
			}
		}

		return S_OK;
	}

	HRESULT OnElementData(const wchar_t* pwchData, int cchData, int) override
	{
		if (cchData > 0)
		{
			std::wstring chunk(pwchData, cchData);
			boost::algorithm::trim(chunk);
			if (!chunk.empty())
			{
				m_curr->content += chunk;
			}
		}
		return S_OK;
	}

	HRESULT OnEndElement(const wchar_t*, int, const wchar_t*, int, int) override
	{
		assert(m_curr);
		m_curr = m_curr->parent;
		return S_OK;
	}

	HRESULT OnXMLError(int , int , const wchar_t* pwchErrorText, unsigned long , bool) override
	{
		std::wcout << "OnXMLError: " << pwchErrorText << std::endl;
		return E_FAIL;
	}

	XmlElement& m_root;
	XmlElement* m_curr;
};

} // anonymous namespace

static bool splitPath(const std::wstring& path, std::wstring& first, std::wstring& remainder)
{
	auto pos = path.find('.');
	first = path.substr(0, pos);
	remainder = path.substr(pos + 1);
	return (pos == std::wstring::npos);
}

void iterateElements(const XmlElement& parent, const std::wstring& path, std::function<void(const XmlElement&)> fn)
{
	if (path.empty())
	{
		return;
	}

	std::wstring first, remainder;
	bool lastLayer = splitPath(path, first, remainder);
	
	for (auto& child : parent.children)
	{
		if (child.name == first)
		{
			if (lastLayer)
			{
				fn(child);
			}
			else
			{
				iterateElements(child, remainder, fn);
			}
		}
	}
}

void iterateElements(XmlElement& parent, const std::wstring& path, std::function<void(XmlElement&)> fn)
{
	if (path.empty())
	{
		return;
	}

	std::wstring first, remainder;
	bool lastLayer = splitPath(path, first, remainder);

	for (auto& child : parent.children)
	{
		if (child.name == first)
		{
			if (lastLayer)
			{
				fn(child);
			}
			else
			{
				iterateElements(child, remainder, fn);
			}
		}
	}
}

bool readFromXmlFile(XmlElement& root, const std::string& xmlFileName)
{
	SimpleXmlElementHandler handler(root);

	SvXml::SaxParser parser;
	parser.AttachElementHandler(&handler);
	auto hr = parser.ParseFile(xmlFileName.c_str());
	return SUCCEEDED(hr);
}

static void serializeChildren(xml::writer& writer, const XmlElement& ele)
{
	std::string name = SvUl::to_utf8(ele.name);
	xml::element xmlEle(name.c_str(), writer);
	for (const auto& attr : ele.attributes)
	{
		xmlEle.attr(SvUl::to_utf8(attr.name).c_str(), SvUl::to_utf8(attr.value).c_str());
	}
	for (const auto& child : ele.children)
	{
		serializeChildren(writer, child);
	}
	xmlEle.contents(SvUl::to_utf8(ele.content));
}

bool writeToFile(const std::string& fname, const XmlElement& ele, bool newLine)
{
	std::ofstream ofs;
	ofs.open(fname.c_str());
	if (!ofs.is_open())
	{
		return false;
	}

	writeToStream(ofs, ele, newLine);
	return true;
}

void writeToStream(std::ostream& str, const XmlElement& ele, bool newLine)
{
	xml::writer writer(str);
	writer.setNewLine(newLine);
	for (const auto& child : ele.children)
	{
		serializeChildren(writer, child);
	}
}

} //namespace SvXml
