//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVConfigurationObject
//* .File Name       : $Workfile:   SVConfigurationObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.37  $
//* .Check In Date   : $Date:   19 Dec 2014 15:01:46  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <comdef.h>


#include "SVConfigurationObject.h"

#include "GuiCommands/InspectionRunOnce.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVObjectLibrary/SVObjectAsynchronousCommandTemplate.h"
#include "SVObjectLibrary/SVObjectSynchronousCommandTemplate.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVUtilityLibrary/SVSafeArray.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVStringConversions.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "CameraLibrary/SVDeviceParams.h"
#include "CameraLibrary/SVBoolValueDeviceParam.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVTriggerLibrary/SVTriggerEnums.h"
#include "SVAcquisitionDevice.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#include "SVObserver.h"
#include "SVInputObjectList.h"
#include "SVOutputObjectList.h"
#include "SVRemoteInputObject.h"
#include "SVCommandInspectionGetItems.h"
#include "SVDigitalInputObject.h"
#include "SVDigitalOutputObject.h"
#include "SVMainFrm.h"
#include "SVIODoc.h"
#include "SVIOController.h"
#include "SVAcquisitionClass.h"
#include "SVToolSet.h"
#include "ObjectInterfaces/SVUserMessage.h"
#include "TriggerHandling/SVAcquisitionInitiator.h"
#include "TriggerInformation/SVTriggerProcessingClass.h"
#include "SVDigitizerProcessingClass.h"
#include "SVConfigurationTreeWriter.h"
#include "TriggerHandling/SVCameraTriggerClass.h"
#include "TriggerInformation/SVHardwareManifest.h"
#include "SVCameraDataInputObject.h"
#include "SVGlobal.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVStorageResult.h"
#include "SVVirtualCamera.h"
#include "SVObjectLibrary\GlobalConst.h"
#include "RemoteMonitorNamedList.h"
#include "RemoteMonitorListHelper.h"
#include "RootObject.h"
#include "SVSystemLibrary\SVThreadManager.h"
#include "RangeClassHelper.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVIPDoc.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "ObjectInterfaces\GlobalConst.h"
#include "TextDefinesSvO.h"
#include "SVColorTool.h"
#pragma endregion Includes

#pragma region Declarations

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

typedef std::deque< SVVirtualCamera* > SVVirtualCameraPtrList;

SV_IMPLEMENT_CLASS( SVConfigurationObject, SVConfigurationObjectGuid );
#pragma endregion Declarations

#pragma region Constructor
SVConfigurationObject::SVConfigurationObject( LPCSTR ObjectName )
	: SVObjectClass( ObjectName )
{
	m_pIOController = new SVIOController;
	m_pInputObjectList	= nullptr;
	m_pOutputObjectList	= nullptr;
	m_bConfigurationValid = false;
	m_ulVersion = 0;

	SetProductType( SVIM_PRODUCT_TYPE_UNKNOWN );

	m_arTriggerArray.RemoveAll();
	m_arPPQArray.RemoveAll();
	m_arCameraArray.RemoveAll();
	m_arInspectionArray.RemoveAll();
}

SVConfigurationObject::SVConfigurationObject( SVObjectClass* POwner, int StringResourceID )
	: SVObjectClass( POwner, StringResourceID )
{
	m_pIOController = new SVIOController;
	m_pInputObjectList	= nullptr;
	m_pOutputObjectList	= nullptr;
	m_bConfigurationValid = false;

	SetProductType( SVIM_PRODUCT_TYPE_UNKNOWN );

	m_arTriggerArray.RemoveAll();
	m_arPPQArray.RemoveAll();
	m_arCameraArray.RemoveAll();
	m_arInspectionArray.RemoveAll();
}

SVConfigurationObject::~SVConfigurationObject()
{
	DestroyConfiguration();
}
#pragma endregion Constructor

void SVConfigurationObject::SetInputObjectList( SVInputObjectList *pInputObjectList )
{
	if( nullptr != m_pInputObjectList && m_pInputObjectList != pInputObjectList )
	{
		delete m_pInputObjectList;
	}

	m_pInputObjectList = pInputObjectList;
}// end SetInputObjectList

SVInputObjectList* SVConfigurationObject::GetInputObjectList( ) const
{
	return m_pInputObjectList;
}// end GetInputObjectList

void SVConfigurationObject::SetOutputObjectList( SVOutputObjectList *pOutputObjectList )
{
	if( nullptr != m_pOutputObjectList && m_pOutputObjectList != pOutputObjectList )
	{
		delete m_pOutputObjectList;
	}

	m_pOutputObjectList = pOutputObjectList;
}// end SetOutputObjectList

SVOutputObjectList* SVConfigurationObject::GetOutputObjectList( ) const
{
	return m_pOutputObjectList;
}// end GetOutputObjectList

HRESULT SVConfigurationObject::RebuildOutputObjectList()
{
	HRESULT l_Status = S_OK;

	// Force the PPQs to rebuild
	long lSize = GetPPQCount( );

	for( long l = 0; l < lSize; l++ )
	{
		SVPPQObject* pPPQ = GetPPQ( l );

		if( nullptr != pPPQ )
		{
			pPPQ->RebuildOutputList();
		}
	}

	return l_Status;
}

bool SVConfigurationObject::AddTrigger( SvTi::SVTriggerObject* pTrigger )
{
	bool Result(false);

	if( nullptr != pTrigger )
	{
		m_arTriggerArray.Add( pTrigger );
		Result = true;
	}

	return Result;
}

bool SVConfigurationObject::RemoveTrigger( SvTi::SVTriggerObject* pTrigger )
{
	if( nullptr == pTrigger ) { return false; }

	int iSize;

	iSize = m_arTriggerArray.GetSize();
	for(int i = 0; i < iSize; i++ )
	{
		if( pTrigger == m_arTriggerArray.GetAt( i ) )
		{
			m_arTriggerArray.RemoveAt( i );
			break;
		}
	}
	
	return true;
}

long SVConfigurationObject::GetTriggerCount( ) const
{
	return m_arTriggerArray.GetSize();
}// end GetTriggerCount

bool SVConfigurationObject::GetChildObjectByName( LPCTSTR tszName, SvTi::SVTriggerObject** ppTrigger ) const
{
	bool bReturn = false;

	//Only do an assert check so that in release mode no check is made
	ASSERT( nullptr != ppTrigger );

	long lCount = GetTriggerCount( );

	SVString Name = tszName;

	for( long l = 0; l < lCount; l++ )
	{ 
		SvTi::SVTriggerObject* pTrigger = GetTrigger( l );
		if( nullptr != pTrigger && Name == pTrigger->GetCompleteName() )
		{
			*ppTrigger = pTrigger;
			bReturn = true;
			break;
		}
	}

	return bReturn;
}

SvTi::SVTriggerObject* SVConfigurationObject::GetTrigger( long lIndex ) const
{
	SvTi::SVTriggerObject* retValue = nullptr;

	ASSERT( 0 <= lIndex && static_cast<long>(m_arTriggerArray.size()) > lIndex );

	if (0 <= lIndex && static_cast<long>(m_arTriggerArray.size()) > lIndex)
	{
		retValue = m_arTriggerArray.GetAt( lIndex );
	}

	return retValue;
}// end GetTrigger

bool SVConfigurationObject::AddAcquisitionDevice( LPCTSTR szName, 
	SVFileNameArrayClass& rsvFiles,
	SVLightReference& rsvLight,
	SVLut& rLut,
	const SVDeviceParamCollection* pDeviceParams )
{
	bool bOk( false );
	SVConfigurationAcquisitionDeviceInfoStruct* pDevice( nullptr );

	mAcquisitionDeviceMap.Lookup( szName, pDevice);

	if( nullptr == pDevice )
	{
		pDevice = new SVConfigurationAcquisitionDeviceInfoStruct;
	}

	bOk = nullptr != pDevice;
	if ( bOk )
	{
		pDevice->msvFiles = rsvFiles;
		pDevice->msvLightArray = rsvLight;
		pDevice->mLut = rLut;
		if ( nullptr != pDeviceParams )
		{
			pDevice->mDeviceParams = *pDeviceParams;
		}
		mAcquisitionDeviceMap.SetAt( szName, pDevice );
	}

	return bOk;
}// end AddAcquisitionDevice

bool SVConfigurationObject::ModifyAcquisitionDevice( LPCTSTR szName, SVLightReference& rsvLight )
{
	bool bOk = false;
	SVConfigurationAcquisitionDeviceInfoStruct* pDevice = nullptr;

	mAcquisitionDeviceMap.Lookup( szName, pDevice);

	bOk = nullptr != pDevice;
	if ( bOk )
	{
		pDevice->msvLightArray = rsvLight;
	}

	return bOk;
}// end AddAcquisitionDevice

bool SVConfigurationObject::ModifyAcquisitionDevice( LPCTSTR szName, const SVLut& lut )
{
	bool bOk = false;
	SVConfigurationAcquisitionDeviceInfoStruct* pDevice = nullptr;

	mAcquisitionDeviceMap.Lookup( szName, pDevice);

	bOk = nullptr != pDevice;
	if ( bOk )
	{
		pDevice->mLut = lut;
	}

	return bOk;
}

bool SVConfigurationObject::ModifyAcquisitionDevice( LPCTSTR szName, const SVDeviceParamCollection* pParams )
{
	bool bOk = false;
	if ( nullptr != pParams )
	{
		SVConfigurationAcquisitionDeviceInfoStruct* pDevice = nullptr;

		mAcquisitionDeviceMap.Lookup( szName, pDevice);

		bOk = nullptr != pDevice;
		if ( bOk )
		{
			// don't do a straight assignment ( m_DeviceParams = pParams )
			// only copy those parameters that are specified.
			SVDeviceParamMap::const_iterator iter;
			for (iter = pParams->mapParameters.begin(); iter != pParams->mapParameters.end(); ++iter)
			{
				const SVDeviceParamWrapper& w = iter->second;

				if ( nullptr != ((const SVDeviceParam*) w) )
				{
					pDevice->mDeviceParams.GetParameter( iter->first ) = w;
				}
			}
		}
	}
	return bOk;

}

bool SVConfigurationObject::RemoveAcquisitionDevice( LPCTSTR szName )
{
	bool bOk = false;

	SVConfigurationAcquisitionDeviceInfoStruct *pDevice = nullptr;

	bOk = mAcquisitionDeviceMap.Lookup( szName, pDevice );
	if ( bOk )
	{
		mAcquisitionDeviceMap.RemoveKey( szName );

		if ( nullptr != pDevice )
		{
			delete pDevice;		
		}
	}

	return bOk;
}

bool SVConfigurationObject::GetAcquisitionDevice( LPCTSTR szName,
	SVFileNameArrayClass*& rpFiles,
	SVLightReference*& rpLight,
	SVLut*& rpLut,
	SVDeviceParamCollection*& rpDeviceParams ) const
{
	bool bOk = false;

	SVConfigurationAcquisitionDeviceInfoStruct* pDevice = nullptr;

	rpFiles = nullptr;
	rpLight = nullptr;
	rpLut   = nullptr;
	rpDeviceParams = nullptr;

	bOk = mAcquisitionDeviceMap.Lookup( szName, pDevice );
	if ( bOk )
	{
		bOk = nullptr != pDevice;
		if ( bOk )
		{
			rpFiles = &(pDevice->msvFiles);
			rpLight = &(pDevice->msvLightArray);
			rpLut   = &(pDevice->mLut);
			rpDeviceParams = &(pDevice->mDeviceParams);
		}
	}

	return bOk;
}

SVConfigurationObject::SVAcquisitionDeviceMap::iterator SVConfigurationObject::GetAcquisitionDeviceStartPosition() const
{
	return mAcquisitionDeviceMap.GetStartPosition();
}

SVConfigurationObject::SVAcquisitionDeviceMap::iterator SVConfigurationObject::GetAcquisitionDeviceEndPosition()
{
	return mAcquisitionDeviceMap.end();
}

void SVConfigurationObject::GetAcquisitionDeviceNextAssoc( SVAcquisitionDeviceMap::iterator& rNextPosition, SVString& rKey ) const
{
	SVConfigurationAcquisitionDeviceInfoStruct* pDevice = nullptr;
	mAcquisitionDeviceMap.GetNextAssoc( rNextPosition, rKey, pDevice );
}

void SVConfigurationObject::GetAcquisitionDeviceNextAssoc( SVAcquisitionDeviceMap::iterator& rNextPosition, 
	SVString& rKey, 
	SVFileNameArrayClass*& rpFiles,
	SVLightReference*& rpLight,
	SVLut*& rpLut,
	SVDeviceParamCollection*& rpDeviceParams ) const
{
	SVConfigurationAcquisitionDeviceInfoStruct* pDevice = nullptr;

	rpFiles = nullptr;
	rpLight = nullptr;
	rpLut   = nullptr;
	rpDeviceParams = nullptr;

	mAcquisitionDeviceMap.GetNextAssoc( rNextPosition, rKey, pDevice );

	if ( nullptr != pDevice)
	{
		rpFiles = &(pDevice->msvFiles);
		rpLight = &(pDevice->msvLightArray);
		rpLut   = &(pDevice->mLut);
		rpDeviceParams = &(pDevice->mDeviceParams);
	}
}

bool SVConfigurationObject::AddPPQ( SVPPQObject* pPPQ )
{
	bool Result( false );

	if( nullptr != pPPQ )
	{
		m_arPPQArray.Add( pPPQ );
		Result = true;
	}

	return Result;
}

bool SVConfigurationObject::RemovePPQ( SVPPQObject* pPPQ )
{
	if( nullptr == pPPQ ) { return false; }

	int iSize;

	iSize = m_arPPQArray.GetSize();
	for( int i = 0; i < iSize; i++ )
	{
		if(  pPPQ == m_arPPQArray.GetAt( i ) )
		{
			m_arPPQArray.RemoveAt( i );
			break;
		}
	}

	return true;
}

long SVConfigurationObject::GetPPQCount( ) const
{
	return m_arPPQArray.GetSize();
}// end GetPPQCount

bool SVConfigurationObject::GetChildObjectByName( LPCTSTR tszName, SVPPQObject** ppPPQ ) const
{
	bool bReturn = false;

	//Only do an assert check so that in release mode no check is made
	ASSERT( nullptr != ppPPQ );

	long lCount = GetPPQCount();

	SVString Name = tszName;

	for( long l = 0; l < lCount; l++ )
	{ 
		SVPPQObject* pPPQ = GetPPQ( l );
		if( nullptr != pPPQ && Name == pPPQ->GetCompleteName() )
		{
			*ppPPQ = pPPQ;
			bReturn = true;
			break;
		}
	}

	return bReturn;
}

SVPPQObject* SVConfigurationObject::GetPPQ( long lIndex ) const
{
	SVPPQObject* retValue = nullptr;
	ASSERT( 0 <= lIndex && static_cast<long>(m_arPPQArray.size()) > lIndex );

	if (0 <= lIndex && static_cast<long>(m_arPPQArray.size()) > lIndex)
	{
		retValue = m_arPPQArray.GetAt( lIndex );
	}

	return retValue;
}// end GetPPQ

bool SVConfigurationObject::GetPPQByName( LPCTSTR name, SVPPQObject** ppPPQ ) const
{
	bool bReturn = false;

	//Only do an assert check so that in release mode no check is made
	ASSERT( nullptr != ppPPQ );

	int iSize = m_arPPQArray.GetSize();
	for (int i = 0;i < iSize && !bReturn;i++)
	{
		SVPPQObject* pPPQ = m_arPPQArray.GetAt(i);
		if ( nullptr != pPPQ)
		{
			SVString ppqName = pPPQ->GetName();
			if (ppqName == name)
			{
				*ppPPQ = pPPQ;
				bReturn = true;
			}
		}
	}
	return bReturn;
}

bool SVConfigurationObject::AddCamera( SVVirtualCamera* pCamera )
{
	bool Result( false );

	if( nullptr != pCamera )
	{
		m_arCameraArray.Add( pCamera );
		Result = true;
	}
	return Result;
}

bool SVConfigurationObject::RemoveCamera( SVVirtualCamera* pCamera )
{
	if( nullptr == pCamera ) { return false; }

	int iSize;

	iSize = m_arCameraArray.GetSize();
	for( int i = 0; i < iSize; i++ )
	{
		if(  pCamera == m_arCameraArray.GetAt( i ) )
		{
			m_arCameraArray.RemoveAt( i );
			break;
		}
	}

	return true;
}

long SVConfigurationObject::GetCameraCount( ) const
{
	return m_arCameraArray.GetSize();
}

bool SVConfigurationObject::GetChildObjectByName( LPCTSTR tszName, SVVirtualCamera** ppCamera ) const
{
	bool bReturn = false;

	//Only do an assert check so that in release mode no check is made
	ASSERT( nullptr != ppCamera );

	long lCount = GetCameraCount( );
	SVString Name = tszName;

	for( long l = 0; l < lCount; l++ )
	{ 
		SVVirtualCamera* pCamera = GetCamera( l );
		if( nullptr != pCamera && Name == pCamera->GetCompleteName() )
		{
			*ppCamera= pCamera;
			bReturn = true;
			break;
		}
	}

	return bReturn;
}

SVVirtualCamera* SVConfigurationObject::GetCamera( long lIndex ) const
{
	SVVirtualCamera* retValue = nullptr;

	ASSERT( 0 <= lIndex && static_cast<long>(m_arCameraArray.size()) > lIndex );

	if (0 <= lIndex && static_cast<long>(m_arCameraArray.size()) > lIndex)
	{
		retValue = m_arCameraArray.GetAt( lIndex );
	}

	return retValue;
}// end GetCamera

bool SVConfigurationObject::AddInspection( SVInspectionProcess* pInspection )
{
	bool Result( false );

	if( nullptr != pInspection )
	{
		m_arInspectionArray.Add( pInspection );
		Result = true;
	}

	return Result;
}

bool SVConfigurationObject::RemoveInspection( SVInspectionProcess* pInspection )
{
	if( nullptr == pInspection ) { return false; }

	int i;
	int iSize;

	iSize = m_arInspectionArray.GetSize();
	for( i = 0; i < iSize; i++ )
	{
		if(  pInspection == m_arInspectionArray.GetAt( i ) )
		{
			m_arInspectionArray.RemoveAt( i );
			break;
		}
	}

	return true;
}

long SVConfigurationObject::GetInspectionCount( ) const
{
	return m_arInspectionArray.GetSize();
}// end GetInspectionCount

bool SVConfigurationObject::GetInspectionObject( LPCTSTR Name, SVInspectionProcess** ppInspection ) const
{
	bool bResult = false;

	SVString InspectionName( Name );
	size_t Pos = InspectionName.find(_T("."));	// assumes first dot is after the inspection name
	if( SVString::npos != Pos )
	{
		bResult = GetChildObjectByName( SvUl_SF::Left(InspectionName, Pos).c_str(), ppInspection );
	}
	else
	{
		bResult = GetChildObjectByName( InspectionName.c_str(), ppInspection );
	}

	return bResult;
}

bool SVConfigurationObject::GetChildObjectByName( LPCTSTR tszName, SVInspectionProcess** ppInspection ) const
{
	bool bReturn = false;

	//Only do an assert check so that in release mode no check is made
	ASSERT( nullptr != ppInspection );

	long lCount = GetInspectionCount( );
	SVString Name = tszName;

	for( long l = 0; l < lCount; l++ )
	{ 
		SVInspectionProcess* pInspection = GetInspection( l );
		if( nullptr != pInspection && Name == pInspection->GetCompleteName() )
		{
			*ppInspection = pInspection;
			bReturn = true;
			break;
		}
	}
	if (!bReturn)
	{
		*ppInspection = nullptr;
	}

	return bReturn;
}

SVInspectionProcess* SVConfigurationObject::GetInspection( long lIndex ) const
{
	SVInspectionProcess* retValue = nullptr;
	ASSERT( 0 <= lIndex && static_cast<long>(m_arInspectionArray.size()) > lIndex );

	if (0 <= lIndex && static_cast<long>(m_arInspectionArray.size()) > lIndex)
	{
		retValue = m_arInspectionArray.GetAt( lIndex );
	}

	return retValue;
}// end GetInspection

void SVConfigurationObject::GetInspections( SVInspectionProcessPtrList& rvecInspections ) const
{
	std::copy( m_arInspectionArray.begin(), m_arInspectionArray.end(), std::inserter( rvecInspections, rvecInspections.begin() ) );
}

// Add the (Imported) Input if it doesn't exists
// Do not adjust the ppqPosition if the position is greater than the PPQ length
HRESULT SVConfigurationObject::AddImportedRemoteInput(SVPPQObject* pPPQ, const SVString& name, long ppqPosition, long index, const _variant_t& p_Value)
{
	HRESULT hr = S_OK;

	//Only do an assert check so that in release mode no check is made
	ASSERT( nullptr != pPPQ );

	SVIOEntryHostStructPtr pIOEntry = pPPQ->GetInput(name);
	if (pIOEntry.empty())
	{
		hr = AddRemoteInput(pPPQ, name, ppqPosition, index, p_Value);
	}
	else
	{
		if (!pIOEntry->m_Enabled)
		{
			pIOEntry->m_ObjectType = IO_REMOTE_INPUT;
			pIOEntry->m_PPQIndex = ppqPosition;
			pIOEntry->m_Enabled = true;

			SVRemoteInputObject* pRemoteInput = nullptr;
			int number = -1;
			sscanf_s(name.c_str(), SvO::cRemoteInputNumberLabel, &number);
			pRemoteInput = dynamic_cast<SVRemoteInputObject*> (m_pInputObjectList->GetInputFlyweight( name, SVRemoteInputObjectType, number - 1));
			if (nullptr != pRemoteInput)
			{
				pRemoteInput->m_lIndex = index;
				pRemoteInput->Create();
				pRemoteInput->WriteCache( p_Value );
			}
		}
	}
	return hr;
}

// Add the (Imported) Input if it doesn't exists
// Do not adjust the ppqPosition if the position is greater than the PPQ length
HRESULT SVConfigurationObject::AddImportedDigitalInput(SVPPQObject* pPPQ, const SVString& name, long ppqPosition)
{
	HRESULT hr = S_OK;

	//Only do an assert check so that in release mode no check is made
	ASSERT( nullptr != pPPQ );

	SVIOEntryHostStructPtr pIOEntry = pPPQ->GetInput(name);
	if (pIOEntry.empty())
	{
		hr = AddDigitalInput(pPPQ, name, ppqPosition);
	}
	else
	{
		if (!pIOEntry->m_Enabled)
		{
			SVDigitalInputObject* pDigitalInput = nullptr;
			pDigitalInput = dynamic_cast<SVDigitalInputObject*> (m_pInputObjectList->GetInputFlyweight( name, SVDigitalInputObjectType));

			pIOEntry->m_ObjectType = IO_DIGITAL_INPUT;
			pIOEntry->m_PPQIndex = ppqPosition;
			pIOEntry->m_Enabled = true;
		}
	}
	return hr;
}

