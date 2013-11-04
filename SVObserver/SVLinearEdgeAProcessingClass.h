// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVLinearEdgeAProcessingClass
// * .File Name       : $Workfile:   SVLinearEdgeAProcessingClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 11:53:06  $
// ******************************************************************************

#ifndef _SVLINEAREDGEAPROCESSINGCLASS_H
#define _SVLINEAREDGEAPROCESSINGCLASS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVLinearEdgeProcessingClass.h"

class SVLinearEdgeAProcessingClass : public SVLinearEdgeProcessingClass  
{
	SV_DECLARE_CLASS( SVLinearEdgeAProcessingClass );

public:
	SVLinearEdgeAProcessingClass( SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVLINEAREDGEAPROCESSING );
	virtual ~SVLinearEdgeAProcessingClass();
};

#endif // _SVLINEAREDGEAPROCESSINGCLASS_H

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVLinearEdgeAProcessingClass.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 11:53:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Feb 2005 10:43:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  463
 * SCR Title:  Add new Linear Measurement Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Adding new files for the new linear tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/