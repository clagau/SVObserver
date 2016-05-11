//******************************************************************************
// * COPYRIGHT (c) 2001 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCommRC.cpp
// * .File Name       : $Workfile:   SVXml.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   30 Sep 2013 14:24:32  $
// ******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVXml.h"
#ifdef _DEBUG
	#include "utilities.h"
#endif
 #pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

SVXml::SVXml()
:m_szSchemaFilename(_T(""))
{
	Init();
}

SVXml::~SVXml()
{
	Reset();
	//do this only if not incorporated into COM object
	if (TRUE == m_blComInit)
	{
		CoUninitialize( );
	}
}

#ifdef _DEBUG
void SVXml::GetXmlParserError(MSXML2::IXMLDOMParseError *errorObj, CString & szError)
#else
void SVXml::GetXmlParserError(MSXML2::IXMLDOMParseError *errorObj)
#endif
{
USES_CONVERSION;
	CString szBstr;
	BSTR reasonString;
	long lErrorCode, lLinePos, lLine;
	errorObj->get_reason(&reasonString);

	AfxBSTR2CString(&szBstr,reasonString);
	//free the string
	::SysFreeString(reasonString);

	errorObj->get_errorCode(&lErrorCode);
	errorObj->get_linepos(&lLinePos);
	errorObj->get_line(&lLine);
	SVStringArray messageList;
	messageList.push_back(SVString(szBstr));
	messageList.push_back(SvUl_SF::Format(_T("%X"), lErrorCode));
	messageList.push_back(SvUl_SF::Format(_T("%d"), lLinePos));
	messageList.push_back(SvUl_SF::Format(_T("%d"), lLine));
	SvStl::MessageData Msg( m_SVException.getMessage() );
	Msg.m_MessageCode = 0;
	Msg.m_AdditionalTextId = SvOi::Tid_XmlParserError;
	Msg.m_AdditionalTextList = messageList;
	m_SVException.setMessage( Msg );
#ifdef _DEBUG
	szError = Msg.getAdditionalText().c_str();
#endif
}

void SVXml::CheckXmlParserError(HRESULT hr)
{
	CString szTemp;
	MSXML2::IXMLDOMParseError *errorObj = nullptr;

	if (hr)
	{
		m_pDoc->get_parseError(&errorObj);
#ifdef _DEBUG
		GetXmlParserError(errorObj, szTemp);
		AfxMessageBox(szTemp);	
#else
		GetXmlParserError(errorObj);
#endif
	}
	SAFERELEASE(errorObj);
}

BOOL SVXml::CreateBlankXmlDoc()
{
	HRESULT hr = S_OK; // = 0
	CString szTemp;

	SAFERELEASE(m_pDoc);

	//do this only if not incorporated into COM object
	//Initializes the COM library for use by the calling thread
//	CHECKHR(CoInitializeEx(nullptr, COINIT_MULTITHREADED));
//	hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
//	if((hr != S_OK) && (hr != S_FALSE) && (hr != RPC_E_CHANGED_MODE ))
//		{
//		CString szTemp;
//		szTemp.Format(_T("Error Initializing the COM library %d"),hr);
//		AfxMessageBox(szTemp);	
//		exit(1);
//		}

//REGDB_E_CLASSNOTREG 0x80040154
//CLASS_E_NOAGGREGATION 0x80040110
//E_NOINTERFACE 0x80004002

	// Create an empty XML document
	hr = CoCreateInstance(MSXML2::CLSID_DOMDocument30, nullptr, CLSCTX_INPROC_SERVER,
							MSXML2::IID_IXMLDOMDocument2, (void**)&m_pDoc);

	if (REGDB_E_CLASSNOTREG == hr)
	{ 
		//REGDB_E_CLASSNOTREG 
		// A specified class is not registered in the registration database. 
		// Also can indicate that the type of server you requested in the 
		// CLSCTX enumeration is not registered or the values for the server 
		// types in the registry are corrupt. 
		AfxMessageBox(_T("The XML parser 3.0 is not registered in the registration database. \r\n Please install the Microsoft XML parser 3.0."));	
		return false;
	}
	else if (0 != hr) //CLASS_E_NOAGGREGATION or E_NOINTERFACE
	{
		//CLASS_E_NOAGGREGATION 
		// This class cannot be created as part of an aggregate. 
		//E_NOINTERFACE 
		// The specified class does not implement the requested interface, 
		// or the controlling IUnknown does not expose the requested interface. 
		AfxMessageBox(_T("Error creating XML COM object"));	
		return false;
	}

	//the xml load will wait until doc is loaded before returning control to caller
    CHECKHR(m_pDoc->put_async(VARIANT_FALSE));

CleanUp:
	CheckXmlParserError(hr);
	RETURN;
}

BOOL SVXml::SaveXmlDocToFile(CString & szFilename)
{
	HRESULT hr = S_OK; // = 0

	VARIANT vName;
	vName.vt = VT_BSTR;
	V_BSTR(&vName) = szFilename.AllocSysString();

	//save the xml doc
	CHECKHR(m_pDoc->save(vName));

CleanUp:
	//free the string 
	::SysFreeString(vName.bstrVal);
	CheckXmlParserError(hr);
	RETURN;
}

