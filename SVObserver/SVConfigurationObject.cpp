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
//Moved to precompiled header: #include <regex>
#include "SVConfigurationObject.h"
#include "RangeClassHelper.h"
#include "RemoteMonitorListHelper.h"
#include "RemoteMonitorNamedList.h"
#include "RootObject.h"
#include "SVCommandInspectionGetItems.h"
#include "SVConfigurationTreeWriter.h"
#include "SVGlobal.h"
#include "SVMainFrm.h"
#include "SVIOController.h"
#include "SVIOTabbedView.h"
#include "SVIPDoc.h"
#include "SVObserver.h"
#include "SVStorageResult.h"
#include "SVToolSet.h"
#include "TextDefinesSvO.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/SVUserMessage.h"
#include "CameraLibrary/SVDeviceParams.h"
#include "CameraLibrary/SVBoolValueDeviceParam.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "InspectionEngine/SVAcquisitionClass.h"
#include "InspectionEngine/SVAcquisitionDevice.h"
#include "InspectionEngine/SVDigitizerProcessingClass.h"
#include "InspectionEngine/SVVirtualCamera.h"
#include "ObjectInterfaces/IObjectWriter.h"
#include "SVIOLibrary/SVCameraDataInputObject.h"
#include "SVIOLibrary/PlcOutputObject.h"
#include "SVIOLibrary/SVDigitalInputObject.h"
#include "SVIOLibrary/SVDigitalOutputObject.h"
#include "SVIOLibrary/SVInputObjectList.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVIOLibrary/SVIOTriggerLoadLibraryClass.h"
#include "SVIOLibrary/SVOutputObjectList.h"
#include "SVIOLibrary/SVRemoteInputObject.h"
#include "SVCommandLibrary/SVObjectAsynchronousCommandTemplate.h"
#include "SVFileSystemLibrary/SVFileNameManagerClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/SVToolsetScriptTags.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVProtoBuf/ConverterHelper.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVSystemLibrary\SVThreadManager.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVUtilityLibrary/SVSafeArray.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVValueObjectLibrary/SVVariantValueObjectClass.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVXMLLibrary/SVObjectXMLWriter.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#include "TriggerHandling/SVTriggerClass.h"
#include "TriggerInformation/SVTriggerObject.h"
#include "TriggerInformation/SVTriggerProcessingClass.h"
#include "TriggerInformation/SVHardwareManifest.h"
#include "Tools/SVColorTool.h"
#pragma endregion Includes

#pragma region Declarations

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

constexpr int cModuleReadyChannel = 15;
constexpr int cDiscreteInputCount = 8;
constexpr int cDiscreteOutputCount = 16;
constexpr int cPlcInputCount = 0;
constexpr int cPlcOutputCount = 14;

///For this class it is not necessary to call SV_IMPLEMENT_CLASS as it is a base class and only derived classes are instantiated.
//SV_IMPLEMENT_CLASS(SVConfigurationObject, SVConfigurationObjectId);
#pragma endregion Declarations

#pragma region Constructor
SVConfigurationObject::SVConfigurationObject(LPCSTR ObjectName) : SVObjectClass(ObjectName) 
	,m_pIOController{std::make_unique<SVIOController>(this)}
	,m_pInputObjectList{std::make_unique<SVInputObjectList>()}
	,m_pOutputObjectList{std::make_unique<SVOutputObjectList>()}
{
	SVObjectManagerClass::Instance().ChangeUniqueObjectID(this, ObjectIdEnum::ConfigObjectId);

	m_arTriggerArray.clear();
	m_arPPQArray.clear();
	m_arCameraArray.clear();
	m_arInspectionArray.clear();

	if (nullptr != m_pInputObjectList)
	{
		m_pInputObjectList->SetName(SvO::InputObjectList);
	}
	if (nullptr != m_pOutputObjectList)
	{
		m_pOutputObjectList->SetName(SvO::OutputObjectList);
	}
}

SVConfigurationObject::SVConfigurationObject(SVObjectClass* pOwner, int StringResourceID) : SVObjectClass(pOwner, StringResourceID)
	, m_pIOController {std::make_unique<SVIOController>(this)}
	, m_pInputObjectList {std::make_unique<SVInputObjectList>()}
	, m_pOutputObjectList {std::make_unique<SVOutputObjectList>()}
{
	SVObjectManagerClass::Instance().ChangeUniqueObjectID(this, ObjectIdEnum::ConfigObjectId);

	m_arTriggerArray.clear();
	m_arPPQArray.clear();
	m_arCameraArray.clear();
	m_arInspectionArray.clear();

	if (nullptr != m_pInputObjectList)
	{
		m_pInputObjectList->SetName(SvO::InputObjectList);
	}
	if (nullptr != m_pOutputObjectList)
	{
		m_pOutputObjectList->SetName(SvO::OutputObjectList);
	}
}

SVConfigurationObject::~SVConfigurationObject()
{
	DestroyConfiguration();
}
#pragma endregion Constructor

HRESULT SVConfigurationObject::RebuildOutputObjectList()
{
	HRESULT l_Status = S_OK;

	// Force the PPQs to rebuild
	long lSize = GetPPQCount();

	for (long l = 0; l < lSize; l++)
	{
		SVPPQObject* pPPQ = GetPPQ(l);

		if (nullptr != pPPQ)
		{
			pPPQ->RebuildOutputList();
		}
	}

	return l_Status;
}

bool SVConfigurationObject::AddTrigger(SvTi::SVTriggerObject* pTrigger)
{
	bool Result(false);

	if (nullptr != pTrigger)
	{
		m_arTriggerArray.push_back(pTrigger);
		Result = true;
	}

	return Result;
}

bool SVConfigurationObject::RemoveTrigger(SvTi::SVTriggerObject* pTrigger)
{
	if (nullptr == pTrigger) { return false; }

	int iSize = static_cast<int> (m_arTriggerArray.size());
	for (int i = 0; i < iSize; i++)
	{
		if (pTrigger == m_arTriggerArray[i])
		{
			m_arTriggerArray.erase(m_arTriggerArray.begin() + i);
			break;
		}
	}

	return true;
}

long SVConfigurationObject::GetTriggerCount() const
{
	return static_cast<long> (m_arTriggerArray.size());
}// end GetTriggerCount

bool SVConfigurationObject::GetChildObjectByName(LPCTSTR tszName, SvTi::SVTriggerObject** ppTrigger) const
{
	bool bReturn = false;

	//Only do an assert check so that in release mode no check is made
	assert(nullptr != ppTrigger);

	long lCount = GetTriggerCount();

	std::string Name = tszName;

	for (long l = 0; l < lCount; l++)
	{
		SvTi::SVTriggerObject* pTrigger = GetTrigger(l);
		if (nullptr != pTrigger && Name == pTrigger->GetCompleteName())
		{
			*ppTrigger = pTrigger;
			bReturn = true;
			break;
		}
	}

	return bReturn;
}

SvTi::SVTriggerObject* SVConfigurationObject::GetTrigger(long lIndex) const
{
	SvTi::SVTriggerObject* retValue = nullptr;

	assert(0 <= lIndex && static_cast<long> (m_arTriggerArray.size()) > lIndex);

	if (0 <= lIndex && static_cast<long> (m_arTriggerArray.size()) > lIndex)
	{
		retValue = m_arTriggerArray[lIndex];
	}

	return retValue;
}// end GetTrigger

bool SVConfigurationObject::AddAcquisitionDevice(LPCTSTR szName,
	const SVFileNameArrayClass& rsvFiles,
	const SVLightReference& rsvLight,
	const SVLut& rLut,
	const SVDeviceParamCollection* pDeviceParams)
{
	bool bOk(false);
	SvIe::SVConfigurationAcquisitionDeviceInfoStruct* pDevice(nullptr);

	SVAcquisitionDeviceMap::iterator Iter = m_AcquisitionDeviceMap.find(szName);
	pDevice = (m_AcquisitionDeviceMap.end() != Iter) ? Iter->second : nullptr;

	if (nullptr == pDevice)
	{
		pDevice = new SvIe::SVConfigurationAcquisitionDeviceInfoStruct;
	}

	bOk = nullptr != pDevice;
	if (bOk)
	{
		pDevice->msvFiles = rsvFiles;
		pDevice->msvLightArray = rsvLight;
		pDevice->mLut = rLut;
		if (nullptr != pDeviceParams)
		{
			pDevice->mDeviceParams = *pDeviceParams;
		}
		m_AcquisitionDeviceMap[szName] = pDevice;
	}

	return bOk;
}// end AddAcquisitionDevice

bool SVConfigurationObject::ModifyAcquisitionDevice(LPCTSTR szName, const SVLightReference& rsvLight)
{
	bool bOk = false;
	SvIe::SVConfigurationAcquisitionDeviceInfoStruct* pDevice = nullptr;

	SVAcquisitionDeviceMap::iterator Iter = m_AcquisitionDeviceMap.find(szName);
	pDevice = (m_AcquisitionDeviceMap.end() != Iter) ? Iter->second : nullptr;

	bOk = nullptr != pDevice;
	if (bOk)
	{
		pDevice->msvLightArray = rsvLight;
	}

	return bOk;
}// end AddAcquisitionDevice

bool SVConfigurationObject::ModifyAcquisitionDevice(LPCTSTR szName, const SVLut& lut)
{
	bool bOk = false;
	SvIe::SVConfigurationAcquisitionDeviceInfoStruct* pDevice = nullptr;

	SVAcquisitionDeviceMap::iterator Iter = m_AcquisitionDeviceMap.find(szName);
	pDevice = (m_AcquisitionDeviceMap.end() != Iter) ? Iter->second : nullptr;

	bOk = nullptr != pDevice;
	if (bOk)
	{
		pDevice->mLut = lut;
	}

	return bOk;
}

bool SVConfigurationObject::ModifyAcquisitionDevice(LPCTSTR szName, const SVDeviceParamCollection* pParams)
{
	bool bOk = false;
	if (nullptr != pParams)
	{
		SvIe::SVConfigurationAcquisitionDeviceInfoStruct* pDevice = nullptr;

		SVAcquisitionDeviceMap::iterator Iter = m_AcquisitionDeviceMap.find(szName);
		pDevice = (m_AcquisitionDeviceMap.end() != Iter) ? Iter->second : nullptr;

		bOk = nullptr != pDevice;
		if (bOk)
		{
			// don't do a straight assignment ( m_DeviceParams = pParams )
			// only copy those parameters that are specified.
			SVDeviceParamMap::const_iterator iter;
			for (iter = pParams->mapParameters.begin(); iter != pParams->mapParameters.end(); ++iter)
			{
				const SVDeviceParamWrapper& w = iter->second;

				if (nullptr != ((const SVDeviceParam*)w))
				{
					pDevice->mDeviceParams.GetParameter(iter->first) = w;
				}
			}
		}
	}
	return bOk;

}

bool SVConfigurationObject::RemoveAcquisitionDevice(LPCTSTR szName)
{
	bool Result = false;

	SvIe::SVConfigurationAcquisitionDeviceInfoStruct *pDevice = nullptr;

	SVAcquisitionDeviceMap::iterator Iter = m_AcquisitionDeviceMap.find(szName);
	pDevice = (m_AcquisitionDeviceMap.end() != Iter) ? Iter->second : nullptr;
	Result = m_AcquisitionDeviceMap.end() != Iter;
	if (Result)
	{
		m_AcquisitionDeviceMap.erase(Iter);

		if (nullptr != pDevice)
		{
			delete pDevice;
		}
	}

	return Result;
}

bool SVConfigurationObject::GetAcquisitionDevice(LPCTSTR szName,
	SVFileNameArrayClass*& rpFiles,
	SVLightReference*& rpLight,
	SVLut*& rpLut,
	SVDeviceParamCollection*& rpDeviceParams) const
{
	bool Result = false;

	rpFiles = nullptr;
	rpLight = nullptr;
	rpLut = nullptr;
	rpDeviceParams = nullptr;

	SVAcquisitionDeviceMap::const_iterator Iter = m_AcquisitionDeviceMap.find(szName);
	SvIe::SVConfigurationAcquisitionDeviceInfoStruct* pDevice = (m_AcquisitionDeviceMap.end() != Iter) ? Iter->second : nullptr;

	Result = nullptr != pDevice;
	if (Result)
	{
		rpFiles = &(pDevice->msvFiles);
		rpLight = &(pDevice->msvLightArray);
		rpLut = &(pDevice->mLut);
		rpDeviceParams = &(pDevice->mDeviceParams);
	}

	return Result;
}

SVConfigurationObject::SVAcquisitionDeviceMap::iterator SVConfigurationObject::GetAcquisitionDeviceStartPosition()
{
	return m_AcquisitionDeviceMap.begin();
}

SVConfigurationObject::SVAcquisitionDeviceMap::iterator SVConfigurationObject::GetAcquisitionDeviceEndPosition()
{
	return m_AcquisitionDeviceMap.end();
}

void SVConfigurationObject::GetAcquisitionDeviceNextAssoc(SVAcquisitionDeviceMap::const_iterator& rNextPosition, std::string& rKey) const
{
	if (rNextPosition != m_AcquisitionDeviceMap.end())
	{
		rKey = rNextPosition->first;
		++rNextPosition;
	}
}

void SVConfigurationObject::GetAcquisitionDeviceNextAssoc(SVAcquisitionDeviceMap::const_iterator& rNextPosition,
	std::string& rKey,
	SVFileNameArrayClass*& rpFiles,
	SVLightReference*& rpLight,
	SVLut*& rpLut,
	SVDeviceParamCollection*& rpDeviceParams) const
{
	SvIe::SVConfigurationAcquisitionDeviceInfoStruct* pDevice = nullptr;

	rpFiles = nullptr;
	rpLight = nullptr;
	rpLut = nullptr;
	rpDeviceParams = nullptr;

	if (rNextPosition != m_AcquisitionDeviceMap.end())
	{
		rKey = rNextPosition->first;
		pDevice = rNextPosition->second;
		++rNextPosition;
	}

	if (nullptr != pDevice)
	{
		rpFiles = &(pDevice->msvFiles);
		rpLight = &(pDevice->msvLightArray);
		rpLut = &(pDevice->mLut);
		rpDeviceParams = &(pDevice->mDeviceParams);
	}
}

bool SVConfigurationObject::AddPPQ(SVPPQObject* pPPQ)
{
	bool Result(false);

	if (nullptr != pPPQ)
	{
		m_arPPQArray.push_back(pPPQ);
		Result = true;
	}

	return Result;
}

bool SVConfigurationObject::RemovePPQ(const SVPPQObject* const pPPQ)
{
	if (nullptr == pPPQ) { return false; }

	int Size = static_cast<int> (m_arPPQArray.size());
	for (int i = 0; i < Size; i++)
	{
		if (pPPQ == m_arPPQArray[i])
		{
			m_arPPQArray.erase(m_arPPQArray.begin() + i);
			break;
		}
	}

	return true;
}

long SVConfigurationObject::GetPPQCount() const
{
	return static_cast<long> (m_arPPQArray.size());
}// end GetPPQCount

bool SVConfigurationObject::GetChildObjectByName(LPCTSTR tszName, SVPPQObject** ppPPQ) const
{
	bool bReturn = false;

	//Only do an assert check so that in release mode no check is made
	assert(nullptr != ppPPQ);

	long lCount = GetPPQCount();

	std::string Name = tszName;

	for (long l = 0; l < lCount; l++)
	{
		SVPPQObject* pPPQ = GetPPQ(l);
		if (nullptr != pPPQ && Name == pPPQ->GetCompleteName())
		{
			*ppPPQ = pPPQ;
			bReturn = true;
			break;
		}
	}

	return bReturn;
}

SVPPQObject* SVConfigurationObject::GetPPQ(long lIndex) const
{
	SVPPQObject* retValue = nullptr;
	assert(0 <= lIndex && static_cast<long> (m_arPPQArray.size()) > lIndex);

	if (0 <= lIndex && static_cast<long>(m_arPPQArray.size()) > lIndex)
	{
		retValue = m_arPPQArray[lIndex];
	}

	return retValue;
}// end GetPPQ

bool SVConfigurationObject::GetPPQByName(LPCTSTR name, SVPPQObject** ppPPQ) const
{
	bool bReturn = false;

	//Only do an assert check so that in release mode no check is made
	assert(nullptr != ppPPQ);

	int Size = static_cast<int> (m_arPPQArray.size());
	for (int i = 0; i < Size && !bReturn; i++)
	{
		SVPPQObject* pPPQ = m_arPPQArray[i];
		if (nullptr != pPPQ)
		{
			std::string ppqName = pPPQ->GetName();
			if (ppqName == name)
			{
				*ppPPQ = pPPQ;
				bReturn = true;
			}
		}
	}
	return bReturn;
}

bool SVConfigurationObject::AddCamera(SvIe::SVVirtualCamera* pCamera)
{
	bool Result(false);

	if (nullptr != pCamera)
	{
		m_arCameraArray.push_back(pCamera);
		Result = true;
	}
	return Result;
}

bool SVConfigurationObject::RemoveCamera(SvIe::SVVirtualCamera* pCamera)
{
	if (nullptr == pCamera) { return false; }

	int iSize = static_cast<int> (m_arCameraArray.size());
	for (int i = 0; i < iSize; i++)
	{
		if (pCamera == m_arCameraArray[i])
		{
			m_arCameraArray.erase(m_arCameraArray.begin() + i);
			break;
		}
	}

	return true;
}

long SVConfigurationObject::GetCameraCount() const
{
	return static_cast<long> (m_arCameraArray.size());
}

bool SVConfigurationObject::GetChildObjectByName(LPCTSTR tszName, SvIe::SVVirtualCamera** ppCamera) const
{
	bool bReturn = false;

	//Only do an assert check so that in release mode no check is made
	assert(nullptr != ppCamera);

	long lCount = GetCameraCount();
	std::string Name = tszName;

	for (long l = 0; l < lCount; l++)
	{
		SvIe::SVVirtualCamera* pCamera = GetCamera(l);
		if (nullptr != pCamera && Name == pCamera->GetCompleteName())
		{
			*ppCamera = pCamera;
			bReturn = true;
			break;
		}
	}

	return bReturn;
}

SvIe::SVVirtualCamera* SVConfigurationObject::GetCamera(long lIndex) const
{
	SvIe::SVVirtualCamera* pCamera = nullptr;

	assert(0 <= lIndex && static_cast<long>(m_arCameraArray.size()) > lIndex);

	if (0 <= lIndex && static_cast<long>(m_arCameraArray.size()) > lIndex)
	{
		pCamera = m_arCameraArray[lIndex];
	}

	return pCamera;
}// end GetCamera

bool SVConfigurationObject::AddInspection(SVInspectionProcess* pInspection)
{
	bool Result(false);

	if (nullptr != pInspection)
	{
		m_arInspectionArray.push_back(pInspection);
		assert(m_arInspectionArray.end() != std::find_if(m_arInspectionArray.begin(), m_arInspectionArray.end(), [pInspection](auto data)->bool
		{
			return (data == pInspection);
		}));
		auto inspId = m_inspList4TRC.add_list();
		inspId->set_id(pInspection->getObjectId());
		*inspId->mutable_name() = pInspection->GetName();

		Result = true;
	}

	return Result;
}

bool SVConfigurationObject::RemoveInspection(SVInspectionProcess* pInspection)
{
	if (nullptr == pInspection) { return false; }

	int i;
	int iSize;

	iSize = static_cast<int>  (m_arInspectionArray.size());
	for (i = 0; i < iSize; i++)
	{
		if (pInspection == m_arInspectionArray[i])
		{
			m_arInspectionArray.erase(m_arInspectionArray.begin() + i);
			m_inspList4TRC.mutable_list()->erase(m_inspList4TRC.list().begin() + i);
			break;
		}
	}

	return true;
}

long SVConfigurationObject::GetInspectionCount() const
{
	return static_cast<long> (m_arInspectionArray.size());
}// end GetInspectionCount

bool SVConfigurationObject::GetInspectionObject(LPCTSTR Name, SVInspectionProcess** ppInspection) const
{
	bool bResult = false;

	std::string InspectionName(Name);
	size_t Pos = InspectionName.find(_T("."));	// assumes first dot is after the inspection name
	if (std::string::npos != Pos)
	{
		bResult = GetChildObjectByName(SvUl::Left(InspectionName, Pos).c_str(), ppInspection);
	}
	else
	{
		bResult = GetChildObjectByName(InspectionName.c_str(), ppInspection);
	}

	return bResult;
}