HRESULT SVConfigurationObject::AddRemoteInput(SVPPQObject* pPPQ, const SVString& name, long ppqPosition, long index, const _variant_t& p_Value)
{
	HRESULT hr = S_OK;

	//Only do an assert check so that in release mode no check is made
	ASSERT( nullptr != pPPQ );


	// Add Remote Inputs to the InputObjectList
	//first check to see if remote input is there, check by name...
	SVRemoteInputObject* pRemoteInput = nullptr;
	int number = -1;
	sscanf_s(name.c_str(), SvO::cRemoteInputNumberLabel, &number);
	pRemoteInput = dynamic_cast<SVRemoteInputObject*> (m_pInputObjectList->GetInputFlyweight(name, SVRemoteInputObjectType, number-1));

	// Add Remote Input to the PPQ
	SVVariantValueObjectClass* pValueObject = new SVVariantValueObjectClass();
	pValueObject->SetName( name.c_str() );
	pValueObject->SetObjectOwner(pPPQ);
	pValueObject->SetObjectDepth(10);
	pValueObject->setResetOptions( false, SvOi::SVResetItemNone );
	pValueObject->setBucketized(true);
	pValueObject->SetObjectAttributesAllowed(SvOi::SV_SELECTABLE_ATTRIBUTES, SvOi::SetAttributeType::RemoveAttribute);
	pValueObject->ResetObject();

	SVIOEntryHostStructPtr pIOEntry = new SVIOEntryHostStruct;
	pIOEntry->setObject( dynamic_cast<SVObjectClass*> (pValueObject) );
	pIOEntry->m_ObjectType		= IO_REMOTE_INPUT;
	pIOEntry->m_PPQIndex		= ppqPosition;
	pIOEntry->m_Enabled			= ppqPosition != -1;

	if( nullptr != pRemoteInput )
	{
		pRemoteInput->m_lIndex = index;
		pRemoteInput->Create();
		pRemoteInput->WriteCache( p_Value );

		pIOEntry->m_IOId = pRemoteInput->GetUniqueObjectID();
	}

	pPPQ->AddInput( pIOEntry );

	return hr;
}

HRESULT SVConfigurationObject::AddDigitalInput(SVPPQObject* pPPQ, const SVString& name, long ppqPosition)
{
	HRESULT hr = S_OK;

	//Only do an assert check so that in release mode no check is made
	ASSERT( nullptr != pPPQ );

	SVDigitalInputObject* pDigitalInput = nullptr;

	pDigitalInput = dynamic_cast<SVDigitalInputObject*> (m_pInputObjectList->GetInputFlyweight( name, SVDigitalInputObjectType));

	// Add Digital Input to the PPQ
	SVBoolValueObjectClass* pValueObject = new SVBoolValueObjectClass();
	pValueObject->SetName(name.c_str());
	pValueObject->SetObjectOwner(pPPQ);
	pValueObject->SetObjectDepth(10);
	pValueObject->setResetOptions(false, SvOi::SVResetItemNone);
	pValueObject->setBucketized(true);
	pValueObject->SetObjectAttributesAllowed(SvOi::SV_SELECTABLE_ATTRIBUTES, SvOi::SetAttributeType::RemoveAttribute);
	pValueObject->ResetObject();

	SVIOEntryHostStructPtr pIOEntry = new SVIOEntryHostStruct;
	pIOEntry->setObject(dynamic_cast<SVObjectClass*> (pValueObject));
	pIOEntry->m_ObjectType		= IO_DIGITAL_INPUT;
	pIOEntry->m_PPQIndex		= ppqPosition;
	pIOEntry->m_Enabled			= ppqPosition != -1;

	if( nullptr != pDigitalInput )
	{
		pIOEntry->m_IOId = pDigitalInput->GetUniqueObjectID();
	}

	pPPQ->AddInput(pIOEntry);

	return hr;
}

HRESULT SVConfigurationObject::AddCameraDataInput(SVPPQObject* pPPQ, SVIOEntryHostStructPtr pIOEntry )
{
	HRESULT hr = S_OK;

	//Only do an assert check so that in release mode no check is made
	ASSERT( nullptr != pPPQ );

	SVCameraDataInputObject* pInput = nullptr;
	SVString name = pIOEntry->getObject()->GetName();
	pInput = dynamic_cast<SVCameraDataInputObject*> (m_pInputObjectList->GetInputFlyweight(name, SVCameraDataInputObjectType));

	// Add Input to the PPQ
	if( nullptr != pInput )
	{
		pInput->Create();
		pIOEntry->m_IOId = pInput->GetUniqueObjectID();
	}

	pPPQ->AddInput(pIOEntry);
	return hr;
}

void SVConfigurationObject::LoadEnviroment(SVTreeType& rTree , bool &ConfigurationColor)
{
	SVTreeType::SVBranchHandle hChild( nullptr );
	bool bOk = SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::CTAG_ENVIRONMENT, nullptr, hChild );

	if( !bOk )
	{
		SvStl::MessageContainer MsgCont;
		MsgCont.setMessage( SVMSG_SVO_82_NO_ENVIROMENT_TAG, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16061_EnviromentTagIsMissing );
		throw MsgCont;
	}

	_variant_t Value;

	if ( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_CONFIGURATION_TYPE, hChild, Value ) )
	{
		int iType = Value;
		SetProductType( SVIMProductEnum( iType ) );

		ConvertColorToStandardProductType( ConfigurationColor );
	}

	if( !SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_VERSION_NUMBER, hChild, Value ) )
	{
		SvStl::MessageContainer MsgCont;
		SVStringVector messageList;
		messageList.push_back(SvXml::CTAG_VERSION_NUMBER);
		MsgCont.setMessage( SVMSG_SVO_77_LOAD_VERSION_NUMBER_FAILED, SvStl::Tid_Default, messageList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16043 );
		throw MsgCont;
	}

	TheSVObserverApp.setLoadingVersion( Value );

	m_ulVersion = TheSVObserverApp.getLoadingVersion();

	//This is the deprecated tag and has changed to SvXml::CTAG_IMAGE_UPDATE and SvXml::CTAG_RESULT_UPDATE
	//Needs to be read for older configurations and becomes the standard default
	bool UpdateFlag = true;
	if (SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_ONLINE_DISPLAY, hChild, Value) )
	{
		UpdateFlag = Value;
	}

	if (SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_IMAGE_DISPLAY_UPDATE, hChild, Value) )
	{
		UpdateFlag = Value;
	}
	RootObject::setRootChildValue( SvOl::FqnEnvironmentImageUpdate, UpdateFlag );

	if (SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_RESULT_DISPLAY_UPDATE, hChild, Value ) )
	{
		UpdateFlag = Value;
	}
	RootObject::setRootChildValue( SvOl::FqnEnvironmentResultUpdate, UpdateFlag );

	// Thread Affinity Setup
	SVTreeType::SVBranchHandle hThreadSetup( nullptr );
	int iThreadNum = 1;
	bool bThreadOk = true;
	while( bThreadOk )
	{
		long lAffinity;
		SVString ThreadTag = SvUl_SF::Format(_T("%s_%d"), SvXml::CTAG_THREAD_SETUP, iThreadNum);
		bThreadOk = SvXml::SVNavigateTree::GetItemBranch( rTree, ThreadTag.c_str(), hChild, hThreadSetup );
		if( bThreadOk )
		{
			bThreadOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_THREAD_AFFINITY, hThreadSetup, Value );
			lAffinity = Value;
			bThreadOk &= SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_THREAD_NAME, hThreadSetup, Value );
			SVString Name = SvUl_SF::createSVString( Value );
			if( bThreadOk  )
			{
				SVThreadManager::Instance().Setup( Name.c_str(), lAffinity );
			}
			else
			{
				bThreadOk = false;
			}
		}
		iThreadNum++;
	}

	// Thread Manager Enable
	BOOL bThreadMgrEnable = false;
	if (SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_THREAD_MGR_ENABLE, hChild, Value) )
	{
		bThreadMgrEnable = Value;
	}
	SVThreadManager::Instance().SetThreadAffinityEnabled( bThreadMgrEnable );
}

bool SVConfigurationObject::LoadIO( SVTreeType& rTree )
{
	SVTreeType::SVBranchHandle hChild( nullptr );
	bool bOk  =  SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::CTAG_IO, nullptr, hChild );
	if (!bOk)
	{
		return false;
	}

	_variant_t Value;

	SVInputObjectList *pInputsList = new SVInputObjectList;
	SVOutputObjectList *pOutputsList = new SVOutputObjectList;
	pInputsList->SetName( SvO::InputObjectList);
	pOutputsList->SetName( SvO::OutputObjectList );
	if( nullptr == pInputsList  || !pInputsList->Create() )
	{
		SVStringVector msgList;
		msgList.push_back(SvO::InputObjectList);
		SvStl::MessageContainer MsgCont;
		MsgCont.setMessage( SVMSG_SVO_78_LOAD_IO_FAILED, SvStl::Tid_CreateSFailed, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16044_CreateInputList );
		throw MsgCont;
	}
	if( nullptr == pOutputsList || !pOutputsList->Create() )
	{
		SVStringVector msgList;
		msgList.push_back(SvO::OutputObjectList);
		SvStl::MessageContainer MsgCont;
		MsgCont.setMessage( SVMSG_SVO_78_LOAD_IO_FAILED, SvStl::Tid_CreateSFailed, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16047_CreateOutputList );
		throw MsgCont;
	}

	SVTreeType::SVBranchHandle hSubChild;
	long lIOSize = 0;
	long i;

	if( !SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_NUMBER_OF_IO_ENTRIES, hChild, Value ))
	{
		SvStl::MessageContainer MsgCont;
		MsgCont.setMessage( SVMSG_SVO_78_LOAD_IO_FAILED, SvStl::Tid_NumberOfIosMissing, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16045_MissingTag );
		throw MsgCont;
	}

	lIOSize = Value;

	SVGUID l_ModuleReadyId;
	SVGUID l_RaidErrorId;

	// Loop through all the "IO EntryXXX"
	for( i = 0; i < lIOSize; i++ )
	{
		SVString IOEntry = SvUl_SF::Format( SvXml::CTAGF_IO_ENTRY_X, i );
		if( !SvXml::SVNavigateTree::GetItemBranch( rTree, IOEntry.c_str(), hChild, hSubChild ) )
		{
			SvStl::MessageContainer MsgCont;
			MsgCont.setMessage( SVMSG_SVO_78_LOAD_IO_FAILED, SvStl::Tid_IOEntryIsMissing, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16060_IOEntryIsMissing );
			throw MsgCont;
		}

		_variant_t Data;
		SVString IOName;
		bool	bOutput;
		DWORD	dwChannel;
		bool	bForced;
		DWORD	dwForcedValue;
		bool	bInverted;
		bool	bCombined;
		bool	bCombinedACK;

		bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_IO_ENTRY_NAME, hSubChild, Data );

		if( bOk && VT_BSTR == Data.vt )
		{
			IOName = SvUl_SF::createSVString( Data );
		}// end if

		bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_IS_OUTPUT, hSubChild, Data );

		if( bOk )
		{
			bOutput = Data;
		}// end if

		bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_CHANNEL, hSubChild, Data );

		if( bOk )
		{
			dwChannel = Data;
		}// end if

		bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_IS_FORCED, hSubChild, Data );

		if( bOk )
		{
			bForced = Data;
		}// end if

		bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_FORCED_VALUE, hSubChild, Data );

		if( bOk )
		{
			dwForcedValue = static_cast< bool >( Data );
		}// end if

		bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_IS_INVERTED, hSubChild, Data );

		if( bOk )
		{
			bInverted = Data;
		}// end if

		bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_IS_FORCED, hSubChild, Data );

		if( bOk )
		{
			bForced = Data;
		}// end if

		bool bCheck;
		bCheck = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_IS_COMBINED, hSubChild, Data );
		if( bCheck )
		{
			bCombined = Data;
		}// end if
		else
		{
			bCombined = false;
		}// end else

		bCheck = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_IS_COMBINED_ACK, hSubChild, Data );
		if( bCheck )
		{
			bCombinedACK = Data;
		}// end if
		else
		{
			bCombinedACK = false;
		}// end else

		if( (long)dwChannel > (long)-1 )
		{
			if( bOutput )
			{
				SVDigitalOutputObject* pOutput(nullptr);
				pOutput = dynamic_cast<SVDigitalOutputObject*> (pOutputsList->GetOutputFlyweight(IOName.c_str(), SVDigitalOutputObjectType, dwChannel));

				if( nullptr != pOutput )
				{
					pOutput->SetChannel( dwChannel );
					pOutput->Force( bForced, ( dwForcedValue != FALSE ) );
					pOutput->Invert( bInverted );
					pOutput->Combine( bCombined, bCombinedACK );

					if(SvO::cModuleReady == IOName)
					{
						l_ModuleReadyId = pOutput->GetUniqueObjectID();
					}
					else if(SvO::cRaidErrorIndicator == IOName)
					{
						l_RaidErrorId = pOutput->GetUniqueObjectID();
					}
				}
			}// end if
			else
			{
				SVDigitalInputObject* pInput = nullptr;
				pInput = dynamic_cast<SVDigitalInputObject*> (pInputsList->GetInputFlyweight( IOName, SVDigitalInputObjectType, dwChannel));

				if( nullptr != pInput )
				{
					pInput->SetChannel( dwChannel );
					pInput->Force( bForced, dwForcedValue != FALSE );
					pInput->Invert( bInverted );							
				}
			}// end else
		}// end if

		if(!bOk)
		{
			SvStl::MessageContainer MsgCont;
			MsgCont.setMessage( SVMSG_SVO_78_LOAD_IO_FAILED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16046_LOAD_IO_FAILED );
			throw MsgCont;
		}
	}// end for

	if( nullptr != m_pInputObjectList )
	{
		delete m_pInputObjectList;
	}

	m_pInputObjectList  = pInputsList;

	if( nullptr != m_pOutputObjectList )
	{
		delete m_pOutputObjectList;
	}

	m_pOutputObjectList = pOutputsList;

	if ( nullptr != m_pIOController )
	{
		delete m_pIOController;
		m_pIOController = nullptr;
	}

	m_pIOController = new SVIOController;
	if ( nullptr != m_pIOController )
	{
		m_pIOController->SetParameters( rTree, hChild );
	}

	if(  l_ModuleReadyId.empty() )
	{
		SVDigitalOutputObject* pOutput(nullptr);
		pOutput = dynamic_cast<SVDigitalOutputObject*> (m_pOutputObjectList->GetOutputFlyweight(SvO::cModuleReady, SVDigitalOutputObjectType));

		if( nullptr != pOutput )
		{
			l_ModuleReadyId = pOutput->GetUniqueObjectID();
		}
	}
	m_pIOController->GetModuleReady()->m_IOId = l_ModuleReadyId;
	m_pIOController->GetRaidErrorBit()->m_IOId = l_RaidErrorId;

	return true;
}

bool SVConfigurationObject::LoadAcquisitionDevice( SVTreeType& rTree, SVString& BoardName, long &lNumBordDig )
{
	SVTreeType::SVBranchHandle hChild( nullptr );
	SVTreeType::SVBranchHandle hBoardChild( nullptr );

	bool bOk  =  SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::CTAG_ACQUISITION_DEVICE, nullptr, hChild );

	hBoardChild = rTree.getFirstBranch( hChild );

	while ( bOk && nullptr != hBoardChild )
	{
		BoardName = rTree.getBranchName( hBoardChild );

		long lNumAcqDig=0;

		// test for File Acquisition, since there is no LUT, Light, or DeviceParams
		if( BoardName == _T("File") ) 
		{
			HRESULT Result = LoadFileAcquisitionConfiguration(rTree, hBoardChild, lNumAcqDig);
		}
		else
		{
			SVTreeType::SVBranchHandle hDigChild( nullptr );

			hDigChild = rTree.getFirstBranch( hBoardChild );

			while ( bOk && nullptr != hDigChild )
			{
				++lNumAcqDig;

				bool bFileDone = false;
				bool bLightDone = false;
				bool bLutDone = false;
				bool bLutCreated = false;

				_variant_t Name;
				SVLightReference svLight;
				SVFileNameArrayClass svFileArray;
				SVLut lut;
				SVDeviceParamCollection svDeviceParams;

				SVString DigName = rTree.getBranchName( hDigChild );

				SVTreeType::SVBranchHandle hDataChild( nullptr );

				hDataChild = rTree.getFirstBranch( hDigChild );

				bFileDone = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_ACQUISITION_DEVICE_FILE_NAME, hDigChild, Name );

				if( bFileDone )
				{
					svFileArray.SetFileNameList( SvUl_SF::createSVString( Name.bstrVal ).c_str() );
				}

				if( SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::CTAG_LIGHT_REFERENCE_ARRAY, hDigChild, hDataChild ) )
				{
					_variant_t Value;

					if ( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_SIZE, hDataChild, Value ) )
					{
						int iBands = Value;

						ASSERT( iBands > 0 );

						if ( svLight.Create( iBands ) )
						{
							for ( int i = 0; i < iBands; i++ )
							{
								SVTreeType::SVBranchHandle hBand = nullptr;

								SVString Band = SvUl_SF::Format( SvXml::CTAGF_BAND_X, i );

								if ( SvXml::SVNavigateTree::GetItemBranch( rTree, Band.c_str(), hDataChild, hBand ) )
								{
									if ( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_SIZE, hBand, Value ) )
									{
										int iSize = Value;

										if ( svLight.Band( i ).Create( iSize ) )
										{
											for ( int j = 0; j < iSize; j++ )
											{
												SVTreeType::SVBranchHandle hLight = nullptr;

												SVString LightRef = SvUl_SF::Format( SvXml::CTAGF_LIGHTREFERENCE_X, j );

												if ( SvXml::SVNavigateTree::GetItemBranch( rTree, LightRef.c_str(), hBand, hLight ) )
												{
													if ( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_NAME, hLight,  Value) )
													{
														svLight.Band( i ).Attribute( j ).strName = SvUl_SF::createSVString(Value);
														//Legacy: changed name from Contrast to Gain
														if( SvO::cCameraContrast == svLight.Band( i ).Attribute( j ).strName )
														{
															svLight.Band( i ).Attribute( j ).strName = SvOl::FqnCameraGain;
														}
													}
													if ( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_RESOURCE_ID, hLight, Value ) )
													{
														svLight.Band( i ).Attribute( j ).iIDCaption = Value;
													}
													if ( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_TYPE, hLight, Value ) )
													{
														svLight.Band( i ).Attribute( j ).dwType = Value;
													}
													if ( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_VALUE, hLight, Value ) )
													{
														svLight.Band( i ).Attribute( j ).lValue = Value;
													}

													bLightDone = true;
												}
											}
										}
									}
								}
							}
						}
					}
				}

				if( SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::CTAG_LUT, hDigChild, hDataChild ) )
				{
					_variant_t Value;
					SVLutInfo lutinfo;

					if ( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_MAX_VALUE, hDataChild, Value ) )
					{
						UINT uiMax = Value;

						lutinfo.SetMaxValue( uiMax );
					}

					if ( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_SIZE, hDataChild, Value ) )
					{
						int iBands = Value;

						if ( lutinfo.MaxValue() == 0 )	// legacy analog support
						{
							if ( iBands == 3 )
								lutinfo.SetMaxValue( 1023 );
							else
								lutinfo.SetMaxValue( 255 );
						}

						lutinfo.SetBands( iBands );
						lutinfo.SetTransform(SVDefaultLutTransform());

						bLutCreated = false;

						if ( true ) //SVLut.Create( iBands ) )
						{
							for ( int iBand = 0; iBand < iBands; iBand++ )
							{
								SVTreeType::SVBranchHandle hBand = nullptr;

								SVString Band = SvUl_SF::Format( SvXml::CTAGF_BAND_X, iBand );

								SVLutTransformParameters param;

								if ( SvXml::SVNavigateTree::GetItemBranch( rTree, Band.c_str(), hDataChild, hBand ) )
								{
									SVSAFEARRAY l_BandData;

									if ( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_LUT_BAND_DATA, hBand, Value ) )
									{
										l_BandData = Value;

										int lSize = static_cast< int >( l_BandData.size() );

										if (!bLutCreated)
										{
											lutinfo.SetBandSize(lSize);
											lut.Create(lutinfo);
											bLutCreated = true;
										}
									}

									if ( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_LUT_TRANSFORM_PARAMETERS, hBand, Value ) )
									{
										SVSAFEARRAY l_Param( Value );

										long lSize = static_cast< long >( l_Param.size() );
										// copy safearray to SVLutTransformParameters
										for (long l=0; l < lSize; l++)
										{
											_variant_t lVal;

											l_Param.GetElement( l, lVal );

											param.push_back(lVal);
										}
									}

									if ( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_LUT_TRANSFORM_OPERATION, hBand, Value ) )
									{
										long lTransformOperation = Value;

										const SVLutTransformOperation* pOperation = SVLutTransform::GetEquivalentType((SVLutTransformOperationEnum)lTransformOperation);
										ASSERT(pOperation);
										if (pOperation)
										{
											lut(iBand).SetTransformOperation(*pOperation);
											lut(iBand).Transform(param);
										}
									}

									if( bLutCreated && 0 < l_BandData.size() )
									{
										bool bSetData = lut(iBand).SetBandData( l_BandData );
									}

								}
							}
							bLutDone = true;
						}
					}
				}

				if( SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::CTAG_DEVICE_PARAM_LIST, hDigChild, hDataChild ) )
				{
					LoadDeviceParameters(rTree, hDataChild, svDeviceParams);
				}

				bOk = bFileDone && bLightDone;
				if ( bOk )
				{
					long l_BandCount = 1;

					// get camera format from DeviceParams
					const SVCameraFormatsDeviceParam* pParam = svDeviceParams.Parameter( DeviceParamCameraFormats ).DerivedValue( pParam );
					if ( pParam )
					{
						SVCameraFormat cf;

						cf.ParseAndAssignCameraFormat( pParam->strValue.c_str() );

						// Band number depends on video type...
						switch( cf.m_eImageType )
						{
						case SVImageFormatRGB8888:  // RGB
							{
								l_BandCount = 3;
								break;
							}
						}
					}

					long lNumLRBands = 0;
					long lBand = 0;

					lNumLRBands = svLight.NumBands();    // if (svLight.NumBands() == 1)
				
					do
					{
						SVString DigitizerName;

						SVAcquisitionClassPtr psvDevice;

						if ( 1 < lNumLRBands )
						{
							lNumLRBands = 1;
						}

						//! If the IP address is saved in the configuration then determine which acquisition device has this address
						//! Important when skipping cameras in the camera manager
						SVStringValueDeviceParam* pParam( nullptr );
						svDeviceParams.GetParameter( DeviceParamIPAddress, pParam );
						if( nullptr != pParam )
						{
							DigitizerName = SVDigitizerProcessingClass::Instance().GetReOrderedCamera( pParam->strValue.c_str() );
						}
						//! If no name available then generate it the way it use to be
						if( DigitizerName.empty() && !DigName.empty() )
						{
							int CameraID = atoi( SvUl_SF::Mid( DigName, SVString(_T("Dig_")).length() ).c_str() );
							if( SvOi::cMaximumCameras <= CameraID )
							{
								CameraID -= SvOi::cMaximumCameras;
							}
							DigitizerName = SVDigitizerProcessingClass::Instance().GetReOrderedCamera( CameraID );
						}

						if ( ! bLutDone || ! bLutCreated )
						{
							psvDevice = SVDigitizerProcessingClass::Instance().GetAcquisitionDevice( DigitizerName.c_str() );
							bOk = nullptr != psvDevice;
							ASSERT( bOk );
							if ( bOk )
							{
								psvDevice->GetLut(lut);
								// create default lut
								SVLutInfo lutinfo = lut.Info();
								if ( lutinfo.MaxValue() == 0 )	// legacy analog support
								{
									if ( lutinfo.Bands() == 3 )
										lutinfo.SetMaxValue( 1023 );
									else
										lutinfo.SetMaxValue( 255 );
								}
								lutinfo.SetTransform(SVDefaultLutTransform());
								lutinfo.SetTransformOperation(SVLutTransformOperationNormal());	// as a default
								lut.Create(lutinfo);
								lut.Transform();
							}
						}
						SVDigitizerProcessingClass::Instance().SetDigitizerColor( DigitizerName.c_str(), l_BandCount > 1 );
						psvDevice = SVDigitizerProcessingClass::Instance().GetAcquisitionDevice( DigitizerName.c_str() );
						bOk = nullptr != psvDevice;
						if ( bOk )
						{
							SVImageInfoClass svImageInfo;

							if( SVMSG_SVO_IGNORE_EXCEPTION == psvDevice->LoadFiles( svFileArray ) )
							{
								//This stops loading without any further messages
								SvStl::MessageContainer MsgCont;
								MsgCont.setMessage( SVMSG_SVO_IGNORE_EXCEPTION, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
								throw MsgCont;
							}

							if( 1 < svLight.Band( 0 ).NumAttributes() )
							{
								// This is what old configurations (SEC) set contrast to. 
								// now read the values from the camera files and save as 
								// the configuration values.

								if( svLight.Band( 0 ).Attribute( 1 /*LR_CONTRAST*/ ).lValue == -999999999L )
								{
									psvDevice->LoadLightReference( svLight );
								}// end if
							}

							// set the camera file metadata for each device param
							SVDeviceParamCollection rCamFileParams;
							psvDevice->GetCameraFileParameters( rCamFileParams );

							SVDeviceParamMap::const_iterator iter;
							for (iter = rCamFileParams.mapParameters.begin(); iter != rCamFileParams.mapParameters.end(); ++iter)
							{
								const SVDeviceParamWrapper& wCamFileParam = iter->second;
								if ( wCamFileParam.IsValid() )
								{
									SVDeviceParamWrapper& wParam = svDeviceParams.GetParameter( wCamFileParam->Type() );
									if ( wParam.IsValid() )
									{
										wParam->SetMetadata(wCamFileParam);
									}
								}
							}

							psvDevice->SetDeviceParameters( svDeviceParams );	// needs to come before LR (quick fix for now)
							psvDevice->SetLightReference( svLight );
							psvDevice->SetLut( lut );
							psvDevice->GetImageInfo( &svImageInfo );
							psvDevice->CreateBuffers( svImageInfo, TheSVObserverApp.GetSourceImageDepth() );

							// set the trigger and strobe polarity in the I/O board based on Acq. device params
							// must get from the acq device instead of using svDeviceParams because the
							// device may set defaults in LoadFiles

							SVDeviceParamCollection params;
							psvDevice->GetDeviceParameters(params);

							// trigger
							if ( params.ParameterExists( DeviceParamAcquisitionTriggerEdge ) )
							{
								int iDigNum = psvDevice->DigNumber();
								const SVBoolValueDeviceParam* pParam = params.Parameter(DeviceParamAcquisitionTriggerEdge).DerivedValue(pParam);
								SVIOConfigurationInterfaceClass::Instance().SetCameraTriggerValue(iDigNum, pParam->bValue);
							}

							// strobe
							if ( params.ParameterExists( DeviceParamAcquisitionStrobeEdge ) )
							{
								int iDigNum = psvDevice->DigNumber();
								const SVBoolValueDeviceParam* pParam = params.Parameter(DeviceParamAcquisitionStrobeEdge).DerivedValue(pParam);
								SVIOConfigurationInterfaceClass::Instance().SetCameraStrobeValue(iDigNum, pParam->bValue);
							}
							// Get Combined parameters
							psvDevice->GetDeviceParameters(svDeviceParams);
							bOk = AddAcquisitionDevice( DigitizerName.c_str(), svFileArray, svLight, lut, &svDeviceParams );
						}
					}
					while ( ++lBand < lNumLRBands );
				}
				hDigChild = rTree.getNextBranch( hBoardChild, hDigChild );
			}
		}
		lNumBordDig = __max( lNumBordDig, lNumAcqDig );

		hBoardChild = rTree.getNextBranch( hChild, hBoardChild );
	}

	return bOk;
}


