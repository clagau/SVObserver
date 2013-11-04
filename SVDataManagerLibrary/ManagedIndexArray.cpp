// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : ManagedIndexArray.cpp
// * .File Name       : $Workfile:   ManagedIndexArray.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   13 May 2013 10:55:08  $
// ******************************************************************************

#include "stdafx.h"
#include "ManagedIndexArray.h"
#include "SVStatusLibrary/SVException.h"
#include "SVStatusLibrary/SVStatusCodes.h"
#include "SVSystemLibrary/SVAutoLockAndReleaseTemplate.h"

SVManagedIndex::SVManagedIndex()
{
	svmlTransactionId = 0;

	svmlAcquisition = 0;
	svmlInspection = 0;
	svmlPPQ = 0;
	svmlArchive = 0;
	svmlDisplay = 0;
	svmlLastInspected = 0;
	svmlDCOM = 0;
	svmlOther = 0;
}

SVString SVManagedIndex::GetReferenceCounts() const
{
	SVString l_Temp;

	l_Temp.Format( _T("ACQ=%ld-IP=%ld-PPQ=%ld-ARC=%ld-DIS=%ld-LIP=%ld-DCOM=%ld-OTH=%ld"),
		svmlAcquisition,
		svmlInspection,
		svmlPPQ,
		svmlArchive,
		svmlDisplay,
		svmlLastInspected,
		svmlDCOM,
		svmlOther );

	return l_Temp;
}

long SVManagedIndex::GetReferenceCount() const
{
	long l_Count = 0;

	l_Count += svmlAcquisition;
	l_Count += svmlInspection;
	l_Count += svmlPPQ;
	l_Count += svmlArchive;
	l_Count += svmlDisplay;
	l_Count += svmlLastInspected;
	l_Count += svmlDCOM;
	l_Count += svmlOther;

	return l_Count;
}

long SVManagedIndex::GetReferenceCount( SVDataManagerLockTypeEnum p_eType ) const
{
	long l_Count = 0;

	switch( p_eType )
	{
	case SV_ACQUISITION:
		{
			l_Count = svmlAcquisition;
			break;
		}
	case SV_INSPECTION:
		{
			l_Count = svmlInspection;
			break;
		}
	case SV_PPQ:
		{
			l_Count = svmlPPQ;
			break;
		}
	case SV_ARCHIVE:
		{
			l_Count = svmlArchive;
			break;
		}
	case SV_DCOM:
		{
			l_Count = svmlDCOM;
			break;
		}
	case SV_DISPLAY:
		{
			l_Count = svmlDisplay;
			break;
		}
	case SV_LAST_INSPECTED:
		{
			l_Count = svmlLastInspected;
			break;
		}
	case SV_OTHER:
		{
			l_Count = svmlOther;
			break;
		}
	default:
		{
			l_Count = -1;
			break;
		}
	}

	return l_Count;
}

HRESULT SVManagedIndex::AddReference( SVDataManagerLockTypeEnum p_eType )
{
	HRESULT l_Status = S_OK;

	switch( p_eType )
	{
	case SV_ACQUISITION:
		{
			::InterlockedIncrement( &svmlAcquisition );
			break;
		}
	case SV_INSPECTION:
		{
			::InterlockedIncrement( &svmlInspection );
			break;
		}
	case SV_PPQ:
		{
			::InterlockedIncrement( &svmlPPQ );
			break;
		}
	case SV_ARCHIVE:
		{
			::InterlockedIncrement( &svmlArchive );
			break;
		}
	case SV_DCOM:
		{
			::InterlockedIncrement( &svmlDCOM );
			break;
		}
	case SV_DISPLAY:
		{
			::InterlockedIncrement( &svmlDisplay );
			break;
		}
	case SV_LAST_INSPECTED:
		{
			::InterlockedIncrement( &svmlLastInspected );
			break;
		}
	case SV_OTHER:
		{
			::InterlockedIncrement( &svmlOther );
			break;
		}
	default:
		{
			l_Status = E_FAIL;
			break;
		}
	}

	return l_Status;
}

