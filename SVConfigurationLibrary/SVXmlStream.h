// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVXmlStream
// * .File Name       : $Workfile:   SVXmlStream.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 18:39:44  $
// ******************************************************************************

/* *************************************************************************************************

	MSXML C++ Wrapper Class (VC++ 6.0) (LOADING / PARSING and SAVING XML DOCUMENTS)
	ANDRE Sébastien, France 2002   (maximus@oreka.com) (http://divxdede.fr.st)
	
   ************************************************************************************************ */

#ifndef XML_PARSER_H
#define XML_PARSER_H

// -- Load MSXML library --
// 
 #import <msxml6.dll> named_guids

template< typename SVTreeType >
class SVXmlStream
{

public:
	// -- Constructor --
	//
	SVXmlStream(SVTreeType& Tree);

	// -- Destructor --
	//
	virtual ~SVXmlStream();

	// -- Load // Save // Reset An XML document --
	//
	bool Load_XML_Document(LPCTSTR strFileName);	// Load an XML Document from file
	void Reset_XML_Document();										// Clear the XML Document 

	// -- **************************************** --
	// -- DERIVED CLASS MUST IMPLEMENT this METHOD --
	// -- **************************************** --
	virtual void Parse_XML_Document();

	// -- *********************** --
	// -- *** READING METHODS *** --
	// -- *********************** --

	// -- Control Chaining Tags
	// --
	bool Is_Tag(LPCTSTR aTag);										// Return true if it's equal to the Current Tag
	bool Is_Child_of(LPCTSTR parent_chain);							// Test if a chain Tag is the parent of current Tag
	bool Is_Root();													// Test if a Tag is the Root
	bool Is_TextNode();												// Return true if it's a text node (ie; CurrentTag_is("<#TEXT#>"); )

	// -- Getting Current Tag
	// --
	CString & Get_CurrentTag();										// Get the Current Tag value  (with "<>")
	CString & Get_CurrentName();									// Get the Current Name value (without "<>")

	// -- Get Text field when node "<#TEXT#>" or when "Is_TextNode()"
	// --
	CString & Get_TextValue();										// Get the Text Value when node "<#TEXT#>"

	// -- Attribute Methods Access
	// --
	bool Is_Having_Attribute(LPCTSTR Name);							// return true if the current node have the specified attribute defined
	CString & Get_Attribute_Value();								// Return the value of the last attribute selected by the method "Is_Having_Attribute()"
	
	// -- ***************************** --
	// -- *** MODIFICATIONS METHODS *** --
	// -- ***************************** --
	
	// -- Creation
	//
	bool Add_LastChildNode(LPCTSTR Name);							// Add a new CHILD NODE at the End of Current Node
	bool Add_FirstChildNode(LPCTSTR Name);							// Add a new CHILD NODE at the Begining of Current Node
	bool Add_NodeBefore(LPCTSTR Name);								// Add a new NODE (same level) before the Current Node
	bool Add_NodeAfter(LPCTSTR Name);								// Add a new NOFR (same level) after the Current Node

	// -- Setting Attributes & Text
	//
	bool Set_Attribute(LPCTSTR AttribName,LPCTSTR AttribValue);		// Set an attribute to the Current Node
	bool Remove_Attribute(LPCTSTR AttribName);						// Remove an attribute the the Current Node
	bool Set_Text(LPCTSTR TextValue);								// Set a Text to the Current Node
	
	// -- Removing
	//
	bool Remove();													// Remove the Current Node (Move to the Parent Node)
	bool RemoveChild(LPCTSTR NodeName);								// Remove a Child Node (No Move);

	// -- *********************************** --
	// -- *** XML Document Moving by name *** --
	// -- *********************************** --
	void Go_to_Root();												// Go to the Root node
	bool Go_to_Child(LPCTSTR NodeName);				// Go to a Direct Child Node
	bool Go_to_Descendant(LPCTSTR NodeName);	// Go to a Descendant Node (Child, Little Child, Etc...)
	bool Go_to_Parent(LPCTSTR NodeName);			// Go to a Parent Node
	bool Go_to_Forward(LPCTSTR NodeName);			// Go to a Node attached at the same Node than the Current Node (Forward sens)
	bool Go_to_Back(LPCTSTR NodeName);				// Go to a Node attached at the same Node than the Current Node (Reverse sens)

	// -- ************************ --
	// -- *** Error Management *** --
	// -- ************************ --

