//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVVisionProcessorHelper
//* .File Name       : $Workfile:   SVVisionProcessorHelper.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.22  $
//* .Check In Date   : $Date:   20 Oct 2014 11:21:10  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include <boost/config.hpp>
#include <boost/assign.hpp>
#include <boost/bind.hpp>
#include "SVVisionProcessorHelper.h"

#include "JsonLib/include/json.h"
#include "SVIMCommand/SVIMCommand.h"
#include "SVSystemLibrary/SVVersionInfo.h"

#include "SVConfigurationObject.h"
#include "SVConfigXMLPrint.h"
#include "SVGlobal.h"
#include "SVObserver.h"
#include "SVSocketRemoteCommandManager.h"
#include "SVSVIMStateClass.h"
#include "BasicValueObject.h"
#include "RemoteMonitorListHelper.h"
#include "SVObjectLibrary/GlobalConst.h"
#pragma endregion Includes

#pragma region Declarations
using namespace Seidenader::SVObjectLibrary;

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
: m_LastModifiedTime( 0 )
, m_PrevModifiedTime( 0 )
{
	m_GetItemsFunctors = boost::assign::map_list_of< SVString, SVGetItemsFunctor >
		( StandardItems, boost::bind( &SVVisionProcessorHelper::GetStandardItems, this, _1, _2 ) )
		( FqnInspections, boost::bind( &SVVisionProcessorHelper::GetInspectionItems, this, _1, _2 ) )
		( FqnRemoteInputs, boost::bind( &SVVisionProcessorHelper::GetRemoteInputItems, this, _1, _2 ) )
		;

	m_SetItemsFunctors = boost::assign::map_list_of< SVString, SVSetItemsFunctor >
		( StandardItems, boost::bind( &SVVisionProcessorHelper::SetStandardItems, this, _1, _2 ) )
		( FqnInspections, boost::bind( &SVVisionProcessorHelper::SetInspectionItems, this, _1, _2 ) )
		( FqnRemoteInputs, boost::bind( &SVVisionProcessorHelper::SetRemoteInputItems, this, _1, _2 ) )
		( FqnCameras, boost::bind( &SVVisionProcessorHelper::SetCameraItems, this, _1, _2 ) )
		;
}

SVVisionProcessorHelper::~SVVisionProcessorHelper()
{
	Shutdown();
}
#pragma endregion Constructor

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

	p_rCount = TheSVObserverApp.getOfflineCount();

	return l_Status;
}

HRESULT SVVisionProcessorHelper::LoadConfiguration( const SVString& p_rPackFileName )
{
	// Check if we are in an allowed state first
	// Not allowed to perform if in Regression or Test
	if (SVSVIMStateClass::CheckState(SV_STATE_TEST | SV_STATE_REGRESSION))
	{
		return SVMSG_63_SVIM_IN_WRONG_MODE;
	}
	HRESULT l_Status = TheSVObserverApp.LoadPackedConfiguration( p_rPackFileName.c_str() );

	return l_Status;
}

HRESULT SVVisionProcessorHelper::SaveConfiguration( const SVString& p_rPackFileName ) const
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
		p_rMode = SVSVIMStateClass::GetMode();
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

