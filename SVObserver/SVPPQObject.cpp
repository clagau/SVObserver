//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPPQObject
//* .File Name       : $Workfile:   SVPPQObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.36  $
//* .Check In Date   : $Date:   19 Jan 2015 11:42:18  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <mmsystem.h>
//Moved to precompiled header: #include <fstream>
//Moved to precompiled header: #include <algorithm>
//Moved to precompiled header: #include <limits>
//Moved to precompiled header: #include <boost/bind.hpp>

#include "SVPPQObject.h"
#include "SVDataManagerLibrary/DataManager.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "Definitions/GlobalConst.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVUtilityLibrary/SVDottedName.h"
#include "SVSharedMemoryLibrary/SVSharedConfiguration.h"
#include "SVSharedMemoryLibrary/SharedMemWriter.h"
#include "SVGlobal.h"
#include "SVObserver.h"
#include "SVOutputObjectList.h"
#include "SVInputObjectList.h"
#include "SVInfoStructs.h"
#include "SVMessage/SVMessage.h"
#include "SVDigitalOutputObject.h"
#include "TriggerInformation/SVTriggerObject.h"
#include "SVAcquisitionClass.h"
#include "TriggerHandling/SVTriggerClass.h"
#include "SVConfigurationObject.h"
#include "SVRemoteInputObject.h"
#include "SVPPQConstants.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary\MessageManager.h"
#include "TextDefinesSvO.h"
#include "Definitions/Color.h"
#ifdef _DEBUG_PERFORMANCE_INFO //Arvid 160212 this is helpful for debugging the creation of Performance Information
#include "SVTimerLibrary\SVProfiler.h"
#endif
#include "SVStatusLibrary\MessageManager.h"
#include "SVVisionProcessorHelper.h"
#include "SVToolSet.h"
#include "SVSharedMemoryLibrary\ShareEvents.h"
#include "SVSharedMemoryLibrary\MLPPQInfo.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const long g_lPPQExtraBufferSize = 50;
static const double TwentyPercent = .20;
static const long cPpqDefaultIndex = 1L;

const long MinReducedPPQPosition = 2;
#pragma endregion Declarations

HRESULT CALLBACK SVFinishTriggerCallback(void *pOwner, void *pCaller, void *pTriggerInfo)
{
	SVPPQObject* pPPQ = reinterpret_cast<SVPPQObject*>(pOwner);
	SvTi::SVTriggerInfoStruct* pInfo = reinterpret_cast<SvTi::SVTriggerInfoStruct*>(pTriggerInfo);

	bool bRet = (nullptr != pPPQ && nullptr != pInfo);

	if (bRet)
	{
		bRet = pPPQ->FinishTrigger(pCaller, *pInfo);
	}

	if (bRet)
	{
		return S_OK;
	}
	else
	{
		return S_FALSE;
	}
}

HRESULT CALLBACK SVFinishCameraCallback(void *pOwner, void *pCaller, void *pResponse)
{
	bool bRet(false);

	SVPPQObject* pPPQ = reinterpret_cast<SVPPQObject*> (pOwner);

	if (nullptr != pPPQ) { bRet = pPPQ->FinishCamera(pCaller, reinterpret_cast<SVODataResponseClass*>(pResponse)); }

	if (bRet)
	{
		return S_OK;
	}
	else
	{
		return S_FALSE;
	}
}

#ifdef _DEBUG_PERFORMANCE_INFO //Arvid 160212 this is helpful for debugging the creation of Performance Information

void logWorkloadInformation(const ProductWorkloadInformation &pwi, LPCSTR heading)
{
	std::string infostring;
	infostring = SvUl::Format(_T("!\t.%7.1lf: %s:\n"), SvTl::GetRelTimeStamp(), heading);
	::OutputDebugString(infostring.c_str());
	infostring = SvUl::(_T("!\t\ttt = %7.1lf, pst = %7.1lf, ct = %7.1lf, "),
		pwi.m_TriggerTime - SvTl::getReferenceTime(),
		pwi.m_ProcessingStartTime - SvTl::getReferenceTime(),
		pwi.m_CompletionTime - SvTl::getReferenceTime());
	::OutputDebugString(infostring.c_str());
	infostring = SvUl::(_T("TtoCo = %7.1lf, TtoSt = %7.1lf\n"),
		pwi.TriggerToCompletionInMilliseconds(),

		pwi.TriggerToStartInMilliseconds());
	::OutputDebugString(infostring.c_str());
	infostring = SvUl::(_T("!\trd\ttt = %.0lf, pst = %.0lf, ct = %.0lf, "),
		pwi.m_TriggerTime,
		pwi.m_ProcessingStartTime,
		pwi.m_CompletionTime);
	::OutputDebugString(infostring.c_str());
	infostring = SvUl::(_T("pst-tt = %7.1lf, ct-tt = %7.1lf, ct-pst= %7.1lf\n"),
		pwi.m_ProcessingStartTime - pwi.m_TriggerTime,
		pwi.m_CompletionTime - pwi.m_TriggerTime,
		pwi.m_CompletionTime - pwi.m_ProcessingStartTime);
	::OutputDebugString(infostring.c_str());
}

#endif


HRESULT SVPPQObject::ProcessDelayOutputs( bool& rProcessed )
/// Used in data completion mode, but not in next trigger mode.
/// Retrieves the process (i.e. trigger) count from the m_oOutputsDelayQueue and uses it to get a "product pointer"
/// from m_ppPPQPositions via GetProductInfoStruct()
/// If the delay time is already over, calls ProcessTimeDelay/AndDataComplete/Outputs().
/// from where WriteOutputs() will be called.
/// Otherwise, puts the trigger count back into the outputs delay queue, 
{
	HRESULT l_Status = S_OK;

	SVProductInfoStruct* l_pProduct = nullptr;

	SvTl::SVTimeStamp l_CurrentTime = SvTl::GetTimeStamp();

	while (0 < m_oOutputsDelayQueue.GetCount() && nullptr == l_pProduct)
	{
		long l_ProcessCount = 0;

		if (m_oOutputsDelayQueue.RemoveHead(&l_ProcessCount))
		{
			GetProductInfoStruct(l_pProduct, l_ProcessCount);
		}
		else
		{
			l_Status = E_FAIL;

			break;
		}

		if (nullptr != l_pProduct)
		{
			if (0 < m_oOutputsDelayQueue.GetCount())
			{
				SVProductInfoStruct* l_pNextProduct = nullptr;

				if (m_oOutputsDelayQueue.GetHead(&l_ProcessCount))
				{
					GetProductInfoStruct(l_pNextProduct, l_ProcessCount);
				}

				if (nullptr != l_pNextProduct && l_pNextProduct->oOutputsInfo.m_EndOutputDelay <= l_CurrentTime)
				{
					l_pProduct = nullptr;
				}
			}
		}

		if (nullptr != l_pProduct)
		{
			// if time delay has not expired yet, then put back on the 
			// m_oOutputsDelayQueue.  Then why did we remove it to begin with?
			if (l_CurrentTime < l_pProduct->oOutputsInfo.m_EndOutputDelay)
			{
				m_NextOutputDelayTimestamp = l_pProduct->oOutputsInfo.m_EndOutputDelay;

				m_oOutputsDelayQueue.AddHead(l_pProduct->ProcessCount());

				l_pProduct = nullptr;

				break;
			}
		}
	}

	rProcessed = ( nullptr != l_pProduct );

	if( rProcessed )
	{
		SvDef::SVPPQOutputModeEnum oOutputMode = SvDef::SVPPQUnknownMode;

		GetPPQOutputMode(oOutputMode);
		switch (oOutputMode)
		{
		case SvDef::SVPPQTimeDelayMode:
		case SvDef::SVPPQExtendedTimeDelayMode:
		{
			l_Status = ProcessTimeDelayOutputs(*l_pProduct);

			break;
		}
		case SvDef::SVPPQTimeDelayAndDataCompleteMode:
		case SvDef::SVPPQExtendedTimeDelayAndDataCompleteMode:
		{
			l_Status = ProcessTimeDelayAndDataCompleteOutputs(*l_pProduct, rProcessed);

			break;
		}
		default:
		{
			l_Status = E_FAIL;

			break;
		}
		}
	}

	::InterlockedExchange(&(m_ProcessingOutputDelay), 0);

	return l_Status;
}

