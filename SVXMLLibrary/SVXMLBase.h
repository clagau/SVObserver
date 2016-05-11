//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVXMLBase
//* .File Name       : $Workfile:   SVXMLBase.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:49:36  $
//******************************************************************************

#pragma once

#include "SVDOM.h"

class SVXMLBase
{
public:
	typedef SVXML::IXMLDOMElementPtr SVElementPtr;
	typedef SVXML::IXMLDOMNodePtr SVNodePtr;

	virtual ~SVXMLBase() {}
	virtual HRESULT CreateDOMData( SVElementPtr& arDOMNewElementPtr, BSTR abstrElementName, VARIANT* avpElementData ) = 0;

	virtual HRESULT CreateElement( BSTR p_bstrElementName, SVElementPtr& p_orDOMElementPtr ) = 0;

	virtual HRESULT GetRootNode( SVElementPtr& p_orDOMRootPtr ) = 0;

	virtual HRESULT AppendChildToDOMNode( SVNodePtr p_oParentNodePtr, SVNodePtr p_oChildNodePtr ) = 0;

	virtual HRESULT GetNameSpace( WCHAR* p_wcsNameSpace, long p_lBufferSize ) = 0;

	virtual HRESULT SetQueryNameSpace( BSTR p_bstrQueryNameSpace ) = 0;

};


