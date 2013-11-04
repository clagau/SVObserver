//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVXMLSafeArrayConverter
//* .File Name       : $Workfile:   SVXMLSafeArrayConverter.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:56:58  $
//******************************************************************************
#ifndef INCL_SVXMLSAFEARRAYCONVERTER_H
#define INCL_SVXMLSAFEARRAYCONVERTER_H

#include "SVXMLLibrary/SVXMLClass.h"

class SVXMLSafeArrayConverter
{
public:
	static HRESULT CopyDOMNodeToSafeArray(SVXMLClass& rXml, SVXML::IXMLDOMElementPtr aDOMElementPtr, _variant_t& value);
	static HRESULT CopySafeArrayToDOMNode(SVXMLClass& rXml, BSTR p_bstrSafeArrayName, SAFEARRAY* p_InputSafeArray, SVXML::IXMLDOMElementPtr& p_roDOMElementPtr);

private:
	SVXMLSafeArrayConverter(); // Non Constructable
	~SVXMLSafeArrayConverter();

	static HRESULT CopyDOMChildNodesToSafeArrayNodes(SVXMLClass& rXml, SVXML::IXMLDOMElementPtr aDOMParentElementPtr, long alDimension, long* alpIndicesArray, SAFEARRAY* apSafeArray);
	static HRESULT GetSafeArrayData(SVXMLClass& rXml, SVXML::IXMLDOMElementPtr aDOMElementPtr, long* alpSafeArrayDepth, long* alpSafeArrayType, SAFEARRAYBOUND* apSafeArrayBounds);

	static HRESULT CreateDOMChildSafeArrayNodes(SVXMLClass& rXml, SVXML::IXMLDOMElementPtr& p_roDOMParentElementPtr, long p_lDimension, long* p_plIndices, SAFEARRAY* p_InputSafeArray);
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVXMLLibrary\SVXMLSafeArrayConverter.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:56:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Oct 2012 10:40:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code and projects to fix whitespace issues in the XML file and consolidate functionality for load and saving to this library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/