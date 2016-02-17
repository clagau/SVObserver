//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDOMClass
//* .File Name       : $Workfile:   SVDom.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   19 Dec 2014 04:44:26  $
//******************************************************************************

#include "stdafx.h"
#include <atlbase.h>					// CComVariant
#include <comdef.h>					// class_bstr_t
#import <msxml6.dll> // MSXML
#include "SVDOM.h"					// SVDOMClass, MSXML3, SVBStr (included)
#include "SVXMLEncryptionClass.h"

#include "SVUtilityLibrary\SVUtilityGlobals.h"
#include "SVMessage\SVMessage.h"
#include "SVVariantCustomTypes.h"
#include "SVVariantConverter.h"

const WCHAR*	g_wcsName		= L"Name";
const WCHAR*	g_wcsRoot		= L"Root";
const WCHAR*	g_wcsType		= L"Type";

const WCHAR*	g_wcsBaseNode	= L"SV_BASENODE";

const char*		g_csValue		= "Value";
const char*		g_csXMLNS		= "xmlns";	// xml name space

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVDOMClass::SVDOMClass()
: svmlInitialized(0)
, svmlCoInitialized(0)
, m_lUseEncryption(0)
, m_opEncryption(NULL)
{
	Init ();
}

HRESULT SVDOMClass::Init ()
{
	HRESULT hr = 0;

	svmlInitialized = FALSE;
	svmlCoInitialized = FALSE;

	m_opEncryption = NULL;

	return hr;
}

HRESULT SVDOMClass::Initialize (long p_lUseEncryption)
{
	HRESULT hr = 0;

	while (1)
	{
		if (svmlInitialized == TRUE)
		{
			hr = 1617;
			break;
		}

		svmlInitialized = TRUE;

		hr = CoInitialize(NULL); // Check the return value, hr...
		if (hr == S_FALSE)
		{
//-		A return value of S_FALSE means that somebody has already 
//-		called coInitialize () for this thread.
			CoUninitialize();
			hr = S_OK;
		}
		else
		if (hr != S_OK)
		{
			hr = -1619;
			break;
		}
		else
		{
			svmlCoInitialized = TRUE;
		}

		hr = InitializeDOM ();

		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		m_lUseEncryption = p_lUseEncryption;
		if (m_opEncryption != NULL)
		{
			hr = -1894;
			break;
		}

		m_opEncryption = new SVXMLEncryptionClass;
		if (m_opEncryption == NULL)
		{
			hr = -1893;
			break;
		}

		hr = m_opEncryption->InitializeDOMInterface (this);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

//-	Clear () sets the root and then sets the header (<?xml....).
		hr = Clear ();

		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			hr = -1621;
			break;
		}

		break;
	}

	if( SEV_SUCCESS != SV_SEVERITY( hr ) )
	{
//-	There was error, initialization is incomplete.
		svmlInitialized = FALSE;
	}
	else
	{
		svmlInitialized = TRUE;
	}

	return hr;
}

SVDOMClass::~SVDOMClass()
{
//-It is assumed that Release () will correctly handle if svmDOMPtr == NULL 
//-or svmRootNodePtr == NULL,
	if (svmDOMPtr != NULL)
	{
		svmDOMPtr.Release ();
	}

	if (svmRootNodePtr != NULL)
	{
		svmRootNodePtr.Release ();
	}

	if (svmlCoInitialized == TRUE)
	{
		CoUninitialize ();
		svmlCoInitialized = FALSE;
	}

	delete m_opEncryption;
	m_opEncryption = NULL;

	svmlInitialized = FALSE;
}

HRESULT SVDOMClass::InitializeDOM ()
{
	HRESULT hr = 0;

	CComVariant	ccvXPath;
	SVBStr bstrSelectionLanguage;

	while (1)
	{

//-	This uses smart pointers
//-	Creates the instance of the COM object.
		hr = svmDOMPtr.CreateInstance( "Msxml2.DOMDocument.6.0" );
		if (hr != S_OK)
		{
			hr = -1618;
			break;
		}

		ccvXPath = "XPath";
		bstrSelectionLanguage = "SelectionLanguage";

		hr = svmDOMPtr->setProperty((BSTR) bstrSelectionLanguage, ccvXPath);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			hr = -1742;
			break;
		}

		svmDOMPtr->async = false;

//-   By default, the XML parser will attempt to manipulate white space.
//-
//-   When the following line is enabled (not commented out), what the parser
//-	loads does not contain children below the "base" node level.  get child
//-	returns NULL, so "Environment" does not exist.
//		svmDOMPtr->PutpreserveWhiteSpace (VARIANT_TRUE);

		break;
	}
	return hr;
}

HRESULT SVDOMClass::CreateRootNode ()
{
	SVXML::IXMLDOMNodePtr	oRootNodePtr;

	HRESULT hr = 0;
	_bstr_t bstrRootName;

	while (1)
	{

		bstrRootName = g_wcsRoot;
//		hr = SetNameSpace (bstrRootName);
//
		hr = CreateDOMNodeChildElement (svmDOMPtr,
												  bstrRootName,
												  NULL,
												  NULL,
												  oRootNodePtr);
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
		svmRootNodePtr = oRootNodePtr;
	}
	return hr;
}

HRESULT SVDOMClass::SetHeader ()
{
	HRESULT	hr = 0;

	_bstr_t	bstrTarget;
	_bstr_t	bstrData;

	CComVariant	ccvTarget;
	CComVariant	ccvData;

	SVXML::IXMLDOMProcessingInstructionPtr	oXMLProcessingInstructionPtr;

	SVXML::IXMLDOMElementPtr   domRootPtr;

	while (1)
	{
		if (svmlInitialized == FALSE)
		{
			hr = -1632;
			break;
		}

		bstrTarget = "xml";
		bstrData = "version='1.0' standalone='yes'";
		oXMLProcessingInstructionPtr = svmDOMPtr->createProcessingInstruction (bstrTarget, 
																							  		  bstrData);
		if (oXMLProcessingInstructionPtr == NULL)
		{
			hr = -1615;
			break;
		}

		hr = AppendChildToDOMDoc (oXMLProcessingInstructionPtr);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}
		break;
	} // while (1)
	return hr;
}

