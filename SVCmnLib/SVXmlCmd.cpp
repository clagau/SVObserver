//******************************************************************************
// * COPYRIGHT (c) 2001 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCommRC.cpp
// * .File Name       : $Workfile:   SVXmlCmd.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 18:10:10  $
// ******************************************************************************

#include "stdafx.h"
#include "SVXmlCmd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

TCHAR g_szCmdSchema[] =
_T("<Schema name=\"SVR_schema\"  \n")
_T("xmlns=\"urn:schemas-microsoft-com:xml-data\"  \n")
_T("xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" > \n")
_T("<AttributeType name=\"Name\" dt:type=\"string\" required=\"yes\"/> \n")
_T("<AttributeType name=\"Type\" dt:type=\"string\" required=\"yes\"/> \n")
_T("<AttributeType name=\"Data\" dt:type=\"string\" required=\"yes\"/> \n")
_T("<ElementType name=\"ResultsData\" content=\"textOnly\" dt:type=\"string\" model=\"closed\" order=\"many\"> \n")
_T("<attribute type=\"Name\"/> \n")
_T("<attribute type=\"Type\"/> \n")
_T("<attribute type=\"Data\"/> \n")
_T("</ElementType> \n")
_T("<ElementType name=\"Tool\" content=\"eltOnly\" model=\"closed\" order=\"many\"> \n")
_T("<attribute type=\"Name\"/> \n")
_T("<element type=\"Tool\"/> \n")
_T("<element type=\"ResultsData\"/> \n")
_T("</ElementType> \n")
_T("<ElementType name=\"IPD\" content=\"eltOnly\" model=\"closed\" order=\"many\"> \n")
_T("<attribute type=\"Name\"/> \n")
_T("<element type=\"Tool\"/> \n")
_T("</ElementType> \n")
_T("<ElementType name=\"Results\" content=\"eltOnly\" model=\"closed\" order=\"many\"> \n")
_T("<element type=\"IPD\"/> \n")
_T("</ElementType> \n")
_T("<AttributeType name=\"Command\" default=\"GetSVIMState\" dt:type=\"enumeration\"  \ndt:values=\"GetSVIMConfig GetSVIMConfigName GetSVIMFile GetSVIMInspectionResults GetSVIMState LoadSVIMConfig PutSVIMConfig PutSVIMFile SetSVIMState\"/>\n")
_T("<ElementType name=\"SVIM_filename\" content=\"textOnly\" dt:type=\"string\" model=\"closed\"/> \n")
_T("<ElementType name=\"Remote_filename\" content=\"textOnly\" dt:type=\"string\" model=\"closed\"/> \n")
_T("<ElementType name=\"SVIM_state\" content=\"textOnly\" dt:type=\"ui4\" model=\"closed\"/> \n")
_T("<ElementType name=\"Bin_data\" content=\"textOnly\" dt:type=\"bin.base64\" model=\"closed\"/> \n")
_T("<ElementType name=\"SVCommand\" content=\"eltOnly\" model=\"closed\" order=\"seq\"> \n")
_T("<attribute type=\"Command\"/> \n")
_T("<element type=\"SVIM_filename\"/> \n")
_T("<element type=\"Remote_filename\"/> \n")
_T("<element type=\"SVIM_state\"/> \n")
_T("<element type=\"Bin_data\"/> \n")
_T("<element type=\"Results\"/> \n")
_T("</ElementType> \n")
_T("</Schema>");

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVXmlCmd::SVXmlCmd()
{
	m_ResultsDataLength = 0;
	m_pResultsDataElements = NULL;
}

SVXmlCmd::~SVXmlCmd()
{
	if(m_pResultsDataElements != NULL)
		{
		free(m_pResultsDataElements);
		m_pResultsDataElements = NULL;
		}
}
BOOL SVXmlCmd::InitXml()
{
	Reset();
	m_ResultsDataLength = 0;
	m_pResultsDataElements = NULL;
	SetSchemaFilename(CString(""));
	BOOL blReturn = SVXml::InitXml( g_szCmdSchema );
	ClearResultsData();
	return blReturn;

}

BOOL SVXmlCmd::InitXml(LPCTSTR szSchemaFileName)
{
	Reset();
	m_ResultsDataLength = 0;
	m_pResultsDataElements = NULL;
	SetSchemaFilename(CString(szSchemaFileName));
	BOOL blReturn = SVXml::InitXml(NULL);
	ClearResultsData();
	return blReturn;
}

BOOL SVXmlCmd::SetCommand(LPCTSTR Data)
{
	Element * pRootElement = NULL;
	Attribute * pAttribute = NULL;

	pRootElement = GetTreeRoot();
	if(pRootElement == NULL)return FALSE;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("Command"));
	if(pAttribute == NULL)return FALSE;
	//set the data
	return (pAttribute->SetData(Data));
}

