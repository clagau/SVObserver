// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVXmlStream.cpp
// * .File Name       : $Workfile:   SVXmlStream.inl  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   23 May 2014 07:19:04  $
// ******************************************************************************

#pragma region Includes
//Moved to precompiled header: #include <fstream>

#include "SVXmlStream.h"
#include "SVOLibrary/SVOLibrary.h"
#include "SVUtilityLibrary/SVSAFEARRAY.h"
#pragma endregion Includes

static LPCTSTR EncodeString(CString& strVal)
{
	strVal.Replace(_T("&"), _T("&amp;"));
	strVal.Replace(_T("<"), _T("&lt;"));
	strVal.Replace(_T(">"), _T("&gt;"));
	return strVal;
}


template< typename SVTreeType >
SVXmlStream< SVTreeType >::SVXmlStream(SVTreeType& rTree)
: m_rTree( rTree ),
	m_Parent( nullptr ),
	m_VarType( VT_EMPTY )
{	// Constructor
	m_Parent = m_rTree.getRoot();

	// Init our members
	//
	this->AttribNames.RemoveAll();
	this->AttribValues.RemoveAll();
	this->Tag.Empty();
	this->Name.Empty();
	this->ChainTag.Empty();
	this->TextField.Empty();

	// Init MSXML members
	//
	CurrentElementNode = nullptr;
	m_plDomDocument    = nullptr;
	m_pDocRoot         = nullptr;

	attrib_index = -1;

	// -- Errors Init --
	//
	ok.Empty();
	ok = "Success";
	lasterror = ok;
}

template< typename SVTreeType >
SVXmlStream< SVTreeType >::~SVXmlStream()
{ 	// Free ressource
	//
	this->Reset_XML_Document();
}

template< typename SVTreeType >
CString & SVXmlStream< SVTreeType >::Get_LastError()
{ return lasterror; }

template< typename SVTreeType >
bool SVXmlStream< SVTreeType >::Init_MSXML()
{	lasterror = ok;

	// -- Instantiate an XML document --
	//
	HRESULT hr = m_plDomDocument.CreateInstance(MSXML2::CLSID_DOMDocument);
	if (FAILED(hr))
	{ 	_com_error er(hr);
		lasterror = er.ErrorMessage();
		return false;
	}

	return true;
}

template< typename SVTreeType >
bool SVXmlStream< SVTreeType >::Load_XML_Document(LPCTSTR strFileName)
{
	lasterror = ok;

	// Reset Document
	//
	this->Reset_XML_Document();

	// Convert xml file name string to something COM can handle (BSTR)
	//
	CString sFileName = strFileName;
	_bstr_t bstrFileName;
	bstrFileName = sFileName.AllocSysString();

	// Call the IXMLDOMDocumentPtr's load function to load the XML document
	//
	variant_t vResult;
	vResult = m_plDomDocument->load(bstrFileName);
	if (((bool)vResult) == TRUE) // success!
	{
		// Now that the document is loaded, we need to initialize the root pointer
		//
		m_pDocRoot = m_plDomDocument->documentElement;

		// Now we can Parse this document !!
		// 
		Parse_Objects(m_pDocRoot);

		this->Go_to_Root();

		return true;
	}
	else
	{	// XML document is not loaded, return error
		//
		lasterror = "XML Document FAILED to load";
		return false;
	}
}