HRESULT SVPPQObject::ProcessTimeDelayOutputs(SVProductInfoStruct& p_rProduct)
{
	HRESULT l_Status = S_OK;

	if (WriteOutputs(&p_rProduct))
	{
		long lResetDelay = 0;

		GetResetDelay(lResetDelay);
		
		if (lResetDelay)
		{
			p_rProduct.oOutputsInfo.m_EndProcess = SvTl::GetTimeStamp();

			p_rProduct.oOutputsInfo.m_EndResetDelay = p_rProduct.oOutputsInfo.m_EndProcess +
				p_rProduct.oOutputsInfo.lResetDelay;

			m_oOutputsResetQueue.AddTail(p_rProduct.ProcessCount());

			SetProductComplete(p_rProduct);
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVPPQObject::ProcessTimeDelayAndDataCompleteOutputs( SVProductInfoStruct& p_rProduct, bool& rProcessed  )
{
	HRESULT l_Status = S_OK;

	if (p_rProduct.bDataComplete)
	{
		l_Status = ProcessTimeDelayOutputs(p_rProduct);
	}
	else
	{
		m_NextOutputDelayTimestamp = p_rProduct.oOutputsInfo.m_EndOutputDelay;

		m_oOutputsDelayQueue.AddHead(p_rProduct.ProcessCount());

		rProcessed = false;
	}

	return l_Status;
}

#pragma region Constructor
SVPPQObject::SVPPQObject(LPCSTR ObjectName)
	: SVObjectClass(ObjectName)
	, m_oTriggerQueue(10)
	, m_NAKParameter(SvDef::DefaultNakParameter)
	, m_NAKCount(0)
	, m_NAKMode(SvDef::Bursts)
	, m_FirstNAKProcessCount(0)
	, m_NewNAKCount(0)
	, m_ReducedPPQPosition(MinReducedPPQPosition)
	, m_conditionalOutputName(PPQ_CONDITIONAL_OUTPUT_ALWAYS)
	, m_bActiveMonitorList(false)
	, m_AttributesAllowedFilterForFillChildObjectList(0)
{
	init();
}

SVPPQObject::SVPPQObject(SVObjectClass* POwner, int StringResourceID)
	: SVObjectClass(POwner, StringResourceID)
	, m_oTriggerQueue(10)
	, m_NAKParameter(SvDef::DefaultNakParameter)
	, m_NAKCount(0)
	, m_NAKMode(SvDef::Bursts)
	, m_FirstNAKProcessCount(0)
	, m_NewNAKCount(0)
	, m_ReducedPPQPosition(MinReducedPPQPosition)
	, m_conditionalOutputName(PPQ_CONDITIONAL_OUTPUT_ALWAYS)
	, m_bActiveMonitorList(false)
	, m_AttributesAllowedFilterForFillChildObjectList(0)
{
	init();
}
#pragma endregion Constructor

SVPPQObject::~SVPPQObject()
{
	m_childObjectsForFillChildObjectList.clear();

	SVObjectManagerClass::Instance().ClearShortPPQIndicator();

	// Stop the multimedia timer thread for the output and reset time delays
	if (0 != m_uOutputTimer)
	{
		::timeKillEvent(m_uOutputTimer);

		::timeEndPeriod(1);

		m_uOutputTimer = 0;
	}

	if (m_isCreated)
	{
		Destroy();
	}

	m_pTriggerToggle.clear();
	m_pOutputToggle.clear();
	m_pDataValid.clear();
	m_SlotManager.reset();
}

void SVPPQObject::init()
{
	m_oOutputMode = SvDef::SVPPQNextTriggerMode;
	m_isCreated = false;
	m_bOnline = false;
	m_bMaintainSourceImages = false;
	m_lInspectionTimeoutMillisec = 0;
	m_lOutputDelay = 100;
	m_lResetDelay = 0;
	m_DataValidDelay = TheSVObserverApp.getDataValidDelay();

	m_ProcessingOutputDelay = 0;
	m_NextOutputDelayTimestamp = 0;
	m_ProcessingOutputReset = 0;
	m_NextOutputResetTimestamp = 0;

	m_uOutputTimer = 0;

	m_pTrigger = nullptr;
	m_pMasterProductInfos = nullptr;
	m_pInputList = nullptr;
	m_pOutputList = nullptr;

	m_voTriggerToggle.SetObjectAttributesAllowed(SvDef::SV_EMBEDABLE | SvDef::SV_PRINTABLE, SvOi::SetAttributeType::OverwriteAttribute);
	m_voOutputToggle.SetObjectAttributesAllowed(SvDef::SV_EMBEDABLE | SvDef::SV_PRINTABLE, SvOi::SetAttributeType::OverwriteAttribute);
	m_voACK.SetObjectAttributesAllowed(SvDef::SV_EMBEDABLE | SvDef::SV_PRINTABLE, SvOi::SetAttributeType::OverwriteAttribute);
	m_voNAK.SetObjectAttributesAllowed(SvDef::SV_EMBEDABLE | SvDef::SV_PRINTABLE, SvOi::SetAttributeType::OverwriteAttribute);
	m_voMasterFault.SetObjectAttributesAllowed(SvDef::SV_EMBEDABLE | SvDef::SV_PRINTABLE, SvOi::SetAttributeType::OverwriteAttribute);
	m_voMasterWarning.SetObjectAttributesAllowed(SvDef::SV_EMBEDABLE | SvDef::SV_PRINTABLE, SvOi::SetAttributeType::OverwriteAttribute);
	m_voNotInspected.SetObjectAttributesAllowed(SvDef::SV_EMBEDABLE | SvDef::SV_PRINTABLE, SvOi::SetAttributeType::OverwriteAttribute);
	m_voDataValid.SetObjectAttributesAllowed(SvDef::SV_EMBEDABLE | SvDef::SV_PRINTABLE, SvOi::SetAttributeType::OverwriteAttribute);
	m_voOutputState.SetObjectAttributesAllowed(SvDef::SV_EMBEDABLE | SvDef::SV_PRINTABLE, SvOi::SetAttributeType::OverwriteAttribute);

	BasicValueObjectPtr pPpqLength = m_PpqValues.setValueObject(SvDef::FqnPpqLength, StandardPpqLength, this);
	SVObjectManagerClass::Instance().IncrementShortPPQIndicator();

	//fill up the child object list
	m_childObjects.push_back(&m_voTriggerToggle);
	m_childObjects.push_back(&m_voOutputToggle);
	m_childObjects.push_back(&m_voACK);
	m_childObjects.push_back(&m_voNAK);
	m_childObjects.push_back(&m_voMasterFault);
	m_childObjects.push_back(&m_voMasterWarning);
	m_childObjects.push_back(&m_voNotInspected);
	m_childObjects.push_back(&m_voDataValid);
	m_childObjects.push_back(&m_voOutputState);
	m_childObjects.push_back(&m_voTriggerCount);
	m_childObjects.push_back(pPpqLength.get());

	//copy full child object list to the temporary list for the method fillChildObjectList,
	//because this is the same as filter = 0 (default)
	m_childObjectsForFillChildObjectList = m_childObjects;
}

HRESULT SVPPQObject::GetChildObject(SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index) const
{
	HRESULT l_Status = SVObjectClass::GetChildObject(rpObject, rNameInfo, Index);

	if (S_OK != l_Status)
	{
		if (static_cast<const size_t> (Index) < rNameInfo.m_NameArray.size() && rNameInfo.m_NameArray[Index] == GetName())
		{
			if (static_cast<const size_t> (Index + 1) == (rNameInfo.m_NameArray.size() - 1))
			{
				SVObjectPtrDeque::const_iterator iter = m_childObjects.cbegin();
				while (iter != m_childObjects.cend() && nullptr == rpObject)
				{
					const SVObjectClass* pObject = *iter;
					l_Status = pObject->GetChildObject(rpObject, rNameInfo, Index + 1);
					++iter;
				}
			}
		}
	}

	return l_Status;
}

HRESULT SVPPQObject::ObserverUpdate(const SVInspectionCompleteInfoStruct& p_rData)
{
	HRESULT l_Status = S_OK;

	SVGUIDSVInspectionInfoStructMap::const_iterator l_Iter = p_rData.m_ProductInfo.m_svInspectionInfos.find(p_rData.m_InspectionID);

	if (l_Iter != p_rData.m_ProductInfo.m_svInspectionInfos.end())
	{
		SVInspectionInfoStruct l_InspectInfo = l_Iter->second;

		l_InspectInfo.m_CallbackReceived = SvTl::GetTimeStamp();

		l_InspectInfo.ClearIndexes();

		SVInspectionInfoPair l_Info(p_rData.m_ProductInfo.ProcessCount(), l_InspectInfo);

		m_oInspectionQueue.AddTail(l_Info);

		m_AsyncProcedure.Signal(nullptr);
	}

	return l_Status;
}

bool SVPPQObject::Create()
{
	_bstr_t bName;
	HRESULT hr;
	int i;
	
	// Return if already created
	if (m_isCreated) { return false; }

	bName = _T("PPQ Result Data");

	hr = TheSVDataManager.CreateManagedIndexArray(m_pResultDataCircleBuffer, bName, GetPPQLength() + g_lPPQExtraBufferSize);
	if (S_OK != hr) { return false; }

	bName = _T("PPQ Result Image Published");

	hr = TheSVDataManager.CreateManagedIndexArray(m_pResultImagePublishedCircleBuffer, bName, TheSVObserverApp.GetSourceImageDepth());
	if (S_OK != hr) { return false; }

	for (i = 0; i < static_cast<long>(m_ppPPQPositions.size()); ++i)
	{
		RecycleProductInfo(m_ppPPQPositions.GetProductAt(i));

		m_ppPPQPositions.SetProductAt(i, nullptr);
	}

	// Create buckets for the PPQ positions
	m_ppPPQPositions.resize(GetPPQLength());

	// Create a set of ProductInfoStructs to use
	if (!m_qAvailableProductInfos.Create()) { return false; }

	SVGuidSVCameraInfoStructMap l_CameraInfos;

	BuildCameraInfos(l_CameraInfos);

	// Set up all the ProductInfo Structs
	m_pMasterProductInfos = new SVProductInfoStruct[GetPPQLength() + g_lPPQExtraBufferSize];
	for (int j = 0; j < GetPPQLength() + g_lPPQExtraBufferSize; j++)
	{
		m_pMasterProductInfos[j].oPPQInfo.pPPQ = this;
		m_pMasterProductInfos[j].oTriggerInfo.pTrigger = m_pTrigger;
		m_pMasterProductInfos[j].m_svCameraInfos = l_CameraInfos;
		m_pMasterProductInfos[j].m_svInspectionInfos.clear();

		for (auto pInspection : m_arInspections)
		{
			SVInspectionInfoStruct l_svInspectionStruct;

			l_svInspectionStruct.pInspection = pInspection;

			m_pMasterProductInfos[j].m_svInspectionInfos[pInspection->GetUniqueObjectID()] = l_svInspectionStruct;
		}// end for

		m_qAvailableProductInfos.AddTail(&m_pMasterProductInfos[j]);
	}// end for

	SVProductInfoStruct* l_pProduct = nullptr;

	if (m_qAvailableProductInfos.RemoveHead(&l_pProduct) && nullptr != l_pProduct)
	{
		if (S_OK == m_ppPPQPositions.SetProductAt(0, l_pProduct))
		{
			l_pProduct->InitProductInfo();
			l_pProduct->GetNextAvailableIndexes(SV_PPQ);

			for (auto pInspection : m_arInspections)
			{
				pInspection->LastProductUpdate(l_pProduct);
			}
		}
		else
		{
			m_qAvailableProductInfos.AddTail(l_pProduct);
		}
	}

	m_isCreated = true;

	// Create the Queues that run the threads
	m_isCreated &= m_oCamerasQueue.Create();
	m_isCreated &= m_CameraResponseQueue.Create();
	m_isCreated &= m_oInspectionQueue.Create();
	m_isCreated &= m_oOutputsDelayQueue.Create();
	m_isCreated &= m_oOutputsResetQueue.Create();
	m_isCreated &= m_DataValidDelayQueue.Create();
	m_isCreated &= m_ProductRequests.Create();

	m_oNotifyInspectionsSet.clear();
	m_ProcessInspectionsSet.clear();

	if (m_isCreated)
	{
		// Force the Inspections to rebuild
		for (auto pInspection : m_arInspections)
		{
			pInspection->RebuildInspection();
		}// end for
	}

	m_voTriggerToggle.SetObjectAttributesAllowed(SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute);
	m_voOutputToggle.SetObjectAttributesAllowed(SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute);
	m_voACK.SetObjectAttributesAllowed(SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute);
	m_voNAK.SetObjectAttributesAllowed(SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute);
	m_voMasterFault.SetObjectAttributesAllowed(SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute);
	m_voMasterWarning.SetObjectAttributesAllowed(SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute);
	m_voNotInspected.SetObjectAttributesAllowed(SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute);
	m_voDataValid.SetObjectAttributesAllowed(SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute);
	m_voOutputState.SetObjectAttributesAllowed(SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute);

	m_TriggerToggle = false;
	m_OutputToggle = false;

	return m_isCreated;
}

bool SVPPQObject::Rebuild()
{
	int j;
	_bstr_t bName;
	HRESULT hr;

	// Return if not created
	if (!m_isCreated) { return false; }

	// Delete buckets for the PPQ positions
	m_ppPPQPositions.clear();

	m_qAvailableProductInfos.RemoveAll();

	delete[] m_pMasterProductInfos;

	m_pMasterProductInfos = nullptr;

	// Create new data manager buffers of new size
	bName = _T("PPQ Result Data");

	hr = TheSVDataManager.CreateManagedIndexArray(m_pResultDataCircleBuffer, bName, GetPPQLength() + g_lPPQExtraBufferSize);
	if (S_OK != hr) { return false; }

	bName = _T("PPQ Result Image Published");

	hr = TheSVDataManager.CreateManagedIndexArray(m_pResultImagePublishedCircleBuffer, bName, TheSVObserverApp.GetSourceImageDepth());
	if (S_OK != hr) { return false; }


	SVGuidSVCameraInfoStructMap l_CameraInfos;

	BuildCameraInfos(l_CameraInfos);

	// Create buckets for the PPQ positions
	m_ppPPQPositions.resize(GetPPQLength());

	// Set up all the ProductInfo Structs
	m_pMasterProductInfos = new SVProductInfoStruct[GetPPQLength() + g_lPPQExtraBufferSize];
	for (j = 0; j < GetPPQLength() + g_lPPQExtraBufferSize; j++)
	{
		m_pMasterProductInfos[j].oPPQInfo.pPPQ = this;
		m_pMasterProductInfos[j].oTriggerInfo.pTrigger = m_pTrigger;
		m_pMasterProductInfos[j].m_svCameraInfos = l_CameraInfos;
		m_pMasterProductInfos[j].m_svInspectionInfos.clear();

		for (auto pInspection : m_arInspections)
		{
			SVInspectionInfoStruct l_svInspectionStruct;
			l_svInspectionStruct.pInspection = pInspection;

			m_pMasterProductInfos[j].m_svInspectionInfos[pInspection->GetUniqueObjectID()] = l_svInspectionStruct;
		}// end for

		m_qAvailableProductInfos.AddTail(&m_pMasterProductInfos[j]);
	}// end for

	SVProductInfoStruct* l_pProduct = nullptr;

	if (m_qAvailableProductInfos.RemoveHead(&l_pProduct) && nullptr != l_pProduct)
	{
		if (S_OK == m_ppPPQPositions.SetProductAt(0, l_pProduct))
		{
			l_pProduct->InitProductInfo();
			l_pProduct->GetNextAvailableIndexes(SV_PPQ);

			for (auto pInspection : m_arInspections)
			{
				pInspection->LastProductUpdate(l_pProduct);
			}
		}
		else
		{
			m_qAvailableProductInfos.AddTail(l_pProduct);
		}
	}

	// Force the Inspections to rebuild as well
	for (auto pInspection : m_arInspections)
	{
		pInspection->RebuildInspection();
	}// end for

	return true;
}// end Rebuild

void SVPPQObject::Destroy()
{
	// Return if not created
	if (!m_isCreated) { return; }

	// Stop the multimedia timer thread for the output and reset time delays
	if (0 != m_uOutputTimer)
	{
		::timeKillEvent(m_uOutputTimer);

		::timeEndPeriod(1);

		m_uOutputTimer = 0;
	}

	// Destroy the Queues that run the threads
	bool l_Processed = true;

	m_oTriggerQueue.clear();

	while (l_Processed)
	{
		ProcessProductRequests(l_Processed);
	}

	m_oNotifyInspectionsSet.clear();
	m_ProcessInspectionsSet.clear();

	if (!(m_PendingCameraResponses.empty()))
	{
		SVObjectManagerClass::Instance().AdjustPendingImageIndicator(-(static_cast<long>(m_PendingCameraResponses.size())));

		m_PendingCameraResponses.clear();
	}

	m_ProductRequests.Destroy();
	m_DataValidDelayQueue.Destroy();
	m_oOutputsDelayQueue.Destroy();
	m_oOutputsResetQueue.Destroy();
	m_oInspectionQueue.Destroy();
	m_CameraResponseQueue.Destroy();
	m_oCamerasQueue.Destroy();

	// Delete buckets for the PPQ positions
	m_ppPPQPositions.clear();

	m_qAvailableProductInfos.RemoveAll();

	delete[] m_pMasterProductInfos;

	m_pMasterProductInfos = nullptr;

	m_qAvailableProductInfos.Destroy();

	m_AllInputs.clear();
	m_UsedInputs.clear();
	m_AllOutputs.clear();
	m_UsedOutputs.clear();

	// Destroy the managed index for the input circle buffer
	m_pResultDataCircleBuffer.clear();
	m_pResultImagePublishedCircleBuffer.clear();

	m_isCreated = false;
}// end Destroy

void SVPPQObject::DetachAll()
{
	DetachTrigger(m_pTrigger);

	std::deque< SVVirtualCamera* > l_Cameras;

	GetCameraList(l_Cameras);

	std::deque< SVVirtualCamera* >::iterator l_Iter = l_Cameras.begin();

	while (l_Iter != l_Cameras.end())
	{
		SVVirtualCamera* pCamera = (*l_Iter);

		DetachCamera(pCamera);

		++l_Iter;
	}

	int iSize = static_cast<int> (m_arInspections.size());
	for (int i = iSize - 1; i >= 0; i--)
	{
		DetachInspection(m_arInspections[i]);
	}
}

void SVPPQObject::SetPPQOutputMode(SvDef::SVPPQOutputModeEnum oPPQOutputMode)
{
	m_oOutputMode = oPPQOutputMode;
}

void SVPPQObject::SetOutputDelay(long lDelayTime)
{
	m_lOutputDelay = lDelayTime;
}

void SVPPQObject::SetResetDelay(long lResetTime)
{
	m_lResetDelay = lResetTime;
}

void SVPPQObject::SetPPQLength(long lPPQLength)
{
	long oldPPQLength = GetPPQLength();
	if (oldPPQLength != lPPQLength)
	{
		if (2 < oldPPQLength && lPPQLength < 3)
		{
			SVObjectManagerClass::Instance().IncrementShortPPQIndicator();
		}
		else if (2 < lPPQLength && oldPPQLength < 3)
		{
			SVObjectManagerClass::Instance().DecrementShortPPQIndicator();
		}
	}

	m_PpqValues.setValueObject(SvDef::FqnPpqLength, lPPQLength);

	if (GetPPQLength() != m_ppPPQPositions.size())
	{
		Rebuild();
	}
}

void SVPPQObject::SetMaintainSourceImages(bool bMaintainImages)
{
	m_bMaintainSourceImages = bMaintainImages;
}

void SVPPQObject::SetInspectionTimeout(long lTimeoutMillisec)
{
	m_lInspectionTimeoutMillisec = lTimeoutMillisec;
}

void SVPPQObject::GetPPQOutputMode(SvDef::SVPPQOutputModeEnum &oPPQOutputMode) const
{
	oPPQOutputMode = m_oOutputMode;
}

void SVPPQObject::GetOutputDelay(long &lDelayTime) const
{
	lDelayTime = m_lOutputDelay;
}

void SVPPQObject::GetResetDelay(long &lResetTime) const
{
	lResetTime = m_lResetDelay;
}

void SVPPQObject::GetPPQLength(long &lPPQLength) const
{
	lPPQLength = GetPPQLength();
}

long SVPPQObject::GetPPQLength() const
{
	long length = 0;
	BasicValueObjectPtr pValue = m_PpqValues.getValueObject(SvDef::FqnPpqLength);
	if (!pValue.empty())
	{
		pValue->getValue(length);
	}
	return length;
}

void SVPPQObject::GetMaintainSourceImages(bool& bMaintainImages) const
{
	bMaintainImages = m_bMaintainSourceImages;
}

void SVPPQObject::GetInspectionTimeout(long& rlTimeoutMillisec) const
{
	rlTimeoutMillisec = m_lInspectionTimeoutMillisec;
}

bool SVPPQObject::AttachTrigger(SvTi::SVTriggerObject* pTrigger)
{
	if (nullptr == pTrigger) { return false; }

	m_pTrigger = pTrigger;
	if (!m_pTrigger->RegisterFinishProcess(this, SVFinishTriggerCallback)) { return false; }

	return true;
}

bool SVPPQObject::AttachCamera(SVVirtualCamera* pCamera, long lPosition, bool p_AllowMinusOne)
{
	bool l_bOk = true;

	if (nullptr == pCamera) { return false; }

	if (!p_AllowMinusOne && lPosition < 0) { lPosition = 0; }

	SVCameraInfoMap::iterator l_Iter = m_Cameras.find(pCamera);

	if (l_Iter != m_Cameras.end())
	{
		l_Iter->second.m_CameraPPQIndex = lPosition;
	}
	else
	{
		std::string l_Name = SvUl::Format(_T("%s Toggle"), pCamera->GetName());

		m_Cameras[pCamera].m_CameraPPQIndex = lPosition;
		m_Cameras[pCamera].m_ToggleState = true;

		RebuildProductInfoStructs();

		l_bOk &= pCamera->RegisterFinishProcess(this, SVFinishCameraCallback);
	}

	return l_bOk;
}

bool SVPPQObject::AttachInspection(SVInspectionProcess* pInspection)
{
	if (nullptr == pInspection) { return false; }

	m_arInspections.push_back(pInspection);

	pInspection->SetPPQIdentifier(GetUniqueObjectID());

	return true;
}

bool SVPPQObject::DetachTrigger(SvTi::SVTriggerObject* pTrigger)
{
	bool bOk = nullptr != m_pTrigger && m_pTrigger == pTrigger;

	if (bOk)
	{
		bOk = m_pTrigger->UnregisterFinishProcess(this);

		m_pTrigger = nullptr;
	}

	return bOk;
}// end DetachTrigger

bool SVPPQObject::DetachCamera(SVVirtualCamera* pCamera, bool bRemoveDepends/*=false*/)
{
	if (nullptr == pCamera) { return false; }

	bool l_Status = true;

	l_Status &= pCamera->UnregisterFinishProcess(this, SVFinishCameraCallback);

	SVCameraInfoMap::iterator l_svIter = m_Cameras.find(pCamera);

	if (l_svIter != m_Cameras.end())
	{
		m_Cameras.erase(l_svIter);
	}

	//check inspections and remove camera from them also...
	if (bRemoveDepends)
	{
		for (auto pInspection : m_arInspections)
		{
			if (nullptr != pInspection) { pInspection->RemoveCamera(pCamera->GetName()); }
		}
	}

	RebuildProductInfoStructs();

	return l_Status;
}// end DetachCamera

bool SVPPQObject::DetachInspection(SVInspectionProcess* pInspection)
{
	if (nullptr == pInspection) { return false; }

	int i;
	bool bFound = false;
	int iSize = static_cast<int> (m_arInspections.size());
	for (i=0; i < iSize; i++)
	{
		if (pInspection == m_arInspections[i])
		{
			bFound = true;
			break;
		}// end if
	}// end for

	if (!bFound) { return false; }

	pInspection->SetPPQIdentifier(SV_GUID_NULL);

	SVProductInfoStruct l_svProduct;

	m_arInspections[i]->LastProductUpdate(&l_svProduct);

	m_arInspections.erase(m_arInspections.begin() + i);

	return true;
}// end DetachInspection

bool SVPPQObject::AddSharedCamera(SVVirtualCamera* pCamera)
{
	bool l_Status = (nullptr != pCamera);

	if (l_Status)
	{
		SVCameraInfoMap::iterator l_svIter = m_Cameras.find(pCamera);

		l_Status = (l_svIter != m_Cameras.end());

		if (l_Status && l_svIter->second.m_CameraPPQIndex == -1)
		{
			l_svIter->second.m_CameraPPQIndex = 0;
		}
	}

	if (!l_Status)
	{
		l_Status = AttachCamera(pCamera, 0);

		Rebuild();
	}

	return l_Status;
}

void SVPPQObject::GetInspectionCount(long &lSize) const
{
	lSize = static_cast<long> (m_arInspections.size());
}

size_t SVPPQObject::GetCameraCount() const
{
	return m_Cameras.size();
}

HRESULT SVPPQObject::GetCameraList(std::deque< SVVirtualCamera* >& p_rCameras) const
{
	HRESULT l_Status = S_OK;

	p_rCameras.clear();

	SVCameraInfoMap::const_iterator l_svIter;

	l_svIter = m_Cameras.begin();

	while (l_svIter != m_Cameras.end())
	{
		p_rCameras.push_back(l_svIter->first);

		++l_svIter;
	}

	return l_Status;
}

HRESULT SVPPQObject::GetVirtualCameras(SVVirtualCameraMap& p_rCameras) const
{
	HRESULT l_Status = S_OK;

	p_rCameras.clear();

	SVCameraInfoMap::const_iterator l_svIter = m_Cameras.begin();

	while (l_svIter != m_Cameras.end())
	{
		if (nullptr != l_svIter->first)
		{
			p_rCameras[l_svIter->first->GetName()] = l_svIter->first;
		}

		++l_svIter;
	}

	return l_Status;
}

void SVPPQObject::GetTrigger(SvTi::SVTriggerObject*& ppTrigger)
{
	ppTrigger = m_pTrigger;
}// end GetTrigger

bool SVPPQObject::GetInspection(long lIndex, SVInspectionProcess*& ppInspection) const
{
	if (lIndex < 0 || lIndex >= static_cast<long> (m_arInspections.size())) { return false; }

	ppInspection = m_arInspections[lIndex];

	return true;
}// end GetInspection

bool SVPPQObject::SetCameraPPQPosition(long lPosition, SVVirtualCamera* pCamera)
{
	bool l_Status = true;

	//Only do an assert check so that in release mode no check is made
	ASSERT(nullptr != pCamera);

	if (m_isCreated && -1 <= lPosition && lPosition < static_cast<long>(m_ppPPQPositions.size()) - 1)
	{
		l_Status = AttachCamera(pCamera, lPosition, true);
	}
	else
	{
		l_Status = false;
	}

	return l_Status;
}// end SetCameraPPQPosition

bool SVPPQObject::GetCameraPPQPosition(long &lPosition, SVVirtualCamera* pCamera) const
{
	bool bFound;

	//Only do an assert check so that in release mode no check is made
	ASSERT(nullptr != pCamera);

	lPosition = -1;

	if (!m_isCreated)
	{
		return false;
	}// end if

	// Try to find to the Camera they sent it
	SVCameraInfoMap::const_iterator l_svIter = m_Cameras.find(pCamera);

	bFound = l_svIter != m_Cameras.end();

	if (!bFound) { return false; }

	lPosition = l_svIter->second.m_CameraPPQIndex;

	return true;
}// end GetCameraPPQPosition

void SVPPQObject::RebuildProductInfoStructs()
{
	SVGuidSVCameraInfoStructMap l_CameraInfos;

	BuildCameraInfos(l_CameraInfos);

	for (int j = 0; nullptr != m_pMasterProductInfos && j < GetPPQLength() + g_lPPQExtraBufferSize; j++)
	{
		m_pMasterProductInfos[j].m_svCameraInfos = l_CameraInfos;
	}// end for
}

void SVPPQObject::AssignCameraToAcquisitionTrigger()
{
	if (m_pTrigger->IsAcquisitionTrigger() && m_pTrigger->mpsvDevice)
	{
		// Get Camera by Trigger DigNum
		int iDigNum = m_pTrigger->mpsvDevice->miChannelNumber;

		for (SVCameraInfoMap::iterator it = m_Cameras.begin(); it != m_Cameras.end(); ++it)
		{
			SVVirtualCamera* pCamera = it->first;
			if (nullptr != pCamera)
			{
				SVAcquisitionClassPtr acquisitionPtr = pCamera->GetAcquisitionDevice();
				if (!acquisitionPtr.empty() && acquisitionPtr->DigNumber() == iDigNum)
				{
					m_pTrigger->mpsvDevice->m_triggerchannel = acquisitionPtr->m_hDigitizer;
					break;
				}
			}
		}
	}
}

void SVPPQObject::PrepareGoOnline()
{
	// Fixup Acquisition triggers (as Cameras can be in a different order than Triggers)
	AssignCameraToAcquisitionTrigger();

	if (!m_pTrigger->CanGoOnline())
	{
		SvDef::StringVector msgList;
		msgList.push_back(m_pTrigger->GetCompleteName());

		SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_CanGoOnlineFailure_Trigger, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10185);
		throw Msg;
	}

	for (SVCameraInfoMap::iterator l_svIter = m_Cameras.begin(); l_svIter != m_Cameras.end(); ++l_svIter)
	{
		if (!(l_svIter->second.m_CameraPPQIndex >= 0 && l_svIter->first->CanGoOnline()))
		{
			SvDef::StringVector msgList;
			msgList.push_back(l_svIter->first->GetCompleteName());

			SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_CanGoOnlineFailure_Acquisition, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10185);
			throw Msg;
		}
	}

	// Reset the PPQ
	for (size_t i = 0; i < m_ppPPQPositions.size(); ++i)
	{
		SVProductInfoStruct* l_pProduct = m_ppPPQPositions.GetProductAt(i);
		if (nullptr != l_pProduct)
		{
			if (!RecycleProductInfo(l_pProduct))
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_GoOnlineFailure_RecycleProduct, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10185);
				throw Msg;
			}

			m_ppPPQPositions.SetProductAt(i, nullptr);
		}

		if (m_qAvailableProductInfos.RemoveHead(&l_pProduct) && nullptr != l_pProduct)
		{
			HRESULT hRTemp = m_ppPPQPositions.SetProductAt(i, l_pProduct);

			if (S_OK == hRTemp)
			{
				if (0 == i)
				{
					hRTemp = GetNextAvailableIndexes(l_pProduct->oPPQInfo, SV_PPQ);
				}
				else
				{
					hRTemp = TheSVDataManager.GetNextAvailableBufferIndex(m_pResultDataCircleBuffer, SV_PPQ, l_pProduct->oPPQInfo.m_ResultDataDMIndexHandle);
				}
			}

			if (S_OK != hRTemp)
			{
				SvDef::StringVector msgList;
				msgList.push_back(SvUl::Format(_T("%X"), hRTemp));
				SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_GoOnlineFailure_RecycleProduct, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10185);
				throw Msg;
			}
		}
		else
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_GoOnlineFailure_RecycleProduct, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10185);
			throw Msg;
		}
	}// end for

	size_t iSize = m_arInspections.size();
	for (size_t i = 0; i < iSize; i++)
	{
		if (!m_arInspections[i]->CanGoOnline())
		{
			SvStl::MessageTextEnum messageId = SvStl::Tid_Empty;
			SvDef::StringVector msgList;

			//@TODO[gra][7.40][25.05.2016]: This should at a later stage show all the tool errors not only the first error
			SvStl::MessageContainerVector AllToolMessages;
			m_arInspections[i]->getToolMessages(std::back_inserter(AllToolMessages));
			SvStl::MessageContainerVector::const_iterator Iter(AllToolMessages.begin());
			SvStl::SourceFileParams sourceFileParam(StdMessageParams);
			if (AllToolMessages.end() != Iter)
			{
				std::string sToolName = SVObjectManagerClass::Instance().GetCompleteObjectName(Iter->getObjectId());
				msgList.push_back(sToolName);
				msgList.push_back(std::string(Iter->what()));
				messageId = SvStl::Tid_CanGoOnlineFailure_InspectionTool;
				sourceFileParam = Iter->getMessage().m_SourceFile;
			}
			else
			{
				messageId = SvStl::Tid_CanGoOnlineFailure_Inspection;
				msgList.push_back(m_arInspections[i]->GetCompleteName());
			}
			SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, messageId, msgList, sourceFileParam, SvStl::Err_10185);
			throw Msg;
		}
	}// end for

	if (!ResolveConditionalOutput())
	{
		SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_CanGoOnlineFailure_ConditionalOutput, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10185);
		throw Msg;
	}
}


