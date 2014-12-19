//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVXMLClass
//* .File Name       : $Workfile:   SVXMLClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   19 Dec 2014 04:44:28  $
//******************************************************************************

#ifndef SVXMLCLASS_H
#define SVXMLCLASS_H

#pragma warning( disable: 4786 )	// identifier truncation 255 chars

#include <typeinfo>
#include "SVXMLBase.h"
#include "SVXMLTree.h"

class SVXMLEncryptionClass;

// SVXMLClass ---------------------------------------------------------------
// The SVXMLClass puts many application based restrictions on the XML 
//	implementation.  
//	Some of the rules are as follows:
//
//	The XML document will always contain the defining schema which will always
//		be the same for all XML files.  
//
//	The XML document can contain a section for revision history.
// 
//	This XML element types will be Node and Data.  
//		A Node element may contain other Node elements or Data elements.  A 
//			Node element may not contain data attributes.  A Node element may,
//			however, contain a type attribute.  The base elements of arrays 
//			are examples of nodes that will contain type information.
//		A Data element may only contain data and will not contain any 
//			sub-elements.
//
//	XREF: SVR Node/Data rules
//	Our rule for (SVR) nodes and (SVR) data is that a node contains only 
//	   children (nodes or data), and data contains only data (no children). 
//
//	There is an exception to every rule.  Our exception is arrays 
//  	(SAFEARRAYs).  The base SAFEARRAY node will be named the same as the 
//  	VARIANT which contained the SAFEARRAY.  The base SAFEARRAY node will 
//  	contain VT_ARRAY as type.  
//	 The base SAFEARRAY will contain children nodes elements that represent 
//	   each dimention.  The Dimention nodes will contain the name 
// 	Dimention_x, where x represents the dimention number. Each of these 
//    nodes will be of type g_lSVArrayNode(SV_ARRAYNODE). Each dimention 
//	   node will contain the array elements as child data elements.  The 
// 	array elements will be labelled Index_x, and will contain the type and
//	   value information reflecting the array element value and type.  
//
//	 If the tree element contained data and children, then a node element is
//	   generated, and a child data element is generated.  The parent node will
//    contain the node type identifier SV
//	 If the tree element contained only data, then a data element is 
//	   generated.
//	 If the tree element contained only children, then a node element is 
// 	generated.
//
//
// Additional word regarding the MSXML Parser. 
// There are actually 3 separate standards that are being used here.
// They are...
//    XML,
//    XDR, and
//		XPath
//
// XML is the base level Extended Markup Language standard.  Our document 
//    must be XML compliant to be read by an XML parser.  SVXML is 
//    responsible for initially creating records in an XML compliant order,
//    but for the most part, the MSXML parser takes care of making sure that
//		individual records are written in an XML complient manor.
// 
// XDR is an XML schema Definition Language.  This is the inline schema 
//    Definition Record standard that is supported by MSXML.  There are 
//    others such as DCD. This is Microsofts flavor, and the one we are 
//    complient with. Including an XDR schema in your XML document forces
//    a number of rules on your XML document that aren't required by XML.
//    Once you've placed an XDR schema in your document, and an XDR compliant
//    parser try's to read it, your document must comply with the XDR rules 
//    in order to be loaded. 
//
//    Complications here include:
//		  In order to implement schemas, XDR uses namespaces to contain the 
//      keywords that the parser reads to recognise the schema.  Using 
//      namespaces in XML opens a new can of worms.  The MSXML parser 
//      paticulatly has some issues which I will attempt to sum up.  IN
//      GENERAL (the may be specific exceptions that I don't understand),
//      once a namespace has been defined, all sub elements (all branches, 
//      all leaves) must contain the same namespace.  If this is done by 
//		  explicitly creating attributes, it will work, but it will show up in 
//      the final XML document.  If this is done by passing the namespace 
//		  into the createNode () function, it also works, but does not show
//      up in the sub elements in the final XML document (it is expected that
//      the namespace not be explicitely specified for each sub element).
//      This means we can not use the function createElement () and must use
//      createNode () instead.
//
//	XPath defines a query language similar in function to SQL.  This did not 
//		impose requirements on our XML document. However it did take quite a bit of 
//    effort to work out.  By defaut, the MSXML parser query functions use 
//		XSLPattern (an earlier Microsoft XSL extension). The programmer must 
//    switch to XPath (which is a more powerful, and a more accepted 
//    standard).  This is done with the function 
//    setProperty ("SelectionLanguage","XPath").
//
//    Even though the XPath functionallity is part of the MSXML parser, 
//    namespaces that are defined within the DOM, must be redefined for the 
//    XPath query functions.  This is done with the function 
//    setProperty("SelectionNamespaces", <ccvProperty>).  The query language
//    does not recognise default namespaces, but does recognise no 
//    namespaces.  Since default namespaces do not have a label in the DOM
//    or XML document, you can program a dummy namesspace label when 
//    registerred with setProperty (), and use the dummy namespace in the 
//		desired query.  
//
//
// All of these standards are as implemented by Microsoft for the sake of 
//    making our document behave correctly in SVObserver (which uses MSXML 
//		Parser, and getting it to load correctly in Microsofts XML Notepad 
//    (special version of Notepad), and Microsofts Internet Explorer.

