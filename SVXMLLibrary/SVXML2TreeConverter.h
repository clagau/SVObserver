//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVXML2TreeConverter
//* .File Name       : $Workfile:   SVXML2TreeConverter.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:49:04  $
//******************************************************************************

#ifndef INCL_SVXML2TREECONVERTER_H
#define INCL_SVXML2TREECONVERTER_H

#include "SVXMLClass.h"
#include "SVMaterialsLibrary/SVMaterialsTree.h"

class SVXML2TreeConverter
{
public:
	template<typename SVT_TREE>
	static HRESULT CopyToTree(SVXMLClass& rXml, SVT_TREE& rTree, const _bstr_t& nodeName, bool bInclusive=true);
	
private:
	SVXML2TreeConverter(); // Non constructable
	~SVXML2TreeConverter();

	template<typename SVT_TREE>
	static HRESULT CopyToChildTree(SVXMLClass& rXml, MSXML2::IXMLDOMNodeListPtr pChildren, SVT_TREE& rTree, typename SVT_TREE::SVBranchHandle& parent);

	template<typename SVT_TREE>
	static HRESULT CopySafeArrayToTree(SVXMLClass& rXml, MSXML2::IXMLDOMNodePtr pNode, SVT_TREE& rTree, typename SVT_TREE::SVBranchHandle parent, const SVString& rName);

	static MSXML2::IXMLDOMNodePtr GetStartingNode(SVXMLClass& rXml, const _bstr_t& nodeName);
	static bool IsDataElement(SVXMLClass& rXml, MSXML2::IXMLDOMNodePtr pNode);
	static bool IsNodeElement(SVXMLClass& rXml, MSXML2::IXMLDOMNodePtr pNode);
	static HRESULT getName(SVXMLClass& rXml, MSXML2::IXMLDOMNodePtr pNode, SVString& nameStr);
	static HRESULT getValue(SVXMLClass& rXml, MSXML2::IXMLDOMNodePtr pNode, _variant_t& rValue);
	static bool IsSafeArray(MSXML2::IXMLDOMNodePtr pNode);
	static HRESULT getSafeArrayData(SVXMLClass& rXml, MSXML2::IXMLDOMNodePtr pNode, _variant_t& value);
};

#include "SVXML2TreeConverter.inl"

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVXMLLibrary\SVXML2TreeConverter.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:49:04   bWalter
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