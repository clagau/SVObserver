//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by Körber Pharma Inspection GmbH. All Rights Reserved /// All Rights Reserved 
/// \Author	Robert Yoho
//*****************************************************************************

#pragma region Includes
#include "Stdafx.h"
#include "SVArchiveRecordsArray.h"
#include "SVArchiveTool.h"
#include "ObjectInterfaces/IValueObject.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVValueObjectLibrary/SVStringValueObjectClass.h"
#pragma endregion Includes

namespace SvTo
{

#pragma region Constructor
SVArchiveRecordsArray::SVArchiveRecordsArray()
{
	m_pArchiveTool = nullptr;
}

SVArchiveRecordsArray::~SVArchiveRecordsArray()
{
	ClearArray();
}
#pragma endregion Constructor

#pragma region Public Methods
void SVArchiveRecordsArray::ResetImageCounts()
{
	for (auto& rRecord : m_vecRecords)
	{
		rRecord.m_lCountImages = 0L;
	}
}

void SVArchiveRecordsArray::ClearArray()
{
	if (m_pArchiveTool)
	{
		//m_vecRecord can be changed with disconnectObject. So it must be copied before.
		auto tmpRecord = m_vecRecords;
		for (auto& rRecord : tmpRecord)
		{
			auto* pObject = rRecord.GetObjectReference().getObject();
			if (pObject)
			{
				pObject->disconnectObject(m_pArchiveTool->getObjectId());
			}
		}
	}
	m_vecRecords.clear();
}

HRESULT SVArchiveRecordsArray::InitializeObjects(SvVol::SVStringValueObjectClass& rObject )	// use array capability of string vo
{
	HRESULT hr = S_OK;
	Log_Assert( nullptr != m_pArchiveTool );
	ClearArray();

	Log_Assert(m_pArchiveTool->GetInspectionInterface());

	int iSize = rObject.getResultSize();
	for (int i = 0; i < iSize; i++)
	{
		std::string Name;
		rObject.GetValue(Name, i);
		if (!Name.empty())
		{
			std::string NewName = AdaptDottedNameToInspectionName(Name);
			SVObjectReference ObjectRef;
			SVObjectManagerClass::Instance().GetObjectByDottedName(NewName.c_str(), ObjectRef);
			size_t Pos = Name.find('[');
			if (std::string::npos != Pos)
			{	//Array brackets found
				if (!ObjectRef.isArray() && 0 < ObjectRef.GetObjectNameInfo().GetIndexValue())
				{
					//delete entry
					continue;
				}
			}
			else
			{
				if (ObjectRef.isArray())
				{
					Name += _T("[1]");
					ObjectRef.SetArrayIndex(0);
				}
			}

			if (nullptr == ObjectRef.getObject())
			{
				std::string msg = std::format("SVArchiveRecordsArray::InitializeObjects-ToolName={}-ObjectName={}\n", m_pArchiveTool->GetCompleteName(), Name);
#if defined  _DEBUG
				OutputDebugString(msg.c_str());
#endif
				SvStl::MessageManager Exception(SvStl::MsgType::Log);
				SvDef::StringVector msgList;
				msgList.push_back(msg);
				Exception.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams));
			}
			else
			{
				m_vecRecords.emplace_back().InitArchiveRecord(m_pArchiveTool, ObjectRef);
			}
		}
	}
	return hr;
}


SvDef::StringVector SVArchiveRecordsArray::RemoveDisconnectedObject(uint32_t objectId)
{
	SvDef::StringVector Result;
	auto func = [&Result, &objectId](const auto& entry)
	{
		if (objectId == entry.m_svObjectReference.getObjectId())
		{
			Result.emplace_back(entry.m_svObjectReference.GetCompleteName());
			return true;
		}
		return false;
	};

	m_vecRecords.erase(std::remove_if(m_vecRecords.begin(), m_vecRecords.end(), func), m_vecRecords.end());

	return Result;
}


