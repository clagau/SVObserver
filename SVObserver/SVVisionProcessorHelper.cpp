//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVVisionProcessorHelper
//* .File Name       : $Workfile:   SVVisionProcessorHelper.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   18 Jun 2013 19:23:22  $
//******************************************************************************

#include "stdafx.h"
#include <boost/config.hpp>
#include <boost/assign.hpp>
#include <boost/bind.hpp>
#include "SVVisionProcessorHelper.h"

#include "JsonLib/include/json.h"
#include "SVIMCommand/SVIMCommand.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVSystemLibrary/SVVersionInfo.h"

#include "SVConfigurationObject.h"
#include "SVConfigXMLPrint.h"
#include "SVGlobal.h"
#include "SVObserver.h"
#include "SVSocketRemoteCommandManager.h"
#include "SVSVIMStateClass.h"

SVVisionProcessorHelper& SVVisionProcessorHelper::Instance()
{
	static SVVisionProcessorHelper l_Object;

	return l_Object;
}

SVVisionProcessorHelper::SVVisionProcessorHelper()
: m_LastModifiedTime( 0 ), m_PrevModifiedTime( 0 )
{
	m_GetItemsFunctors = boost::assign::map_list_of< SVString, SVGetItemsFunctor >
		( "Windows", boost::bind( &SVVisionProcessorHelper::GetWindowsItems, this, _1, _2 ) )
		( "Environment", boost::bind( &SVVisionProcessorHelper::GetEnvironmentItems, this, _1, _2 ) )
		( "Inspections", boost::bind( &SVVisionProcessorHelper::GetInspectionItems, this, _1, _2 ) )
		( "RemoteInputs", boost::bind( &SVVisionProcessorHelper::GetRemoteInputItems, this, _1, _2 ) )
		;

	m_SetItemsFunctors = boost::assign::map_list_of< SVString, SVSetItemsFunctor >
		( "Inspections", boost::bind( &SVVisionProcessorHelper::SetInspectionItems, this, _1, _2 ) )
		( "RemoteInputs", boost::bind( &SVVisionProcessorHelper::SetRemoteInputItems, this, _1, _2 ) )
		;
}

SVVisionProcessorHelper::~SVVisionProcessorHelper()
{
	Shutdown();
}

HRESULT SVVisionProcessorHelper::GetVersion( unsigned long& p_rVersion ) const
{
	HRESULT l_Status = S_OK;

	p_rVersion = SeidenaderVision::SVVersionInfo::GetLongVersion();

	return l_Status;
}

HRESULT SVVisionProcessorHelper::GetVersion( SVString& p_rVersion ) const
{
	HRESULT l_Status = S_OK;

	p_rVersion = SeidenaderVision::SVVersionInfo::GetShortTitleVersion();

	return l_Status;
}

HRESULT SVVisionProcessorHelper::GetState( unsigned long& p_rState ) const
{
	HRESULT l_Status = S_OK;

	p_rState = 0;

	if ( SVSVIMStateClass::CheckState( SV_STATE_READY ) )
	{
		p_rState |= SVIM_CONFIG_LOADED;
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_SAVING ) )
	{
		p_rState |= SVIM_SAVING_CONFIG;
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_LOADING ) )
	{
		p_rState |= SVIM_CONFIG_LOADING;
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) )
	{
		p_rState |= SVIM_ONLINE;

		if (! SVSVIMStateClass::CheckState( SV_STATE_TEST ) ) // testing (but not regression testing) sets the running flag
		{
			p_rState |= SVIM_RUNNING;
		}
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_REGRESSION ) )
	{
		p_rState |= SVIM_REGRESSION_TEST;
	}
	else if ( SVSVIMStateClass::CheckState( SV_STATE_TEST ) )// can be testing without regression testing, but can't be regression testing without testing
	{
		p_rState |= SVIM_RUNNING_TEST;
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_EDIT ) )
	{
		p_rState |= SVIM_SETUPMODE;
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_CLOSING ) )
	{
		p_rState |= SVIM_STOPPING;
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_START_PENDING ) )
	{
		p_rState |= SVIM_ONLINE_PENDING;
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_RAID_FAILURE ) )
	{
		p_rState |= SVIM_RAID_FAILURE;
	}

	return l_Status;
}

HRESULT SVVisionProcessorHelper::GetOfflineCount( unsigned long& p_rCount ) const
{
	HRESULT l_Status = S_OK;

	p_rCount = TheSVObserverApp.m_lOfflineCount;

	return l_Status;
}

