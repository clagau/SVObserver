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
#include "Definitions/StringTypeDef.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVSharedMemoryLibrary/SVProductFilterEnum.h"
#include "SVStorage.h"
#include "SVStorageResult.h"
#include "SVDataDefinitionStruct.h"
#include "RemoteMonitorNamedList.h"
#include "SVPPQObject.h"
#include "SVRPCLibrary/Observer.h"
#include "SVRPCLibrary/ServerStreamContext.h"
#pragma endregion Includes

#pragma region Declarations
//This enum is the same as the client interface of the SVRC
enum SVDataDefinitionListType
{
	UnknownDataDefinition = 0,
	SelectedValues = 1,
	SelectedImages = 2,
	SelectedValuesAndSelectedImages = 3,
	AllValues = 4,
	AllValuesAndSelectedImages = 6,
	AllImages = 8,
	SelectedValuesAndAllImages = 9,
	AllValuesAndAllImages = 12,
};

struct MonitorlistProperties
{

	MonitorlistProperties() :RejectQueDepth(0), isActive(false)
	{
	};
	int RejectQueDepth;
	bool isActive;
	std::string ppqName;
};

constexpr TCHAR StandardItems[] = _T("StandardItems");
#pragma endregion Declarations

class SVVisionProcessorHelper
{
public:
	static SVVisionProcessorHelper& Instance();

	virtual ~SVVisionProcessorHelper();

	HRESULT LoadConfiguration(std::string& rFileName) const;

	HRESULT GetConfigurationPrintReport(std::string& rReport) const;

	HRESULT GetDataDefinitionList(const std::string& rInspectionName, const SVDataDefinitionListType& rListType, SVDataDefinitionStructVector& rDataDefinitionArray) const;

	//************************************
	//! Get Items value for the Item in NameSet to the SVNameStorageResultMap 
	//! \param rNames [in]
	//! \param rItems [out]
	//! \returns HRESULT
	//************************************
	HRESULT GetItems(const SvDef::StringSet& rNames, SVNameStorageResultMap& rItems) const;

	//************************************
	//! Set the item values in SVnameStorage map Result are in SVNameStatusMap 
	//! \param rItems [in]
	//! \param rStatusItems [out]
	//! \param RunOnce [in] True if run once should be called
	//! \returns HRESULT
	//************************************
	HRESULT SetItems(const SVNameStorageMap& rItems, SVNameStatusMap& rStatusItems, bool RunOnce);

	void FireNotification(SvPb::NotifyType notifyType, const _variant_t& parameter) const;
	void FireMessageNotification(const SvStl::MessageContainer& rMessage, int messageType) const;
	HRESULT QueryProductList(const std::string& rListName, SvDef::StringSet& rNames) const;
	HRESULT QueryRejectCondList(const std::string& rListName, SvDef::StringSet& rNames) const;
	HRESULT QueryFailStatusList(const std::string& rListName, SvDef::StringSet& rNames) const;
	HRESULT ActivateMonitorList(const std::string& rListName, bool bActivate);
	HRESULT QueryMonitorListNames(SvDef::StringSet& rNames) const;
	HRESULT GetInspectionNames(SvDef::StringSet& rNames) const;
	HRESULT SetProductFilter(const std::string& rListName, SvSml::SVProductFilterEnum filter);
	HRESULT GetProductFilter(const std::string& rListName, SvSml::SVProductFilterEnum& filter) const;
	HRESULT RegisterMonitorList(const std::string& rListName, const std::string& rPPQName, int rejectDepth, const SvDef::StringSet& rProdList, const SvDef::StringSet& rRejectCondList, const SvDef::StringSet& rFailStatusList, SVNameStatusMap& rStatusOfItemsWithError);


	//! Give the Properties of an existing Monitorlist
	//! \param rListName [in]
	//! \param ppqName [out]
	//! \param RejectQueDepth [out]
	//! \param isActive [out]
	//! \returns HRESULT
	HRESULT GetMonitorListProperties(const std::string& rListName, MonitorlistProperties& properties);

