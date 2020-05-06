//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInputObjectList
//* .File Name       : $Workfile:   SVInputObjectList.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   07 Aug 2013 13:27:28  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVInputObjectList.h"
#include "SVCameraDataInputObject.h"
#include "SVDigitalInputObject.h"
#include "SVRemoteInputObject.h"
#include "SVIOConfigurationInterfaceClass.h"
#pragma endregion Includes

SVInputObjectList::SVInputObjectList( LPCSTR ObjectName ) : SVObjectClass( ObjectName )
{
	m_isCreated = true;
	m_outObjectInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVInputObjectListType;
	SVObjectManagerClass::Instance().ChangeUniqueObjectID(this, ObjectIdEnum::InputObjectListId);
}

SVInputObjectList::SVInputObjectList( SVObjectClass* POwner, int StringResourceID ) : SVObjectClass( POwner, StringResourceID )
{
	m_isCreated = true;
	m_outObjectInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVInputObjectListType;
	SVObjectManagerClass::Instance().ChangeUniqueObjectID(this, ObjectIdEnum::InputObjectListId);
}

SVInputObjectPtr SVInputObjectList::GetInput(uint32_t inputID) const
{
	SVInputObjectPtr pResult;

	std::lock_guard<std::mutex> guard(m_protectInputObjectList);

	const auto iter = m_inputObjectMap.find(inputID);
	if (iter != m_inputObjectMap.end())
	{
		pResult = iter->second;
	}

	return pResult;
}

SVInputObjectPtr SVInputObjectList::GetInput(const std::string& rInputName) const
{
	return findInputName(rInputName);
}

SVInputObjectPtr SVInputObjectList::GetInputFlyweight(const std::string& rInputName, SvPb::SVObjectSubTypeEnum ObjectSubType, int index)
{
	SVInputObjectPtr pResult = findInputName(rInputName);

	if (nullptr == pResult)
	{
		switch (ObjectSubType)
		{
		case SvPb::SVDigitalInputObjectType:
		{
			pResult = std::make_shared<SVDigitalInputObject>();
			pResult->updateObjectId(index);
			break;
		}
		case SvPb::SVRemoteInputObjectType:
		{
			pResult = std::make_shared<SVRemoteInputObject>();
			pResult->updateObjectId(index);
			break;
		}
		case SvPb::SVCameraDataInputObjectType:
			pResult = std::make_shared<SVCameraDataInputObject>();
			break;
		}

		if (nullptr != pResult)
		{
			pResult->SetName(rInputName.c_str());

			if (S_OK != AttachInput(pResult))
			{
				pResult.reset();
			}
		}
	}

	return pResult;
}

HRESULT SVInputObjectList::AttachInput(SVInputObjectPtr pInput)
{
	HRESULT l_Status = S_OK;

	if( nullptr != pInput)
	{
		std::lock_guard<std::mutex> guard(m_protectInputObjectList);

		auto iter = m_inputObjectMap.find( pInput->getObjectId() );

		if(iter == m_inputObjectMap.end())
		{
			m_inputObjectMap[pInput->getObjectId()] = pInput;
		}
	}
	else
	{
		l_Status = E_POINTER;
	}

	return l_Status;
}

HRESULT SVInputObjectList::DetachInput(uint32_t inputID)
{
	std::lock_guard<std::mutex> guard(m_protectInputObjectList);

	auto iter = m_inputObjectMap.find(inputID);
	if(iter != m_inputObjectMap.end())
	{
		m_inputObjectMap.erase(iter);
	}

	return S_OK;
}

bool SVInputObjectList::ReadInputs(const SVIOEntryHostStructPtrVector& rInputs, std::vector<_variant_t>& rInputValues)
{
	size_t inputSize( rInputs.size() );

	rInputValues.resize(inputSize);

	bool isFirstDIO = true;

	for(size_t i = 0; i < inputSize; ++i)
	{
		const SVIOEntryHostStructPtr& pIOEntry = rInputs[i];

		// Check if output is enabled for this call
		if(nullptr != pIOEntry && pIOEntry->m_Enabled )
		{
			SVInputObjectPtr pInput;
		
			{
				std::lock_guard<std::mutex> guard(m_protectInputObjectList);
				auto iter = m_inputObjectMap.find(pIOEntry->m_IOId);
				if(iter != m_inputObjectMap.end())
				{
					pInput = iter->second;
				}
			}

			if(nullptr != pInput)
			{
				if (isFirstDIO && SvPb::SVDigitalInputObjectType == pInput->GetObjectSubType())
				{
					isFirstDIO = false;
					SVIOConfigurationInterfaceClass::Instance().readDigitalInputBatch();
				}
				pInput->Read(rInputValues[i]);
			}
			else
			{
				rInputValues[i].Clear();
			}
		}
	}

	SVIOConfigurationInterfaceClass::Instance().clearDigitalInputBatch();		

	return true;
}

SVIOEntryHostStructPtrVector SVInputObjectList::getInputList() const
{
	SVIOEntryHostStructPtrVector result;

	result.reserve(m_inputObjectMap.size());
	std::lock_guard<std::mutex> guard(m_protectInputObjectList);
	for (const auto& rInput : m_inputObjectMap)
	{
		SVIOEntryHostStructPtr pIOEntry = std::make_shared<SVIOEntryHostStruct>();

		pIOEntry->m_IOId = rInput.second->getObjectId();

		switch (rInput.second->GetObjectSubType())
		{
		case SvPb::SVDigitalInputObjectType:
			pIOEntry->m_ObjectType = IO_DIGITAL_INPUT;
			break;
		case SvPb::SVRemoteInputObjectType:
			pIOEntry->m_ObjectType = IO_REMOTE_INPUT;
			break;
		case SvPb::SVCameraDataInputObjectType:
			pIOEntry->m_ObjectType = IO_CAMERA_DATA_INPUT;
			break;
		default:
			break;
		}
		result.emplace_back(pIOEntry);
	}

	return result;
}

long SVInputObjectList::getRemoteInputCount() const
{
	return static_cast<long> (std::count_if(m_inputObjectMap.begin(), m_inputObjectMap.end(), [](const auto& rEntry)->bool
	{
		return (nullptr != rEntry.second) && SvPb::SVRemoteInputObjectType == rEntry.second->GetObjectSubType();
	}));
}

bool SVInputObjectList::SetRemoteInput( long index, const _variant_t& rValue)
{
	bool result{false};


	SVRemoteInputObject* pInput{nullptr};

	std::lock_guard<std::mutex> guard(m_protectInputObjectList);
	const auto iter = std::find_if(m_inputObjectMap.begin(), m_inputObjectMap.end(), [&](const auto& rEntry)->bool
	{
		pInput = dynamic_cast<SVRemoteInputObject*> (nullptr != rEntry.second ? rEntry.second.get() : nullptr);

		return (nullptr != pInput) && (index == pInput->GetChannel());
	});

	if(iter != m_inputObjectMap.end() && nullptr != pInput)
	{
		result = (S_OK == pInput->writeCache(rValue));
	}

	return result;
}

SVInputObjectPtr SVInputObjectList::findInputName(const std::string& rInputName) const
{
	SVInputObjectPtr pResult;

	std::lock_guard<std::mutex> guard(m_protectInputObjectList);
	const auto iter = std::find_if(m_inputObjectMap.begin(), m_inputObjectMap.end(), [&rInputName](const auto& rEntry)->bool
	{
		return (nullptr != rEntry.second) && (rInputName == rEntry.second->GetName());
	});

	if (iter != m_inputObjectMap.end())
	{
		pResult = iter->second;
	}
	return pResult;
}