template< typename SVTreeType >
void SVXmlStream< SVTreeType >::Parse_XML_Document()
{
	CString		tag, name, type;

	tag = Get_CurrentTag();

	if (Is_Having_Attribute(_T("name")))
		name = Get_Attribute_Value();
	
	if (Is_Having_Attribute(_T("type")))
		type = Get_Attribute_Value();

	if (tag == _T("<Configuration>"))
	{
		m_rTree.Clear();

		m_Parent = m_rTree.getRoot();
	}

	if (tag == _T("<NODE>"))
	{
		if( !( m_LeafName.IsEmpty() ) )
		{
			m_rTree.createLeaf( m_Parent, m_LeafName, m_Variant );

			m_LeafName.Empty();
			m_Variant.Clear();
		}

		m_rTree.createBranch( m_Parent, name, &m_Parent );
	}
	else 	if (tag == _T("<DATA>"))
	{
		TCHAR	*stopstring;

		if( !( m_LeafName.IsEmpty() ) )
		{
			m_rTree.createLeaf( m_Parent, m_LeafName, m_Variant );

			m_LeafName.Empty();
			m_Variant.Clear();
		}

		m_LeafName = name;
		m_VarType = static_cast< VARTYPE >( _tcstoul( type, &stopstring, 0 ) );
	}
	else 	if (tag == _T("<VALUE>"))
	{
		if( m_VarType & VT_ARRAY)
		{
			mhr = S_OK;
			SAFEARRAYBOUND	sabData;

			if (nullptr == V_ARRAY(&m_Variant) || VT_EMPTY == m_Variant.vt)
			{
				sabData.cElements = 1;
				sabData.lLbound   = 0;
				V_ARRAY(&m_Variant)     = SafeArrayCreate(m_VarType & VT_TYPEMASK, 1, &sabData);
			}
			else
			{
				sabData.cElements = V_ARRAY(&m_Variant)->rgsabound[0].cElements + 1;
				sabData.lLbound   = V_ARRAY(&m_Variant)->rgsabound[0].lLbound;
				mhr = SafeArrayRedim(V_ARRAY(&m_Variant), &sabData);
			}
		}
	}
	else if (tag == _T("<#TEXT#>"))
	{
		TCHAR		*stopstring;
	
		switch (m_VarType)
		{
			case VT_BOOL:
			case VT_BOOL | VT_ARRAY:
			{
				bool l_Val = Get_TextValue().CompareNoCase(_T("true")) == 0;
				if (m_VarType & VT_ARRAY)
				{
					BOOL boolVal = ( l_Val ) ? VARIANT_TRUE : VARIANT_FALSE;

					long lIndex = V_ARRAY(&m_Variant)->rgsabound[0].cElements - 1;
					mhr = SafeArrayPutElement(V_ARRAY(&m_Variant), &lIndex, &boolVal);
					if (S_OK == mhr && VT_EMPTY == m_Variant.vt)
					{
						m_Variant.vt = VT_BOOL | VT_ARRAY;
					}
				}
				else
					m_Variant = l_Val;
				break;
			}
			case VT_UI1:
			case VT_UI1 | VT_ARRAY:
			{
				BYTE bVal = (BYTE)_tcstoul(Get_TextValue(), &stopstring, 0);
				if (m_VarType & VT_ARRAY)
				{
					long lIndex = V_ARRAY(&m_Variant)->rgsabound[0].cElements - 1;
					mhr = SafeArrayPutElement(V_ARRAY(&m_Variant), &lIndex, &bVal);
					if (S_OK == mhr && VT_EMPTY == m_Variant.vt)
					{
						m_Variant.vt = VT_UI1 | VT_ARRAY;
					}
				}
				else
					m_Variant = bVal;
				break;
			}
			case VT_UI2:
			case VT_UI2 | VT_ARRAY:
			{
				USHORT uiVal = (USHORT)_tcstoul(Get_TextValue(), &stopstring, 0);
				if (m_VarType & VT_ARRAY)
				{
					long lIndex = V_ARRAY(&m_Variant)->rgsabound[0].cElements - 1;
					mhr = SafeArrayPutElement(V_ARRAY(&m_Variant), &lIndex, &uiVal);
					if (S_OK == mhr && VT_EMPTY == m_Variant.vt)
					{
						m_Variant.vt = VT_UI2 | VT_ARRAY;
					}
				}
				else
					m_Variant = uiVal;
				break;
			}
			case VT_UI4:
			case VT_UI4 | VT_ARRAY:
			{
				ULONG ulVal = _tcstoul(Get_TextValue(), &stopstring, 0);
				if (m_VarType & VT_ARRAY)
				{
					long lIndex = V_ARRAY(&m_Variant)->rgsabound[0].cElements - 1;
					mhr = SafeArrayPutElement(V_ARRAY(&m_Variant), &lIndex, &ulVal);
					if (S_OK == mhr && VT_EMPTY == m_Variant.vt)
					{
						m_Variant.vt = VT_UI4 | VT_ARRAY;
					}
				}
				else
					m_Variant = ulVal;
				break;
			}
			case VT_UINT:
			case VT_UINT | VT_ARRAY:
			{
				UINT uintVal = _tcstoul(Get_TextValue(), &stopstring, 0);
				if (m_VarType & VT_ARRAY)
				{
					long lIndex = V_ARRAY(&m_Variant)->rgsabound[0].cElements - 1;
					mhr = SafeArrayPutElement(V_ARRAY(&m_Variant), &lIndex, &uintVal);
					if (S_OK == mhr && VT_EMPTY == m_Variant.vt)
					{
						m_Variant.vt = VT_UINT | VT_ARRAY;
					}
				}
				else
					m_Variant = uintVal;
				break;
			}
			case VT_I1:
			case VT_I1 | VT_ARRAY:
			{
				char cVal = Get_TextValue().GetAt(0);
				if (m_VarType & VT_ARRAY)
				{
					long lIndex = V_ARRAY(&m_Variant)->rgsabound[0].cElements - 1;
					mhr = SafeArrayPutElement(V_ARRAY(&m_Variant), &lIndex, &cVal);
					if (S_OK == mhr && VT_EMPTY == m_Variant.vt)
					{
						m_Variant.vt = VT_I1 | VT_ARRAY;
					}
				}
				else
					m_Variant = cVal;
				break;
			}
			case VT_I2:
			case VT_I2 | VT_ARRAY:
			{
				short iVal = (short)_tcstol(Get_TextValue(), &stopstring, 0);
				if (m_VarType & VT_ARRAY)
				{
					long lIndex = V_ARRAY(&m_Variant)->rgsabound[0].cElements - 1;
					mhr = SafeArrayPutElement(V_ARRAY(&m_Variant), &lIndex, &iVal);
					if (S_OK == mhr && VT_EMPTY == m_Variant.vt)
					{
						m_Variant.vt = VT_I2 | VT_ARRAY;
					}
				}
				else
					m_Variant = iVal;
				break;
			}
			case VT_I4:
			case VT_I4 | VT_ARRAY:
			{
				long lVal = _tcstoul(Get_TextValue(), &stopstring, 0);
				if (m_VarType & VT_ARRAY)
				{
					long lIndex = V_ARRAY(&m_Variant)->rgsabound[0].cElements - 1;
					mhr = SafeArrayPutElement(V_ARRAY(&m_Variant), &lIndex, &lVal);
					if (S_OK == mhr && VT_EMPTY == m_Variant.vt)
					{
						m_Variant.vt = VT_I4 | VT_ARRAY;
					}
				}
				else
					m_Variant = lVal;
				break;
			}
			case VT_INT:
			case VT_INT | VT_ARRAY:
			{
				int intVal = _tcstol(Get_TextValue(), &stopstring, 0);
				if (m_VarType & VT_ARRAY)
				{
					long lIndex = V_ARRAY(&m_Variant)->rgsabound[0].cElements - 1;
					mhr = SafeArrayPutElement(V_ARRAY(&m_Variant), &lIndex, &intVal);
					if (S_OK == mhr && VT_EMPTY == m_Variant.vt)
					{
						m_Variant.vt = VT_INT | VT_ARRAY;
					}
				}
				else
					m_Variant = intVal;
				break;
			}
			case VT_R4:
			case VT_R4 | VT_ARRAY:
			{
				float fltVal = (float)_tcstod(Get_TextValue(), &stopstring);
				if (m_VarType & VT_ARRAY)
				{
					long lIndex = V_ARRAY(&m_Variant)->rgsabound[0].cElements - 1;
					mhr = SafeArrayPutElement(V_ARRAY(&m_Variant), &lIndex, &fltVal);
					if (S_OK == mhr && VT_EMPTY == m_Variant.vt)
					{
						m_Variant.vt = VT_R4 | VT_ARRAY;
					}
				}
				else
					m_Variant = fltVal;
				break;
			}
			case VT_R8:
			case VT_R8 | VT_ARRAY:
			{
				double dblVal = _tcstod(Get_TextValue(), &stopstring);
				if (m_VarType & VT_ARRAY)
				{
					long lIndex = V_ARRAY(&m_Variant)->rgsabound[0].cElements - 1;
					mhr = SafeArrayPutElement(V_ARRAY(&m_Variant), &lIndex, &dblVal);
					if (S_OK == mhr && VT_EMPTY == m_Variant.vt)
					{
						m_Variant.vt = VT_R8 | VT_ARRAY;
					}
				}
				else
					m_Variant = dblVal;
				break;
			}
			case VT_BSTR:
			case VT_BSTR | VT_ARRAY:
			{
				BSTR bstrVal = Get_TextValue().AllocSysString();
				if (m_VarType & VT_ARRAY)
				{
					long lIndex = V_ARRAY(&m_Variant)->rgsabound[0].cElements - 1;
					mhr = SafeArrayPutElement(V_ARRAY(&m_Variant), &lIndex, &bstrVal);
					SysFreeString( bstrVal );
					if (S_OK == mhr && VT_EMPTY == m_Variant.vt)
					{
						m_Variant.vt = VT_BSTR | VT_ARRAY;
					}
				}
				else
					m_Variant = bstrVal;
				break;
			}
			default:
				VariantClear(&m_Variant);
				break;
		}
	}
}

