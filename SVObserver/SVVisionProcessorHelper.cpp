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

#include "Definitions/SVIMCommand.h"
#include "SVSystemLibrary/SVVersionInfo.h"

#include "SVConfigurationObject.h"
#include "SVConfigXMLPrint.h"
#include "SVGlobal.h"
#include "SVObserver.h"
#include "SVRemoteControlConstants.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVValueObjectLibrary/BasicValueObject.h"
#include "RemoteMonitorListHelper.h"
#include "Definitions/GlobalConst.h"
#include "SVObjectLibrary\SVObjectLibrary.h"
#include "Definitions/SVUserMessage.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SVVisionProcessorHelper& SVVisionProcessorHelper::Instance()
{
	static SVVisionProcessorHelper l_Object;

	return l_Object;
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
}
#pragma endregion Constructor

HRESULT SVVisionProcessorHelper::GetVersion( unsigned long& rVersion ) const
{
	HRESULT l_Status = S_OK;

	rVersion = SvSyl::SVVersionInfo::GetLongVersion();

	return l_Status;
}

HRESULT SVVisionProcessorHelper::GetVersion( std::string& rVersion ) const
{
	HRESULT l_Status = S_OK;

	rVersion = SvSyl::SVVersionInfo::GetShortTitleVersion();

	return l_Status;
}

HRESULT SVVisionProcessorHelper::GetState( unsigned long& rState ) const
{
	HRESULT l_Status = S_OK;

	rState = 0;

	if ( SVSVIMStateClass::CheckState( SV_STATE_READY ) )
	{
		rState |= SvDef::SVIM_CONFIG_LOADED;
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_SAVING ) )
	{
		rState |= SvDef::SVIM_SAVING_CONFIG;
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_LOADING ) )
	{
		rState |= SvDef::SVIM_CONFIG_LOADING;
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) )
	{
		rState |= SvDef::SVIM_ONLINE;

		if (! SVSVIMStateClass::CheckState( SV_STATE_TEST ) ) // testing (but not regression testing) sets the running flag
		{
			rState |= SvDef::SVIM_RUNNING;
		}
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_REGRESSION ) )
	{
		rState |= SvDef::SVIM_REGRESSION_TEST;
	}
	else if ( SVSVIMStateClass::CheckState( SV_STATE_TEST ) )// can be testing without regression testing, but can't be regression testing without testing
	{
		rState |= SvDef::SVIM_RUNNING_TEST;
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_EDIT ) )
	{
		rState |= SvDef::SVIM_SETUPMODE;
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_CLOSING ) )
	{
		rState |= SvDef::SVIM_STOPPING;
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_START_PENDING ) )
	{
		rState |= SvDef::SVIM_ONLINE_PENDING;
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_RAID_FAILURE ) )
	{
		rState |= SvDef::SVIM_RAID_FAILURE;
	}

	return l_Status;
}

HRESULT SVVisionProcessorHelper::GetOfflineCount( unsigned long& rCount ) const
{
	HRESULT l_Status = S_OK;

	rCount = TheSVObserverApp.getOfflineCount();

	return l_Status;
}

HRESULT SVVisionProcessorHelper::LoadConfiguration( const std::string& rPackFileName )
{
	DWORD notAllowedStates = SV_STATE_START_PENDING | SV_STATE_STARTING | SV_STATE_STOP_PENDING | SV_STATE_STOPING |
		SV_STATE_CREATING | SV_STATE_LOADING | SV_STATE_SAVING | SV_STATE_CLOSING;

	if (SVSVIMStateClass::CheckState(notAllowedStates))
	{
		return SVMSG_SVO_ACCESS_DENIED;
	}

	// Check if we are in an allowed state first
	// Not allowed to perform if in Regression or Test
	if (SVSVIMStateClass::CheckState(SV_STATE_TEST | SV_STATE_REGRESSION))
	{
		return SVMSG_63_SVIM_IN_WRONG_MODE;
	}
	HRESULT l_Status = TheSVObserverApp.LoadPackedConfiguration( rPackFileName.c_str() );

	return l_Status;
}

HRESULT SVVisionProcessorHelper::SaveConfiguration( const std::string& rPackFileName ) const
{
	DWORD notAllowedStates = SV_STATE_START_PENDING | SV_STATE_STARTING | SV_STATE_STOP_PENDING | SV_STATE_STOPING |
		SV_STATE_CREATING | SV_STATE_LOADING | SV_STATE_SAVING | SV_STATE_CLOSING;

	if (SVSVIMStateClass::CheckState(notAllowedStates))
	{
		return SVMSG_SVO_ACCESS_DENIED;
	}

	SVConfigurationObject* pConfig = nullptr;
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr == pConfig|| !pConfig->IsConfigurationLoaded())
	{
		return SVMSG_CONFIGURATION_NOT_LOADED;
	}
	HRESULT l_Status = TheSVObserverApp.SavePackedConfiguration(rPackFileName.c_str());
	return l_Status;
}