HRESULT SVVisionProcessorHelper::LoadConfiguration( const SVString& p_rPackFileName )
{
	HRESULT l_Status = TheSVObserverApp.LoadPackedConfiguration( p_rPackFileName.c_str() );

	return l_Status;
}

HRESULT SVVisionProcessorHelper::SaveConfiguration( const SVString& p_rPackFileName )
{
	HRESULT l_Status = TheSVObserverApp.SavePackedConfiguration( p_rPackFileName.c_str() );

	return l_Status;
}

HRESULT SVVisionProcessorHelper::GetConfigurationMode( unsigned long& p_rMode ) const
{
	HRESULT l_Status = S_OK;

	SVConfigurationObject* l_pConfig = NULL;

	l_Status = SVObjectManagerClass::Instance().GetConfigurationObject( l_pConfig );

	if( l_pConfig != NULL )
	{
		l_Status = l_pConfig->GetMode( p_rMode );
	}
	else if( l_Status == S_OK )
	{
		p_rMode = SVIM_MODE_UNKNOWN;

		l_Status = E_UNEXPECTED;
	}

	return l_Status;
}

HRESULT SVVisionProcessorHelper::SetConfigurationMode( unsigned long p_Mode )
{
	HRESULT l_Status = S_OK;

	SVConfigurationObject* l_pConfig = NULL;

	l_Status = SVObjectManagerClass::Instance().GetConfigurationObject( l_pConfig );

	if( l_pConfig != NULL )
	{
		l_Status = l_pConfig->SetMode( p_Mode );
	}
	else if( l_Status == S_OK )
	{
		l_Status = E_UNEXPECTED;
	}

	return l_Status;
}

HRESULT SVVisionProcessorHelper::GetConfigurationPrintReport( SVString& p_rReport ) const
{
	HRESULT l_Status = S_OK;

	SVConfigXMLPrint printSEC;
	try
	{
		p_rReport = printSEC.Print();
	}
	catch(std::exception & ex)
	{
		l_Status = atoi(ex.what());
	}
	catch(...)
	{
		l_Status = E_UNEXPECTED;
	}

	return l_Status;
}
HRESULT SVVisionProcessorHelper::GetItems( const SVNameSet& p_rNames, SVNameStorageResultMap& p_rItems ) const
{
	typedef std::map< SVString, SVNameSet > SVNameSetMap;

	HRESULT l_Status = S_OK;

	SVNameSetMap l_NameSets;

	for( SVNameSet::const_iterator l_Iter = p_rNames.begin(); l_Iter != p_rNames.end(); ++l_Iter )
	{
		SVObjectNameInfo l_Info;

		HRESULT l_LoopStatus = SVObjectNameInfo::ParseObjectName( l_Info, *l_Iter );

		if( l_LoopStatus == S_OK )
		{
			if( 0 < l_Info.m_NameArray.size() )
			{
				SVGetItemsFunctorMap::const_iterator l_FunctorIter = m_GetItemsFunctors.find( l_Info.m_NameArray[ 0 ] );

				if( l_FunctorIter != m_GetItemsFunctors.end() )
				{
					l_NameSets[ l_FunctorIter->first ].insert( *l_Iter );
				}
				else
				{
					p_rItems[ l_Iter->c_str() ] = SVStorageResult( SVStorage(), SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST, 0 );

					if( l_Status == S_OK )
					{
						l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
					}
				}
			}
			else
			{
				p_rItems[ l_Iter->c_str() ] = SVStorageResult( SVStorage(), SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST, 0 );

				if( l_Status == S_OK )
				{
					l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
				}
			}
		}
		else
		{
			p_rItems[ l_Iter->c_str() ] = SVStorageResult( SVStorage(), SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST, 0 );

			if( l_Status == S_OK )
			{
				l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
			}
		}
	}

	for( SVNameSetMap::iterator l_NameIterator = l_NameSets.begin(); l_NameIterator != l_NameSets.end(); ++l_NameIterator )
	{
		SVGetItemsFunctorMap::const_iterator l_FunctorIter = m_GetItemsFunctors.find( l_NameIterator->first );

		if( l_FunctorIter != m_GetItemsFunctors.end() )
		{
			SVNameStorageResultMap l_Items;

			HRESULT l_LoopStatus = l_FunctorIter->second( l_NameIterator->second, l_Items );

			p_rItems.insert( l_Items.begin(), l_Items.end() );

			if( l_Status == S_OK )
			{
				l_Status = l_LoopStatus;
			}
		}
		else
		{
			p_rItems[ l_NameIterator->first.c_str() ] = SVStorageResult( SVStorage(), SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST, 0 );

			if( l_Status == S_OK )
			{
				l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
			}
		}
	}

	return l_Status;
}

