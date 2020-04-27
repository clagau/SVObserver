//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH /// All Rights Reserved 
/// \Author	Robert Yoho
//*****************************************************************************

#pragma region Includes
#include "Stdafx.h"
#include "SVArchiveRecordsArray.h"
#include "SVArchiveTool.h"
#include "ObjectInterfaces/IValueObject.h"
#include "SVObjectLibrary/SVObjectInfoStruct.h"
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
	for (auto& rRecord : m_vecRecords)
	{
		rRecord.DisconnectInputObject();
	}
	m_vecRecords.clear();
}

HRESULT SVArchiveRecordsArray::InitializeObjects(SVArchiveTool* pToolArchive, SvVol::SVStringValueObjectClass& rObject )	// use array capability of string vo
{
	HRESULT hr = S_OK;
	assert( nullptr != m_pArchiveTool );
	ClearArray();

	assert(m_pArchiveTool->GetInspectionInterface());

	DWORD setDisplayWidths;
	m_pArchiveTool->m_bvoFormatResults.GetValue(setDisplayWidths);

	DWORD totalWidth;
	m_pArchiveTool->m_dwArchiveResultsMinimumNumberOfCharacters.GetValue(totalWidth);
	DWORD decimals;
	m_pArchiveTool->m_dwArchiveResultsNumberOfDecimals.GetValue(decimals);

	int iSize = rObject.getResultSize();
	for (int i = 0; i < iSize; i++)
	{
		std::string Name;
		rObject.GetValue(Name, i);
		if (!Name.empty())
		{
			SVObjectReference ObjectRef;
			SVObjectManagerClass::Instance().GetObjectByDottedName(Name.c_str(), ObjectRef);
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

			m_vecRecords.emplace_back();

			if (nullptr == ObjectRef.getObject())
			{
#if defined (TRACE_THEM_ALL) || defined (TRACE_ARCHIVE)
				TRACE(_T("SVArchiveRecordsArray::InitializeObjects-ToolName=%s-ObjectName=%s\n"), m_pArchiveTool->GetCompleteName(), Name);
#endif
			}
			else
			{
				auto pValueObject = ObjectRef.getValueObject();
				m_vecRecords.back().InitArchiveRecord(pToolArchive, ObjectRef);

				if (nullptr != pValueObject)
				{
					if (setDisplayWidths)
					{
						pValueObject->setFixedWidthFormatString(totalWidth, decimals);
					}
					else
					{
						pValueObject->setStandardFormatString();
					}
				}
			}
		}
	}
	return hr;
}


void SVArchiveRecordsArray::resetStandardFormatStringsOfValueObjects()
{
	for (auto& record : m_vecRecords)
	{
		auto pValueObject = record.GetObjectReference().getValueObject();

		if (nullptr != pValueObject)
		{
			pValueObject->setStandardFormatString();
		}
	}
}


SvDef::StringVector SVArchiveRecordsArray::RemoveDisconnectedObject(const SVObjectInfoStruct& p_rInfoObject)
{
	SvDef::StringVector Result;
	auto func = [&Result, &p_rInfoObject](const auto& entry)
	{
		if (p_rInfoObject.getObjectId() == entry.m_svObjectReference.getObjectId())
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
	auto func = [](auto& entry)
	{
		entry.DisconnectInputObject();

		SvIe::SVImageClass* pImageObject = entry.GetImage();
		if (pImageObject)
		{
			entry.m_svObjectReference = pImageObject;
			entry.BuildFileName();
			entry.ConnectInputObject();

			return false;
		}
		else
		{
			return true; //delete object
		}
	};

	m_vecRecords.erase(std::remove_if(m_vecRecords.begin(), m_vecRecords.end(), func), m_vecRecords.end());
}


void SVArchiveRecordsArray::SetArchiveTool( SVArchiveTool* pTool )
{
	m_pArchiveTool = pTool;
	for ( auto& rRecord: m_vecRecords )
	{
		rRecord.Init(m_pArchiveTool);
	}
}

bool SVArchiveRecordsArray::WriteArchiveImageFiles(const SvTrc::ITriggerRecordRPtr& pTriggerRecord )
{
	assert( nullptr != m_pArchiveTool );
	bool bOk = true;

	for (auto& rRecord : m_vecRecords)
	{
		bOk = S_OK == rRecord.WriteImage(pTriggerRecord.get());
		if (!bOk)
		{
			break;
		}
	}
	
	return bOk;
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
	auto func = [](auto& entry)
	{
		entry.DisconnectInputObject();
		SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(entry.m_svObjectReference.getObjectId());

		bool bRecordOK = false;
		if (nullptr != pObject)
		{
			if (pObject != entry.m_svObjectReference.getObject())
			{
				long lArrayIndex = entry.m_svObjectReference.ArrayIndex();
				entry.m_svObjectReference = SVObjectReference(pObject);
				entry.m_svObjectReference.SetArrayIndex(lArrayIndex);
			}

			double Value;
			if (pObject->getValue(Value, 0) != SVMSG_SVO_33_OBJECT_INDEX_INVALID)
			{
				bRecordOK = true;
				entry.ConnectInputObject();
			}
		}

		return !bRecordOK;
	};

	m_vecRecords.erase(std::remove_if(m_vecRecords.begin(), m_vecRecords.end(), func), m_vecRecords.end());

	//
	// return the count of objects to archive.
	//
	return static_cast<int> (m_vecRecords.size());
}

void SVArchiveRecordsArray::BuildArchiveImageFilePaths()
{
	for (auto& rRecord : m_vecRecords)
	{
		rRecord.BuildArchiveImageFilePaths();
	}
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
		assert( nullptr != pValueObject );

		if ( nullptr != pValueObject )
		{
			std::string Temp;
			HRESULT hr = pValueObject->getValue( Temp, rRecord.m_svObjectReference.ArrayIndex());
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

void SVArchiveRecordsArray::DisconnectAllResultObjects()
{
	for (auto& rRecord : m_vecRecords)
	{
		rRecord.DisconnectInputObject();
	}
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

SVArchiveRecord& SVArchiveRecordsArray::push_back()
{
	m_vecRecords.emplace_back();
	return m_vecRecords.at( m_vecRecords.size() - 1 );
}
#pragma endregion Public Methods

} //namespace SvTo
