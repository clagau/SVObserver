//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVJsonParser
//* .File Name       : $Workfile:   SVJsonParser.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   16 Oct 2014 09:59:26  $
//******************************************************************************
using namespace SVJson;

// Fills next available token with JSON primitive.
template <typename JsonRepo>
 jsonerr_t Parser::ParsePrimitive(JsonRepo& js) 
{
	int start = m_pos;
	int len = static_cast<int>(js.length());
	char val = '\0';
	for (; m_pos < len && (val = js[m_pos]) != '\0'; m_pos++) 
	{
		switch (val) 
		{
#ifndef JSON_STRICT
			// In strict mode primitive must be followed by "," or "}" or "]"
			case ':':
#endif
			case '\t': 
			case '\r': 
			case '\n': 
			case ' ':
			case ',': 
			case ']': 
			case '}':
				goto found;
		}
		if (val < 32 || val >= 127) 
		{
			m_pos = start;
			return JSON_ERROR_INVAL;
		}
	}
#ifdef JSON_STRICT
	// In strict mode primitive must be followed by a comma/object/array
	m_ParseTree.pos = start;
	return JSON_ERROR_PART;
#endif

found:
	jsontok_t token(JSON_PRIMITIVE, start, m_pos, m_toksuper);
	try
	{
		AddToken(token);
	}
	catch (std::bad_alloc& e)
	{
		std::cout << e.what() << std::endl;
		m_pos = start;
		return JSON_ERROR_NOMEM;
	}
	m_pos--;
	return JSON_SUCCESS;
}

// Fills next token with JSON string.
template <typename JsonRepo>
jsonerr_t Parser::ParseString(JsonRepo& js) 
{
	int len = static_cast<int>(js.length());
	int start = m_pos;

	m_pos++;

	char c = '\0';
	// Skip starting quote
	for (; m_pos < len && (c = js[m_pos]) != '\0'; m_pos++) 
	{
		// Quote: end of string
		if (c == '\"') 
		{
			jsontok_t token(JSON_STRING, start+1, m_pos, m_toksuper);
			try
			{
				AddToken(token);
			}
			catch( std::bad_alloc& e)
			{
				std::cout << e.what() << std::endl;
				m_pos = start;
				return JSON_ERROR_NOMEM;
			}
			return JSON_SUCCESS;
		}

		// Backslash: Quoted symbol expected
		if (c == '\\') 
		{
			m_pos++;
			switch (js[m_pos]) 
			{
				// Allowed escaped symbols
				case '\"': 
				case '/': 
				case '\\': 
				case 'b':
				case 'f': 
				case 'r': 
				case 'n': 
				case 't':
					break;

				// Allows escaped symbol \uXXXX
				case 'u':
				{
					m_pos++;
					int i = 0;
					char val = '\0';
					for(; i < 4 && (val = js[m_pos]) != '\0'; i++) 
					{
						// If it isn't a hex character we have an error
						if (!((val >= 48 && val <= 57) || // 0-9
								(val >= 65 && val <= 70) || // A-F
								(val >= 97 && val <= 102)))  // a-f
						{
							m_pos = start;
							return JSON_ERROR_INVAL;
						}
						m_pos++;
					}
					m_pos--;
				}
				break;

				// Unexpected symbol
				default:
					m_pos = start;
					return JSON_ERROR_INVAL;
			}
		}
	}
	m_pos = start;
	return JSON_ERROR_PART;
}

// Parse JSON string and fill tokens.
template<typename JsonRepo>
jsonerr_t Parser::Parse(JsonRepo& js) 
{
	int i = 0;
	int count = 0;
	int len = static_cast<int>(js.length());
	char c = '\0';
	for (; m_pos < len && (c = js[m_pos]) != '\0'; m_pos++) 
	{
		jsontype_t type;

		switch (c) 
		{
			case '{': 
			case '[':
			{
				count++;

				if (-1 != m_toksuper) 
				{
					m_tokens[m_toksuper].size++;
				}
				jsontype_t type = (c == '{' ? JSON_OBJECT : JSON_ARRAY);
				jsontok_t token(type, m_pos, -1, m_toksuper);
				try
				{
					AddToken(token);
					m_toksuper = m_toknext - 1;
				}
				catch (std::bad_alloc& e)
				{
					std::cout << e.what() << std::endl;
					return JSON_ERROR_NOMEM;
				}
			}
			break;

			case '}': 
			case ']':
			{
				type = (c == '}' ? JSON_OBJECT : JSON_ARRAY);
#ifdef JSMN_PARENT_LINKS
				if (m_toknext < 1) 
				{
					return JSON_ERROR_INVAL;
				}
				jsontok_t& token = m_tokens[m_toknext - 1];
				for (;;) 
				{
					if (-1 != token.start && -1 == token.end) 
					{
						if (token.type != type)
						{
							return JSON_ERROR_INVAL;
						}
						token.end = m_pos + 1;
						m_toksuper = token.parent;
						break;
					}
					if (-1 == token.parent) 
					{
						break;
					}
					token = m_tokens[token.parent];
				}
#else
				for (i = m_toknext - 1; i >= 0; i--) 
				{
					jsontok_t& token = m_tokens[i];
					if (-1 != token.start && -1 == token.end) 
					{
						if (token.type != type)
						{
							return JSON_ERROR_INVAL;
						}
						m_toksuper = -1;
						token.end = m_pos + 1;
						break;
					}
				}
				// Error if unmatched closing bracket
				if (-1 == i) return JSON_ERROR_INVAL;
				
				for (; i >= 0; i--) 
				{
					const jsontok_t& token = m_tokens[i];
					if (-1 != token.start && -1 == token.end) 
					{
						m_toksuper = i;
						break;
					}
				}
#endif
			}
			break;

			case '\"':
			{
				jsonerr_t r = ParseString(js);
				if (r < 0) return r;
				count++;
				if (-1 != m_toksuper && m_tokens.size())
				{
					m_tokens[m_toksuper].size++;
				}
			}
			break;
			
			case '\t' : 
			case '\r' : 
			case '\n' : 
			case ':' : 
			case ',': 
			case ' ': 
				break;

#ifdef JSON_STRICT
			// In strict mode primitives are: numbers and booleans
			case '-': 
			case '0': 
			case '1': 
			case '2': 
			case '3': 
			case '4':
			case '5': 
			case '6': 
			case '7': 
			case '8': 
			case '9':
			case 't': 
			case 'f': 
			case 'n':
#else
			// In non-strict mode every unquoted value is a primitive
			default:
#endif
			{
				jsonerr_t r = ParsePrimitive(js);
				if (r < 0) return r;
				count++;
				if (-1 != m_toksuper && m_tokens.size())
				{
					m_tokens[m_toksuper].size++;
				}
			}
			break;

#ifdef JSON_STRICT
			// Unexpected char in strict mode
			default:
				return JSON_ERROR_INVAL;
#endif
		}
	}

	for (i = m_toknext - 1; i >= 0; i--)
	{
		// Unmatched opened object or array
		if (-1 != m_tokens[i].start && -1 == m_tokens[i].end) 
		{
			return JSON_ERROR_PART;
		}
	}
	//return count;
	return JSON_SUCCESS;
}
