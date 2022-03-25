//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVXMLClass
//* .File Name       : $Workfile:   SVXMLClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   19 Dec 2014 04:44:28  $
//******************************************************************************

#include "stdafx.h"
#import <msxml6.dll> // MSXML
//Moved to precompiled header: #include <assert.h>
#include "SVXMLClass.h"
#include "SVLibrary/SVBStr.h"

#include "SVXMLEncryptionClass.h"
#include "SVXMLSVRSchemaClass.h"
#include "SVMessage\SVMessage.h"


SVXMLClass::SVXMLClass()
: svmlUseRevisionHistoryInfo(0)
, svmlUseCheckSums(0)
, m_lUseEncryption(0)
, svmopDOM(nullptr)
, svmlInitialized(0)
, m_lIsEncrypted(0)
, m_lCurrentVersion(0)
, m_lCurrentRevision(0)
, m_opEncryption(nullptr)
{
	Init ();
}

SVXMLClass::~SVXMLClass()
{
	svmlInitialized = FALSE;

	if (svmopDOM)
	{
		delete svmopDOM;
		svmopDOM = nullptr;
	}
}

HRESULT SVXMLClass::Init()
{
	long lErr = 0;

	svmlInitialized = false;
	m_lIsEncrypted = false;

	svmopDOM = nullptr;
	m_opEncryption = nullptr;
	m_wczRevisionHistory [0] = 0;
	m_lCurrentVersion = 0;
	m_lCurrentRevision = 0;

	return lErr;
}

// Initialize - must be called after instantiation, and before use.
HRESULT	SVXMLClass::Initialize (long p_lUseCheckSums, long p_lUseRevisionHistoryInfo, long p_lUseEncryption)
{
	HRESULT hr = 0;

	while (1)
	{
		if (svmlInitialized == TRUE)
		{
//-		Not treated as a real error.  No action taken because it is already 
//-		initialized.
			hr = 1612;
			break;
		}

		if (nullptr != svmopDOM)
		{
			hr = -1613;
			break;
		}

		svmopDOM = new SVDOMClass;
		if (nullptr == svmopDOM)
		{
			hr = -1611;
			break;
		}

		hr = svmopDOM->Initialize (p_lUseEncryption);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		hr = svmopDOM->GetEncryptionInterface (&m_opEncryption);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		hr = m_opEncryption->InitializeXMLInterface (this);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		svmlUseCheckSums =				p_lUseCheckSums;
		svmlUseRevisionHistoryInfo =	p_lUseRevisionHistoryInfo;
		m_lUseEncryption =				p_lUseEncryption;
		m_lIsEncrypted =				FALSE;
		m_wczRevisionHistory [0] =		_T('\0');
		m_lCurrentVersion = 0;
		m_lCurrentRevision = 0;

		break;
	}

	if( SEV_SUCCESS != SV_SEVERITY( hr ) )
	{
//-	There was error, initialization is incomplete.
	}
	else
	{
		svmlInitialized = TRUE;
	}
	return hr;
}

HRESULT SVXMLClass::CopyDOMToXMLFile (BSTR abstrFileName)
{
	HRESULT	hr = 0;

	while (1)
	{
		if (svmlInitialized == 0)
		{
			hr = -1638;
			break;
		}

		hr = svmopDOM->CopyDOMToXMLFile (abstrFileName);

		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}
		break;
	}
	return hr;
}


HRESULT SVXMLClass::CreateDOMNode (SVXML::IXMLDOMElementPtr& arDOMNewElementPtr, BSTR abstrElementName, long alType)
{
	HRESULT hr = 0;

	SVXML::IXMLDOMElementPtr oNewElementPtr;  

	SVBStr bstrElementTagName;

	VARIANT vElementData;
	vElementData.vt = VT_EMPTY;
	vElementData.lVal = 0;

	while (1)
	{
//-	Type cast is to convert from const.
		hr = bstrElementTagName.CopyFromWChar ((WCHAR*) g_wcsNode); 
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		vElementData.vt = (unsigned short) alType;

		hr = svmopDOM->CreateDOMNodeElement (bstrElementTagName, abstrElementName, &vElementData, oNewElementPtr);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}
		break;
	}

	if( SEV_SUCCESS != SV_SEVERITY( hr ) )
	{
	}
	else
	{
		arDOMNewElementPtr = oNewElementPtr;
	}
	return hr;
}

HRESULT SVXMLClass::CreateDOMData (SVXML::IXMLDOMElementPtr& arDOMNewElementPtr, BSTR abstrElementName, VARIANT* avpElementData)
{
	HRESULT hr = 0;
	SVXML::IXMLDOMElementPtr oNewElementPtr;
	SVBStr bstrElementTagName;

	while (1)
	{
		hr = bstrElementTagName.CopyFromWChar (g_wcsData);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		hr = svmopDOM->CreateDOMNodeElement (bstrElementTagName, abstrElementName, avpElementData, oNewElementPtr);
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
		arDOMNewElementPtr = oNewElementPtr;
	}
	return hr;
}

HRESULT SVXMLClass::CreateDOMChildData (SVXML::IXMLDOMNodePtr aDOMParentNodePtr, BSTR abstrElementName, VARIANT* avpElementData, SVXML::IXMLDOMElementPtr& arDOMChildDataPtr)
{
	HRESULT hr = 0;
	SVXML::IXMLDOMNodePtr tempChildDataPtr;
	SVBStr bstrElementTagName;

	while (1)
	{
		hr = bstrElementTagName.CopyFromWChar (g_wcsData);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		hr = svmopDOM->CreateDOMNodeChildElement (aDOMParentNodePtr, bstrElementTagName, abstrElementName, avpElementData, tempChildDataPtr);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		arDOMChildDataPtr = tempChildDataPtr;
		break;
	}
	return hr;
}

