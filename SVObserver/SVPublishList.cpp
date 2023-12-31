//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPublishList.cpp
//* .File Name       : $Workfile:   SVPublishList.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.7  $
//* .Check In Date   : $Date:   15 May 2014 12:31:44  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVPublishList.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "InspectionEngine/SVTaskObject.h"
#include "SVIOLibrary/SVOutputObjectList.h"
#include "SVInfoStructs.h"
#include "SVInspectionProcess.h"
#include "SVConfigurationObject.h"
#include "SVPPQObject.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVOLibrary/SVHardwareManifest.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void SVPublishList::Refresh(SvIe::SVTaskObjectClass * pRootObject)
{
	// Find all outputs marked as selected for publishing
	// Note:: both Lists must be from the same root object
	// which at this point is SVToolSet
	auto outputList = pRootObject->getOutputListFiltered(SvPb::publishable);

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	// check for removed items
	auto iter_removeStart = std::remove_if(m_objectIdList.begin(), m_objectIdList.end(), [outputList](const auto objectId) 
		{return std::none_of(outputList.begin(), outputList.end(), [objectId](auto* pObject) { return objectId == pObject->getObjectId(); }); });
	auto iter = iter_removeStart;
	// Now remove it from all of the PPQs
	SVPPQObject* pPPQ = m_pInspection->GetPPQ();
	if (nullptr != pPPQ)
	{
		while (m_objectIdList.end() != iter)
		{
			const SVIOEntryHostStructPtrVector& rAllOutputs {pPPQ->GetAllOutputs()};
			auto iterPPQ = std::find_if(rAllOutputs.begin(), rAllOutputs.end(), [iter](const auto pEntry) { return *iter == pEntry->getObject()->getObjectId(); });
			if (rAllOutputs.end() != iterPPQ)
			{
				pPPQ->RemoveOutput(*iterPPQ);
			}
			else
			{
				SvStl::MessageManager e(SvStl::MsgType::Log);
				e.setMessage(SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorFindingPPQEntries, SvStl::SourceFileParams(StdMessageParams));
				DebugBreak();
			}
			++iter;
		}
	}
	m_objectIdList.erase(iter_removeStart, m_objectIdList.end());

	// check for new items
	for( auto* pIObject : outputList )
	{
		bool found = std::any_of(m_objectIdList.begin(), m_objectIdList.end(), [pIObject](auto objectId) { return objectId == pIObject->getObjectId(); });

		if( !found ) // not in original list - have to add it
		{
			auto* pObject = dynamic_cast<SVObjectClass*>(pIObject);
			if (nullptr != dynamic_cast<SvOi::IValueObject*> (pObject))
			{
				pObject->connectObject(m_pInspection->getObjectId());
				SVOutputObjectPtr pOutput;
				SVOutputObjectList* pOutputList(nullptr);
				if (nullptr != pConfig) { pOutputList = pConfig->GetOutputObjectList(); }

				if (nullptr != pOutputList)
				{
					pOutput = pOutputList->GetOutput(pObject->GetCompleteName().c_str());
				}

				SVIOEntryHostStructPtr pIOEntry = std::make_shared<SVIOEntryHostStruct>();
				pIOEntry->setLinkedObject(pObject);
				pIOEntry->getObject()->SetObjectOwner(pObject->GetParent());
				pIOEntry->m_PPQIndex = -1;
				if (pIOEntry->isAimObjectBool())
				{
					pIOEntry->m_ObjectType = SVHardwareManifest::isPlcSystem(pConfig->GetProductType()) ? IO_PLC_OUTPUT : IO_DIGITAL_OUTPUT;
					pIOEntry->m_Enabled = (nullptr != pOutput);

					if (pIOEntry->m_Enabled)
					{
						pIOEntry->m_IOId = pOutput->getObjectId();
					}
				}
				else
				{
					pIOEntry->m_ObjectType = IO_REMOTE_OUTPUT;
					pIOEntry->m_Enabled = true;

					pIOEntry->m_IOId = pObject->getObjectId();
				}

				if (nullptr != pPPQ) { pPPQ->AddOutput(pIOEntry); }
				// add to the list
				m_objectIdList.push_back(pObject->getObjectId());
			}// end if
		}// end if
	}// end for
}// end Refresh

void SVPublishList::Release(SvIe::SVTaskObjectClass*)
{
	// We must release all the outputs that had been marked for publishing
	// all outputs marked as selected for publishing
	// Note:: both Lists must be from the same root object
	// which at this point is SVToolSet
	// check for removed items
	assert(m_pInspection);
	SVPPQObject* pPPQ = m_pInspection->GetPPQ();
	if (nullptr != pPPQ)
	{
		for (auto objectId : m_objectIdList)
		{
			const SVIOEntryHostStructPtrVector& rAllOutputs {pPPQ->GetAllOutputs()};
			auto iterPPQ = std::find_if(rAllOutputs.begin(), rAllOutputs.end(), [objectId](const auto pEntry) { return objectId == pEntry->getObject()->getObjectId(); });
			if (rAllOutputs.end() != iterPPQ)
			{
				pPPQ->RemoveOutput(*iterPPQ);
			}
		}// end for
	}
	// remove from the list
	m_objectIdList.clear();

}// end Release

bool SVPublishList::RemovePublishedEntry(uint32_t id )
{
	auto iter = std::find_if(m_objectIdList.begin(), m_objectIdList.end(), [id](auto entryId) { return id == entryId; });

	if (m_objectIdList.end() != iter)// && pPublishedOutObjectInfo->getObjectId() == id )
	{
		// remove from the list
		m_objectIdList.erase(iter);

		// Now remove it from all of the PPQs
		SVPPQObject* pPPQ = m_pInspection->GetPPQ();
		if (nullptr != pPPQ)
		{
			const SVIOEntryHostStructPtrVector& rAllOutputs {pPPQ->GetAllOutputs()};
			auto iterPPQ = std::find_if(rAllOutputs.begin(), rAllOutputs.end(), [id](const auto pEntry) { return id == pEntry->getObject()->getObjectId(); });
			if (rAllOutputs.end() != iterPPQ)
			{
				pPPQ->RemoveOutput(*iterPPQ);
			}
		}

		return true;
	}// end if

	return false;
}// end RemovePublishedEntry

