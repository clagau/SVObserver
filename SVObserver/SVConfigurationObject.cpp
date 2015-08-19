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
#include <comdef.h>
#include "SVConfigurationObject.h"

#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVObjectLibrary/SVObjectAsynchronousCommandTemplate.h"
#include "SVObjectLibrary/SVObjectSynchronousCommandTemplate.h"
#include "SVObjectLibrary/SVObjectScriptUsage.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVUtilityLibrary/SVSAFEARRAY.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVOMFCLibrary/SVDeviceParams.h"
#include "SVOMFCLibrary/SVBoolValueDeviceParam.h"
#include "SVOMFCLibrary/StringMunge.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVTriggerLibrary/SVTriggerEnums.h"
#include "SVAcquisitionDevice.h"
#include "SVXMLLibrary/SVNavigateTreeClass.h"
#include "SVObserver.h"
#include "SVInputObjectList.h"
#include "SVOutputObjectList.h"
#include "SVRemoteInputObject.h"
#include "SVCommandInspectionGetItems.h"
#include "SVDigitalInputObject1.h"
#include "SVDigitalOutputObject1.h"
#include "SVMainFrm.h"
#include "SVIODoc.h"
#include "SVIOController.h"
#include "SVImageProcessingClass.h"
#include "SVAcquisitionClass.h"
#include "SVTriggerClass.h"
#include "SVSoftwareTriggerClass.h"
#include "SVToolSet.h"
#include "ObjectInterfaces/SVUserMessage.h"
#include "SVSubscriberProviderInterfaces.h"
#include "SVAcquisitionInitiator.h"
#include "SVTriggerProcessingClass.h"
#include "SVDigitizerProcessingClass.h"
#include "SVCommandInspectionRunOnce.h"
#include "SVConfigurationTreeWriter.h"
#include "SVCameraTriggerClass.h"
#include "SVHardwareManifest.h"
#include "SVCameraDataInputObject.h"
#include "SVGlobal.h"
#include "SVSVIMStateClass.h"
#include "SVStorageResult.h"
#include "SVVirtualCamera.h"
#include "SVObjectLibrary/GlobalConst.h"
#include "RemoteMonitorNamedList.h"
#include "RemoteMonitorListHelper.h"
#include "RootObject.h"
#include "SVSystemLibrary/SVThreadManager.h"
#include "RangeClassHelper.h"
#include "SVObjectLibrary/SVToolsetScriptTags.h"
#include "SVIPDoc.h"
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
	m_pInputObjectList	= NULL;
	m_pOutputObjectList	= NULL;
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
	m_pInputObjectList	= NULL;
	m_pOutputObjectList	= NULL;
	m_bConfigurationValid = false;

	SetProductType( SVIM_PRODUCT_TYPE_UNKNOWN );

	m_arTriggerArray.RemoveAll();
	m_arPPQArray.RemoveAll();
	m_arCameraArray.RemoveAll();
	m_arInspectionArray.RemoveAll();
}

SVConfigurationObject::~SVConfigurationObject()
{
	Destroy();
}
#pragma endregion Constructor

BOOL SVConfigurationObject::Create()
{
	return TRUE;
}// end Create

BOOL SVConfigurationObject::Destroy()
{
	return DestroyConfiguration();
}// end Destroy

BOOL SVConfigurationObject::SetInputObjectList( SVInputObjectList *pInputObjectList )
{
	if( m_pInputObjectList != NULL && m_pInputObjectList != pInputObjectList )
	{
		delete m_pInputObjectList;
	}

	m_pInputObjectList = pInputObjectList;

	return TRUE;
}// end SetInputObjectList

SVInputObjectList* SVConfigurationObject::GetInputObjectList( ) const
{
	return m_pInputObjectList;
}// end GetInputObjectList

