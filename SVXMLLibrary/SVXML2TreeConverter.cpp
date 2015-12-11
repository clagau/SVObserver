//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVXML2TreeConverter
//* .File Name       : $Workfile:   SVXML2TreeConverter.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:48:48  $
//******************************************************************************
#include "stdafx.h"
#include <map>
#include <boost/assign/list_of.hpp>
#include <comdef.h>
#include "SVXML2TreeConverter.h"
#include "SVVariantCustomTypes.h"
#include "SVVariantConverter.h"
#include "SVXMLSafeArrayConverter.h"
#include "SVNavigateTree.h"

typedef std::map<_bstr_t, long> SVVariantTypeNameTable;
SVVariantTypeNameTable vtTable = boost::assign::map_list_of<_bstr_t, long>
	(_bstr_t(L"VT_ARRAY"), VT_ARRAY)
	(_bstr_t(L"VT_EMPTY"), VT_EMPTY)
	(_bstr_t(L"SV_ARRAYNODE"), VT_SVARRAYNODE)
	(_bstr_t(L"VT_BSTR"), VT_BSTR)
	(_bstr_t(L"VT_BOOL"), VT_BOOL)
	(_bstr_t(L"VT_INT"), VT_I4)
	(_bstr_t(L"VT_I4"), VT_I4)
	(_bstr_t(L"VT_UINT"), VT_UI4)
	(_bstr_t(L"VT_UI4"), VT_UI4)
	(_bstr_t(L"VT_R4"), VT_R4)
	(_bstr_t(L"VT_R8"), VT_R8)
	(_bstr_t(L"VT_I8"), VT_I8)
	(_bstr_t(L"VT_UI8"), VT_UI8)
	(_bstr_t(L"VT_UI1"), VT_UI1)
	(_bstr_t(L"VT_I1"), VT_I1)
;
		
// this doesn't handle array data per se (VT_ARRAY)
static _variant_t ConvertToType(_bstr_t& type, _bstr_t& text)
{
	_variant_t value;
	SVVariantConverter::RestoreVariant(text.GetBSTR(), type.GetBSTR(), value.GetAddress());
	return value;
}

bool SVXML2TreeConverter::IsDataElement(SVXMLClass& rXml, MSXML2::IXMLDOMNodePtr pNode)
{
	_bstr_t name;
	pNode->get_nodeName(name.GetAddress());

	return (rXml.IsElementData(name.GetBSTR()) == S_OK);
}

bool SVXML2TreeConverter::IsNodeElement(SVXMLClass& rXml, MSXML2::IXMLDOMNodePtr pNode)
{
	_bstr_t name;
	pNode->get_nodeName(name.GetAddress());

	return (rXml.IsElementNode(name.GetBSTR()) == S_OK);
}

HRESULT SVXML2TreeConverter::getName(SVXMLClass& rXml, MSXML2::IXMLDOMNodePtr pNode, SVString& nameStr)
{
	_bstr_t name;
/*
	MSXML2::IXMLDOMNamedNodeMapPtr attributesPtr = pNode->attributes;
	if (attributesPtr)
	{
		MSXML2::IXMLDOMNodePtr attrNamePtr = attributesPtr->getNamedItem(L"Name");
		if (attrNamePtr)
		{
			_variant_t val;
			attrNamePtr->get_nodeValue(val.GetAddress());
			name = val.bstrVal;
		}
	}
*/
	MSXML2::IXMLDOMElement* pElementPtr = NULL;
	MSXML2::IXMLDOMElementPtr domElementPtr;

	HRESULT hr = pNode->QueryInterface(__uuidof(MSXML2::IXMLDOMElement), (void**)&pElementPtr);
	if (hr == S_OK)
	{
		domElementPtr.Attach(pElementPtr);
		hr = rXml.GetElementName(domElementPtr, name.GetAddress());
		if (hr == S_OK)
		{
			nameStr = name;
		}
	}
	else
	{
		hr = -15102;
	}
	return hr;
}

