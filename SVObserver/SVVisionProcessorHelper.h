//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVVisionProcessorHelper
//* .File Name       : $Workfile:   SVVisionProcessorHelper.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   17 Mar 2014 15:35:00  $
//******************************************************************************

#ifndef SVVISIONPROCESSORHELPER_H
#define SVVISIONPROCESSORHELPER_H

#pragma region Includes
#include <comdef.h>
#include <map>
#include <set>
#include <boost/config.hpp>
#include <boost/function.hpp>
#include "SVSystemLibrary/SVAsyncProcedure.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVStorage.h"
#include "SVStorageResult.h"
#include "SVVisionProcessorConstants.h"
#include "SVDataDefinitionStruct.h"
#pragma endregion Includes

#pragma region Declarations
//This enum is the same as the client interface of the SVRC
enum SVDataDefinitionListType
{
	UnknownDataDefinition			= 0,
	SelectedValues					= 1,
	SelectedImages					= 2,
	SelectedValuesAndSelectedImages = 3,
	AllValues						= 4,
	AllValuesAndSelectedImages		= 6,
	AllImages						= 8,
	SelectedValuesAndAllImages		= 9,
	AllValuesAndAllImages			= 12,
};
#pragma endregion Declarations

class SVVisionProcessorHelper
{
public:
	friend class SVObserverApp;

	static SVVisionProcessorHelper& Instance();

	virtual ~SVVisionProcessorHelper();

	HRESULT GetVersion( unsigned long& p_rVersion ) const;
	HRESULT GetVersion( SVString& p_rVersion ) const;

	HRESULT GetState( unsigned long& p_rState ) const;

	HRESULT GetOfflineCount( unsigned long& p_rCount ) const;

	HRESULT LoadConfiguration( const SVString& p_rPackFileName );

	HRESULT SaveConfiguration( const SVString& p_rPackFileName ) const;

	HRESULT GetConfigurationMode( unsigned long& p_rMode ) const;
	HRESULT SetConfigurationMode( unsigned long p_Mode );

	HRESULT GetConfigurationPrintReport( SVString& p_rReport ) const;

	HRESULT GetDataDefinitionList( const SVString& p_rInspectionName, const SVDataDefinitionListType& p_rListType, SVDataDefinitionStructArray& p_rDataDefinitionArray) const;

	HRESULT GetItems( const SVNameSet& p_rNames, SVNameStorageResultMap& p_rItems ) const;
	HRESULT SetItems( const SVNameStorageMap& p_rItems, SVNameStatusMap& p_rStatus );

	HRESULT SetLastModifiedTime();

protected:
	typedef boost::function< HRESULT ( const SVNameSet&, SVNameStorageResultMap& ) > SVGetItemsFunctor;
	typedef boost::function< HRESULT ( const SVNameStorageMap&, SVNameStatusMap& ) > SVSetItemsFunctor;
	typedef std::map< SVString, SVGetItemsFunctor > SVGetItemsFunctorMap;
	typedef std::map< SVString, SVSetItemsFunctor > SVSetItemsFunctorMap;
	typedef void ( CALLBACK * SVAPCSignalHandler )( DWORD_PTR );
	typedef boost::function<void ( bool& )> SVThreadProcessHandler;

	static void CALLBACK APCThreadProcess( DWORD_PTR dwParam );

	SVVisionProcessorHelper();

	void Startup();
	void Shutdown();

	HRESULT GetStandardItems( const SVNameSet& p_rNames, SVNameStorageResultMap& p_rItems ) const;
	HRESULT GetInspectionItems( const SVNameSet& p_rNames, SVNameStorageResultMap& p_rItems ) const;
	HRESULT GetRemoteInputItems( const SVNameSet& p_rNames, SVNameStorageResultMap& p_rItems ) const;

	HRESULT SetInspectionItems( const SVNameStorageMap& p_rItems, SVNameStatusMap& p_rStatus );
	HRESULT SetRemoteInputItems( const SVNameStorageMap& p_rItems, SVNameStatusMap& p_rStatus );
	HRESULT SetCameraItems( const SVNameStorageMap& p_rItems, SVNameStatusMap& p_rStatus );

	HRESULT GetObjectDefinition( const SVObjectClass& p_rObj, const long p_Filter, SVDataDefinitionStruct& p_rDataDef ) const;

	void ThreadProcess( bool& p_WaitForEvents );

	void ProcessLastModified( bool& p_WaitForEvents );

	SVGetItemsFunctorMap m_GetItemsFunctors;
	SVSetItemsFunctorMap m_SetItemsFunctors;

	__time32_t m_PrevModifiedTime;
	__time32_t m_LastModifiedTime;

	SVAsyncProcedure< SVAPCSignalHandler, SVThreadProcessHandler > m_AsyncProcedure;

private:
#pragma region Private Methods
	SVVisionProcessorHelper( const SVVisionProcessorHelper& p_rObject );
	const SVVisionProcessorHelper& operator=( const SVVisionProcessorHelper& p_rObject );
#pragma endregion Private Methods
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVVisionProcessorHelper.h_v  $
 * 
 *    Rev 1.6   17 Mar 2014 15:35:00   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  869
 * SCR Title:  Add PPQ and Environment Variables to Object Manager and Update Pickers
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Removed GetWindowItems and GetEnviromentItems and created GetStandardItems and SetCameraItems.Added method SetCameraItems.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   07 Mar 2014 18:25:58   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added regions.
 *   Made methods const.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   30 Oct 2013 15:43:26   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  866
 * SCR Title:  Add GetDataDefinitionList Command to SVObserver's Remote Command Socket
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added include of SVObjectManagerClass.h from SVVisionProcessorHelper.cpp.
 *   Added SVDataDefinitionListType enum.
 *   Added GetDataDefinitionList and GetObjectDefinition.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   02 Oct 2013 08:39:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   18 Jun 2013 19:23:24   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 019.
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
 *    Rev 1.1   11 Jun 2013 15:37:44   bWalter
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
 *    Rev 1.0   23 Apr 2013 16:30:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Feb 2013 12:50:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial check-in for new framework objects used for new remote socket interface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
