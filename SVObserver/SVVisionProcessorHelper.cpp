//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVVisionProcessorHelper
//* .File Name       : $Workfile:   SVVisionProcessorHelper.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.26  $
//* .Check In Date   : $Date:   01 Dec 2014 13:22:48  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/bind.hpp>
#include "SVVisionProcessorHelper.h"

#include "RemoteCommand.h"
#include "RemoteMonitorListController.h"
#include "SVConfigurationObject.h"
#include "SVConfigXMLPrint.h"
#include "SVObserver.h"
#include "SVRemoteControlConstants.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVValueObjectLibrary/BasicValueObject.h"
#include "RemoteMonitorListHelper.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/SVUserMessage.h"
#include "Definitions/StringTypeDef.h"
#include "SVProtoBuf/ConverterHelper.h"
#include "SVProtoBuf/SVRC.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

void FireNotification(SvPb::NotifyType notifyType, const _variant_t& parameter)
{
	SVVisionProcessorHelper::Instance().FireNotification(notifyType, parameter);
}

void FireMessageNotification(const SvStl::MessageContainer& rMessage, int messageType)
{
	SVVisionProcessorHelper::Instance().FireMessageNotification(rMessage, messageType);
}

SVVisionProcessorHelper& SVVisionProcessorHelper::Instance()
{
	static SVVisionProcessorHelper object;

	return object;
}

#pragma region Constructor
SVVisionProcessorHelper::SVVisionProcessorHelper()
{

	m_GetItemsFunctors = SVGetItemsFunctorMap
	{
		{StandardItems, boost::bind(&SVVisionProcessorHelper::GetStandardItems, this, _1, _2)},
		{SvDef::FqnInspections, boost::bind(&SVVisionProcessorHelper::GetInspectionItems, this, _1, _2)},
		{SvDef::FqnRemoteInputs, boost::bind(&SVVisionProcessorHelper::GetRemoteInputItems, this, _1, _2)}
	};

	m_SetItemsFunctors = SVSetItemsFunctorMap
	{
		{StandardItems, boost::bind(&SVVisionProcessorHelper::SetStandardItems, this, _1, _2, _3)},
		{SvDef::FqnInspections, boost::bind(&SVVisionProcessorHelper::SetInspectionItems, this, _1, _2, _3)},
		{SvDef::FqnRemoteInputs, boost::bind(&SVVisionProcessorHelper::SetRemoteInputItems, this, _1, _2, _3)},
		{SvDef::FqnCameras, boost::bind(&SVVisionProcessorHelper::SetCameraItems, this, _1, _2, _3)}
	};
}

SVVisionProcessorHelper::~SVVisionProcessorHelper()
{
	Shutdown();
}
#pragma endregion Constructor

HRESULT SVVisionProcessorHelper::LoadConfiguration(std::string& rFileName) const
{
	HRESULT Result{S_OK};

	if (!SVSVIMStateClass::CheckState(SV_STATE_TEST | SV_STATE_REGRESSION))
	{
		TCHAR szDrive[_MAX_DRIVE];
		TCHAR szDir[_MAX_DIR];
		TCHAR szFile[_MAX_FNAME];
		TCHAR szExt[_MAX_EXT];
		bool bSuccess {false};

		_tsplitpath(rFileName.c_str(), szDrive, szDir, szFile, szExt);
		
		std::string ConfigFile{rFileName};
		if (!_tcscmp(szDrive, _T("")))
		{ //just the file name, search the run directory for the filename
			if (0 == _tcscmp(szExt, SvDef::cConfigExtension) || 0 == _tcscmp(szExt, _T("")))
			{
				ConfigFile = SvStl::GlobalPath::Inst().GetRunPath() + _T("\\");
				ConfigFile += szFile;
				ConfigFile += SvDef::cConfigExtension;
				//check for existence of file first
				bSuccess = (0 == _access(ConfigFile.c_str(), 0));

				if (bSuccess)
				{
					//global function to close config and clean up c:\run dir
					GlobalRCCloseAndCleanConfiguration();
				}
			}
			else
			{
				bSuccess = false;
			}
		}
		else if (0 == _tcscmp(szExt, SvDef::cConfigExtension) || 0 == _tcscmp(szExt, SvDef::cPackedConfigExtension))
		{
			//check for existence of file first
			bSuccess = (0 == _access(ConfigFile.c_str(), 0));

			if (bSuccess)
			{
				//global function to close config and clean up c:\run dir
				GlobalRCCloseAndCleanConfiguration();
			}
		}
		else
		{
			bSuccess = false;
		}

		if (bSuccess)
		{
			GlobalRCOpenConfiguration(ConfigFile.c_str());
		}
		else
		{
			Result = SVMSG_CMDCOMCTRL_FILE_ERROR;
		}
	}
	else
	{
		Result = SVMSG_63_SVIM_IN_WRONG_MODE;
	}

	return Result;
}

HRESULT SVVisionProcessorHelper::GetConfigurationPrintReport(std::string& rReport) const
{
	HRESULT l_Status = S_OK;

	SVConfigXMLPrint printSEC;
	try
	{
		rReport = printSEC.Print();
	}
	catch (std::exception & ex)
	{
		l_Status = atoi(ex.what());
	}
	catch (...)
	{
		l_Status = E_UNEXPECTED;
	}

	return l_Status;
}