	void Startup(); // This method is only meant to be called by the main application class
	void Shutdown();	// This method is only meant to be called by the main application certain class
	void RegisterNotificationStream(const SvPb::GetNotificationStreamRequest& rRequest,
		SvRpc::Observer<SvPb::GetNotificationStreamResponse> observer,
		SvRpc::ServerStreamContext::Ptr ctx);
	void RegisterMessageStream(const SvPb::GetMessageStreamRequest& rRequest,
		SvRpc::Observer<SvPb::GetMessageStreamResponse> Observer,
		SvRpc::ServerStreamContext::Ptr ctx);

	void setMainhWnd(HWND mainhWnd) { m_mainhWnd = mainhWnd; }
private:
	using SVGetItemsFunctor = std::function<HRESULT(const SvDef::StringSet&, SVNameStorageResultMap&)>;
	using SVSetItemsFunctor = std::function<HRESULT(const SVNameStorageMap&, SVNameStatusMap&, bool RunOnce)>;
	typedef std::map<std::string, SVGetItemsFunctor> SVGetItemsFunctorMap;
	typedef std::map<std::string, SVSetItemsFunctor> SVSetItemsFunctorMap;



	HRESULT GetStandardItems(const SvDef::StringSet& rNames, SVNameStorageResultMap& rItems) const;
	HRESULT GetInspectionItems(const SvDef::StringSet& rNames, SVNameStorageResultMap& rItems) const;
	HRESULT GetRemoteInputItems(const SvDef::StringSet& rNames, SVNameStorageResultMap& rItems) const;

	HRESULT SetStandardItems(const SVNameStorageMap& rItems, SVNameStatusMap& rStatus, bool RunOnce);
	HRESULT SetInspectionItems(const SVNameStorageMap& rItems, SVNameStatusMap& rStatus, bool RunOnce);
	HRESULT SetRemoteInputItems(const SVNameStorageMap& rItems, SVNameStatusMap& rStatus, bool RunOnce);
	HRESULT SetCameraItems(const SVNameStorageMap& rItems, SVNameStatusMap& rStatus, bool RunOnce);

	HRESULT GetObjectDefinition(const SvOi::IObjectClass& rObj, const long p_Filter, SVDataDefinitionStruct& rDataDef) const;


	void ProcessNotification(SvPb::GetNotificationStreamResponse&& response) const;

	SVGetItemsFunctorMap m_GetItemsFunctors;
	SVSetItemsFunctorMap m_SetItemsFunctors;


	SVVisionProcessorHelper() = default;
	SVVisionProcessorHelper(const SVVisionProcessorHelper& rObject) = delete;
	const SVVisionProcessorHelper& operator=(const SVVisionProcessorHelper& rObject) = delete;

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
	void SetValuesOrImagesMonitoredObjectLists(const SvDef::StringSet& rObjectNameList, const SVPPQObject& pPPQ, MonitoredObjectList &rMonitoredValueObjectList, MonitoredObjectList *pMonitoredImageObjectList, SVNameStatusMap &rStatus, HRESULT &hr);


#pragma region Private Members
private:
	struct NotificationSubscription
	{
		SvRpc::Observer<SvPb::GetNotificationStreamResponse> m_observer;
		SvRpc::ServerStreamContext::Ptr m_context;
		std::vector<int> m_notifyList;
	};
	struct MessageSubscription
	{
		SvRpc::Observer<SvPb::GetMessageStreamResponse> m_observer;
		SvRpc::ServerStreamContext::Ptr m_context;
		std::vector<int> m_severityList;
	};

	mutable std::vector<NotificationSubscription> m_Subscriptions;
	mutable std::vector<MessageSubscription> m_MessageSubscriptions;
	mutable std::mutex m_SubscriptionsMutex;

	HWND m_mainhWnd {nullptr};
#pragma endregion Private Members
};