class SVXMLClass : public SVXMLBase
{
public:
	SVXMLClass();

	virtual ~SVXMLClass();

	HRESULT Initialize(long p_lUseCheckSums = 0, long p_lUseFileVersionInfo = 0, long p_lUseEncryption = 0);

// CopyTreeToXMLFile --------------------------------------------------------
//		A tree must previously have been attached using AttachTree ().
//		This function references copying the data from one external object (the 
//			Tree), to a second external object (the File). 
//		This function will modify the existing DOM. Anything Currently in the 
//			DOM will be destroyed.  A new DOM will be generated which will 
//			reflect the contents of the Tree.
//
// alSVOCurrentVersion - input -
//   This parameter represents the version of the current format. This 
//   will often be the application version number.
//
// abstrFormat - input -
//   This parameter represents the current file format.  This will often be 
//	  the name of the the application that generates the file.
//   
// abstrRevisionHistory - input -
//   This parameter should contain the XML string which represents the 
//   revision history as it was last pulled from the XML file. This revision 
//   history excludes this SVObserver instance.  The current SVObserver 
//   instance should not be added to the list until the actual save.
//
//	  This parameter can be NULL if there is no revision history.
//
// abstrFileName - input -
//   
	template <typename SVT_TREE>
	HRESULT CopyTreeToXMLFile(SVT_TREE& rTree, long alSVOCurrentVersion, BSTR abstrFormat, BSTR abstrRevisionHistory, BSTR abstrFileName);

// CopyXMLFileToTree () -----------------------------------------------------
//		A tree must previously have been attached using AttachTree ().
//		This function references copying the data from one external object (the 
//			File), to a second external object (the Tree). 
//		This function will modify the existing DOM. Anything Currently in the 
//			DOM will be destroyed.  A new DOM will be generated which will 
//			reflect the contents of the XML File.
//
// alSVOCurrentVersion - input -
//   This parameter represents the current configuration file format. This 
//   value will have come from the file being loaded.
//   
// abstrFileName - input -
//
// abstrpRevisionHistory - output -
//   This parameter should contain the XML string which represents the 
//   revision history as it was in the XML file. 
//
//	  This parameter can be NULL if there is no revision history in the file.
//
// abstrpChangedNode - output -
//	  If the return value equals 1, indicating that the checksums did not 
//	  match, then this string will contain the fully qualified name of the 
//   first node that did not have the correct checksum.
//
// Return Value - 
//   1 - Failed checksum.
	template <typename SVT_TREE>
	HRESULT CopyXMLFileToTree(SVT_TREE& rTree, long alSVOCurrentVersion, BSTR abstrFilename, BSTR* abstrpRevisionHistory, BSTR* abstrpChangedNode);

//	CopyTreeToDOM () ---------------------------------------------------------
//
// alSVOCurrentVersion - input -
//   This parameter represents the current configuration file format. This 
//   will often be the application version number.
//   
// abstrRevisionHistory - input -
//   This parameter should contain the XML string which represents the 
//   revision history as it was last pulled from the XML file. This revision 
//   history excludes this SVObserver instance.  The current SVObserver 
//   instance should not be added to the list until the actual save.
//
//	  This parameter can be NULL if there is no revision history.
//
	template <typename SVT_TREE>
	HRESULT CopyTreeToDOM(SVT_TREE& rTree, long alSVOCurrentVersion, BSTR abstrFormat, BSTR	abstrRevisionHistory);

// CopyDOMToTree () ---------------------------------------------------------
//  This function requires that the SVTreeClass derived class already be 
//	 attached, and that it have full access to the tree it is intended to 
//  support.  For the SVCTreeControlClass, the CTreeControl must already be 
//  attached to the SVTreeClass derived class.
// 
//  This function will call SVTreeClass::Clear () which is intended to 
//	 destroy the existing contents of the tree.
	template <typename SVT_TREE>
	HRESULT CopyDOMToTree(SVT_TREE& rTree, long alSVOConfigurationVersion);