HRESULT SVVisionProcessorHelper::GetConfigurationMode( unsigned long& rMode ) const
{
	HRESULT l_Status = S_OK;

	rMode = SVSVIMStateClass::GetMode();

	return l_Status;
}

HRESULT SVVisionProcessorHelper::SetConfigurationMode( unsigned long p_Mode )
{
	HRESULT l_Status = S_OK;

	// Check if we are in an allowed state first
	// Not allowed to transition to or from Regression or Test
	if (p_Mode & (SV_STATE_TEST | SV_STATE_REGRESSION))
	{
		return SVMSG_64_SVIM_MODE_NOT_REMOTELY_SETABLE;
	}
	if (SVSVIMStateClass::CheckState(SV_STATE_TEST | SV_STATE_REGRESSION))
	{
		return SVMSG_63_SVIM_IN_WRONG_MODE;
	}

	SVConfigurationObject* pConfig( nullptr );

	l_Status = SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr != pConfig )
	{
		//Note this needs to be done using SendMessage due to this being a worker thread
		l_Status = static_cast< HRESULT >( SendMessage( AfxGetApp()->m_pMainWnd->m_hWnd, SV_SET_MODE, 0, ( LPARAM )p_Mode ) );
	}
	else
	{
		l_Status = SVMSG_SVO_95_NO_CONFIGURATION_OBJECT;
	}

	return l_Status;
}