HRESULT SVDOMClass::Clear ()
{
	HRESULT	hr = 0;

	SVXML::IXMLDOMNodeListPtr oNodeListPtr;
	SVXML::IXMLDOMNodePtr oNodePtr;
	SVXML::IXMLDOMNodePtr oNode1Ptr;

	_bstr_t bstrNodeName;

//	long lListLength;
//	long lLoop;

//	pNodeList = NULL;
//	pNode = NULL;

	while (1)
	{
		if (svmlInitialized == FALSE) 
		{
			hr = -1615;
			break;
		}

		hr = ClearNameSpace ();
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

//-	Get child node list from main document.  Remove all items from the 
//-	list.  At this time I'm assuming that removing an item removes all 
//-	children of that item.

		// BRW:  Added check here to avoid crash.
		if ( svmDOMPtr != NULL )
		{
			oNodeListPtr = svmDOMPtr->GetchildNodes();
		}

		if (oNodeListPtr == NULL)
		{
			hr = -1620;
			break;
		}

//-   Hmmmmmm, this list should only contain the Root element.  But incase
//-   things are not what they seem, we will delete everything and then 
//-	re-initialize the DOM.
		do
		{
			oNodeListPtr->reset ();

			oNodePtr = oNodeListPtr->nextNode ();
			if (oNodePtr == NULL)
			{
//-			It is assumed that we are at the end of the list.				
				break;
			}

			oNode1Ptr = svmDOMPtr->removeChild (oNodePtr);
			if (oNode1Ptr == NULL)
			{
				hr = -1692;
				break;
			}
		} while (1); // relies on nodePtr == NULL to leave the loop


		hr = SetHeader ();
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		hr = CreateRootNode ();
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}
		break;
	} // while (1)
	return hr;
}

HRESULT SVDOMClass::GetRootNode (SVXML::IXMLDOMElementPtr& apDOMRootPtr)
{
	HRESULT	hr = 0;
	apDOMRootPtr = NULL;

	while (1)
	{
		if (svmlInitialized == FALSE)
		{
			hr = -1631;
			break;
		}

		apDOMRootPtr = svmDOMPtr->GetdocumentElement ();
		break;
	}
	return hr;
}

HRESULT SVDOMClass::CopyDOMToXMLFile (BSTR abstrFileName)
{
	HRESULT	hr = 0;

	while (1)
	{
		if (svmlInitialized == FALSE)
		{
			hr = -1645;
			break;
		}
		
		hr = svmDOMPtr->save (abstrFileName);

//		if (hr == XML_BAD_ENCODING)
//		{
//			hr = -1646;
//			break;
//		}
//		else
		if (hr == E_INVALIDARG)
		{
			hr = -1647;
			break;
		}
		else
		if (hr == E_ACCESSDENIED)
		{
			hr = -1648;
			break;
		}
		else
		if (hr == E_OUTOFMEMORY)
		{
			hr = -1649;
			break;
		}
		else
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			hr = -1650;
			break;
		}

		break;
	}

	return hr;
}

HRESULT SVDOMClass::CreateDOMNodeChildElement(SVXML::IXMLDOMNodePtr	aDOMParentNodePtr, BSTR abstrElementTag, BSTR abstrElementName, VARIANT* avpElementValue, SVXML::IXMLDOMNodePtr& arDOMChildElementPtr)
{
	HRESULT hr = 0;

	SVXML::IXMLDOMElementPtr oTempElementPtr;

	while (1)
	{
		
		hr = CreateDOMNodeElement (abstrElementTag, abstrElementName, avpElementValue, oTempElementPtr);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		arDOMChildElementPtr = aDOMParentNodePtr->appendChild(oTempElementPtr);
		if (arDOMChildElementPtr == NULL)
		{
			hr = -1643;
			break;
		}

		break;
	}

//	SysFreeString (bstrValue);
//	SysFreeString (bstrType);

	return hr;
}