BOOL SVXml::LoadSchemaCollection()
{
USES_CONVERSION;
	HRESULT hr = S_OK; // = 0
	CHECKXMLDOC;
	VARIANT varValue;
	CString szName, szTemp;

	try
	{	
		CFile schemaFile;
		DWORD dwFileLength = 0;
		BYTE * pBuf = nullptr;
		// open the file

		if (schemaFile.Open(m_szSchemaFilename, CFile::modeRead))
		{
			// determine the file length
			dwFileLength = (DWORD)schemaFile.GetLength();
			//create a buffer to hold the file data
				pBuf = (BYTE *)malloc(dwFileLength+1);
			//transfer the file data into the buffer
			schemaFile.Read(pBuf,dwFileLength);
			// close the file
			schemaFile.Close();
			//add the terminating null
			pBuf[dwFileLength] = 0;
		}
		else
		{ //error opening file
			return false;
		}

		_bstr_t bstrBuf((char*)pBuf);
		GetQuotedStringAfter(bstrBuf,_T("name="),szName);

		szTemp = _T("x-schema:") + szName;

		//set the schema namespace BSTR
		m_bstrSchemaNamespace = SysAllocString(T2W(szTemp.GetBuffer(szTemp.GetLength())));
		free(pBuf);

		if (nullptr == m_pSchemaCollection)
		{
			CHECKHR(CoCreateInstance(MSXML2::CLSID_XMLSchemaCache30, NULL, CLSCTX_SERVER, 
				MSXML2::IID_IXMLDOMSchemaCollection, 
				(LPVOID*)(&m_pSchemaCollection)));
		}

		if (m_pSchemaCollection)
		{
			//check for file
			CFileStatus rStatus;
			if (!CFile::GetStatus(m_szSchemaFilename,rStatus )) return false;

			CHECKHR(m_pSchemaCollection->add(m_bstrSchemaNamespace,
				_variant_t(LPTSTR(m_szSchemaFilename.GetBuffer(m_szSchemaFilename.GetLength())) )));

			varValue.vt = VT_DISPATCH;
			varValue.pdispVal = m_pSchemaCollection;
			CHECKHR(m_pDoc->putref_schemas(varValue));
		}
	}
	catch (_com_error &e)
	{
		e;	// remove unreference variable warning
#ifdef _DEBUG
		CString szMsg,szMsg1;
		szMsg.Format(_T("Code = %08lx\nCode meaning = %s\nSource = %s\nError Description = %s\n")
		  ,e.Error()
		  ,e.ErrorMessage()
		  ,e.Source()
		  ,e.Description()
		  );
		AfxMessageBox(szMsg1);
#endif
		return false;
	}
	catch( CFileException E )
	{
		return false;
	}
	catch (...)
	{
#ifdef _DEBUG
		CString szMsg,szMsg1;
		DWORD dwError = GetLastSystemErrorText(szMsg);
		if (dwError)
		{
			AfxMessageBox(szMsg1);
		}
		else
		{
			CString szTemp;
			MSXML2::IXMLDOMParseError *errorObj = nullptr;	
			m_pDoc->get_parseError(&errorObj);
			GetXmlParserError(errorObj, szTemp);
			szMsg1.Format(_T("SVXml::LoadSchemaCollection5, %08X-%s\n"),dwError,szTemp);
			AfxMessageBox(szMsg1);
		}
#endif
		return false;
	}
      
CleanUp:
	CheckXmlParserError(hr);
	RETURN;
}

BOOL SVXml::IsXmlDocValid()
{
	HRESULT hr = S_OK; // = 0
	long lReadyState;
	MSXML2::IXMLDOMParseError *errorObj = nullptr;

	//check the ready state
	CHECKHR(m_pDoc->get_readyState(&lReadyState));

	//validate the doc
	if (4 == lReadyState)//LOADING = 1,LOADED = 2,INTERACTIVE = 3,COMPLETED = 4 
	{
		CString szTemp;
		if( S_OK != m_pDoc->validate(&errorObj) )
		{
#ifdef _DEBUG
			GetXmlParserError(errorObj, szTemp);
			AfxMessageBox(szTemp);	
#else
			GetXmlParserError(errorObj);
#endif
			goto CleanUp;
		}
	}

CleanUp:
	CheckXmlParserError(hr);
	SAFERELEASE(errorObj);
	RETURN;
}

HRESULT SVXml::Load(TCHAR* pUrl)
{
USES_CONVERSION;
	HRESULT hr = S_OK; // = 0
	VARIANT varUrl;
	VARIANT_BOOL varBool;

	varUrl.vt = VT_BSTR;
	V_BSTR(&varUrl) = SysAllocString(T2W(pUrl));

	CHECKHR(m_pDoc->load(varUrl,&varBool));

CleanUp:
	CheckXmlParserError(hr);
	SysFreeString(varUrl.bstrVal);
	RETURN;
}

void SVXml::Reset()
{
	if (m_pSchemaCollection)
	{
		m_pSchemaCollection->remove(m_bstrSchemaNamespace);
		ULONG ulRefCount = m_pSchemaCollection->Release();
		m_pSchemaCollection = nullptr;
	}
	
	if (m_bstrSchemaNamespace)
	{
		::SysFreeString(m_bstrSchemaNamespace);
		m_bstrSchemaNamespace = nullptr;
	}

	if (m_pDoc)
	{
		ULONG ulRefCount = m_pDoc->Release();
		m_pDoc = nullptr;
	}
	
	POSITION pos;
	Element * pElement = nullptr;
	Attribute * pAttribute = nullptr;

	pos = ElementList.GetHeadPosition();
	while(nullptr != pos)
	{
		pElement = ElementList.GetNext(pos);
		delete pElement;
	}
	ElementList.RemoveAll();

	pos = AttributeList.GetHeadPosition();
	while(nullptr != pos)
	{
		pAttribute = AttributeList.GetNext(pos);
		delete pAttribute;
	}
	AttributeList.RemoveAll();

	if (m_pRootElement)
	{
		delete m_pRootElement;
	}
}

void SVXml::Init()
{
	HRESULT hr = S_OK; // = 0

	//do this only if not incorporated into COM object
	//Initializes the COM library for use by the calling thread
	hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if( (S_OK != hr) && (S_FALSE != hr) && (RPC_E_CHANGED_MODE != hr))
	{
		CString szTemp;
		szTemp.Format(_T("Error Initializing the COM library %d"),hr);
		AfxMessageBox(szTemp);	
		m_blComInit = FALSE;
		exit(1);
	}
	else if (S_OK == hr)
	{
		//COM has been initialized
		m_blComInit = true;
	}
	else if (S_FALSE == hr)
	{
		//COM already initialize
		CoUninitialize( );
		m_blComInit = false;
	}
	else if (RPC_E_CHANGED_MODE == hr)
	{
		m_blComInit = false;
	}
	else
	{
		m_blComInit = false;
	}

	m_pDoc = nullptr;
	m_pSchemaCollection=nullptr;
	m_bstrSchemaNamespace = nullptr;
	m_pRootElement = nullptr;
}

