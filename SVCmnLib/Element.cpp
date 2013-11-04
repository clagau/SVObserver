// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : Element
// * .File Name       : $Workfile:   Element.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 18:10:00  $
// ******************************************************************************

// Element.cpp: implementation of the Element,Attribute, 
// & SchemaElement classes
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "element.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SchemaElement::SchemaElement()
{
	m_lpszName  = NULL;	//name of element/attribute
	VariantInit(&m_var);  //Initialize our variant
}


SchemaElement::SchemaElement(SchemaElement& source)
{
	HRESULT hr = S_OK;
	//copy the string
	int nLength = _tcsclen(source.m_lpszName);
	//create storage
	m_lpszName = (TCHAR *)malloc((nLength + 1) * sizeof(TCHAR));
	//copy the data in
	_tcscpy(m_lpszName,source.m_lpszName);

	//copy the variant
	VariantInit(&m_var);  //Initialize our variant
  	hr = VariantCopy(&m_var,&(source.m_var));  

	//copy the type of element
	m_ElementType = source.m_ElementType;
}



SchemaElement::~SchemaElement()
{
	if(m_lpszName)
		{
		int x = _tcsclen(m_lpszName);
		if(x>0)free(m_lpszName);
		m_lpszName = NULL;
		}
/*	if(m_var.vt == VT_BSTR)
		{
		UINT uLen = SysStringLen(m_var.bstrVal);
		if(uLen)
			{
			SysFreeString(m_var.bstrVal);
			}
		}
	else if(m_var.vt == (VT_ARRAY | VT_UI1))
		{
		SafeArrayDestroy(m_var.parray);
		}//*/
	VariantClear(&m_var);
}

void SchemaElement::DeleteContent()
{
	if(m_lpszName)
		{
		int x = _tcsclen(m_lpszName);
		if(x>0)free(m_lpszName);
		m_lpszName = NULL;
		}
	VariantClear(&m_var); 
}


SchemaElement& SchemaElement::operator=(const SchemaElement& source)
{
	m_lpszName = source.m_lpszName;
	m_var = source.m_var;
	m_ElementType = source.m_ElementType;
	return *this;
}

SchemaElement* SchemaElement::operator=(const SchemaElement* source)
{
	m_lpszName = source->m_lpszName;
	m_var = source->m_var;
	m_ElementType = source->m_ElementType;
	return this;
}


void SchemaElement::SetName(LPTSTR szName)
{
	if(m_lpszName != NULL)
		{
		int x = _tcsclen(m_lpszName);
		if(x>0)free(m_lpszName);
		m_lpszName = NULL;
		}

	//create storage
	m_lpszName = (TCHAR *)malloc((_tcslen(szName) + 1) * sizeof(TCHAR));
	//copy the data in
	_tcscpy(m_lpszName,szName);
}


LPTSTR SchemaElement::GetType()
{
	LPTSTR szType;

	switch(m_var.vt)
		{
		case VT_BSTR:
			{
			szType = _T("string");
			break;
			}
		case VT_I4:
			{
			szType = _T("i4");
			break;
			}
		case VT_I2:
			{
			szType = _T("i2");
			break;
			}
		case VT_I1:
			{
			szType = _T("i1");
			break;
			}
		case VT_UI4:
			{
			szType = _T("ui4");
			break;
			}
		case VT_UI2:
			{
			szType = _T("ui2");
			break;
			}
		case VT_UI1:
			{
			szType = _T("ui1");
			break;
			}
		case (VT_ARRAY | VT_UI1):
			{
			szType = _T("bin.base64");
			break;
			}
		case VT_EMPTY:
			{
			szType = _T("");
			break;
			}
		case VT_NULL:
			{
			szType = _T("NULL");
			break;
			}
		case VT_BOOL:
			{
			szType = _T("boolean");
			break;
			}
		default:
			szType = _T("");
			break;
		}
	return szType;
}