HRESULT SVVisionProcessorHelper::GetDataDefinitionList( const SVString& p_rInspectionName, const SVDataDefinitionListType& p_rListType, SVDataDefinitionStructArray& p_rDataDefinitionArray) const
{
	HRESULT l_Status = S_OK;
	long l_ValueFilter = -1;
	long l_ImageFilter = -1;

	if(0 != ( p_rListType & SelectedValues ))
	{
		l_ValueFilter = SV_DD_VALUE;
	}

	if(0 != ( p_rListType & AllValues))
	{
		l_ValueFilter = SV_DD_VALUE | SV_VIEWABLE;
	}

	if(0 != ( p_rListType & SelectedImages))
	{
		l_ImageFilter = SV_DD_IMAGE;
	}

	if(0 != ( p_rListType & AllImages))
	{
		l_ImageFilter = SV_DD_IMAGE | SV_VIEWABLE;
	}

	SVInspectionProcess* pInspection = nullptr;
	SVConfigurationObject* l_pConfig = nullptr;
	SVObjectManagerClass::Instance().GetConfigurationObject( l_pConfig );

	if ( (NULL != l_pConfig) && l_pConfig->GetInspectionObject(p_rInspectionName.c_str(), &pInspection) )
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
				if( NULL != pInfoItem )
				{
					l_ObjRef = pInfoItem->GetObjectReference();
				}
				else
				{
					ASSERT(0);
					break;
				}

				SVObjectClass* l_pObject = l_ObjRef.Object();

				if( l_pObject )
				{
					SVDataDefinitionStruct l_DataDefinition;
					if(S_OK == GetObjectDefinition(*l_pObject, l_ValueFilter, l_DataDefinition))
					{
						p_rDataDefinitionArray.push_back(l_DataDefinition);
					}
				}
			}
		}

		if( l_ImageFilter != -1 )
		{
			//Add image definition list
			SVImageListClass l_ImageList;
			pToolSet->GetImageList( l_ImageList );

			int nCount = l_ImageList.GetSize();
			for( int i = 0; i < nCount; i++ )
			{
				SVImageClass* l_pImage = l_ImageList.GetAt( i );

				if ( l_pImage )
				{
					SVDataDefinitionStruct l_DataDefinition;
					if(S_OK == GetObjectDefinition(*l_pImage, l_ImageFilter, l_DataDefinition))
					{
						p_rDataDefinitionArray.push_back(l_DataDefinition);
					}
				}
			}
		}
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

				//If name is found in list then use it otherwise use the standard method
				if( l_FunctorIter != m_GetItemsFunctors.end() )
				{
					l_NameSets[ l_FunctorIter->first ].insert( *l_Iter );
				}
				else
				{
					l_NameSets[ StandardItems ].insert( *l_Iter );
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
					l_NameStorageItems[ StandardItems ].insert( *l_Iter );
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

HRESULT SVVisionProcessorHelper::GetStandardItems( const SVNameSet& p_rNames, SVNameStorageResultMap& p_rItems ) const
{
	HRESULT Status = S_OK;

	p_rItems.clear();

	for( SVNameSet::const_iterator l_Iter = p_rNames.begin(); SUCCEEDED( Status ) && l_Iter != p_rNames.end(); ++l_Iter )
	{
		HRESULT LoopStatus = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
		SVObjectReference ref;
		SVStorage ValueStorage;

		SVObjectManagerClass::Instance().GetObjectByDottedName( *l_Iter, ref );

		if( ref.Object() != NULL )
		{
			BasicValueObject* pValueObject = dynamic_cast< BasicValueObject* >( ref.Object() );

			if( NULL != pValueObject )
			{
				_variant_t Value;

				if( S_OK == pValueObject->getValue( Value ) )
				{
					ValueStorage.m_Variant = Value;
					ValueStorage.m_StorageType = SVVisionProcessor::SVStorageValue;
					LoopStatus = S_OK;
				}
			}
		}
		p_rItems[ l_Iter->c_str() ] = SVStorageResult(ValueStorage, LoopStatus, 0);
		
		if( S_OK == Status  && S_OK != LoopStatus )
		{
			Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
		}
	}

	return Status;
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

HRESULT SVVisionProcessorHelper::SetStandardItems( const SVNameStorageMap& rItems, SVNameStatusMap& rStatus )
{
	HRESULT Status = S_OK;

	rStatus.clear();

	if( !( rItems.empty() ) )
	{
		bool Online = SVSVIMStateClass::CheckState( SV_STATE_RUNNING );

		for( SVNameStorageMap::const_iterator Iter = rItems.begin(); Iter != rItems.end(); ++Iter )
		{
			HRESULT LoopStatus = S_OK;
			BasicValueObject* pValueObject = NULL;

			SVObjectManagerClass::Instance().GetObjectByDottedName( Iter->first, pValueObject );

			if( pValueObject != NULL )
			{
				bool Attribute = ( ( pValueObject->ObjectAttributesAllowed() & SV_REMOTELY_SETABLE ) == SV_REMOTELY_SETABLE );

				if( Attribute )
				{
					Attribute = !Online || ( ( pValueObject->ObjectAttributesAllowed() & SV_SETABLE_ONLINE ) == SV_SETABLE_ONLINE );

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
			if( Status == S_OK && LoopStatus != S_OK )
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

HRESULT SVVisionProcessorHelper::SetCameraItems( const SVNameStorageMap& p_rItems, SVNameStatusMap& p_rStatus )
{
	HRESULT l_Status = S_OK;

	SVConfigurationObject* l_pConfig = NULL;

	l_Status = SVObjectManagerClass::Instance().GetConfigurationObject( l_pConfig );

	if( l_pConfig != NULL )
	{
		l_Status = l_pConfig->SetCameraItems( p_rItems, p_rStatus );
	}
	else if( l_Status == S_OK )
	{
		l_Status = E_UNEXPECTED;
	}

	return l_Status;
}

HRESULT SVVisionProcessorHelper::GetObjectDefinition( const SVObjectClass& p_rObj, const long p_Filter, SVDataDefinitionStruct& p_rDataDef ) const
{
	HRESULT l_Status = S_OK;

	//Check using the filter if object should be included
	bool l_bValueIncluded = false;
	if((SV_DD_VALUE == p_Filter) || (SV_DD_IMAGE == p_Filter))
	{
		//This is called when selected values or images
		l_bValueIncluded = (p_rObj.ObjectAttributesSet() & p_Filter) != 0;
	}
	else
	{
		//This is called when all values or all images
		l_bValueIncluded = (p_rObj.ObjectAttributesAllowed() & p_Filter) != 0;
	}
	l_bValueIncluded = l_bValueIncluded && ( (p_rObj.ObjectAttributesAllowed() & SV_HIDDEN) == 0 );
	if( l_bValueIncluded )
	{
		CString l_String;
		l_String = _T("Inspections.") + p_rObj.GetCompleteObjectName();
		p_rDataDef.m_Name = l_String;
		p_rDataDef.m_Writable = (p_rObj.ObjectAttributesAllowed() & SV_REMOTELY_SETABLE) == SV_REMOTELY_SETABLE;
		p_rDataDef.m_Published = (p_rObj.ObjectAttributesSet() & SV_PUBLISHABLE) != 0;
		const SVValueObjectClass* l_pValueObject = dynamic_cast<const SVValueObjectClass*> (&p_rObj);
		//If null we assume its an image
		if( NULL != l_pValueObject)
		{
			l_pValueObject->GetTypeName(l_String);
			p_rDataDef.m_Type = l_String;
		}
		else
		{
			//For now we are setting the type to a generic "Image" could change in the future
			p_rDataDef.m_Type = _T("Image");
		}
		//This part fills the additional info section
		if( SVEnumValueObjectType == p_rObj.GetObjectType() )
		{
			// Get the strings from the enumeration value object class.
			const SVEnumerateValueObjectClass* l_pEnumVO = dynamic_cast<const SVEnumerateValueObjectClass*> (&p_rObj);
			if( NULL != l_pEnumVO )
			{
				SVEnumerateVector l_EnumVect;
				SVEnumerateVector::iterator l_EnumIter;

				l_pEnumVO->GetEnumTypes( l_EnumVect );
				for( l_EnumIter = l_EnumVect.begin(); l_EnumIter != l_EnumVect.end(); l_EnumIter++)
				{
					p_rDataDef.m_AdditionalInfo.push_back(l_EnumIter->first);
				}
			}
		}
		else if( SVBoolValueObjectType == p_rObj.GetObjectType() )
		{
			// Get the strings from the enumeration value object class.
			const SVBoolValueObjectClass* l_pBoolVO = dynamic_cast<const SVBoolValueObjectClass*> (&p_rObj);
			if( NULL != l_pBoolVO)
			{
				SVBoolValueObjectClass::SVValidTypesVector l_StringVect;
				SVBoolValueObjectClass::SVValidTypesVector::iterator l_StringIter;
				l_pBoolVO->GetValidTypes(l_StringVect);
				for( l_StringIter = l_StringVect.begin(); l_StringIter != l_StringVect.end(); l_StringIter++)
				{
					p_rDataDef.m_AdditionalInfo.push_back(*l_StringIter);
				}
			}
		}
		else
		//This should be an image so check for its additional info
		{
			const SVImageClass* l_pImage = dynamic_cast<const SVImageClass*> (&p_rObj);
			if(NULL != l_pImage)
			{
				SVToolClass* l_pTool = l_pImage->GetTool();
				if( NULL != l_pTool )
				{
					SVStringValueObjectClass* l_pSourceNames=NULL;
					l_pTool->GetInputImageNames( l_pSourceNames );
					if( l_pSourceNames )
					{
						long l_lSize = l_pSourceNames->GetArraySize();
						for( long l_lIndex = 0; l_lIndex < l_lSize ; l_lIndex++ )
						{
							HRESULT l_hr = l_pSourceNames->GetValue( l_pSourceNames->GetLastSetIndex(), l_lIndex, l_String );
							// Prepend the "Inspections." prefix for use with SVRC.
							l_String = _T( "Inspections." ) + l_String;
							p_rDataDef.m_AdditionalInfo.push_back(l_String);
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

static void BuildNameSetForMonitoredObjectList( const MonitoredObjectList& rList, SVNameSet& rNames )
{
	for (MonitoredObjectList::const_iterator it = rList.begin();it != rList.end();++it)
	{
		const SVString& name = RemoteMonitorListHelper::GetNameFromMonitoredObject(*it);
		if (!name.empty())
		{
			rNames.insert(name);
		}
	}
}

HRESULT SVVisionProcessorHelper::QueryProductList( const SVString& rListName, SVNameSet& rNames ) const
{
	SVConfigurationObject* pConfig = nullptr;

	HRESULT hr = SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	if ( nullptr != pConfig )
	{
		const RemoteMonitorList& rList = pConfig->GetRemoteMonitorList();
		RemoteMonitorList::const_iterator it = rList.find( rListName );
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
		hr = E_POINTER;
	}
	return hr;
}

HRESULT SVVisionProcessorHelper::QueryRejectCondList( const SVString& rListName, SVNameSet& rNames ) const
{
	SVConfigurationObject* pConfig = nullptr;

	HRESULT hr = SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	if ( nullptr != pConfig )
	{
		const RemoteMonitorList& rList = pConfig->GetRemoteMonitorList();
		RemoteMonitorList::const_iterator it = rList.find( rListName );
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
		hr = E_POINTER;
	}
	return hr;
}

HRESULT SVVisionProcessorHelper::QueryFailStatusList( const SVString& rListName, SVNameSet& rNames ) const
{
	SVConfigurationObject* pConfig = nullptr;

	HRESULT hr = SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	if ( nullptr != pConfig )
	{
		const RemoteMonitorList& rList = pConfig->GetRemoteMonitorList();
		RemoteMonitorList::const_iterator it = rList.find( rListName );
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
		hr = E_POINTER;
	}
	return hr;
}

HRESULT SVVisionProcessorHelper::ActivateMonitorList( const SVString& rListName, bool bActivate )
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
			hr = E_POINTER;
		}
	}
	else
	{
		hr = SVMSG_SVF_ACCESS_DENIED;
	}
	return hr;
}

HRESULT SVVisionProcessorHelper::QueryMonitorListNames( SVNameSet& rNames ) const
{
	SVConfigurationObject* pConfig = nullptr;

	HRESULT hr = SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	if ( nullptr != pConfig )
	{
		const RemoteMonitorList& rList = pConfig->GetRemoteMonitorList();
		for (RemoteMonitorList::const_iterator it = rList.begin();it != rList.end();++it)
		{
			// insert monitor list to result name set
			rNames.insert(it->first);
		}
	}
	else
	{
		hr = E_POINTER;
	}
	return hr;
}

HRESULT SVVisionProcessorHelper::SetProductFilter(const SVString& rListName, SVProductFilterEnum filter) 
{
	SVConfigurationObject* pConfig = nullptr;

	HRESULT hr = SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	if ( nullptr != pConfig )
	{
		hr = pConfig->SetRemoteMonitorListProductFilter( rListName, filter );
	}
	else
	{
		hr = E_POINTER;
	}
	return  hr;
}

HRESULT SVVisionProcessorHelper::GetProductFilter(const SVString& rListName, SVProductFilterEnum& rFilter) const
{
	SVConfigurationObject* pConfig = nullptr;

	HRESULT hr = SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	if ( nullptr != pConfig )
	{
		hr = pConfig->GetRemoteMonitorListProductFilter( rListName, rFilter );
	}
	else
	{
		hr = E_POINTER;
	}
	return  hr;
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

void CALLBACK SVVisionProcessorHelper::APCThreadProcess( DWORD_PTR dwParam )
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
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVVisionProcessorHelper.cpp_v  $
 * 
 *    Rev 1.22   20 Oct 2014 11:21:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  951
 * SCR Title:  Correct Responses to Remote Commands
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised LoadConfiguration to return an error if SVObserver is in Regression or Test mode.
 * Revised SetConfigurationMode to return an error if transitioning from or to Regression or Test mode.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   29 Aug 2014 17:49:06   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added support for get/set product filter.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   29 Aug 2014 15:45:04   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  934
 * SCR Title:  Add Remote Access to Environment.Mode Parameters
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed GetConfigurationMode to call SVSVIMStateClass::GetMode.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   22 Aug 2014 14:42:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised QueryMonitorListNames to remove state restrictions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   22 Aug 2014 14:18:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised QueryFailStatusList to get the fail statis list instead of the reject condition list.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   18 Aug 2014 07:37:54   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  900
 * SCR Title:  Separate View Image Update, View Result Update flags; remote access E55,E92
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Fixed: Missed return value for a certain path in the method GetStandardItems
 * Methods changed: GetStandardItems
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   17 Jul 2014 20:51:54   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Removed namespaces and code review changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   09 Jul 2014 17:04:06   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  915
 * SCR Title:  Add command QueryMonitorListNames for RemoteControl
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   add method QueryMonitorListNames
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   08 Jul 2014 08:58:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised BuildNameSetForMonitoredObjectList to use RemoteMonitorListHelper
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   09 Jun 2014 16:12:08   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed type for nCount in method GetDataDefinitionList to avoid warning on x64 platform.
 * Changed assignments in method GetDataDefinitionList to use nullptr.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   02 Jun 2014 10:24:38   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  900
 * SCR Title:  Separate View Image Update, View Result Update flags; remote access E55,E92
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added SetStandardItems method and the constant StandardItems.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   24 Apr 2014 11:29:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised QueryProductList, QueryRejectCondList, and QueryFailStatusList to improve error reporting.
 * Revised ActivateMonitorList to check for allowed SVIM state.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   23 Apr 2014 18:04:54   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed DeactivateMonitorList method.
 * Revised ActivateMonitorList method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   23 Apr 2014 10:43:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added QueryProductList, QueryRejectCondList, QueryFailStatusList, ActivateMonitorList, and DeactivateMonitorList methods.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   17 Mar 2014 15:34:04   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  869
 * SCR Title:  Add PPQ and Environment Variables to Object Manager and Update Pickers
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed functors to new use Object Manager constants.
 *   Removed GetWindowItems and GetEnviromentItems and created GetStandardItems and SetCameraItems methods to have SVRC access the new variables in the OM.
 *   Added method SetCameraItems.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   07 Mar 2014 18:25:18   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added regions.
 *   Added DEBUG_NEW.
 *   Made methods const.
 *   Various code changes to better follow coding guidelines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   31 Jan 2014 17:16:24   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed to follow guidelines more closely.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   18 Nov 2013 12:44:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added static_casts to build without warnings.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   30 Oct 2013 15:42:42   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  866
 * SCR Title:  Add GetDataDefinitionList Command to SVObserver's Remote Command Socket
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Moved include of SVObjectManagerClass.h to SVVisionProcessorHelper.h.
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