BOOL SVXml::SetAttribute(MSXML2::IXMLDOMElement * m_pElement, LPTSTR szName, VARIANT vValue)
{
USES_CONVERSION;
	HRESULT hr = S_OK; // = 0
	BSTR bstrName;

	bstrName = SysAllocString(T2W(szName));
	//set the attribute with this name
	//set the value
	CHECKHR(m_pElement->setAttribute(bstrName,vValue));

CleanUp:
	CheckXmlParserError(hr);
	//Deallocates a string
	SysFreeString(bstrName);
	RETURN;
}

//this will always attempt to load the schema defined in the m_szSchemaFilename
//memeber variable unless this variable is empty.
BOOL SVXml::LoadSchema( LPCTSTR p_szSchema )
{
	USES_CONVERSION;

	_TCHAR * pTemp;
	_TCHAR pFilename[MAX_PATH];
	static _TCHAR pStaticFilename[L_tmpnam];
	CString szSchema( p_szSchema );
	int nLength;
	BOOL blReturn = FALSE;

	if( m_szSchemaFilename.IsEmpty() )
	{
		if( nullptr != p_szSchema )
		{
			nLength = szSchema.GetLength();

			//get the current working directory
			_tgetcwd(pFilename, MAX_PATH);
			//create a file name
			pTemp =  _ttmpnam(pStaticFilename);
			//create the full path
			_tcscat(pFilename,pTemp);

			CFile SchemaFile;
			SchemaFile.Open(pFilename,CFile::modeCreate | CFile::modeWrite);
			SchemaFile.Write(T2A(szSchema.GetBuffer(nLength)),nLength * sizeof (char) );
			SchemaFile.Close();
			//save the file

			m_szSchemaFilename = CString(pFilename);

			blReturn = LoadSchemaCollection();

			m_szSchemaFilename = "";

			//delete the temporary file
			CFile::Remove( pFilename );
		}
		else
		{
			//no resource given
			blReturn = false;
		}
	}
	else
	{
		blReturn = LoadSchemaCollection();
	}
	return blReturn;
}

BOOL SVXml::InitXml( LPCTSTR p_szSchema )
{
	if(!CreateBlankXmlDoc()) return false;

	if( !LoadSchema( p_szSchema ) ) return false;

	if(!CreateXmlTree()) return false;
	return TRUE;
}

MSXML2::IXMLDOMNodePtr SVXml::GetChildElement(MSXML2::IXMLDOMNode * pNode, TCHAR * pName)
{
	HRESULT hr = S_OK;
	MSXML2::IXMLDOMNodePtr pChildNode = nullptr;
	MSXML2::IXMLDOMNodePtr pTempChildNode = nullptr;
	BSTR bstrText;

	CString szXmlText, szName;
	CString szSearchName(pName);
	
	//are there any children ?
	CHECKHR(pNode->get_firstChild(&pChildNode));

	//walk throught the siblings looking for the name
	while (nullptr != pChildNode)
	{
		// find the name
		pChildNode->get_xml(&bstrText);

		// get the name
		GetQuotedStringAfter(bstrText,_T("name="),szName);
		::SysFreeString(bstrText);

		if (szSearchName == szName)
		{
			pTempChildNode = pChildNode;
			break;
		}
		//else, onto the next child
		pTempChildNode = pChildNode;
		CHECKHR(pTempChildNode->get_nextSibling(&pChildNode));
	}

CleanUp:
	::SysFreeString(bstrText);
	return pTempChildNode;
}

BOOL SVXml::CreateXmlTree()
{
	HRESULT hr = S_OK;
	CString szName;
	MSXML2::IXMLDOMNode * pSchemaNode = nullptr;
	
	CHECKHR(m_pSchemaCollection->get(m_bstrSchemaNamespace,&pSchemaNode));

	InitXmlTree(pSchemaNode);

CleanUp:
	CheckXmlParserError(hr);
	SAFERELEASE(pSchemaNode);
	RETURN;
}

long SVXml::GetChildNodes(MSXML2::IXMLDOMNode * pNode, MSXML2::IXMLDOMNodeList ** ppNodeList)
{
	HRESULT hr = S_OK; // = 0

	long lNumberOfChildren = 0;	
	if (pNode)
	{
		CHECKHR(pNode->get_childNodes(ppNodeList));   
		CHECKHR(((MSXML2::IXMLDOMNodeList *)*ppNodeList)->get_length(&lNumberOfChildren));
	}

CleanUp:
	CheckXmlParserError(hr);
	return lNumberOfChildren;
}

void SVXml::GetQuotedStringAfter(BSTR bstrString,CString szSearch, CString & szFound)
{
	CString szString;

	AfxBSTR2CString(&szString,bstrString);
	szString.ReleaseBuffer();

	int pos1, pos2, pos3;
	pos1 = pos2 = pos3 = 0;

	pos1 = szString.Find(szSearch);
	if (-1 == pos1)
	{
		szFound = "";
		return;
	}
	pos2 = szString.Find(_T("\""),pos1 + szSearch.GetLength());
	pos3 = szString.Find(_T("\""),pos2 + 1);

	szFound = szString.Mid(pos2+1, pos3-pos2-1);
}