BOOL SVConfigurationObject::SetOutputObjectList( SVOutputObjectList *pOutputObjectList )
{
	if( m_pOutputObjectList != NULL && m_pOutputObjectList != pOutputObjectList )
	{
		delete m_pOutputObjectList;
	}

	m_pOutputObjectList = pOutputObjectList;
	return TRUE;
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

BOOL SVConfigurationObject::AddTrigger( SVTriggerObject* pTrigger )
{
	bool Result(false);

	if( nullptr != pTrigger )
	{
		m_arTriggerArray.Add( pTrigger );
		Result = true;
	}
	return Result;
}// end AddTrigger

BOOL SVConfigurationObject::RemoveTrigger( SVTriggerObject* pTrigger )
{
	if( nullptr == pTrigger ) { return FALSE; }

	int iSize;

	iSize = m_arTriggerArray.GetSize();
	for(int i = 0; i < iSize; i++ )
	{
		if( pTrigger == m_arTriggerArray.GetAt( i ) )
		{
			m_arTriggerArray.RemoveAt( i );
			break;
		}// end if
	}// end for
	return TRUE;
}// end RemoveTrigger

long SVConfigurationObject::GetTriggerCount( ) const
{
	return m_arTriggerArray.GetSize();
}// end GetTriggerCount

BOOL SVConfigurationObject::GetChildObjectByName( LPCTSTR tszName, SVTriggerObject** ppTrigger ) const
{
	bool bReturn = false;

	//Only do an assert check so that in release mode no check is made
	ASSERT( nullptr != ppTrigger );

	long lCount = GetTriggerCount( );

	CString sName = tszName;

	for( long l = 0; l < lCount; l++ )
	{ 
		SVTriggerObject* pTrigger = GetTrigger( l );
		if( nullptr != pTrigger && sName == pTrigger->GetCompleteObjectName() )
		{
			*ppTrigger = pTrigger;
			bReturn = true;
			break;
		}
	}

	return bReturn;
}

SVTriggerObject* SVConfigurationObject::GetTrigger( long lIndex ) const
{
	SVTriggerObject* retValue = nullptr;

	ASSERT( 0 <= lIndex && static_cast<long>(m_arTriggerArray.size()) > lIndex );

	if (0 <= lIndex && static_cast<long>(m_arTriggerArray.size()) > lIndex)
	{
		retValue = m_arTriggerArray.GetAt( lIndex );
	}

	return retValue;
}// end GetTrigger

BOOL SVConfigurationObject::AddAcquisitionDevice( LPCTSTR szName, 
																								 SVFileNameArrayClass& rsvFiles,
																								 SVLightReference& rsvLight,
																								 SVLut& rLut,
																								 const SVDeviceParamCollection* pDeviceParams )
{
	BOOL bOk = FALSE;
	SVConfigurationAcquisitionDeviceInfoStruct* pDevice = NULL;

	mAcquisitionDeviceMap.Lookup( szName, pDevice);

	if( !pDevice )
		pDevice = new SVConfigurationAcquisitionDeviceInfoStruct;

	bOk = pDevice != NULL;
	if ( bOk )
	{
		pDevice->msvFiles = rsvFiles;
		pDevice->msvLightArray = rsvLight;
		pDevice->mLut = rLut;
		if ( pDeviceParams != NULL )
			pDevice->mDeviceParams = *pDeviceParams;

		mAcquisitionDeviceMap.SetAt( szName, pDevice );
	}

	return bOk;
}// end AddAcquisitionDevice

BOOL SVConfigurationObject::ModifyAcquisitionDevice( LPCTSTR szName, SVLightReference& rsvLight )
{
	BOOL bOk = FALSE;
	SVConfigurationAcquisitionDeviceInfoStruct* pDevice = NULL;

	mAcquisitionDeviceMap.Lookup( szName, pDevice);

	bOk = pDevice != NULL;
	if ( bOk )
	{
		pDevice->msvLightArray = rsvLight;
	}

	return bOk;
}// end AddAcquisitionDevice

BOOL SVConfigurationObject::ModifyAcquisitionDevice( LPCTSTR szName, const SVLut& lut )
{
	BOOL bOk = FALSE;
	SVConfigurationAcquisitionDeviceInfoStruct* pDevice = NULL;

	mAcquisitionDeviceMap.Lookup( szName, pDevice);

	bOk = pDevice != NULL;
	if ( bOk )
	{
		pDevice->mLut = lut;
	}

	return bOk;
}

BOOL SVConfigurationObject::ModifyAcquisitionDevice( LPCTSTR szName, const SVDeviceParamCollection* pParams )
{
	BOOL bOk = FALSE;
	if ( pParams != NULL )
	{
		SVConfigurationAcquisitionDeviceInfoStruct* pDevice = NULL;

		mAcquisitionDeviceMap.Lookup( szName, pDevice);

		bOk = pDevice != NULL;
		if ( bOk )
		{
			// don't do a straight assignment ( m_DeviceParams = pParams )
			// only copy those parameters that are specified.
			SVDeviceParamMap::const_iterator iter;
			for (iter = pParams->mapParameters.begin(); iter != pParams->mapParameters.end(); ++iter)
			{
				const SVDeviceParamWrapper& w = iter->second;

				if ( ((const SVDeviceParam*) w) != NULL )
				{
					pDevice->mDeviceParams.GetParameter( iter->first ) = w;
				}
			}
		}
	}
	return bOk;

}

BOOL SVConfigurationObject::RemoveAcquisitionDevice( LPCTSTR szName )
{
	BOOL bOk = FALSE;

	SVConfigurationAcquisitionDeviceInfoStruct *pDevice = NULL;

	bOk = mAcquisitionDeviceMap.Lookup( szName, pDevice );
	if ( bOk )
	{
		mAcquisitionDeviceMap.RemoveKey( szName );

		if ( pDevice != NULL )
		{
			delete pDevice;		
		}
	}

	return bOk;
}

BOOL SVConfigurationObject::GetAcquisitionDevice( LPCTSTR szName, 
	SVFileNameArrayClass*& rpFiles,
	SVLightReference*& rpLight,
	SVLut*& rpLut,
	SVDeviceParamCollection*& rpDeviceParams ) const
{
	BOOL bOk = FALSE;

	SVConfigurationAcquisitionDeviceInfoStruct* pDevice = NULL;

	rpFiles = NULL;
	rpLight = NULL;
	rpLut   = NULL;
	rpDeviceParams = NULL;

	bOk = mAcquisitionDeviceMap.Lookup( szName, pDevice );
	if ( bOk )
	{
		bOk = pDevice != NULL;
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

void SVConfigurationObject::GetAcquisitionDeviceNextAssoc( SVAcquisitionDeviceMap::iterator& rNextPosition, 
	CString& rKey ) const
{
	SVConfigurationAcquisitionDeviceInfoStruct* pDevice = NULL;
	mAcquisitionDeviceMap.GetNextAssoc( rNextPosition, rKey, pDevice );
}

void SVConfigurationObject::GetAcquisitionDeviceNextAssoc( SVAcquisitionDeviceMap::iterator& rNextPosition, 
	CString& rKey, 
	SVFileNameArrayClass*& rpFiles,
	SVLightReference*& rpLight,
	SVLut*& rpLut,
	SVDeviceParamCollection*& rpDeviceParams ) const
{
	SVConfigurationAcquisitionDeviceInfoStruct* pDevice = NULL;

	rpFiles = NULL;
	rpLight = NULL;
	rpLut   = NULL;
	rpDeviceParams = NULL;

	mAcquisitionDeviceMap.GetNextAssoc( rNextPosition, rKey, pDevice );

	if ( pDevice != NULL )
	{
		rpFiles = &(pDevice->msvFiles);
		rpLight = &(pDevice->msvLightArray);
		rpLut   = &(pDevice->mLut);
		rpDeviceParams = &(pDevice->mDeviceParams);
	}
}

BOOL SVConfigurationObject::AddPPQ( SVPPQObject* pPPQ )
{
	bool Result( false );

	if( nullptr != pPPQ )
	{
		m_arPPQArray.Add( pPPQ );
		Result = true;
	}

	return Result;
}// end AddPPQ

BOOL SVConfigurationObject::RemovePPQ( SVPPQObject* pPPQ )
{
	if( nullptr == pPPQ ) { return FALSE; }

	int iSize;

	iSize = m_arPPQArray.GetSize();
	for( int i = 0; i < iSize; i++ )
	{
		if(  pPPQ == m_arPPQArray.GetAt( i ) )
		{
			m_arPPQArray.RemoveAt( i );
			break;
		}// end if
	}// end for
	return TRUE;
}// end RemovePPQ

long SVConfigurationObject::GetPPQCount( ) const
{
	return m_arPPQArray.GetSize();
}// end GetPPQCount

BOOL SVConfigurationObject::GetChildObjectByName( LPCTSTR tszName, SVPPQObject** ppPPQ ) const
{
	bool bReturn = false;

	//Only do an assert check so that in release mode no check is made
	ASSERT( nullptr != ppPPQ );

	long lCount = GetPPQCount();

	CString sName = tszName;

	for( long l = 0; l < lCount; l++ )
	{ 
		SVPPQObject* pPPQ = GetPPQ( l );
		if( nullptr != pPPQ && sName == pPPQ->GetCompleteObjectName() )
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

BOOL SVConfigurationObject::AddCamera( SVVirtualCamera* pCamera )
{
	bool Result( false );

	if( nullptr != pCamera )
	{
		m_arCameraArray.Add( pCamera );
		Result = true;
	}
	return Result;
}// end AddCamera

BOOL SVConfigurationObject::RemoveCamera( SVVirtualCamera* pCamera )
{
	if( nullptr == pCamera ) { return FALSE; }

	int iSize;

	iSize = m_arCameraArray.GetSize();
	for( int i = 0; i < iSize; i++ )
	{
		if(  pCamera == m_arCameraArray.GetAt( i ) )
		{
			m_arCameraArray.RemoveAt( i );
			break;
		}// end if
	}// end for
	return TRUE;
}// end RemoveCamera

long SVConfigurationObject::GetCameraCount( ) const
{
	return m_arCameraArray.GetSize();
}// end GetCameraCount

BOOL SVConfigurationObject::GetChildObjectByName( LPCTSTR tszName, SVVirtualCamera** ppCamera ) const
{
	bool bReturn = false;

	//Only do an assert check so that in release mode no check is made
	ASSERT( nullptr != ppCamera );

	long lCount = GetCameraCount( );
	CString sName = tszName;

	for( long l = 0; l < lCount; l++ )
	{ 
		SVVirtualCamera* pCamera = GetCamera( l );
		if( nullptr != pCamera && sName == pCamera->GetCompleteObjectName() )
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

BOOL SVConfigurationObject::AddInspection( SVInspectionProcess* pInspection )
{
	bool Result( false );

	if( nullptr != pInspection )
	{
		m_arInspectionArray.Add( pInspection );
		Result = true;
	}

	return Result;
}// end AddInspection

BOOL SVConfigurationObject::RemoveInspection( SVInspectionProcess* pInspection )
{
	if( nullptr == pInspection ) { return FALSE; }

	int i;
	int iSize;

	iSize = m_arInspectionArray.GetSize();
	for( i = 0; i < iSize; i++ )
	{
		if(  pInspection == m_arInspectionArray.GetAt( i ) )
		{
			m_arInspectionArray.RemoveAt( i );
			break;
		}// end if
	}// end for
	return TRUE;
}// end RemoveInspection

long SVConfigurationObject::GetInspectionCount( ) const
{
	return m_arInspectionArray.GetSize();
}// end GetInspectionCount

BOOL SVConfigurationObject::GetInspectionObject( LPCTSTR tszName, SVInspectionProcess** ppInspection ) const
{
	BOOL bResult = FALSE;

	CString sName( tszName );
	int iPos = sName.Find(_T("."));	// assumes first dot is after the inspection name
	if ( iPos > -1 )
	{
		bResult = GetChildObjectByName(sName.Left(iPos), ppInspection);
	}
	else
	{
		bResult = GetChildObjectByName(sName, ppInspection);
	}

	return bResult;
}

BOOL SVConfigurationObject::GetChildObjectByName( LPCTSTR tszName, SVInspectionProcess** ppInspection ) const
{
	BOOL bReturn = FALSE;

	//Only do an assert check so that in release mode no check is made
	ASSERT( nullptr != ppInspection );

	long lCount = GetInspectionCount( );
	CString sName = tszName;

	for( long l = 0; l < lCount; l++ )
	{ 
		SVInspectionProcess* pInspection = GetInspection( l );
		if( nullptr != pInspection && sName == pInspection->GetCompleteObjectName() )
		{
			*ppInspection = pInspection;
			bReturn = TRUE;
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

BOOL SVConfigurationObject::GetInspections( SVInspectionProcessPtrList& rvecInspections ) const
{
	std::copy( m_arInspectionArray.begin(), m_arInspectionArray.end(), std::inserter( rvecInspections, rvecInspections.begin() ) );
	return TRUE;
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

			SVRemoteInputObject* pRemoteInput = NULL;
			m_pInputObjectList->GetInputFlyweight( name.c_str(), pRemoteInput );
			if (pRemoteInput != NULL)
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
			SVDigitalInputObject* pDigitalInput = NULL;
			m_pInputObjectList->GetInputFlyweight( name.c_str(), pDigitalInput );

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

	SVRemoteInputObject* pRemoteInput = NULL;

	// Add Remote Inputs to the InputObjectList
	//first check to see if remote input is there, check by name...
	m_pInputObjectList->GetInputFlyweight( name.c_str(), pRemoteInput );

	// Add Remote Input to the PPQ
	SVValueObjectClass* pObject = new SVVariantValueObjectClass();
	pObject->SetName( name.c_str() );
	pObject->SetObjectDepth( 10 );
	pObject->SetResetOptions( false, SVResetItemNone );
	pObject->ResetObject();

	SVIOEntryHostStructPtr pIOEntry = new SVIOEntryHostStruct;
	pIOEntry->m_pValueObject	= pObject;
	pIOEntry->m_pValueParent	= pObject;
	pIOEntry->m_ObjectType		= IO_REMOTE_INPUT;
	pIOEntry->m_PPQIndex		= ppqPosition;
	pIOEntry->m_Enabled			= ppqPosition != -1;

	if( pRemoteInput != NULL )
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

	SVDigitalInputObject* pDigitalInput = NULL;

	m_pInputObjectList->GetInputFlyweight( name.c_str(), pDigitalInput );

	// Add Digital Input to the PPQ
	SVValueObjectClass* pObject = new SVBoolValueObjectClass();
	pObject->SetName(name.c_str());
	pObject->SetObjectDepth(10);
	pObject->SetResetOptions(false, SVResetItemNone);
	pObject->ResetObject();

	SVIOEntryHostStructPtr pIOEntry = new SVIOEntryHostStruct;
	pIOEntry->m_pValueObject	= pObject;
	pIOEntry->m_pValueParent	= pObject;
	pIOEntry->m_ObjectType		= IO_DIGITAL_INPUT;
	pIOEntry->m_PPQIndex		= ppqPosition;
	pIOEntry->m_Enabled			= ppqPosition != -1;

	if( pDigitalInput != NULL )
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

	SVCameraDataInputObject* pInput = NULL;
	SVString name = pIOEntry->m_pValueObject->GetName();
	m_pInputObjectList->GetInputFlyweight( name.c_str(), pInput );
	pInput->Create(); // why o why...

	// Add Input to the PPQ
	if( pInput != NULL )
	{
		pIOEntry->m_IOId = pInput->GetUniqueObjectID();
	}

	pPPQ->AddInput(pIOEntry);
	return hr;
}

HRESULT SVConfigurationObject::LoadConfiguration(SVTreeType& rTree)
{
	HRESULT		hr;

	hr = S_OK;

	BOOL bOk = DestroyConfiguration();
	if ( bOk )
	{
		SVTreeType::SVBranchHandle htiChild = NULL;

		CString l_sBoardName;
		long l_lNumBoardDig=0;
		long l_lNumCameras=0;


		bOk = SVNavigateTreeClass::GetItemBranch( rTree, CTAG_ENVIRONMENT, NULL, htiChild );
		if (bOk != TRUE)
		{
			hr = -1797;
		}
		else
		{
			_variant_t svValue;

			if ( SVNavigateTreeClass::GetItem( rTree, CTAG_CONFIGURATION_TYPE, htiChild, svValue ) )
			{
				int iType = svValue;
				SetProductType( (SVIMProductEnum) iType );
			}

			bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_VERSION_NUMBER, htiChild, svValue );

			if( bOk )
			{
				TheSVObserverApp.setLoadingVersion( svValue );

				m_ulVersion = TheSVObserverApp.getLoadingVersion();
			}

			//This is the deprecated tag and has changed to CTAG_IMAGE_UPDATE and CTAG_RESULT_UPDATE
			//Needs to be read for older configurations and becomes the standard default
			bool Value = true;
			if (SVNavigateTreeClass::GetItem( rTree, CTAG_ONLINE_DISPLAY, htiChild, svValue) )
			{
				Value = svValue;
			}

			if (SVNavigateTreeClass::GetItem( rTree, CTAG_IMAGE_DISPLAY_UPDATE, htiChild, svValue) )
			{
				Value = svValue;
			}
			RootObject::setRootChildValue( ::EnvironmentImageUpdate, Value );

			if (SVNavigateTreeClass::GetItem( rTree, CTAG_RESULT_DISPLAY_UPDATE, htiChild, svValue) )
			{
				Value = svValue;
			}
			RootObject::setRootChildValue( ::EnvironmentResultUpdate, Value );

			// Thread Affinity Setup
			SVTreeType::SVBranchHandle htiThreadSetup = NULL;
			CString strThreadTag;
			CString strName;
			int iThreadNum = 1;
			bool bThreadOk = true;
			while( bThreadOk )
			{
				long lAffinity;
				strThreadTag.Format(_T("%s_%d"), CTAG_THREAD_SETUP, iThreadNum);
				bThreadOk = SVNavigateTreeClass::GetItemBranch( rTree, strThreadTag, htiChild, htiThreadSetup );
				if( bThreadOk )
				{
					bThreadOk = SVNavigateTreeClass::GetItem( rTree, CTAG_THREAD_AFFINITY, htiThreadSetup, svValue);
					lAffinity = svValue;
					bThreadOk &= SVNavigateTreeClass::GetItem( rTree, CTAG_THREAD_NAME, htiThreadSetup, svValue);
					strName = svValue;
					if( bThreadOk  )
					{
						SVThreadManager::Instance().Setup( strName, lAffinity );
					}
					else
					{
						bThreadOk = FALSE;
					}
				}
				iThreadNum++;
			}

			// Thread Manager Enable
			BOOL bThreadMgrEnable = FALSE;
			if (SVNavigateTreeClass::GetItem( rTree, CTAG_THREAD_MGR_ENABLE, htiChild, svValue) )
			{
				bThreadMgrEnable = svValue;
			}
			SVThreadManager::Instance().SetThreadAffinityEnabled(bThreadMgrEnable);


		}// end if ( SVNavigateTreeClass::GetItem( rTree, CTAG_ENVIRONMENT, NULL, &htiChild ) )

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		if ( SVNavigateTreeClass::GetItemBranch( rTree, CTAG_IO, NULL, htiChild ) )
		{
			_variant_t svVariant;
			_variant_t svValue;
			CString strEntry;

			SVInputObjectList *pInputsList = new SVInputObjectList;
			SVOutputObjectList *pOutputsList = new SVOutputObjectList;
			pInputsList->SetName( _T( "Input Object List" ) );
			pOutputsList->SetName( _T( "Output Object List" ) );
			bOk &= pInputsList != NULL && pInputsList->Create();
			bOk &= pOutputsList != NULL && pOutputsList->Create();

			SVTreeType::SVBranchHandle htiSubChild;
			long lIOSize = 0;
			long i;

			if( bOk )
			{
				bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_NUMBER_OF_IO_ENTRIES, htiChild, svValue );

				if( bOk )
				{
					lIOSize = svValue;
				}// end if

			}// end if

			SVGUID l_ModuleReadyId;
			SVGUID l_RaidErrorId;

			// Loop through all the "IO EntryXXX"
			for( i = 0; bOk && i < lIOSize; i++ )
			{
				strEntry.Format( CTAGF_IO_ENTRY_X, i );
				bOk = SVNavigateTreeClass::GetItemBranch( rTree, strEntry, htiChild, htiSubChild );

				if ( bOk )
				{
					_variant_t svoData;
					CString strName;
					bool	bOutput;
					DWORD	dwChannel;
					bool	bForced;
					DWORD	dwForcedValue;
					bool	bInverted;
					bool	bCombined;
					bool	bCombinedACK;

					bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_IO_ENTRY_NAME, htiSubChild, svoData );

					if( bOk )
					{
						strName = svoData;
					}// end if

					bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_IS_OUTPUT, htiSubChild, svoData );

					if( bOk )
					{
						bOutput = svoData;
					}// end if

					bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_CHANNEL, htiSubChild, svoData );

					if( bOk )
					{
						dwChannel = svoData;
					}// end if

					bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_IS_FORCED, htiSubChild, svoData );

					if( bOk )
					{
						bForced = svoData;
					}// end if

					bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_FORCED_VALUE, htiSubChild, svoData );

					if( bOk )
					{
						dwForcedValue = static_cast< bool >( svoData );
					}// end if

					bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_IS_INVERTED, htiSubChild, svoData );

					if( bOk )
					{
						bInverted = svoData;
					}// end if

					bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_IS_FORCED, htiSubChild, svoData );

					if( bOk )
					{
						bForced = svoData;
					}// end if

					BOOL bCheck;
					bCheck = SVNavigateTreeClass::GetItem( rTree, CTAG_IS_COMBINED, htiSubChild, svoData );
					if( bCheck )
					{
						bCombined = svoData;
					}// end if
					else
					{
						bCombined = false;
					}// end else

					bCheck = SVNavigateTreeClass::GetItem( rTree, CTAG_IS_COMBINED_ACK, htiSubChild, svoData );
					if( bCheck )
					{
						bCombinedACK = svoData;
					}// end if
					else
					{
						bCombinedACK = false;
					}// end else

					if( (long)dwChannel > (long)-1 )
					{
						if( bOutput )
						{
							SVDigitalOutputObject* pOutput = NULL;

							pOutputsList->GetOutputFlyweight( strName, pOutput );

							if( pOutput != NULL )
							{
								pOutput->SetChannel( dwChannel );
								pOutput->Force( bForced, ( dwForcedValue != FALSE ) );
								pOutput->Invert( bInverted );
								pOutput->Combine( bCombined, bCombinedACK );

								if( strName.Compare( _T("Module Ready") ) == 0 )
								{
									l_ModuleReadyId = pOutput->GetUniqueObjectID();
								}
								else if( strName.Compare( _T("Raid Error Indicator") ) == 0 )
								{
									l_RaidErrorId = pOutput->GetUniqueObjectID();
								}
							}
						}// end if
						else
						{
							SVDigitalInputObject* pInput = NULL;

							pInputsList->GetInputFlyweight( static_cast< LPCTSTR >( strName ), pInput );

							if( pInput != NULL )
							{
								pInput->SetChannel( dwChannel );
								pInput->Force( bForced, dwForcedValue != FALSE );
								pInput->Invert( bInverted );							
							}
						}// end else
					}// end if

				}// end if
				else
				{					
					bOk = false;
					break;
				}// end else

			}// end for

			if ( bOk )
			{
				if( m_pInputObjectList != NULL )
				{
					delete m_pInputObjectList;
				}

				m_pInputObjectList  = pInputsList;

				if( m_pOutputObjectList != NULL )
				{
					delete m_pOutputObjectList;
				}

				m_pOutputObjectList = pOutputsList;
			}// end if

			if ( bOk )
			{
				if ( m_pIOController != NULL )
				{
					delete m_pIOController;
					m_pIOController = NULL;
				}

				m_pIOController = new SVIOController;
				if ( m_pIOController != NULL )
				{
					m_pIOController->SetParameters( rTree, htiChild );
				}
			}// end if

			if( bOk && l_ModuleReadyId.empty() )
			{
				SVDigitalOutputObject* pOutput = NULL;

				m_pOutputObjectList->GetOutputFlyweight( "Module Ready", pOutput );

				if( pOutput != NULL )
				{
					l_ModuleReadyId = pOutput->GetUniqueObjectID();
				}
			}

			m_pIOController->GetModuleReady()->m_IOId = l_ModuleReadyId;
			m_pIOController->GetRaidErrorBit()->m_IOId = l_RaidErrorId;
		}// end if ( SVNavigateTreeClass::GetItem( rTree, CTAG_IO, NULL, &htiChild ) )

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		if ( SVNavigateTreeClass::GetItemBranch( rTree, CTAG_ACQUISITION_DEVICE, NULL, htiChild ) )
		{
			SVTreeType::SVBranchHandle htiBoardChild = NULL;
			
			rTree.GetFirstBranch( htiChild, htiBoardChild );

			while ( bOk && htiBoardChild != NULL )
			{
				_bstr_t BoardName;
			
				rTree.GetBranchName( htiBoardChild, BoardName.GetBSTR() );

				CString csBoardName = static_cast< LPCTSTR >( BoardName );
				l_sBoardName = csBoardName;

				long lNumAcqDig=0;

				// test for File Acquisition, since there is no LUT, Light, or DeviceParams
				if (l_sBoardName == _T("File")) 
				{
					hr = LoadFileAcquisitionConfiguration(rTree, htiBoardChild, lNumAcqDig);
				}
				else
				{
					SVTreeType::SVBranchHandle htiDigChild = NULL;
					
					rTree.GetFirstBranch( htiBoardChild, htiDigChild );

					while ( bOk && htiDigChild != NULL )
					{
						++lNumAcqDig;

						bool bFileDone = false;
						bool bLightDone = false;
						bool bLutDone = false;
						bool bLutCreated = false;

						_variant_t l_Variant;
						SVLightReference svLight;
						SVFileNameArrayClass svFileArray;
						SVLut lut;
						SVDeviceParamCollection svDeviceParams;

						_bstr_t DigName;

						rTree.GetBranchName( htiDigChild, DigName.GetBSTR() );

						CString csDigName = static_cast< LPCTSTR >( DigName );

						SVTreeType::SVBranchHandle htiDataChild = NULL;
						
						rTree.GetFirstBranch( htiDigChild, htiDataChild );

						bFileDone = SVNavigateTreeClass::GetItem( rTree, CTAG_ACQUISITION_DEVICE_FILE_NAME, htiDigChild, l_Variant );

						if( bFileDone )
						{
								_bstr_t l_String( l_Variant );

								svFileArray.SetFileNameList( static_cast< LPCTSTR >( l_String ) );
						}

						if( SVNavigateTreeClass::GetItemBranch( rTree, CTAG_LIGHT_REFERENCE_ARRAY, htiDigChild, htiDataChild ) )
						{
							_variant_t svValue;

							if ( SVNavigateTreeClass::GetItem( rTree, CTAG_SIZE, htiDataChild, svValue ) )
							{
								int iBands = svValue;

								ASSERT( iBands > 0 );

								if ( svLight.Create( iBands ) )
								{
									for ( int i = 0; i < iBands; i++ )
									{
										CString csBand;

										SVTreeType::SVBranchHandle htiBand = NULL;

										csBand.Format( CTAGF_BAND_X, i );

										if ( SVNavigateTreeClass::GetItemBranch( rTree, (LPCTSTR)csBand, htiDataChild, htiBand ) )
										{
											if ( SVNavigateTreeClass::GetItem( rTree, CTAG_SIZE, htiBand, svValue ) )
											{
												int iSize = svValue;

												if ( svLight.Band( i ).Create( iSize ) )
												{
													for ( int j = 0; j < iSize; j++ )
													{
														CString csLight;

														SVTreeType::SVBranchHandle htiLight = NULL;

														csBand.Format( CTAGF_LIGHTREFERENCE_X, j );

														if ( SVNavigateTreeClass::GetItemBranch( rTree, (LPCTSTR)csBand, htiBand, htiLight ) )
														{
															if ( SVNavigateTreeClass::GetItem( rTree, CTAG_NAME, htiLight, svValue ) )
															{
																_bstr_t l_String( svValue );

																svLight.Band( i ).Attribute( j ).strName = static_cast< LPCTSTR >( l_String );
																//Legacy: changed name from Contrast to Gain
																if( CameraContrast == svLight.Band( i ).Attribute( j ).strName )
																{
																	svLight.Band( i ).Attribute( j ).strName = CameraGain;
																}
															}
															if ( SVNavigateTreeClass::GetItem( rTree, CTAG_RESOURCE_ID, htiLight, svValue ) )
															{
																svLight.Band( i ).Attribute( j ).iIDCaption = svValue;
															}
															if ( SVNavigateTreeClass::GetItem( rTree, CTAG_TYPE, htiLight, svValue ) )
															{
																svLight.Band( i ).Attribute( j ).dwType = svValue;
																ConvertLightReferenceEnum(svLight.Band( i ).Attribute( j ).dwType);
															}
															if ( SVNavigateTreeClass::GetItem( rTree, CTAG_VALUE, htiLight, svValue ) )
															{
																svLight.Band( i ).Attribute( j ).lValue = svValue;
															}

															bLightDone = true;
														}
													}// end for ( int j = 0; j < iSize; j++ )
												}// end if ( svLight.Band( i ).Create( iSize ) )
											}
										}
									}// end for ( int i = 0; i < iBands; i++ )
								}// end if ( svLight.Create( iBands ) )
							}// end if ( SVNavigateTreeClass::GetItem( rTree, CTAG_SIZE, htiDataChild, svValue ) )
						}

						if( SVNavigateTreeClass::GetItemBranch( rTree, CTAG_LUT, htiDigChild, htiDataChild ) )
						{
							_variant_t svValue;
							SVLutInfo lutinfo;

							if ( SVNavigateTreeClass::GetItem( rTree, CTAG_MAX_VALUE, htiDataChild, svValue ) )
							{
								UINT uiMax = svValue;

								lutinfo.SetMaxValue( uiMax );
							}

							if ( SVNavigateTreeClass::GetItem( rTree, CTAG_SIZE, htiDataChild, svValue ) )
							{
								int iBands = svValue;

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
										CString csBand;
										SVTreeType::SVBranchHandle htiBand = NULL;

										csBand.Format( CTAGF_BAND_X, iBand );

										SVLutTransformParameters param;

										if ( SVNavigateTreeClass::GetItemBranch( rTree, (LPCTSTR)csBand, htiDataChild, htiBand ) )
										{
											SVSAFEARRAY l_BandData;

											if ( SVNavigateTreeClass::GetItem( rTree, CTAG_LUT_BAND_DATA, htiBand, svValue ) )
											{
												l_BandData = svValue;

												int lSize = static_cast< int >( l_BandData.size() );

												if (!bLutCreated)
												{
													lutinfo.SetBandSize(lSize);
													lut.Create(lutinfo);
													bLutCreated = true;
												}
											}// end if ( SVNavigateTreeClass::GetItem( rTree, CTAG_LUT_BAND_DATA, htiBand, svValue ) )

											if ( SVNavigateTreeClass::GetItem( rTree, CTAG_LUT_TRANSFORM_PARAMETERS, htiBand, svValue ) )
											{
												SVSAFEARRAY l_Param( svValue );

												long lSize = static_cast< long >( l_Param.size() );
												// copy safearray to SVLutTransformParameters
												for (long l=0; l < lSize; l++)
												{
													_variant_t lVal;

													l_Param.GetElement( l, lVal );

													param.Add(lVal);
												}
											}// end if ( SVNavigateTreeClass::GetItem( rTree, CTAG_LUT_TRANSFORM_PARAMETERS, htiBand, svValue ) )

											if ( SVNavigateTreeClass::GetItem( rTree, CTAG_LUT_TRANSFORM_OPERATION, htiBand, svValue ) )
											{
												long lTransformOperation = svValue;

												const SVLutTransformOperation* pOperation = SVLutTransform::GetEquivalentType((SVLutTransformOperationEnum)lTransformOperation);
												ASSERT(pOperation);
												if (pOperation)
												{
													lut(iBand).SetTransformOperation(*pOperation);
													lut(iBand).Transform(param);
												}
											}// end if ( SVNavigateTreeClass::GetItem( rTree, CTAG_LUT_TRANSFORM_OPERATION, htiBand, svValue ) )

											if( bLutCreated && 0 < l_BandData.size() )
											{
												bool bSetData = lut(iBand).SetBandData( l_BandData );
											}

										}
									}// end for ( int i = 0; i < iBands; i++ )
									bLutDone = true;
								}// end if 
							}// end if ( SVNavigateTreeClass::GetItem( rTree, CTAG_SIZE, htiDataChild, svValue ) )
						}

						if( SVNavigateTreeClass::GetItemBranch( rTree, CTAG_DEVICE_PARAM_LIST, htiDigChild, htiDataChild ) )
						{
								LoadDeviceParameters(rTree, htiDataChild, svDeviceParams);
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

								cf.ParseAndAssignCameraFormat( pParam->strValue.ToString() );

								// Band number depends on video type...
								switch( cf.eImageType )
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
							// one band (viper quad / ?rgb_mono?)
							// if (svLight.NumBands() == 3) 
							// may need to change to handle RGB mono

							do
							{
								CString csFullName;

								SVAcquisitionClassPtr psvDevice;

								if ( 1 < lNumLRBands )
								{
									lNumLRBands = 1;
								}

								if ( 1 < l_BandCount )
								{
									csFullName.Format( "%s.%s.Ch_All", csBoardName, csDigName );
								}
								else
								{
									csFullName.Format( "%s.%s.Ch_%d", csBoardName, csDigName, lBand );
								}

								if ( ! bLutDone || ! bLutCreated )
								{
									bOk = SVImageProcessingClass::Instance().GetAcquisitionDevice( csFullName, psvDevice ) == S_OK;
									ASSERT( bOk );
									if (psvDevice)
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
								SVString strNewAcquisitionDeviceName = SVDigitizerProcessingClass::Instance().GetReOrderedCamera(csFullName);
								bOk = SVImageProcessingClass::Instance().GetAcquisitionDevice( strNewAcquisitionDeviceName.c_str(), psvDevice ) == S_OK;
								if ( bOk )
								{
									if ( !( psvDevice.empty() ) )
									{
										SVImageInfoClass svImageInfo;

										psvDevice->LoadFiles( svFileArray ); // @WARNING:  May crash if svFileArray is empty.

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
									}
									// SEJ - Get Combined parameters
									psvDevice->GetDeviceParameters(svDeviceParams);
									bOk = AddAcquisitionDevice( csFullName, svFileArray, svLight, lut, &svDeviceParams );
								}
							}// end do
							while ( ++lBand < lNumLRBands );
						}// end if bFileDone && bLightDone
						rTree.GetNextBranch( htiBoardChild, htiDigChild );
					}// end while ( bOk && htiDigChild != NULL )
				}
				l_lNumBoardDig = __max( l_lNumBoardDig, lNumAcqDig );

				rTree.GetNextBranch( htiChild, htiBoardChild );
			}// end while ( bOk && htiBoardChild != NULL )
		}// end if ( SVNavigateTreeClass::.GetItem( rTree, CTAG_ACQUISITION_DEVICE, NULL, &htiChild ) )

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		if ( SVNavigateTreeClass::GetItemBranch( rTree, CTAG_CAMERA, NULL, htiChild ) )
		{
			SVTreeType::SVBranchHandle htiSubChild = NULL;
			
			rTree.GetFirstBranch( htiChild, htiSubChild );

			while ( bOk && htiSubChild != NULL )
			{
				++l_lNumCameras;

				_bstr_t ItemName;

				rTree.GetBranchName( htiSubChild, ItemName.GetBSTR() );

				CString csItemName = static_cast< LPCTSTR >( ItemName );

				SVVirtualCamera *pCamera = new SVVirtualCamera;
				pCamera->SetName( csItemName );
				bOk = pCamera != NULL;
				if ( bOk )
				{
					_variant_t svValue;
					long lBandLink = 0;
					CString csDeviceName;

					SVTreeType::SVBranchHandle htiDataChild = NULL;
					
					if( SVNavigateTreeClass::GetItem( rTree, CTAG_ACQUISITION_DEVICE, htiSubChild, svValue ) )
					{
						_bstr_t l_String( svValue );

						csDeviceName = static_cast< LPCTSTR >( l_String );

						if ( csDeviceName.Find( "Ch_All"  ) >  -1 )
						{
							lBandLink = 0;
							pCamera->SetBandLink( lBandLink );
						}
						else
						{
							int iPos = csDeviceName.Find("Ch_");
							if (iPos > -1)
							{
								int iChannel = (int)(csDeviceName.GetAt( iPos + 3 ) - '0');
								ASSERT( iChannel >=0 && iChannel <= 9 );  // sanity check
								if ( 0 <= iChannel && iChannel <= 9)
								{
									lBandLink = iChannel;
									pCamera->SetBandLink( lBandLink );
								}
							}
						}
					}

					if( SVNavigateTreeClass::GetItem( rTree, CTAG_UNIQUE_REFERENCE_ID, htiSubChild, svValue ) )
					{
						SVGUID l_Guid( svValue );

						SVObjectManagerClass::Instance().CloseUniqueObjectID( pCamera );

						pCamera->outObjectInfo.UniqueObjectID = l_Guid;

						SVObjectManagerClass::Instance().OpenUniqueObjectID( pCamera );
					}

					if( SVNavigateTreeClass::GetItem( rTree, CTAG_BAND_LINK, htiSubChild, svValue ) )
					{
						pCamera->SetBandLink( svValue );
					}

					if( SVNavigateTreeClass::GetItem( rTree, CTAG_FILEACQUISITION_MODE, htiSubChild, svValue ) )
					{
						pCamera->SetFileAcquisitionMode( svValue );
					}

					if( SVNavigateTreeClass::GetItem( rTree, CTAG_FILEACQUISITION_IMAGE_FILENAME, htiSubChild, svValue ) )
					{
						SVString filename(svValue);
						pCamera->SetImageFilename(filename);
					}

					if( SVNavigateTreeClass::GetItem( rTree, CTAG_FILEACQUISITION_IMAGE_DIRECTORYNAME, htiSubChild, svValue ) )
					{
						SVString dirName(svValue);
						pCamera->SetImageDirectoryName(dirName);
					}

					if( SVNavigateTreeClass::GetItem( rTree, CTAG_FILEACQUISITION_LOADINGMODE, htiSubChild, svValue ) )
					{
						pCamera->SetFileLoadingMode(svValue);
					}

					if( SVNavigateTreeClass::GetItem( rTree, CTAG_FILEACQUISITION_IMAGE_SIZE_EDIT_MODE, htiSubChild, svValue ) )
					{
						pCamera->SetFileImageSizeEditModeFileBased(svValue);
					}

					if( SVNavigateTreeClass::GetItem( rTree, CTAG_FILEACQUISITION_FILE_IMAGE_WIDTH, htiSubChild, svValue ) )
					{
						pCamera->SetFileImageWidth(svValue);
					}

					if( SVNavigateTreeClass::GetItem( rTree, CTAG_FILEACQUISITION_FILE_IMAGE_HEIGHT, htiSubChild, svValue ) )
					{
						pCamera->SetFileImageHeight(svValue);
					}

					bOk = ! csDeviceName.IsEmpty();

					if ( bOk )
					{
						SVString strRemappedDeviceName = SVDigitizerProcessingClass::Instance().GetReOrderedCamera(csDeviceName);
						bOk = pCamera->Create( strRemappedDeviceName.c_str() );
					}

					if ( bOk )
					{
						bOk = AddCamera( pCamera );
					}

					rTree.GetNextBranch( htiChild, htiSubChild );
				}// end if pCamera != NULL
			}// end while ( bOk && htiSubChild != NULL )
		}// end if ( SVNavigateTreeClass::GetItem( rTree, CTAG_CAMERA, NULL, &htiChild ) )

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		if ( SVNavigateTreeClass::GetItemBranch( rTree, CTAG_TRIGGER, NULL, htiChild ) )
		{
			SVTreeType::SVBranchHandle htiSubChild = NULL;
			
			rTree.GetFirstBranch( htiChild, htiSubChild );

			while ( bOk && htiSubChild != NULL )
			{
				_bstr_t ItemName;

				rTree.GetBranchName( htiSubChild, ItemName.GetBSTR() );

				CString csItemName = static_cast< LPCTSTR >( ItemName );

				SVTriggerObject *pTrigger = new SVTriggerObject;
				pTrigger->SetName( csItemName );
				bOk = pTrigger != NULL;
				if ( bOk )
				{
					_variant_t svValue;
					CString csDeviceName;

					if( SVNavigateTreeClass::GetItem( rTree, CTAG_TRIGGER_DEVICE, htiSubChild, svValue ) )
					{
								_bstr_t l_String( svValue );

								csDeviceName = static_cast< LPCTSTR >( l_String );

								// legacy conversion
								StringMunge::ReplaceStrings( &csDeviceName, 1, _T("NI_6527"), _T("IO_Board") );
					}

					if( SVNavigateTreeClass::GetItem( rTree, CTAG_SOFTWARETRIGGER_DEVICE, htiSubChild, svValue ) )
					{
						pTrigger->SetSoftwareTrigger( svValue );
					}

					if( SVNavigateTreeClass::GetItem( rTree, CTAG_SOFTWARETRIGGER_PERIOD, htiSubChild, svValue ) )
					{
						pTrigger->SetSoftwareTriggerPeriod( svValue );
					}

					bOk = ! csDeviceName.IsEmpty();

					if ( bOk )
					{
						SVTriggerClass *psvDevice = SVTriggerProcessingClass::Instance().GetTrigger( csDeviceName );

						if ( psvDevice != NULL )
						{
							bOk = pTrigger->Create( psvDevice );
						}
					}

					if ( bOk )
					{
						bOk = AddTrigger( pTrigger );
					}

					rTree.GetNextBranch( htiChild, htiSubChild );
				}// end if pTrigger != NULL
			}// end while ( bOk && htiSubChild != NULL )
		}// end if ( SVNavigateTreeClass::GetItem( rTree, CTAG_TRIGGER, NULL, &htiChild ) )

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		if ( SVNavigateTreeClass::GetItemBranch( rTree, CTAG_INSPECTION, NULL, htiChild ) )
		{
			SVTreeType::SVBranchHandle htiSubChild = NULL;
			
			rTree.GetFirstBranch( htiChild, htiSubChild );

			while ( bOk && htiSubChild != NULL )
			{
				_bstr_t ItemName;

				rTree.GetBranchName( htiSubChild, ItemName.GetBSTR() );

				CString csItemName = static_cast< LPCTSTR >( ItemName );

				SVInspectionProcess* pInspection( nullptr );

				SVObjectManagerClass::Instance().ConstructObject( SVInspectionProcessGuid, pInspection );

				bOk = nullptr != pInspection;
				if ( bOk )
				{
					_variant_t svValue;
					CString csIPName;
					CString csToolsetName;
					CString csNewDisableMethod;
					CString csEnableAuxiliaryExtent;

					pInspection->SetName( csItemName );

					pInspection->SetDeviceName( csItemName );

					SVTreeType::SVBranchHandle htiSVIPDoc = NULL;
					SVTreeType::SVBranchHandle htiSVInspectionProcess = NULL;

					if( SVNavigateTreeClass::GetItem( rTree, CTAG_INSPECTION_FILE_NAME, htiSubChild, svValue ) )
					{
						_bstr_t l_String( svValue );

						csIPName = static_cast< LPCTSTR >( l_String );
					}

					SVNavigateTreeClass::GetItemBranch( rTree, CTAG_SVIPDOC, htiSubChild, htiSVIPDoc );

					SVNavigateTreeClass::GetItemBranch( rTree, CTAG_INSPECTION_PROCESS, htiSubChild, htiSVInspectionProcess );

					if( SVNavigateTreeClass::GetItem( rTree, CTAG_INSPECTION_TOOLSET_IMAGE, htiSubChild, svValue ) )
					{
						_bstr_t l_String( svValue );

						csToolsetName = static_cast< LPCTSTR >( l_String );
					}

					bOk = htiSVIPDoc != NULL && ! csIPName.IsEmpty();

					if ( bOk )
					{
						_variant_t svVariant;

						SVTreeType::SVBranchHandle htiTempIPItem = htiSubChild;
						SVTreeType::SVBranchHandle htiTempIPObjectItem = htiSVIPDoc;

						if( htiSVInspectionProcess != NULL )
						{
							htiTempIPItem = htiSVInspectionProcess;
							htiTempIPObjectItem = htiSVInspectionProcess;
						}

						bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_INSPECTION_NEW_DISABLE_METHOD, htiTempIPItem, svVariant );
						if ( bOk )
						{
							csNewDisableMethod = svVariant;
						}

						bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_INSPECTION_ENABLE_AUXILIARY_EXTENT, htiTempIPItem, svVariant );
						if ( bOk )
						{
							csEnableAuxiliaryExtent = svVariant;
						}

						bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_UNIQUE_REFERENCE_ID, htiTempIPObjectItem, svVariant );
						if ( bOk )
						{
							SVGUID ObjectID( svVariant );

							SVObjectManagerClass::Instance().CloseUniqueObjectID( pInspection );

							pInspection->outObjectInfo.UniqueObjectID = ObjectID;

							SVObjectManagerClass::Instance().OpenUniqueObjectID( pInspection );
						}
					}

					if ( bOk )
					{
						SVFileNameClass svFileName;

						svFileName.SetFullFileName( csIPName );

						pInspection->SetToolsetImage( csToolsetName );

						bOk = pInspection->CreateInspection( svFileName.GetFileNameOnly() );
						if ( bOk )
						{
							pInspection->SetNewDisableMethod( csNewDisableMethod == _T( "1" ) );
							pInspection->SetEnableAuxiliaryExtent( csEnableAuxiliaryExtent == _T("1") );

							SVTreeType::SVBranchHandle htiDataChild = NULL;
					
							if( SVNavigateTreeClass::GetItemBranch( rTree, CTAG_VIEWED_INPUTS, htiSubChild, htiDataChild ) )
							{
								SVTreeType::SVBranchHandle htiViewed;

								rTree.GetFirstBranch( htiDataChild, htiViewed );

								while( htiViewed != NULL )
								{	
									_bstr_t Name;

									rTree.GetBranchName( htiViewed, Name.GetBSTR() );

									CString strName = static_cast< LPCTSTR >( Name );

									pInspection->m_arViewedInputNames.Add( strName );

									rTree.GetNextBranch( htiDataChild, htiViewed );
								}// end while

							}// end if
						}
					}

					if ( bOk )
					{
						bOk = AddInspection( pInspection );
					}

					rTree.GetNextBranch( htiChild, htiSubChild );
				}// end if pInspection != NULL
			}// end while ( bOk && htiSubChild != NULL )
		}// end if ( SVNavigateTreeClass::GetItem( rTree, CTAG_INSPECTION, NULL, &htiChild ) )

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		if ( SVNavigateTreeClass::GetItemBranch( rTree, CTAG_PPQ, NULL, htiChild ) )
		{
			SVTreeType::SVBranchHandle htiSubChild = NULL;
				
			rTree.GetFirstBranch( htiChild, htiSubChild );

			while ( bOk && htiSubChild != NULL )
			{
				_bstr_t ItemName;

				rTree.GetBranchName( htiSubChild, ItemName.GetBSTR() );

				CString csItemName = static_cast< LPCTSTR >( ItemName );

				SVPPQObject* pPPQ = new SVPPQObject;
				pPPQ->SetName( csItemName );

				bOk = nullptr != pPPQ;
				if ( bOk )
				{
					SVTreeType::SVBranchHandle htiDeviceChild;
					SVTreeType::SVBranchHandle htiDataChild;
					_variant_t svValue;
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
					bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_UNIQUE_REFERENCE_ID, htiSubChild, svValue );

					if( bOk )
					{
						SVGUID ObjectID = svValue;

						SVObjectManagerClass::Instance().CloseUniqueObjectID( pPPQ );

						pPPQ->outObjectInfo.UniqueObjectID = ObjectID;

						SVObjectManagerClass::Instance().OpenUniqueObjectID( pPPQ );
					}// end if

					bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_PPQ_MODE, htiSubChild, svValue );

					if( bOk )
					{
						lMode = svValue;

						bOk = pPPQ->SetPPQOutputMode( (SVPPQOutputModeEnum)lMode );
					}// end if

					bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_PPQ_LENGTH, htiSubChild, svValue );

					if( bOk )
					{
						lLength = svValue;

						bOk = pPPQ->SetPPQLength( lLength );
					}// end if

					bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_PPQ_OUTPUT_RESET_DELAY_TIME, htiSubChild, svValue );

					if( bOk )
					{
						lDelay = svValue;

						bOk = pPPQ->SetResetDelay( lDelay );
					}// end if

					bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_PPQ_OUTPUT_DELAY_TIME, htiSubChild, svValue );

					if( bOk )
					{
						lDelay = svValue;

						bOk = pPPQ->SetOutputDelay( lDelay );
					}// end if

					bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_PPQ_MAINTAIN_SRC_IMAGE, htiSubChild, svValue );

					if ( bOk )
					{
						bMaintainSrcImg = svValue;

						bOk = pPPQ->SetMaintainSourceImages(bMaintainSrcImg);
					}

					bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_PPQ_INSPECTION_TIMEOUT, htiSubChild, svValue );

					if ( bOk )
					{
						lInspectionTimeout = svValue;

						bOk = pPPQ->SetInspectionTimeout(lInspectionTimeout);
					}

					bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_PPQ_CONDITIONAL_OUTPUT, htiSubChild, svValue );
					if ( bOk )	// Conditional Output.
					{
						SVString condition(svValue);
						if (!condition.empty())
						{
							pPPQ->SetConditionalOutputName(condition);
						}
					}
					// Update source to remove SVOVariant
					// PPQ State Variable
					// Load the Unique Reference ID for the PPQ
					BOOL bTmp = SVNavigateTreeClass::GetItem( rTree, CTAG_PPQ_STATE_UNIQUEID, htiSubChild, svValue );

					if( bTmp )
					{
						SVGUID ObjectID = svValue;

						SVObjectManagerClass::Instance().CloseUniqueObjectID( &pPPQ->m_voOutputState );

						pPPQ->m_voOutputState.outObjectInfo.UniqueObjectID = ObjectID;

						SVObjectManagerClass::Instance().OpenUniqueObjectID( &pPPQ->m_voOutputState );
					}// end if

					bTmp = SVNavigateTreeClass::GetItem( rTree, CTAG_PPQ_TRIGGER_COUNT_ID, htiSubChild, svValue );
					if ( bTmp )
					{
						SVGUID l_TriggercountId = svValue;

						SVObjectManagerClass::Instance().CloseUniqueObjectID( &pPPQ->m_voTriggerCount );

						pPPQ->m_voTriggerCount.outObjectInfo.UniqueObjectID = l_TriggercountId;

						SVObjectManagerClass::Instance().OpenUniqueObjectID( &pPPQ->m_voTriggerCount );
					}

					bOk = SVNavigateTreeClass::GetItemBranch( rTree, CTAG_TRIGGER, htiSubChild, htiDeviceChild );

					if( bOk )
					{
						rTree.GetFirstBranch( htiDeviceChild, htiDataChild );

						while ( bOk && htiDataChild != NULL )
						{
							lCount = GetTriggerCount( );

							_bstr_t DataName;

							rTree.GetBranchName( htiDataChild, DataName.GetBSTR() );

							CString csDataName = static_cast< LPCTSTR >( DataName );

							for ( long l = 0; bOk && l < lCount; l++ )
							{
								SVTriggerObject *pTrigger = GetTrigger( l );

								//Returns true when pointer valid
								bOk = (nullptr != pTrigger);
								if ( bOk )
								{
									CString csDeviceName = pTrigger->GetName();

									if ( csDeviceName == csDataName )
									{
										bOk = pPPQ->AttachTrigger( pTrigger );

										break;
									}
								}// end if
							}// end for

							rTree.GetNextBranch( htiDeviceChild, htiDataChild );
						}// end while ( bOk && htiDataChild != NULL )
					}// end if SVNavigateTreeClass::GetItem( rTree, CTAG_TRIGGER, htiSubChild, &htiDeviceChild )

					bOk = SVNavigateTreeClass::GetItemBranch( rTree, CTAG_CAMERA, htiSubChild, htiDeviceChild );

					if( bOk )
					{
						rTree.GetFirstBranch( htiDeviceChild, htiDataChild );

						while ( bOk && htiDataChild != NULL )
						{
							lCount = GetCameraCount( );

							_bstr_t DataName;

							rTree.GetBranchName( htiDataChild, DataName.GetBSTR() );

							CString csDataName = static_cast< LPCTSTR >( DataName );

							bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_POSITION, htiDataChild, svValue );

							if ( bOk )
							{
								lPosition = svValue;
							}// end if

							for ( long l = 0; bOk && l < lCount; l++ )
							{
								SVVirtualCamera* pCamera =GetCamera( l );

								bOk = (nullptr != pCamera);
								if ( bOk )
								{
									CString csDeviceName = pCamera->GetName();

									if ( csDeviceName == csDataName )
									{
										bOk = pPPQ->AttachCamera( pCamera, lPosition );

										break;
									}// end if
								}// end if
							}// end for

							rTree.GetNextBranch( htiDeviceChild, htiDataChild );
						}// end while ( bOk && htiDataChild != NULL )
					}// end if SVNavigateTreeClass::GetItem( rTree, CTAG_CAMERA, htiSubChild, &htiDeviceChild );

					bOk = SVNavigateTreeClass::GetItemBranch( rTree, CTAG_INSPECTION, htiSubChild, htiDeviceChild );

					if( bOk )
					{
						rTree.GetFirstBranch( htiDeviceChild, htiDataChild );

						while ( bOk && htiDataChild != NULL )
						{
							lCount = GetInspectionCount( );
							_bstr_t DataName;

							rTree.GetBranchName( htiDataChild, DataName.GetBSTR() );

							CString csDataName = static_cast< LPCTSTR >( DataName );

							for ( long l = 0; bOk && l < lCount; l++ )
							{
								SVInspectionProcess* pInspection = GetInspection( l );

								bOk = (nullptr != pInspection);
								if ( bOk )
								{
									CString csDeviceName = pInspection->GetDeviceName();

									if ( csDeviceName == csDataName )
									{
										bOk = pPPQ->AttachInspection( pInspection );

										break;
									}// end if
								}// end if
							}// end for

							rTree.GetNextBranch( htiDeviceChild, htiDataChild );
						}// end while ( bOk && htiDataChild != NULL )
					}// end if SVNavigateTreeClass::GetItem( rTree, CTAG_INSPECTION, htiSubChild, &htiDeviceChild )
					else
					{
						// It is OK to not have an inspection attached to the PPQ
						bOk = TRUE;
					}

					bOk = SVNavigateTreeClass::GetItemBranch( rTree, CTAG_INPUT, htiSubChild, htiDeviceChild );

					if( bOk )
					{
						rTree.GetFirstBranch( htiDeviceChild, htiDataChild );

						while ( bOk && htiDataChild != NULL )
						{
							_bstr_t DataName;

							rTree.GetBranchName( htiDataChild, DataName.GetBSTR() );

							CString csDataName = static_cast< LPCTSTR >( DataName );

							CString csValueName = csDataName;

							SVRemoteInputObject *pRemoteInput = NULL;
							SVValueObjectClass *pObject = NULL;
							_variant_t svValue;
							CString strName;
							CString strType;
							long lIndex;
							long lPPQPosition=0;

							bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_IO_TYPE, htiDataChild, svValue );
							if ( bOk )
							{
								strType = static_cast< LPCTSTR >( static_cast< _bstr_t >( svValue ) );
							}// end if

							// This means it is a Digital input
							if( _T("Digital") == strType )
							{								
								bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_ITEM_NAME, htiDataChild, svValue );

								if ( bOk )
								{
									csDataName = static_cast< LPCTSTR >( static_cast< _bstr_t >( svValue ) );
								}// end if

								bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_PPQ_POSITION, htiDataChild, svValue );

								if ( bOk )
								{
									lPPQPosition = svValue;
								}// end if

								// Add Digital Input to the PPQ
								AddDigitalInput(pPPQ, csDataName, lPPQPosition); //Arvid 150108 should this really be called if bOk == false?
																				 //Arvid 150108 if so: what value of lPPQPosition should be used
							}// end if

							// This means it is a Remote input
							if( _T("Remote") == strType )
							{								
								_variant_t l_Variant = 0.0;

								bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_ITEM_NAME, htiDataChild, svValue );

								if ( bOk )
								{
									csDataName = static_cast< LPCTSTR >( static_cast< _bstr_t >( svValue ) );
								}// end if

								bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_PPQ_POSITION, htiDataChild, svValue );

								if ( bOk )
								{
									lPPQPosition = svValue;
								}// end if

								bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_REMOTE_INDEX, htiDataChild, svValue );

								if ( bOk )
								{
									lIndex = svValue;
								}// end if

								if( SVNavigateTreeClass::GetItem( rTree, CTAG_REMOTE_INITIAL_VALUE, htiDataChild, svValue ) )
								{
									l_Variant = svValue;
								}// end if

								// Add Remote Inputs to the InputObjectList
								//first check to see if remote input is there, check by name...
								AddRemoteInput(pPPQ, csDataName, lPPQPosition, lIndex, l_Variant);
							}// end if

							rTree.GetNextBranch( htiDeviceChild, htiDataChild );

						}// end while ( bOk && htiDataChild != NULL )
					}// end if SVNavigateTreeClass::GetItem( rTree, CTAG_INPUT, htiSubChild, &htiDeviceChild )

					if ( bOk )
					{
						bOk = pPPQ->Create();

						bOk &= pPPQ->RebuildInputList(HasCameraTrigger(pPPQ));

						bOk &= pPPQ->RebuildOutputList();

						if ( bOk )
						{
							bOk = AddPPQ( pPPQ );
						}
					}// end if
				}// end if pPPQ != NULL

				rTree.GetNextBranch( htiChild, htiSubChild );
			}// end while ( bOk && htiSubChild != NULL )
		}// end if ( SVNavigateTreeClass::GetItem( rTree, CTAG_PPQ, NULL, &htiChild ) )

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// EB 20031203
		// a temp solution
		// the better solution is to have the acqs subscribe and the triggers provide
		HRESULT hrAttach = AttachAcqToTriggers();

		// figure out what kind of configuration we have ( support for old configurations without CTAG_CONFIGURATION_TYPE )
		if ( bOk && GetProductType() == SVIM_PRODUCT_TYPE_UNKNOWN )
		{
			bool bViperRGB = false;
			bool bViperRGBMono = false;
			bool bViperQuad = false;
			bool bMatrox1394 = false;

			if ( l_sBoardName.Find(_T("RGB")) != -1 )
			{
				bViperRGB = true;
				if ( l_lNumCameras > 1 )	// RGB Color doesn't have channel information
				{
					bViperRGBMono = true;
				}
			}
			else if ( l_sBoardName.Find(_T("Quad")) != -1 )
			{
				bViperQuad = true;
			}
			else if ( l_sBoardName.Find(_T("1394")) != -1 )
				bMatrox1394 = true;

			if ( bViperRGB )
			{
				if ( bViperRGBMono )
					SetProductType( SVIM_PRODUCT_RGB_MONO );
				else
					SetProductType( SVIM_PRODUCT_RGB_COLOR );
			}
			else if ( bViperQuad )
			{
				if ( l_lNumBoardDig <= 2 )
				{
					SetProductType( SVIM_PRODUCT_05 );
				}
				else
				{
					SetProductType( SVIM_PRODUCT_FULL );
				}
			}
			else if ( bMatrox1394 )
			{
				SetProductType( SVIM_PRODUCT_D3 );
			}
		}// end if ( GetProductType() == SVIM_PRODUCT_TYPE_UNKNOWN )

		if ( ! bOk )
		{
			DestroyConfiguration();
		}
		else
		{
			m_bConfigurationValid = true;
		}
	}// end if DestroyConfiguration()

	return hr;
}