BOOL SVXmlCmd::SetSVIMfilename(LPCTSTR Data)
{
	Element * pRootElement = NULL;
	Element * pElement = NULL;

	pRootElement = GetTreeRoot();
	if(pRootElement == NULL)return FALSE;
	//find the Attribute
	pElement = pRootElement->GetChildElement(_T("SVIM_filename"));
	if(pElement == NULL)return FALSE;
	//set the data
	return (pElement->SetData(Data));
}

BOOL SVXmlCmd::SetRemotefilename(LPCTSTR Data)
{
	Element * pRootElement = NULL;
	Element * pElement = NULL;

	pRootElement = GetTreeRoot();
	if(pRootElement == NULL)return FALSE;
	//find the Attribute
	pElement = pRootElement->GetChildElement(_T("Remote_filename"));
	if(pElement == NULL)return FALSE;
	//set the data
	return (pElement->SetData(Data));
}

BOOL SVXmlCmd::SetSVIMstate(ULONG Data)
{
	Element * pRootElement = NULL;
	Element * pElement = NULL;

	pRootElement = GetTreeRoot();
	if(pRootElement == NULL)return FALSE;
	//find the Attribute
	pElement = pRootElement->GetChildElement(_T("SVIM_state"));
	if(pElement == NULL)return FALSE;
	//set the data
	return (pElement->SetData(Data));
}


BOOL SVXmlCmd::SetBinData(BYTE *pBuf, ULONG FileLength)
{
	Element * pRootElement = NULL;
	Element * pElement = NULL;

	pRootElement = GetTreeRoot();
	if(pRootElement == NULL)return FALSE;
	//find the Attribute
	pElement = pRootElement->GetChildElement(_T("Bin_data"));
	if(pElement == NULL)return FALSE;
	//set the data
	return (pElement->SetData(pBuf,FileLength));
}



BOOL SVXmlCmd::GetCommand(LPTSTR * szTemp)
{
	Element * pRootElement = NULL;
	Attribute * pAttribute = NULL;

	pRootElement = GetTreeRoot();
	if(pRootElement == NULL)return FALSE;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("Command"));
	if(pAttribute == NULL)return FALSE;
	//set the data
	pAttribute->GetData(szTemp);
	return TRUE;
}

BOOL SVXmlCmd::GetSVIMfilename(LPTSTR * szTemp)
{
	Element * pRootElement = NULL;
	Element * pElement = NULL;

	pRootElement = GetTreeRoot();
	if(pRootElement == NULL)return FALSE;
	//find the Attribute
	pElement = pRootElement->GetChildElement(_T("SVIM_filename"));
	if(pElement == NULL)return FALSE;
	//set the data
	pElement->GetData(szTemp);
	return TRUE;
}

BOOL SVXmlCmd::GetRemotefilename(LPTSTR *szTemp)
{
	Element * pRootElement = NULL;
	Element * pElement = NULL;

	pRootElement = GetTreeRoot();
	if(pRootElement == NULL)return FALSE;
	//find the Attribute
	pElement = pRootElement->GetChildElement(_T("Remote_filename"));
	if(pElement == NULL)return FALSE;
	//set the data
	pElement->GetData(szTemp);
	return TRUE;
}

BOOL SVXmlCmd::GetSVIMstate(ULONG * Data)
{
	Element * pRootElement = NULL;
	Element * pElement = NULL;

	pRootElement = GetTreeRoot();
	if(pRootElement == NULL)return FALSE;
	//find the Attribute
	pElement = pRootElement->GetChildElement(_T("SVIM_state"));
	if(pElement == NULL)return FALSE;
	//set the data
	pElement->GetData(Data);
	return TRUE;
}