//##ModelId=41264A79024E
HRESULT SVDOMClass::CreateDOMNodeElement (BSTR abstrElementTag, BSTR abstrElementName, VARIANT* avpElementValue, SVXML::IXMLDOMElementPtr& arDOMChildElementPtr)
{
	HRESULT hr = 0;

	long lType;
	long l_lNameEncryptionMethod;

	SVXML::IXMLDOMElementPtr oTempElementPtr;

	_bstr_t bstrAttributeName;
	_bstr_t bstrElementText;

	SVBStr l_bstrElementName;
	SVBStr l_bstrElementValue;
	SVBStr l_bstrElementType;
	SVBStr l_bstrDebugDecryptedString;

	BSTR bstrValue;
	BSTR bstrType;

	CComVariant ccvAttributeValue;
	VARIANT vAttributeValue;

//	FILE*								hFile;
//	char								l_csHexDump [4000];

	bstrValue = NULL;
	bstrType = NULL;

	while (1)
	{
//		hFile = _wfopen (L"c:\\EncryptLog.txt", L"at");

		if (svmlInitialized != TRUE)
		{
			hr = -1889;
			break;
		}

		if (m_lUseEncryption)
		{
			if (m_opEncryption->m_lEncryptionMethod == -1)
			{
//--------- We are doing pre-encryption stuff.
//fwrite ("Pre initialization Element Name = \"", 35, 1, hFile); 
//fwrite ((BSTR) abstrElementName, SysStringByteLen (abstrElementName), 1, hFile);
//fwrite ("\"\n", 2, 1, hFile);

				l_lNameEncryptionMethod = 1;
				hr = m_opEncryption->EncryptString (l_lNameEncryptionMethod, abstrElementName, &l_bstrElementName);
				if( SEV_SUCCESS != SV_SEVERITY( hr ) )
				{
					break;
				}

			}
			else
			{
//fwrite ("Element Name = \"", 16, 1, hFile); 
//fwrite ((BSTR) abstrElementName, SysStringByteLen (abstrElementName), 1, hFile);
//fwrite ("\"\n", 2, 1, hFile);

				hr = m_opEncryption->EncryptNameAttribute (abstrElementName, &l_bstrElementName);

#ifdef _DEBUG

				hr = m_opEncryption->DecryptNameAttribute (l_bstrElementName, &l_bstrDebugDecryptedString);

				if( SEV_SUCCESS != SV_SEVERITY( hr ) )
				{
					break;
				}

//fwrite ("Encrypted Element Name = \"", 26, 1, hFile); 
//fwrite ((BSTR) l_bstrElementName, SysStringByteLen (l_bstrElementName), 1, hFile);
//fwrite ("\"\n", 2, 1, hFile);

				if (((WCHAR*) l_bstrElementName == NULL) && 
					 ((WCHAR*) l_bstrElementName == (WCHAR *) l_bstrDebugDecryptedString))
				{
				}
				else
				if (wcscmp (abstrElementName, l_bstrDebugDecryptedString) != 0)
				{
		//-		Source and decrypted string not equal.
//AfxMessageBox(_T("Write Compare Failed1"));
					ASSERT (0);
				}

#endif

			}
		}
		else
		{
			l_bstrElementName = abstrElementName;
		}


//-	Create the element using the Tag name.
		hr = CreateElement (abstrElementTag, oTempElementPtr);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}


//-	If an element name/label was specified, then add it.
		if (abstrElementName != NULL)
		{
			bstrAttributeName = g_wcsName;
		
			vAttributeValue.vt = VT_BSTR;
			vAttributeValue.bstrVal = l_bstrElementName;

//-		As an explanation to the commented out code, and why it's not used.  
//-      A BSTR is permitted to contain NULLs.  However, the = operator for 
//-		CComVariant uses SysAllocString; which doesn't work on BSTRs that 
//-      contain NULLs.
//			ccvAttributeValue = (BSTR) l_bstrElementName;
//			oTempElementPtr->setAttribute (bstrAttributeName, ccvAttributeValue);
//			ccvAttributeValue.Clear ();

			oTempElementPtr->setAttribute (bstrAttributeName, vAttributeValue);

//-		Make sure nobody attempts to VariantClear () our BSTR.
			vAttributeValue.vt = VT_EMPTY;
			vAttributeValue.bstrVal = NULL;
		}

		
		if (avpElementValue != NULL)
		{
			hr = SVVariantConverter::TranslateVariant (avpElementValue, &bstrValue, &bstrType);
			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				break;
			}

			lType = avpElementValue->vt;

//-   	The value should not be included, only if, type equals VT_ARRAY,  
//-		type equals array node type (g_lSVArrayNode), type equals base node
//-		type (g_lSVBaseNode), or VT_NULL.  If type is VT_ARRAY 
//-		and VT_I4, then the value would be included.
			if ((lType == VT_ARRAY) ||
				 (lType == VT_SVARRAYNODE) ||
				 (lType == VT_SVBASENODE) ||
				 (lType == VT_NULL))
			{
//-			Don't write value.
			}
			else
			{

//fwrite ("Element Value = \"", 17, 1, hFile); 
//fwrite ((BSTR) bstrValue, SysStringByteLen (bstrValue), 1, hFile);
//fwrite ("\"\n", 2, 1, hFile);
				if (m_lUseEncryption)
				{
					if (m_opEncryption->m_lEncryptionMethod == -1)
					{
//--------------- We are doing pre-encryption stuff.
						l_lNameEncryptionMethod = 1;
						hr = m_opEncryption->EncryptString (l_lNameEncryptionMethod, bstrValue, &l_bstrElementValue);
						if( SEV_SUCCESS != SV_SEVERITY( hr ) )
						{
							break;
						}
					}
					else
					{
						hr = m_opEncryption->EncryptString (m_opEncryption->m_lEncryptionMethod, bstrValue, &l_bstrElementValue);
						if( SEV_SUCCESS != SV_SEVERITY( hr ) )
						{
							break;
						}
					}

//fwrite ("CreateNode11\n", 13, 1, hFile);
//CString x;
//x.Format ("Length = %d\n", l_bstrElementValue.GetLength ());
//fwrite (x, x.GetLength (), 1, hFile);
//l_bstrElementValue.CharHexDump (l_csHexDump, sizeof (l_csHexDump));
//fwrite ("Encrypted buffer\n", 17, 1, hFile);
//fwrite (l_csHexDump, strlen (l_csHexDump), 1, hFile);
//fwrite ("\n", 1, 1, hFile);		
#ifdef _DEBUG

					hr = m_opEncryption->DecryptString (l_bstrElementValue, &l_bstrDebugDecryptedString);

					if( SEV_SUCCESS != SV_SEVERITY( hr ) )
					{	
//AfxMessageBox(_T("Calling the dump2"));
//l_bstrDebugDecryptedString.CharHexDump (l_csHexDump, sizeof (l_csHexDump));
//fwrite ("Decrypted buffer\n", 17, 1, hFile);
//fwrite (l_csHexDump, strlen (l_csHexDump), 1, hFile);
//fwrite ("\n", 1, 1, hFile);			

						if (hr == -1933)
						{
							hr = -1954;
						}
						break;
					}

					if (wcscmp (bstrValue, l_bstrDebugDecryptedString) != 0)
					{
			//-		Source and decrypted string not equal.
						ASSERT (0);
					}

#endif

				}
				else
				{
					hr = l_bstrElementValue.CopyFromWChar (bstrValue);
					if( SEV_SUCCESS != SV_SEVERITY( hr ) )
					{
						break;
					}
				}
//fwrite ("Encrypted Element Value = \"", 27, 1, hFile); 
//fwrite ((BSTR) l_bstrElementValue, SysStringByteLen (l_bstrElementValue), 1, hFile);
//fwrite ("\"\n", 2, 1, hFile);

//- DATA AS TEXT ------------------------------------------------------------
				bstrElementText = (BSTR) l_bstrElementValue;
				oTempElementPtr->Puttext (bstrElementText);
//---------------------------------------------------------------------------

/*- DATA AS ATTRIBUTE.................
//-			Write value.
				bstrAttributeName = g_csValue;
				ccvAttributeValue = bstrValue;
				oTempElementPtr->setAttribute (bstrAttributeName, ccvAttributeValue);
				ccvAttributeValue.Clear ();
*/
			}

			if (lType == VT_NULL)
			{
//-			Don't write type.
			}
			else
			{
//-			Write type.
				bstrAttributeName = g_wcsType;
				ccvAttributeValue = bstrType;

//fwrite ("Element Type = \"", 16, 1, hFile); 
//fwrite ((BSTR) bstrType, SysStringByteLen (bstrType), 1, hFile);
//fwrite ("\"\n", 2, 1, hFile);

				if (m_lUseEncryption)
				{
					if (m_opEncryption->m_lEncryptionMethod == -1)
					{
//--------------- We are doing pre-encryption stuff.
						l_lNameEncryptionMethod = 1;
						hr = m_opEncryption->EncryptString (l_lNameEncryptionMethod, bstrType, &l_bstrElementType);
						if( SEV_SUCCESS != SV_SEVERITY( hr ) )
						{
							break;
						}
					}
					else
					if (lType == VT_SVBASENODE)
					{
						l_lNameEncryptionMethod = 2;
						hr = m_opEncryption->EncryptString (l_lNameEncryptionMethod, bstrType, &l_bstrElementType);
						if( SEV_SUCCESS != SV_SEVERITY( hr ) )
						{
							break;
						}
					}
					else
					{
						hr = m_opEncryption->EncryptString (m_opEncryption->m_lEncryptionMethod, bstrType, &l_bstrElementType);
						if( SEV_SUCCESS != SV_SEVERITY( hr ) )
						{
							break;
						}
					}

//fwrite ("Encrypted Element Type = \"", 26, 1, hFile); 
//fwrite ((BSTR) l_bstrElementType, SysStringByteLen (l_bstrElementType), 1, hFile);
//fwrite ("\"\n", 2, 1, hFile);


#ifdef _DEBUG

					hr = m_opEncryption->DecryptString (l_bstrElementType, &l_bstrDebugDecryptedString);

					if( SEV_SUCCESS != SV_SEVERITY( hr ) )
					{
						if (hr == -1933)
						{

//AfxMessageBox(_T("Calling the dump1"));
//l_bstrElementType.CharHexDump (l_csHexDump, sizeof (l_csHexDump));
//fwrite ("Encrypted buffer\n", 17, 1, hFile);
//fwrite (l_csHexDump, strlen (l_csHexDump), 1, hFile);
//fwrite ("\n", 1, 1, hFile);		

//l_bstrDebugDecryptedString.CharHexDump (l_csHexDump, sizeof (l_csHexDump));
//fwrite ("Decrypted buffer\n", 17, 1, hFile);
//fwrite (l_csHexDump, strlen (l_csHexDump), 1, hFile);
//fwrite ("\n", 1, 1, hFile);		
	
							hr = -1955;
						}

						break;
					}

					if (wcscmp (bstrType, l_bstrDebugDecryptedString) != 0)
					{
			//-		Source and decrypted string not equal.
//AfxMessageBox(_T("Write Type Failed1"));
						ASSERT (0);
					}
#endif

				}
				else
				{
					hr = l_bstrElementType.CopyFromWChar (bstrType);
					if( SEV_SUCCESS != SV_SEVERITY( hr ) )
					{
						break;
					}
				}

				vAttributeValue.vt = VT_BSTR;
				vAttributeValue.bstrVal = l_bstrElementType;

	//-		As an explanation to why the CComVariant is not used.  
	//-      A BSTR is permitted to contain NULLs.  However, the = operator for 
	//-		CComVariant uses SysAllocString; which doesn't work on BSTRs that 
	//-      contain NULLs.

				oTempElementPtr->setAttribute (bstrAttributeName, vAttributeValue);

	//-		Make sure nobody attempts to VariantClear () our BSTR.
				vAttributeValue.vt = VT_EMPTY;
   			vAttributeValue.bstrVal = NULL;
			}

		} // if (avpElementValue != NULL)

		break;
	} // while (1)

	SysFreeString (bstrValue);
	SysFreeString (bstrType);

	if( SEV_SUCCESS != SV_SEVERITY( hr ) )
	{
	}
	else
	{
//-	No errors
		arDOMChildElementPtr = oTempElementPtr;
	}