void SVPPQObject::GoOnline()
{
	std::string FailureObjectName;

#ifdef EnableTracking
	m_PPQTracking.clear();
	m_PPQTracking.m_QueueLength = m_ppPPQPositions.size();
	m_PPQTracking.m_TimeLength = m_lOutputDelay + 50;
#endif
	RebuildOutputList();

	// First, make sure the trigger toggle output is set to the right default
	SvOi::IValueObject* pValueObject(nullptr);
	if (!m_pTriggerToggle.empty())
	{
		pValueObject = m_pTriggerToggle->getValueObject();
	}
	if (nullptr != pValueObject)
	{
		_variant_t Value(false);
		pValueObject->setValue(Value);
		pValueObject->CopyValue(cPpqDefaultIndex);
		m_TriggerToggle = static_cast<bool> (Value);
	}// end if

	pValueObject = nullptr;
	if (!m_pOutputToggle.empty())
	{
		pValueObject = m_pOutputToggle->getValueObject();
	}
	// Also, make sure the output toggle output is set to the right default
	if (nullptr != pValueObject)
	{
		_variant_t Value(false);
		pValueObject->setValue( Value );
		pValueObject->CopyValue(cPpqDefaultIndex);
		m_OutputToggle = static_cast<bool> (Value);
	}// end if

	// Reset the Outputs
	if (!(m_pTriggerToggle.empty()))
	{
		m_pTriggerToggle->m_Enabled = true;
	}

	if (!(m_pOutputToggle.empty()))
	{
		m_pOutputToggle->m_Enabled = true;
	}

	ResetOutputs();

	if (!(m_pTriggerToggle.empty()))
	{
		m_pTriggerToggle->m_Enabled = false;
	}

	if (!(m_pOutputToggle.empty()))
	{
		m_pOutputToggle->m_Enabled = false;
	}

	size_t lSize = m_arInspections.size();
	bool bInspGoOnline = true;
	for (size_t i = 0; i < lSize; i++)
	{
		if (!m_arInspections[i]->GoOnline())
		{
			FailureObjectName = m_arInspections[i]->GetCompleteName();
			bInspGoOnline = false;
			break;
		}
	}// end for

	if (!bInspGoOnline)
	{
		lSize = m_arInspections.size();
		for (size_t i = 0; i < lSize; i++)
		{
			m_arInspections[i]->GoOffline();
		}// end for

		SvDef::StringVector msgList;
		msgList.push_back(FailureObjectName);

		SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_GoOnlineFailure_Inspection, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10185);
		throw Msg;
	}// end if

	if (m_pTrigger->IsSoftwareTrigger())
	{
		// must do this before the Camera starts for Digital cameras
		HRESULT hr = m_pTrigger->EnableInternalTrigger();
		if (S_OK != hr)
		{
			SvDef::StringVector msgList;
			msgList.push_back(SvUl::Format(_T("%X"), hr));

			SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_GoOnlineFailure_InternalTrigger, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10185);
			throw Msg;
		}
	}

	SVCameraInfoMap::iterator l_svIter;
	bool bCameraGoOnline = true;
	for (l_svIter = m_Cameras.begin(); l_svIter != m_Cameras.end(); ++l_svIter)
	{
		if (!l_svIter->first->GoOnline())
		{
			FailureObjectName = l_svIter->first->GetCompleteName();

			bCameraGoOnline = false;
			break;
		}
	}// end for

	if (!bCameraGoOnline)
	{
		for (l_svIter = m_Cameras.begin(); l_svIter != m_Cameras.end(); ++l_svIter)
		{
			l_svIter->first->GoOffline();
		}// end for

		lSize = m_arInspections.size();
		for (size_t i = 0; i < lSize; i++)
		{
			m_arInspections[i]->GoOffline();
		}// end for

		SvDef::StringVector msgList;
		msgList.push_back(FailureObjectName);

		SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_GoOnlineFailure_Acquisition, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10185);
		throw Msg;
	}// end if

	if (!m_pTrigger->GoOnline())
	{
		FailureObjectName = m_pTrigger->GetCompleteName();
		m_pTrigger->GoOffline();

		SVCameraInfoMap::iterator l_svIter;

		for (l_svIter = m_Cameras.begin(); l_svIter != m_Cameras.end(); ++l_svIter)
		{
			l_svIter->first->GoOffline();
		}// end for

		lSize = m_arInspections.size();
		for (size_t i = 0; i < lSize; i++)
		{
			m_arInspections[i]->GoOffline();
		}// end for

		SvDef::StringVector msgList;
		msgList.push_back(FailureObjectName);

		SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_GoOnlineFailure_Trigger, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10185);
		throw Msg;
	}// end if

	// Create a multimedia timer thread for the output and reset time delays
	if (TIMERR_NOERROR != ::timeBeginPeriod(1))
	{
		SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_GoOnlineFailure_CreateTimerThread, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10185);
		throw Msg;
	}

	//The timer should start when not "Next Trigger Mode" or when reset or data valid delay are not 0
	bool StartTimer(SvDef::SVPPQNextTriggerMode != m_oOutputMode);
	StartTimer |= 0 < m_lResetDelay;
	StartTimer |= 0 < m_DataValidDelay;
	if (StartTimer)
	{
		m_uOutputTimer = ::timeSetEvent(1, 0, SVPPQObject::OutputTimerCallback, reinterpret_cast<DWORD_PTR>(this),
			TIME_PERIODIC | TIME_CALLBACK_FUNCTION);
	}// end if

	// Create the PPQ's threads
	if (S_OK != m_AsyncProcedure.Create(&SVPPQObject::APCThreadProcess, boost::bind(&SVPPQObject::ThreadProcess, this, _1), GetName(), SVAffinityPPQ))
	{
		SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_GoOnlineFailure_CreatePPQThread, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10185);
		throw Msg;
	}

	// Kick the threads up a notch, for real!
	m_AsyncProcedure.SetPriority(THREAD_PRIORITY_HIGHEST);

	m_bOnline = true;

	// This snippet of code here apears to do nothing, but it is actually 
	// initializing the RTTI for the output object parents.  This insures that there 
	// is no appreciable delay in the writeOutputs function that uses dynaic_cast.
	for (size_t i = 0; i < m_UsedOutputs.size(); i++)
	{
		SVObjectClass* pObject = m_UsedOutputs[i]->getObject();
		void* l_pInit = nullptr != pObject ? dynamic_cast<SVPPQObject*>(pObject->GetOwner()) : nullptr;
	}

	if (SvDef::SVPPQNextTriggerMode == m_oOutputMode)
	{
		m_NAKCount = -static_cast<long>(m_ppPPQPositions.size());
	}
	else
	{
		m_NAKCount = 0;
	}
	m_NewNAKCount = m_NAKCount;
	m_FirstNAKProcessCount = 0;
	m_ReducedPPQPosition = MinReducedPPQPosition;
	const int HundredPercent = 100;
	if ((SvDef::FixedMaximum == m_NAKMode || SvDef::RepairedLegacy == m_NAKMode) && m_NAKParameter > 0 && m_NAKParameter <= HundredPercent)
	{
		m_ReducedPPQPosition = (long(m_ppPPQPositions.size()) * m_NAKParameter) / HundredPercent;
		if (m_ReducedPPQPosition < MinReducedPPQPosition)
		{
			m_ReducedPPQPosition = MinReducedPPQPosition;
		}
		m_ReducedPPQPosition = std::min< long >(m_ReducedPPQPosition, static_cast<long>(m_ppPPQPositions.size()));
	}
}// end GoOnline



bool SVPPQObject::GoOffline()
{
	if (!m_bOnline) { return false; }

#ifdef EnableTracking
	if (TheSVObserverApp.UpdateAndGetLogDataManager())
	{
		std::string l_FileName;

		l_FileName.Format(_T("C:\\SVObserver\\ProductIndexes_%ld-%s.log"),
			SVObjectManagerClass::Instance().GetFileSequenceNumber(), GetName());

		std::fstream l_Stream(l_FileName.ToString(), std::ios_base::trunc | std::ios_base::out);

		if (l_Stream.is_open())
		{
			for (size_t i = 0; i < m_ppPPQPositions.size(); ++i)
			{
				std::string l_Info;

				SVProductInfoStruct* l_pProduct = m_ppPPQPositions.GetProductAt(i);

				if (nullptr != l_pProduct)
				{
					l_pProduct->DumpIndexInfo(l_Info);
				}

				l_Stream << _T("Position ") << i << _T(" : ");
				l_Stream << l_Info.ToString();
				l_Stream << std::endl;
			}

			l_Stream.close();
		}

		std::string l_Name;

		l_Name.Format(_T("%s-%ld"), GetName(), SVObjectManagerClass::Instance().GetFileSequenceNumber());

		DumpDMInfo(l_Name.ToString());

		l_Name.Format(_T("C:\\SVObserver\\%s-Counts-%ld.csv"), GetName(), SVObjectManagerClass::Instance().GetFileSequenceNumber());

		std::fstream l_TrackingStream(l_Name.ToString(), std::ios_base::trunc | std::ios_base::out);

		if (l_TrackingStream.is_open())
		{
			l_TrackingStream << _T("Name,Count") << std::endl;

			SVPPQTracking::SVPPQTrackingMap::iterator l_PPQIter = m_PPQTracking.m_Counts.begin();

			while (l_PPQIter != m_PPQTracking.m_Counts.end())
			{
				l_TrackingStream << l_PPQIter->first.c_str() << _T(",");
				l_TrackingStream << l_PPQIter->second;
				l_TrackingStream << std::endl;

				++l_PPQIter;
			}

			l_TrackingStream << std::endl;

			l_TrackingStream << _T("Name");

			for (size_t i = 0; i < m_ppPPQPositions.size(); ++i)
			{
				l_TrackingStream << _T(",") << i;
			}

			l_TrackingStream << std::endl;

			SVPPQTracking::SVQueueTrackingMap::iterator l_QueueIter = m_PPQTracking.m_QueueCounts.begin();

			while (l_QueueIter != m_PPQTracking.m_QueueCounts.end())
			{
				l_TrackingStream << l_QueueIter->first.c_str();

				SVPPQTrackingElement& l_rElement = l_QueueIter->second;

				for (size_t i = 0; i < l_rElement.m_TrackedCounts.size(); ++i)
				{
					l_TrackingStream << _T(",") << l_rElement.m_TrackedCounts[i];
				}

				l_TrackingStream << std::endl;

				++l_QueueIter;
			}

			l_TrackingStream << std::endl;

			// Write Outputs Time...
			l_TrackingStream << _T("Time ms");

			for (size_t i = 0; i < m_PPQTracking.m_TimeLength; ++i)
			{
				l_TrackingStream << _T(",") << i;
			}

			l_TrackingStream << std::endl;

			l_QueueIter = m_PPQTracking.m_QueueWriteTimeCounts.begin();  // m_QueueCounts.begin();

			while (l_QueueIter != m_PPQTracking.m_QueueWriteTimeCounts.end())
			{
				l_TrackingStream << l_QueueIter->first.c_str();

				SVPPQTrackingElement& l_rElement = l_QueueIter->second;

				for (size_t i = 0; i < l_rElement.m_TrackedCounts.size(); ++i)
				{
					l_TrackingStream << _T(",") << l_rElement.m_TrackedCounts[i];
				}

				l_TrackingStream << std::endl;

				++l_QueueIter;
			}
			l_TrackingStream << std::endl;

			l_TrackingStream.close();
		}
	}
#endif EnableTracking

	m_pTrigger->GoOffline();

	SVCameraInfoMap::iterator l_svIter;

	for (l_svIter = m_Cameras.begin(); l_svIter != m_Cameras.end(); ++l_svIter)
	{
		l_svIter->first->GoOffline();
	}// end for



	for (auto pInspection : m_arInspections)
	{
		pInspection->GoOffline();
	}// end for

	// Stop the multimedia timer thread for the output and reset time delays
	if (0 != m_uOutputTimer)
	{
		::timeKillEvent(m_uOutputTimer);

		::timeEndPeriod(1);

		m_uOutputTimer = 0;
	}

	// Bring the threads back down to earth
	m_AsyncProcedure.SetPriority(THREAD_PRIORITY_NORMAL);

	m_AsyncProcedure.Destroy();

	m_bOnline = false;

	unsigned long l_ClearCount = 0;

	for (long i = static_cast<long>(m_ppPPQPositions.size()) - 1; l_ClearCount < 5 && 0 < i; --i)
	{
		SVProductInfoStruct* l_pProduct = m_ppPPQPositions.GetProductAt(i);

		if (nullptr != l_pProduct)
		{
			if (!l_pProduct->oPPQInfo.m_ResultImagePublishedDMIndexHandle.empty())
			{
				l_pProduct->oPPQInfo.m_ResultImagePublishedDMIndexHandle.clear();

				++l_ClearCount;
			}

			l_pProduct->SetProductComplete();
		}
	}
	if (HasActiveMonitorList())
	{
		SetMonitorList(ActiveMonitorList(false, RejectDepthAndMonitorList()));
	}

	return true;
}// end GoOffline

bool SVPPQObject::IsOnline() const
{
	return m_bOnline;
}// end IsOnline

void SVPPQObject::AddInput(SVIOEntryHostStructPtr pInput)
{
	m_AllInputs.push_back(pInput);
}// end AddInput

bool SVPPQObject::RemoveInput(SVIOEntryHostStructPtr pInput)
{
	bool l_Status = false;

	std::string strName;

	if (nullptr != pInput->getObject())
	{
		strName = pInput->getObject()->GetName();
	}
	else if (!(pInput->m_IOId.empty()))
	{
		SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject(pInput->m_IOId);

		strName = l_pObject->GetName();
	}

	SVIOEntryHostStructPtrVector::iterator l_Iter = m_AllInputs.begin();

	while (l_Iter != m_AllInputs.end())
	{
		l_Status = (strName == (*l_Iter)->getObject()->GetName());

		if (l_Status)
		{
			m_AllInputs.erase(l_Iter);

			break;
		}

		++l_Iter;
	}

	return l_Status;
}

// AssignInputs () ----------------------------------------------------------
// This is only ever called from SVPPQObject::InitializeProduct ().
// InitializeProduct () is only ever called from ProcessTrigger ().

bool SVPPQObject::AssignInputs(const SVVariantBoolVector& rInputValues)
{
	for (size_t i = 0; i < m_UsedInputs.size(); i++)
	{
		SVIOEntryHostStructPtr pIOEntry = m_UsedInputs[i];

		if (!(pIOEntry.empty()) && pIOEntry->m_Enabled)
		{
			if (0 <= pIOEntry->m_PPQIndex && pIOEntry->m_PPQIndex < static_cast<long>(m_ppPPQPositions.size()))
			{
				SVProductInfoStruct* l_pProduct = m_ppPPQPositions.GetProductAt(pIOEntry->m_PPQIndex);

				SVIOEntryStruct& l_pIOEntry = l_pProduct->oPPQInfo.m_InputData[i];
				long lDataIndex = l_pProduct->oPPQInfo.m_ResultDataDMIndexHandle.GetIndex();

				if (pIOEntry->m_ObjectType != IO_CAMERA_DATA_INPUT)
				{
					SvOi::IValueObject* pValueObject = l_pIOEntry.m_IOEntryPtr->getValueObject();
					if (nullptr != pValueObject)
					{
						pValueObject->setValue(rInputValues[i].first);
						pValueObject->CopyValue(lDataIndex);
					}
					l_pIOEntry.m_EntryValid = rInputValues[i].second;
				}
				else
				{
					l_pIOEntry.m_EntryValid = true;
				}

				// @Warning - I believe this is redundant.  AssignInputs () 
				// is only processed through ProcessTrigger (), where it is 
				// also inserted.  Should not cause a problem because it is an
				// std::set, but only needs done in one place and that should
				// be within ProcessTrigger ().
				m_oNotifyInspectionsSet.insert(l_pProduct->ProcessCount());
			}
		}
	}

	return true;
}

HRESULT SVPPQObject::GetInputIOValues(SVVariantBoolVector& rInputValues) const
{
	HRESULT l_Status(S_OK);

	if (!m_pInputList->ReadInputs(m_UsedInputs, rInputValues))
	{
		l_Status = E_FAIL;
	}

#if defined (TRACE_THEM_ALL) || defined (TRACE_INPUT_VALUES)
	SVVariantBoolVector::const_iterator Iter = rInputValues.begin();
	std::string DebugString = _T("Inputs");
	for (; rInputValues.end() != Iter; ++Iter)
	{
		//Only valid if second is true
		if (Iter->second)
		{
			DebugString += Iter->first ? _T(", 1") : _T(", 0");
		}
	}
	DebugString += _T("\r\n");
	::OutputDebugString(DebugString.c_str());
#endif

	return l_Status;
}

bool SVPPQObject::RebuildInputList(bool bHasCameraTrigger)
{
	SVIOEntryHostStructPtrVector ppNewInputs;
	SVIOEntryHostStructPtr pOldInput;
	SVIOEntryHostStructPtr pNewInput;
	size_t iNew;
	size_t iOld;
	size_t j;
	size_t lNewSize;

	// Make sure all the defaults are here for old configurations
	AddDefaultInputs();

	if (m_pInputList && m_pInputList->FillInputs(ppNewInputs))
	{
		RemoveCameraDataInputs(ppNewInputs);
		if (bHasCameraTrigger)
		{
			AddCameraDataInputs(ppNewInputs);
		}
		lNewSize = ppNewInputs.size();

		for (iOld = 0; iOld < m_AllInputs.size(); iOld++)
		{
			pOldInput = m_AllInputs[iOld];

			for (iNew = 0; iNew < lNewSize; iNew++)
			{
				pNewInput = ppNewInputs[iNew];

				SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject(pNewInput->m_IOId);

				if ((0 == strcmp(l_pObject->GetName(), pOldInput->getObject()->GetName())) &&
					(pNewInput->m_ObjectType == pOldInput->m_ObjectType))
				{
					pNewInput->m_DeleteValueObject = false;
					pNewInput->setObject(pOldInput->getObject());
					pNewInput->m_PPQIndex = pOldInput->m_PPQIndex;
					pNewInput->m_Enabled = pOldInput->m_Enabled;
					pOldInput->m_ObjectType = pNewInput->m_ObjectType;
					pOldInput->m_IOId = pNewInput->m_IOId;
					pOldInput->getObject()->SetObjectDepth(GetPPQLength() + g_lPPQExtraBufferSize);
					pOldInput->getObject()->ResetObject();
					break;
				}// end if
			}// end for
		}// end for

		m_UsedInputs = ppNewInputs;

		std::sort(m_UsedInputs.begin(), m_UsedInputs.end(), &SVIOEntryHostStruct::PtrGreater);

		// Fill out all the product info structs with the current inputs
		for (j = 0; j < (m_ppPPQPositions.size() + g_lPPQExtraBufferSize); j++)
		{
			m_pMasterProductInfos[j].oPPQInfo.m_InputData.resize(m_UsedInputs.size());

			for (size_t z = 0; z < m_UsedInputs.size(); ++z)
			{
				m_pMasterProductInfos[j].oPPQInfo.m_InputData[z].m_IOEntryPtr = m_UsedInputs[z];
			}
		}

		return true;
	}// end if

	return false;
}// end RebuildInputList

typedef boost::function<bool(SVIOEntryHostStructPtr ioEntry, const SVIOObjectType& type)> CompareIOEntryTypeFunc;
static bool CompareTypeWithIOEntry(SVIOEntryHostStructPtr ioEntry, const SVIOObjectType& type)
{
	bool bRetVal = false;
	if (ioEntry)
	{
		bRetVal = (ioEntry->m_ObjectType == type);
	}
	return bRetVal;
}

typedef boost::function<bool(SVIOEntryHostStructPtr ioEntry, const std::string& name)> CompareIOEntryNameFunc;
static bool CompareNameWithIOEntry(SVIOEntryHostStructPtr pIoEntry, const std::string& name)
{
	bool bRetVal = false;
	if (nullptr != pIoEntry && nullptr != pIoEntry->getObject())
	{
		bRetVal = (pIoEntry->getObject()->GetName() == name);
	}
	return bRetVal;
}

static bool CompareCompleteNameWithIOEntry(SVIOEntryHostStructPtr pIoEntry, const std::string& name)
{
	bool bRetVal = false;
	if (nullptr != pIoEntry && nullptr != pIoEntry->getObject())
	{
		bRetVal = (pIoEntry->getObject()->GetCompleteName() == name);
	}
	return bRetVal;
}

// @CLEAN This is a different class that should be moved to its own file.
template<typename CompareTo, typename CompareFunc>
class FindIOEntry
{
public:
	FindIOEntry(const CompareTo& data, CompareFunc func)
		: m_data(data), m_compareFunc(func) {}

	bool operator()(SVIOEntryHostStructPtr ioEntry) const
	{
		return m_compareFunc(ioEntry, m_data);
	}

private:
	CompareTo m_data;
	CompareFunc m_compareFunc;
};

void SVPPQObject::AddCameraDataInputs(SVIOEntryHostStructPtrVector& list)
{
	// Added the new Camera inputs...
	SVIOEntryHostStructPtrVector::iterator it = std::find_if(list.begin(), list.end(), FindIOEntry<std::string, CompareIOEntryNameFunc>(m_CameraInputData.GetTimestampName(), CompareNameWithIOEntry));
	if (it == list.end())
	{
		// Get the Input from the All list
		SVIOEntryHostStructPtrVector::const_iterator refIt = std::find_if(m_AllInputs.begin(), m_AllInputs.end(), FindIOEntry<std::string, CompareIOEntryNameFunc>(m_CameraInputData.GetTimestampName(), CompareNameWithIOEntry));
		if (refIt != m_AllInputs.end())
		{
			list.push_back(*refIt);
		}
	}
	it = std::find_if(list.begin(), list.end(), FindIOEntry<std::string, CompareIOEntryNameFunc>(m_CameraInputData.GetLineStateName(), CompareNameWithIOEntry));
	if (it == list.end())
	{
		// Get the Input from the All list
		SVIOEntryHostStructPtrVector::const_iterator refIt = std::find_if(m_AllInputs.begin(), m_AllInputs.end(), FindIOEntry<std::string, CompareIOEntryNameFunc>(m_CameraInputData.GetLineStateName(), CompareNameWithIOEntry));
		if (refIt != m_AllInputs.end())
		{
			list.push_back(*refIt);
		}
	}
}

void SVPPQObject::RemoveCameraDataInputs(SVIOEntryHostStructPtrVector& list)
{
	SVIOEntryHostStructPtrVector::iterator it = std::remove_if(list.begin(), list.end(), FindIOEntry<SVIOObjectType, CompareIOEntryTypeFunc>(IO_CAMERA_DATA_INPUT, CompareTypeWithIOEntry));
	if (it != list.end())
	{
		list.erase(it, list.end());
	}
}

void SVPPQObject::SetDefaultConditionalOutput()
{
	m_conditionalOutputName = PPQ_CONDITIONAL_OUTPUT_ALWAYS;
	m_conditionalOutputValueID = GUID_NULL;
}

bool SVPPQObject::HasCameraDataInputForConditionalOutput() const
{
	return (std::string::npos != m_conditionalOutputName.find(_T("Camera")));
}

bool SVPPQObject::HasDigitalInputForConditionalOutput() const
{
	return (std::string::npos != m_conditionalOutputName.find(_T("DIO")));
}

