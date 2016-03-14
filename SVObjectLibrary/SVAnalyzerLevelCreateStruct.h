//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAnalyzerLevelCreateStruct
//* .File Name       : $Workfile:   SVAnalyzerLevelCreateStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:35:10  $
//******************************************************************************

#ifndef SVANALYZERLEVELCREATESTRUCT_H
#define SVANALYZERLEVELCREATESTRUCT_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "SVToolLevelCreateStruct.h"

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVAnalyzerLevelCreateStruct
// -----------------------------------------------------------------------------
// .Description : Structure for SVM_CREATE... messages to objects below the
//				: Analyzer level
//              : Supports structure identifying by structure name
// -----------------------------------------------------------------------------
// .Export
//	 Public Method				Description
//  :
//  :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :29.06.1999 SEJ			First Implementation
////////////////////////////////////////////////////////////////////////////////
struct SVAnalyzerLevelCreateStruct : public SVToolLevelCreateStruct
{
//******************************************************************************
// Constructor(s):
//******************************************************************************

	SVAnalyzerLevelCreateStruct();
	SVAnalyzerLevelCreateStruct( SVObjectLevelCreateStruct& ROLCS );

	virtual ~SVAnalyzerLevelCreateStruct();
//******************************************************************************
// Operator(s):
//******************************************************************************

//******************************************************************************
// Data Element(s):
//******************************************************************************

	SVObjectInfoStruct AnalyzerObjectInfo;
};

////////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// DevStudio inserts additional declarations immediate in front of the preceding line
////////////////////////////////////////////////////////////////////////////////

#endif	//	SVANALYZERLEVELCREATESTRUCT_H

