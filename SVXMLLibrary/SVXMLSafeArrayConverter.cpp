//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVXMLSafeArrayConverter
//* .File Name       : $Workfile:   SVXMLSafeArrayConverter.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:56:42  $
//******************************************************************************
#include "stdafx.h"
#import <msxml6.dll> // MSXML
//Moved to precompiled header: #include <atlbase.h>
//Moved to precompiled header: #include <comdef.h>
#include "SVXMLSafeArrayConverter.h"
#include "SVXMLLibraryGlobals.h"
#include "SVVariantCustomTypes.h"
#include "SVMessage\SVMessage.h"
#include "SVUtilityLibrary\SVUtilityGlobals.h"

static const long g_lXMLSafeArrayMaxDimensions = 12;

HRESULT SVXMLSafeArrayConverter::CopyDOMNodeToSafeArray(SVXMLClass& rXml, SVXML::IXMLDOMElementPtr aDOMElementPtr, _variant_t& value)
{
	HRESULT hr = 0;
	
	long lSafeArrayDepth = 0;
	long lSafeArrayType;
	long lIndices [g_lXMLSafeArrayMaxDimensions];	// This puts a dimension limit on the SAFEARRAYs that can be handled.

	SAFEARRAYBOUND oSafeArrayBounds [g_lXMLSafeArrayMaxDimensions];
	SAFEARRAY* pSafeArray = nullptr;

	for (long lCounter = 0; lCounter < g_lXMLSafeArrayMaxDimensions; lCounter++)
	{
		lIndices [lCounter] = 0;
	}

	value.Clear();
	
	hr = SVXMLSafeArrayConverter::GetSafeArrayData(rXml, aDOMElementPtr, &lSafeArrayDepth, &lSafeArrayType, oSafeArrayBounds);
	if (S_OK == hr)
	{
		pSafeArray = ::SafeArrayCreate((VARTYPE) lSafeArrayType, lSafeArrayDepth, oSafeArrayBounds);
		if (nullptr == pSafeArray)
		{
			hr = -1787;
		}
	}
	if (S_OK == hr)
	{
//-	The element that is being sent here is the root which contains the type
//-	VT_ARRAY.  At this time the DOM element does not also contain the array
//-	type.  Array type data must come from the elements.  Here the VT_ARRAY 
//-	is being or'ed with the data type. (ex. VT_ARRAY | VT_I4)
		value.vt = VT_ARRAY | (VARTYPE)lSafeArrayType;
		value.parray = pSafeArray;

		hr = SVXMLSafeArrayConverter::CopyDOMChildNodesToSafeArrayNodes(rXml, aDOMElementPtr, 1, lIndices, pSafeArray);
/*
//-	The tree element data was set during creation, but in this case it is 
//-	set again with updated data that was not available during creation.
		hr = svmpAttachedTree->SetNodeData(alTreeNodeHandle, vElementValue);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}
*/
	}
	return hr;
}