HRESULT SVVisionProcessorHelper::GetDataDefinitionList(const std::string& rInspectionName, const SVDataDefinitionListType& rListType, SVDataDefinitionStructVector& rDataDefinitionArray) const
{
	HRESULT l_Status = S_OK;
	long l_ValueFilter = -1;
	long l_ImageFilter = -1;

	if (0 != (rListType & SelectedValues))
	{
		l_ValueFilter = SvPb::dataDefinitionValue;
	}

	if (0 != (rListType & AllValues))
	{
		l_ValueFilter = SvPb::dataDefinitionValue | SvPb::viewable;
	}

	if (0 != (rListType & SelectedImages))
	{
		l_ImageFilter = SvPb::dataDefinitionImage;
	}

	if (0 != (rListType & AllImages))
	{
		l_ImageFilter = SvPb::dataDefinitionImage | SvPb::viewable;
	}

	SVInspectionProcess* pInspection = nullptr;

	if (SVConfigurationObject::GetInspection(rInspectionName.c_str(), pInspection))
	{
		// Get Data Definition list from inspection
		SVToolSet* pToolSet = pInspection->GetToolSet();
		SvIe::SVTaskObjectListClass* pTaskObjectList = dynamic_cast <SvIe::SVTaskObjectListClass*> (pToolSet);

		if (l_ValueFilter != -1)
		{
			//Add value definition list
			std::vector<SvOi::IObjectClass*> outputList;
			pTaskObjectList->getOutputList(std::back_inserter(outputList));

			for (auto* pObject : outputList)
			{
				if (pObject)
				{
					SVDataDefinitionStruct l_DataDefinition;
					if (S_OK == GetObjectDefinition(*pObject, l_ValueFilter, l_DataDefinition))
					{
						rDataDefinitionArray.push_back(l_DataDefinition);
					}
				}
			}
		}

		if (l_ImageFilter != -1)
		{
			//Add image definition list
			SvIe::SVImageClassPtrVector ImageList;
			pToolSet->GetImageList(ImageList);

			int nCount = static_cast<int> (ImageList.size());
			for (int i = 0; i < nCount; i++)
			{
				SvIe::SVImageClass* pImage = ImageList[i];

				if (nullptr != pImage)
				{
					SVDataDefinitionStruct l_DataDefinition;
					if (S_OK == GetObjectDefinition(*pImage, l_ImageFilter, l_DataDefinition))
					{
						rDataDefinitionArray.push_back(l_DataDefinition);
					}
				}
			}
		}
	}

	return l_Status;
}

HRESULT SVVisionProcessorHelper::GetItems(const SvDef::StringSet& rNames, SVNameStorageResultMap& rItems) const
{
	HRESULT l_Status = S_OK;

	typedef std::map<std::string, SvDef::StringSet> SVNameSetMap;
	SVNameSetMap l_NameSets;

	for (const auto& rEntry : rNames)
	{
		SVObjectNameInfo l_Info;

		HRESULT l_LoopStatus = SVObjectNameInfo::ParseObjectName(l_Info, rEntry);

		if (S_OK == l_LoopStatus)
		{
			if (0 < l_Info.m_NameArray.size())
			{
				SVGetItemsFunctorMap::const_iterator l_FunctorIter = m_GetItemsFunctors.find(l_Info.m_NameArray[0]);

				//If name is found in list then use it otherwise use the standard method
				if (l_FunctorIter != m_GetItemsFunctors.end())
				{
					l_NameSets[l_FunctorIter->first].insert(rEntry);
				}
				else
				{
					l_NameSets[StandardItems].insert(rEntry);
				}
			}
			else
			{
				rItems[rEntry.c_str()] = SVStorageResult(SVStorage(), SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST, 0);

				if (S_OK == l_Status)
				{
					l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
				}
			}
		}
		else
		{
			rItems[rEntry.c_str()] = SVStorageResult(SVStorage(), SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST, 0);

			if (S_OK == l_Status)
			{
				l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
			}
		}
	}

	for (SVNameSetMap::iterator l_NameIterator = l_NameSets.begin(); l_NameIterator != l_NameSets.end(); ++l_NameIterator)
	{
		SVGetItemsFunctorMap::const_iterator l_FunctorIter = m_GetItemsFunctors.find(l_NameIterator->first);

		if (l_FunctorIter != m_GetItemsFunctors.end())
		{
			SVNameStorageResultMap Items;
			HRESULT l_LoopStatus = l_FunctorIter->second(l_NameIterator->second, Items);

			SVNameStorageResultMap::const_iterator ItemIter = Items.begin();
			for (; Items.end() != ItemIter; ++ItemIter)
			{
				rItems.insert(*ItemIter);
			}
			if (S_OK == l_Status)
			{
				l_Status = l_LoopStatus;
			}
		}
		else
		{
			rItems[l_NameIterator->first.c_str()] = SVStorageResult(SVStorage(), SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST, 0);

			if (S_OK == l_Status)
			{
				l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
			}
		}
	}

	return l_Status;
}

// Check for Remote Input as it must only be set via "RemoteInputs.Remote N" and Not Inspections.InspectionName.Remote Input N ...
// Even though Inspections.InspectionName.Remote Input N will find the correct value object to set, 
// the RemoteInput Object is what really needs to be set because all Inspections can access the Remote Input.
// Check if value can be converted to VT_R8
//Check no commas allowed in value
static HRESULT CheckForRemoteInputProblems(const SVObjectNameInfo& rInfo, const _variant_t &value)
{
	HRESULT hres {S_OK};
	bool bRemoteInput = false;

	if (0 < rInfo.m_NameArray.size())
	{
		size_t pos = rInfo.m_NameArray[rInfo.m_NameArray.size() - 1].find("Remote Input");
		if (0 == pos)
		{
			bRemoteInput = true;
			if(std::string(SvDef::FqnRemoteInputs) != rInfo.m_NameArray[0])
			{
				hres = SVMSG_OBJECT_WRONG_TYPE;
			}
		}
	}
	
	if (bRemoteInput && hres == S_OK )
	{
		_variant_t temp;
		if (value.vt & VT_ARRAY)
		{
			SvUl::SVSAFEARRAY SafeArray(value);

			if (1 == SafeArray.size())
			{
				hres = SafeArray.GetElement(0, temp);
			}
			else
			{
				hres = SVMSG_ONE_OR_MORE_OBJECTS_INVALID;
			}
		}
		else
		{
			temp = value;
		}
		if (hres == S_OK && temp.vt == VT_BSTR)
		{
			_bstr_t bstrtemp(temp);
			std::string stemp(bstrtemp);
			if (std::string::npos != stemp.find(','))
			{
				hres = SVMSG_ONE_OR_MORE_OBJECTS_INVALID;
			}
		}

		if (hres == S_OK)
		{
			if (S_OK != ::VariantChangeTypeEx(&temp, &temp, SvDef::LCID_USA, 0, VT_R8))
			{
				hres = SVMSG_ONE_OR_MORE_OBJECTS_INVALID;
			}
		}
			
	}
	return hres;

}



