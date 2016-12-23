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
#include "SVParserProgressStruct.h"
#include "ObjectInterfaces/SVUserMessage.h"
#include "CameraLibrary/SVDeviceParams.h"

unsigned long SVObjectScriptParserClass::GetParserHandle()
{
	static unsigned long handle=0;
	return ++handle;
}

UINT SVObjectScriptParserClass::ParserThread( LPVOID lParam )
{
	SVObjectScriptParserClass* pParser = reinterpret_cast<SVObjectScriptParserClass*> (lParam);
	
	pParser->Parse(); 
	pParser->Complete();
	return 0;
}

SVObjectScriptParserClass::SVObjectScriptParserClass(SVObjectScriptParserBase* pParser)
: m_pParser(pParser)
{
}

SVObjectScriptParserClass::~SVObjectScriptParserClass()
{
	if (m_pParser)
	{
		delete m_pParser;
		m_pParser = nullptr;
	}
}

const GUID& SVObjectScriptParserClass::GetOwnerGuid() const
{
	return m_pParser->GetOwnerGuid();
}

SVObjectClass* SVObjectScriptParserClass::GetOwnerObject()
{
	return m_pParser->GetOwnerObject();
}

unsigned long SVObjectScriptParserClass::GetTotal() const
{
	return static_cast<int>(m_pParser->GetTotal());
}

HRESULT SVObjectScriptParserClass::Parse()
{
	HRESULT hr = E_FAIL;
	if( m_pParser->GetOwnerObject() )
	{
		hr = m_pParser->DoParse();
	}
	return hr;
}

void SVObjectScriptParserClass::Complete()
{
	m_pParser->Complete();
}