HRESULT SVXMLSafeArrayConverter::CopyDOMChildNodesToSafeArrayNodes(SVXMLClass& rXml, SVXML::IXMLDOMElementPtr aDOMParentElementPtr, long alDimension, long* alpIndicesArray, SAFEARRAY* apSafeArray)
{
	HRESULT hr = 0;
	VARIANT vDOMElementData;
	::VariantClear(&vDOMElementData);

	SVXML::IXMLDOMElementPtr oDOMChildElementPtr = rXml.GetFirstElementChild(aDOMParentElementPtr);

	if (nullptr == oDOMChildElementPtr)
	{
//-		This is exceptable.  Empty array.
//-		This is acceptable?  Empty array.
		hr = 2;
	}
	while (S_OK == hr && nullptr != oDOMChildElementPtr)
	{
		hr = rXml.GetElementData(oDOMChildElementPtr, &vDOMElementData);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		if (vDOMElementData.vt == VT_SVARRAYNODE)
		{
			hr = SVXMLSafeArrayConverter::CopyDOMChildNodesToSafeArrayNodes(rXml, oDOMChildElementPtr, alDimension + 1, alpIndicesArray, apSafeArray);
			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				break;
			}
		}
		else
		{
//-			These are assumed to be the array elements.
			switch (vDOMElementData.vt)
			{
				case VT_I4:
				{
					hr = ::SafeArrayPutElement (apSafeArray, &(alpIndicesArray[alDimension]), &vDOMElementData.lVal);
					if( SEV_SUCCESS != SV_SEVERITY( hr ) )
					{
						hr = -1790;
					}
				}
				break;

				case VT_UI4:
				{
					hr = ::SafeArrayPutElement(apSafeArray, &(alpIndicesArray[alDimension]), &vDOMElementData.ulVal);
					if( SEV_SUCCESS != SV_SEVERITY( hr ) )
					{
						hr = -1791;
					}
				}
				break;

				case VT_BSTR:
				{
					hr = ::SafeArrayPutElement(apSafeArray, &(alpIndicesArray[alDimension]), &vDOMElementData.bstrVal);
					if( SEV_SUCCESS != SV_SEVERITY( hr ) )
					{
						hr = -1792;
					}
				}
				break;

				default:
				{
					hr = -1789;
				}
				break;
			}
		}
		if (S_OK == hr)
		{
			alpIndicesArray[alDimension] = alpIndicesArray [alDimension] + 1;

			oDOMChildElementPtr = rXml.GetNextElementSibling(oDOMChildElementPtr);
		}
	}
//-We specifically do not want to call VariantClear (vDOMElementData); on 
//-vDOMElementData ????
	return hr;
}

HRESULT SVXMLSafeArrayConverter::GetSafeArrayData(SVXMLClass& rXml, SVXML::IXMLDOMElementPtr aDOMElementPtr, long* alpSafeArrayDepth, long* alpSafeArrayType, SAFEARRAYBOUND* apSafeArrayBounds)
{
	HRESULT	hr = 0;
	long lDepth = 0;
	long lType = 0;
	long lNbrOfChildren = 0;

	VARIANT	vElementValue;
	::VariantClear(&vElementValue);

//-	The passed in element aDOMElementPtr is assumed to be the base of a 
//-	SAFEARRAY.
		
	SVXML::IXMLDOMElementPtr oDOMElementPtr = aDOMElementPtr;

	while (nullptr != oDOMElementPtr) // This loop actually exits internally.
	{
		if (lDepth >= g_lXMLSafeArrayMaxDimensions)
		{
			hr = -1786;
			break;
		}

		hr = rXml.GetElementNbrOfChildren(oDOMElementPtr, &lNbrOfChildren);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		apSafeArrayBounds [lDepth].lLbound = 0;
		apSafeArrayBounds [lDepth].cElements = lNbrOfChildren;

		lDepth++;
	
		if (lNbrOfChildren == 0)
		{
//-			This case can only happen on the first itteration (parent is 
//-			VT_ARRAY).  Depth must be at least 1.

//-			Nothing bad, just no data elements.
//-         This senario represents a SAFEARRAY with no elements.
//-			It is understood that when we create a SAFEARRAY with no
//-			element, we loose the data type of the SAFEARRAY.  To implement 
//-			fix this, it is recommended that information about the array type
//-			be added to the VT_ARRAY node.
			lType = VT_I4;
			break;
		}

		SVXML::IXMLDOMElementPtr oDOMChildElementPtr = rXml.GetFirstElementChild(oDOMElementPtr);
		if (nullptr == oDOMChildElementPtr)
		{
			hr = -1792;
			break;
		}

		hr = rXml.GetElementData(oDOMChildElementPtr, &vElementValue);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		if (vElementValue.vt != VT_SVARRAYNODE)
		{
//-			element type is NOT "SV_ARRAYNODE"
//-			We assume that the node is then data.
//-			This is considered the "normal" exit point.
			lType =  vElementValue.vt;
			break;
		}
		oDOMElementPtr = oDOMChildElementPtr;
	}

	if( SEV_SUCCESS != SV_SEVERITY( hr ) )
	{
	}
	else
	{
		*alpSafeArrayDepth = lDepth;
		*alpSafeArrayType = lType;
	}
	return hr;
}

