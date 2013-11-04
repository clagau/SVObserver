//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSequencer
//* .File Name       : $Workfile:   SVSequencer.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:02:02  $
//******************************************************************************

template<typename IteratorType>
SVSequencer<IteratorType>::SVSequencer()
: bContinuousMode(true)
{
}

template<typename IteratorType>
void SVSequencer<IteratorType>::Init(const IteratorType& begin, const IteratorType& end, bool bWrap)
{
	bContinuousMode = bWrap;
	startIt = begin;
	endIt = end;
	currentIt = startIt;
}

template<typename IteratorType>
typename SVSequencer<IteratorType>::ValueType SVSequencer<IteratorType>::GetNext()
{
	ValueType info;
	if (currentIt == endIt)
	{
		// if continuous - wrap back around
		if (bContinuousMode)
		{
			currentIt = startIt;
		}
	}
	if (currentIt != endIt)
	{
		info = (*currentIt);
		currentIt++;
	}
	return info;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSystemLibrary\SVSequencer.inl_v  $
 * 
 *    Rev 1.0   25 Apr 2013 18:02:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Sep 2009 15:45:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   File moved to this project to allow for multple project inclusion.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   05 Dec 2008 15:25:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added flag to wrap or stop after one iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Sep 2008 10:29:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