void SchemaElement::SetType(CString szType)
{
	VariantClear(&m_var);// != S_OK)return fRetVal;

	VariantInit(&m_var);  //Initialize our variant

	//set to null so that destructor does not fail
	m_var.parray = NULL;
	
	// determine the type
	if((szType == _T("enumeration")) || (szType == _T("string")) )// || szType.IsEmpty())
		{
		m_var.vt = VT_BSTR;
//		V_BSTR(&m_var) = ::SysAllocString(_T("T"));
		V_BSTR(&m_var) = ::SysAllocStringByteLen(NULL,0);
		}
	else if(szType.IsEmpty())
		{
		m_var.vt = VT_NULL;
		}
	else if (szType == _T("i4"))
		{
		m_var.vt = VT_I4;
		}
	else if (szType == _T("boolean"))
		{
		m_var.vt = VT_BOOL;
		m_var.boolVal = -1;
		}
	else if (szType == _T("i2"))
		{
		m_var.vt = VT_I2;
		}
	else if (szType == _T("i1"))
		{
		m_var.vt = VT_I1;
		}
	else if (szType == _T("ui4"))
		{
		m_var.vt = VT_UI4;
		}
	else if (szType == _T("ui2"))
		{
		m_var.vt = VT_UI2;
		}
	else if (szType == _T("ui1"))
		{
		m_var.vt = VT_UI1;
		}
	else if (szType == _T("bin.base64"))
		{
		m_var.vt = (VT_ARRAY | VT_UI1);
		
		//init to one element
		//Set up the bounds structure
		SAFEARRAYBOUND  rgsabound[1];

		rgsabound[0].cElements = 0;
//		rgsabound[0].cElements = 1;
		rgsabound[0].lLbound = 0;

		//Create an OLE SAFEARRAY
		m_var.parray = SafeArrayCreate(VT_UI1,1,rgsabound);
		}
	else
		{
		m_var.vt = VT_NULL;
		}
}

BOOL SchemaElement::SetData(BSTR * Data)
{
	BOOL fRetVal = TRUE;
	ULONG * pPtr = (ULONG*)*Data;
	pPtr--;

	if(m_var.vt == VT_BSTR)
		{
		if(m_var.bstrVal != NULL)
			{
			::SysFreeString(m_var.bstrVal);
			}
		//copy the BSTR
		V_BSTR(&m_var) = ::SysAllocStringByteLen((LPCSTR)(*Data),(UINT)(*pPtr));
		}
	else
		{ //error
		fRetVal = FALSE;
		}
	return fRetVal;
}


BOOL SchemaElement::SetData(LPCTSTR Data)
{
	USES_CONVERSION;
	BOOL fRetVal = TRUE;

	if(m_var.vt == VT_BSTR)
		{
		if(m_var.bstrVal != NULL)
			{
			::SysFreeString(m_var.bstrVal);
			}
		V_BSTR(&m_var) = ::SysAllocString(T2W((LPTSTR)Data));
		}
	else
		{ //error
		fRetVal = FALSE;
		}
	return fRetVal;
}


BOOL SchemaElement::SetData(VARIANT vData)
{
	//clear the variant
	if(VariantClear(&m_var) != S_OK)return FALSE;
	//init the variant
	VariantInit(&m_var);
	//copy the variant
  	if(VariantCopy(&m_var,&vData) != S_OK)return FALSE;  
//	UINT uLen = SysStringLen(m_var.bstrVal);

	return TRUE;
}



BOOL SchemaElement::SetData(long Data)
{
	BOOL fRetVal = TRUE;

	if (m_var.vt == VT_I4)
		{
		V_I4(&m_var) = Data;
		}
	else if (m_var.vt == VT_I2)
		{
		V_I2(&m_var) = (short)Data;
		}
	else if (m_var.vt == VT_I1)
		{
		V_I2(&m_var) = (char)Data;
		}
	else
		{ //error
		fRetVal = FALSE;
		}
	return fRetVal;
}

BOOL SchemaElement::SetData(unsigned long Data)
{
	BOOL fRetVal = TRUE;

	if (m_var.vt == VT_UI4)
		{
		V_UI4(&m_var) = Data;
		}
	else if (m_var.vt == VT_UI2)
		{
		V_UI2(&m_var) = (unsigned short)Data;
		}
	else if (m_var.vt == VT_UI1)
		{
		V_UI2(&m_var) = (BYTE)Data;
		}
	else
		{ //error
		fRetVal = FALSE;
		}
	return fRetVal;
}

BOOL SchemaElement::SetData(BOOL Data)
{
	BOOL fRetVal = TRUE;

	if (m_var.vt == VT_BOOL)
		{
		if(Data == FALSE)V_BOOL(&m_var) = Data;
		else V_BOOL(&m_var) = VARIANT_TRUE;
		}
	else
		{ //error
		fRetVal = FALSE;
		}
	return fRetVal;
}