bool SVConfigurationObject::GetChildObjectByName(LPCTSTR tszName, SVInspectionProcess** ppInspection) const
{
	bool bReturn = false;

	//Only do an assert check so that in release mode no check is made
	assert(nullptr != ppInspection);

	long lCount = GetInspectionCount();
	std::string Name = tszName;

	for (long l = 0; l < lCount; l++)
	{
		SVInspectionProcess* pInspection = GetInspection(l);
		if (nullptr != pInspection && Name == pInspection->GetCompleteName())
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

SVInspectionProcess* SVConfigurationObject::GetInspection(long lIndex) const
{
	SVInspectionProcess* pResult = nullptr;
	assert(0 <= lIndex && static_cast<long>(m_arInspectionArray.size()) > lIndex);

	if (0 <= lIndex && static_cast<long>(m_arInspectionArray.size()) > lIndex)
	{
		pResult = m_arInspectionArray[lIndex];
	}

	return pResult;
}// end GetInspection

// Add the (Imported) Input if it doesn't exists
// Do not adjust the ppqPosition if the position is greater than the PPQ length
HRESULT SVConfigurationObject::AddImportedRemoteInput(SVPPQObject* pPPQ, const std::string& name, long ppqPosition, long index, const _variant_t& p_Value)
{
	HRESULT hr = S_OK;

	//Only do an assert check so that in release mode no check is made
	assert(nullptr != pPPQ);

	SVIOEntryHostStructPtr pIOEntry = pPPQ->GetInput(name);
	if (nullptr == pIOEntry)
	{
		hr = AddRemoteInput(pPPQ, name, ppqPosition, index, p_Value);
	}
	else
	{
		if (!pIOEntry->m_Enabled)
		{
			pIOEntry->m_ObjectType = IO_REMOTE_INPUT;
			pIOEntry->m_PPQIndex = ppqPosition;
			pIOEntry->m_Enabled = ppqPosition != -1;

			SVRemoteInputObject* pRemoteInput = nullptr;
			int number = -1;
			sscanf_s(name.c_str(), SvO::cRemoteInputNumberLabel, &number);
			pRemoteInput = dynamic_cast<SVRemoteInputObject*> (m_pInputObjectList->GetInputFlyweight(name, SvPb::SVRemoteInputObjectType, number - 1).get());
			if (nullptr != pRemoteInput)
			{
				pRemoteInput->SetChannel(index);
				pRemoteInput->writeCache(p_Value);
			}
		}
	}
	return hr;
}

// Add the (Imported) Input if it doesn't exists
// Do not adjust the ppqPosition if the position is greater than the PPQ length
HRESULT SVConfigurationObject::AddImportedDigitalInput(SVPPQObject* pPPQ, const std::string& name, long ppqPosition)
{
	HRESULT hr = S_OK;

	//Only do an assert check so that in release mode no check is made
	assert(nullptr != pPPQ);

	SVIOEntryHostStructPtr pIOEntry = pPPQ->GetInput(name);
	if (nullptr == pIOEntry)
	{
		hr = AddDigitalInput(pPPQ, name, ppqPosition);
	}
	else
	{
		if (!pIOEntry->m_Enabled)
		{
			m_pInputObjectList->GetInputFlyweight(name, SvPb::SVDigitalInputObjectType);

			pIOEntry->m_ObjectType = IO_DIGITAL_INPUT;
			pIOEntry->m_PPQIndex = ppqPosition;
			pIOEntry->m_Enabled = ppqPosition != -1;

		}
	}
	return hr;
}

HRESULT SVConfigurationObject::AddRemoteInput(SVPPQObject* pPPQ, const std::string& name, long ppqPosition, long index, const _variant_t& rValue)
{
	HRESULT result{S_OK};

	//Only do an assert check so that in release mode no check is made
	assert(nullptr != pPPQ);

	// Add Remote Inputs to the InputObjectList
	//first check to see if remote input is there, check by name...
	SVRemoteInputObject* pRemoteInput = nullptr;
	int number = -1;
	sscanf_s(name.c_str(), SvO::cRemoteInputNumberLabel, &number);
	pRemoteInput = dynamic_cast<SVRemoteInputObject*> (m_pInputObjectList->GetInputFlyweight(name, SvPb::SVRemoteInputObjectType, number - 1).get());

	// Add Remote Input to the PPQ
	std::shared_ptr<SvOi::IValueObject> pInputValueObject = std::make_shared<SvVol::SVVariantValueObjectClass>();
	if(nullptr != pInputValueObject)
	{
		SvVol::SVVariantValueObjectClass* pValueObject = dynamic_cast<SvVol::SVVariantValueObjectClass*> (pInputValueObject.get());
		pValueObject->SetName(name.c_str());
		pValueObject->SetObjectOwner(pPPQ);
		pValueObject->setResetOptions(false, SvOi::SVResetItemNone);
		pValueObject->SetObjectAttributesAllowed(SvDef::selectableAttributes, SvOi::SetAttributeType::RemoveAttribute);
		variant_t defaultValue;
		defaultValue.ChangeType(rValue.vt);
		pValueObject->SetDefaultValue(defaultValue, false);
		pValueObject->ResetObject();

		SVIOEntryHostStructPtr pIOEntry = std::make_shared<SVIOEntryHostStruct>();
		pIOEntry->setValueObject(pInputValueObject);
		pIOEntry->m_ObjectType = IO_REMOTE_INPUT;
		pIOEntry->m_PPQIndex = ppqPosition;
		pIOEntry->m_Enabled = ppqPosition != -1;

		if (nullptr != pRemoteInput)
		{
			pRemoteInput->SetChannel(index);
			pRemoteInput->writeCache(rValue);

			pIOEntry->m_IOId = pRemoteInput->getObjectId();
		}

		pPPQ->AddInput(pIOEntry);
	}
	else
	{
		result = E_POINTER;
	}

	return result;
}

HRESULT SVConfigurationObject::AddDigitalInput(SVPPQObject* pPPQ, const std::string& name, long ppqPosition)
{
	HRESULT result{S_OK};

	//Only do an assert check so that in release mode no check is made
	assert(nullptr != pPPQ);

	SVDigitalInputObject* pDigitalInput = nullptr;

	pDigitalInput = dynamic_cast<SVDigitalInputObject*> (m_pInputObjectList->GetInputFlyweight(name, SvPb::SVDigitalInputObjectType).get());

	// Add Digital Input to the PPQ
	std::shared_ptr<SvOi::IValueObject> pInputValueObject = std::make_shared<SvVol::SVBoolValueObjectClass>();
	if (nullptr != pInputValueObject)
	{
		SvVol::SVBoolValueObjectClass* pValueObject = dynamic_cast<SvVol::SVBoolValueObjectClass*> (pInputValueObject.get());
		pValueObject->SetName(name.c_str());
		pValueObject->SetObjectOwner(pPPQ);
		pValueObject->setResetOptions(false, SvOi::SVResetItemNone);
		pValueObject->SetObjectAttributesAllowed(SvDef::selectableAttributes, SvOi::SetAttributeType::RemoveAttribute);
		pValueObject->ResetObject();

		SVIOEntryHostStructPtr pIOEntry = std::make_shared<SVIOEntryHostStruct>();
		pIOEntry->setValueObject(pInputValueObject);
		pIOEntry->m_ObjectType = IO_DIGITAL_INPUT;
		pIOEntry->m_PPQIndex = ppqPosition;
		pIOEntry->m_Enabled = ppqPosition != -1;

		if (nullptr != pDigitalInput)
		{
			pIOEntry->m_IOId = pDigitalInput->getObjectId();
		}

		pPPQ->AddInput(pIOEntry);
	}
	else
	{
		result = E_POINTER;
	}

	return result;
}

HRESULT SVConfigurationObject::AddCameraDataInput(SVPPQObject* pPPQ, SVIOEntryHostStructPtr pIOEntry)
{
	HRESULT hr = S_OK;

	//Only do an assert check so that in release mode no check is made
	assert(nullptr != pPPQ);

	std::string name = pIOEntry->getObject()->GetName();
	SVCameraDataInputObject* pInput = dynamic_cast<SVCameraDataInputObject*> (m_pInputObjectList->GetInputFlyweight(name, SvPb::SVCameraDataInputObjectType).get());

	// Add Input to the PPQ
	if (nullptr != pInput)
	{
		pIOEntry->m_IOId = pInput->getObjectId();
	}

	pPPQ->AddInput(pIOEntry);
	return hr;
}

void SVConfigurationObject::LoadEnviroment(SVTreeType& rTree, bool &ConfigurationColor)
{
	SVTreeType::SVBranchHandle hChild(nullptr);
	bool bOk = SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_ENVIRONMENT, nullptr, hChild);

	if (!bOk)
	{
		SvStl::MessageContainer MsgCont;
		MsgCont.setMessage(SVMSG_SVO_82_NO_ENVIROMENT_TAG, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16061_EnviromentTagIsMissing);
		throw MsgCont;
	}

	_variant_t Value;

	if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_CONFIGURATION_TYPE, hChild, Value))
	{
		int iType = Value;
		SetProductType(SVIMProductEnum(iType));

		ConvertColorToStandardProductType(ConfigurationColor);
	}

	if (!SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_VERSION_NUMBER, hChild, Value))
	{
		SvStl::MessageContainer MsgCont;
		SvDef::StringVector messageList;
		messageList.push_back(SvXml::CTAG_VERSION_NUMBER);
		MsgCont.setMessage(SVMSG_SVO_77_LOAD_VERSION_NUMBER_FAILED, SvStl::Tid_Default, messageList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16043);
		throw MsgCont;
	}

	TheSVObserverApp.setLoadingVersion(Value);

	//This is the deprecated tag and has changed to SvXml::CTAG_IMAGE_UPDATE and SvXml::CTAG_RESULT_UPDATE
	//Needs to be read for older configurations and becomes the standard default
	bool UpdateFlag = true;
	if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_ONLINE_DISPLAY, hChild, Value))
	{
		UpdateFlag = Value;
	}

	if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_IMAGE_DISPLAY_UPDATE, hChild, Value))
	{
		UpdateFlag = Value;
	}
	RootObject::setRootChildValue(SvDef::FqnEnvironmentImageUpdate, UpdateFlag);

	if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_RESULT_DISPLAY_UPDATE, hChild, Value))
	{
		UpdateFlag = Value;
	}
	RootObject::setRootChildValue(SvDef::FqnEnvironmentResultUpdate, UpdateFlag);

	// Thread Affinity Setup
	SVTreeType::SVBranchHandle hThreadSetup(nullptr);
	int iThreadNum = 1;
	bool bThreadOk = true;
	while (bThreadOk)
	{
		std::string ThreadTag = SvUl::Format(_T("%s_%d"), SvXml::CTAG_THREAD_SETUP, iThreadNum);
		bThreadOk = SvXml::SVNavigateTree::GetItemBranch(rTree, ThreadTag.c_str(), hChild, hThreadSetup);
		if (bThreadOk)
		{
			bThreadOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_THREAD_AFFINITY, hThreadSetup, Value);
			long lAffinity = Value;
			bThreadOk &= SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_THREAD_NAME, hThreadSetup, Value);
			std::string Name = SvUl::createStdString(Value);
			if (bThreadOk)
			{
				SVThreadManager::Instance().Setup(Name.c_str(), lAffinity);
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
	if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_THREAD_MGR_ENABLE, hChild, Value))
	{
		bThreadMgrEnable = Value;
	}
	SVThreadManager::Instance().SetThreadAffinityEnabled(bThreadMgrEnable);

	if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_NEXT_OBJECT_ID, hChild, Value))
	{
		SVObjectManagerClass::Instance().fitNextObjectId(Value);
	}
}

bool SVConfigurationObject::LoadIO(SVTreeType& rTree)
{
	SVTreeType::SVBranchHandle hChild(nullptr);
	bool bOk = SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_IO, nullptr, hChild);
	if (!bOk)
	{
		return false;
	}
	if (nullptr == m_pInputObjectList)
	{
		SvDef::StringVector msgList;
		msgList.push_back(SvO::InputObjectList);
		SvStl::MessageContainer MsgCont;
		MsgCont.setMessage(SVMSG_SVO_78_LOAD_IO_FAILED, SvStl::Tid_CreateSFailed, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16044_CreateInputList);
		throw MsgCont;
	}
	if (nullptr == m_pOutputObjectList)
	{
		SvDef::StringVector msgList;
		msgList.push_back(SvO::OutputObjectList);
		SvStl::MessageContainer MsgCont;
		MsgCont.setMessage(SVMSG_SVO_78_LOAD_IO_FAILED, SvStl::Tid_CreateSFailed, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16047_CreateOutputList);
		throw MsgCont;
	}

	bool isDiscreteIO = SvTi::SVHardwareManifest::isDiscreteIOSystem(GetProductType());

	_variant_t Value;

	SVTreeType::SVBranchHandle hSubChild;
	long lIOSize = 0;
	long i;

	if (!SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_NUMBER_OF_IO_ENTRIES, hChild, Value))
	{
		SvStl::MessageContainer MsgCont;
		MsgCont.setMessage(SVMSG_SVO_78_LOAD_IO_FAILED, SvStl::Tid_NumberOfIosMissing, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16045_MissingTag);
		throw MsgCont;
	}

	lIOSize = Value;

	uint32_t l_ModuleReadyId = SvDef::InvalidObjectId;
	uint32_t l_RaidErrorId = SvDef::InvalidObjectId;

	// Loop through all the "IO EntryXXX"
	for (i = 0; i < lIOSize; i++)
	{
		std::string IOEntry = SvUl::Format(SvXml::CTAGF_IO_ENTRY_X, i);
		if (!SvXml::SVNavigateTree::GetItemBranch(rTree, IOEntry.c_str(), hChild, hSubChild))
		{
			SvStl::MessageContainer MsgCont;
			MsgCont.setMessage(SVMSG_SVO_78_LOAD_IO_FAILED, SvStl::Tid_IOEntryIsMissing, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16060_IOEntryIsMissing);
			throw MsgCont;
		}

		_variant_t Data;
		std::string IOName;
		bool	bOutput{false};
		long	channel{-1L};
		bool	bForced{false};
		DWORD	dwForcedValue{0};
		bool	bInverted{false};
		bool	bCombined{false};
		bool	bCombinedACK{false};
		SvPb::SVObjectSubTypeEnum ioType{SvPb::SVObjectSubTypeEnum::SVNotSetSubObjectType};

		bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_IO_ENTRY_NAME, hSubChild, Data);
		if (bOk && VT_BSTR == Data.vt)
		{
			IOName = SvUl::createStdString(Data);
		}

		bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_IS_OUTPUT, hSubChild, Data);
		if (bOk)
		{
			bOutput = Data;
		}

		//Default output type is Digital Output Object
		if(bOutput)
		{
			ioType = SvPb::SVDigitalOutputObjectType;
		}
		bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_TYPE, hSubChild, Data);
		if (bOk)
		{
			ioType = static_cast<SvPb::SVObjectSubTypeEnum> (Data.lVal);
		}

		bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_CHANNEL, hSubChild, Data);
		if (bOk)
		{
			channel = Data;
		}

		bool bCheck = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_IS_FORCED, hSubChild, Data);
		if (bCheck)
		{
			bForced = Data;
		}

		bCheck = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_FORCED_VALUE, hSubChild, Data);
		if (bCheck)
		{
			dwForcedValue = static_cast<bool>(Data);
		}

		bCheck = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_IS_INVERTED, hSubChild, Data);
		if (bCheck)
		{
			bInverted = Data;
		}

		bCheck = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_IS_FORCED, hSubChild, Data);
		if (bCheck)
		{
			bForced = Data;
		}// end if

		bCheck = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_IS_COMBINED, hSubChild, Data);
		if (bCheck)
		{
			bCombined = Data;
		}
		else
		{
			bCombined = false;
		}

		bCheck = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_IS_COMBINED_ACK, hSubChild, Data);
		if (bCheck)
		{
			bCombinedACK = Data;
		}// end if
		else
		{
			bCombinedACK = false;
		}// end else

		if (channel > -1L)
		{
			if (bOutput)
			{
				if(SvPb::SVDigitalOutputObjectType == ioType)
				{
					SVDigitalOutputObject* pOutput = dynamic_cast<SVDigitalOutputObject*> (m_pOutputObjectList->GetOutputFlyweight(IOName.c_str(), ioType, channel).get());
					if (nullptr != pOutput)
					{
						pOutput->SetChannel(channel);
						pOutput->Force(bForced, (dwForcedValue != FALSE));
						pOutput->Invert(bInverted);
						pOutput->Combine(bCombined, bCombinedACK);

						if (SvDef::cModuleReady == IOName)
						{
							l_ModuleReadyId = pOutput->getObjectId();
						}
						else if (SvDef::cRaidErrorIndicator == IOName)
						{
							l_RaidErrorId = pOutput->getObjectId();
						}
					}
				}
				else if (SvPb::PlcOutputObjectType == ioType)
				{
					PlcOutputObject* pOutput = dynamic_cast<PlcOutputObject*> (m_pOutputObjectList->GetOutputFlyweight(IOName.c_str(), ioType, channel).get());
					if (nullptr != pOutput)
					{
						pOutput->SetChannel(channel);
						pOutput->Combine(bCombined, bCombinedACK);
					}
				}
			}
			else
			{
				SVDigitalInputObject* pInput = dynamic_cast<SVDigitalInputObject*> (m_pInputObjectList->GetInputFlyweight(IOName, SvPb::SVDigitalInputObjectType, channel).get());
				if (nullptr != pInput)
				{
					pInput->SetChannel(channel);
					pInput->Force(bForced, dwForcedValue != FALSE);
					pInput->Invert(bInverted);
				}
			}// end else
		}// end if

		if (!bOk)
		{
			SvStl::MessageContainer MsgCont;
			MsgCont.setMessage(SVMSG_SVO_78_LOAD_IO_FAILED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16046_LOAD_IO_FAILED);
			throw MsgCont;
		}
	}// end for

	if (nullptr != m_pIOController)
	{
		m_pIOController->SetParameters(rTree, hChild);
	}

	if (isDiscreteIO && SvDef::InvalidObjectId == l_ModuleReadyId)
	{
		SVOutputObjectPtr pOutput = m_pOutputObjectList->GetOutputFlyweight(SvDef::cModuleReady, SvPb::SVDigitalOutputObjectType);

		if (nullptr != pOutput && SvPb::SVDigitalOutputObjectType == pOutput->GetObjectSubType())
		{
			l_ModuleReadyId = pOutput->getObjectId();
		}
	}
	if(nullptr != m_pIOController->GetModuleReady())
	{
		m_pIOController->GetModuleReady()->m_IOId = l_ModuleReadyId;
	}
	if (nullptr != m_pIOController->GetRaidErrorBit())
	{
		m_pIOController->GetRaidErrorBit()->m_IOId = l_RaidErrorId;
	}

	return true;
}

bool SVConfigurationObject::LoadAcquisitionDevice(SVTreeType& rTree, std::string& BoardName, long &lNumBordDig)
{
	SVTreeType::SVBranchHandle hChild(nullptr);
	SVTreeType::SVBranchHandle hBoardChild(nullptr);

	bool bOk = SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_ACQUISITION_DEVICE, nullptr, hChild);

	hBoardChild = rTree.getFirstBranch(hChild);

	while (bOk && nullptr != hBoardChild)
	{
		BoardName = rTree.getBranchName(hBoardChild);

		long lNumAcqDig = 0;

		// test for File Acquisition, since there is no LUT, Light, or DeviceParams
		if (BoardName == _T("File"))
		{
			LoadFileAcquisitionConfiguration(rTree, hBoardChild, lNumAcqDig);
		}
		else
		{
			SVTreeType::SVBranchHandle hDigChild(nullptr);

			hDigChild = rTree.getFirstBranch(hBoardChild);

			while (bOk && nullptr != hDigChild)
			{
				++lNumAcqDig;

				bool bLightReferenceDone = false;
				bool bLutDone = false;
				bool bLutCreated = false;

				_variant_t Name;
				SVLightReference svLight;
				SVFileNameArrayClass svFileArray;
				SVLut lut;
				SVDeviceParamCollection svDeviceParams;

				std::string DigName = rTree.getBranchName(hDigChild);

				SVTreeType::SVBranchHandle hDataChild(nullptr);

				hDataChild = rTree.getFirstBranch(hDigChild);

				if (!SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_ACQUISITION_DEVICE_FILE_NAME, hDigChild, Name))
				{
					SvStl::MessageContainer MsgCont;
					SvDef::StringVector msgList;
					msgList.push_back(SvUl::createStdString(Name.bstrVal));
					MsgCont.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Config_CameraFileNameInvalid, msgList, SvStl::SourceFileParams(StdMessageParams));
					throw MsgCont;
				}
				svFileArray.SetFileNameList(SvUl::createStdString(Name.bstrVal).c_str());

				if (SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_LIGHT_REFERENCE_ARRAY, hDigChild, hDataChild))
				{
					_variant_t Value;

					if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_SIZE, hDataChild, Value))
					{
						int iBands = Value;

						assert(iBands > 0);

						if (svLight.Create(iBands))
						{
							for (int i = 0; i < iBands; i++)
							{
								SVTreeType::SVBranchHandle hBand = nullptr;

								std::string Band = SvUl::Format(SvXml::CTAGF_BAND_X, i);

								if (SvXml::SVNavigateTree::GetItemBranch(rTree, Band.c_str(), hDataChild, hBand))
								{
									if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_SIZE, hBand, Value))
									{
										int iSize = Value;

										svLight.Band(i).Create(iSize);

										for (int j = 0; j < iSize; j++)
										{
											SVTreeType::SVBranchHandle hLight = nullptr;

											std::string LightRef = SvUl::Format(SvXml::CTAGF_LIGHTREFERENCE_X, j);

											if (SvXml::SVNavigateTree::GetItemBranch(rTree, LightRef.c_str(), hBand, hLight))
											{
												if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_NAME, hLight, Value))
												{
													svLight.Band(i).Attribute(j).strName = SvUl::createStdString(Value);
													//Legacy: changed name from Contrast to Gain
													if (SvO::cCameraContrast == svLight.Band(i).Attribute(j).strName)
													{
														svLight.Band(i).Attribute(j).strName = SvDef::FqnCameraGain;
													}
												}
												if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_RESOURCE_ID, hLight, Value))
												{
													svLight.Band(i).Attribute(j).iIDCaption = Value;
												}
												if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_TYPE, hLight, Value))
												{
													svLight.Band(i).Attribute(j).dwType = Value;
												}
												if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_VALUE, hLight, Value))
												{
													svLight.Band(i).Attribute(j).lValue = Value;
												}

												bLightReferenceDone = true;
											}
										}
									}
								}
							}
						}
					}
				}

				if (!bLightReferenceDone)
				{
					SvStl::MessageContainer MsgCont;
					SvDef::StringVector msgList;
					msgList.push_back(DigName);
					MsgCont.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_LightReference_NotAvailable, msgList, SvStl::SourceFileParams(StdMessageParams));
					throw MsgCont;
				}

				if (SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_LUT, hDigChild, hDataChild))
				{
					_variant_t Value;
					SVLutInfo lutinfo;

					if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_MAX_VALUE, hDataChild, Value))
					{
						UINT uiMax = Value;

						lutinfo.SetMaxValue(uiMax);
					}

					if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_SIZE, hDataChild, Value))
					{
						int iBands = Value;

						if (lutinfo.MaxValue() == 0)	// legacy analog support
						{
							if (iBands == 3)
								lutinfo.SetMaxValue(1023);
							else
								lutinfo.SetMaxValue(255);
						}

						lutinfo.SetBands(iBands);
						lutinfo.SetTransform(SVDefaultLutTransform());

						bLutCreated = false;

						if (true) //SVLut.Create( iBands ) )
						{
							for (int iBand = 0; iBand < iBands; iBand++)
							{
								SVTreeType::SVBranchHandle hBand = nullptr;

								std::string Band = SvUl::Format(SvXml::CTAGF_BAND_X, iBand);

								SVLutTransformParameters param;

								if (SvXml::SVNavigateTree::GetItemBranch(rTree, Band.c_str(), hDataChild, hBand))
								{
									SvUl::SVSAFEARRAY l_BandData;

									if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_LUT_BAND_DATA, hBand, Value))
									{
										l_BandData = Value;

										int lSize = static_cast<int>(l_BandData.size());

										if (!bLutCreated)
										{
											lutinfo.SetBandSize(lSize);
											lut.Create(lutinfo);
											bLutCreated = true;
										}
									}

									if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_LUT_TRANSFORM_PARAMETERS, hBand, Value))
									{
										SvUl::SVSAFEARRAY l_Param(Value);

										long lSize = static_cast<long>(l_Param.size());
										// copy safearray to SVLutTransformParameters
										for (long l = 0; l < lSize; l++)
										{
											_variant_t lVal;

											l_Param.GetElement(l, lVal);

											param.push_back(lVal);
										}
									}

									if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_LUT_TRANSFORM_OPERATION, hBand, Value))
									{
										long lTransformOperation = Value;

										const SVLutTransformOperation* pOperation = SVLutTransform::GetEquivalentType((SVLutTransformOperationEnum)lTransformOperation);
										assert(pOperation);
										if (pOperation)
										{
											lut(iBand).SetTransformOperation(*pOperation);
											lut(iBand).Transform(param);
										}
									}

									if (bLutCreated && 0 < l_BandData.size())
									{
										lut(iBand).SetBandData(l_BandData);
									}

								}
							}
							bLutDone = true;
						}
					}
				}
				if (!bLutDone)
				{
					SvStl::MessageContainer MsgCont;
					MsgCont.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_LUT_NotAvailable, SvStl::SourceFileParams(StdMessageParams));
					throw MsgCont;
				}

				if (SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_DEVICE_PARAM_LIST, hDigChild, hDataChild))
				{
					LoadDeviceParameters(rTree, hDataChild, svDeviceParams);
				}

				bOk = bLightReferenceDone & bLutDone;
				if (bOk)
				{
					long l_BandCount = 1;

					// get camera format from DeviceParams
					const SVCameraFormatsDeviceParam* pParam = svDeviceParams.Parameter(DeviceParamCameraFormats).DerivedValue(pParam);
					if (pParam)
					{
						SVCameraFormat cf;

						cf.ParseAndAssignCameraFormat(pParam->strValue.c_str());

						// Band number depends on video type...
						switch (cf.m_eImageType)
						{
							case SvDef::SVImageFormatBGR888X:  // RGB
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
						std::string DigitizerName;

						SvIe::SVAcquisitionClassPtr psvDevice;

						if (1 < lNumLRBands)
						{
							lNumLRBands = 1;
						}

						//! If the IP address is saved in the configuration then determine which acquisition device has this address
						//! Important when skipping cameras in the camera manager
						SVStringValueDeviceParam* pIpValue(nullptr);
						svDeviceParams.GetParameter(DeviceParamIPAddress, pIpValue);
						if (nullptr != pIpValue)
						{
							DigitizerName = SvIe::SVDigitizerProcessingClass::Instance().GetReOrderedCamera(pIpValue->strValue.c_str());
						}
						//! If no name available then generate it the way it use to be
						if (DigitizerName.empty() && !DigName.empty())
						{
							int CameraID = atoi(SvUl::Mid(DigName, std::string(_T("Dig_")).length()).c_str());
							if (SvDef::cMaximumCameras <= CameraID)
							{
								CameraID -= SvDef::cMaximumCameras;
							}
							DigitizerName = SvIe::SVDigitizerProcessingClass::Instance().GetReOrderedCamera(CameraID);
						}

						if (!bLutDone || !bLutCreated)
						{
							psvDevice = SvIe::SVDigitizerProcessingClass::Instance().GetAcquisitionDevice(DigitizerName.c_str());
							bOk = nullptr != psvDevice;
							assert(bOk);
							if (bOk)
							{
								psvDevice->GetLut(lut);
								// create default lut
								SVLutInfo lutinfo = lut.Info();
								if (lutinfo.MaxValue() == 0)	// legacy analog support
								{
									if (lutinfo.Bands() == 3)
										lutinfo.SetMaxValue(1023);
									else
										lutinfo.SetMaxValue(255);
								}
								lutinfo.SetTransform(SVDefaultLutTransform());
								lutinfo.SetTransformOperation(SVLutTransformOperationNormal());	// as a default
								lut.Create(lutinfo);
								lut.Transform();
							}
						}
						SvIe::SVDigitizerProcessingClass::Instance().SetDigitizerColor(DigitizerName.c_str(), l_BandCount > 1);
						psvDevice = SvIe::SVDigitizerProcessingClass::Instance().GetAcquisitionDevice(DigitizerName.c_str());
						bOk = nullptr != psvDevice;
						if (bOk)
						{
							SVImageInfoClass svImageInfo;
							HRESULT CameraFileResult = psvDevice->LoadFiles(svFileArray);
							if (SVMSG_SVO_IGNORE_EXCEPTION == CameraFileResult)
							{
								//This stops loading without any further messages
								SvStl::MessageContainer MsgCont;
								MsgCont.setMessage(SVMSG_SVO_IGNORE_EXCEPTION, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
								throw MsgCont;
							}
							else if (S_OK != CameraFileResult && ERROR_FILE_NOT_FOUND != CameraFileResult)
							{
								SvStl::MessageContainer MsgCont;
								SvDef::StringVector msgList;
								msgList.push_back(std::string(svFileArray.GetFileNameList()));
								MsgCont.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Config_CameraFileInvalid, msgList, SvStl::SourceFileParams(StdMessageParams));
								throw MsgCont;
							}

							if (1 < svLight.Band(0).NumAttributes())
							{
								// This is what old configurations (SEC) set contrast to. 
								// now read the values from the camera files and save as 
								// the configuration values.

								if (svLight.Band(0).Attribute(1 /*LR_CONTRAST*/).lValue == -999999999L)
								{
									psvDevice->LoadLightReference(svLight);
								}// end if
							}

							// set the camera file metadata for each device param
							SVDeviceParamCollection rCamFileParams;
							psvDevice->GetCameraFileParameters(rCamFileParams);

							SVDeviceParamMap::const_iterator iter;
							for (iter = rCamFileParams.mapParameters.begin(); iter != rCamFileParams.mapParameters.end(); ++iter)
							{
								const SVDeviceParamWrapper& wCamFileParam = iter->second;
								if (wCamFileParam.IsValid())
								{
									SVDeviceParamWrapper& wParam = svDeviceParams.GetParameter(wCamFileParam->Type());
									if (wParam.IsValid())
									{
										wParam->SetMetadata(wCamFileParam);
									}
								}
							}

							psvDevice->SetDeviceParameters(svDeviceParams);	// needs to come before LR (quick fix for now)
							psvDevice->SetLightReference(svLight);
							psvDevice->SetLut(lut);
							psvDevice->GetImageInfo(&svImageInfo);
							psvDevice->CreateBuffers(svImageInfo);

							// set the trigger and strobe polarity in the I/O board based on Acq. device params
							// must get from the acq device instead of using svDeviceParams because the
							// device may set defaults in LoadFiles

							SVDeviceParamCollection params;
							psvDevice->GetDeviceParameters(params);

							// trigger
							if (params.ParameterExists(DeviceParamAcquisitionTriggerEdge))
							{
								int iDigNum = psvDevice->DigNumber();
								const SVBoolValueDeviceParam* pTriggerEdge = params.Parameter(DeviceParamAcquisitionTriggerEdge).DerivedValue(pTriggerEdge);
								SVIOConfigurationInterfaceClass::Instance().SetCameraTriggerValue(iDigNum, pTriggerEdge->bValue);
							}

							// strobe
							if (params.ParameterExists(DeviceParamAcquisitionStrobeEdge))
							{
								int iDigNum = psvDevice->DigNumber();
								const SVBoolValueDeviceParam* pStrobeEdge = params.Parameter(DeviceParamAcquisitionStrobeEdge).DerivedValue(pStrobeEdge);
								SVIOConfigurationInterfaceClass::Instance().SetCameraStrobeValue(iDigNum, pStrobeEdge->bValue);
							}
							// Get Combined parameters
							psvDevice->GetDeviceParameters(svDeviceParams);
							bOk = AddAcquisitionDevice(DigitizerName.c_str(), svFileArray, svLight, lut, &svDeviceParams);
						}
					} while (++lBand < lNumLRBands);
				}
				hDigChild = rTree.getNextBranch(hBoardChild, hDigChild);
			}
		}
		lNumBordDig = __max(lNumBordDig, lNumAcqDig);

		hBoardChild = rTree.getNextBranch(hChild, hBoardChild);
	}

	return bOk;
}


