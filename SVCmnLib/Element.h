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

#if !defined __ELEMENT_H
#define __ELEMENT_H


#define ELEMENT		0x0000
#define ATTRIBUTE	0x0001


//##ModelId=3A9E4BDA0287
class SchemaElement
{
	public:
	void DeleteContent();
	//##ModelId=3A9E4BDA02C4
	SchemaElement();
	//##ModelId=3A9E4BDA02C5
	SchemaElement(SchemaElement& source);
	//##ModelId=3A9E4BDA02C3
	virtual ~SchemaElement();
	//##ModelId=3A9E4BDA02BF
	SchemaElement& operator=(const SchemaElement& source);
	//##ModelId=3A9E4BDA02C1
	SchemaElement* operator=(const SchemaElement* source);
	//##ModelId=3A9E4BDA02BD
	void SetName(LPTSTR szName);
	//##ModelId=3A9E4BDA02BC
	TCHAR* GetName(){return m_lpszName;};
	//##ModelId=3A9E4BDA02BA
	void SetType(CString szType);
	//##ModelId=3A9E4BDA02B9
	LPTSTR GetType();
	//##ModelId=3A9E4BDA02AA
	BOOL SetData(BYTE *pBuf, unsigned long cBufLen);
	//##ModelId=3A9E4BDA02AD
	BOOL SetData(unsigned long Data);
	//##ModelId=3A9E4BDA02AF
	BOOL SetData(long Data);
	//##ModelId=3A9E4BDA02B1
	BOOL SetData(LPCTSTR Data);
	//##ModelId=3A9E4BDA02B3
	BOOL SetData(BOOL Data);
	//##ModelId=3A9E4BDA02B5
	BOOL SetData(BSTR * Data);
	//##ModelId=3A9E4BDA0296
	VARIANT GetData(){return m_var;};
	//##ModelId=3A9E4BDA0297
	BOOL GetData(BYTE **ppBuf, unsigned long * pcBufLen);
	//##ModelId=3A9E4BDA029C
	void GetData(BSTR * Data);
#ifndef _UNICODE
	void GetData(TCHAR** Data);//BSTR * Data);
#endif
	//##ModelId=3A9E4BDA029E
	void GetData(LPCTSTR * lpChar)
		{
		*lpChar = (LPCTSTR)m_var.pcVal;
		};
	//##ModelId=3A9E4BDA02A0
	void GetData(short * lpShort){*lpShort = m_var.iVal;};
	//##ModelId=3A9E4BDA02A2
	void GetData(long * lpLong){*lpLong = m_var.lVal;};
	//##ModelId=3A9E4BDA02A4
	void GetData(BYTE * lpByte){*lpByte = m_var.bVal;};
	//##ModelId=3A9E4BDA02A6
	void GetData(ULONG * lpULong){*lpULong = m_var.ulVal;};
	//##ModelId=3A9E4BDA02A8
	void GetData(BOOL * lpBool);
	//##ModelId=3A9E4BDA0295
	void SetAsElement(){m_ElementType = ELEMENT;};
	//##ModelId=3A9E4BDA0294
	void SetAsAttribute(){m_ElementType = ATTRIBUTE;};
	//##ModelId=3A9E4BDA0293
	BOOL IsElement(){return (m_ElementType == ELEMENT) ? TRUE : FALSE;};
	//##ModelId=3A9E4BDA0292
	BOOL IsAtrribute(){return (m_ElementType == ATTRIBUTE) ? TRUE : FALSE;};
	//##ModelId=3A9E4BDA02B7
	BOOL SetData(VARIANT vData);

	protected:
	//##ModelId=3A9E4BDA0291
	TCHAR*	m_lpszName;			//name of element/attribute
	//##ModelId=3A9E4BDA028B
	VARIANT m_var;				//variant data
	//##ModelId=3A9E4BDA0288
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
	//##ModelId=3A9E4BDB0164
		void DeleteChildElement(Element *);
	//##ModelId=3A9E4BDB0163
		void DeleteContent();
	//##ModelId=3A9E4BDB015F
	int FindChildElements(LPTSTR pName, Element *** ppElements,BOOL blSubs = FALSE);
	//##ModelId=3A9E4BDB015E
	int HasChildAttributes();
	//##ModelId=3A9E4BDB015D
	int HasChildElements();
	//##ModelId=3A9E4BDB015B
	Attribute * GetChildAttribute(LPTSTR pName);
	//##ModelId=3A9E4BDB0159
	Element * GetChildElement(LPTSTR pName);
	//##ModelId=3A9E4BDB0156
	Element();
	//##ModelId=3A9E4BDB0157
	Element(Element&);
	//##ModelId=3A9E4BDB0152
	Element& operator=(const Element& source);
	//##ModelId=3A9E4BDB0154
	Element* operator=(const Element* source);
	//##ModelId=3A9E4BDB0150
	virtual ~Element();
	//##ModelId=3A9E4BDB0140
	Element **ppElements;		//list of child element pointers
	//##ModelId=3A9E4BDB0139
	Attribute **ppAttributes;   //list of attribute object pointers
	//##ModelId=3A9E4BDB0134
	Element * m_pParent;			//pointer to parent element

	//##ModelId=3A9E4BDB014F
	Element * FindRootElement();
	//##ModelId=3A9E4BDB014D
	void SetParent(Element * lpParent){m_pParent = lpParent;};
	//##ModelId=3A9E4BDB014C
	Element * GetParent(){return m_pParent;};
	//##ModelId=3A9E4BDB014A
	void AddChildElement(Element* pChild);
	//##ModelId=3A9E4BDB0148
	void AddChildAttribute(Attribute* pChild);
};


#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCmnLib\Element.h_v  $
 * 
 *    Rev 1.0   18 Apr 2013 18:10:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 Jun 2007 12:48:12   Joe
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
*/