HRESULT SVXMLClass::SetRevisionHistory (BSTR abstrRevisionHistory)
{
	HRESULT	hr = 0;
	SVXML::IXMLDOMElementPtr oDestinationElementPtr;
	SVXML::IXMLDOMElementPtr oDestinationRootElementPtr;
	SVXML::IXMLDOMElementPtr oSourceRootElementPtr;

	SVXML::IXMLDOMNodePtr oNodePtr1;
	SVXML::IXMLDOMNodePtr oNodePtr2;
	SVXML::IXMLDOMNodeListPtr oNodeListPtr;

	SVBStr bstrRevisionHistoryBranchName;

	SVDOMClass oSourceDOM;

	while (1)
	{
		if (svmlInitialized == 0)
		{
			hr = -1666;
			break;
		}

		if (nullptr == abstrRevisionHistory)
		{
//-		If the bstr was not allocated, then there is no revision history 
//-		available and a past revision history will not be entered.  This
//-		is an acceptable scenario.
			break;
		}

//-	Check string size.
		if ((((char *)abstrRevisionHistory) - 2) == 0)
		{
//-		If string size equals zero, then there is no revision history 
//-		string.
			break;
		}

		oSourceDOM.Initialize (0);	// Do not use encryption.

//-	Load XML bstr into temp DOM. 
		hr = oSourceDOM.CopyXMLTextToDOM (abstrRevisionHistory);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
//		if (oTempDOM.svmDOMPtr->load (abstrRevisionHistory) != TRUE)
		{
			hr = -1625;
			break;
		};

//-	Get child node list from main document.  Remove all items from the 
//-	list.  At this time I'm assuming that removing an item removes all 
//-	children of that item.
//-
//-   Ok, here's the problem.  There can only be one IXMLDOMElement in the
//-	DOM document's list; therefore the abstrRevisionHistory string must 
//-	contain a root element that the individual revision histories are part 
//-	of.

		hr = oSourceDOM.GetRootNode (oSourceRootElementPtr);

		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		if (nullptr == oSourceRootElementPtr)
		{
			hr = -1691;
			break;
		}

		oNodeListPtr = oSourceRootElementPtr->GetchildNodes ();
		if (nullptr == oNodeListPtr)
		{
			break;
		}

		bstrRevisionHistoryBranchName = "RevisionHistory";

		hr = CreateElement (bstrRevisionHistoryBranchName, 
			  					  oDestinationElementPtr);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

//-   Hmmmmmm, this list should contain all revision history elements.
		do
		{
			oNodeListPtr->reset ();

			oNodePtr1 = oNodeListPtr->nextNode ();
			if (nullptr == oNodePtr1)
			{
//-			It is assumed that we are at the end of the list.				
				break;
			}

			oNodePtr2 = oDestinationElementPtr->appendChild (oNodePtr1);
			if (nullptr == oNodePtr2)
			{
				hr = -1668;
				break;
			}

			oNodePtr2 = oSourceRootElementPtr->removeChild (oNodePtr1);
			if (nullptr == oNodePtr2)
			{
				break;
			}
		} while (1); // relies on nullptr == nodePtr to leave the loop

		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

//-	If a revision history exists off the root, this function should 
//-	delete it.  If a revision history does not exist off the root, then 
//-	this function should return a 1.		
		hr = DeleteRevisionHistory ();
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		hr = GetRootNode (oDestinationRootElementPtr);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		if (nullptr == oDestinationRootElementPtr)
		{
			hr = -1685;
			break;
		}

		hr = AppendChildToDOMNode (oDestinationRootElementPtr, oDestinationElementPtr);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}
		break;
	} // while (1)
	return hr;
}

// AddToRevisionHistory () --------------------------------------------------
// First we check to see if the RevisionHistory node exists.  If doesn't 
// exist, it's ok, we'll make one.  Then we create our new Revision element.  
// We then try to find the first existing Revision element.  If there is no 
// existing revision element, that's fine.  We then place our new revision 
// element at the top of the list. ------------------------------------------
HRESULT SVXMLClass::AddToRevisionHistory (long	alSVOCurrentVersion, BSTR abstrFormat)
{
	HRESULT hr = 0;
	SVXML::IXMLDOMNodePtr oFirstRevNodePtr;
	SVXML::IXMLDOMNodePtr oOutputNodePtr;

	SVXML::IXMLDOMElementPtr oNewRevElementPtr;
	SVXML::IXMLDOMElementPtr oRootElementPtr;
	SVXML::IXMLDOMElementPtr oRevHistoryBaseElementPtr;

	SVXML::IXMLDOMNodeListPtr oDOMNodeListPtr;

	_bstr_t bstrAttributeName;
	_bstr_t bstrElementTagName;

	_bstr_t bstrSelectName;

	CComVariant ccvAttributeValue;
	CComVariant ccvInsertBefore;

	char szBuffer [512];

	while (1)
	{
		if (svmlInitialized == 0)
		{
			hr = -1686;
			break;
		}

//		hr = svmopDOM->GetRootNode (oRootElementPtr);
		hr = GetRevisionHistoryBaseNode (oRevHistoryBaseElementPtr);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		if (nullptr == oRevHistoryBaseElementPtr)
		{
			hr = AddRevisionHistoryBaseNode ();
			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				break;
			}

//			hr = svmopDOM->GetRootNode (oRootElementPtr);
			hr = GetRevisionHistoryBaseNode (oRevHistoryBaseElementPtr);
			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				break;
			}

			if (nullptr == oRevHistoryBaseElementPtr)
			{
				hr = -1675;
				break;
			}
		} // if (nullptr == oRevHistoryBaseNodePtr)

		bstrElementTagName = g_wcsRevision;

		hr = CreateElement (bstrElementTagName, oNewRevElementPtr);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		bstrAttributeName = g_wcsFormat;
		ccvAttributeValue = abstrFormat;

		oNewRevElementPtr->setAttribute (bstrAttributeName, ccvAttributeValue);
		ccvAttributeValue.Clear ();

//-	Element Names (an attribute is a type of element) may NOT contain 
//-	spaces. ":" is used only to reference name spaces.  "Element names can
//-	start with a letter, underscore, or colon, but cannot start with other 
//-	non-alphabetic characters or a number, or the letters xml."
		bstrAttributeName = g_wcsFormatVersion;
		sprintf (szBuffer,
		 		   "%d.%d", 
					alSVOCurrentVersion >> 16, 
					(alSVOCurrentVersion >> 8) & 0x000000ff);
		ccvAttributeValue = szBuffer;

		oNewRevElementPtr->setAttribute (bstrAttributeName, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		bstrAttributeName = g_wcsRevisionAtt;

		oDOMNodeListPtr = oRevHistoryBaseElementPtr->GetchildNodes ();
		if (nullptr == oDOMNodeListPtr)
		{
			hr = -1667;
			break;
		}

		hr = oDOMNodeListPtr->reset ();
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			hr = -1669;
			break;
		}

		oFirstRevNodePtr = oDOMNodeListPtr->nextNode ();
		ccvAttributeValue = sprintf (szBuffer, "%d", oDOMNodeListPtr->Getlength () + 1);

		oNewRevElementPtr->setAttribute (bstrAttributeName, ccvAttributeValue);

		ccvInsertBefore = (SVXML::IXMLDOMNode *) oFirstRevNodePtr;

		oOutputNodePtr = oRevHistoryBaseElementPtr->insertBefore (oNewRevElementPtr, ccvInsertBefore);
		if (nullptr == oOutputNodePtr)
		{
			hr = -1672;
			break;
		}
		break;
	}
	return hr;
}