template< typename SVTreeType >
void SVXmlStream< SVTreeType >::Parse_Objects(MSXML2::IXMLDOMNodePtr pObject)
{
	// Child node
	//
	MSXML2::IXMLDOMNodePtr pChild;

	// Grab Informations from the pObject node
	//
	this->Grab_Node_Informations(pObject);

	// Update "CurrentElementNode"
	//
	CurrentElementNode = pObject;

	// Call User Parse Handling for let him what he want.
	//
	this->Parse_XML_Document();

	if(!Is_MSXML_Node(pObject))
		return;

	// add the ChainTag
	//
	this->ChainTag_Add(this->Tag);

	for (pChild = pObject->firstChild; nullptr != pChild;  pChild = pChild->nextSibling)
	{
		// Parse Child nodes
		//
		this->Parse_Objects(pChild);
	}

	// Remove Current Tag from ChainTag
	//
	ChainTag_Remove(1);
}

template< typename SVTreeType >
bool SVXmlStream< SVTreeType >::Is_Tag(LPCTSTR aTag)
{ return this->Get_CurrentTag() == aTag; }

template< typename SVTreeType >
bool SVXmlStream< SVTreeType >::Is_TextNode()
{ return (this->Is_Tag("<#TEXT#>")); }

template< typename SVTreeType >
CString & SVXmlStream< SVTreeType >::Get_CurrentTag()
{	return this->Tag; }

