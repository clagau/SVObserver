//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file UserDatabase.cpp
/// All Rights Reserved
//******************************************************************************
/// Not much to see here.
//******************************************************************************

#pragma region Includes
#include "stdafx.h"

#include <atlstr.h>
#include <comutil.h>
#include <codecvt>
#include <locale>

#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>

#include "SVAuthLibrary/UserDatabaseLoader.h"
#include "SVLogLibrary/Logging.h"
#include "SVXmlLibrary/ISaxElementHandler.h"
#include "SVXmlLibrary/SaxParser.h"
#include "SVXmlLibrary/SVMaterialData.h"
#pragma endregion Includes

namespace SvAuth
{

namespace
{

enum ParserState
{
	PS_UNKNOWN = 0,
	PS_START = 1,
	PS_ASPHERE_ROOT,
	PS_SIGNATURE,
	PS_USER_DATABASE,
	PS_AUTH_QUALITY_CHECKERS,
	PS_AUTH_QUALITY_CHECKER,
	PS_USERS,
	PS_USER,
	PS_PASSWORD_AUTH,
	PS_WORK_ENVIRONMENT,
	PS_PROPERTIES,
	PS_PERMISSIONS,
};

std::string ws2s(const std::wstring& wstr)
{
#pragma warning(push)
#pragma warning(disable : 4996)
	return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(wstr);
#pragma warning(pop)
}

using TItFnMap = std::map<std::string, std::function<void(const std::string&)>>;

static void iterateAttributes(ISAXAttributes* pAttributes, const TItFnMap& fns)
{
	if (!pAttributes)
	{
		return;
	}

	int NoOfAttr(0);
	pAttributes->getLength(&NoOfAttr);
	for (int i = 0; i < NoOfAttr; ++i)
	{
		const wchar_t *pName(nullptr), *pValue(nullptr);
		int lName(0), lValue = 0;

		pAttributes->getLocalName(i, &pName, &lName);
		if (lName == 0)
		{
			///localname is empty for namespace attributes xmln dt:xmln
			pAttributes->getQName(i, &pName, &lName);
		}
		pAttributes->getValue(i, &pValue, &lValue);
		if (lValue == 0 || lName == 0)
		{
			continue;
		}

		const auto name = ws2s(std::wstring(pName, lName));
		const auto value = ws2s(std::wstring(pValue, lValue));
		const auto it = fns.find(name);
		if (it != fns.end())
		{
			it->second(value);
		}
	}
}

static void loadUserAttributes(UserDatabaseEntry& entry, ISAXAttributes* pAttributes)
{
	TItFnMap itMap;
	itMap["name"] = [&entry](const std::string& v)
	{
		entry.set_firstname(v);
		entry.set_username(v); // will be overwritten in case we find a passwordAuth node
	};
	itMap["level"] = [&entry](const std::string& v)
	{
		try
		{
			entry.set_level(boost::lexical_cast<int>(v));
		}
		catch (const boost::bad_lexical_cast &)
		{
			SV_LOG_GLOBAL(warning) << "Error while parsing user level " << v;
		}
	};
	iterateAttributes(pAttributes, itMap);
}

static void loadPasswordAttributes(UserDatabaseEntry& entry, ISAXAttributes* pAttributes)
{
	TItFnMap itMap;
	itMap["userName"] = [&entry](const std::string& v)
	{
		entry.set_username(v);
	};
	itMap["passwordCoded"] = [&entry](const std::string& v)
	{
		entry.set_password(v);
	};
	iterateAttributes(pAttributes, itMap);
}

static bool isValidUser(const UserDatabaseEntry& entry, std::string& errMsg)
{
	if (entry.username().empty())
	{
		errMsg = "empty username";
		return false;
	}
	if (entry.password().empty())
	{
		errMsg = "empty password";
		return false;
	}
	return true;
}

struct UserDatabaseXmlElementHandler : public SvXml::ISaxElementHandler
{
public:
	explicit UserDatabaseXmlElementHandler(std::map<std::string, UserDatabaseEntry>& rDb)
		: m_rDb(rDb)
	{
		m_State.push_back(PS_START);
	}