HRESULT SVXMLClass::GetRevisionHistory(BSTR* abstrpRevisionHistory)
{
	HRESULT	hr = S_OK;

	while (1)
	{
		if (nullptr != *abstrpRevisionHistory)
		{
			hr = -1752;
			break;
		}

		*abstrpRevisionHistory = SysAllocString (m_wczRevisionHistory);

		break;
	}
	return hr;
}

HRESULT SVXMLClass::GetCurrentVersion (long* p_lpCurrentVersion)
{
	HRESULT hr = S_OK;

	while (1)
	{
		*p_lpCurrentVersion = m_lCurrentVersion;

		break;
	}
	return hr;
}



HRESULT SVXMLClass::AddRevisionHistoryBaseNode ()
{
	HRESULT hr = S_OK;

	SVXML::IXMLDOMElementPtr oNewRevHistBaseElementPtr;
	SVXML::IXMLDOMElementPtr oRootElementPtr;

	_bstr_t bstrRevisionHistoryBaseName;

	while (1)
	{
		hr = GetRootNode (oRootElementPtr);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		if (nullptr == oRootElementPtr)
		{
			hr = -1696;
			break;
		}

		bstrRevisionHistoryBaseName = g_csRevisionHistory;

		hr = CreateElement (bstrRevisionHistoryBaseName, oNewRevHistBaseElementPtr);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		hr = AppendChildToDOMNode (oRootElementPtr, oNewRevHistBaseElementPtr);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}
		break;
	}
	return hr;
}

HRESULT SVXMLClass::GetRevisionHistoryBaseNode (SVXML::IXMLDOMElementPtr& arRevHistoryBaseNodePtr)
{
	HRESULT hr = S_OK;

	SVXML::IXMLDOMElementPtr oRootElementPtr;

	WCHAR wcsNameSpace [128];
	_bstr_t bstrSelectName;
	SVBStr bstrQueryNameSpace;

	while (1)
	{
		hr = GetRootNode (oRootElementPtr);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		if (nullptr == oRootElementPtr)
		{
			hr = -1687;
			break;
		}

		hr = GetNameSpace (wcsNameSpace, sizeof (wcsNameSpace));
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

//-	xmlns:SVR1="x-schema:#SVR00001"
		bstrQueryNameSpace = (BSTR)((_bstr_t) g_csXMLNS + ":SVR1=\"x-schema:#" + (_bstr_t) g_csSVR00001SchemaName + "\"");

		hr = SetQueryNameSpace (bstrQueryNameSpace);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

//-	/Root/SVR1:RevisionHistory  -  XPath string
		bstrSelectName = (_bstr_t) L"/" + g_wcsRoot + L"/SVR1:" + (_bstr_t) g_csRevisionHistory;

//-	A FEW WORDS ABOUT selectSingleNode (). --------------------------------
//-	We use the XPath standard to define our search.   This requires that 
//-	we call setProperty ("SelectionLanguage","XPath"), prior to calling 
//-	selectSingleNode ().  This is now getting done within 
//-	SVDOMClass::InitializeDOM () which is accessed from SVXML by calling
//-	SVDOMClass::Initialize (), which happens in SVXML::Initialize ().  
//-	There is no reason for this value to be set or changed to anything 
//-	other than XPath.
//-
//-   We also must register all namespaces used in the DOM, separately for 
//-   the XPath query functions.  This is done with a call to 
//-	setProperty("SelectionNamespaces", <ccvProperty>) which is currently 
//-   being done imediately prior to the query.
		arRevHistoryBaseNodePtr = oRootElementPtr->selectSingleNode (bstrSelectName);

		if (nullptr == arRevHistoryBaseNodePtr)
		{
//-		No revision history base node found.
			hr = 2;
		}
		break;
	} // while (1)
	return hr;
}

HRESULT SVXMLClass::GetRevisionNodeByIndex (long p_lIndex, SVXML::IXMLDOMElementPtr& p_orRevNodePtr)
{
	HRESULT hr = S_OK;

	long l_lNbrOfChildren;
	long l_lCount;
	long l_lIndex;
	long l_lIndexFound;

	_bstr_t l_bstrTagName;
	_bstr_t l_bstrFormatVersionAttribute;
	_bstr_t l_bstrRevisionAttribute;

	_variant_t l_vFormatVersionValue;

	SVXML::IXMLDOMElementPtr l_oRevHistoryBaseNodePtr;
	SVXML::IXMLDOMElementPtr l_oRevNodePtr;

	while (1)
	{
		if (svmlInitialized == 0)
		{
			hr = -1760;
			break;
		}

		hr = GetRevisionHistoryBaseNode (l_oRevHistoryBaseNodePtr);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}
		else
		if (hr == 2)
		{
//-		Revision History Base node not found.
			break;
		}

		if (nullptr == l_oRevHistoryBaseNodePtr)
		{
			hr = -1922;
			break;
		} // if (nullptr == oRevHistoryBaseNodePtr)


		hr = svmopDOM->GetElementNbrOfChildren (l_oRevHistoryBaseNodePtr, 
			                                     &l_lNbrOfChildren);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

//-	l_lCount counts all children of the Revision History Base Node.
//-	l_lIndex will only count children with the tag "Revision".
		l_lCount      = 0;
		l_lIndex		  = 0;
		l_lIndexFound = FALSE;

		while ((l_lCount < l_lNbrOfChildren) && (l_lIndexFound == FALSE))
		{
			if (l_lCount == 0)
			{
				l_oRevNodePtr = GetFirstElementChild(l_oRevHistoryBaseNodePtr);
			}
			else
			{
				l_oRevNodePtr = GetNextElementSibling(l_oRevNodePtr);
			}

			if (nullptr == l_oRevNodePtr)
			{
				hr = -1917;
				break;
			}

			l_bstrTagName = l_oRevNodePtr->GettagName ();

			if (wcscmp (l_bstrTagName, g_wcsRevision) == 0)
			{
//-				They are equal.
				l_lIndex = l_lIndex + 1;
				if (l_lIndex == p_lIndex)
				{
					l_lIndexFound = TRUE;
				}
			}

			l_lCount = l_lCount + 1;
		} // while (l_lCount < l_lNbrOfChildren)

		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		if (l_lIndexFound == FALSE)
		{
			hr = 3;
			break;
		}

		p_orRevNodePtr = l_oRevNodePtr;

		break;
	} // while (1)
	return hr;
}