	HRESULT CopyDOMToXMLFile(BSTR abstrFileName);

	HRESULT CopyXMLFileToDOM(BSTR abstrFileName, BSTR* abstrpRevisionHistory);

	HRESULT CopyXMLTextToDOM(BSTR XmlText);

// CopyTreeNodeToDOMNode () -------------------------------------------------
//  A tree must previously have been attached using AttachTree ().
//  This function does not destroy anything within the existing DOM.
//  The entire tree branch is copied to the DOM node. Zero will not be a 
//	 "real" tree handle.
//	
//	This will create the arDOMNode to be used; meaning that it is up to the 
//	calling function to actually insert this DOM node into the DOM tree.
//
// alTreeRootHandle - input -
// arDOMNodePtr - output -
//
// Return Value:
//   2 - alTreeRootHandle is 0.
//	
	template <typename SVT_TREE>
	HRESULT CopyTreeNodeToDOMNode( SVT_TREE& rTree, const typename SVT_TREE::SVBranchHandle& alTreeRootHandle, SVXML::IXMLDOMElementPtr& arDOMNodePtr );

// CreateTreeChildNodeFromDOMNode () ----------------------------------------
//  This function creates all tree child nodes copies the existing DOM data 
//  into the new child elements.  
//	
//  The tree must alread exist and be attached.
//
//  aDOMElementPtr - input - 
//  alTreeParentNodeHandle - input - 
//
//  Return Value:
//   2 - aDOMElementPtr is NULL.
//
//  If there is an error, the error result will always be returned with 
//	 either bit 31 or 32 set.
	template<typename SVT_TREE>
	HRESULT CreateTreeChildNodeFromDOMNode (SVXML::IXMLDOMElementPtr aDOMElementPtr, SVT_TREE& rTree, const typename SVT_TREE::SVBranchHandle& alTreeParentNodeHandle);

	template<typename SVT_TREE>
	HRESULT CopyTreeDataToDOMData( SVT_TREE& rTree, const typename SVT_TREE::SVLeafHandle& alNodeHandle, SVXML::IXMLDOMElementPtr& arDOMDataPtr );

// Beginning of REVISION HISTORY FUNCTIONS --------------------------------\/
	HRESULT AddRevisionHistoryBaseNode ();
	
// SetRevisionHistory () ----------------------------------------------------
// If a previous RevisionHistory element is found off the Base Element, it 
// will be deleted and will be replaced by a new RevisionHistory element.
//
// abstrRevisionHistory - input -
//   This parameter should contain the XML representation of the 
//   RevisionHistory node of the previous document.
	HRESULT SetRevisionHistory (BSTR abstrRevisionHistory);