/////////////////////////////////////////////////////////////////////////////
//
void SVArchiveRecordsArray::ValidateImageObjects()
{
	auto func = [this](auto& entry)
	{
		auto& rRef = entry.GetObjectReference();
		auto* pObject = rRef.getObject();
		auto* pImageObject = dynamic_cast<SvIe::SVImageClass*>(rRef.getFinalObject());
		if (nullptr != pObject && nullptr != pImageObject)
		{
			if (nullptr == m_pArchiveTool || false == m_pArchiveTool->checkIfValidDependency(pObject))
			{
				return true;
			}

			entry.m_svObjectReference = rRef;
			entry.BuildImageFileName();
			pObject->connectObject(m_pArchiveTool->getObjectId());
			return false;
		}
		else
		{
			return true; //delete object
		}
	};

	m_vecRecords.erase(std::remove_if(m_vecRecords.begin(), m_vecRecords.end(), func), m_vecRecords.end());
}

void SVArchiveRecordsArray::SetArchiveTool( SVArchiveTool* pArchiveTool)
{
	m_pArchiveTool = pArchiveTool;
	for ( auto& rRecord: m_vecRecords )
	{
		rRecord.SetArchiveTool(m_pArchiveTool);
	}
}

long SVArchiveRecordsArray::WriteAllArchiveImages(const SvOi::ITriggerRecordRPtr& pTriggerRecord )
{
	Log_Assert( nullptr != m_pArchiveTool );

	long queueLength = 0;

	for (auto& rRecord : m_vecRecords)
	{
		queueLength = rRecord.WriteArchiveImage(pTriggerRecord.get());
		if (queueLength < 0)
		{
			break;
		}
	}
	
	return queueLength;
}

/////////////////////////////////////////////////////////////////////////////
//
//
// Make sure the results objects to archive are valid.  
// 'Connect as input' the good ones and remove bad ones.
//
// Return the final count of results objects to archive.
//
int SVArchiveRecordsArray::ValidateResultsObjects()
{
	auto func = [this](auto& entry)
	{
		SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(entry.m_svObjectReference.getObjectId());

		if (nullptr != pObject)
		{
			if (nullptr == m_pArchiveTool || false == m_pArchiveTool->checkIfValidDependency(pObject))
			{
				return true;
			}
			if (pObject != entry.m_svObjectReference.getObject())
			{
				long lArrayIndex = entry.m_svObjectReference.ArrayIndex();
				entry.m_svObjectReference = SVObjectReference(pObject);
				entry.m_svObjectReference.SetArrayIndex(lArrayIndex);
			}

			double Value;
			if (pObject->getValue(Value, 0) != SVMSG_SVO_33_OBJECT_INDEX_INVALID)
			{
				pObject->connectObject(m_pArchiveTool->getObjectId());
				return false;
			}
		}
		
		return true;
	};
	auto newEndIter = std::remove_if(m_vecRecords.begin(), m_vecRecords.end(), func);
	//because disconnect can be changed m_vecRecords, the disconnect-method can be call only after m_vecRecords-changes finished.
	std::vector<SVObjectReference> refList;
	if (m_pArchiveTool)
	{
		for (auto iter = newEndIter; iter != m_vecRecords.end(); ++iter)
		{
			auto findIter = std::find_if(m_vecRecords.begin(), newEndIter,
				[newEndIter](const auto& rEntry) { return rEntry.m_svObjectReference.getObjectId() == newEndIter->m_svObjectReference.getObjectId(); });
			if (newEndIter == findIter)
			{
				refList.push_back(iter->GetObjectReference());
			}
		}
	}
	m_vecRecords.erase(newEndIter, m_vecRecords.end());
	if (m_pArchiveTool)
	{
		for (const auto& rRef : refList)
		{
			SVObjectClass* pObject = rRef.getObject();
			if (nullptr != pObject)
			{
				pObject->disconnectObject(m_pArchiveTool->getObjectId());
			}
		}
	}

	return static_cast<int> (m_vecRecords.size());
}