bool  SVConfigurationObject::LoadCameras(SVTreeType&  rTree, long& lNumCameras, bool ConfigurationColor)
{
	SVTreeType::SVBranchHandle hChild(nullptr);
	bool bOk = SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_CAMERA, nullptr, hChild);
	if (!bOk)
	{
		return false;
	}

	SVTreeType::SVBranchHandle hSubChild(nullptr);
	hSubChild = rTree.getFirstBranch(hChild);

	while (bOk && nullptr != hSubChild)
	{
		++lNumCameras;

		std::string ItemName = rTree.getBranchName(hSubChild);

		SvIe::SVVirtualCamera *pCamera = new SvIe::SVVirtualCamera;
		bOk = nullptr != pCamera;
		if (bOk)
		{
			_variant_t Value;
			std::string DeviceName;
			int CameraID = -1;

			pCamera->SetName(ItemName.c_str());
			if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_ACQUISITION_DEVICE, hSubChild, Value))
			{
				_bstr_t l_String(Value);

				//! We no longer need the channel information
				DeviceName = static_cast<LPCTSTR>(l_String);
				std::string::size_type Pos = DeviceName.find(_T(".Ch_"));
				if (std::string::npos != Pos)
				{
					DeviceName = SvUl::Left(DeviceName, Pos);
				}
				if (std::string::npos != (Pos = DeviceName.find(_T("Dig_"))))
				{
					CameraID = atoi(SvUl::Mid(DeviceName, Pos + std::string(_T("Dig_")).length()).c_str());
					if (SvDef::cMaximumCameras <= CameraID)
					{
						CameraID -= SvDef::cMaximumCameras;
					}
				}
			}

			//If camera ID is not available then set it to the Device name Digitizer number
			if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_CAMERA_ID, hSubChild, Value))
			{
				pCamera->setCameraID(Value);
			}
			else if (-1 != CameraID)
			{
				pCamera->setCameraID(CameraID);
			}

			if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_UNIQUE_REFERENCE_ID, hSubChild, Value))
			{
				SVObjectManagerClass::Instance().CloseUniqueObjectID(pCamera);
				pCamera->m_outObjectInfo.GetObjectReference().setObjectId(calcObjectId(Value));
				SVObjectManagerClass::Instance().OpenUniqueObjectID(pCamera);
			}

			if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_BAND_LINK, hSubChild, Value))
			{
				pCamera->SetBandLink(Value);
			}

			//If color not available then we need to set it using the global color which comes from the product type
			if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_COLOR, hSubChild, Value))
			{
				pCamera->SetIsColor(Value);
			}
			else
			{
				pCamera->SetIsColor(ConfigurationColor);
			}

			if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_FILEACQUISITION_MODE, hSubChild, Value))
			{
				pCamera->SetFileAcquisitionMode(Value);
			}

			if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_FILEACQUISITION_IMAGE_FILENAME, hSubChild, Value))
			{
				std::string filename = SvUl::createStdString(Value.bstrVal);
				pCamera->SetImageFilename(filename);
			}

			if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_FILEACQUISITION_IMAGE_DIRECTORYNAME, hSubChild, Value))
			{
				std::string dirName = SvUl::createStdString(Value);
				pCamera->SetImageDirectoryName(dirName);
			}

			if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_FILEACQUISITION_LOADINGMODE, hSubChild, Value))
			{
				pCamera->SetFileLoadingMode(Value);
			}

			if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_FILEACQUISITION_IMAGE_SIZE_EDIT_MODE, hSubChild, Value))
			{
				pCamera->SetFileImageSizeEditModeFileBased(Value);
			}

			if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_FILEACQUISITION_FILE_IMAGE_WIDTH, hSubChild, Value))
			{
				pCamera->SetFileImageWidth(Value);
			}

			if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_FILEACQUISITION_FILE_IMAGE_HEIGHT, hSubChild, Value))
			{
				pCamera->SetFileImageHeight(Value);
			}

			bOk = !DeviceName.empty();

			if (bOk)
			{
				std::string RemappedDeviceName;
				if (pCamera->IsFileAcquisition())
				{
					RemappedDeviceName = DeviceName;
				}
				else
				{
					RemappedDeviceName = SvIe::SVDigitizerProcessingClass::Instance().GetReOrderedCamera(pCamera->getCameraID());
				}
				bOk = pCamera->Create(RemappedDeviceName.c_str()) ? true : false;
			}

			if (bOk)
			{
				bOk = AddCamera(pCamera);
			}

			hSubChild = rTree.getNextBranch(hChild, hSubChild);
		}
	}

	return bOk;
}

bool SVConfigurationObject::LoadTrigger(SVTreeType& rTree)
{
	SVTreeType::SVBranchHandle hChild(nullptr);
	bool bOk = SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_TRIGGER, nullptr, hChild);
	if (!bOk)
	{
		return false;
	}

	SVTreeType::SVBranchHandle hSubChild(nullptr);
	hSubChild = rTree.getFirstBranch(hChild);

	while (bOk && nullptr != hSubChild)
	{
		std::string ItemName = rTree.getBranchName(hSubChild);

		SvTi::SVTriggerObject* pTrigger = new SvTi::SVTriggerObject;
		pTrigger->SetName(ItemName.c_str());
		bOk = nullptr != pTrigger;
		if (bOk)
		{
			_variant_t Value;
			std::string DeviceName;

			if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_TRIGGER_DEVICE, hSubChild, Value))
			{
				DeviceName = SvUl::createStdString(Value);
			}

			if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_SOFTWARETRIGGER_PERIOD, hSubChild, Value))
			{
				pTrigger->SetSoftwareTriggerPeriod(Value);
			}

			bOk = !DeviceName.empty();

			if (bOk)
			{
				SvTh::SVTriggerClass* psvDevice = SvTi::SVTriggerProcessingClass::Instance().GetTrigger(DeviceName.c_str());

				if (nullptr != psvDevice)
				{
					bOk = pTrigger->Create(psvDevice);
				}
			}

			if (bOk)
			{
				bOk = AddTrigger(pTrigger);
			}

			hSubChild = rTree.getNextBranch(hChild, hSubChild);
		}
	}

	return bOk;
}

bool SVConfigurationObject::LoadInspection(SVTreeType& rTree)
{
	SVTreeType::SVBranchHandle hChild(nullptr);
	bool bOk = SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_INSPECTION, nullptr, hChild);
	SVTreeType::SVBranchHandle hSubChild(nullptr);
	if (bOk)
	{
		hSubChild = rTree.getFirstBranch(hChild);
	}

	if (nullptr == hSubChild)
	{
		bOk = false;
	}

	while (bOk && nullptr != hSubChild)
	{
		std::string ItemName = rTree.getBranchName(hSubChild);

		SVInspectionProcess* pInspection(nullptr);

		SVObjectManagerClass::Instance().ConstructObject(SvPb::InspectionProcessClassId, pInspection);
		if (nullptr == pInspection)
		{
			SvStl::MessageContainer MsgCont;
			MsgCont.setMessage(SVMSG_SVO_80_LOAD_INSPECTION_FAILED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16050_ConstructObjectInspection);
			throw MsgCont;
		}

		_variant_t Value;
		std::string IPName;
		std::string ToolsetName;
		long NewDisableMethod {0L};
		long EnableAuxiliaryExtent {-1L};

		pInspection->SetName(ItemName.c_str());

		pInspection->SetDeviceName(ItemName.c_str());

		SVTreeType::SVBranchHandle hIPDoc(nullptr);
		SVTreeType::SVBranchHandle hInspectionProcess(nullptr);

		if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_INSPECTION_FILE_NAME, hSubChild, Value))
		{
			IPName = SvUl::createStdString(_bstr_t(Value.bstrVal));
		}

		SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_SVIPDOC, hSubChild, hIPDoc);

		SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_INSPECTION_PROCESS, hSubChild, hInspectionProcess);

		if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_INSPECTION_TOOLSET_IMAGE, hSubChild, Value))
		{
			ToolsetName = SvUl::createStdString(_bstr_t(Value.bstrVal));
		}

		bOk = nullptr != hIPDoc && !IPName.empty();

		if (bOk)
		{

			SVTreeType::SVBranchHandle hTempIPItem = hSubChild;
			SVTreeType::SVBranchHandle hTempIPObjectItem = hIPDoc;

			if (nullptr != hInspectionProcess)
			{
				hTempIPItem = hInspectionProcess;
				hTempIPObjectItem = hInspectionProcess;
			}

			bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_INSPECTION_NEW_DISABLE_METHOD, hTempIPItem, Value);
			if (bOk)
			{
				NewDisableMethod = Value.lVal;
			}

			bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_INSPECTION_ENABLE_AUXILIARY_EXTENT, hTempIPItem, Value);
			if (bOk)
			{
				EnableAuxiliaryExtent = Value.lVal;
			}

			bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_UNIQUE_REFERENCE_ID, hTempIPObjectItem, Value);
			if (bOk)
			{
				SVObjectManagerClass::Instance().CloseUniqueObjectID(pInspection);
				pInspection->m_outObjectInfo.GetObjectReference().setObjectId(calcObjectId(Value));
				SVObjectManagerClass::Instance().OpenUniqueObjectID(pInspection);
			}
		}

		if (bOk)
		{
			SVFileNameClass svFileName;

			svFileName.SetFullFileName(IPName.c_str());

			pInspection->SetToolsetImage(ToolsetName);

			bOk = pInspection->CreateInspection(svFileName.GetFileNameOnly().c_str());
			if (bOk)
			{
				pInspection->SetNewDisableMethod(1 == NewDisableMethod);
				if (-1 != EnableAuxiliaryExtent)
				{
					pInspection->setEnableAuxiliaryExtent(1 == EnableAuxiliaryExtent);
				}
			}
		}

		//Add invalid inspection to avoid memory leaks. The inspections will be destroyed. 
		if (nullptr != pInspection)
		{
			bOk = AddInspection(pInspection) && bOk;
		}
		hSubChild = rTree.getNextBranch(hChild, hSubChild);
	}
	if (false == bOk)
	{
		SvStl::MessageContainer MsgCont;
		MsgCont.setMessage(SVMSG_SVO_91_LOAD_INSPECTION_FAILED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16076_CouldNotLoadInspection);
		throw MsgCont;

	}
	return bOk;
}

bool SVConfigurationObject::LoadPPQ(SVTreeType& rTree)
{
	SVTreeType::SVBranchHandle hChild(nullptr);
	bool bOk = SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_PPQ, nullptr, hChild);
	if (!bOk)
	{
		return false;
	}

	SVTreeType::SVBranchHandle hSubChild(nullptr);
	hSubChild = rTree.getFirstBranch(hChild);

	while (bOk && nullptr != hSubChild)
	{
		std::string ItemName = rTree.getBranchName(hSubChild);

		SVPPQObject* pPPQ = new SVPPQObject;
		if (nullptr == pPPQ)
		{
			SvStl::MessageContainer MsgCont;
			MsgCont.setMessage(SVMSG_SVO_79_LOAD_PPQ_FAILED, SvStl::Tid_CreationOfPPQFailed, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16048_ErrrorCreatePPQObject);
			throw MsgCont;
		}
		pPPQ->SetName(ItemName.c_str());
		SVTreeType::SVBranchHandle hDeviceChild(nullptr);
		SVTreeType::SVBranchHandle hDataChild(nullptr);
		_variant_t Value;
		long lDelay = 0;
		long lCount = 0;

		// Set the IO lists
		pPPQ->m_pInputList = GetInputObjectList();
		pPPQ->m_pOutputList = GetOutputObjectList();

		// Update source to remove SVOVariant
		// Load the Unique Reference ID for the PPQ
		bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_UNIQUE_REFERENCE_ID, hSubChild, Value);

		if (bOk)
		{
			SVObjectManagerClass::Instance().CloseUniqueObjectID(pPPQ);
			pPPQ->m_outObjectInfo.GetObjectReference().setObjectId(calcObjectId(Value));
			SVObjectManagerClass::Instance().OpenUniqueObjectID(pPPQ);
		}// end if

		bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_PPQ_MODE, hSubChild, Value);

		if (bOk)
		{
			long lMode = Value;
			pPPQ->SetPPQOutputMode((SvDef::SVPPQOutputModeEnum)lMode);
		}// end if

		bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_PPQ_LENGTH, hSubChild, Value);

		if (bOk)
		{
			long lLength = Value;
			pPPQ->SetPPQLength(lLength);
		}// end if

		bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_PPQ_OUTPUT_RESET_DELAY_TIME, hSubChild, Value);

		if (bOk)
		{
			lDelay = Value;

			pPPQ->SetResetDelay(lDelay);
		}// end if

		bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_PPQ_OUTPUT_DELAY_TIME, hSubChild, Value);

		if (bOk)
		{
			lDelay = Value;

			pPPQ->SetOutputDelay(lDelay);
		}// end if

		bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_PPQ_MAX_PROCESSINGOFFSET_4INTEREST, hSubChild, Value);

		if (bOk)
		{
			lDelay = Value;

			pPPQ->setMaxProcessingOffset4Interest(lDelay);
		}// end if

		bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_PPQ_MAINTAIN_SRC_IMAGE, hSubChild, Value);

		if (bOk)
		{
			bool bMaintainSrcImg = Value;
			pPPQ->SetMaintainSourceImages(bMaintainSrcImg);
		}

		bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_PPQ_INSPECTION_TIMEOUT, hSubChild, Value);

		if (bOk)
		{
			long lInspectionTimeout = Value;
			pPPQ->SetInspectionTimeout(lInspectionTimeout);
		}

		bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_PPQ_CONDITIONAL_OUTPUT, hSubChild, Value);
		if (bOk)	// Conditional Output.
		{
			std::string condition = SvUl::createStdString(Value.bstrVal);
			if (!condition.empty())
			{
				pPPQ->SetConditionalOutputName(condition);
			}
		}

		bOk = SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_TRIGGER, hSubChild, hDeviceChild);

		if (bOk)
		{
			hDataChild = rTree.getFirstBranch(hDeviceChild);

			while (bOk && nullptr != hDataChild)
			{
				lCount = GetTriggerCount();

				std::string DataName = rTree.getBranchName(hDataChild);

				for (long l = 0; bOk && l < lCount; l++)
				{
					SvTi::SVTriggerObject* pTrigger = GetTrigger(l);

					bOk = (nullptr != pTrigger);
					if (bOk)
					{
						std::string DeviceName = pTrigger->GetName();

						if (DeviceName == DataName)
						{
							bOk = pPPQ->AttachTrigger(pTrigger);

							break;
						}
					}
				}

				hDataChild = rTree.getNextBranch(hDeviceChild, hDataChild);
			}
		}

		bOk = SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_CAMERA, hSubChild, hDeviceChild);
		if (bOk)
		{
			hDataChild = rTree.getFirstBranch(hDeviceChild);
			long lPosition = 0;
			while (bOk && nullptr != hDataChild)
			{
				lCount = GetCameraCount();

				std::string DataName = rTree.getBranchName(hDataChild);

				bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_POSITION, hDataChild, Value);

				if (bOk)
				{
					lPosition = Value;
				}// end if

				for (long l = 0; bOk && l < lCount; l++)
				{
					SvIe::SVVirtualCamera* pCamera = GetCamera(l);

					bOk = (nullptr != pCamera);
					if (bOk)
					{
						std::string DeviceName = pCamera->GetName();

						if (DeviceName == DataName)
						{
							bOk = pPPQ->AttachCamera(pCamera, lPosition);

							break;
						}
					}
				}

				hDataChild = rTree.getNextBranch(hDeviceChild, hDataChild);
			}
		}

		bOk = SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_INSPECTION, hSubChild, hDeviceChild);
		if (bOk)
		{
			hDataChild = rTree.getFirstBranch(hDeviceChild);

			while (bOk && nullptr != hDataChild)
			{
				lCount = GetInspectionCount();

				std::string DataName = rTree.getBranchName(hDataChild);

				for (long l = 0; bOk && l < lCount; l++)
				{
					SVInspectionProcess* pInspection = GetInspection(l);

					bOk = (nullptr != pInspection);
					if (bOk)
					{
						std::string DeviceName = pInspection->GetDeviceName();

						if (DeviceName == DataName)
						{
							bOk = pPPQ->AttachInspection(pInspection);

							break;
						}
					}
				}

				hDataChild = rTree.getNextBranch(hDeviceChild, hDataChild);
			}
		}

		if (!SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_INPUT, hSubChild, hDeviceChild))
		{

			SvStl::MessageContainer MsgCont;
			MsgCont.setMessage(SVMSG_SVO_79_LOAD_PPQ_FAILED, SvStl::Tid_InputIsMissing, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16049_ErrorMissingInpuTag);
			throw MsgCont;
		}

		hDataChild = rTree.getFirstBranch(hDeviceChild);
		while (bOk && nullptr != hDataChild)
		{
			std::string DataName = rTree.getBranchName(hDataChild);

			long lIndex{0L};
			long lPPQPosition{0L};

			if (!SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_IO_TYPE, hDataChild, Value))
			{

				SvStl::MessageContainer MsgCont;
				MsgCont.setMessage(SVMSG_SVO_79_LOAD_PPQ_FAILED, SvStl::Tid_MsgIOTypeIsMissing, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16053_ErrorMissingIOTypeTag);
				throw MsgCont;
			}
			std::string Type = SvUl::createStdString(Value);


			// This means it is a Digital input
			if (_T("Digital") == Type)
			{
				if (!SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_ITEM_NAME, hDataChild, Value))
				{

					SvStl::MessageContainer MsgCont;
					MsgCont.setMessage(SVMSG_SVO_79_LOAD_PPQ_FAILED, SvStl::Tid_MsgIONameIsMissing, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16052_ErrorMissingItemNameTag);
					throw MsgCont;
				}


				DataName = SvUl::createStdString(Value.bstrVal);

				if (!SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_PPQ_POSITION, hDataChild, Value))
				{

					SvStl::MessageContainer MsgCont;
					MsgCont.setMessage(SVMSG_SVO_79_LOAD_PPQ_FAILED, SvStl::Tid_MsgPPQPOsitionIsMissing, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16054_ErrorMissingPPQPOsitionTag);
					throw MsgCont;
				}
				lPPQPosition = Value;


				// Add Digital Input to the PPQ
				AddDigitalInput(pPPQ, DataName, lPPQPosition);
			}
			// This means it is a Remote input
			else if (_T("Remote") == Type)
			{
				_variant_t RemoteValue = 0.0;
				if (!SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_ITEM_NAME, hDataChild, Value))
				{
					SvStl::MessageContainer MsgCont;
					MsgCont.setMessage(SVMSG_SVO_79_LOAD_PPQ_FAILED, SvStl::Tid_MsgIONameIsMissing, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16059_ErrorMissingItemNameTag);
					throw MsgCont;
				}

				DataName = SvUl::createStdString(Value.bstrVal);
				if (!SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_PPQ_POSITION, hDataChild, Value))
				{
					SvStl::MessageContainer MsgCont;
					MsgCont.setMessage(SVMSG_SVO_79_LOAD_PPQ_FAILED, SvStl::Tid_MsgPPQPOsitionIsMissing, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16056_ErrorMissingPPQPOsitionTag);
					throw MsgCont;
				}

				lPPQPosition = Value;

				if (!SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_REMOTE_INDEX, hDataChild, Value))
				{
					SvStl::MessageContainer MsgCont;
					MsgCont.setMessage(SVMSG_SVO_79_LOAD_PPQ_FAILED, SvStl::Tid_MsgRemoteIndexIsMissing, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16058_ErrorMissingRemoteIndexTag);
					throw MsgCont;
				}
				lIndex = Value;


				if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_REMOTE_INITIAL_VALUE, hDataChild, Value))
				{
					RemoteValue = Value;
				}

				// Add Remote Inputs to the InputObjectList
				//first check to see if remote input is there, check by name...
				AddRemoteInput(pPPQ, DataName, lPPQPosition, lIndex, RemoteValue);
			}

			hDataChild = rTree.getNextBranch(hDeviceChild, hDataChild);
		}


		if (bOk)
		{
			bOk = pPPQ->Create();

			bOk &= pPPQ->RebuildInputList(HasCameraTrigger(pPPQ));

			bOk &= pPPQ->RebuildOutputList();

			if (bOk)
			{
				bOk = AddPPQ(pPPQ);
			}
		}

		hSubChild = rTree.getNextBranch(hChild, hSubChild);
	}

	return bOk ? true : false;
}