	HRESULT AddToRevisionHistory (long	alSVOCurrentVersion, BSTR  abstrFormat);

// GetRevisionHistory () ----------------------------------------------------
// This function retrieved the revision history from the revision history 
//	class property.  It is required that LoadRevisionHistory () was called 
//	previously to extract the revision history from the DOM into the class
// revision history property.  See the LoadRevisionHistory () description for 
//	the list of functions that result in LoadRevisionHistory () being called.
//
// abstrpRevisionHistory - output - 
//
	HRESULT GetRevisionHistory (BSTR* abstrpRevisionHistory);

// GetCurrentRevision () ----------------------------------------------------
// This function retrieves a numeric representation of the current revision 
//	from the revision history class property.  It is required that 
//	LoadRevisionHistory () was called previously to extract the revision 
//	history from the DOM into the class current revision property.  See the 
//	LoadRevisionHistory () description for the list of functions that result 
//	in LoadRevisionHistory () being called.  
//
// p_lpRevisionHistory - output - See the m_lCurrentRevision discription for
//	the format of the p_lpRevisionHistory output.
//
	HRESULT GetCurrentVersion (long* p_lpCurrentVersion);

// GetRevisionHistoryBaseNode () --------------------------------------------
// arRevHistoryBaseNodePtr - output -
//
// Return Value - 
//   2 - Revision history base node does not exist in the DOM.
	HRESULT GetRevisionHistoryBaseNode (SVXML::IXMLDOMElementPtr& arRevHistoryBaseNodePtr);

// GetRevisionNodeByIndex () ------------------------------------------------
//
// Return Value - 
//   2 - Revision history base node does not exist in the DOM.
//	  3 - Desired index did not exist.

	HRESULT GetRevisionNodeByIndex (long p_lIndex, SVXML::IXMLDOMElementPtr& p_orRevNodePtr);

// End of REVISION HISTORY FUNCTIONS --------------------------------------/\
//

// SetEncryption () ---------------------------------------------------------
// If an Encryption element already exists off the base element then no 
//	action will be taken.  The Encryption element will NOT be created.
//
//	Return Value -
//	  3 - The Encryption branch base node already exists within the DOM.
	HRESULT SetEncryption ();

// GetEncryptionBaseNode () --------------------------------------------
// p_orEncryptionBaseNodePtr - output -
//
// Return Value - 
//   2 - Encryption base node does not exist in the DOM.
	HRESULT GetEncryptionBaseNode (SVXML::IXMLDOMElementPtr& p_orEncryptionBaseNodePtr);

// GetBaseElement () --------------------------------------------------------
// The returned base node is the base of the data structure.
// This means it gets the NODE element with the name "Base".
//  arBaseElementPtr - output - 
//
	HRESULT GetBaseElement (SVXML::IXMLDOMElementPtr& arBaseElementPtr);

	HRESULT GetElementData (SVXML::IXMLDOMElementPtr aDOMElementPtr, VARIANT* vpDOMElementData);

// GetElementName () --------------------------------------------------------
//  This function will create the BSTR.
	HRESULT GetElementName (SVXML::IXMLDOMElementPtr aDOMElementPtr, BSTR* abstrpDOMElementName);

	HRESULT GetElementNbrOfChildren (SVXML::IXMLDOMElementPtr aDOMElementPtr, long* alpNbrOfChildren);

// CreateDOMData () ---------------------------------------------------------
// This function was moved from protected to public in order to satisfy the 
// need to access it from SVXMLEncryptionClass.
	HRESULT CreateDOMData (SVXML::IXMLDOMElementPtr& arDOMNewElementPtr, BSTR abstrElementName, VARIANT* avpElementData);

	HRESULT CreateDOMChildData (SVXML::IXMLDOMNodePtr aDOMParentNodePtr, BSTR abstrElementName, VARIANT* avpElementData, SVXML::IXMLDOMElementPtr& arDOMChildDataPtr);

