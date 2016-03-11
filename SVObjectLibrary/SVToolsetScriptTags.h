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
#pragma once

namespace 
{
	const TCHAR* const scAttributesAllowedTag(_T( "AttributesAllowed" ));
	const TCHAR* const scAttributesSetTag(_T( "AttributesSet" ));
	const TCHAR* const scAttributeTag(_T( "Attribute" ));
	const TCHAR* const scDefaultTag(_T("Default"));
	const TCHAR* const scArrayElementsTag(_T("Array_Elements"));
	const TCHAR* const scElementTag(_T("Element"));

	const TCHAR* const scNameTag(_T( "Name" ));
	const TCHAR* const scObjectNameTag(_T( "ObjectName" ));
	const TCHAR* const scUniqueReferenceIDTag(_T( "UniqueReferenceID" ));
	const TCHAR* const scEmbeddedIDTag(_T( "EmbeddedID" ));
	const TCHAR* const scClassIDTag(_T( "ClassID" ));
	const TCHAR* const scFriendsTag(_T( "Friends" ));
	const TCHAR* const scEmbeddedsTag(_T( "Embeddeds" ));
	const TCHAR* const scInputsTag(_T( "Inputs" ));
	const TCHAR* const scInputTag(_T( "Input" ));
	const TCHAR* const scEquationBufferTag(_T("EquationBuffer"));
	const TCHAR* const scDigitizerIDTag(_T("DigitizerID"));
	const TCHAR* const scPixelDepthTag(_T( "PixelDepth" ));
	const TCHAR* const scBandNumberTag(_T( "BandNumber" ));
	const TCHAR* const scBandLinkTag(_T( "BandLink" ));
	const TCHAR* const scMaskDataTag(_T( "MaskData" ));
	const TCHAR* const scValueTag(_T( "Value" ));
}

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