//	fclose (hFile);
	return hr;
}

HRESULT	SVDOMClass::CopyXMLFileToDOM (BSTR	abstrLoadText)
{
	HRESULT hr = 0;

	SVXML::IXMLDOMParseErrorPtr oXMLParseErrorPtr;

	long lParseErrorNbr;
	long lLineNbr;
	long lLinePosition;

	SVBStr bstrReason;
//	_bstr_t bstrR;

	VARIANT_BOOL bResult;

	while (1)
	{
		if (svmlInitialized == FALSE)
		{
			hr = -1745;
			break;
		}
		
		bResult = svmDOMPtr->load (abstrLoadText);

		if (bResult == VARIANT_FALSE)	// VARIANT_TRUE is -1 or 0xffff
		{
			oXMLParseErrorPtr = svmDOMPtr->GetparseError ();
			if (oXMLParseErrorPtr == NULL)
			{
				hr = -1670;
				break;
			}

			lParseErrorNbr = oXMLParseErrorPtr->errorCode;

			lLineNbr = oXMLParseErrorPtr->line;
			lLinePosition = oXMLParseErrorPtr->linepos;
			bstrReason = oXMLParseErrorPtr->reason;

			hr = -1748;
			break;
		}
		break;
	}
	return hr;
}


HRESULT SVDOMClass::CopyXMLTextToDOM (BSTR	abstrLoadText)
{
	HRESULT	Result = S_OK;

	if (svmlInitialized == FALSE)
	{
		Result = -1746;
	}

	if( S_OK == Result )
	{
		VARIANT_BOOL LoadXmlResult;
		LoadXmlResult = svmDOMPtr->loadXML (abstrLoadText);
		
		if(LoadXmlResult == VARIANT_FALSE)	// VARIANT_TRUE is -1 or 0xffff
		{
			Result = -1747;
		}
	}

	return Result;
}