bool  SVConfigurationObject::LoadCameras( SVTreeType&  rTree, long& lNumCameras, bool ConfigurationColor )
{
	SVTreeType::SVBranchHandle hChild( nullptr );
	bool bOk  =  SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::CTAG_CAMERA, nullptr, hChild );
	if( !bOk )
	{
		return false;
	}

	SVTreeType::SVBranchHandle hSubChild( nullptr );
	hSubChild = rTree.getFirstBranch( hChild );

	while ( bOk && nullptr != hSubChild )
	{
		++lNumCameras;

		SVString ItemName = rTree.getBranchName( hSubChild );

		SVVirtualCamera *pCamera = new SVVirtualCamera;
		bOk = nullptr != pCamera;
		if( bOk )
		{
			_variant_t Value;
			long lBandLink = 0;
			SVString DeviceName;
			int CameraID = -1;

			pCamera->SetName( ItemName.c_str() );
			SVTreeType::SVBranchHandle hDataChild( nullptr );

			if( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_ACQUISITION_DEVICE, hSubChild, Value ) )
			{
				_bstr_t l_String( Value );

				//! We no longer need the channel information
				DeviceName = static_cast< LPCTSTR >( l_String );
				SVString::size_type Pos = SVString::npos;
				if( SVString::npos !=  (Pos = DeviceName.find( _T(".Ch_") )) )
				{
					DeviceName = SvUl_SF::Left( DeviceName, Pos );
				}
				if( SVString::npos !=  (Pos = DeviceName.find( _T("Dig_") )) )
				{
					CameraID = atoi( SvUl_SF::Mid( DeviceName, Pos + SVString(_T("Dig_")).length() ).c_str() );
					if( SvOi::cMaximumCameras <= CameraID )
					{
						CameraID -= SvOi::cMaximumCameras;
					}
				}
			}

			//If camera ID is not available then set it to the Device name Digitizer number
			if( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_CAMERA_ID, hSubChild, Value ) )
			{
				pCamera->setCameraID( Value );
			}
			else if( -1 != CameraID )
			{
				pCamera->setCameraID( CameraID );
			}

			if( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_UNIQUE_REFERENCE_ID, hSubChild, Value ) )
			{
				SVGUID l_Guid( Value );

				SVObjectManagerClass::Instance().CloseUniqueObjectID( pCamera );

				pCamera->m_outObjectInfo.m_UniqueObjectID = l_Guid;

				SVObjectManagerClass::Instance().OpenUniqueObjectID( pCamera );
			}

			if( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_BAND_LINK, hSubChild, Value ) )
			{
				pCamera->SetBandLink( Value );
			}

			//If color not available then we need to set it using the global color which comes from the product type
			if( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_COLOR, hSubChild, Value ) )
			{
				pCamera->SetIsColor( Value );
			}
			else
			{
				pCamera->SetIsColor( ConfigurationColor );
			}

			if( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_FILEACQUISITION_MODE, hSubChild, Value ) )
			{
				pCamera->SetFileAcquisitionMode( Value );
			}

			if( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_FILEACQUISITION_IMAGE_FILENAME, hSubChild, Value ) )
			{
				SVString filename = SvUl_SF::createSVString(Value.bstrVal);
				pCamera->SetImageFilename(filename);
			}

			if( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_FILEACQUISITION_IMAGE_DIRECTORYNAME, hSubChild, Value ) )
			{
				SVString dirName = SvUl_SF::createSVString(Value);
				pCamera->SetImageDirectoryName(dirName);
			}

			if( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_FILEACQUISITION_LOADINGMODE, hSubChild, Value ) )
			{
				pCamera->SetFileLoadingMode(Value);
			}

			if( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_FILEACQUISITION_IMAGE_SIZE_EDIT_MODE, hSubChild, Value ) )
			{
				pCamera->SetFileImageSizeEditModeFileBased(Value);
			}

			if( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_FILEACQUISITION_FILE_IMAGE_WIDTH, hSubChild, Value ) )
			{
				pCamera->SetFileImageWidth(Value);
			}

			if( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_FILEACQUISITION_FILE_IMAGE_HEIGHT, hSubChild, Value ) )
			{
				pCamera->SetFileImageHeight(Value);
			}

			bOk = ! DeviceName.empty();

			if ( bOk )
			{
				SVString RemappedDeviceName;
				if( pCamera->IsFileAcquisition() )
				{
					RemappedDeviceName = DeviceName;
				}
				else
				{
					RemappedDeviceName = SVDigitizerProcessingClass::Instance().GetReOrderedCamera( pCamera->getCameraID() );
				}
				bOk = pCamera->Create( RemappedDeviceName.c_str() ) ? true : false;
			}

			if ( bOk )
			{
				bOk = AddCamera( pCamera );
			}

			hSubChild = rTree.getNextBranch( hChild, hSubChild );
		}
	}

	return bOk;
}

bool SVConfigurationObject::LoadTrigger( SVTreeType& rTree )
{
	SVTreeType::SVBranchHandle hChild( nullptr );
	bool bOk  = SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::CTAG_TRIGGER, nullptr, hChild ) ;
	if( !bOk )
	{
		return false;
	}

	SVTreeType::SVBranchHandle hSubChild( nullptr );
	hSubChild = rTree.getFirstBranch( hChild );

	while ( bOk && nullptr != hSubChild )
	{
		SVString ItemName = rTree.getBranchName( hSubChild );

		SvTi::SVTriggerObject* pTrigger = new SvTi::SVTriggerObject;
		pTrigger->SetName( ItemName.c_str() );
		bOk = nullptr != pTrigger;
		if ( bOk )
		{
			_variant_t Value;
			SVString DeviceName;

			if( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_TRIGGER_DEVICE, hSubChild, Value ) )
			{
				DeviceName = SvUl_SF::createSVString( Value );
			}

			if( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_SOFTWARETRIGGER_DEVICE, hSubChild, Value ) )
			{
				pTrigger->SetSoftwareTrigger( Value );
			}

			if( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_SOFTWARETRIGGER_PERIOD, hSubChild, Value ) )
			{
				pTrigger->SetSoftwareTriggerPeriod( Value );
			}

			bOk = !DeviceName.empty();

			if ( bOk )
			{
				SvTh::SVTriggerClass* psvDevice = SvTi::SVTriggerProcessingClass::Instance().GetTrigger( DeviceName.c_str() );

				if ( nullptr != psvDevice )
				{
					bOk = pTrigger->Create( psvDevice );
				}
			}

			if ( bOk )
			{
				bOk = AddTrigger( pTrigger );
			}

			hSubChild = rTree.getNextBranch( hChild, hSubChild );
		}
	}

	return bOk;
}

bool SVConfigurationObject::LoadInspection( SVTreeType& rTree )
{
	SVTreeType::SVBranchHandle hChild( nullptr );
	bool bOk  = SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::CTAG_INSPECTION, nullptr, hChild ) ;
	SVTreeType::SVBranchHandle hSubChild( nullptr );
	if(bOk)
	{
		hSubChild = rTree.getFirstBranch( hChild );
	}

	if(nullptr == hSubChild)
	{
		bOk= false;
	}

	while ( bOk && nullptr != hSubChild )
	{
		SVString ItemName = rTree.getBranchName( hSubChild );

		SVInspectionProcess* pInspection( nullptr );

		SVObjectManagerClass::Instance().ConstructObject( SVInspectionProcessGuid, pInspection );
		if( nullptr == pInspection )
		{
			SvStl::MessageContainer MsgCont;
			MsgCont.setMessage( SVMSG_SVO_80_LOAD_INSPECTION_FAILED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16050_ConstructObjectInspection );
			throw MsgCont;
		}

		_variant_t Value;
		SVString IPName;
		SVString ToolsetName;
		long NewDisableMethod;
		long EnableAuxiliaryExtent;

		pInspection->SetName( ItemName.c_str() );

		pInspection->SetDeviceName( ItemName.c_str() );

		SVTreeType::SVBranchHandle hIPDoc( nullptr );
		SVTreeType::SVBranchHandle hInspectionProcess( nullptr );

		if( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_INSPECTION_FILE_NAME, hSubChild, Value ) )
		{
			IPName = SvUl_SF::createSVString( _bstr_t(Value.bstrVal) );
		}

		SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::CTAG_SVIPDOC, hSubChild, hIPDoc );

		SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::CTAG_INSPECTION_PROCESS, hSubChild, hInspectionProcess );

		if( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_INSPECTION_TOOLSET_IMAGE, hSubChild, Value ) )
		{
			ToolsetName = SvUl_SF::createSVString( _bstr_t(Value.bstrVal) );
		}

		bOk = nullptr != hIPDoc && !IPName.empty();

		if ( bOk )
		{

			SVTreeType::SVBranchHandle hTempIPItem = hSubChild;
			SVTreeType::SVBranchHandle hTempIPObjectItem = hIPDoc;

			if( nullptr != hInspectionProcess )
			{
				hTempIPItem = hInspectionProcess;
				hTempIPObjectItem = hInspectionProcess;
			}

			bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_INSPECTION_NEW_DISABLE_METHOD, hTempIPItem, Value );
			if ( bOk )
			{
				NewDisableMethod = Value.lVal;
			}

			bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_INSPECTION_ENABLE_AUXILIARY_EXTENT, hTempIPItem, Value );
			if ( bOk )
			{
				EnableAuxiliaryExtent = Value.lVal;
			}

			bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_UNIQUE_REFERENCE_ID, hTempIPObjectItem, Value );
			if ( bOk )
			{
				SVGUID ObjectID( Value );

				SVObjectManagerClass::Instance().CloseUniqueObjectID( pInspection );

				pInspection->m_outObjectInfo.m_UniqueObjectID = ObjectID;

				SVObjectManagerClass::Instance().OpenUniqueObjectID( pInspection );
			}
		}

		if ( bOk )
		{
			SVFileNameClass svFileName;

			svFileName.SetFullFileName( IPName.c_str() );

			pInspection->SetToolsetImage( ToolsetName );

			bOk = pInspection->CreateInspection( svFileName.GetFileNameOnly().c_str() );
			if ( bOk )
			{
				pInspection->SetNewDisableMethod( 1 == NewDisableMethod );
				pInspection->SetEnableAuxiliaryExtent( EnableAuxiliaryExtent );

				SVTreeType::SVBranchHandle hDataChild( nullptr );

				if( SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::CTAG_VIEWED_INPUTS, hSubChild, hDataChild ) )
				{
					SVTreeType::SVBranchHandle hViewed( nullptr );

					hViewed = rTree.getFirstBranch( hDataChild );

					while( nullptr != hViewed )
					{	
						SVString Name = rTree.getBranchName( hViewed );

						pInspection->getViewedInputNames().push_back( Name );

						hViewed = rTree.getNextBranch( hDataChild, hViewed );
					}
				}
			}
		}

		//Add invalid inspection to avoid memory leaks. The inspections will be destroyed. 
		if( nullptr != pInspection )
		{
			bOk = AddInspection( pInspection ) && bOk;
		}
		hSubChild = rTree.getNextBranch( hChild, hSubChild );
	}
	if(false == bOk)
	{
		SvStl::MessageContainer MsgCont;
		MsgCont.setMessage( SVMSG_SVO_91_LOAD_INSPECTION_FAILED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16076_CouldNotLoadInspection );
		throw MsgCont;
	
	}
	return bOk;
}