// AddToAvailableInputs searches the m_AllInputs by name. If it does not exist,
// then the new input is added. Two types are supported: 
// IO_REMOTE_INPUT and IO_DIGITAL_INPUT.
// A variant value object is created for the remote input.
// A Boolean value object is created for the digital input.
bool SVPPQObject::AddToAvailableInputs(SVIOObjectType eType, const std::string& rName)
{
	bool bFound = false;

	// search for the input. 
	for (size_t k = 0; k < m_AllInputs.size(); k++)
	{
		SVIOEntryHostStructPtr pOldInput = m_AllInputs[k];
		if (rName == pOldInput->getObject()->GetName())
		{
			bFound = true;
			break;
		}// end if
	}// end for

	if (!bFound)
	{
		SVObjectClass* pObject = nullptr;
		if (eType == IO_REMOTE_INPUT)
		{
			// new variant value object for Remote Inputs.
			SVVariantValueObjectClass* pRemoteObject = new SVVariantValueObjectClass();
			pRemoteObject->setBucketized(true);
			pObject = dynamic_cast<SVObjectClass*> (pRemoteObject);
		}
		else
		{
			// new Bool Value Object for Digital Inputs.
			SVBoolValueObjectClass* pDigitalObject = new SVBoolValueObjectClass();
			pDigitalObject->setBucketized(true);
			pObject = dynamic_cast<SVObjectClass*> (pDigitalObject);
		}

		pObject->SetName(rName.c_str());
		pObject->SetObjectOwner(this);
		pObject->SetObjectDepth(GetPPQLength() + g_lPPQExtraBufferSize);
		pObject->SetObjectAttributesAllowed(SvDef::SV_SELECTABLE_ATTRIBUTES, SvOi::SetAttributeType::RemoveAttribute);
		pObject->ResetObject();

		SVIOEntryHostStructPtr pIOEntry = new SVIOEntryHostStruct;
		pIOEntry->setObject(pObject);
		pIOEntry->m_ObjectType = eType;
		pIOEntry->m_PPQIndex = -1;
		pIOEntry->m_Enabled = false;

		// Add input to PPQ.
		AddInput(pIOEntry);
		return true;
	}// end if
	return false;
}

// Add Default Inputs adds inputs to the m_AllInputs 
// m_AllInputs appears to be the available input list.
void SVPPQObject::AddDefaultInputs()
{
	unsigned long ulCount = 0;
	unsigned long l;

	SVIOConfigurationInterfaceClass::Instance().GetDigitalInputCount(ulCount);

	// Create all the default Digital Inputs
	for (l = 0; l < ulCount; l++)
	{
		std::string Name = SvUl::Format(_T("DIO.Input%d"), l + 1);
		AddToAvailableInputs(IO_DIGITAL_INPUT, Name);
	}// end for

	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
	SVInputObjectList* pInputObjectList(nullptr);
	if (nullptr != pConfig) { pInputObjectList = pConfig->GetInputObjectList(); }
	long lCount = 0;
	//If pointer is a nullptr then count is 0
	if (nullptr != pInputObjectList) { pInputObjectList->GetRemoteInputCount(lCount); }

	// Create all the default Remote Inputs
	for (l = 0; l < static_cast<unsigned long>(lCount); l++)
	{
		std::string Name = SvUl::Format(SvO::cRemoteInputNumberLabel, l + 1 );
		AddToAvailableInputs(IO_REMOTE_INPUT, Name);
	}// end for
}

void SVPPQObject::GetAvailableInputs(SVIOEntryHostStructPtrVector& ppIOEntries) const
{
	ppIOEntries = m_UsedInputs;
}

void SVPPQObject::GetAllInputs(SVIOEntryHostStructPtrVector& ppIOEntries) const
{
	ppIOEntries = m_AllInputs;
}

SVIOEntryHostStructPtr SVPPQObject::GetInput(const std::string& name) const
{
	for (SVIOEntryHostStructPtrVector::const_iterator it = m_AllInputs.begin(); it != m_AllInputs.end(); ++it)
	{
		if (!it->empty())
		{
			SVIOEntryHostStructPtr ptr = (*it);
			if (name == ptr->getObject()->GetName())
			{
				return ptr;
			}
		}
	}
	return SVIOEntryHostStructPtr();
}

void SVPPQObject::AddOutput(SVIOEntryHostStructPtr pOutput)
{
	m_AllOutputs.push_back(pOutput);
}

bool SVPPQObject::RemoveOutput(SVIOEntryHostStructPtr pOutput)
{
	bool l_Status = false;

	if (m_voOutputState.GetName() != "State")
	{
		m_voOutputState.SetName(_T("State"));
		m_voOutputState.SetObjectOwner(this);
	}

	std::string Name;

	if (nullptr != pOutput->getObject())
	{
		Name = pOutput->getObject()->GetCompleteName();
	}
	else if (!(pOutput->m_IOId.empty()))
	{
		SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject(pOutput->m_IOId);

		Name = l_pObject->GetCompleteName();
	}

	SVIOEntryHostStructPtrVector::iterator l_Iter = m_AllOutputs.begin();

	while (l_Iter != m_AllOutputs.end())
	{
		std::string PPQName;
		PPQName = (*l_Iter)->getObject()->GetCompleteName();

		l_Status = (Name == PPQName);

		if (l_Status)
		{
			m_AllOutputs.erase(l_Iter);

			break;
		}

		++l_Iter;
	}

	return l_Status;
}

bool SVPPQObject::ResolveConditionalOutput()
{
	bool bRetVal = true;

	m_conditionalOutputValueID = GUID_NULL;

	if (!AlwaysWriteOutputs())
	{
		// Get Input with this name and assign guid
		SVIOEntryHostStructPtrVector::const_iterator it = std::find_if(m_UsedInputs.begin(), m_UsedInputs.end(), FindIOEntry<std::string, CompareIOEntryNameFunc>(m_conditionalOutputName, CompareNameWithIOEntry));
		if (it != m_UsedInputs.end())
		{
			SVIOEntryHostStructPtr pIoEntry = (*it);
			if (!pIoEntry.empty() && nullptr != pIoEntry->getObject())
			{
				m_conditionalOutputValueID = pIoEntry->getObject()->GetUniqueObjectID();
				bRetVal = true;
			}
		}
		else
		{
			bRetVal = false;
		}
	}
	return bRetVal;
}

bool SVPPQObject::AlwaysWriteOutputs() const
{
	return (m_conditionalOutputName == PPQ_CONDITIONAL_OUTPUT_ALWAYS);
}

bool SVPPQObject::EvaluateConditionalOutput(long DataIndex) const
{
	bool bRetVal = AlwaysWriteOutputs();
	if (!bRetVal)
	{
		SVObjectClass* pObject = nullptr;
		HRESULT hr = SVObjectManagerClass::Instance().GetObjectByIdentifier(m_conditionalOutputValueID, pObject);
		if (S_OK == hr && nullptr != pObject)
		{
			double Value;
			hr = pObject->getValue(Value, -1, DataIndex);
			if (S_OK == hr)
			{
				bRetVal = (std::numeric_limits<double>::epsilon() < Value) ? true : false;
			}
		}
	}
	return bRetVal;
}

bool SVPPQObject::WriteOutputs(SVProductInfoStruct *pProduct)
{
	bool bRet = true;
	SVDataManagerHandle Handle;
	long DataIndex(-1);
	long TriggerCount(0);
	bool bTriggerToggle(false);
	BOOL bACK(false);
	BOOL bNAK(true);

#ifdef _DEBUG
#ifdef SHOW_PPQ_STATE
	std::string l_ProductState;
#endif
#endif

	m_OutputToggle = !m_OutputToggle;

	if (nullptr != pProduct)
	{
		TriggerCount = pProduct->ProcessCount();
		bTriggerToggle = pProduct->oTriggerInfo.m_ToggleState;

		pProduct->oTriggerInfo.m_PushedOutputs = SvTl::GetTimeStamp();

		DataIndex = pProduct->oPPQInfo.m_ResultDataDMIndexHandle.GetIndex();

		if (!(pProduct->bDataComplete))
		{
			SetProductIncomplete(*pProduct);
		}

#ifdef _DEBUG
#ifdef SHOW_PPQ_STATE
		l_ProductState = pProduct->m_ProductState;
#endif
#endif
	}

	bool bWriteOutputs = true;
	if (!AlwaysWriteOutputs())
	{
		// get value that represenst condition
		bWriteOutputs = EvaluateConditionalOutput(DataIndex);
	}

	if (0 <= DataIndex)
	{
		m_voACK.GetValue(bACK, -1, DataIndex);

		m_voNAK.GetValue(bNAK, -1, DataIndex);

		// First, write the trigger toggle output if it is okay
		if (!(m_pTriggerToggle.empty()) && nullptr != m_pTriggerToggle->getValueObject())
		{
			m_pTriggerToggle->getValueObject()->setValue(_variant_t(bTriggerToggle));
			m_pTriggerToggle->getValueObject()->CopyValue(DataIndex);
		}

		// Toggle the Output Toggle if it is okay
		if (!(m_pOutputToggle.empty()) && nullptr != m_pOutputToggle->getValueObject())
		{
			m_pOutputToggle->getValueObject()->setValue(_variant_t(m_OutputToggle));
			m_pOutputToggle->getValueObject()->CopyValue(DataIndex);
		}
	}
	else
	{
		if (S_OK == TheSVDataManager.GetNextAvailableBufferIndex(m_pResultDataCircleBuffer, SV_PPQ, Handle))
		{

			ResetOutputValueObjects(Handle.GetIndex());
		}

		// Caution! enabling the logging here will cause thread contention because
		// the tracking class is not lock-less. It needs more work before we can use it here.
#ifdef EnableTracking
		//m_PPQTracking.IncrementCount( _T( "Product Missing" ) );
#endif
	}

	if (0 <= DataIndex)
	{
		if (bWriteOutputs)
		{
			bRet = m_pOutputList->WriteOutputs(m_UsedOutputs, DataIndex, bACK ? true : false, bNAK ? true : false);

			SVConfigurationObject* pConfig(nullptr);
			SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);


			BOOL bValue = false;
			m_voDataValid.GetValue(bValue, -1, DataIndex);
			if (0 == m_DataValidDelay)
			{
				if (!m_pDataValid.empty())
				{
					m_pOutputList->WriteOutputValue(m_pDataValid, bValue);
				}
				if (!m_pOutputToggle.empty())
				{
					m_pOutputList->WriteOutputValue(m_pOutputToggle, m_OutputToggle);
				}
			}
			else
			{
				pProduct->oOutputsInfo.DataValidResult = bValue ? true : false;
				pProduct->oOutputsInfo.OutputToggleResult = m_OutputToggle;
				// Set output data valid expire time
				pProduct->oOutputsInfo.m_EndDataValidDelay = SvTl::GetTimeStamp() + pProduct->oOutputsInfo.lDataValidDelay;
				m_DataValidDelayQueue.AddTail(pProduct->ProcessCount());
			}
		}
		pProduct->oTriggerInfo.m_PushedOutputs = SvTl::GetTimeStamp();
		long l_lTime = static_cast<long>(pProduct->oTriggerInfo.m_PushedOutputs - pProduct->oTriggerInfo.m_ToggleTimeStamp);
		// Caution! enabling the logging here will cause thread contention because
		// the tracking class is not lock-less. It needs more work before we can use it here.
#ifdef EnableTracking
		//m_PPQTracking.IncrementTimeCount( _T( "Output Toggle"), l_lTime );
#endif
	}
	else
	{
		bRet = false;
	}

#ifdef _DEBUG
#ifdef SHOW_PPQ_STATE
	if (!(l_ProductState.empty()))
	{
		l_ProductState += _T("\n");
		OutputDebugString(l_ProductState.c_str());
	}
#endif
#endif

	if (bNAK)
	{
		// Caution! enabling the logging here will cause thread contention because
		// the tracking class is not lock-less. It needs more work before we can use it here.
#ifdef EnableTracking
		//m_PPQTracking.IncrementCount( _T( "Product NAK" ) );
#endif
		::InterlockedIncrement(&m_NAKCount);
		::InterlockedIncrement(&m_NewNAKCount);
	}
	else
	{
		::InterlockedExchange(&m_NewNAKCount, 0);
	}

	return bRet;
}// end WriteOutputs

bool SVPPQObject::ResetOutputs()
{
	bool l_bRet = false;

	if (!(m_pDataValid.empty()))
	{
		m_pDataValid->m_Enabled = true;
	}

	if (m_pOutputList)
	{
		l_bRet = m_pOutputList->ResetOutputs(m_UsedOutputs);
	}

	if (!(m_pDataValid.empty()))
	{
		m_pDataValid->m_Enabled = false;
	}

	return l_bRet;
}// end ResetOutputs

bool SVPPQObject::RebuildOutputList()
{
	SVIOEntryHostStructPtrVector ppNewOutputs;
	SVIOEntryHostStructPtr pOldOutput;
	SVIOEntryHostStructPtr pNewOutput;
	std::string OldName;
	std::string NewName;
	size_t iOld;
	size_t iNew;
	size_t lNewSize;

	// Make sure all the defaults are here for old configurations
	AddDefaultOutputs();

	if (m_pOutputList && m_pOutputList->FillOutputs(ppNewOutputs))
	{
		lNewSize = ppNewOutputs.size();

		m_pTriggerToggle.clear();
		m_pOutputToggle.clear();
		m_pDataValid.clear();

		for (iOld = 0; iOld < m_AllOutputs.size(); iOld++)
		{
			pOldOutput = m_AllOutputs[iOld];
			pOldOutput->m_IOId.clear();

			for (iNew = 0; iNew < lNewSize; iNew++)
			{
				pNewOutput = ppNewOutputs[iNew];

				SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(pNewOutput->m_IOId);

				NewName = pObject->GetCompleteName();
				OldName = pOldOutput->getObject()->GetCompleteName();

				if (pNewOutput->m_ObjectType == pOldOutput->m_ObjectType && NewName == OldName)
				{
					if (SvDef::SVIoObjectType == pObject->GetObjectType())
					{
						//IO object types require the complete name
						pObject->SetName(pOldOutput->getObject()->GetCompleteName().c_str());
						pNewOutput->setObject(pOldOutput->getObject());
					}
					else
					{
						//Normal object types require the name and set the owner
						pObject->SetName(pOldOutput->getObject()->GetName());
						pNewOutput->setObject(pOldOutput->getObject());
						pNewOutput->getObject()->SetObjectOwner(pOldOutput->getObject()->GetOwner());
					}

					pNewOutput->m_DeleteValueObject = false;
					pNewOutput->m_Enabled = pOldOutput->m_Enabled;
					pOldOutput->m_ObjectType = pNewOutput->m_ObjectType;
					pOldOutput->m_IOId = pNewOutput->m_IOId;
					pOldOutput->getObject()->SetObjectDepth(GetPPQLength() + g_lPPQExtraBufferSize);
					pOldOutput->getObject()->ResetObject();

					// Check for prefix of PPQ for these special signals...
					if (0 == NewName.find(_T("PPQ_")))
					{
						// Disable Trigger Toggle since it is not written with the outputs
						if (std::string::npos != NewName.find(_T("Trigger Toggle")))
						{
							m_pTriggerToggle = pNewOutput;
							pNewOutput->m_Enabled = false;
						}// end if

						// Find Output Toggle now to make it quicker later
						if (std::string::npos != NewName.find(_T("Output Toggle")))
						{
							m_pOutputToggle = pNewOutput;
							pNewOutput->m_Enabled = false;
						}// end if

						// Find Data Valid now to make it quicker later
						if (std::string::npos != NewName.find(_T("Data Valid")))
						{
							m_pDataValid = pNewOutput;
							pNewOutput->m_Enabled = false;
						}// end if
					}
					break;
				}// end if
			}// end for
		}// end for

		m_UsedOutputs = ppNewOutputs;

		return true;
	}// end if

	return false;
}// end RebuildOutputList

void SVPPQObject::AddDefaultOutputs()
{
	// Setup PPQ variables that are available as outputs
	SVIOEntryHostStructPtr pIOEntry;
	std::string Name;
	bool	bFound;
	size_t	l;

	Name = _T("Not Inspected");
	for( l = 0, bFound = false; !bFound && l < m_AllOutputs.size(); l++ )
	{
		pIOEntry = m_AllOutputs[l];

		if (Name == pIOEntry->getObject()->GetName())
		{
			bFound = true;
			break;
		}// end if
	}// end for

	m_voNotInspected.SetName(_T("Not Inspected"));
	m_voNotInspected.SetObjectOwner(this);
	m_voNotInspected.setBucketized(true);
	m_voNotInspected.SetObjectDepth(GetPPQLength() + g_lPPQExtraBufferSize);
	m_voNotInspected.ResetObject();
	m_voNotInspected.SetDefaultValue(BOOL(true), true);
	m_voNotInspected.SetValue(BOOL(true));
	m_voNotInspected.CopyValue(cPpqDefaultIndex);

	if (!bFound)
	{
		pIOEntry = new SVIOEntryHostStruct;
		pIOEntry->m_DeleteValueObject = false;
		pIOEntry->setObject(dynamic_cast<SVObjectClass*> (&m_voNotInspected));
		pIOEntry->m_ObjectType = IO_DIGITAL_OUTPUT;
		pIOEntry->m_Enabled = true;

		AddOutput(pIOEntry);
	}// end if

	m_voNAK.SetName(_T("NAK"));
	m_voNAK.SetObjectOwner(this);
	m_voNAK.setBucketized(true);
	m_voNAK.SetObjectDepth(GetPPQLength() + g_lPPQExtraBufferSize);
	m_voNAK.ResetObject();
	m_voNAK.SetDefaultValue(BOOL(true), true);
	m_voNAK.SetValue(BOOL(true));
	m_voNAK.CopyValue(cPpqDefaultIndex);

	if (!bFound)
	{
		pIOEntry = new SVIOEntryHostStruct;
		pIOEntry->m_DeleteValueObject = false;
		pIOEntry->setObject(dynamic_cast<SVObjectClass*> (&m_voNAK));
		pIOEntry->m_ObjectType = IO_DIGITAL_OUTPUT;
		pIOEntry->m_Enabled = true;

		AddOutput(pIOEntry);
	}// end if

	m_voMasterWarning.SetName(_T("Master Warning"));
	m_voMasterWarning.SetObjectOwner(this);
	m_voMasterWarning.setBucketized(true);
	m_voMasterWarning.SetObjectDepth(GetPPQLength() + g_lPPQExtraBufferSize);
	m_voMasterWarning.ResetObject();
	m_voMasterWarning.SetDefaultValue(BOOL(true), true);
	m_voMasterWarning.SetValue(BOOL(true));
	m_voMasterWarning.CopyValue(cPpqDefaultIndex);

	if (!bFound)
	{
		pIOEntry = new SVIOEntryHostStruct;
		pIOEntry->m_DeleteValueObject = false;
		pIOEntry->setObject(dynamic_cast<SVObjectClass*> (&m_voMasterWarning));
		pIOEntry->m_ObjectType = IO_DIGITAL_OUTPUT;
		pIOEntry->m_Enabled = true;

		AddOutput(pIOEntry);
	}// end if

	m_voMasterFault.SetName(_T("Master Fault"));
	m_voMasterFault.SetObjectOwner(this);
	m_voMasterFault.setBucketized(true);
	m_voMasterFault.SetObjectDepth(GetPPQLength() + g_lPPQExtraBufferSize);
	m_voMasterFault.ResetObject();
	m_voMasterFault.SetDefaultValue(BOOL(true), true);
	m_voMasterFault.SetValue(BOOL(true));
	m_voMasterFault.CopyValue(cPpqDefaultIndex);

	if (!bFound)
	{
		pIOEntry = new SVIOEntryHostStruct;
		pIOEntry->m_DeleteValueObject = false;
		pIOEntry->setObject(dynamic_cast<SVObjectClass*> (&m_voMasterFault));
		pIOEntry->m_ObjectType = IO_DIGITAL_OUTPUT;
		pIOEntry->m_Enabled = true;

		AddOutput(pIOEntry);
	}// end if

	m_voDataValid.SetName(_T("Data Valid"));
	m_voDataValid.SetObjectOwner(this);
	m_voDataValid.setBucketized(true);
	m_voDataValid.SetObjectDepth(GetPPQLength() + g_lPPQExtraBufferSize);
	m_voDataValid.ResetObject();
	m_voDataValid.SetDefaultValue(BOOL(false), true);
	m_voDataValid.SetValue(BOOL(false));
	m_voDataValid.CopyValue(cPpqDefaultIndex);

	if (!bFound)
	{
		pIOEntry = new SVIOEntryHostStruct;
		pIOEntry->m_DeleteValueObject = false;
		pIOEntry->setObject(dynamic_cast<SVObjectClass*> (&m_voDataValid));
		pIOEntry->m_ObjectType = IO_DIGITAL_OUTPUT;
		pIOEntry->m_Enabled = true;

		AddOutput(pIOEntry);
	}// end if

	m_voACK.SetName(_T("ACK"));
	m_voACK.SetObjectOwner(this);
	m_voACK.setBucketized(true);
	m_voACK.SetObjectDepth(GetPPQLength() + g_lPPQExtraBufferSize);
	m_voACK.ResetObject();
	m_voACK.SetDefaultValue(BOOL(false), true);
	m_voACK.SetValue(BOOL(false));
	m_voACK.CopyValue(cPpqDefaultIndex);

	if (!bFound)
	{
		pIOEntry = new SVIOEntryHostStruct;
		pIOEntry->m_DeleteValueObject = false;
		pIOEntry->setObject(dynamic_cast<SVObjectClass*> (&m_voACK));
		pIOEntry->m_ObjectType = IO_DIGITAL_OUTPUT;
		pIOEntry->m_Enabled = true;

		AddOutput(pIOEntry);
	}// end if

	m_voTriggerToggle.SetName(_T("Trigger Toggle"));
	m_voTriggerToggle.SetObjectOwner(this);
	m_voTriggerToggle.setBucketized(true);
	m_voTriggerToggle.SetObjectDepth(GetPPQLength() + g_lPPQExtraBufferSize);
	m_voTriggerToggle.ResetObject();
	m_voTriggerToggle.SetDefaultValue(BOOL(true), true);
	m_voTriggerToggle.SetValue(BOOL(true));
	m_voTriggerToggle.CopyValue(cPpqDefaultIndex);
	m_TriggerToggle = true;

	if (!bFound)
	{
		pIOEntry = new SVIOEntryHostStruct;
		pIOEntry->m_DeleteValueObject = false;
		pIOEntry->setObject(dynamic_cast<SVObjectClass*> (&m_voTriggerToggle));
		pIOEntry->m_ObjectType = IO_DIGITAL_OUTPUT;
		pIOEntry->m_Enabled = false;

		AddOutput(pIOEntry);
	}// end if

	m_voOutputToggle.SetName(_T("Output Toggle"));
	m_voOutputToggle.SetObjectOwner(this);
	m_voOutputToggle.setBucketized(true);
	m_voOutputToggle.SetObjectDepth(GetPPQLength() + g_lPPQExtraBufferSize);
	m_voOutputToggle.ResetObject();
	m_voOutputToggle.SetDefaultValue(BOOL(true), true);
	m_voOutputToggle.SetValue(BOOL(true));
	m_voOutputToggle.CopyValue(cPpqDefaultIndex);
	m_OutputToggle = true;

	if (!bFound)
	{
		pIOEntry = new SVIOEntryHostStruct;
		pIOEntry->m_DeleteValueObject = false;
		pIOEntry->setObject(dynamic_cast<SVObjectClass*> (&m_voOutputToggle));
		pIOEntry->m_ObjectType = IO_DIGITAL_OUTPUT;
		pIOEntry->m_Enabled = false;

		AddOutput(pIOEntry);
	}// end if

	// This value object is used as the first default PLC output.
	m_voTriggerCount.SetName(_T("Trigger Count"));
	m_voTriggerCount.SetObjectOwner(this);
	m_voTriggerCount.setBucketized(true);
	m_voTriggerCount.SetObjectDepth(GetPPQLength() + g_lPPQExtraBufferSize);
	m_voTriggerCount.ResetObject();
	m_voTriggerCount.SetDefaultValue(0L, true);
	m_voTriggerCount.SetValue(0L);
	m_voTriggerCount.CopyValue(cPpqDefaultIndex);

	BasicValueObjectPtr pPpqLength = m_PpqValues.getValueObject(SvDef::FqnPpqLength);
	SVGUID PpqLengthUid = PpqBaseLengthUidGuid;
	std::string PpqName = GetName();
	long PpqID(0);
	const size_t PpqFixedNameLength = strlen(SvO::cPpqFixedName);
	if (PpqFixedNameLength < PpqName.size())
	{
		PpqID = atol(PpqName.c_str() + PpqFixedNameLength);
		//Zero based PPQ ID, note PPQ name is one based!
		PpqID--;
	}
	//Make sure it is above 0
	if (0 <= PpqID && nullptr != pPpqLength)
	{
		PpqLengthUid.ToGUID().Data1 += PpqID;
		SVObjectManagerClass::Instance().ChangeUniqueObjectID(pPpqLength.get(), PpqLengthUid);
		pPpqLength->SetObjectOwner(this);
	}
}// end AddDefaultOutputs

