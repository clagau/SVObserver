//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectScriptParser
//* .File Name       : $Workfile:   SVObjectScriptParser.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 15:24:36  $
//******************************************************************************

#include "stdafx.h"
#include "SVObjectScriptParser.h"

unsigned long SVObjectScriptParser::GetParserHandle()
{
	static unsigned long handle=0;
	return ++handle;
}

UINT SVObjectScriptParser::ParserThread( LPVOID lParam )
{
	SVObjectScriptParser* pParser = reinterpret_cast<SVObjectScriptParser*> (lParam);
	
	pParser->Parse(); 
	pParser->Complete();
	return 0;
}

SVObjectScriptParser::SVObjectScriptParser(SVObjectScriptParserBase* pParser)
: m_pParser(pParser)
{
}

SVObjectScriptParser::~SVObjectScriptParser()
{
	if (m_pParser)
	{
		delete m_pParser;
		m_pParser = nullptr;
	}
}

uint32_t SVObjectScriptParser::GetOwnerId() const
{
	return m_pParser->GetOwnerId();
}

SVObjectClass* SVObjectScriptParser::GetOwnerObject()
{
	return m_pParser->GetOwnerObject();
}

unsigned long SVObjectScriptParser::GetTotal() const
{
	return static_cast<int>(m_pParser->GetTotal());
}

HRESULT SVObjectScriptParser::Parse()
{
	HRESULT hr = E_FAIL;
	if( m_pParser->GetOwnerObject() )
	{
		hr = m_pParser->DoParse();
	}
	return hr;
}

void SVObjectScriptParser::Complete()
{
	m_pParser->Complete();
}