HRESULT SVManagedIndex::ReleaseReference( SVDataManagerLockTypeEnum p_eType )
{
	HRESULT l_Status = S_OK;

	switch( p_eType )
	{
	case SV_ACQUISITION:
		{
			long l_Result = ::InterlockedDecrement( &svmlAcquisition );

			if( l_Result == 0 )
			{
				l_Status = SVDM_1525FINALACQUISITIONRELEASE_INFORMATION;
			}
			else if( l_Result < 0 )
			{
				::InterlockedExchange( &svmlAcquisition, 0 );

				l_Status = SVDM_1519ACQUISITIONNOLOCKTORELEASE_FATAL;
			}

			break;
		}
	case SV_INSPECTION:
		{
			long l_Result = ::InterlockedDecrement( &svmlInspection );

			if( l_Result == 0 )
			{
				l_Status = SVDM_1526FINALINSPECTIONRELEASE_INFORMATION;
			}
			else if( l_Result < 0 )
			{
				::InterlockedExchange( &svmlInspection, 0 );

				l_Status = SVDM_1520INSPECTIONNOLOCKTORELEASE_FATAL;
			}

			break;
		}
	case SV_PPQ:
		{
			long l_Result = ::InterlockedDecrement( &svmlPPQ );

			if( l_Result == 0 )
			{
				l_Status = SVDM_1527FINALPPQRELEASE_INFORMATION;
			}
			else if( l_Result < 0 )
			{
				::InterlockedExchange( &svmlPPQ, 0 );

				l_Status = SVDM_1521PPQNOLOCKTORELEASE_FATAL;
			}

			break;
		}
	case SV_ARCHIVE:
		{
			long l_Result = ::InterlockedDecrement( &svmlArchive );

			if( l_Result == 0 )
			{
				l_Status = SVDM_1528FINALARCHIVERELEASE_INFORMATION;
			}
			else if( l_Result < 0 )
			{
				::InterlockedExchange( &svmlArchive, 0 );

				l_Status = SVDM_1522ARCHIVENOLOCKTORELEASE_FATAL;
			}

			break;
		}
	case SV_DCOM:
		{
			long l_Result = ::InterlockedDecrement( &svmlDCOM );

			if( l_Result == 0 )
			{
				l_Status = SVDM_1529FINALDCOMRELEASE_INFORMATION;
			}
			else if( l_Result < 0 )
			{
				::InterlockedExchange( &svmlDCOM, 0 );

				l_Status = SVDM_1523DCOMNOLOCKTORELEASE_FATAL;
			}

			break;
		}
	case SV_DISPLAY:
		{
			long l_Result = ::InterlockedDecrement( &svmlDisplay );

			if( l_Result == 0 )
			{
				l_Status = SVDM_1537DISPLAY_FINALRELEASE_INFORMATION;
			}
			else if( l_Result < 0 )
			{
				::InterlockedExchange( &svmlDisplay, 0 );

				l_Status = SVDM_1536DISPLAY_NOLOCKTORELEASE_FATAL;
			}

			break;
		}
	case SV_LAST_INSPECTED:
		{
			long l_Result = ::InterlockedDecrement( &svmlLastInspected );

			if( l_Result == 0 )
			{
				l_Status = SVDM_1539LASTINSPECTED_FINALRELEASE_INFORMATION;
			}
			else if( l_Result < 0 )
			{
				::InterlockedExchange( &svmlLastInspected, 0 );

				l_Status = SVDM_1538LASTINSPECTED_NOLOCKTORELEASE_FATAL;
			}

			break;
		}
	case SV_OTHER:
		{
			long l_Result = ::InterlockedDecrement( &svmlOther );

			if( l_Result == 0 )
			{
				l_Status = SVDM_1530FINALOTHERRELEASE_INFORMATION;
			}
			else if( l_Result < 0 )
			{
				::InterlockedExchange( &svmlOther, 0 );

				l_Status = SVDM_1524OTHERNOLOCKTORELEASE_FATAL;
			}

			break;
		}
	default:
		{
			l_Status = E_FAIL;
			break;
		}
	}

	if( GetReferenceCount() == 0 )
	{
		svmlTransactionId = 0;

		if( SV_SUCCEEDED( l_Status ) )
		{
			l_Status = SVDM_1507FINALINDEXRELEASE_INFORMATION;
		}
	}

	return l_Status;
}

HRESULT SVManagedIndex::ReleaseAllReferences()
{
	HRESULT l_Status = S_OK;

	svmlAcquisition = 0;
	svmlInspection = 0;
	svmlPPQ = 0;
	svmlArchive = 0;
	svmlDisplay = 0;
	svmlLastInspected = 0;
	svmlDCOM = 0;
	svmlOther = 0;

	svmlTransactionId = 0;

	return l_Status;
}