HRESULT SVDOMClass::CreateElement (BSTR abstrElementName, SVXML::IXMLDOMElementPtr&	arDOMElementPtr)
{
	HRESULT hr = 0;

	CComVariant ccvNodeType;

	while (1)
	{
		ccvNodeType = SVXML::NODE_ELEMENT;

		arDOMElementPtr = svmDOMPtr->createNode (ccvNodeType, abstrElementName, (BSTR) svmbstrNameSpace);

		if (arDOMElementPtr == NULL)
		{
			hr = -1910;
			break;
		}
		break;
	}
	return hr;
}

HRESULT SVDOMClass::AppendChildToDOMDoc (SVXML::IXMLDOMNodePtr	aNodePtr)
{
	HRESULT	hr = 0;

	SVXML::IXMLDOMNodePtr oNewNodePtr;

	while (1)
	{
		if (svmlInitialized == FALSE)
		{
			hr = -1684;
			break;
		}

//-	Calling AppendChildToDOMNode () and attempting to opperate at the 
//-	IXMLNode level will not work. or will it?
		hr = AppendChildToDOMNode (svmDOMPtr, aNodePtr);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}
		break;
	} // while (1)
	return hr;
}


//##ModelId=41264A790289
HRESULT SVDOMClass::AppendChildToDOMNode (SVXML::IXMLDOMNodePtr aParentNodePtr, SVXML::IXMLDOMNodePtr aChildNodePtr)
{
	HRESULT	hr = 0;
	SVXML::IXMLDOMNodePtr oNewNodePtr;

	while (1)
	{
		if (svmlInitialized == FALSE)
		{
			hr = -1694;
			break;
		}

		oNewNodePtr = aParentNodePtr->appendChild (aChildNodePtr);
		if (oNewNodePtr == NULL)
		{
			hr = -1674;
			break;
		}

		break;
	} // while (1)

	if( SEV_SUCCESS != SV_SEVERITY( hr ) )
	{
	}
	else
	{
	}
	return hr;
}

HRESULT SVDOMClass::AppendCarriageReturnToDOMNode (SVXML::IXMLDOMNodePtr aParentNodePtr)
{
	HRESULT	hr = 0;

	CComVariant ccvText;

	SVXML::IXMLDOMNodePtr l_oTextNodePtr;
	SVXML::IXMLDOMNodePtr l_oResultNodePtr;
//	SVXML::IXMLDOMElementPtr l_oDOMElementPtr;

	SVBStr l_bstrElementName;
	SVBStr l_bstrElementValue;

	while (1)
	{
		if (svmlInitialized == FALSE)
		{
			hr = -1694;
			break;
		}

		hr = l_bstrElementName.CopyFromWChar (L"");
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		hr = CreateNode (SVXML::NODE_TEXT, 
			              (BSTR*) l_bstrElementName, 
							  (BSTR*) svmbstrNameSpace, 
							  l_oTextNodePtr);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

//		l_oDOMElementPtr = svmDOMPtr->createNode (ccvNodeType, (BSTR) l_bstrElementName, (BSTR) svmbstrNameSpace);
//
//		if (l_oDOMElementPtr == NULL)
//		{
//			hr = -1909;
//			break;
//		}

		hr = l_bstrElementValue.CopyFromWChar (L"\n");
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		ccvText = (BSTR) l_bstrElementValue;
		l_oTextNodePtr->PutnodeValue (ccvText);

		l_oResultNodePtr = aParentNodePtr->appendChild (l_oTextNodePtr);
		if (l_oResultNodePtr == NULL)
		{
			hr = -1911;
			break;
		}

		break;
	} // while (1)

	if( SEV_SUCCESS != SV_SEVERITY( hr ) )
	{
	}
	else
	{
	}

	return hr;
}

HRESULT SVDOMClass::CreateNode (long p_lNodeType, const BSTR* p_bstrpNodeName, const BSTR* p_bstrpNameSpace, SVXML::IXMLDOMNodePtr& p_rNewNodePtr)
{
	HRESULT	hr = S_OK;

	while (1)
	{
		p_rNewNodePtr = svmDOMPtr->createNode (p_lNodeType, const_cast<BSTR> (*p_bstrpNodeName), const_cast<BSTR> (*p_bstrpNameSpace));

		if (p_rNewNodePtr == NULL)
		{
			hr = -1909;
			break;
		}
		break;
	}
	return hr;
}

HRESULT SVDOMClass::ClearNameSpace ()
{
	HRESULT	hr = 0;

	while (1)
	{
		svmbstrNameSpace.CopyFromWChar (L"");

		break;
	} // while (1)

	if( SEV_SUCCESS != SV_SEVERITY( hr ) )
	{
	}
	else
	{
	}
	return hr;
}