HRESULT SVXMLClass::DeleteRevisionHistory ()
{
	HRESULT hr = S_OK;
	SVXML::IXMLDOMElementPtr oRevHistoryBaseElementPtr;
	SVXML::IXMLDOMElementPtr oRootElementPtr;

	SVXML::IXMLDOMNodePtr oTempNodePtr;

	while (1)
	{
		hr = GetRevisionHistoryBaseNode (oRevHistoryBaseElementPtr);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		if (nullptr == oRevHistoryBaseElementPtr)
		{
//-		No element found, no element to delete.  We consider this to be a 
//-		successful path to the end goal.
			break;
		}

		hr = GetRootNode (oRootElementPtr);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		if (nullptr == oRootElementPtr)
		{
			hr = -1689;
			break;
		}
		
		oTempNodePtr = oRootElementPtr->removeChild (oRevHistoryBaseElementPtr);
		if (nullptr == oTempNodePtr)
		{
			hr = -1693;
			break;
		}
		break;
	} // while (1)
	return hr;
}

HRESULT SVXMLClass::GetEncryptionBaseNode (SVXML::IXMLDOMElementPtr& p_orEncryptionBaseNodePtr)
{
	HRESULT	hr = S_OK;

	while (1)
	{
		hr = m_opEncryption->GetEncryptionBaseNode (p_orEncryptionBaseNodePtr);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}
		break;
	}
	return hr;
}

HRESULT SVXMLClass::CalculateNodeCheckSums (SVXML::IXMLDOMNodePtr	aDOMNodePtr, unsigned long* aulpCheckSum, long alMode, BSTR* bstrpChangedNode)
{
	HRESULT hr = S_OK;

	SVXML::IXMLDOMNodePtr oNodePtr;
	SVXML::IXMLDOMNodePtr oAttributePtr;

	SVXML::IXMLDOMNamedNodeMapPtr oAttributeListPtr;

	SVXML::DOMNodeType oNodeType;

	unsigned long ulRawCheckSum;
	unsigned long ulTempCheckSum;

	long lNbrOfAttributes;
	long lCounter;

	_bstr_t bstrCheckSumTitle;
	_bstr_t bstrNameTitle;
	_bstr_t bstrDataNodeTitle;

	_bstr_t bstrNodeName;
	_bstr_t bstrAttributeName;

	CComVariant ccvNodeValue;
	CComVariant ccvAttributeValue;

	ulRawCheckSum = 0;

	while (1)
	{
		if (nullptr == aDOMNodePtr)
		{
//-		Return harmlessly.
			break;
		}

		if ((alMode == g_lXMLCheckSumCompare) &&
			 (nullptr == bstrpChangedNode))
		{
//-		If we are doing a checksum compare, then the BSTR pointer must 
//-		exist for returning the error path.
			hr = -1735;
			break;
		}

		oNodeType = aDOMNodePtr->GetnodeType ();
		if (oNodeType != SVXML::NODE_ELEMENT)
		{
//-		If this is not an element node, then we will leave peacefully 
//-		without attempting to calculate a checksum (checksum will be 
//-		zero).
			hr = 3;
			break;
		}

//-	Checksum name.
		bstrNodeName = aDOMNodePtr->GetnodeName ();

		hr = CalculateStringCheckSum (bstrNodeName, &ulTempCheckSum);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		ulRawCheckSum = ulRawCheckSum + ulTempCheckSum;

//-	Checksum value.
//-	Although ccvNodeValue is a variant, it should always contain a bstr.
		ccvNodeValue = aDOMNodePtr->GetnodeValue ();

		if ((VT_BSTR != ccvNodeValue.vt) || (nullptr == ccvNodeValue.bstrVal))
		{
//-		For now we do not expect nodes to contain values (only attributes
//-		contain values.  However, if there is a value, we will try to
//-		checksum it.
		}
		else
		{
			hr = CalculateStringCheckSum (ccvNodeValue.bstrVal, &ulTempCheckSum);
			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				break;
			}

			ulRawCheckSum = ulRawCheckSum + ulTempCheckSum;
		}

//-	Checksum attributes.
		oAttributeListPtr = aDOMNodePtr->attributes;

		if (nullptr == oAttributeListPtr)
		{
			hr = -1681;
			break;
		}

		bstrDataNodeTitle =	g_wcsData;
		bstrCheckSumTitle =	g_csCheckSum;
		bstrNameTitle =		g_wcsName;

		lNbrOfAttributes = oAttributeListPtr->Getlength ();
		for (lCounter = 0; lCounter < lNbrOfAttributes; lCounter++)
		{
			oAttributePtr = oAttributeListPtr->Getitem (lCounter);
			if (nullptr == oAttributePtr)
			{
				hr = -1682;
				break;
			}

			bstrAttributeName = oAttributePtr->GetnodeName ();

			hr = CalculateStringCheckSum (bstrAttributeName, &ulTempCheckSum);
			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				break;
			}

			ulRawCheckSum = ulRawCheckSum + (ulTempCheckSum << 1);

//-		Although ccvAttributeValue is a variant, it should always contain a
//-		bstr.
			ccvAttributeValue = oAttributePtr->GetnodeValue ();

			if ((VT_BSTR != ccvAttributeValue.vt) || (nullptr == ccvAttributeValue.bstrVal))
			{
				hr = -1683;
				break;
			}

			if ((wcscmp (bstrNodeName, bstrDataNodeTitle) == 0) &&
				 (wcscmp (bstrAttributeName, bstrNameTitle) == 0) &&
				 (wcscmp (ccvAttributeValue.bstrVal, bstrCheckSumTitle)))
			{
//-			If the node name is "DATA", and attribute name is "Name", and the 
//-			attribute value is "CheckSum", then we will assume that this node 
//-			is the check sum node.  This node and all of its sub-nodes will 
//-			be ignored. This is so that the presence or absence of the 
//-			check sum field does not affect the check sum result.
				ulRawCheckSum = 0;
				hr = 2;
				break;
			}

			hr = CalculateStringCheckSum (ccvAttributeValue.bstrVal, &ulTempCheckSum);
			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				break;
			}

			ulRawCheckSum = ulRawCheckSum + (ulTempCheckSum << 1);
		} // for (lCounter = 0; lCounter < lNbrOfAttributes; lCounter++)

		if (hr == 2)
		{
//-		This is the check sum node and should not be included in any checksum data.
			break;
		}
		
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

