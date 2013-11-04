// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVSkeletonFilter
// * .File Name       : $Workfile:   SVSkeletonFilter.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   13 Aug 2013 10:32:12  $
// ******************************************************************************

#ifndef SVSKELETONFILTERS_H
#define SVSKELETONFILTERS_H

#include "SVFilterClass.h"

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVSkeletonFilterClass
// -----------------------------------------------------------------------------
// .Description : Base class for all kinds of Skeleton filters
// -----------------------------------------------------------------------------
// .Export
//	 Public Method				Description
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
////////////////////////////////////////////////////////////////////////////////
class SVSkeletonFilterClass : public SVFilterClass
{
	SV_DECLARE_CLASS( SVSkeletonFilterClass );

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
// Standard constructor
	SVSkeletonFilterClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVSKELETONFILTER );
	virtual ~SVSkeletonFilterClass();
	
private:

protected:
	virtual BOOL onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus );

//******************************************************************************
// Operator(s):
//******************************************************************************

public:

////////////////////////////////////////////////////////////////////////////////
// Create Operator
////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
// Operation(s) Of Reading Access:
//******************************************************************************
public:


//******************************************************************************
// Operation(s) Of Process:
//******************************************************************************
protected:

//******************************************************************************
// Data Element(s):
//******************************************************************************
protected:
};

#endif // SVSKELETONFILTERS_H

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVSkeletonFilter.h_v  $
 * 
 *    Rev 1.1   13 Aug 2013 10:32:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Included SVFilterClass.h instead of SVOperator.h.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:07:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   10 Feb 2011 15:13:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   27 Jan 2011 11:58:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   06 Oct 2005 10:37:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed  ShouldResetIPDoc(true) to reduce delay since this filter has no parameters this is unnecessary.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Aug 2005 08:11:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Feb 2005 15:29:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Made changes to include OnValidate( to support the new reset, create, validate methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Sep 2004 09:59:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  446
 * SCR Title:  Add new filters to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in of new Skeleton Filter Class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
//** EOF **