template< typename SVTreeType >
CString & SVXmlStream< SVTreeType >::Get_CurrentName()
{  return this->Name; }

template< typename SVTreeType >
bool SVXmlStream< SVTreeType >::Is_Root()
{   return (this->ChainTag.IsEmpty() != 0); }

template< typename SVTreeType >
bool SVXmlStream< SVTreeType >::Is_Child_of(LPCTSTR parent_chain)
{
 	int pos = this->ChainTag.Find(parent_chain);
	int len = strlen(parent_chain);

	if(pos != -1)
	{	// look if it's the end of this chainTag
		//
		return ( this->ChainTag.GetLength() == pos + len );
	}

	return false;
}

template< typename SVTreeType >
CString & SVXmlStream< SVTreeType >::Get_TextValue()
{ return this->TextField; }

template< typename SVTreeType >
bool SVXmlStream< SVTreeType >::Is_MSXML_Node(MSXML2::IXMLDOMNodePtr pChild)
{  return (pChild->nodeType != MSXML2::NODE_TEXT); }

template< typename SVTreeType >
void SVXmlStream< SVTreeType >::Grab_Node_Informations(MSXML2::IXMLDOMNodePtr pChild)
{
	if(this->Is_MSXML_Node(pChild))
	{	// It's a node
		//

		// Tag Update
		//
		Tag.Format("<%s>",(const char*)(pChild->nodeName));
		Name = (const char*)(pChild->nodeName);

		// TextField no significant value
		TextField.Empty();

		// Update Attribute List
		//
		this->AttribNames.RemoveAll();
		this->AttribValues.RemoveAll();

		MSXML2::IXMLDOMNamedNodeMapPtr pAttribs = pChild->Getattributes();
		if(nullptr != pAttribs)
		{ 	long nAttriCnt = pAttribs->Getlength();
			// Iterate over the attributes
			//
			for(int i=0; i<nAttriCnt; ++i)
			{  MSXML2::IXMLDOMNodePtr pAttrib = pAttribs->Getitem(i);
			   if(nullptr != pAttrib)
			   {
					this->AttribNames.Add(	(CString)(const char *) pAttrib->GetnodeName() );

					_variant_t vVal = pAttrib->GetnodeValue ( );
					this->AttribValues.Add( (CString)(const char *) _bstr_t(vVal) );
			   }
			}
		}
	}
	else
	{   // Tag is #TEXT#
		//
		Tag.Empty();
		Tag = "<#TEXT#>";
		Name.Empty();
		Name = "#TEXT#";

		// TextField Update
		//
		TextField = (const char*)(pChild->text);

		// Update Attribute List have no means
		//
		this->AttribNames.RemoveAll();
		this->AttribValues.RemoveAll();
	}

	return;
}

