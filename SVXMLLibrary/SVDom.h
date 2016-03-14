//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDOMClass
//* .File Name       : $Workfile:   SVDom.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:47:18  $
//******************************************************************************

#ifndef SVDOM_H
#define SVDOM_H


#define SVXML MSXML2

#include "SVLibrary/SVBStr.h" 					// SVBStr

class SVXMLEncryptionClass;

class SVDOMClass  
{
public:
	SVDOMClass();
	virtual ~SVDOMClass();

// Initialize () - must be called after instantiation, and before use.
	HRESULT Initialize (long p_lUseEncryption);

// Clear () - destroy the contents of the DOM.
	HRESULT Clear ();

// SetHeader () -
//		Insert <?xml version="1.0" standalone="YES"?>
	HRESULT SetHeader ();

	HRESULT GetEncryptionInterface (SVXMLEncryptionClass** p_oppEncryptionInterface);

	HRESULT SetNameSpace (BSTR	abstrNameSpace);
	HRESULT GetNameSpace (WCHAR* awcsNameSpace, long alBufferSize);

// GetElementNbrOfChildren () -----------------------------------------------
// This method return really only the numbers of elements. To get the element children use GetFirstElementChild or GetNextElementSibling.
// ATTENTIONS: If you use GetfirstChild or GetnextSibling, you can get more children, because you can also get nodes of the type other types like TEXT.
	HRESULT	GetElementNbrOfChildren (const SVXML::IXMLDOMElementPtr aDOMElementPtr, long* alpNbrOfChildren) const;
	//************************************
	/// \param aDOMElementPtr [in] the parent node pointer
	/// \returns Return the first child node pointer of the type element and if no element found nullptr.
	//************************************
	SVXML::IXMLDOMElementPtr GetFirstElementChild(const SVXML::IXMLDOMElementPtr aDOMElementPtr) const;
	//************************************
	/// \param aDOMElementPtr [in] the sibling node pointer
	/// \returns Return the next sibling node pointer of the type element and if no element found nullptr.
	//************************************
	SVXML::IXMLDOMElementPtr GetNextElementSibling(const SVXML::IXMLDOMElementPtr aDOMElementPtr) const;

	HRESULT	SetQueryNameSpace (BSTR	abstrQueryNameSpace);
	HRESULT ClearNameSpace ();

// GetRootNode () -----------------------------------------------------------
// apDOMRootPtr - output -
//   This parameter represents the root element of the DOM tree.
	HRESULT	GetRootNode (SVXML::IXMLDOMElementPtr& arDOMRootPtr);

	HRESULT	CopyXMLFileToDOM (BSTR	abstrLoadText);

	HRESULT	CopyXMLTextToDOM (BSTR	abstrLoadText);

	HRESULT	CopyDOMToXMLFile (BSTR abstrFileName);

// CreateDOMNodeChildElement () ---------------------------------------------
// aDOMParentNodePtr	- input -
//	  This parameter is the node to which the newly created element will be
//   attached.
// abstrElementTag	- input - 
//   This parameter is the Element "type" (tag name).
// abstrElementName	- input - 
//	  This parameter is the name (attribute) to be associated with this 
//   element.  If this value is NULL, then the name attribute will not be 
//   created.
// avpElementValue		- input -
//   This parameter represents the data (value) and data type to be 
//   associated with this element.  If this input is NULL, then the 
//   attributes value and type will not be created. If data type equals 
//   VT_ARRAY, and only VT_ARRAY, then a value will not be inserted in the
//   node.
//   
//   This function will create the child element with the associated 
//	attributes, and append it to the node list of the parect node.
	HRESULT CreateDOMNodeChildElement(SVXML::IXMLDOMNodePtr aDOMParentNodePtr, BSTR abstrElementTag, BSTR abstrElementName, VARIANT* avpElementValue, SVXML::IXMLDOMNodePtr& arDOMChildElementPtr);
	HRESULT CreateDOMNodeElement(BSTR abstrElementTag, BSTR abstrElementName, VARIANT* avpElementValue, SVXML::IXMLDOMElementPtr& arDOMChildElementPtr);

// CreateElement () ---------------------------------------------------------
//	abstrElementName	- input -
//	arDOMElement		- output - 
	HRESULT CreateElement (BSTR	abstrElementName, SVXML::IXMLDOMElementPtr&	arDOMElementPtr);

