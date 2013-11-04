//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : UseTypeExceptMatch
//* .File Name       : $Workfile:   UseTypeExceptMatch.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 16:32:22  $
//******************************************************************************

#ifndef USETYPEEXCEPTMATCH_H
#define USETYPEEXCEPTMATCH_H

#include "SVLoki/TypeTraits.h"

// this is used to prevent a member function from being declared twice
// e.g. in class SVValueObjectClassImpl<double>, the 
//   GetArrayValues(int iBucket, std::vector<T>& raValues) is instantiated with double.
// therefore we don't want to have another function
//   GetArrayValues(int iBucket, std::vector<double>& raValues)
// this prevents that scenario by changing the function to
//   GetArrayValues(int iBucket, std::vector<Loki::Type2Type<double> >& raValues)
template <typename TYPE, typename EXCEPT> 
struct UseTypeExceptMatch
{
	typedef typename Loki::Select< Loki::IsEqualType<TYPE, EXCEPT>::value, Loki::Type2Type<TYPE>, TYPE >::Result type;
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\UseTypeExceptMatch.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 16:32:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Dec 2009 13:33:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Dec 2009 13:32:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Apr 2008 15:00:20   JSpila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source to split the value objects into individual files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
