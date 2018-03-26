//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH /// All Rights Reserved 
/// \Author	Robert Yoho
//*****************************************************************************

#pragma region Includes
#include "Stdafx.h"
#include "SVArchiveRecordsArray.h"
#include "SVArchiveTool.h"
#include "SVInspectionProcess.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "ObjectInterfaces/IValueObject.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

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
	int nCount = static_cast< int >( m_vecRecords.size() );
	for (int i = 0; i < nCount; i++)
	{
		SVArchiveRecord* pRecord = m_vecRecords.at(i);
		if (pRecord)
		{
			//
			// Reset the first image 'index' to 1.
			//
			pRecord->m_lCountImages = 1L;
		}
	}
}

void SVArchiveRecordsArray::ClearArray()
{
	int nCount = static_cast< int >( m_vecRecords.size() );
	for (int i = 0; i < nCount; i++)
	{
		SVArchiveRecord* pRecord = m_vecRecords.at(i);
		if (pRecord)
		{
			//
			// Make sure object is 'disconnected' as registered input.
			//
			pRecord->DisconnectInputObject();
			delete pRecord;
		}
	}
	m_vecRecords.clear();
}


HRESULT SVArchiveRecordsArray::InitializeObjects(SVArchiveTool* pToolArchive, SVStringValueObjectClass& rObject )	// use array capability of string vo
{
	HRESULT hr = S_OK;
	ASSERT( nullptr != m_pArchiveTool );
	ClearArray();
	SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess *>(m_pArchiveTool->GetInspection());
	ASSERT( pInspection );

	int iSize = rObject.getResultSize();
	for ( int i = 0; i < iSize; i++ )
	{
		std::string Name;
		rObject.GetValue( Name, i );
		if ( !Name.empty() )
		{
			SVArchiveRecord* pArchiveRecord = new SVArchiveRecord;
			SVObjectReference ObjectRef;
			size_t Pos = Name.find('.');
			if( std::string::npos != Pos )	// This assumes that the first part of the dotted name is the inspection.
			{				// Build the object name with the current inspection name.
				std::string NewName = pToolArchive->GetInspection()->GetName();
				NewName += SvUl::Mid( Name, Pos );
				HRESULT hrGetObject = SVObjectManagerClass::Instance().GetObjectByDottedName( NewName.c_str(), ObjectRef );
				if( S_OK == hrGetObject )
				{
					if( NewName != Name )
					{
						// Set value with new inspection name.
						rObject.SetValue(NewName, i );
					}
				}
			}
			else
			{				// We should always find a dotted name here.
				ASSERT(false);
			}

			if ( nullptr == ObjectRef.getObject() )
			{
#if defined (TRACE_THEM_ALL) || defined (TRACE_ARCHIVE)
				TRACE( _T( "SVArchiveRecordsArray::InitializeObjects-ToolName=%s-ObjectName=%s\n" ), m_pArchiveTool->GetCompleteName(), Name );
#endif
			}
			else
			{
				pArchiveRecord->InitArchiveRecord( pToolArchive, ObjectRef );
			}
			m_vecRecords.push_back(pArchiveRecord);
		}
	}
	return hr;
}

void SVArchiveRecordsArray::ConvertStringToGuids( SVArchiveTool * pToolArchive, LPCTSTR Value )
{
	ClearArray();
	
	std::string Text = Value;
	
	bool bDone = false;
	while (!bDone)
	{
		std::string GuidText;
		size_t Pos = Text.find( _T('\n') );
		if( std::string::npos != Pos )
		{
			GuidText = SvUl::Left( Text, Pos );
		}
		else
		{
			GuidText = Text;
			bDone = true;             // we are done 
		}
		//
		// Adjust for next iteration.
		//
		Text = SvUl::Right( Text, Text.size() - Pos -1 );
		
		//
		// Convert string guid to guid structure
		//
		SVGUID objectGuid( _bstr_t(GuidText.c_str()) );
	
		//
		// The image record has a dotted name that needs to be 
		// associated with a pointer to an image later.
		//
		if ( GUID_NULL != objectGuid && !GuidText.empty())
		{
			SVArchiveRecord* pArchiveRecord = new SVArchiveRecord;
			SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject( objectGuid );
			pArchiveRecord->InitArchiveRecord( pToolArchive, SVObjectReference( pObject ) );
			m_vecRecords.push_back(pArchiveRecord);
		}
	}
}

SvDef::StringVector SVArchiveRecordsArray::RemoveDisconnectedObject( const SVObjectInfoStruct& p_rInfoObject )
{
	SvDef::StringVector Result;

	// Effective STL Item 9 p46: forward iteration / erase
	int nCount = static_cast< int >( m_vecRecords.size() );
	RecordsType::iterator iter;
	for ( iter = m_vecRecords.begin(); iter != m_vecRecords.end();  )
	{
		SVArchiveRecord* pImageRecord = *iter;
		if ( pImageRecord )
		{
			if( p_rInfoObject.getUniqueObjectID() == pImageRecord->m_svObjectReference.Guid() )
			{
				Result.push_back( pImageRecord->m_svObjectReference.GetCompleteName() );
				delete pImageRecord;
				iter = m_vecRecords.erase(iter);
			}
			else
			{
				++iter;
			}
		}
	}

	return Result;
}