template< typename SVTreeType >
void SVXmlStream< SVTreeType >::ChainTag_Add(CString & val)
{
		// Add a tag to the ChainTag
		//
		if(!ChainTag.IsEmpty())
			ChainTag += val;
		else
			ChainTag = val;
}

template< typename SVTreeType >
void SVXmlStream< SVTreeType >::ChainTag_Remove(int number)
{
		// Remove the n tag to the ChainTag
		//
		for(int bcl = 0; bcl < number; bcl ++)
		{
			CString EndTag;
			int pos = ChainTag.ReverseFind('<');
			if(pos == -1)
			{
				ChainTag.Empty();
			}
			else
			{
				EndTag = ChainTag.Mid(pos);
				ParseEndTag(EndTag);
				ChainTag = ChainTag.Left(pos);
			}
		}
}

template< typename SVTreeType >
bool SVXmlStream< SVTreeType >::Is_Having_Attribute(LPCTSTR Name)
{
   	// Create the CString Name Object
   	//
   	CString sName = Name;

   	// Clear attribute index
   	//
   	attrib_index = -1;

 	int bcl;
  	for(bcl = 0; bcl < this->AttribNames.GetSize() ; bcl++)
   	{	// Check if the name is equal
   		//
   		if(this->AttribNames[bcl] == sName)
   		{ // set index fot let user to retrieve value with "Get_Attribute_Value()" method
   		  //
   		  attrib_index = bcl;
   		  return true;
   		}
   	}
   	return false;
}

template< typename SVTreeType >
CString & SVXmlStream< SVTreeType >::Get_Attribute_Value()
{  // Assume Success
   //
   lasterror = ok;

   if(attrib_index != -1)
	 return this->AttribValues[attrib_index];

   // We can't retrieve a Attribute values
   //
   lasterror = "Can't Retrieve an Attribute";
   return lasterror;
}

template< typename SVTreeType >
bool SVXmlStream< SVTreeType >::Add_LastChildNode(LPCTSTR Name)
{
	// Nodes members
	// IXMLDOMElementPtr CurrentElementNode;	/* Global Member */
	//
	MSXML2::IXMLDOMNodePtr newNode;

	// Create NODE TYPE
	//
	CComVariant vType(MSXML2::NODE_ELEMENT);

	// Create the NODE
	//
	newNode = m_plDomDocument->createNode(vType,Name,"");

	// Attach the Node to the document
	//
	if(nullptr != CurrentElementNode)
	{	this->ChainTag_Add( this->Get_CurrentTag() );
		newNode = CurrentElementNode->appendChild(newNode);
	}
	else if(nullptr != m_pDocRoot)
	{	this->ChainTag_Add( (CString)(const char*)m_pDocRoot->nodeName );
		newNode = m_pDocRoot->appendChild(newNode);
	}
	else
	{	this->ChainTag.Empty();
		m_pDocRoot = newNode;
		m_plDomDocument->documentElement = m_pDocRoot;
	}

	// Update Current Node (cast operation)
	//
	CurrentElementNode = newNode;

	// Update information for this Node
	//
	this->Grab_Node_Informations(CurrentElementNode);

	return true;
}

template< typename SVTreeType >
bool SVXmlStream< SVTreeType >::Add_NodeBefore(LPCTSTR Name)
{	// Nodes members
	// IXMLDOMElementPtr CurrentElementNode;	/* Global Member */
	//
	MSXML2::IXMLDOMNodePtr newNode;
	
	// Can't use this function on the root node
	//
	if(this->Is_Root())
		return false;
	// Create NODE TYPE
	//
	CComVariant vType(MSXML2::NODE_ELEMENT);

	// Create the NODE
	//
	newNode = m_plDomDocument->createNode(vType,Name,"");

	if(this->Is_Root())
		return false;

	// Create Reference Node for the Insertion
	//
	MSXML2::IXMLDOMNodePtr pParent;
	this->CurrentElementNode->get_parentNode(&pParent);
	_variant_t NodeRef = (IUnknown *)CurrentElementNode;
	

	// Attach the Node to the document
	//
	if(nullptr != CurrentElementNode)
		newNode = pParent->insertBefore(newNode,NodeRef);
	else if(nullptr != m_pDocRoot)
		newNode = m_pDocRoot->insertBefore(newNode,NodeRef);
	else
	{	m_pDocRoot = newNode;
		m_plDomDocument->documentElement = m_pDocRoot;
	}

	// Update Current Node (cast operation)
	//
	CurrentElementNode = newNode;

	// Update information for this Node
	//
	this->Grab_Node_Informations(CurrentElementNode);

	return true;
}