	HRESULT CalculateCheckSum (SVXML::IXMLDOMNodePtr aDOMNodePtr);

// IsElementNode () ---------------------------------------------------------
//
// Notes regarding the two IsElementNode () functions.  
//	
//	The first function checks the element to determine whether or not the 
//	 element is a node. In order to perform this check, the function must 
//	 retrieve the tag name from the element. If a BSTR is passed into this 
//	 function, the tag name of the element will be returned.  If a BSTR is 
//  not provided, the function still works.
//
// aDOMElementPtr - input -
//
// abstrpDOMElementTag - output - If a BSTR is passed in, it will be set to 
//  the tag name to be returned.  If this is done, this function will do
//  a SysAlloc () which must then be managed by the calling application.
//
// The second function accepts a BSTR, which is assumed to be a tag name.  
//  This function simply compares the input BSTR to see if it is the currect
//  value for a node.
//
// abstrrDOMElementTag - input -
//
// Return Value - 
//  0 - Is node.
//  1 - Is not node.
	HRESULT IsElementNode (SVXML::IXMLDOMElementPtr	aDOMElementPtr, BSTR* abstrpDOMElementTag = 0);
	HRESULT IsElementNode (BSTR& abstrrDOMElementTag);

// IsElementData () ---------------------------------------------------------
//
// Notes regarding the two IsElementData () functions.  
//	
//	The first function checks the element to determine whether or not the 
//	 element is a data element. In order to perform this check, the function 
//	 must retrieve the tag name from the element. If a BSTR is passed into 
//	 this function, the tag name of the element will be returned.  If a BSTR 
//	 is not provided, the function still works.
//
// aDOMElementPtr - input -
//
// abstrpDOMElementTag - output - If a BSTR is passed in, it will be set to 
//  the tag name to be returned.  If this is done, this function will do
//  a SysAlloc () which must then be managed by the calling application.
//
// The second function accepts a BSTR, which is assumed to be a tag name.  
//  This function simply compares the input BSTR to see if it is the currect
//  value for a data element.
//
// abstrrDOMElementTag - input -
//
//
// Return Value - 
//  0 - Has data.
//  1 - No data.
	HRESULT IsElementData (SVXML::IXMLDOMElementPtr	aDOMElementPtr, BSTR* abstrpDOMElementTag = 0);
	HRESULT IsElementData (BSTR& abstrrDOMElementTag);

// IsElementFromParent () ---------------------------------------------------
// aDOMElementPtr - input -
//
// Return Value - 
//  0 - Is from parent.
//  1 - Is not from parent.
	HRESULT IsElementFromParent (SVXML::IXMLDOMElementPtr aDOMElementPtr);

// IsElementSafeArray () ----------------------------------------------------
// aDOMElementPtr - input -
//
// Return Value - 
//  0 - Is SAFEARRAY.
//  1 - Is not SAFEARRAY.
	HRESULT IsElementSafeArray (SVXML::IXMLDOMElementPtr aDOMElementPtr);

// SVXMLEncryptionClass INTERFACES ----------------------------------------\/
// In general, these functions should not be used as an XML interface. They 
// are here for use by SVXMLEncryptionClass.
//
// CreateElement () ---------------------------------------------------------
	HRESULT CreateElement (BSTR p_bstrElementName, SVXML::IXMLDOMElementPtr&   p_orDOMElementPtr);

	HRESULT GetRootNode (SVXML::IXMLDOMElementPtr& p_orDOMRootPtr);

	HRESULT	AppendChildToDOMNode (SVXML::IXMLDOMNodePtr p_oParentNodePtr, SVXML::IXMLDOMNodePtr p_oChildNodePtr);

	HRESULT GetNameSpace (WCHAR* p_wcsNameSpace, long p_lBufferSize);

	HRESULT SetQueryNameSpace (BSTR	p_bstrQueryNameSpace);

	HRESULT PreserveWhitespace(bool bPreserve);

// END OF SVXMLEncryptionClass INTERFACES ---------------------------------/\