/*
This constructor initializes the internal attributes.
*/
SVManagedIndexArray::SVManagedIndexArray()
: m_TransactionCounter( 10000 )
, m_ArrayDepth( 0 )
, m_LastIndex( 0 )
, m_IndexArray()
, m_MessageCounter( 0 )
, m_Lock()
, m_LastMessageTimeStamp( 0.0 )
{
}

SVManagedIndexArray::~SVManagedIndexArray()
{
	RemoveAll();
}

HRESULT SVManagedIndexArray::RemoveAll()
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( l_AutoLock.Assign( &m_Lock ) )
	{
		m_ArrayDepth = 0;
		m_LastIndex = 0;

		SVManagedIndexPtrVector::iterator l_Iter = m_IndexArray.begin();

		while( l_Iter != m_IndexArray.end() )
		{
			SVManagedIndex* l_pIndex = *l_Iter;

			if( l_pIndex != NULL )
			{
				delete l_pIndex;
			}

			l_Iter = m_IndexArray.erase( l_Iter );
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

size_t SVManagedIndexArray::GetSize() const
{
	return m_ArrayDepth;
}

HRESULT SVManagedIndexArray::SetSize( size_t p_Size )
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( l_AutoLock.Assign( &m_Lock ) )
	{
		size_t l_OrigSize = m_IndexArray.size();

		if( l_OrigSize < p_Size )
		{
			m_IndexArray.resize( p_Size, NULL );
		}

		for( size_t i = 0; i < m_IndexArray.size(); ++i )
		{
			SVManagedIndex* l_pIndex = m_IndexArray[ i ];

			if( l_pIndex == NULL )
			{
				m_IndexArray[ i ] = new SVManagedIndex;
			}
			else if( l_OrigSize <= i )
			{
				l_pIndex->ReleaseAllReferences();
			}
			else if( p_Size <= i )
			{
				l_pIndex->ReleaseAllReferences();
			}
		}

		m_ArrayDepth = p_Size;
		m_LastIndex = 0;
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVManagedIndexArray::GetAt( size_t p_Index, SVManagedIndex*& p_rpManagedIndex ) const
{
	HRESULT l_Status = S_OK;

	p_rpManagedIndex = NULL;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( l_AutoLock.Assign( &m_Lock ) )
	{
		l_Status = GetAtNoLock( p_Index, p_rpManagedIndex );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVManagedIndexArray::ValidateIndex( size_t p_Index ) const
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( l_AutoLock.Assign( &m_Lock ) )
	{
		if( p_Index < m_ArrayDepth )
		{
			if( p_Index < m_IndexArray.size() )
			{
				SVManagedIndex* l_pIndex = m_IndexArray[ p_Index ];

				if( l_pIndex != NULL )
				{
					if( l_pIndex->svmlTransactionId == 0 )
					{
						l_Status = SVDM_1531MANAGEDINDEXARRAY_FATAL;
					}
				}
				else
				{
					l_Status = SVDM_1503INVALIDBUFFERINDEX_ERROR;
				}
			}
			else
			{
				l_Status = SVDM_1503INVALIDBUFFERINDEX_ERROR;
			}
		}
		else
		{
			l_Status = SVDM_1503INVALIDBUFFERINDEX_ERROR;
		}
	}
	else
	{
		l_Status = SVDM_1515COULDNOTLOCKMANAGEDARRAY_FATAL;
	}

	return l_Status;
}

HRESULT SVManagedIndexArray::ValidateIndex( size_t p_Index, long p_TransactionId ) const
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( l_AutoLock.Assign( &m_Lock ) )
	{
		l_Status = ValidateIndexNoLock( p_Index, p_TransactionId );
	}
	else
	{
		l_Status = SVDM_1515COULDNOTLOCKMANAGEDARRAY_FATAL;
	}

	return l_Status;
}

HRESULT SVManagedIndexArray::ReleaseAllIndexes()
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( l_AutoLock.Assign( &m_Lock ) )
	{
		m_ArrayDepth = 0;
		m_LastIndex = 0;

		for( size_t i = 0; i < m_IndexArray.size(); ++i )
		{
			SVManagedIndex* l_pIndex = m_IndexArray[ i ];

			if( l_pIndex != NULL )
			{
				l_pIndex->ReleaseAllReferences();
			}
		}
	}
	else
	{
		l_Status = SVDM_1513COULDNOTLOCKMANAGEDARRAY_FATAL;
	}

	return l_Status;
}

SVString SVManagedIndexArray::GetReferenceCounts() const
{
	SVString l_Temp;

	l_Temp.Format( _T( "%s-MC=%ld" ), GetName().c_str(), m_MessageCounter );

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( l_AutoLock.Assign( &m_Lock ) )
	{
		for( size_t i = 0; i < m_IndexArray.size(); ++i )
		{
			SVManagedIndex* l_pIndex = m_IndexArray[ i ];

			if( l_pIndex != NULL )
			{
				SVString l_Element;

				l_Element.Format( _T( ":Index=%ld-" ), i );

				l_Temp += l_Element;
				l_Temp += l_pIndex->GetReferenceCounts();
			}
		}
	}

	return l_Temp;
}

long SVManagedIndexArray::GetReferenceCount( size_t p_Index ) const
{
	long l_Count = 0;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( l_AutoLock.Assign( &m_Lock ) )
	{
		if( p_Index < m_ArrayDepth )
		{
			if( p_Index < m_IndexArray.size() )
			{
				SVManagedIndex* l_pIndex = m_IndexArray[ p_Index ];

				if( l_pIndex != NULL )
				{
					l_Count = l_pIndex->GetReferenceCount();
				}
			}
		}
	}

	return l_Count;
}

long SVManagedIndexArray::GetReferenceCount( size_t p_Index, SVDataManagerLockTypeEnum p_eType ) const
{
	long l_Count = 0;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( l_AutoLock.Assign( &m_Lock ) )
	{
		if( p_Index < m_ArrayDepth )
		{
			if( p_Index < m_IndexArray.size() )
			{
				SVManagedIndex* l_pIndex = m_IndexArray[ p_Index ];

				if( l_pIndex != NULL )
				{
					l_Count = l_pIndex->GetReferenceCount( p_eType );
				}
			}
		}
	}

	return l_Count;
}

HRESULT SVManagedIndexArray::GetNextIndex( long& p_rIndex, long& p_rTransactionID, bool p_WaitForLock )
{
	HRESULT l_Status = S_OK;

	p_rIndex = -1;
	p_rTransactionID = ::InterlockedExchangeAdd( &( m_TransactionCounter ), 1 );

	if( p_rTransactionID == 0 )
	{
		p_rTransactionID = ::InterlockedExchangeAdd( &( m_TransactionCounter ), 1 );
	}

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	bool l_Process = true;

	if( p_WaitForLock )
	{
		l_Process = ( l_AutoLock.Assign( &m_Lock ) != FALSE );
	}

	if( l_Process )
	{
		long l_TempIndex = m_LastIndex;

		for( size_t i = 0; p_rIndex == -1 && i < m_ArrayDepth; ++i )
		{
			l_TempIndex = ( l_TempIndex + 1 ) % m_ArrayDepth;

			SVManagedIndex* l_pIndex = m_IndexArray[ l_TempIndex ];

			if( l_pIndex != NULL )
			{
				if( ::InterlockedCompareExchange( &( l_pIndex->svmlTransactionId ), p_rTransactionID, 0 ) == 0 )
				{
					p_rIndex = l_TempIndex;

					::InterlockedExchange( &m_LastIndex, l_TempIndex );
				}
			}
		}

		if( p_rIndex == -1 )
		{
			l_Status = SVDM_1502NO_INDEXESAVAILABLE_ERROR;
		}
	}
	else
	{
		l_Status = SVDM_1513COULDNOTLOCKMANAGEDARRAY_FATAL;
	}

	return l_Status;
}

HRESULT SVManagedIndexArray::AddReference( size_t p_Index, long p_TransactionId, SVDataManagerLockTypeEnum p_eType )
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( l_AutoLock.Assign( &m_Lock ) )
	{
		l_Status = AddReferenceNoLock( p_Index, p_TransactionId, p_eType );
	}
	else
	{
		l_Status = SVDM_1515COULDNOTLOCKMANAGEDARRAY_FATAL;
	}

	return l_Status;
}

HRESULT SVManagedIndexArray::ReleaseReference( size_t p_Index, long p_TransactionId, SVDataManagerLockTypeEnum p_eType )
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( l_AutoLock.Assign( &m_Lock ) )
	{
		l_Status = ReleaseReferenceNoLock( p_Index, p_TransactionId, p_eType );
	}
	else
	{
		l_Status = SVDM_1516COULDNOTLOCKMANAGEDARRAY_FATAL;
	}

	return l_Status;
}

