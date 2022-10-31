//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOutputObjectList
//* .File Name       : $Workfile:   SVOutputObjectList.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.7  $
//* .Check In Date   : $Date:   15 May 2014 11:21:44  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "PlcOutputObject.h"
#include "SVDigitalOutputObject.h"
#include "SVIOConfigurationInterfaceClass.h"
#include "SVIOParameterEnum.h"
#include "SVRemoteOutputObject.h"
#include "SVOutputObjectList.h"
#include "SVMessage/SVMessage.h"
#include "Definitions/TextDefinesSvDef.h"
#include "Definitions/StringTypeDef.h"
#include "ObjectInterfaces/IValueObject.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

constexpr uint8_t cPlcInvalid = 4;
constexpr uint8_t cPlcBad = 5;
constexpr uint8_t cPlcGood = 6;

SVOutputObjectList::SVOutputObjectList(LPCSTR ObjectName) : SVObjectClass( ObjectName )
{
	m_isCreated = true;
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVOutputObjectListType;
	SVObjectManagerClass::Instance().ChangeUniqueObjectID(this, ObjectIdEnum::OutputObjectListId);
}

SVOutputObjectList::SVOutputObjectList(SVObjectClass* POwner, int StringResourceID) : SVObjectClass( POwner, StringResourceID )
{
	m_isCreated = true;
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVOutputObjectListType;
	SVObjectManagerClass::Instance().ChangeUniqueObjectID(this, ObjectIdEnum::OutputObjectListId);
}

SVOutputObjectPtr SVOutputObjectList::GetOutput(uint32_t outputID) const
{
	SVOutputObjectPtr pResult;

	std::lock_guard<std::mutex> guard(m_protectOutputList);

	auto iter = m_outputObjectMap.find(outputID);
	if(iter != m_outputObjectMap.end())
	{
		pResult = iter->second;
	}

	return pResult;
}

SVOutputObjectPtr SVOutputObjectList::GetOutput(const std::string& rName) const
{
	return findOutputName(rName);
}

SVOutputObjectPtr SVOutputObjectList::GetOutputFlyweight(const std::string& rName, SvPb::SVObjectSubTypeEnum ObjectSubType, int index)
{
	SVOutputObjectPtr pResult = findOutputName(rName);

	if (nullptr == pResult)
	{
		switch (ObjectSubType)
		{
		case SvPb::SVDigitalOutputObjectType:
		{
			pResult = std::make_shared<SVDigitalOutputObject>();
			pResult->updateObjectId(index);
			break;
		}
		case SvPb::PlcOutputObjectType:
		{
			pResult = std::make_shared<PlcOutputObject>();
			pResult->updateObjectId(index);
			break;
		}
		case SvPb::SVRemoteOutputObjectType:
			pResult = std::make_shared<SVRemoteOutputObject>();
			break;
		}

		if (nullptr != pResult)
		{
			pResult->SetName(rName.c_str());

			if(S_OK != AttachOutput(pResult))
			{
				pResult.reset();
			}
		}
	}

	return pResult;
}