HRESULT SVConfigurationObject::ValidateOutputList( )
{
	HRESULT l_Status = S_OK;

	SVOutputObjectList::StringVect l_aInspNames;
	SVOutputObjectList::StringVect l_aPPQNames;

	long l_iInspections = GetInspectionCount( );

	for( int i = 0 ; i < l_iInspections ; i++ )
	{
		SVInspectionProcess* l_pInsp = m_arInspectionArray[i];
		if( l_pInsp != NULL )
		{
			l_aInspNames.push_back( l_pInsp->GetName() );
		}
	}
	for( SVPPQObjectArray::iterator it = m_arPPQArray.begin(); it != m_arPPQArray.end() ; ++it)
	{
		if( *it != NULL )
		{
			l_aPPQNames.push_back((*it)->GetName());
		}
	}

	l_Status = m_pOutputObjectList->RemoveUnusedOutputs( l_aInspNames, l_aPPQNames );

	return l_Status;
}

HRESULT SVConfigurationObject::LoadAcquisitionDeviceFilename(SVTreeType& rTree, SVTreeType::SVBranchHandle htiDig, SVFileNameArrayClass& svFileArray)
{
	HRESULT hr = S_FALSE;

	_variant_t l_Variant;

	if( SVNavigateTreeClass::GetItem( rTree, CTAG_ACQUISITION_DEVICE_FILE_NAME, htiDig, l_Variant ) )
	{
		_bstr_t svValue = l_Variant;

		svFileArray.SetFileNameList( svValue );

		hr = S_OK;
	}
	return hr;
}

