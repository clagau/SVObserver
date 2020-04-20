//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectScriptParser
//* .File Name       : $Workfile:   SVObjectScriptParser.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:40:48  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVObjectScriptParserBase.h"
#pragma endregion Includes

class SVObjectScriptParserClass
{
public:	
	SVObjectScriptParserClass(SVObjectScriptParserBase* pParser);
	~SVObjectScriptParserClass();	// not to be inherited from

	HRESULT Parse();
	void Complete();

	uint32_t GetOwnerId() const;
	SVObjectClass* GetOwnerObject();

	unsigned long GetTotal() const;

	static UINT ParserThread( LPVOID lParam );
	static unsigned long GetParserHandle();

protected:	
	SVObjectScriptParserBase* m_pParser;
};

