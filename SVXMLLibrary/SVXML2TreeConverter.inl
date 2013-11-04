//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVXML2TreeConverter
//* .File Name       : $Workfile:   SVXML2TreeConverter.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:49:20  $
//******************************************************************************

#ifndef INCL_SVXML2TREECONVERTER_INL
#define INCL_SVXML2TREECONVERTER_INL

#include "SVNavigateTreeClass.h"

template<typename SVT_TREE>
HRESULT SVXML2TreeConverter::CopyToChildTree(SVXMLClass& rXml, MSXML2::IXMLDOMNodeListPtr pChildren, SVT_TREE& rTree, typename SVT_TREE::SVBranchHandle& parent)
{
	HRESULT hr = S_OK;
	if (pChildren)
	{
		for (int i = 0;i < pChildren->length && hr == S_OK;i++)
		{
			MSXML2::IXMLDOMNodePtr pChild = pChildren->item[i];
			if (pChild)
			{
				// get Name, Type and Attributes
				if (SVXML2TreeConverter::IsDataElement(rXml, pChild))
				{
					SVString name;
					hr = SVXML2TreeConverter::getName(rXml, pChild, name);
					if (hr == S_OK)
					{
						_variant_t value;
						hr = SVXML2TreeConverter::getValue(rXml, pChild, value);
						if (hr == S_OK)
						{
							SVNavigateTreeClass::AddItem(rTree, parent, name.ToString(), value);
						}
					}
				}
				else if (SVXML2TreeConverter::IsNodeElement(rXml, pChild))
				{
					typename SVT_TREE::SVBranchHandle branch;
					// Node Element
					SVString name;
					hr = SVXML2TreeConverter::getName(rXml, pChild, name);
					if (hr == S_OK)
					{
						if (SVXML2TreeConverter::IsSafeArray(pChild))
						{
							hr = CopySafeArrayToTree(rXml, pChild, rTree, parent, name);
						}
						else
						{
							SVNavigateTreeClass::AddBranch(rTree, parent, name.ToString(), &branch);
							MSXML2::IXMLDOMNodeListPtr pChildList = pChild->childNodes;
							if (pChildList)
							{
								hr = SVXML2TreeConverter::CopyToChildTree(rXml, pChildList, rTree, branch);
							}
						}
					}
				}
			}
		}
	}
	return hr;
}

template<typename SVT_TREE>
HRESULT SVXML2TreeConverter::CopySafeArrayToTree(SVXMLClass& rXml, MSXML2::IXMLDOMNodePtr pNode, SVT_TREE& rTree, typename SVT_TREE::SVBranchHandle parent, const SVString& rName)
{
	HRESULT hr = S_OK;
	_variant_t arrayValues;
	hr = SVXML2TreeConverter::getSafeArrayData(rXml, pNode, arrayValues);
	if (hr == S_OK)
	{
		typename SVT_TREE::SVLeafHandle leaf;
		SVNavigateTreeClass::AddItem(rTree, parent, rName.ToString(), arrayValues, &leaf);
	}
	else if( SUCCEEDED( hr ) )
	{
		hr = S_OK;
	}
	return hr;
}

// Start at "Base" Element, but "Base" Element  is not in CtreeCtrl as root!
// Start at Named Element, but Named Element  is not in CtreeCtrl as root!
template<typename SVT_TREE>
HRESULT SVXML2TreeConverter::CopyToTree(SVXMLClass& rXml, SVT_TREE& rTree, const _bstr_t& nodeName, bool bInclusive)
{
	HRESULT hr = S_OK;
	MSXML2::IXMLDOMNodePtr pNode = SVXML2TreeConverter::GetStartingNode(rXml, nodeName);
	if (pNode)
	{
		typename SVT_TREE::SVBranchHandle branch;

		if (bInclusive)
		{
			// Node Element
			SVString name;
			hr = SVXML2TreeConverter::getName(rXml, pNode, name);
			if (hr == S_OK)
			{
				if (SVXML2TreeConverter::IsSafeArray(pNode))
				{
					hr = CopySafeArrayToTree(rXml, pNode, rTree, NULL, name);
				}
				else
				{
					SVNavigateTreeClass::AddBranch(rTree, NULL, name.ToString(), &branch);
							
					MSXML2::IXMLDOMNodeListPtr pChildList = pNode->childNodes;
					if (pChildList)
					{
						hr = SVXML2TreeConverter::CopyToChildTree(rXml, pChildList, rTree, branch);
					}
				}
			}
		}
		else
		{
			rTree.GetRoot( branch );

			MSXML2::IXMLDOMNodeListPtr pChildren = pNode->childNodes;
			if (pChildren)
			{
				hr = CopyToChildTree(rXml, pChildren, rTree, branch);
			}
			/*
			for (int i = 0;i < pChildren->length;i++)
			{
				MSXML2::IXMLDOMNodePtr pChild = pChildren->item[i];
				if (pChild)
				{
					// get Name, Type and Attributes
					if (SVXML2TreeConverter::IsDataElement(rXml, pChild))
					{
						SVString name;
						hr = SVXML2TreeConverter::getName(rXml, pChild, name);
						if (hr == S_OK)
						{
							_variant_t value;
							hr = SVXML2TreeConverter::getValue(rXml, pChild, value);
							if (hr == S_OK)
							{
								SVNavigateTreeClass::AddItem(rTree, branch, name.ToString(), value);
							}
						}
					}
					else if (SVXML2TreeConverter::IsNodeElement(rXml, pChild))
					{
						SVString name;
						hr = SVXML2TreeConverter::getName(rXml, pChild, name);
						if (hr == S_OK)
						{
							if (SVXML2TreeConverter::IsSafeArray(pChild))
							{
								hr = CopySafeArrayToTree(rXml, pChild, rTree, branch, name);
							}
							else
							{
								SVNavigateTreeClass::AddBranch(rTree, NULL, name.ToString(), &branch);
								
								MSXML2::IXMLDOMNodeListPtr pChildList = pChild->childNodes;
								if (pChildList)
								{
									hr = SVXML2TreeConverter::CopyToChildTree(rXml, pChildList, rTree, branch);
								}
							}
						}
					}
				}
			}
			*/
		}
	}
	else
	{
		hr = -151000;
	}
	return hr;
}

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVXMLLibrary\SVXML2TreeConverter.inl_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:49:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Oct 2012 10:40:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code and projects to fix whitespace issues in the XML file and consolidate functionality for load and saving to this library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/