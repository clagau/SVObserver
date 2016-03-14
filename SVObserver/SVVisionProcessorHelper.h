//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVVisionProcessorHelper
//* .File Name       : $Workfile:   SVVisionProcessorHelper.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.12  $
//* .Check In Date   : $Date:   20 Nov 2014 05:06:18  $
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
#include "SVSharedMemoryLibrary/SVProductFilterEnum.h"
#include "SVStorage.h"
#include "SVStorageResult.h"
#include "SVVisionProcessorConstants.h"
#include "SVDataDefinitionStruct.h"
#include "RemoteMonitorNamedList.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "MessageNotification.h"
#pragma endregion Includes

#pragma region Declarations
enum svModeEnum;
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

const TCHAR			StandardItems[]				= _T( "StandardItems" );
#pragma endregion Declarations

typedef std::map<SVGUID,SVString> SVErrorMap;

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

	//************************************
	//! Get Items value for the Item in NameSet to the SVNameStorageResultMap 
	//! \param p_rNames [in]
	//! \param p_rItems [out]
	//! \param IsOneBased [in] if One based is true the index for arrays are taken as one based otherwise as zero based  
	//! \returns HRESULT
	//************************************
	HRESULT GetItems( const SVNameSet& p_rNames, SVNameStorageResultMap& p_rItems , bool IsOneBased) const;
	
	//************************************
	//! Set the item values in SVnameStorage map Result are in SVNameStatusMap 
	//! \param p_rItems [in]
	//! \param rStatusOfItems [out]
	//! \param IsOneBased [in] if One based is true the index for arrays are taken as one based otherwise as zero based  
	//! \returns HRESULT
	//************************************
	HRESULT SetItems( const SVNameStorageMap& p_rItems, SVNameStatusMap& rStatusOfItems,  bool IsOneBased );

	HRESULT SetLastModifiedTime();
	HRESULT FireModeChanged(svModeEnum mode);
	//************************************
	//! Starts an Message notification via SVRC
	//! \param type 
	//! \param ErrorNumber 
	//! \param errormessage 
	//! \returns HRESULT
	//************************************
	HRESULT FireMessageNotification(SvStl::NotificationEnum type, int ErrorNumber, LPCTSTR errormessage  );
	HRESULT QueryProductList( const SVString& rListName, SVNameSet& rNames ) const;
	HRESULT QueryRejectCondList( const SVString& rListName, SVNameSet& rNames ) const;
	HRESULT QueryFailStatusList( const SVString& rListName, SVNameSet& rNames ) const;
	HRESULT ActivateMonitorList( const SVString& rListName, bool bActivate );
	HRESULT QueryMonitorListNames( SVNameSet& rNames ) const;
	HRESULT GetInspectionNames( SVNameSet& rNames ) const;
	HRESULT SetProductFilter( const SVString& rListName, SVProductFilterEnum filter );
	HRESULT GetProductFilter( const SVString& rListName, SVProductFilterEnum& filter ) const;
	HRESULT RegisterMonitorList( const SVString& rListName, const SVString& rPPQName, int rejectDepth, const SVNameSet& rProdList, const SVNameSet& rRejectCondList, const SVNameSet& rFailStatusList, SVNameStatusMap& rStatusOfItemsWithError );


	//methods for Tool Errors for going into RunMode
	//////////////////////////////////////////
	// ClearToolErrorMap 
	// Clears the map of tool errors before the start of going into RunMode
	//////////////////////////////////////////
	void ClearToolErrorMap();
	
	//////////////////////////////////////////
    // AddToolError
	// Parameters:
	//     - SVGUID - the Tool's Unique ID
	//     - SVString - Error Message for that tool
	//////////////////////////////////////////
	void AddToolError(SVGUID guid, SVString sErrorMessage);

	//////////////////////////////////////////
    // GetNumberofToolError
	// Returns the number of errors in the map. 
	//////////////////////////////////////////
	int GetNumberOfToolErrors() const;

	//////////////////////////////////////////
	// GetFirstErrorMessage
	// returns true along with the SVGUID and ErrorMessage string.
	// returns false if it was unable to get anything from the map. this should never happen
	//////////////////////////////////////////
	bool GetFirstErrorMessage(SVGUID &guid, SVString &sErrorMessage) const;

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

	HRESULT SetStandardItems( const SVNameStorageMap& p_rItems, SVNameStatusMap& p_rStatus );
	HRESULT SetInspectionItems( const SVNameStorageMap& p_rItems, SVNameStatusMap& p_rStatus );
	HRESULT SetRemoteInputItems( const SVNameStorageMap& p_rItems, SVNameStatusMap& p_rStatus );
	HRESULT SetCameraItems( const SVNameStorageMap& p_rItems, SVNameStatusMap& p_rStatus );

	HRESULT GetObjectDefinition( const SVObjectClass& p_rObj, const long p_Filter, SVDataDefinitionStruct& p_rDataDef ) const;

	void ThreadProcess( bool& p_WaitForEvents );

	void ProcessLastModified( bool& p_WaitForEvents );
	void NotifyModeChanged( bool& p_WaitForEvents );

	SVGetItemsFunctorMap m_GetItemsFunctors;
	SVSetItemsFunctorMap m_SetItemsFunctors;

	__time32_t m_PrevModifiedTime;
	__time32_t m_LastModifiedTime;
	svModeEnum m_prevMode;
	svModeEnum m_lastMode;

	SVAsyncProcedure< SVAPCSignalHandler, SVThreadProcessHandler > m_AsyncProcedure;
	
private:
#pragma region Private Methods
	SVVisionProcessorHelper( const SVVisionProcessorHelper& p_rObject );
	const SVVisionProcessorHelper& operator=( const SVVisionProcessorHelper& p_rObject );

	//************************************
	// Method:    SetValuesOrImagesMonitoredObjectLists
	// Description:  Check if objects exist for the all names from the list, if the object valid and part of the ppq and if no error add them to the monitored-Lists, else add error to status list.
	// Parameter: const SVNameSet & rObjectNameList Name list of the objects, which should be added.
	// Parameter: const SVPPQObject & pPPQ The PPQ object
	// Parameter: MonitoredObjectList & rMonitoredValueObjectList [out] Referents of the monitored list for value object.
	// Parameter: MonitoredObjectList * pMonitoredImageObjectList [out] Pointer to the monitored list for image object, if no images permited, is pointer should nullptr. Then every image object get an error.
	// Parameter: SVNameStatusMap & rStatus [out] Status of the objects with errors.
	// Parameter: HRESULT & hr [in/out] Change the result value only if an error happens. If no error happens input is equal output.
	// Returns:   void
	//************************************
	void SetValuesOrImagesMonitoredObjectLists( const SVNameSet& rObjectNameList, const SVPPQObject& pPPQ, MonitoredObjectList &rMonitoredValueObjectList, MonitoredObjectList *pMonitoredImageObjectList, SVNameStatusMap &rStatus, HRESULT &hr );
#pragma endregion Private Methods

#pragma region Private Members
private:   //Data
	SVErrorMap m_ToolErrorMap;
	MessageNotification m_MessageNotification;
#pragma endregion Private Members
};

#endif