HRESULT SVConfigurationObject::LoadFileAcquisitionConfiguration(SVTreeType& rTree, SVTreeType::SVBranchHandle htiBoardChild, long& lNumAcqDig)
{
	HRESULT hr = S_OK;
	_bstr_t BoardName;
	rTree.GetBranchName( htiBoardChild, BoardName.GetBSTR() );
	CString csBoardName = static_cast< LPCTSTR >( BoardName );
	SVTreeType::SVBranchHandle htiDigChild = NULL;
	rTree.GetFirstBranch( htiBoardChild, htiDigChild );

	SVLightReference svLight;
	SVLut lut;

	while ( hr == S_OK && htiDigChild != NULL )
	{
		++lNumAcqDig;

		SVFileNameArrayClass svFileArray;
		SVDeviceParamCollection svDeviceParams;

		_bstr_t DigName;

		rTree.GetBranchName( htiDigChild, DigName.GetBSTR() );

		CString csDigName = static_cast< LPCTSTR >( DigName );

		SVTreeType::SVBranchHandle htiDataChild = NULL;

		if( SVNavigateTreeClass::GetItemBranch( rTree, CTAG_ACQUISITION_DEVICE_FILE_NAME, htiDigChild, htiDataChild ) )
		{
			hr = LoadAcquisitionDeviceFilename(rTree, htiDataChild, svFileArray);
		}

		if( SVNavigateTreeClass::GetItemBranch( rTree, CTAG_DEVICE_PARAM_LIST, htiDigChild, htiDataChild ) )
		{
				LoadDeviceParameters(rTree, htiDataChild, svDeviceParams);
		}

		if( hr == S_OK )
		{
			// need to determine Digitizer Number and Channel
			CString csFullName;

			SVAcquisitionClassPtr psvDevice;

			// Get Color versus Mono
			const SVDeviceParam* pParam = svDeviceParams.GetParameter(DeviceParamFileAcqImageFormat);
			_variant_t var;
			pParam->GetValue(var);
			if (var.lVal == SVImageFormatMono8)
			{
				csFullName.Format( "%s.%s.Ch_0", csBoardName, csDigName );
			}
			else
			{
				csFullName.Format( "%s.%s.Ch_All", csBoardName, csDigName );
			}

			hr = SVImageProcessingClass::Instance().GetAcquisitionDevice( csFullName, psvDevice );
			if ( hr == S_OK )
			{
				if ( psvDevice != NULL )
				{
					SVImageInfoClass svImageInfo;

					// Set the device parameters for the File Acquisition device (Note: do this before calling LoadFiles)
					psvDevice->SetDeviceParameters( svDeviceParams );	// needs to come before LR (quick fix for now)
					psvDevice->SetLightReference( svLight );
					psvDevice->SetLut( lut );

					// cause the image info to be updated for the File Acquisition device
					psvDevice->LoadFiles( svFileArray );

					psvDevice->GetImageInfo( &svImageInfo );
					//psvDevice->DestroyBuffers();
					psvDevice->CreateBuffers( svImageInfo, TheSVObserverApp.GetSourceImageDepth() );
				}
				SVString strRemappedName = SVDigitizerProcessingClass::Instance().GetReOrderedCamera(csFullName);
				if( !( AddAcquisitionDevice( strRemappedName.c_str(), svFileArray, svLight, lut, &svDeviceParams ) ) )
				{
					hr = E_FAIL;
				}
			}
		}
		rTree.GetNextBranch( htiBoardChild, htiDigChild );
	}
	return hr;
}

HRESULT SVConfigurationObject::LoadDeviceParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiDataChild, SVDeviceParamCollection& svDeviceParams)
{
	_variant_t svValue;
	CString strParam;
	int iParam = 0;
	SVTreeType::SVBranchHandle htiParam = NULL;

	strParam.Format( CTAGF_DEVICE_PARAM_X, ++iParam );

	while ( SVNavigateTreeClass::GetItemBranch( rTree, (LPCTSTR) strParam, htiDataChild, htiParam ) )
	{
		if ( SVNavigateTreeClass::GetItem( rTree, CTAG_DEVICE_PARAM_TYPE, htiParam, svValue ) )
		{
			long lType = svValue;

			if ( SVNavigateTreeClass::GetItem( rTree, CTAG_DEVICE_PARAM_VALUE, htiParam, svValue ) )
			{
				HRESULT hrParam = svDeviceParams.CreateParameter( (SVDeviceParamEnum) lType, svValue );
				SVDeviceParam* pParam = svDeviceParams.GetParameter( (SVDeviceParamEnum) lType );
				LoadDeviceParamSpecial( rTree, htiParam, pParam );
			}
		}

		htiParam = NULL;
		strParam.Format( CTAGF_DEVICE_PARAM_X, ++iParam );
	}

	return S_OK;
}

HRESULT SVConfigurationObject::LoadDeviceParamSpecial( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent, SVDeviceParam* pParam )
{
	HRESULT hr = S_OK;
	_variant_t svVariant;
	VARIANT vValue;
	::VariantInit( &vValue );
	SVTreeType::SVBranchHandle hti=NULL;

	switch ( pParam->Type() )
	{
		case DeviceParamCameraFormats:	
		{
			SVCameraFormatsDeviceParam* pcf = dynamic_cast<SVCameraFormatsDeviceParam*> (pParam);
			CString sOptionTag;
			int iOption=0;
			SVTreeType::SVBranchHandle htiParam=NULL;
			if ( SVNavigateTreeClass::GetItemBranch( rTree, CTAG_OPTIONS, htiParent, hti ) )
			{
				sOptionTag.Format( CTAGF_OPTION_X, ++iOption );
				while ( SVNavigateTreeClass::GetItemBranch( rTree, sOptionTag, hti, htiParam ) )
				{
					if ( SVNavigateTreeClass::GetItem( rTree, CTAG_NAME, htiParam, svVariant ) )
					{
						CString sName( static_cast< LPCTSTR >( static_cast< _bstr_t >( svVariant ) ) );

						SVCameraFormat& rcf = pcf->options[sName];
						rcf.ParseAndAssignCameraFormat( sName );
						if ( SVNavigateTreeClass::GetItem( rTree, CTAG_VARIABLE_CAMERA_FORMAT, htiParam, svVariant ) )
							rcf.bVariableROI = svVariant;
						if ( SVNavigateTreeClass::GetItem( rTree, CTAG_TOP, htiParam, svVariant ) )
							rcf.lVPos = svVariant;
						if ( SVNavigateTreeClass::GetItem( rTree, CTAG_LEFT, htiParam, svVariant ) )
							rcf.lHPos = svVariant;
						if ( SVNavigateTreeClass::GetItem( rTree, CTAG_CX, htiParam, svVariant ) )
							rcf.lWidth = svVariant;
						if ( SVNavigateTreeClass::GetItem( rTree, CTAG_CY, htiParam, svVariant ) )
							rcf.lHeight = svVariant;
					}// end if ( SVNavigateTreeClass::GetItem( rTree, CTAG_NAME, htiParam, svVariant ) )
					sOptionTag.Format( CTAGF_OPTION_X, ++iOption );
				}// end while ( SVNavigateTreeClass::GetItem( rTree, sOptionTag, hti, &htiParam ) )
			}// end if ( SVNavigateTreeClass::GetItem( rTree, CTAG_OPTIONS, htiParent, &hti ) )
		}// end case DeviceParamCameraFormats:	
		break;

	}// end switch ( pParam->Type() )

	return hr;
}

BOOL SVConfigurationObject::DestroyConfiguration()
{
	BOOL bOk = TRUE;

	m_bConfigurationValid = false;

	long l = 0;
	long lSize = 0;

	//destroy inspections...
	lSize = m_arInspectionArray.GetSize();
	for ( l = lSize - 1; -1 < l; l-- )
	{
		if ( m_arInspectionArray[l] != NULL )
		{
			delete m_arInspectionArray[l];
			m_arInspectionArray[l] = NULL;
		}
	}
	m_arInspectionArray.RemoveAll();

	//destroy camera's
	lSize = m_arCameraArray.GetSize();
	for ( l = lSize - 1; -1 < l; l-- )
	{
		if ( m_arCameraArray[l] != NULL )
		{
			delete m_arCameraArray[l];
			m_arCameraArray[l] = NULL;
		}
	}
	m_arCameraArray.RemoveAll();

	CString csKey;
	SVConfigurationAcquisitionDeviceInfoStruct *pDevice = NULL;

	SVAcquisitionDeviceMap::iterator pos = mAcquisitionDeviceMap.GetStartPosition();
	while ( pos != mAcquisitionDeviceMap.end() )
	{
		csKey = pos->first;
		pDevice = pos->second;

		pos = mAcquisitionDeviceMap.erase( pos );

		if ( pDevice != NULL )
		{
			SVAcquisitionClassPtr psvDevice;

			delete pDevice;

			bOk = SVImageProcessingClass::Instance().GetAcquisitionDevice( csKey, psvDevice ) == S_OK;
			if ( bOk )
			{
				if( !( psvDevice.empty() ) )
				{
					psvDevice->DestroyBuffers();
					psvDevice->ResetLightReference();
					psvDevice->UnloadFiles();
				}
			}
		}
	}

	mAcquisitionDeviceMap.RemoveAll();

	//destroy trigger's
	lSize = m_arTriggerArray.GetSize();
	for ( l = lSize - 1; -1 < l; l-- )
	{
		if ( m_arTriggerArray[l] != NULL )
		{
			delete m_arTriggerArray[l];
			m_arTriggerArray[l] = NULL;
		}
	}
	m_arTriggerArray.RemoveAll();

	//destroy PPQ's
	lSize = m_arPPQArray.GetSize();
	for ( l = lSize - 1; -1 < l; l-- )
	{
		if ( m_arPPQArray[l] != NULL )
		{
			delete m_arPPQArray[l];
			m_arPPQArray[l] = NULL;
		}
	}
	m_arPPQArray.RemoveAll();

	//destroy Input Object List
	if ( m_pInputObjectList	!= NULL )
	{
		delete m_pInputObjectList;
		m_pInputObjectList = NULL;
	}

	//destroy Output Object List    
	if ( m_pOutputObjectList	!= NULL )
	{
		delete m_pOutputObjectList;
		m_pOutputObjectList = NULL;
	}

	if( m_pIOController != NULL )
	{
		delete m_pIOController;
		m_pIOController = NULL;
	}

	//Delete all Global Constants
	RootObject::resetRootChildValue( SvOl::FqnGlobal );

	SetProductType( SVIM_PRODUCT_TYPE_UNKNOWN );

	SVDigitizerProcessingClass::Instance().ClearDevices();

	SVThreadManager::Instance().Clear();

	return bOk;
}

