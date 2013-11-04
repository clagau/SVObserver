//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVParserProgressStruct
//* .File Name       : $Workfile:   SVParserProgressStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:18:16  $
//******************************************************************************
#ifndef INCL_SVPARSERPROGRESSSTRUCT_H
#define INCL_SVPARSERPROGRESSSTRUCT_H

struct SVParserProgressStruct
{
	size_t amtProcessed;
	size_t amtTotal;
	unsigned long parserHandle;

	SVParserProgressStruct(unsigned long p_parserHandle)
	: parserHandle(p_parserHandle), amtProcessed(0), amtTotal(0) 
	{
	}
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVParserProgressStruct.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:18:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   04 Sep 2012 14:00:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial check-in of new files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