HRESULT SVXMLSafeArrayConverter::CopySafeArrayToDOMNode(SVXMLClass& rXml, BSTR p_bstrSafeArrayName, SAFEARRAY* p_InputSafeArray, SVXML::IXMLDOMElementPtr& p_roDOMElementPtr)
{
	HRESULT	hr;

	SVXML::IXMLDOMElementPtr oDOMElementPtr;

	long lCounter;
	long lIndices [g_lXMLSafeArrayMaxDimensions];	// This puts a dimension limit on the SAFEARRAYs that can be handled.
	SVBStr	bstrElementTag;

	VARIANT	vDummy;

	hr = 0;
	for (lCounter = 0; lCounter < g_lXMLSafeArrayMaxDimensions; lCounter++)
	{
		lIndices [lCounter] = 0;
	}

	while (1)
	{
		vDummy.vt = VT_ARRAY;
		vDummy.parray = 0;

//-	Working around const.
		hr = bstrElementTag.CopyFromWChar ((WCHAR*) g_wcsNode);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

//-   It is understood that the SAFEARRAY base node violates the separate 
//-	 node/data element rules, and that this "node" is being created with
//-	 the CreateDOMData () function.

		hr = rXml.svmopDOM->CreateDOMNodeElement(bstrElementTag, p_bstrSafeArrayName, &vDummy, oDOMElementPtr);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		hr = SVXMLSafeArrayConverter::CreateDOMChildSafeArrayNodes (rXml, oDOMElementPtr, 1, lIndices, p_InputSafeArray);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}
		break;
	} // while (1)

	if( SEV_SUCCESS != SV_SEVERITY( hr ) )
	{
	}
	else
	{
		p_roDOMElementPtr = oDOMElementPtr;
	}
	return hr;
}

HRESULT SVXMLSafeArrayConverter::CreateDOMChildSafeArrayNodes(SVXMLClass& rXml, SVXML::IXMLDOMElementPtr& p_roDOMParentElementPtr, long p_lDimension, long* p_plIndices, SAFEARRAY* p_InputSafeArray)
{
	USES_CONVERSION;

	HRESULT hr = 0;

	SVXML::IXMLDOMElementPtr oDOMNewElementPtr;

	unsigned short unArrayType;

	long lDimension;
	long lInputDimensionMinus1;
	long lNumberOfElements;
	long lTempBuildBufferIndex;

	long lUpperBound;
	long lLowerBound;

	long lIndex;
	long lIndiceCntr;

	VARIANT vTreeNodeData;
	VARIANT vDummy;

	WCHAR wszTempBuildBuffer [500];

	SVBStr bstrNodeName;
	SVBStr bstrElementTag;

	lTempBuildBufferIndex = 0;

	vTreeNodeData.vt = VT_EMPTY;
	vTreeNodeData.lVal = 0;

	while (1)
	{
		lDimension = SafeArrayGetDim (p_InputSafeArray);
		if (lDimension > g_lXMLSafeArrayMaxDimensions)
		{
			hr = -1788;
			break;
		}

		lInputDimensionMinus1 = p_lDimension - 1;

		hr = SafeArrayGetUBound (p_InputSafeArray, p_lDimension, &lUpperBound);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			hr = -1735;
			break;
		}

		hr = SafeArrayGetLBound (p_InputSafeArray, p_lDimension, &lLowerBound);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			hr = -1736;
			break;
		}

		hr = SafeArrayGetVartype(p_InputSafeArray, &unArrayType);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			hr = -1737;
			break;
		}
		