bool SVConfigurationObject::LoadPPQ( SVTreeType& rTree )
{
	SVTreeType::SVBranchHandle hChild( nullptr );
	bool bOk  = SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::CTAG_PPQ, nullptr, hChild );
	if( !bOk )
	{
		return false;
	}

	SVTreeType::SVBranchHandle hSubChild( nullptr );
	hSubChild = rTree.getFirstBranch( hChild );

	while ( bOk && nullptr != hSubChild )
	{
		SVString ItemName = rTree.getBranchName( hSubChild );

		SVPPQObject* pPPQ = new SVPPQObject;
		if( nullptr == pPPQ )
		{
			SvStl::MessageContainer MsgCont;
			MsgCont.setMessage( SVMSG_SVO_79_LOAD_PPQ_FAILED, SvStl::Tid_CreationOfPPQFailed, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16048_ErrrorCreatePPQObject );
			throw MsgCont;
		}
		pPPQ->SetName( ItemName.c_str() );
		SVTreeType::SVBranchHandle hDeviceChild( nullptr );
		SVTreeType::SVBranchHandle hDataChild( nullptr );
		_variant_t Value;
		long lMode = 0;
		long lLength = 0;
		long lDelay = 0;
		long lCount = 0;
		long lPosition = 0;
		bool bMaintainSrcImg = false;
		long lInspectionTimeout = 0;

		// Set the IO lists
		pPPQ->m_pInputList = GetInputObjectList( );
		pPPQ->m_pOutputList = GetOutputObjectList( );

		// Update source to remove SVOVariant
		// Load the Unique Reference ID for the PPQ
		bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_UNIQUE_REFERENCE_ID, hSubChild, Value );

		if( bOk )
		{
			SVGUID ObjectID( Value );

			SVObjectManagerClass::Instance().CloseUniqueObjectID( pPPQ );

			pPPQ->m_outObjectInfo.m_UniqueObjectID = ObjectID;

			SVObjectManagerClass::Instance().OpenUniqueObjectID( pPPQ );
		}// end if

		bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_PPQ_MODE, hSubChild, Value );

		if( bOk )
		{
			lMode = Value;

			pPPQ->SetPPQOutputMode( (SvOi::SVPPQOutputModeEnum)lMode );
		}// end if

		bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_PPQ_LENGTH, hSubChild, Value );

		if( bOk )
		{
			lLength = Value;

			pPPQ->SetPPQLength( lLength );
		}// end if

		bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_PPQ_OUTPUT_RESET_DELAY_TIME, hSubChild, Value );

		if( bOk )
		{
			lDelay = Value;

			pPPQ->SetResetDelay( lDelay );
		}// end if

		bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_PPQ_OUTPUT_DELAY_TIME, hSubChild, Value );

		if( bOk )
		{
			lDelay = Value;

			pPPQ->SetOutputDelay( lDelay );
		}// end if

		bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_PPQ_MAINTAIN_SRC_IMAGE, hSubChild, Value );

		if ( bOk )
		{
			bMaintainSrcImg = Value;

			pPPQ->SetMaintainSourceImages(bMaintainSrcImg);
		}

		bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_PPQ_INSPECTION_TIMEOUT, hSubChild, Value );

		if ( bOk )
		{
			lInspectionTimeout = Value;

			pPPQ->SetInspectionTimeout(lInspectionTimeout);
		}

		bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_PPQ_CONDITIONAL_OUTPUT, hSubChild, Value );
		if ( bOk )	// Conditional Output.
		{
			SVString condition= SvUl_SF::createSVString(Value.bstrVal);
			if (!condition.empty())
			{
				pPPQ->SetConditionalOutputName(condition);
			}
		}
		// Update source to remove SVOVariant
		// PPQ State Variable
		// Load the Unique Reference ID for the PPQ
		bool bTmp = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_PPQ_STATE_UNIQUEID, hSubChild, Value );

		if( bTmp )
		{
			SVGUID ObjectID( Value );

			SVObjectManagerClass::Instance().CloseUniqueObjectID( &pPPQ->m_voOutputState );

			pPPQ->m_voOutputState.m_outObjectInfo.m_UniqueObjectID = ObjectID;

			SVObjectManagerClass::Instance().OpenUniqueObjectID( &pPPQ->m_voOutputState );
		}// end if

		bTmp = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_PPQ_TRIGGER_COUNT_ID, hSubChild, Value );
		if ( bTmp )
		{
			SVGUID l_TriggercountId( Value );

			SVObjectManagerClass::Instance().CloseUniqueObjectID( &pPPQ->m_voTriggerCount );

			pPPQ->m_voTriggerCount.m_outObjectInfo.m_UniqueObjectID = l_TriggercountId;

			SVObjectManagerClass::Instance().OpenUniqueObjectID( &pPPQ->m_voTriggerCount );
		}

		bOk = SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::CTAG_TRIGGER, hSubChild, hDeviceChild );

		if( bOk )
		{
			hDataChild = rTree.getFirstBranch( hDeviceChild );

			while ( bOk && nullptr != hDataChild )
			{
				lCount = GetTriggerCount( );

				SVString DataName = rTree.getBranchName( hDataChild );

				for ( long l = 0; bOk && l < lCount; l++ )
				{
					SvTi::SVTriggerObject* pTrigger = GetTrigger( l );

					bOk = (nullptr != pTrigger);
					if( bOk )
					{
						SVString DeviceName = pTrigger->GetName();

						if( DeviceName == DataName )
						{
							bOk = pPPQ->AttachTrigger( pTrigger );

							break;
						}
					}
				}

				hDataChild = rTree.getNextBranch( hDeviceChild, hDataChild );
			}
		}

		bOk = SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::CTAG_CAMERA, hSubChild, hDeviceChild );
		if( bOk )
		{
			hDataChild = rTree.getFirstBranch( hDeviceChild );

			while ( bOk && nullptr != hDataChild )
			{
				lCount = GetCameraCount( );

				SVString DataName = rTree.getBranchName( hDataChild );

				bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_POSITION, hDataChild, Value );

				if ( bOk )
				{
					lPosition = Value;
				}// end if

				for ( long l = 0; bOk && l < lCount; l++ )
				{
					SVVirtualCamera* pCamera =GetCamera( l );

					bOk = (nullptr != pCamera);
					if ( bOk )
					{
						SVString DeviceName = pCamera->GetName();

						if ( DeviceName == DataName )
						{
							bOk = pPPQ->AttachCamera( pCamera, lPosition );

							break;
						}
					}
				}

				hDataChild = rTree.getNextBranch( hDeviceChild, hDataChild );
			}
		}

		bOk = SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::CTAG_INSPECTION, hSubChild, hDeviceChild );
		if( bOk )
		{
			hDataChild = rTree.getFirstBranch( hDeviceChild );

			while ( bOk && nullptr != hDataChild )
			{
				lCount = GetInspectionCount();

				SVString DataName = rTree.getBranchName( hDataChild );

				for ( long l = 0; bOk && l < lCount; l++ )
				{
					SVInspectionProcess* pInspection = GetInspection( l );

					bOk = (nullptr != pInspection);
					if ( bOk )
					{
						SVString DeviceName = pInspection->GetDeviceName();

						if ( DeviceName == DataName )
						{
							bOk = pPPQ->AttachInspection( pInspection );

							break;
						}
					}
				}

				hDataChild = rTree.getNextBranch( hDeviceChild, hDataChild );
			}
		}

		if( !SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::CTAG_INPUT, hSubChild, hDeviceChild ) )
		{
			
			SvStl::MessageContainer MsgCont;
			MsgCont.setMessage( SVMSG_SVO_79_LOAD_PPQ_FAILED, SvStl::Tid_InputIsMissing, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16049_ErrorMissingInpuTag );
			throw MsgCont;
		}
		
		hDataChild = rTree.getFirstBranch( hDeviceChild );
		while ( bOk && nullptr != hDataChild )
		{
			SVString DataName = rTree.getBranchName( hDataChild );

			SVRemoteInputObject *pRemoteInput = nullptr;
			_variant_t Value;
			long lIndex;
			long lPPQPosition=0;

			if( !SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_IO_TYPE, hDataChild, Value ) )
			{
				
				SvStl::MessageContainer MsgCont;
				MsgCont.setMessage( SVMSG_SVO_79_LOAD_PPQ_FAILED, SvStl::Tid_MsgIOTypeIsMissing, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16053_ErrorMissingIOTypeTag );
				throw MsgCont;
			}
			SVString Type = SvUl_SF::createSVString( Value );


			// This means it is a Digital input
			if( _T("Digital") == Type )
			{								
				if( !SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_ITEM_NAME, hDataChild, Value ))
				{
					
					SvStl::MessageContainer MsgCont;
					MsgCont.setMessage( SVMSG_SVO_79_LOAD_PPQ_FAILED, SvStl::Tid_MsgIONameIsMissing, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16052_ErrorMissingItemNameTag );
					throw MsgCont;
				}


				DataName = SvUl_SF::createSVString( Value.bstrVal );

				if( !SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_PPQ_POSITION, hDataChild, Value ) )
				{
					
					SvStl::MessageContainer MsgCont;
					MsgCont.setMessage(SVMSG_SVO_79_LOAD_PPQ_FAILED,SvStl::Tid_MsgPPQPOsitionIsMissing, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16054_ErrorMissingPPQPOsitionTag);
					throw MsgCont;
				}
				lPPQPosition = Value;


				// Add Digital Input to the PPQ
				AddDigitalInput(pPPQ, DataName, lPPQPosition); 
			}
			// This means it is a Remote input
			else if( _T("Remote") == Type )
			{								
				_variant_t RemoteValue = 0.0;
				if( !SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_ITEM_NAME, hDataChild, Value ) )
				{
					SvStl::MessageContainer MsgCont;
					MsgCont.setMessage( SVMSG_SVO_79_LOAD_PPQ_FAILED, SvStl::Tid_MsgIONameIsMissing, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16059_ErrorMissingItemNameTag );
					throw MsgCont;
				}

				DataName = SvUl_SF::createSVString( Value.bstrVal );
				if( !SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_PPQ_POSITION, hDataChild, Value ) )
				{
					SvStl::MessageContainer MsgCont;
					MsgCont.setMessage( SVMSG_SVO_79_LOAD_PPQ_FAILED, SvStl::Tid_MsgPPQPOsitionIsMissing, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16056_ErrorMissingPPQPOsitionTag );
					throw MsgCont;
				}

				lPPQPosition = Value;

				if( !SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_REMOTE_INDEX, hDataChild, Value ) )
				{
					SvStl::MessageContainer MsgCont;
					MsgCont.setMessage( SVMSG_SVO_79_LOAD_PPQ_FAILED, SvStl::Tid_MsgRemoteIndexIsMissing, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16058_ErrorMissingRemoteIndexTag );
					throw MsgCont;
				}
				lIndex = Value;


				if( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_REMOTE_INITIAL_VALUE, hDataChild, Value ) )
				{
					RemoteValue = Value;
				}

				// Add Remote Inputs to the InputObjectList
				//first check to see if remote input is there, check by name...
				AddRemoteInput(pPPQ, DataName, lPPQPosition, lIndex, RemoteValue);
			}

			hDataChild = rTree.getNextBranch( hDeviceChild, hDataChild );
		}
		

		if ( bOk )
		{
			bOk = pPPQ->Create();

			bOk &= pPPQ->RebuildInputList(HasCameraTrigger(pPPQ));

			bOk &= pPPQ->RebuildOutputList();

			if ( bOk )
			{
				bOk = AddPPQ( pPPQ );
			}
		}
		
		hSubChild = rTree.getNextBranch( hChild, hSubChild );
	}

	return bOk ? true : false;
}

HRESULT SVConfigurationObject::LoadConfiguration(SVTreeType& rTree)
{

	HRESULT	Result( S_OK );

	SVString BoardName;
	long lNumBoardDig(0);
	long lNumCameras(0);
	bool ConfigurationColor( false ); 

	try
	{
		LoadEnviroment(rTree, ConfigurationColor); 
		LoadIO(rTree);
		LoadAcquisitionDevice(rTree, BoardName, lNumBoardDig);
		LoadCameras(rTree,lNumCameras, ConfigurationColor);
		LoadTrigger(rTree);
		LoadInspection(rTree);
		LoadPPQ(rTree);
		// EB 20031203
		// a temp solution
		// the better solution is to have the acqs subscribe and the triggers provide
		HRESULT hrAttach = AttachAcqToTriggers();
	}
	catch (	const SvStl::MessageContainer& rMsgCont )
	{
		DestroyConfiguration();
		throw rMsgCont;
	}
	m_bConfigurationValid = true;

	return Result;
}

HRESULT SVConfigurationObject::ValidateOutputList( )
{
	HRESULT l_Status = S_OK;

	SVStringVector InspectionNames;
	SVStringVector PPQNames;

	long l_iInspections = GetInspectionCount( );

	for( int i = 0 ; i < l_iInspections ; i++ )
	{
		SVInspectionProcess* pInspection = m_arInspectionArray[i];
		if( nullptr != pInspection )
		{
			InspectionNames.push_back( SVString(pInspection->GetName()) );
		}
	}
	for( SVPPQObjectPtrVector::iterator it = m_arPPQArray.begin(); it != m_arPPQArray.end() ; ++it)
	{
		if( nullptr != *it )
		{
			PPQNames.push_back( SVString((*it)->GetName()) );
		}
	}

	l_Status = m_pOutputObjectList->RemoveUnusedOutputs( InspectionNames, PPQNames );

	return l_Status;
}

void SVConfigurationObject::UpgradeConfiguration()
{
	bool ConfigChanged(false);

	SVObjectPtrVector ColorTools;
	SVObjectManagerClass::Instance().getObjectsOfType(std::back_inserter(ColorTools), SVToolObjectType, SVColorToolObjectType);
	SVObjectPtrVector::iterator Iter = ColorTools.begin();
	for (; ColorTools.end() != Iter; ++Iter)
	{
		SVColorToolClass* pColorTool = dynamic_cast<SVColorToolClass*> (*Iter);
		if (pColorTool->isConverted())
		{
			ConfigChanged = true;
		}
	}
	if (ConfigChanged)
	{
		SvStl::MsgTypeEnum  MsgType = SVSVIMStateClass::CheckState(SV_STATE_REMOTE_CMD) ? SvStl::LogOnly : SvStl::LogAndDisplay;
		SvStl::MessageMgrStd Exception(MsgType);
		Exception.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ColorToolExtentsChanged, SvStl::SourceFileParams(StdMessageParams));
	}
	if (ConfigChanged)
	{
		SVSVIMStateClass::AddState(SV_STATE_MODIFIED);
	}
}

HRESULT SVConfigurationObject::LoadAcquisitionDeviceFilename(SVTreeType& rTree, SVTreeType::SVBranchHandle hDig, SVFileNameArrayClass& rFileArray)
{
	HRESULT hr = S_FALSE;

	_variant_t l_Variant;

	if( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_ACQUISITION_DEVICE_FILE_NAME, hDig, l_Variant ) )
	{
		rFileArray.SetFileNameList( SvUl_SF::createSVString( l_Variant ).c_str() );

		hr = S_OK;
	}

	return hr;
}

HRESULT SVConfigurationObject::LoadFileAcquisitionConfiguration(SVTreeType& rTree, SVTreeType::SVBranchHandle hBoardChild, long& lNumAcqDig)
{
	HRESULT hr( S_OK );

	SVTreeType::SVBranchHandle hDigChild( nullptr );

	SVString BoardName = rTree.getBranchName( hBoardChild );
	hDigChild = rTree.getFirstBranch( hBoardChild );

	SVLightReference svLight;
	SVLut lut;

	while ( S_OK == hr && nullptr != hDigChild )
	{
		++lNumAcqDig;

		SVFileNameArrayClass svFileArray;
		SVDeviceParamCollection svDeviceParams;

		SVString DigName = rTree.getBranchName( hDigChild );

		SVTreeType::SVBranchHandle hDataChild( nullptr );

		if( SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::CTAG_ACQUISITION_DEVICE_FILE_NAME, hDigChild, hDataChild ) )
		{
			hr = LoadAcquisitionDeviceFilename(rTree, hDataChild, svFileArray);
		}

		if( SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::CTAG_DEVICE_PARAM_LIST, hDigChild, hDataChild ) )
		{
			LoadDeviceParameters(rTree, hDataChild, svDeviceParams);
		}

		if( S_OK == hr )
		{
			// need to determine Digitizer Number and Channel
			SVString DeviceName;

			const SVDeviceParam* pParam = svDeviceParams.GetParameter(DeviceParamFileAcqImageFormat);
			DeviceName = SvUl_SF::Format( _T("%s.%s"), BoardName.c_str(), DigName.c_str() );

			SVAcquisitionClassPtr psvDevice( SVDigitizerProcessingClass::Instance().GetAcquisitionDevice( DeviceName.c_str() ) );
			if ( nullptr != psvDevice )
			{
				SVImageInfoClass svImageInfo;

				// Set the device parameters for the File Acquisition device (Note: do this before calling LoadFiles)
				psvDevice->SetDeviceParameters( svDeviceParams );	// needs to come before LR (quick fix for now)
				psvDevice->SetLightReference( svLight );
				psvDevice->SetLut( lut );

				// cause the image info to be updated for the File Acquisition device
				if( SVMSG_SVO_IGNORE_EXCEPTION == psvDevice->LoadFiles( svFileArray ) )
				{
					//This stops loading without any further messages
					SvStl::MessageContainer MsgCont;
					MsgCont.setMessage( SVMSG_SVO_IGNORE_EXCEPTION, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
					throw MsgCont;
				}

				psvDevice->GetImageInfo( &svImageInfo );
				psvDevice->CreateBuffers( svImageInfo, TheSVObserverApp.GetSourceImageDepth() );
			}
			if( !( AddAcquisitionDevice( DeviceName.c_str(), svFileArray, svLight, lut, &svDeviceParams ) ) )
			{
				//This should be addressed when the new error handling is done.
				//This is to avoid returning E_FAIL which would not be displayed but just stop loading the configuration any further
				hr = -1798;
			}
		}

		hDigChild = rTree.getNextBranch( hBoardChild, hDigChild );
	}
	return hr;
}

HRESULT SVConfigurationObject::LoadDeviceParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle hDataChild, SVDeviceParamCollection& svDeviceParams)
{
	_variant_t svValue;
	int iParam = 0;
	SVTreeType::SVBranchHandle hParam( nullptr );

	SVString Parameter = SvUl_SF::Format( SvXml::CTAGF_DEVICE_PARAM_X, ++iParam );

	while ( SvXml::SVNavigateTree::GetItemBranch( rTree, Parameter.c_str(), hDataChild, hParam ) )
	{
		if ( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_DEVICE_PARAM_TYPE, hParam, svValue ) )
		{
			long lType = svValue;

			if ( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_DEVICE_PARAM_VALUE, hParam, svValue ) )
			{
				HRESULT hrParam = svDeviceParams.CreateParameter( (SVDeviceParamEnum) lType, svValue );
				SVDeviceParam* pParam = svDeviceParams.GetParameter( (SVDeviceParamEnum) lType );
				LoadDeviceParamSpecial( rTree, hParam, pParam );
			}
		}

		hParam = nullptr;
		Parameter = SvUl_SF::Format( SvXml::CTAGF_DEVICE_PARAM_X, ++iParam );
	}

	return S_OK;
}

HRESULT SVConfigurationObject::LoadDeviceParamSpecial( SVTreeType& rTree, SVTreeType::SVBranchHandle hParent, SVDeviceParam* pParam )
{
	HRESULT hr = S_OK;
	_variant_t svVariant;
	VARIANT vValue;
	::VariantInit( &vValue );
	SVTreeType::SVBranchHandle hChild( nullptr );

	switch ( pParam->Type() )
	{
	case DeviceParamCameraFormats:	
		{
			SVCameraFormatsDeviceParam* pcf = dynamic_cast<SVCameraFormatsDeviceParam*> (pParam);
			int iOption=0;
			SVTreeType::SVBranchHandle hParam( nullptr );
			if ( SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::CTAG_OPTIONS, hParent, hChild ) )
			{
				SVString OptionTag = SvUl_SF::Format( SvXml::CTAGF_OPTION_X, ++iOption );
				while ( SvXml::SVNavigateTree::GetItemBranch( rTree, OptionTag.c_str(), hChild, hParam ) )
				{
					if ( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_NAME, hParam, svVariant ) )
					{
						SVString Name = SvUl_SF::createSVString( svVariant );

						SVCameraFormat& rcf = pcf->options[Name];
						rcf.ParseAndAssignCameraFormat( Name );
						if ( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_VARIABLE_CAMERA_FORMAT, hParam, svVariant ) )
						{
							rcf.m_bVariableROI = svVariant;
						}
						if ( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_TOP, hParam, svVariant ) )
						{
							rcf.m_lVPos = svVariant;
						}
						if ( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_LEFT, hParam, svVariant ) )
						{
							rcf.m_lHPos = svVariant;
						}
						if ( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_CX, hParam, svVariant ) )
						{
							rcf.m_lWidth = svVariant;
						}
						if ( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_CY, hParam, svVariant ) )
						{
							rcf.m_lHeight = svVariant;
						}
					}
					OptionTag = SvUl_SF::Format( SvXml::CTAGF_OPTION_X, ++iOption );
				}
			}
		}
		break;
	}
	return hr;
}

bool SVConfigurationObject::DestroyConfiguration()
{
	bool bOk = true;

	m_bConfigurationValid = false;

	long l = 0;
	long lSize = 0;

	//destroy inspections...
	lSize = m_arInspectionArray.GetSize();
	for ( l = lSize - 1; -1 < l; l-- )
	{
		if ( nullptr != m_arInspectionArray[l] )
		{
			delete m_arInspectionArray[l];
			m_arInspectionArray[l] = nullptr;
		}
	}
	m_arInspectionArray.RemoveAll();

	//destroy camera's
	lSize = m_arCameraArray.GetSize();
	for ( l = lSize - 1; -1 < l; l-- )
	{
		if ( nullptr != m_arCameraArray[l] )
		{
			delete m_arCameraArray[l];
			m_arCameraArray[l] = nullptr;
		}
	}
	m_arCameraArray.RemoveAll();

	SVConfigurationAcquisitionDeviceInfoStruct *pDevice = nullptr;

	SVAcquisitionDeviceMap::iterator pos = mAcquisitionDeviceMap.GetStartPosition();
	while ( pos != mAcquisitionDeviceMap.end() )
	{
		SVString DeviceName = pos->first;
		pDevice = pos->second;

		pos = mAcquisitionDeviceMap.erase( pos );

		if ( nullptr != pDevice )
		{
			SVAcquisitionClassPtr psvDevice;

			delete pDevice;

			psvDevice = SVDigitizerProcessingClass::Instance().GetAcquisitionDevice( DeviceName.c_str() );
			bOk = nullptr != psvDevice;
			if ( bOk )
			{
				psvDevice->DestroyBuffers();
				psvDevice->ResetLightReference();
				psvDevice->UnloadFiles();
			}
		}
	}

	mAcquisitionDeviceMap.RemoveAll();

	//destroy trigger's
	lSize = m_arTriggerArray.GetSize();
	for ( l = lSize - 1; -1 < l; l-- )
	{
		if ( nullptr != m_arTriggerArray[l] )
		{
			delete m_arTriggerArray[l];
			m_arTriggerArray[l] = nullptr;
		}
	}
	m_arTriggerArray.RemoveAll();

	//destroy PPQ's
	lSize = m_arPPQArray.GetSize();
	for ( l = lSize - 1; -1 < l; l-- )
	{
		if ( nullptr != m_arPPQArray[l] )
		{
			delete m_arPPQArray[l];
			m_arPPQArray[l] = nullptr;
		}
	}
	m_arPPQArray.RemoveAll();

	//destroy Input Object List
	if ( nullptr != m_pInputObjectList )
	{
		delete m_pInputObjectList;
		m_pInputObjectList = nullptr;
	}

	//destroy Output Object List    
	if ( nullptr != m_pOutputObjectList	)
	{
		delete m_pOutputObjectList;
		m_pOutputObjectList = nullptr;
	}

	if( nullptr != m_pIOController )
	{
		ClearRemoteMonitorList();
		delete m_pIOController;
		m_pIOController = nullptr;
	}

	//Delete all Global Constants
	RootObject::resetRootChildValue( SvOl::FqnGlobal );

	SetProductType( SVIM_PRODUCT_TYPE_UNKNOWN );

	SVDigitizerProcessingClass::Instance().ClearDevices();

	SVThreadManager::Instance().Clear();

	return bOk;
}

HRESULT SVConfigurationObject::GetChildObject( SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index ) const
{
	HRESULT l_Status = S_OK;

	rpObject = nullptr;

	if( 0 < rNameInfo.m_NameArray.size() )
	{
		if( rNameInfo.m_NameArray[ 0 ] == "RemoteInputs" )
		{
			if(nullptr != m_pInputObjectList)
			{
				rpObject = m_pInputObjectList->GetInput( rNameInfo.GetObjectName( 1 ));
			}
			else
			{
				l_Status = S_FALSE;
				return l_Status;
			}
		}

		if( nullptr == rpObject )
		{
			SVInspectionProcessVector::const_iterator l_InspectIter;

			for( l_InspectIter = m_arInspectionArray.begin(); nullptr == rpObject && l_InspectIter != m_arInspectionArray.end(); ++l_InspectIter )
			{
				SVInspectionProcess* pInspection = ( *l_InspectIter );

				if( nullptr != pInspection )
				{
					if( static_cast<const size_t> (Index) < rNameInfo.m_NameArray.size() && rNameInfo.m_NameArray[ Index ] == pInspection->GetName() )
					{
						l_Status = pInspection->GetChildObject( rpObject, rNameInfo, Index );
					}
				}
			}
		}

		if( nullptr == rpObject )
		{
			if( rNameInfo.m_NameArray[ 0 ].substr( 0, 3 ) == _T( "PPQ" ) )
			{
				SVPPQObjectPtrVector::const_iterator l_PPQIter;

				for( l_PPQIter = m_arPPQArray.begin(); nullptr == rpObject && l_PPQIter != m_arPPQArray.end(); ++l_PPQIter )
				{
					SVPPQObject* pPPQ = ( *l_PPQIter );

					if( nullptr != pPPQ )
					{
						l_Status = pPPQ->GetChildObject( rpObject, rNameInfo, Index );
					}
				}
			}
			else if( rNameInfo.m_NameArray[ 0 ].substr( 0, 6 ) == _T( "Camera" ) )
			{
				SVVirtualCameraPtrVector::const_iterator l_CameraIter;

				for( l_CameraIter = m_arCameraArray.begin(); nullptr == rpObject && l_CameraIter != m_arCameraArray.end(); ++l_CameraIter )
				{
					SVVirtualCamera* pCamera = ( *l_CameraIter );

					if( nullptr != pCamera )
					{
						l_Status = pCamera->GetChildObject( rpObject, rNameInfo, Index );
					}
				}
			}
			else if( rNameInfo.m_NameArray[ 0 ].substr( 0, 7 ) == _T( "Trigger" ) )
			{
				SvTi::SVTriggerObjectPtrVector::const_iterator l_TriggerIter;

				for( l_TriggerIter = m_arTriggerArray.begin(); nullptr == rpObject && l_TriggerIter != m_arTriggerArray.end(); ++l_TriggerIter )
				{
					SvTi::SVTriggerObject* pTrigger = ( *l_TriggerIter );

					if( nullptr != pTrigger )
					{
						l_Status = pTrigger->GetChildObject( rpObject, rNameInfo, Index );
					}
				}
			}
			else
			{
				l_Status = S_FALSE;
			}
		}
	}
	else
	{
		l_Status = S_FALSE;
	}

	return l_Status;
}

