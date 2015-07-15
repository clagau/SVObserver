//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOutputStreamManager
//* .File Name       : $Workfile:   SVOutputStreamManager.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:16:48  $
//******************************************************************************

#pragma once

#include <string>

#include "SVCommandLibrary/SVCommandDataHolder.h"
#include "SVObjectLibrary/SVObserverNotificationFunctor.h"
#include "SVObjectLibrary/SVObserverTemplate.h"
#include "SVJsonCommandServerLibrary/SVJsonCommandServer.h"
#include "SVUtilityLibrary/SVGUID.h"

class SVOutputStreamManager
{
public:
	friend class SVObserverApp;

	static SVOutputStreamManager& Instance();

	virtual ~SVOutputStreamManager();

	HRESULT InsertOutputController( const SVGUID& rObjectId );
	HRESULT EraseOutputController();

	HRESULT InsertOutputStream( const std::string& rName, const SVGUID& rObjectId );
	HRESULT EraseOutputStream();

	HRESULT ProcessJsonCommand( const std::string& rJsonCommand, std::string& rJsonResults );
	HRESULT Rename( LPCTSTR OldName, LPCTSTR NewName);

protected:
	typedef std::pair< std::string, SVGUID > SVOutputStreamPair;

	class SVOutputSocketObserver : 
		public SVObserverNotificationFunctor,
		public SVObserverTemplate< std::string >
	{
	public:
		typedef boost::function< HRESULT ( const std::string& ) > SVObserverFunction;
		SVOutputSocketObserver( SVObserverFunction Function ) : m_Function( Function ) {}

		virtual HRESULT ObserverUpdate( const std::string& rData ) override
		{
			HRESULT l_Status = S_OK;

			if( !( m_Function.empty() ) )
			{
				l_Status = m_Function( rData );
			}
			else
			{
				l_Status = E_FAIL;
			}
			return l_Status;
		}

	private:
		SVOutputSocketObserver() : m_Function() {}
		SVObserverFunction m_Function;
	};

	SVOutputStreamManager();

	void Startup(unsigned short PortNumber);
	void Shutdown();

	HRESULT SendCommandToOutputStream( const std::string& rCmdName, const std::string& rName, const std::string& rJsonCommand, std::string& rJsonResults );

	HRESULT ProcessStreamManagerJsonCommand( const std::string& rJsonCommand, std::string& rJsonResults );

	SVJsonCommandServer m_SocketServer;
	DWORD m_OutputSocketCookie;
	SVObserverNotificationFunctorPtr m_SocketNotifyPtr;
	SVGUID m_OutputControllerId;
	SVOutputStreamPair m_OutputStream;

};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOutputStreamManager.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:16:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Feb 2013 14:13:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code that uses the subject/observer design patten as part of the object manager with the new parameter.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   03 Apr 2012 11:36:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added remane function for the output stream name.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   30 Jan 2012 11:51:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Modified source code to consolidate PLC and New Output stream functionality to I/O Controller.  Added new output stream functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