HRESULT SVVisionProcessorHelper::SetItems(const SVNameStorageMap& rItems, SVNameStatusMap& rStatusItems, bool RunOnce)
{
	DWORD notAllowedStates = SV_STATE_START_PENDING | SV_STATE_STARTING | SV_STATE_STOP_PENDING | SV_STATE_STOPING |
		SV_STATE_CREATING | SV_STATE_LOADING | SV_STATE_SAVING | SV_STATE_CLOSING;

	if (SVSVIMStateClass::CheckState(notAllowedStates) || SVSVIMStateClass::isSvrcBlocked())
	{
		return SVMSG_SVO_ACCESS_DENIED;
	}

	HRESULT l_Status = S_OK;

	typedef std::map<std::string, SVNameStorageMap> SVStringNameStorageMap;
	SVStringNameStorageMap l_NameStorageItems;

	for (SVNameStorageMap::const_iterator l_Iter = rItems.begin(); l_Iter != rItems.end(); ++l_Iter)
	{
		SVObjectNameInfo l_Info;

		HRESULT l_LoopStatus = SVObjectNameInfo::ParseObjectName(l_Info, l_Iter->first);

		const std::string* pInputName = &(l_Iter->first);

		if (S_OK == l_LoopStatus)
		{
			if (0 < l_Info.m_NameArray.size())
			{
				
				HRESULT CheckResult = CheckForRemoteInputProblems(l_Info, l_Iter->second.m_Variant);

				if (S_OK != CheckResult)
				{
					rStatusItems[l_Iter->first.c_str()] = CheckResult;
				}
				else
				{
					SVSetItemsFunctorMap::const_iterator l_FunctorIter = m_SetItemsFunctors.find(l_Info.m_NameArray[0]);

					if (l_FunctorIter != m_SetItemsFunctors.end())
					{
						l_NameStorageItems[l_FunctorIter->first].insert(std::pair<std::string, SVStorage>(*pInputName, l_Iter->second));
					}
					else
					{
						l_NameStorageItems[StandardItems].insert(std::pair<std::string, SVStorage>(*pInputName, l_Iter->second));;
					}
				}
			}
			else
			{
				rStatusItems[l_Iter->first.c_str()] = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;

				if (S_OK == l_Status)
				{
					l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
				}
			}
		}
		else
		{
			rStatusItems[l_Iter->first.c_str()] = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;

			if (S_OK == l_Status)
			{
				l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
			}
		}
	}

	for (SVStringNameStorageMap::iterator l_NameIterator = l_NameStorageItems.begin(); l_NameIterator != l_NameStorageItems.end(); ++l_NameIterator)
	{
		SVSetItemsFunctorMap::const_iterator l_FunctorIter = m_SetItemsFunctors.find(l_NameIterator->first);

		if (l_FunctorIter != m_SetItemsFunctors.end())
		{
			SVNameStatusMap StatusItems;
			HRESULT l_LoopStatus = l_FunctorIter->second(l_NameIterator->second, StatusItems, RunOnce);

			SVNameStatusMap::const_iterator StatusItemIter = StatusItems.begin();
			for (; StatusItems.end() != StatusItemIter; ++StatusItemIter)
			{
				rStatusItems.insert(*StatusItemIter);
			}
			if (S_OK == l_Status)
			{
				l_Status = l_LoopStatus;
			}
		}
		else
		{
			rStatusItems[l_NameIterator->first.c_str()] = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;

			if (S_OK == l_Status)
			{
				l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
			}
		}
	}

	return l_Status;
}

HRESULT SVVisionProcessorHelper::GetStandardItems(const SvDef::StringSet& rNames, SVNameStorageResultMap& rItems) const
{
	HRESULT Status = S_OK;

	rItems.clear();

	for (SvDef::StringSet::const_iterator l_Iter = rNames.begin(); SUCCEEDED(Status) && l_Iter != rNames.end(); ++l_Iter)
	{
		HRESULT LoopStatus = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
		SVObjectReference ObjectRef;
		SVStorage ValueStorage;

		SVObjectManagerClass::Instance().GetObjectByDottedName(*l_Iter, ObjectRef);

		if (nullptr != ObjectRef.getObject())
		{
			SvVol::BasicValueObject* pBasicValueObject = dynamic_cast<SvVol::BasicValueObject*>(ObjectRef.getObject());

			if (nullptr != pBasicValueObject)
			{
				_variant_t Value;
				HRESULT getResult = pBasicValueObject->getValue(Value);
				bool notNode = !pBasicValueObject->isNode();

				if (notNode && S_OK == getResult)
				{
					ValueStorage.m_Variant = Value;
					ValueStorage.m_StorageType = SVVisionProcessor::SVStorageValue;
					LoopStatus = S_OK;
				}
				else // This is an error, was a request for a basic value object node.
				{
					LoopStatus = SVMSG_OBJECT_WRONG_TYPE;
				}
			}
						}
		rItems[l_Iter->c_str()] = SVStorageResult(ValueStorage, LoopStatus, 0);

		if (S_OK == Status  && S_OK != LoopStatus)
		{
			Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
		}
	}

	return Status;
}