std::string SVArchiveRecordsArray::BuildResultsArchiveString()
{
	std::string Result;
	
	bool bFirst = true;	
	for (const auto& rRecord: m_vecRecords)
	{
		//
		// We assume all archivable objects are Value Objects.
		//
		SvOi::IValueObject* pValueObject = rRecord.m_svObjectReference.getValueObject();
		Log_Assert( nullptr != pValueObject );

		if ( nullptr != pValueObject )
		{
			std::string Temp;
			HRESULT hr = pValueObject->getValue( Temp, rRecord.m_svObjectReference.ArrayIndex(), rRecord.m_formatString);
			if ( S_OK == hr || SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == hr )
			{
				if ( bFirst )
				{
					bFirst = false;
				}
				else
				{
					Result += _T(", ");
				}

				Result += Temp;
			}
			else
			{
				Result += _T(", ");
			}
		}
	}

	return Result;
}

HRESULT SVArchiveRecordsArray::AllocateBuffers(long bufferNumber, BufferStructCountMap& rBufferMap, int toolPos)
{
	HRESULT hr = S_OK;
	for (auto& rRecord : m_vecRecords)
	{
		HRESULT hrRecord = rRecord.AllocateBuffers(bufferNumber, rBufferMap, toolPos);
		if (S_OK == hr)
		{
			hr = hrRecord;
		}
	}
	return hr;
}

HRESULT SVArchiveRecordsArray::WriteImageQueue()
{
	HRESULT hr = S_OK;
	for (auto& rRecord : m_vecRecords)
	{
		HRESULT hrRecord = rRecord.WriteImageQueue();
		if (S_OK == hr)
		{
			hr = hrRecord;
		}
	}
	return hr;
}

const std::vector <SVArchiveRecord>& SVArchiveRecordsArray::getRecordVec() const
{
	return m_vecRecords;
}

int SVArchiveRecordsArray::GetSize()
{
	return static_cast< int >( m_vecRecords.size() );
}

void SVArchiveRecordsArray::emplaceRecordAtBack(const SVObjectReference& rObjectRef)
{
	SVArchiveRecord& rArchiveRecord = m_vecRecords.emplace_back();

	rArchiveRecord.InitArchiveRecord(m_pArchiveTool, rObjectRef);
	rArchiveRecord.GetObjectReference() = rObjectRef;

	if (SvDef::InvalidObjectId != rObjectRef.getObjectId())
	{
		SVObjectClass* pObject{ SVObjectManagerClass::Instance().GetObject(rObjectRef.getObjectId()) };

		if (nullptr == dynamic_cast<SvOi::IValueObject*> (pObject) &&
			nullptr == dynamic_cast<SvIe::SVImageClass*>(pObject))
		{
			pObject = nullptr;
		}

		if (pObject != rObjectRef.getObject())
		{
			long lArrayIndex = rObjectRef.ArrayIndex();
			rArchiveRecord.m_svObjectReference = SVObjectReference{ pObject };
			rArchiveRecord.m_svObjectReference.SetArrayIndex(lArrayIndex);
		}

		if (m_pArchiveTool)
		{
			if (pObject)
			{
				pObject->connectObject(m_pArchiveTool->getObjectId());
			}
		}
	}
}

std::string SVArchiveRecordsArray::AdaptDottedNameToInspectionName(std::string DottedName)
{
	//If the tool set name is in the name it should be a name with inspection name.
	if (std::string::npos != DottedName.find(SvUl::LoadedStrings::g_ToolSetName))
	{
		SvOi::IObjectClass* pInspection = m_pArchiveTool->GetAncestorInterface(SvPb::SVInspectionObjectType);
		if (nullptr != pInspection)
		{
			size_t Pos = DottedName.find('.');
			if (std::string::npos != Pos)	// This assumes that the first part of the dotted name is the inspection.
			{	// Build the object name with the current inspection name.
				return pInspection->GetName() + SvUl::Mid(DottedName, Pos);
			}
			else
			{	// We should always find a dotted name here.
				Log_Assert(false);
			}
		}
	}

	return DottedName;
}
#pragma endregion Public Methods

} //namespace SvTo