HRESULT SVVisionProcessorHelper::GetConfigurationPrintReport( std::string& rReport ) const
{
	HRESULT l_Status = S_OK;

	SVConfigXMLPrint printSEC;
	try
	{
		rReport = printSEC.Print();
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

HRESULT SVVisionProcessorHelper::GetDataDefinitionList( const std::string& rInspectionName, const SVDataDefinitionListType& rListType, SVDataDefinitionStructVector& rDataDefinitionArray) const
{
	HRESULT l_Status = S_OK;
	long l_ValueFilter = -1;
	long l_ImageFilter = -1;

	if(0 != ( rListType & SelectedValues ))
	{
		l_ValueFilter = SvDef::SV_DD_VALUE;
	}

	if(0 != ( rListType & AllValues))
	{
		l_ValueFilter = SvDef::SV_DD_VALUE | SvDef::SV_VIEWABLE;
	}

	if(0 != ( rListType & SelectedImages))
	{
		l_ImageFilter = SvDef::SV_DD_IMAGE;
	}

	if(0 != ( rListType & AllImages))
	{
		l_ImageFilter = SvDef::SV_DD_IMAGE | SvDef::SV_VIEWABLE;
	}

	SVInspectionProcess* pInspection = nullptr;

	if ( SVConfigurationObject::GetInspection( rInspectionName.c_str(), pInspection ) )
	{
		// Get Data Definition list from inspection
		SVToolSetClass* pToolSet = pInspection->GetToolSet();
		SVTaskObjectListClass* pTaskObjectList = dynamic_cast <SVTaskObjectListClass*> ( pToolSet );

		if( l_ValueFilter != -1 )
		{
			//Add value definition list
			SVOutputInfoListClass l_OutputList;

			pTaskObjectList->GetOutputList( l_OutputList );

			int nCount = l_OutputList.GetSize();

			for( int i = 0; i < nCount; i++ )
			{
				// Get OutObjectInfoStruct...
				SVOutObjectInfoStruct* pInfoItem = nullptr;

				pInfoItem = l_OutputList.GetAt( i );

				SVObjectReference l_ObjRef;
				if( nullptr != pInfoItem )
				{
					l_ObjRef = pInfoItem->GetObjectReference();
				}
				else
				{
					ASSERT(0);
					break;
				}

				SVObjectClass* l_pObject = l_ObjRef.getObject();

				if( l_pObject )
				{
					SVDataDefinitionStruct l_DataDefinition;
					if(S_OK == GetObjectDefinition(*l_pObject, l_ValueFilter, l_DataDefinition))
					{
						rDataDefinitionArray.push_back(l_DataDefinition);
					}
				}
			}
		}

		if( l_ImageFilter != -1 )
		{
			//Add image definition list
			SVImageClassPtrVector ImageList;
			pToolSet->GetImageList( ImageList );

			int nCount = static_cast<int> (ImageList.size());
			for( int i = 0; i < nCount; i++ )
			{
				SVImageClass* pImage = ImageList[i];

				if ( nullptr != pImage )
				{
					SVDataDefinitionStruct l_DataDefinition;
					if(S_OK == GetObjectDefinition(*pImage, l_ImageFilter, l_DataDefinition))
					{
						rDataDefinitionArray.push_back(l_DataDefinition);
					}
				}
			}
		}
	}

	return l_Status;
}

HRESULT SVVisionProcessorHelper::GetItems( const SvDef::StringSet& rNames, SVNameStorageResultMap& rItems) const
{
	HRESULT l_Status = S_OK;

	typedef std::map<std::string, SvDef::StringSet> SVNameSetMap;
	SVNameSetMap l_NameSets;

	for( const auto& rEntry : rNames)
	{
		SVObjectNameInfo l_Info;
		
		HRESULT l_LoopStatus = SVObjectNameInfo::ParseObjectName( l_Info, rEntry);
		
		if( S_OK == l_LoopStatus )
		{
			if( 0 < l_Info.m_NameArray.size() )
			{
				SVGetItemsFunctorMap::const_iterator l_FunctorIter = m_GetItemsFunctors.find( l_Info.m_NameArray[ 0 ] );

				//If name is found in list then use it otherwise use the standard method
				if( l_FunctorIter != m_GetItemsFunctors.end() )
				{
					l_NameSets[ l_FunctorIter->first ].insert(rEntry);
				}
				else
				{
					l_NameSets[ StandardItems ].insert(rEntry);
				}
			}
			else
			{
				rItems[rEntry.c_str()] = SVStorageResult( SVStorage(), SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST, 0 );

				if( S_OK == l_Status )
				{
					l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
				}
			}
		}
		else
		{
			rItems[rEntry.c_str()] = SVStorageResult( SVStorage(), SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST, 0 );

			if( S_OK == l_Status )
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
			SVNameStorageResultMap Items;
			HRESULT l_LoopStatus = l_FunctorIter->second( l_NameIterator->second, Items );

			SVNameStorageResultMap::const_iterator ItemIter =  Items.begin();
			for( ; Items.end() != ItemIter; ++ItemIter )
			{
				rItems.insert( *ItemIter );
			}
			if( S_OK == l_Status )
			{
				l_Status = l_LoopStatus;
			}
		}
		else
		{
			rItems[ l_NameIterator->first.c_str() ] = SVStorageResult( SVStorage(), SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST, 0 );

			if( S_OK == l_Status )
			{
				l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
			}
		}
	}

	return l_Status;
}

static bool IsRemoteInputRequest(const SVObjectNameInfo& rInfo, bool& bValidRemoteInputRequest)
{
	bool bRemoteInput = false;
	bValidRemoteInputRequest = false;

	if (0 < rInfo.m_NameArray.size())
	{
		size_t pos = rInfo.m_NameArray[rInfo.m_NameArray.size() - 1].find("Remote Input");
		if (0 == pos)
		{
			bRemoteInput = true;
			bValidRemoteInputRequest = (std::string(SvDef::FqnRemoteInputs) == rInfo.m_NameArray[0]);
		}
	}
	return bRemoteInput;
}

HRESULT SVVisionProcessorHelper::SetItems(const SVNameStorageMap& rItems, SVNameStatusMap& rStatusItems, bool RunOnce)
{
	DWORD notAllowedStates = SV_STATE_START_PENDING | SV_STATE_STARTING | SV_STATE_STOP_PENDING | SV_STATE_STOPING |
		SV_STATE_CREATING | SV_STATE_LOADING | SV_STATE_SAVING | SV_STATE_CLOSING;

	if (SVSVIMStateClass::CheckState(notAllowedStates))
	{
		return SVMSG_SVO_ACCESS_DENIED;
	}
		
	HRESULT l_Status = S_OK;

	typedef std::map<std::string, SVNameStorageMap> SVStringNameStorageMap;
	SVStringNameStorageMap l_NameStorageItems;

	for( SVNameStorageMap::const_iterator l_Iter = rItems.begin(); l_Iter != rItems.end(); ++l_Iter )
	{
		SVObjectNameInfo l_Info;

		HRESULT l_LoopStatus = SVObjectNameInfo::ParseObjectName( l_Info, l_Iter->first );

		const std::string* pInputName = &( l_Iter->first);

		if( S_OK == l_LoopStatus )
		{
			if( 0 < l_Info.m_NameArray.size() )
			{
				// Check for Remote Input as it must only be set via "RemoteInputs.Remote N" and Not Inspections.InspectionName.Remote Input N ...
				// Even though Inspections.InspectionName.Remote Input N will find the correct value object to set, 
				// the RemoteInput Object is what really needs to be set because all Inspections can access the Remote Input.
				bool bValidRemoteInputRequest = false;
				if (IsRemoteInputRequest(l_Info, bValidRemoteInputRequest) && !bValidRemoteInputRequest)
				{
					rStatusItems[ l_Iter->first.c_str() ] = SVMSG_OBJECT_WRONG_TYPE;//SVMSG_OBJECT_NOT_PROCESSED;
				}
				else
				{
					SVSetItemsFunctorMap::const_iterator l_FunctorIter = m_SetItemsFunctors.find( l_Info.m_NameArray[ 0 ] );

					if( l_FunctorIter != m_SetItemsFunctors.end() )
					{
						l_NameStorageItems[ l_FunctorIter->first ].insert( std::pair<std::string, SVStorage>(*pInputName,l_Iter->second)  );
					}
					else
					{
						l_NameStorageItems[ StandardItems ].insert( std::pair<std::string, SVStorage>(*pInputName,l_Iter->second)  );;
					}
				}
			}
			else
			{
				rStatusItems[ l_Iter->first.c_str() ] = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;

				if( S_OK == l_Status )
				{
					l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
				}
			}
		}
		else
		{
			rStatusItems[ l_Iter->first.c_str() ] = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;

			if( S_OK == l_Status )
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
			SVNameStatusMap StatusItems;
			HRESULT l_LoopStatus = l_FunctorIter->second(l_NameIterator->second, StatusItems, RunOnce);
	
			SVNameStatusMap::const_iterator StatusItemIter =  StatusItems.begin();
			for( ; StatusItems.end() != StatusItemIter; ++StatusItemIter )
			{
				rStatusItems.insert( *StatusItemIter );
				}
			if( S_OK == l_Status )
			{
				l_Status = l_LoopStatus;
			}
		}
		else
		{
			rStatusItems[ l_NameIterator->first.c_str() ] = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;

			if( S_OK == l_Status )
			{
				l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
			}
		}
	}

	return l_Status;
}

HRESULT SVVisionProcessorHelper::GetStandardItems( const SvDef::StringSet& rNames, SVNameStorageResultMap& rItems ) const
{
	HRESULT Status = S_OK;

	rItems.clear();

	for( SvDef::StringSet::const_iterator l_Iter = rNames.begin(); SUCCEEDED( Status ) && l_Iter != rNames.end(); ++l_Iter )
	{
		HRESULT LoopStatus = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
		SVObjectReference ObjectRef;
		SVStorage ValueStorage;

		SVObjectManagerClass::Instance().GetObjectByDottedName( *l_Iter, ObjectRef );

		if( nullptr != ObjectRef.getObject() )
		{
			BasicValueObject* pBasicValueObject = dynamic_cast< BasicValueObject* >( ObjectRef.getObject() );

			if( nullptr != pBasicValueObject )
			{
				_variant_t Value;
				HRESULT getResult = pBasicValueObject->getValue( Value );
				bool notNode = !pBasicValueObject->isNode();

				if( notNode && S_OK == getResult )
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
			else
			{
				SvOi::IValueObject* pValueObject = ObjectRef.getValueObject();

				if ( nullptr != pValueObject )
				{
					_variant_t Value;
					//Enumeration Value objects need to return the text and not the value
					if (SvDef::SVEnumValueObjectType == ObjectRef.getObject()->GetObjectSubType())
					{
						std::string StringValue;
						if (S_OK == pValueObject->getValue(StringValue))
						{
							Value.SetString(StringValue.c_str());
							ValueStorage.m_Variant = Value;
							ValueStorage.m_StorageType = SVVisionProcessor::SVStorageValue;
							LoopStatus = S_OK;
						}
					}
					else
					{
						if (S_OK == pValueObject->getValue(Value))
						{
							ValueStorage.m_Variant = Value;
							ValueStorage.m_StorageType = SVVisionProcessor::SVStorageValue;
							LoopStatus = S_OK;
						}
					}
				}
				else // This is an error, was a request for a non value object.
				{
					LoopStatus = SVMSG_OBJECT_WRONG_TYPE;
				}
			}
		}
		rItems[ l_Iter->c_str() ] = SVStorageResult(ValueStorage, LoopStatus, 0);

		if( S_OK == Status  && S_OK != LoopStatus )
		{
			Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
		}
	}

	return Status;
}

HRESULT SVVisionProcessorHelper::GetInspectionItems( const SvDef::StringSet& rNames, SVNameStorageResultMap& rItems ) const
{
	DWORD notAllowedStates = SV_STATE_START_PENDING | SV_STATE_STARTING | SV_STATE_STOP_PENDING | SV_STATE_STOPING |
		SV_STATE_CREATING | SV_STATE_LOADING | SV_STATE_SAVING | SV_STATE_CLOSING;

	if (SVSVIMStateClass::CheckState(notAllowedStates))
	{
		return SVMSG_SVO_ACCESS_DENIED;
	}

	HRESULT l_Status = S_OK;

	SVConfigurationObject* pConfig( nullptr );

	l_Status = SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr != pConfig )
	{
		l_Status = pConfig->GetInspectionItems( rNames, rItems );
		//@TODO [gra][7.50][18.08.2017] This is only for HMI compatibility which needs bool as 1 or 0
		//This should be removed when HMI can handle bool
		SVNameStorageResultMap::iterator Iter(rItems.begin());
		for (; rItems.end() != Iter; ++Iter)
		{
			if (VT_BOOL == Iter->second.m_Storage.m_Variant.vt)
			{
				bool Value = Iter->second.m_Storage.m_Variant ? true : false;
				Iter->second.m_Storage.m_Variant.Clear();
				Iter->second.m_Storage.m_Variant = static_cast<VARIANT_BOOL> (Value);
			}
		}
	}
	else if( S_OK == l_Status )
	{
		l_Status = SVMSG_SVO_95_NO_CONFIGURATION_OBJECT;
	}

	return l_Status;
}

HRESULT SVVisionProcessorHelper::GetRemoteInputItems( const SvDef::StringSet& rNames, SVNameStorageResultMap& rItems ) const
{
	DWORD notAllowedStates = SV_STATE_START_PENDING | SV_STATE_STARTING | SV_STATE_STOP_PENDING | SV_STATE_STOPING |
		SV_STATE_CREATING | SV_STATE_LOADING | SV_STATE_SAVING | SV_STATE_CLOSING;

	if (SVSVIMStateClass::CheckState(notAllowedStates))
	{
		return SVMSG_SVO_ACCESS_DENIED;
	}

	HRESULT l_Status = S_OK;

	SVConfigurationObject* pConfig( nullptr ); 

	l_Status = SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr != pConfig )
	{
		l_Status = pConfig->GetRemoteInputItems( rNames, rItems );
	}
	else if( S_OK == l_Status )
	{
		l_Status = SVMSG_SVO_95_NO_CONFIGURATION_OBJECT;
	}

	return l_Status;
}

HRESULT SVVisionProcessorHelper::SetStandardItems( const SVNameStorageMap& rItems, SVNameStatusMap& rStatus, bool RunOnce)
{
	HRESULT Status = S_OK;

	rStatus.clear();

	if( !( rItems.empty() ) )
	{
		bool Online = SVSVIMStateClass::CheckState( SV_STATE_RUNNING );

		for( SVNameStorageMap::const_iterator Iter = rItems.begin(); Iter != rItems.end(); ++Iter )
		{
			HRESULT LoopStatus = S_OK;
			BasicValueObject* pValueObject = nullptr;

			SVObjectManagerClass::Instance().GetObjectByDottedName( Iter->first, pValueObject );

			if( nullptr != pValueObject )
			{
				bool Attribute = ( ( pValueObject->ObjectAttributesAllowed() & SvDef::SV_REMOTELY_SETABLE ) == SvDef::SV_REMOTELY_SETABLE );

				if( Attribute )
				{
					Attribute = !Online || ( ( pValueObject->ObjectAttributesAllowed() & SvDef::SV_SETABLE_ONLINE ) == SvDef::SV_SETABLE_ONLINE );

					if( Attribute )
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

			rStatus[ Iter->first ] = LoopStatus;
			if( S_OK == Status && S_OK != LoopStatus )
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

	SVConfigurationObject* pConfig( nullptr );

	l_Status = SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr != pConfig )
	{
		l_Status = pConfig->SetInspectionItems(rItems, rStatus, RunOnce);
	}
	else if( S_OK == l_Status )
	{
		l_Status = SVMSG_SVO_95_NO_CONFIGURATION_OBJECT;
	}

	return l_Status;
}

HRESULT SVVisionProcessorHelper::SetRemoteInputItems(const SVNameStorageMap& rItems, SVNameStatusMap& rStatus, bool RunOnce)
{
	HRESULT l_Status = S_OK;

	SVConfigurationObject* pConfig( nullptr );

	l_Status = SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr != pConfig )
	{
		l_Status = pConfig->SetRemoteInputItems( rItems, rStatus );
	}
	else if( S_OK == l_Status )
	{
		l_Status = SVMSG_SVO_95_NO_CONFIGURATION_OBJECT;
	}

	return l_Status;
}

HRESULT SVVisionProcessorHelper::SetCameraItems(const SVNameStorageMap& rItems, SVNameStatusMap& rStatus, bool RunOnce)
{
	HRESULT l_Status = S_OK;

	SVConfigurationObject* pConfig( nullptr );

	l_Status = SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr != pConfig )
	{
		l_Status = pConfig->SetCameraItems( rItems, rStatus );
	}
	else if( S_OK == l_Status )
	{
		l_Status = SVMSG_SVO_95_NO_CONFIGURATION_OBJECT;
	}

	return l_Status;
}

HRESULT SVVisionProcessorHelper::GetObjectDefinition( const SVObjectClass& rObject, const long Filter, SVDataDefinitionStruct& rDataDef ) const
{
	DWORD notAllowedStates = SV_STATE_START_PENDING | SV_STATE_STARTING | SV_STATE_STOP_PENDING | SV_STATE_STOPING |
		SV_STATE_CREATING | SV_STATE_LOADING | SV_STATE_SAVING | SV_STATE_CLOSING;

	if (SVSVIMStateClass::CheckState(notAllowedStates))
	{
		return SVMSG_SVO_ACCESS_DENIED;
	}

	HRESULT l_Status = S_OK;

	//Check using the filter if object should be included
	bool l_bValueIncluded = false;
	if((SvDef::SV_DD_VALUE == Filter) || (SvDef::SV_DD_IMAGE == Filter))
	{
		//This is called when selected values or images
		l_bValueIncluded = (rObject.ObjectAttributesSet() & Filter) != 0;
	}
	else
	{
		//This is called when all values or all images
		l_bValueIncluded = (rObject.ObjectAttributesAllowed() & Filter) != 0;
	}
	l_bValueIncluded = l_bValueIncluded && ( (rObject.ObjectAttributesAllowed() & SvDef::SV_HIDDEN) == 0 );
	if( l_bValueIncluded )
	{
		std::string Temp;
		Temp = _T("Inspections.") + rObject.GetCompleteName();
		rDataDef.m_Name = Temp;
		rDataDef.m_Writable = (rObject.ObjectAttributesAllowed() & SvDef::SV_REMOTELY_SETABLE) == SvDef::SV_REMOTELY_SETABLE;
		rDataDef.m_Published = (rObject.ObjectAttributesSet() & SvDef::SV_PUBLISHABLE) != 0;
		//If null we assume its an image
		const SvOi::IValueObject* pValueObject = dynamic_cast<const SvOi::IValueObject*> (&rObject);
		if( nullptr != pValueObject )
		{
			rDataDef.m_Type = pValueObject->getTypeName();
		}
		else
		{
			//For now we are setting the type to a generic "Image" could change in the future
			rDataDef.m_Type = _T("Image");
		}
		//This part fills the additional info section
		if( SvDef::SVEnumValueObjectType == rObject.GetObjectSubType() )
		{
			// Get the strings from the enumeration value object class.
			const SVEnumerateValueObjectClass* pEnumVO = dynamic_cast<const SVEnumerateValueObjectClass*> (&rObject);
			if( nullptr != pEnumVO )
			{
				for (auto const& rEntry : pEnumVO->GetEnumVector())
				{
					rDataDef.m_AdditionalInfo.push_back( rEntry.first );
				}
			}
		}
		else if( SvDef::SVBoolValueObjectType == rObject.GetObjectSubType() )
		{
			// Get the strings from the enumeration value object class.
			const SVBoolValueObjectClass* l_pBoolVO = dynamic_cast<const SVBoolValueObjectClass*> (&rObject);
			if( nullptr != l_pBoolVO)
			{
				SvDef::StringVector Types;
				SvDef::StringVector::iterator Iter;
				l_pBoolVO->GetValidTypes(Types);
				for( Iter = Types.begin(); Iter != Types.end(); Iter++)
				{
					rDataDef.m_AdditionalInfo.push_back(*Iter);
				}
			}
		}
		else
		//This should be an image so check for its additional info
		{
			const SVImageClass* pImage = dynamic_cast<const SVImageClass*> (&rObject);
			if( nullptr != pImage )
			{
				SVToolClass* pTool = dynamic_cast<SVToolClass*>(pImage->GetTool());
				if( nullptr != pTool )
				{
					SVStringValueObjectClass* pSourceNames = pTool->GetInputImageNames();
					if( nullptr != pSourceNames )
					{
						long l_lSize = pSourceNames->getArraySize();
						for( long l_lIndex = 0; l_lIndex < l_lSize ; l_lIndex++ )
						{
							HRESULT l_hr = pSourceNames->GetValue( Temp, l_lIndex );
							// Prepend the "Inspections." prefix for use with SVRC.
							Temp = _T( "Inspections." ) + Temp;
							rDataDef.m_AdditionalInfo.push_back( Temp );
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

static void BuildNameSetForMonitoredObjectList( const MonitoredObjectList& rList, SvDef::StringSet& rNames )
{
	for (MonitoredObjectList::const_iterator it = rList.begin();it != rList.end();++it)
	{
		const std::string& name = RemoteMonitorListHelper::GetNameFromMonitoredObject(*it);
		if (!name.empty())
		{
			rNames.insert(name);
		}
	}
}

HRESULT SVVisionProcessorHelper::QueryProductList( const std::string& rListName, SvDef::StringSet& rNames ) const
{
	SVConfigurationObject* pConfig( nullptr );

	HRESULT hr = SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	if ( nullptr != pConfig )
	{
		const RemoteMonitorListMap& rList = pConfig->GetRemoteMonitorList();
		RemoteMonitorListMap::const_iterator it = rList.find( rListName );
		if ( it != rList.end() )
		{
			const RemoteMonitorNamedList& rNamedList = it->second;
			BuildNameSetForMonitoredObjectList( rNamedList.GetProductValuesList(), rNames );
			BuildNameSetForMonitoredObjectList( rNamedList.GetProductImagesList(), rNames );
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

HRESULT SVVisionProcessorHelper::QueryRejectCondList( const std::string& rListName, SvDef::StringSet& rNames ) const
{
	SVConfigurationObject* pConfig = nullptr;

	HRESULT hr = SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	if ( nullptr != pConfig )
	{
		const RemoteMonitorListMap& rList = pConfig->GetRemoteMonitorList();
		RemoteMonitorListMap::const_iterator it = rList.find( rListName );
		if ( it != rList.end() )
		{
			const RemoteMonitorNamedList& rNamedList = it->second;
			BuildNameSetForMonitoredObjectList( rNamedList.GetRejectConditionList(), rNames );
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

HRESULT SVVisionProcessorHelper::QueryFailStatusList( const std::string& rListName, SvDef::StringSet& rNames ) const
{
	SVConfigurationObject* pConfig = nullptr;

	HRESULT hr = SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	if ( nullptr != pConfig )
	{
		const RemoteMonitorListMap& rList = pConfig->GetRemoteMonitorList();
		RemoteMonitorListMap::const_iterator it = rList.find( rListName );
		if ( it != rList.end() )
		{
			const RemoteMonitorNamedList& rNamedList = it->second;
			BuildNameSetForMonitoredObjectList( rNamedList.GetFailStatusList(), rNames );
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

	HRESULT hr = SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	if ( nullptr != pConfig )
	{
		const RemoteMonitorListMap& rList = pConfig->GetRemoteMonitorList();
		RemoteMonitorListMap::const_iterator it = rList.find( rListName );
		if ( it != rList.end() )
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


HRESULT SVVisionProcessorHelper::ActivateMonitorList( const std::string& rListName, bool bActivate )
{
	HRESULT hr = S_OK;
	DWORD notAllowedStates = SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION | 
							SV_STATE_START_PENDING | SV_STATE_STARTING | SV_STATE_STOP_PENDING | SV_STATE_STOPING |
							SV_STATE_CREATING | SV_STATE_LOADING | SV_STATE_SAVING | SV_STATE_CLOSING;
	if ( !SVSVIMStateClass::CheckState( notAllowedStates ) && SVSVIMStateClass::CheckState( SV_STATE_READY ) )
	{
		SVConfigurationObject* pConfig = nullptr;

		hr = SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
		if ( nullptr != pConfig )
		{
			hr = pConfig->ActivateRemoteMonitorList( rListName, bActivate );
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


HRESULT SVVisionProcessorHelper::GetInspectionNames( SvDef::StringSet& rNames ) const
{
	SVConfigurationObject* pConfig = nullptr;

	HRESULT hr = SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	if ( nullptr != pConfig )
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

HRESULT SVVisionProcessorHelper::QueryMonitorListNames( SvDef::StringSet& rNames ) const
{
	SVConfigurationObject* pConfig = nullptr;

	HRESULT hr = SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	if ( nullptr != pConfig )
	{
		const RemoteMonitorListMap& rList = pConfig->GetRemoteMonitorList();
		for (RemoteMonitorListMap::const_iterator it = rList.begin();it != rList.end();++it)
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
	if ( SVSVIMStateClass::CheckState( notAllowedStates ) )
	{
		return  SVMSG_SVO_ACCESS_DENIED;
	}
	
	SVConfigurationObject* pConfig = nullptr;
	HRESULT hr = SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	if ( nullptr != pConfig )
	{
		hr = pConfig->SetRemoteMonitorListProductFilter( rListName, filter );
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

	HRESULT hr = SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	if ( nullptr != pConfig )
	{
		hr = pConfig->GetRemoteMonitorListProductFilter( rListName, rFilter );
	}
	else
	{
		hr = SVMSG_SVO_95_NO_CONFIGURATION_OBJECT;
	}
	return  hr;
}

HRESULT SVVisionProcessorHelper::RegisterMonitorList( const std::string& rListName, const std::string& rPPQName, int rejectDepth, const SvDef::StringSet& rProdList, const SvDef::StringSet& rRejectCondList, const SvDef::StringSet& rFailStatusList, SVNameStatusMap& rStatusOfItemsWithError )
{
	HRESULT hr = S_OK;
	DWORD notAllowedStates = SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION | 
		SV_STATE_START_PENDING | SV_STATE_STARTING | SV_STATE_STOP_PENDING | SV_STATE_STOPING |
		SV_STATE_CREATING | SV_STATE_LOADING | SV_STATE_SAVING | SV_STATE_CLOSING;
	if ( !SVSVIMStateClass::CheckState( notAllowedStates ) && SVSVIMStateClass::CheckState( SV_STATE_READY ) )
	{
		SVConfigurationObject* pConfig = nullptr;
		hr = SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
		if ( nullptr != pConfig )
		{
			SVPPQObject* pPPQ = nullptr;
			if (pConfig->GetPPQByName(rPPQName.c_str(), &pPPQ) && nullptr != pPPQ)
			{
				MonitoredObjectList prodCondValuesList;
				MonitoredObjectList prodCondImageList;
				MonitoredObjectList rejectCondObjectList;
				MonitoredObjectList failStatusObjectList;
				SetValuesOrImagesMonitoredObjectLists( rProdList, *pPPQ, prodCondValuesList, &prodCondImageList, rStatusOfItemsWithError, hr);
				SetValuesOrImagesMonitoredObjectLists( rRejectCondList, *pPPQ, rejectCondObjectList, nullptr, rStatusOfItemsWithError, hr);
				SetValuesOrImagesMonitoredObjectLists( rFailStatusList, *pPPQ, failStatusObjectList, nullptr, rStatusOfItemsWithError, hr);

				if (S_OK == hr)
				{
					RemoteMonitorNamedList newMonitorlist(rPPQName, rListName, prodCondValuesList, prodCondImageList, rejectCondObjectList, failStatusObjectList, rejectDepth );
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

HRESULT SVVisionProcessorHelper::FireNotification( int Type, int MessageNumber, LPCTSTR MessageText )
{
	SvStl::NotificationEnum NotificationType( SvStl::NotificationEnum::MsgUknown );
	NotificationType = static_cast<SvStl::NotificationEnum> (Type);
	if (SvStl::NotificationEnum::MsgUknown != NotificationType)
	{
		m_MessageNotification.SetNotification(NotificationType, MessageNumber, MessageText);
	}
	return S_OK;
}

void CALLBACK SVVisionProcessorHelper::APCThreadProcess( DWORD_PTR dwParam )
{
}

void SVVisionProcessorHelper::ThreadProcess( bool& rWaitForEvents )
{
	//@TODO[gra][8.10][18.04.2018]: These notifications need to be replaced using websockets
	ProcessLastModified( rWaitForEvents );
	NotifyModeChanged( rWaitForEvents );
	ProcessNotification( rWaitForEvents );
}

void SVVisionProcessorHelper::ProcessLastModified( bool& rWaitForEvents )
{
	if(SVSVIMStateClass::m_PrevModifiedTime != SVSVIMStateClass::m_LastModifiedTime )
	{
		//@TODO[gra][8.10][18.04.2018]: These notifications need to be replaced using websockets
		//std::string l_JsonNotification;
		//Json::FastWriter l_Writer;
		//Json::Value l_Object(Json::objectValue);
		//Json::Value l_ElementObject(Json::objectValue);

		//l_ElementObject[ SVRC::notification::timestamp ] = SVSVIMStateClass::m_LastModifiedTime;

		//l_Object[ SVRC::notification::notification ] = SVRC::notification::lastmodified;
		//l_Object[ SVRC::notification::dataitems ] = l_ElementObject;

		//l_JsonNotification = l_Writer.write( l_Object ).c_str();

		//SVSocketRemoteCommandManager::Instance().ProcessJsonNotification( l_JsonNotification );

		SVSVIMStateClass::m_PrevModifiedTime = SVSVIMStateClass::m_LastModifiedTime;
	}
}

void SVVisionProcessorHelper::ProcessNotification( bool& rWaitForEvents )
{

	Concurrency::critical_section::scoped_lock  scopedLock( m_MessageNotification.getLock() );

	if( !m_MessageNotification.isProcessed() )
	{
		//@TODO[gra][8.10][18.04.2018]: These notifications need to be replaced using websockets
		//std::string JsonNotification;
		//Json::FastWriter Writer;
		//Json::Value Object(Json::objectValue);
		//Json::Value ElementObject(Json::objectValue);

		//ElementObject[SVRC::notification::MessageType] = m_MessageNotification.getType();
		//ElementObject[SVRC::notification::MessageNumber] = m_MessageNotification.getMessageNumber();
		//ElementObject[SVRC::notification::MessageText] = m_MessageNotification.getMessageText();

		//Object[SVRC::notification::notification ] = SVRC::notification::MessageNotification;
		//Object[SVRC::notification::dataitems] = ElementObject;

		//JsonNotification = Writer.write( Object ).c_str();

		//SVSocketRemoteCommandManager::Instance().ProcessJsonNotification( JsonNotification );

		m_MessageNotification.setProcessed( true );
	}

}

void SVVisionProcessorHelper::NotifyModeChanged( bool& rWaitForEvents )
{
	if(SVSVIMStateClass::m_prevMode != SVSVIMStateClass::m_lastMode )
	{
		//@TODO[gra][8.10][18.04.2018]: These notifications need to be replaced using websockets
		//std::string l_JsonNotification;
		//Json::FastWriter l_Writer;
		//Json::Value l_Object(Json::objectValue);
		//Json::Value l_ElementObject(Json::objectValue);

		//l_ElementObject[SVRC::notification::mode] = SVSVIMStateClass::m_lastMode;

		//l_Object[SVRC::notification::notification ] = SVRC::notification::currentmode;
		//l_Object[SVRC::notification::dataitems] = l_ElementObject;

		//l_JsonNotification = l_Writer.write( l_Object ).c_str();

		//SVSocketRemoteCommandManager::Instance().ProcessJsonNotification( l_JsonNotification );

		SVSVIMStateClass::m_prevMode = SVSVIMStateClass::m_lastMode;
	}
}

void SVVisionProcessorHelper::SetValuesOrImagesMonitoredObjectLists( const SvDef::StringSet& rObjectNameList, const SVPPQObject& pPPQ, MonitoredObjectList &rMonitoredValueObjectList, MonitoredObjectList *pMonitoredImageObjectList, SVNameStatusMap &rStatus, HRESULT &hr )
{
	for( SvDef::StringSet::const_iterator iter = rObjectNameList.begin(); iter != rObjectNameList.end(); ++iter )
	{
		const MonitoredObject& rObj = RemoteMonitorListHelper::GetMonitoredObjectFromName(*iter);
		SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject( rObj.guid );
		if (nullptr != pObject)
		{
			if ( RemoteMonitorListController::IsValidMonitoredObject(pObject) )
			{
				if (pPPQ.IsObjectInPPQ(*pObject))
				{
					if( nullptr != dynamic_cast<SvOi::IValueObject*> (pObject) )
					{
						rMonitoredValueObjectList.push_back(rObj);
					}
					else if (nullptr != pMonitoredImageObjectList && SV_IS_KIND_OF(pObject, SVImageClass))
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