HRESULT SVVisionProcessorHelper::GetInspectionItems(const SvDef::StringSet& rNames, SVNameStorageResultMap& rItems) const
{
	DWORD notAllowedStates = SV_STATE_START_PENDING | SV_STATE_STARTING | SV_STATE_STOP_PENDING | SV_STATE_STOPING |
		SV_STATE_CREATING | SV_STATE_LOADING | SV_STATE_SAVING | SV_STATE_CLOSING;

	if (SVSVIMStateClass::CheckState(notAllowedStates) || SVSVIMStateClass::isSvrcBlocked())
	{
		return SVMSG_SVO_ACCESS_DENIED;
	}

	HRESULT l_Status = S_OK;

	SVConfigurationObject* pConfig(nullptr);

	l_Status = SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig)
	{
		l_Status = pConfig->GetInspectionItems(rNames, rItems);
	}
	else if (S_OK == l_Status)
	{
		l_Status = SVMSG_SVO_95_NO_CONFIGURATION_OBJECT;
	}

	return l_Status;
}

HRESULT SVVisionProcessorHelper::GetRemoteInputItems(const SvDef::StringSet& rNames, SVNameStorageResultMap& rItems) const
{
	DWORD notAllowedStates = SV_STATE_START_PENDING | SV_STATE_STARTING | SV_STATE_STOP_PENDING | SV_STATE_STOPING |
		SV_STATE_CREATING | SV_STATE_LOADING | SV_STATE_SAVING | SV_STATE_CLOSING;

	if (SVSVIMStateClass::CheckState(notAllowedStates) || SVSVIMStateClass::isSvrcBlocked())
	{
		return SVMSG_SVO_ACCESS_DENIED;
	}

	HRESULT l_Status = S_OK;

	SVConfigurationObject* pConfig(nullptr);

	l_Status = SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig)
	{
		l_Status = pConfig->GetRemoteInputItems(rNames, rItems);
	}
	else if (S_OK == l_Status)
	{
		l_Status = SVMSG_SVO_95_NO_CONFIGURATION_OBJECT;
	}

	return l_Status;
}

HRESULT SVVisionProcessorHelper::SetStandardItems(const SVNameStorageMap& rItems, SVNameStatusMap& rStatus, bool)
{
	HRESULT Status = S_OK;

	rStatus.clear();

	if (!(rItems.empty()))
	{
		bool Online = SVSVIMStateClass::CheckState(SV_STATE_RUNNING);

		for (SVNameStorageMap::const_iterator Iter = rItems.begin(); Iter != rItems.end(); ++Iter)
		{
			HRESULT LoopStatus = S_OK;
			SvVol::BasicValueObject* pValueObject = nullptr;

			SVObjectManagerClass::Instance().GetObjectByDottedName(Iter->first, pValueObject);

			if (nullptr != pValueObject)
			{
				bool Attribute = ((pValueObject->ObjectAttributesAllowed() & SvPb::remotelySetable) == SvPb::remotelySetable);

				if (Attribute)
				{
					Attribute = !Online || ((pValueObject->ObjectAttributesAllowed() & SvPb::setableOnline) == SvPb::setableOnline);

					if (Attribute)
					{
						LoopStatus = pValueObject->setValue(Iter->second.m_Variant);
					}
					else
					{
						LoopStatus = SVMSG_OBJECT_CANNOT_BE_SET_WHILE_ONLINE;
					}
				}
				else
				{
					LoopStatus = SVMSG_OBJECT_CANNOT_BE_SET_REMOTELY;
				}
			}
			else
			{
				LoopStatus = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
			}

			rStatus[Iter->first] = LoopStatus;
			if (S_OK == Status && S_OK != LoopStatus)
			{
				Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
			}
		}
	}
	else
	{
		Status = E_INVALIDARG;
	}

	return Status;
}

HRESULT SVVisionProcessorHelper::SetInspectionItems(const SVNameStorageMap& rItems, SVNameStatusMap& rStatus, bool RunOnce)
{
	HRESULT l_Status = S_OK;

	SVConfigurationObject* pConfig(nullptr);

	l_Status = SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig)
	{
		l_Status = pConfig->SetInspectionItems(rItems, rStatus, RunOnce);
	}
	else if (S_OK == l_Status)
	{
		l_Status = SVMSG_SVO_95_NO_CONFIGURATION_OBJECT;
	}

	return l_Status;
}

HRESULT SVVisionProcessorHelper::SetRemoteInputItems(const SVNameStorageMap& rItems, SVNameStatusMap& rStatus, bool )
{
	HRESULT l_Status = S_OK;

	DWORD notAllowedStates = SV_STATE_START_PENDING | SV_STATE_STARTING | SV_STATE_STOP_PENDING | SV_STATE_STOPING |
		SV_STATE_CREATING | SV_STATE_LOADING | SV_STATE_SAVING | SV_STATE_CLOSING;

	if (SVSVIMStateClass::CheckState(notAllowedStates) || SVSVIMStateClass::isSvrcBlocked())
	{
		return SVMSG_SVO_ACCESS_DENIED;
	}

	SVConfigurationObject* pConfig(nullptr);

	l_Status = SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig)
	{
		l_Status = pConfig->SetRemoteInputItems(rItems, rStatus);
	}
	else if (S_OK == l_Status)
	{
		l_Status = SVMSG_SVO_95_NO_CONFIGURATION_OBJECT;
	}

	return l_Status;
}