BOOL SchemaElement::SetData(BYTE *pBuf, unsigned long cBufLen)
{
	BOOL fRetVal = FALSE;

	//the type of the array is unsigned chars (OLE SAFEARRAY)
	if(m_var.vt == (VT_ARRAY | VT_UI1))
		{
		SafeArrayDestroy(m_var.parray);

		//Set up the bounds structure
		SAFEARRAYBOUND  rgsabound[1];

		rgsabound[0].cElements = cBufLen;
		rgsabound[0].lLbound = 0;

		//Create an OLE SAFEARRAY
		m_var.parray = SafeArrayCreate(VT_UI1,1,rgsabound);

		if(m_var.parray != NULL)
			{
			void * pArrayData = NULL;

			//Get a safe pointer to the array
			SafeArrayAccessData(m_var.parray,&pArrayData);

			//Copy data to it
			memcpy(pArrayData, pBuf, cBufLen);

			//Unlock the variant data
			SafeArrayUnaccessData(m_var.parray);

			fRetVal = TRUE;
			}
		}
	else
		{ //error
		fRetVal = FALSE;
		}
	return fRetVal;
}


void SchemaElement::GetData(BOOL * lpBool)
{
	if(m_var.boolVal == VARIANT_TRUE)*lpBool = TRUE;
	else *lpBool = FALSE;
}


#ifndef _UNICODE
void SchemaElement::GetData(TCHAR ** Data)
{
USES_CONVERSION;
	//copy the data
	UINT nLength = SysStringLen(m_var.bstrVal); 
	//create storage
	*Data = (TCHAR *)malloc((nLength + 1) * sizeof(TCHAR));
	//copy the data in
	_tcscpy(*Data,W2T(m_var.bstrVal));
//	*Data = W2T(m_var.bstrVal);
}
#endif

void SchemaElement::GetData(BSTR * Data)
{
	//Do not init BSTR "Data"
	*Data = m_var.bstrVal;
}	  
  



BOOL SchemaElement::GetData(BYTE **ppBuf, unsigned long * pcBufLen)
{
     BOOL fRetVal = FALSE;

	 *pcBufLen = 0;

   //Binary data is stored in the variant as an array of unsigned char
     if(m_var.vt == (VT_ARRAY|VT_UI1))  // (OLE SAFEARRAY)
     {
       //Retrieve size of array
       *pcBufLen = m_var.parray->rgsabound[0].cElements;

       *ppBuf = new BYTE[*pcBufLen]; //Allocate a buffer to store the data
       if(*ppBuf != NULL)
       {
         void * pArrayData;

         //Obtain safe pointer to the array
         SafeArrayAccessData(m_var.parray,&pArrayData);

         //Copy the data into our buffer
         memcpy(*ppBuf, pArrayData, *pcBufLen);

         //Unlock the variant data
         SafeArrayUnaccessData(m_var.parray);
         fRetVal = TRUE;
       }
     }
	 else if (m_var.vt == VT_NULL)fRetVal = TRUE;
     return fRetVal;
}


// implementation of the Element class.
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


Element::Element()
{
	m_pParent = NULL;
	ppElements = NULL;
	ppAttributes = NULL;
}


//copy constructor
Element::Element(Element& source)
	:SchemaElement(source)
{
	//only make a copy of the data!
	m_pParent = NULL;
	ppElements = NULL;
	ppAttributes = NULL;
}


Element& Element::operator=(const Element& source)
{
	m_pParent = source.m_pParent;
	ppElements = source.ppElements;
	ppAttributes = source.ppAttributes;
	return *this;
}

Element* Element::operator=(const Element* source)
{
	m_pParent = source->m_pParent;
	ppElements = source->ppElements;
	ppAttributes = source->ppAttributes;
	return this;
}

void Element::DeleteContent()
{
	SchemaElement::DeleteContent();
	//delete the attributes first
	if(ppAttributes!= NULL)
		{
		for(int x=0; x<HasChildAttributes(); x++)
			{
			delete ppAttributes[x];
			}
		free( ppAttributes);
		}

	//next delete the elements, but walk the children first
	if(ppElements!= NULL)
		{
		for(int x=0; x<HasChildElements(); x++)
			{
			delete ppElements[x];
			}
		free( ppElements);
		}
	m_pParent = NULL;
	ppElements = NULL;
	ppAttributes = NULL;
}


Element::~Element()
{
	//delete the attributes first
	if(ppAttributes!= NULL)
		{
		for(int x=0; x<HasChildAttributes(); x++)
			{
			delete ppAttributes[x];
			}
		free( ppAttributes);
		}

	//next delete the elements, but walk the children first
	if(ppElements!= NULL)
		{
		for(int x=0; x<HasChildElements(); x++)
			{
			delete ppElements[x];
			}
		free( ppElements);
		}
}