template< typename SVTreeType >
bool SVXmlStream< SVTreeType >::Add_NodeAfter(LPCTSTR Name)
{	// Nodes members
	// IXMLDOMElementPtr CurrentElementNode;	/* Global Member */
	//
	MSXML2::IXMLDOMNodePtr newNode;
	
	// Can't use this function on the root node
	//
	if(this->Is_Root())
		return false;

	// Create NODE TYPE
	//
	CComVariant vType(MSXML2::NODE_ELEMENT);

	// Create the NODE
	//
	newNode = m_plDomDocument->createNode(vType,Name,"");

	if(this->Is_Root())
		return false;

	// If nullptr == CurrentElementNode->NextSibling then must call Add_LastChildNode on Parent Node
	// Because we can't use InsertBefore on a NULL Reference ;o)
	//
	// We are sure that a Parent node exist because before we control that we aren't not on root node.
	//
	if(nullptr == CurrentElementNode->nextSibling)
	{	// Get Parent Node
		//
		MSXML2::IXMLDOMNodePtr pParent;
		this->CurrentElementNode->get_parentNode(&pParent);

		// Set Parent node as Current Node
		//
		this->CurrentElementNode = pParent;
		this->Grab_Node_Informations(pParent);

		// Add Node as Last Child Node
		//
		return (this->Add_LastChildNode(Name));
	}

	// Create Reference Node for the Insertion
	//
	MSXML2::IXMLDOMNodePtr pParent;
	this->CurrentElementNode->get_parentNode(&pParent);
	_variant_t NodeRef = (IUnknown *)CurrentElementNode->nextSibling;
	

	// Attach the Node to the document
	//
	if(nullptr != CurrentElementNode)
		newNode = pParent->insertBefore(newNode,NodeRef);
	else if(nullptr != m_pDocRoot)
		newNode = m_pDocRoot->insertBefore(newNode,NodeRef);
	else
	{	m_pDocRoot = newNode;
		m_plDomDocument->documentElement = m_pDocRoot;
	}

	// Update Current Node (cast operation)
	//
	CurrentElementNode = newNode;

	// Update information for this Node
	//
	this->Grab_Node_Informations(CurrentElementNode);

	return true;
}

template< typename SVTreeType >
bool SVXmlStream< SVTreeType >::Add_FirstChildNode(LPCTSTR Name)
{	// Nodes members
	// IXMLDOMElementPtr CurrentElementNode;	/* Global Member */
	//
	MSXML2::IXMLDOMNodePtr newNode;
	//IXMLDOMNodeListPtr ChildList;

	// If no child then use Add_LastChildNode or CurrentElement node not set yet
	//
	if(nullptr != CurrentElementNode)
	{	if( !this->CurrentElementNode->hasChildNodes() )
			return (this->Add_LastChildNode(Name));
	}
	else
		return (this->Add_LastChildNode(Name)); 

	// Create NODE TYPE
	//
	CComVariant vType(MSXML2::NODE_ELEMENT);

	// Create the NODE
	//
	newNode = m_plDomDocument->createNode(vType,Name,"");

	// Create Reference Node for the Insertion
	//
	_variant_t NodeRef = (IUnknown *)CurrentElementNode->firstChild;

	// Attach the Node to the document
	//
	if(nullptr != CurrentElementNode)
	{	this->ChainTag_Add( this->Get_CurrentTag() );
		newNode = CurrentElementNode->insertBefore(newNode,NodeRef);
	}
	else if(nullptr != m_pDocRoot)
	{	this->ChainTag_Add( (CString)(const char*)m_pDocRoot->nodeName );
		newNode = m_pDocRoot->insertBefore(newNode,NodeRef);
	}
	else
	{	this->ChainTag.Empty();
		m_pDocRoot = newNode;
		m_plDomDocument->documentElement = m_pDocRoot;
	}

	// Update Current Node (cast operation)
	//
	CurrentElementNode = newNode;

	// Update information for this Node
	//
	this->Grab_Node_Informations(CurrentElementNode);

	return true;
}

