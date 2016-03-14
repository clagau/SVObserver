//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectScriptParserBase
//* .File Name       : $Workfile:   SVObjectScriptParserBase.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:41:08  $
//******************************************************************************

#ifndef INCL_SVOBJECTSCRIPTPARSERBASE_H
#define INCL_SVOBJECTSCRIPTPARSERBASE_H

#include "SVObjectLibrary/SVObjectClass.h"

// base class to parser implementations
class SVObjectScriptParserBase
{
public:
	SVObjectScriptParserBase(unsigned long parserHandle, const GUID& OwnerGuid, SVObjectClass* pOwnerObject, CWnd* pWnd);
	
	virtual ~SVObjectScriptParserBase();
	
	const GUID& GetOwnerGuid() const;
	SVObjectClass* GetOwnerObject();

	virtual size_t GetTotal() const = 0;
	virtual HRESULT DoParse()=0;

	void Complete();
	void UpdateProgress( size_t amtProcessed, size_t amtTotal );

protected:
	GUID m_OwnerGuid;
	SVObjectClass* m_pOwnerObject;
	CWnd* m_pWnd;

	unsigned long m_parserHandle;
};

#endif