HRESULT SVConfigurationObject::LoadConfiguration(SVTreeType& rTree)
{
	HRESULT	result(S_OK);

	try
	{
		std::string BoardName;
		long lNumBoardDig(0);
		long lNumCameras(0);
		bool ConfigurationColor(false);
		LoadEnviroment(rTree, ConfigurationColor);
		LoadIO(rTree);
		LoadAcquisitionDevice(rTree, BoardName, lNumBoardDig);
		LoadCameras(rTree, lNumCameras, ConfigurationColor);
		LoadTrigger(rTree);
		LoadInspection(rTree);
		LoadPPQ(rTree);
		LoadAdditionalFiles(rTree);
		// EB 20031203
		// a temp solution
		// the better solution is to have the acqs subscribe and the triggers provide
		AttachAcqToTriggers();
	}
	catch (SvStl::MessageContainer&)
	{
		DestroyConfiguration();
		throw;
	}
	m_bConfigurationValid = true;

	return result;
}

HRESULT SVConfigurationObject::ValidateOutputList()
{
	HRESULT l_Status = S_OK;

	SvDef::StringVector InspectionNames;
	SvDef::StringVector PPQNames;

	long l_iInspections = GetInspectionCount();

	for (int i = 0; i < l_iInspections; i++)
	{
		SVInspectionProcess* pInspection = m_arInspectionArray[i];
		if (nullptr != pInspection)
		{
			InspectionNames.push_back(std::string(pInspection->GetName()));
		}
	}
	for (SVPPQObjectPtrVector::iterator it = m_arPPQArray.begin(); it != m_arPPQArray.end(); ++it)
	{
		if (nullptr != *it)
		{
			PPQNames.push_back(std::string((*it)->GetName()));
		}
	}

	l_Status = m_pOutputObjectList->RemoveUnusedOutputs(InspectionNames, PPQNames);

	return l_Status;
}

void SVConfigurationObject::UpgradeConfiguration()
{
	SVObjectPtrVector objectList;
	SVObjectManagerClass::Instance().getObjectsOfType(std::back_inserter(objectList), SvPb::SVToolObjectType, SvPb::SVColorToolObjectType);
	for (const auto pObject : objectList)
	{
		const SvTo::SVColorToolClass* pColorTool = dynamic_cast<const SvTo::SVColorToolClass*> (pObject);
		if (nullptr != pColorTool && pColorTool->isConverted())
		{
			SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Exception.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ColorToolExtentsChanged, SvStl::SourceFileParams(StdMessageParams));

			SVSVIMStateClass::AddState(SV_STATE_MODIFIED);
			break;
		}
	}

	objectList.clear();
	SVObjectManagerClass::Instance().getObjectsOfType(std::back_inserter(objectList), SvPb::SVEquationObjectType, SvPb::SVNotSetSubObjectType);
	for (const auto pObject : objectList)
	{
		const SvOi::IEquation* pEquation = dynamic_cast<const SvOi::IEquation*> (pObject);
		if(nullptr != pEquation)
		{
			const std::string& rEquaution = pEquation->GetEquationText();
			//Check if we are using incorrect Trigger Count variable
			constexpr LPCTSTR cPpqTriggerCount = R"(PPQ_[1-9]\.Trigger Count)";
			if(std::regex_search(rEquaution.cbegin(), rEquaution.cend(), std::regex(cPpqTriggerCount)))
			{
				SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
				Exception.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ErrorPpqTriggerCount, SvStl::SourceFileParams(StdMessageParams));
				break;
			}
		}
	}
}

HRESULT SVConfigurationObject::LoadAcquisitionDeviceFilename(SVTreeType& rTree, SVTreeType::SVBranchHandle hDig, SVFileNameArrayClass& rFileArray)
{
	HRESULT hr = S_FALSE;

	_variant_t l_Variant;

	if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_ACQUISITION_DEVICE_FILE_NAME, hDig, l_Variant))
	{
		rFileArray.SetFileNameList(SvUl::createStdString(l_Variant).c_str());

		hr = S_OK;
	}

	return hr;
}

HRESULT SVConfigurationObject::LoadFileAcquisitionConfiguration(SVTreeType& rTree, SVTreeType::SVBranchHandle hBoardChild, long& lNumAcqDig)
{
	HRESULT hr(S_OK);

	SVTreeType::SVBranchHandle hDigChild(nullptr);

	std::string BoardName = rTree.getBranchName(hBoardChild);
	hDigChild = rTree.getFirstBranch(hBoardChild);

	SVLightReference svLight;
	SVLut lut;

	while (S_OK == hr && nullptr != hDigChild)
	{
		++lNumAcqDig;

		SVFileNameArrayClass svFileArray;
		SVDeviceParamCollection svDeviceParams;

		std::string DigName = rTree.getBranchName(hDigChild);

		SVTreeType::SVBranchHandle hDataChild(nullptr);

		if (SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_ACQUISITION_DEVICE_FILE_NAME, hDigChild, hDataChild))
		{
			hr = LoadAcquisitionDeviceFilename(rTree, hDataChild, svFileArray);
		}

		if (SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_DEVICE_PARAM_LIST, hDigChild, hDataChild))
		{
			LoadDeviceParameters(rTree, hDataChild, svDeviceParams);
		}

		if (S_OK == hr)
		{
			// need to determine Digitizer Number and Channel
			std::string DeviceName;

			DeviceName = SvUl::Format(_T("%s.%s"), BoardName.c_str(), DigName.c_str());

			SvIe::SVAcquisitionClassPtr psvDevice(SvIe::SVDigitizerProcessingClass::Instance().GetAcquisitionDevice(DeviceName.c_str()));
			if (nullptr != psvDevice)
			{
				SVImageInfoClass svImageInfo;

				// Set the device parameters for the File Acquisition device (Note: do this before calling LoadFiles)
				psvDevice->SetDeviceParameters(svDeviceParams);	// needs to come before LR (quick fix for now)
				psvDevice->SetLightReference(svLight);
				psvDevice->SetLut(lut);

				// cause the image info to be updated for the File Acquisition device
				if (SVMSG_SVO_IGNORE_EXCEPTION == psvDevice->LoadFiles(svFileArray))
				{
					//This stops loading without any further messages
					SvStl::MessageContainer MsgCont;
					MsgCont.setMessage(SVMSG_SVO_IGNORE_EXCEPTION, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
					throw MsgCont;
				}

				psvDevice->GetImageInfo(&svImageInfo);
				psvDevice->CreateBuffers(svImageInfo);
			}
			if (!(AddAcquisitionDevice(DeviceName.c_str(), svFileArray, svLight, lut, &svDeviceParams)))
			{
				//This should be addressed when the new error handling is done.
				//This is to avoid returning E_FAIL which would not be displayed but just stop loading the configuration any further
				hr = -1798;
			}
		}

		hDigChild = rTree.getNextBranch(hBoardChild, hDigChild);
	}
	return hr;
}

HRESULT SVConfigurationObject::LoadDeviceParameters(SVTreeType& rTree, SVTreeType::SVBranchHandle hDataChild, SVDeviceParamCollection& svDeviceParams)
{
	_variant_t svValue;
	int iParam = 0;
	SVTreeType::SVBranchHandle hParam(nullptr);

	std::string Parameter = SvUl::Format(SvXml::CTAGF_DEVICE_PARAM_X, ++iParam);

	while (SvXml::SVNavigateTree::GetItemBranch(rTree, Parameter.c_str(), hDataChild, hParam))
	{
		if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_DEVICE_PARAM_TYPE, hParam, svValue))
		{
			long lType = svValue;

			if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_DEVICE_PARAM_VALUE, hParam, svValue))
			{
				svDeviceParams.CreateParameter((SVDeviceParamEnum)lType, svValue);
				SVDeviceParam* pParam = svDeviceParams.GetParameter((SVDeviceParamEnum)lType);
				LoadDeviceParamSpecial(rTree, hParam, pParam);
			}
		}

		hParam = nullptr;
		Parameter = SvUl::Format(SvXml::CTAGF_DEVICE_PARAM_X, ++iParam);
	}

	return S_OK;
}

HRESULT SVConfigurationObject::LoadDeviceParamSpecial(SVTreeType& rTree, SVTreeType::SVBranchHandle hParent, SVDeviceParam* pParam)
{
	HRESULT hr = S_OK;
	_variant_t svVariant;
	VARIANT vValue;
	::VariantInit(&vValue);
	SVTreeType::SVBranchHandle hChild(nullptr);

	switch (pParam->Type())
	{
		case DeviceParamCameraFormats:
		{
			SVCameraFormatsDeviceParam* pcf = dynamic_cast<SVCameraFormatsDeviceParam*> (pParam);
			SVTreeType::SVBranchHandle hParam(nullptr);
			if (SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_OPTIONS, hParent, hChild))
			{
				int iOption = 0;
				std::string OptionTag = SvUl::Format(SvXml::CTAGF_OPTION_X, ++iOption);
				while (SvXml::SVNavigateTree::GetItemBranch(rTree, OptionTag.c_str(), hChild, hParam))
				{
					if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_NAME, hParam, svVariant))
					{
						std::string Name = SvUl::createStdString(svVariant);

						SVCameraFormat& rcf = pcf->options[Name];
						rcf.ParseAndAssignCameraFormat(Name);
						if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_VARIABLE_CAMERA_FORMAT, hParam, svVariant))
						{
							rcf.m_bVariableROI = svVariant;
						}
						if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_TOP, hParam, svVariant))
						{
							rcf.m_lVPos = svVariant;
						}
						if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_LEFT, hParam, svVariant))
						{
							rcf.m_lHPos = svVariant;
						}
						if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_CX, hParam, svVariant))
						{
							rcf.m_lWidth = svVariant;
						}
						if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_CY, hParam, svVariant))
						{
							rcf.m_lHeight = svVariant;
						}
					}
					OptionTag = SvUl::Format(SvXml::CTAGF_OPTION_X, ++iOption);
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

	//destroy inspections...
	while (0 < m_arInspectionArray.size())
	{
		//it need to be removed from vector before deleting object, because the destructor can lead to action (GUI-Update) which call methods using m_arInspectionArray.
		auto* pIP = m_arInspectionArray[0];
		m_arInspectionArray.erase(m_arInspectionArray.begin());
		delete pIP;
	}

	//destroy camera's
	while (0 < m_arCameraArray.size())
	{
		auto* pCamera = m_arCameraArray[0];
		m_arCameraArray.erase(m_arCameraArray.begin());
		delete pCamera;
	}

	SvIe::SVConfigurationAcquisitionDeviceInfoStruct *pDevice = nullptr;

	for (auto& rItem : m_AcquisitionDeviceMap)
	{
		std::string DeviceName = rItem.first;
		pDevice = rItem.second;

		if (nullptr != pDevice)
		{
			SvIe::SVAcquisitionClassPtr psvDevice;

			delete pDevice;

			psvDevice = SvIe::SVDigitizerProcessingClass::Instance().GetAcquisitionDevice(DeviceName.c_str());
			bOk = nullptr != psvDevice;
			if (bOk)
			{
				psvDevice->DestroyBuffers();
				psvDevice->ResetLightReference();
				psvDevice->UnloadFiles();
			}
		}
	}

	m_AcquisitionDeviceMap.clear();

	//destroy trigger's
	for (auto pEntry : m_arTriggerArray)
	{
		delete pEntry;
	}
	m_arTriggerArray.clear();

	//destroy PPQ's
	for (auto& rPPQ : m_arPPQArray)
	{
		if (nullptr != rPPQ)
		{
			delete rPPQ;
		}
	}
	m_arPPQArray.clear();

	//Delete all Global Constants
	RootObject::resetRootChildValue(SvDef::FqnGlobal);

	SetProductType(SVIM_PRODUCT_TYPE_UNKNOWN);

	SvIe::SVDigitizerProcessingClass::Instance().ClearDevices();

	SVThreadManager::Instance().Clear();

	SvTrc::getTriggerRecordControllerRWInstance().unlockReset();
	SvTrc::getTriggerRecordControllerRWInstance().clearAll();

	for(const auto& rFile : m_AdditionalFiles)
	{
		SVFileNameManagerClass::Instance().RemoveItem(&rFile);
	}

	SVObjectManagerClass::Instance().resetNextObjectId();
	
	return bOk;
}

