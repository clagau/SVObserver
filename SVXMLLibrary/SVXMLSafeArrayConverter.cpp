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
#include <atlbase.h>
#include <comdef.h>
#include "SVXMLSafeArrayConverter.h"
#include "SVVariantCustomTypes.h"

static const long g_lXMLSafeArrayMaxDimensions = 12;

HRESULT SVXMLSafeArrayConverter::CopyDOMNodeToSafeArray(SVXMLClass& rXml, SVXML::IXMLDOMElementPtr aDOMElementPtr, _variant_t& value)
{
	HRESULT hr = 0;
	
	long lSafeArrayDepth = 0;
	long lSafeArrayType;
	long lIndices [g_lXMLSafeArrayMaxDimensions];	// This puts a dimension limit on the SAFEARRAYs that can be handled.

	SAFEARRAYBOUND oSafeArrayBounds [g_lXMLSafeArrayMaxDimensions];
	SAFEARRAY* pSafeArray = NULL;

	for (long lCounter = 0; lCounter < g_lXMLSafeArrayMaxDimensions; lCounter++)
	{
		lIndices [lCounter] = 0;
	}

	value.Clear();
	
	hr = SVXMLSafeArrayConverter::GetSafeArrayData(rXml, aDOMElementPtr, &lSafeArrayDepth, &lSafeArrayType, oSafeArrayBounds);
	if (hr == S_OK)
	{
		pSafeArray = ::SafeArrayCreate((VARTYPE) lSafeArrayType, lSafeArrayDepth, oSafeArrayBounds);
		if (pSafeArray == NULL)
		{
			hr = -1787;
		}
	}
	if (hr == S_OK)
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
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
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

	SVXML::IXMLDOMElementPtr oDOMChildElementPtr = aDOMParentElementPtr->GetfirstChild();

	if (oDOMChildElementPtr == NULL)
	{
//-		This is exceptable.  Empty array.
//-		This is acceptable?  Empty array.
		hr = 2;
	}
	while (hr == S_OK && oDOMChildElementPtr != NULL)
	{
		hr = rXml.GetElementData(oDOMChildElementPtr, &vDOMElementData);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		if (vDOMElementData.vt == VT_SVARRAYNODE)
		{
			hr = SVXMLSafeArrayConverter::CopyDOMChildNodesToSafeArrayNodes(rXml, oDOMChildElementPtr, alDimension + 1, alpIndicesArray, apSafeArray);
			if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
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
					if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
					{
						hr = -1790;
					}
				}
				break;

				case VT_UI4:
				{
					hr = ::SafeArrayPutElement(apSafeArray, &(alpIndicesArray[alDimension]), &vDOMElementData.ulVal);
					if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
					{
						hr = -1791;
					}
				}
				break;

				case VT_BSTR:
				{
					hr = ::SafeArrayPutElement(apSafeArray, &(alpIndicesArray[alDimension]), &vDOMElementData.bstrVal);
					if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
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
		if (hr == S_OK)
		{
			alpIndicesArray[alDimension] = alpIndicesArray [alDimension] + 1;

			SVXML::IXMLDOMElementPtr oDOMNextElementPtr = oDOMChildElementPtr->GetnextSibling();

			oDOMChildElementPtr = oDOMNextElementPtr;
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

	while (oDOMElementPtr != NULL) // This loop actually exits internally.
	{
		if (lDepth >= g_lXMLSafeArrayMaxDimensions)
		{
			hr = -1786;
			break;
		}

		hr = rXml.GetElementNbrOfChildren(oDOMElementPtr, &lNbrOfChildren);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
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

		SVXML::IXMLDOMElementPtr oDOMChildElementPtr = oDOMElementPtr->GetfirstChild();
		if (oDOMChildElementPtr == NULL)
		{
			hr = -1792;
			break;
		}

		hr = rXml.GetElementData(oDOMChildElementPtr, &vElementValue);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
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

	if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
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
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

//-   It is understood that the SAFEARRAY base node violates the separate 
//-	 node/data element rules, and that this "node" is being created with
//-	 the CreateDOMData () function.

		hr = rXml.svmopDOM->CreateDOMNodeElement(bstrElementTag, p_bstrSafeArrayName, &vDummy, oDOMElementPtr);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = SVXMLSafeArrayConverter::CreateDOMChildSafeArrayNodes (rXml, oDOMElementPtr, 1, lIndices, p_InputSafeArray);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}
		break;
	} // while (1)

	if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
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
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			hr = -1735;
			break;
		}

		hr = SafeArrayGetLBound (p_InputSafeArray, p_lDimension, &lLowerBound);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			hr = -1736;
			break;
		}

		hr = SafeArrayGetVartype(p_InputSafeArray, &unArrayType);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
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
			if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
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
						if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
						{
							hr = -1738;
						}
					}
					break;

				case VT_UI4:
					{
						hr = SafeArrayGetElement(p_InputSafeArray, p_plIndices, &vTreeNodeData.ulVal);
						if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
						{
							hr = -1739;
						}
					}
					break;

				case VT_BSTR:
					{
						hr = SafeArrayGetElement(p_InputSafeArray, p_plIndices, &vTreeNodeData.bstrVal);
						if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
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

				if( hr == S_OK )
				{
					hr = rXml.CreateDOMChildData(p_roDOMParentElementPtr, bstrNodeName, &vTreeNodeData, oDOMNewElementPtr);
					if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
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
				if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
				{
					break;
				}

				hr = rXml.svmopDOM->CreateDOMNodeElement(bstrElementTag, bstrNodeName, &vDummy, oDOMNewElementPtr);
				if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
				{
					break;
				}

				hr = SVXMLSafeArrayConverter::CreateDOMChildSafeArrayNodes(rXml, oDOMNewElementPtr, p_lDimension + 1,  p_plIndices, p_InputSafeArray);
				if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
				{
					break;
				}

				hr = rXml.AppendChildToDOMNode(p_roDOMParentElementPtr, oDOMNewElementPtr);
				if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
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
/*
HRESULT SVXMLClass< SVT_TREE >::CopySafeArrayToDOMNode (BSTR p_bstrSafeArrayName, SAFEARRAY* p_InputSafeArray, SVXML::IXMLDOMElementPtr&	p_roDOMElementPtr)
{
	HRESULT	hr;

	SVXML::IXMLDOMElementPtr oDOMElementPtr;

	long		lCounter;

	long		lIndices[ SV_SAFE_ARRAY_DIMENSIONS ];	// This puts a dimension limit on the 
												                  //  SAFEARRAYs that can be handled.

	SVBStr	bstrDimensionName;
	SVBStr	bstrElementTag;

	VARIANT	vDummy;


	hr = 0;
	for (lCounter = 0; lCounter < SV_SAFE_ARRAY_DIMENSIONS; lCounter++)
	{
		lIndices [lCounter] = 0;
	}

	while (1)
	{
		vDummy.vt = VT_ARRAY;
		vDummy.parray = 0;

//-	Working around const.
		hr = bstrElementTag.CopyFromWChar ((WCHAR*) g_wcsNode);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

//-   It is understood that the SAFEARRAY base node violates the separate 
//-	 node/data element rules, and that this "node" is being created with
//-	 the CreateDOMData () function.
//		hr = CreateDOMData (p_bstrSafeArrayName, &vDummy, oDOMElementPtr);

		hr = svmopDOM->CreateDOMNodeElement (bstrElementTag, p_bstrSafeArrayName, &vDummy, oDOMElementPtr);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = CreateDOMChildSafeArrayNodes (oDOMElementPtr,
													  1,		// Dimension
													  lIndices,
													  p_InputSafeArray);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		break;
	} // while (1)


	if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
	{
	}
	else
	{
		p_roDOMElementPtr = oDOMElementPtr;
	}
	return hr;
}

HRESULT SVXMLClass< SVT_TREE >::CreateDOMChildSafeArrayNodes (SVXML::IXMLDOMElementPtr&	p_roDOMParentElementPtr, long p_lDimension, long* p_plIndices, SAFEARRAY* p_InputSafeArray)
{
	USES_CONVERSION;

	HRESULT hr;

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

	hr = 0;
	lTempBuildBufferIndex = 0;

	vTreeNodeData.vt = VT_EMPTY;
	vTreeNodeData.lVal = 0;

	while (1)
	{

		lDimension = SafeArrayGetDim (p_InputSafeArray);
		if (lDimension > SV_SAFE_ARRAY_DIMENSIONS)
		{
			hr = -1788;
			break;
		}

		lInputDimensionMinus1 = p_lDimension - 1;

		hr = SafeArrayGetUBound (p_InputSafeArray, p_lDimension, &lUpperBound);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			hr = -1735;
			break;
		}

		hr = SafeArrayGetLBound (p_InputSafeArray, p_lDimension, &lLowerBound);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			hr = -1736;
			break;
		}

		hr = SafeArrayGetVartype(p_InputSafeArray, &unArrayType);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
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
			if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
			{
				break;
			}

//-		If on the final dimension, we're creating data.  If not, we're still
//-		creating nodes.
			if (p_lDimension == lDimension)
			{
				switch (unArrayType)
				{
				case VT_I4:
					{
						hr = SafeArrayGetElement(p_InputSafeArray, p_plIndices, &vTreeNodeData.lVal);
						if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
						{
							hr = -1738;
							break;
						}

						vTreeNodeData.vt = unArrayType;

						break;
					}
				case VT_UI4:
					{
						hr = SafeArrayGetElement(p_InputSafeArray, p_plIndices, &vTreeNodeData.ulVal);
						if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
						{
							hr = -1739;
							break;
						}

						vTreeNodeData.vt = unArrayType;

						break;
					}
				case VT_BSTR:
					{
						hr = SafeArrayGetElement(p_InputSafeArray, p_plIndices, &vTreeNodeData.bstrVal);
						if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
						{
							hr = -1780;
							break;
						}

						vTreeNodeData.vt = unArrayType;

						break;
					}
				case VT_VARIANT:
					{
						hr = SafeArrayGetElement(p_InputSafeArray, p_plIndices, &vTreeNodeData);
						if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
						{
							hr = -1740;
							break;
						}
						break;
					}
				default:
					{
						hr = -1740;
						break;
					}
					break;
				} // switch (lArrayType)

				hr = CreateDOMChildData (p_roDOMParentElementPtr, bstrNodeName, &vTreeNodeData, oDOMNewElementPtr);
				if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
				{
					break;
				}

			} // if (p_lDimension == lDimension)
			else
			{
				vDummy.vt = (unsigned short) g_lSVArrayNode;
				vDummy.parray = 0;

//-			Working around const.
				hr = bstrElementTag.CopyFromWChar ((WCHAR*) g_wcsNode);
				if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
				{
					break;
				}

				hr = svmopDOM->CreateDOMNodeElement (bstrElementTag, bstrNodeName, &vDummy, oDOMNewElementPtr);
				if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
				{
					break;
				}

				hr = CreateDOMChildSafeArrayNodes (oDOMNewElementPtr, p_lDimension + 1, p_plIndices, p_InputSafeArray);
				if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
				{
					break;
				}

				hr = AppendChildToDOMNode (p_roDOMParentElementPtr, oDOMNewElementPtr);
				if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
				{
					break;
				}
			} // if (p_lDimenion == lDimension) else....
		} // for (lIndex = 0; lIndex < lNumberOfElements; lIndex++)
		break;
	}

	VariantClear (&vTreeNodeData);

	return hr;
}


HRESULT ::CreateTreeChildNodeFromDOMNode( SVXML::IXMLDOMElementPtr aDOMElementPtr, const SVXMLBranchHandle& alTreeParentNodeHandle )
{
	HRESULT	l_Status( S_OK );

	if( aDOMElementPtr == NULL )
	{
		//-		No node.  Leave peacefully.
		return 2;
	}

	if( svmlInitialized != 0 )
	{
		if (svmlAttached == 0)
		{
			l_Status = -1766;
		}
	}
	else
	{
		l_Status = -1765;
	}

	if( l_Status == S_OK )
	{
		VARIANT vDOMElementData;

		BSTR bstrDOMElementName( NULL );

		::VariantInit( &vDOMElementData );

		//-   GetElementName () is executed first for debugging, so that the name of
		//-   the items being manipulated can be seen.
		l_Status = GetElementName( aDOMElementPtr, &bstrDOMElementName );

		if( SV_SUCCEEDED( l_Status ) )
		{
			//-   For NODE elements with type (no data), vDOMElementData will return with
			//-	the type set and the value set to zero (0). 
			//-   For Node elements without type, vDOMElementData will return with type
			//-	set to VT_NULL, and the value set to zero (0).
			l_Status = GetElementData( aDOMElementPtr, &vDOMElementData );

			assert( SV_SUCCEEDED( l_Status ) );
		}

		if( SV_SUCCEEDED( l_Status ) )
		{
			BSTR bstrDOMElementTag( NULL );

			l_Status = IsElementData( aDOMElementPtr, &bstrDOMElementTag );

			if( SV_SUCCEEDED( l_Status ) )
			{
				if( l_Status == S_OK )
				{
					l_Status = svmpAttachedTree->CreateLeaf( alTreeParentNodeHandle, bstrDOMElementName, vDOMElementData );
				}
				else
				{
					if( ( vDOMElementData.vt & VT_ARRAY ) == VT_ARRAY && vDOMElementData.vt != VT_ARRAY )
					{
						l_Status = svmpAttachedTree->CreateLeaf( alTreeParentNodeHandle, bstrDOMElementName, vDOMElementData );
					}
					else
					{
						SVXML::IXMLDOMElementPtr oDOMChildElementPtr;

						SVXMLBranchHandle l_Branch;

						if( vDOMElementData.vt == g_lSVBaseNode )
						{
							l_Status = svmpAttachedTree->GetRoot( l_Branch );
						}
						else
						{
							l_Status = svmpAttachedTree->CreateBranch( alTreeParentNodeHandle, bstrDOMElementName, &l_Branch );
						}

						oDOMChildElementPtr = aDOMElementPtr->GetfirstChild ();

						while( SV_SUCCEEDED( l_Status ) && oDOMChildElementPtr != NULL )
						{
							l_Status = CreateTreeChildNodeFromDOMNode( oDOMChildElementPtr, l_Branch );

							if( SV_SUCCEEDED( l_Status ) )
							{
								SVXML::IXMLDOMElementPtr oDOMNextChildElementPtr;

								oDOMNextChildElementPtr = oDOMChildElementPtr->GetnextSibling ();

								oDOMChildElementPtr = oDOMNextChildElementPtr;
							}
						} // while (oDOMChildElementPtr != NULL)
					}
				}
			}

			::SysFreeString( bstrDOMElementTag );
			bstrDOMElementTag = NULL;
		}

		::VariantClear( &vDOMElementData );

		::SysFreeString( bstrDOMElementName );
		bstrDOMElementName = NULL;
	}
	return l_Status;
}
*/
//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVXMLLibrary\SVXMLSafeArrayConverter.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:56:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Oct 2012 10:40:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code and projects to fix whitespace issues in the XML file and consolidate functionality for load and saving to this library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/