void SVConfigurationObject::SaveEnvironment(SvXml::SVObjectXMLWriter& rWriter) const
{
	rWriter.StartElement(SvXml::CTAG_ENVIRONMENT);

	_variant_t svValue = TheSVObserverApp.getCurrentVersion();
	rWriter.WriteAttribute(SvXml::CTAG_VERSION_NUMBER, svValue);
	svValue = GetProductType();
	rWriter.WriteAttribute( SvXml::CTAG_CONFIGURATION_TYPE, svValue );

	RootObject::getRootChildValue( SvOl::FqnEnvironmentImageUpdate, svValue );
	rWriter.WriteAttribute( SvXml::CTAG_IMAGE_DISPLAY_UPDATE, svValue );
	RootObject::getRootChildValue( SvOl::FqnEnvironmentResultUpdate, svValue );
	rWriter.WriteAttribute( SvXml::CTAG_RESULT_DISPLAY_UPDATE, svValue );

	// Thread Manager Enable.
	BOOL bEnable = SVThreadManager::Instance().GetThreadAffinityEnabled();
	svValue = bEnable;
	rWriter.WriteAttribute( SvXml::CTAG_THREAD_MGR_ENABLE, svValue );

	SVThreadManager::ThreadList threads;
	SVThreadManager::Instance().GetThreadInfo(threads, SVThreadAttribute::SVAffinityUser );
	int iCount = 1;
	for( SVThreadManager::ThreadList::const_iterator it = threads.begin() ; it != threads.end(); ++it)
	{
		SVString Branch = SvUl_SF::Format( _T("%s_%d"), SvXml::CTAG_THREAD_SETUP, iCount);
		rWriter.StartElement( Branch.c_str() );
		svValue = it->m_strName.c_str();
		rWriter.WriteAttribute( SvXml::CTAG_THREAD_NAME, svValue );
		svValue = it->m_lAffinity;
		rWriter.WriteAttribute( SvXml::CTAG_THREAD_AFFINITY, svValue );
		rWriter.EndElement();
		iCount++;
	}

	rWriter.EndElement(); //End of Enviroment
}

void SVConfigurationObject::SaveIO(SvXml::SVObjectXMLWriter& rWriter) const
{
	rWriter.StartElement( SvXml::CTAG_IO );

	SVIOEntryHostStructPtrVector ppInList;

	if ( nullptr != m_pInputObjectList )
	{
		m_pInputObjectList->FillInputs( ppInList );
	}

	long lInSize = static_cast< long >( ppInList.size() );
	long lCount = 0;

	for ( long lIn = 0; lIn < lInSize; lIn++ )
	{
		 SVString IOEntry = SvUl_SF::Format( SvXml::CTAGF_IO_ENTRY_X, lCount );

		if( ppInList[lIn]->m_ObjectType == IO_DIGITAL_INPUT )
		{
			rWriter.StartElement( IOEntry.c_str() );

			_variant_t svVariant;

			SVDigitalInputObject* pInput = nullptr;
			pInput = dynamic_cast<SVDigitalInputObject*> (m_pInputObjectList->GetInput( ppInList[lIn]->m_IOId));

			SVString EntryName = pInput->GetName();
			svVariant.SetString( EntryName.c_str() );
			rWriter.WriteAttribute( SvXml::CTAG_IO_ENTRY_NAME, svVariant );
			svVariant.Clear();

			svVariant = pInput->GetChannel();
			rWriter.WriteAttribute( SvXml::CTAG_CHANNEL, svVariant );
			svVariant.Clear();

			svVariant = false;
			rWriter.WriteAttribute( SvXml::CTAG_IS_OUTPUT, svVariant );
			svVariant.Clear(); //Must clear VT_BOOL Type after use

			svVariant = ( pInput->IsInverted() );
			rWriter.WriteAttribute( SvXml::CTAG_IS_INVERTED, svVariant );
			svVariant.Clear(); //Must clear VT_BOOL Type after use

			svVariant = pInput->IsForced();
			rWriter.WriteAttribute( SvXml::CTAG_IS_FORCED, svVariant );
			svVariant.Clear(); //Must clear VT_BOOL Type after use

			svVariant = pInput->GetForcedValue();
			rWriter.WriteAttribute( SvXml::CTAG_FORCED_VALUE, svVariant );
			svVariant.Clear(); //Must clear VT_BOOL Type after use

			rWriter.EndElement();

			lCount++;
		}
	}// end for

	SVIOEntryHostStructPtrVector ppOutList;
	long lOutSize = 0;
	if ( nullptr != m_pOutputObjectList )
	{
		m_pOutputObjectList->FillOutputs( ppOutList );

		lOutSize = static_cast< long >( ppOutList.size() );
	}

	for ( long lOut = 0; lOut < lOutSize; lOut++ )
	{
		SVString IOEntry = SvUl_SF::Format( SvXml::CTAGF_IO_ENTRY_X, lCount );

		if( ppOutList[lOut]->m_ObjectType == IO_DIGITAL_OUTPUT )
		{
			rWriter.StartElement( IOEntry.c_str() );

			_variant_t svVariant;

			SVDigitalOutputObject* pOutput = nullptr;

			pOutput = dynamic_cast<SVDigitalOutputObject*> (m_pOutputObjectList->GetOutput(ppOutList[lOut]->m_IOId));

			svVariant.SetString( pOutput->GetName() );
			rWriter.WriteAttribute( SvXml::CTAG_IO_ENTRY_NAME, svVariant );
			svVariant.Clear();

			svVariant = pOutput->GetChannel();
			rWriter.WriteAttribute( SvXml::CTAG_CHANNEL, svVariant );
			svVariant.Clear();

			svVariant = true;
			rWriter.WriteAttribute( SvXml::CTAG_IS_OUTPUT, svVariant );
			svVariant.Clear(); //Must clear VT_BOOL Type after use

			svVariant = ( pOutput->IsInverted() );
			rWriter.WriteAttribute( SvXml::CTAG_IS_INVERTED, svVariant );
			svVariant.Clear(); //Must clear VT_BOOL Type after use

			svVariant = ( pOutput->IsForced() );
			rWriter.WriteAttribute( SvXml::CTAG_IS_FORCED, svVariant );
			svVariant.Clear(); //Must clear VT_BOOL Type after use

			svVariant = ( pOutput->GetForcedValue() );
			rWriter.WriteAttribute( SvXml::CTAG_FORCED_VALUE, svVariant );
			svVariant.Clear(); //Must clear VT_BOOL Type after use

			svVariant = ( pOutput->IsCombined() );
			rWriter.WriteAttribute( SvXml::CTAG_IS_COMBINED, svVariant );
			svVariant.Clear(); //Must clear VT_BOOL Type after use

			svVariant = pOutput->GetCombinedValue();
			rWriter.WriteAttribute( SvXml::CTAG_IS_COMBINED_ACK, svVariant );
			svVariant.Clear(); //Must clear VT_BOOL Type after use

			rWriter.EndElement();

			lCount++;
		}
	}// end for ( long lOut = 0; lOut < lOutSize; lOut++ )

	_variant_t svVariant = lCount;
	rWriter.WriteAttribute( SvXml::CTAG_NUMBER_OF_IO_ENTRIES, svVariant );


	//***************** Save the IODoc ... *****************************************
	if ( nullptr != m_pIOController )
	{
		rWriter.StartElement( SvXml::CTAG_IODOC );
		m_pIOController->GetParameters( rWriter );
		rWriter.EndElement();
	}

	rWriter.EndElement(); //End of SvXml::CTAG_IO
}

void SVConfigurationObject::SaveAcquisitionDevice(SvXml::SVObjectXMLWriter& rWriter)  const
{
	rWriter.StartElement( SvXml::CTAG_ACQUISITION_DEVICE );

	SVString Name;
	SVFileNameArrayClass *pFiles = nullptr;
	SVLightReference *pLight = nullptr;
	SVLut* pLut = nullptr;
	SVDeviceParamCollection* pDeviceParams = nullptr;

	SVAcquisitionDeviceMap::iterator pos = mAcquisitionDeviceMap.GetStartPosition();
	while ( pos != mAcquisitionDeviceMap.end() )
	{
		GetAcquisitionDeviceNextAssoc( pos, Name, pFiles, pLight, pLut, pDeviceParams );
		SVString Board;
		SVString Digitizer;

		size_t Pos = Name.find( '.' );
		Board = SvUl_SF::Left( Name, Pos );

		if ( !Board.empty() )
		{
			Digitizer = SvUl_SF::Mid( Name,  Pos + 1 );
			if ( ! Digitizer.empty() )
			{
				rWriter.StartElement( Board.c_str() );
				rWriter.StartElement( Digitizer.c_str() );
				if ( nullptr != pFiles )
				{
					SVString Files = pFiles->GetFileNameList();

					_variant_t svVariant;
					svVariant.SetString( Files.c_str() );
					rWriter.WriteAttribute( SvXml::CTAG_ACQUISITION_DEVICE_FILE_NAME, svVariant );
				}

				if ( nullptr != pDeviceParams )
				{
					// test for File Acquisition, since there is no LUT, Light, or DeviceParams per se
					if (Board == _T("File")) 
					{
						SaveFileAcquisitionConfiguration(rWriter, *pDeviceParams);
					}
					else
					{
						if ( nullptr != pLight && nullptr != pLut )
						{
							SaveAcquistionConfiguration(rWriter, *pLight, *pLut, *pDeviceParams );
						}
					}
				}

				rWriter.EndElement(); //csDig
				rWriter.EndElement(); //csBoard
			}
		}
	}// end while ( nullptr != pos )

	rWriter.EndElement(); //End of SvXml::CTAG_ACQUISITION_DEVICE
}

void SVConfigurationObject::SaveAcquistionConfiguration(SvXml::SVObjectXMLWriter& rWriter, const SVLightReference& rLight, const SVLut& rLut, const SVDeviceParamCollection& rDeviceParams) const
{
	rWriter.StartElement( SvXml::CTAG_LIGHT_REFERENCE_ARRAY );

	_variant_t svVariant = rLight.NumBands();
	rWriter.WriteAttribute( SvXml::CTAG_SIZE, svVariant );

	for ( int i = 0; i < rLight.NumBands(); i++)
	{
		SVString Band = SvUl_SF::Format( SvXml::CTAGF_BAND_X, i );
		rWriter.StartElement( Band.c_str() );

		svVariant = rLight.Band( i ).NumAttributes();
		rWriter.WriteAttribute( SvXml::CTAG_SIZE, svVariant );
		svVariant.Clear();

		for ( int j = 0; j < rLight.Band( i ).NumAttributes(); j++ )
		{
			SVString LightRef = SvUl_SF::Format( SvXml::CTAGF_LIGHTREFERENCE_X, j );

			rWriter.StartElement( LightRef.c_str() );

			svVariant.SetString( rLight.Band( i ).Attribute( j ).strName.c_str() );
			rWriter.WriteAttribute( SvXml::CTAG_NAME, svVariant );
			svVariant.Clear();

			svVariant = rLight.Band( i ).Attribute( j ).iIDCaption;
			rWriter.WriteAttribute( SvXml::CTAG_RESOURCE_ID, svVariant );
			svVariant.Clear();

			svVariant = rLight.Band( i ).Attribute( j ).dwType;
			rWriter.WriteAttribute( SvXml::CTAG_TYPE, svVariant );
			svVariant.Clear();

			svVariant = rLight.Band( i ).Attribute( j ).lValue;
			rWriter.WriteAttribute( SvXml::CTAG_VALUE, svVariant );
			svVariant.Clear();
			rWriter.EndElement(); // csRef
		}// end for ( int j = 0; j < rLight.Band( i ).NumAttributes(); j++ )

		rWriter.EndElement(); // csBand
	}// end for ( int i = 0; i < rLight.NumBands(); i++)
	rWriter.EndElement(); // SvXml::CTAG_LIGHT_REFERENCE_ARRAY

	rWriter.StartElement( SvXml::CTAG_LUT );

	svVariant = rLut.Info().Bands();
	rWriter.WriteAttribute( SvXml::CTAG_SIZE, svVariant );
	svVariant.Clear();

	svVariant = rLut.Info().MaxValue();
	rWriter.WriteAttribute( SvXml::CTAG_MAX_VALUE, svVariant );
	svVariant.Clear();

	for (int iBand=0; iBand < static_cast<int>(rLut.Info().Bands()); iBand++)
	{
		SVString Band = SvUl_SF::Format( SvXml::CTAGF_BAND_X, iBand );
		rWriter.StartElement( Band.c_str() );

		SAFEARRAY* psaParam=nullptr;
		bool bGotParam = rLut(iBand).Info().GetTransformParameters(psaParam);
		svVariant = SVSAFEARRAY( psaParam );
		rWriter.WriteAttribute( SvXml::CTAG_LUT_TRANSFORM_PARAMETERS, svVariant );
		svVariant.Clear();

		const SVLutTransformOperation* pOperation = rLut(iBand).GetTransformOperation();
		SVLutTransformOperationEnum e = SVLutTransform::GetEquivalentType(pOperation);
		ASSERT( e != LutTransformTypeUnknown );
		svVariant = (long) e;
		rWriter.WriteAttribute( SvXml::CTAG_LUT_TRANSFORM_OPERATION, svVariant );
		svVariant.Clear();

		SAFEARRAY* psaBandData = nullptr;
		bool bGotData = rLut(iBand).GetBandData(psaBandData);
		svVariant = SVSAFEARRAY( psaBandData );
		rWriter.WriteAttribute( SvXml::CTAG_LUT_BAND_DATA, svVariant );
		svVariant.Clear();
		rWriter.EndElement(); // csBand
	}
	rWriter.EndElement(); // SvXml::CTAG_LUT

	SaveDeviceParameters(rWriter, rDeviceParams);
}

void SVConfigurationObject::SaveCamera(SvXml::SVObjectXMLWriter& rWriter) const
{
	rWriter.StartElement( SvXml::CTAG_CAMERA );

	long lCount = GetCameraCount( );

	for ( long l = 0; l < lCount; l++ )
	{
		SVVirtualCamera* pCamera = GetCamera( l );

		if ( nullptr != pCamera )
		{
			rWriter.StartElement( pCamera->GetName() );

			if ( !pCamera->mpsvDevice.empty() )
			{
				_variant_t svVariant;
				svVariant = pCamera->getCameraID();
				rWriter.WriteAttribute( SvXml::CTAG_CAMERA_ID, svVariant );
				svVariant.Clear();

				//@WARNING [gra][7.40][02.11.2016] We need to add the channel for forward compatibility with version 7.30, can be removed in next version
				SVString AcquisitionName = pCamera->mpsvDevice->DeviceName();
				if( pCamera->IsColor() )
				{
					AcquisitionName += _T(".Ch_All");
				}
				else
				{
					AcquisitionName += _T(".Ch_0");
				}
				svVariant.SetString( AcquisitionName.c_str() );
				rWriter.WriteAttribute( SvXml::CTAG_ACQUISITION_DEVICE, svVariant );
				svVariant.Clear();

				svVariant = pCamera->GetUniqueObjectID().ToVARIANT();
				rWriter.WriteAttribute( SvXml::CTAG_UNIQUE_REFERENCE_ID, svVariant );
				svVariant.Clear();

				svVariant = pCamera->BandLink();
				rWriter.WriteAttribute( SvXml::CTAG_BAND_LINK, svVariant );
				svVariant.Clear();

				svVariant = pCamera->IsColor();
				rWriter.WriteAttribute( SvXml::CTAG_COLOR, svVariant );
				svVariant.Clear();

				svVariant = pCamera->IsFileAcquisition();
				rWriter.WriteAttribute( SvXml::CTAG_FILEACQUISITION_MODE, svVariant );
				svVariant.Clear();

				if (pCamera->IsFileAcquisition())
				{
					svVariant = pCamera->GetImageFilename();
					rWriter.WriteAttribute( SvXml::CTAG_FILEACQUISITION_IMAGE_FILENAME, svVariant );
					svVariant.Clear();

					svVariant = pCamera->GetImageDirectoryName();
					rWriter.WriteAttribute( SvXml::CTAG_FILEACQUISITION_IMAGE_DIRECTORYNAME, svVariant );
					svVariant.Clear();

					svVariant = pCamera->GetFileLoadingMode();
					rWriter.WriteAttribute( SvXml::CTAG_FILEACQUISITION_LOADINGMODE, svVariant );
					svVariant.Clear();

					svVariant = ( pCamera->IsFileImageSizeEditModeFileBased() ? 1 : 0 );
					rWriter.WriteAttribute( SvXml::CTAG_FILEACQUISITION_IMAGE_SIZE_EDIT_MODE, svVariant );
					svVariant.Clear();

					const SIZE& size = pCamera->GetFileImageSize();
					svVariant = size.cx;
					rWriter.WriteAttribute( SvXml::CTAG_FILEACQUISITION_FILE_IMAGE_WIDTH,  svVariant );
					svVariant.Clear();

					svVariant = size.cy;
					rWriter.WriteAttribute( SvXml::CTAG_FILEACQUISITION_FILE_IMAGE_HEIGHT,  svVariant );
					svVariant.Clear();
				}
			}

			rWriter.EndElement(); // pCamera->GetName()
		}
	}// end for ( long l = 0; l < lCount; l++ )

	rWriter.EndElement(); //SvXml::CTAG_CAMERA
}

void SVConfigurationObject::SaveTrigger(SvXml::SVObjectXMLWriter& rWriter) const
{
	rWriter.StartElement( SvXml::CTAG_TRIGGER );

	long lCount = GetTriggerCount( );
	for ( long l = 0; l < lCount; l++ )
	{
		SvTi::SVTriggerObject* pTrigger = GetTrigger( l );

		if ( nullptr != pTrigger )
		{
			rWriter.StartElement( pTrigger->GetName() );

			if ( nullptr != pTrigger->mpsvDevice )
			{
				_variant_t svVariant;
				svVariant.SetString( pTrigger->mpsvDevice->GetDeviceName() );
				rWriter.WriteAttribute( SvXml::CTAG_TRIGGER_DEVICE, svVariant );
			}
			if ( pTrigger->IsSoftwareTrigger())
			{
				_variant_t svVariant = true;
				rWriter.WriteAttribute( SvXml::CTAG_SOFTWARETRIGGER_DEVICE, svVariant );
				svVariant.Clear();

				svVariant = pTrigger->GetSoftwareTriggerPeriod();
				rWriter.WriteAttribute( SvXml::CTAG_SOFTWARETRIGGER_PERIOD, svVariant );
				svVariant.Clear();
			}
			rWriter.EndElement(); //pTrigger->GetName()
		}
	}// end for ( long l = 0; l < lCount; l++ )
	rWriter.EndElement();  // SvXml::CTAG_TRIGGER
}

void SVConfigurationObject::SaveInspection(SvXml::SVObjectXMLWriter& rWriter) const
{
	rWriter.StartElement( SvXml::CTAG_INSPECTION );

	long lCount = GetInspectionCount( );
	for ( long l = 0; l < lCount; l++ )
	{
		SVInspectionProcess* pInspection = GetInspection( l );

		if ( nullptr != pInspection )
		{
			rWriter.StartElement( pInspection->GetName() );

			_variant_t svVariant;
			svVariant.SetString( pInspection->GetName() );
			rWriter.WriteAttribute( SvXml::CTAG_INSPECTION_FILE_NAME, svVariant );
			svVariant.Clear();

			svVariant.SetString( pInspection->GetToolsetImage() );
			rWriter.WriteAttribute( SvXml::CTAG_INSPECTION_TOOLSET_IMAGE, svVariant );
			svVariant.Clear();

			//Inspection Process
			pInspection->Persist(rWriter);

			//SVIPDoc
			SVIPDoc* pDoc = TheSVObserverApp.GetIPDoc(pInspection->GetUniqueObjectID());
			if (pDoc)
			{
				rWriter.StartElement(SvXml::CTAG_SVIPDOC);
				pDoc->GetParameters(rWriter);
				rWriter.EndElement();
			}

			rWriter.EndElement(); //pInspection->GetName()
		}
	}

	rWriter.EndElement(); //SvXml::CTAG_INSPECTION
}

void SVConfigurationObject::SavePPQ(SvXml::SVObjectXMLWriter& rWriter) const
{
	rWriter.StartElement(SvXml::CTAG_PPQ);

	SVString strName;
	SVPPQObject* pPPQ( nullptr );
	long lPPQCount = GetPPQCount();

	for( long lPPQ = 0; lPPQ < lPPQCount; lPPQ++ )
	{
		pPPQ = this->GetPPQ( lPPQ );
		if ( nullptr != pPPQ )
		{
			strName = pPPQ->GetName();
			rWriter.StartElement( strName.c_str() );

			SavePPQ_Attributes(rWriter, *pPPQ);

			SvTi::SVTriggerObject* pTrigger( nullptr );
			pPPQ->GetTrigger( pTrigger );

			if( nullptr != pTrigger )
			{
				rWriter.StartElement( SvXml::CTAG_TRIGGER );
				strName = pTrigger->GetName();
				rWriter.StartElement( strName.c_str() );
				rWriter.EndElement();
				rWriter.EndElement(); //SvXml::CTAG_TRIGGER
			}// end if

			SavePPQ_Cameras(rWriter, *pPPQ);
			SavePPQ_Inspections(rWriter, *pPPQ);

			//Input
			pPPQ->PersistInputs(rWriter);

			rWriter.EndElement(); //pPPQ->GetName()
		}// end if (nullptr != pPPQ);
	}// end for( lPPQ = 0; lPPQ < lPPQCount; lPPQ++ )
	rWriter.EndElement();  // SvXml::CTAG_PPQ
}

