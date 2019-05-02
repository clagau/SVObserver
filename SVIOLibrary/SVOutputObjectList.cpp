//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#include "SVOutputObjectList.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVValueObjectLibrary/SVValueObject.h"
#include "SVDigitalOutputObject.h"
#include "SVRemoteOutputObject.h"
#include "SVIOEntryStruct.h"
#include "SVInputObjectList.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVMessage\SVMessage.h"
#include "Definitions/TextDefineSvDef.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

SVOutputObjectList::SVOutputObjectList( LPCSTR ObjectName )
: SVObjectClass( ObjectName ), m_bCreated( false )
{
}

SVOutputObjectList::SVOutputObjectList( SVObjectClass* POwner, int StringResourceID )
: SVObjectClass( POwner, StringResourceID ), m_bCreated( false )
{
}

SVOutputObjectList::~SVOutputObjectList()
{
	if( m_bCreated )
	{
		Destroy();
	}
}

bool SVOutputObjectList::Create()
{
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVOutputObjectListType;

	try
	{
		m_bCreated = (TRUE == ::InitializeCriticalSectionAndSpinCount( &m_hCriticalSection, 4000 ));
	}
	catch(...)
	{
		m_bCreated = false;
	}

	return m_bCreated;
}// end Create

void SVOutputObjectList::Destroy()
{
	try
	{
		if( m_bCreated )
		{
			Lock();

			ClearOutputList();

			Unlock();

			m_bCreated = false;

			::DeleteCriticalSection( &m_hCriticalSection );
		}// end if
	}
	catch(...)
	{
	}
}// end Destroy

SVOutputObject* SVOutputObjectList::GetOutput(const SVGUID& rOutputID ) const
{
	SVOutputObject* pResult(nullptr);

	if (Lock())
	{
		SVGuidSVOutputObjectPtrMap::const_iterator l_Iter = m_OutputObjects.find(rOutputID);

		if (l_Iter != m_OutputObjects.end())
		{
			pResult = l_Iter->second;
		}
		Unlock();
	}

	return pResult;
}

SVOutputObject* SVOutputObjectList::GetOutput(const std::string& rName) const
{
	SVOutputObject* pResult(nullptr);

	if (Lock())
	{
		SVGuidSVOutputObjectPtrMap::const_iterator l_Iter = m_OutputObjects.begin();

		while (nullptr == pResult && l_Iter != m_OutputObjects.end())
		{
			SVOutputObject* pOutput = l_Iter->second;

			if (nullptr != pOutput && rName == pOutput->GetName())
			{
				pResult = pOutput;
			}

			++l_Iter;
		}

		Unlock();
	}

	return pResult;
}

SVOutputObject* SVOutputObjectList::GetOutputFlyweight(const std::string& rName, SvPb::SVObjectSubTypeEnum ObjectSubType, int GuidIndex)
{
	SVOutputObject* pResult(nullptr);

	if (Lock())
	{
		pResult = GetOutput(rName);

		if (nullptr == pResult)
		{
			switch (ObjectSubType)
			{
			case SvPb::SVDigitalOutputObjectType:
			{
				SVDigitalOutputObject* pDigOutput = new SVDigitalOutputObject;
				pDigOutput->updateGuid(GuidIndex);
				pResult = pDigOutput;
				break;
			}
			case SvPb::SVRemoteOutputObjectType:
				pResult = new SVRemoteOutputObject;
				break;
			}

			if (nullptr != pResult)
			{
				pResult->SetName(rName.c_str());

				if(S_OK != AttachOutput(pResult))
				{
					delete pResult;
					pResult = nullptr;
				}
			}
		}

		Unlock();
	}

	return pResult;
}

