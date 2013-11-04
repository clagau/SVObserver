//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerProcessingClass
//* .File Name       : $Workfile:   SVTriggerProcessingClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 12:29:36  $
//******************************************************************************

#include "stdafx.h"
#include "SVTriggerProcessingClass.h"
#include "SVHardwareManifest.h"
#include "SVIOLibrary/SVIOTriggerLoadLibraryClass.h"
#include "SVTriggerClass.h"
#include "SVSoftwareTriggerClass.h"
#include "SVCameraTriggerClass.h"
#include "SVTriggerConstants.h"

SVTriggerProcessingClass& SVTriggerProcessingClass::Instance()
{
	static SVTriggerProcessingClass l_Object;

	return l_Object;
}

SVTriggerProcessingClass::SVTriggerProcessingClass()
{
}

SVTriggerProcessingClass::~SVTriggerProcessingClass()
{
	Shutdown();
}

void SVTriggerProcessingClass::Startup()
{
	const SVTriggerDeviceParamsVector& l_rTriggerParams = SVHardwareManifest::Instance().GetTriggerDeviceParams();

	SVTriggerDeviceParamsVector::const_iterator l_Iter = l_rTriggerParams.begin();

	while( l_Iter != l_rTriggerParams.end() )
	{
		SVTriggerClass* l_pTrigger = NULL;

		if( l_Iter->m_Name.find( SV_SOFTWARE_TRIGGER_NAME ) == 0 )
		{
			l_pTrigger = new SVSoftwareTriggerClass( l_Iter->m_Name.ToString() );
		}
		else if( l_Iter->m_Name.find( _T( "IO_Board" ) ) == 0 )
		{
			l_pTrigger = new SVTriggerClass( l_Iter->m_Name.ToString() );
		}
		else if( l_Iter->m_Name.find( _T( "Viper_" ) ) == 0 )
		{
			l_pTrigger = new SVTriggerClass( l_Iter->m_Name.ToString() );
		}
		else if( l_Iter->m_Name.find( SV_CAMERA_TRIGGER_NAME ) == 0 )
		{
			l_pTrigger = new SVCameraTriggerClass( l_Iter->m_Name.ToString() );
		}

		if( l_pTrigger != NULL )
		{
			l_pTrigger->miChannelNumber = l_Iter->m_Channel;

			m_Triggers[ l_Iter->m_Name ] = l_pTrigger;
		}

		++l_Iter;
	}
}

void SVTriggerProcessingClass::Shutdown()
{
	clear();

	if( !( m_Triggers.empty() ) )
	{
		SVNameTriggerMap::iterator l_Iter = m_Triggers.begin();

		while( l_Iter != m_Triggers.end() )
		{
			SVTriggerClass* l_pTrigger = l_Iter->second;

			if( l_pTrigger != NULL )
			{
				delete l_pTrigger;
			}

			l_Iter = m_Triggers.erase( l_Iter );
		}
	}
}

void SVTriggerProcessingClass::clear()
{
	if( !( m_Triggers.empty() ) )
	{
		SVNameTriggerMap::iterator l_Iter = m_Triggers.begin();

		while( l_Iter != m_Triggers.end() )
		{
			if( l_Iter->second != NULL )
			{
				l_Iter->second->m_pDLLTrigger = NULL;
				l_Iter->second->m_ulHandle = NULL;
			}

			++l_Iter;
		}
	}

	if( !( m_Subsystems.empty() ) )
	{
		m_Subsystems.clear();
	}

	if( !( m_TriggerSubsystems.empty() ) )
	{
		m_TriggerSubsystems.clear();
	}
}