HRESULT SVVisionProcessorHelper::SetItems( const SVNameStorageMap& p_rItems, SVNameStatusMap& p_rStatus )
{
	typedef std::map< SVString, SVNameStorageMap > SVStringNameStorageMap;

	HRESULT l_Status = S_OK;

	SVStringNameStorageMap l_NameStorageItems;

	for( SVNameStorageMap::const_iterator l_Iter = p_rItems.begin(); l_Iter != p_rItems.end(); ++l_Iter )
	{
		SVObjectNameInfo l_Info;

		HRESULT l_LoopStatus = SVObjectNameInfo::ParseObjectName( l_Info, l_Iter->first );

		if( l_LoopStatus == S_OK )
		{
			if( 0 < l_Info.m_NameArray.size() )
			{
				SVSetItemsFunctorMap::const_iterator l_FunctorIter = m_SetItemsFunctors.find( l_Info.m_NameArray[ 0 ] );

				if( l_FunctorIter != m_SetItemsFunctors.end() )
				{
					l_NameStorageItems[ l_FunctorIter->first ].insert( *l_Iter );
				}
				else
				{
					p_rStatus[ l_Iter->first.c_str() ] = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;

					if( l_Status == S_OK )
					{
						l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
					}
				}
			}
			else
			{
				p_rStatus[ l_Iter->first.c_str() ] = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;

				if( l_Status == S_OK )
				{
					l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
				}
			}
		}
		else
		{
			p_rStatus[ l_Iter->first.c_str() ] = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;

			if( l_Status == S_OK )
			{
				l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
			}
		}
	}

	for( SVStringNameStorageMap::iterator l_NameIterator = l_NameStorageItems.begin(); l_NameIterator != l_NameStorageItems.end(); ++l_NameIterator )
	{
		SVSetItemsFunctorMap::const_iterator l_FunctorIter = m_SetItemsFunctors.find( l_NameIterator->first );

		if( l_FunctorIter != m_SetItemsFunctors.end() )
		{
			SVNameStatusMap l_StatusItems;

			HRESULT l_LoopStatus = l_FunctorIter->second( l_NameIterator->second, l_StatusItems );

			p_rStatus.insert( l_StatusItems.begin(), l_StatusItems.end() );

			if( l_Status == S_OK )
			{
				l_Status = l_LoopStatus;
			}
		}
		else
		{
			p_rStatus[ l_NameIterator->first.c_str() ] = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;

			if( l_Status == S_OK )
			{
				l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
			}
		}
	}

	return l_Status;
}

HRESULT SVVisionProcessorHelper::GetWindowsItems( const SVNameSet& p_rNames, SVNameStorageResultMap& p_rItems ) const
{
	HRESULT l_Status = TheSVObserverApp.GetWindowsItems( p_rNames, p_rItems );

	return l_Status;
}

HRESULT SVVisionProcessorHelper::GetEnvironmentItems( const SVNameSet& p_rNames, SVNameStorageResultMap& p_rItems ) const
{
	HRESULT l_Status = TheSVObserverApp.GetEnvironmentItems( p_rNames, p_rItems );

	return l_Status;
}

HRESULT SVVisionProcessorHelper::GetInspectionItems( const SVNameSet& p_rNames, SVNameStorageResultMap& p_rItems ) const
{
	HRESULT l_Status = S_OK;

	SVConfigurationObject* l_pConfig = NULL;

	l_Status = SVObjectManagerClass::Instance().GetConfigurationObject( l_pConfig );

	if( l_pConfig != NULL )
	{
		l_Status = l_pConfig->GetInspectionItems( p_rNames, p_rItems );
	}
	else if( l_Status == S_OK )
	{
		l_Status = E_UNEXPECTED;
	}

	return l_Status;
}

HRESULT SVVisionProcessorHelper::GetRemoteInputItems( const SVNameSet& p_rNames, SVNameStorageResultMap& p_rItems ) const
{
	HRESULT l_Status = S_OK;

	SVConfigurationObject* l_pConfig = NULL;

	l_Status = SVObjectManagerClass::Instance().GetConfigurationObject( l_pConfig );

	if( l_pConfig != NULL )
	{
		l_Status = l_pConfig->GetRemoteInputItems( p_rNames, p_rItems );
	}
	else if( l_Status == S_OK )
	{
		l_Status = E_UNEXPECTED;
	}

	return l_Status;
}