	HRESULT	CreateNode (long p_lNodeType, const BSTR* p_bstrpNodeName,  const BSTR* p_bstrpNameSpace, SVXML::IXMLDOMNodePtr& p_rNewNodePtr);
	
// AppendChildToDOMDoc () ---------------------------------------------------
// aNodePtr - input - This parameter is defined as IXMLDOMNodePtr instead of 
//  IXMLDOMElementPtr in the hopes that a processing instruction may be 
//	 appended to the DOM Document (IXMLDOMProcessingInstructionPtr is not 
//	 derived from IXMLDOMElementPtr).
	HRESULT AppendChildToDOMDoc (SVXML::IXMLDOMNodePtr	aNodePtr);

// AppendChildToDOMNode () --------------------------------------------------
// aParentNodePtr - input - This parameter is defined as IXMLDOMNodePtr 
//  instead of IXMLDOMElementPtr in the hopes that the DOM Document may also 
//  be the parent node.
// aChildNodePtr - input - This parameter is defined as IXMLDOMNodePtr 
//	 instead of IXMLDOMElementPtr in the hopes that a processing instruction 
//	 may be appended to the DOM Document (IXMLDOMProcessingInstructionPtr is 
//	 not derived from IXMLDOMElementPtr).
	HRESULT AppendChildToDOMNode (SVXML::IXMLDOMNodePtr aParentNodePtr, SVXML::IXMLDOMNodePtr aChildNodePtr);

	HRESULT AppendCarriageReturnToDOMNode (SVXML::IXMLDOMNodePtr aParentNodePtr);

// GetDOMNodeElementValue () ------------------------------------------------
//  Please keep in mind that this function does not create a variant, but if 
//  the variant references a SAFEARRAY, or a BSTR (string), it will have 
//	 allocated that memory.
// 
//  This function should be able to handle DOM element that do not contain
//  Type or Value attributes.  Treat no type attribute as VT_EMPTY, and no
//  value attribute as 0.
	HRESULT GetDOMNodeElementValue (SVXML::IXMLDOMElementPtr aDOMElementPtr, VARIANT* avpValue);

// GetDOMNodeElementName () -------------------------------------------------
//  This function will create a BSTR that must be managed and freed by the  calling application.
	HRESULT GetDOMNodeElementName (SVXML::IXMLDOMElementPtr	aDOMElementPtr, BSTR* abstrpDOMElementName);

// GetDOMNodeMainText () ----------------------------------------------------
//  This function will return the first element/node of non-whitespace based  text.
	HRESULT GetDOMNodeMainText (SVXML::IXMLDOMElementPtr p_oDOMElementPtr, SVBStr* p_bstrpText);

	SVXML::IXMLDOMNodePtr GetDOMNodeNextText (SVXML::IXMLDOMNodeListPtr	p_oNodeListPtr, SVXML::IXMLDOMTextPtr p_oDOMTextPtr);

	HRESULT PreserveWhitespace(bool bPreserve);

protected:
	HRESULT Init ();
	HRESULT InitializeDOM ();

// CreateRoot () ------------------------------------------------------------
//  At this point it is believed that this function should only be accessed 
//	through InitializeDOM ().
	HRESULT CreateRootNode ();

// svmDOMPtr - smart pointer
	SVXML::IXMLDOMDocument2Ptr svmDOMPtr;

//	svmRootNode --------------------------------------------------------------
//  This variable should always mirror svmDOMPtr->documentElement.  If one is 
//	being modified, then so should the other.
	SVXML::IXMLDOMElementPtr svmRootNodePtr;

// svmbstrNameSpace ---------------------------------------------------------
	SVBStr svmbstrNameSpace;

// svmlInitialized - public methods should check this.  Protected methods will not.
//	Set by Initialized ().
	long svmlInitialized;

//	Set by Initialized ().
	long svmlCoInitialized;
	long m_lUseEncryption;
	SVXMLEncryptionClass* m_opEncryption;
};

extern const WCHAR*	g_wcsName;
extern const WCHAR*	g_wcsRoot;
extern const WCHAR*	g_wcsType;

extern const WCHAR*	g_wcsBaseNode;

extern const char*	g_csValue;
extern const char*	g_csXMLNS;

#endif