Element * Element::FindRootElement()
{
	if(m_pParent == NULL)
		{
		return this;
		}
	else
		{
		return m_pParent->FindRootElement();
		}
}

void Element::AddChildAttribute(Attribute* pChild)
{
	int size = HasChildAttributes();// 0;
	size += 2;

	pChild->SetAsAttribute();

	ppAttributes = (Attribute**)realloc(ppAttributes, sizeof(Attribute *) * size);
	ppAttributes[size - 2] = pChild;
	ppAttributes[size - 1] = NULL;
}

void Element::AddChildElement(Element* pChild)
{
	int size = HasChildElements();
	size += 2;

	pChild->SetParent(this);
	pChild->SetAsElement();

	ppElements = (Element**)realloc(ppElements, sizeof(Element *) * size);
	ppElements[size - 2] = pChild;
	ppElements[size - 1] = NULL;
}

int Element::FindChildElements(LPTSTR pName, Element *** ppElmnts, BOOL blSubs /* = FALSE */)
{
//	Element ** lppElements = *ppElmnts;
	//init if NULL
	// this is a NULL TERMINATED LIST (ARRAY) OF Element pointers
	//is this redundant ?
	if(*ppElmnts == NULL)
		{
		*ppElmnts = (Element**)malloc(sizeof(Element *));
		(*ppElmnts)[0] = NULL;
		}

	//walk this list of child elements searching for the elements with the 
	//passed in name
	for(int x=0; x<HasChildElements(); x++)
		{
		if(!_tcsicmp(ppElements[x]->GetName(),pName))
			{
			int size = 0;
			//determine size,  this is a NULL TERMINATED ARRAY OF POINTERS
			if((*ppElmnts)[0] == NULL)size = 0;
			else
				{
				Element** pPtr =  (*ppElmnts);
				while(*pPtr != NULL){size++;pPtr++;}
				}
			size++;
			(*ppElmnts) = (Element**)realloc((*ppElmnts), sizeof(Element *) * (size + 1));
			(*ppElmnts)[size -1] = ppElements[x];
			(*ppElmnts)[size] = NULL;
			
			}
		if(blSubs)ppElements[x]->FindChildElements(pName,ppElmnts,blSubs);
		}
	//return the size
	int size = 0;
	//determine size,  this is a NULL TERMINATED ARRAY OF POINTERS
	if((*ppElmnts)[0] == NULL)return 0;
	else
		{
		Element** pPtr =  (*ppElmnts);
		while(*pPtr != NULL){size++;pPtr++;}
		}
	return size;
}


Element * Element::GetChildElement(LPTSTR pName)
{
	//walk this list of child elements searching for the element with the 
	//passed in name
	for(int x=0; x<HasChildElements(); x++)
		{
		if(!_tcsicmp(ppElements[x]->GetName(),pName))return ppElements[x];
		}
	return NULL;
}

Attribute * Element::GetChildAttribute(LPTSTR pName)
{
	//walk this list of child attributes searching for the attributes with the 
	//passed in name
	for(int x=0; x<HasChildAttributes(); x++)
		{
		if(!_tcsicmp(ppAttributes[x]->GetName(),pName))return ppAttributes[x];
		}
	return NULL;
}

int Element::HasChildAttributes()
{
	int size = 0;
	//determine size,  this is a NULL TERMINATED ARRAY OF POINTERS
	if(ppAttributes == NULL)return 0;
	Attribute** pPtr =  ppAttributes;
	while(*pPtr != NULL){size++;pPtr++;}
	return size;
}

int Element::HasChildElements()
{
	int size = 0;
	//determine size,  this is a NULL TERMINATED ARRAY OF POINTERS
	if(ppElements == NULL)return 0;
	Element** pPtr =  ppElements;
	while(*pPtr != NULL){size++;pPtr++;}
	return size;
}



// implementation of the Attribute class.
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Attribute::Attribute()
{
}

Attribute::~Attribute()
{
}




void Element::DeleteChildElement(Element * pElement)
{
	Element * pTemp = NULL;
	int size = HasChildElements();
	for(int index = 0; index < size; index++)
		{
		if(ppElements[index] == pElement)
			{
			for(int x = index; x<size; x++)
				{
				ppElements[index] = ppElements[index+1];
				}
			//reallocate
			ppElements = (Element**)realloc(ppElements, sizeof(Element *) * size);
			delete pElement;
			}
		}
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCmnLib\Element.cpp_v  $
 * 
 *    Rev 1.0   18 Apr 2013 18:10:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
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
*/