HRESULT SVVisionProcessorHelper::SetInspectionItems( const SVNameStorageMap& p_rItems, SVNameStatusMap& p_rStatus )
{
	HRESULT l_Status = S_OK;

	SVConfigurationObject* l_pConfig = NULL;

	l_Status = SVObjectManagerClass::Instance().GetConfigurationObject( l_pConfig );

	if( l_pConfig != NULL )
	{
		l_Status = l_pConfig->SetInspectionItems( p_rItems, p_rStatus );
	}
	else if( l_Status == S_OK )
	{
		l_Status = E_UNEXPECTED;
	}

	return l_Status;
}

HRESULT SVVisionProcessorHelper::SetRemoteInputItems( const SVNameStorageMap& p_rItems, SVNameStatusMap& p_rStatus )
{
	HRESULT l_Status = S_OK;

	SVConfigurationObject* l_pConfig = NULL;

	l_Status = SVObjectManagerClass::Instance().GetConfigurationObject( l_pConfig );

	if( l_pConfig != NULL )
	{
		l_Status = l_pConfig->SetRemoteInputItems( p_rItems, p_rStatus );
	}
	else if( l_Status == S_OK )
	{
		l_Status = E_UNEXPECTED;
	}

	return l_Status;
}

void SVVisionProcessorHelper::Startup()
{
	m_AsyncProcedure.Create( &SVVisionProcessorHelper::APCThreadProcess, boost::bind(&SVVisionProcessorHelper::ThreadProcess, this, _1), "SVVisionProcessorHelper" );
}

void SVVisionProcessorHelper::Shutdown()
{
	m_AsyncProcedure.Destroy();
}

HRESULT SVVisionProcessorHelper::SetLastModifiedTime()
{
	HRESULT l_Status = S_OK;

	__time32_t l_LastModifiedTime = m_LastModifiedTime;

	::InterlockedExchange( &m_LastModifiedTime, ::_time32( NULL ) );

	if( l_LastModifiedTime != m_LastModifiedTime )
	{
		l_Status = m_AsyncProcedure.Signal( NULL );
	}

	return l_Status;
}

void CALLBACK SVVisionProcessorHelper::APCThreadProcess( DWORD dwParam )
{
}

void SVVisionProcessorHelper::ThreadProcess( bool& p_WaitForEvents )
{
	ProcessLastModified( p_WaitForEvents );
}

void SVVisionProcessorHelper::ProcessLastModified( bool& p_WaitForEvents )
{
	if( m_PrevModifiedTime != m_LastModifiedTime )
	{
		std::string l_JsonNotification;
		Json::FastWriter l_Writer;
		Json::Value l_Object(Json::objectValue);
		Json::Value l_ElementObject(Json::objectValue);

		l_ElementObject[ _T( "TimeStamp" ) ] = m_LastModifiedTime;

		l_Object[ _T( "Notification" ) ] = _T( "LastModified" );
		l_Object[ _T( "DataItems" ) ] = l_ElementObject;

		l_JsonNotification = l_Writer.write( l_Object ).c_str();

		SVSocketRemoteCommandManager::Instance().ProcessJsonNotification( l_JsonNotification );

		m_PrevModifiedTime = m_LastModifiedTime;
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVVisionProcessorHelper.cpp_v  $
 * 
 *    Rev 1.2   18 Jun 2013 19:23:22   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 019.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   03 Jun 2013 03:37:38   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  841
 * SCR Title:  Change configuration print report format to xml
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   31 May 2013 08:28:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  839
 * SCR Title:  Add Last Modified Notification to Remote Socket Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object ot handle the notification fucntkionaity by adding a tread to queue the notifications and push them out the socket when the socket is ready.  If the client socket is disconnected, the notification queue is emptied.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   11 Jun 2013 15:37:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 017.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   21 May 2013 13:21:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  824
 * SCR Title:  Add Remote Access to SVObserver Application Configuration Variables
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Udated get and set items methods to allow for the new naming requirements in IF00100.9401.003 and the ability to collect application elements and remote input elements.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 16:29:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Feb 2013 12:50:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial check-in for new framework objects used for new remote socket interface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
