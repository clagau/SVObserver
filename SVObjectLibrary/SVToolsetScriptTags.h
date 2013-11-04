//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolsetScriptTags
//* .File Name       : $Workfile:   SVToolsetScriptTags.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 17:25:00  $
//******************************************************************************
#ifndef INCL_SVTOOLSETSCRIPT_TAGS_H
#define INCL_SVTOOLSETSCRIPT_TAGS_H

namespace 
{
	const CString scAttributesAllowedTag(_T( "AttributesAllowed" ));
	const CString scAttributesSetTag(_T( "AttributesSet" ));
	const CString scAttributeTag(_T( "Attribute" ));
	const CString scDefaultTag(_T("Default"));
	const CString scArrayElementsTag(_T("Array_Elements"));
	const CString scElementTag(_T("Element"));

	const CString scNameTag(_T( "Name" ));
	const CString scObjectNameTag(_T( "ObjectName" ));
	const CString scUniqueReferenceIDTag(_T( "UniqueReferenceID" ));
	const CString scEmbeddedIDTag(_T( "EmbeddedID" ));
	const CString scClassIDTag(_T( "ClassID" ));
	const CString scFriendsTag(_T( "Friends" ));
	const CString scEmbeddedsTag(_T( "Embeddeds" ));
	const CString scInputsTag(_T( "Inputs" ));
	const CString scInputTag(_T( "Input" ));
	const CString scEquationBufferTag(_T("EquationBuffer"));
	const CString scDigitizerIDTag(_T("DigitizerID"));
	const CString scPixelDepthTag(_T( "PixelDepth" ));
	const CString scBandNumberTag(_T( "BandNumber" ));
	const CString scBandLinkTag(_T( "BandLink" ));
	const CString scMaskDataTag(_T( "MaskData" ));
	const CString scValueTag(_T( "Value" ));
}

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObjectLibrary\SVToolsetScriptTags.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 17:25:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   04 Sep 2012 14:00:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial check-in of new files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/