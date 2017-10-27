//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVJsonParser
//* .File Name       : $Workfile:   SVJsonParser.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   16 Oct 2014 09:59:26  $
//******************************************************************************
#pragma once
#pragma region Includes
#include <iostream>
#include <deque>
#pragma endregion Includes

namespace SVJson
{
	enum jsontype_t
	{
		JSON_PRIMITIVE = 0,	// 	o Other primitive: number, boolean (true/false) or null
		JSON_OBJECT = 1,	// 	o Object
		JSON_ARRAY = 2,		// 	o Array
		JSON_STRING = 3,	// 	o String
		JSON_COMMENT = 4	// 	o Comment
	};

	enum jsonerr_t
	{
		JSON_SUCCESS = 0,
		JSON_ERROR_NOMEM = -1,	// Not enough tokens were provided
		JSON_ERROR_INVAL = -2,	// Invalid character inside JSON string
		JSON_ERROR_PART = -3,	// The string is not a full JSON packet, more bytes expected
	};

	struct jsontok_t
	{
		jsontype_t type;	// type (object, array, string etc.)
		int start;			// start position in JSON data string
		int end;			// end position in JSON data string
		int parent;			// Id of parent node
		int size;			// count of children ?

		jsontok_t(jsontype_t t, int s, int e=-1, int p=-1)
		: type(t), start(s), end(e), parent(p), size(0) {}
	};

	class ParseStreamAdapter
	{
		std::istream& m_rSS;
		size_t m_len;
	public:
		ParseStreamAdapter(std::istream& rSS) : m_rSS(rSS), m_len(0)
		{
			m_rSS.clear(); // clear error/eof bits
			m_rSS.seekg(0, std::ios_base::end);
			m_len = static_cast<size_t>(m_rSS.tellg());
			m_rSS.seekg(0, std::ios_base::beg);
		}
		size_t length() const { return m_len; }
		char operator[](size_t index)
		{
			char val = '\0'; 
			if (!m_rSS.eof())
			{
				std::ios::streamoff pos = m_rSS.tellg();
				if (pos != index)
				{
					m_rSS.seekg(index, std::ios_base::beg);
				}
				val = m_rSS.get();
			}
			return val;
		}
	};

	class Parser
	{
	public:
		typedef std::deque<jsontok_t> TokenList;

		Parser() : m_pos(0), m_toknext(0), m_toksuper(-1) {}

		template<typename JsonRepo>
		jsonerr_t Parse(JsonRepo& js);

		const TokenList& GetTokens() const { return m_tokens; }

	private:
		int m_pos;			// current offset in the JSON string/stream
		int m_toknext;		// next token to allocate
		int m_toksuper;		// superior token node, e.g parent object or array
		TokenList m_tokens;

		int AddToken(const jsontok_t& token)
		{
			m_tokens.push_back(token);
			m_toknext = static_cast<int>(m_tokens.size());
			return m_toknext;
		}

		template<typename JsonRepo>
		jsonerr_t ParsePrimitive(JsonRepo& js);

		template<typename JsonRepo>
		jsonerr_t ParseString(JsonRepo& js);

	};
	#include "SVJsonParser.inl"
}