void SVConfigurationObject::SavePPQ_Attributes(SvXml::SVObjectXMLWriter& rWriter, const SVPPQObject& rPPQ ) const
{
	SVGUID ObjectGuid = rPPQ.GetUniqueObjectID();
	_variant_t svValue = ObjectGuid.ToVARIANT();
	rWriter.WriteAttribute( SvXml::CTAG_UNIQUE_REFERENCE_ID, svValue );
	svValue.Clear();

	SvOi::SVPPQOutputModeEnum lMode;
	rPPQ.GetPPQOutputMode( lMode );
	svValue = lMode;
	rWriter.WriteAttribute( SvXml::CTAG_PPQ_MODE, svValue );
	svValue.Clear();

	long lLength = 0;
	rPPQ.GetPPQLength( lLength );
	svValue = lLength;
	rWriter.WriteAttribute( SvXml::CTAG_PPQ_LENGTH, svValue );
	svValue.Clear();

	long lDelay = 0;
	rPPQ.GetResetDelay( lDelay );
	svValue = lDelay;
	rWriter.WriteAttribute( SvXml::CTAG_PPQ_OUTPUT_RESET_DELAY_TIME, svValue );
	svValue.Clear();

	rPPQ.GetOutputDelay( lDelay );
	svValue = lDelay;
	rWriter.WriteAttribute( SvXml::CTAG_PPQ_OUTPUT_DELAY_TIME, svValue );
	svValue.Clear();

	bool bMaintainSrcImg = false;
	rPPQ.GetMaintainSourceImages( bMaintainSrcImg );
	svValue = bMaintainSrcImg;
	rWriter.WriteAttribute( SvXml::CTAG_PPQ_MAINTAIN_SRC_IMAGE, svValue );
	svValue.Clear();

	long lInspectionTimeout = 0;
	rPPQ.GetInspectionTimeout( lInspectionTimeout );
	svValue = lInspectionTimeout;
	rWriter.WriteAttribute( SvXml::CTAG_PPQ_INSPECTION_TIMEOUT, svValue );
	svValue.Clear();

	SVGUID l_PPQTriggerCountGUID = rPPQ.m_voTriggerCount.GetUniqueObjectID();
	svValue = l_PPQTriggerCountGUID.ToVARIANT();
	rWriter.WriteAttribute( SvXml::CTAG_PPQ_TRIGGER_COUNT_ID, svValue );
	svValue.Clear();

	// Conditional Output.
	const SVString& l_condition = rPPQ.GetConditionalOutputName();
	svValue.SetString(l_condition.c_str());
	rWriter.WriteAttribute( SvXml::CTAG_PPQ_CONDITIONAL_OUTPUT, svValue );
	svValue.Clear();

	// Save State Objects unique ID
	ObjectGuid = rPPQ.m_voOutputState.GetUniqueObjectID();
	svValue = ObjectGuid.ToVARIANT();
	rWriter.WriteAttribute( SvXml::CTAG_PPQ_STATE_UNIQUEID, svValue );
	svValue.Clear();
}

void SVConfigurationObject::SavePPQ_Cameras(SvXml::SVObjectXMLWriter& rWriter, const SVPPQObject& rPPQ ) const
{
	std::deque< SVVirtualCamera* > l_Cameras;
	rPPQ.GetCameraList( l_Cameras );
	if( 0 < l_Cameras.size() )
	{
		rWriter.StartElement( SvXml::CTAG_CAMERA );

		std::deque< SVVirtualCamera* >::iterator l_Iter = l_Cameras.begin();
		while( l_Iter != l_Cameras.end() )
		{
			if( nullptr != ( *l_Iter ) )
			{
				SVString strName = ( *l_Iter )->GetName();
				rWriter.StartElement( strName.c_str() );

				long l_PPQIndex = -1;
				rPPQ.GetCameraPPQPosition( l_PPQIndex, *l_Iter );

				_variant_t svValue = l_PPQIndex;
				rWriter.WriteAttribute( SvXml::CTAG_POSITION, svValue );
				svValue.Clear();
				rWriter.EndElement();
			}
			++l_Iter;
		}
		rWriter.EndElement(); //SvXml::CTAG_CAMERA
	}
}

void SVConfigurationObject::SavePPQ_Inspections(SvXml::SVObjectXMLWriter& rWriter, const SVPPQObject& rPPQ ) const
{
	long lInspectCount;
	rPPQ.GetInspectionCount( lInspectCount );
	if( lInspectCount )
	{
		rWriter.StartElement( SvXml::CTAG_INSPECTION );
		SVInspectionProcess *pInspection( nullptr );
		for( long lInspect = 0; lInspect < lInspectCount; lInspect++ )
		{
			rPPQ.GetInspection( lInspect, pInspection );
			if( nullptr != pInspection )
			{
				SVString strName = pInspection->GetName();
				rWriter.StartElement( strName.c_str() );
				rWriter.EndElement();
			}// end if
		}// end for( lInspect = 0; lInspect < lInspectCount; lInspect++ )

		rWriter.EndElement(); //SvXml::CTAG_INSPECTION
	}// end if
}

bool SVConfigurationObject::SaveRemoteMonitorList(SvXml::SVObjectXMLWriter& rWriter ) const
{
	bool bOk = true;
	rWriter.StartElement( SvXml::CTAG_MONITOR_LISTS );

	const RemoteMonitorListMap& remoteMonitorLists = GetRemoteMonitorList();
	RemoteMonitorListMap::const_iterator iterMonitorList = remoteMonitorLists.begin();
	while ( bOk && remoteMonitorLists.end() != iterMonitorList )
	{
		const SVString& strName = iterMonitorList->first;
		rWriter.StartElement( strName.c_str() );

		_variant_t svValue;
		const RemoteMonitorNamedList& monitorList = iterMonitorList->second;
		svValue.SetString( monitorList.GetPPQName().c_str() );
		rWriter.WriteAttribute( SvXml::CTAG_PPQ_NAME, svValue );
		svValue.Clear();

		svValue = monitorList.GetRejectDepthQueue();			
		rWriter.WriteAttribute( SvXml::CTAG_REJECT_QUEUE_DEPTH, svValue );
		svValue.Clear();

		svValue = monitorList.IsActive();
		rWriter.WriteAttribute( SvXml::CTAG_IS_ACTIVATED, svValue );
		svValue.Clear();

		bOk = SaveMonitoredObjectList( rWriter, SvXml::CTAG_PRODUCTVALUES_LIST, monitorList.GetProductValuesList() ) && bOk;
		bOk = SaveMonitoredObjectList( rWriter, SvXml::CTAG_PRODUCTIMAGE_LIST, monitorList.GetProductImagesList() ) && bOk;
		bOk = SaveMonitoredObjectList( rWriter, SvXml::CTAG_REJECTCONDITION_LIST, monitorList.GetRejectConditionList() ) && bOk;
		bOk = SaveMonitoredObjectList( rWriter, SvXml::CTAG_FAILSTATUS_LIST, monitorList.GetFailStatusList() ) && bOk;
		rWriter.EndElement(); //strName.c_str()
		iterMonitorList++;
	}

	rWriter.EndElement(); //SvXml::CTAG_MONITOR_LISTS
	return bOk;
}

bool SVConfigurationObject::SaveMonitoredObjectList(SvXml::SVObjectXMLWriter& rWriter, const SVString& listName, const MonitoredObjectList& rList ) const
{
	bool bOk = true;
	rWriter.StartElement( listName.c_str() );

	_variant_t svValue;
	svValue.SetString( _T("") );
	MonitoredObjectList::const_iterator iter = rList.begin();
	while ( bOk && rList.end() != iter )
	{
		const MonitoredObject& rObj = *iter;
		const SVString& objectName = RemoteMonitorListHelper::GetNameFromMonitoredObject(rObj);
		if ( !objectName.empty() )
		{
			rWriter.WriteAttribute( objectName.c_str(), svValue );
		}
		else
		{
			bOk = false;
		}
		iter++;
	}
	rWriter.EndElement(); 
	return bOk;
}

void SVConfigurationObject::SaveGlobalConstants(SvXml::SVObjectXMLWriter &rWriter ) const
{
	rWriter.StartElement( SvXml::CTAG_GLOBAL_CONSTANTS );

	BasicValueObjects::ValueVector GlobalConstantObjects;
	RootObject::getRootChildObjectList( GlobalConstantObjects, SvOl::FqnGlobal, 0 );
	BasicValueObjects::ValueVector::const_iterator Iter( GlobalConstantObjects.cbegin() );
	while ( GlobalConstantObjects.cend() != Iter  && !Iter->empty() )
	{
		SVString Name( (*Iter)->GetCompleteName() );
		rWriter.StartElement( Name.c_str() );

		_variant_t Value;
		(*Iter)->getValue( Value );
		rWriter.WriteAttribute( SvXml::CTAG_VALUE, Value );
		Value.Clear();

		//Save the global constant unique GUID id to have the same id when loading the configuration
		Value = (*Iter)->GetUniqueObjectID().ToBSTR();
		rWriter.WriteAttribute( scUniqueReferenceIDTag, Value );
		Value.Clear();

		SVString Description( (*Iter)->getDescription() );
		//This is needed to remove any CR LF in the description
		SvUl::AddEscapeSpecialCharacters( Description, true );
		Value.SetString( Description.c_str() );
		rWriter.WriteAttribute( SvXml::CTAG_DESCRIPTION, Value );
		Value.Clear();

		rWriter.EndElement();
		++Iter;
	}

	rWriter.EndElement(); //SvXml::CTAG_GLOBAL_CONSTANTS
}

void SVConfigurationObject::ConvertColorToStandardProductType( bool& rConfigColor )
{
	SVIMProductEnum CurrentType( TheSVObserverApp.GetSVIMType() );
	SVIMProductEnum ConfigType( GetProductType() );

	//Need to change the product type from color to standard product type
	bool isColor = SvTi::SVHardwareManifest::IsColorSystem( ConfigType );
	if( isColor )
	{
		rConfigColor = isColor;
		SetProductType( CurrentType );
		SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
	}
}

void SVConfigurationObject::SaveConfiguration(SvXml::SVObjectXMLWriter& rWriter) const
{
	SVString RootName( SvOl::FqnRoot );
	rWriter.WriteRootElement( RootName.c_str() );
	rWriter.WriteSchema();

	DWORD versionNumber = TheSVObserverApp.getCurrentVersion();
	SVString versionString = SvUl_SF::Format("%d.%d", versionNumber >> 16, (versionNumber >> 8) & 0x000000ff);
	rWriter.WriteRevisionHistory(versionString.c_str(), 1);
	rWriter.WriteStartOfBase();

	SaveEnvironment(rWriter);
	SaveIO(rWriter);
	SaveAcquisitionDevice(rWriter);
	SaveCamera(rWriter);
	SaveTrigger(rWriter);
	SaveInspection(rWriter);
	SavePPQ(rWriter);
	SaveRemoteMonitorList(rWriter);
	SaveGlobalConstants(rWriter);

	rWriter.EndElement(); // end of BaseNode
	rWriter.EndElement(); // end of Root Element
}

void SVConfigurationObject::SaveFileAcquisitionConfiguration(SvXml::SVObjectXMLWriter& rWriter, const SVDeviceParamCollection& rDeviceParams) const
{
	// save the camera params
	SaveDeviceParameters(rWriter, rDeviceParams);
}

void SVConfigurationObject::SaveDeviceParameters( SvXml::SVObjectXMLWriter& rWriter, const SVDeviceParamCollection& rDeviceParams ) const
{
	rWriter.StartElement( SvXml::CTAG_DEVICE_PARAM_LIST );

	SVDeviceParamMap::const_iterator iter;
	int i = 0;
	for (iter = rDeviceParams.mapParameters.begin(); iter != rDeviceParams.mapParameters.end(); ++iter)
	{
		const SVDeviceParam* pParam = iter->second;
		if ( nullptr != pParam )
		{
			if ( pParam->Type() != DeviceParamInvalid )
			{
				VARIANT vValue;
				::VariantInit( &vValue );
				HRESULT hrValue = pParam->GetValue( vValue );
				if ( S_OK == hrValue )
				{
					SVString Parameter = SvUl_SF::Format( SvXml::CTAGF_DEVICE_PARAM_X, ++i );
					rWriter.StartElement( Parameter.c_str() );

					_variant_t svVariant;
					svVariant = SVDeviceParam::GetParameterName( pParam->Type() ).c_str();
					rWriter.WriteAttribute( SvXml::CTAG_NAME, svVariant );
					svVariant.Clear();

					svVariant = (long) pParam->Type();
					rWriter.WriteAttribute( SvXml::CTAG_DEVICE_PARAM_TYPE, svVariant );
					svVariant.Clear();

					svVariant = vValue;
					rWriter.WriteAttribute( SvXml::CTAG_DEVICE_PARAM_VALUE, svVariant );
					svVariant.Clear();

					// HACK!!! EB 20031215: This was put in at Jim's request
					// to clarify to someone looking at the config what units the value is stored in.
					svVariant.SetString( _T("native") );
					rWriter.WriteAttribute( SvXml::CTAG_DEVICE_PARAM_UNITS, svVariant );
					svVariant.Clear();

					SaveDeviceParamSpecial( rWriter, pParam );

					rWriter.EndElement();
				}
			}
		}
	}

	rWriter.EndElement(); //SvXml::CTAG_DEVICE_PARAM_LIST
}

void SVConfigurationObject::SaveDeviceParamSpecial( SvXml::SVObjectXMLWriter& rWriter, const SVDeviceParam* pParam ) const
{
	_variant_t svVariant;
	VARIANT vValue;
	::VariantInit( &vValue );

	switch ( pParam->Type() )
	{
	case DeviceParamCameraFormats:	
		{
			const SVCameraFormatsDeviceParam* pcf = dynamic_cast<const SVCameraFormatsDeviceParam*> (pParam);
			SVString Parameter( SvXml::CTAG_OPTIONS );
			rWriter.StartElement( Parameter.c_str() );

			int iOption=0;
			SVCameraFormatsDeviceParam::OptionsType::const_iterator iter;
			for ( iter = pcf->options.begin(); iter != pcf->options.end(); ++iter)
			{
				const SVCameraFormat& rcf = iter->second;
				Parameter = SvUl_SF::Format( SvXml::CTAGF_OPTION_X, ++iOption );
				rWriter.StartElement( Parameter.c_str() );

				svVariant.SetString( rcf.m_strName.c_str() );
				rWriter.WriteAttribute( SvXml::CTAG_NAME, svVariant );
				svVariant.Clear();

				svVariant = rcf.m_bVariableROI;
				rWriter.WriteAttribute( SvXml::CTAG_VARIABLE_CAMERA_FORMAT, svVariant );
				svVariant.Clear();

				svVariant = rcf.m_lHPos;
				rWriter.WriteAttribute( SvXml::CTAG_LEFT, svVariant );
				svVariant.Clear();

				svVariant = rcf.m_lVPos;
				rWriter.WriteAttribute( SvXml::CTAG_TOP, svVariant );
				svVariant.Clear();

				svVariant = rcf.m_lWidth;
				rWriter.WriteAttribute( SvXml::CTAG_CX, svVariant );
				svVariant.Clear();

				svVariant = rcf.m_lHeight;
				rWriter.WriteAttribute( SvXml::CTAG_CY, svVariant );
				svVariant.Clear();

				rWriter.EndElement(); // SvXml::CTAGF_OPTION_X
			}// end for ( iter = pcf->options.begin(); iter != pcf->options.end(); iter++)
			rWriter.EndElement(); // SvXml::CTAG_OPTIONS
		}// end case DeviceParamCameraFormats:	
		break;
	}// end switch ( pParam->Type() )
}

bool SVConfigurationObject::FinishIPDoc( SVInspectionProcess* pInspection )
{
	bool bOk = false;

	if ( nullptr != pInspection )
	{
		SVPPQObject* pPPQ( pInspection->GetPPQ() );

		if( nullptr != pPPQ )
		{
			pPPQ->m_pInputList = GetInputObjectList( );
			pPPQ->RebuildInputList(HasCameraTrigger(pPPQ));

			pPPQ->m_pOutputList = GetOutputObjectList( );
			pPPQ->RebuildOutputList();

			pInspection->RebuildInspectionInputList();

			pInspection->RebuildInspection();

			// Init Document
			pInspection->ValidateAndInitialize( true, false );

			bOk = true;
		}

	}

	return bOk;
}

bool SVConfigurationObject::Activate()
{
	bool bOk = true;

	long lSize = m_arInspectionArray.GetSize();

	for ( long l = lSize - 1; -1 < l; l-- )
	{
		SVInspectionProcess* l_pInspection = m_arInspectionArray[l];

		SvCmd::InspectionRunOncePtr l_CommandPtr = new SvCmd::InspectionRunOnce( l_pInspection->GetUniqueObjectID() );
		SVObjectSynchronousCommandTemplate< SvCmd::InspectionRunOncePtr > l_Command( l_pInspection->GetUniqueObjectID(), l_CommandPtr );

		l_Command.Execute( TWO_MINUTE_CMD_TIMEOUT );
	}

	return bOk;
}

bool SVConfigurationObject::RebuildInputOutputLists()
	// called after Environment is edited
{
	bool bOk = true;

	long l( 0 );

	long lSize = m_arInspectionArray.GetSize();

	for ( l = 0; l < lSize; l++ )
	{
		SVInspectionProcess* pInspection = m_arInspectionArray[l];

		if ( nullptr != pInspection )
		{
			bOk = FinishIPDoc( pInspection ) && bOk;
		}
	}

	if( nullptr != m_pIOController )
	{
		m_pIOController->RebuildOutputList();
	}
	return bOk;
}

SVIMProductEnum SVConfigurationObject::GetProductType() const
{
	return m_eProductType;
}

void SVConfigurationObject::SetProductType( SVIMProductEnum eProductType )
{
	m_eProductType = eProductType;
}

bool SVConfigurationObject::IsConfigurationLoaded() const
{
	return m_bConfigurationValid;
}

void SVConfigurationObject::SetConfigurationLoaded()
{
	m_bConfigurationValid = true;
}

unsigned long SVConfigurationObject::GetFileVersion() const
{
	return m_ulVersion;
}

unsigned long SVConfigurationObject::GetSVXFileVersion(SVTreeType& rTree)
{
	bool bOk = true;
	SVTreeType::SVBranchHandle hChild = nullptr;
	unsigned long ulVersion;

	if ( SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::CTAG_ENVIRONMENT, nullptr, hChild ) )
	{
		_variant_t svValue;
		if ( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_CONFIGURATION_TYPE, hChild, svValue ) )
		{
			int iType( svValue );

			SetProductType( (SVIMProductEnum) iType );
		}

		bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_VERSION_NUMBER, hChild, svValue );

		if( bOk )
		{
			ulVersion = svValue;
		}
	}

	if ( bOk )
	{
		return ulVersion;
	}
	else
	{
		return 0;
	}
}

void SVConfigurationObject::SetupSoftwareTrigger(SvTh::SVSoftwareTriggerClass* pTriggerDevice, int iDigNum, long triggerPeriod, SVPPQObject* pPPQ)
{
	ASSERT( nullptr != pTriggerDevice && nullptr != pPPQ);
	if( nullptr == pTriggerDevice || nullptr == pPPQ )
	{
		return;
	}

	unsigned long triggerHandle;
	pTriggerDevice->m_pDLLTrigger->GetHandle(&triggerHandle, iDigNum);
	VARIANT l_vValue;
	::VariantInit( &l_vValue);
	l_vValue.vt = VT_I4;
	l_vValue.lVal = triggerPeriod;

	// Setup the Triggering device
	pTriggerDevice->m_pDLLTrigger->SetParameterValue(triggerHandle, 0, &l_vValue);

	// get Handles for all cameras associated with this trigger

	// Get The PPQ for this Trigger Object
	if (pPPQ)
	{
		SvTh::SVAcquisitionInitiator  acqInitiator;

		SVVirtualCameraPtrList l_Cameras;
		pPPQ->GetCameraList( l_Cameras );

		for (SVVirtualCameraPtrList ::iterator l_Iter = l_Cameras.begin(); l_Iter != l_Cameras.end(); ++l_Iter)
		{
			SVVirtualCamera* pCamera = ( *l_Iter );
			if ( nullptr != pCamera )
			{
				// remove Trigger Relay, as we are using the Software Trigger
				pCamera->UnregisterTriggerRelay();

				SVAcquisitionClassPtr pAcq = pCamera->GetAcquisitionDevice();
				if( !( pAcq.empty() ) )
				{
					// need the digitizer name here ...
					SVDigitizerLoadLibraryClass* pAcqDLL = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(pAcq->DigName().c_str());
					if (pAcqDLL)
					{
						acqInitiator.Add(pAcqDLL, pAcq->m_hDigitizer);
					}
				}
			}
		}
		pTriggerDevice->RegisterAcquistionInitiator(acqInitiator);
	}
}

void SVConfigurationObject::SetupCameraTrigger(SvTh::SVCameraTriggerClass* pTriggerDevice, int iDigNum, SVPPQObject* pPPQ, bool bSoftwareTrigger, long triggerPeriod)
{
	ASSERT( nullptr != pTriggerDevice && nullptr != pPPQ);
	if( nullptr == pTriggerDevice || nullptr == pPPQ )
	{
		return;
	}

	if (bSoftwareTrigger)
	{
		SVString DeviceName = SvTi::SVHardwareManifest::BuildSoftwareTriggerDeviceName(iDigNum);
		SvTh::SVTriggerClass* psvDevice = SvTi::SVTriggerProcessingClass::Instance().GetTrigger( DeviceName.c_str() );
		if (psvDevice)
		{
			psvDevice->Create();
		}

		SvTh::SVSoftwareTriggerClass* pSoftwareTriggerDevice = dynamic_cast<SvTh::SVSoftwareTriggerClass*>(psvDevice);
		pTriggerDevice->SetSoftwareTriggerDevice(pSoftwareTriggerDevice);

		SetupSoftwareTrigger(pSoftwareTriggerDevice, iDigNum, triggerPeriod, pPPQ);
	}
	else
	{
		if (pTriggerDevice)
		{
			// reassign trigger handle to match camera digitizer handle
			long l_Count = GetCameraCount(); 
			for (long i = 0;i < l_Count;i++)
			{
				SVVirtualCamera* pCamera = GetCamera(i);
				if ( nullptr != pCamera)
				{
					if (!pCamera->mpsvDevice.empty())
					{
						if (iDigNum == pCamera->mpsvDevice->DigNumber())
						{
							pTriggerDevice->m_triggerchannel = pCamera->mpsvDevice->m_hDigitizer;
							break;
						}
					}
				}
			}
		}
	}

	if (pPPQ)
	{
		long depth;
		pPPQ->GetPPQLength(depth);
		depth += pPPQ->GetExtraBufferSize();

		SvTh::SVCameraTriggerData& rCameraTriggerData = pPPQ->GetCameraInputData();
		SVIOEntryHostStructPtr ioEntry = rCameraTriggerData.SetupLineStateInput(depth);
		AddCameraDataInput(pPPQ, ioEntry);

		ioEntry = rCameraTriggerData.SetupTimestampInput(depth);
		AddCameraDataInput(pPPQ, ioEntry);
	}
}