BOOL SVXml::CreateXmlElements(Element * pElement, MSXML2::IXMLDOMElement * pXmlElement)
{
USES_CONVERSION;
	HRESULT hr = S_OK; // = 0
	CHECKXMLDOC;
	MSXML2::IXMLDOMElement * pNextXmlElement = nullptr;
	MSXML2::IXMLDOMNamedNodeMap * attributeMap = nullptr;
	LPTSTR pszTemp = nullptr;
#ifdef _DEBUG
	BSTR namespaceURI; 
#endif
	int index;

	try
	{
		hr = pXmlElement->get_attributes(&attributeMap);

		for (index=0; index<pElement->HasChildAttributes(); index++)
		{
			MSXML2::IXMLDOMNode * 	pIXMLDOMNode = nullptr;
			//get the attribute with tis name
			hr = attributeMap->getNamedItem(T2W(pElement->ppAttributes[index]->GetName()), &pIXMLDOMNode);

			//if NULL attribute is not in the list, so create the attribute
			if (nullptr == pIXMLDOMNode)
			{
				CreateDOMNode(m_pDoc,MSXML2::NODE_ATTRIBUTE, _bstr_t(pElement->ppAttributes[index]->GetName()), &pIXMLDOMNode);

#ifdef _DEBUG
				VARIANT dataTypeName;
				CHECKHR(pIXMLDOMNode->get_dataType(&dataTypeName));
#endif
				//set the the dt: type
				BSTR bstrType = SysAllocString(T2W(pElement->ppAttributes[index]->GetType()));
				CHECKHR(pIXMLDOMNode->put_dataType(bstrType));
				SysFreeString(bstrType);
#ifdef _DEBUG
				CHECKHR(pIXMLDOMNode->get_dataType(&dataTypeName));
				hr = pIXMLDOMNode->get_namespaceURI(&namespaceURI); 
#endif
				//set the value
				CHECKHR(pIXMLDOMNode->put_nodeTypedValue(pElement->ppAttributes[index]->GetData()));
				// add the attribute to the node
				CHECKHR(attributeMap->setNamedItem(pIXMLDOMNode, nullptr)); 
			}
			else
			{
				SetAttribute(pXmlElement, pElement->ppAttributes[index]->GetName(), pElement->ppAttributes[index]->GetData());
			}
			SAFERELEASE(pIXMLDOMNode);
		}

		SAFERELEASE(attributeMap);
		//iterate through the elements
		for (index=0; index<pElement->HasChildElements(); index++)
		{
			//create the element
			CHECKHR(m_pDoc->createElement(_bstr_t(pElement->ppElements[index]->GetName()),&pNextXmlElement));

			//take care of the kids
			CreateXmlElements(pElement->ppElements[index],pNextXmlElement);

			CHECKHR(pXmlElement->appendChild(pNextXmlElement, nullptr));

			SAFERELEASE(pNextXmlElement);
		}
	}
   catch (_com_error &e)
   {
      _tprintf(_T("Error:\n"));
      _tprintf(_T("Code = %08lx\n"), e.Error());
      _tprintf(_T("Code meaning = %s\n"), (char*) e.ErrorMessage());
      _tprintf(_T("Source = %s\n"), (char*) e.Source());
      _tprintf(_T("Error Description = %s\n"), (char*) e.Description());
   }
CleanUp:
	CheckXmlParserError(hr);
	SAFERELEASE(pNextXmlElement);
	SAFERELEASE(attributeMap);
	RETURN;
}

BOOL SVXml:: GetXmlDoc(BSTR * bstrDoc)
{
	HRESULT hr = S_OK; // = 0
	CHECKHR(m_pDoc->get_xml(bstrDoc));
CleanUp:
	CheckXmlParserError(hr);
	RETURN;
}

BOOL SVXml::CreateDOMNode(MSXML2::IXMLDOMDocument2* pDoc,int type, BSTR bstrName,MSXML2::IXMLDOMNode ** pNode)
{
	HRESULT hr = S_OK; // = 0
    VARIANT vtype;

    vtype.vt = VT_I4;
    V_I4(&vtype) = (int)type;

	CHECKHR(pDoc->createNode(vtype, bstrName, m_bstrSchemaNamespace, pNode));

CleanUp:
	CheckXmlParserError(hr);
	RETURN;
}

BOOL SVXml::CreateXMLDocTree()
{
USES_CONVERSION;
	HRESULT hr = S_OK; // = 0
	CHECKXMLDOC;
	VARIANT vvalue;
	VARIANT vtype;
	MSXML2::IXMLDOMProcessingInstruction *pIXMLDOMProcessingInstruction=nullptr;
	MSXML2::IXMLDOMNode *pIXMLDOMNode = nullptr;
	MSXML2::IXMLDOMElement * pXmlElement;
	LPTSTR pszTemp = nullptr;
	MSXML2::IXMLDOMNodePtr pChildNode = nullptr;
	
	CHECKHR(m_pDoc->createProcessingInstruction(T2W(_T("xml")),
		T2W(_T("version='1.0'")), &pIXMLDOMProcessingInstruction));

	CHECKHR(m_pDoc->appendChild(pIXMLDOMProcessingInstruction, &pIXMLDOMNode));

	CHECKHR(m_pDoc->createElement(T2W(m_pRootElement->GetName()),&pXmlElement));

	//set the attribute with its name
	vvalue.vt = VT_BSTR;
	V_BSTR(&vvalue) = ::SysAllocString(m_bstrSchemaNamespace);
	SetAttribute(pXmlElement,_T("xmlns"),vvalue);
	//Deallocates a string
	SysFreeString(vvalue.bstrVal);

	//now walk the tree set attributes and creating elements as required
	CreateXmlElements(m_pRootElement,pXmlElement);
	
	//append the tree to the doc
	CHECKHR(m_pDoc->appendChild(pXmlElement, nullptr));
							  
CleanUp:
	CheckXmlParserError(hr);

	//Deallocates the string if not already 
	VariantClear(&vvalue);
	VariantClear(&vtype);

	SAFERELEASE(pIXMLDOMProcessingInstruction);
	SAFERELEASE(pIXMLDOMNode);
	SAFERELEASE(pXmlElement);

	RETURN;
}

////////////////////////////////////////////////////////////////////////
// helper functions
////////////////////////////////////////////////////////////////////////
Element * SVXml::GetTreeRoot()
{
	return m_pRootElement;
}