HRESULT SVVisionProcessorHelper::SetCameraItems(const SVNameStorageMap& rItems, SVNameStatusMap& rStatus, bool )
{
	HRESULT l_Status = S_OK;

	SVConfigurationObject* pConfig(nullptr);

	l_Status = SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig)
	{
		l_Status = pConfig->SetCameraItems(rItems, rStatus);
	}
	else if (S_OK == l_Status)
	{
		l_Status = SVMSG_SVO_95_NO_CONFIGURATION_OBJECT;
	}

	return l_Status;
}

HRESULT SVVisionProcessorHelper::GetObjectDefinition(const SvOi::IObjectClass& rObject, const long Filter, SVDataDefinitionStruct& rDataDef) const
{
	DWORD notAllowedStates = SV_STATE_START_PENDING | SV_STATE_STARTING | SV_STATE_STOP_PENDING | SV_STATE_STOPING |
		SV_STATE_CREATING | SV_STATE_LOADING | SV_STATE_SAVING | SV_STATE_CLOSING;

	if (SVSVIMStateClass::CheckState(notAllowedStates))
	{
		return SVMSG_SVO_ACCESS_DENIED;
	}

	HRESULT l_Status = S_OK;

	//Check using the filter if object should be included
	bool includeValue = false;
	if ((SvPb::dataDefinitionValue == Filter) || (SvPb::dataDefinitionImage == Filter))
	{
		//This is called when selected values or images
		includeValue = (rObject.ObjectAttributesSet() & Filter) != 0;
	}
	else
	{
		//This is called when all values or all images
		includeValue = (rObject.ObjectAttributesAllowed() & Filter) != 0;
	}
	//Is valid if it has selectable or archivable image attribute
	constexpr UINT cAttribute {SvDef::selectableAttributes | SvPb::archivableImage};
	includeValue = includeValue && 0 != (rObject.ObjectAttributesAllowed() & cAttribute);
	if (includeValue)
	{
		std::string Temp;
		Temp = _T("Inspections.") + rObject.GetCompleteName();
		rDataDef.m_Name = Temp;
		rDataDef.m_Writable = (rObject.ObjectAttributesAllowed() & SvPb::remotelySetable) == SvPb::remotelySetable;
		rDataDef.m_Published = (rObject.ObjectAttributesSet() & SvPb::publishable) != 0;
		//If null we assume its an image
		const SvOi::IValueObject* pValueObject = dynamic_cast<const SvOi::IValueObject*> (&rObject);
		if (nullptr != pValueObject)
		{
			rDataDef.m_Type = pValueObject->getTypeName();
		}
		else
		{
			//For now we are setting the type to a generic "Image" could change in the future
			rDataDef.m_Type = _T("Image");
		}
		//This part fills the additional info section
		if (SvPb::SVEnumValueObjectType == rObject.GetObjectSubType())
		{
			// Get the strings from the enumeration value object class.
			const SvVol::SVEnumerateValueObjectClass* pEnumVO = dynamic_cast<const SvVol::SVEnumerateValueObjectClass*> (&rObject);
			if (nullptr != pEnumVO)
			{
				for (auto const& rEntry : pEnumVO->GetEnumVector())
				{
					rDataDef.m_AdditionalInfo.push_back(rEntry.first);
				}
			}
		}
		else if (SvPb::SVBoolValueObjectType == rObject.GetObjectSubType())
		{
			// Get the strings from the enumeration value object class.
			const SvVol::SVBoolValueObjectClass* l_pBoolVO = dynamic_cast<const SvVol::SVBoolValueObjectClass*> (&rObject);
			if (nullptr != l_pBoolVO)
			{
				SvDef::StringVector Types;
				SvDef::StringVector::iterator Iter;
				l_pBoolVO->GetValidTypes(Types);
				for (Iter = Types.begin(); Iter != Types.end(); ++Iter)
				{
					rDataDef.m_AdditionalInfo.push_back(*Iter);
				}
			}
		}
		else
			//This should be an image so check for its additional info
		{
			const SvIe::SVImageClass* pImage = dynamic_cast<const SvIe::SVImageClass*> (&rObject);
			if (nullptr != pImage)
			{
				SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*>(pImage->GetTool());
				if (nullptr != pTool)
				{
					SvVol::SVStringValueObjectClass* pSourceNames = pTool->GetInputImageNames();
					if (nullptr != pSourceNames)
					{
						long l_lSize = pSourceNames->getArraySize();
						for (long l_lIndex = 0; l_lIndex < l_lSize; l_lIndex++)
						{
							pSourceNames->GetValue(Temp, l_lIndex);
							// Prepend the "Inspections." prefix for use with SVRC.
							Temp = _T("Inspections.") + Temp;
							rDataDef.m_AdditionalInfo.push_back(Temp);
						}
					}
				}
			}
		}
	}
	else
	{
		l_Status = S_FALSE;
	}

	return l_Status;
}

static void BuildNameSetForMonitoredObjectList(const MonitoredObjectList& rList, SvDef::StringSet& rNames)
{
	for (MonitoredObjectList::const_iterator it = rList.begin(); it != rList.end(); ++it)
	{
		const std::string& name = RemoteMonitorListHelper::GetNameFromMonitoredObject(*it);
		if (!name.empty())
		{
			rNames.insert(name);
		}
	}
}

HRESULT SVVisionProcessorHelper::QueryProductList(const std::string& rListName, SvDef::StringSet& rNames) const
{
	SVConfigurationObject* pConfig(nullptr);

	HRESULT hr = SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
	if (nullptr != pConfig)
	{
		const RemoteMonitorListMap& rList = pConfig->GetRemoteMonitorList();
		RemoteMonitorListMap::const_iterator it = rList.find(rListName);
		if (it != rList.end())
		{
			const RemoteMonitorNamedList& rNamedList = it->second;
			BuildNameSetForMonitoredObjectList(rNamedList.GetProductValuesList(), rNames);
			BuildNameSetForMonitoredObjectList(rNamedList.GetProductImagesList(), rNames);
		}
		else
		{
			hr = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_NT_BIT, ERROR_NOT_FOUND);
		}
	}
	else
	{
		hr = SVMSG_SVO_95_NO_CONFIGURATION_OBJECT;
	}
	return hr;
}