HRESULT SVConfigurationObject::AttachAcqToTriggers()
{
	HRESULT hr = S_OK;

	// For Software Triggers
	// Iterate thru Trigger List and Connect Acquistion Initiator for Software Triggers
	// the channel number for the trigger object must be set at this point, it represents the Digitizer Number
	// Set the Software Timer Trigger period as well here
	long lCount = GetTriggerCount( );

	bool bOk = true;
	for ( long l = 0; bOk && l < lCount; l++ )
	{
		SvTi::SVTriggerObject* pTrigger = GetTrigger( l );
		bOk = ( nullptr != pTrigger );
		if ( bOk )
		{
			if (pTrigger->IsAcquisitionTrigger())
			{
				int iDigNum = pTrigger->mpsvDevice->miChannelNumber;
				SvTh::SVCameraTriggerClass* pTriggerDevice = dynamic_cast<SvTh::SVCameraTriggerClass*>(pTrigger->mpsvDevice);
				if( nullptr != pTriggerDevice )
				{
					SVPPQObject* pPPQ = reinterpret_cast<SVPPQObject*>(pTrigger->m_pOwner);
					if( nullptr != pPPQ){ SetupCameraTrigger(pTriggerDevice, iDigNum, pPPQ, pTrigger->IsSoftwareTrigger(), pTrigger->GetSoftwareTriggerPeriod()); }
				}
			}
			else if (pTrigger->IsSoftwareTrigger())
			{
				int iDigNum = pTrigger->mpsvDevice->miChannelNumber;
				SvTh::SVSoftwareTriggerClass* pTriggerDevice = dynamic_cast<SvTh::SVSoftwareTriggerClass *>(pTrigger->mpsvDevice);

				if( nullptr != pTriggerDevice && nullptr != pTriggerDevice->m_pDLLTrigger )
				{
					SVPPQObject* pPPQ = reinterpret_cast<SVPPQObject*>(pTrigger->m_pOwner);
					if( nullptr != pPPQ){  SetupSoftwareTrigger(pTriggerDevice, iDigNum, pTrigger->GetSoftwareTriggerPeriod(), pPPQ); }
				}
			}
			else
			{
				// need to wire up Trigger Relay for File Acquisition
				// Get all cameras for this trigger
				// Get The PPQ for this Trigger Object
				SVPPQObject* pPPQ = reinterpret_cast<SVPPQObject*>(pTrigger->m_pOwner);
				if( nullptr != pPPQ && nullptr != pTrigger->mpsvDevice && nullptr != pTrigger->mpsvDevice->m_pDLLTrigger )
				{
					// Get The cameras Attached to this PPQ
					SVVirtualCameraPtrList l_Cameras;

					pPPQ->GetCameraList( l_Cameras );

					for (SVVirtualCameraPtrList::iterator l_Iter = l_Cameras.begin(); l_Iter != l_Cameras.end();++l_Iter )
					{
						SVVirtualCamera* pCamera = ( *l_Iter );
						if ( nullptr != pCamera )
						{
							if (pCamera->IsFileAcquisition())
							{
								hr = pCamera->RegisterTriggerRelay(pTrigger->mpsvDevice->m_pDLLTrigger, pTrigger->mpsvDevice->m_triggerchannel);
							}
						}
					}
				}
			}
		}
	}

	return hr;
}