BOOL SVXml::SaveAndLoad(BOOL blValidate /*= FALSE */)
{
	HRESULT hr = S_OK; // = 0
	STATSTG statstg;
    LARGE_INTEGER li = {0, 0};
	IStream *pStm = nullptr;
    IPersistStreamInit *pPSI = nullptr;

	//convert this XML doc to an IStream object
	CHECKHR(m_pDoc->QueryInterface(IID_IStream, (void **)&pStm));

	//unknown why we need to call Stat, if not call no root element error is seen on load ???
	// get the status of the IStream 
	CHECKHR(pStm->Stat(&statstg,STATFLAG_NONAME));

	//load the xml data back into doc so that validation through schema is checked
    // Reset the stream back to the beginning
    CHECKHR(pStm->Seek(li, STREAM_SEEK_SET, nullptr));

	// Now, load the document from the stream
    CHECKHR(m_pDoc->QueryInterface(IID_IPersistStreamInit, (void **)&pPSI));
    CHECKHR(pPSI->Load(pStm));

	SAFERELEASE(pStm);

	if(TRUE == blValidate)
	{ //check for valid initial xml cmd doc
		if (!IsXmlDocValid()) goto CleanUp2;
	}
CleanUp:
	CheckXmlParserError(hr);
	
CleanUp2:
	SAFERELEASE(pStm);
	SAFERELEASE(pPSI);
	RETURN;
}

BOOL SVXml::CreateXmlDocFromTreeObj()
{
	//create the XML cmd doc
	if(!CreateXMLDocTree()) return false;
			
	if(!SaveAndLoad(TRUE) )return false;

	if(!SetDataValues()) return false;

	return true;
}

BOOL SVXml::CreateTreeObjFromXmlDoc(BSTR* bstrXML)
{
	HRESULT hr = S_OK; // = 0
	VARIANT_BOOL isSuccessful;

	//create the XML cmd doc
	CHECKHR(m_pDoc->loadXML(*bstrXML,&isSuccessful));

	if (!GetDataValues()) return false;

	return true;
CleanUp:
	CheckXmlParserError(hr);
	RETURN;
}

BOOL SVXml::PutXmlCmdDocInBuf(BYTE ** ppBuf, unsigned long * cBufLen)
{
	HRESULT hr = S_OK; // = 0
	STATSTG statstg;
    LARGE_INTEGER li = {0, 0};
	IStream *pStm = nullptr;
    IPersistStreamInit *pPSI = nullptr;

	*cBufLen = 0;

	//check for valid xml cmd doc
	IsXmlDocValid();

	//convert this XML doc to an IStream object
	CHECKHR(m_pDoc->QueryInterface(IID_IStream, (void **)&pStm));

	CHECKHR(pStm->Stat(&statstg,STATFLAG_NONAME));

	// get the status of the IStream 
	CHECKHR(pStm->Stat(&statstg,STATFLAG_NONAME));

	//cannot reallocate enough memory !
	if(statstg.cbSize.HighPart)goto CleanUp2;
	
	*ppBuf = (BYTE*)realloc(*ppBuf,statstg.cbSize.LowPart);
	
	CHECKHR(pStm->Read(
	  *ppBuf,					//Pointer to buffer into which the stream is read
	  statstg.cbSize.LowPart,	//Specifies the number of bytes to read
	  cBufLen					//Pointer to location that contains actual
								// number of bytes read
					));

CleanUp:
	CheckXmlParserError(hr);
	
CleanUp2:
	SAFERELEASE(pStm);
	SAFERELEASE(pPSI);
	RETURN;
}

BOOL SVXml::SetDataValues()
{
	HRESULT hr = S_OK; // = 0
	MSXML2::IXMLDOMElement * pRootElement = nullptr;

	//get the Xml document root element
	CHECKHR(m_pDoc->get_documentElement(&pRootElement));

	//set the values
	if (!SetXmlElementValues(GetTreeRoot())) { hr = S_FALSE; goto CleanUp2; }

CleanUp:
	CheckXmlParserError(hr);
CleanUp2:
	SAFERELEASE(pRootElement);
	RETURN;
}

BOOL SVXml::SetXmlElementValues(Element * pElement)
{
	HRESULT hr = S_OK; // = 0
	CHECKXMLDOC;
	MSXML2::IXMLDOMElement * pNextXmlElement = nullptr;
	MSXML2::IXMLDOMNodeList * resultList = nullptr;
	
	int index = 0;
	long length = 0;

	if (nullptr == pElement) {hr = S_FALSE; goto CleanUp2;}

	CHECKHR(m_pDoc->getElementsByTagName(_bstr_t(pElement->GetName()),&resultList));

	if (nullptr == resultList) {hr = S_FALSE; goto CleanUp2;}

	//determine length of found elements
	CHECKHR(resultList->get_length(&length));
	
	//return if no elements found
	if (0 == length) {hr = S_FALSE; goto CleanUp2;}

	else if (1 != length)
	{//determine the correct element from the list
		for (int x=0; x<length;x++)
		{
			CHECKHR(resultList->get_item(x,(MSXML2::IXMLDOMNode **)&pNextXmlElement));
			if(CheckAncestors(pElement,pNextXmlElement))break;
			SAFERELEASE(pNextXmlElement);
			pNextXmlElement = nullptr;
		}
	}
	else 
	{//must have found 1 xml doc element
		CHECKHR(resultList->get_item(0,(MSXML2::IXMLDOMNode **)&pNextXmlElement));
		if (!CheckAncestors(pElement,pNextXmlElement)) {hr = S_FALSE; goto CleanUp2;}
	}

	//retrun if no Xml Doc element found
	if (nullptr == pNextXmlElement) {hr = S_FALSE; goto CleanUp2;}
		
	//set the value
	if (VT_NULL != pElement->GetData().vt)
	{
		CHECKHR(pNextXmlElement->put_nodeTypedValue(pElement->GetData()));
	}

	// take care of the kids
	for (index=0; index<pElement->HasChildElements(); index++)
	{
		if (!SetXmlElementValues(pElement->ppElements[index])) {hr = S_FALSE; goto CleanUp2;}
		else {hr = S_OK;}
	}
CleanUp:
	CheckXmlParserError(hr);
CleanUp2:
	SAFERELEASE(pNextXmlElement);
	SAFERELEASE(resultList);
	RETURN;
}