size_t SVManagedIndexArray::GetAvailableIndexCount() const
{
	size_t l_Count = 0;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( l_AutoLock.Assign( &m_Lock ) )
	{
		for( size_t i = 0; i < m_ArrayDepth && i < m_IndexArray.size(); ++i )
		{
			SVManagedIndex* l_pIndex = m_IndexArray[ i ];

			if( l_pIndex != NULL && l_pIndex->svmlTransactionId == 0 )
			{
				++l_Count;
			}
		}
	}

	return l_Count;
}

bool SVManagedIndexArray::LogEventMessage()
{
	m_MessageCounter++;

	SVClock::SVTimeStamp l_CurrentTime = SVClock::GetTimeStamp();

	bool l_Status = ( ( m_LastMessageTimeStamp + SVClock::ConvertFrom( SVClock::Seconds, 60 ) ) < l_CurrentTime );

	if( l_Status )
	{
		m_LastMessageTimeStamp = l_CurrentTime;
	}

	return l_Status;
}

const SVString& SVManagedIndexArray::GetName() const
{
	return m_Name;
}

void SVManagedIndexArray::SetName( const SVString& p_rName )
{
	m_Name = p_rName;
}

long SVManagedIndexArray::GetMessageCounter() const
{
	return m_MessageCounter;
}