HRESULT SVVisionProcessorHelper::QueryRejectCondList(const std::string& rListName, SvDef::StringSet& rNames) const
{
	SVConfigurationObject* pConfig = nullptr;

	HRESULT hr = SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
	if (nullptr != pConfig)
	{
		const RemoteMonitorListMap& rList = pConfig->GetRemoteMonitorList();
		RemoteMonitorListMap::const_iterator it = rList.find(rListName);
		if (it != rList.end())
		{
			const RemoteMonitorNamedList& rNamedList = it->second;
			BuildNameSetForMonitoredObjectList(rNamedList.GetRejectConditionList(), rNames);
		}
		else
		{
			hr = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_NT_BIT, ERROR_NOT_FOUND);
		}
	}
	else
	{
		hr = SVMSG_SVO_95_NO_CONFIGURATION_OBJECT;
	}
	return hr;
}

HRESULT SVVisionProcessorHelper::QueryFailStatusList(const std::string& rListName, SvDef::StringSet& rNames) const
{
	SVConfigurationObject* pConfig = nullptr;

	HRESULT hr = SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
	if (nullptr != pConfig)
	{
		const RemoteMonitorListMap& rList = pConfig->GetRemoteMonitorList();
		RemoteMonitorListMap::const_iterator it = rList.find(rListName);
		if (it != rList.end())
		{
			const RemoteMonitorNamedList& rNamedList = it->second;
			BuildNameSetForMonitoredObjectList(rNamedList.GetFailStatusList(), rNames);
		}
		else
		{
			hr = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_NT_BIT, ERROR_NOT_FOUND);
		}
	}
	else
	{
		hr = SVMSG_SVO_95_NO_CONFIGURATION_OBJECT;
	}
	return hr;
}


HRESULT SVVisionProcessorHelper::GetMonitorListProperties(const std::string& rListName, MonitorlistProperties& properties)
{
	SVConfigurationObject* pConfig = nullptr;

	HRESULT hr = SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
	if (nullptr != pConfig)
	{
		const RemoteMonitorListMap& rList = pConfig->GetRemoteMonitorList();
		RemoteMonitorListMap::const_iterator it = rList.find(rListName);
		if (it != rList.end())
		{
			const RemoteMonitorNamedList& rNamedList = it->second;
			properties.isActive = rNamedList.IsActive();
			properties.RejectQueDepth = rNamedList.GetRejectDepthQueue();
			properties.ppqName = rNamedList.GetPPQName();
		}
		else
		{
			hr = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_NT_BIT, ERROR_NOT_FOUND);
		}
	}
	else
	{
		hr = SVMSG_SVO_95_NO_CONFIGURATION_OBJECT;
	}
	return hr;



}


HRESULT SVVisionProcessorHelper::ActivateMonitorList(const std::string& rListName, bool bActivate)
{
	HRESULT hr = S_OK;
	DWORD notAllowedStates = SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION |
		SV_STATE_START_PENDING | SV_STATE_STARTING | SV_STATE_STOP_PENDING | SV_STATE_STOPING |
		SV_STATE_CREATING | SV_STATE_LOADING | SV_STATE_SAVING | SV_STATE_CLOSING;
	if (!SVSVIMStateClass::CheckState(notAllowedStates) && SVSVIMStateClass::CheckState(SV_STATE_READY))
	{
		SVConfigurationObject* pConfig = nullptr;

		hr = SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
		if (S_OK == hr && nullptr != pConfig)
		{
			hr = pConfig->ActivateRemoteMonitorList(rListName, bActivate);
		}
		else
		{
			hr = SVMSG_SVO_95_NO_CONFIGURATION_OBJECT;
		}
	}
	else
	{
		hr = SVMSG_SVO_ACCESS_DENIED;
	}
	return hr;
}


HRESULT SVVisionProcessorHelper::GetInspectionNames(SvDef::StringSet& rNames) const
{
	SVConfigurationObject* pConfig = nullptr;

	HRESULT hr = SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
	if (nullptr != pConfig)
	{
		const SVInspectionProcessVector& rInspections = pConfig->GetInspections();

		for (auto pInspection : rInspections)
		{
			rNames.insert(pInspection->GetName());
		}
	}
	else
	{
		hr = SVMSG_SVO_95_NO_CONFIGURATION_OBJECT;
	}
	return hr;
}

HRESULT SVVisionProcessorHelper::QueryMonitorListNames(SvDef::StringSet& rNames) const
{
	SVConfigurationObject* pConfig = nullptr;

	HRESULT hr = SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
	if (nullptr != pConfig)
	{
		const RemoteMonitorListMap& rList = pConfig->GetRemoteMonitorList();
		for (RemoteMonitorListMap::const_iterator it = rList.begin(); it != rList.end(); ++it)
		{
			// insert monitor list to result name set
			rNames.insert(it->first);
		}
	}
	else
	{
		hr = SVMSG_SVO_95_NO_CONFIGURATION_OBJECT;
	}
	return hr;
}