//-	Check child nodes.

		oNodePtr = aDOMNodePtr->GetfirstChild ();

//-	It was initially assumed that the GetfirstChild () function could 
//-	return attribute nodes.  This is apparently incorrect; attribute node 
//-	will not be returned by this function (at least not from an element 
//-	node).

		while (nullptr != oNodePtr)
		{
//-		If this node is an element (which will usually be the case), then 
//-		checksum the sub nodes.
			if (oNodeType == SVXML::NODE_ELEMENT)
			{
//				hr = CreateNodeCheckSums (oNodePtr, &ulTempCheckSum);
				hr = CalculateNodeCheckSums (oNodePtr, &ulTempCheckSum, alMode, bstrpChangedNode);
				if( SEV_SUCCESS != SV_SEVERITY( hr ) )
				{
					break;
				}
				
				ulRawCheckSum = ulRawCheckSum + (ulTempCheckSum << 16);
			}
			else
			{
//-			Not an element node.	
			}

//-		Get next node.
			oNodePtr = oNodePtr->GetnextSibling ();
		}// while (nullptr != oNodePtr)

		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		if (alMode & g_lXMLCheckSumCreate)
		{
			hr = UpdateCheckSumValue (aDOMNodePtr,
											  ulRawCheckSum);
			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				break;
			}
		}

		if (alMode & g_lXMLCheckSumCompare)
		{
			hr = CompareCheckSumValue (aDOMNodePtr,
												ulRawCheckSum,
												bstrpChangedNode);
			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				break;
			}
			else
			if (hr == 2)	// CheckSum compare failed.
			{
				hr = 4;
				break;
			}
		}

		break;
	} // while (1)

	if( SEV_SUCCESS != SV_SEVERITY( hr ) )
	{
	}
	else
	{
//-	If checksum failed, this value may be incomplete and hr will be equal 
//-	to 4.
		*aulpCheckSum = ulRawCheckSum;
	}
	return hr;
}

HRESULT SVXMLClass::CalculateStringCheckSum (BSTR abstrSource, unsigned long* aulpCheckSum)
{
	HRESULT	hr = S_OK;

	long lLength;
	long lLoop;

	WCHAR* wcpChar;

	unsigned long ulRawCheckSum = 0;
	
	while (1)
	{
		lLength = SysStringLen (abstrSource);
		wcpChar = abstrSource;

		for (lLoop = 0; lLoop < lLength; lLoop++)
		{
			ulRawCheckSum = ulRawCheckSum + ((long) (wcpChar [lLoop] << (lLoop % 3)));
		}
		break;
	}// while (1)

	if( SEV_SUCCESS != SV_SEVERITY( hr ) )
	{
		aulpCheckSum = 0;
	}
	else
	{
		*aulpCheckSum = ulRawCheckSum;
	}
	return hr;
}

HRESULT SVXMLClass::UpdateCheckSumValue (SVXML::IXMLDOMNodePtr aDOMNodePtr, unsigned long aulCheckSum)
{
	HRESULT hr = S_OK;

	SVXML::IXMLDOMElementPtr oElementPtr;

//	SVXML::IXMLDOMNodePtr oCheckSumNodePtr;
	SVXML::IXMLDOMElementPtr oCheckSumElementPtr;

	WCHAR wcsNameSpace [128];

	_bstr_t bstrSelectName;
	_bstr_t bstrElementName;
	_bstr_t bstrAttributeName;

	SVBStr bstrQueryNameSpace;

	CComVariant ccvElementValue;

	while (1)
	{
//-	Shmucks don't have an equals operator for unsigned long.
//		ccvElementValue = aulCheckSum;  
		ccvElementValue.vt = VT_UI4;
		ccvElementValue.ulVal = aulCheckSum;

		hr = GetNameSpace (wcsNameSpace, sizeof (wcsNameSpace));
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

//-	XMLNS:SVR1="x-schema:#SVR00001"
		bstrQueryNameSpace = (BSTR)((_bstr_t) g_csXMLNS + 
			                         ":SVR1=\"" + 
											 (_bstr_t) g_csSVR00001SchemaName + 
											 "\"");

		hr = SetQueryNameSpace (bstrQueryNameSpace);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

//-	SVR1:DATA [@Name = "CheckSum"]  -  XPath string
//		bstrSelectName = L"SVR1:"+ (_bstr_t) g_wcsData + L"[@" + g_csName + L" = \"" + g_csCheckSum + L"\"]";

		bstrSelectName = (_bstr_t) L"SVR1:"+ g_wcsData + L"[@" + g_wcsName + L" = \"" + g_csCheckSum + L"\"]";
//-	A FEW WORDS ABOUT selectSingleNode (). --------------------------------
//-	We use the XPath standard to define our search.   This requires that 
//-	we call setProperty ("SelectionLanguage","XPath"), prior to calling 
//-	selectSingleNode ().  This is now getting done within 
//-	SVDOMClass::InitializeDOM () which is accessed from SVXML by calling
//-	SVDOMClass::Initialize (), which happens in SVXML::Initialize ().  
//-	There is no reason for this value to be set or changed to anything 
//-	other than XPath.
//-
//-   We also must register all namespaces used in the DOM, separately for 
//-   the XPath query functions.  This is done with a call to 
//-	setProperty("SelectionNamespaces", <ccvProperty>) which is currently 
//-   being done imediately prior to the query.
		oCheckSumElementPtr = aDOMNodePtr->selectSingleNode (bstrSelectName);
		if (nullptr == oCheckSumElementPtr)
		{
			bstrElementName = g_csCheckSum;

			hr = CreateDOMChildData (aDOMNodePtr,
											 bstrElementName,
											 &ccvElementValue,
											 oCheckSumElementPtr);
												
			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				break;
			}

			if (nullptr == aDOMNodePtr)
			{
				hr = -1678;
				break;
			}
		}
		else
		{
			bstrAttributeName = g_csValue;

			hr = oCheckSumElementPtr->setAttribute (bstrAttributeName, ccvElementValue);
			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				hr = -1679;
				break;
			}
		}
		break;
	} // while (1)
	return hr;
}