HRESULT SVConfigurationObject::ObserverUpdate( const SVRenameObject& p_rData )
{
	HRESULT l_Status = S_OK;

	SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( p_rData.m_ObjectId );

	if( l_pObject != NULL )
	{
		SVPPQObject* pPPQ( nullptr );
		SVOutputObjectList *pOutputs = GetOutputObjectList();
		long lPPQ;

		if( nullptr != pOutputs )
		{
			::SVSendMessage( pOutputs, SVM_OBJECT_RENAMED,
				reinterpret_cast<DWORD_PTR>( l_pObject ),
				reinterpret_cast<DWORD_PTR>( static_cast<LPCTSTR>( p_rData.m_OldName.c_str() )) );
		}
		else
		{
			l_Status = E_FAIL;
		}

		long lCount = GetPPQCount();
		for( lPPQ = 0; lPPQ < lCount; lPPQ++ )
		{
			pPPQ = GetPPQ( lPPQ );

			if( nullptr != pPPQ )
			{
				::SVSendMessage( pPPQ, SVM_OBJECT_RENAMED,
					reinterpret_cast<DWORD_PTR>( l_pObject ),
					reinterpret_cast<DWORD_PTR>( static_cast<LPCTSTR>( p_rData.m_OldName.c_str() )));

				pPPQ->RebuildOutputList();
			}
			else
			{
				l_Status = E_FAIL;
			}
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVConfigurationObject::GetChildObject( SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, long Index ) const
{
	HRESULT l_Status = S_OK;

	rpObject = NULL;

	if( 0 < rNameInfo.m_NameArray.size() )
	{
		if( rNameInfo.m_NameArray[ 0 ] == "RemoteInputs" )
		{
			if(nullptr != m_pInputObjectList)
			{
				m_pInputObjectList->GetInput( rNameInfo.GetObjectName( 1 ), rpObject );
			}
			else
			{
				l_Status = S_FALSE;
				return l_Status;
			}
		}

		if( rpObject == NULL )
		{
			SVInspectionProcessVector::const_iterator l_InspectIter;

			//@WARNING [gra][7.20][07.07.2015] This is very dangerous when a name starting with just "Tool Set." it will find the first matching name in all inspections
			for( l_InspectIter = m_arInspectionArray.begin(); rpObject == NULL && l_InspectIter != m_arInspectionArray.end(); ++l_InspectIter )
			{
				SVInspectionProcess* pInspection = ( *l_InspectIter );

				if( nullptr != pInspection )
				{
					l_Status = pInspection->GetChildObject( rpObject, rNameInfo, Index );
				}
			}
		}

		if( rpObject == NULL )
		{
			if( rNameInfo.m_NameArray[ 0 ].Left( 3 ) == _T( "PPQ" ) )
			{
				SVPPQObjectArray::const_iterator l_PPQIter;

				for( l_PPQIter = m_arPPQArray.begin(); rpObject == NULL && l_PPQIter != m_arPPQArray.end(); ++l_PPQIter )
				{
					SVPPQObject* pPPQ = ( *l_PPQIter );

					if( nullptr != pPPQ )
					{
						l_Status = pPPQ->GetChildObject( rpObject, rNameInfo, Index );
					}
				}
			}
			else if( rNameInfo.m_NameArray[ 0 ].Left( 6 ) == _T( "Camera" ) )
			{
				SVVirtualCameraArray::const_iterator l_CameraIter;

				for( l_CameraIter = m_arCameraArray.begin(); rpObject == NULL && l_CameraIter != m_arCameraArray.end(); ++l_CameraIter )
				{
					SVVirtualCamera* pCamera = ( *l_CameraIter );

					if( nullptr != pCamera )
					{
						l_Status = pCamera->GetChildObject( rpObject, rNameInfo, Index );
					}
				}
			}
			else if( rNameInfo.m_NameArray[ 0 ].Left( 7 ) == _T( "Trigger" ) )
			{
				SVTriggerObjectArray::const_iterator l_TriggerIter;

				for( l_TriggerIter = m_arTriggerArray.begin(); rpObject == NULL && l_TriggerIter != m_arTriggerArray.end(); ++l_TriggerIter )
				{
					SVTriggerObject* pTrigger = ( *l_TriggerIter );

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

void SVConfigurationObject::SaveEnvironment(SVObjectXMLWriter& rWriter) const
{
	rWriter.StartElement(CTAG_ENVIRONMENT);

	_variant_t svValue = TheSVObserverApp.getCurrentVersion();
	rWriter.WriteAttribute(CTAG_VERSION_NUMBER, svValue);
	svValue = GetProductType();
	rWriter.WriteAttribute( CTAG_CONFIGURATION_TYPE, svValue );

	RootObject::getRootChildValue( ::EnvironmentImageUpdate, svValue );
	rWriter.WriteAttribute( CTAG_IMAGE_DISPLAY_UPDATE, svValue );
	RootObject::getRootChildValue( ::EnvironmentResultUpdate, svValue );
	rWriter.WriteAttribute( CTAG_RESULT_DISPLAY_UPDATE, svValue );

	// Thread Manager Enable.
	BOOL bEnable = SVThreadManager::Instance().GetThreadAffinityEnabled();
	svValue = bEnable;
	rWriter.WriteAttribute( CTAG_THREAD_MGR_ENABLE, svValue );

	SVThreadManager::ThreadList threads;
	SVThreadManager::Instance().GetThreadInfo(threads, SVThreadAttribute::SVAffinityUser );
	int iCount = 1;
	for( SVThreadManager::ThreadList::const_iterator it = threads.begin() ; it != threads.end(); ++it)
	{
		CString strBranch;
		strBranch.Format( _T("%s_%d"), CTAG_THREAD_SETUP, iCount);
		rWriter.StartElement( strBranch );
		svValue = it->m_strName.c_str();
		rWriter.WriteAttribute( CTAG_THREAD_NAME, svValue );
		svValue = it->m_lAffinity;
		rWriter.WriteAttribute( CTAG_THREAD_AFFINITY, svValue );
		rWriter.EndElement();
		iCount++;
	}

	rWriter.EndElement(); //End of Enviroment
}

void SVConfigurationObject::SaveIO(SVObjectXMLWriter& rWriter) const
{
	rWriter.StartElement( CTAG_IO );

	CString csName;
	SVIOEntryHostStructPtrList ppInList;

	if ( nullptr != m_pInputObjectList )
	{
		m_pInputObjectList->FillInputs( ppInList );
	}

	long lInSize = static_cast< long >( ppInList.size() );
	long lCount = 0;

	for ( long lIn = 0; lIn < lInSize; lIn++ )
	{
		csName.Format( CTAGF_IO_ENTRY_X, lCount );

		if( ppInList[lIn]->m_ObjectType == IO_DIGITAL_INPUT )
		{
			rWriter.StartElement( csName );

			CString csEntryName;
			_variant_t svVariant;

			SVDigitalInputObject* pInput = nullptr;

			m_pInputObjectList->GetInput( ppInList[lIn]->m_IOId, pInput );

			csEntryName = pInput->GetName();
			svVariant = csEntryName;
			rWriter.WriteAttribute( CTAG_IO_ENTRY_NAME, svVariant );
			svVariant.Clear();

			svVariant = pInput->GetChannel();
			rWriter.WriteAttribute( CTAG_CHANNEL, svVariant );
			svVariant.Clear();

			svVariant = false;
			rWriter.WriteAttribute( CTAG_IS_OUTPUT, svVariant );
			svVariant.Clear(); //Must clear VT_BOOL Type after use

			svVariant = ( pInput->IsInverted() == TRUE );
			rWriter.WriteAttribute( CTAG_IS_INVERTED, svVariant );
			svVariant.Clear(); //Must clear VT_BOOL Type after use

			svVariant = pInput->IsForced() == TRUE;
			rWriter.WriteAttribute( CTAG_IS_FORCED, svVariant );
			svVariant.Clear(); //Must clear VT_BOOL Type after use

			svVariant = pInput->GetForcedValue();
			rWriter.WriteAttribute( CTAG_FORCED_VALUE, svVariant );
			svVariant.Clear(); //Must clear VT_BOOL Type after use

			rWriter.EndElement();

			lCount++;
		}
	}// end for

	SVIOEntryHostStructPtrList ppOutList;
	long lOutSize = 0;
	if ( nullptr != m_pOutputObjectList )
	{
		m_pOutputObjectList->FillOutputs( ppOutList );

		lOutSize = static_cast< long >( ppOutList.size() );
	}

	for ( long lOut = 0; lOut < lOutSize; lOut++ )
	{
		csName.Format( CTAGF_IO_ENTRY_X, lCount );

		if( ppOutList[lOut]->m_ObjectType == IO_DIGITAL_OUTPUT )
		{
			rWriter.StartElement( csName );

			CString csEntryName;
			_variant_t svVariant;

			SVDigitalOutputObject* pOutput = nullptr;

			m_pOutputObjectList->GetOutput( ppOutList[lOut]->m_IOId, pOutput );

			csEntryName = pOutput->GetName();
			svVariant.SetString( csEntryName );
			rWriter.WriteAttribute( CTAG_IO_ENTRY_NAME, svVariant );
			svVariant.Clear();

			svVariant = pOutput->GetChannel();
			rWriter.WriteAttribute( CTAG_CHANNEL, svVariant );
			svVariant.Clear();

			svVariant = true;
			rWriter.WriteAttribute( CTAG_IS_OUTPUT, svVariant );
			svVariant.Clear(); //Must clear VT_BOOL Type after use

			svVariant = ( pOutput->IsInverted() != FALSE );
			rWriter.WriteAttribute( CTAG_IS_INVERTED, svVariant );
			svVariant.Clear(); //Must clear VT_BOOL Type after use

			svVariant = ( pOutput->IsForced() != FALSE );
			rWriter.WriteAttribute( CTAG_IS_FORCED, svVariant );
			svVariant.Clear(); //Must clear VT_BOOL Type after use

			svVariant = ( pOutput->GetForcedValue() != FALSE );
			rWriter.WriteAttribute( CTAG_FORCED_VALUE, svVariant );
			svVariant.Clear(); //Must clear VT_BOOL Type after use

			svVariant = ( pOutput->IsCombined() != FALSE );
			rWriter.WriteAttribute( CTAG_IS_COMBINED, svVariant );
			svVariant.Clear(); //Must clear VT_BOOL Type after use

			svVariant = pOutput->GetCombinedValue();
			rWriter.WriteAttribute( CTAG_IS_COMBINED_ACK, svVariant );
			svVariant.Clear(); //Must clear VT_BOOL Type after use

			rWriter.EndElement();

			lCount++;
		}
	}// end for ( long lOut = 0; lOut < lOutSize; lOut++ )

	_variant_t svVariant = lCount;
	rWriter.WriteAttribute( CTAG_NUMBER_OF_IO_ENTRIES, svVariant );


	//***************** Save the IODoc ... *****************************************
	if ( nullptr != m_pIOController )
	{
		rWriter.StartElement( CTAG_IODOC );
		m_pIOController->GetParameters( rWriter );
		rWriter.EndElement();
	}

	rWriter.EndElement(); //End of CTAG_IO
}

void SVConfigurationObject::SaveAcquisitionDevice(SVObjectXMLWriter& rWriter)  const
{
	rWriter.StartElement( CTAG_ACQUISITION_DEVICE );

	CString csName;
	SVFileNameArrayClass *pFiles = nullptr;
	SVLightReference *pLight = nullptr;
	SVLut* pLut = nullptr;
	SVDeviceParamCollection* pDeviceParams = nullptr;

	SVAcquisitionDeviceMap::iterator pos = mAcquisitionDeviceMap.GetStartPosition();
	while ( pos != mAcquisitionDeviceMap.end() )
	{
		GetAcquisitionDeviceNextAssoc( pos, csName, pFiles, pLight, pLut, pDeviceParams );
		CString csBoard;
		CString csDig;

		int iIndex = csName.Find( '.', 0 );
		csBoard = csName.Left( iIndex );

		bool l_bSaveAcq = false;

		if ( csBoard.Compare("Viper_RGB_1") == 0 )
		{
			if ( csName.Compare("Viper_RGB_1.Dig_0.Ch_All") == 0 )
			{
				l_bSaveAcq = true;
			}
		}
		else
		{ 
			//not an RGB write it out
			l_bSaveAcq = true;
		}

		if ( l_bSaveAcq )
		{
			if ( ! csBoard.IsEmpty() )
			{
				int iDigIndex = csName.Find( '.', iIndex + 1 );
				csDig = csName.Mid( iIndex + 1, iDigIndex - iIndex - 1 );
				if ( ! csDig.IsEmpty() )
				{
					rWriter.StartElement( csBoard ); //htiBoard
					rWriter.StartElement( csDig ); //htiDig
					if ( nullptr != pFiles )
					{
						CString csFiles = pFiles->GetFileNameList();

						_variant_t svVariant;
						svVariant.SetString( csFiles );
						rWriter.WriteAttribute( CTAG_ACQUISITION_DEVICE_FILE_NAME, svVariant );
					}

					if ( nullptr != pDeviceParams )
					{
						// test for File Acquisition, since there is no LUT, Light, or DeviceParams per se
						if (csBoard == _T("File")) 
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
		}// end if ( l_bSaveAcq )
	}// end while ( pos != NULL )

	rWriter.EndElement(); //End of CTAG_ACQUISITION_DEVICE
}

void SVConfigurationObject::SaveAcquistionConfiguration(SVObjectXMLWriter& rWriter, const SVLightReference& rLight, const SVLut& rLut, const SVDeviceParamCollection& rDeviceParams) const
{
	rWriter.StartElement( CTAG_LIGHT_REFERENCE_ARRAY );

	_variant_t svVariant = rLight.NumBands();
	rWriter.WriteAttribute( CTAG_SIZE, svVariant );

	for ( int i = 0; i < rLight.NumBands(); i++)
	{
		CString csBand;
		csBand.Format( CTAGF_BAND_X, i );
		rWriter.StartElement( csBand );

		svVariant = rLight.Band( i ).NumAttributes();
		rWriter.WriteAttribute( CTAG_SIZE, svVariant );
		svVariant.Clear();

		for ( int j = 0; j < rLight.Band( i ).NumAttributes(); j++ )
		{
			CString csRef;
			csRef.Format( CTAGF_LIGHTREFERENCE_X, j );

			rWriter.StartElement( csRef );

			svVariant.SetString( rLight.Band( i ).Attribute( j ).strName.ToString() );
			rWriter.WriteAttribute( CTAG_NAME, svVariant );
			svVariant.Clear();

			svVariant = rLight.Band( i ).Attribute( j ).iIDCaption;
			rWriter.WriteAttribute( CTAG_RESOURCE_ID, svVariant );
			svVariant.Clear();

			svVariant = rLight.Band( i ).Attribute( j ).dwType;
			rWriter.WriteAttribute( CTAG_TYPE, svVariant );
			svVariant.Clear();

			svVariant = rLight.Band( i ).Attribute( j ).lValue;
			rWriter.WriteAttribute( CTAG_VALUE, svVariant );
			svVariant.Clear();
			rWriter.EndElement(); // csRef
		}// end for ( int j = 0; j < rLight.Band( i ).NumAttributes(); j++ )

		rWriter.EndElement(); // csBand
	}// end for ( int i = 0; i < rLight.NumBands(); i++)
	rWriter.EndElement(); // CTAG_LIGHT_REFERENCE_ARRAY

	rWriter.StartElement( CTAG_LUT );

	svVariant = rLut.Info().Bands();
	rWriter.WriteAttribute( CTAG_SIZE, svVariant );
	svVariant.Clear();

	svVariant = rLut.Info().MaxValue();
	rWriter.WriteAttribute( CTAG_MAX_VALUE, svVariant );
	svVariant.Clear();

	for (int iBand=0; iBand < static_cast<int>(rLut.Info().Bands()); iBand++)
	{
		CString csBand;
		csBand.Format( CTAGF_BAND_X, iBand );
		rWriter.StartElement( csBand );

		SAFEARRAY* psaParam=nullptr;
		bool bGotParam = rLut(iBand).Info().GetTransformParameters(psaParam);
		svVariant = SVSAFEARRAY( psaParam );
		rWriter.WriteAttribute( CTAG_LUT_TRANSFORM_PARAMETERS, svVariant );
		svVariant.Clear();

		const SVLutTransformOperation* pOperation = rLut(iBand).GetTransformOperation();
		SVLutTransformOperationEnum e = SVLutTransform::GetEquivalentType(pOperation);
		ASSERT( e != LutTransformTypeUnknown );
		svVariant = (long) e;
		rWriter.WriteAttribute( CTAG_LUT_TRANSFORM_OPERATION, svVariant );
		svVariant.Clear();

		SAFEARRAY* psaBandData = nullptr;
		bool bGotData = rLut(iBand).GetBandData(psaBandData);
		svVariant = SVSAFEARRAY( psaBandData );
		rWriter.WriteAttribute( CTAG_LUT_BAND_DATA, svVariant );
		svVariant.Clear();
		rWriter.EndElement(); // csBand
	}
	rWriter.EndElement(); // CTAG_LUT

	SaveDeviceParameters(rWriter, rDeviceParams);
}

void SVConfigurationObject::SaveCamera(SVObjectXMLWriter& rWriter) const
{
	rWriter.StartElement( CTAG_CAMERA );

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
				svVariant.SetString( pCamera->mpsvDevice->DeviceName() );
				rWriter.WriteAttribute( CTAG_ACQUISITION_DEVICE, svVariant );
				svVariant.Clear();

				svVariant = SVGUID( pCamera->GetUniqueObjectID() ).ToVARIANT();
				rWriter.WriteAttribute( CTAG_UNIQUE_REFERENCE_ID, svVariant );
				svVariant.Clear();

				svVariant = pCamera->BandLink();
				rWriter.WriteAttribute( CTAG_BAND_LINK, svVariant );
				svVariant.Clear();

				svVariant = pCamera->IsFileAcquisition();
				rWriter.WriteAttribute( CTAG_FILEACQUISITION_MODE, svVariant );
				svVariant.Clear();

				if (pCamera->IsFileAcquisition())
				{
					svVariant = pCamera->GetImageFilename();
					rWriter.WriteAttribute( CTAG_FILEACQUISITION_IMAGE_FILENAME, svVariant );
					svVariant.Clear();

					svVariant = pCamera->GetImageDirectoryName();
					rWriter.WriteAttribute( CTAG_FILEACQUISITION_IMAGE_DIRECTORYNAME, svVariant );
					svVariant.Clear();

					svVariant = pCamera->GetFileLoadingMode();
					rWriter.WriteAttribute( CTAG_FILEACQUISITION_LOADINGMODE, svVariant );
					svVariant.Clear();

					svVariant = ( pCamera->IsFileImageSizeEditModeFileBased() ? 1 : 0 );
					rWriter.WriteAttribute( CTAG_FILEACQUISITION_IMAGE_SIZE_EDIT_MODE, svVariant );
					svVariant.Clear();

					const SIZE& size = pCamera->GetFileImageSize();
					svVariant = size.cx;
					rWriter.WriteAttribute( CTAG_FILEACQUISITION_FILE_IMAGE_WIDTH,  svVariant );
					svVariant.Clear();

					svVariant = size.cy;
					rWriter.WriteAttribute( CTAG_FILEACQUISITION_FILE_IMAGE_HEIGHT,  svVariant );
					svVariant.Clear();
				}
			}

			rWriter.EndElement(); // pCamera->GetName()
		}
	}// end for ( long l = 0; l < lCount; l++ )

	rWriter.EndElement(); //CTAG_CAMERA
}

void SVConfigurationObject::SaveTrigger(SVObjectXMLWriter& rWriter) const
{
	rWriter.StartElement( CTAG_TRIGGER );

	long lCount = GetTriggerCount( );
	for ( long l = 0; l < lCount; l++ )
	{
		SVTriggerObject *pTrigger = GetTrigger( l );
		
		if ( nullptr != pTrigger )
		{
			rWriter.StartElement( pTrigger->GetName() );

			if ( nullptr != pTrigger->mpsvDevice )
			{
				_variant_t svVariant;
				svVariant.SetString( pTrigger->mpsvDevice->GetDeviceName() );
				rWriter.WriteAttribute( CTAG_TRIGGER_DEVICE, svVariant );
			}
			if ( pTrigger->IsSoftwareTrigger())
			{
				_variant_t svVariant = true;
				rWriter.WriteAttribute( CTAG_SOFTWARETRIGGER_DEVICE, svVariant );
				svVariant.Clear();

				svVariant = pTrigger->GetSoftwareTriggerPeriod();
				rWriter.WriteAttribute( CTAG_SOFTWARETRIGGER_PERIOD, svVariant );
				svVariant.Clear();
			}
			rWriter.EndElement(); //pTrigger->GetName()
		}
	}// end for ( long l = 0; l < lCount; l++ )
	rWriter.EndElement();  // CTAG_TRIGGER
}

void SVConfigurationObject::SaveInspection(SVObjectXMLWriter& rWriter) const
{
	rWriter.StartElement( CTAG_INSPECTION );

	long lCount = GetInspectionCount( );
	for ( long l = 0; l < lCount; l++ )
	{
		SVInspectionProcess* pInspection = GetInspection( l );

		if ( nullptr != pInspection )
		{
			rWriter.StartElement( pInspection->GetName() );

			_variant_t svVariant;
			svVariant.SetString( pInspection->GetName() );
			rWriter.WriteAttribute( CTAG_INSPECTION_FILE_NAME, svVariant );
			svVariant.Clear();

			svVariant.SetString( pInspection->GetToolsetImage() );
			rWriter.WriteAttribute( CTAG_INSPECTION_TOOLSET_IMAGE, svVariant );
			svVariant.Clear();

#ifdef USE_OBJECT_SCRIPT
			svVariant = pInspection->GetNewDisableMethod();
			rWriter.WriteAttribute( CTAG_INSPECTION_NEW_DISABLE_METHOD, svVariant );
			svVariant.Clear();

			svVariant = pInspection->GetEnableAuxiliaryExtent();
			rWriter.WriteAttribute( CTAG_INSPECTION_ENABLE_AUXILIARY_EXTENT , svVariant );
			svVariant.Clear();
#endif
			//Inspection Process
			pInspection->Persist(rWriter);
			
			//SVIPDoc
			SVIPDoc* pDoc =  SVObjectManagerClass::Instance().GetIPDoc(pInspection->GetUniqueObjectID());
			if (pDoc)
			{
				rWriter.StartElement(CTAG_SVIPDOC);
				pDoc->GetParameters(rWriter);
				rWriter.EndElement();
			}

			rWriter.EndElement(); //pInspection->GetName()
		}
	}

	rWriter.EndElement(); //CTAG_INSPECTION
}

void SVConfigurationObject::SavePPQ(SVObjectXMLWriter& rWriter) const
{
	rWriter.StartElement(CTAG_PPQ);
	
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

			SVTriggerObject *pTrigger( nullptr );
			pPPQ->GetTrigger( pTrigger );

			if( nullptr != pTrigger )
			{
				rWriter.StartElement( CTAG_TRIGGER );
				strName = pTrigger->GetName();
				rWriter.StartElement( strName.c_str() );
				rWriter.EndElement();
				rWriter.EndElement(); //CTAG_TRIGGER
			}// end if

			SavePPQ_Cameras(rWriter, *pPPQ);
			SavePPQ_Inspections(rWriter, *pPPQ);

			//Input
			pPPQ->PersistInputs(rWriter);

			rWriter.EndElement(); //pPPQ->GetName()
		}// end if (nullptr != pPPQ);
	}// end for( lPPQ = 0; lPPQ < lPPQCount; lPPQ++ )
	rWriter.EndElement();  // CTAG_PPQ
}

void SVConfigurationObject::SavePPQ_Attributes( SVObjectXMLWriter &rWriter, const SVPPQObject& rPPQ ) const
{
	SVGUID ObjectGuid = rPPQ.GetUniqueObjectID();
	_variant_t svValue = ObjectGuid.ToVARIANT();
	rWriter.WriteAttribute( CTAG_UNIQUE_REFERENCE_ID, svValue );
	svValue.Clear();

	SVPPQOutputModeEnum lMode;
	rPPQ.GetPPQOutputMode( lMode );
	svValue = lMode;
	rWriter.WriteAttribute( CTAG_PPQ_MODE, svValue );
	svValue.Clear();

	long lLength = 0;
	rPPQ.GetPPQLength( lLength );
	svValue = lLength;
	rWriter.WriteAttribute( CTAG_PPQ_LENGTH, svValue );
	svValue.Clear();

	long lDelay = 0;
	rPPQ.GetResetDelay( lDelay );
	svValue = lDelay;
	rWriter.WriteAttribute( CTAG_PPQ_OUTPUT_RESET_DELAY_TIME, svValue );
	svValue.Clear();

	rPPQ.GetOutputDelay( lDelay );
	svValue = lDelay;
	rWriter.WriteAttribute( CTAG_PPQ_OUTPUT_DELAY_TIME, svValue );
	svValue.Clear();

	bool bMaintainSrcImg = false;
	rPPQ.GetMaintainSourceImages( bMaintainSrcImg );
	svValue = bMaintainSrcImg;
	rWriter.WriteAttribute( CTAG_PPQ_MAINTAIN_SRC_IMAGE, svValue );
	svValue.Clear();

	long lInspectionTimeout = 0;
	rPPQ.GetInspectionTimeout( lInspectionTimeout );
	svValue = lInspectionTimeout;
	rWriter.WriteAttribute( CTAG_PPQ_INSPECTION_TIMEOUT, svValue );
	svValue.Clear();

	SVGUID l_PPQTriggerCountGUID = rPPQ.m_voTriggerCount.GetUniqueObjectID();
	svValue = l_PPQTriggerCountGUID.ToVARIANT();
	rWriter.WriteAttribute( CTAG_PPQ_TRIGGER_COUNT_ID, svValue );
	svValue.Clear();

	// Conditional Output.
	const SVString& l_condition = rPPQ.GetConditionalOutputName();
	svValue.SetString(l_condition.ToString());
	rWriter.WriteAttribute( CTAG_PPQ_CONDITIONAL_OUTPUT, svValue );
	svValue.Clear();

				// Save State Objects unique ID
	ObjectGuid = rPPQ.m_voOutputState.GetUniqueObjectID();
	svValue = ObjectGuid.ToVARIANT();
	rWriter.WriteAttribute( CTAG_PPQ_STATE_UNIQUEID, svValue );
	svValue.Clear();
}

void SVConfigurationObject::SavePPQ_Cameras( SVObjectXMLWriter &rWriter, const SVPPQObject& rPPQ ) const
{
	std::deque< SVVirtualCamera* > l_Cameras;
	rPPQ.GetCameraList( l_Cameras );
	if( 0 < l_Cameras.size() )
	{
		rWriter.StartElement( CTAG_CAMERA );

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
				rWriter.WriteAttribute( CTAG_POSITION, svValue );
				svValue.Clear();
				rWriter.EndElement();
			}
			++l_Iter;
		}
		rWriter.EndElement(); //CTAG_CAMERA
	}
}

void SVConfigurationObject::SavePPQ_Inspections( SVObjectXMLWriter &rWriter, const SVPPQObject& rPPQ ) const
{
	long lInspectCount;
	rPPQ.GetInspectionCount( lInspectCount );
	if( lInspectCount )
	{
		rWriter.StartElement( CTAG_INSPECTION );
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

		rWriter.EndElement(); //CTAG_INSPECTION
	}// end if
}

bool SVConfigurationObject::SaveRemoteMonitorList( SVObjectXMLWriter &rWriter ) const
{
	bool bOk = true;
	rWriter.StartElement( CTAG_MONITOR_LISTS );

	const RemoteMonitorList& remoteMonitorLists = GetRemoteMonitorList();
	RemoteMonitorList::const_iterator iterMonitorList = remoteMonitorLists.begin();
	while ( bOk && remoteMonitorLists.end() != iterMonitorList )
	{
		const SVString& strName = iterMonitorList->first;
		rWriter.StartElement( strName.c_str() );
		
		_variant_t svValue;
		const RemoteMonitorNamedList& monitorList = iterMonitorList->second;
		svValue.SetString( monitorList.GetPPQName().ToString() );
		rWriter.WriteAttribute( CTAG_PPQ_NAME, svValue );
		svValue.Clear();
		
		svValue = monitorList.GetRejectDepthQueue();			
		rWriter.WriteAttribute( CTAG_REJECT_QUEUE_DEPTH, svValue );
		svValue.Clear();

		bOk = SaveMonitoredObjectList( rWriter, CTAG_PRODUCTVALUES_LIST, monitorList.GetProductValuesList() ) && bOk;
		bOk = SaveMonitoredObjectList( rWriter, CTAG_PRODUCTIMAGE_LIST, monitorList.GetProductImagesList() ) && bOk;
		bOk = SaveMonitoredObjectList( rWriter, CTAG_REJECTCONDITION_LIST, monitorList.GetRejectConditionList() ) && bOk;
		bOk = SaveMonitoredObjectList( rWriter, CTAG_FAILSTATUS_LIST, monitorList.GetFailStatusList() ) && bOk;
		rWriter.EndElement(); //strName.c_str()
		iterMonitorList++;
	}

	rWriter.EndElement(); //CTAG_MONITOR_LISTS
	return bOk;
}

bool SVConfigurationObject::SaveMonitoredObjectList( SVObjectXMLWriter &rWriter, const SVString& listName, const MonitoredObjectList& rList ) const
{
	bool bOk = true;
	rWriter.StartElement( listName.c_str() );

	_variant_t svValue;
	svValue.SetString( _T("") );
	MonitoredObjectList::const_iterator iter = rList.begin();
	while ( bOk && rList.end() != iter )
	{
		const MonitoredObject& rObj = *iter;
		const SVString& objectName = RemoteMonitorListHelper::GetNameFromMonitoredObject(rObj);//SVObjectManagerClass::Instance().GetCompleteObjectName( guid );
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

void SVConfigurationObject::SaveGlobalConstants( SVObjectXMLWriter &rWriter ) const
{
	rWriter.StartElement( CTAG_GLOBAL_CONSTANTS );

	BasicValueObjects::ValueVector GlobalConstantObjects;
	RootObject::getRootChildObjectList( GlobalConstantObjects, SvOl::FqnGlobal, 0 );
	BasicValueObjects::ValueVector::const_iterator Iter( GlobalConstantObjects.cbegin() );
	while ( GlobalConstantObjects.cend() != Iter  && !Iter->empty() )
	{
		SVString Name( (*Iter)->GetCompleteObjectName() );
		rWriter.StartElement( Name.c_str() );

		_variant_t Value;
		(*Iter)->getValue( Value );
		rWriter.WriteAttribute( CTAG_VALUE, Value );
		Value.Clear();


		CString Description( (*Iter)->getDescription() );
		//This is needed to remove any CR LF in the description
		::SVAddEscapeSpecialCharacters( Description, true );
		Value.SetString( Description );
		rWriter.WriteAttribute( CTAG_DESCRIPTION, Value );
		Value.Clear();

		rWriter.EndElement();
		++Iter;
	}

	rWriter.EndElement(); //CTAG_GLOBAL_CONSTANTS
}

BOOL SVConfigurationObject::SaveConfiguration(SVObjectXMLWriter& rWriter) const
{
	BOOL bOk = true;
	SVString rootNodeName(SvOl::FqnRoot);
	rWriter.WriteRootElement(rootNodeName);
	rWriter.WriteSchema();

	DWORD versionNumber = TheSVObserverApp.getCurrentVersion();
	SVString versionString;
	versionString.Format("%d.%d", versionNumber >> 16, (versionNumber >> 8) & 0x000000ff);
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
	return bOk;
}

void SVConfigurationObject::SaveFileAcquisitionConfiguration(SVObjectXMLWriter& rWriter, const SVDeviceParamCollection& rDeviceParams) const
{
	// save the camera params
	SaveDeviceParameters(rWriter, rDeviceParams);
}

void SVConfigurationObject::SaveDeviceParameters( SVObjectXMLWriter& rWriter, const SVDeviceParamCollection& rDeviceParams ) const
{
	rWriter.StartElement( CTAG_DEVICE_PARAM_LIST );

	SVDeviceParamMap::const_iterator iter;
	int i = 0;
	for (iter = rDeviceParams.mapParameters.begin(); iter != rDeviceParams.mapParameters.end(); ++iter)
	{
		const SVDeviceParam* pParam = iter->second;
		if ( pParam != NULL )
		{
			//ASSERT( pParam->Type() != DeviceParamInvalid );
			if ( pParam->Type() != DeviceParamInvalid )
			{
				VARIANT vValue;
				::VariantInit( &vValue );
				HRESULT hrValue = pParam->GetValue( vValue );
				if ( hrValue == S_OK )
				{
					CString strParam;
					strParam.Format( CTAGF_DEVICE_PARAM_X, ++i );
					rWriter.StartElement( strParam );
					
					_variant_t svVariant;
					svVariant = SVDeviceParam::GetParameterName( pParam->Type() );
					rWriter.WriteAttribute( CTAG_NAME, svVariant );
					svVariant.Clear();

					svVariant = (long) pParam->Type();
					rWriter.WriteAttribute( CTAG_DEVICE_PARAM_TYPE, svVariant );
					svVariant.Clear();

					svVariant = vValue;
					rWriter.WriteAttribute( CTAG_DEVICE_PARAM_VALUE, svVariant );
					svVariant.Clear();

					// HACK!!! EB 20031215: This was put in at Jim's request
					// to clarify to someone looking at the config what units the value is stored in.
					svVariant.SetString( _T("native") );
					rWriter.WriteAttribute( CTAG_DEVICE_PARAM_UNITS, svVariant );
					svVariant.Clear();

					SaveDeviceParamSpecial( rWriter, pParam );

					rWriter.EndElement();
				}
			}
		}
	}

	rWriter.EndElement(); //CTAG_DEVICE_PARAM_LIST
}

void SVConfigurationObject::SaveDeviceParamSpecial( SVObjectXMLWriter& rWriter, const SVDeviceParam* pParam ) const
{
	_variant_t svVariant;
	VARIANT vValue;
	::VariantInit( &vValue );

	switch ( pParam->Type() )
	{
		case DeviceParamCameraFormats:	
		{
			const SVCameraFormatsDeviceParam* pcf = dynamic_cast<const SVCameraFormatsDeviceParam*> (pParam);
			CString strParam( CTAG_OPTIONS );
			rWriter.StartElement( CTAG_OPTIONS );

			int iOption=0;
			SVCameraFormatsDeviceParam::OptionsType::const_iterator iter;
			for ( iter = pcf->options.begin(); iter != pcf->options.end(); ++iter)
			{
				const SVCameraFormat& rcf = iter->second;
				strParam.Format( CTAGF_OPTION_X, ++iOption );
				rWriter.StartElement( strParam );

				svVariant.SetString( rcf.m_strName.ToString() );
				rWriter.WriteAttribute( CTAG_NAME, svVariant );
				svVariant.Clear();

				svVariant = rcf.bVariableROI;
				rWriter.WriteAttribute( CTAG_VARIABLE_CAMERA_FORMAT, svVariant );
				svVariant.Clear();

				svVariant = rcf.lHPos;
				rWriter.WriteAttribute( CTAG_LEFT, svVariant );
				svVariant.Clear();

				svVariant = rcf.lVPos;
				rWriter.WriteAttribute( CTAG_TOP, svVariant );
				svVariant.Clear();

				svVariant = rcf.lWidth;
				rWriter.WriteAttribute( CTAG_CX, svVariant );
				svVariant.Clear();

				svVariant = rcf.lHeight;
				rWriter.WriteAttribute( CTAG_CY, svVariant );
				svVariant.Clear();

				rWriter.EndElement(); // CTAGF_OPTION_X
			}// end for ( iter = pcf->options.begin(); iter != pcf->options.end(); iter++)
			rWriter.EndElement(); // CTAG_OPTIONS
		}// end case DeviceParamCameraFormats:	
		break;
	}// end switch ( pParam->Type() )
}

BOOL SVConfigurationObject::FinishIPDoc( SVInspectionProcess* pInspection )
{
	BOOL bOk = FALSE;

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

			bOk = TRUE;
		}

	}

	return bOk;
}

BOOL SVConfigurationObject::Activate()
{
	BOOL bOk = TRUE;

	long lSize = m_arInspectionArray.GetSize();

	for ( long l = lSize - 1; -1 < l; l-- )
	{
		SVInspectionProcess* l_pInspection = m_arInspectionArray[l];

		SVCommandInspectionRunOncePtr l_CommandPtr = new SVCommandInspectionRunOnce( l_pInspection->GetUniqueObjectID() );
		SVObjectSynchronousCommandTemplate< SVCommandInspectionRunOncePtr > l_Command( l_pInspection->GetUniqueObjectID(), l_CommandPtr );

		l_Command.Execute( 120000 );
	}

	return bOk;
}

BOOL SVConfigurationObject::RebuildInputOutputLists()
// called after Environment is edited
{
	BOOL bOk = TRUE;

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

	if( m_pIOController != NULL )
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
	BOOL bOk = TRUE;
	SVTreeType::SVBranchHandle htiChild = NULL;
	unsigned long ulVersion;

	if ( SVNavigateTreeClass::GetItemBranch( rTree, CTAG_ENVIRONMENT, NULL, htiChild ) )
	{
		_variant_t svValue;
		if ( SVNavigateTreeClass::GetItem( rTree, CTAG_CONFIGURATION_TYPE, htiChild, svValue ) )
		{
			int iType( svValue );

			SetProductType( (SVIMProductEnum) iType );
		}

		bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_VERSION_NUMBER, htiChild, svValue );

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

void SVConfigurationObject::SetupSoftwareTrigger(SVSoftwareTriggerClass* pTriggerDevice, int iDigNum, long triggerPeriod, SVPPQObject* pPPQ)
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
		SVAcquisitionInitiator acqInitiator;

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
					SVDigitizerLoadLibraryClass* pAcqDLL = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(pAcq->DigName());
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

void SVConfigurationObject::SetupCameraTrigger(SVCameraTriggerClass* pTriggerDevice, int iDigNum, SVPPQObject* pPPQ, bool bSoftwareTrigger, long triggerPeriod)
{
	ASSERT( nullptr != pTriggerDevice && nullptr != pPPQ);
	if( nullptr == pTriggerDevice || nullptr == pPPQ )
	{
		return;
	}

	if (bSoftwareTrigger)
	{
		CString sDeviceName = SVHardwareManifest::BuildSoftwareTriggerDeviceName(iDigNum).ToString();
		SVTriggerClass* psvDevice = SVTriggerProcessingClass::Instance().GetTrigger( sDeviceName );
		if (psvDevice)
		{
			psvDevice->Create();
		}

		SVSoftwareTriggerClass* pSoftwareTriggerDevice = dynamic_cast<SVSoftwareTriggerClass*>(psvDevice);
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
							pTriggerDevice->m_ulHandle = pCamera->mpsvDevice->m_hDigitizer;
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

		SVCameraTriggerData& rCameraTriggerData = pPPQ->GetCameraInputData();
		SVIOEntryHostStructPtr ioEntry = rCameraTriggerData.SetupLineStateInput(depth);
		AddCameraDataInput(pPPQ, ioEntry);

		ioEntry = rCameraTriggerData.SetupTimestampInput(depth);
		AddCameraDataInput(pPPQ, ioEntry);
	}
}

HRESULT SVConfigurationObject::AttachAcqToTriggers()
{
	HRESULT hr = S_OK;

	// SEJ - for Software Triggers
	// Iterate thru Trigger List and Connect Acquistion Initiator for Software Triggers
	// the channel number for the trigger object must be set at this point, it represents the Digitizer Number
	// Set the Software Timer Trigger period as well here
	long lCount = GetTriggerCount( );

	BOOL bOk = true;
	for ( long l = 0; bOk && l < lCount; l++ )
	{
		SVTriggerObject *pTrigger = GetTrigger( l );
		bOk = ( nullptr != pTrigger );
		if ( bOk )
		{
			if (pTrigger->IsAcquisitionTrigger())
			{
				int iDigNum = pTrigger->mpsvDevice->miChannelNumber;
				SVCameraTriggerClass* pTriggerDevice = dynamic_cast<SVCameraTriggerClass*>(pTrigger->mpsvDevice);
				if( nullptr != pTriggerDevice )
				{
					SVPPQObject* pPPQ = reinterpret_cast<SVPPQObject*>(pTrigger->m_pOwner);
					if( nullptr != pPPQ){ SetupCameraTrigger(pTriggerDevice, iDigNum, pPPQ, pTrigger->IsSoftwareTrigger(), pTrigger->GetSoftwareTriggerPeriod()); }
				}
			}
			else if (pTrigger->IsSoftwareTrigger())
			{
				int iDigNum = pTrigger->mpsvDevice->miChannelNumber;
				SVSoftwareTriggerClass* pTriggerDevice = dynamic_cast<SVSoftwareTriggerClass *>(pTrigger->mpsvDevice);

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
								hr = pCamera->RegisterTriggerRelay(pTrigger->mpsvDevice->m_pDLLTrigger, pTrigger->mpsvDevice->m_ulHandle);
							}
						}
					}
				}
			}
		}
	}

	return hr;
}

void SVConfigurationObject::ConvertLightReferenceEnum(DWORD &dwType)
{
	switch ( dwType )
	{
		case SVCorLightReferenceTypeBrightness :
		{
			dwType = SVLightReferenceTypeBrightness;
			break;
		}
		case SVCorLightReferenceTypeBrightnessRed :   
		{	
			dwType = SVLightReferenceTypeBrightnessRed;
			break;
		}
		case SVCorLightReferenceTypeBrightnessGreen :
		{
			dwType = SVLightReferenceTypeBrightnessGreen;
			break;
		}
		case SVCorLightReferenceTypeBrightnessBlue :
		{
			dwType = SVLightReferenceTypeBrightnessBlue;
			break;
		}
		case SVCorLightReferenceTypeContrast :
		{
			dwType = SVLightReferenceTypeContrast;
			break;
		}
		case SVCorLightReferenceTypeContrastRed : 
		{
			dwType = SVLightReferenceTypeContrastRed;
			break;
		}
		case SVCorLightReferenceTypeContrastGreen :
		{
			dwType = SVLightReferenceTypeContrastGreen;
			break;
		}
		case SVCorLightReferenceTypeContrastBlue :
		{
			dwType = SVLightReferenceTypeContrastBlue;
			break;
		}
		default:
		{
			break;
		}
	}
}

HRESULT SVConfigurationObject::SetModuleReady( bool p_Value )
{
	HRESULT l_Status( S_OK );

	if( m_pIOController != NULL )
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

	if( m_pIOController != NULL )
	{
		l_Status = m_pIOController->SetRaidErrorBit( p_Value );
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

	if( m_pIOController != NULL )
	{
		l_IOEntryPtr = m_pIOController->GetModuleReady();
	}

	return l_IOEntryPtr;
}

SVIOEntryHostStructPtr SVConfigurationObject::GetRaidErrorBit()
{
	SVIOEntryHostStructPtr l_IOEntryPtr;

	if( m_pIOController != NULL )
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

	if( m_pIOController != NULL )
	{
		l_ObjectId = m_pIOController->GetUniqueObjectID();
	}

	return l_ObjectId;
}



SVGUID SVConfigurationObject::GetRemoteOutputController() const
{
	SVGUID l_ObjectId;

	if( m_pIOController != NULL )
	{
		l_ObjectId = m_pIOController->GetRemoteOutputController();
	}

	return l_ObjectId;
}

size_t SVConfigurationObject::GetRemoteOutputGroupCount() const
{
	size_t l_Count = 0;

	if( m_pIOController != NULL )
	{
		l_Count = m_pIOController->GetRemoteOutputGroupCount();
	}

	return l_Count;
}

void SVConfigurationObject::SetupRemoteOutput()
{
	if( m_pIOController != NULL )
	{
		m_pIOController->SetupRemoteOutput( this );
	}
}

HRESULT SVConfigurationObject::ClearRemoteOutputUnUsedData()
{
	HRESULT l_Status = S_OK;

	if( m_pIOController != NULL )
	{
		l_Status = m_pIOController->ClearRemoteOutputUnUsedData();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

SVRemoteOutputGroup* SVConfigurationObject::GetRemoteOutputGroup( const CString& p_strRemoteGroupID ) const
{
	SVRemoteOutputGroup* l_pObject = NULL;

	if( m_pIOController != NULL )
	{
		l_pObject = m_pIOController->GetRemoteOutputGroup( p_strRemoteGroupID );
	}

	return l_pObject;
}

HRESULT SVConfigurationObject::GetRemoteOutputGroupNames( std::vector<CString>& p_astrPPQs ) const
{
	HRESULT l_Status = S_OK;

	if( m_pIOController != NULL )
	{
		l_Status = m_pIOController->GetRemoteOutputGroupNames( p_astrPPQs );
	}
	else
	{
		p_astrPPQs.clear();

		l_Status = E_FAIL;
	}

	return l_Status;
}

size_t SVConfigurationObject::GetRemoteOutputGroupItemCount( const CString& p_strRemoteGroupID ) const
{
	size_t l_Count = 0;

	if( m_pIOController != NULL )
	{
		l_Count = m_pIOController->GetRemoteOutputGroupItemCount( p_strRemoteGroupID );
	}

	return l_Count;
}

HRESULT SVConfigurationObject::GetRemoteOutputItem( const CString& p_strRemoteGroupId, long l_lIndex, SVRemoteOutputObject*& p_rItem ) const
{
	HRESULT l_Status = S_OK;

	if( m_pIOController != NULL )
	{
		l_Status = m_pIOController->GetRemoteOutputItem( p_strRemoteGroupId, l_lIndex, p_rItem );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

SVRemoteOutputObject* SVConfigurationObject::GetFirstRemoteOutputObject( const CString& p_strRemoteGroupId ) const
{
	SVRemoteOutputObject* l_pObject = NULL;

	if( m_pIOController != NULL )
	{
		l_pObject = m_pIOController->GetFirstRemoteOutputObject( p_strRemoteGroupId );
	}

	return l_pObject;
}

HRESULT SVConfigurationObject::AddRemoteOutputItem( const CString& p_strRemoteGroupId, SVRemoteOutputObject*& p_pNewOutput, GUID p_InputObjectID, const CString p_strPPQ )
{
	HRESULT l_Status = S_OK;

	if( m_pIOController != NULL )
	{
		l_Status = m_pIOController->AddRemoteOutputItem( p_strRemoteGroupId, p_pNewOutput, p_InputObjectID, p_strPPQ );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVConfigurationObject::DeleteRemoteOutput( const CString& p_strRemoteGroupId )
{
	HRESULT l_Status = S_OK;

	if( m_pIOController != NULL )
	{
		l_Status = m_pIOController->DeleteRemoteOutput( p_strRemoteGroupId );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVConfigurationObject::DeleteRemoteOutputEntry( const CString& p_strRemoteGroupId, SVRemoteOutputObject* p_pOutputObject )
{
	HRESULT l_Status = S_OK;

	if( m_pIOController != NULL )
	{
		l_Status = m_pIOController->DeleteRemoteOutputEntry( p_strRemoteGroupId, p_pOutputObject );
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

	if( m_pIOController != NULL )
	{
		l_Status = m_pIOController->RemoteOutputValidateInputs();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

bool SVConfigurationObject::RenameOutputListInspectionNames(CString& OldInspectionName, CString& NewInspectionName)
{	
	return m_pOutputObjectList->RenameInspection( OldInspectionName, NewInspectionName);
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

			if( SvOl::FqnInspections == l_Info.m_NameArray[ 0 ] )
			{
				SVObjectReference ref;
				SVObjectManagerClass::Instance().GetObjectByDottedName( l_Info.GetObjectArrayName( 0 ), ref );

				if( nullptr != ref.Object() )
				{
					SVInspectionProcess* pInspection = nullptr;

					GetInspectionObject( l_Info.GetObjectArrayName( 1 ).c_str(), &pInspection );

					if( nullptr != pInspection )
					{
						l_Inspections[ pInspection->GetName() ] = pInspection;
						SVCommandInspectionGetItems::SVFullNameObjectPair newPair(*l_Iter, ref);
						l_InspectionItems[ pInspection->GetName() ].insert(newPair);
					}
					else
					{
						p_rItems[ l_Iter->c_str() ] = SVStorageResult( SVStorage(), SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST, 0 );

						if( S_OK == l_Status )
						{
							l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
						}
					}
				}
				else
				{
					p_rItems[ l_Iter->c_str() ] = SVStorageResult( SVStorage(), SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST, 0 );

					if( S_OK == l_Status )
					{
						l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
					}
				}
			}
			else
			{
				p_rItems[ l_Iter->c_str() ] = SVStorageResult( SVStorage(), SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST, 0 );

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
			SVClock::SVTimeStamp l_EndTimeStamp = SVClock::GetTimeStamp() + SVClock::ConvertFrom( SVClock::Minutes, 10.0 );

			while( !( l_AsyncCommands.empty() ) && ( SVClock::GetTimeStamp() < l_EndTimeStamp ) )
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

			if( SvOl::FqnRemoteInputs == l_Info.m_NameArray[ 0 ] )
			{
				SVRemoteInputObject* l_pInput = NULL;

				SVObjectManagerClass::Instance().GetObjectByDottedName( l_Info.GetObjectArrayName( 0 ), l_pInput );

				if( l_pInput != NULL )
				{
					_variant_t l_Value;

					HRESULT l_TempStatus = l_pInput->Read( l_Value );

					if( l_TempStatus == S_OK )
					{
						p_rItems[ l_Iter->c_str() ] = SVStorageResult( SVStorage( SVVisionProcessor::SVStorageValue, l_Value ), S_OK, 0 );
					}
					else
					{
						p_rItems[ l_Iter->c_str() ] = SVStorageResult( SVStorage(), E_INVALIDARG, 0 );

						if( l_Status == S_OK )
						{
							l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
						}
					}
				}
				else
				{
					p_rItems[ l_Iter->c_str() ] = SVStorageResult( SVStorage(), SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST, 0 );

					if( l_Status == S_OK )
					{
						l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
					}
				}
			}
			else
			{
				p_rItems[ l_Iter->c_str() ] = SVStorageResult( SVStorage(), SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST, 0 );

				if( l_Status == S_OK )
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

			if( l_Info.m_NameArray.size() >0 &&  SvOl::FqnInspections == l_Info.m_NameArray[ 0 ] )
			{
				SVObjectReference ref;
				SVObjectManagerClass::Instance().GetObjectByDottedName( l_Info.GetObjectArrayName( 0 ), ref );

				if( ref.Object() != nullptr )
				{
					///someone wants to set this variable check if this is allowed
					bool l_AddParameter = ( ( ref.ObjectAttributesAllowed() & SV_REMOTELY_SETABLE ) == SV_REMOTELY_SETABLE );

					if( l_AddParameter )
					{
						l_AddParameter = !l_Online || ( ( ref.ObjectAttributesAllowed() & SV_SETABLE_ONLINE ) == SV_SETABLE_ONLINE );

						if( !l_AddParameter )
						{
							p_rStatus[ l_Iter->first ] = SVMSG_OBJECT_CANNOT_BE_SET_WHILE_ONLINE;

							if( l_Status == S_OK )
							{
								l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
							}
						}

						bool rangeParameter = RangeClassHelper::IsOwnedByRangeObject(*ref.Object());

						if( l_AddParameter && rangeParameter )
						{
							HRESULT hresult;
							CString csValue;
							if( l_Iter->second.m_Variant.vt == (VT_BSTR | VT_ARRAY) )
							{
								long  index=0;
								BSTR bstrValue = nullptr;
								SafeArrayGetElementNoCopy( l_Iter->second.m_Variant.parray, &index,&bstrValue);
								csValue = bstrValue;
							}

							if (!RangeClassHelper::IsAllowedToSet(*ref.Object(),csValue,l_Online, hresult))
							{
								p_rStatus[ l_Iter->first ] = hresult;
								l_AddParameter = false;
								if( l_Status == S_OK )
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
								SVImageClass* l_pImage = dynamic_cast< SVImageClass* >( ref.Object() );

								if( l_pImage != nullptr )
								{
									p_rStatus[ l_Iter->first ] = pInspection->AddInputImageFileNameRequest( l_pImage, l_Iter->second.m_Variant );
								}
								else
								{
									p_rStatus[ l_Iter->first ] = E_INVALIDARG;

									if( l_Status == S_OK )
									{
										l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
									}
								}
							}
							else if( l_Iter->second.m_StorageType == SVVisionProcessor::SVStorageValue )
							{
								if( pInspection->AddInputRequest( ref, l_Iter->second.m_Variant ) )
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
					SVCommandInspectionRunOncePtr l_CommandPtr = new SVCommandInspectionRunOnce( pInspection->GetUniqueObjectID() );
					SVObjectSynchronousCommandTemplate< SVCommandInspectionRunOncePtr > l_Command( pInspection->GetUniqueObjectID(), l_CommandPtr );

					l_Command.Execute( 120000 );
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

			if( SvOl::FqnRemoteInputs == l_Info.m_NameArray[ 0 ] )
			{
				SVRemoteInputObject* l_pInput = NULL;

				SVObjectManagerClass::Instance().GetObjectByDottedName( l_Iter->first, l_pInput );

				if( l_pInput != NULL )
				{
					HRESULT l_LoopStatus = l_pInput->WriteCache( l_Iter->second.m_Variant );

					p_rStatus[ l_Iter->first ] = l_LoopStatus;

					if( l_Status == S_OK && l_LoopStatus != S_OK )
					{
						l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
					}
				}
				else
				{
					p_rStatus[ l_Iter->first ] = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;

					if( l_Status == S_OK )
					{
						l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
					}
				}
			}
			else
			{
				p_rStatus[ l_Iter->first ] = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;

				if( l_Status == S_OK )
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

						if( pInspection != NULL )
						{
							l_Inspections[ pInspection->GetName() ] = pInspection;

							SVString l_Name;
							SVObjectReference ref;

							l_Name += pInspection->GetName();
							l_Name += ".";
							l_Name += l_Info.GetObjectName( 1 );

							SVObjectManagerClass::Instance().GetObjectByDottedName( l_Name, ref );

							if( ref.Object() != NULL )
							{
								if( pInspection->AddInputRequest( ref, l_Iter->second.m_Variant ) )
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
					SVCommandInspectionRunOncePtr l_CommandPtr = new SVCommandInspectionRunOnce( pInspection->GetUniqueObjectID() );
					SVObjectSynchronousCommandTemplate< SVCommandInspectionRunOncePtr > l_Command( pInspection->GetUniqueObjectID(), l_CommandPtr );

					l_Command.Execute( 120000 );
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

			if( SvOl::FqnCameras == Info.m_NameArray[ 0 ] )
			{
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

	//Check which cameras device parameters have changed
	for( SVVirtualCameraPtrSet::iterator l_Iter = CamerasChanged.begin(); l_Iter != CamerasChanged.end(); ++l_Iter )
	{
		SVDeviceParamCollection CameraParameters;
		HRESULT LoopStatus = (*l_Iter)->UpdateDeviceParameters(CameraParameters);
		if(S_OK == LoopStatus)
		{
			CString DeviceName = (*l_Iter)->GetAcquisitionDevice()->GetRootDeviceName();
			ModifyAcquisitionDevice(DeviceName, &CameraParameters);
			SVLightReference LightRef;
			(*l_Iter)->GetAcquisitionDevice()->GetLightReference(LightRef);
			ModifyAcquisitionDevice(DeviceName, LightRef );
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
			SVObjectReference ref;
			SVObjectNameInfo l_Info;

			SVObjectNameInfo::ParseObjectName( l_Info, p_rRemoteInputName );

			l_Name += l_pInspection->GetName();
			l_Name += ".";
			l_Name += l_Info.GetObjectName( 1 );

			SVObjectManagerClass::Instance().GetObjectByDottedName( l_Name, ref );

			if( ref.Object() != NULL )
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
		SVTriggerObject *pTrigger = GetTrigger( i );
		if ( nullptr != pTrigger )
		{
			if (pTrigger->IsAcquisitionTrigger())
			{
				SVCameraTriggerClass* pTriggerDevice = dynamic_cast<SVCameraTriggerClass*>(pTrigger->mpsvDevice);
				if( pTriggerDevice != NULL )
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

void SVConfigurationObject::updateConfTreeToNewestVersion(SVXMLMaterialsTree &rTree, SVXMLMaterialsTree::SVBranchHandle &rToolset)
{
	SVXMLMaterialsTree::SVBranchHandle lutEquationBranch;
	
	//look in all children of the toolset if it had to add information
	SVTreeType::SVBranchHandle htiSubChild = nullptr;
	rTree.GetFirstBranch( rToolset, htiSubChild );
	while ( nullptr != htiSubChild )
	{
		//check for LUT Equation clip-value
		if ( SVNavigateTreeClass::FindBranch( rTree, htiSubChild, SVFindPredicate( rTree, IDS_CLASSNAME_SVLUTEQUATION ), lutEquationBranch ) )
		{
			SVXMLMaterialsTree::SVBranchHandle lutEquationClipBranch;
			SVXMLMaterialsTree::SVBranchHandle lutEquationEmbeddedsBranch;
			if ( SVNavigateTreeClass::FindBranch( rTree, lutEquationBranch, SVFindPredicate( rTree, scEmbeddedsTag ), lutEquationEmbeddedsBranch ) )
			{
				//check if clip for LUT equation is existing.
				if ( !SVNavigateTreeClass::FindBranch( rTree, lutEquationEmbeddedsBranch, SVFindPredicate( rTree, IDS_OBJECTNAME_LUT_EQUATION_CLIP ), lutEquationClipBranch ) )
				{
					//add clip value to tree, with value FALSE
					SVConfigurationTreeWriter< SVXMLMaterialsTree > writer(rTree, lutEquationEmbeddedsBranch);
					SVBoolValueObjectClass isLUTFormulaClipped;
					isLUTFormulaClipped.SetResetOptions( false, SVResetItemTool );
					isLUTFormulaClipped.SetObjectEmbedded(SVLUTEquationClipFlagObjectGuid, nullptr, IDS_OBJECTNAME_LUT_EQUATION_CLIP);
					isLUTFormulaClipped.SetDefaultValue( TRUE, TRUE );
					isLUTFormulaClipped.SetValue(0, FALSE);
					isLUTFormulaClipped.Persist(writer);
				}
			}
		}
		rTree.GetNextBranch( rToolset, htiSubChild );
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

RemoteMonitorList SVConfigurationObject::GetRemoteMonitorList() const
{
	if (nullptr != m_pIOController)
	{
		return m_pIOController->GetRemoteMonitorList();
	}
	return RemoteMonitorList();
}

void SVConfigurationObject::SetRemoteMonitorList(const RemoteMonitorList& rList)
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
		SendMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),SV_REGISTER_MONITOR_LIST, NULL, 0);
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

void SVConfigurationObject::GetActiveRemoteMonitorList(RemoteMonitorList& rActiveList) const
{
	rActiveList.clear();
	if (nullptr != m_pIOController)
	{
		m_pIOController->GetActiveRemoteMonitorList(rActiveList);
	}
}

HRESULT SVConfigurationObject::GetRemoteMonitorListProductFilter(const SVString& listName, SVProductFilterEnum& rFilter) const
{
	HRESULT hr = E_POINTER;
	if (nullptr != m_pIOController)
	{
		hr = m_pIOController->GetRemoteMonitorListProductFilter(listName, rFilter);
	}
	return hr;

}
HRESULT SVConfigurationObject::SetRemoteMonitorListProductFilter(const SVString& listName, SVProductFilterEnum filter)
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
	SVTreeType::SVBranchHandle htiChild = nullptr;
	if ( SVNavigateTreeClass::GetItemBranch( rTree, CTAG_MONITOR_LISTS, nullptr, htiChild ) )
	{
		RemoteMonitorList lists;
		SVTreeType::SVBranchHandle htiSubChild = nullptr;
		rTree.GetFirstBranch( htiChild, htiSubChild );
		while ( S_OK == result && nullptr != htiSubChild )
		{
			_bstr_t listName;
			SVString ppqName = "";
			int rejectDepth = 0;
			result = rTree.GetBranchName( htiSubChild, listName.GetBSTR() );
			_variant_t svValue = 0.0;
			if ( S_OK == result )
			{
				if ( SVNavigateTreeClass::GetItem( rTree, CTAG_PPQ_NAME, htiSubChild, svValue ) )
				{
					ppqName = static_cast< LPCTSTR >( static_cast< _bstr_t >( svValue ) );
				}
				else
				{
					result = SVMSG_SVO_48_LOAD_CONFIGURATION_MONITOR_LIST;
				}
			}

			if ( S_OK == result )
			{
				if ( SVNavigateTreeClass::GetItem( rTree, CTAG_REJECT_QUEUE_DEPTH, htiSubChild, svValue ) )
				{
					rejectDepth = svValue;
				}
				else
				{
					result = SVMSG_SVO_48_LOAD_CONFIGURATION_MONITOR_LIST;
				}
			}

			MonitoredObjectList productValueList;
			if ( S_OK == result )
			{
				result = LoadMonitoredObjectList( rTree, htiSubChild, CTAG_PRODUCTVALUES_LIST, productValueList );	
			}
			MonitoredObjectList productImageList;
			if ( S_OK == result )
			{
				result = LoadMonitoredObjectList( rTree, htiSubChild, CTAG_PRODUCTIMAGE_LIST, productImageList );	
			}
			MonitoredObjectList rejectConditionList;
			if ( S_OK == result )
			{
				result = LoadMonitoredObjectList( rTree, htiSubChild, CTAG_REJECTCONDITION_LIST, rejectConditionList );	
			}
			MonitoredObjectList failStatusList;
			if ( S_OK == result )
			{
				result = LoadMonitoredObjectList( rTree, htiSubChild, CTAG_FAILSTATUS_LIST, failStatusList );	
			}
			if (S_OK == result)
			{
				RemoteMonitorNamedList monitorList( ppqName, listName, productValueList, productImageList, rejectConditionList, failStatusList, rejectDepth );
				lists[listName] = monitorList;

				rTree.GetNextBranch( htiChild, htiSubChild );
			}
		}
		if ( S_OK == result )
		{
			SetRemoteMonitorList(lists);
		}
	}
	return result;
}

HRESULT SVConfigurationObject::LoadMonitoredObjectList( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent, const SVString& listName, MonitoredObjectList& rList )
{
	HRESULT retValue = S_OK;
	SVTreeType::SVBranchHandle htiChild = nullptr;
	// search for the branch by list name
	if ( SVNavigateTreeClass::GetItemBranch( rTree, listName.c_str(), htiParent, htiChild ) )
	{
		SVTreeType::SVLeafHandle htiLeaf;
		rTree.GetFirstLeaf( htiChild, htiLeaf );
		while ( S_OK == retValue && S_OK == rTree.IsValidLeaf( htiChild, htiLeaf ) )
		{
			_bstr_t name;
			rTree.GetLeafName( htiLeaf, name.GetBSTR() );

			const MonitoredObject& rObj = RemoteMonitorListHelper::GetMonitoredObjectFromName(name);
			if (!rObj.guid.empty())
			{
				// add object for this leaf to the list
				rList.push_back(rObj);
				rTree.GetNextLeaf( htiChild, htiLeaf );
			}
			else
			{
				retValue = SVMSG_SVO_48_LOAD_CONFIGURATION_MONITOR_LIST;
			}
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
	if ( SVNavigateTreeClass::GetItemBranch( rTree, CTAG_GLOBAL_CONSTANTS, nullptr, hBranch ) )
	{
		SVTreeType::SVBranchHandle hChild( nullptr );
		rTree.GetFirstBranch( hBranch, hChild );
		while ( S_OK == Result && nullptr != hChild )
		{
			_bstr_t GlobalConstantName;
			Result = rTree.GetBranchName( hChild, GlobalConstantName.GetBSTR() );

			_variant_t Value;
			CString Description;
			if ( S_OK == Result )
			{
				if ( SVNavigateTreeClass::GetItem( rTree, CTAG_DESCRIPTION, hChild, Value ) )
				{
					Description = Value.bstrVal;
					//This is needed to insert any CR LF in the description which were replaced while saving
					::SVRemoveEscapedSpecialCharacters( Description, true );
				}
				else
				{
					Result = SVMSG_SVO_63_LOAD_GLOBAL_CONSTANTS;
				}

				Value.Clear();
				if ( !SVNavigateTreeClass::GetItem( rTree, CTAG_VALUE, hChild, Value ) )
				{
					Result = SVMSG_SVO_63_LOAD_GLOBAL_CONSTANTS;
				}

				if( S_OK == Result )
				{
					BasicValueObjectPtr pValue( nullptr );
					pValue = RootObject::setRootChildValue( GlobalConstantName, Value );
					if( pValue.empty() )
					{
						Result = SVMSG_SVO_63_LOAD_GLOBAL_CONSTANTS;
					}
					else
					{
						pValue->setDescription( Description );
						// If type string then remove Selectable for Equation flag.
						if( VT_BSTR == Value.vt )
						{
							pValue->ObjectAttributesAllowedRef() &= ~SV_SELECTABLE_FOR_EQUATION;
						}
					}
				}
			}
			rTree.GetNextBranch( hBranch, hChild );
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVConfigurationObject.cpp_v  $
 * 
 *    Rev 1.37   19 Dec 2014 15:01:46   mEichengruen
 * Project:  SVObserver
 * Change Request (SCR) nbr:  979
 * SCR Title:  Provide additional options to input the feature range for the blob analyzer.
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   check if it is allowed to set indirectdirect values for range Object
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   17 Dec 2014 07:08:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  941
 * SCR Title:  Update SVObserver Version Number for the 7.10 Release
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added Thread Manager Enable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   12 Dec 2014 13:08:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  918
 * SCR Title:  Implement Method RegisterMonitorList for RemoteControl (SVO-369)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed ReplaceOrAddMonitorList to send a message so that the IO page will add/show the Monitor List tab.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   01 Dec 2014 13:07:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added thread manager serialization.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   20 Nov 2014 05:01:58   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  918
 * SCR Title:  Implement Method RegisterMonitorList for RemoteControl (SVO-369)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   add method ReplaceOrAddMonitorList
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   13 Nov 2014 10:10:12   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  932
 * SCR Title:  Clean up GetInspectionItems and SVCommandInspectionGetItemsPtr (SVO-150)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   use inspection-object instead of ID and SVObjectClass instead of SVGUID in method GetInspectionItems
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   29 Aug 2014 17:49:04   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added support for get/set product filter.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   29 Aug 2014 15:43:14   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  934
 * SCR Title:  Add Remote Access to Environment.Mode Parameters
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Removed GetMode.  Use SVSVIMStateClass::GetMode instead.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   04 Aug 2014 07:24:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  893
 * SCR Title:  Fix Camera Index Issue for Digital Cameras (e115)
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added GetRe-ordered Cameras function to insure the correct camera order when they are re-ordered with the camera manager.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   17 Jul 2014 18:35:56   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Removed namespaces and code review changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   08 Jul 2014 09:01:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised SaveMonitoredObjectList to use RemoteMonitorListHelper.
 * Revised LoadMonitoredObjectList to use RemoteMonitorListHelper.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   18 Jun 2014 11:28:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Conditionally added the PLC code for 32bit platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   02 Jun 2014 09:34:28   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  900
 * SCR Title:  Separate View Image Update, View Result Update flags; remote access E55,E92
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added loading and saving the Image and Result display update flags into the configuration.
 * Set the default and conversion (CTAG_ONLINE_DISPLAY)  values for the flags when loading.
 * Optimize the LoopStatus for the method SetCameraItems.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   15 May 2014 11:10:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised SVSendMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   28 Apr 2014 14:22:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added BuildPPQMonitorList method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   24 Apr 2014 10:47:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised the GetActiveRemoteMonitorList method to use a reference rather than return a copy of the list.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   23 Apr 2014 10:39:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added ActivateRemoteMonitorList and GetActiveRemoteMonitorList methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   22 Apr 2014 09:44:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added ValidateRemoteMonitorList method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   17 Apr 2014 16:58:36   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new methods for the Remote Monitor List
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   17 Mar 2014 15:20:24   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  869
 * SCR Title:  Add PPQ and Environment Variables to Object Manager and Update Pickers
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed ModifyAcquisitionDevice, LoadConfiguration,  GetChildObject, GetInspectionItems, GetRemoteInputItems, SetInspectionItems, SetRemoteInputItems for new objects.
 *   Added method SetCameraItems.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   07 Mar 2014 18:09:38   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added regions.
 *   Added DEBUG_NEW.
 *   Removed empty method DetachAcqFromTriggers.
 *   Made methods const.
 *   Various code changes to better follow coding guidelines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   01 Feb 2014 10:23:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed sendmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   31 Jan 2014 17:16:26   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed to follow guidelines more closely.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   30 Oct 2013 10:45:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added #ifndef _WIN64 to prevent depricated PLC code from compiling in 64bit.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   01 Oct 2013 12:16:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   07 Aug 2013 13:31:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Renamed SVIOEntryHostStructPtrVector to SVIOEntryHostStructPtrList.
 * Added HasCameraTrigger method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   01 Aug 2013 10:36:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  849
 * SCR Title:  Modify the Remove Unused Outputs function to check PPQ names
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added SVOutputObjectList StringVect typedef to ValidateOutputList for consistancy.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   30 Jul 2013 13:07:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  849
 * SCR Title:  Modify the Remove Unused Outputs function to check PPQ names
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added parameter (list of valid PPQs)to the function RemoveUnusedOutputs. This allows us to remove outputs  associated with a deleted PPQ.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   09 Jul 2013 13:28:12   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 023.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.147   03 Jul 2013 13:45:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised AttachAcqToTriggers method to not call the trigger object's SetAcquisitionTriggered method.
 * Revised SetupCameraTrigger to assign the digitizer handle to the trigger handle.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   05 Jul 2013 15:01:26   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 021.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.146   14 Jun 2013 13:58:00   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  824
 * SCR Title:  Add Remote Access to SVObserver Application Configuration Variables
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed invalid iterator in GetInspectionItems.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   12 Jun 2013 15:19:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 018.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.145   23 May 2013 08:51:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  824
 * SCR Title:  Add Remote Access to SVObserver Application Configuration Variables
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated set remote inputs method to assign correct status code to each elements.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   11 Jun 2013 15:26:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 017.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.144   21 May 2013 15:45:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  824
 * SCR Title:  Add Remote Access to SVObserver Application Configuration Variables
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated get child object method to not set error status when object is found.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   13 May 2013 12:15:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.143   21 May 2013 13:00:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  824
 * SCR Title:  Add Remote Access to SVObserver Application Configuration Variables
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated configuration object to include the new naming requirements found in IF00100.9401.003.  Also add new funcitonality to get and set remote inputs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.142   07 May 2013 13:20:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use new SVClock conversions functions, which will clarify timing tolerance checks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   08 May 2013 16:02:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.141   29 Apr 2013 14:06:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   07 May 2013 08:20:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 011.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.140   19 Apr 2013 13:19:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  822
 * SCR Title:  Remove DDE Input and Output functionality from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed SavePPQ by putting back pPPQ->PersistInputs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   06 May 2013 20:05:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.139   16 Apr 2013 14:15:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  822
 * SCR Title:  Remove DDE Input and Output functionality from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed DDE functionality from the source code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 May 2013 14:38:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 009.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.138   10 Apr 2013 11:20:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix error status of methods that did not get changed correctly when the Inspection Process Add Inputs methods were changed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:02:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.137   05 Apr 2013 12:26:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated method that uses inspection method with changed return type from BOOL to HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.136   20 Mar 2013 14:11:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  817
 * SCR Title:  Import/Export of Inspections
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised AddImportedDDEInput to only adjust an existing input if it's not enabled.
 * Revised AddImportedRemoteInput to only adjust an existing input if it's not enabled.
 * Revised AddImportedDigitalInput to only adjust an existing input if it's not enabled.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.135   20 Mar 2013 13:36:50   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  817
 * SCR Title:  Import/Export of Inspections
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised AddImportedDDEInput to only import the input if it's not used.
 * Revised AddImportedDigitalInput to only import the input if it's not used.
 * Revised AddImportedRemoteInput to only import the input if it's not used.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.134   19 Mar 2013 14:45:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  817
 * SCR Title:  Import/Export of Inspections
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Renamed AddOrUpdateDigitalInput to AddImportedDigitalInput.
 * Renamed AddOrUpdateDDEInput to AddImportedDDEInput.
 * Renamed AddOrUpdateRemoteInput to AddImportedRemoteInput.
 * Revised AddImportedDigitalInput to only add the input is it doesn't already exists.
 * Revised AddImportedDDEInput to only add the input is it doesn't already exists.
 * Revised AddImportedRemoteInput to only add the input is it doesn't already exists.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.133   15 Mar 2013 13:03:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised AttachAcqToTrigger to remove dead code
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.132   18 Feb 2013 13:25:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to allow the socket interface to process the set items command.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.131   14 Feb 2013 11:04:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update configuration object get configuraiton items method to use the new asynchrous fucntionality to get the data from the inspections.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.130   11 Feb 2013 14:16:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new framework for the remote socket interface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.129   21 Jan 2013 10:50:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added GetPPQByName method
 * Added AddCameraDataInput method
 * Added SetupSoftwareTrigger method
 * Added SetupCameraTrigger method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.128   16 Jan 2013 16:02:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Migrating branch code into primary code base.  
 * 
 * Consolidated adding and updating inputs to configuration elements to configuratoin object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.127   11 Oct 2012 10:54:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information based on the navigate tree class moving to the XML library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.126   11 Oct 2012 10:49:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Change from post to pre incrementer.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.125   28 Sep 2012 14:33:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Upated code to use new templated classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.124   25 Sep 2012 15:16:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new tree type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.123   18 Sep 2012 18:19:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library and to the Configuration Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.122   06 Sep 2012 09:54:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Load Configuration and Save Configuration methods to restructure the data in the configuration file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.121   04 Sep 2012 15:27:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Navigate Tree class to not force the programmer to create an instance of the class to use it.  Converted all functionality to static.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.120   30 Aug 2012 09:37:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  786
 * SCR Title:  Fix Problems with IEEE 1394 Light Reference and LUT Camera Parameters
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the Load Configuration method in the Configuration object to synchronize camara parameters with acquisition device.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.119   15 Aug 2012 14:30:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new Singlton version of the SVIOConfigurationInterfaceClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.118   14 Aug 2012 15:56:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated output list functionality to only allow unique elements to the output list and notify user when duplicates are detected.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.117   13 Aug 2012 14:24:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to change the functionality associated with the Input Object List to use a new method to get the input from the input list.  If the input does not exist in the input list, a NULL pointer is returned.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.116   07 Aug 2012 10:44:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Load Configuration method to fix an issue with the object assignment of the DDE Inputs and DDE Outputs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.115   01 Aug 2012 11:44:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new command functionality to allow for synchronous command execution through the inspection thread.  This will fix problems with race conditions with the inspection process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.114   17 Jul 2012 15:47:34   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  771
 * SCR Title:  Upgrade to the Next Version of Matrox Imaging Library (MIL)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   SaveConfiguration( CTreeCtrl ):  Removed unnecessary ASSERTs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.113   02 Jul 2012 16:39:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.112   28 Jun 2012 17:40:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  768
 * SCR Title:  Add the display time-out and throttling options for 5.50
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Reverted timeout value back to original value.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.111   18 Jun 2012 18:15:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Centralized the location of the configuration object identifier and removed synchronization problems.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.110   16 May 2012 18:57:42   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  763
 * SCR Title:  Add the display time-out and throttling options.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Merge 5.43 changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.109   13 Mar 2012 13:23:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  745
 * SCR Title:  Fix contention issue during load process and display
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated Activate method to move the message call to a dialog scope.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.108   05 Mar 2012 13:46:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  755
 * SCR Title:  Fix saving of "forced outputs"
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added bool cast to the temperary value of type _variant _t  where it is assigned to the dword forced value.  This will prevent an illegal value from being assigned.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.107   31 Jan 2012 15:24:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Fixed GetRemoteOutputCount
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.106   30 Jan 2012 11:09:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Modified source code to consolidate PLC and New Output stream functionality to I/O Controller.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.105   20 Dec 2011 14:55:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  741
 * SCR Title:  Fix IO List when changing Inspection Name
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added function calls to rename the output list when inspection name has been changed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.104   19 Dec 2011 15:41:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the configuration object to handle new functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.103.1.0   14 May 2012 14:12:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  763
 * SCR Title:  Add the display time-out and throttling options.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Support saving new display setting
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.103   28 Sep 2011 12:54:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to fix issues with camera image interface and methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.102   16 Sep 2011 15:50:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated camera management functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.101   26 Jul 2011 07:49:30   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  733
 * SCR Title:  fix bug with Remote Inputs not being avaliable
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed issues with Remote Inputs being put into the list multiple times. 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.100   07 Jun 2011 11:32:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated loading process to store correct object ids in the iocontroller object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.99   06 Jun 2011 13:06:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  722
 * SCR Title:  Fix a problem with Raid Error Information on X2 systems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to correct issues with the RAID error information associated with the X2 products.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.98   29 Apr 2011 08:06:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to allow for inspection to for the display to give up when an inspection get queued.  The display is now regulated to a maximum of 10 frames per second.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.97   19 Apr 2011 16:07:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix issues with IO Assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.96   18 Apr 2011 09:22:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to move the shared data for the IO Entry data to a different class to fix issues with PPQ and Inspection shared data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.95   30 Mar 2011 15:56:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to clairify construction functionality and add clean-up functionality to remove memory leaks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.94   22 Mar 2011 07:45:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated static global classes to singletons to promote proper and defined construction and destruction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.93   16 Mar 2011 09:22:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  715
 * SCR Title:  Change the reset functionality for Input Object in the Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to include change in reset requirements for input value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.92   18 Feb 2011 09:58:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  700
 * SCR Title:  Remove String Buffer Problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to remove duplicate string class, and fixed string conversion issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.91   15 Dec 2010 09:49:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix connections and synchronization issues with the IO Sub-system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.90   13 Dec 2010 11:15:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Change the usage of _variant_t to clear the variable after use to eliminate invalid variant type for assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.89   08 Dec 2010 07:51:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.88   09 Nov 2010 16:09:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.87   05 Nov 2010 10:29:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove redundent data objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.86   04 Nov 2010 13:30:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.85   21 Oct 2010 11:02:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change internal value object to variant value object from double value object.  Added persistance to configuration object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.84   01 Jun 2010 10:34:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.83   15 Feb 2010 12:25:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to remove the generations of board image processing classes from the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.82   15 Dec 2009 15:13:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.81   30 Jul 2009 11:18:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.80   03 Jun 2009 09:51:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Include file definitions due to changes in SVDeviceParams
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.79   06 Feb 2009 09:21:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised LoadConfiguration to correct a problem setting whether file acquisition should be used or not, which affected the camera manager behavior.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.78   04 Dec 2008 09:33:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Load Configuration changes to support PLC
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.77   14 Nov 2008 13:54:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added code to save PLC Outputs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.76   14 Nov 2008 12:07:24   jbrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  641
 * SCR Title:  BoundsChecker results
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   For
 * SVConfigurationObject::LoadConfiguration ()
 * SVConfigurationObject::LoadAcquisitionDeviceFile ()
 * SVConfigurationObject::LoadDeviceParamSpecial ()
 * 
 * Replace delete of character string from SVOVariantClass, with SVOString::StaticDestroy ()
 * 
 * For 
 * SVConfigurationObject::FinishIPDoc ()
 *   Changed to use delete [] pbValidate instead of delete 
 *   pbValidate.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.75   23 Oct 2008 17:51:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised AttachAcqToTrigger method to reset TriggerRelay for File Acquisition if Software Trigger
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.74   17 Oct 2008 10:32:12   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed call to EnableInternalTrigger (now done in SVPPQObject)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.73   08 Oct 2008 09:33:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised AttachAcqToTriggers to use hardware based triggers with File Acquisition
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.72   03 Oct 2008 17:29:20   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Changed Frequency to Period for the Software Timer Trigger
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.71   16 Sep 2008 14:33:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added SaveFileAcquisitionConfiguration method.
 * Added LoadFileAcquisitionConfiguration method.
 * Revised LoadConfiguration to handle FileAquisition settings.
 * Revised SaveConfiguration to handle FileAquisition settings.
 * Revised call to getDigitizerSubsystem to use device name in support of multiple acquisition devices.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.70   10 Sep 2008 16:55:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added SaveAcquisitionDeviceFilename
 * Added LoadAcquisitionDeviceFilename
 * Added SaveDeviceParameters
 * Added LoadDeviceParameters
 * Revised Loading/Saving to support Software Trigger aprameters
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.69   30 Oct 2007 15:00:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   added code to free allocated memory in SaveConfiguration.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.68   21 Jun 2007 12:01:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.67.1.1   14 Jul 2009 12:44:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Udated code to use the new data manager objects and signal processing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.67.1.0   30 Oct 2007 09:17:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Free Allocated memory - added deleted ppIOEntries to Saveconfiguration.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.67   09 Apr 2007 11:54:14   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  588
 * SCR Title:  Add the RAID Error Bit for the digital IO
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new method to determine if the configuration is of a RAID type
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.66   22 Jan 2007 09:29:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  587
 * SCR Title:  Add Timeout Threshold to Extended Time Delay Mode
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added code for the new Inspection Timeout Threshold for the Extended Time Delay Mode.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.65   17 Jan 2006 10:00:46   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added GetInspections
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.64   10 Nov 2005 06:44:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update code to rename member variable for more clarity.  Used new SVObserverApp method to determine type of hardware.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.63   24 Oct 2005 09:40:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changes to support adding a flag to the Inspection that will enable Auxiliary Extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.62   08 Sep 2005 13:29:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed refresh when activating configuration to a RunOnce.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.61   31 Aug 2005 08:34:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update source code call to handle new refresh methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.60   22 Aug 2005 10:09:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to include changes to SVProductInfoStruct and supporting classes to handle proper assignment and copy construction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.59   29 Jun 2005 07:37:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  493
 * SCR Title:  Fix Bug in SVOutputObjectList
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added ValidateOutputList Function
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.58   09 Mar 2005 06:43:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed ASSERT in debug mode when camera does not have a LUT table.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.57   21 Feb 2005 10:24:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  452
 * SCR Title:  Upgrade SVObserver to version 4.50
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed unnecessary ASSERT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.56   17 Feb 2005 13:28:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.55   16 Feb 2005 13:18:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.54   19 Jan 2005 14:50:24   jBrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  375
 * SCR Title:  Remodel XML usage for configuration storage and SIAC transfers.
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   The function 
 * 
 * SVConfigurationObject::LoadConfiguration()
 * 
 * was modified to better conform to HRESULT vs BOOL standards.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.54   19 Jan 2005 14:50:16   jBrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  375
 * SCR Title:  Remodel XML usage for configuration storage and SIAC transfers.
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   The function 
 * 
 * SVConfigurationObject::LoadConfiguration()
 * 
 * was modified to better conform to HRESULT vs BOOL standards.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.53   20 Oct 2004 15:41:42   jBrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  375
 * SCR Title:  Remodel XML usage for configuration storage and SIAC transfers.
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   The return type of SVConfigurationObject::LoadConfiguration(CTreeCtrl &rTree) was changed from BOOL to HRESULT in order to better debug loading problems.
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.52   12 Jul 2004 12:53:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  428
 * SCR Title:  Improve Load Configuration time
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented get file version information
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.51   11 Jun 2004 14:10:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  410
 * SCR Title:  Fix DDE IO
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated LoadConfiguration method to fix issue with loading DDE output PPQ_x.State variable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.50   11 Jun 2004 07:50:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  410
 * SCR Title:  Fix DDE IO
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed problem with DDE State.  State will now appear as a PPQ variable PPQ_X.State
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.49   25 May 2004 08:26:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  421
 * SCR Title:  Allow SVObserver to get the Trigger Edge and Strobe Edge from the Camera File
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated LoadConfiguration method to set the trigger edge and strobe edge data into the system from the camera file data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.48   11 Feb 2004 17:30:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed compiler warnings
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.47   09 Feb 2004 14:04:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed legacy support for lut MaxValue
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.46   04 Feb 2004 12:49:22   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   made change to field name
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.45   03 Feb 2004 17:21:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added support for named device params
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.44   22 Jan 2004 12:26:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   name changed of called function
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.43   21 Jan 2004 15:28:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   made strobe start frame (un)registration conditional
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.42   16 Jan 2004 07:48:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added temporary functions to connect acquisitionclass objects to trigger signals (including the strobe start frame)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.41   12 Jan 2004 10:43:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  404
 * SCR Title:  Change Camera Parameters that represent time to use standard units
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changes to LoadConfiguration( to store Metadata in ConfigurationDeviceParams from CameraDeviceParams
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.40   15 Dec 2003 11:57:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added hack to store "native" units for device params
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.39   10 Dec 2003 09:16:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added convert function for Light Refernece values from the Coreco values to SVObservers ENUMS
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.38   21 Nov 2003 09:54:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   set strobe polarity on IO board based on Acq Device param
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.37   21 Nov 2003 09:45:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   workaround for LR loading problem on 1394
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   12 Nov 2003 13:43:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  396
 * SCR Title:  Add New Product Types for 1394 SVIM
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed enumeration names
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   13 Oct 2003 11:28:16   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  386
 * SCR Title:  Change Light Reference handling to match earlier versions of SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added code to loading a configuration to create a default LUT if one wasn't loaded from the file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   02 Oct 2003 10:48:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  386
 * SCR Title:  Change Light Reference handling to match earlier versions of SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Add new code to handle the defaulting of unset light reference values to the values in the camera files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   25 Sep 2003 14:29:06   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  381
 * SCR Title:  Combine PPQ Data to Inspection Data for a Digital Output
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Add code to support the combining of digital outputs with PPQ values.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   24 Sep 2003 07:48:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added LUT MaxValue support
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   17 Sep 2003 08:59:22   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added legacy (beta) conversion from old trigger names
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   16 Sep 2003 09:26:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated LoadConfiguration method to use new trigger member variable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   29 Aug 2003 13:13:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   don't save Null device parameters
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   18 Aug 2003 15:35:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated LoadConfiguration method to fix IO Configuration Interface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   07 Aug 2003 12:45:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   modified CreateBuffers to use GetSourceImageDepth in response to request by Emhart
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   05 Aug 2003 13:50:34   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  373
 * SCR Title:  Add a new disable method that doesn't copy forward all result values
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes to the saving/loading/editing of configurations to add a new property for the inspection's disable method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   31 Jul 2003 09:48:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added #include "SVImagingDeviceParams.h"
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   28 Jul 2003 10:44:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added support for specialized DeviceParams
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   08 Jul 2003 11:47:22   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added DeviceParam load/save for Digital
 * SVLightReferenceArray --> SVLighrReference
 * changed Digital IO / DDE handling
 * changed BOOL to HRESULT for Image Processing functions (mpsvImaging)
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   08 Jul 2003 09:15:46   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  374
 * SCR Title:  Add check before changing tool parameters when processing input list
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added call to BuildValueObjectMap to the loading process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   11 Jun 2003 13:49:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  359
 * SCR Title:  Version Check on Configurations that are saved with  SVX extensions
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new method to return the SVX version before the configuration is loaded.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   22 May 2003 10:37:24   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  355
 * SCR Title:  Save tool freeform LUT with the configuration
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed bug with the SetLut function not setting the transform method correctly and that it gets saved correctly
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   07 May 2003 15:09:40   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  341
 * SCR Title:  DDE Input and Outputs are no longer working correctly
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Modified code so that some inputs that were being exposed as outputs were removed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   25 Apr 2003 11:25:42   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  341
 * SCR Title:  DDE Input and Outputs are no longer working correctly
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Fixed several problems so that the DDE inputs and outputs will work like they used to in pre SVO 4.00 versions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   17 Apr 2003 17:20:26   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  341
 * SCR Title:  DDE Input and Outputs are no longer working correctly
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Modified configuration load to add inputs as available outputs so they can be echoed through the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   17 Apr 2003 17:16:56   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   28 Mar 2003 12:31:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  318
 * SCR Title:  Image Transfer Compression
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added support for knowing if the configuration is valid (loaded) - IsConfigurationLoaded, SetConfigurationLoaded, m_bIsConfigurationValid
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   20 Mar 2003 09:28:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  336
 * SCR Title:  Change Light Reference and LUT dialogs on RGB mono system
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed problem with Configuration type recognition
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   20 Mar 2003 07:14:06   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  336
 * SCR Title:  Change Light Reference and LUT dialogs on RGB mono system
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed uninitialized m_eProductType in constructor
 * changed all access to this variable to use Get / Set
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   19 Mar 2003 15:20:28   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  332
 * SCR Title:  Module Ready not available after being unassigned and other small I/O bugs
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed some problems with input/outputs and the Go online/Go offline process. Also, made changes to force the PPQ to always have valid inputs and outputs even without an inspection attached.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   14 Mar 2003 07:04:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  336
 * SCR Title:  Change Light Reference and LUT dialogs on RGB mono system
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added awareness of configuration type
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   13 Mar 2003 14:30:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  335
 * SCR Title:  Fix Light Reference and LUT problems with RGB Mono systems
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   during the save if the board is an RGB it will only write out the device Viper_RGB_1_Dig_0.Ch_All.  It will not use the individual channels.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   10 Feb 2003 13:17:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  316, 321
 * SCR Title:  Add option to extent acquisition image source buffers to the length of the PPQ
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed how the PPQ's input list is generated.  After reading in the PPQ, it will now rebuild the input list.   
 * 
 * Also added a section in the load/save of the configuration of the PPQ's for the new property 'Maintain Source Images'
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   05 Feb 2003 17:29:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Made modifications to the display logic to display the current source and result images indexes, respectively. This should make it work correctly regardless of whether it is online or performing RunOnce/Regression.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   13 Jan 2003 14:44:06   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed code segment that was mis merged.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   13 Jan 2003 11:27:14   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added new code to fully enable Remote Inputs in the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   10 Jan 2003 18:29:48   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes to how the images are displayed while running and while doing a RunOnce operation to use the image that is specified in the SVProductInfoStruct.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   10 Jan 2003 09:42:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Changed use of class name SVAcquisitionDevice to SVConfigurationAcquisitionDeviceInfoStuct
 * Changed AcquisitionDevice functions to use Lut
 * Added Lut to SaveConfiguration and LoadConfiguration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   18 Dec 2002 09:13:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  231
 * SCR Title:  Update OCR Font Editor code to be stable & prevent crash when close SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 Dec 2002 21:35:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  311
 * SCR Title:  Fix Light Reference save for RGB Mono
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated LoadConfiguration and SaveConfiguration methods of the SVConfigurationObject to reflect changes in references of SVAcquisitionClass member variables to accessor functions
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 12:51:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/