void SVPPQObject::GetAllOutputs(SVIOEntryHostStructPtrVector& ppIOEntries) const
{
	ppIOEntries = m_AllOutputs;
}

SVProductInfoStruct* SVPPQObject::IndexPPQ(SvTi::SVTriggerInfoStruct& p_rTriggerInfo)
{
	SVProductInfoStruct* l_pProduct = nullptr;
	SVProductInfoStruct* l_pNewProduct = nullptr;
	SVProductInfoStruct* l_pPrevProduct = m_ppPPQPositions.GetProductAt(0);
	SVProductInfoStruct* l_pLastProduct = m_ppPPQPositions.IndexRegister();

	if (m_qAvailableProductInfos.RemoveHead(&l_pNewProduct) && nullptr != l_pNewProduct)
	{
		l_pNewProduct->oTriggerInfo = p_rTriggerInfo;

		if (nullptr != l_pPrevProduct)
		{
			l_pNewProduct->oTriggerInfo.m_PreviousTrigger = l_pPrevProduct->oTriggerInfo.m_BeginProcess;
		}

		//This is faster than SvUl::Format
		TCHAR TriggerCount[50];
		sprintf_s(TriggerCount, 50, _T("%ld"), p_rTriggerInfo.lTriggerCount);

		l_pNewProduct->bTriggered = true;

		l_pNewProduct->m_ProductState += _T("|");
		l_pNewProduct->m_ProductState += GetName();
		l_pNewProduct->m_ProductState += _T("|TRI=");
		l_pNewProduct->m_ProductState += TriggerCount;

		l_pNewProduct->SetProductActive();

		m_ppPPQPositions.SetProductAt(0, l_pNewProduct);
	}

	if (SvDef::SVPPQNextTriggerMode == m_oOutputMode)
	{
		l_pProduct = m_ppPPQPositions.GetProductAt(m_ppPPQPositions.size() - 1);
	}
	else
	{
		l_pProduct = l_pNewProduct;
	}

	// Start the Outputs. This will notify the cameras to acquire
	StartOutputs(l_pProduct);

	if (nullptr != l_pLastProduct)
	{
		// Recycle the exiting SVProductInfoStruct
		if (!RecycleProductInfo(l_pLastProduct))
		{
			SvStl::MessageMgrStd Exception(SvStl::LogOnly);
			Exception.setMessage(SVMSG_SVO_69_PPQ_INDEX_NOT_RELEASED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
		}
	}

#ifdef _DEBUG
	if (TheSVObserverApp.GetLogDataManager())
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_IP)
		std::string l_ProductStates;

		l_ProductStates += GetName();
		l_ProductStates += _T("|SVPPQObject::IndexPPQ\n");

		m_ppPPQPositions.GetProductStates(l_ProductStates);

		::OutputDebugString(l_ProductStates.c_str());
#endif
	}
#endif

	return l_pNewProduct;
}

void SVPPQObject::InitializeProduct(SVProductInfoStruct* p_pNewProduct, const SVVariantBoolVector& p_rInputValues)
{
	HRESULT hr;

	// Now we need to get the IO ready for this Product. Make sure that all locks are set
	// and that all indexes are set correctly
	// ************************************************************************
	// Begin preparing the IO
	// Get Next available indexes from THE Data Manager
	hr = GetNextAvailableIndexes(p_pNewProduct->oPPQInfo, SV_PPQ);

	// ************************************************************************
	// Now we need to get the IO ready for this Product. Make sure that all locks are set
	// and that all indexes are set correctly
	// Set all IO input objects for this product to use the new input data index

	// Reset all IO output objects for this product to use the new output data index
	ResetOutputValueObjects(p_pNewProduct->oPPQInfo.m_ResultDataDMIndexHandle.GetIndex());

	// End preparing the IO
	// ************************************************************************


	// ************************************************************************
	// Now we need to read the inputs. Make sure that we record the timestamps
	// Begin reading the inputs
	p_pNewProduct->oInputsInfo.m_BeginProcess = SvTl::GetTimeStamp();

	if (!AssignInputs(p_rInputValues))
	{
		p_pNewProduct->hrPPQStatus = -12387;
	}

	p_pNewProduct->oInputsInfo.m_EndProcess = SvTl::GetTimeStamp();

	// End reading the inputs
	// ************************************************************************
}// end IndexPPQ


// p_Offset represents PPQ position of product.
HRESULT SVPPQObject::NotifyInspections(long p_Offset)
{
	HRESULT l_Status = S_OK;

	// ************************************************************************
	// Now we need to check if any inspections were waiting on inputs read after
	// the first position. Only check the products at positions that have a input set

	// Begin checking inspections to start processing
	if (0 <= p_Offset && static_cast<size_t>(p_Offset) < m_ppPPQPositions.size())
	{
		SVProductInfoStruct* pTempProduct = m_ppPPQPositions.GetProductAt(p_Offset);

		// See if the Inspection Processes can inspect this product
		int iSize = static_cast<int> (m_arInspections.size());

		bool l_Start = true;

		for (int i = 0; i < iSize; i++)
		{
			SVInspectionInfoStruct& l_rInfo = pTempProduct->m_svInspectionInfos[m_arInspections[i]->GetUniqueObjectID()];
			bool tmpIsProductActive = pTempProduct->IsProductActive();

			// if product has not been previously determined that it "can 
			// process".  That would generally mean that it's been here 
			// before, and should not be able to happen.
			if (!(l_rInfo.m_CanProcess) && !(l_rInfo.m_InProcess) && tmpIsProductActive)
			{
				// Tests if we have all required inputs.  Includes assigned 
				// discrete inputs and camera images.
				// If images or inputs are missing, CanProcess will return 
				// FALSE(0).  m_CanProcess will also equal false.
				l_rInfo.m_CanProcess = m_arInspections[i]->CanProcess(pTempProduct);

				if (l_rInfo.m_CanProcess)
				{
					if (pTempProduct->IsProductActive())
					{
						m_ProcessInspectionsSet.insert(m_arInspections[i]->GetUniqueObjectID());

						pTempProduct->m_ProductState += _T("|CP=");
						pTempProduct->m_ProductState += m_arInspections[i]->GetName();
#ifdef EnableTracking
						std::string l_Title = m_arInspections[i]->GetName();

						l_Title += _T(" CP");

						m_PPQTracking.IncrementCount(l_Title, p_Offset);
#endif
					}
				} //	if( l_rInfo.m_CanProcess )
				else
				{
					// If it can not process, this still does not change the 
					// l_Status, so the returned status will still be S_OK
					// which indicates NOT "Processed".
					l_Status = S_FALSE; // 1
				}
			} // if( !( l_rInfo.m_CanProcess ) && !( l_rInfo.m_InProcess ) && pTempProduct->IsProductActive() )
		}
	}
	else
	{
		l_Status = E_FAIL;
	}
	// Now we need to check if any inspections were waiting on inputs read after
	// the first position. Only check the products at positions that have a input set
	// End checking inspections to start processing
	// ************************************************************************

	return l_Status;
}

HRESULT SVPPQObject::StartInspection(const SVGUID& p_rInspectionID)
{
#ifdef _DEBUG_PERFORMANCE_INFO //Arvid 160212 this is helpful for debugging the creation of Performance Information
	std::string infostring = SvUl::Format(_T("!\t.%7.1lf: SVPPQObject::StartInspection(%s)\n"), SvTl::GetRelTimeStamp(), p_rInspectionID.ToString().c_str());
	::OutputDebugString(infostring.c_str());
#endif

	HRESULT l_Status = S_OK;

	SVProductInfoStruct* l_pProduct = nullptr;

	size_t l_ProductIndex = m_ppPPQPositions.size();
	size_t l_Count = m_ppPPQPositions.size();

	switch (m_NAKMode)
	{
	case SvDef::Legacy: ///old Behavior 
		if (0 < m_NAKCount)
		{
			long l_NAKCount = m_NAKCount;
			if ((l_NAKCount + 2) < static_cast<long>(l_Count))
			{
				l_Count -= l_NAKCount;
			}
			else
			{
				l_Count = std::min< size_t >(2, m_ppPPQPositions.size());
			}
		}
		break;
	case SvDef::Bursts:
		if (m_FirstNAKProcessCount && !IsProductAlive(m_FirstNAKProcessCount))
		{
			m_FirstNAKProcessCount = 0;
		}
		break;

	case SvDef::RepairedLegacy:
		if (0 < m_NewNAKCount)
		{
			long l_NAKCount = m_NewNAKCount;
			if ((l_NAKCount + m_ReducedPPQPosition) < static_cast<long>(l_Count))
			{
				l_Count -= l_NAKCount;
			}
			else
			{
				l_Count = std::min< size_t >(m_ReducedPPQPosition, m_ppPPQPositions.size());
			}
		}
		break;

	case SvDef::FixedMaximum:
		l_Count = m_ReducedPPQPosition;
		break;


	}

	// Begin checking inspections to start processing
	for (size_t i = 0; i < l_Count; ++i)
	{
		SVProductInfoStruct* pTempProduct = m_ppPPQPositions.GetProductAt(i);

		if (nullptr != pTempProduct)
		{
			if (pTempProduct->IsProductActive())
			{
				SVInspectionInfoStruct& l_rInfo = pTempProduct->m_svInspectionInfos[p_rInspectionID];

				if (l_rInfo.m_CanProcess &&				// all inputs are available and inspection can start
					!(l_rInfo.m_InProcess) &&			// inspection is not currently running
					!(l_rInfo.m_HasBeenQueued))		// This flag prevents the inspection from getting queued more than once
				{
					l_pProduct = pTempProduct; // product info
					l_ProductIndex = i;
					if (SvDef::Bursts == m_NAKMode)
					{
						if ((m_NewNAKCount > 2 && m_FirstNAKProcessCount == 0))
						{
							m_FirstNAKProcessCount = pTempProduct->ProcessCount();
							break;
						}
						else if ((m_FirstNAKProcessCount > pTempProduct->ProcessCount()))
						{
							break;
						}
					}
				}
			}
			else
			{
				break;
			}
		}
	}

	if (nullptr != l_pProduct && nullptr != l_pProduct->m_svInspectionInfos[p_rInspectionID].pInspection)
	{

		l_pProduct->m_WorkloadInfo.m_ProcessingStartTime = SvTl::GetTimeStamp(); //ProductWorkloadInformation may be incorrect if there are several inspections per product
#ifdef _DEBUG_PERFORMANCE_INFO //Arvid 160212 this is helpful for debugging the creation of Performance Information

		std::string infostring = SvUl::Format(_T("set m_ProcessingStartTime, trID = %ld"), l_pProduct->ProcessCount());
		logWorkloadInformation(l_pProduct->m_WorkloadInfo, infostring.c_str());
#endif

		l_Status = l_pProduct->m_svInspectionInfos[p_rInspectionID].pInspection->StartProcess(l_pProduct);

#ifdef EnableTracking
		std::string l_Title = l_pProduct->m_svInspectionInfos[p_rInspectionID].pInspection->GetName();
		l_Title += _T(" Start");
		m_PPQTracking.IncrementCount(l_Title, l_ProductIndex);
#endif

#ifdef _DEBUG
		if (TheSVObserverApp.GetLogDataManager())
		{
			std::string l_ProductStates;

			l_ProductStates += GetName();
			l_ProductStates += _T("|SVPPQObject::StartInspection\n");

			m_ppPPQPositions.GetProductStates(l_ProductStates);

#if defined (TRACE_THEM_ALL) || defined (TRACE_PPQ)
			::OutputDebugString(l_ProductStates.c_str());
#endif
		}
#endif
	}

	return l_Status;
}

bool SVPPQObject::StartOutputs(SVProductInfoStruct* p_pProduct)
{
	if (p_pProduct)
	{
		p_pProduct->oOutputsInfo.m_BeginProcess = SvTl::GetTimeStamp();
		p_pProduct->oOutputsInfo.lOutputDelay = m_lOutputDelay;
		p_pProduct->oOutputsInfo.lResetDelay = m_lResetDelay;
		p_pProduct->oOutputsInfo.lDataValidDelay = m_DataValidDelay;
		p_pProduct->oOutputsInfo.m_EndOutputDelay = SvTl::GetMinTimeStamp();
		p_pProduct->oOutputsInfo.m_EndResetDelay = SvTl::GetMinTimeStamp();
		p_pProduct->oOutputsInfo.m_EndDataValidDelay = SvTl::GetMinTimeStamp();

		switch (m_oOutputMode)
		{
		case SvDef::SVPPQNextTriggerMode:
		{
			// The trigger has come, time to write the outputs
			WriteOutputs(p_pProduct);

			p_pProduct->oOutputsInfo.m_EndProcess = SvTl::GetTimeStamp();

			// Check if we should fire up the reset outputs thread
			if (0 < m_lResetDelay)
			{
				// Set output reset expire time
				p_pProduct->oOutputsInfo.m_EndResetDelay = p_pProduct->oOutputsInfo.m_EndProcess +
					p_pProduct->oOutputsInfo.lResetDelay;

				m_oOutputsResetQueue.AddTail(p_pProduct->ProcessCount());
			}

			SetProductComplete(*p_pProduct);

			break;
		}
		case SvDef::SVPPQTimeDelayMode:
		case SvDef::SVPPQTimeDelayAndDataCompleteMode:
		case SvDef::SVPPQExtendedTimeDelayMode:
		case SvDef::SVPPQExtendedTimeDelayAndDataCompleteMode:
		{
			SvTl::SVTimeStamp l_Offset = p_pProduct->oOutputsInfo.lOutputDelay;

			// Set output delay expire time
			p_pProduct->oOutputsInfo.m_EndOutputDelay = p_pProduct->oTriggerInfo.m_ToggleTimeStamp + l_Offset;

			if (p_pProduct->oOutputsInfo.m_BeginProcess < p_pProduct->oOutputsInfo.m_EndOutputDelay)
			{
				m_oOutputsDelayQueue.AddTail(p_pProduct->ProcessCount());
			}

			break;
		}
		default:
			DWORD dw = GetLastError();
			return false;
		}// end switch

		// Now we need to write the inputs. Make sure that we record the timestamps
		// ************************************************************************
		// End writing the outputs
	}

	return true;
}// end StartOutputs

bool SVPPQObject::AddResultsToPPQ(long p_PPQIndex)
{
	SVProductInfoStruct* pProduct = m_ppPPQPositions.GetProductAt(p_PPQIndex);

	bool bValid = (nullptr != pProduct);

	if (bValid)
	{
		long oState = 0x0000;

		SVGUIDSVInspectionInfoStructMap::iterator l_Iter = pProduct->m_svInspectionInfos.begin();

		while (l_Iter != pProduct->m_svInspectionInfos.end())
		{
			oState |= l_Iter->second.oInspectedState;

			++l_Iter;
		}// end for


		// Set the value objects depending on the inspection state result
		if (PRODUCT_INSPECTION_NOT_RUN & oState)
		{
			if (PRODUCT_NOT_INSPECTED & oState)
			{
				m_voACK.SetValue(BOOL(false));
				m_voNAK.SetValue(BOOL(true));
				m_voMasterFault.SetValue(BOOL(true));
				m_voMasterWarning.SetValue(BOOL(true));
				m_voNotInspected.SetValue(BOOL(true));

				pProduct->m_ProductState += _T("|NI");
			}// end if
			else if (PRODUCT_NOT_ACKNOWLEDGED & oState)
			{
				m_voACK.SetValue(BOOL(false));
				m_voNAK.SetValue(BOOL(true));
				m_voMasterFault.SetValue(BOOL(true));
				m_voMasterWarning.SetValue(BOOL(true));
				m_voNotInspected.SetValue(BOOL(true));

				pProduct->m_ProductState += _T("|NAK");
			}// end else if
			else if (PRODUCT_INSPECTION_DISABLED & oState)
			{
				m_voACK.SetValue(BOOL(true));
				m_voNAK.SetValue(BOOL(false));
				m_voMasterFault.SetValue(BOOL(true));
				m_voMasterWarning.SetValue(BOOL(true));
				m_voNotInspected.SetValue(BOOL(true));

				pProduct->m_ProductState += _T("|DISABLED");
			}// end else if
			else
			{
				m_voACK.SetValue(BOOL(false));
				m_voNAK.SetValue(BOOL(true));
				m_voMasterFault.SetValue(BOOL(true));
				m_voMasterWarning.SetValue(BOOL(true));
				m_voNotInspected.SetValue(BOOL(true));

				pProduct->m_ProductState += _T("|E_UNKNOWN");
			}// end else
		}// end if
		else
		{
			if (PRODUCT_INSPECTION_FAILED & oState)
			{
				m_voACK.SetValue(BOOL(true));
				m_voNAK.SetValue(BOOL(false));
				m_voMasterFault.SetValue(BOOL(true));
				m_voMasterWarning.SetValue(BOOL(true));
				m_voNotInspected.SetValue(BOOL(false));

				pProduct->m_ProductState += _T("|FAILED");
			}// end if
			else if (PRODUCT_INSPECTION_WARNING & oState)
			{
				m_voACK.SetValue(BOOL(true));
				m_voNAK.SetValue(BOOL(false));
				m_voMasterFault.SetValue(BOOL(false));
				m_voMasterWarning.SetValue(BOOL(true));
				m_voNotInspected.SetValue(BOOL(false));

				pProduct->m_ProductState += _T("|WARNING");
			}// end else if
			else if (PRODUCT_INSPECTION_PASSED & oState)
			{
				m_voACK.SetValue(BOOL(true));
				m_voNAK.SetValue(BOOL(false));
				m_voMasterFault.SetValue(BOOL(false));
				m_voMasterWarning.SetValue(BOOL(false));
				m_voNotInspected.SetValue(BOOL(false));

				pProduct->m_ProductState += _T("|PASSED");
			}// end else if
			else
			{
				m_voACK.SetValue(BOOL(false));
				m_voNAK.SetValue(BOOL(true));
				m_voMasterFault.SetValue(BOOL(true));
				m_voMasterWarning.SetValue(BOOL(true));
				m_voNotInspected.SetValue(BOOL(true));

				pProduct->m_ProductState += _T("|S_UNKNOWN");
			}// end else
		}// end else
		long lIndex = pProduct->oPPQInfo.m_ResultDataDMIndexHandle.GetIndex();
		m_voACK.CopyValue(lIndex);
		m_voNAK.CopyValue(lIndex);
		m_voMasterFault.CopyValue(lIndex);
		m_voMasterWarning.CopyValue(lIndex);
		m_voNotInspected.CopyValue(lIndex);

		SVGUIDSVInspectionInfoStructMap::iterator l_InspectIter = pProduct->m_svInspectionInfos.begin();

		while (l_InspectIter != pProduct->m_svInspectionInfos.end())
		{
			bValid &= (l_InspectIter->second.m_EndInspection > 0);

			++l_InspectIter;
		}// end for

		// Set the Data Valid to true
		m_voDataValid.SetValue(bValid);
		m_voDataValid.CopyValue(lIndex);
	}

	return true;
}// end AddResultsToPPQ

