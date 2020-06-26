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

#pragma once

// SVResearch defined data types (in addition to standard Variant data types)
// for use in DOM nodes (and XML file).
#define VT_SVARRAYNODE 500		// Does not appear to conflict with any Variant types.
#define VT_SVNODEWITHDATA 501	// This represents a tree node that contained both data and children.
#define VT_SVBASENODE 502		// This represents a tree base node.