void SVManagedIndexArray::Dump( const SVString& p_rSourceName ) const
{
	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( l_AutoLock.Assign( &m_Lock ) )
	{
		// Open a File Name derived from handle ...
		FILE* l_pfh=NULL;
		char l_sFileName[128];
		sprintf( l_sFileName, _T("c:\\SVObserver\\Data Manager %s %s.csv"), p_rSourceName.c_str(), GetName().c_str() );
		l_pfh = fopen( l_sFileName, _T("w"));
		if( l_pfh == NULL )
			return;
		
		fprintf( l_pfh, _T("%s\n"), GetName().c_str() );
		fprintf( l_pfh, _T(" Acquisition, PPQ, Inspection, Display, Last Insp, Archive, DCOM, Other\n"));
		for( size_t i = 0 ; i < GetSize(); i++ )
		{
			SVManagedIndex* l_pIndex = NULL;

			GetAt( i, l_pIndex );
			
			fprintf( l_pfh, _T("%d,"), l_pIndex->GetReferenceCount( SV_ACQUISITION ) );
			fprintf( l_pfh, _T("%d,"), l_pIndex->GetReferenceCount( SV_PPQ ) );
			fprintf( l_pfh, _T("%d,"), l_pIndex->GetReferenceCount( SV_INSPECTION ) );
			fprintf( l_pfh, _T("%d,"), l_pIndex->GetReferenceCount( SV_DISPLAY ) );
			fprintf( l_pfh, _T("%d,"), l_pIndex->GetReferenceCount( SV_LAST_INSPECTED ) );
			fprintf( l_pfh, _T("%d,"), l_pIndex->GetReferenceCount( SV_ARCHIVE ) );
			fprintf( l_pfh, _T("%d,"), l_pIndex->GetReferenceCount( SV_DCOM ) );
			fprintf( l_pfh, _T("%d,"), l_pIndex->GetReferenceCount( SV_OTHER ) );
			fprintf( l_pfh, _T("\n") );
		}
		
		fclose( l_pfh );
	}
}