BOOL SVXmlCmd::GetBinData(BYTE **pBuf, ULONG * FileLength)
{
	Element * pRootElement = NULL;
	Element * pElement = NULL;

	pRootElement = GetTreeRoot();
	if(pRootElement == NULL)return FALSE;
	//find the Attribute
	pElement = pRootElement->GetChildElement(_T("Bin_data"));
	if(pElement == NULL)return FALSE;
	//set the data
	return (pElement->GetData(pBuf,FileLength));
}

BOOL SVXmlCmd::SetResultsData(CString szIPD, CString szFullname, CString szValue, CString szType)
{

	Element * pRootElement = NULL;
	Element * pResultsElement = NULL;
	Element ** pIpdElements = NULL;
	Element * pIpdElement = NULL;
	Element * pElement = NULL;
	Element * pSubElement = NULL;
	Element * pNewSubElement = NULL;


	int nIpds = 0;

	pRootElement = GetTreeRoot();
	if(pRootElement == NULL)return FALSE;

	//first search for the Results element
	pResultsElement = pRootElement->GetChildElement(_T("Results"));
	if(pResultsElement == NULL)return FALSE;

	//next find all the IPD elements under the Results element
	nIpds = pResultsElement->FindChildElements(_T("IPD"),&pIpdElements);

	//remove any empty IPDs
	if(nIpds > 0)
		{
		//walk through the found ipds looking for an ipd element with the szIPD name
		Element * pPtr = *pIpdElements;
		pPtr--;
		TCHAR * pName;
		for(int x=0; x<nIpds; x++)
			{
			pPtr++;
			//get Name attribute
			Attribute * pAttr = pPtr->GetChildAttribute(_T("Name"));
			if(pAttr == NULL)continue;
			//get the name value from the Name attribute			
			pAttr->GetData(&pName);
			//compare to search name
			if(!_tcsicmp(pName,szIPD.GetBuffer(szIPD.GetLength())))
				{//we found the IPD with szIPD name
				pIpdElement = pPtr;
				break;
				}
			else if (_tcsclen(pName) == 0)
				{
				pResultsElement->DeleteChildElement(pPtr);
				nIpds--;
				if(x>0)x--;
				}
			}
		}

	//free the list of ipd pointers
	if(pIpdElements)free(pIpdElements);

	if(pIpdElement == NULL)
		{//create IPD element since it does not exist
		pIpdElement = GetElementFromList(_T("IPD"));
		//return if not found
		if(pIpdElement == NULL)return FALSE;
		//create the child attribute
		Attribute * pAttr = GetAttributeFromList(_T("Name"));
		//set the data
		pAttr->SetData(szIPD.GetBuffer(szIPD.GetLength()));
		//add to parent
		pIpdElement->AddChildAttribute(pAttr);
		//add to parent
		pResultsElement->AddChildElement(pIpdElement);
		}

	
	//determine the number of tools in the name
	int index = 0;
	CStringArray szToolNames;
	while(index != -1)
		{
		index = szFullname.Find(_T("."),index);
		if(index != -1)
			{
			szToolNames.Add(szFullname.Left(index));
			szFullname.Delete(0,index+1);
			}
		}
	if(!szFullname.IsEmpty())szToolNames.Add(szFullname);

	//these toolnames are similar to a directory structure
	//therefore we will loop through the list of tools to find 
	//the element with the given name or to create an element with 
	//the tool name.

	pSubElement = pIpdElement;
	TCHAR * pName;

	for(index = 0; index < szToolNames.GetSize(); index++)
		{
		for(int y=0; y<pSubElement->HasChildElements(); y++)
			{
			Element * pElem = pSubElement->ppElements[y];
			//get Name attribute
			Attribute * pAttr = pElem->GetChildAttribute(_T("Name"));
			if(pAttr == NULL)continue;
			//get the name value from the Name attribute			
			pAttr->GetData(&pName);
			//compare to search name
			if(!_tcsicmp(pName,szToolNames[index].GetBuffer(szToolNames[index].GetLength())))
				{//we found it
				pNewSubElement = pElem;
				break;
				}
			}
		if(pNewSubElement == NULL)
			{//no kids, create the tool
			if(index == szToolNames.GetSize()-1)
				{
				pNewSubElement = GetElementFromList(_T("ResultsData"));
				}
			else
				{
				pNewSubElement = GetElementFromList(_T("Tool"));
				}
			//return if not found
			if(pNewSubElement == NULL)return FALSE;
			//create the child attribute
			Attribute * pAttr = GetAttributeFromList(_T("Name"));
			//set the data
			pAttr->SetData(szToolNames[index].GetBuffer(szToolNames[index].GetLength()));
			//add to parent
			pNewSubElement->AddChildAttribute(pAttr);
			//add to parent
			pSubElement->AddChildElement(pNewSubElement);
			}
		pSubElement = pNewSubElement;
		pNewSubElement = NULL;
		}
	
	//using the last element, set the results data
	//create the child attribute
	Attribute * pAttrValue = GetAttributeFromList(_T("Data"));
	//set the data
	pAttrValue->SetData(szValue.GetBuffer(szValue.GetLength()));
	//add to parent
	pSubElement->AddChildAttribute(pAttrValue);

	//create the child attribute
	Attribute * pAttrType = GetAttributeFromList(_T("Type"));
	//set the data
	pAttrType->SetData(szType.GetBuffer(szValue.GetLength()));
	//add to parent
	pSubElement->AddChildAttribute(pAttrType);

	return TRUE;
}