BOOL SVXml::CheckAncestors(Element * pElement,MSXML2::IXMLDOMElement * pXmlElement)
{
USES_CONVERSION;
	HRESULT hr = S_OK; // = 0
	MSXML2::IXMLDOMNode * parent = nullptr;
	MSXML2::DOMNodeType type;

	Element * pParent = pElement->GetParent();
	CHECKHR(pXmlElement->get_parentNode(&parent));
	CHECKHR(parent->get_nodeType(&type));
	if (type == MSXML2::NODE_DOCUMENT)
	{
		SAFERELEASE(parent);
		parent = nullptr;
	}

	if ((nullptr == pParent) && (nullptr == parent))
	{
		hr = S_OK;
		goto CleanUp2;
	}
	else if((nullptr != pParent) && (nullptr == parent))
	{
		hr = S_FALSE;
		goto CleanUp2;
	}
	else if((nullptr == pParent) && (nullptr != parent))
	{
		hr = S_FALSE;
		goto CleanUp2;
	}
	else
	{
		BSTR bstrNodeName;
		CHECKHR(pXmlElement->get_nodeName(&bstrNodeName));
		if (!_tcsicmp(pElement->GetName(), W2T(bstrNodeName)))
		{ //we have a match
			if (!CheckAncestors(pParent,(MSXML2::IXMLDOMElement *)parent))
			{
				hr = S_FALSE;
				goto CleanUp2;
			}
		}
		else
		{
			hr = S_FALSE;
			goto CleanUp2;
		}
	}

CleanUp:
	CheckXmlParserError(hr);
CleanUp2:
	SAFERELEASE(parent);
	RETURN;
}


BOOL SVXml::GetDataValues()
{
	HRESULT hr = S_OK; // = 0
	MSXML2::IXMLDOMElement * pDomElement = nullptr;
	Element * pRootElement = nullptr;

	pRootElement = GetTreeRoot();
	//error, return if NULL
	if (nullptr == pRootElement) {hr = S_FALSE; goto CleanUp2;}

	//reset the tree content
	pRootElement->DeleteContent();

	CHECKHR(m_pDoc->get_documentElement(&pDomElement));
	if (nullptr == pDomElement) {hr = S_FALSE;goto CleanUp2;}

	if (!GetXmlElementValues(pDomElement,pRootElement)) {hr = S_FALSE; goto CleanUp2;}

CleanUp:
	CheckXmlParserError(hr);
CleanUp2:
	SAFERELEASE(pDomElement);
	RETURN;
}

BOOL SVXml::GetXmlElementValues(MSXML2::IXMLDOMElement * pDomElement,Element * pElement)
{
USES_CONVERSION;
	HRESULT hr = S_OK; // = 0
	CHECKXMLDOC;
	MSXML2::IXMLDOMNamedNodeMap * attributeMap = nullptr;
	MSXML2::IXMLDOMNode * children = nullptr;
	MSXML2::IXMLDOMNode * nextchild = nullptr;
	long listLength = 0;
	long index = 0;
	BSTR bstrNodeName = ::SysAllocStringByteLen(nullptr, 0);

	if ((nullptr == pDomElement) || (nullptr == pElement)) {hr = S_FALSE;goto CleanUp2;}

	//set the data
	VARIANT vTemp;
	CHECKHR(pDomElement->get_nodeTypedValue(&vTemp));
	//set the data
	pElement->SetData(vTemp);
	if( S_OK != VariantClear(&vTemp) ) { hr = S_FALSE; goto CleanUp2; }

	
	//take care of any attributes
	CHECKHR(pDomElement->get_attributes(&attributeMap));
	if (nullptr != attributeMap)
	{
		CHECKHR(attributeMap->get_length(&listLength));

		for (index=0; index<listLength; index++)
		{
			MSXML2::IXMLDOMNode * 	pIXMLDOMNode = nullptr;
			//get the attribute
			CHECKHR(attributeMap->get_item(index, &pIXMLDOMNode));

			//if NULL, error
			if (nullptr == pIXMLDOMNode) {hr = S_FALSE; goto CleanUp2;}
			else
			{
				//get the name
				pIXMLDOMNode->get_nodeName(&bstrNodeName);
				//create the child attribute
				Attribute * pAttr = GetAttributeFromList(W2T(bstrNodeName));
				if (nullptr != pAttr)
				{
					//get the data
					VARIANT vTemp;
					CHECKHR(pIXMLDOMNode->get_nodeTypedValue(&vTemp));
					//set the data
					pAttr->SetData(vTemp);
					//add to parent
					pElement->AddChildAttribute(pAttr);
				}
				if( S_OK != VariantClear(&vTemp) ) { hr = S_FALSE; goto CleanUp2; }
			}
			SysFreeString(bstrNodeName);
			SAFERELEASE(pIXMLDOMNode);
		}
	}

	// take care of the kids
	pDomElement->get_firstChild(&children);
	while(nullptr != children)
	{
		//get the name
		children->get_nodeName(&bstrNodeName);
		//create the child element
		Element * pElem = GetElementFromList(W2T(bstrNodeName));
		if (nullptr != pElem)
		{
			//add to parent
			pElement->AddChildElement(pElem);
			GetXmlElementValues((MSXML2::IXMLDOMElement *)children,pElem);
		}
		children->get_nextSibling(&nextchild);
		SAFERELEASE(children);
		children = nextchild;
		pElem = nullptr;
	}

CleanUp:
	CheckXmlParserError(hr);
CleanUp2:
	if (nullptr != bstrNodeName) SysFreeString(bstrNodeName);
	SAFERELEASE(attributeMap);
	SAFERELEASE(nextchild);
	SAFERELEASE(children);
	RETURN;
}