HRESULT SVTriggerProcessingClass::UpdateTriggerSubsystem( SVIOTriggerLoadLibraryClass* p_pDLLTrigger )
{
	HRESULT l_hrOk = S_OK;

	if ( p_pDLLTrigger != NULL )
	{
		unsigned long l_ulSize = 0;

		l_hrOk = p_pDLLTrigger->GetCount( &l_ulSize );

		for ( unsigned long i = 0; l_hrOk == S_OK && i < l_ulSize; i++ )
		{
			SVString l_Name;

			unsigned long l_ulHandle = NULL;

			l_hrOk = p_pDLLTrigger->GetHandle( &l_ulHandle, i );

			if ( l_hrOk == S_OK )
			{
				BSTR l_bstrName = NULL;

				l_hrOk = p_pDLLTrigger->GetName( l_ulHandle, &l_bstrName );

				if( l_hrOk == S_OK )
				{
					l_Name = l_bstrName;

					if ( l_bstrName != NULL )
					{
						::SysFreeString( l_bstrName );

						l_bstrName = NULL;
					}
				}
			}
			else
			{
				l_hrOk = S_FALSE;
			}

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddTrigger( l_Name.ToString(), p_pDLLTrigger, l_ulHandle );
			}
		}
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

SVTriggerClass* SVTriggerProcessingClass::GetTrigger( LPCTSTR szName ) const
{
	SVTriggerClass* l_pTrigger = NULL;

	SVNameTriggerMap::const_iterator l_Iter = m_Triggers.find( szName );

	if( l_Iter != m_Triggers.end() )
	{
		l_pTrigger = l_Iter->second;
	}

	return l_pTrigger;
}

HRESULT SVTriggerProcessingClass::AddTrigger( LPCTSTR p_szName, SVIOTriggerLoadLibraryClass* p_pTriggerSubsystem, unsigned long p_Handle )
{
	HRESULT l_Status = S_OK;

	SVNameTriggerMap::iterator l_Iter = m_Triggers.find( p_szName );

	if( l_Iter != m_Triggers.end() && l_Iter->second != NULL )
	{
		if( l_Iter->second->m_pDLLTrigger != p_pTriggerSubsystem )
		{
			l_Iter->second->m_pDLLTrigger = p_pTriggerSubsystem;
			l_Iter->second->m_ulHandle = p_Handle;

			SVNameTriggerSubsystemMap::iterator l_SubsystemIter = m_TriggerSubsystems.find( p_szName );

			if( l_SubsystemIter != m_TriggerSubsystems.end() )
			{
				l_SubsystemIter->second = p_pTriggerSubsystem;
			}
			else
			{
				m_TriggerSubsystems[ p_szName ] = p_pTriggerSubsystem;
			}
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTriggerProcessingClass.cpp_v  $
 * 
 *    Rev 1.0   24 Apr 2013 12:29:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   21 Jan 2013 11:22:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added support for SVCameraTriggerClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   16 Jan 2013 16:46:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Migrating branch code into primary code base.  
 * 
 * Updated fucntioality associated with triggers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   22 Mar 2011 08:03:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated static global classes to singletons to promote proper and defined construction and destruction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   08 Dec 2010 13:48:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   18 Feb 2010 14:40:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated device construction fucntionality to use new hardware manifest object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   16 Feb 2010 13:47:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added the Viper triggers to the static manafest.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   15 Feb 2010 12:42:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to remove the generations of board image processing classes from the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   16 Dec 2009 13:23:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   30 Jul 2009 13:09:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   10 Sep 2008 17:03:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to support more than one trigger DLL being loaded at a time.
 * Revised to support Software Trigger.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3.1.0   14 Jul 2009 15:27:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code with new data manager objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Jan 2004 07:49:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed the Create functions to allow the caller to supply the base name
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   29 Oct 2003 14:13:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  389
 * SCR Title:  Present Error Messages for Invalid Hardware using User Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated LocalCreate method to add an error condition when the number of triggers are zero.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Sep 2003 08:48:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Changed the type of the variable that interfaces with the hardware from SVIOConfigurationInterfaceClass to SVIOTriggerLoadLibraryClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   08 Jul 2003 08:40:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added a new class to interface the Trigger sub-system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
