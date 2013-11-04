//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTreeLibraryEnums
//* .File Name       : $Workfile:   SVTreeLibraryEnums.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:51:10  $
//******************************************************************************

#ifndef SVTREELIBRARYENUMS_H
#define SVTREELIBRARYENUMS_H

/**
@SVObjectName Tree Insertion Flags

@SVObjectOverview

@SVObjectOperations

*/
struct SVTreeInsertionFlags
{
	enum InsertionFlags
	{
		InsertAtEnd = 0,
		InsertBefore = 1,
		InsertAfter = 2
	};
};

#endif // #ifndef SVTREELIBRARYENUMS_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVTreeLibrary\SVTreeLibraryEnums.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 18:51:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Nov 2008 07:24:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