BOOL SVXml::GetXmlElementValues(Element * pElement)
{
USES_CONVERSION;
	HRESULT hr = S_OK; // = 0
	CHECKXMLDOC;
	MSXML2::IXMLDOMElement * pNextXmlElement = nullptr;
	MSXML2::IXMLDOMNodeList * resultList = nullptr;
	MSXML2::IXMLDOMNamedNodeMap * attributeMap = nullptr;
	
	int index = 0;
	long length = 0;

	if (nullptr == pElement) {hr = S_FALSE; goto CleanUp2;}

	CHECKHR(m_pDoc->getElementsByTagName(_bstr_t(pElement->GetName()), &resultList));

	if (nullptr == resultList) {hr = S_FALSE; goto CleanUp2;}

	//determine length of found elements
	CHECKHR(resultList->get_length(&length));
	
	//return if no elements found
	if (0 == length) {hr = S_FALSE; goto CleanUp2;}

	else if (1 != length)
	{//determine the correct element from the list
		for (int x=0; x<length;x++)
		{
			CHECKHR(resultList->get_item(x,(MSXML2::IXMLDOMNode **)&pNextXmlElement));
			if (CheckAncestors(pElement, pNextXmlElement)) break;
			SAFERELEASE(pNextXmlElement);
			pNextXmlElement = nullptr;
		}
	}
	else 
	{//must have found 1 xml doc element
		CHECKHR(resultList->get_item(0,(MSXML2::IXMLDOMNode **)&pNextXmlElement));
		if (!CheckAncestors(pElement,pNextXmlElement)) {hr = S_FALSE; goto CleanUp2;}
	}

	//return if no Xml Doc element found
	if (nullptr == pNextXmlElement) {hr = S_FALSE; goto CleanUp2;}
		
	//get the value
	if (VT_NULL != pElement->GetData().vt )
	{
		VARIANT vTemp;
		CHECKHR(pNextXmlElement->get_nodeTypedValue(&vTemp));
		pElement->SetData(vTemp);
		if( S_OK != VariantClear(&vTemp) ) { hr = S_FALSE; goto CleanUp2; }
	}

	//take care of any attributes
	CHECKHR(pNextXmlElement->get_attributes(&attributeMap));

	for (index=0; index<pElement->HasChildAttributes(); index++)
	{
		MSXML2::IXMLDOMNode * 	pIXMLDOMNode = nullptr;
		//get the attribute with tis name
		hr = attributeMap->getNamedItem(T2W(pElement->ppAttributes[index]->GetName()), &pIXMLDOMNode);

		//if NULL attribute is not in the list, so create the attribute
		if (nullptr == pIXMLDOMNode) {hr = S_FALSE; goto CleanUp2;}
		else
		{
			//get the value
			if (VT_NULL != pElement->ppAttributes[index]->GetData().vt)
			{
				VARIANT vTemp;
				CHECKHR(pIXMLDOMNode->get_nodeTypedValue(&vTemp));
				pElement->ppAttributes[index]->SetData(vTemp);
				if( S_OK != VariantClear(&vTemp) ) { hr = S_FALSE; goto CleanUp2; }
			}
		}
		SAFERELEASE(pIXMLDOMNode);
	}

	// take care of the kids
	for (index=0; index<pElement->HasChildElements(); index++)
	{
		if(!GetXmlElementValues(pElement->ppElements[index])) {hr = S_FALSE; goto CleanUp2;}
		else{hr = S_OK;}
	}

CleanUp:
	CheckXmlParserError(hr);
CleanUp2:
	SAFERELEASE(attributeMap);
	SAFERELEASE(pNextXmlElement);
	SAFERELEASE(resultList);
	RETURN;
}

// ..........................................................
// Functions used to create a list of elements and a list or attributes
// define in the schema
// ..........................................................

void SVXml::AddElementToList(LPTSTR lpName, LPTSTR lpType)
{
	CString szName, szType;
	
	Element * lpElement = new Element;

	// set the name
	lpElement->SetName(lpName);

	//get and set the data type
	lpElement->SetType(lpType);

	//set the element type
	lpElement->SetAsElement();

	// add the element to the list
	ElementList.AddTail(lpElement);
}

void SVXml::AddAttributeToList(LPTSTR lpName, LPTSTR lpType)
{
	CString szName, szType;
	
	Attribute * lpAttribute = new Attribute;

	// set the name
	lpAttribute->SetName(lpName);

	//set the data type
	lpAttribute->SetType(lpType);

	//set the attribute type
	lpAttribute->SetAsAttribute();

	// add the element to the list
	AttributeList.AddTail(lpAttribute);
}

Element * SVXml::GetElementFromList(LPTSTR lpName)
{
	//calling function or process to free returned element
	Element * pElement;
	POSITION pos = ElementList.GetHeadPosition();
	for (int i=0;i < ElementList.GetCount();i++)
	{
		pElement = ElementList.GetNext(pos);
		if (0 == _tcsicmp(pElement->GetName(),lpName))
		{
			// make a copy using copy constructor 
			Element * pReturnedElement = new Element(*pElement);
			return pReturnedElement;
		}
	}
	return nullptr;
}

Attribute * SVXml::GetAttributeFromList(LPTSTR lpName)
{
	//calling function or process to free returned element
	Attribute * pAttribute;
	POSITION pos = AttributeList.GetHeadPosition();
	for (int i=0;i < AttributeList.GetCount();i++)
	{
		pAttribute = AttributeList.GetNext(pos);
		if(0 == _tcsicmp(pAttribute->GetName(),lpName))
		{
			// make a copy using copy constructor 
			Attribute * pReturnedAttribute = new Attribute(*pAttribute);
			return pReturnedAttribute;
		}
	}
	return nullptr;
}