HRESULT SVDOMClass::SetNameSpace (BSTR	abstrNameSpace)
{
	HRESULT hr = 0;

	CComVariant	ccvProperty;

	while (1)
	{
		hr = svmbstrNameSpace.CopyFromWChar (abstrNameSpace);
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
	}
	return hr;
}

HRESULT SVDOMClass::GetNameSpace (WCHAR* awcsNameSpace, long alBufferSize)
{
	HRESULT	hr = 0;

	long lNbrChars;
	long lCharacterSize;

	while (1)
	{
		lCharacterSize = sizeof (awcsNameSpace[0]);
		lNbrChars = alBufferSize/lCharacterSize;

		wcsncpy (awcsNameSpace, 
			      svmbstrNameSpace, 
					lNbrChars - 2);

		awcsNameSpace [lNbrChars - 1] = 0;
		break;
	} // while (1)

	return hr;
}

HRESULT SVDOMClass::SetQueryNameSpace (BSTR	abstrQueryNameSpace)
{
	HRESULT hr = 0;
	CComVariant	ccvProperty;

	while (1)
	{
		ccvProperty = abstrQueryNameSpace;
		hr = svmDOMPtr->setProperty("SelectionNamespaces", ccvProperty);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			hr = -1743;
			break;
		}

		break;
	}
	return hr;
}

HRESULT SVDOMClass::GetDOMNodeElementValue (SVXML::IXMLDOMElementPtr aDOMElementPtr, VARIANT* avpValue)
{
	HRESULT hr = 0;

	long l_lIsEncrypted;

	_bstr_t bstrTypeAttributeTitle;
	_bstr_t bstrValueAttributeTitle;
//	_bstr_t bstrTempValue;
	SVBStr bstrTempValue;
	
	SVBStr bstrValue;
	SVBStr bstrTempValueType;
	BSTR bstrValueType;

	_variant_t vType;
	_variant_t vValue;
	VARIANT vNewVariant;

//	FILE* hFile;

	vNewVariant.vt = VT_EMPTY;
	vNewVariant.lVal = 0;

//	hFile = _wfopen (L"c:\\DecryptLog.txt", L"at");

	while (1)
	{
		bstrTypeAttributeTitle = g_wcsType;
		bstrValueAttributeTitle = g_csValue;

//-	All attributes are string types (from XML Parser perspective), 
//-	therefore all variants returned should be BSTRs.  It is assumed that
//-   getAttribute copies the BSTRs and does not give a reference to the 
//-   existing BSTR.
		vType = aDOMElementPtr->getAttribute (bstrTypeAttributeTitle);
		if ((vType.vt != VT_BSTR) && (vType.vt != VT_NULL))
		{
			hr = -1769;
			break;
		}

		hr = m_opEncryption->GetIsEncrypted (&l_lIsEncrypted);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		if (l_lIsEncrypted == TRUE)
		{
			hr = GetDOMNodeMainText (aDOMElementPtr,
											 &bstrTempValue);
			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				break;
			}

//			bstrTempValue = aDOMElementPtr->Gettext ();
			hr = m_opEncryption->DecryptString (bstrTempValue, &bstrValue);
			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				break;
			}

			hr = m_opEncryption->DecryptString (vType.bstrVal, &bstrTempValueType);
			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				break;
			}

//-		This sets a BSTR to the allocated string of the SVBStr.  Nobody
//-		should attempt to desroy the bstrValueType.
			bstrValueType = bstrTempValueType;


//fwrite ("Element Value = \"", 17, 1, hFile); 
//fwrite ((BSTR) bstrValue, SysStringByteLen (bstrValue), 1, hFile);
//fwrite ("\"\n", 2, 1, hFile);

//fwrite ("Encrypted Element Value = \"", 27, 1, hFile); 
//fwrite ((BSTR) bstrTempValue, SysStringByteLen (bstrTempValue), 1, hFile);
//fwrite ("\"\n", 2, 1, hFile);

//fwrite ("Element Type = \"", 16, 1, hFile); 
//fwrite ((BSTR) bstrTempValueType, SysStringByteLen (bstrTempValueType), 1, hFile);
//fwrite ("\"\n", 2, 1, hFile);

//fwrite ("Encrypted Element Type = \"", 26, 1, hFile); 
//fwrite ((BSTR) vType.bstrVal, SysStringByteLen (vType.bstrVal), 1, hFile);
//fwrite ("\"\n", 2, 1, hFile);

		}
		else
		{
//-		DATA AS TEXT -------------------------------------------------------
			hr = GetDOMNodeMainText (aDOMElementPtr,
											 &bstrValue);
			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				break;
			}

//			bstrTempValue = aDOMElementPtr->Gettext ();
//			hr = bstrValue.CopyFromWChar ((WCHAR*) bstrTempValue);
//-		--------------------------------------------------------------------
			
			bstrValueType = vType.bstrVal;
		}

/*- DATA AS ATTRIBUTE.................
		vValue = aDOMElementPtr->getAttribute (bstrValueAttributeTitle);
		if ((vValue.vt != VT_BSTR) && (vValue.vt != VT_NULL))
		{
			hr = -1770;
			break;
		}
*/

		hr = SVVariantConverter::RestoreVariant (bstrValue, bstrValueType, &vNewVariant);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

//-	Do not attempt to destroy bstrValueType.
		bstrValueType = NULL;

		break;
	}

	if( SEV_SUCCESS != SV_SEVERITY( hr ) )
	{
	}
	else
	{
//-	For BSTRs I expect the address of the BSTR to be copied, but not the 
//-	BSTR.  vNewVariant should then NOT attempt to destroy the BSTR;
		*avpValue = vNewVariant;

		vNewVariant.vt = VT_EMPTY;
		vNewVariant.bstrVal = NULL;
	}

//	fclose (hFile);
	return hr;
}