HRESULT	SVXMLClass::CompareCheckSumValue (SVXML::IXMLDOMNodePtr	aDOMNodePtr, unsigned long aulCheckSum, BSTR* abstrpChangedNode)
{
	HRESULT hr = 0;

	SVXML::IXMLDOMElementPtr oElementPtr;

//	SVXML::IXMLDOMNodePtr oCheckSumNodePtr;
	SVXML::IXMLDOMElementPtr oCheckSumElementPtr;

	WCHAR wcsNameSpace [128];

	_bstr_t bstrSelectName;
	_bstr_t bstrElementName;
	_bstr_t bstrAttributeName;

	SVBStr bstrQueryNameSpace;

	CComVariant ccvElementValue;

	while (1)
	{
		if (nullptr == abstrpChangedNode)
		{
			hr = -1736;
			break;
		}

//-	Shmucks don't have an equals operator for unsigned long.
//		ccvElementValue = aulCheckSum;  
		ccvElementValue.vt = VT_UI4;
		ccvElementValue.ulVal = aulCheckSum;

		hr = GetNameSpace (wcsNameSpace, sizeof (wcsNameSpace));
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

//-	XMLNS:SVR1="x-schema:#SVR00001"
		bstrQueryNameSpace = (BSTR)((_bstr_t) g_csXMLNS + 
			                         ":SVR1=\"" + 
											 (_bstr_t) g_csSVR00001SchemaName + 
											 "\"");

		hr = SetQueryNameSpace (bstrQueryNameSpace);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

//-	SVR1:DATA [@Name = "CheckSum"]  -  XPath string
		bstrSelectName = (_bstr_t) L"SVR1:"+ g_wcsData + L"[@" + g_wcsName + L" = \"" + g_csCheckSum + L"\"]";

//-	A FEW WORDS ABOUT selectSingleNode (). --------------------------------
//-	We use the XPath standard to define our search.   This requires that 
//-	we call setProperty ("SelectionLanguage","XPath"), prior to calling 
//-	selectSingleNode ().  This is now getting done within 
//-	SVDOMClass::InitializeDOM () which is accessed from SVXML by calling
//-	SVDOMClass::Initialize (), which happens in SVXML::Initialize ().  
//-	There is no reason for this value to be set or changed to anything 
//-	other than XPath.
//-
//-   We also must register all namespaces used in the DOM, separately for 
//-   the XPath query functions.  This is done with a call to 
//-	setProperty("SelectionNamespaces", <ccvProperty>) which is currently 
//-   being done imediately prior to the query.
		bstrSelectName = (_bstr_t) g_wcsData + L"[@" + g_wcsName + L" = \"" + g_csCheckSum + L"\"]";

		oCheckSumElementPtr = aDOMNodePtr->selectSingleNode (bstrSelectName);
		if (nullptr == oCheckSumElementPtr)
		{
//-		Checksum not found will be treated the same as checksum not matched.
			hr = 2;
		}
		else
		{
//-		Checksum found.
			bstrAttributeName = g_csValue;

			hr = oCheckSumElementPtr->setAttribute (bstrAttributeName, ccvElementValue);
			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				hr = -1679;
				break;
			}

		} // if (nullptr == oCheckSumElementPtr) ... else

		break;
	} // while (1)

	if (hr != 0)
	{
		hr = GetNamePath (aDOMNodePtr, abstrpChangedNode);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
//-	  Error within GetNamePath ()

		}
	}
	return hr;
}

HRESULT SVXMLClass::GetNamePath (SVXML::IXMLDOMNodePtr	aDOMNodePtr, BSTR* abstrpChangedNode)
{
	HRESULT hr = S_OK;

	SVXML::IXMLDOMElementPtr oElementPtr;

	while (1)
	{
		oElementPtr = aDOMNodePtr;	
		break;
	}
	return hr;
}

HRESULT SVXMLClass::SetSVRSchema ()
{
	HRESULT	 hr = S_OK;

	SVXMLSVRSchemaClass oSVRSchema;

	while (1)
	{
		hr = oSVRSchema.SetSVRSchema (svmopDOM);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}
		break;
	} // while (1)
	return hr;
}

HRESULT SVXMLClass::ConfigureAllElementsOffRootToValidateAgainstSchema ()
{
	HRESULT hr = S_OK;
	SVXML::IXMLDOMElementPtr oDOMRootPtr;
	SVXML::IXMLDOMElementPtr oElementPtr;
	SVXML::IXMLDOMNodePtr oNodePtr;

//	_variant_t vElementName;

	SVBStr bstrElementName;
//	SVBStr bstrAttributeNameTitle;

	while (1)
	{
		hr = GetRootNode (oDOMRootPtr);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		if (nullptr == oDOMRootPtr)
		{
			hr = -1721;
			break;
		}

		oNodePtr = oDOMRootPtr->GetfirstChild ();
		if (nullptr == oNodePtr)
		{
			hr = -1722;
			break;
		}

		oElementPtr = oNodePtr;

		while (nullptr != oElementPtr)
		{

			bstrElementName = oNodePtr->GetbaseName ();
//??			if ((strcmp (bstrElementName, g_csSchema) == 0) ||
//??				 (strcmp (bstrElementName, g_wcsData) == 0)) 
			if (1) //??
			{
//-			bypass schema branch.
//-         It is assumed that the only DATA element is the final checksum; 
//-			and this should be bypassed.
			}
			else
			if ((strcmp (bstrElementName, g_csRevisionHistory) == 0) ||
				 (wcscmp (bstrElementName, g_wcsNode) == 0))
			{
//-			Add schema validation.

				hr = ConfigureBranchToValidateAgainstSchema (oElementPtr);
				if( SEV_SUCCESS != SV_SEVERITY( hr ) )
				{
					break;
				}
			}
			else
			{
//-			Unknown quantity.  Will not flag as an error at this time.
			}
			
			oNodePtr = oNodePtr->GetnextSibling ();
			oElementPtr = oNodePtr;
		} // while (nullptr != oElementPtr)

		break;
	} // while (1)
	return hr;
}

