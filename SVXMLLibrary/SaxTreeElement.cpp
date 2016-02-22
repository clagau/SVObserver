
//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
// class SaxTreeElement 
//*****************************************************************************


#pragma region Includes
#include "StdAfx.h"
#include "SaxTreeElement.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <msxml2.h>
#include "VariantHelper.h"
#pragma endregion Includes


#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace Seidenader { namespace  SVXMLLibrary
{
	const std::wstring  SaxTreeElement::ElementName[Unknown_Element +1]  = 
	{	
		L"DATA",
		L"NODE",
		L"Revision",
		L"RevisionHistory",
		L"Encryption",
		L"Root",
		L"UNKNOWN"
	};
	const std::wstring SaxTreeElement::s_NameAttTag = L"Name";
	const std::wstring SaxTreeElement::s_TypeAttTag = L"Type";;

	SaxTreeElement::EType SaxTreeElement::GetElementType(LPCWSTR name)
	{
		std::wstring wsName  = name;
		for(int i = 0; i < Unknown_Element; i++ )
		{
			if(wsName.compare(ElementName[i])==0)
				return EType(i);
		}
		return Unknown_Element;
	}

	VARTYPE SaxTreeElement::GetVarTypeAtt()
	{
		if(m_bTypeAtt)
		{
			return m_VarTypeAtt;
		}
		else
		{
			return VT_EMPTY;
		}

	}
	
	const wchar_t* SaxTreeElement::GetTypeAtt() const
	{
		return m_TypeAtt.c_str(); 
	}

	const wchar_t* SaxTreeElement::GetNameAtt() const 
	{
		return m_NameAtt.c_str(); 
	}

		
		
	SaxTreeElement::SaxTreeElement(const wchar_t *pwchName, int cchName)
	{
		m_Elementname.assign(pwchName,cchName);
		m_ElementType = GetElementType(m_Elementname.c_str());
		m_depth =0;
		m_bNameAtt = false;
		m_bTypeAtt = false;
		
		m_VarTypeAtt = VT_UNKNOWN;
	}

	SaxTreeElement::SaxTreeElement(std::wstring const &name)
	{
		m_Elementname = name;
		m_ElementType = GetElementType(m_Elementname.c_str());
		m_depth =0;
		m_bNameAtt = false;
		m_bTypeAtt = false;
			
	}


	SaxTreeElement::EType SaxTreeElement::GetElementType() const
	{
		return m_ElementType;
	}

	bool SaxTreeElement::IsDataBaseNode(LPCWSTR lpAttType, LPCWSTR lpoAttName) const
	{
		if(lpoAttName)
		{
			return (m_ElementType == NODE_Element && m_bTypeAtt  && m_TypeAtt.compare(lpAttType) == 0  && m_NameAtt.compare(lpoAttName) ==0) ;
		}
		else
		{
			return (m_ElementType == NODE_Element && m_bTypeAtt  && m_NameAtt.compare(lpoAttName) ==0);
		}
	}

	bool SaxTreeElement::IsRevisionBaseNode() const
	{
		return (m_ElementType == RevisionHistory_Element);
	}

	bool SaxTreeElement::IsEncryptionBaseNode() const
	{
		return (m_ElementType == Encryption_Element);
	}

	bool SaxTreeElement:: IsVtArrayNode() const
	{
		return (m_ElementType == NODE_Element && m_bTypeAtt  && m_TypeAtt == L"VT_ARRAY");
	}

	void SaxTreeElement::PrintElement(std::wstringstream &stream )
	{
		
		if(m_ElementType !=  Unknown_Element)
		{
			m_Elementname = ElementName[m_ElementType];
		}

		stream << L"<" << m_Elementname.c_str();  

		if( false == m_NameAtt.empty() )
		{
			stream << L"  Name=\"" << m_NameAtt.c_str() << L"\" ";
		}
		if( false == m_TypeAtt.empty() )
		{
			stream << L" Type=\"" <<  m_TypeAtt.c_str() << L"\" ";
		}


		std::map<std::wstring,std::wstring>::const_iterator it = m_SpecialAtributes.begin();
		for(; it != m_SpecialAtributes.end(); it++ )
		{
			stream <<  L" "  << it->first.c_str() <<"=\""  << it->second.c_str() <<"\""; 
		}

		if(m_Content.empty()== false)
		{
			stream << ">" ;
			stream <<  m_Content.c_str() ;
			stream << "</" <<  m_Elementname.c_str() << ">" <<  std::endl;
		}
		else
		{
			stream << "/>"   << std::endl;
		}
	}

	void SaxTreeElement::PrintElementStart(std::wstringstream  &stream )
	{
			
		if(m_ElementType !=  Unknown_Element)
		{
			m_Elementname = ElementName[m_ElementType];
		}

		stream << "<" << m_Elementname.c_str();  

		if(m_NameAtt.empty() == false )
		{
			stream << "  Name=\"" << m_NameAtt.c_str() << "\" ";
		}
		if(m_TypeAtt.empty() == false )
		{
			stream << " Type=\"" <<  m_TypeAtt.c_str() << "\" ";
		}


		std::map<std::wstring,std::wstring>::const_iterator it = m_SpecialAtributes.begin();
		for(; it != m_SpecialAtributes.end(); it++ )
		{
			stream <<  " "  << it->first.c_str() <<"=\""  << it->second.c_str() <<"\""; 
		}
		stream << ">" ;


		if(m_Content.empty()== false)
		{
			stream <<  std::endl << m_Content.c_str() << std::endl ;
		}
		else
		{
			stream  <<  std::endl;
		}
	}

	void SaxTreeElement::PrintElementEnd(std::wstringstream  &stream )
	{
			
		stream << L"</" <<  m_Elementname.c_str() << L">" <<  std::endl;
	}

	int SaxTreeElement::SetAttributes(ISAXAttributes *pAttributes)
	{
		int NoOfAt(0);

		pAttributes->getLength(&NoOfAt);
		for (int i = 0; i < NoOfAt; ++i)
		{
			const wchar_t *pName(nullptr), *pValue(nullptr);
			int lName(0), lValue = 0;

			pAttributes->getLocalName(i, &pName, &lName); 
			if(lName == 0)
			{
				///localname is empty for namespace attributes xmln dt:xmln
				pAttributes->getQName(i, &pName, &lName); 
			}
			pAttributes->getValue(i, &pValue, &lValue);
			if(lValue ==0  || lName == 0 )
			{
				continue;
			}
			std::wstring aName,aValue;
			aName.assign(pName, lName);
			aValue.assign(pValue,lValue);
			if(aName.compare(s_NameAttTag) == 0)
			{
				m_NameAtt = aValue;
				m_bNameAtt = true;
			}
			else if(aName.compare(s_TypeAttTag) == 0)
			{
				m_TypeAtt = aValue;
				m_bTypeAtt = true;
				m_VarTypeAtt = VariantHelper::WCHAR2Type(aValue.c_str());
			}
			else
			{
				m_SpecialAtributes[aName] = aValue;
			}
		}

		return NoOfAt;
	}

	int SaxTreeElement::GetDepth() const
	{
		return m_depth;
	}

	void SaxTreeElement::SetDepth(int depth ) 
	{
		m_depth = depth;
	}

	const wchar_t *  SaxTreeElement::GetContent() const
	{
		return m_Content.c_str();
	}

	void SaxTreeElement::AppendToContent(const wchar_t * cont, int len)
	{
		m_Content.append(cont,len);
	}

	void SaxTreeElement::ClearEmptyContent()
	{
		bool OnlyWhitspace = true;
		std::wstring::const_iterator it = m_Content.begin();
		while(OnlyWhitspace && it != m_Content.end())
		{
			if(!iswspace(*it))
			{
				OnlyWhitspace = false;
				break;
			}
				it++;
		}

		if(OnlyWhitspace )
		{
			m_Content.clear();
		}

			
	}

	const wchar_t* SaxTreeElement::GetSpecialAtt(LPCWSTR pAttributeName ) const
	{
		std::wstring toFind(pAttributeName);
		std::map<std::wstring,std::wstring>::const_iterator it = m_SpecialAtributes.find(toFind);

		if(it == m_SpecialAtributes.end())
		{
			return nullptr;
		}
		else
		{
			return it->second.c_str();
		}
	}

	void SaxTreeElement::SetNameAttribute(LPCWSTR name)
	{
		m_NameAtt = name;
		m_bNameAtt = (m_NameAtt.size() > 0);
	}

	void SaxTreeElement::SetTypeAttribute(LPCWSTR name)
	{
		m_TypeAtt = name;	
		m_bTypeAtt = (m_TypeAtt.size() > 0);
		m_VarTypeAtt = VariantHelper::WCHAR2Type(name);
	}


	void SaxTreeElement::SetContent(LPCWSTR name)
	{
		m_Content = name;
	}

} /* namespace SVXMLLibrary */ } /* namespace Seidenader */