HRESULT SVConfigurationObject::GetChildObject(SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index) const
{
	HRESULT l_Status = S_OK;

	rpObject = nullptr;

	if (0 < rNameInfo.m_NameArray.size())
	{
		if (rNameInfo.m_NameArray[0] == "RemoteInputs")
		{
			if (nullptr != m_pInputObjectList)
			{
				rpObject = m_pInputObjectList->GetInput(rNameInfo.GetObjectName(1)).get();
			}
			else
			{
				l_Status = S_FALSE;
				return l_Status;
			}
		}

		if (nullptr == rpObject)
		{
			SVInspectionProcessVector::const_iterator l_InspectIter;

			for (l_InspectIter = m_arInspectionArray.begin(); nullptr == rpObject && l_InspectIter != m_arInspectionArray.end(); ++l_InspectIter)
			{
				SVInspectionProcess* pInspection = (*l_InspectIter);

				if (nullptr != pInspection)
				{
					if (static_cast<const size_t> (Index) < rNameInfo.m_NameArray.size() && rNameInfo.m_NameArray[Index] == pInspection->GetName())
					{
						l_Status = pInspection->GetChildObject(rpObject, rNameInfo, Index);
					}
				}
			}
		}

		if (nullptr == rpObject)
		{
			if (rNameInfo.m_NameArray[0].substr(0, 3) == _T("PPQ"))
			{
				SVPPQObjectPtrVector::const_iterator PPQIter;

				for (PPQIter = m_arPPQArray.begin(); nullptr == rpObject && PPQIter != m_arPPQArray.end(); ++PPQIter)
				{
					SVPPQObject* pPPQ = (*PPQIter);

					if (nullptr != pPPQ)
					{
						l_Status = pPPQ->GetChildObject(rpObject, rNameInfo, Index);
					}
				}
			}
			else if (rNameInfo.m_NameArray[0].substr(0, 6) == _T("Camera"))
			{
				SvIe::SVVirtualCameraPtrVector::const_iterator l_CameraIter;

				for (l_CameraIter = m_arCameraArray.begin(); nullptr == rpObject && l_CameraIter != m_arCameraArray.end(); ++l_CameraIter)
				{
					SvIe::SVVirtualCamera* pCamera = (*l_CameraIter);

					if (nullptr != pCamera)
					{
						l_Status = pCamera->GetChildObject(rpObject, rNameInfo, Index);
					}
				}
			}
			else if (rNameInfo.m_NameArray[0].substr(0, 7) == _T("Trigger"))
			{
				SvTi::SVTriggerObjectPtrVector::const_iterator l_TriggerIter;

				for (l_TriggerIter = m_arTriggerArray.begin(); nullptr == rpObject && l_TriggerIter != m_arTriggerArray.end(); ++l_TriggerIter)
				{
					SvTi::SVTriggerObject* pTrigger = (*l_TriggerIter);

					if (nullptr != pTrigger)
					{
						l_Status = pTrigger->GetChildObject(rpObject, rNameInfo, Index);
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

void SVConfigurationObject::SaveEnvironment(SvOi::IObjectWriter& rWriter) const
{
	rWriter.StartElement(SvXml::CTAG_ENVIRONMENT);

	_variant_t svValue = TheSVObserverApp.getCurrentVersion();
	rWriter.WriteAttribute(SvXml::CTAG_VERSION_NUMBER, svValue);
	svValue = GetProductType();
	rWriter.WriteAttribute(SvXml::CTAG_CONFIGURATION_TYPE, svValue);

	RootObject::getRootChildValue(SvDef::FqnEnvironmentImageUpdate, svValue);
	rWriter.WriteAttribute(SvXml::CTAG_IMAGE_DISPLAY_UPDATE, svValue);
	RootObject::getRootChildValue(SvDef::FqnEnvironmentResultUpdate, svValue);
	rWriter.WriteAttribute(SvXml::CTAG_RESULT_DISPLAY_UPDATE, svValue);

	// Thread Manager Enable.
	BOOL bEnable = SVThreadManager::Instance().GetThreadAffinityEnabled();
	svValue = bEnable;
	rWriter.WriteAttribute(SvXml::CTAG_THREAD_MGR_ENABLE, svValue);

	SVThreadManager::ThreadList threads;
	SVThreadManager::Instance().GetThreadInfo(threads, SVThreadAttribute::SVAffinityUser);
	int iCount = 1;
	for (SVThreadManager::ThreadList::const_iterator it = threads.begin(); it != threads.end(); ++it)
	{
		std::string Branch = SvUl::Format(_T("%s_%d"), SvXml::CTAG_THREAD_SETUP, iCount);
		rWriter.StartElement(Branch.c_str());
		svValue = it->m_strName.c_str();
		rWriter.WriteAttribute(SvXml::CTAG_THREAD_NAME, svValue);
		svValue = it->m_lAffinity;
		rWriter.WriteAttribute(SvXml::CTAG_THREAD_AFFINITY, svValue);
		rWriter.EndElement();
		iCount++;
	}

	rWriter.WriteAttribute(SvXml::CTAG_NEXT_OBJECT_ID, SVObjectManagerClass::Instance().getCurrentNextObjectId());

	rWriter.EndElement(); //End of Enviroment
}

void SVConfigurationObject::SaveIO(SvOi::IObjectWriter& rWriter) const
{
	rWriter.StartElement(SvXml::CTAG_IO);

	SVIOEntryHostStructPtrVector inputEntryVector;

	if (nullptr != m_pInputObjectList)
	{
		inputEntryVector = m_pInputObjectList->getInputList();
	}

	long lInSize = static_cast<long>(inputEntryVector.size());
	long lCount = 0;

	for (long lIn = 0; lIn < lInSize; lIn++)
	{
		std::string IOEntry = SvUl::Format(SvXml::CTAGF_IO_ENTRY_X, lCount);

		if (inputEntryVector[lIn]->m_ObjectType == IO_DIGITAL_INPUT)
		{
			rWriter.StartElement(IOEntry.c_str());

			_variant_t svVariant;

			SVDigitalInputObject* pInput = dynamic_cast<SVDigitalInputObject*> (m_pInputObjectList->GetInput(inputEntryVector[lIn]->m_IOId).get());
			if(nullptr == pInput) { continue; }

			std::string EntryName = pInput->GetName();
			svVariant.SetString(EntryName.c_str());
			rWriter.WriteAttribute(SvXml::CTAG_IO_ENTRY_NAME, svVariant);
			svVariant.Clear();

			svVariant = static_cast<int> (pInput->GetObjectSubType());
			rWriter.WriteAttribute(SvXml::CTAG_TYPE, svVariant);
			svVariant.Clear();

			svVariant = pInput->GetChannel();
			rWriter.WriteAttribute(SvXml::CTAG_CHANNEL, svVariant);
			svVariant.Clear();

			svVariant = false;
			rWriter.WriteAttribute(SvXml::CTAG_IS_OUTPUT, svVariant);
			svVariant.Clear(); //Must clear VT_BOOL Type after use

			svVariant = (pInput->IsInverted());
			rWriter.WriteAttribute(SvXml::CTAG_IS_INVERTED, svVariant);
			svVariant.Clear(); //Must clear VT_BOOL Type after use

			svVariant = pInput->IsForced();
			rWriter.WriteAttribute(SvXml::CTAG_IS_FORCED, svVariant);
			svVariant.Clear(); //Must clear VT_BOOL Type after use

			svVariant = pInput->GetForcedValue();
			rWriter.WriteAttribute(SvXml::CTAG_FORCED_VALUE, svVariant);
			svVariant.Clear(); //Must clear VT_BOOL Type after use

			rWriter.EndElement();

			lCount++;
		}
	}// end for

	SVIOEntryHostStructPtrVector outputEntryVector;
	long lOutSize = 0;
	if (nullptr != m_pOutputObjectList)
	{
		outputEntryVector = m_pOutputObjectList->getOutputList();
		lOutSize = static_cast<long>(outputEntryVector.size());
	}

	for (long lOut = 0; lOut < lOutSize; lOut++)
	{
		std::string IOEntry = SvUl::Format(SvXml::CTAGF_IO_ENTRY_X, lCount);

		if (outputEntryVector[lOut]->m_ObjectType == IO_DIGITAL_OUTPUT)
		{
			rWriter.StartElement(IOEntry.c_str());

			_variant_t svVariant;
			SVDigitalOutputObject* pOutput = dynamic_cast<SVDigitalOutputObject*> (m_pOutputObjectList->GetOutput(outputEntryVector[lOut]->m_IOId).get());
			if (nullptr == pOutput) { continue; }

			svVariant.SetString(pOutput->GetName());
			rWriter.WriteAttribute(SvXml::CTAG_IO_ENTRY_NAME, svVariant);
			svVariant.Clear();

			svVariant = static_cast<int> (pOutput->GetObjectSubType());
			rWriter.WriteAttribute(SvXml::CTAG_TYPE, svVariant);
			svVariant.Clear();

			svVariant = pOutput->GetChannel();
			rWriter.WriteAttribute(SvXml::CTAG_CHANNEL, svVariant);
			svVariant.Clear();

			svVariant = true;
			rWriter.WriteAttribute(SvXml::CTAG_IS_OUTPUT, svVariant);
			svVariant.Clear(); //Must clear VT_BOOL Type after use

			svVariant = (pOutput->IsInverted());
			rWriter.WriteAttribute(SvXml::CTAG_IS_INVERTED, svVariant);
			svVariant.Clear(); //Must clear VT_BOOL Type after use

			svVariant = (pOutput->IsForced());
			rWriter.WriteAttribute(SvXml::CTAG_IS_FORCED, svVariant);
			svVariant.Clear(); //Must clear VT_BOOL Type after use

			svVariant = (pOutput->GetForcedValue());
			rWriter.WriteAttribute(SvXml::CTAG_FORCED_VALUE, svVariant);
			svVariant.Clear(); //Must clear VT_BOOL Type after use

			svVariant = (pOutput->isCombined());
			rWriter.WriteAttribute(SvXml::CTAG_IS_COMBINED, svVariant);
			svVariant.Clear(); //Must clear VT_BOOL Type after use

			svVariant = pOutput->isAndACK();
			rWriter.WriteAttribute(SvXml::CTAG_IS_COMBINED_ACK, svVariant);
			svVariant.Clear(); //Must clear VT_BOOL Type after use

			rWriter.EndElement();

			lCount++;
		}
		else if(outputEntryVector[lOut]->m_ObjectType == IO_PLC_OUTPUT)
		{
			rWriter.StartElement(IOEntry.c_str());

			_variant_t svVariant;

			PlcOutputObject* pOutput = dynamic_cast<PlcOutputObject*> (m_pOutputObjectList->GetOutput(outputEntryVector[lOut]->m_IOId).get());
			if (nullptr == pOutput) { continue; }

			svVariant.SetString(pOutput->GetName());
			rWriter.WriteAttribute(SvXml::CTAG_IO_ENTRY_NAME, svVariant);
			svVariant.Clear();

			svVariant = static_cast<int> (pOutput->GetObjectSubType());
			rWriter.WriteAttribute(SvXml::CTAG_TYPE, svVariant);
			svVariant.Clear();

			svVariant = pOutput->GetChannel();
			rWriter.WriteAttribute(SvXml::CTAG_CHANNEL, svVariant);
			svVariant.Clear();

			svVariant = true;
			rWriter.WriteAttribute(SvXml::CTAG_IS_OUTPUT, svVariant);
			svVariant.Clear(); //Must clear VT_BOOL Type after use

			svVariant = (pOutput->isCombined());
			rWriter.WriteAttribute(SvXml::CTAG_IS_COMBINED, svVariant);
			svVariant.Clear(); //Must clear VT_BOOL Type after use

			svVariant = pOutput->isAndACK();
			rWriter.WriteAttribute(SvXml::CTAG_IS_COMBINED_ACK, svVariant);
			svVariant.Clear(); //Must clear VT_BOOL Type after use

			rWriter.EndElement();

			lCount++;
		}

	}// end for ( long lOut = 0; lOut < lOutSize; lOut++ )

	_variant_t svVariant = lCount;
	rWriter.WriteAttribute(SvXml::CTAG_NUMBER_OF_IO_ENTRIES, svVariant);


	//***************** Save the IODoc ... *****************************************
	if (nullptr != m_pIOController)
	{
		rWriter.StartElement(SvXml::CTAG_IODOC);
		m_pIOController->GetParameters(rWriter);
		rWriter.EndElement();
	}

	rWriter.EndElement(); //End of SvXml::CTAG_IO
}

void SVConfigurationObject::SaveAcquisitionDevice(SvOi::IObjectWriter& rWriter)  const
{
	rWriter.StartElement(SvXml::CTAG_ACQUISITION_DEVICE);

	std::string Name;
	SVFileNameArrayClass *pFiles = nullptr;
	SVLightReference *pLight = nullptr;
	SVLut* pLut = nullptr;
	SVDeviceParamCollection* pDeviceParams = nullptr;

	SVAcquisitionDeviceMap::const_iterator pos = m_AcquisitionDeviceMap.begin();
	while (pos != m_AcquisitionDeviceMap.end())
	{
		GetAcquisitionDeviceNextAssoc(pos, Name, pFiles, pLight, pLut, pDeviceParams);
		std::string Board;
		std::string Digitizer;

		size_t Pos = Name.find('.');
		Board = SvUl::Left(Name, Pos);

		if (!Board.empty())
		{
			Digitizer = SvUl::Mid(Name, Pos + 1);
			if (!Digitizer.empty())
			{
				rWriter.StartElement(Board.c_str());
				rWriter.StartElement(Digitizer.c_str());
				if (nullptr != pFiles)
				{
					std::string Files = pFiles->GetFileNameList();

					_variant_t svVariant;
					svVariant.SetString(Files.c_str());
					rWriter.WriteAttribute(SvXml::CTAG_ACQUISITION_DEVICE_FILE_NAME, svVariant);
				}

				if (nullptr != pDeviceParams)
				{
					// test for File Acquisition, since there is no LUT, Light, or DeviceParams per se
					if (Board == _T("File"))
					{
						SaveFileAcquisitionConfiguration(rWriter, *pDeviceParams);
					}
					else
					{
						if (nullptr != pLight && nullptr != pLut)
						{
							SaveAcquistionConfiguration(rWriter, *pLight, *pLut, *pDeviceParams);
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

void SVConfigurationObject::SaveAcquistionConfiguration(SvOi::IObjectWriter& rWriter, const SVLightReference& rLight, const SVLut& rLut, const SVDeviceParamCollection& rDeviceParams) const
{
	rWriter.StartElement(SvXml::CTAG_LIGHT_REFERENCE_ARRAY);

	_variant_t svVariant = rLight.NumBands();
	rWriter.WriteAttribute(SvXml::CTAG_SIZE, svVariant);

	for (int i = 0; i < rLight.NumBands(); i++)
	{
		std::string Band = SvUl::Format(SvXml::CTAGF_BAND_X, i);
		rWriter.StartElement(Band.c_str());

		svVariant = rLight.Band(i).NumAttributes();
		rWriter.WriteAttribute(SvXml::CTAG_SIZE, svVariant);
		svVariant.Clear();

		for (int j = 0; j < rLight.Band(i).NumAttributes(); j++)
		{
			std::string LightRef = SvUl::Format(SvXml::CTAGF_LIGHTREFERENCE_X, j);

			rWriter.StartElement(LightRef.c_str());

			svVariant.SetString(rLight.Band(i).Attribute(j).strName.c_str());
			rWriter.WriteAttribute(SvXml::CTAG_NAME, svVariant);
			svVariant.Clear();

			svVariant = rLight.Band(i).Attribute(j).iIDCaption;
			rWriter.WriteAttribute(SvXml::CTAG_RESOURCE_ID, svVariant);
			svVariant.Clear();

			svVariant = rLight.Band(i).Attribute(j).dwType;
			rWriter.WriteAttribute(SvXml::CTAG_TYPE, svVariant);
			svVariant.Clear();

			svVariant = rLight.Band(i).Attribute(j).lValue;
			rWriter.WriteAttribute(SvXml::CTAG_VALUE, svVariant);
			svVariant.Clear();
			rWriter.EndElement(); // csRef
		}// end for ( int j = 0; j < rLight.Band( i ).NumAttributes(); j++ )

		rWriter.EndElement(); // csBand
	}// end for ( int i = 0; i < rLight.NumBands(); i++)
	rWriter.EndElement(); // SvXml::CTAG_LIGHT_REFERENCE_ARRAY

	rWriter.StartElement(SvXml::CTAG_LUT);

	svVariant = rLut.Info().Bands();
	rWriter.WriteAttribute(SvXml::CTAG_SIZE, svVariant);
	svVariant.Clear();

	svVariant = rLut.Info().MaxValue();
	rWriter.WriteAttribute(SvXml::CTAG_MAX_VALUE, svVariant);
	svVariant.Clear();

	for (int iBand = 0; iBand < static_cast<int>(rLut.Info().Bands()); iBand++)
	{
		std::string Band = SvUl::Format(SvXml::CTAGF_BAND_X, iBand);
		rWriter.StartElement(Band.c_str());

		SAFEARRAY* psaParam = nullptr;
		rLut(iBand).Info().GetTransformParameters(psaParam);
		svVariant = SvUl::SVSAFEARRAY(psaParam);
		rWriter.WriteAttribute(SvXml::CTAG_LUT_TRANSFORM_PARAMETERS, svVariant);
		svVariant.Clear();

		const SVLutTransformOperation* pOperation = rLut(iBand).GetTransformOperation();
		SVLutTransformOperationEnum e = SVLutTransform::GetEquivalentType(pOperation);
		assert(e != LutTransformTypeUnknown);
		svVariant = (long)e;
		rWriter.WriteAttribute(SvXml::CTAG_LUT_TRANSFORM_OPERATION, svVariant);
		svVariant.Clear();

		SAFEARRAY* psaBandData = nullptr;
		rLut(iBand).GetBandData(psaBandData);
		svVariant = SvUl::SVSAFEARRAY(psaBandData);
		rWriter.WriteAttribute(SvXml::CTAG_LUT_BAND_DATA, svVariant);
		svVariant.Clear();
		rWriter.EndElement(); // csBand
	}
	rWriter.EndElement(); // SvXml::CTAG_LUT

	SaveDeviceParameters(rWriter, rDeviceParams);
}

void SVConfigurationObject::SaveCamera(SvOi::IObjectWriter& rWriter) const
{
	rWriter.StartElement(SvXml::CTAG_CAMERA);

	long lCount = GetCameraCount();

	for (long l = 0; l < lCount; l++)
	{
		SvIe::SVVirtualCamera* pCamera = GetCamera(l);

		if (nullptr != pCamera)
		{
			rWriter.StartElement(pCamera->GetName());

			if (nullptr != pCamera->GetAcquisitionDevice())
			{
				_variant_t svVariant;
				svVariant = pCamera->getCameraID();
				rWriter.WriteAttribute(SvXml::CTAG_CAMERA_ID, svVariant);
				svVariant.Clear();

				//@WARNING [gra][7.40][02.11.2016] We need to add the channel for forward compatibility with version 7.30, can be removed in next version
				std::string AcquisitionName = pCamera->GetAcquisitionDevice()->DeviceName();
				if (pCamera->IsColor())
				{
					AcquisitionName += _T(".Ch_All");
				}
				else
				{
					AcquisitionName += _T(".Ch_0");
				}
				svVariant.SetString(AcquisitionName.c_str());
				rWriter.WriteAttribute(SvXml::CTAG_ACQUISITION_DEVICE, svVariant);
				svVariant.Clear();

				svVariant = convertObjectIdToVariant(pCamera->getObjectId());
				rWriter.WriteAttribute(SvXml::CTAG_UNIQUE_REFERENCE_ID, svVariant);
				svVariant.Clear();

				svVariant = pCamera->BandLink();
				rWriter.WriteAttribute(SvXml::CTAG_BAND_LINK, svVariant);
				svVariant.Clear();

				svVariant = pCamera->IsColor();
				rWriter.WriteAttribute(SvXml::CTAG_COLOR, svVariant);
				svVariant.Clear();

				svVariant = pCamera->IsFileAcquisition();
				rWriter.WriteAttribute(SvXml::CTAG_FILEACQUISITION_MODE, svVariant);
				svVariant.Clear();

				if (pCamera->IsFileAcquisition())
				{
					svVariant = pCamera->GetImageFilename();
					rWriter.WriteAttribute(SvXml::CTAG_FILEACQUISITION_IMAGE_FILENAME, svVariant);
					svVariant.Clear();

					svVariant = pCamera->GetImageDirectoryName();
					rWriter.WriteAttribute(SvXml::CTAG_FILEACQUISITION_IMAGE_DIRECTORYNAME, svVariant);
					svVariant.Clear();

					svVariant = pCamera->GetFileLoadingMode();
					rWriter.WriteAttribute(SvXml::CTAG_FILEACQUISITION_LOADINGMODE, svVariant);
					svVariant.Clear();

					svVariant = (pCamera->IsFileImageSizeEditModeFileBased() ? 1 : 0);
					rWriter.WriteAttribute(SvXml::CTAG_FILEACQUISITION_IMAGE_SIZE_EDIT_MODE, svVariant);
					svVariant.Clear();

					const SIZE& size = pCamera->GetFileImageSize();
					svVariant = size.cx;
					rWriter.WriteAttribute(SvXml::CTAG_FILEACQUISITION_FILE_IMAGE_WIDTH, svVariant);
					svVariant.Clear();

					svVariant = size.cy;
					rWriter.WriteAttribute(SvXml::CTAG_FILEACQUISITION_FILE_IMAGE_HEIGHT, svVariant);
					svVariant.Clear();
				}
			}

			rWriter.EndElement(); // pCamera->GetName()
		}
	}// end for ( long l = 0; l < lCount; l++ )

	rWriter.EndElement(); //SvXml::CTAG_CAMERA
}

void SVConfigurationObject::SaveTrigger(SvOi::IObjectWriter& rWriter) const
{
	rWriter.StartElement(SvXml::CTAG_TRIGGER);

	long lCount = GetTriggerCount();
	for (long l = 0; l < lCount; l++)
	{
		SvTi::SVTriggerObject* pTrigger = GetTrigger(l);

		if (nullptr != pTrigger)
		{
			rWriter.StartElement(pTrigger->GetName());

			if (nullptr != pTrigger->getDevice())
			{
				_variant_t svVariant;
				svVariant.SetString(pTrigger->getDevice()->GetDeviceName());
				rWriter.WriteAttribute(SvXml::CTAG_TRIGGER_DEVICE, svVariant);
			}
			if (SvDef::TriggerType::SoftwareTrigger == pTrigger->getType())
			{
				_variant_t svVariant = true;
				rWriter.WriteAttribute(SvXml::CTAG_SOFTWARETRIGGER_DEVICE, svVariant);
				svVariant.Clear();

				svVariant = pTrigger->GetSoftwareTriggerPeriod();
				rWriter.WriteAttribute(SvXml::CTAG_SOFTWARETRIGGER_PERIOD, svVariant);
				svVariant.Clear();
			}
			rWriter.EndElement(); //pTrigger->GetName()
		}
	}// end for ( long l = 0; l < lCount; l++ )
	rWriter.EndElement();  // SvXml::CTAG_TRIGGER
}

void SVConfigurationObject::SaveInspection(SvOi::IObjectWriter& rWriter, AttributesSetMap& rAttributeSetPairVector) const
{
	rWriter.StartElement(SvXml::CTAG_INSPECTION);

	long lCount = GetInspectionCount();
	for (long l = 0; l < lCount; l++)
	{
		SVInspectionProcess* pInspection = GetInspection(l);

		if (nullptr != pInspection)
		{
			getInspectionObjectAttributesSet(pInspection, rAttributeSetPairVector);
			rWriter.StartElement(pInspection->GetName());

			_variant_t svVariant;
			svVariant.SetString(pInspection->GetName());
			rWriter.WriteAttribute(SvXml::CTAG_INSPECTION_FILE_NAME, svVariant);
			svVariant.Clear();

			svVariant.SetString(pInspection->GetToolsetImage());
			rWriter.WriteAttribute(SvXml::CTAG_INSPECTION_TOOLSET_IMAGE, svVariant);
			svVariant.Clear();

			//Inspection Process
			pInspection->Persist(rWriter);

			//SVIPDoc
			SVIPDoc* pDoc = TheSVObserverApp.GetIPDoc(pInspection->getObjectId());
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

void SVConfigurationObject::SavePPQ(SvOi::IObjectWriter& rWriter) const
{
	rWriter.StartElement(SvXml::CTAG_PPQ);

	std::string strName;
	SVPPQObject* pPPQ(nullptr);
	long lPPQCount = GetPPQCount();

	for (long lPPQ = 0; lPPQ < lPPQCount; lPPQ++)
	{
		pPPQ = this->GetPPQ(lPPQ);
		if (nullptr != pPPQ)
		{
			strName = pPPQ->GetName();
			rWriter.StartElement(strName.c_str());

			SavePPQ_Attributes(rWriter, *pPPQ);

			if (nullptr != pPPQ->GetTrigger())
			{
				rWriter.StartElement(SvXml::CTAG_TRIGGER);
				strName = pPPQ->GetTrigger()->GetName();
				rWriter.StartElement(strName.c_str());
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

void SVConfigurationObject::SavePPQ_Attributes(SvOi::IObjectWriter& rWriter, const SVPPQObject& rPPQ) const
{
	_variant_t svValue = convertObjectIdToVariant(rPPQ.getObjectId());
	rWriter.WriteAttribute(SvXml::CTAG_UNIQUE_REFERENCE_ID, svValue);
	svValue.Clear();

	svValue = rPPQ.getPPQOutputMode();
	rWriter.WriteAttribute(SvXml::CTAG_PPQ_MODE, svValue);
	svValue.Clear();

	svValue = rPPQ.getPPQLength();
	rWriter.WriteAttribute(SvXml::CTAG_PPQ_LENGTH, svValue);
	svValue.Clear();

	svValue = rPPQ.getResetDelay();
	rWriter.WriteAttribute(SvXml::CTAG_PPQ_OUTPUT_RESET_DELAY_TIME, svValue);
	svValue.Clear();

	svValue = rPPQ.getOutputDelay();
	rWriter.WriteAttribute(SvXml::CTAG_PPQ_OUTPUT_DELAY_TIME, svValue);
	svValue.Clear();

	svValue = rPPQ.getMaintainSourceImages();
	rWriter.WriteAttribute(SvXml::CTAG_PPQ_MAINTAIN_SRC_IMAGE, svValue);
	svValue.Clear();

	svValue = rPPQ.getMaxProcessingOffset4Interest();
	rWriter.WriteAttribute(SvXml::CTAG_PPQ_MAX_PROCESSINGOFFSET_4INTEREST, svValue);
	svValue.Clear();

	svValue = rPPQ.getInspectionTimeout();
	rWriter.WriteAttribute(SvXml::CTAG_PPQ_INSPECTION_TIMEOUT, svValue);
	svValue.Clear();

	// Conditional Output.
	const std::string& l_condition = rPPQ.GetConditionalOutputName();
	svValue.SetString(l_condition.c_str());
	rWriter.WriteAttribute(SvXml::CTAG_PPQ_CONDITIONAL_OUTPUT, svValue);
	svValue.Clear();
}

void SVConfigurationObject::SavePPQ_Cameras(SvOi::IObjectWriter& rWriter, const SVPPQObject& rPPQ) const
{
	SvIe::SVVirtualCameraPtrVector cameraVector = rPPQ.GetVirtualCameras();
	if (0 < cameraVector.size())
	{
		rWriter.StartElement(SvXml::CTAG_CAMERA);

		for(const auto* const pCamera : cameraVector)
		{
			if (nullptr != pCamera)
			{
				std::string strName = pCamera->GetName();
				rWriter.StartElement(strName.c_str());

				long l_PPQIndex = -1;
				rPPQ.GetCameraPPQPosition(l_PPQIndex, pCamera);

				_variant_t svValue = l_PPQIndex;
				rWriter.WriteAttribute(SvXml::CTAG_POSITION, svValue);
				svValue.Clear();
				rWriter.EndElement();
			}
		}
		rWriter.EndElement(); //SvXml::CTAG_CAMERA
	}
}

void SVConfigurationObject::SavePPQ_Inspections(SvOi::IObjectWriter& rWriter, const SVPPQObject& rPPQ) const
{
	long lInspectCount;
	rPPQ.GetInspectionCount(lInspectCount);
	if (lInspectCount)
	{
		rWriter.StartElement(SvXml::CTAG_INSPECTION);
		SVInspectionProcess *pInspection(nullptr);
		for (long lInspect = 0; lInspect < lInspectCount; lInspect++)
		{
			rPPQ.GetInspection(lInspect, pInspection);
			if (nullptr != pInspection)
			{
				std::string strName = pInspection->GetName();
				rWriter.StartElement(strName.c_str());
				rWriter.EndElement();
			}// end if
		}// end for( lInspect = 0; lInspect < lInspectCount; lInspect++ )

		rWriter.EndElement(); //SvXml::CTAG_INSPECTION
	}// end if
}

bool SVConfigurationObject::SaveRemoteMonitorList(SvOi::IObjectWriter& rWriter) const
{
	bool bOk = true;
	rWriter.StartElement(SvXml::CTAG_MONITOR_LISTS);

	const RemoteMonitorListMap& remoteMonitorLists = GetRemoteMonitorList();
	RemoteMonitorListMap::const_iterator iterMonitorList = remoteMonitorLists.begin();
	while (bOk && remoteMonitorLists.end() != iterMonitorList)
	{
		const std::string& strName = iterMonitorList->first;
		rWriter.StartElement(strName.c_str());

		_variant_t svValue;
		const RemoteMonitorNamedList& monitorList = iterMonitorList->second;
		svValue.SetString(monitorList.GetPPQName().c_str());
		rWriter.WriteAttribute(SvXml::CTAG_PPQ_NAME, svValue);
		svValue.Clear();

		svValue = monitorList.GetRejectDepthQueue();
		rWriter.WriteAttribute(SvXml::CTAG_REJECT_QUEUE_DEPTH, svValue);
		svValue.Clear();

		svValue = monitorList.IsActive();
		rWriter.WriteAttribute(SvXml::CTAG_IS_ACTIVATED, svValue);
		svValue.Clear();

		bOk = SaveMonitoredObjectList(rWriter, SvXml::CTAG_PRODUCTVALUES_LIST, monitorList.GetProductValuesList()) && bOk;
		bOk = SaveMonitoredObjectList(rWriter, SvXml::CTAG_PRODUCTIMAGE_LIST, monitorList.GetProductImagesList()) && bOk;
		bOk = SaveMonitoredObjectList(rWriter, SvXml::CTAG_REJECTCONDITION_LIST, monitorList.GetRejectConditionList()) && bOk;
		bOk = SaveMonitoredObjectList(rWriter, SvXml::CTAG_FAILSTATUS_LIST, monitorList.GetFailStatusList()) && bOk;
		rWriter.EndElement(); //strName.c_str()
		++iterMonitorList;
	}

	rWriter.EndElement(); //SvXml::CTAG_MONITOR_LISTS
	return bOk;
}

bool SVConfigurationObject::SaveMonitoredObjectList(SvOi::IObjectWriter& rWriter, const std::string& listName, const MonitoredObjectList& rList) const
{
	bool bOk = true;
	rWriter.StartElement(listName.c_str());

	_variant_t svValue;
	svValue.SetString(_T(""));
	MonitoredObjectList::const_iterator iter = rList.begin();
	while (bOk && rList.end() != iter)
	{
		const MonitoredObject& rObj = *iter;
		const std::string& objectName = RemoteMonitorListHelper::GetNameFromMonitoredObject(rObj);
		if (!objectName.empty())
		{
			rWriter.WriteAttribute(objectName.c_str(), svValue);
		}
		else
		{
			bOk = false;
		}
		++iter;
	}
	rWriter.EndElement();
	return bOk;
}

void SVConfigurationObject::SaveGlobalConstants(SvOi::IObjectWriter& rWriter) const
{
	rWriter.StartElement(SvXml::CTAG_GLOBAL_CONSTANTS);

	SvVol::BasicValueObjects::ValueVector GlobalConstantObjects;
	RootObject::getRootChildObjectList(GlobalConstantObjects, SvDef::FqnGlobal, 0);
	SvVol::BasicValueObjects::ValueVector::const_iterator Iter(GlobalConstantObjects.cbegin());
	while (GlobalConstantObjects.cend() != Iter  && nullptr != *Iter)
	{
		std::string Name((*Iter)->GetCompleteName());
		rWriter.StartElement(Name.c_str());

		_variant_t Value;
		(*Iter)->getValue(Value);
		rWriter.WriteAttribute(SvXml::CTAG_VALUE, Value);
		Value.Clear();

		//Save the global constant unique ID id to have the same id when loading the configuration
		Value = convertObjectIdToVariant((*Iter)->getObjectId());
		rWriter.WriteAttribute(scUniqueReferenceIDTag, Value);
		Value.Clear();

		std::string Description((*Iter)->getDescription());
		//This is needed to remove any CR LF in the description
		SvUl::AddEscapeSpecialCharacters(Description, true);
		Value.SetString(Description.c_str());
		rWriter.WriteAttribute(SvXml::CTAG_DESCRIPTION, Value);
		Value.Clear();

		rWriter.EndElement();
		++Iter;
	}

	rWriter.EndElement(); //SvXml::CTAG_GLOBAL_CONSTANTS
}

void SVConfigurationObject::SaveObjectAttributesSet(SvOi::IObjectWriter& rWriter, const AttributesSetMap& rAttributesSetMap) const
{
	const std::map<UINT, std::string> AttributeSetTypes
	{
		{ SvPb::publishable, std::string(_T("Publishable Value")) },
		{ SvPb::publishResultImage, std::string(_T("Publishable Image")) },
		{ SvPb::dataDefinitionValue, std::string(_T("Data Definition Value")) },
		{ SvPb::dataDefinitionImage, std::string(_T("Data Definition Image")) }
	};

	rWriter.StartElement(SvXml::CTAG_OBJECT_ATTRIBUTES_SET);
	for (auto const& rEntry : rAttributesSetMap)
	{
		std::map<UINT, std::string>::const_iterator Iter = AttributeSetTypes.find(rEntry.first);
		if (AttributeSetTypes.end() != Iter)
		{
			rWriter.StartElement(Iter->second.c_str());
			uint32_t arrayId{ SvDef::InvalidObjectId };
			std::string ArrayIndexList;
			int StartIndex {-1};
			int EndIndex {-1};
			for (auto const& rObjectRef : rEntry.second)
			{
				//!If previous object is an array and new object is different then we need to save the array values
				if (SvDef::InvalidObjectId != arrayId && rObjectRef.getObjectId() != arrayId)
				{
					ArrayIndexList += (StartIndex == EndIndex) ? SvUl::Format(_T("%d,"), StartIndex) : SvUl::Format(_T("%d-%d,"), StartIndex, EndIndex);
					_variant_t Value {ArrayIndexList.c_str()};
					rWriter.WriteAttribute(scUniqueReferenceIDTag, Value);

					arrayId = SvDef::InvalidObjectId;
					StartIndex = -1;
					EndIndex = -1;
					ArrayIndexList.clear();
				}
				if (!rObjectRef.isArray())
				{
					//Not an array element is saved directly
					_variant_t Value {rObjectRef.GetObjectIdAndIndexOneBased().c_str()};
					rWriter.WriteAttribute(scUniqueReferenceIDTag, Value);
				}
				else
				{
					//If an array element then bundle the zero based indexes in the format 2-15,20,25-30 etc..
					if (SvDef::InvalidObjectId == arrayId)
					{
						arrayId = rObjectRef.getObjectId();
						ArrayIndexList = convertObjectIdToString(arrayId);
						StartIndex = rObjectRef.ArrayIndex();
						EndIndex = rObjectRef.ArrayIndex();
					}
					else if (arrayId == rObjectRef.getObjectId())
					{
						if (EndIndex + 1 == rObjectRef.ArrayIndex())
						{
							EndIndex = rObjectRef.ArrayIndex();
						}
						else
						{
							ArrayIndexList += (StartIndex == EndIndex) ? SvUl::Format(_T("%d,"), StartIndex) : SvUl::Format(_T("%d-%d,"), StartIndex, EndIndex);
							StartIndex = rObjectRef.ArrayIndex();
							EndIndex = rObjectRef.ArrayIndex();
						}
					}
				}
			}
			//! Last object is of array then we need to save the object
			if (SvDef::InvalidObjectId != arrayId)
			{
				ArrayIndexList += (StartIndex == EndIndex) ? SvUl::Format(_T("%d,"), StartIndex) : SvUl::Format(_T("%d-%d,"), StartIndex, EndIndex);
				_variant_t Value {ArrayIndexList.c_str()};
				rWriter.WriteAttribute(scUniqueReferenceIDTag, Value);
			}
			rWriter.EndElement();
		}
	}
	rWriter.EndElement(); //SvXml::CTAG_OBJECT_ATTRIBUTES_SET
}

void SVConfigurationObject::SaveAdditionalFiles(SvOi::IObjectWriter& rWriter) const
{
	if(m_AdditionalFiles.size() > 0)
	{
		rWriter.StartElement(SvXml::CTAG_ADDITIONAL_CONFIG_FILES);
		for(const auto& rFile : m_AdditionalFiles)
		{
			_variant_t Value{rFile.GetFullFileName().c_str()};
			rWriter.WriteAttribute(SvXml::CTAG_FILENAME, Value);
			Value.Clear();
		}

		rWriter.EndElement(); //SvXml::CTAG_ADDITIONAL_CONFIG_FILES
	}
}

void SVConfigurationObject::getInspectionObjectAttributesSet(const SVInspectionProcess* pInspection, AttributesSetMap& rAttributesSetMap) const
{
	if (nullptr != pInspection)
	{
		SVToolSet* pToolSet = pInspection->GetToolSet();
		if (nullptr != pToolSet)
		{
			SVOutputInfoListClass ToolSetOutputList;
			pToolSet->GetOutputList(ToolSetOutputList);

			ToolSetOutputList.GetSetAttributesList(SvPb::publishable, rAttributesSetMap[SvPb::publishable]);
			ToolSetOutputList.GetSetAttributesList(SvPb::publishResultImage, rAttributesSetMap[SvPb::publishResultImage]);
			ToolSetOutputList.GetSetAttributesList(SvPb::dataDefinitionValue, rAttributesSetMap[SvPb::dataDefinitionValue]);
			ToolSetOutputList.GetSetAttributesList(SvPb::dataDefinitionImage, rAttributesSetMap[SvPb::dataDefinitionImage]);
		}
	}
}


void SVConfigurationObject::ConvertColorToStandardProductType(bool& rConfigColor)
{
	SVIMProductEnum CurrentType(TheSVObserverApp.GetSVIMType());
	SVIMProductEnum ConfigType(GetProductType());

	//Need to change the product type from color to standard product type
	bool isColor = SvTi::SVHardwareManifest::IsColorSystem(ConfigType);
	if (isColor)
	{
		rConfigColor = isColor;
		SetProductType(CurrentType);
		SVSVIMStateClass::AddState(SV_STATE_MODIFIED);
	}
}

void SVConfigurationObject::SaveConfiguration(SvXml::SVObjectXMLWriter& rWriter) const
{
	std::string RootName(SvDef::FqnRoot);
	rWriter.WriteRootElement(RootName.c_str());
	rWriter.WriteSchema();

	DWORD versionNumber = TheSVObserverApp.getCurrentVersion();
	std::string versionString = SvUl::Format("%d.%d", versionNumber >> 16, (versionNumber >> 8) & 0x000000ff);
	rWriter.WriteRevisionHistory(versionString.c_str(), 1);
	rWriter.WriteStartOfBase();
	AttributesSetMap AttributeSetPairVector;

	SaveEnvironment(rWriter);
	SaveIO(rWriter);
	SaveAcquisitionDevice(rWriter);
	SaveCamera(rWriter);
	SaveTrigger(rWriter);
	SaveInspection(rWriter, AttributeSetPairVector);
	SavePPQ(rWriter);
	SaveRemoteMonitorList(rWriter);
	SaveGlobalConstants(rWriter);
	SaveObjectAttributesSet(rWriter, AttributeSetPairVector);
	SaveAdditionalFiles(rWriter);

	rWriter.EndElement(); // end of BaseNode
	rWriter.EndElement(); // end of Root Element
}

void SVConfigurationObject::SaveFileAcquisitionConfiguration(SvOi::IObjectWriter& rWriter, const SVDeviceParamCollection& rDeviceParams) const
{
	// save the camera params
	SaveDeviceParameters(rWriter, rDeviceParams);
}

void SVConfigurationObject::SaveDeviceParameters(SvOi::IObjectWriter& rWriter, const SVDeviceParamCollection& rDeviceParams) const
{
	rWriter.StartElement(SvXml::CTAG_DEVICE_PARAM_LIST);

	SVDeviceParamMap::const_iterator iter;
	int i = 0;
	for (iter = rDeviceParams.mapParameters.begin(); iter != rDeviceParams.mapParameters.end(); ++iter)
	{
		const SVDeviceParam* pParam = iter->second;
		if (nullptr != pParam)
		{
			if (pParam->Type() != DeviceParamInvalid)
			{
				VARIANT vValue;
				::VariantInit(&vValue);
				HRESULT hrValue = pParam->GetValue(vValue);
				if (S_OK == hrValue)
				{
					std::string Parameter = SvUl::Format(SvXml::CTAGF_DEVICE_PARAM_X, ++i);
					rWriter.StartElement(Parameter.c_str());

					_variant_t svVariant;
					svVariant = SVDeviceParam::GetParameterName(pParam->Type()).c_str();
					rWriter.WriteAttribute(SvXml::CTAG_NAME, svVariant);
					svVariant.Clear();

					svVariant = (long)pParam->Type();
					rWriter.WriteAttribute(SvXml::CTAG_DEVICE_PARAM_TYPE, svVariant);
					svVariant.Clear();

					svVariant = vValue;
					rWriter.WriteAttribute(SvXml::CTAG_DEVICE_PARAM_VALUE, svVariant);
					svVariant.Clear();

					// HACK!!! EB 20031215: This was put in at Jim's request
					// to clarify to someone looking at the config what units the value is stored in.
					svVariant.SetString(_T("native"));
					rWriter.WriteAttribute(SvXml::CTAG_DEVICE_PARAM_UNITS, svVariant);
					svVariant.Clear();

					SaveDeviceParamSpecial(rWriter, pParam);

					rWriter.EndElement();
				}
			}
		}
	}

	rWriter.EndElement(); //SvXml::CTAG_DEVICE_PARAM_LIST
}

void SVConfigurationObject::SaveDeviceParamSpecial(SvOi::IObjectWriter& rWriter, const SVDeviceParam* pParam) const
{
	_variant_t svVariant;
	VARIANT vValue;
	::VariantInit(&vValue);

	switch (pParam->Type())
	{
		case DeviceParamCameraFormats:
		{
			const SVCameraFormatsDeviceParam* pcf = dynamic_cast<const SVCameraFormatsDeviceParam*> (pParam);
			std::string Parameter(SvXml::CTAG_OPTIONS);
			rWriter.StartElement(Parameter.c_str());

			int iOption = 0;
			SVCameraFormatsDeviceParam::OptionsType::const_iterator iter;
			for (iter = pcf->options.begin(); iter != pcf->options.end(); ++iter)
			{
				const SVCameraFormat& rcf = iter->second;
				Parameter = SvUl::Format(SvXml::CTAGF_OPTION_X, ++iOption);
				rWriter.StartElement(Parameter.c_str());

				svVariant.SetString(rcf.m_strName.c_str());
				rWriter.WriteAttribute(SvXml::CTAG_NAME, svVariant);
				svVariant.Clear();

				svVariant = rcf.m_bVariableROI;
				rWriter.WriteAttribute(SvXml::CTAG_VARIABLE_CAMERA_FORMAT, svVariant);
				svVariant.Clear();

				svVariant = rcf.m_lHPos;
				rWriter.WriteAttribute(SvXml::CTAG_LEFT, svVariant);
				svVariant.Clear();

				svVariant = rcf.m_lVPos;
				rWriter.WriteAttribute(SvXml::CTAG_TOP, svVariant);
				svVariant.Clear();

				svVariant = rcf.m_lWidth;
				rWriter.WriteAttribute(SvXml::CTAG_CX, svVariant);
				svVariant.Clear();

				svVariant = rcf.m_lHeight;
				rWriter.WriteAttribute(SvXml::CTAG_CY, svVariant);
				svVariant.Clear();

				rWriter.EndElement(); // SvXml::CTAGF_OPTION_X
			}// end for ( iter = pcf->options.begin(); iter != pcf->options.end(); iter++)
			rWriter.EndElement(); // SvXml::CTAG_OPTIONS
		}// end case DeviceParamCameraFormats:	
		break;
	}// end switch ( pParam->Type() )
}

bool SVConfigurationObject::FinishIPDoc(SVInspectionProcess* pInspection, bool isLoad)
{
	bool bOk = false;

	if (nullptr != pInspection)
	{
		SVPPQObject* pPPQ(pInspection->GetPPQ());

		if (nullptr != pPPQ)
		{
			pPPQ->m_pInputList = GetInputObjectList();
			pPPQ->RebuildInputList(HasCameraTrigger(pPPQ));

			pPPQ->m_pOutputList = GetOutputObjectList();
			pPPQ->RebuildOutputList();

			pInspection->RebuildInspectionInputList();

			pInspection->RebuildInspection(isLoad);

			// Init Document
			pInspection->ValidateAndInitialize(true);

			bOk = true;
		}

	}

	return bOk;
}

bool SVConfigurationObject::Activate()
{
	bool bOk = true;

	long lSize = static_cast<long> (m_arInspectionArray.size());

	for (long l = lSize - 1; -1 < l; l--)
	{
		SVInspectionProcess* pInspection = m_arInspectionArray[l];
		SvCmd::RunOnceSynchronous(pInspection->getObjectId());
	}

	return bOk;
}

bool SVConfigurationObject::RebuildInputOutputLists(bool isLoad)
// called after Environment is edited
{
	bool bOk = true;

	try
	{
		//avoid that TRC-memory will be recreated for every inspection, but do it once at the end.
		SvTrc::getTriggerRecordControllerRWInstance().setGlobalInit();
		for (auto pInspection : m_arInspectionArray)
		{
			if (nullptr != pInspection)
			{
				bOk = FinishIPDoc(pInspection, isLoad) && bOk;
			}
		}
		SvTrc::getTriggerRecordControllerRWInstance().finishGlobalInit();
	}
	catch (const SvStl::MessageContainer& rSvE)
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display );
		Exception.setMessage(rSvE.getMessage());
		return false;
	}

	for (auto pInspection : m_arInspectionArray)
	{
		if (nullptr != pInspection)
		{
			SVSVIMStateClass::AddState(SV_STATE_INTERNAL_RUN);
			pInspection->RunOnce();
			SVSVIMStateClass::RemoveState(SV_STATE_INTERNAL_RUN);
		}
	}

	if (nullptr != m_pIOController)
	{
		m_pIOController->RebuildOutputList();
	}
	return bOk;
}

SVIMProductEnum SVConfigurationObject::GetProductType() const
{
	return m_eProductType;
}

void SVConfigurationObject::SetProductType(SVIMProductEnum eProductType)
{
	SVIMProductEnum prevType{ SVIM_PRODUCT_TYPE_UNKNOWN == m_eProductType ? eProductType : m_eProductType};
	m_eProductType = eProductType;
	///When the product type is changed the IO controller needs to initialize outputs
	m_pIOController->initializeOutputs();

	///When changing from discrete IO to PLC or vice versa then we need to reset the inputs and outputs
	if ((SvTi::SVHardwareManifest::isPlcSystem(m_eProductType) != SvTi::SVHardwareManifest::isPlcSystem(prevType)) ||
		(SvTi::SVHardwareManifest::isDiscreteIOSystem(m_eProductType) != SvTi::SVHardwareManifest::isDiscreteIOSystem(prevType)))
	{
		changeSystemResetIO(m_eProductType);
	}
}

bool SVConfigurationObject::IsConfigurationLoaded() const
{
	return m_bConfigurationValid;
}

void SVConfigurationObject::SetConfigurationLoaded()
{
	m_bConfigurationValid = true;
}

void SVConfigurationObject::SetupSoftwareTrigger(SvTh::SVTriggerClass* pTrigger, int iDigNum, long triggerPeriod, SVPPQObject* pPPQ)
{
	assert(nullptr != pTrigger && nullptr != pPPQ);
	if (nullptr == pTrigger || nullptr == pPPQ)
	{
		return;
	}

	unsigned long triggerHandle;
	pTrigger->getDLLTrigger()->GetHandle(&triggerHandle, iDigNum);
	VARIANT l_vValue;
	::VariantInit(&l_vValue);
	l_vValue.vt = VT_I4;
	l_vValue.lVal = triggerPeriod;

	// Setup the Triggering device
	pTrigger->getDLLTrigger()->SetParameterValue(triggerHandle, 0, &l_vValue);

	// get Handles for all cameras associated with this trigger
	SvIe::SVVirtualCameraPtrVector cameraVector = pPPQ->GetVirtualCameras();

	for (auto* pCamera : cameraVector)
	{
		if (nullptr != pCamera)
		{
			SvIe::SVAcquisitionClassPtr pAcq = pCamera->GetAcquisitionDevice();
			if (nullptr != pAcq)
			{
				// need the digitizer name here ...
				SvTh::SVDigitizerLoadLibraryClass* pAcqDLL = SvIe::SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(pAcq->DigName().c_str());
				if (pAcqDLL)
				{
					pTrigger->addAcquisitionTrigger(pAcqDLL, pAcq->m_hDigitizer);
				}
			}
		}
	}
}

void SVConfigurationObject::SetupCameraTrigger(SvTh::SVTriggerClass* pTriggerDevice, int iDigNum, SVPPQObject* pPPQ, bool bSoftwareTrigger, long triggerPeriod)
{
	assert(nullptr != pTriggerDevice && nullptr != pPPQ);
	if (nullptr == pTriggerDevice || nullptr == pPPQ)
	{
		return;
	}

	if (bSoftwareTrigger)
	{
		std::string DeviceName = SvTi::SVHardwareManifest::BuildSoftwareTriggerDeviceName(iDigNum);
		SvTh::SVTriggerClass* pTrigger = SvTi::SVTriggerProcessingClass::Instance().GetTrigger(DeviceName.c_str());

		SetupSoftwareTrigger(pTrigger, iDigNum, triggerPeriod, pPPQ);
	}
	else
	{
		if (pTriggerDevice)
		{
			// reassign trigger handle to match camera digitizer handle
			long l_Count = GetCameraCount();
			for (long i = 0; i < l_Count; i++)
			{
				SvIe::SVVirtualCamera* pCamera = GetCamera(i);
				if (nullptr != pCamera)
				{
					if (nullptr != pCamera->GetAcquisitionDevice())
					{
						if (iDigNum == pCamera->GetAcquisitionDevice()->DigNumber())
						{
							pTriggerDevice->setTriggerChannel(pCamera->GetAcquisitionDevice()->m_hDigitizer);
							break;
						}
					}
				}
			}
		}
	}

	if (pPPQ)
	{
		SvTi::SVCameraTriggerData& rCameraTriggerData = pPPQ->GetCameraInputData();
		SVIOEntryHostStructPtr ioEntry = rCameraTriggerData.SetupLineStateInput();
		AddCameraDataInput(pPPQ, ioEntry);

		ioEntry = rCameraTriggerData.SetupTimestampInput();
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
	long lCount = GetTriggerCount();

	bool bOk = true;
	for (long l = 0; bOk && l < lCount; l++)
	{
		SvTi::SVTriggerObject* pTrigger = GetTrigger(l);
		bOk = (nullptr != pTrigger);
		if (bOk)
		{
			SvTh::SVTriggerClass* pTriggerDevice = pTrigger->getDevice();
			SVPPQObject* pPPQ = reinterpret_cast<SVPPQObject*>(pTrigger->GetParent());
			if(nullptr != pTriggerDevice)
			{
				pTriggerDevice->clearAcquisitionTriggers();
			}
			if (SvDef::TriggerType::CameraTrigger == pTrigger->getType())
			{
				int iDigNum = pTrigger->getDevice()->getDigitizerNumber();
				if (nullptr != pPPQ && nullptr != pTriggerDevice)
				{ 
					SetupCameraTrigger(pTriggerDevice, iDigNum, pPPQ, true, pTrigger->GetSoftwareTriggerPeriod());
				}
			}
			else if (SvDef::TriggerType::SoftwareTrigger == pTrigger->getType())
			{
				int iDigNum = pTrigger->getDevice()->getDigitizerNumber();
				if (nullptr != pPPQ && nullptr != pTriggerDevice && nullptr != pTriggerDevice->getDLLTrigger())
				{
					SetupSoftwareTrigger(pTriggerDevice, iDigNum, pTrigger->GetSoftwareTriggerPeriod(), pPPQ);
				}
			}
			// need to add acquisition trigger for File Acquisition
			if (nullptr != pPPQ && nullptr != pTrigger->getDevice() && nullptr != pTrigger->getDevice()->getDLLTrigger())
			{
				SvIe::SVVirtualCameraPtrVector cameraVector = pPPQ->GetVirtualCameras();

				for (auto* pCamera : cameraVector)
				{
					if (nullptr != pCamera)
					{
						///File acquisition require acquisition trigger
						if (pCamera->IsFileAcquisition())
						{
							pCamera->RegisterTrigger(pTrigger->getDevice());
						}
					}
				}
			}
		}
	}

	return hr;
}

HRESULT SVConfigurationObject::SetModuleReady(bool p_Value)
{
	HRESULT l_Status(S_OK);

	if (nullptr != m_pIOController)
	{
		l_Status = m_pIOController->SetModuleReady(p_Value);
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVConfigurationObject::SetRaidErrorBit(bool p_Value)
{
	HRESULT l_Status(S_OK);

	if (nullptr != m_pIOController)
	{
		if (!m_pIOController->SetRaidErrorBit(p_Value))
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

	if (nullptr != m_pIOController)
	{
		l_IOEntryPtr = m_pIOController->GetModuleReady();
	}

	return l_IOEntryPtr;
}

SVIOEntryHostStructPtr SVConfigurationObject::GetRaidErrorBit()
{
	SVIOEntryHostStructPtr l_IOEntryPtr;

	if (nullptr != m_pIOController)
	{
		l_IOEntryPtr = m_pIOController->GetRaidErrorBit();
	}

	return l_IOEntryPtr;
}

uint32_t SVConfigurationObject::GetRemoteOutputController() const
{
	uint32_t l_ObjectId = SvDef::InvalidObjectId;

	if (nullptr != m_pIOController)
	{
		l_ObjectId = m_pIOController->GetRemoteOutputController();
	}

	return l_ObjectId;
}

size_t SVConfigurationObject::GetRemoteOutputGroupCount() const
{
	size_t l_Count = 0;

	if (nullptr != m_pIOController)
	{
		l_Count = m_pIOController->GetRemoteOutputGroupCount();
	}

	return l_Count;
}

void SVConfigurationObject::SetupRemoteOutput()
{
	if (nullptr != m_pIOController)
	{
		m_pIOController->SetupRemoteOutput(this);
	}
}

HRESULT SVConfigurationObject::ClearRemoteOutputUnUsedData()
{
	HRESULT l_Status = S_OK;

	if (nullptr != m_pIOController)
	{
		l_Status = m_pIOController->ClearRemoteOutputUnUsedData();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

SVRemoteOutputGroup* SVConfigurationObject::GetRemoteOutputGroup(const std::string& rRemoteGroupID) const
{
	SVRemoteOutputGroup* l_pObject = nullptr;

	if (nullptr != m_pIOController)
	{
		l_pObject = m_pIOController->GetRemoteOutputGroup(rRemoteGroupID);
	}

	return l_pObject;
}

HRESULT SVConfigurationObject::GetRemoteOutputGroupNames(SvDef::StringVector& rPPQs) const
{
	HRESULT l_Status = S_OK;

	if (nullptr != m_pIOController)
	{
		l_Status = m_pIOController->GetRemoteOutputGroupNames(rPPQs);
	}
	else
	{
		rPPQs.clear();

		l_Status = E_FAIL;
	}

	return l_Status;
}

size_t SVConfigurationObject::GetRemoteOutputGroupItemCount(const std::string& rRemoteGroupID) const
{
	size_t l_Count = 0;

	if (nullptr != m_pIOController)
	{
		l_Count = m_pIOController->GetRemoteOutputGroupItemCount(rRemoteGroupID);
	}

	return l_Count;
}

HRESULT SVConfigurationObject::GetRemoteOutputItem(const std::string& rRemoteGroupID, long l_lIndex, SVRemoteOutputObject*& p_rItem) const
{
	HRESULT l_Status = S_OK;

	if (nullptr != m_pIOController)
	{
		l_Status = m_pIOController->GetRemoteOutputItem(rRemoteGroupID, l_lIndex, p_rItem);
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

SVRemoteOutputObject* SVConfigurationObject::GetFirstRemoteOutputObject(const std::string& rRemoteGroupID) const
{
	SVRemoteOutputObject* l_pObject = nullptr;

	if (nullptr != m_pIOController)
	{
		l_pObject = m_pIOController->GetFirstRemoteOutputObject(rRemoteGroupID);
	}

	return l_pObject;
}

HRESULT SVConfigurationObject::AddRemoteOutputItem(const std::string& rRemoteGroupID, SVRemoteOutputObject*& p_pNewOutput, uint32_t p_InputObjectID, const std::string& rPPQ)
{
	HRESULT l_Status = S_OK;

	if (nullptr != m_pIOController)
	{
		l_Status = m_pIOController->AddRemoteOutputItem(rRemoteGroupID, p_pNewOutput, p_InputObjectID, rPPQ);
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVConfigurationObject::DeleteRemoteOutput(const std::string& rRemoteGroupID)
{
	HRESULT l_Status = S_OK;

	if (nullptr != m_pIOController)
	{
		l_Status = m_pIOController->DeleteRemoteOutput(rRemoteGroupID);
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVConfigurationObject::DeleteRemoteOutputEntry(const std::string& rRemoteGroupID, SVRemoteOutputObject* p_pOutputObject)
{
	HRESULT l_Status = S_OK;

	if (nullptr != m_pIOController)
	{
		l_Status = m_pIOController->DeleteRemoteOutputEntry(rRemoteGroupID, p_pOutputObject);
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

	if (nullptr != m_pIOController)
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
	return m_pOutputObjectList->RenameInspection(OldInspectionName, NewInspectionName);
}

HRESULT SVConfigurationObject::GetInspectionItems(const SvDef::StringSet& rNames, SVNameStorageResultMap& rItems) const
{
	typedef std::map<std::string, SVInspectionProcess*> SVInspectionMap;
	typedef std::map<std::string, SVCommandInspectionGetItems::SVNameObjectSet> SVInspectionNameItemNameMap;
	typedef SVObjectAsynchronousCommandTemplate< SVCommandInspectionGetItemsPtr > SVAsyncCommand;
	typedef std::deque< SVAsyncCommand > SVAsyncCommandDeque;

	HRESULT l_Status = S_OK;

	rItems.clear();

	if (!(rNames.empty()))
	{
		SVInspectionMap l_Inspections;
		SVInspectionNameItemNameMap InspectionItems;

		for (const auto& rEntry : rNames)
		{
			SVObjectNameInfo l_Info;

			SVObjectNameInfo::ParseObjectName(l_Info, rEntry);

			if (std::string(SvDef::FqnInspections) == l_Info.m_NameArray[0])
			{
				SVObjectReference ObjectRef;
				SVObjectManagerClass::Instance().GetObjectByDottedName(l_Info.GetObjectArrayName(0), ObjectRef);

				if (nullptr != ObjectRef.getObject())
				{
					SVInspectionProcess* pInspection = nullptr;

					GetInspectionObject(l_Info.GetObjectArrayName(1).c_str(), &pInspection);

					if (nullptr != pInspection)
					{
						l_Inspections[pInspection->GetName()] = pInspection;
						SVCommandInspectionGetItems::SVFullNameObjectPair newPair(rEntry, ObjectRef);
						InspectionItems[pInspection->GetName()].insert(newPair);
					}
					else
					{
						rItems[rEntry] = SVStorageResult(SVStorage(), SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST, 0);

						if (S_OK == l_Status)
						{
							l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
						}
					}
				}
				else
				{
					rItems[rEntry] = SVStorageResult(SVStorage(), SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST, 0);

					if (S_OK == l_Status)
					{
						l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
					}
				}
			}
			else
			{
				rItems[rEntry] = SVStorageResult(SVStorage(), SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST, 0);

				if (S_OK == l_Status)
				{
					l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
				}
			}
		}

		SVAsyncCommandDeque l_AsyncCommands;

		for (SVInspectionNameItemNameMap::const_iterator l_InspectionIter = InspectionItems.begin(); l_InspectionIter != InspectionItems.end(); ++l_InspectionIter)
		{
			SVInspectionMap::iterator l_ProcessIter = l_Inspections.find(l_InspectionIter->first);

			if (l_ProcessIter != l_Inspections.end() && nullptr != l_ProcessIter->second)
			{
				SVCommandInspectionGetItemsPtr l_DataPtr {new SVCommandInspectionGetItems(*l_ProcessIter->second, l_InspectionIter->second)};
				SVObjectAsynchronousCommandTemplate<SVCommandInspectionGetItemsPtr> l_Command(l_ProcessIter->second->getObjectId(), l_DataPtr);

				HRESULT l_CommandStatus = l_Command.SubmitCommand();

				if (SUCCEEDED(l_CommandStatus))
				{
					l_AsyncCommands.push_back(l_Command);

					if (S_OK == l_Status)
					{
						l_Status = l_CommandStatus;
					}
				}
				else
				{
					l_Status = l_CommandStatus;

					rItems.clear();

					break;
				}
			}
			else
			{
				l_Status = E_UNEXPECTED;

				rItems.clear();

				break;
			}
		}

		if (SUCCEEDED(l_Status))
		{
			double l_EndTimeStamp = SvTl::GetTimeStamp() + SvTl::ConvertFrom(SvTl::Minutes, 10.0);

			while (!(l_AsyncCommands.empty()) && (SvTl::GetTimeStamp() < l_EndTimeStamp))
			{
				SVAsyncCommandDeque::iterator l_AsyncIter = l_AsyncCommands.begin();

				while (!(l_AsyncCommands.empty()) && (l_AsyncIter != l_AsyncCommands.end()))
				{
					HRESULT l_CommandStatus = l_AsyncIter->IsCommandComplete();

					if (SUCCEEDED(l_CommandStatus))
					{
						if (S_OK == l_CommandStatus)
						{
							rItems.insert(l_AsyncIter->GetCommandPtr()->GetResultItems().begin(), l_AsyncIter->GetCommandPtr()->GetResultItems().end());

							if (S_OK == l_Status)
							{
								l_Status = l_CommandStatus;
							}

							l_AsyncIter = l_AsyncCommands.erase(l_AsyncIter);
						}
						else
						{
							++l_AsyncIter;
						}
					}
					else
					{
						l_Status = l_CommandStatus;

						rItems.clear();

						l_AsyncCommands.clear();
					}
				}

				if (!(l_AsyncCommands.empty()))
				{
					::Sleep(0);
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

HRESULT SVConfigurationObject::GetRemoteInputItems(const SvDef::StringSet& rNames, SVNameStorageResultMap& rItems) const
{
	HRESULT l_Status = S_OK;

	rItems.clear();

	if (!(rNames.empty()))
	{
		for (const auto& rEntry : rNames)
		{
			SVObjectNameInfo l_Info;

			SVObjectNameInfo::ParseObjectName(l_Info, rEntry);

			if (std::string(SvDef::FqnRemoteInputs) == l_Info.m_NameArray[0])
			{
				SVRemoteInputObject* l_pInput = nullptr;

				SVObjectManagerClass::Instance().GetObjectByDottedName(l_Info.GetObjectArrayName(0), l_pInput);

				if (nullptr != l_pInput)
				{
					_variant_t l_Value;

					HRESULT l_TempStatus = l_pInput->Read(l_Value);

					if (S_OK == l_TempStatus)
					{
						rItems[rEntry] = SVStorageResult(SVStorage(SVVisionProcessor::SVStorageValue, l_Value), S_OK, 0);
					}
					else
					{
						rItems[rEntry] = SVStorageResult(SVStorage(), E_INVALIDARG, 0);

						if (S_OK == l_Status)
						{
							l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
						}
					}
				}
				else
				{
					rItems[rEntry] = SVStorageResult(SVStorage(), SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST, 0);

					if (S_OK == l_Status)
					{
						l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
					}
				}
			}
			else
			{
				rItems[rEntry] = SVStorageResult(SVStorage(), SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST, 0);

				if (S_OK == l_Status)
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

HRESULT SVConfigurationObject::SetInspectionItems(const SVNameStorageMap& p_rItems, SVNameStatusMap& p_rStatus, bool RunOnce)
{
	typedef std::map<std::string, SVInspectionProcess*> SVInspectionMap;

	HRESULT l_Status = S_OK;

	p_rStatus.clear();

	if (!(p_rItems.empty()))
	{
		SVInspectionMap l_ValueInspections;
		SVInspectionMap l_Inspections;

		bool l_Online = SVSVIMStateClass::CheckState(SV_STATE_RUNNING);

		for (SVNameStorageMap::const_iterator l_Iter = p_rItems.begin(); l_Iter != p_rItems.end(); ++l_Iter)
		{
			SVObjectNameInfo l_Info;

			SVObjectNameInfo::ParseObjectName(l_Info, l_Iter->first);

			if (l_Info.m_NameArray.size() > 0 && std::string(SvDef::FqnInspections) == l_Info.m_NameArray[0])
			{
				SVObjectReference ObjectRef;
				SVObjectManagerClass::Instance().GetObjectByDottedName(l_Info.GetObjectArrayName(0), ObjectRef);

				if (nullptr != ObjectRef.getObject())
				{
					///someone wants to set this variable check if this is allowed
					bool l_AddParameter = ((ObjectRef.ObjectAttributesAllowed() & SvPb::remotelySetable) == SvPb::remotelySetable);

					if (l_AddParameter)
					{
						l_AddParameter = !l_Online || ((ObjectRef.ObjectAttributesAllowed() & SvPb::setableOnline) == SvPb::setableOnline);

						if (!l_AddParameter)
						{
							p_rStatus[l_Iter->first] = SVMSG_OBJECT_CANNOT_BE_SET_WHILE_ONLINE;

							if (S_OK == l_Status)
							{
								l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
							}
						}
						if (l_AddParameter &&  ObjectRef.getValueObject() &&  ObjectRef.getValueObject()->isIndirectValue())
						{
						
							p_rStatus[l_Iter->first] = SVMSG_OBJECT_CANNOT_BE_SET_INDIRECT_VALUE;
							l_AddParameter = false;
							if (S_OK == l_Status)
							{
								l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
							}
						}

						bool rangeParameter = RangeClassHelper::IsOwnedByRangeObject(*ObjectRef.getObject());

						if (l_AddParameter && rangeParameter)
						{
							HRESULT hresult;
							std::string Value;
							if (l_Iter->second.m_Variant.vt == (VT_BSTR | VT_ARRAY))
							{
								long  index = 0;
								BSTR bstrValue = nullptr;
								SafeArrayGetElementNoCopy(l_Iter->second.m_Variant.parray, &index, &bstrValue);
								Value = SvUl::createStdString(bstrValue);
							}

							if (!RangeClassHelper::IsAllowedToSet(*ObjectRef.getObject(), Value, l_Online, hresult))
							{
								p_rStatus[l_Iter->first] = hresult;
								l_AddParameter = false;
								if (S_OK == l_Status)
								{
									l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
								}
							}
						}
					}
					else
					{
						p_rStatus[l_Iter->first] = SVMSG_OBJECT_CANNOT_BE_SET_REMOTELY;

						if (S_OK == l_Status)
						{
							l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
						}
					}

					if (l_AddParameter)
					{
						SVInspectionProcess* pInspection(nullptr);

						GetInspectionObject(l_Info.GetObjectArrayName(1).c_str(), &pInspection);

						if (nullptr != pInspection)
						{
							l_Inspections[pInspection->GetName()] = pInspection;

							if (l_Iter->second.m_StorageType == SVVisionProcessor::SVStorageImageFileName)
							{
								SvIe::SVImageClass* l_pImage = dynamic_cast<SvIe::SVImageClass*>(ObjectRef.getObject());

								if (nullptr != l_pImage)
								{
									p_rStatus[l_Iter->first] = pInspection->AddInputImageFileNameRequest(l_pImage, SvUl::createStdString(l_Iter->second.m_Variant));
								}
								else
								{
									p_rStatus[l_Iter->first] = E_INVALIDARG;

									if (S_OK == l_Status)
									{
										l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
									}
								}
							}
							else if (l_Iter->second.m_StorageType == SVVisionProcessor::SVStorageValue)
							{
								if (pInspection->AddInputRequest(ObjectRef, l_Iter->second.m_Variant))
								{
									l_ValueInspections[pInspection->GetName()] = pInspection;

									p_rStatus[l_Iter->first] = S_OK;
								}
								else
								{
									p_rStatus[l_Iter->first] = SVMSG_OBJECT_NOT_PROCESSED;

									if (S_OK == l_Status)
									{
										l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
									}
								}
							}
							else
							{
								p_rStatus[l_Iter->first] = E_INVALIDARG;

								if (S_OK == l_Status)
								{
									l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
								}
							}
						}
						else
						{
							p_rStatus[l_Iter->first] = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;

							if (S_OK == l_Status)
							{
								l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
							}
						}
					}
				}
				else
				{
					p_rStatus[l_Iter->first] = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;

					if (S_OK == l_Status)
					{
						l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
					}
				}
			}
			else
			{
				p_rStatus[l_Iter->first] = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;

				if (S_OK == l_Status)
				{
					l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
				}
			}
		}

		for (SVInspectionMap::iterator l_ValueInspectionIter = l_ValueInspections.begin(); l_ValueInspectionIter != l_ValueInspections.end(); ++l_ValueInspectionIter)
		{
			SVInspectionProcess* pInspection = l_ValueInspectionIter->second;

			if (nullptr != pInspection)
			{
				pInspection->AddInputRequestMarker();
			}
		}

		if (!l_Online && RunOnce)
		{
			for (SVInspectionMap::iterator l_InspectionIter = l_Inspections.begin(); l_InspectionIter != l_Inspections.end(); ++l_InspectionIter)
			{
				SVInspectionProcess* pInspection = l_InspectionIter->second;

				if (nullptr != pInspection)
				{
					SvCmd::RunOnceSynchronous(pInspection->getObjectId());
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

HRESULT SVConfigurationObject::SetRemoteInputItems(const SVNameStorageMap& p_rItems, SVNameStatusMap& p_rStatus)
{
	typedef std::map<std::string, SVInspectionProcess*> SVInspectionMap;

	HRESULT l_Status = S_OK;

	p_rStatus.clear();

	if (!(p_rItems.empty()))
	{
		bool l_Online = SVSVIMStateClass::CheckState(SV_STATE_RUNNING);

		for (SVNameStorageMap::const_iterator l_Iter = p_rItems.begin(); l_Iter != p_rItems.end(); ++l_Iter)
		{
			SVObjectNameInfo l_Info;

			SVObjectNameInfo::ParseObjectName(l_Info, l_Iter->first);

			if (std::string(SvDef::FqnRemoteInputs) == l_Info.m_NameArray[0])
			{
				SVRemoteInputObject* l_pInput = nullptr;

				SVObjectManagerClass::Instance().GetObjectByDottedName(l_Iter->first, l_pInput);

				if (nullptr != l_pInput)
				{
					HRESULT l_LoopStatus = l_pInput->writeCache(l_Iter->second.m_Variant);

					p_rStatus[l_Iter->first] = l_LoopStatus;

					if (S_OK == l_Status && S_OK != l_LoopStatus)
					{
						l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
					}
				}
				else
				{
					p_rStatus[l_Iter->first] = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;

					if (S_OK == l_Status)
					{
						l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
					}
				}
			}
			else
			{
				p_rStatus[l_Iter->first] = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;

				if (S_OK == l_Status)
				{
					l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
				}
			}
		}

		if (!l_Online)
		{
			SVInspectionMap l_ValueInspections;
			SVInspectionMap l_Inspections;

			for (SVNameStorageMap::const_iterator l_Iter = p_rItems.begin(); l_Iter != p_rItems.end(); ++l_Iter)
			{
				SVObjectNameInfo l_Info;

				SVObjectNameInfo::ParseObjectName(l_Info, l_Iter->first);

				if (l_Info.m_NameArray[0] == "RemoteInputs")
				{
					SVInspectionSet l_RemoteInspections;

					GetRemoteInputInspections(l_Iter->first, l_RemoteInspections);

					for (SVInspectionSet::iterator l_InspectionIter = l_RemoteInspections.begin(); l_InspectionIter != l_RemoteInspections.end(); ++l_InspectionIter)
					{
						SVInspectionProcess* pInspection = *l_InspectionIter;

						if (nullptr != pInspection)
						{
							l_Inspections[pInspection->GetName()] = pInspection;

							std::string l_Name;
							SVObjectReference ObjectRef;

							l_Name += pInspection->GetName();
							l_Name += ".";
							l_Name += l_Info.GetObjectName(1);

							SVObjectManagerClass::Instance().GetObjectByDottedName(l_Name, ObjectRef);

							if (nullptr != ObjectRef.getObject())
							{
								if (pInspection->AddInputRequest(ObjectRef, l_Iter->second.m_Variant))
								{
									l_ValueInspections[pInspection->GetName()] = pInspection;
								}
							}
						}
					}
				}
			}

			for (SVInspectionMap::iterator l_ValueInspectionIter = l_ValueInspections.begin(); l_ValueInspectionIter != l_ValueInspections.end(); ++l_ValueInspectionIter)
			{
				SVInspectionProcess* pInspection = l_ValueInspectionIter->second;

				if (nullptr != pInspection)
				{
					pInspection->AddInputRequestMarker();
				}
			}

			for (SVInspectionMap::iterator l_InspectionIter = l_Inspections.begin(); l_InspectionIter != l_Inspections.end(); ++l_InspectionIter)
			{
				SVInspectionProcess* pInspection = l_InspectionIter->second;

				if (nullptr != pInspection)
				{
					SvCmd::RunOnceSynchronous(pInspection->getObjectId());
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

HRESULT SVConfigurationObject::SetCameraItems(const SVNameStorageMap& rItems, SVNameStatusMap& rStatus)
{
	HRESULT Status = S_OK;

	SvIe::SVVirtualCameraPtrVector cameraChangedVector;

	rStatus.clear();

	if (!(rItems.empty()))
	{
		bool Online = SVSVIMStateClass::CheckState(SV_STATE_RUNNING);

		for (SVNameStorageMap::const_iterator Iter = rItems.begin(); Iter != rItems.end(); ++Iter)
		{
			SVObjectNameInfo Info;
			HRESULT LoopStatus = S_OK;

			SVObjectNameInfo::ParseObjectName(Info, Iter->first);

			if (std::string(SvDef::FqnCameras) == Info.m_NameArray[0])
			{
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

							SvIe::SVVirtualCamera* pVirtualCamera = dynamic_cast<SvIe::SVVirtualCamera*> (pValueObject->GetParent());
							if (nullptr != pVirtualCamera)
							{
								cameraChangedVector.emplace_back(pVirtualCamera);
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

	//Check which cameras device parameters have changed
	for (auto* pCamera : cameraChangedVector)
	{
		SVDeviceParamCollection CameraParameters;
		HRESULT LoopStatus = pCamera->updateDeviceParameters(CameraParameters);
		if (S_OK == LoopStatus)
		{
			std::string DeviceName = pCamera->GetAcquisitionDevice()->DeviceName();
			ModifyAcquisitionDevice(DeviceName.c_str(), &CameraParameters);
			SVLightReference LightRef;
			pCamera->GetAcquisitionDevice()->GetLightReference(LightRef);
			ModifyAcquisitionDevice(DeviceName.c_str(), LightRef);
		}
	}

	return Status;
}

void SVConfigurationObject::GetRemoteInputInspections(const std::string& p_rRemoteInputName, SVInspectionSet& p_rInspections) const
{
	p_rInspections.clear();

	for (SVInspectionProcessVector::const_iterator l_Iter = m_arInspectionArray.begin(); l_Iter != m_arInspectionArray.end(); ++l_Iter)
	{
		SVInspectionProcess* l_pInspection = *l_Iter;

		if (nullptr != l_pInspection)
		{
			std::string l_Name;
			SVObjectReference ObjectRef;
			SVObjectNameInfo l_Info;

			SVObjectNameInfo::ParseObjectName(l_Info, p_rRemoteInputName);

			l_Name += l_pInspection->GetName();
			l_Name += ".";
			l_Name += l_Info.GetObjectName(1);

			SVObjectManagerClass::Instance().GetObjectByDottedName(l_Name, ObjectRef);

			if (nullptr != ObjectRef.getObject())
			{
				p_rInspections.insert(l_pInspection);
			}
		}
	}
}

bool SVConfigurationObject::HasCameraTrigger(SVPPQObject* pCameraPPQ) const
{
	bool bRetVal = false;
	long lCount = GetTriggerCount();

	for (long i = 0; !bRetVal && i < lCount; i++)
	{
		SvTi::SVTriggerObject* pTrigger = GetTrigger(i);
		if (nullptr != pTrigger)
		{
			if (SvDef::TriggerType::CameraTrigger == pTrigger->getType())
			{
				SvTh::SVTriggerClass* pTriggerDevice = pTrigger->getDevice();
				if (nullptr != pTriggerDevice)
				{
					SVPPQObject* pPPQ = reinterpret_cast<SVPPQObject*>(pTrigger->GetParent());
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

void SVConfigurationObject::UpdateInspectionList4TRC()
{
	m_inspList4TRC = SvTrc::getTriggerRecordControllerRInstance().getInspections();
}

void SVConfigurationObject::updateConfTreeToNewestVersion(SVTreeType &rTree, SVTreeType::SVBranchHandle &rToolset)
{
	DWORD version = TheSVObserverApp.getLoadingVersion();
	if (version < 0x71400) //< 7.20  Add lut Equation  clip 
	{
		SVTreeType::SVBranchHandle lutEquationBranch;

		//look in all children of the toolset if it had to add information
		SVTreeType::SVBranchHandle hSubChild(nullptr);
		hSubChild = rTree.getFirstBranch(rToolset);
		while (nullptr != hSubChild)
		{
			//check for LUT Equation clip-value
			if (SvXml::SVNavigateTree::FindBranch(rTree, hSubChild, SVFindPredicate(rTree, IDS_CLASSNAME_SVLUTEQUATION), lutEquationBranch))
			{
				SVTreeType::SVBranchHandle lutEquationClipBranch;
				SVTreeType::SVBranchHandle lutEquationEmbeddedsBranch;
				if (SvXml::SVNavigateTree::FindBranch(rTree, lutEquationBranch, SVFindPredicate(rTree, scEmbeddedsTag), lutEquationEmbeddedsBranch))
				{
					//check if clip for LUT equation is existing.
					if (!SvXml::SVNavigateTree::FindBranch(rTree, lutEquationEmbeddedsBranch, SVFindPredicate(rTree, IDS_OBJECTNAME_LUT_EQUATION_CLIP), lutEquationClipBranch))
					{
						SVObjectManagerClass::Instance().setDeletedFlag(false);
						//add clip value to tree, with value FALSE
						SVConfigurationTreeWriter< SVTreeType > writer(rTree, lutEquationEmbeddedsBranch);
						SvVol::SVBoolValueObjectClass isLUTFormulaClipped;
						isLUTFormulaClipped.setResetOptions(false, SvOi::SVResetItemTool);
						std::string Name = SvUl::LoadStdString(IDS_OBJECTNAME_LUT_EQUATION_CLIP);
						isLUTFormulaClipped.SetObjectEmbedded(SvPb::LUTEquationClipFlagEId, nullptr, Name.c_str());
						isLUTFormulaClipped.SetDefaultValue(BOOL(true), true);
						isLUTFormulaClipped.SetValue(BOOL(false));
						isLUTFormulaClipped.Persist(writer);
					}
					SVObjectManagerClass::Instance().setDeletedFlag(true);
				}
			}

			hSubChild = rTree.getNextBranch(rToolset, hSubChild);
		}
	}
}

bool SVConfigurationObject::isAddParameter2MonitorListPossible(LPCTSTR ppqName, uint32_t toolId) const
{
	bool retVal = false;
	RemoteMonitorListMap monitorList = GetRemoteMonitorList();
	auto iter = find_if(monitorList.begin(), monitorList.end(), [ppqName](const auto item)->bool
	{
		return item.second.GetPPQName() == ppqName && item.second.IsActive();
	});
	if (iter != monitorList.end())
	{
		SvPb::InspectionCmdResponse response;
		retVal = getObjectsForMonitorList(toolId, response);
		retVal = retVal && response.has_getobjectsformonitorlistresponse();
		retVal = retVal && (response.getobjectsformonitorlistresponse().list_size() > 0);
		
	}
	return retVal;
}

bool SVConfigurationObject::isRemoveParameter2MonitorListPossible(LPCTSTR ppqName, uint32_t toolId) const
{
	bool retVal = false;
	RemoteMonitorListMap monitorList = GetRemoteMonitorList();
	auto iter = find_if(monitorList.begin(), monitorList.end(), [ppqName](const auto item)->bool
	{
		return item.second.GetPPQName() == ppqName && item.second.IsActive();
	});
	if (iter != monitorList.end())
	{
		SvPb::InspectionCmdResponse response;
		retVal = getObjectsForMonitorList(toolId, response);
		retVal = retVal && response.has_getobjectsformonitorlistresponse();
		retVal = retVal && (response.getobjectsformonitorlistresponse().list_size() > 0);

	}
	return retVal;
}

bool SVConfigurationObject::areParametersInMonitorList(LPCTSTR ppqName, uint32_t toolId) const
{
	bool retVal = false;
	RemoteMonitorListMap monitorList = GetRemoteMonitorList();
	auto iter = find_if(monitorList.begin(), monitorList.end(), [ppqName](const auto item)->bool
	{
		return item.second.GetPPQName() == ppqName && item.second.IsActive();
	});
	if (iter != monitorList.end())
	{
		SvPb::InspectionCmdResponse response;
		retVal = getObjectsForMonitorList(toolId, response);
		retVal  = retVal && response.has_getobjectsformonitorlistresponse();
		retVal = retVal && (response.getobjectsformonitorlistresponse().list_size() > 0);
		
		if (retVal)
		{
			MonitoredObjectList productList = iter->second.GetProductValuesList();
			for (auto const& rEntry : response.getobjectsformonitorlistresponse().list())
			{
				std::string ObjectName(SvDef::FqnInspections);
				ObjectName += _T(".") + rEntry.objectname();
				const MonitoredObject& monitoredObj = RemoteMonitorListHelper::GetMonitoredObjectFromName(ObjectName);
				if (SvDef::InvalidObjectId == monitoredObj.m_objectId)
				{
					retVal = false;
				}
				else
				{
					auto findIter = find_if(productList.begin(), productList.end(), [monitoredObj](const auto& item)->bool
					{
						return monitoredObj.isSimilar(item);
					});
					if (productList.end() == findIter)
					{
						retVal = false;
					}
				}

				if (!retVal)
				{
					break;
				}
			}

			SvStl::MessageContainerVector messages = SvPb::setMessageVectorFromMessagePB(response.getobjectsformonitorlistresponse().messages());
			retVal &= (0 == messages.size());
		}
	}
	return retVal;
}

SvStl::MessageContainerVector SVConfigurationObject::addParameter2MonitorList(LPCTSTR ppqName, uint32_t toolId)
{
	SvStl::MessageContainerVector messages;
	RemoteMonitorListMap monitorList = GetRemoteMonitorList();
	auto iter = find_if(monitorList.begin(), monitorList.end(), [ppqName](const auto item)->bool
	{
		return item.second.GetPPQName() == ppqName && item.second.IsActive();
	});
	if (iter != monitorList.end())
	{
		
		SvPb::InspectionCmdResponse response;
		bool isOk =  getObjectsForMonitorList(toolId, response);
		isOk = isOk && response.has_getobjectsformonitorlistresponse();

		if (isOk)
		{
			MonitoredObjectList productList = iter->second.GetProductValuesList();
			for (auto const& rEntry : response.getobjectsformonitorlistresponse().list())
			{
				std::string ObjectName(SvDef::FqnInspections);
				ObjectName += _T(".") + rEntry.objectname();
				const MonitoredObject& monitoredObj = RemoteMonitorListHelper::GetMonitoredObjectFromName(ObjectName);
				if (SvDef::InvalidObjectId != monitoredObj.m_objectId)
				{
					auto findIter = find_if(productList.begin(), productList.end(), [monitoredObj](const auto& item)->bool
					{
						return monitoredObj.isSimilar(item);
					});
					if (productList.end() == findIter)
					{
						productList.push_back(monitoredObj);
					}
				}
			}
			iter->second.SetProductValuesList(productList);
			SetRemoteMonitorList(monitorList);
			messages = SvPb::setMessageVectorFromMessagePB(response.getobjectsformonitorlistresponse().messages());
		}
		else
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetParameterToMonitorListFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			messages.push_back(Msg);
		}
	}
	return messages;
}

SvStl::MessageContainerVector SVConfigurationObject::removeParameter2MonitorList(LPCTSTR ppqName, uint32_t toolId)
{
	SvStl::MessageContainerVector messages;
	RemoteMonitorListMap monitorList = GetRemoteMonitorList();
	auto iter = find_if(monitorList.begin(), monitorList.end(), [ppqName](const auto item)->bool
	{
		return item.second.GetPPQName() == ppqName && item.second.IsActive();
	});
	if (iter != monitorList.end())
	{
		
		SvPb::InspectionCmdResponse response;
		bool isOk = getObjectsForMonitorList(toolId, response);
		isOk = isOk && response.has_getobjectsformonitorlistresponse();

		if (isOk)
		{
			MonitoredObjectList productList = iter->second.GetProductValuesList();
			for (auto const& rEntry : response.getobjectsformonitorlistresponse().list())
			{
				std::string ObjectName(SvDef::FqnInspections);
				ObjectName += _T(".") + rEntry.objectname();
				const MonitoredObject& monitoredObj = RemoteMonitorListHelper::GetMonitoredObjectFromName(ObjectName);
				if (SvDef::InvalidObjectId != monitoredObj.m_objectId)
				{
					auto findIter = find_if(productList.begin(), productList.end(), [monitoredObj](const auto& item)->bool
					{
						return monitoredObj.isSimilar(item);
					});
					if (productList.end() != findIter)
					{
						productList.erase(findIter);
					}
				}
			}
			iter->second.SetProductValuesList(productList);
			SetRemoteMonitorList(monitorList);
			messages = SvPb::setMessageVectorFromMessagePB(response.getobjectsformonitorlistresponse().messages());
		}
		else
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetParameterToMonitorListFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			messages.push_back(Msg);
		}
	}
	return messages;
}

void SVConfigurationObject::OnObjectRenamed(const SVObjectClass& rRenamedObject, const std::string& rOldName)
{
	SVOutputObjectList *pOutputs = GetOutputObjectList();

	if (nullptr != pOutputs)
	{
		pOutputs->OnObjectRenamed(rRenamedObject, rOldName);
	}

	long lCount = GetPPQCount();
	for (long lPPQ = 0; lPPQ < lCount; lPPQ++)
	{
		SVPPQObject* pPPQ = GetPPQ(lPPQ);

		if (nullptr != pPPQ)
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
		SendMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), SV_REGISTER_MONITOR_LIST, 0, 0);
	}
}

void SVConfigurationObject::ValidateRemoteMonitorList()
{
	if (nullptr != m_pIOController)
	{
		m_pIOController->ValidateRemoteMonitorList();
	}
}

HRESULT SVConfigurationObject::ActivateRemoteMonitorList(const std::string& listName, bool bActivate)
{
	HRESULT hr = E_POINTER;
	if (nullptr != m_pIOController)
	{
		hr = m_pIOController->ActivateRemoteMonitorList(listName, bActivate);
	}
	return hr;
}

int SVConfigurationObject::GetActiveMonitorListCount() const
{
	if (nullptr != m_pIOController)
	{
		return m_pIOController->GetActiveMonitorListCount();
	}

	return 0;
}


HRESULT SVConfigurationObject::GetRemoteMonitorListProductFilter(const std::string& listName, SvSml::SVProductFilterEnum& rFilter) const
{
	HRESULT hr = E_POINTER;
	if (nullptr != m_pIOController)
	{
		hr = m_pIOController->GetRemoteMonitorListProductFilter(listName, rFilter);
	}
	return hr;

}
HRESULT SVConfigurationObject::SetRemoteMonitorListProductFilter(const std::string& listName, SvSml::SVProductFilterEnum filter)
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

HRESULT SVConfigurationObject::LoadRemoteMonitorList(SVTreeType& rTree)
{
	HRESULT result = S_OK;
	SVTreeType::SVBranchHandle hChild(nullptr);
	if (SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_MONITOR_LISTS, nullptr, hChild))
	{
		RemoteMonitorListMap lists;
		SVTreeType::SVBranchHandle hSubChild = nullptr;
		hSubChild = rTree.getFirstBranch(hChild);
		while (S_OK == result && nullptr != hSubChild)
		{
			std::string ppqName = "";
			int rejectDepth = 0;

			std::string listName(rTree.getBranchName(hSubChild));
			_variant_t svValue = 0.0;

			if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_PPQ_NAME, hSubChild, svValue))
			{
				ppqName = static_cast<LPCTSTR>(static_cast<_bstr_t>(svValue));
			}
			else
			{
				result = SVMSG_SVO_48_LOAD_CONFIGURATION_MONITOR_LIST;
			}

			if (S_OK == result)
			{
				if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_REJECT_QUEUE_DEPTH, hSubChild, svValue))
				{
					rejectDepth = svValue;
				}
				else
				{
					result = SVMSG_SVO_48_LOAD_CONFIGURATION_MONITOR_LIST;
				}
			}
			bool isactivated(false);
			if (S_OK == result && SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_IS_ACTIVATED, hSubChild, svValue))
			{
				isactivated = svValue;
			}

			MonitoredObjectList productValueList;
			if (S_OK == result)
			{
				result = LoadMonitoredObjectList(rTree, hSubChild, SvXml::CTAG_PRODUCTVALUES_LIST, productValueList);
			}
			MonitoredObjectList productImageList;
			if (S_OK == result)
			{
				result = LoadMonitoredObjectList(rTree, hSubChild, SvXml::CTAG_PRODUCTIMAGE_LIST, productImageList);
			}
			MonitoredObjectList rejectConditionList;
			if (S_OK == result)
			{
				result = LoadMonitoredObjectList(rTree, hSubChild, SvXml::CTAG_REJECTCONDITION_LIST, rejectConditionList);
			}
			MonitoredObjectList failStatusList;
			if (S_OK == result)
			{
				result = LoadMonitoredObjectList(rTree, hSubChild, SvXml::CTAG_FAILSTATUS_LIST, failStatusList);
			}
			if (S_OK == result)
			{
				RemoteMonitorNamedList monitorList(ppqName, listName, productValueList, productImageList, rejectConditionList, failStatusList, rejectDepth);
				monitorList.Activate(isactivated);

				lists[listName] = monitorList;

				hSubChild = rTree.getNextBranch(hChild, hSubChild);
			}
		}
		if (S_OK == result)
		{
			SetRemoteMonitorList(lists);
		}
	}
	return result;
}

HRESULT SVConfigurationObject::LoadMonitoredObjectList(SVTreeType& rTree, SVTreeType::SVBranchHandle hParent, const std::string& listName, MonitoredObjectList& rList)
{
	HRESULT retValue = S_OK;
	SVTreeType::SVBranchHandle hChild(nullptr);
	// search for the branch by list name
	if (SvXml::SVNavigateTree::GetItemBranch(rTree, listName.c_str(), hParent, hChild))
	{
		SVTreeType::SVLeafHandle hLeaf(rTree.getFirstLeaf(hChild));

		while (S_OK == retValue && rTree.isValidLeaf(hChild, hLeaf))
		{
			std::string Name(rTree.getLeafName(hLeaf));
			if (0 != Name.find(SvDef::FqnInspections))
			{
				std::string InspectionsPrefix(SvDef::FqnInspections);
				InspectionsPrefix += _T(".");
				Name = InspectionsPrefix + Name;
			}

			const MonitoredObject& rObj = RemoteMonitorListHelper::GetMonitoredObjectFromName(Name);
			if (SvDef::InvalidObjectId != rObj.m_objectId)
			{
				// add object for this leaf to the list
				rList.push_back(rObj);
			}
			else
			{
				
				SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
				SvDef::StringVector msgList;
				msgList.push_back(Name);
				INT_PTR DlgResult = Exception.setMessage(SVMSG_SVO_106_MONITOR_LIST_OBJECT_MISSING, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams), 0, SvDef::InvalidObjectId, MB_YESNO);
				if (IDNO == DlgResult)
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

HRESULT SVConfigurationObject::LoadGlobalConstants(SVTreeType& rTree)
{
	HRESULT Result = S_OK;
	SVTreeType::SVBranchHandle hBranch(nullptr);
	if (SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_GLOBAL_CONSTANTS, nullptr, hBranch))
	{
		SVTreeType::SVBranchHandle hChild(nullptr);
		hChild = rTree.getFirstBranch(hBranch);

		while (S_OK == Result && nullptr != hChild)
		{
			std::string GlobalConstantName(rTree.getBranchName(hChild));

			_variant_t Value;
			uint32_t UniqueID(SvDef::InvalidObjectId);
			std::string Description;

			if (SvXml::SVNavigateTree::GetItem(rTree, scUniqueReferenceIDTag, hChild, Value))
			{
				UniqueID = calcObjectId(Value);
			}
			if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_DESCRIPTION, hChild, Value))
			{
				Description = SvUl::createStdString(Value);
				//This is needed to insert any CR LF in the description which were replaced while saving
				SvUl::RemoveEscapedSpecialCharacters(Description, true);
			}
			else
			{
				Result = SVMSG_SVO_63_LOAD_GLOBAL_CONSTANTS;
			}

			Value.Clear();
			if (!SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_VALUE, hChild, Value))
			{
				Result = SVMSG_SVO_63_LOAD_GLOBAL_CONSTANTS;
			}

			if (S_OK == Result)
			{
				SvVol::BasicValueObjectPtr pValue(nullptr);
				pValue = RootObject::setRootChildValue(GlobalConstantName.c_str(), Value);
				if (nullptr == pValue)
				{
					Result = SVMSG_SVO_63_LOAD_GLOBAL_CONSTANTS;
				}
				else
				{
					//Set the unique id for the global constant that was saved
					SVObjectManagerClass::Instance().ChangeUniqueObjectID(pValue.get(), UniqueID);
					pValue->setDescription(Description.c_str());
					//All Global constants can be remotely settable
					pValue->SetObjectAttributesAllowed(SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
					// If type string then remove Selectable for Equation flag.
					if (VT_BSTR == Value.vt)
					{
						pValue->SetObjectAttributesAllowed(SvPb::selectableForEquation, SvOi::SetAttributeType::RemoveAttribute);
					}
				}
			}

			hChild = rTree.getNextBranch(hBranch, hChild);
		}
	}
	return Result;
}

HRESULT SVConfigurationObject::LoadObjectAttributesSet(SVTreeType& rTree)
{
	HRESULT Result {S_OK};

	const std::map<std::string, UINT> AttributeSetTypes
	{
		{  std::string(_T("Publishable Value")), SvPb::publishable },
		{  std::string(_T("Publishable Image")), SvPb::publishResultImage },
		{ std::string(_T("Data Definition Value")), SvPb::dataDefinitionValue },
		{ std::string(_T("Data Definition Image")), SvPb::dataDefinitionImage }
	};

	SVTreeType::SVBranchHandle hBranch(nullptr);
	if (SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_OBJECT_ATTRIBUTES_SET, nullptr, hBranch))
	{
		SVTreeType::SVBranchHandle hChild(nullptr);
		hChild = rTree.getFirstBranch(hBranch);

		while (S_OK == Result && nullptr != hChild)
		{
			auto const Iter = AttributeSetTypes.find(rTree.getBranchName(hChild));
			if (AttributeSetTypes.end() != Iter)
			{
				UINT Attribute = Iter->second;

				SVTreeType::SVLeafHandle hLeaf(rTree.getFirstLeaf(hChild));

				while (S_OK == Result && rTree.isValidLeaf(hChild, hLeaf))
				{
					if (scUniqueReferenceIDTag == rTree.getLeafName(hLeaf))
					{
						_variant_t Value;
						if (S_OK == rTree.getLeafData(hLeaf, Value))
						{
							std::string ValueText {SvUl::createStdString(Value.bstrVal)};
							std::string objectIdString;
							auto Pos = ValueText.find('}');
							if (std::string::npos != Pos)
							{
								Pos++;
								objectIdString = SvUl::Left(ValueText, Pos);
								ValueText = SvUl::Mid(ValueText, Pos);
							}

							uint32_t uniqueID = calcObjectId(objectIdString);
							SVObjectReference ObjectRef {SVObjectManagerClass::Instance().GetObject(uniqueID)};
							if (nullptr != ObjectRef.getObject())
							{
								if (ValueText.empty())
								{
									ObjectRef.SetObjectAttributesSet(Attribute, SvOi::AddAttribute);
								}
								else
								{
									std::vector<int> AttributeIndexes;
									auto StartPos = 0LL;
									auto EndPos = ValueText.find(',');
									while (std::string::npos != EndPos)
									{
										std::string Range = ValueText.substr(StartPos, EndPos - StartPos);
										auto RangePos = Range.find('-');
										if (std::string::npos != RangePos)
										{
											int IndexStart = atoi(SvUl::Left(Range, RangePos).c_str());
											int IndexEnd = atoi(SvUl::Mid(Range, RangePos + 1).c_str());
											for (int i = IndexStart; i <= IndexEnd; i++)
											{
												AttributeIndexes.push_back(i);
											}
										}
										else
										{
											AttributeIndexes.push_back(atoi(Range.c_str()));
										}
										StartPos = EndPos + 1;
										EndPos = ValueText.find(',', StartPos);
									}
									for (auto const& rEntry : AttributeIndexes)
									{
										ObjectRef.SetArrayIndex(rEntry);
										ObjectRef.SetObjectAttributesSet(Attribute, SvOi::AddAttribute);

									}
								}
							}
						}
					}
					hLeaf = rTree.getNextLeaf(hChild, hLeaf);
				}
			}

			hChild = rTree.getNextBranch(hBranch, hChild);
		}
	}
	return Result;
}

HRESULT SVConfigurationObject::LoadAdditionalFiles(SVTreeType& rTree)
{
	HRESULT Result = S_OK;
	SVTreeType::SVBranchHandle hBranch(nullptr);
	if (SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_ADDITIONAL_CONFIG_FILES, nullptr, hBranch))
	{
		SVTreeType::SVLeafHandle hLeaf(rTree.getFirstLeaf(hBranch));

		while (S_OK == Result && rTree.isValidLeaf(hBranch, hLeaf))
		{
			if (SvXml::CTAG_FILENAME == rTree.getLeafName(hLeaf))
			{
				_variant_t Value;
				if (S_OK == rTree.getLeafData(hLeaf, Value) && VT_BSTR == Value.vt)
				{
					std::string FilePath{SvUl::createStdString(Value.bstrVal)};
					m_AdditionalFiles.emplace_back(SVFileNameClass{FilePath.c_str()});
					SVFileNameManagerClass::Instance().AddItem(&m_AdditionalFiles.back());
				}
			}
			hLeaf = rTree.getNextLeaf(hBranch, hLeaf);
		}
	}

	return Result;
}

/*static*/ bool SVConfigurationObject::GetInspection(LPCTSTR InspectionName, SVInspectionProcess*& prInspection)
{
	bool Result(false);

	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig)
	{
		if (pConfig->GetInspectionObject(InspectionName, &prInspection))
		{
			Result = true;
		}
	}

	return Result;
}

bool SVConfigurationObject::getObjectsForMonitorList(uint32_t toolId, SvPb::InspectionCmdResponse& rResponse) const
{
	SVObjectClass* pObject = nullptr;
	SVObjectManagerClass::Instance().GetObjectByIdentifier(toolId, pObject);
	if (nullptr == pObject)
	{
		return false;
	}
	auto* pInspection = pObject->GetAncestorInterface(SvPb::SVObjectTypeEnum::SVInspectionObjectType);
	if (nullptr == pInspection)
	{
		return false;
	}

	SvPb::InspectionCmdRequest requestCmd;
	auto* pRequest = requestCmd.mutable_getobjectsformonitorlistrequest();
	pRequest->set_objectid(toolId);

	return S_OK == SvCmd::InspectionCommands(pInspection->getObjectId(), requestCmd, &rResponse);
}

void SVConfigurationObject::initializeIO(SVIMProductEnum newConfigType)
{

	bool isDiscrete = SvTi::SVHardwareManifest::isDiscreteIOSystem(newConfigType);
	bool isPlc = SvTi::SVHardwareManifest::isPlcSystem(newConfigType);
	int inputCount = isPlc ? cPlcInputCount : isDiscrete ? cDiscreteInputCount : 0;
	int outputCount = isPlc ? cPlcOutputCount : isDiscrete ? cDiscreteOutputCount : 0;
	SVIOConfigurationInterfaceClass::Instance().initializeIO(inputCount, outputCount);
	if (isDiscrete)
	{
		SVConfigurationObject* pConfig(nullptr);
		SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

		if (nullptr != pConfig)
		{
			// Create all the default inputs
			SVInputObjectList* pInputObjectList = pConfig->GetInputObjectList();
			if (nullptr != pInputObjectList)
			{
				for (unsigned long l = 0; l < cDiscreteInputCount; l++)
				{
					std::string Name = SvUl::Format(_T("DIO.Input%d"), l + 1);

					SVDigitalInputObject* pInput = dynamic_cast<SVDigitalInputObject*> (pInputObjectList->GetInputFlyweight(Name, SvPb::SVDigitalInputObjectType, l).get());

					if (nullptr != pInput)
					{
						pInput->SetChannel(l);
					}
				}
			}
			// Create all the default outputs
			SVOutputObjectList *pOutputObjectList = pConfig->GetOutputObjectList();
			if (nullptr != pOutputObjectList)
			{
				SVDigitalOutputObject* pOutput = dynamic_cast<SVDigitalOutputObject*> (pOutputObjectList->GetOutputFlyweight(SvDef::cModuleReady, SvPb::SVDigitalOutputObjectType, cModuleReadyChannel).get());

				if (nullptr != pOutput && nullptr != pConfig->GetModuleReady())
				{
					pOutput->SetChannel(cModuleReadyChannel);

					pConfig->GetModuleReady()->m_IOId = pOutput->getObjectId();

					SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputIsInverted(cModuleReadyChannel, pOutput->IsInverted());
					SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputIsForced(cModuleReadyChannel, pOutput->IsForced());
					SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputForcedValue(cModuleReadyChannel, pOutput->GetForcedValue());
				}
			}
		}
	}
	///Need to change the output type
	for(auto& pPPQ : m_arPPQArray)
	{
		SVIOEntryHostStructPtrVector IOEntriesVector;
		pPPQ->GetAllOutputs(IOEntriesVector);

		for(auto& pIoEntry : IOEntriesVector)
		{
			pIoEntry->m_ObjectType = SvTi::SVHardwareManifest::isPlcSystem(newConfigType) ? IO_PLC_OUTPUT : IO_DIGITAL_OUTPUT;
		}
	}
}

void SVConfigurationObject::changeSystemResetIO(SVIMProductEnum newConfigType)
{
	if (nullptr != m_pInputObjectList)
	{
		SVIOEntryHostStructPtrVector inputVector = m_pInputObjectList->getInputList();
		for (const auto& pInput : inputVector)
		{
			switch (pInput->m_ObjectType)
			{
				case IO_DIGITAL_INPUT:
				{
					for (auto& pPPQ : m_arPPQArray)
					{
						pPPQ->RemoveInput(pInput);
					}
					m_pInputObjectList->DetachInput(pInput->m_IOId);
					pInput->m_Enabled = false;
					pInput->m_PPQIndex = -1;
					break;
				}
				default:
					break;
			}
		}
	}

	if (nullptr != m_pOutputObjectList)
	{
		SVIOEntryHostStructPtrVector outputVector = m_pOutputObjectList->getOutputList();
		for (const auto& pOutput : outputVector)
		{
			switch (pOutput->m_ObjectType)
			{
				case IO_DIGITAL_OUTPUT:
				case IO_PLC_OUTPUT:
				{
					m_pOutputObjectList->DetachOutput(pOutput->m_IOId);
					pOutput->m_Enabled = false;
					break;
				}
				default:
					break;
			}
		}
	}

	initializeIO(newConfigType);

	if (SvTi::SVHardwareManifest::isPlcSystem(newConfigType))
	{
		TheSVObserverApp.ShowIOTab(SVIOPlcOutputsViewID);
	}
	else
	{
		TheSVObserverApp.HideIOTab(SVIOPlcOutputsViewID);
	}
	if (SvTi::SVHardwareManifest::isDiscreteIOSystem(newConfigType))
	{
		TheSVObserverApp.ShowIOTab(SVIODiscreteInputsViewID);
		TheSVObserverApp.ShowIOTab(SVIODiscreteOutputsViewID);
	}
	else
	{
		TheSVObserverApp.HideIOTab(SVIODiscreteInputsViewID);
		TheSVObserverApp.HideIOTab(SVIODiscreteOutputsViewID);
	}
}