Element * SVXml::GetRootFromList()
{
	//the root element is always the last Element put into the element list
	POSITION pos = ElementList.GetTailPosition();
	//return a copy
	Element * pElement = new Element(*(ElementList.GetAt(pos)));
	return pElement;	
}

BOOL SVXml::InitXmlTree(void* pSchemaNode)
{
	HRESULT hr = S_OK;
	MSXML2::IXMLDOMNode* pNode = (MSXML2::IXMLDOMNode*)pSchemaNode;

	//clear the lists
	ElementList.RemoveAll();
	AttributeList.RemoveAll();
	
	//add all the schema defined Elements and Attributes to the lists

	if (!AddChildrenToLists(pNode)) return false;

	//now walk the schema and build the xml tree using the previously define 
	// Elements and Attributes

	//we are getting a COPY of the root element from the list
	m_pRootElement = GetRootFromList();

	if (m_pRootElement)
	{
		AddChildren(pNode,m_pRootElement);
		return true;
	}
	return false;
}

BOOL SVXml::AddChildren(MSXML2::IXMLDOMNode* pSchemaNode, Element * pParent)
{
	HRESULT hr = S_OK;
	MSXML2::IXMLDOMNodeList * childList = nullptr;
	MSXML2::IXMLDOMNode * listItem = nullptr;			
	MSXML2::IXMLDOMNodePtr resultNode = nullptr;
	long lNumberOfChildren;
	CString szName, szXmlText, szNodeName;
	BSTR bstrName, bstrText;

	//find the element in the schema
	resultNode = GetChildElement(pSchemaNode,pParent->GetName());

	if (resultNode)
	{
		//we found the root element in the schema, now walk the children 
		//and add the children to the root element from a copy of the 
		//children in the lists
		lNumberOfChildren = GetChildNodes(resultNode,&childList);
		for (long x = 0; x < lNumberOfChildren; x++)
		{
			CHECKHR(childList->get_item(x, &listItem));			
			// determine type of element "ElementType" or "AttributeType"
			CHECKHR(listItem->get_nodeName(&bstrName));
			//convert BSTR to Cstring

			AfxBSTR2CString(&szNodeName,bstrName);
			//free the string
			::SysFreeString(bstrName);

			// find the name
			listItem->get_xml(&bstrText);
			
			// get the name
			GetQuotedStringAfter(bstrText,_T("type="),szName);
			::SysFreeString(bstrText);

			if (szNodeName == _T("element"))
			{
				Element * pElement = GetElementFromList(szName.GetBuffer(szName.GetLength()));
				//check the parent and the element name, if the same do not proceed !!!!
				if ((pElement) && _tcsicmp(pParent->GetName(), pElement->GetName()))
				{
					//add to parent
					pParent->AddChildElement(pElement);
					//add any children
					AddChildren(pSchemaNode,pElement);
				}
				else if ((pElement) && !_tcsicmp(pParent->GetName(), pElement->GetName()))
				{
					delete (pElement);
				}
				else
				{
					//error
				}
			}
			else if(szNodeName == _T("attribute"))
			{
				Attribute * pAttribute = GetAttributeFromList(szName.GetBuffer(szName.GetLength()));
				if (pAttribute)
				{
					//add to parent
					pParent->AddChildAttribute(pAttribute);
				}
				else
				{

				}
			}
			else
			{
				// error
			}

			SAFERELEASE(listItem);			
		}
	}

CleanUp:
	CheckXmlParserError(hr);
	SAFERELEASE(listItem);			
	SAFERELEASE(childList);			
	RETURN;
}

BOOL SVXml::AddChildrenToLists(MSXML2::IXMLDOMNode * pNode)
{
	HRESULT hr = S_OK;
	BSTR bstrName;
	BSTR bstrText;
	long lNumberOfChildren;
	MSXML2::IXMLDOMNodeList * childList = nullptr;
	MSXML2::IXMLDOMNode * listItem = nullptr;			
	CString szXmlText,szName,szDtType, szNodeName;
	int Elength = 0;
	int Alength = 0;

	//find number of child nodes
	lNumberOfChildren= GetChildNodes(pNode,&childList);

	if (lNumberOfChildren >= 0)
	{
		for (long x = 0; x < lNumberOfChildren; x++)
		{
			CHECKHR(childList->get_item(x, &listItem));			
			// determine type of element "ElementType" or "AttributeType"
			CHECKHR(listItem->get_nodeName(&bstrName));
			//convert BSTR to Cstring

			AfxBSTR2CString(&szNodeName,bstrName);
			//free the string
			::SysFreeString(bstrName);
			
			// find the name
			listItem->get_xml(&bstrText);

			// get the name
			GetQuotedStringAfter(bstrText,_T("name="),szName);
			::SysFreeString(bstrText);

			//get the data type
			GetQuotedStringAfter(bstrText,_T("dt:type="),szDtType);
			::SysFreeString(bstrText);

			if (szNodeName == _T("ElementType"))
			{
				// Add any children first. The allows for the last defined
				// Element to be the XML doc root element.
				AddChildrenToLists(listItem);
				// add the element
				AddElementToList(szName.GetBuffer(szName.GetLength()),szDtType.GetBuffer(szDtType.GetLength()));
			}
			else if(szNodeName == _T("AttributeType"))
			{
				//create attribute type
				AddAttributeToList(szName.GetBuffer(szName.GetLength()),szDtType.GetBuffer(szDtType.GetLength()));
				//never any children
			}
			else
			{
				// error
			}
			SAFERELEASE(listItem);			
		}
	}

	Elength = GetElementListLength();
	Alength = GetAttributeListLength();

CleanUp:
	CheckXmlParserError(hr);
	SAFERELEASE(listItem);			
	SAFERELEASE(childList);			
	RETURN;
}

int SVXml::GetElementListLength()
{
	return static_cast< int >( ElementList.GetCount() );
}

int SVXml::GetAttributeListLength()
{
	return static_cast< int >( AttributeList.GetCount() );
}

DWORD SVXml::GetParserErrorCode()
{
	return m_SVException.getMessage().m_MessageCode;
}