BOOL SVXmlCmd::ClearResultsData()
{
	Element * pRootElement = NULL;
	Element * pResultsElement = NULL;
	Element ** pIpdElements = NULL;
	Element * pIpdElement = NULL;
	int nIpds = 0;

	pRootElement = GetTreeRoot();
	if(pRootElement == NULL)return FALSE;

	//first search for the Results element
	pResultsElement = pRootElement->GetChildElement(_T("Results"));
	if(pResultsElement == NULL)return FALSE;

	//next find all the IPD elements under the Results element
	nIpds = pResultsElement->FindChildElements(_T("IPD"),&pIpdElements);

	//remove all IPDs
	if(nIpds)
		{
	Element * pPtr = *pIpdElements;
	pPtr--;
	for(int x=0; x<nIpds; x++)
		{
		pPtr++;
		pResultsElement->DeleteChildElement(pPtr);
		nIpds--;
		if(x>0)x--;
			}
		//free the list of ipd pointers
		free(pIpdElements);
		}

	return TRUE;
}



long SVXmlCmd::EnumerateResultsData(LPTSTR pIpdName)
{
	Element * pResultsElement = NULL;
	Element ** ppIpdElements = NULL;
	Element * pRootElement = NULL;
	int nIPDitems = 0;
	LPCTSTR szTemp = NULL;
	BOOL blFound = FALSE;
	int x = 0;

	//SaveXmlDocToFile(CString("ResultsTest.txt"));

	pRootElement = GetTreeRoot();
	if(pRootElement == NULL)return FALSE;

	//first, search for the Results element
	pResultsElement = pRootElement->GetChildElement(_T("Results"));
	if(pResultsElement == NULL)return FALSE;

	//next, find all the IPD elements under the Results element
	nIPDitems = pResultsElement->FindChildElements(_T("IPD"),&ppIpdElements,TRUE);
	if(nIPDitems == 0)return FALSE;

	//find the IPD element with the passed in name attribute

	for (x=0;x<nIPDitems;x++)
		{
		ppIpdElements[x]->GetChildAttribute(_T("Name"))->GetData(&szTemp);
		if(!_tcsicmp(szTemp,pIpdName))
			{
			blFound = TRUE;
			break;
			}
		}

	//next find all the ResultsData elements under the Results element
	if(blFound)
		{
		m_ResultsDataLength = ppIpdElements[x]->FindChildElements(_T("ResultsData"),&m_pResultsDataElements,TRUE);
		}
	else
		{
		m_ResultsDataLength = 0;
		}

	//free the list of ipd pointers
	if(ppIpdElements)free(ppIpdElements);

	return (LONG)m_ResultsDataLength;
}

