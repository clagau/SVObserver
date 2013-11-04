//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVXMLBase
//* .File Name       : $Workfile:   SVXMLBase.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:49:36  $
//******************************************************************************

#ifndef SVXMLBASE_H
#define SVXMLBASE_H

#include "SVDOM.h"

class SVXMLBase
{
public:
	typedef SVXML::IXMLDOMElementPtr SVElementPtr;
	typedef SVXML::IXMLDOMNodePtr SVNodePtr;

	virtual HRESULT CreateDOMData( SVElementPtr& arDOMNewElementPtr, BSTR abstrElementName, VARIANT* avpElementData ) = 0;

	virtual HRESULT CreateElement( BSTR p_bstrElementName, SVElementPtr& p_orDOMElementPtr ) = 0;

	virtual HRESULT GetRootNode( SVElementPtr& p_orDOMRootPtr ) = 0;

	virtual HRESULT AppendChildToDOMNode( SVNodePtr p_oParentNodePtr, SVNodePtr p_oChildNodePtr ) = 0;

	virtual HRESULT GetNameSpace( WCHAR* p_wcsNameSpace, long p_lBufferSize ) = 0;

	virtual HRESULT SetQueryNameSpace( BSTR p_bstrQueryNameSpace ) = 0;

};

#endif // #ifndef SVXMLBASE_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVXMLLibrary\SVXMLBase.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:49:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
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
