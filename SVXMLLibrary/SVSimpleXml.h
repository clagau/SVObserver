//*****************************************************************************
/// \copyright (c) 2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
///  class SVSimpleXml
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <ostream>
//Moved to precompiled header: #include <vector>
#pragma endregion Includes

namespace SvXml
{

	struct XmlAttribute
	{
		std::wstring name{ L"" };
		std::wstring value{ L"" };
	};

	struct XmlElement
	{
		std::wstring name{ L"" };
		XmlElement* parent{ nullptr };
		std::vector<XmlElement> children{};
		std::vector<XmlAttribute> attributes{};
		std::wstring content{ L"" };

		XmlElement* addChild(const std::wstring& childName)
		{
			children.push_back(XmlElement());
			auto ptr = &children.back();
			ptr->name = childName;
			ptr->parent = this;
			return ptr;
		}

		void addAttribute(const std::wstring& rName, const std::wstring& rValue)
		{
			XmlAttribute attr;
			attr.name = rName;
			attr.value = rValue;
			attributes.emplace_back(std::move(attr));
		}
	};

	void iterateElements(const XmlElement& parent, const std::wstring& path, std::function<void(const XmlElement&)> fn);
	void iterateElements(XmlElement& parent, const std::wstring& path, std::function<void(XmlElement&)> fn);
	bool readFromXmlFile(XmlElement& root, const std::string& xmlFileName);
	bool writeToFile(const std::string& fname, const XmlElement& ele, bool newLine = true);
	void writeToStream(std::ostream& str, const XmlElement& ele, bool newLine = true);

} //namespace SvXml