HRESULT SVOutputObjectList::AttachOutput( SVOutputObject *pOutput )
{
	HRESULT l_Status = S_OK;

	if( nullptr != pOutput )
	{
		if( Lock() )
		{
			SVGuidSVOutputObjectPtrMap::const_iterator l_Iter = m_OutputObjects.find( pOutput->GetUniqueObjectID() );

			if( l_Iter == m_OutputObjects.end() )
			{
				m_OutputObjects[ pOutput->GetUniqueObjectID() ] = pOutput;
			}

			Unlock();
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

HRESULT SVOutputObjectList::DetachOutput( const SVGUID& rOutputID )
{
	HRESULT l_Status = S_OK;

	if( Lock() )
	{
		SVGuidSVOutputObjectPtrMap::iterator l_Iter = m_OutputObjects.find( rOutputID );

		if( l_Iter != m_OutputObjects.end() )
		{
			SVOutputObject* l_pOutput = l_Iter->second;

			m_OutputObjects.erase( l_Iter );

			if( nullptr != l_pOutput )
			{
				delete l_pOutput;
			}
		}

		Unlock();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

GuidVariantPairVector SVOutputObjectList::getOutputValues(const SVIOEntryHostStructPtrVector& rIOEntries, bool useDefaults, bool ACK, bool NAK)
{
	GuidVariantPairVector Result;

	if( Lock() )
	{
		for(const auto& pIOEntry :rIOEntries)
		{
			// Check if output is enabled for this call (This is to filter outputs which are set later eg. Data Valid and Output toggle
			if(nullptr != pIOEntry && pIOEntry->m_Enabled )
			{
				std::pair<GUID, _variant_t> OutputValue;

				if( pIOEntry->m_ObjectType == IO_DIGITAL_OUTPUT )
				{
					OutputValue = getDigitalOutputValue(pIOEntry, useDefaults, ACK, NAK);
				}// end if
				else if( pIOEntry->m_ObjectType == IO_REMOTE_OUTPUT )
				{
					if( nullptr != pIOEntry->getValueObject())
					{
						OutputValue.first = pIOEntry->m_IOId;
						if(useDefaults)
						{
							OutputValue.second = pIOEntry->getValueObject()->getDefaultValue();
						}
						else
						{
							pIOEntry->getValueObject()->getValue(OutputValue.second);
						}
					}
				}

				if(GUID_NULL != OutputValue.first && VT_EMPTY != OutputValue.second.vt)
				{
					Result.push_back(OutputValue);
				}
			}
		}

		Unlock();
	}

	return Result;
}

bool SVOutputObjectList::ResetOutputs( SVIOEntryHostStructPtrVector& rIOEntries )
{
	if( Lock() )
	{
		for( size_t i = 0; i < rIOEntries.size(); i++ )
		{
			SVIOEntryHostStructPtr pIOEntry = rIOEntries[i];

			// Check if output is enabled for this call
			if(nullptr != pIOEntry && pIOEntry->m_Enabled )
			{
				SVOutputObject* pOutput = GetOutput(pIOEntry->m_IOId);

				if(nullptr != pOutput)
				{
					pOutput->Reset();
				}
			}
		}

		Unlock();

		return true;
	}// end if

	return false;
}// end ResetOutputs

bool SVOutputObjectList::WriteOutputs(const GuidVariantPairVector& rOutputValues)
{
	bool Result(false);

	if (Lock())
	{
		for(const auto& rOutputValue : rOutputValues)
		{
			SVOutputObject* pOutput = GetOutput(rOutputValue.first);
			if(nullptr != pOutput)
			{
				Result = (S_OK == pOutput->Write(rOutputValue.second));
			}
		}
		Unlock();

	}

	return Result;
}

bool SVOutputObjectList::WriteOutput( SVIOEntryHostStructPtr pIOEntry, bool ACK, bool NAK )
{
	bool Result( false );
	_variant_t OutputValue;

	if( Lock() )
	{

		// We are only writing one output, don't worry if it is marked enabled for this call
		if(nullptr != pIOEntry)
		{
			if( pIOEntry->m_ObjectType == IO_DIGITAL_OUTPUT )
			{
				std::pair<GUID, _variant_t> ValueOutput = getDigitalOutputValue(pIOEntry, false, ACK, NAK);

				SVOutputObject* pOutput = GetOutput(ValueOutput.first);
				if (nullptr != pOutput)
				{
					Result = (S_OK == pOutput->Write(ValueOutput.second));
				}
			}// end if
			else if( pIOEntry->m_ObjectType == IO_REMOTE_OUTPUT )
			{
				SVOutputObject* pOutput = nullptr;

				SVGuidSVOutputObjectPtrMap::iterator l_Iter = m_OutputObjects.find( pIOEntry->m_IOId );

				if( l_Iter != m_OutputObjects.end() )
				{
					pOutput	= l_Iter->second;
				}
				
				if( nullptr != pIOEntry->getValueObject())
				{
					pIOEntry->getValueObject()->getValue(OutputValue);
				}

				Result = (S_OK == pOutput->Write( &OutputValue ));
			}// end else if

		}// end if

		Unlock();

	}// end if

	return Result;
}// end WriteOutput

bool SVOutputObjectList::WriteOutputValue( SVIOEntryHostStructPtr pIOEntry, const _variant_t& p_rValue )
{

	if( Lock() )
	{
		// We are only writing one output, don't worry if it is marked enabled for this call
		if(nullptr != pIOEntry)
		{
			SVOutputObject* pOutput = GetOutput(pIOEntry->m_IOId);
			
			if (SV_IS_KIND_OF(pOutput, SVDigitalOutputObject) || SV_IS_KIND_OF(pOutput, SVRemoteOutputObject))
			{
				pOutput->Write(p_rValue);
			}// end if
		}

		Unlock();

		return true;
	}// end if

	return false;
}

bool SVOutputObjectList::ResetOutput( SVIOEntryHostStructPtr pIOEntry )
{
	if( Lock() )
	{
		// Check if output is enabled for this call
		if(nullptr != pIOEntry)
		{
			SVOutputObject* pOutput = GetOutput(pIOEntry->m_IOId);
			if (nullptr != pOutput)
			{
				bool l_bEnable = pIOEntry->m_Enabled;
				if (!l_bEnable)	// Act as if enabled when Module Ready or Raid Error 
				{
					std::string Name = pOutput->GetName();
					if (SvDef::cModuleReady == Name || SvDef::cRaidErrorIndicator == Name)
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

		Unlock();

		return true;
	}// end if

	return false;
}// end ResetOutput


bool SVOutputObjectList::FillOutputs( SVIOEntryHostStructPtrVector& rIOEntries )
{
	bool Result(false);
	rIOEntries.clear();

	if( Lock() )
	{
		auto Iter = m_OutputObjects.cbegin();
		Result = true;
		for( ;Iter != m_OutputObjects.end(); ++Iter )
		{
			if (nullptr != Iter->second)
			{
				SVIOEntryHostStructPtr pIOEntry{ new SVIOEntryHostStruct };

				pIOEntry->m_IOId = Iter->second->GetUniqueObjectID();

				switch (Iter->second->GetObjectSubType())
				{
				case SvPb::SVDigitalOutputObjectType:
					pIOEntry->m_ObjectType = IO_DIGITAL_OUTPUT;
					break;
				case SvPb::SVRemoteOutputObjectType:
					pIOEntry->m_ObjectType = IO_REMOTE_OUTPUT;
					break;
				default:
					Result = false;
					break;
				}

				rIOEntries.push_back(pIOEntry);
			}
		}

		Unlock();
	}// end if

	return Result;
}// end FillOutputs

bool SVOutputObjectList::RenameInspection( LPCTSTR OldInspection, LPCTSTR NewInspectionName)
{
	bool l_bRet = false;

	for(const auto rOutput : m_OutputObjects)
	{
		SVOutputObject* pOutput = rOutput.second;

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

	if( Lock() )
	{
		// if sizes are the same, Loop through inspection names and check for differences.
		// if there is a difference, then search for the old name in the output object list.
		// once we find the oldname in the output object list, replace with the new name.
		SvDef::StringSet ValidObjects;

		for( size_t l_lIndex = 0 ; l_lIndex < rInspectionNames.size() ; l_lIndex++ )
		{
			// for each output object search for the inspection name in the new names.
			// if found
			const std::string& rInspection = rInspectionNames[l_lIndex];

			SVGuidSVOutputObjectPtrMap::const_iterator Iter = m_OutputObjects.begin();

			while( m_OutputObjects.end() != Iter  )
			{
				SVOutputObject* pOutput = Iter->second;

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
		if( !( m_OutputObjects.empty() ) )
		{
			SVGuidSVOutputObjectPtrMap::iterator l_Iter = m_OutputObjects.begin();

			while( l_Iter != m_OutputObjects.end() )
			{
				SVOutputObject* pOutput = l_Iter->second;

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
					for(SvDef::StringVector::const_iterator it = rPPQNames.begin() ; it != rPPQNames.end() ; ++it)
					{
						if( 0 == OutputName.find( *it ) )
						{
							l_Skip = true;
							break;
						}
					}

					l_Skip = l_Skip || (SvDef::cModuleReady == OutputName );
					l_Skip = l_Skip || (SvDef::cRaidErrorIndicator == OutputName );

					if( ! l_Skip )
					{
						SvDef::StringSet::const_iterator l_OkIter = ValidObjects.find( OutputName );

						Remove = ( l_OkIter == ValidObjects.end() );
						Remove = Remove || OutputIsNotValid( pOutput->GetName() );
					}
				}

				if( Remove )
				{
					l_Iter = m_OutputObjects.erase( l_Iter );

					if( nullptr != pOutput )
					{
						delete pOutput;
					}
				}
				else
				{
					if( nullptr != pOutput )
					{
						SVDigitalOutputObject* l_pDigital = dynamic_cast< SVDigitalOutputObject* >( pOutput );

						if( nullptr != l_pDigital )
						{
							if( 0 <= l_pDigital->GetChannel() && l_pDigital->GetChannel() <= 15 )
							{
								l_ChannelOutputMap[ l_pDigital->GetChannel() ].push_back( l_pDigital );
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

		Unlock();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

bool SVOutputObjectList::OutputIsNotValid( std::string Name )
{
	bool l_bRet = true;
	
	SVObjectClass* l_pObject = nullptr;
	SVObjectManagerClass::Instance().GetObjectByDottedName( Name, l_pObject );
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

void SVOutputObjectList::OnObjectRenamed(const SVObjectClass& rRenamedObject, const std::string& rOldName)
{
	if( Lock() )
	{

		// Search the list of outputs
		SVGuidSVOutputObjectPtrMap::iterator l_Iter = m_OutputObjects.begin();

		while( l_Iter != m_OutputObjects.end() )
		{
			SVOutputObject* l_pOutput = l_Iter->second;

			std::string CurrentName = l_pOutput->GetName();
			size_t Pos = CurrentName.find( rOldName );

			if( std::string::npos != Pos )
			{
				std::string NewName = SvUl::Left( CurrentName, Pos ) + rRenamedObject.GetName();
				std::string Root  = NewName;
				NewName += SvUl::Mid( CurrentName, Pos + rOldName.size() );

				if( Root == rRenamedObject.GetCompleteName() )
				{
					l_pOutput->SetName( NewName.c_str() );
				}// end if

			}// end if
			++l_Iter;
		}// end for

		Unlock();
	}
}

std::pair<GUID, _variant_t>  SVOutputObjectList::getDigitalOutputValue(const SVIOEntryHostStructPtr& pIOEntry, bool useDefault, bool ACK, bool NAK )
{
	std::pair<GUID, _variant_t> Result{GUID_NULL, _variant_t{}};
	SVOutputObject* pOutput = GetOutput(pIOEntry->m_IOId);

	if( nullptr != pOutput )
	{
		_variant_t Value {false};
		bool CombinedValue {false};
		SvOi::IValueObject* pValueObject = pIOEntry->getValueObject();

		if( !useDefault)
		{
			std::string l_String = pOutput->GetName();
			if( ACK || l_String.substr( 0, 3 ).compare( _T( "PPQ" ) ) == 0 )
			{
				if( nullptr != pValueObject )
				{
					pValueObject->getValue(Value);
				}
			}

			if( pOutput->IsCombined() )
			{
				CombinedValue = pOutput->GetCombinedValue() ? Value && ACK : Value || NAK;
			}
		}

		if(useDefault)
		{
			if( nullptr != pIOEntry->getValueObject())
			{
				Value = pIOEntry->getValueObject()->getDefaultValue();
			}
		}
		else if( pOutput->IsCombined() )
		{
			Value = CombinedValue;
		}
		else
		{
			if (nullptr != pValueObject)
			{
				pValueObject->getValue(Value);
			}
		}

		Result.first = pIOEntry->m_IOId;
		Result.second = Value;

#if defined (TRACE_THEM_ALL) || defined (TRACE_OUTPUT_VALUES)
		std::string DebugString = SvUl::Format(_T("%s, %c, %d\r\n"), pOutput->GetName(), bValue ? '1' : '0', lDataIndex);
		::OutputDebugString(DebugString.c_str());
#endif
	}

	return Result;
}

void SVOutputObjectList::ClearOutputList()
{
	if( !( m_OutputObjects.empty() ) )
	{
		SVGuidSVOutputObjectPtrMap::iterator l_Iter = m_OutputObjects.begin();

		while( l_Iter != m_OutputObjects.end() )
		{
			SVOutputObject* l_pOutput = l_Iter->second;

			l_Iter = m_OutputObjects.erase( l_Iter );

			delete l_pOutput;
		}
	}
}

bool SVOutputObjectList::Lock() const
{
	if (m_bCreated)
	{
		::EnterCriticalSection(const_cast<LPCRITICAL_SECTION>(&m_hCriticalSection));

		return true;
	}

	return false;
}

bool SVOutputObjectList::Unlock() const
{
	if (m_bCreated)
	{
		::LeaveCriticalSection(const_cast<LPCRITICAL_SECTION>(&m_hCriticalSection));

		return true;
	}

	return false;
}