template< typename SVTreeType >
bool SVXmlStream< SVTreeType >::Set_Attribute(LPCTSTR AttribName,LPCTSTR AttribValue)
{
	// Nodes members
	// IXMLDOMElementPtr CurrentElementNode;	/* Global Member */
	//

	// Create Attribute variable
	//
	CComVariant sAttribute(AttribValue);

	// Set the new attribute
	//
	if(nullptr != CurrentElementNode)
	{
		CurrentElementNode->setAttribute( AttribName, sAttribute );
		this->Grab_Node_Informations(CurrentElementNode);
	}
	else
	{	m_pDocRoot->setAttribute( AttribName, sAttribute );
		this->Grab_Node_Informations(m_pDocRoot);
	}

	// Return
	//
	return true;
}

template< typename SVTreeType >
bool SVXmlStream< SVTreeType >::Remove_Attribute(LPCTSTR AttribName)
{
	if(nullptr == CurrentElementNode)
	{	if(nullptr != this->m_pDocRoot)
			this->CurrentElementNode = this->m_pDocRoot;
		else
			return false;
	}

	this->CurrentElementNode->removeAttribute(AttribName);
	return true;
}

template< typename SVTreeType >
bool SVXmlStream< SVTreeType >::Set_Text(LPCTSTR TextValue)
{
	// Nodes members
	// IXMLDOMElementPtr CurrentElementNode;	/* Global Member */
	//
	MSXML2::IXMLDOMNodePtr newNode;

	// Create NODE TEXT type
	//
	CComVariant vType(MSXML2::NODE_TEXT);

	// Create the node
	//
	newNode = m_plDomDocument->createTextNode(TextValue);

	// Find if a CHILD TEXT NODE exist or not
	//
	if(nullptr == CurrentElementNode)
	{	if(nullptr != this->m_pDocRoot)
			this->CurrentElementNode = this->m_pDocRoot;
		else
			return false;
	}

	if(this->CurrentElementNode->hasChildNodes())
	{	
		MSXML2::IXMLDOMNodePtr pChild;
		for(pChild = this->CurrentElementNode->firstChild; nullptr != pChild; pChild = pChild->nextSibling)
		{	// Find it's a NODE TEXT
			//
			if(!this->Is_MSXML_Node(pChild))
			{	// A Text Node is found, Replace it now!!
				//
				this->CurrentElementNode->replaceChild(newNode,pChild);
				return true;
			}
		}
	}
		
	// No previous Text was defined before, we can add it.
	//

	// Attach the Node to the document
	//
	newNode = CurrentElementNode->appendChild(newNode);
	return true;
}

template< typename SVTreeType >
void SVXmlStream< SVTreeType >::Reset_XML_Document()
{
	// Init
	//
	this->Init_MSXML();
	m_plDomDocument->loadXML("<?xml version=\"1.0\" standalone=\"yes\" ?>");
	m_pDocRoot = m_plDomDocument->documentElement;
	CurrentElementNode = nullptr;

	// Init our members
	//
	this->AttribNames.RemoveAll();
	this->AttribValues.RemoveAll();
	this->Tag.Empty();
	this->Name.Empty();
	this->ChainTag.Empty();
	this->TextField.Empty();
	attrib_index = -1;

}

template< typename SVTreeType >
void SVXmlStream< SVTreeType >::Go_to_Root() 
{ this->CurrentElementNode = this->m_pDocRoot;
  this->ChainTag.Empty();
}

template< typename SVTreeType >
bool SVXmlStream< SVTreeType >::Go_to_Child(LPCTSTR NodeName)
{
	// Child node
	//
	MSXML2::IXMLDOMNodePtr pChild;

	if(!Is_MSXML_Node(this->CurrentElementNode))
		return false;

	ChainTag_Add(this->Get_CurrentTag());

	for (pChild = CurrentElementNode->firstChild; nullptr != pChild;  pChild = pChild->nextSibling)
	{
		// Grab Information from Child node
		//
		this->Grab_Node_Informations(pChild);

		if(this->Get_CurrentName() == NodeName)
		{	// Update new Position
			//
			this->CurrentElementNode = pChild;
			return true; 
		}
	}

	// Node Not Found (Restore Node informations)
	//
	this->ChainTag_Remove(1);
	this->Grab_Node_Informations(this->CurrentElementNode);
	return false;
}
	
