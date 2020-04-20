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
#pragma once

#pragma region Includes
#pragma endregion Includes

class SVObjectClass;

// base class to parser implementations
class SVObjectScriptParserBase
{
public:
	SVObjectScriptParserBase(unsigned long parserHandle, uint32_t ownerId, SVObjectClass* pOwnerObject, CWnd* pWnd);
	
	virtual ~SVObjectScriptParserBase();
	
	uint32_t GetOwnerId() const;
	SVObjectClass* GetOwnerObject();

	virtual size_t GetTotal() const = 0;
	virtual HRESULT DoParse()=0;

	void Complete();
	void UpdateProgress( size_t amtProcessed, size_t amtTotal );

protected:
	uint32_t m_OwnerId;
	SVObjectClass* m_pOwnerObject;
	CWnd* m_pWnd;

	unsigned long m_parserHandle;
};