//##ModelId=3BF00CB90196
BOOL SVXmlCmd::LoadDoc(BSTR * bstrDoc)
{
	return CreateTreeObjFromXmlDoc(bstrDoc);
}

BOOL SVXmlCmd::GetResultsData(int index,CString szIPD, CString& szFullname, CString& szValue, CString& szType)
{
	CString szTemp;
	BSTR bstrTemp;

	if(m_pResultsDataElements != NULL)
		{
		free(m_pResultsDataElements);
		m_pResultsDataElements = NULL;
		}


	EnumerateResultsData(szIPD.GetBuffer(10));


	if((index < 0) || (index >= m_ResultsDataLength))return FALSE;

	
	// get the data 
	Attribute * pAttr = m_pResultsDataElements[index]->GetChildAttribute(_T("Data"));
	if(pAttr == NULL)return FALSE;
	pAttr->GetData(&bstrTemp);
	szValue	= bstrTemp;
	pAttr = NULL;

	pAttr = m_pResultsDataElements[index]->GetChildAttribute(_T("Type"));
	if(pAttr == NULL)return FALSE;
	pAttr->GetData(&bstrTemp);
	szType	= bstrTemp;
	pAttr = NULL;

	pAttr = m_pResultsDataElements[index]->GetChildAttribute(_T("Name"));
	pAttr->GetData(&bstrTemp);
	szFullname	= bstrTemp;
	szFullname = _T(".") + szFullname;
	pAttr = NULL;

	//now walk up through the parent elements until we get to the IPD element
	
	Element * pParent = m_pResultsDataElements[index]->GetParent();
	if(pParent == NULL)return FALSE;
	CString szName = pParent->GetName();
	
	while (szName != _T("IPD"))
		{
		pAttr = pParent->GetChildAttribute(_T("Name"));
		if(pAttr == NULL)return FALSE;
		pAttr->GetData(&bstrTemp);
		szTemp	= bstrTemp;
		szFullname = szTemp + szFullname;
		szFullname = _T(".") + szFullname;
		pParent = pParent->GetParent();
		if(pParent == NULL)return FALSE;
		szName = pParent->GetName();
		}

	
	szFullname.Delete(0);
	pAttr = pParent->GetChildAttribute(_T("Name"));
	if(pAttr == NULL)return FALSE;

	return TRUE;

}

/*
LPTSTR SVXmlCmd::GetCmdType()
{
	Element * pRootElement = NULL;
	Attribute * pAttribute = NULL;
	LPTSTR szTemp;

	pRootElement = GetTreeRoot();
	if(pRootElement == NULL)return NULL;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("CmdType"));
	if(pAttribute == NULL)return NULL;
	//set the data
	pAttribute->GetData(&szTemp);
	return szTemp;
}

BOOL SVXmlCmd::SetCmdType(LPCTSTR Data)
{
	Element * pRootElement = NULL;
	Attribute * pAttribute = NULL;

	pRootElement = GetTreeRoot();
	if(pRootElement == NULL)return FALSE;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("CmdType"));
	if(pAttribute == NULL)return FALSE;
	//set the data
	return (pAttribute->SetData(Data));
}



*/

BOOL SVXmlCmd::GetXmlDoc(BSTR *bstrDoc)
{
	if(!CreateXmlDocFromTreeObj())
		{
		*bstrDoc = SysAllocStringByteLen (NULL, 0 );
		return FALSE;
		}
	if(!SVXml::GetXmlDoc(bstrDoc))return FALSE;
	return TRUE;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCmnLib\SVXmlCmd.cpp_v  $
 * 
 *    Rev 1.0   18 Apr 2013 18:10:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3.1.0   22 Oct 2010 08:00:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix include problems and compilier efficiency.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   19 Jun 2007 12:48:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * Other source files were removed from the project because they are not used as part of the SVObserver project.
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   03 Jan 2002 13:53:32   ed
 * Project: SVSVIMInterface
 * Change Request (SCR) nbr: 3
 * SCR Title: Fix XmlCmd class in common lib so that results data can be retrieved.
 * Checked in by: Ed; Ed Chobanoff
 * Change Description: Modified the following functions: ~SVXmlCmd, 
 * EnumerateResultsData, GetResultsData. 
 * 
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/