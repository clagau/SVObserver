//******************************************************************************
//* COPYRIGHT (c) 2007 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIOBoardCapabilities
//* .File Name       : $Workfile:   SVIOBoardCapabilities.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   :     $Date:   11 Jun 2013 15:26:18  $
//******************************************************************************

#if !defined(AFX_SVIOBOARDCAPABILITIES_H_INCLUDED_)
#define AFX_SVIOBOARDCAPABILITIES_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVConfigurationLibrary/SVObserverEnums.h"

class SVIOBoardCapabilities  
{
public:
	SVIOBoardCapabilities();
	SVIOBoardCapabilities( const SVIOBoardCapabilities& p_rCapable );
	const SVIOBoardCapabilities& operator=( const SVIOBoardCapabilities& p_rCapable );
	virtual ~SVIOBoardCapabilities();

	int GetTriggerInverterCount() const;
	int GetStrobeInverterCount() const;
	int GetTriggerCount() const;
	int GetInputCount() const;
	int GetOutputCount() const;

	void SetInputCount( int p_iInputs );
	void SetOutputCount( int p_iOutputs );
	void SetTriggerCount( int p_iTriggers );
	void SetTriggerInverters( int p_iTrigInverters );
	void SetStrobeInverters( int p_iStrobeInverters );

	bool IsNonIOSVIM() const;
	void SetNonIOSVIM(int numTriggers);

private:
	int m_iTriggerInverters;
	int m_iStrobeInverters;
	int m_iNumTriggers;
	int m_iNumOutputs;
	int m_iNumInputs;
};

#endif // !defined(AFX_SVIOBOARDCAPABILITIES_H_INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVIOBoardCapabilities.h_v  $
 * 
 *    Rev 1.1   11 Jun 2013 15:26:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 017.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   21 May 2013 10:00:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  836
 * SCR Title:  Fix loading SEC bug
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Moved SVObserverEnums.h to SCConfigurationLibrary.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 11:08:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   21 Jan 2013 10:55:12   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed unused constructor overload
 * Added IsNonIOSVIM method
 * Aded SetNonIOSVIM method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   21 Jun 2007 13:05:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 Feb 2007 09:08:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  592
 * SCR Title:  Add  X3 product type to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added PVCS Headers
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