template< typename SVTreeType >
bool SVXmlStream< SVTreeType >::Go_to_Parent(LPCTSTR NodeName)			
{
	// Parent node
	//
	MSXML2::IXMLDOMNodePtr pParent = nullptr;
   MSXML2::IXMLDOMNodePtr oldCurrent = this->CurrentElementNode;
	
	if(!Is_MSXML_Node(this->CurrentElementNode))
		return false;

	if(this->CurrentElementNode == this->m_pDocRoot)
		return false;

	CString oldChainTag = this->ChainTag;

	
	this->CurrentElementNode->get_parentNode(&pParent);
	
	while (true)
	{	
		this->CurrentElementNode = pParent;
		this->ChainTag_Remove(1);
		this->Grab_Node_Informations(this->CurrentElementNode);
		if(this->Get_CurrentName() == NodeName)
			return true;

		if(this->CurrentElementNode == this->m_pDocRoot)
			break;

		this->CurrentElementNode->get_parentNode(&pParent);
	}
	
	// Parent not found
	//
	this->CurrentElementNode = oldCurrent;
	this->ChainTag = oldChainTag;
	this->Grab_Node_Informations(this->CurrentElementNode);
	return false;
}

// Go to a Node attached at the same Node than the Current Node (Forward sens)
//
template< typename SVTreeType >
bool SVXmlStream< SVTreeType >::Go_to_Forward(LPCTSTR NodeName)
{
	// Sibling node
	//
	MSXML2::IXMLDOMNodePtr pSibling = nullptr;

	if(!Is_MSXML_Node(this->CurrentElementNode))
		return false;

	for (pSibling= CurrentElementNode;  true ;  pSibling = pSibling->nextSibling)
	{
		if(nullptr == pSibling)
			break;

		// Grab Information from Sibling node
		//
		this->Grab_Node_Informations(pSibling);

		if(this->Get_CurrentName() == NodeName)
		{	// Update new Position
			//
			this->CurrentElementNode = pSibling;
			return true; 
		}
	}

	// Node Not Found (Restore Node informations)
	//
	this->Grab_Node_Informations(this->CurrentElementNode);
	return false;
}
	
template< typename SVTreeType >
bool SVXmlStream< SVTreeType >::Go_to_Back(LPCTSTR NodeName)
{
	// Sibling node
	//
	MSXML2::IXMLDOMNodePtr pSibling;

	if(!Is_MSXML_Node(this->CurrentElementNode))
		return false;

	for (pSibling = CurrentElementNode; nullptr != pSibling;  pSibling = pSibling->previousSibling)
	{
		// Grab Information from Sibling node
		//
		this->Grab_Node_Informations(pSibling);

		if(this->Get_CurrentName() == NodeName)
		{	// Update new Position
			//
			this->CurrentElementNode = pSibling;
			return true; 
		}
	}

	// Node Not Found (Restore Node informations)
	//
	this->Grab_Node_Informations(this->CurrentElementNode);
	return false;
}
	
template< typename SVTreeType >
bool SVXmlStream< SVTreeType >::Remove()
{	
	// Parent node
	//
	MSXML2::IXMLDOMNodePtr pParent = nullptr;

	if(nullptr == CurrentElementNode)
	{	if(nullptr != this->m_pDocRoot)
			this->CurrentElementNode = this->m_pDocRoot;
		else
			return false;
	}

	if(this->CurrentElementNode != this->m_pDocRoot)
	{	this->CurrentElementNode->get_parentNode(&pParent);
		pParent->removeChild(this->CurrentElementNode);
		this->CurrentElementNode = pParent;
		this->Grab_Node_Informations(this->CurrentElementNode);
	}
	else
		this->Reset_XML_Document();

	return true;
}

template< typename SVTreeType >
bool SVXmlStream< SVTreeType >::RemoveChild(LPCTSTR NodeName)
{
	// Find the Child
	//
	if( this->Go_to_Child(NodeName) )
		return this->Remove(); 
	else
		return false;
}

template< typename SVTreeType >
void SVXmlStream< SVTreeType >::ParseEndTag(CString& endtag)
{
	if (endtag == _T("<NODE>"))
	{
		if( !( m_LeafName.IsEmpty() ) )
		{
			m_rTree.createLeaf( m_Parent, m_LeafName, m_Variant );

			m_LeafName.Empty();
			m_Variant.Clear();
		}

		m_Parent = m_rTree.getParentBranch( m_Parent );
	}
}


template< typename SVTreeType >
void SVXmlStream< SVTreeType >::ReleaseTreeItemData( SVTreeType& Tree )
{
	Tree.Clear();
}