bool SVPPQObject::SetInspectionComplete(long p_PPQIndex)
{
	SVProductInfoStruct* pProduct = m_ppPPQPositions.GetProductAt(p_PPQIndex);
	bool bValid = (nullptr != pProduct);

	if (bValid)
	{
		SVGUIDSVInspectionInfoStructMap::iterator l_Iter = pProduct->m_svInspectionInfos.begin();

		while (l_Iter != pProduct->m_svInspectionInfos.end())
		{
			bool l_Complete = (l_Iter->second.m_EndInspection > 0);

			bValid &= l_Complete;

			++l_Iter;
		}
	}

	if (bValid)
	{
		// Currently only used for Remote Outputs and Fail Status Stream.
		// returns E_FAIL when there are no listeners/observers.  Not having 
		// Remote Outputs or Fail Status is not an error in this case.
		SVObjectManagerClass::Instance().UpdateObservers(std::string(SvO::cPPQObjectTag), GetUniqueObjectID(), *pProduct);

	}

	if (bValid)
	{
		bValid = AddResultsToPPQ(p_PPQIndex);
	}

	//Only place that could set bDataComplete to true
	pProduct->bDataComplete = bValid;

	if (pProduct->bDataComplete)
	{
		if ((SvDef::SVPPQTimeDelayAndDataCompleteMode == m_oOutputMode) ||
			(SvDef::SVPPQExtendedTimeDelayAndDataCompleteMode == m_oOutputMode))
		{
			NotifyProcessTimerOutputs();
		}
	}

	return bValid;
}

bool SVPPQObject::SetProductComplete(long p_PPQIndex)
{
	SVProductInfoStruct* pProduct = m_ppPPQPositions.GetProductAt(p_PPQIndex);

	bool l_Status = false;

	if (nullptr != pProduct)
	{
		// record the current time and PPQ position for later display
		pProduct->m_WorkloadInfo.m_CompletionTime = SvTl::GetTimeStamp();
		pProduct->m_WorkloadInfo.m_PPQIndexAtCompletion = p_PPQIndex;

		pProduct->m_WorkloadInfo.m_TriggerTime = pProduct->oTriggerInfo.m_BeginProcess;

#ifdef _DEBUG_PERFORMANCE_INFO //Arvid 160212 this is helpful for debugging the creation of Performance Information
		std::string infostring = SvUl::Format(_T("SVPPQObject::SetProductComplete(@ppq: %d) >> m_CompletionTime\n\t\t(oTriggerInfo.m_BeginProcess -> m_TriggerTime)"), p_PPQIndex);
		logWorkloadInformation(pProduct->m_WorkloadInfo, infostring.c_str());
#endif

		m_MostRecentWorkLoadInfo = pProduct->GetWorkloadInformation();

		CommitSharedMemory(*pProduct);
		l_Status = SetProductComplete(*pProduct);


#ifdef EnableTracking
		m_PPQTracking.IncrementCount(_T("Product Complete"), p_PPQIndex);
#endif
	}

	return l_Status;
}

bool SVPPQObject::SetProductComplete(SVProductInfoStruct& p_rProduct)
{
	bool l_Status = true;

	///@TODO[mec, 26.07.2016]  Normaly this is a bug and SVPPQObject::IsProductAlive  should  be used this code exist only to 
	/// have the possibility to switch with nakmode M_Old to the old behavior and should be corrected or removed 
	// if this is not longer necessary 
	if (m_ppPPQPositions.IsProductAlive(p_rProduct.ProcessCount()))
	{
		::InterlockedExchange(&m_NAKCount, 0);
	}

	bool bMaintainImages;

	// if bInspecting is false, this loop will fall through
	// if we are maintaining source images it will also fall through
	// we don't want to release all the source images unless all the IPs have started
	GetMaintainSourceImages(bMaintainImages);

	if (!bMaintainImages)
	{
		SVGuidSVCameraInfoStructMap::iterator Iter(p_rProduct.m_svCameraInfos.begin());

		for (; p_rProduct.m_svCameraInfos.end() != Iter; ++Iter)
		{
			Iter->second.ClearCameraInfo();
		}

		p_rProduct.oPPQInfo.m_ResultImagePublishedDMIndexHandle.clear();
	}

	p_rProduct.SetProductComplete();

	p_rProduct.m_ProductState += _T("|COMPLETE");

	return l_Status;
}

bool SVPPQObject::SetProductIncomplete(long p_PPQIndex)
{
	SVProductInfoStruct* pProduct = m_ppPPQPositions.GetProductAt(p_PPQIndex);
	bool l_Status = (nullptr != pProduct);

	if (l_Status)
	{
		l_Status = SetProductIncomplete(*pProduct);

#ifdef EnableTracking
		m_PPQTracking.IncrementCount(_T("Product Incomplete"), p_PPQIndex);
#endif
	}

	return l_Status;
}

bool SVPPQObject::SetProductIncomplete(SVProductInfoStruct& p_rProduct)
{
	bool l_Status = true;
	// Release from Shared Memory
	ReleaseSharedMemory(p_rProduct);

	SVGuidSVCameraInfoStructMap::iterator Iter(p_rProduct.m_svCameraInfos.begin());

	for (; Iter != p_rProduct.m_svCameraInfos.end(); ++Iter)
	{
		Iter->second.ClearCameraInfo();
	}

	p_rProduct.oPPQInfo.m_ResultImagePublishedDMIndexHandle.clear();

	p_rProduct.SetProductComplete();

	p_rProduct.m_ProductState += _T("|INCOMPLETE");

	return l_Status;
}

bool SVPPQObject::RecycleProductInfo(SVProductInfoStruct *pProduct)
{
	bool l_Status = false;

	if (nullptr != pProduct)
	{
		if (-1 < pProduct->oPPQInfo.m_ResultDataDMIndexHandle.GetIndex())
		{
			ResetOutputValueObjects(pProduct->oPPQInfo.m_ResultDataDMIndexHandle.GetIndex());
		}

		pProduct->InitProductInfo();
	}

	l_Status = m_qAvailableProductInfos.AddTail(pProduct);

	return l_Status;
}// end RecycleProductInfo