HRESULT SVManagedIndexArray::ClearIndex( size_t p_Index, long p_TransactionId )
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( l_AutoLock.Assign( &m_Lock ) )
	{
		if( p_Index < m_ArrayDepth )
		{
			if( p_Index < m_IndexArray.size() )
			{
				SVManagedIndex* l_pIndex = m_IndexArray[ p_Index ];

				if( l_pIndex != NULL )
				{
					if( l_pIndex->svmlTransactionId == p_TransactionId )
					{
						l_Status = l_pIndex->ReleaseAllReferences();
					}
					else
					{
						l_Status = SVDM_1541RELEASE_INVALIDTRANSACTIONID_FATAL;
					}
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
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = SVDM_1516COULDNOTLOCKMANAGEDARRAY_FATAL;
	}

	return l_Status;
}

HRESULT SVManagedIndexArray::GetAtNoLock( size_t p_Index, SVManagedIndex*& p_rpManagedIndex ) const
{
	HRESULT l_Status = S_OK;

	p_rpManagedIndex = NULL;

	if( p_Index < m_ArrayDepth )
	{
		if( p_Index < m_IndexArray.size() )
		{
			p_rpManagedIndex = m_IndexArray[ p_Index ];
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

HRESULT SVManagedIndexArray::AddReferenceNoLock( size_t p_Index, long p_TransactionId, SVDataManagerLockTypeEnum p_eType )
{
	HRESULT l_Status = S_OK;

	if( p_Index < m_ArrayDepth )
	{
		if( p_Index < m_IndexArray.size() )
		{
			SVManagedIndex* l_pIndex = m_IndexArray[ p_Index ];

			if( l_pIndex != NULL )
			{
				if( l_pIndex->svmlTransactionId == p_TransactionId )
				{
					l_Status = l_pIndex->AddReference( p_eType );
				}
				else
				{
					l_Status = SVDM_1540LOCK_INVALIDTRANSACTIONID_FATAL;
				}
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
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVManagedIndexArray::ReleaseReferenceNoLock( size_t p_Index, long p_TransactionId, SVDataManagerLockTypeEnum p_eType )
{
	HRESULT l_Status = S_OK;

	if( p_Index < m_ArrayDepth )
	{
		if( p_Index < m_IndexArray.size() )
		{
			SVManagedIndex* l_pIndex = m_IndexArray[ p_Index ];

			if( l_pIndex != NULL )
			{
				if( l_pIndex->svmlTransactionId == p_TransactionId )
				{
					l_Status = l_pIndex->ReleaseReference( p_eType );
				}
				else
				{
					l_Status = SVDM_1541RELEASE_INVALIDTRANSACTIONID_FATAL;
				}
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
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVManagedIndexArray::ValidateIndexNoLock( size_t p_Index, long p_TransactionId ) const
{
	HRESULT l_Status = S_OK;

	if( p_Index < m_ArrayDepth )
	{
		if( p_Index < m_IndexArray.size() )
		{
			SVManagedIndex* l_pIndex = m_IndexArray[ p_Index ];

			if( l_pIndex != NULL )
			{
				if( l_pIndex->svmlTransactionId != p_TransactionId )
				{
					l_Status = SVDM_1531MANAGEDINDEXARRAY_FATAL;
				}
			}
			else
			{
				l_Status = SVDM_1503INVALIDBUFFERINDEX_ERROR;
			}
		}
		else
		{
			l_Status = SVDM_1503INVALIDBUFFERINDEX_ERROR;
		}
	}
	else
	{
		l_Status = SVDM_1503INVALIDBUFFERINDEX_ERROR;
	}

	return l_Status;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVDataManagerLibrary\ManagedIndexArray.cpp_v  $
 * 
 *    Rev 1.2   13 May 2013 10:55:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   07 May 2013 13:00:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use new SVClock conversions functions, which will clarify timing tolerance checks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 May 2013 19:59:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 012.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   29 Apr 2013 12:27:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 07:53:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   16 Sep 2011 14:10:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated Data Manager functionality to remove locking issues and resource issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   05 Jul 2011 14:35:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  724
 * SCR Title:  Add Resource Logging to SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to create a method to get the number of availiable indexes and to manage logging when resources are consumed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   24 May 2010 13:16:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include a no lock path when locking is not required due to owner lock.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Feb 2010 09:07:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated old and added missing source code comments to existing code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   01 Sep 2009 16:47:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved code from SVLibrary to SVDataManagerLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   13 Jul 2009 12:34:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change the methodology for managing the indexes using the data manager.  The new methodology uses the smart pointer to externally reference count the index usage and release the array handle when all indexes are released.  Atransaction id was added to prevent unintended index reuse.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   02 Nov 2005 09:25:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed check-in directory
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   02 Nov 2005 09:24:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   corrected check-in directory
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   01 Nov 2005 14:35:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added svmlDisplay and svmlLastInspected
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   10 Jan 2003 13:31:50   cschmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  318
 * SCR Title:  Image Transfer Compression
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Code was moved to the SVLibrary Project
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