//-	If NumberOfElements ends up equalling -1, this means that the SAFEARRAY
//-	is empty.  0 is not less than -1 so the for loop will not execute.  All
//-	should be ok in this case.
		lNumberOfElements = lUpperBound - lLowerBound + 1;

		for (lIndex = 0; lIndex < lNumberOfElements; lIndex++)
		{
			p_plIndices [lInputDimensionMinus1] = lIndex;

//-		If on the final dimension, we're creating data.  If not, we're still
//-	   creating nodes.
			if (p_lDimension == lDimension)
			{
				lTempBuildBufferIndex = swprintf (wszTempBuildBuffer, A2W("DataIndex"));
			}
			else
			{
				lTempBuildBufferIndex = swprintf (wszTempBuildBuffer, A2W("ArrayNode"));
			}

//-		Build node name by appending indices.
//-		 ex.  ArrayNode_1_6_4
			for (lIndiceCntr = 0; lIndiceCntr < p_lDimension; lIndiceCntr++)
			{
//-			The indice array is base 0 but we want the text to show as base 
//-			1.
				lTempBuildBufferIndex = lTempBuildBufferIndex + 
										swprintf (wszTempBuildBuffer + lTempBuildBufferIndex, 
													A2W("_%d"), 
													p_plIndices [lIndiceCntr] + 1);
			}

			hr = bstrNodeName.CopyFromWChar (wszTempBuildBuffer);
			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				break;
			}

//-		If on the final dimension, we're creating data.  If not, we're still
//-		creating nodes.
			if (p_lDimension == lDimension)
			{
				vTreeNodeData.vt = unArrayType;

				switch (unArrayType)
				{
				case VT_I4:
					{
						hr = SafeArrayGetElement(p_InputSafeArray, p_plIndices, &vTreeNodeData.lVal);
						if( SEV_SUCCESS != SV_SEVERITY( hr ) )
						{
							hr = -1738;
						}
					}
					break;

				case VT_UI4:
					{
						hr = SafeArrayGetElement(p_InputSafeArray, p_plIndices, &vTreeNodeData.ulVal);
						if( SEV_SUCCESS != SV_SEVERITY( hr ) )
						{
							hr = -1739;
						}
					}
					break;

				case VT_BSTR:
					{
						hr = SafeArrayGetElement(p_InputSafeArray, p_plIndices, &vTreeNodeData.bstrVal);
						if( SEV_SUCCESS != SV_SEVERITY( hr ) )
						{
							hr = -1780;
						}
					}
					break;

				default:
					{
						hr = -1740;
					}
					break;
				} // switch (lArrayType)

				if( S_OK == hr)
				{
					hr = rXml.CreateDOMChildData(p_roDOMParentElementPtr, bstrNodeName, &vTreeNodeData, oDOMNewElementPtr);
					if( SEV_SUCCESS != SV_SEVERITY( hr ) )
					{
						break;
					}
				}
				else
				{
					vTreeNodeData.vt = unArrayType;
				}
			} // if (p_lDimension == lDimension)
			else
			{
				vDummy.vt = (unsigned short) VT_SVARRAYNODE;
				vDummy.parray = 0;

//-			Working around const.
				hr = bstrElementTag.CopyFromWChar ((WCHAR*) g_wcsNode);
				if( SEV_SUCCESS != SV_SEVERITY( hr ) )
				{
					break;
				}

				hr = rXml.svmopDOM->CreateDOMNodeElement(bstrElementTag, bstrNodeName, &vDummy, oDOMNewElementPtr);
				if( SEV_SUCCESS != SV_SEVERITY( hr ) )
				{
					break;
				}

				hr = SVXMLSafeArrayConverter::CreateDOMChildSafeArrayNodes(rXml, oDOMNewElementPtr, p_lDimension + 1,  p_plIndices, p_InputSafeArray);
				if( SEV_SUCCESS != SV_SEVERITY( hr ) )
				{
					break;
				}

				hr = rXml.AppendChildToDOMNode(p_roDOMParentElementPtr, oDOMNewElementPtr);
				if( SEV_SUCCESS != SV_SEVERITY( hr ) )
				{
					break;
				}
			} // if (p_lDimension == lDimension) else....
		} // for (lIndex = 0; lIndex < lNumberOfElements; lIndex++)
		break;
	}
	::VariantClear(&vTreeNodeData);

	return hr;
}

