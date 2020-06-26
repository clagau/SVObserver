//******************************************************************************
//* Copyright (c) 2010
//* All Rights Reserved
//* Author           : Ferruccio Barletta (ferruccio.barletta@gmail.com)
//* License          : 
//* From             : 
//******************************************************************************
//* .Module Name     : XMLWriter
//* .File Name       : $Workfile:   XMLWriter.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   13 Nov 2014 08:58:44  $
//******************************************************************************

/* 
Copyright (C) 2010, Ferruccio Barletta (ferruccio.barletta@gmail.com)

Permission is hereby granted, free of charge, to any person 
obtaining a copy of this software and associated documentation 
files (the "Software"), to deal in the Software without 
restriction, including without limitation the rights to use, 
copy, modify, merge, publish, distribute, sublicense, and/or sell 
copies of the Software, and to permit persons to whom the 
Software is furnished to do so, subject to the following 
conditions: 

The above copyright notice and this permission notice shall be 
included in all copies or substantial portions of the Software. 

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR 
OTHER DEALINGS IN THE SOFTWARE. 
*/ 

#pragma once

//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <iostream>
//Moved to precompiled header: #include <sstream>
//Moved to precompiled header: #include <stack>
//Moved to precompiled header: #include <cassert>

namespace xml
{
	class element;

	// xml::writer class
	class writer
	{
		friend element;
	public:
		// writer must be bound to an ostream
		writer(std::ostream& os) : os(os), need_header(true), m_NewLine(true) {}
		~writer(void) { assert(elements.empty()); }
		void setHeader(LPCSTR header) { headestring = header;} ;
		void setNewLine(bool NewLine){ m_NewLine = NewLine; };

	private:
		// write XML header, if necessary
		writer& header()
		{
			if (need_header)
			{
				if(headestring.empty() ) 
				{
					os << "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
				}
				else
				{
					os << headestring;
				}
				need_header = false;
			}
			return *this;
		}

		// write a single character to the output stream
		writer& putc(char c)
		{
			os.put(c);
			return *this;
		}

		// write a string to the output stream
		writer& puts(const char* str)
		{
			os << str;
			return *this;
		}

	private:
		std::ostream& os; // output stream
		bool need_header; // have we written an XML header yet?
		std::stack<element*> elements; // stack of open element tags
		bool m_NewLine;
		std::string headestring;
	};

	// xml::element class
	class element
	{
	public:
		// create a new element tag, bound to an xml::writer
		element(const char* name, writer& wr) : m_name(name), m_writer(wr)
		{
			assert(m_name != nullptr);
			check_parent( true );
			wr.header().putc('<').puts(name);
			tagopen = true;
			m_writer.elements.push(this);
		}

		// close the current element tag
		~element()
		{
			if (!m_writer.elements.empty() && m_writer.elements.top() == this)
			{
				m_writer.elements.pop();
				if (tagopen)
				{
					m_writer.puts("/>");
				}
				else
				{
					m_writer.puts("</").puts(m_name).puts(">");
				}
				if( m_writer.m_NewLine )
				{
					m_writer.putc('\n');
				}
			}
		}

		// write an attribute for the current element
		element& attr(const char* name, const char* value)
		{
			assert(name != 0);
			assert(value != 0);
			assert(tagopen);
			m_writer.putc(' ').puts(name).puts("=\"");
			qputs(value);
			m_writer.putc('"');
			return *this;
		}

		// attr() overload for std::string type
		element& attr(const char* name, const std::string& value) { return attr(name, value.c_str()); }

		// attr() function template for all streamable types
		template <class T>
		element& attr(const char* name, T value)
		{
			std::stringstream ss;
			ss << value;
			attr(name, ss.str());
			return *this;
		}

		// write text content for the current element
		element& contents(const char* str)
		{
			assert(str != 0);
			check_parent( false );
			qputs(str);
			return *this;
		}

		// contents() overload for std::string type
		element& contents(const std::string& str) { return contents(str.c_str()); }

		// contents() function template for all streamable types
		template <class T>
		element& contents(T value)
		{
			std::stringstream ss;
			ss << value;
			contents(ss.str());
			return *this;
		}

		// write CDATA section
		element& cdata(const char* str) {
			assert(str != 0);
			check_parent( false );
			m_writer.puts("<![CDATA[");
			m_writer.puts(str);
			m_writer.puts("]]>");
			return *this;
		}

		// cdata() overload for std::string type
		element& cdata(const std::string& str) { return cdata(str.c_str()); }

	private:
		// write a string quoting characters which have meaning in xml
		element& qputs(const char* str)
		{
			for (; *str; ++str)
				switch (*str)
				{
					case '&': m_writer.puts("&amp;"); break;
					case '<': m_writer.puts("&lt;"); break;
					case '>': m_writer.puts("&gt;"); break;
					case '\'': m_writer.puts("&apos;"); break;
					case '"': m_writer.puts("&quot;"); break;
					default: m_writer.putc(*str); break;
				}
			return *this;
		}

		// check to see if we have a parent tag which needs to be closed
		void check_parent(bool NewLine)
		{
			if (!m_writer.elements.empty() && m_writer.elements.top()->tagopen)
			{
				if(NewLine && m_writer.m_NewLine)
					m_writer.puts(">\n");
				else
					m_writer.putc('>');

				m_writer.elements.top()->tagopen = false;
			}
		}

	private:
		writer& m_writer; // bound XML writer
		const char* m_name; // name of current element
		bool tagopen; // is the element tag for this element still open?
	};
}

