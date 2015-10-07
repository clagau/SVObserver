//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVXml
//* .File Name       : $Workfile:   SVXml.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   30 Sep 2013 14:24:32  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVStatusLibrary\MessageContainer.h"
#include "xmlmacros.h"
#include "element.h"
#pragma endregion Includes

#import <msxml6.dll> named_guids

class SVXml  
{
public:
	DWORD GetParserErrorCode();
	SVXml();
	virtual ~SVXml();
	Element * GetTreeRoot();
	BOOL CreateXmlDocFromTreeObj();
	BOOL CreateTreeObjFromXmlDoc(BSTR * bstrXML);
	BOOL SaveXmlDocToFile(CString &);
	void GetSchemaFilename(CString & szSchemaFilename){szSchemaFilename=m_szSchemaFilename;};
	void SetSchemaFilename(CString & szSchemaFilename){m_szSchemaFilename=szSchemaFilename;};
	virtual BOOL InitXml( LPCTSTR p_szSchema = NULL );
	HRESULT Load(TCHAR * pUrl);
	MSXML2::IXMLDOMNodePtr GetChildElement(MSXML2::IXMLDOMNode * pNode, TCHAR * pName);
	virtual BOOL GetXmlDoc(BSTR * bstrDoc);
	BOOL PutXmlCmdDocInBuf(BYTE ** ppBuf, unsigned long * cBufLen);
	SvStl::MessageContainer& GetParserError(){return m_SVException;};

protected:
	SvStl::MessageContainer m_SVException;
	BOOL m_ExceptionSet;
	void Reset();
	BOOL GetXmlElementValues(Element * pElement);
	BOOL GetXmlElementValues(MSXML2::IXMLDOMElement * pDomElement,Element * pElement);
	BOOL GetDataValues();
	BOOL SetDataValues();
	BOOL CheckAncestors(Element * pElement,MSXML2::IXMLDOMElement * pXmlElement);
	BOOL SetXmlElementValues(Element * pElement);
	BOOL CreateXmlTree();
	BOOL LoadSchema( LPCTSTR p_szSchema = NULL );
	BOOL CreateDOMNode(MSXML2::IXMLDOMDocument2* pDoc,int type, BSTR bstrName,MSXML2::IXMLDOMNode ** pNode);
	BOOL LoadSchemaCollection();
	BOOL SaveAndLoad(BOOL blValidate = FALSE);
	BOOL CreateXMLDocTree();
	BOOL CreateXmlElements(Element * pElement, MSXML2::IXMLDOMElement * pXmlElement);
	void GetQuotedStringAfter(BSTR bstrString,CString szSearch, CString & szFound);
	long GetChildNodes(MSXML2::IXMLDOMNode * pNode, MSXML2::IXMLDOMNodeList ** ppNodeList);
	BOOL SetAttribute(MSXML2::IXMLDOMElement * m_pElement, LPTSTR szName, VARIANT vValue);

#ifdef _DEBUG
void GetXmlParserError(MSXML2::IXMLDOMParseError *errorObj, CString & szError);
#else
void GetXmlParserError(MSXML2::IXMLDOMParseError *errorObj);
#endif
	void Init();
	BOOL IsXmlDocValid();
	BOOL CreateBlankXmlDoc();
	void CheckXmlParserError(HRESULT hr);
	MSXML2::IXMLDOMDocument2 * m_pDoc;
	MSXML2::IXMLDOMSchemaCollection * m_pSchemaCollection;
	CString m_szSchemaFilename;
	BSTR m_bstrSchemaNamespace;

	BOOL InitXmlTree(void*);
	void DeleteRoot(){delete(m_pRootElement);m_pRootElement=NULL;};
	Element * GetElementFromList(LPTSTR lpName);
	Attribute * GetAttributeFromList(LPTSTR lpName);

	BOOL AddChildren(MSXML2::IXMLDOMNode* pSchemaNode, Element * pParent);
	Element * GetRootFromList();
	BOOL AddChildrenToLists(MSXML2::IXMLDOMNode * pNode);
	int GetElementListLength();
	int GetAttributeListLength();
	void AddElementToList(LPTSTR lpName, LPTSTR lpType);
	void AddAttributeToList(LPTSTR lpName, LPTSTR lpType);
	Element* m_pRootElement;
	CList< Element*, Element* > ElementList;		//list of defined xml doc elements from the loaded schema
	CList< Attribute*, Attribute* > AttributeList;	//list of defined xml doc attributes from the loaded schema

private:
	BOOL m_blComInit;
};

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVCmnLib\SVXml.h_v  $
 * 
 *    Rev 1.1   30 Sep 2013 14:24:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platforms.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Apr 2013 18:10:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   04 Oct 2012 08:41:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed class scope declaration on header methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   09 Nov 2010 15:32:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change formatting to a common searchable format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   22 Oct 2010 08:00:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix include problems and compilier efficiency.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   02 Sep 2009 16:11:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix include issues.
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
 *    Rev 1.2   22 Oct 2004 09:09:48   jBrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  375
 * SCR Title:  Remodel XML usage for configuration storage and SIAC transfers.
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   #import <msxml4.dll> named_guids 
 * was added here so that it would not need to be explicitly added to  other projects that use the SVXML class (not to be confused with the SVXMLClass).
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Oct 2001 10:09:28   ed
 * Project: SVSVIMInterface
 * Change Request (SCR) nbr: 2
 * SCR Title: Fix memory leaks in Activex Ctrl
 * Checked in by: Ed; Ed Chobanoff
 * Change Description: Modified the following functions definitions: 
 * SVXml::SetXmlElementValues,
 * SVXml::GetQuotedStringAfter, SVXml::AddChildren.
 * 
 * 
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/
