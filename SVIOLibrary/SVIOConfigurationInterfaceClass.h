// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIOConfigurationInterfaceClass
// * .File Name       : $Workfile:   SVIOConfigurationInterfaceClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   06 May 2013 19:13:20  $
// ******************************************************************************

#ifndef SVIOCONFIGURATIONINTERFACECLASS_H
#define SVIOCONFIGURATIONINTERFACECLASS_H

#include "SVIODigitalArrayClass.h"
#include "SVIODigitalLoadLibraryClass.h"

class SVIOConfigurationInterfaceClass  
{
public:
	inline static SVIOConfigurationInterfaceClass& Instance();

	virtual ~SVIOConfigurationInterfaceClass();

	void Shutdown();

	HRESULT SetOpto22InvertInputValue( bool p_bInvert );
	HRESULT SetOpto22InvertOutputValue( bool p_bInvert );

	HRESULT OpenDigital( LPCTSTR p_szName );
	HRESULT CloseDigital();

	HRESULT GetDigitalInputCount( unsigned long &p_rulCount );
	HRESULT GetDigitalInputIsInverted( unsigned long p_ulChannel, bool &p_rbValue );
	HRESULT SetDigitalInputIsInverted( unsigned long p_ulChannel, bool p_rbValue );
	HRESULT GetDigitalInputIsForced( unsigned long p_ulChannel, bool &p_rbValue );
	HRESULT SetDigitalInputIsForced( unsigned long p_ulChannel, bool p_rbValue );
	HRESULT GetDigitalInputForcedValue( unsigned long p_ulChannel, bool &p_rbValue );
	HRESULT SetDigitalInputForcedValue( unsigned long p_ulChannel, bool p_rbValue );
	HRESULT GetDigitalInputValue( unsigned long p_ulChannel, bool &p_rbValue );

	HRESULT GetDigitalInputPortCount( unsigned long &p_rulCount );
	HRESULT GetDigitalInputPortValue( unsigned long p_ulPort, unsigned long &p_rulValue );

	HRESULT GetDigitalOutputCount( unsigned long &p_rulCount );
	HRESULT GetDigitalOutputIsInverted( unsigned long p_ulChannel, bool &p_rbValue );
	HRESULT SetDigitalOutputIsInverted( unsigned long p_ulChannel, bool p_rbValue );
	HRESULT GetDigitalOutputIsForced( unsigned long p_ulChannel, bool &p_rbValue );
	HRESULT SetDigitalOutputIsForced( unsigned long p_ulChannel, bool p_rbValue );
	HRESULT GetDigitalOutputForcedValue( unsigned long p_ulChannel, bool &p_rbValue );
	HRESULT SetDigitalOutputForcedValue( unsigned long p_ulChannel, bool p_rbValue );
	HRESULT GetDigitalOutputValue( unsigned long p_ulChannel, bool &p_rbValue );
	HRESULT SetDigitalOutputValue( unsigned long p_ulChannel, bool p_bValue );
	HRESULT GetDigitalOutputState( unsigned long p_ulSizeInChars, unsigned char *p_pucBlock );

	HRESULT GetDigitalOutputPortCount( unsigned long &p_rulCount );
	HRESULT SetDigitalOutputPortValue( unsigned long p_ulPort, unsigned long p_ulValue );

	HRESULT ClearDigitalOutputs();
	HRESULT TestDigitalOutputs();

	HRESULT SetSVIMTriggerValue( unsigned long p_ulChannel, bool p_bRising );
	HRESULT SetSVIMStrobeValue( unsigned long p_ulChannel, bool p_bRising );
	HRESULT SetSVIMStrobeStartFrameActive( unsigned long p_ulChannel, bool p_bActive );
	HRESULT GetSVIMStrobeStartFrameActive( unsigned long p_ulChannel, bool &p_rbActive );

	HRESULT GetIOTriggerValue( unsigned long p_ulChannel, bool &p_rbRising );
	HRESULT GetIOStrobeValue( unsigned long p_ulChannel, bool &p_rbRising );

	HRESULT SetCameraTriggerValue( unsigned long p_ulChannel, bool p_bRising );
	HRESULT SetCameraStrobeValue( unsigned long p_ulChannel, bool p_bRising );

	// Parameter related functions. Not implemented in all dlls.
	HRESULT GetParameterCount( unsigned long *p_pulCount );
	HRESULT GetParameterName( unsigned long p_ulIndex, BSTR *p_pbstrName );
	HRESULT GetParameterValue( unsigned long p_ulIndex, VARIANT *p_pvarValue );
	HRESULT SetParameterValue( unsigned long p_ulIndex, VARIANT *p_pvarValue );

private:
	SVIOConfigurationInterfaceClass();
	SVIOConfigurationInterfaceClass( const SVIOConfigurationInterfaceClass& p_rObject );
	const SVIOConfigurationInterfaceClass& operator=( const SVIOConfigurationInterfaceClass& p_rObject );

	SVIODigitalLoadLibraryClass m_svDigitalBoard;

	bool m_Opto22InputInvert;
	bool m_Opto22OutputInvert;
	bool m_pbSVIMTriggerRising[4];
	bool m_pbSVIMStrobeRising[4];
	bool m_abSVIMStrobeStartFrame[4];

	SVIODigitalArrayClass m_svDigitalInputs;
	SVIODigitalArrayClass m_svDigitalOutputs;

};

#include "SVIOConfigurationInterfaceClass.inl"

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVIOLibrary\SVIOConfigurationInterfaceClass.h_v  $
 * 
 *    Rev 1.1   06 May 2013 19:13:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   16 Apr 2013 14:00:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  822
 * SCR Title:  Remove DDE Input and Output functionality from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed DDE functionality from the source code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 12:44:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   16 Aug 2012 16:41:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated WriteDDE method parameter to be const correct.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   15 Aug 2012 14:21:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVIOConfigurationInterfaceClass to become a singleton.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   13 Dec 2010 08:12:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  702
 * SCR Title:  Updated Coreco Acquisition Functionality to Include Xcelera-AN Board
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update source code to use new Xcede-AN board and Rabbit IO.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   27 Jun 2006 09:05:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  564
 * SCR Title:  Add configurability to SVTVicLpt DLL to set the system type
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added SetParameterValue, GetParameterValue, GetParameterCount, and GetParameterName.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   27 Jun 2006 08:49:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  564
 * SCR Title:  Add configurability to SVTVicLpt DLL to set the system type
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Set and Get Parameter Functions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   15 Jun 2004 13:56:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  410
 * SCR Title:  Fix DDE IO
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated ReadDDEInput and WriteDDEOutput methods to handle changes in SVDDEItemData struct.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Jun 2004 13:47:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  410
 * SCR Title:  Fix DDE IO
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated ReadDDEInput and WriteDDEOutput methods to use the changes to the SVDDEItemData structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   22 Jan 2004 12:25:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed name of function
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   21 Jan 2004 15:24:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added code to make strobe start frame optional
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   16 Jan 2004 07:19:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added functions to allow configuring IO to work with special SVIM hardware
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Oct 2003 14:52:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Update digital output port methods to fix problems with not setting enough data and added methods to handle the inverters for the tiggers and strobs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Sep 2003 15:06:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Removed Triggers from I/O Configuration Objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   18 Aug 2003 15:30:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed SVIODDELoadLibraryClass from class.  Removed SendDDEData and ReceiveDDEData from class.  Updated constructor, destructor, GetDigitalOutputState, OpenDDE, CloseDDE, and SetDDEServer with DDE pump changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Jul 2003 13:20:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added I/O objects to new I/O configuration interface library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
