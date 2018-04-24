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

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comdef.h>
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <set>
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/function.hpp>
#include "SVSystemLibrary/SVAsyncProcedure.h"

#include "Definitions/StringTypeDef.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVSharedMemoryLibrary/SVProductFilterEnum.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVStatusLibrary/MessageNotification.h"
#include "SVSystemLibrary/SVAsyncProcedure.h"
#include "SVStorage.h"
#include "SVStorageResult.h"
#include "SVVisionProcessorConstants.h"
#include "SVDataDefinitionStruct.h"
#include "RemoteMonitorNamedList.h"
#include "SVPPQObject.h"
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

struct MonitorlistProperties
{
	
	MonitorlistProperties():RejectQueDepth(0), isActive(false)
	{};
	int RejectQueDepth;
	bool isActive;
	std::string ppqName;
};

const TCHAR StandardItems[] = _T( "StandardItems" );
#pragma endregion Declarations

class SVVisionProcessorHelper
{
public:
	static SVVisionProcessorHelper& Instance();

	virtual ~SVVisionProcessorHelper();

	HRESULT GetVersion( unsigned long& rVersion ) const;
	HRESULT GetVersion( std::string& rVersion ) const;

	HRESULT GetState( unsigned long& rState ) const;

	HRESULT GetOfflineCount( unsigned long& rCount ) const;

	HRESULT LoadConfiguration( const std::string& rPackFileName );

	HRESULT SaveConfiguration( const std::string& rPackFileName ) const;

	HRESULT GetConfigurationMode( unsigned long& rMode ) const;
	HRESULT SetConfigurationMode( unsigned long Mode );

	HRESULT GetConfigurationPrintReport( std::string& rReport ) const;

	HRESULT GetDataDefinitionList( const std::string& rInspectionName, const SVDataDefinitionListType& rListType, SVDataDefinitionStructVector& rDataDefinitionArray) const;

	//************************************
	//! Get Items value for the Item in NameSet to the SVNameStorageResultMap 
	//! \param rNames [in]
	//! \param rItems [out]
	//! \returns HRESULT
	//************************************
	HRESULT GetItems( const SvDef::StringSet& rNames, SVNameStorageResultMap& rItems) const;
	
	//************************************
	//! Set the item values in SVnameStorage map Result are in SVNameStatusMap 
	//! \param rItems [in]
	//! \param rStatusItems [out]
	//! \param RunOnce [in] True if run once should be called
	//! \returns HRESULT
	//************************************
	HRESULT SetItems(const SVNameStorageMap& rItems, SVNameStatusMap& rStatusItems,  bool RunOnce);

	//************************************
	//! Starts an Message notification via SVRC
	//! \param type 
	//! \param ErrorNumber 
	//! \param errormessage 
	//! \returns HRESULT
	//************************************
	HRESULT FireNotification( int Type, int MesssageNumber, LPCTSTR MessageText );
	HRESULT QueryProductList( const std::string& rListName, SvDef::StringSet& rNames ) const;
	HRESULT QueryRejectCondList( const std::string& rListName, SvDef::StringSet& rNames ) const;
	HRESULT QueryFailStatusList( const std::string& rListName, SvDef::StringSet& rNames ) const;
	HRESULT ActivateMonitorList( const std::string& rListName, bool bActivate );
	HRESULT QueryMonitorListNames( SvDef::StringSet& rNames ) const;
	HRESULT GetInspectionNames( SvDef::StringSet& rNames ) const;
	HRESULT SetProductFilter( const std::string& rListName, SvSml::SVProductFilterEnum filter );
	HRESULT GetProductFilter( const std::string& rListName, SvSml::SVProductFilterEnum& filter ) const;
	HRESULT RegisterMonitorList( const std::string& rListName, const std::string& rPPQName, int rejectDepth, const SvDef::StringSet& rProdList, const SvDef::StringSet& rRejectCondList, const SvDef::StringSet& rFailStatusList, SVNameStatusMap& rStatusOfItemsWithError );
	
	
	//! Give the Properties of an existing Monitorlist
	//! \param rListName [in]
	//! \param ppqName [out]
	//! \param RejectQueDepth [out]
	//! \param isActive [out]
	//! \returns HRESULT
	HRESULT GetMonitorListProperties(const std::string& rListName, MonitorlistProperties& properties);
	
protected:
	typedef boost::function< HRESULT ( const SvDef::StringSet&, SVNameStorageResultMap& ) > SVGetItemsFunctor;
	typedef boost::function< HRESULT ( const SVNameStorageMap&, SVNameStatusMap&, bool RunOnce ) > SVSetItemsFunctor;
	typedef std::map<std::string, SVGetItemsFunctor> SVGetItemsFunctorMap;
	typedef std::map<std::string, SVSetItemsFunctor> SVSetItemsFunctorMap;

	static void CALLBACK APCThreadProcess( DWORD_PTR dwParam );

	HRESULT GetStandardItems( const SvDef::StringSet& rNames, SVNameStorageResultMap& rItems ) const;
	HRESULT GetInspectionItems( const SvDef::StringSet& rNames, SVNameStorageResultMap& rItems ) const;
	HRESULT GetRemoteInputItems( const SvDef::StringSet& rNames, SVNameStorageResultMap& rItems ) const;

	HRESULT SetStandardItems(const SVNameStorageMap& rItems, SVNameStatusMap& rStatus, bool RunOnce);
	HRESULT SetInspectionItems(const SVNameStorageMap& rItems, SVNameStatusMap& rStatus, bool RunOnce);
	HRESULT SetRemoteInputItems(const SVNameStorageMap& rItems, SVNameStatusMap& rStatus, bool RunOnce);
	HRESULT SetCameraItems(const SVNameStorageMap& rItems, SVNameStatusMap& rStatus, bool RunOnce);

	HRESULT GetObjectDefinition( const SVObjectClass& rObj, const long p_Filter, SVDataDefinitionStruct& rDataDef ) const;

	void ThreadProcess( bool& rWaitForEvents );

	void ProcessLastModified( bool& rWaitForEvents );
	void ProcessNotification( bool& rWaitForEvents );
	void NotifyModeChanged( bool& rWaitForEvents );

	SVGetItemsFunctorMap m_GetItemsFunctors;
	SVSetItemsFunctorMap m_SetItemsFunctors;

private:
#pragma region Private Methods
	SVVisionProcessorHelper();
	SVVisionProcessorHelper( const SVVisionProcessorHelper& rObject );
	const SVVisionProcessorHelper& operator=( const SVVisionProcessorHelper& rObject );

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
	void SetValuesOrImagesMonitoredObjectLists( const SvDef::StringSet& rObjectNameList, const SVPPQObject& pPPQ, MonitoredObjectList &rMonitoredValueObjectList, MonitoredObjectList *pMonitoredImageObjectList, SVNameStatusMap &rStatus, HRESULT &hr );
#pragma endregion Private Methods

#pragma region Private Members
private:   //Data

	SvStl::MessageNotification m_MessageNotification;
#pragma endregion Private Members
};