HRESULT SVXMLClass::ConfigureBranchToValidateAgainstSchema (SVXML::IXMLDOMElementPtr& arElementPtr)
{
	HRESULT hr = S_OK;
	_bstr_t bstrAttributeName;

	CComVariant ccvAttributeValue;

	while (1)
	{
		bstrAttributeName = g_csXMLNS;
		ccvAttributeValue = (BSTR) ((_bstr_t) "x-schema:#" + (_bstr_t) g_csSVR00001SchemaName);

		hr = arElementPtr->setAttribute (bstrAttributeName, ccvAttributeValue);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			hr = -1723;
			break;
		}
		break;
	}
	return hr;
}

HRESULT SVXMLClass::Clear ()
{
	HRESULT hr = S_OK;

	while (1)
	{
		m_lIsEncrypted = FALSE;
		m_wczRevisionHistory [0] = _T('\0');
		m_lCurrentVersion = 0;
		m_lCurrentRevision = 0;

		hr = svmopDOM->Clear ();
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		hr = SetSVRSchema ();
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}
		break;
	} // while (1)
	return hr;
}

HRESULT SVXMLClass::IsElementNode (SVXML::IXMLDOMElementPtr	aDOMElementPtr, BSTR* abstrpDOMElementTag)
{
	HRESULT hr = S_OK;

	long lIsElementNode;

	_bstr_t bstrDOMElementTag;

	BSTR bstrHack;

	lIsElementNode = 1;  // FALSE
	bstrHack = nullptr;

	while (1)
	{
		bstrDOMElementTag = aDOMElementPtr->GetnodeName ();

//-	It was difficult to get the _bstr_t to be treated as a standard
//-   BSTR. So bstrHack was created.  This should be a direct 
//-	reference to the underlying BSTR, and as such should not be 
//-   modified in any way.
		bstrHack = (wchar_t*) bstrDOMElementTag;
		if (nullptr == bstrHack)
		{
			hr = -1772;
			break;
		}

		hr = IsElementNode (bstrHack);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			hr = -1808;
			break;
		}

		lIsElementNode = hr;

		if (nullptr != abstrpDOMElementTag)
		{
//-		We would love to detach the BSTR so that we didn't need to 
//-		make a copy.
			*abstrpDOMElementTag = bstrDOMElementTag.copy ();
		}
		break;
	}

	if( SEV_SUCCESS != SV_SEVERITY( hr ) )
	{
	}
	else
	{
		hr = lIsElementNode;
	}
	return hr;
}

HRESULT SVXMLClass::IsElementNode (BSTR& abstrrDOMElementTag)
{
	HRESULT hr = 0;

	while (1)
	{
	   	if (!_wcsnicmp (abstrrDOMElementTag,  g_wcsNode,  SysStringLen (abstrrDOMElementTag)))
		{
			hr = 0; // Is node.
		}
		else
		{
			hr = 1; // Is not node.
		}
		break;
	}
	return hr;
}

HRESULT SVXMLClass::IsElementData (SVXML::IXMLDOMElementPtr	aDOMElementPtr, BSTR* abstrpDOMElementTag)
{
	HRESULT hr = 0;

	long lIsElementData;

	_bstr_t bstrDOMElementTag;

	BSTR bstrHack;

	lIsElementData = 1;  // FALSE
	bstrHack = nullptr;

	while (1)
	{
		bstrDOMElementTag = aDOMElementPtr->GetnodeName ();

		bstrHack = (wchar_t*) bstrDOMElementTag;
		if (nullptr == bstrHack)
		{
			hr = -1809;
			break;
		}

		hr = IsElementData (bstrHack);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			hr = -1810;
			break;
		}

		lIsElementData = hr;

		if (nullptr != abstrpDOMElementTag)
		{
			*abstrpDOMElementTag = bstrDOMElementTag.copy ();
		}
			
		break;
	}

	if( SEV_SUCCESS != SV_SEVERITY( hr ) )
	{
	}
	else
	{
		hr = lIsElementData;
	}
	
	return hr;
}

HRESULT SVXMLClass::IsElementData (BSTR& abstrrDOMElementTag)
{
	HRESULT hr = 0;

	while (1)
	{
   		if (!_wcsnicmp (abstrrDOMElementTag, g_wcsData, SysStringLen (abstrrDOMElementTag)))
		{
			hr = 0; // Is data.
		}
		else
		{
			hr = 1; // Is not data.
		}
		break;
	}
	return hr;
}

HRESULT SVXMLClass::GetElementData (SVXML::IXMLDOMElementPtr aDOMElementPtr, VARIANT* vpDOMElementData)
{
	HRESULT l_Status( S_OK );

	l_Status = svmopDOM->GetDOMNodeElementValue( aDOMElementPtr, vpDOMElementData );

	if( S_OK == l_Status && VT_ARRAY == ( vpDOMElementData->vt & VT_ARRAY ) )
	{
		_variant_t value;
		l_Status = SVXMLSafeArrayConverter::CopyDOMNodeToSafeArray(*this, aDOMElementPtr, value );
		if (S_OK == l_Status )
		{
			*vpDOMElementData = value.Detach();
		}
	}
	return l_Status;
}

HRESULT SVXMLClass::GetElementName (SVXML::IXMLDOMElementPtr aDOMElementPtr, BSTR* abstrpDOMElementName)
{
	HRESULT	hr = 0;

	while (1)
	{
		hr = svmopDOM->GetDOMNodeElementName (aDOMElementPtr, abstrpDOMElementName);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}
		break;
	}
	return hr;
}

HRESULT SVXMLClass::IsElementFromParent (SVXML::IXMLDOMElementPtr aDOMElementPtr)
{
	HRESULT hr = 0;

	BSTR bstrElementName = nullptr;
	SVBStr bstrFromParent;

	while (1)
	{
		bstrFromParent = g_wcsFromParent;

		hr = GetElementName (aDOMElementPtr, &bstrElementName);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		if (_wcsnicmp (bstrElementName, bstrFromParent, SysStringLen (bstrElementName)))
		{
//-		Result not zero, element name is not "<FromParent>".
			hr = 1; // S_FALSE
		}
		else
		{
//-		Result zero, element name is "<FromParent>".
			hr = 0; // S_OK
		}
		
		break;
	}
	if (bstrElementName)
	{
		SysFreeString (bstrElementName);
	}

	return hr;
}

