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

