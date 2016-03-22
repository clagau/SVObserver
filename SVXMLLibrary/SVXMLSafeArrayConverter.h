//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVXMLSafeArrayConverter
//* .File Name       : $Workfile:   SVXMLSafeArrayConverter.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:56:58  $
//******************************************************************************

#pragma once

#include "SVXMLClass.h"

class SVXMLSafeArrayConverter
{
public:
	static HRESULT CopyDOMNodeToSafeArray(SVXMLClass& rXml, SVXML::IXMLDOMElementPtr aDOMElementPtr, _variant_t& value);
	static HRESULT CopySafeArrayToDOMNode(SVXMLClass& rXml, BSTR p_bstrSafeArrayName, SAFEARRAY* p_InputSafeArray, SVXML::IXMLDOMElementPtr& p_roDOMElementPtr);

private:
	SVXMLSafeArrayConverter(); // Non Constructable
	~SVXMLSafeArrayConverter();

	static HRESULT CopyDOMChildNodesToSafeArrayNodes(SVXMLClass& rXml, SVXML::IXMLDOMElementPtr aDOMParentElementPtr, long alDimension, long* alpIndicesArray, SAFEARRAY* apSafeArray);
	static HRESULT GetSafeArrayData(SVXMLClass& rXml, SVXML::IXMLDOMElementPtr aDOMElementPtr, long* alpSafeArrayDepth, long* alpSafeArrayType, SAFEARRAYBOUND* apSafeArrayBounds);

	static HRESULT CreateDOMChildSafeArrayNodes(SVXMLClass& rXml, SVXML::IXMLDOMElementPtr& p_roDOMParentElementPtr, long p_lDimension, long* p_plIndices, SAFEARRAY* p_InputSafeArray);
};