HRESULT SVXML2TreeConverter::getValue(SVXMLClass& rXml, MSXML2::IXMLDOMNodePtr pNode, _variant_t& rValue)
{
/*
	_bstr_t textValue = pNode->Gettext(); // text
	MSXML2::IXMLDOMNamedNodeMapPtr attributesPtr = pNode->attributes;

	if (attributesPtr)
	{
		MSXML2::IXMLDOMNodePtr attrTypePtr = attributesPtr->getNamedItem(L"Type");
		if (attrTypePtr)
		{
			_variant_t val;
			attrTypePtr->get_nodeValue(val.GetAddress());
			_bstr_t typeStr(val);
			value = ConvertToType(typeStr, textValue);
		}
	}
*/
	MSXML2::IXMLDOMElement* pElementPtr = NULL;
	MSXML2::IXMLDOMElementPtr domElementPtr;

	HRESULT hr = pNode->QueryInterface(__uuidof(MSXML2::IXMLDOMElement), (void**)&pElementPtr);
	if (hr == S_OK)
	{
		domElementPtr.Attach(pElementPtr);
		hr = rXml.GetElementData(domElementPtr, rValue.GetAddress());
	}
	else
	{
		hr = -15103;
	}
	return hr;
}

static long getType(MSXML2::IXMLDOMNodePtr pNode)
{
	long type = VT_EMPTY;
	MSXML2::IXMLDOMNamedNodeMapPtr attributesPtr = pNode->attributes;

	if (attributesPtr)
	{
		MSXML2::IXMLDOMNodePtr attrTypePtr = attributesPtr->getNamedItem(L"Type");
		if (attrTypePtr)
		{
			_variant_t val;
			attrTypePtr->get_nodeValue(val.GetAddress());
			_bstr_t typeStr(val);

			SVVariantTypeNameTable::const_iterator it = vtTable.find(typeStr);
			if (it != vtTable.end())
			{
				type = it->second;
			}
		}
	}
	return type;
}

bool SVXML2TreeConverter::IsSafeArray(MSXML2::IXMLDOMNodePtr pNode)
{
	long type = getType(pNode);
	return (type == VT_ARRAY);
}

HRESULT SVXML2TreeConverter::getSafeArrayData(SVXMLClass& rXml, MSXML2::IXMLDOMNodePtr pNode, _variant_t& value)
{
	MSXML2::IXMLDOMElementPtr aDOMElementPtr;
	MSXML2::IXMLDOMElement* pDOMElement = NULL;
	HRESULT hr = pNode->QueryInterface(__uuidof(MSXML2::IXMLDOMElement), (void **)&pDOMElement);
	if (hr == S_OK && pDOMElement)
	{
		aDOMElementPtr.Attach(pDOMElement);
		hr = SVXMLSafeArrayConverter::CopyDOMNodeToSafeArray(rXml, aDOMElementPtr, value);
	}
	return hr;
}

MSXML2::IXMLDOMNodePtr SVXML2TreeConverter::GetStartingNode(SVXMLClass& rXml, const _bstr_t& nodeName)
{
	MSXML2::IXMLDOMNodePtr pNode;
	MSXML2::IXMLDOMElementPtr pRoot;
	rXml.GetRootNode(pRoot);
	if (pRoot)
	{
		_bstr_t bstrQueryNameSpace = L"xmlns:svr1=\"x-schema:#SVR00001\"";
		HRESULT hr = rXml.SetQueryNameSpace(bstrQueryNameSpace);
		if (hr == S_OK)
		{
			_bstr_t query = L"//svr1:NODE[@Name='";
			query += nodeName;
			query += "']";
			pNode = pRoot->selectSingleNode(query);
		}
	}
	return pNode;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVXMLLibrary\SVXML2TreeConverter.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:48:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Oct 2012 10:40:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code and projects to fix whitespace issues in the XML file and consolidate functionality for load and saving to this library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/