	SVDOMClass* svmopDOM;

// svmlUseRevisionHistoryInfo -----------------------------------------------
//  If this variable equals 1, then the class will accept and create a branch
//  containing program (format) version and data revision information.
//  The calling application must maintain the RevisionHistoryInfo from load 
//  to save so that it may persist. 
	long svmlUseRevisionHistoryInfo;

// svmlUseCheckSums ---------------------------------------------------------
//  If this variable equals 1, then the DOM will perform checksums.
//  As of SVO 4.50, this feature is NOT YET WORKING.
	long svmlUseCheckSums;

// svmlUseEncryption --------------------------------------------------------
//  This variable pertains to importing of XML records from tree only.  If 
//  this variable equals 1, then the created file will be encrypted.  This 
//  parameter does not affect reading documents.
	long m_lUseEncryption;

protected:
	HRESULT Init ();

// LoadRevisionHistory () ---------------------------------------------------
// Extracts the revision history from the DOM and places it in the revision
// history property (See GetRevisionHistory ()).
// This function is only called from within CopyXMLFileToDOM (), which is
// only called from within CopyXMLFileToTree (), which is the external 
//	interface access.
//
// Return Value - 
//   3 - The revision history base node does not exist in the DOM.
//
	HRESULT LoadRevisionHistory ();

	HRESULT LoadEncryption ();

// BEGIN CREATE ELEMENT FUNCTIONS (SVR node or SVR data) ------------------\/

	HRESULT CreateDOMNode (SVXML::IXMLDOMElementPtr& arDOMNewNodePtr, BSTR abstrNodeName, long altype = VT_NULL);

// END CREATE ELEMENT FUNCTIONS -------------------------------------------/\

// DeleteRevisionHistory () - 
	HRESULT DeleteRevisionHistory ();

// Clear () -----------------------------------------------------------------
// This clear will do DOM clear (which sets header) and set the schema.
	HRESULT Clear ();

// SetSVRSchema () ----------------------------------------------------------
// This function will set a specific schema branch into the Root element.  
//
// The location of the schema will be at the end (append) of the root element
//	child list.  
//	
// It is required that the schema be defined (present in the DOM document) 
//	before the it is attempted to be used. It is therefore usually required 
//	that SetSVRSchema () be called prior to inserting other elements into 
//	the DOM (with the exception of the header, "<?xml....",  element).
	HRESULT SetSVRSchema ();

// SetSVRSchemaHeader () ----------------------------------------------------
// This is a sub function of SetSVRSchema (), and should only be called from
// within SetSVRSchema ().
	HRESULT SetSVRSchemaHeader (SVXML::IXMLDOMElementPtr& p_roSchemaBaseElementPtr);

// SetSVRSchemaNameAttrForNodeOrDataElements () -----------------------------
// This is a sub function of SetSVRSchema (), and should only be called from
// within SetSVRSchema ().
	HRESULT SetSVRSchemaNameAttrForNodeOrDataElements (SVXML::IXMLDOMElementPtr& p_roSchemaBaseElementPtr);

// SetSVRSchemaValueAttrForNodeOrDataElements () ----------------------------
// This is a sub function of SetSVRSchema (), and should only be called from
// within SetSVRSchema ().
	HRESULT SetSVRSchemaValueAttrForNodeOrDataElements (SVXML::IXMLDOMElementPtr& p_roSchemaBaseElementPtr);

	HRESULT SetRevisionHistorySchema ();

	HRESULT ConfigureAllElementsOffRootToValidateAgainstSchema ();