	HRESULT  OnStartElement(const wchar_t  *pwchNamespaceUri, int cchNamespaceUri, const wchar_t *pwchName, int cchName, ISAXAttributes *pAttributes, int ) override
	{
		const auto ns = std::wstring(pwchNamespaceUri, cchNamespaceUri);
		const auto name = std::wstring(pwchName, cchName);

		ParserState state = PS_UNKNOWN;

		switch (m_State.back())
		{
			case PS_START:
				if (name != L"asphereDatabase")
				{
					SV_LOG_GLOBAL(info) << "Unexpected node. Only asphereDatabase allowed, but found " << name;
					return ERROR_XML_PARSE_ERROR;
				}
				state = PS_ASPHERE_ROOT;
				break;

			case PS_ASPHERE_ROOT:
				if (ns == L"http://www.w3.org/2000/09/xmldsig#")
				{
					// TODO parse and validate signature
					state = PS_SIGNATURE;
				}
				else if (name == L"userDatabase")
				{
					state = PS_USER_DATABASE;
				}
				break;

			case PS_USER_DATABASE:
				if (name == L"users")
				{
					state = PS_USERS;
				}
				break;

			case PS_USERS:
				if (name == L"user")
				{
					state = PS_USER;
					m_CurrUser = std::make_unique<UserDatabaseEntry>();
					loadUserAttributes(*m_CurrUser, pAttributes);
				}
				break;

			case PS_USER:
				if (name == L"passwordAuth")
				{
					state = PS_PASSWORD_AUTH;
					loadPasswordAttributes(*m_CurrUser, pAttributes);
				}
				break;

			default:
				break;
		}

		m_State.push_back(state);
		m_Nodes.push_back(name);

		return S_OK;
	}

	HRESULT  OnElementData(const wchar_t*, int , int ) override
	{
		// We do not need any data. All required data is inside attributes.
		return S_OK;
	}

	HRESULT  OnEndElement(const wchar_t*, int , const wchar_t*, int , int ) override
	{
		const auto lastState = m_State.back();
		const auto lastNode = m_Nodes.back();

		m_State.pop_back();
		m_Nodes.pop_back();

		switch (lastState)
		{
			case PS_USER:
			{
				std::string errMsg;
				if (!isValidUser(*m_CurrUser, errMsg))
				{
					SV_LOG_GLOBAL(debug) << "Found invalid user in user database: " << errMsg;
					break;
				}
				m_rDb.insert(std::make_pair(m_CurrUser->username(), *m_CurrUser));
				break;
			}

			default:
				break;
		}

		return S_OK;
	}

	HRESULT  OnXMLError(int , int , const wchar_t* pwchErrorText, unsigned long , bool ) override
	{
		std::wcout << "OnXMLError: " << pwchErrorText << std::endl;
		return S_OK;
	}

private:
	int m_Depth = 0;
	std::vector<ParserState> m_State;
	std::vector<std::wstring> m_Nodes;
	std::unique_ptr<UserDatabaseEntry> m_CurrUser;
	std::map<std::string, UserDatabaseEntry>& m_rDb;
};
}

UserDatabase UserDatabaseLoader::load(std::filesystem::path path)
{
	UserDatabase db;
	UserDatabaseXmlElementHandler handler(db.m_Entries);

	SvXml::SaxParser parser;
	parser.AttachElementHandler(&handler);
	auto hr = parser.ParseFile(path.string().c_str());
	if (!SUCCEEDED(hr))
	{
		SV_LOG_GLOBAL(error) << "Unable to load user database from " << path;
	}

	SV_LOG_GLOBAL(info) << "Successfully loaded " << db.m_Entries.size() << " users from " << path;

	return db;
}

} // namespace SvAuth