HRESULT SVPPQObject::ProcessCameraResponse(const SVCameraQueueElement& p_rElement)
/// Either gets the appropriate product information (if available) or increments the pending image indicator.
/// If product information is available: 
/// Adds the process count to the cameras queue and signals the asynchronous procedure
{
	HRESULT l_Status = S_OK;

	if ((nullptr != p_rElement.m_pCamera) && (0 <= p_rElement.m_Data.mDMHandle.GetIndex()))
	{
		SvTl::SVTimeStamp l_StartTick = 0;
		size_t l_CameraPositionOnPPQ = -1;
		size_t l_ProductIndex = -1;
		SVProductInfoStruct* l_pProduct = nullptr;
		SVCameraInfoMap::iterator l_svIter;
		long	ppqSize = static_cast<long> (m_ppPPQPositions.size());

		l_svIter = m_Cameras.find(p_rElement.m_pCamera);

		if (l_svIter != m_Cameras.end())
		{
			l_CameraPositionOnPPQ = l_svIter->second.m_CameraPPQIndex;
		}

		p_rElement.m_Data.GetStartTick(l_StartTick);

		if (l_CameraPositionOnPPQ < ppqSize)
		{
			long l_Position = -1; // gets initialized to ppq size (default) by 
			// GetProductIndex (). 

			// Attempts for find the trigger (which may not have happened 
			// yet) that correlates to this image, based on image time stamp.
			GetProductIndex(l_Position, l_StartTick);

			// If trigger has not occurred yet, l_Position will equal 0.
			if (0 <= l_Position && l_Position < ppqSize)
			{
				size_t l_Index = l_Position + l_CameraPositionOnPPQ;

				if (l_Index < ppqSize)
				{
					l_ProductIndex = l_Index;
					l_pProduct = m_ppPPQPositions.GetProductAt(l_Index);
				}
			}
			else if (l_Position < 0) // l_Position can never be less than 0??
			{
				m_PendingCameraResponses[p_rElement.m_pCamera] = p_rElement;

				SVObjectManagerClass::Instance().IncrementPendingImageIndicator();
#ifdef EnableTracking

				std::string l_Title = _T("Pending ");

				l_Title += p_rElement.m_pCamera->GetName();

				m_PPQTracking.IncrementCount(l_Title);
#endif
			}
			else// if l_Position > than PPQ size.
			{
				l_Status = E_FAIL;
			}
		}
		else
		{
			l_Status = E_FAIL;
		}

		if (nullptr != l_pProduct)
		{
			SVGuidSVCameraInfoStructMap::iterator IterCamera;

			IterCamera = l_pProduct->m_svCameraInfos.find(p_rElement.m_pCamera->GetUniqueObjectID());

			if (IterCamera != l_pProduct->m_svCameraInfos.end())
			{

				SvTl::SVTimeStamp	priorCameraSF = IterCamera->second.m_StartFrameTimeStamp;
				long					productComplete = l_pProduct->bDataComplete;

				// Attempting to make sure we don't have the previous trigger 
				// count where the image has already been assigned (and 
				// possibly inspected).  This scenario occurs when the Camera
				// Response Queue notification arrives before the Trigger 
				// Queue notification.
				if ((priorCameraSF == 0) && (1 != productComplete))
				{
					SvTl::SVTimeStamp iEF = 0;

					p_rElement.m_Data.GetEndTick(iEF);

					if (p_rElement.m_Data.IsComplete())
					{
						IterCamera->second.Assign(l_StartTick, iEF, p_rElement.m_Data.mDMHandle, SV_PPQ);
					}
					else
					{
						IterCamera->second.ClearInfo();
					}

					l_pProduct->m_ProductState += _T("|");
					l_pProduct->m_ProductState += p_rElement.m_pCamera->GetName();

					if (IterCamera->second.GetIndex() < 0)
					{
						l_pProduct->m_ProductState += _T("=NAK");

#ifdef EnableTracking
						std::string l_Title = p_rElement.m_pCamera->GetName();

						l_Title += _T(" Index NAK");

						m_PPQTracking.IncrementCount(l_Title);
						m_PPQTracking.IncrementCount(l_Title, l_ProductIndex);
#endif
					}
					else
					{
#ifdef EnableTracking
						m_PPQTracking.IncrementCount(p_rElement.m_pCamera->GetName(), l_ProductIndex);
#endif
					}

					for (size_t i = (l_ProductIndex + 1); i < m_ppPPQPositions.size(); ++i)
					{
						SVProductInfoStruct* l_pAcqProduct = m_ppPPQPositions.GetProductAt(i);

						if (nullptr != l_pAcqProduct)
						{
							SVGuidSVCameraInfoStructMap::iterator IterCamera2(l_pAcqProduct->m_svCameraInfos.find(p_rElement.m_pCamera->GetUniqueObjectID()));

							if (IterCamera2 != l_pAcqProduct->m_svCameraInfos.end())
							{
								if (IterCamera2->second.m_CallbackTimeStamp != 0)
								{
									break;
								}
								IterCamera2->second.m_CallbackTimeStamp = SvTl::GetTimeStamp();
							}
#ifdef EnableTracking
							std::string l_Title = p_rElement.m_pCamera->GetName();

							l_Title += _T(" NAK");

							m_PPQTracking.IncrementCount(l_Title);
							m_PPQTracking.IncrementCount(l_Title, l_ProductIndex);
#endif

							MarkProductInspectionsMissingAcquisiton(*l_pAcqProduct, p_rElement.m_pCamera);
						}
					}

					IterCamera->second.m_CallbackTimeStamp = SvTl::GetTimeStamp();

					if (m_oCamerasQueue.AddTail(l_pProduct->ProcessCount()))
					{
						m_AsyncProcedure.Signal(nullptr);
					}
					else
					{
						l_Status = E_FAIL;
					}
				} // if (( priorCameraSF == 0 ) && (1 != productComplete))
				else
				{

					// trying to use the wrong Trigger Count.  
					// Trigger even for this camera image has not 
					// occurred yet.
					m_PendingCameraResponses[p_rElement.m_pCamera] = p_rElement;

					SVObjectManagerClass::Instance().IncrementPendingImageIndicator();

#ifdef EnableTracking
					std::string l_Title = _T("Pending ");
					l_Title += p_rElement.m_pCamera->GetName();
					m_PPQTracking.IncrementCount(l_Title);
#endif
				}
			} // if( l_svIter1 != l_pProduct->m_svCameraInfos.end() )
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

HRESULT SVPPQObject::BuildCameraInfos(SVGuidSVCameraInfoStructMap& p_rCameraInfos) const
{
	HRESULT l_Status = S_OK;

	p_rCameraInfos.clear();

	SVCameraInfoMap::const_iterator Iter(m_Cameras.begin());

	for (; m_Cameras.end() != Iter; ++Iter)
	{
		if (-1 != Iter->second.m_CameraPPQIndex)
		{
			const SVGUID& rCameraID(Iter->first->GetUniqueObjectID());
			p_rCameraInfos[rCameraID].setCamera(rCameraID, boost::bind(&SVVirtualCamera::ReserveNextImageHandleIndex, Iter->first, _1, _2));
		}
	}

	return l_Status;
}

bool SVPPQObject::FinishCamera(void *pCaller, SVODataResponseClass *pResponse)
{
	bool l_Status = (m_bOnline && (nullptr != pCaller) && (nullptr != pResponse) && (0 <= pResponse->mDMHandle.GetIndex()));

	if (l_Status)
	{
		SVVirtualCamera* pCamera = reinterpret_cast<SVVirtualCamera*>(pCaller);

		if (nullptr != pCamera && m_CameraResponseQueue.AddTail(SVCameraQueueElement(pCamera, *pResponse)))
		{
			m_AsyncProcedure.Signal(nullptr);
		}
	}

	return l_Status;
}

bool SVPPQObject::FinishTrigger(void *pCaller, SvTi::SVTriggerInfoStruct& p_rTriggerInfo)
{
	bool l_Status = m_bOnline;

	if (l_Status)
	{
		SVTriggerQueueElement l_TriggerInfo;

		l_TriggerInfo.m_TriggerInfo = p_rTriggerInfo;

		l_TriggerInfo.m_TriggerInfo.m_CallbackReceived = SvTl::GetTimeStamp();

		GetInputIOValues(l_TriggerInfo.m_Inputs);

		switch (m_oOutputMode)
		{
		case SvDef::SVPPQTimeDelayMode:
		case SvDef::SVPPQTimeDelayAndDataCompleteMode:
		{
			ResetOutputs();
			break;
		}
		}

		m_TriggerToggle = !m_TriggerToggle;

		if (!(m_pTriggerToggle.empty()) && nullptr != m_pTriggerToggle->getObject())
		{
			m_pOutputList->WriteOutputValue(m_pTriggerToggle, m_TriggerToggle);
		}

		l_TriggerInfo.m_TriggerInfo.m_ToggleState = m_TriggerToggle;
		l_TriggerInfo.m_TriggerInfo.m_ToggleTimeStamp = SvTl::GetTimeStamp();

		// Caution! enabling the logging here will cause thread contention because
		// the tracking class is not lock-less. It needs more work before we can use it here.
#ifdef EnableTracking
		//long l_lTime = static_cast<long>( l_TriggerInfo.m_TriggerInfo.m_ToggleTimeStamp - l_TriggerInfo.m_TriggerInfo.m_BeginProcess );
		//m_PPQTracking.IncrementTimeCount( _T( "Trigger Toggle"), l_lTime );
#endif

		m_oTriggerQueue.PushTail(l_TriggerInfo);

		m_AsyncProcedure.Signal(nullptr);
	}

	return l_Status;
}

bool SVPPQObject::IsProductAlive(long p_ProductCount) const
{
	bool l_Status = (S_OK == m_ppPPQPositions.IsProductAlive(p_ProductCount));

	return l_Status;
}

HRESULT SVPPQObject::GetNextAvailableIndexes(SVPPQInfoStruct& p_rPPQInfo, SVDataManagerLockTypeEnum p_LockType) const
{
	HRESULT l_Status = S_OK;
	HRESULT l_Temp = S_OK;

	l_Temp = TheSVDataManager.GetNextAvailableBufferIndex(m_pResultDataCircleBuffer, p_LockType, p_rPPQInfo.m_ResultDataDMIndexHandle);

	if (S_OK == l_Status)
	{
		l_Status = l_Temp;
	}

	l_Temp = TheSVDataManager.GetNextAvailableBufferIndex(m_pResultImagePublishedCircleBuffer, p_LockType, p_rPPQInfo.m_ResultImagePublishedDMIndexHandle);

	if (S_OK == l_Status)
	{
		l_Status = l_Temp;
	}

	return l_Status;
}

HRESULT SVPPQObject::GetProductIndex(long& p_rIndex, long lProcessCount) const
{
	HRESULT l_Status = m_ppPPQPositions.GetIndexByTriggerCount(p_rIndex, lProcessCount);

	return l_Status;
}

HRESULT SVPPQObject::GetProductIndex(long& p_rIndex, SvTl::SVTimeStamp p_TimeStamp) const
{
	HRESULT l_Status = S_OK;

	bool l_SkipUpperThreshold = false;

	l_SkipUpperThreshold = l_SkipUpperThreshold || (GetPPQLength() == 1);
	l_SkipUpperThreshold = l_SkipUpperThreshold || (m_oOutputMode == SvDef::SVPPQExtendedTimeDelayAndDataCompleteMode);
	l_SkipUpperThreshold = l_SkipUpperThreshold || ((m_oOutputMode == SvDef::SVPPQNextTriggerMode) && (GetPPQLength() == 2));

	if (l_SkipUpperThreshold)
	{
		l_Status = m_ppPPQPositions.GetIndexByTriggerTimeStamp(p_rIndex, p_TimeStamp, -2);
	}
	else
	{
		l_Status = m_ppPPQPositions.GetIndexByTriggerTimeStamp(p_rIndex, p_TimeStamp, -2, 73);
	}

	return l_Status;
}

HRESULT SVPPQObject::GetProductInfoStruct(SVProductInfoStruct*& p_rpProduct, long lProcessCount) const
{
	HRESULT l_Status = m_ppPPQPositions.GetProductByTriggerCount(p_rpProduct, lProcessCount);

	return l_Status;
}

HRESULT SVPPQObject::GetProductInfoStruct(SVProductInfoStruct*& p_rpProduct, SvTl::SVTimeStamp p_TimeStamp) const
{
	HRESULT l_Status = S_OK;
	long l_Index = -1;

	p_rpProduct = nullptr;

	l_Status = GetProductIndex(l_Index, p_TimeStamp);

	if (0 <= l_Index && l_Index < static_cast<long>(m_ppPPQPositions.size()))
	{
		p_rpProduct = m_ppPPQPositions.GetProductAt(l_Index);
	}
	else
	{
		if (S_OK == l_Status)
		{
			l_Status = S_FALSE;
		}
	}

	return l_Status;
}

bool SVPPQObject::ReserveNextRunOnceProductInfoStruct(SVProductInfoStruct& p_rsvProduct, SVDataManagerLockTypeEnum p_LockType)
{
	bool l_bOk = true;

	SVProductInfoStruct l_svProduct;

	l_svProduct.oPPQInfo.pPPQ = this;

	l_svProduct.oTriggerInfo.pTrigger = m_pTrigger;

	BuildCameraInfos(l_svProduct.m_svCameraInfos);

	l_svProduct.m_svInspectionInfos.clear();

	for (auto pInspection : m_arInspections)
	{
		SVInspectionInfoStruct l_svInspectionStruct;

		l_svInspectionStruct.pInspection = pInspection;

		l_svProduct.m_svInspectionInfos[pInspection->GetUniqueObjectID()] = l_svInspectionStruct;
	}

	l_bOk = (S_OK == l_svProduct.GetNextAvailableIndexes(p_LockType));

	if (l_bOk)
	{
		p_rsvProduct = l_svProduct;
	}

	return l_bOk;
}

bool SVPPQObject::IsObjectInPPQ(const SVObjectClass& object) const
{
	bool retValue = false;
	const SVObjectClass *inspectObject = object.GetAncestor(SvDef::SVInspectionObjectType);

	if (nullptr != inspectObject)
	{
		for (SVInspectionProcessVector::const_iterator it = m_arInspections.begin(); it != m_arInspections.end(); ++it)
		{
			if (inspectObject == (*it))
			{
				return true;
			}
		}
	}

	return retValue;
}

bool SVPPQObject::IsProductExpired(const SVProductInfoStruct* pProduct) const
{
	if (SvDef::SVPPQExtendedTimeDelayMode == m_oOutputMode && 0 < m_lInspectionTimeoutMillisec)
	{
		SvTl::SVTimeStamp l_CurrentTimestamp = SvTl::GetTimeStamp();
		SvTl::SVTimeStamp l_ProductTimestamp = pProduct->oTriggerInfo.m_BeginProcess;
		return ((l_CurrentTimestamp - l_ProductTimestamp) > static_cast<double>(m_lOutputDelay - m_lInspectionTimeoutMillisec));
	}
	else
	{
		return false;
	}
}

void SVPPQObject::DumpDMInfo(LPCTSTR p_szName) const
{
	// Camera Info Struct - Source Image Circle Buffer Just the first Camera....
	SVCameraInfoMap::const_iterator l_Iter;

	l_Iter = m_Cameras.begin();

	while (l_Iter != m_Cameras.end())
	{
		l_Iter->first->DumpDMInfo(p_szName);

		++l_Iter;
	}

	if (!m_pResultDataCircleBuffer.empty())
	{
		m_pResultDataCircleBuffer->Dump(p_szName);
	}

	if (!m_pResultImagePublishedCircleBuffer.empty())
	{
		m_pResultImagePublishedCircleBuffer->Dump(p_szName);
	}

	for (auto pInspection : m_arInspections)
	{
		pInspection->DumpDMInfo(p_szName);
	}
}

void CALLBACK SVPPQObject::OutputTimerCallback(UINT uTimerID, UINT uRsvd, DWORD_PTR dwUser, DWORD_PTR dwRsvd1, DWORD_PTR dwRsvd2)
{
	SVPPQObject* pPPQ = reinterpret_cast<SVPPQObject*>(dwUser);

	if (nullptr != pPPQ)
	{
		if (pPPQ->m_AsyncProcedure.IsActive())
		{
			pPPQ->NotifyProcessTimerOutputs();
		}
	}
}

HRESULT SVPPQObject::MarkProductInspectionsMissingAcquisiton(SVProductInfoStruct& p_rProduct, SVVirtualCamera* pCamera)
{
	HRESULT l_Status = S_OK;

	if (nullptr != pCamera)
	{
		for (auto pInspection : m_arInspections)
		{
			if (nullptr != pInspection && pInspection->IsCameraInInspection(pCamera->GetName()))
			{
				SVInspectionInfoStruct l_InspectInfo = p_rProduct.m_svInspectionInfos[pInspection->GetUniqueObjectID()];

				l_InspectInfo.oInspectedState = PRODUCT_NOT_INSPECTED;
				l_InspectInfo.m_CanProcess = false;
				l_InspectInfo.m_InProcess = true;

				l_InspectInfo.m_EndInspection = SvTl::GetTimeStamp();

				SVInspectionInfoPair l_Info(p_rProduct.ProcessCount(), l_InspectInfo);

				p_rProduct.m_ProductState += _T("|MC=");
				p_rProduct.m_ProductState += pInspection->GetName();
				p_rProduct.m_ProductState += _T("-");
				p_rProduct.m_ProductState += pCamera->GetName();

				m_oInspectionQueue.AddTail(l_Info);
			}
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

void CALLBACK SVPPQObject::APCThreadProcess(DWORD_PTR dwParam)
{
}

void SVPPQObject::ThreadProcess(bool& p_WaitForEvents)
{
	bool l_Processed = false;

	// will only execute if 0 < m_oTriggerQueue.size().
	// will then pop trigger queue.
	// pushes onto notify inspection queue.
	ProcessTrigger(l_Processed);

	if (!l_Processed)
	{
		ProcessDelayOutputs(l_Processed);
	}

	if (!l_Processed)
	{
		ProcessResetOutputs(l_Processed);
	}

	if (!l_Processed)
	{
		ProcessDataValidDelay(l_Processed);
	}

	if (!l_Processed)
	{
		// Inserts items onto the m_oCamerasQueue.
		ProcessCameraResponses(l_Processed);
	}

	if (!l_Processed)
	{
		// will only execute if 0 == m_oTriggerQueue.size().
		// pushes onto notify inspection queue.
		// The only function that can place items onto the m_oCameraQueue is
		// ProcessCameraResponses.
		ProcessCameraInputs(l_Processed);
	}

	if (!l_Processed)
	{
		// will only execute if 0 == m_oTriggerQueue.size().
		ProcessNotifyInspections(l_Processed);
	}

	if (!l_Processed)
	{
		// will only execute if 0 == m_oTriggerQueue.size().
		ProcessInspections(l_Processed);
	}

	if (!l_Processed)
	{
		ProcessCompleteInspections(l_Processed);
	}

	if (!l_Processed)
	{
		ProcessProductRequests(l_Processed);
	}

	p_WaitForEvents = !l_Processed;
}

HRESULT SVPPQObject::NotifyProcessTimerOutputs()
{
	HRESULT l_Status = S_OK;

	bool l_Process = false;

	if (0 < m_lOutputDelay && ::InterlockedCompareExchange(&(m_ProcessingOutputDelay), 1, 0) == 0)
	{
		SvTl::SVTimeStamp l_CurrentTime = SvTl::GetTimeStamp();

		if (0 < m_oOutputsDelayQueue.GetCount() || (0 < m_NextOutputDelayTimestamp && m_NextOutputDelayTimestamp <= l_CurrentTime))
		{
			l_Process = true;
		}
		else
		{
			::InterlockedExchange(&(m_ProcessingOutputDelay), 0);
		}
	}

	if (0 < m_lResetDelay && ::InterlockedCompareExchange(&(m_ProcessingOutputReset), 1, 0) == 0)
	{
		SvTl::SVTimeStamp l_CurrentTime = SvTl::GetTimeStamp();

		if (0 < m_oOutputsResetQueue.GetCount() || (0 < m_NextOutputResetTimestamp && m_NextOutputResetTimestamp <= l_CurrentTime))
		{
			l_Process = true;
		}
		else
		{
			::InterlockedExchange(&(m_ProcessingOutputReset), 0);
		}
	}

	if (0 < m_DataValidDelay && ::InterlockedCompareExchange(&(m_ProcessingDataValidDelay), 1, 0) == 0)
	{
		SvTl::SVTimeStamp l_CurrentTime = SvTl::GetTimeStamp();

		if (0 < m_DataValidDelayQueue.GetCount() || (0 < m_NextDataValidDelayTimestamp && m_NextDataValidDelayTimestamp <= l_CurrentTime))
		{
			l_Process = true;
		}
		else
		{
			::InterlockedExchange(&(m_ProcessingDataValidDelay), 0);
		}
	}

	if (l_Process)
	{
		m_AsyncProcedure.Signal(nullptr);
	}

	return l_Status;
}

HRESULT SVPPQObject::ProcessTrigger( bool& rProcessed )
{
	HRESULT l_Status = S_OK;

	/// Works through the queued triggers (e.g., pushed by FinishTrigger()).
	/// This is done by taking a SVTriggerQueueElement from the head of m_oTriggerQueue 
	/// and using the information in the SVTriggerInfoStruct therein to "create" a new product
	/// (the ProcessCount of which will be added to m_oNotifyInspectionsSet).
	/// In NextTriggerMode the results of the oldest product will be output through IndexPPQ().
	/// m_CameraInputData will contain trigger-data including the data index 
	rProcessed = ( 0 < m_oTriggerQueue.size() );

	if( rProcessed )
	{
		SVTriggerQueueElement poppedFromQueue;

		if (S_OK == m_oTriggerQueue.PopHead(poppedFromQueue))
		{
			if (m_bOnline)
			{
				SVProductInfoStruct* pProduct = IndexPPQ(poppedFromQueue.m_TriggerInfo);

				if (nullptr != pProduct)
				{
					InitializeProduct(pProduct, poppedFromQueue.m_Inputs);

					long lDataIndex = pProduct->oPPQInfo.m_ResultDataDMIndexHandle.GetIndex();

					m_voTriggerCount.SetValue(poppedFromQueue.m_TriggerInfo.lTriggerCount);
					m_voTriggerCount.CopyValue(lDataIndex);

					m_oNotifyInspectionsSet.insert(pProduct->ProcessCount());

					if (!poppedFromQueue.m_TriggerInfo.m_Data.empty())
					{
						m_CameraInputData.Set(boost::any_cast<SvTi::SVCameraTriggerData::NameVariantMap>(poppedFromQueue.m_TriggerInfo.m_Data));
					}
					// Get Shared Memory Slot
					if (HasActiveMonitorList() && GetSlotmanager().get())
					{
						try
						{

							long writerslot = GetSlotmanager()->GetNextWriteSlot();
							//TRACE("Get Writeslot %i\n", writerslot);
							pProduct->m_lastInspectedSlot = writerslot;
							for (auto& mapElement : pProduct->m_svInspectionInfos)
							{
								mapElement.second.m_lastInspectedSlot = writerslot;
							}
						}
						catch (const std::exception& e)
						{
							SvDef::StringVector msgList;
							msgList.push_back(e.what());
							SvStl::MessageMgrStd Exception(SvStl::LogOnly);
							Exception.setMessage(SVMSG_SVO_44_SHARED_MEMORY, SvStl::Tid_ProcessTrigger, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_15026);
						}
						catch (...)
						{
							SvDef::StringVector msgList;
							msgList.push_back(SvStl::MessageData::convertId2AddtionalText(SvStl::Tid_Unknown));
							SvStl::MessageMgrStd Exception(SvStl::LogOnly);
							Exception.setMessage(SVMSG_SVO_44_SHARED_MEMORY, SvStl::Tid_ProcessTrigger, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_15027);
						}
					} // if (HasActiveMonitorList())
				}
			} // if( m_bOnline )
		} // if( S_OK == m_oTriggerQueue.PopHead( poppedFromQueue )  )
		else
		{
			l_Status = E_FAIL;
		}
	} // if( p_rProcessed )

	return l_Status;
}

HRESULT SVPPQObject::ProcessNotifyInspections( bool& rProcessed )
{
	/// Does nothing if the trigger queue is nonempty or the m_oNotifyInspectionsSet is empty.
	/// Otherwise works through m_oNotifyInspectionsSet trying to notify one (and only one) inspection.
	HRESULT l_Status = S_OK;

	size_t	triggerQueueSize = m_oTriggerQueue.size();

	rProcessed = ( 0 <  triggerQueueSize );

	if( ! rProcessed ) // if there are no triggers remaining on the queue.
	{

		// Currently, 
		// SVPPQOjbect::AssignInputs () - called from ProcessTrigger (),
		// SVPPQOjbect::ProcessTrigger (), and
		// SVPPQOjbect::ProcessCameraInputs ()
		// can add items to m_oNotifyInspectionSet.
		rProcessed = ( 0 < m_oNotifyInspectionsSet.size() );

		if( rProcessed ) // if there is a request to notify.
		{
			// offset is from the beginning of the PPQ.  
			long l_Offset = -1;
			rProcessed = false;

			SVProcessCountSet::iterator l_Iter(m_oNotifyInspectionsSet.begin());

			// if the offset is outside the range of the PPQ, then execute 
			// this while statement.  The offset is initially forced outside 
			// the range in order to force the first execution.  
			do
			{
				long l_ProcessCount = *l_Iter;

				m_ppPPQPositions.GetIndexByTriggerCount(l_Offset, l_ProcessCount);

				rProcessed = ( 0 <= l_Offset && l_Offset < static_cast< long >( m_ppPPQPositions.size() ) );

				if (true == rProcessed) // if offset for element is within PPQ.
				{
					l_Status = NotifyInspections(l_Offset);

					if (S_OK == l_Status)
					{
						// event has been placed on the inspection queue and
						// should be removed from the NotifyInspectionQueue.
						// Processed is already set to true.
						l_Iter = m_oNotifyInspectionsSet.erase(l_Iter);
					}
					else if (S_FALSE == l_Status)
					{
						// this means that inspection was not notified 
						// because inputs or image were not ready.  
						// proceed to next inspection.
						l_Iter++;
						rProcessed = false;
						l_Status = S_OK;
					}
				}
				else // of offset for element is outside PPQ (fell off/dead).
				{
					// product is dead and process should be killed here 
					// so as to now waste inspection time.
					// processed is already false.
					l_Iter = m_oNotifyInspectionsSet.erase(l_Iter);
				}

			} while ((true != rProcessed) && 
				(l_Iter != m_oNotifyInspectionsSet.end()) &&
				SUCCEEDED(l_Status));


		} // if there is an item on the notify inspection queue.

	} // if there are no triggers on trigger event queue.

	return l_Status;
}

HRESULT SVPPQObject::ProcessInspections( bool& rProcessed )
/// Does nothing if there is at least one trigger in the trigger queue.
/// Otherwise, extracts all the GUIDs from m_ProcessInspectionsSet and starts the 
/// corresponding inspections via StartInspection().
/// They will be completed asynchronously in the class SVInspectionProcess.
{
	HRESULT l_Status = S_OK;

	rProcessed = ( 0 < m_oTriggerQueue.size() );

	if( ! rProcessed )
	{
		rProcessed = ( 0 < m_ProcessInspectionsSet.size() );

		if( rProcessed )
		{
			SVGuidSet::iterator l_Iter(m_ProcessInspectionsSet.begin());

			while (l_Iter != m_ProcessInspectionsSet.end())
			{
				StartInspection(*l_Iter);
				m_ProcessInspectionsSet.erase(l_Iter++);
			}
		}
	}

	return l_Status;
}

HRESULT SVPPQObject::ProcessResetOutputs( bool& rProcessed )
/// Removes the "head process count" from the outputs reset queue
/// and determines from it the corresponding product pointer.
/// If there is a new "head process count" in the outputs reset queue:
/// Determines if the corresponding product's m_EndResetDelay has already been reached.
/// If so, calls ResetOutputs().
/// If not, puts the original "head process count" back onto the outputs reset queue
{
	HRESULT l_Status = S_OK;

	rProcessed = ( 0 < m_oTriggerQueue.size() );

	if( ! rProcessed )
	{
		rProcessed = ( 0 < m_oOutputsResetQueue.GetCount() );

		if( rProcessed )
		{
			SVProductInfoStruct* l_pProduct = nullptr;

			SvTl::SVTimeStamp l_CurrentTime = SvTl::GetTimeStamp();

			while (0 < m_oOutputsResetQueue.GetCount() && nullptr == l_pProduct)
			{
				long l_ProcessCount = 0;

				if (m_oOutputsResetQueue.RemoveHead(&l_ProcessCount))
				{
					GetProductInfoStruct(l_pProduct, l_ProcessCount);
				}
				else
				{
					l_Status = E_FAIL;

					break;
				}

				if (nullptr != l_pProduct && 0 < m_oOutputsResetQueue.GetCount())
				{
					if (0 < m_oOutputsResetQueue.GetCount())
					{
						SVProductInfoStruct* l_pNextProduct = nullptr;

						if (m_oOutputsResetQueue.GetHead(&l_ProcessCount))
						{
							GetProductInfoStruct(l_pNextProduct, l_ProcessCount);
						}

						if (nullptr != l_pNextProduct && l_pNextProduct->oOutputsInfo.m_EndResetDelay <= l_CurrentTime)
						{
							l_pProduct = nullptr;
						}
					}
				}

				if (nullptr != l_pProduct)
				{
					if (l_CurrentTime < l_pProduct->oOutputsInfo.m_EndResetDelay)
					{
						m_NextOutputResetTimestamp = l_pProduct->oOutputsInfo.m_EndResetDelay;

						m_oOutputsResetQueue.AddHead(l_pProduct->ProcessCount());

						l_pProduct = nullptr;

						break;
					}
				}
			}

			rProcessed = ( nullptr != l_pProduct );

			if( rProcessed )
			{
				ResetOutputs();
			}
			else
			{
				l_Status = E_FAIL;
			}

			::InterlockedExchange(&(m_ProcessingOutputReset), 0);
		}
	}

	return l_Status;
}

HRESULT SVPPQObject::ProcessDataValidDelay(bool& rProcessed)
{
	HRESULT Status = S_OK;

	rProcessed = (0 < m_oTriggerQueue.size());

	if (!rProcessed)
	{
		rProcessed = (0 < m_DataValidDelayQueue.GetCount());

		if (rProcessed)
		{
			SVProductInfoStruct* pProduct(nullptr);

			SvTl::SVTimeStamp CurrentTime = SvTl::GetTimeStamp();

			while (0 < m_DataValidDelayQueue.GetCount() && nullptr == pProduct)
			{
				long ProcessCount = 0;

				if (m_DataValidDelayQueue.RemoveHead(&ProcessCount))
				{
					GetProductInfoStruct(pProduct, ProcessCount);
				}
				else
				{
					Status = E_FAIL;
					break;
				}

				if (nullptr != pProduct && 0 < m_DataValidDelayQueue.GetCount())
				{
					SVProductInfoStruct* pNextProduct(nullptr);

					if (m_DataValidDelayQueue.GetHead(&ProcessCount))
					{
						GetProductInfoStruct(pNextProduct, ProcessCount);
					}
					if (nullptr != pNextProduct && pNextProduct->oOutputsInfo.m_EndDataValidDelay <= CurrentTime)
					{
						pProduct = nullptr;
					}
				}

				if (nullptr != pProduct)
				{
					if (CurrentTime < pProduct->oOutputsInfo.m_EndDataValidDelay)
					{
						m_NextOutputDelayTimestamp = pProduct->oOutputsInfo.m_EndDataValidDelay;

						m_DataValidDelayQueue.AddHead(pProduct->ProcessCount());

						pProduct = nullptr;
						break;
					}
				}
			}

			rProcessed = (nullptr != pProduct);

			if (rProcessed)
			{
				if (!m_pDataValid.empty())
				{
					m_pOutputList->WriteOutputValue(m_pDataValid, pProduct->oOutputsInfo.DataValidResult);
				}
				if (!m_pOutputToggle.empty())
				{
					m_pOutputList->WriteOutputValue(m_pOutputToggle, pProduct->oOutputsInfo.OutputToggleResult);
				}
			}
			else
			{
				Status = E_FAIL;
			}

			::InterlockedExchange(&(m_ProcessingDataValidDelay), 0);
		}
	}

	return Status;
}

HRESULT SVPPQObject::ProcessCameraResponses( bool& rProcessed )
/// If camera responses pending AND trigger queue empty:
/// Removes camera responses from m_PendingCameraResponses and tries to process them
/// until the first processing succeeds.
/// If no responses processed: Removes the head of m_CameraResponseQueue and processes that
/// until one processing attempt is successful or m_CameraResponseQueue is empty.
{
	HRESULT l_Status = S_OK;
	size_t	triggerQueueSize = 0;
	size_t	pendingCameraQueueSize = m_PendingCameraResponses.size();
	rProcessed = false;

	if (0 < pendingCameraQueueSize)
	{
		SVCameraInfoMap::const_iterator l_CameraIter = m_Cameras.begin();

		while( S_OK == l_Status  && l_CameraIter != m_Cameras.end() && ! rProcessed )
		{
			SVCameraQueueElement l_Data;
			triggerQueueSize = m_oTriggerQueue.size();

			// stop processing camera responses if there is an element on the trigger queue.
			if (0 >= triggerQueueSize)
			{
				SVPendingCameraResponseMap::iterator l_PendingIter = m_PendingCameraResponses.find(l_CameraIter->first);

				if (l_PendingIter != m_PendingCameraResponses.end())
				{
					l_Data = l_PendingIter->second;

					m_PendingCameraResponses.erase(l_PendingIter);

					SVObjectManagerClass::Instance().DecrementPendingImageIndicator();

					rProcessed = ( S_OK == ProcessCameraResponse( l_Data )  );
				}
			}

			++l_CameraIter;
		}
	} // if( 0 < m_PendingCameraResponses.size() )

	while( S_OK == l_Status  && 0 < m_CameraResponseQueue.GetCount() && ! rProcessed )
	{
		SVCameraQueueElement l_Data;

		triggerQueueSize = m_oTriggerQueue.size();
		rProcessed = ( 0 < m_oTriggerQueue.size() );

		if (0 >= triggerQueueSize)
		{
			if (m_CameraResponseQueue.RemoveHead(&l_Data))
			{
				rProcessed = ( S_OK == ProcessCameraResponse( l_Data )  );
			}
			else
			{
				l_Status = E_FAIL;

				break;
			}
		}
	}

	return l_Status;
}

HRESULT SVPPQObject::ProcessCameraInputs( bool& rProcessed )
/// If possible, removes the first item from m_oCamerasQueue and gets its product information.
/// Sets the product information's bFinishAcquisition member to true.
/// Adds the product's process count to m_oNotifyInspectionsSet.
{
	HRESULT l_Status = S_OK;
	long	cameraCount = -1;
	long	triggerCount = -1;
	long	tmpProcessCount = -1;

	rProcessed = false; // rProcessed = ( 0 < m_oTriggerQueue.size() );

	// if trigger queue size <= 0
	if( ! rProcessed )
	{

		cameraCount = m_oCamerasQueue.GetCount();
		rProcessed = ( 0 <  cameraCount);

		if( rProcessed )
		{
			SVProductInfoStruct* pProduct = nullptr;
			long l_PPQIndex = static_cast<long>(m_ppPPQPositions.size());

			while (0 < m_oCamerasQueue.GetCount() && nullptr == pProduct)
			{
				long l_ProcessCount = 0;

				if (m_oCamerasQueue.RemoveHead(&l_ProcessCount))
				{
					GetProductIndex(l_PPQIndex, l_ProcessCount);

					if (0 <= l_PPQIndex && l_PPQIndex < static_cast<long>(m_ppPPQPositions.size()))
					{
						pProduct = m_ppPPQPositions.GetProductAt(l_PPQIndex);
						triggerCount = pProduct->oTriggerInfo.lTriggerCount;
					}
				}
				else
				{
					l_Status = E_FAIL;
					break;
				}
			}

			rProcessed = ( nullptr != pProduct );

			if( rProcessed )
			{
				if (!(pProduct->bFinishAcquisition))
				{
					pProduct->bFinishAcquisition = true;

					SVGuidSVCameraInfoStructMap::const_iterator Iter(pProduct->m_svCameraInfos.begin());

					for (; pProduct->m_svCameraInfos.end() != Iter; ++Iter)
					{
						pProduct->bFinishAcquisition &= (Iter->second.GetIndex() != -1);
					}
				}

				// Doesn't necessarilly have the correct Trigger Count yet.
				tmpProcessCount = pProduct->ProcessCount();
				m_oNotifyInspectionsSet.insert(tmpProcessCount);
			} 
		} 
	}

	return l_Status;
}

HRESULT SVPPQObject::ProcessCompleteInspections( bool& rProcessed )
/// Does nothing unless m_oTriggerQueue is empty and m_oInspectionQueue is non-empty.
/// If so, gets all SVInspectionInfoPairs from m_oInspectionQueue and inserts all
/// inspection GUIDs into m_ProcessInspectionsSet.
/// If m_ProcessInspectionsSet then is non-empty, calls ProcessInspection(false),
/// so the next product (if any) can be inspected,
/// then extracts the first valid SVInspectionInfoPair form m_oInspectionQueue
/// and gets a pointer to the corresponding SVProductInfoStruct.
/// If m_EndProcess == 0.0 SVInspectionInfoStruct::ClearIndexes() and then 
/// SetInspectionComplete() are called.
/// The product is then set to complete and all products further back in m_ppPPQPositions
/// are set to incomplete.
{
	HRESULT l_Status = S_OK;

	rProcessed = ( 0 < m_oTriggerQueue.size() );

	if( ! rProcessed )
	{
		rProcessed = ( 0 < m_oInspectionQueue.GetCount() );

		if( rProcessed )
		{
			SVInspectionInfoPair l_Info;
			SVProductInfoStruct* l_pPPQProduct = nullptr;
			long l_PPQIndex = static_cast<long>(m_ppPPQPositions.size());

			for (int i = 0; i < m_oInspectionQueue.GetCount(); ++i)
			{
				if (m_oInspectionQueue.GetAt(i, &l_Info))
				{
					if (nullptr != l_Info.second.pInspection)
					{
						m_ProcessInspectionsSet.insert(l_Info.second.pInspection->GetUniqueObjectID());
					}
				}
			}

			if (!(m_ProcessInspectionsSet.empty()))
			{
				bool l_Processed = false;

				ProcessInspections(l_Processed);
			}

			while (0 < m_oInspectionQueue.GetCount() && nullptr == l_pPPQProduct)
			{
				if (m_oInspectionQueue.RemoveHead(&l_Info))
				{
					GetProductIndex(l_PPQIndex, l_Info.first);

					if (0 <= l_PPQIndex && l_PPQIndex < static_cast<long>(m_ppPPQPositions.size()))
					{
						l_pPPQProduct = m_ppPPQPositions.GetProductAt(l_PPQIndex);
					}
				}
				else
				{
					l_Status = E_FAIL;

					break;
				}
			}

			rProcessed = ( nullptr != l_pPPQProduct );

			if( rProcessed )
			{
				if (l_pPPQProduct->oOutputsInfo.m_EndProcess == 0.0)
				{
					SVInspectionInfoStruct& l_rInspectInfo = l_Info.second;
					SVInspectionInfoStruct& l_rPPQInspectInfo = l_pPPQProduct->m_svInspectionInfos[l_rInspectInfo.pInspection->GetUniqueObjectID()];

					l_rPPQInspectInfo = l_rInspectInfo;

					l_rPPQInspectInfo.ClearIndexes();

					l_rPPQInspectInfo.m_CallbackReceived = SvTl::GetTimeStamp();

					l_pPPQProduct->m_ProductState += _T("|");
					l_pPPQProduct->m_ProductState += l_rInspectInfo.pInspection->GetName();
#ifdef EnableTracking
					std::string l_Title = l_rInspectInfo.pInspection->GetName();

					l_Title += _T(" Complete");


					m_PPQTracking.IncrementCount(l_Title, l_PPQIndex);
#endif

					// Inspection Process is done, let everyone know.
					if (!SetInspectionComplete(l_PPQIndex))
					{
						l_Status = E_FAIL;
					}
				}

				if (l_pPPQProduct->bDataComplete)
				{

					SetProductComplete(l_PPQIndex);


					for (size_t i = l_PPQIndex + 1; i < m_ppPPQPositions.size(); ++i)
					{
						l_pPPQProduct = m_ppPPQPositions.GetProductAt(l_PPQIndex);

						if (nullptr != l_pPPQProduct && l_pPPQProduct->IsProductActive())
						{
							SetProductIncomplete(static_cast<long>(i));
						}
						else
						{
							break;
						}
					}
				}
			}
		}
	}

	return l_Status;
}

HRESULT SVPPQObject::ProcessProductRequests( bool& rProcessed )
/// Does nothing unless both m_oTriggerQueue and m_ProductRequests are nonempty.
/// Otherwise extracts the first SVProductRequestPair from m_ProductRequests,
/// gets the SVProductInfoStruct specified in the request.
/// Then copies the SVProductInfoStruct into the m_pProduct member of the SVProductInfoRequestStruct
/// contained in the request pair.
{
	HRESULT l_Status = S_OK;

	rProcessed = ( 0 < m_oTriggerQueue.size() );

	if( ! rProcessed )
	{
		rProcessed = ( 0 < m_ProductRequests.GetCount() );

		if( rProcessed )
		{
			SVProductRequestPair l_Request;

			if (m_ProductRequests.RemoveHead(&l_Request))
			{
				if (nullptr != l_Request.second.m_pProduct)
				{
					SVProductInfoStruct* pProduct = nullptr;

					GetProductInfoStruct(pProduct, l_Request.first);

					if (nullptr != pProduct)
					{
						*(l_Request.second.m_pProduct) = *(pProduct);
					}
					else
					{
						l_Request.second.m_pProduct->InitProductInfo();
					}
				}

				l_Status = l_Request.second.NotifyRequestComplete();
			}
			else
			{
				l_Status = E_FAIL;
			}
		}
	}

	return l_Status;
}

HRESULT SVPPQObject::GetProduct(SVProductInfoStruct& p_rProduct, long lProcessCount) const
{
	HRESULT l_Status = S_OK;

	p_rProduct.InitProductInfo();

	if (m_AsyncProcedure.IsActive() && m_ProductRequests.IsCreated())
	{
		SVProductInfoStruct* l_pProduct = new SVProductInfoStruct;

		if (nullptr != l_pProduct)
		{
			SVProductRequestPair l_Request(lProcessCount, SVProductInfoRequestStruct(l_pProduct));

			if (m_ProductRequests.AddTail(l_Request))
			{
				m_AsyncProcedure.Signal(nullptr);

				l_Status = l_Request.second.WaitForRequest();

				if (S_OK == l_Status)
				{
					p_rProduct = *l_pProduct;

					if (p_rProduct.ProcessCount() < 1)
					{
						l_Status = E_FAIL;
					}
				}
			}
			else
			{
				l_Status = E_FAIL;
			}

			delete l_pProduct;
		}
		else
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

void SVPPQObject::PersistInputs(SvOi::IObjectWriter& rWriter)
{
	SVIOEntryHostStructPtrVector ppIOEntries;
	_variant_t l_svValue;
	std::string l_svName;

	rWriter.StartElement(SvXml::CTAG_INPUT);

	GetAllInputs(ppIOEntries);

	long lCount = static_cast<long>(ppIOEntries.size());

	for (long lInput = 0; lInput < lCount; lInput++)
	{
		SVIOEntryHostStructPtr pIOEntry = ppIOEntries[lInput];
		if (!pIOEntry.empty()
			&& pIOEntry->m_ObjectType != IO_CAMERA_DATA_INPUT)
		{
			l_svName = SvUl::Format(SvXml::CTAGF_INPUT_X, lInput);
			rWriter.StartElement(l_svName.c_str());

			switch (pIOEntry->m_ObjectType)
			{
			case IO_DIGITAL_INPUT:
			{
				l_svValue.SetString(_T("Digital"));
				rWriter.WriteAttribute(SvXml::CTAG_IO_TYPE, l_svValue);
				l_svValue.Clear();

				l_svValue.SetString(pIOEntry->getObject()->GetName());
				rWriter.WriteAttribute(SvXml::CTAG_ITEM_NAME, l_svValue);
				l_svValue.Clear();

				l_svValue = -1;
				rWriter.WriteAttribute(SvXml::CTAG_ATTRIBUTE, l_svValue);
				l_svValue.Clear();

				l_svValue = pIOEntry->m_PPQIndex;
				rWriter.WriteAttribute(SvXml::CTAG_PPQ_POSITION, l_svValue);
				l_svValue.Clear();
			}
			break;

			case IO_REMOTE_INPUT:
			{
				l_svValue.SetString(_T("Remote"));
				rWriter.WriteAttribute(SvXml::CTAG_IO_TYPE, l_svValue);
				l_svValue.Clear();

				l_svValue.SetString(pIOEntry->getObject()->GetName());
				rWriter.WriteAttribute(SvXml::CTAG_ITEM_NAME, l_svValue);
				l_svValue.Clear();

				SVRemoteInputObject* pRemote = nullptr;

				pRemote = dynamic_cast<SVRemoteInputObject*> (m_pInputList->GetInput(pIOEntry->m_IOId));

				if (nullptr != pRemote)
				{
					l_svValue = pRemote->m_lIndex;
					rWriter.WriteAttribute(SvXml::CTAG_REMOTE_INDEX, l_svValue);
					l_svValue.Clear();
				}

				l_svValue = pIOEntry->m_PPQIndex;
				rWriter.WriteAttribute(SvXml::CTAG_PPQ_POSITION, l_svValue);
				l_svValue.Clear();

				if (nullptr != pRemote)
				{
					pRemote->GetCache(l_svValue);

					if (l_svValue.vt != VT_EMPTY)
					{
						rWriter.WriteAttribute(SvXml::CTAG_REMOTE_INITIAL_VALUE, l_svValue);
					}
				}
			}
			break;

			default:
			{
				break; // Do nothing.
			}
			}
			rWriter.EndElement();
		}
	}
	rWriter.EndElement();
}

SvTi::SVCameraTriggerData& SVPPQObject::GetCameraInputData()
{
	return m_CameraInputData;
}

long SVPPQObject::GetExtraBufferSize() const
{
	return g_lPPQExtraBufferSize;
}

static bool CompareInspectionName(const std::string& name, const std::string& dottedName)
{
	SVDottedName parsedName(dottedName.c_str());
	if (parsedName.size())
	{
		std::string inspectionName = parsedName[0];
		int cmp = inspectionName.compare(name);
		return (cmp > 0) ? true : false;
	}
	return false;
}

void SVPPQObject::SetMonitorList(const ActiveMonitorList& rActiveList)
{
	m_bActiveMonitorList = rActiveList.first;
}

bool SVPPQObject::HasActiveMonitorList() const
{
	return m_bActiveMonitorList;
}

void SVPPQObject::SetSlotmanager(const SvSml::RingBufferPointer& Slotmanager)
{
	if (m_SlotManager.get())
	{
		m_SlotManager.reset();
	}
	m_SlotManager = Slotmanager;
}
SvSml::RingBufferPointer SVPPQObject::GetSlotmanager()
{
	return m_SlotManager;
}


void SVPPQObject::fillChildObjectList(SVObjectClass::SVObjectPtrDeque& objectList, UINT AttributesAllowedFilter) const
{
	if (AttributesAllowedFilter != m_AttributesAllowedFilterForFillChildObjectList)
	{ //only recreate the list if the attributes changed.
		m_childObjectsForFillChildObjectList.clear();
		for (SVObjectPtrDeque::const_iterator iter = m_childObjects.cbegin(); iter != m_childObjects.cend(); ++iter)
		{
			if (((*iter)->ObjectAttributesAllowed() & AttributesAllowedFilter) == AttributesAllowedFilter)
			{
				m_childObjectsForFillChildObjectList.push_back(*iter);
			}
		}
		//save the attribute value for the created list.
		m_AttributesAllowedFilterForFillChildObjectList = AttributesAllowedFilter;
	}

	objectList.insert(objectList.end(), m_childObjectsForFillChildObjectList.begin(), m_childObjectsForFillChildObjectList.end());
}

const std::string& SVPPQObject::GetConditionalOutputName() const
{
	return m_conditionalOutputName;
}

void SVPPQObject::SetConditionalOutputName(const std::string& conditionalOutputName)
{
	m_conditionalOutputName = conditionalOutputName;
}

#pragma region SVTriggerQueueElement Constructor
SVPPQObject::SVTriggerQueueElement::SVTriggerQueueElement()
	: m_TriggerInfo()
	, m_Inputs()
{
}

SVPPQObject::SVTriggerQueueElement::SVTriggerQueueElement(const SVTriggerQueueElement& p_rObject)
	: m_TriggerInfo(p_rObject.m_TriggerInfo)
	, m_Inputs(p_rObject.m_Inputs)
{
}

SVPPQObject::SVTriggerQueueElement::~SVTriggerQueueElement()
{
}
#pragma endregion SVTriggerQueueElement Constructor

#pragma region SVCameraQueueElement Constructor
SVPPQObject::SVCameraQueueElement::SVCameraQueueElement()
	: m_pCamera(nullptr)
	, m_Data()
{
}

SVPPQObject::SVCameraQueueElement::SVCameraQueueElement(const SVCameraQueueElement& rObject)
	: m_pCamera(rObject.m_pCamera)
	, m_Data(rObject.m_Data)
{
}

SVPPQObject::SVCameraQueueElement::SVCameraQueueElement(SVVirtualCamera* pCamera, const SVODataResponseClass& rData)
	: m_pCamera(pCamera)
	, m_Data(rData)
{
}

SVPPQObject::SVCameraQueueElement::~SVCameraQueueElement()
{
}
#pragma endregion SVCameraQueueElement Constructor

#pragma region SVCameraInfoElement Constructor
SVPPQObject::SVCameraInfoElement::SVCameraInfoElement()
	: m_CameraPPQIndex(-1)
	, m_IOEntryPtr()
	, m_ToggleState(true)
{
}

SVPPQObject::SVCameraInfoElement::SVCameraInfoElement(const SVCameraInfoElement& p_rObject)
	: m_CameraPPQIndex(p_rObject.m_CameraPPQIndex)
	, m_IOEntryPtr(p_rObject.m_IOEntryPtr)
	, m_ToggleState(p_rObject.m_ToggleState)
{
}

SVPPQObject::SVCameraInfoElement::~SVCameraInfoElement()
{
}
#pragma endregion SVCameraInfoElement Constructor

#ifdef EnableTracking
#pragma region SVPPQTrackingElement Constructor
SVPPQObject::SVPPQTrackingElement::SVPPQTrackingElement()
	: m_TrackedCounts()
{
}

SVPPQObject::SVPPQTrackingElement::SVPPQTrackingElement(const SVPPQTrackingElement& p_rObject)
	: m_TrackedCounts(p_rObject.m_TrackedCounts)
{
}

SVPPQObject::SVPPQTrackingElement::~SVPPQTrackingElement()
{
	clear();
}
#pragma endregion SVPPQTrackingElement Constructor

void SVPPQObject::SVPPQTrackingElement::clear()
{
	if (!(m_TrackedCounts.empty()))
	{
		m_TrackedCounts.clear();
	}
}

void SVPPQObject::SVPPQTrackingElement::IncrementCount(size_t p_Index, size_t p_VectorSize)
{
	if (p_Index < p_VectorSize)
	{
		if (m_TrackedCounts.size() != p_VectorSize)
		{
			m_TrackedCounts.resize(p_VectorSize, 0);
		}

		++(m_TrackedCounts[p_Index]);
	}
}

#pragma region SVPPQTracking Constructor
SVPPQObject::SVPPQTracking::SVPPQTracking()
	: m_QueueLength(0)
	, m_Counts()
	, m_QueueCounts()
	, m_QueueWriteTimeCounts()
{
}

SVPPQObject::SVPPQTracking::SVPPQTracking(const SVPPQTracking& p_rObject)
	: m_QueueLength(p_rObject.m_QueueLength)
	, m_Counts(p_rObject.m_Counts)
	, m_QueueCounts(p_rObject.m_QueueCounts)
	, m_TimeLength(p_rObject.m_TimeLength)
	, m_QueueWriteTimeCounts(p_rObject.m_QueueWriteTimeCounts)
{
}

SVPPQObject::SVPPQTracking::~SVPPQTracking()
{
	clear();
}
#pragma endregion SVPPQTracking Constructor

void SVPPQObject::SVPPQTracking::clear()
{
	m_QueueLength = 0;

	if (!(m_Counts.empty()))
	{
		m_Counts.clear();
	}

	if (!(m_QueueCounts.empty()))
	{
		m_QueueCounts.clear();
	}
	if (!(m_QueueWriteTimeCounts.empty()))
	{
		m_QueueWriteTimeCounts.clear();
	}
}

void SVPPQObject::SVPPQTracking::IncrementCount(const std::string& p_rName)
{
	++(m_Counts[p_rName]);
}

void SVPPQObject::SVPPQTracking::IncrementCount(const std::string& p_rName, size_t p_Index)
{
	m_QueueCounts[p_rName].IncrementCount(p_Index, m_QueueLength);
}

void SVPPQObject::SVPPQTracking::IncrementTimeCount(const std::string& p_rName, size_t p_Index)
{
	m_QueueWriteTimeCounts[p_rName].IncrementCount(p_Index, m_TimeLength);
}
#endif //EnableTracking

static SVGUID GetInspectionGuid(const std::string& rName)
{
	SVGUID guid;
	SVDottedName dottedName(rName.c_str());

	if (dottedName.size())
	{
		guid = SVObjectManagerClass::Instance().GetObjectIdFromCompleteName(dottedName[0].c_str());
	}
	return guid;
}

static long GetProductInfoInspectionIndex(const SVGUID& rGuid, const SVProductInfoStruct& rProduct)
{
	long index = -1;
	SVGUIDSVInspectionInfoStructMap::const_iterator it = rProduct.m_svInspectionInfos.find(rGuid);
	if (it != rProduct.m_svInspectionInfos.end())
	{
		index = it->second.m_lastInspectedSlot;
	}
	return index;
}

void SVPPQObject::ResetOutputValueObjects(long DataIndex)
{
	m_voNotInspected.SetValue(BOOL(true));
	m_voNAK.SetValue(BOOL(true));
	m_voMasterWarning.SetValue(BOOL(true));
	m_voMasterFault.SetValue(BOOL(true));
	m_voDataValid.SetValue(BOOL(false));
	m_voACK.SetValue(BOOL(false));
	m_voNotInspected.CopyValue(DataIndex);
	m_voNAK.CopyValue(DataIndex);
	m_voMasterWarning.CopyValue(DataIndex);
	m_voMasterFault.CopyValue(DataIndex);
	m_voDataValid.CopyValue(DataIndex);
	m_voACK.CopyValue(DataIndex);
}

// This method is for releasing the writer lock for Inspections when the product is incomplete
void SVPPQObject::ReleaseSharedMemory( SVProductInfoStruct& rProduct)
{
	if (HasActiveMonitorList())
	{
		try
		{
			long shareSlotIndex = rProduct.m_lastInspectedSlot;
			if (shareSlotIndex >= 0 && GetSlotmanager().get())
			{
				GetSlotmanager()->ReleaseWriteSlot(shareSlotIndex, rProduct.ProcessCount(), true);
				rProduct.m_lastInspectedSlot = -1;
			}
			
		}
		catch (const std::exception& e)
		{
			SvDef::StringVector msgList;
			msgList.push_back(e.what());
			SvStl::MessageMgrStd Exception(SvStl::LogOnly);
			Exception.setMessage(SVMSG_SVO_44_SHARED_MEMORY, SvStl::Tid_ReleaseProduct, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_15029);
		}
		catch (...)
		{
			SvDef::StringVector msgList;
			msgList.push_back(SvStl::MessageData::convertId2AddtionalText(SvStl::Tid_Unknown));
			SvStl::MessageMgrStd Exception(SvStl::LogOnly);
			Exception.setMessage(SVMSG_SVO_44_SHARED_MEMORY, SvStl::Tid_ReleaseProduct, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_15030);
		}
	}
}

// This method writes the inspection info to the PPQ shared memory when the product is Complete
void SVPPQObject::CommitSharedMemory( SVProductInfoStruct& rProduct)
{
	if (HasActiveMonitorList())
	{
		try
		{
			long shareSlotIndex = rProduct.m_lastInspectedSlot;
			if (shareSlotIndex >= 0 && GetSlotmanager().get())
			{
				GetSlotmanager()->ReleaseWriteSlot(shareSlotIndex, rProduct.ProcessCount(), true);
				rProduct.m_lastInspectedSlot = -1;
			}
			 
		}
		catch (const std::exception& e)
		{
			SvDef::StringVector msgList;
			msgList.push_back(e.what());
			SvStl::MessageMgrStd Exception(SvStl::LogOnly);
			Exception.setMessage(SVMSG_SVO_44_SHARED_MEMORY, SvStl::Tid_CommitSharedMemory, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_15032);
		}
		catch (...)
		{
			SvDef::StringVector msgList;
			msgList.push_back(SvStl::MessageData::convertId2AddtionalText(SvStl::Tid_Unknown));
			SvStl::MessageMgrStd Exception(SvStl::LogOnly);
			Exception.setMessage(SVMSG_SVO_44_SHARED_MEMORY, SvStl::Tid_CommitSharedMemory, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_15033);
		}
	}
}




DWORD SVPPQObject::GetObjectColor() const
{
	return SvDef::DefaultWhiteColor;
}
void SVPPQObject::SetNAKMode(SvDef::NakGeneration nakMode, int NAKPar)
{
	m_NAKMode = nakMode;
	m_NAKParameter = NAKPar;
}