HRESULT SVOutputObjectList::AttachOutput(SVOutputObjectPtr pOutput)
{
	HRESULT l_Status = S_OK;

	if( nullptr != pOutput )
	{
		std::lock_guard<std::mutex> guard(m_protectOutputList);

		auto iter = m_outputObjectMap.find(pOutput->getObjectId());
		if(iter == m_outputObjectMap.end() )
		{
			m_outputObjectMap[pOutput->getObjectId()] = pOutput;
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

HRESULT SVOutputObjectList::DetachOutput(uint32_t outputID)
{
	HRESULT l_Status = S_OK;

	std::lock_guard<std::mutex> guard(m_protectOutputList);

	auto  iter = m_outputObjectMap.find(outputID);

	if(iter != m_outputObjectMap.end() )
	{
		m_outputObjectMap.erase(iter);
	}

	return l_Status;
}

ObjectIdVariantPairVector SVOutputObjectList::getOutputValues(const SVIOEntryHostStructPtrVector& rIOEntries, const std::vector<bool>& rOutputResult, bool useDefaults, bool ACK, bool NAK)
{
	ObjectIdVariantPairVector Result;

	for(const auto& pIOEntry :rIOEntries)
	{
		// Check if output is enabled for this call (This is to filter outputs which are set later eg. Data Valid and Output toggle
		if(nullptr != pIOEntry && pIOEntry->m_Enabled )
		{
			switch(pIOEntry->m_ObjectType)
			{
				case SVIOObjectType::IO_DIGITAL_OUTPUT:
				{
					std::pair<uint32_t, _variant_t> OutputValue = getDigitalOutputValue(pIOEntry, rOutputResult, useDefaults, ACK, NAK);
					if (SvDef::InvalidObjectId != OutputValue.first && VT_EMPTY != OutputValue.second.vt)
					{
						Result.push_back(OutputValue);
					}
					break;
				}
				case SVIOObjectType::IO_PLC_OUTPUT:
				{
					std::pair<uint32_t, _variant_t> OutputValue = getDigitalOutputValue(pIOEntry, rOutputResult, useDefaults, ACK, NAK);
					if (VT_UI1 == OutputValue.second.vt)
					{
						///This is to get the correct index for the respective PPQ
						long index = OutputValue.first % rOutputResult.size();
						if(Result.size() > 0)
						{
							if(rOutputResult.size() > index)
							{
								::SafeArrayPutElement(Result[0].second.parray, &index, static_cast<void*>(&OutputValue.second.bVal));
							}
						}
						else
						{
							_variant_t outputResults;
							SAFEARRAYBOUND arrayBound;
							arrayBound.lLbound = 0;
							arrayBound.cElements =static_cast<ULONG> (rOutputResult.size());
							outputResults.vt = VT_UI1 | VT_ARRAY;
							outputResults.parray = ::SafeArrayCreate(VT_UI1, 1, &arrayBound);
							memset(outputResults.parray->pvData, 0, rOutputResult.size() * sizeof(uint8_t));
							if (rOutputResult.size() > index)
							{
								::SafeArrayPutElement(outputResults.parray, &index, static_cast<void*>(&OutputValue.second.bVal));
							}
							Result.emplace_back(std::pair<uint32_t, _variant_t>(0UL, outputResults));
						}
					}
					break;
				}
				case SVIOObjectType::IO_REMOTE_OUTPUT:
				{
					if (nullptr != pIOEntry->getValueObject())
					{
						std::pair<uint32_t, _variant_t> OutputValue;
						OutputValue.first = pIOEntry->m_IOId;
						if (useDefaults)
						{
							OutputValue.second = pIOEntry->getValueObject()->getDefaultValue();
						}
						else
						{
							pIOEntry->getValueObject()->getValue(OutputValue.second);
						}
						if (SvDef::InvalidObjectId != OutputValue.first && VT_EMPTY != OutputValue.second.vt)
						{
							Result.push_back(OutputValue);
						}
					}
					break;
				}
				default:
					break;
			}
		}
	}

	return Result;
}

bool SVOutputObjectList::ResetOutputs(const SVIOEntryHostStructPtrVector& rIOEntries )
{
	for( size_t i = 0; i < rIOEntries.size(); i++ )
	{
		SVIOEntryHostStructPtr pIOEntry = rIOEntries[i];

		// Check if output is enabled for this call
		if(nullptr != pIOEntry && pIOEntry->m_Enabled )
		{
			SVOutputObjectPtr pOutput = GetOutput(pIOEntry->m_IOId);

			if(nullptr != pOutput)
			{
				pOutput->Reset();
			}
		}
	}
	return true;

}// end ResetOutputs

bool SVOutputObjectList::WriteOutputs(const ObjectIdVariantPairVector& rOutputValues)
{
	bool Result(false);

	for(const auto& rOutputValue : rOutputValues)
	{
		SVOutputObjectPtr pOutput = GetOutput(rOutputValue.first);
		if(nullptr != pOutput)
		{
			Result = (S_OK == pOutput->Write(rOutputValue.second));
		}
	}

	return Result;
}

bool SVOutputObjectList::WriteOutput( SVIOEntryHostStructPtr pIOEntry, bool ACK, bool NAK )
{
	bool Result( false );
	_variant_t OutputValue;

	// We are only writing one output, don't worry if it is marked enabled for this call
	if(nullptr != pIOEntry)
	{
		if( SVIOObjectType::IO_DIGITAL_OUTPUT == pIOEntry->m_ObjectType)
		{
			std::pair<uint32_t, _variant_t> ValueOutput = getDigitalOutputValue(pIOEntry, std::vector<bool>{}, false, ACK, NAK);

			SVOutputObjectPtr pOutput = GetOutput(ValueOutput.first);
			if (nullptr != pOutput)
			{
				Result = (S_OK == pOutput->Write(ValueOutput.second));
			}
		}// end if
		else if( SVIOObjectType::IO_REMOTE_OUTPUT == pIOEntry->m_ObjectType)
		{
			SVOutputObjectPtr pOutput;

			auto iter = m_outputObjectMap.find(pIOEntry->m_IOId);

			if(iter != m_outputObjectMap.end())
			{
				pOutput	= iter->second;
			}
				
			if( nullptr != pIOEntry->getValueObject())
			{
				pIOEntry->getValueObject()->getValue(OutputValue);
			}

			Result = (S_OK == pOutput->Write( &OutputValue ));
		}// end else if

	}// end if

	return Result;
}// end WriteOutput

bool SVOutputObjectList::WriteOutputValue( SVIOEntryHostStructPtr pIOEntry, const _variant_t& rValue )
{
	// We are only writing one output, don't worry if it is marked enabled for this call
	if(nullptr != pIOEntry)
	{
		SVOutputObjectPtr pOutput = GetOutput(pIOEntry->m_IOId);
		SvPb::SVObjectSubTypeEnum outputType = (nullptr != pOutput) ? pOutput->GetObjectSubType() : SvPb::SVNotSetSubObjectType;
		if (SvPb::SVDigitalOutputObjectType == outputType || SvPb::SVRemoteOutputObjectType  == outputType || SvPb::PlcOutputObjectType == outputType)
		{
			pOutput->Write(rValue);
		}// end if
	}

	return true;
}

bool SVOutputObjectList::ResetOutput( SVIOEntryHostStructPtr pIOEntry )
{
	// Check if output is enabled for this call
	if(nullptr != pIOEntry)
	{
		SVOutputObjectPtr pOutput = GetOutput(pIOEntry->m_IOId);
		if (nullptr != pOutput)
		{
			bool l_bEnable = pIOEntry->m_Enabled;
			if (!l_bEnable)	// Act as if enabled when Module Ready
			{
				std::string Name = pOutput->GetName();
				if (SvDef::cModuleReady == Name)
				{
					l_bEnable = true;
				}
			}

			if (l_bEnable)
			{
				pOutput->Reset();
			}
		}
	}// end if

	return true;
}// end ResetOutput


SVIOEntryHostStructPtrVector SVOutputObjectList::getOutputList() const
{
	SVIOEntryHostStructPtrVector result;

	result.reserve(m_outputObjectMap.size());
	std::lock_guard<std::mutex> guard(m_protectOutputList);
	for(const auto& rOutput : m_outputObjectMap)
	{
		if (nullptr != rOutput.second)
		{
			SVIOEntryHostStructPtr pIOEntry = std::make_shared<SVIOEntryHostStruct>();

			pIOEntry->m_IOId = rOutput.second->getObjectId();

			switch (rOutput.second->GetObjectSubType())
			{
			case SvPb::SVDigitalOutputObjectType:
				pIOEntry->m_ObjectType = SVIOObjectType::IO_DIGITAL_OUTPUT;
				break;
			case SvPb::PlcOutputObjectType:
				pIOEntry->m_ObjectType = SVIOObjectType::IO_PLC_OUTPUT;
				break;
			case SvPb::SVRemoteOutputObjectType:
				pIOEntry->m_ObjectType = SVIOObjectType::IO_REMOTE_OUTPUT;
				break;
			default:
				break;
			}

			result.push_back(pIOEntry);
		}
	}

	return result;
}

bool SVOutputObjectList::RenameInspection( LPCTSTR OldInspection, LPCTSTR NewInspectionName)
{
	bool l_bRet = false;

	for(const auto rOutput : m_outputObjectMap)
	{
		SVOutputObjectPtr pOutput = rOutput.second;

		std::string Name = pOutput->GetName();

		size_t Pos = Name.find('.');
		std::string InspectionName = SvUl::Left( Name, Pos );

		if( OldInspection == InspectionName )
		{
			std::string NewName = NewInspectionName + SvUl::Mid( Name, Pos );
			pOutput->SetName( NewName.c_str() );
			l_bRet = true;
		}
	}
	return l_bRet;
}


HRESULT SVOutputObjectList::RemoveUnusedOutputs( const SvDef::StringVector& rInspectionNames, const SvDef::StringVector& rPPQNames )
{
	typedef std::deque<SVDigitalOutputObject*> SVDigitalOutputPtrList;
	typedef std::map<long, SVDigitalOutputPtrList> SVDigitalOutputChannelMap;

	HRESULT l_Status = S_OK;

	std::lock_guard<std::mutex> guard(m_protectOutputList);
	// if sizes are the same, Loop through inspection names and check for differences.
	// if there is a difference, then search for the old name in the output object list.
	// once we find the oldname in the output object list, replace with the new name.
	SvDef::StringSet ValidObjects;

	for( size_t l_lIndex = 0 ; l_lIndex < rInspectionNames.size() ; l_lIndex++ )
	{
		// for each output object search for the inspection name in the new names.
		// if found
		const std::string& rInspection = rInspectionNames[l_lIndex];

		ObjectIdSVOutputObjectPtrMap::const_iterator Iter = m_outputObjectMap.begin();

		while( m_outputObjectMap.end() != Iter  )
		{
			SVOutputObjectPtr pOutput = Iter->second;

			std::string OutputName = pOutput->GetName();

			std::string ObjInspectionName;
			size_t Pos = OutputName.find('.');
			if( std::string::npos != Pos )
			{
				ObjInspectionName = SvUl::Left( OutputName, Pos );
			}

			if( rInspection == ObjInspectionName )
			{
				ValidObjects.insert( OutputName );
			}

			++Iter;
		}
	}
	SVDigitalOutputChannelMap l_ChannelOutputMap;

	// if the output object inspection is not found, then delete the entry.
	// but make sure it is not a ppq object.
	if( !( m_outputObjectMap.empty() ) )
	{
		ObjectIdSVOutputObjectPtrMap::iterator l_Iter = m_outputObjectMap.begin();

		while( l_Iter != m_outputObjectMap.end() )
		{
			SVOutputObjectPtr pOutput = l_Iter->second;

			std::string OutputName;

			if( nullptr != pOutput )
			{
				OutputName = pOutput->GetName();
			}

			bool Remove = OutputName.empty();

			if( !Remove )
			{
				bool l_Skip = false;

				// Check if the name starts with a valid PPQ_X prefix. Skip these outputs.
				for(const std::string& rName : rPPQNames)
				{
					if( 0 == OutputName.compare( 0, rName.length(), rName) )
					{
						l_Skip = true;
						break;
					}
				}

				l_Skip = l_Skip || (SvDef::cModuleReady == OutputName );

				if( ! l_Skip )
				{
					SvDef::StringSet::const_iterator l_OkIter = ValidObjects.find( OutputName );

					Remove = ( l_OkIter == ValidObjects.end() );
					Remove = Remove || OutputIsNotValid( pOutput->GetName() );
				}
			}

			if( Remove )
			{
				l_Iter = m_outputObjectMap.erase( l_Iter );
			}
			else
			{
				if( nullptr != pOutput )
				{
					SVDigitalOutputObject* pDigital = dynamic_cast<SVDigitalOutputObject*> (pOutput.get());

					if( nullptr != pDigital )
					{
						if( 0 <= pDigital->GetChannel() && pDigital->GetChannel() <= 15 )
						{
							l_ChannelOutputMap[ pDigital->GetChannel() ].push_back( pDigital );
						}
					}
				}

				++l_Iter;
			}
		}
	}

	SVDigitalOutputChannelMap::iterator l_ChannelIter = l_ChannelOutputMap.find( 15 );

	if( l_ChannelIter != l_ChannelOutputMap.end() )
	{
		if( 1 < l_ChannelIter->second.size() )
		{
			SVDigitalOutputPtrList::iterator l_OutputIter = l_ChannelIter->second.begin();

			while( l_OutputIter != l_ChannelIter->second.end() )
			{
				SVDigitalOutputObject* l_pOutput = *l_OutputIter;

				if( nullptr != l_pOutput && std::string(SvDef::cModuleReady ) == l_pOutput->GetName() )
				{
					l_pOutput->SetChannel( -1 );

					l_OutputIter = l_ChannelIter->second.erase( l_OutputIter );
				}
				else
				{
					++l_OutputIter;
				}
			}
		}
	}

	bool l_DescreteOutputProblem = false;

	for( l_ChannelIter = l_ChannelOutputMap.begin(); l_ChannelIter != l_ChannelOutputMap.end(); ++l_ChannelIter )
	{
		l_DescreteOutputProblem |= ( 1 < l_ChannelIter->second.size() );
	}

	if( l_DescreteOutputProblem )
	{
		l_Status = SVMSG_SVO_70_DUPLICATE_DISCRETE_OUTPUT;
	}

	return l_Status;
}

bool SVOutputObjectList::OutputIsNotValid(const std::string& rName )
{
	bool l_bRet = true;
	
	SVObjectClass* l_pObject = nullptr;
	SVObjectManagerClass::Instance().GetObjectByDottedName( rName, l_pObject );
	// Check if the object exists.
	if( nullptr != l_pObject )
	{
		if( l_pObject->ObjectAttributesSet() & SvPb::publishable )
		{
			l_bRet = false;
		}
	}
	// Return true if object does not exist or is not published.
	return l_bRet;
}


void SVOutputObjectList::WriteOutputData(unsigned long triggerChannel, const TriggerData& rData)
{
	std::lock_guard<std::mutex> guard(m_protectOutputList);
	SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputData(triggerChannel, rData);
}

void SVOutputObjectList::setModuleReady(bool value)
{
	_variant_t moduleReadyState{value};
	std::lock_guard<std::mutex> guard(m_protectOutputList);
	SVIOConfigurationInterfaceClass::Instance().SetParameterValue(SVModuleReady, &moduleReadyState.GetVARIANT());
}

	
void SVOutputObjectList::OnObjectRenamed(const SVObjectClass& rRenamedObject, const std::string& rOldName)
{
	std::lock_guard<std::mutex> guard(m_protectOutputList);

	// Search the list of outputs
	auto l_Iter = m_outputObjectMap.begin();

	while( l_Iter != m_outputObjectMap.end() )
	{
		SVOutputObjectPtr pOutput = l_Iter->second;

		std::string CurrentName = pOutput->GetName();
		size_t Pos = CurrentName.find( rOldName );

		if( std::string::npos != Pos )
		{
			std::string NewName = SvUl::Left( CurrentName, Pos ) + rRenamedObject.GetName();
			std::string Root  = NewName;
			NewName += SvUl::Mid( CurrentName, Pos + rOldName.size() );

			if( Root == rRenamedObject.GetCompleteName() )
			{
				pOutput->SetName( NewName.c_str() );
			}// end if

		}// end if
		++l_Iter;
	}// end for
}

std::pair<uint32_t, _variant_t>  SVOutputObjectList::getDigitalOutputValue(const SVIOEntryHostStructPtr& pIOEntry, const std::vector<bool>& rOutputResult, bool useDefault, bool ACK, bool NAK )
{
	std::pair<uint32_t, _variant_t> Result{ SvDef::InvalidObjectId, _variant_t{} };
	SVOutputObjectPtr pOutput = GetOutput(pIOEntry->m_IOId);

	if( nullptr != pOutput )
	{
		_variant_t Value{false};
		SvOi::IValueObject* pValueObject = pIOEntry->getValueObject();

		if(nullptr != pValueObject)
		{
			if (useDefault)
			{
				Value = pValueObject->getDefaultValue();
			}
			else if (ACK || (0 == pIOEntry->m_inspectionId))
			{
				if (0 == rOutputResult.size())
				{
					pValueObject->getValue(Value);
				}
				else
				{
					int outputIndex = pOutput->GetChannel() % rOutputResult.size();
					Value = rOutputResult[outputIndex];
				}
			}
		}

		if( pOutput->isCombined() )
		{
			if(SVIOObjectType::IO_DIGITAL_OUTPUT == pIOEntry->m_ObjectType)
			{
				Value = pOutput->isAndACK() ? Value && ACK : Value || NAK;
			}
			else if(SVIOObjectType::IO_PLC_OUTPUT == pIOEntry->m_ObjectType)
			{
				if(pOutput->isAndACK())
				{
					Value = ACK ? Value ? cPlcGood : cPlcBad : cPlcInvalid;
				}
				else
				{
					///As this is value or NAK if it is set then is bad
					Value = Value || NAK;
					Value = Value ? cPlcBad : cPlcGood;
				}
			}
		}
		else if(false == useDefault)
		{
			if (nullptr != pValueObject)
			{
				if (0 == rOutputResult.size())
				{
					pValueObject->getValue(Value);
				}
				else
				{
					int outputIndex = pOutput->GetChannel() % rOutputResult.size();
					Value = rOutputResult[outputIndex];
				}
			}
		}
		///Need to convert bool value to variant
		if (VT_BOOL == Value.vt && SVIOObjectType::IO_PLC_OUTPUT == pIOEntry->m_ObjectType)
		{
			bool outputState = Value ? true : false;
			Value.Clear();
			Value = outputState ? cPlcGood : cPlcBad;
		}
		///Discrete outputs has the first value the objectID while for PLC outputs its the channel number
		Result.first = SVIOObjectType::IO_PLC_OUTPUT == pIOEntry->m_ObjectType ? pOutput->GetChannel() : pIOEntry->m_IOId;
		Result.second = Value;

#if defined (TRACE_THEM_ALL) || defined (TRACE_OUTPUT_VALUES)
		std::string DebugString = std::format(_T("{}, {:c}, {}\r\n"), pOutput->GetName(), bValue ? '1' : '0', lDataIndex);
		::OutputDebugString(DebugString.c_str());
#endif
	}

	return Result;
}

SVOutputObjectPtr SVOutputObjectList::findOutputName(const std::string& rOutputName) const
{
	SVOutputObjectPtr pResult;

	std::lock_guard<std::mutex> guard(m_protectOutputList);
	const auto iter = std::find_if(m_outputObjectMap.begin(), m_outputObjectMap.end(), [&rOutputName](const auto& rEntry)->bool
	{
		return (nullptr != rEntry.second) && (rOutputName == rEntry.second->GetName());
	});

	if (iter != m_outputObjectMap.end())
	{
		pResult = iter->second;
	}
	return pResult;
}
