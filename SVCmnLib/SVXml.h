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

