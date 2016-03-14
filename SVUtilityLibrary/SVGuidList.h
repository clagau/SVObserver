//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGuidList
//* .File Name       : $Workfile:   SVGuidList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:21:42  $
//******************************************************************************
#ifndef INCL_SVGUIDLIST_H
#define INCL_SVGUIDLIST_H

#include <deque>
#include <set>
#include <map>
#include <guiddef.h>

typedef std::deque<GUID> SVGuidList;
typedef std::set<GUID> SVUniqueGuidList;
typedef std::map<_bstr_t, GUID> SVNameGuidList;

#endif

