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


HRESULT SVArchiveRecordsArray::InitializeObjects(SVArchiveTool* p_pToolArchive, SVStringValueObjectClass& p_svoObjects )	// use array capability of string vo
{
	HRESULT hr = S_OK;
	ASSERT( nullptr != m_pArchiveTool );
	ClearArray();
	SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess *>(m_pArchiveTool->GetInspection());
	ASSERT( pInspection );

	int iSize = p_svoObjects.GetResultSize();
	for ( int i = 0; i < iSize; i++ )
	{
		SVString sName;
		long l_lIndex = p_svoObjects.GetLastSetIndex();
		p_svoObjects.GetValue( l_lIndex, i, sName );
		if ( !sName.empty() )
		{
			SVArchiveRecord* pArchiveRecord = new SVArchiveRecord;
			SVObjectReference ref;
			size_t Pos = sName.find('.');
			if( SVString::npos != Pos )	// This assumes that the first part of the dotted name is the inspection.
			{				// Build the object name with the current inspection name.
				SVString sNewName = p_pToolArchive->GetInspection()->GetName();
				sNewName += SvUl_SF::Mid( sName, Pos );
				HRESULT hrGetObject = SVObjectManagerClass::Instance().GetObjectByDottedName( sNewName.c_str(), ref );
				if( S_OK == hrGetObject )
				{
					/// names in m_svoArchiveResultNames are zero based!!!
					ref.IncrementIndex();

					if( sNewName != sName )
					{
						// Set value with new inspection name.
						p_svoObjects.SetValue(l_lIndex, i, sNewName);
					}
				}
			}
			else
			{				// We should always find a dotted name here.
				ASSERT(FALSE);
			}

			if ( nullptr == ref.Object() )
			{
#if defined (TRACE_THEM_ALL) || defined (TRACE_ARCHIVE)
				TRACE( _T( "SVArchiveRecordsArray::InitializeObjects-ToolName=%s-ObjectName=%s\n" ), m_pArchiveTool->GetCompleteName(), sName );
#endif
			}
			else
			{
				pArchiveRecord->InitArchiveRecord( p_pToolArchive, ref );
			}
			m_vecRecords.push_back(pArchiveRecord);
		}
	}
	return hr;
}

void SVArchiveRecordsArray::ConvertStringToGuids( SVArchiveTool * pToolArchive, LPCTSTR Value )
{
	ClearArray();
	
	SVString Text = Value;
	
	BOOL bDone = false;
	while (!bDone)
	{
		SVString GuidText;
		size_t Pos = Text.find( _T('\n') );
		if( SVString::npos != Pos )
		{
			GuidText = SvUl_SF::Left( Text, Pos );
		}
		else
		{
			GuidText = Text;
			bDone = true;             // we are done 
		}
		//
		// Adjust for next iteration.
		//
		Text = SvUl_SF::Right( Text, Text.size() - Pos -1 );
		
		//
		// Convert string guid to guid structure
		//
		SVGUID objectGuid( _bstr_t(GuidText.c_str()) );
	
		//
		// The image record has a dotted name that needs to be 
		// associated with a pointer to an image later.
		//
		if ( SV_GUID_NULL != objectGuid && !GuidText.empty())
		{
			SVArchiveRecord* pArchiveRecord = new SVArchiveRecord;
			SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject( objectGuid );
			pArchiveRecord->InitArchiveRecord( pToolArchive, SVObjectReference( pObject ) );
			m_vecRecords.push_back(pArchiveRecord);
		}
	}
}

SVStringVector SVArchiveRecordsArray::RemoveDisconnectedObject( const SVObjectInfoStruct& p_rInfoObject )
{
	SVStringVector Result;

	// Effective STL Item 9 p46: forward iteration / erase
	int nCount = static_cast< int >( m_vecRecords.size() );
	RecordsType::iterator iter;
	for ( iter = m_vecRecords.begin(); iter != m_vecRecords.end();  )
	{
		SVArchiveRecord* pImageRecord = *iter;
		if ( pImageRecord )
		{
			if( p_rInfoObject.UniqueObjectID == pImageRecord->m_svObjectReference.Guid() )
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

BOOL SVArchiveRecordsArray::WriteArchiveImageFiles( )
{
	ASSERT( nullptr != m_pArchiveTool );
	BOOL bOk = TRUE;
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
	// Effective STL Item 9 p46: forward iterate / erase

	RecordsType::iterator iter;
	for ( iter = m_vecRecords.begin(); iter != m_vecRecords.end(); )
	{
		SVArchiveRecord* pResultRecord = *iter;
		pResultRecord->DisconnectInputObject();
		
		SVValueObjectClass* pValueObject = nullptr;

		GUID guid = pResultRecord->m_svObjectReference.Guid();

		pValueObject = dynamic_cast <SVValueObjectClass*> (SVObjectManagerClass::Instance().GetObject(guid));

		bool bRecordOK = false;
		if (pValueObject)
		{
			if ( static_cast <SVObjectClass*> (pValueObject) != pResultRecord->m_svObjectReference.Object() )
			{
				long lArrayIndex = pResultRecord->m_svObjectReference.ArrayIndex();
				pResultRecord->m_svObjectReference = pValueObject;
				pResultRecord->m_svObjectReference.SetArrayIndex( lArrayIndex );
			}

			SVValueObjectReference voref( pResultRecord->m_svObjectReference );

			SVString sTemp;
			if ( voref.GetValue(0, sTemp) != SVMSG_SVO_33_OBJECT_INDEX_INVALID )
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
	return static_cast< int >( m_vecRecords.size() );
}

SVString SVArchiveRecordsArray::BuildResultsArchiveString()
{
	SVString Result;
	
	BOOL bFirst = TRUE;	
	int nCount = static_cast< int >( m_vecRecords.size() );
	for (int i = 0; i < nCount; i++)
	{
		SVArchiveRecord* pResultRecord = m_vecRecords.at(i);
		ASSERT(pResultRecord);
		//
		// We assume all archivable objects are Value Objects.
		//
		SVValueObjectReference voref( pResultRecord->m_svObjectReference );

		ASSERT( voref.Object() );

		if ( voref.Object() )
		{
			SVString Temp;

			HRESULT hr = voref.GetValue( Temp );
			if ( S_OK == hr || SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == hr )
			{
				if ( bFirst )
				{
					bFirst = FALSE;
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