HRESULT SVVisionProcessorHelper::SetProductFilter(const std::string& rListName, SvSml::SVProductFilterEnum filter)
{


	DWORD notAllowedStates = SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION |
		SV_STATE_START_PENDING | SV_STATE_STARTING | SV_STATE_STOP_PENDING | SV_STATE_STOPING |
		SV_STATE_CREATING | SV_STATE_LOADING | SV_STATE_SAVING | SV_STATE_CLOSING;
	if (SVSVIMStateClass::CheckState(notAllowedStates))
	{
		return  SVMSG_SVO_ACCESS_DENIED;
	}

	SVConfigurationObject* pConfig = nullptr;
	HRESULT hr = SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
	if (S_OK == hr && nullptr != pConfig)
	{
		hr = pConfig->SetRemoteMonitorListProductFilter(rListName, filter);
	}
	else
	{
		hr = SVMSG_SVO_95_NO_CONFIGURATION_OBJECT;
	}
	return  hr;
}

HRESULT SVVisionProcessorHelper::GetProductFilter(const std::string& rListName, SvSml::SVProductFilterEnum& rFilter) const
{
	SVConfigurationObject* pConfig = nullptr;

	HRESULT hr = SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
	if (S_OK == hr && nullptr != pConfig)
	{
		hr = pConfig->GetRemoteMonitorListProductFilter(rListName, rFilter);
	}
	else
	{
		hr = SVMSG_SVO_95_NO_CONFIGURATION_OBJECT;
	}
	return  hr;
}

HRESULT SVVisionProcessorHelper::RegisterMonitorList(const std::string& rListName, const std::string& rPPQName, int rejectDepth, const SvDef::StringSet& rProdList, const SvDef::StringSet& rRejectCondList, const SvDef::StringSet& rFailStatusList, SVNameStatusMap& rStatusOfItemsWithError)
{
	HRESULT hr = S_OK;
	DWORD notAllowedStates = SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION |
		SV_STATE_START_PENDING | SV_STATE_STARTING | SV_STATE_STOP_PENDING | SV_STATE_STOPING |
		SV_STATE_CREATING | SV_STATE_LOADING | SV_STATE_SAVING | SV_STATE_CLOSING;
	if (!SVSVIMStateClass::CheckState(notAllowedStates) && SVSVIMStateClass::CheckState(SV_STATE_READY))
	{
		SVConfigurationObject* pConfig = nullptr;
		hr = SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
		if (nullptr != pConfig)
		{
			SVPPQObject* pPPQ = nullptr;
			if (pConfig->GetPPQByName(rPPQName.c_str(), &pPPQ) && nullptr != pPPQ)
			{
				MonitoredObjectList prodCondValuesList;
				MonitoredObjectList prodCondImageList;
				MonitoredObjectList rejectCondObjectList;
				MonitoredObjectList failStatusObjectList;
				SetValuesOrImagesMonitoredObjectLists(rProdList, *pPPQ, prodCondValuesList, &prodCondImageList, rStatusOfItemsWithError, hr);
				SetValuesOrImagesMonitoredObjectLists(rRejectCondList, *pPPQ, rejectCondObjectList, nullptr, rStatusOfItemsWithError, hr);
				SetValuesOrImagesMonitoredObjectLists(rFailStatusList, *pPPQ, failStatusObjectList, nullptr, rStatusOfItemsWithError, hr);

				if (S_OK == hr)
				{
					RemoteMonitorNamedList newMonitorlist(rPPQName, rListName, prodCondValuesList, prodCondImageList, rejectCondObjectList, failStatusObjectList, rejectDepth);
					pConfig->ReplaceOrAddMonitorList(newMonitorlist);
				}
			}
			else
			{
				hr = SVMSG_NO_PPQ_FOUND;
			}
		}
		else
		{
			hr = SVMSG_SVO_95_NO_CONFIGURATION_OBJECT;
		}
	}
	else
	{
		hr = SVMSG_SVO_ACCESS_DENIED;
	}
	return hr;
}


void SVVisionProcessorHelper::Startup()
{
	SvStl::MessageManager::setNotificationFunction(::FireMessageNotification);
	SVSVIMStateClass::setNotificationFunction(::FireNotification);
}

void SVVisionProcessorHelper::Shutdown()
{
	//SvStl::MessageManager::setNotificationFunction(nullptr);
	SVSVIMStateClass::setNotificationFunction(nullptr);
}

void SVVisionProcessorHelper::FireNotification(SvPb::NotifyType notifyType, const _variant_t& parameter) const
{
	if (SvPb::NotifyType::currentMode == notifyType)
	{
		svModeEnum currentMode = static_cast<svModeEnum> (parameter.lVal);
		RootObject::setRootChildValue(SvDef::FqnEnvironmentModeValue, static_cast<long> (currentMode));
		RootObject::setRootChildValue(SvDef::FqnEnvironmentModeIsRun, (SvPb::DeviceModeType::runMode == currentMode));
		RootObject::setRootChildValue(SvDef::FqnEnvironmentModeIsStop, (SvPb::DeviceModeType::stopMode == currentMode));
		RootObject::setRootChildValue(SvDef::FqnEnvironmentModeIsRegressionTest, (SvPb::DeviceModeType::regressionMode == currentMode));
		RootObject::setRootChildValue(SvDef::FqnEnvironmentModeIsTest, (SvPb::DeviceModeType::testMode == currentMode));
		RootObject::setRootChildValue(SvDef::FqnEnvironmentModeIsEdit, (SvPb::DeviceModeType::editMode == currentMode));
		::PostMessage(AfxGetMainWnd()->m_hWnd, SV_REFRESH_STATUS_BAR, 0, 0);
	}

	SvPb::GetNotificationStreamResponse response;
	switch (notifyType) 
	{
	case SvPb::NotifyType::currentMode:
		response.set_currentmode(static_cast<SvPb::DeviceModeType>(parameter.lVal));
		break;
	case SvPb::NotifyType::lastModified:
		response.set_lastmodified(static_cast<SvPb::DeviceModeType>(parameter.llVal));
		break;
	case SvPb::NotifyType::configLoaded:
		response.set_configfileloaded(SvUl::createStdString(parameter));
		break;
	default:
		break;
	}
	response.set_type(notifyType);
	SvPb::ConvertVariantToProtobuf(parameter, response.mutable_parameter());
	ProcessNotification(response);
}