HRESULT SVXMLClass::IsElementSafeArray (SVXML::IXMLDOMElementPtr aDOMElementPtr)
{
	HRESULT hr = 0;

	BSTR bstrElementName = nullptr;

	VARIANT vElementData;

	while (1)
	{
		hr = GetElementData( aDOMElementPtr, &vElementData );
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}
		if (vElementData.vt == VT_ARRAY)
		{
//-		element is a "VT_ARRAY" type
			hr = 0;
		}
		else
		{
//-		element is not a "VT_ARRAY" type
			hr = 1;
		}
		break;
	}
	return hr;
}

HRESULT SVXMLClass::GetBaseElement (SVXML::IXMLDOMElementPtr& arBaseElementPtr)
{
	HRESULT hr = 0;

	SVXML::IXMLDOMElementPtr oRootElementPtr;

	WCHAR wcsNameSpace [128];
	_bstr_t bstrSelectName;

	SVBStr bstrTempNodeType;
	SVBStr bstrNodeType;
	SVBStr bstrQueryNameSpace;

	while (1)
	{
		hr = GetRootNode (oRootElementPtr);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		if (nullptr == oRootElementPtr)
		{
			hr = -1793;
			break;
		}

		hr = GetNameSpace (wcsNameSpace, sizeof (wcsNameSpace));
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

//-	xmlns:SVR1="x-schema:#SVR00001"
		bstrQueryNameSpace = (BSTR)((_bstr_t) g_csXMLNS + ":SVR1=\"x-schema:#" + (_bstr_t) g_csSVR00001SchemaName + "\"");
		hr = SetQueryNameSpace (bstrQueryNameSpace);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		if (m_lIsEncrypted == TRUE)
		{
			hr = bstrTempNodeType.CopyFromWChar (g_wcsBaseNode);
			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				break;
			}

			hr = m_opEncryption->EncryptString(2, bstrTempNodeType, &bstrNodeType);
			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				break;
			}
		}
		else // m_lIsEncrypted == FALSE
		{
			hr = bstrNodeType.CopyFromWChar (g_wcsBaseNode);
			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				break;
			}
		}

//		bstrSelectName = "/" +  (_bstr_t) g_wcsRoot + "/SVR1:" + (_bstr_t) g_csRevisionHistory;
//-	/Root/SVR1:NODE[@Name = "Base"]  -  XPath string
//-   The objective is to find element "/Root/NODE where attribute "Name" equals "Base".
// - no one cares what the root node is called, let me use whatever I want...
		bstrSelectName = (_bstr_t) L"/*" /*+ g_wcsRoot*/ + 
			                        L"/SVR1:" + g_wcsNode + 
						            L"[@" + g_wcsType + 
						            L" = \"" +
									(BSTR) bstrNodeType +
									L"\"]";

//-	search for .... A FEW WORDS ABOUT selectSingleNode (). ----------------
		arBaseElementPtr = oRootElementPtr->selectSingleNode (bstrSelectName);

		if (nullptr == arBaseElementPtr)
		{
//-		No revision history base node found.
			hr = 2;
		}
		break;
	} // while (1)
	return hr;
}

HRESULT SVXMLClass::GetElementNbrOfChildren (const SVXML::IXMLDOMElementPtr aDOMElementPtr, long* alpNbrOfChildren) const
{
	return svmopDOM->GetElementNbrOfChildren (aDOMElementPtr, alpNbrOfChildren);
}

SVXML::IXMLDOMElementPtr SVXMLClass::GetFirstElementChild(const SVXML::IXMLDOMElementPtr aDOMElementPtr) const
{
	return svmopDOM->GetFirstElementChild(aDOMElementPtr);
}

SVXML::IXMLDOMElementPtr SVXMLClass::GetNextElementSibling(const SVXML::IXMLDOMElementPtr aDOMElementPtr) const
{
	return svmopDOM->GetNextElementSibling(aDOMElementPtr);
}

HRESULT SVXMLClass::CreateElement (BSTR p_bstrElementName, SVXML::IXMLDOMElementPtr& p_orDOMElementPtr)
{
	HRESULT hr = S_OK;

	while (1)
	{
		hr = svmopDOM->CreateElement (p_bstrElementName, p_orDOMElementPtr);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}
		break;
	} // while (1)
	return hr;
}

HRESULT SVXMLClass::GetRootNode (SVXML::IXMLDOMElementPtr& p_orDOMRootPtr)
{
	HRESULT hr = S_OK;

	while (1)
	{
		hr = svmopDOM->GetRootNode (p_orDOMRootPtr);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}
		break;
	} // while (1)
	return hr;
}

HRESULT SVXMLClass::AppendChildToDOMNode (SVXML::IXMLDOMNodePtr p_oParentNodePtr, SVXML::IXMLDOMNodePtr p_oChildNodePtr)
{
	HRESULT hr = S_OK;
	if(nullptr != p_oChildNodePtr)
	{
		hr = svmopDOM->AppendChildToDOMNode (p_oParentNodePtr, p_oChildNodePtr);
	}
	return hr;
}

HRESULT SVXMLClass::AppendCarriageReturnToDOMNode (SVXML::IXMLDOMNodePtr p_oParentNodePtr)
{
	HRESULT hr = S_OK;

	while (1)
	{
		hr = svmopDOM->AppendCarriageReturnToDOMNode (p_oParentNodePtr);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}
		break;
	} // while (1)
	return hr;
}

HRESULT SVXMLClass::GetNameSpace (WCHAR* p_wcsNameSpace, long p_lBufferSize)
{
	HRESULT hr = S_OK;

	while (1)
	{
		hr = svmopDOM->GetNameSpace (p_wcsNameSpace, p_lBufferSize);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}
		break;
	} // while (1)
	return hr;
}

HRESULT SVXMLClass::SetQueryNameSpace (BSTR	p_bstrQueryNameSpace)
{
	HRESULT hr = S_OK;

	while (1)
	{
		hr = svmopDOM->SetQueryNameSpace (p_bstrQueryNameSpace);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}
		break;
	} // while (1)
	return hr;
}

HRESULT SVXMLClass::PreserveWhitespace(bool bPreserve)
{
	HRESULT hr = S_OK;
	if (svmopDOM)
	{
		hr = svmopDOM->PreserveWhitespace(bPreserve);
	}
	else
	{
		hr = E_POINTER;
	}
	return hr;
}