	HRESULT ConfigureBranchToValidateAgainstSchema (SVXML::IXMLDOMElementPtr&	arElementPtr);

// BEGIN CHECKSUM FUNCTIONS -----------------------------------------------\/
 
// CalculateNodeCheckSums () - This function will go through all branches from 
//  the input node, and calculate (or re-calculate) all checksums. 
//
//	alMode - input - specifies the mode in which CalculateNodeCheckSums () 
//  should operate
//  0 - the checksum will be created or updated for each node.	If alMode is 
//   set to 0, then bstrpChangedNode will be ignored and can be set to NULL.
//  1 - the checksum will be calculated and compared for each node.  The 
//   first node who's checksum fails the compare will have the fully 
//   qualified name returned within bstrpChangedNode.
//
// Return Value - 
//  2 - The aDOMNodePtr element represents the Checksum node.  No checksum 
//   	  will be performed on this element.
//	 3 - The aDOMNodePtr is not a "NODE" element.  No checksum will be 
//	     performed on this element.
//  4 - Checksum failed. A change was detected.
	HRESULT CalculateNodeCheckSums (SVXML::IXMLDOMNodePtr aDOMNodePtr, unsigned long* aulpCheckSum, long alMode = 0, BSTR* abstrpChangedNode = NULL);

	HRESULT CalculateStringCheckSum (BSTR abstrSource, unsigned long* aulpCheckSum);

// UpdateCheckSumValue () - If the element "CheckSum" does not already exist,
//  it will be created.
	HRESULT UpdateCheckSumValue (SVXML::IXMLDOMNodePtr aDOMNodePtr, unsigned long aulCheckSum);

// CompareCheckSumValue () --------------------------------------------------
// aDOMNodePtr - input -
// aulCheckSum - input -
// abstrpChangedNode - output -
//
// Return Value - 
//   2 - Checksum failed.
	HRESULT CompareCheckSumValue (SVXML::IXMLDOMNodePtr	aDOMNodePtr, unsigned long aulCheckSum, BSTR* abstrpChangedNode);

// END CHECKSUM FUNCTIONS -------------------------------------------------/\
//

	HRESULT GetNamePath (SVXML::IXMLDOMNodePtr	aDOMNodePtr, BSTR* abstrpChangedNode);

	HRESULT AppendCarriageReturnToDOMNode (SVXML::IXMLDOMNodePtr p_oParentNodePtr);

	long svmlInitialized;
	
// m_lIsEncrypted -----------------------------------------------------------
// This value indicates whether the contents of the DOM are currently 
//	encrypted.  This can be set to true by the load function or the copy from 
// tree functions if UseEncryption was turned on.
	long m_lIsEncrypted;

//	unsigned long m_ulNameSeed;

	WCHAR m_wczRevisionHistory [6000];

// m_lCurrentVersion --------------------------------------------------------
// This value will be treated as a property, and will be set whenever the DOM 
// is loaded from an XML, if the revision section exists within the XML file.
// This represents the XML file revision section and not the vision 
//	configuration documents SVObserver version number.  
//	Although for SVObserver, the two values should be the same with the 
//	exception that the XML version number does not track betas.
// The version number refers to the the "format version" used by the program 
// that saved the XML file.  For SVObserver this should correspond to the 
// SVObserver version number of the program that saved the configuration.
// This version number format will be of the form 
//	MMmmbb, where 
//   MM - one or two digit major version number,
//   mm - two digit minor version number,
//   bb - and two digit beta version number (yes, I know the current 
//        SVXMLClass doesn't track beta versions).
//   MM * 10,000 + mm * 100 + bb
//
	long m_lCurrentVersion;

// m_lCurrentRevision -------------------------------------------------------
	long m_lCurrentRevision;

	SVXMLEncryptionClass* m_opEncryption;
};

#include "SVXMLLibrary/SVXMLClass.inl"

#endif // #ifndef SVXML2_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVXMLLibrary\SVXMLClass.h_v  $
 * 
 *    Rev 1.1   19 Dec 2014 04:44:28   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  978
 * SCR Title:  Copy and Paste a Tool within an Inspection or Between Different Inspections
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added CopyXMLTextToDOM to generate a DOM from a string
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 19:50:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Oct 2012 10:40:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code and projects to fix whitespace issues in the XML file and consolidate functionality for load and saving to this library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   28 Sep 2012 14:22:42   jspila
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
*/