	// -- Get the last error signification --
	//
	CString & Get_LastError();

private:
	// Node Value
	//
	CString Tag;				// The last Tag (the name with "<...>")
	CString Name;				// The Name (without "<...>")
	CString ChainTag;		// The Chain Tag
	CString TextField;	// The Text Value

	// -- Init MSXML, for let this Class to Load and Parse an XML Document --
	// -- Parse_XML_document start the Parsing --
	//
	bool Init_MSXML();

	// XML document pointer
	//
	MSXML2::IXMLDOMDocumentPtr m_plDomDocument;

	// Root node of XML document
	//
	MSXML2::IXMLDOMElementPtr m_pDocRoot;

	// The current Node added
	//
	MSXML2::IXMLDOMElementPtr CurrentElementNode;

	// Error Status String
	//
	CString lasterror,ok;

	// Attrib Values
	//
	CArray< CString, CString& > AttribNames;
	CArray< CString, CString& > AttribValues;
	int attrib_index;

	// Internal Methods, Parse the Tree nodes
	//
	void Parse_Objects(MSXML2::IXMLDOMNodePtr pObject);

	// Update "Node Values" & Test type
	//
	void Grab_Node_Informations(MSXML2::IXMLDOMNodePtr pChild);
	bool Is_MSXML_Node(MSXML2::IXMLDOMNodePtr pChild);

	// ChainTag methods management
	//
	void ChainTag_Add(CString & val);
	void ChainTag_Remove(int number);

private:
	void ReleaseTreeItemData(SVTreeType& Tree);
	void ParseEndTag(CString& endtag);

private:
	SVTreeType m_Tree;
	typename SVTreeType::SVBranchHandle m_Parent;

	CString m_LeafName;
	VARTYPE m_VarType;
	_variant_t m_Variant;

	HRESULT mhr;

};

#include "SVXmlStream.inl"

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVConfigurationLibrary\SVXmlStream.h_v  $
 * 
 *    Rev 1.0   18 Apr 2013 18:39:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Sep 2012 17:54:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library.
 * 
 * This code was moved from the SVOConfigMgr Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   09 Nov 2010 15:58:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to change the formatting to make statements more searchable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   27 Oct 2010 09:20:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix include issues and comment issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   20 Jun 2007 13:44:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Oct 2004 08:42:42   jBrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  375
 * SCR Title:  Remodel XML usage for configuration storage and SIAC transfers.
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   When implementing the new XML classes the decision was made to upgrade the Microsoft XML parser from version 3 to version 4.
 * 
 * When compiling with MSXML 4 parser, the compiler errors referencing ambiguouty.  The ambiguouty was eliminated by removing the using namespace statement and explicitly indicating each objects namespace.
 * 
 * The functions that have been modified within the SVXMLStreemClass include:
 *   Init_MSXML(),
 *   Parse_Objects (),
 *   Is_MSXML_Node (),
 *   Grab_Node_Informations (),
 *   Add_LastChildNode (),
 *   Add_NodeBefore (),
 *   Add_NodeAfter (),
 *   Add_FirstChildNode (),
 *   Set_Text (),
 *   Go_to_Child (),
 *   Go_to_Parent (),			,
 *   Go_to_Forward (),
 *   Go_to_Back (),
 *   Remove (),
 * 
 * In addition, the header file was changed to import msxml4 instead of msxml3 (reflecting the upgrade in the MS parser version).
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Oct 2004 08:38:38   jBrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  375
 * SCR Title:  Remodel XML usage for configuration storage and SIAC transfers.
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   When implementing the new XML classes the decision was made to upgrade the Microsoft XML parser from version 3 to version 4.
 * 
 * When compiling with MSXML 4 parser, the compiler errors referencing ambiguouty.  The ambiguouty was eliminated by removing the using namespace statement and explicitly indicating each objects namespace.
 * 
 * The functions that have been modified within the SVXMLStreemClass include:
 *   Init_MSXML(),
 *   Parse_Objects (),
 *   Is_MSXML_Node (),
 *   Grab_Node_Informations (),
 *   Add_LastChildNode (),
 *   Add_NodeBefore (),
 *   Add_NodeAfter (),
 *   Add_FirstChildNode (),
 *   Set_Text (),
 *   Go_to_Child (),
 *   Go_to_Parent (),			,
 *   Go_to_Forward (),
 *   Go_to_Back (),
 *   Remove (),
 * 
 * In addition, the header file was changed to import msxml4 instead of msxml3 (reflecting the upgrade in the MS parser version).
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Nov 2002 09:57:10   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first version of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/