HRESULT SVDOMClass::GetDOMNodeMainText (SVXML::IXMLDOMElementPtr p_oDOMElementPtr, SVBStr* p_bstrpText)
{
	HRESULT	hr = S_OK;

	long l_lFound = false;

	SVXML::IXMLDOMNodeListPtr l_oNodeListPtr;
	SVXML::IXMLDOMNodePtr l_oDOMNodePtr;

	_bstr_t bstrTempValue;

	while (1)
	{
		l_oNodeListPtr = p_oDOMElementPtr->GetchildNodes ();   
		if (l_oNodeListPtr == NULL)
		{
			hr = -1940;
			break;
		}

		do
		{
			l_oDOMNodePtr = GetDOMNodeNextText (l_oNodeListPtr, l_oDOMNodePtr);
			
			if (l_oDOMNodePtr == NULL)
			{
				break;
			}

			bstrTempValue = l_oDOMNodePtr->Gettext ();

//-		There should be conditionals here listing the exceptions
//-		(possibly white space).
			l_lFound = TRUE;

		} while ((l_oDOMNodePtr != NULL) && (l_lFound == FALSE));

		if (l_lFound == TRUE)
		{
			hr = p_bstrpText->CopyFromWChar ((WCHAR*) bstrTempValue);
			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				break;
			}
		}
		else
		{
			hr = p_bstrpText->CopyFromWChar (L"");
			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				break;
			}
		}
		break;
	}
	return hr;
}

SVXML::IXMLDOMNodePtr SVDOMClass::GetDOMNodeNextText (SVXML::IXMLDOMNodeListPtr	p_oNodeListPtr, SVXML::IXMLDOMTextPtr p_oDOMTextPtr)
{
	HRESULT hr = S_OK;

	long l_lTextFound = false;

	SVXML::IXMLDOMNodePtr l_oDOMNodePtr;
	SVXML::DOMNodeType l_oNodeType;

	while (1)
	{
		if (p_oDOMTextPtr == NULL)
		{
			hr = p_oNodeListPtr->reset ();
			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				break;
			}
		}

		do
		{
			l_oDOMNodePtr = p_oNodeListPtr->nextNode ();
			if (l_oDOMNodePtr == NULL)
			{
				break;
			}

			l_oNodeType = l_oDOMNodePtr->GetnodeType ();
			if (l_oNodeType == SVXML::NODE_TEXT)
			{
				l_lTextFound = TRUE;
			}
		} while ((l_lTextFound == FALSE) && (l_oDOMNodePtr != NULL));
		break;
	}
	return l_oDOMNodePtr;
}

HRESULT SVDOMClass::GetDOMNodeElementName (SVXML::IXMLDOMElementPtr	aDOMElementPtr, BSTR* abstrpDOMElementName)
{
	HRESULT	hr = 0;

	long l_lIsEncrypted;

	_bstr_t bstrNameAttributeTitle;

	_variant_t vName;
	_variant_t vTempName;

	SVBStr l_bstrDecryptedName;

//	FILE* hFile;

	vName.vt = VT_EMPTY;
	vName.lVal = 0;

//	hFile = _wfopen (L"c:\\DecryptLog.txt", L"at");

	while (1)
	{
		if (aDOMElementPtr == NULL)
		{
			hr = -1783;
			break;
		}

		hr = m_opEncryption->GetIsEncrypted (&l_lIsEncrypted);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		bstrNameAttributeTitle = g_wcsName;

		if (l_lIsEncrypted == TRUE)
		{
//-	All attributes are string types (from XML Parser perspective), 
//-	therefore all variants returned should be BSTRs.  It is assumed that
//-   getAttribute copies the BSTRs and does not give a reference to the 
//-   existing BSTR.
			vTempName = aDOMElementPtr->getAttribute (bstrNameAttributeTitle);
			if (vTempName.vt != VT_BSTR)
			{
				hr = -1937;
				break;
			}

//			hr = m_opEncryption->DecryptString (vTempName.bstrVal, &l_bstrDecryptedName);
			hr = m_opEncryption->DecryptNameAttribute (vTempName.bstrVal, &l_bstrDecryptedName);

			*abstrpDOMElementName = SysAllocString (l_bstrDecryptedName);
			if (*abstrpDOMElementName == NULL)
			{
				hr = -1938;
				break;
			}

//fwrite ("Element Name = \"", 16, 1, hFile); 
//fwrite ((BSTR) l_bstrDecryptedName, SysStringByteLen (l_bstrDecryptedName), 1, hFile);
//fwrite ("\"\n", 2, 1, hFile);

//fwrite ("Encrypted Element Name = \"", 26, 1, hFile); 
//fwrite (vTempName.bstrVal, SysStringByteLen (vTempName.bstrVal), 1, hFile);
//fwrite ("\"\n", 2, 1, hFile);


		}
		else
		{
//-		All attributes are string types (from XML Parser perspective), 
//-		therefore all variants returned should be BSTRs.  It is assumed that
//-		getAttribute copies the BSTRs and does not give a reference to the 
//-		existing BSTR.
			vName = aDOMElementPtr->getAttribute (bstrNameAttributeTitle);
			if (vName.vt != VT_BSTR)
			{
				hr = -1784;
				break;
			}

//-		We must explicitly NOT call the VariantClear () function so that the 
//-		BSTR does not get destroyed.
			*abstrpDOMElementName = vName.bstrVal;
			vName.Detach ();

		}
		break;
	}

	if( SEV_SUCCESS != SV_SEVERITY( hr ) )
	{
	}
	else
	{
	}

//	fclose (hFile);
	return hr;
}

