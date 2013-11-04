//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVVariantCustomTypes
//* .File Name       : $Workfile:   SVVariantCustomTypes.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:48:34  $
//******************************************************************************
#ifndef INCL_SVVARIANTCUSTOMTYPES_H
#define INCL_SVVARIANTCUSTOMTYPES_H

// SVResearch defined data types (in addition to standard Variant data types)
// for use in DOM nodes (and XML file).
#define VT_SVARRAYNODE 500L		// Does not appear to conflict with any Variant types.
#define VT_SVNODEWITHDATA 501L	// This represents a tree node that contained both data and children.
#define VT_SVBASENODE 502L		// This represents a tree base node.

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVXMLLibrary\SVVariantCustomTypes.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:48:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Oct 2012 10:40:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code and projects to fix whitespace issues in the XML file and consolidate functionality for load and saving to this library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/