/////////////////////////////////////////////////////////////////////////////
//
void SVArchiveRecordsArray::ValidateImageObjects()
{
	//
	// Validate the image objects to be archived.
	//
	// Effective STL Item 9 p46: forward iteration / erase
	int nCount = static_cast< int >( m_vecRecords.size() );
	RecordsType::iterator iter;
	for ( iter = m_vecRecords.begin(); iter != m_vecRecords.end(); )
	{
		SVArchiveRecord* pImageRecord = *iter;
		ASSERT(pImageRecord);
		pImageRecord->DisconnectInputObject();
		
		SVImageClass* pImageObject = pImageRecord->GetImage();
		if (pImageObject)
		{
			pImageRecord->m_svObjectReference = pImageObject;
			pImageRecord->BuildFileName();
			pImageRecord->ConnectInputObject();

			++iter;
		}
		else
		{
			//
			// Remove this image record from our list.
			//
			delete pImageRecord;
			iter = m_vecRecords.erase(iter);
		}
	}
}


void SVArchiveRecordsArray::SetArchiveTool( SVArchiveTool* pTool )
{
	m_pArchiveTool = pTool;
	RecordsType::iterator iter;
	for ( iter = m_vecRecords.begin(); iter != m_vecRecords.end(); ++iter )
	{
		(*iter)->Init(m_pArchiveTool);
	}
}

bool SVArchiveRecordsArray::WriteArchiveImageFiles( )
{
	ASSERT( nullptr != m_pArchiveTool );
	bool bOk = true;
	HRESULT	hr = 0;
	
	int nCount = static_cast< int >( m_vecRecords.size() );
	
	for ( int i = 0; bOk && i < nCount; i++ )
	{
		SVArchiveRecord* pImageRecord = m_vecRecords.at(i);
		bOk = bOk && (S_OK == pImageRecord->WriteImage());
	}// end for ( int i = 0; bOk && i < nCount; i++ )
	
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
	RecordsType::iterator iter;
	for ( iter = m_vecRecords.begin(); iter != m_vecRecords.end(); )
	{
		SVArchiveRecord* pResultRecord = *iter;
		pResultRecord->DisconnectInputObject();
		
		GUID guid = pResultRecord->m_svObjectReference.Guid();

		SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(guid);

		bool bRecordOK = false;
		if( nullptr != pObject )
		{
			if ( pObject != pResultRecord->m_svObjectReference.getObject() )
			{
				long lArrayIndex = pResultRecord->m_svObjectReference.ArrayIndex();
				pResultRecord->m_svObjectReference = SVObjectReference( pObject );
				pResultRecord->m_svObjectReference.SetArrayIndex( lArrayIndex );
			}

			double Value;
			if( pObject->getValue(Value, 0) != SVMSG_SVO_33_OBJECT_INDEX_INVALID )
			{
				bRecordOK = true;
				pResultRecord->ConnectInputObject();
				++iter;
			}
		}

		if ( !bRecordOK )
		{
			pResultRecord->m_svObjectReference = nullptr;
			iter = m_vecRecords.erase(iter);
		}
	}
	
	//
	// return the count of objects to archive.
	//
	return static_cast<int> (m_vecRecords.size());
}

std::string SVArchiveRecordsArray::BuildResultsArchiveString()
{
	std::string Result;
	
	bool bFirst = true;	
	int nCount = static_cast< int >( m_vecRecords.size() );
	for (int i = 0; i < nCount; i++)
	{
		SVArchiveRecord* pResultRecord = m_vecRecords.at(i);
		ASSERT(pResultRecord);
		//
		// We assume all archivable objects are Value Objects.
		//
		SvOi::IValueObject* pValueObject = pResultRecord->m_svObjectReference.getValueObject();
		assert( nullptr != pValueObject );

		if ( nullptr != pValueObject )
		{
			std::string Temp;
			HRESULT hr = pValueObject->getValue( Temp, pResultRecord->m_svObjectReference.ArrayIndex());
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
	int nCount = static_cast< int >( m_vecRecords.size() );
	for (int i = 0; i < nCount; i++)
	{
		SVArchiveRecord* pResultRecord = m_vecRecords.at(i);
		if ( pResultRecord )
		{
			pResultRecord->DisconnectInputObject();
		}
	}
}

HRESULT SVArchiveRecordsArray::AllocateBuffers( long lBufferSize )
{
	HRESULT hr = S_OK;
	int nCount = static_cast< int >( m_vecRecords.size() );
	for (int i = 0; i < nCount; i++)
	{
		SVArchiveRecord* pResultRecord = m_vecRecords.at(i);
		HRESULT hrRecord = pResultRecord->AllocateBuffers( lBufferSize );
		if ( S_OK == hr )
		{
			hr = hrRecord;
		}
	}
	return hr;
}

HRESULT SVArchiveRecordsArray::WriteImageQueue()
{
	HRESULT hr = S_OK;
	int nCount = static_cast< int >( m_vecRecords.size() );
	for (int i = 0; i < nCount; i++)
	{
		SVArchiveRecord* pResultRecord = m_vecRecords.at(i);
		HRESULT hrRecord = pResultRecord->WriteImageQueue();
		if ( S_OK == hr )
		{
			hr = hrRecord;
		}
	}
	return hr;
}

int SVArchiveRecordsArray::GetSize()
{
	return static_cast< int >( m_vecRecords.size() );
}

SVArchiveRecord* SVArchiveRecordsArray::GetAt(int i)
{
	return m_vecRecords.at(i);
}

int SVArchiveRecordsArray::Add( SVArchiveRecord* pRecord )
{
	m_vecRecords.push_back( pRecord );
	return static_cast< int >( m_vecRecords.size() - 1 );
}
#pragma endregion Public Methods