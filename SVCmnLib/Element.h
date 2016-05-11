// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SchemaElement
// * .File Name       : $Workfile:   Element.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 18:10:02  $
// ******************************************************************************

#pragma once

#define ELEMENT		0x0000
#define ATTRIBUTE	0x0001

class SchemaElement
{
public:
	void DeleteContent();
	SchemaElement();
	SchemaElement(const SchemaElement& source);
	virtual ~SchemaElement();
	SchemaElement& operator=(const SchemaElement& source);
	SchemaElement* operator=(const SchemaElement* source);
	void SetName(LPTSTR szName);
	TCHAR* GetName(){ return m_lpszName; }
	void SetType(CString szType);
	LPTSTR GetType();
	BOOL SetData(BYTE *pBuf, unsigned long cBufLen);
	BOOL SetData(unsigned long Data);
	BOOL SetData(long Data);
	BOOL SetData(LPCTSTR Data);
	BOOL SetData(BOOL Data);
	BOOL SetData(BSTR * Data);
	VARIANT GetData(){return m_var;};
	BOOL GetData(BYTE **ppBuf, unsigned long * pcBufLen);
	void GetData(BSTR * Data);
#ifndef _UNICODE
	void GetData(TCHAR** Data);//BSTR * Data);
#endif
	void GetData(LPCTSTR * lpChar)
	{
		*lpChar = (LPCTSTR)m_var.pcVal;
	}

	void GetData(short * lpShort){*lpShort = m_var.iVal;};
	void GetData(long * lpLong){*lpLong = m_var.lVal;};
	void GetData(BYTE * lpByte){*lpByte = m_var.bVal;};
	void GetData(ULONG * lpULong){*lpULong = m_var.ulVal;};
	void GetData(BOOL * lpBool);
	void SetAsElement(){m_ElementType = ELEMENT;};
	void SetAsAttribute(){m_ElementType = ATTRIBUTE;};
	BOOL IsElement(){return (m_ElementType == ELEMENT) ? TRUE : FALSE;};
	BOOL IsAtrribute(){return (m_ElementType == ATTRIBUTE) ? TRUE : FALSE;};
	BOOL SetData(VARIANT vData);

protected:
	TCHAR*	m_lpszName;			//name of element/attribute
	VARIANT m_var;				//variant data
	UINT m_ElementType;
};


//##ModelId=3A9E4BDB0198
class Attribute : public SchemaElement
{
	public:
	//##ModelId=3A9E4BDB01A3
		Attribute(){};
	//##ModelId=3A9E4BDB01A2
		virtual ~Attribute(){};
};

//##ModelId=3A9E4BDB012A
class Element : public SchemaElement
{
public:
	void DeleteChildElement(Element *);
	void DeleteContent();
	int FindChildElements(LPTSTR pName, Element *** ppElements,BOOL blSubs = FALSE);
	int HasChildAttributes();
	int HasChildElements();
	Attribute * GetChildAttribute(LPTSTR pName);
	Element * GetChildElement(LPTSTR pName);
	Element();
	Element(const Element&);
	Element& operator=(const Element& source);
	Element* operator=(const Element* source);
	virtual ~Element();
	Element **ppElements;		//list of child element pointers
	Attribute **ppAttributes;   //list of attribute object pointers
	Element * m_pParent;			//pointer to parent element

	Element * FindRootElement();
	void SetParent(Element * lpParent){m_pParent = lpParent;};
	Element * GetParent(){return m_pParent;};
	void AddChildElement(Element* pChild);
	void AddChildAttribute(Attribute* pChild);
};