HRESULT SVDOMClass::GetElementNbrOfChildren (const SVXML::IXMLDOMElementPtr aDOMElementPtr, long* alpNbrOfChildren) const
{
	HRESULT hr = 0;

	SVXML::IXMLDOMNodeListPtr oNodeListPtr;

	while (1)
	{
		oNodeListPtr = aDOMElementPtr->GetchildNodes ();   
		if (oNodeListPtr == NULL)
		{
			hr = -1794;
			break;
		}
		break;
	} // while (1)

	if( SEV_SUCCESS != SV_SEVERITY( hr ) )
	{
	}
	else
	{
		*alpNbrOfChildren = 0;
		int count = oNodeListPtr->Getlength ();
		SVXML::IXMLDOMElementPtr oElementPtr = nullptr;
		for (int i=0; i<count; ++i)
		{
			 oElementPtr = oNodeListPtr->Getitem(i);
			 if (nullptr != oElementPtr)
			 {
				 (*alpNbrOfChildren)++;
			 }
		}
	}
	return hr;
}

SVXML::IXMLDOMElementPtr SVDOMClass::GetFirstElementChild(const SVXML::IXMLDOMElementPtr aDOMElementPtr) const
{
	SVXML::IXMLDOMNodePtr nodePtr = aDOMElementPtr->GetfirstChild();
	SVXML::IXMLDOMElementPtr returnPtr = nodePtr;

	while (nullptr == returnPtr && nullptr != nodePtr)
	{
		nodePtr = nodePtr->GetnextSibling();
		returnPtr = nodePtr;
	}

	return returnPtr;
}

SVXML::IXMLDOMElementPtr SVDOMClass::GetNextElementSibling(const SVXML::IXMLDOMElementPtr aDOMElementPtr) const
{
	SVXML::IXMLDOMNodePtr nodePtr = aDOMElementPtr->GetnextSibling();
	SVXML::IXMLDOMElementPtr returnPtr = nodePtr;

	while (nullptr == returnPtr && nullptr != nodePtr)
	{
		nodePtr = nodePtr->GetnextSibling();
		returnPtr = nodePtr;
	}

	return returnPtr;
}

HRESULT SVDOMClass::GetEncryptionInterface (SVXMLEncryptionClass** p_oppEncryptionInterface)
{
	*p_oppEncryptionInterface = m_opEncryption;
	return S_OK;
}

HRESULT SVDOMClass::PreserveWhitespace(bool bPreserve)
{
	HRESULT hr = S_OK;
	if (svmDOMPtr)
	{
		VARIANT_BOOL value = (bPreserve) ? VARIANT_TRUE : VARIANT_FALSE;
		svmDOMPtr->put_preserveWhiteSpace(value);
	}
	else
	{
		hr = E_POINTER;
	}
	return hr;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVXMLLibrary\SVDom.cpp_v  $
 * 
 *    Rev 1.1   19 Dec 2014 04:44:26   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  978
 * SCR Title:  Copy and Paste a Tool within an Inspection or Between Different Inspections
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added CopyXMLTextToDOM to generate a DOM from a string
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 19:47:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Oct 2012 10:39:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code and projects to fix whitespace issues in the XML file and consolidate functionality for load and saving to this library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   28 Sep 2012 14:22:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated class to manage the perserve whitespace functionality and removed check for attached tree in copy method that does not require the tree.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Sep 2012 18:12:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library.
 * 
 * This code is the refacting of the items from the SVLibrary.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   30 Oct 2007 14:18:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Set async flag to false in initializeDom
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   19 Jun 2007 15:12:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   06 Oct 2005 11:01:32   jBrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  505
 * SCR Title:  Encrypt security file
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   The following functions were modified to provide additional information for encryption debugging.
 * 
 * SVDOMClass::CreateDOMChildElement ()
 * SVDOMClass::CreateDOMNodeElement ()
 * SVDOMClass::GetDOMNodeElementValue ()
 * SVDOMClass::GetDOMNodeElementName ()
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   19 Sep 2005 11:38:44   jBrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  505
 * SCR Title:  Encrypt security file
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   SVDOMClass was modified and enhanced to support encryption.
 * 
 * The following functions were modified:
 *  SVDOMClass::Init ()
 *  SVDOMClass::Initialize ()
 *  SVDOMClass::~SVDOMClass
 *  SVDOMClass::InitializeDOM ()
 *  SVDOMClass::CreateDOMNodeElement ()
 *  SVDOMClass::TranslateVariant ()
 *  SVDOMClass::RestoreVariant ()
 *  SVDOMClass::CreateElement ()
 *  SVDOMClass::ClearNameSpace ()
 *  SVDOMClass::GetDOMNodeElementValue ()
 *  SVDOMClass::GetDOMNodeElementName ()
 * 
 * The following functions were added:
 *  SVDOMClass::AppendCarriageReturn ()
 *  SVDOMClass::CreateNode ()
 *  SVDOMClass::GetDOMNodeMainText ()
 *  SVDOMClass::GetDOMNodeNextText ()
 *  SVDOMClass::GetEncryptionInterface ()
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Aug 2005 11:22:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   removed warnings
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Aug 2005 11:21:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   removed warnings
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 Jan 2005 07:26:46   jBrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  375
 * SCR Title:  Remodel XML usage for configuration storage and SIAC transfers.
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   MSXML DOM behavior forced a switch from placing element values in element attributes to placing them in element text.
 * 
 * Modified...
 * SVDOMClass::InitializeDOM (),
 * SVDOMClass::CreateDOMNodeElement (), and
 * SVDOMClass::GetDOMNodeElement ().
 * 
 * Also removed previously commented out code.
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   25 Oct 2004 07:50:06   jBrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  375
 * SCR Title:  Remodel XML usage for configuration storage and SIAC transfers.
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Changed
 * TranslateVariant () and 
 * RestoreVariant () 
 * to support variant types VT_R4 and VT_R8.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Oct 2004 09:30:10   jBrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  375
 * SCR Title:  Remodel XML usage for configuration storage and SIAC transfers.
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Added new classes:
 *   SVDOMClass,
 *   SVXMLClass,
 *   SVTreeClass, and
 *   SVCTreeControlClass.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