void SVVisionProcessorHelper::FireMessageNotification(const SvStl::MessageContainer& rMessage, int type) const
{
	SvPb::MessageType messageType = static_cast<SvPb::MessageType> (type);
	SvPb::MessageContainerVector messagelist;
	if (SvPb::MessageType::endMsgBox == messageType)
	{
		SvStl::MessageContainer message{ rMessage };
		message.clearWhat();
		SvPb::convertMessageToProtobuf(message, messagelist.add_messages());
	}
	else
	{
		SvPb::convertMessageToProtobuf(rMessage, messagelist.add_messages());
	}
	std::lock_guard<std::mutex> lockGuard(m_SubscriptionsMutex);
	for (auto it = m_MessageSubscriptions.begin(); it != m_MessageSubscriptions.end(); )
	{
		// client either unsubscripted or disconnected. remove from list.
		if (it->m_context->isCancelled())
		{
			it = m_MessageSubscriptions.erase(it);
			continue;
		}
		SvPb::GetMessageStreamResponse response;
		*response.mutable_messagelist() = messagelist;
		response.set_type(messageType);
		bool isSeverityInList = it->m_severityList.end() != std::find(it->m_severityList.begin(), it->m_severityList.end(), static_cast<int> (rMessage.getSeverity()));
		bool allSverities = it->m_severityList.empty();
		if (allSverities || isSeverityInList)
		{
			it->m_observer.onNext(std::move(response));
		}
		++it;
	}
}

void SVVisionProcessorHelper::ProcessNotification(const SvPb::GetNotificationStreamResponse& response) const
{
	std::lock_guard<std::mutex> lockGuard(m_SubscriptionsMutex);
	for(auto it = m_Subscriptions.begin(); it != m_Subscriptions.end(); )
	{
		// client either unsubscripted or disconnected. remove from list.
		if (it->m_context->isCancelled())
		{
			it = m_Subscriptions.erase(it);
			continue;
		}
		bool isNotifyTypeInList = it->m_notifyList.end() != std::find(it->m_notifyList.begin(), it->m_notifyList.end(), static_cast<int> (response.type()));
		bool allNotifications = it->m_notifyList.empty();
		if (allNotifications || isNotifyTypeInList)
		{
			SvPb::GetNotificationStreamResponse copyResponse;
			copyResponse.CopyFrom(response);
			it->m_observer.onNext(std::move(copyResponse));
		}
		++it;
	}
}

void SVVisionProcessorHelper::SetValuesOrImagesMonitoredObjectLists(const SvDef::StringSet& rObjectNameList, const SVPPQObject& pPPQ, MonitoredObjectList &rMonitoredValueObjectList, MonitoredObjectList *pMonitoredImageObjectList, SVNameStatusMap &rStatus, HRESULT &hr)
{
	for (SvDef::StringSet::const_iterator iter = rObjectNameList.begin(); iter != rObjectNameList.end(); ++iter)
	{
		const MonitoredObject& rObj = RemoteMonitorListHelper::GetMonitoredObjectFromName(*iter);
		SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(rObj.m_objectId);
		if (nullptr != pObject)
		{
			if (RemoteMonitorListController::IsValidMonitoredObject(pObject))
			{
				if (pPPQ.IsObjectInPPQ(*pObject))
				{
					if (nullptr != dynamic_cast<SvOi::IValueObject*> (pObject))
					{
						rMonitoredValueObjectList.push_back(rObj);
					}
					else if (nullptr != pMonitoredImageObjectList && SV_IS_KIND_OF(pObject, SvIe::SVImageClass))
					{
						pMonitoredImageObjectList->push_back(rObj);
					}
					else
					{
						hr = SVMSG_ONE_OR_MORE_OBJECTS_INVALID;
						rStatus[*iter] = SVMSG_OBJECT_WRONG_TYPE;
					}
				}
				else
				{
					hr = SVMSG_ONE_OR_MORE_OBJECTS_INVALID;
					rStatus[*iter] = SVMSG_OBJECT_IN_WRONG_PPQ;
				}
			}
			else
			{
				hr = SVMSG_ONE_OR_MORE_OBJECTS_INVALID;
				rStatus[*iter] = SVMSG_OBJECT_WRONG_TYPE;
			}
		}
		else
		{
			hr = SVMSG_ONE_OR_MORE_OBJECTS_INVALID;
			rStatus[*iter] = SVMSG_OBJECT_NOT_FOUND;
		}
	}
}

void SVVisionProcessorHelper::RegisterNotificationStream(const SvPb::GetNotificationStreamRequest& rRequest,
	SvRpc::Observer<SvPb::GetNotificationStreamResponse> observer,
	SvRpc::ServerStreamContext::Ptr ctx)
{
	std::vector<int> notifyList{ rRequest.notifylist().begin(), rRequest.notifylist().end() };
	std::lock_guard<std::mutex> lockGuard(m_SubscriptionsMutex);
	m_Subscriptions.push_back({std::move(observer), ctx, std::move(notifyList)});
}

void SVVisionProcessorHelper::RegisterMessageStream(const SvPb::GetMessageStreamRequest& rRequest,
	SvRpc::Observer<SvPb::GetMessageStreamResponse> observer,
	SvRpc::ServerStreamContext::Ptr ctx)
{
	std::vector<int> severityList{ rRequest.severitylist().begin(), rRequest.severitylist().end() };
	std::lock_guard<std::mutex> lockGuard(m_SubscriptionsMutex);
	m_MessageSubscriptions.push_back({ std::move(observer), ctx, std::move(severityList) });
}