HRESULT SVConfigurationObject::SetModuleReady( bool p_Value )
{
	HRESULT l_Status( S_OK );

	if( nullptr != m_pIOController )
	{
		l_Status = m_pIOController->SetModuleReady( p_Value );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVConfigurationObject::SetRaidErrorBit( bool p_Value )
{
	HRESULT l_Status( S_OK );

	if( nullptr != m_pIOController )
	{
		if ( !m_pIOController->SetRaidErrorBit( p_Value ) )
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

SVIOEntryHostStructPtr SVConfigurationObject::GetModuleReady()
{
	SVIOEntryHostStructPtr l_IOEntryPtr;

	if( nullptr != m_pIOController )
	{
		l_IOEntryPtr = m_pIOController->GetModuleReady();
	}

	return l_IOEntryPtr;
}

SVIOEntryHostStructPtr SVConfigurationObject::GetRaidErrorBit()
{
	SVIOEntryHostStructPtr l_IOEntryPtr;

	if( nullptr != m_pIOController )
	{
		l_IOEntryPtr = m_pIOController->GetRaidErrorBit();
	}

	return l_IOEntryPtr;
}

SVIOController* SVConfigurationObject::GetIOController() const
{
	return m_pIOController;
}

SVGUID SVConfigurationObject::GetIOControllerID() const
{
	SVGUID l_ObjectId;

	if( nullptr != m_pIOController )
	{
		l_ObjectId = m_pIOController->GetUniqueObjectID();
	}

	return l_ObjectId;
}



SVGUID SVConfigurationObject::GetRemoteOutputController() const
{
	SVGUID l_ObjectId;

	if( nullptr != m_pIOController )
	{
		l_ObjectId = m_pIOController->GetRemoteOutputController();
	}

	return l_ObjectId;
}

size_t SVConfigurationObject::GetRemoteOutputGroupCount() const
{
	size_t l_Count = 0;

	if( nullptr != m_pIOController )
	{
		l_Count = m_pIOController->GetRemoteOutputGroupCount();
	}

	return l_Count;
}

void SVConfigurationObject::SetupRemoteOutput()
{
	if( nullptr != m_pIOController )
	{
		m_pIOController->SetupRemoteOutput( this );
	}
}

HRESULT SVConfigurationObject::ClearRemoteOutputUnUsedData()
{
	HRESULT l_Status = S_OK;

	if( nullptr != m_pIOController )
	{
		l_Status = m_pIOController->ClearRemoteOutputUnUsedData();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

SVRemoteOutputGroup* SVConfigurationObject::GetRemoteOutputGroup( const SVString& rRemoteGroupID ) const
{
	SVRemoteOutputGroup* l_pObject = nullptr;

	if( nullptr != m_pIOController )
	{
		l_pObject = m_pIOController->GetRemoteOutputGroup( rRemoteGroupID );
	}

	return l_pObject;
}

HRESULT SVConfigurationObject::GetRemoteOutputGroupNames( SVStringVector& rPPQs ) const
{
	HRESULT l_Status = S_OK;

	if( nullptr != m_pIOController )
	{
		l_Status = m_pIOController->GetRemoteOutputGroupNames( rPPQs );
	}
	else
	{
		rPPQs.clear();

		l_Status = E_FAIL;
	}

	return l_Status;
}

size_t SVConfigurationObject::GetRemoteOutputGroupItemCount( const SVString& rRemoteGroupID ) const
{
	size_t l_Count = 0;

	if( nullptr != m_pIOController )
	{
		l_Count = m_pIOController->GetRemoteOutputGroupItemCount( rRemoteGroupID );
	}

	return l_Count;
}

HRESULT SVConfigurationObject::GetRemoteOutputItem( const SVString& rRemoteGroupID, long l_lIndex, SVRemoteOutputObject*& p_rItem ) const
{
	HRESULT l_Status = S_OK;

	if( nullptr != m_pIOController )
	{
		l_Status = m_pIOController->GetRemoteOutputItem( rRemoteGroupID, l_lIndex, p_rItem );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

SVRemoteOutputObject* SVConfigurationObject::GetFirstRemoteOutputObject( const SVString& rRemoteGroupID ) const
{
	SVRemoteOutputObject* l_pObject = nullptr;

	if( nullptr != m_pIOController )
	{
		l_pObject = m_pIOController->GetFirstRemoteOutputObject( rRemoteGroupID );
	}

	return l_pObject;
}

HRESULT SVConfigurationObject::AddRemoteOutputItem( const SVString& rRemoteGroupID, SVRemoteOutputObject*& p_pNewOutput, GUID p_InputObjectID, const SVString& rPPQ )
{
	HRESULT l_Status = S_OK;

	if( nullptr != m_pIOController )
	{
		l_Status = m_pIOController->AddRemoteOutputItem( rRemoteGroupID, p_pNewOutput, p_InputObjectID, rPPQ );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVConfigurationObject::DeleteRemoteOutput( const SVString& rRemoteGroupID )
{
	HRESULT l_Status = S_OK;

	if( nullptr != m_pIOController )
	{
		l_Status = m_pIOController->DeleteRemoteOutput( rRemoteGroupID );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVConfigurationObject::DeleteRemoteOutputEntry( const SVString& rRemoteGroupID, SVRemoteOutputObject* p_pOutputObject )
{
	HRESULT l_Status = S_OK;

	if( nullptr != m_pIOController )
	{
		l_Status = m_pIOController->DeleteRemoteOutputEntry( rRemoteGroupID, p_pOutputObject );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVConfigurationObject::RemoteOutputValidateInputs()
{
	HRESULT l_Status = S_OK;

	if( nullptr != m_pIOController )
	{
		l_Status = m_pIOController->RemoteOutputValidateInputs();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

bool SVConfigurationObject::RenameOutputListInspectionNames(LPCTSTR OldInspectionName, LPCTSTR NewInspectionName)
{	
	return m_pOutputObjectList->RenameInspection( OldInspectionName, NewInspectionName );
}

HRESULT SVConfigurationObject::GetInspectionItems( const SVNameSet& p_rNames, SVNameStorageResultMap& p_rItems ) const
{
	typedef std::map< SVString, SVInspectionProcess* > SVInspectionMap;
	typedef std::map< SVString, SVCommandInspectionGetItems::SVNameObjectSet > SVInspectionNameItemNameMap;
	typedef SVObjectAsynchronousCommandTemplate< SVCommandInspectionGetItemsPtr > SVAsyncCommand;
	typedef std::deque< SVAsyncCommand > SVAsyncCommandDeque;

	HRESULT l_Status = S_OK;

	p_rItems.clear();

	if( !( p_rNames.empty() ) )
	{
		SVInspectionMap l_Inspections;
		SVInspectionNameItemNameMap l_InspectionItems;

		for( SVNameSet::const_iterator l_Iter = p_rNames.begin(); l_Iter != p_rNames.end(); ++l_Iter )
		{
			SVObjectNameInfo l_Info;

			SVObjectNameInfo::ParseObjectName( l_Info, *l_Iter );

			if( SVString( SvOl::FqnInspections ) == l_Info.m_NameArray[ 0 ] )
			{
				SVObjectReference ObjectRef;
				SVObjectManagerClass::Instance().GetObjectByDottedName( l_Info.GetObjectArrayName( 0 ), ObjectRef );

				if( nullptr != ObjectRef.getObject() )
				{
					SVInspectionProcess* pInspection = nullptr;

					GetInspectionObject( l_Info.GetObjectArrayName( 1 ).c_str(), &pInspection );

					if( nullptr != pInspection )
					{
						l_Inspections[ pInspection->GetName() ] = pInspection;
						SVCommandInspectionGetItems::SVFullNameObjectPair newPair(*l_Iter, ObjectRef);
						l_InspectionItems[ pInspection->GetName() ].insert(newPair);
					}
					else
					{
						p_rItems[ *l_Iter ] = SVStorageResult( SVStorage(), SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST, 0 );

						if( S_OK == l_Status )
						{
							l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
						}
					}
				}
				else
				{
					p_rItems[ *l_Iter ] = SVStorageResult( SVStorage(), SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST, 0 );

					if( S_OK == l_Status )
					{
						l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
					}
				}
			}
			else
			{
				p_rItems[ *l_Iter ] = SVStorageResult( SVStorage(), SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST, 0 );

				if( S_OK == l_Status )
				{
					l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
				}
			}
		}

		SVAsyncCommandDeque l_AsyncCommands;

		for( SVInspectionNameItemNameMap::const_iterator l_InspectionIter = l_InspectionItems.begin(); l_InspectionIter != l_InspectionItems.end(); ++l_InspectionIter )
		{
			SVInspectionMap::iterator l_ProcessIter = l_Inspections.find( l_InspectionIter->first );

			if( l_ProcessIter != l_Inspections.end() && nullptr != l_ProcessIter->second )
			{
				SVCommandInspectionGetItemsPtr l_DataPtr = new SVCommandInspectionGetItems( *l_ProcessIter->second, l_InspectionIter->second );
				SVObjectAsynchronousCommandTemplate< SVCommandInspectionGetItemsPtr > l_Command( *l_ProcessIter->second, l_DataPtr );

				HRESULT l_CommandStatus = l_Command.SubmitCommand();

				if( SUCCEEDED( l_CommandStatus ) )
				{
					l_AsyncCommands.push_back( l_Command );

					if( S_OK == l_Status )
					{
						l_Status = l_CommandStatus;
					}
				}
				else
				{
					l_Status = l_CommandStatus;

					p_rItems.clear();

					break;
				}
			}
			else
			{
				l_Status = E_UNEXPECTED;

				p_rItems.clear();

				break;
			}
		}

		if( SUCCEEDED( l_Status ) )
		{
			SvTl::SVTimeStamp l_EndTimeStamp = SvTl::GetTimeStamp() + SvTl::ConvertFrom( SvTl::Minutes, 10.0 );

			while( !( l_AsyncCommands.empty() ) && ( SvTl::GetTimeStamp() < l_EndTimeStamp ) )
			{
				SVAsyncCommandDeque::iterator l_AsyncIter = l_AsyncCommands.begin();

				while( !( l_AsyncCommands.empty() ) && ( l_AsyncIter != l_AsyncCommands.end() ) )
				{
					HRESULT l_CommandStatus = l_AsyncIter->IsCommandComplete();

					if( SUCCEEDED( l_CommandStatus ) )
					{
						if( S_OK == l_CommandStatus )
						{
							p_rItems.insert( l_AsyncIter->GetCommandPtr()->GetResultItems().begin(), l_AsyncIter->GetCommandPtr()->GetResultItems().end() );

							if( S_OK == l_Status )
							{
								l_Status = l_CommandStatus;
							}

							l_AsyncIter = l_AsyncCommands.erase( l_AsyncIter );
						}
						else
						{
							++l_AsyncIter;
						}
					}
					else
					{
						l_Status = l_CommandStatus;

						p_rItems.clear();

						l_AsyncCommands.clear();
					}
				}

				if( !( l_AsyncCommands.empty() ) )
				{
					::Sleep( 0 );
				}
			}
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

HRESULT SVConfigurationObject::GetRemoteInputItems( const SVNameSet& p_rNames, SVNameStorageResultMap& p_rItems ) const
{
	HRESULT l_Status = S_OK;

	p_rItems.clear();

	if( !( p_rNames.empty() ) )
	{
		for( SVNameSet::const_iterator l_Iter = p_rNames.begin(); l_Iter != p_rNames.end(); ++l_Iter )
		{
			SVObjectNameInfo l_Info;

			SVObjectNameInfo::ParseObjectName( l_Info, *l_Iter );

			if( SVString( SvOl::FqnRemoteInputs ) == l_Info.m_NameArray[ 0 ] )
			{
				SVRemoteInputObject* l_pInput = nullptr;

				SVObjectManagerClass::Instance().GetObjectByDottedName( l_Info.GetObjectArrayName( 0 ), l_pInput );

				if( nullptr != l_pInput )
				{
					_variant_t l_Value;

					HRESULT l_TempStatus = l_pInput->Read( l_Value );

					if( S_OK == l_TempStatus )
					{
						p_rItems[ *l_Iter ] = SVStorageResult( SVStorage( SVVisionProcessor::SVStorageValue, l_Value ), S_OK, 0 );
					}
					else
					{
						p_rItems[ *l_Iter ] = SVStorageResult( SVStorage(), E_INVALIDARG, 0 );

						if( S_OK == l_Status )
						{
							l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
						}
					}
				}
				else
				{
					p_rItems[ *l_Iter ] = SVStorageResult( SVStorage(), SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST, 0 );

					if( S_OK == l_Status )
					{
						l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
					}
				}
			}
			else
			{
				p_rItems[ *l_Iter ] = SVStorageResult( SVStorage(), SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST, 0 );

				if( S_OK == l_Status )
				{
					l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
				}
			}
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

HRESULT SVConfigurationObject::SetInspectionItems( const SVNameStorageMap& p_rItems, SVNameStatusMap& p_rStatus )
{
	typedef std::map< SVString, SVInspectionProcess* > SVInspectionMap;

	HRESULT l_Status = S_OK;

	p_rStatus.clear();

	if( !( p_rItems.empty() ) )
	{
		SVInspectionMap l_ValueInspections;
		SVInspectionMap l_Inspections;

		bool l_Online = SVSVIMStateClass::CheckState( SV_STATE_RUNNING );

		for( SVNameStorageMap::const_iterator l_Iter = p_rItems.begin(); l_Iter != p_rItems.end(); ++l_Iter )
		{
			SVObjectNameInfo l_Info;

			SVObjectNameInfo::ParseObjectName( l_Info, l_Iter->first );

			if( l_Info.m_NameArray.size() >0 &&  SVString( SvOl::FqnInspections ) == l_Info.m_NameArray[ 0 ] )
			{
				SVObjectReference ObjectRef;
				SVObjectManagerClass::Instance().GetObjectByDottedName( l_Info.GetObjectArrayName( 0 ), ObjectRef );

				if( nullptr != ObjectRef.getObject() )
				{
					///someone wants to set this variable check if this is allowed
					bool l_AddParameter = ( ( ObjectRef.ObjectAttributesAllowed() & SvOi::SV_REMOTELY_SETABLE ) == SvOi::SV_REMOTELY_SETABLE );

					if( l_AddParameter )
					{
						l_AddParameter = !l_Online || ( ( ObjectRef.ObjectAttributesAllowed() & SvOi::SV_SETABLE_ONLINE ) == SvOi::SV_SETABLE_ONLINE );

						if( !l_AddParameter )
						{
							p_rStatus[ l_Iter->first ] = SVMSG_OBJECT_CANNOT_BE_SET_WHILE_ONLINE;

							if( S_OK == l_Status )
							{
								l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
							}
						}

						bool rangeParameter = RangeClassHelper::IsOwnedByRangeObject(*ObjectRef.getObject());

						if( l_AddParameter && rangeParameter )
						{
							HRESULT hresult;
							SVString Value;
							if( l_Iter->second.m_Variant.vt == (VT_BSTR | VT_ARRAY) )
							{
								long  index=0;
								BSTR bstrValue = nullptr;
								SafeArrayGetElementNoCopy( l_Iter->second.m_Variant.parray, &index,&bstrValue);
								Value = SvUl_SF::createSVString( bstrValue );
							}

							if (!RangeClassHelper::IsAllowedToSet( *ObjectRef.getObject(), Value, l_Online, hresult))
							{
								p_rStatus[ l_Iter->first ] = hresult;
								l_AddParameter = false;
								if( S_OK == l_Status )
								{
									l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
								}
							}
						}
					}
					else
					{
						p_rStatus[ l_Iter->first ] = SVMSG_OBJECT_CANNOT_BE_SET_REMOTELY;

						if( S_OK == l_Status )
						{
							l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
						}
					}

					if( l_AddParameter )
					{
						SVInspectionProcess* pInspection( nullptr );

						GetInspectionObject( l_Info.GetObjectArrayName( 1 ).c_str(), &pInspection );

						if( nullptr != pInspection )
						{
							l_Inspections[ pInspection->GetName() ] = pInspection;

							if( l_Iter->second.m_StorageType == SVVisionProcessor::SVStorageImageFileName )
							{
								SVImageClass* l_pImage = dynamic_cast< SVImageClass* >( ObjectRef.getObject() );

								if( nullptr != l_pImage )
								{
									p_rStatus[ l_Iter->first ] = pInspection->AddInputImageFileNameRequest( l_pImage, SvUl_SF::createSVString(l_Iter->second.m_Variant) );
								}
								else
								{
									p_rStatus[ l_Iter->first ] = E_INVALIDARG;

									if( S_OK == l_Status )
									{
										l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
									}
								}
							}
							else if( l_Iter->second.m_StorageType == SVVisionProcessor::SVStorageValue )
							{
								if( pInspection->AddInputRequest( ObjectRef, l_Iter->second.m_Variant ) )
								{
									l_ValueInspections[ pInspection->GetName() ] = pInspection;

									p_rStatus[ l_Iter->first ] = S_OK;
								}
								else
								{
									p_rStatus[ l_Iter->first ] = SVMSG_OBJECT_NOT_PROCESSED;

									if( S_OK == l_Status )
									{
										l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
									}
								}
							}
							else
							{
								p_rStatus[ l_Iter->first ] = E_INVALIDARG;

								if( S_OK == l_Status )
								{
									l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
								}
							}
						}
						else
						{
							p_rStatus[ l_Iter->first ] = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;

							if( S_OK == l_Status )
							{
								l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
							}
						}
					}
				}
				else
				{
					p_rStatus[ l_Iter->first ] = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;

					if( S_OK == l_Status )
					{
						l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
					}
				}
			}
			else
			{
				p_rStatus[ l_Iter->first ] = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;

				if( S_OK == l_Status )
				{
					l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
				}
			}
		}

		for( SVInspectionMap::iterator l_ValueInspectionIter = l_ValueInspections.begin(); l_ValueInspectionIter != l_ValueInspections.end(); ++l_ValueInspectionIter )
		{
			SVInspectionProcess* pInspection = l_ValueInspectionIter->second;

			if( nullptr != pInspection )
			{
				pInspection->AddInputRequestMarker();
			}
		}

		if( !l_Online )
		{
			for( SVInspectionMap::iterator l_InspectionIter = l_Inspections.begin(); l_InspectionIter != l_Inspections.end(); ++l_InspectionIter )
			{
				SVInspectionProcess* pInspection = l_InspectionIter->second;

				if( nullptr != pInspection )
				{
					SvCmd::InspectionRunOncePtr l_CommandPtr = new SvCmd::InspectionRunOnce( pInspection->GetUniqueObjectID() );
					SVObjectSynchronousCommandTemplate< SvCmd::InspectionRunOncePtr > l_Command( pInspection->GetUniqueObjectID(), l_CommandPtr );

					l_Command.Execute( TWO_MINUTE_CMD_TIMEOUT );
				}
			}
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

HRESULT SVConfigurationObject::SetRemoteInputItems( const SVNameStorageMap& p_rItems, SVNameStatusMap& p_rStatus )
{
	typedef std::map< SVString, SVInspectionProcess* > SVInspectionMap;

	HRESULT l_Status = S_OK;

	p_rStatus.clear();

	if( !( p_rItems.empty() ) )
	{
		bool l_Online = SVSVIMStateClass::CheckState( SV_STATE_RUNNING );

		for( SVNameStorageMap::const_iterator l_Iter = p_rItems.begin(); l_Iter != p_rItems.end(); ++l_Iter )
		{
			SVObjectNameInfo l_Info;

			SVObjectNameInfo::ParseObjectName( l_Info, l_Iter->first );

			if( SVString( SvOl::FqnRemoteInputs ) == l_Info.m_NameArray[ 0 ] )
			{
				SVRemoteInputObject* l_pInput = nullptr;

				SVObjectManagerClass::Instance().GetObjectByDottedName( l_Iter->first, l_pInput );

				if( nullptr != l_pInput )
				{
					HRESULT l_LoopStatus = l_pInput->WriteCache( l_Iter->second.m_Variant );

					p_rStatus[ l_Iter->first ] = l_LoopStatus;

					if( S_OK == l_Status && S_OK != l_LoopStatus )
					{
						l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
					}
				}
				else
				{
					p_rStatus[ l_Iter->first ] = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;

					if( S_OK == l_Status )
					{
						l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
					}
				}
			}
			else
			{
				p_rStatus[ l_Iter->first ] = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;

				if( S_OK == l_Status )
				{
					l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
				}
			}
		}

		if( !l_Online )
		{
			SVInspectionMap l_ValueInspections;
			SVInspectionMap l_Inspections;

			for( SVNameStorageMap::const_iterator l_Iter = p_rItems.begin(); l_Iter != p_rItems.end(); ++l_Iter )
			{
				SVObjectNameInfo l_Info;

				SVObjectNameInfo::ParseObjectName( l_Info, l_Iter->first );

				if( l_Info.m_NameArray[ 0 ] == "RemoteInputs" )
				{
					SVInspectionSet l_RemoteInspections;

					GetRemoteInputInspections( l_Iter->first, l_RemoteInspections );

					for( SVInspectionSet::iterator l_InspectionIter = l_RemoteInspections.begin(); l_InspectionIter != l_RemoteInspections.end(); ++l_InspectionIter )
					{
						SVInspectionProcess* pInspection = *l_InspectionIter;

						if( nullptr != pInspection )
						{
							l_Inspections[ pInspection->GetName() ] = pInspection;

							SVString l_Name;
							SVObjectReference ObjectRef;

							l_Name += pInspection->GetName();
							l_Name += ".";
							l_Name += l_Info.GetObjectName( 1 );

							SVObjectManagerClass::Instance().GetObjectByDottedName( l_Name, ObjectRef );

							if( nullptr != ObjectRef.getObject() )
							{
								if( pInspection->AddInputRequest( ObjectRef, l_Iter->second.m_Variant ) )
								{
									l_ValueInspections[ pInspection->GetName() ] = pInspection;
								}
							}
						}
					}
				}
			}

			for( SVInspectionMap::iterator l_ValueInspectionIter = l_ValueInspections.begin(); l_ValueInspectionIter != l_ValueInspections.end(); ++l_ValueInspectionIter )
			{
				SVInspectionProcess* pInspection = l_ValueInspectionIter->second;

				if( nullptr != pInspection )
				{
					pInspection->AddInputRequestMarker();
				}
			}

			for( SVInspectionMap::iterator l_InspectionIter = l_Inspections.begin(); l_InspectionIter != l_Inspections.end(); ++l_InspectionIter )
			{
				SVInspectionProcess* pInspection = l_InspectionIter->second;

				if( nullptr != pInspection )
				{
					SvCmd::InspectionRunOncePtr l_CommandPtr = new SvCmd::InspectionRunOnce( pInspection->GetUniqueObjectID() );
					SVObjectSynchronousCommandTemplate< SvCmd::InspectionRunOncePtr > l_Command( pInspection->GetUniqueObjectID(), l_CommandPtr );

					l_Command.Execute( TWO_MINUTE_CMD_TIMEOUT );
				}
			}
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

HRESULT SVConfigurationObject::SetCameraItems( const SVNameStorageMap& rItems, SVNameStatusMap& rStatus )
{
	HRESULT Status = S_OK;

	SVVirtualCameraPtrSet CamerasChanged;

	rStatus.clear();

	if( !( rItems.empty() ) )
	{
		bool Online = SVSVIMStateClass::CheckState( SV_STATE_RUNNING );

		for( SVNameStorageMap::const_iterator Iter = rItems.begin(); Iter != rItems.end(); ++Iter )
		{
			SVObjectNameInfo Info;
			HRESULT LoopStatus = S_OK;

			SVObjectNameInfo::ParseObjectName( Info, Iter->first );

			if( SVString( SvOl::FqnCameras ) == Info.m_NameArray[ 0 ] )
			{
				BasicValueObject* pValueObject = nullptr;

				SVObjectManagerClass::Instance().GetObjectByDottedName( Iter->first, pValueObject );

				if( nullptr != pValueObject )
				{
					bool Attribute = ( ( pValueObject->ObjectAttributesAllowed() & SvOi::SV_REMOTELY_SETABLE ) == SvOi::SV_REMOTELY_SETABLE );

					if( Attribute )
					{
						Attribute = !Online || ( ( pValueObject->ObjectAttributesAllowed() & SvOi::SV_SETABLE_ONLINE ) == SvOi::SV_SETABLE_ONLINE );

						if( Attribute )
						{
							LoopStatus = pValueObject->setValue(Iter->second.m_Variant);

							SVVirtualCamera* pVirtualCamera = dynamic_cast< SVVirtualCamera* > (pValueObject->GetOwner());
							if( nullptr != pVirtualCamera )
							{
								CamerasChanged.insert(pVirtualCamera);
							}
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

	//Check which cameras device parameters have changed
	for( SVVirtualCameraPtrSet::iterator l_Iter = CamerasChanged.begin(); l_Iter != CamerasChanged.end(); ++l_Iter )
	{
		SVDeviceParamCollection CameraParameters;
		HRESULT LoopStatus = (*l_Iter)->updateDeviceParameters(CameraParameters);
		if(S_OK == LoopStatus)
		{
			SVString DeviceName = (*l_Iter)->GetAcquisitionDevice()->DeviceName();
			ModifyAcquisitionDevice(DeviceName.c_str(), &CameraParameters);
			SVLightReference LightRef;
			(*l_Iter)->GetAcquisitionDevice()->GetLightReference(LightRef);
			ModifyAcquisitionDevice(DeviceName.c_str(), LightRef );
		}
	}

	return Status;
}

void SVConfigurationObject::GetRemoteInputInspections( const SVString& p_rRemoteInputName, SVInspectionSet& p_rInspections ) const
{
	p_rInspections.clear();

	for( SVInspectionProcessVector::const_iterator l_Iter = m_arInspectionArray.begin(); l_Iter != m_arInspectionArray.end(); ++l_Iter )
	{
		HRESULT l_LoopStatus = S_OK;
		SVInspectionProcess* l_pInspection = *l_Iter;

		if( nullptr != l_pInspection )
		{
			SVString l_Name;
			SVObjectReference ObjectRef;
			SVObjectNameInfo l_Info;

			SVObjectNameInfo::ParseObjectName( l_Info, p_rRemoteInputName );

			l_Name += l_pInspection->GetName();
			l_Name += ".";
			l_Name += l_Info.GetObjectName( 1 );

			SVObjectManagerClass::Instance().GetObjectByDottedName( l_Name, ObjectRef );

			if( nullptr != ObjectRef.getObject() )
			{
				p_rInspections.insert( l_pInspection );
			}
		}
	}
}

bool SVConfigurationObject::HasCameraTrigger(SVPPQObject* pCameraPPQ) const
{
	bool bRetVal = false;
	long lCount = GetTriggerCount( );

	for ( long i = 0; !bRetVal && i < lCount; i++ )
	{
		SvTi::SVTriggerObject* pTrigger = GetTrigger( i );
		if ( nullptr != pTrigger )
		{
			if (pTrigger->IsAcquisitionTrigger())
			{
				SvTh::SVCameraTriggerClass* pTriggerDevice = dynamic_cast<SvTh::SVCameraTriggerClass*>(pTrigger->mpsvDevice);
				if( nullptr != pTriggerDevice )
				{
					SVPPQObject* pPPQ = reinterpret_cast<SVPPQObject*>(pTrigger->m_pOwner);
					if (pCameraPPQ == pPPQ)
					{
						bRetVal = true;
					}
				}
			}
		}
	}
	return bRetVal;
}

void SVConfigurationObject::updateConfTreeToNewestVersion(SVTreeType &rTree, SVTreeType::SVBranchHandle &rToolset)
{
	DWORD version = TheSVObserverApp.getLoadingVersion();
	if(version < 0x71400) //< 7.20  Add lut Equation  clip 
	{
		SVTreeType::SVBranchHandle lutEquationBranch;

		//look in all children of the toolset if it had to add information
		SVTreeType::SVBranchHandle hSubChild( nullptr );
		hSubChild = rTree.getFirstBranch( rToolset );
		while ( nullptr != hSubChild )
		{
			//check for LUT Equation clip-value
			if ( SvXml::SVNavigateTree::FindBranch( rTree, hSubChild, SVFindPredicate( rTree, IDS_CLASSNAME_SVLUTEQUATION ), lutEquationBranch ) )
			{
				SVTreeType::SVBranchHandle lutEquationClipBranch;
				SVTreeType::SVBranchHandle lutEquationEmbeddedsBranch;
				if ( SvXml::SVNavigateTree::FindBranch( rTree, lutEquationBranch, SVFindPredicate( rTree, scEmbeddedsTag ), lutEquationEmbeddedsBranch ) )
				{
					//check if clip for LUT equation is existing.
					if ( !SvXml::SVNavigateTree::FindBranch( rTree, lutEquationEmbeddedsBranch, SVFindPredicate( rTree, IDS_OBJECTNAME_LUT_EQUATION_CLIP ), lutEquationClipBranch ) )
					{
						//add clip value to tree, with value FALSE
						SVConfigurationTreeWriter< SVTreeType > writer(rTree, lutEquationEmbeddedsBranch);
						SVBoolValueObjectClass isLUTFormulaClipped;
						isLUTFormulaClipped.setResetOptions( false, SvOi::SVResetItemTool );
						SVString Name = SvUl_SF::LoadSVString( IDS_OBJECTNAME_LUT_EQUATION_CLIP );
						isLUTFormulaClipped.SetObjectEmbedded(SVLUTEquationClipFlagObjectGuid, nullptr, Name.c_str() );
						isLUTFormulaClipped.SetDefaultValue(BOOL(true), true);
						isLUTFormulaClipped.SetValue(BOOL(false));
						isLUTFormulaClipped.Persist(writer);
					}
				}
			}

			hSubChild = rTree.getNextBranch( rToolset, hSubChild );
		}
	}
}

void SVConfigurationObject::OnObjectRenamed(const SVObjectClass& rRenamedObject, const SVString& rOldName)
{
	SVOutputObjectList *pOutputs = GetOutputObjectList();

	if( nullptr != pOutputs )
	{
		pOutputs->OnObjectRenamed(rRenamedObject, rOldName);
	}

	long lCount = GetPPQCount();
	for( long lPPQ = 0; lPPQ < lCount; lPPQ++ )
	{
		SVPPQObject* pPPQ = GetPPQ( lPPQ );

		if( nullptr != pPPQ )
		{
			pPPQ->OnObjectRenamed(rRenamedObject, rOldName);
			pPPQ->RebuildOutputList();
		}
	}
}

bool SVConfigurationObject::SetupRemoteMonitorList()
{
	bool bRetVal = false;
	if (nullptr != m_pIOController)
	{
		bRetVal = m_pIOController->SetupRemoteMonitorList(this);
	}
	return bRetVal;
}

void SVConfigurationObject::ClearRemoteMonitorList()
{
	if (nullptr != m_pIOController)
	{
		m_pIOController->ClearRemoteMonitorList();
	}
}

RemoteMonitorListMap SVConfigurationObject::GetRemoteMonitorList() const
{
	if (nullptr != m_pIOController)
	{
		return m_pIOController->GetRemoteMonitorList();
	}
	return RemoteMonitorListMap();
}

void SVConfigurationObject::SetRemoteMonitorList(const RemoteMonitorListMap& rList)
{
	if (nullptr != m_pIOController)
	{
		m_pIOController->SetRemoteMonitorList(rList);
	}
}

void SVConfigurationObject::ReplaceOrAddMonitorList(const RemoteMonitorNamedList& rList)
{
	if (nullptr != m_pIOController)
	{
		m_pIOController->ReplaceOrAddMonitorList(rList);
		SendMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),SV_REGISTER_MONITOR_LIST, 0, 0);
	}
}

void SVConfigurationObject::ValidateRemoteMonitorList()
{
	if (nullptr != m_pIOController)
	{
		m_pIOController->ValidateRemoteMonitorList();
	}
}

HRESULT SVConfigurationObject::ActivateRemoteMonitorList(const SVString& listName, bool bActivate)
{
	HRESULT hr = E_POINTER;
	if (nullptr != m_pIOController)
	{
		hr = m_pIOController->ActivateRemoteMonitorList(listName, bActivate);
	}
	return hr;
}

bool SVConfigurationObject::ActivateDefaultMonitorList()
{
	bool activated(false);
	RemoteMonitorListMap rActiveList ;
	GetActiveRemoteMonitorList( rActiveList);
	if( rActiveList.size() ==0)
	{
		int Size = GetPPQCount();
		for( int i = 0;  i < Size; i++ )
		{
			SVString Name( RemoteMonitorListController::s_DefaultMonitorListName);
			SVPPQObject* pPPQ =  GetPPQ( i);
			if(nullptr == pPPQ)
			{
				break;
			}
			Name += pPPQ->GetName();
			if(S_OK == ActivateRemoteMonitorList( Name,true))
			{
				activated = true;
			}
		}
	}
	return activated;
}

void SVConfigurationObject::GetActiveRemoteMonitorList(RemoteMonitorListMap& rActiveList) const
{
	rActiveList.clear();
	if (nullptr != m_pIOController)
	{
		m_pIOController->GetActiveRemoteMonitorList(rActiveList);
	}
}

int SVConfigurationObject::GetActiveMonitorListCount() const
{
	if (nullptr != m_pIOController)
	{
		return m_pIOController->GetActiveMonitorListCount();
	}

	return 0;
}


HRESULT SVConfigurationObject::GetRemoteMonitorListProductFilter(const SVString& listName, SvSml::SVProductFilterEnum& rFilter) const
{
	HRESULT hr = E_POINTER;
	if (nullptr != m_pIOController)
	{
		hr = m_pIOController->GetRemoteMonitorListProductFilter(listName, rFilter);
	}
	return hr;

}
HRESULT SVConfigurationObject::SetRemoteMonitorListProductFilter(const SVString& listName, SvSml::SVProductFilterEnum filter)
{
	HRESULT hr = E_POINTER;
	if (nullptr != m_pIOController)
	{
		hr = m_pIOController->SetRemoteMonitorListProductFilter(listName, filter);
	}
	return hr;
}

void SVConfigurationObject::BuildPPQMonitorList(PPQMonitorList& ppqMonitorList)  const
{
	ppqMonitorList.clear();
	if (nullptr != m_pIOController)
	{
		m_pIOController->BuildPPQMonitorList(ppqMonitorList);
	}
}

HRESULT SVConfigurationObject::LoadRemoteMonitorList( SVTreeType& rTree )
{
	HRESULT result = S_OK;
	SVTreeType::SVBranchHandle hChild( nullptr );
	if ( SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::CTAG_MONITOR_LISTS, nullptr, hChild ) )
	{
		RemoteMonitorListMap lists;
		SVTreeType::SVBranchHandle hSubChild = nullptr;
		hSubChild = rTree.getFirstBranch( hChild );
		while ( S_OK == result && nullptr != hSubChild )
		{
			SVString ppqName = "";
			int rejectDepth = 0;
			
			SVString listName( rTree.getBranchName( hSubChild ) );
			_variant_t svValue = 0.0;

			if ( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_PPQ_NAME, hSubChild, svValue ) )
			{
				ppqName = static_cast< LPCTSTR >( static_cast< _bstr_t >( svValue ) );
			}
			else
			{
				result = SVMSG_SVO_48_LOAD_CONFIGURATION_MONITOR_LIST;
			}

			if ( S_OK == result )
			{
				if ( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_REJECT_QUEUE_DEPTH, hSubChild, svValue ) )
				{
					rejectDepth = svValue;
				}
				else
				{
					result = SVMSG_SVO_48_LOAD_CONFIGURATION_MONITOR_LIST;
				}
			}
			bool isactivated(false);
			if (  S_OK == result && SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_IS_ACTIVATED, hSubChild, svValue ) )
			{
				isactivated = svValue;
			}

			MonitoredObjectList productValueList;
			if ( S_OK == result )
			{
				result = LoadMonitoredObjectList( rTree, hSubChild, SvXml::CTAG_PRODUCTVALUES_LIST, productValueList );	
			}
			MonitoredObjectList productImageList;
			if ( S_OK == result )
			{
				result = LoadMonitoredObjectList( rTree, hSubChild, SvXml::CTAG_PRODUCTIMAGE_LIST, productImageList );	
			}
			MonitoredObjectList rejectConditionList;
			if ( S_OK == result )
			{
				result = LoadMonitoredObjectList( rTree, hSubChild, SvXml::CTAG_REJECTCONDITION_LIST, rejectConditionList );	
			}
			MonitoredObjectList failStatusList;
			if ( S_OK == result )
			{
				result = LoadMonitoredObjectList( rTree, hSubChild, SvXml::CTAG_FAILSTATUS_LIST, failStatusList );	
			}
			if (S_OK == result)
			{
				RemoteMonitorNamedList monitorList( ppqName, listName, productValueList, productImageList, rejectConditionList, failStatusList, rejectDepth );
				monitorList.Activate(isactivated);
				
				lists[listName] = monitorList;

				hSubChild = rTree.getNextBranch( hChild, hSubChild );
			}
		}
		if ( S_OK == result )
		{
			SetRemoteMonitorList(lists);
		}
	}
	return result;
}

HRESULT SVConfigurationObject::LoadMonitoredObjectList( SVTreeType& rTree, SVTreeType::SVBranchHandle hParent, const SVString& listName, MonitoredObjectList& rList )
{
	HRESULT retValue = S_OK;
	SVTreeType::SVBranchHandle hChild( nullptr );
	// search for the branch by list name
	if ( SvXml::SVNavigateTree::GetItemBranch( rTree, listName.c_str(), hParent, hChild ) )
	{
		SVTreeType::SVLeafHandle hLeaf( rTree.getFirstLeaf( hChild ) );

		while ( S_OK == retValue && rTree.isValidLeaf( hChild, hLeaf ) )
		{
			SVString Name( rTree.getLeafName( hLeaf ) );

			const MonitoredObject& rObj = RemoteMonitorListHelper::GetMonitoredObjectFromName(Name);
			if (!rObj.guid.empty())
			{
				// add object for this leaf to the list
				rList.push_back(rObj);
			}
			else
			{
				SvStl::MsgTypeEnum  MsgType = SVSVIMStateClass::CheckState(SV_STATE_REMOTE_CMD) ? SvStl::LogOnly : SvStl::LogAndDisplay;
				SvStl::MessageMgrStd Exception(MsgType);
				SVStringVector msgList;
				msgList.push_back(Name);
				INT_PTR DlgResult = Exception.setMessage(SVMSG_SVO_106_MONITOR_LIST_OBJECT_MISSING, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams), 0, SV_GUID_NULL, MB_YESNO);
				if (SvStl::LogAndDisplay == MsgType && IDNO == DlgResult)
				{
					rList.clear();
					retValue = SVMSG_SVO_48_LOAD_CONFIGURATION_MONITOR_LIST;
				}
				else
				{
					//Configuration has changed so set it to modified
					SVSVIMStateClass::AddState(SV_STATE_MODIFIED);
				}
			}
			hLeaf = rTree.getNextLeaf(hChild, hLeaf);
		}
	}
	else
	{
		retValue = SVMSG_SVO_48_LOAD_CONFIGURATION_MONITOR_LIST;
	}
	return retValue;
}

HRESULT SVConfigurationObject::LoadGlobalConstants( SVTreeType& rTree )
{
	HRESULT Result = S_OK;
	SVTreeType::SVBranchHandle hBranch( nullptr );
	if ( SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::CTAG_GLOBAL_CONSTANTS, nullptr, hBranch ) )
	{
		SVTreeType::SVBranchHandle hChild( nullptr );
		hChild = rTree.getFirstBranch( hBranch );

		while ( S_OK == Result && nullptr != hChild )
		{
			SVString GlobalConstantName( rTree.getBranchName( hChild ) );

			_variant_t Value;
			SVGUID UniqueID(SV_GUID_NULL);
			SVString Description;

			if ( S_OK == Result )
			{
				if ( SvXml::SVNavigateTree::GetItem( rTree, scUniqueReferenceIDTag, hChild, Value ) )
				{
					UniqueID = Value.bstrVal;
				}
				if ( SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_DESCRIPTION, hChild, Value ) )
				{
					Description = SvUl_SF::createSVString( Value );
					//This is needed to insert any CR LF in the description which were replaced while saving
					SvUl::RemoveEscapedSpecialCharacters( Description, true );
				}
				else
				{
					Result = SVMSG_SVO_63_LOAD_GLOBAL_CONSTANTS;
				}

				Value.Clear();
				if ( !SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_VALUE, hChild, Value ) )
				{
					Result = SVMSG_SVO_63_LOAD_GLOBAL_CONSTANTS;
				}

				if( S_OK == Result )
				{
					BasicValueObjectPtr pValue( nullptr );
					pValue = RootObject::setRootChildValue( GlobalConstantName.c_str(), Value );
					if( pValue.empty() )
					{
						Result = SVMSG_SVO_63_LOAD_GLOBAL_CONSTANTS;
					}
					else
					{
						//Set the unique GUID id for the global constant that was saved
						SVObjectManagerClass::Instance().ChangeUniqueObjectID( pValue.get(), UniqueID );
						pValue->setDescription( Description.c_str() );
						//All Global constants can be remotely settable
						pValue->SetObjectAttributesAllowed( SvOi::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute );
						// If type string then remove Selectable for Equation flag.
						if( VT_BSTR == Value.vt )
						{
							pValue->SetObjectAttributesAllowed( SvOi::SV_SELECTABLE_FOR_EQUATION, SvOi::SetAttributeType::RemoveAttribute );
						}
					}
				}
			}
			hChild = rTree.getNextBranch( hBranch, hChild );
		}
	}
	return Result;
}

/*static*/ bool SVConfigurationObject::GetInspection( LPCTSTR InspectionName, SVInspectionProcess*& prInspection )
{
	bool Result( false );

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if ( nullptr != pConfig )
	{
		if( pConfig->GetInspectionObject(InspectionName, &prInspection) )
		{
			Result = true;
		}
	}

	return Result;
}

