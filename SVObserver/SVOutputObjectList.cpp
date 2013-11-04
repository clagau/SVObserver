//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOutputObjectList
//* .File Name       : $Workfile:   SVOutputObjectList.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   07 Aug 2013 13:24:24  $
//******************************************************************************

#include "stdafx.h"
#include "SVOutputObjectList.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVValueObject.h"
#include "SVDigitalOutputObject1.h"
#include "SVRemoteOutputObject.h"
#include "SVInfoStructs.h"
#include "SVObserver.h"
#include "SVPPQObject.h"
#include "SVInputObjectList.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"

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

BOOL SVOutputObjectList::Create()
{
	outObjectInfo.ObjectTypeInfo.ObjectType = SVOutputObjectListType;

	try
	{
		#if( _WIN32_WINNT >= 0x0500 )
		  m_bCreated = ::InitializeCriticalSectionAndSpinCount( &m_hCriticalSection, 4000 );
		#else
		  m_bCreated = ::InitializeCriticalSection( &m_hCriticalSection );
		#endif
	}
	catch(...)
	{
		m_bCreated = false;
	}

	return m_bCreated;
}// end Create

BOOL SVOutputObjectList::Destroy()
{
	try
	{
		if( m_bCreated )
		{
			Lock();

			ClearOutputList();

			Unlock();

			m_bCreated = FALSE;

			::DeleteCriticalSection( &m_hCriticalSection );
		}// end if
	}
	catch(...)
	{
	}

	return TRUE;
}// end Destroy

BOOL SVOutputObjectList::Lock() const
{
	if( m_bCreated )
	{
		::EnterCriticalSection( const_cast< LPCRITICAL_SECTION >( &m_hCriticalSection ) );

		return TRUE;
	}// end if

	return FALSE;
}// end Lock

BOOL SVOutputObjectList::Unlock() const
{
	if( m_bCreated )
	{
		::LeaveCriticalSection( const_cast< LPCRITICAL_SECTION >( &m_hCriticalSection ) );

		return TRUE;
	}// end if

	return FALSE;
}// end Unlock

HRESULT SVOutputObjectList::AttachOutput( SVOutputObject *pOutput )
{
	HRESULT l_Status = S_OK;

	if( pOutput != NULL )
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

HRESULT SVOutputObjectList::DetachOutput( const SVGUID& p_rOutputID )
{
	HRESULT l_Status = S_OK;

	if( Lock() )
	{
		SVGuidSVOutputObjectPtrMap::iterator l_Iter = m_OutputObjects.find( p_rOutputID );

		if( l_Iter != m_OutputObjects.end() )
		{
			SVOutputObject* l_pOutput = l_Iter->second;

			m_OutputObjects.erase( l_Iter );

			if( l_pOutput != NULL )
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

BOOL SVOutputObjectList::WriteOutputs( SVIOEntryStructVector& p_IOEntries, long lDataIndex, bool p_ACK, bool p_NAK )
{
	
	SVOutputObject	*pStateOutput = NULL;
	SVDigitalOutputObject	*pDigOutput = NULL;
	_variant_t l_Variant;
	CString strValue;
	BOOL bValue = false;
	double dValue = 0.0;
	DWORD dwValue = 0;
	long lValue = 0;
	TCHAR *pStrValue = NULL;

	if( Lock() )
	{
		pStateOutput = NULL;

		for( size_t i = 0; i < p_IOEntries.size(); i++ )
		{
			SVIOEntryStruct& pIOEntry = p_IOEntries[i];

			// Check if output is enabled for this call
			if( !( pIOEntry.empty() ) && pIOEntry.m_IOEntryPtr->m_Enabled )
			{
				if( pIOEntry.m_IOEntryPtr->m_ObjectType == IO_DIGITAL_OUTPUT )
				{
					WriteDigitalOutput( pIOEntry, lDataIndex, p_ACK, p_NAK );
				}// end if
				else if( pIOEntry.m_IOEntryPtr->m_ObjectType == IO_REMOTE_OUTPUT )
				{
					SVOutputObject* pOutput = NULL;

					SVGuidSVOutputObjectPtrMap::iterator l_Iter = m_OutputObjects.find( pIOEntry.m_IOEntryPtr->m_IOId );

					if( l_Iter != m_OutputObjects.end() )
					{
						pOutput = l_Iter->second;
					}
					
					if( lDataIndex == -1 )
					{
						((SVVariantValueObjectClass*)pIOEntry.m_IOEntryPtr->m_pValueObject)->GetDefaultValue( l_Variant );
					}
					else
					{
						pIOEntry.m_IOEntryPtr->m_pValueObject->GetValue( lDataIndex, l_Variant );
					}
					
					pOutput->Write( &l_Variant );
				}// end else if

			}// end if

		}// end for

		if( pStateOutput )
		{
			dwValue = 0;

			SVIOConfigurationInterfaceClass::Instance().GetDigitalOutputState( sizeof dwValue, (unsigned char *)&dwValue );

			strValue.Format( "%d", dwValue );

			TCHAR l_szStrValue[MAX_PATH];
			strcpy( l_szStrValue, strValue );

			pStateOutput->Write( l_szStrValue );
		}// end if

		Unlock();

		return TRUE;
	}// end if

	return FALSE;
}// end WriteOutputs

BOOL SVOutputObjectList::WriteOutputs( SVIOEntryHostStructPtrList& p_IOEntries, long lDataIndex, bool p_ACK, bool p_NAK )
{
	
	SVOutputObject	*pStateOutput = NULL;
	SVDigitalOutputObject	*pDigOutput = NULL;
	_variant_t l_Variant;
	CString strValue;
	BOOL bValue = false;
	double dValue = 0.0;
	DWORD dwValue = 0;
	long lValue = 0;
	TCHAR *pStrValue = NULL;

	if( Lock() )
	{
		pStateOutput = NULL;

		for( size_t i = 0; i < p_IOEntries.size(); i++ )
		{
			SVIOEntryHostStructPtr pIOEntry = p_IOEntries[i];

			// Check if output is enabled for this call
			if( !( pIOEntry.empty() ) && pIOEntry->m_Enabled )
			{
				if( pIOEntry->m_ObjectType == IO_DIGITAL_OUTPUT )
				{
					SVIOEntryStruct l_Entry;

					l_Entry.m_EntryValid = true;
					l_Entry.m_IOEntryPtr = pIOEntry;

					WriteDigitalOutput( l_Entry, lDataIndex, p_ACK, p_NAK );
				}// end if
				else if( pIOEntry->m_ObjectType == IO_REMOTE_OUTPUT )
				{
					SVOutputObject* pOutput = NULL;

					SVGuidSVOutputObjectPtrMap::iterator l_Iter = m_OutputObjects.find( pIOEntry->m_IOId );

					if( l_Iter != m_OutputObjects.end() )
					{
						pOutput = l_Iter->second;
					}
					
					if( lDataIndex == -1 )
					{
						((SVVariantValueObjectClass*)pIOEntry->m_pValueObject)->GetDefaultValue( l_Variant );
					}
					else
					{
						pIOEntry->m_pValueObject->GetValue( lDataIndex, l_Variant );
					}
					
					pOutput->Write( &l_Variant );
				}// end else if

			}// end if

		}// end for

		if( pStateOutput )
		{
			dwValue = 0;

			SVIOConfigurationInterfaceClass::Instance().GetDigitalOutputState( sizeof dwValue, (unsigned char *)&dwValue );

			strValue.Format( "%d", dwValue );

			TCHAR l_szStrValue[MAX_PATH];
			strcpy( l_szStrValue, strValue );

			pStateOutput->Write( l_szStrValue );
		}// end if

		Unlock();

		return TRUE;
	}// end if

	return FALSE;
}// end WriteOutputs

BOOL SVOutputObjectList::ResetOutputs( SVIOEntryStructVector& p_IOEntries )
{
	if( Lock() )
	{
		for( size_t i = 0; i < p_IOEntries.size(); i++ )
		{
			SVIOEntryStruct pIOEntry = p_IOEntries[i];

			// Check if output is enabled for this call
			if( !( pIOEntry.empty() ) && pIOEntry.m_IOEntryPtr->m_Enabled )
			{
				SVOutputObject* pOutput = NULL;

				SVGuidSVOutputObjectPtrMap::iterator l_Iter = m_OutputObjects.find( pIOEntry.m_IOEntryPtr->m_IOId );

				if( l_Iter != m_OutputObjects.end() )
				{
					pOutput	= l_Iter->second;

					pOutput->Reset();
				}
			}
		}

		Unlock();

		return TRUE;
	}// end if

	return FALSE;
}// end ResetOutputs

BOOL SVOutputObjectList::ResetOutputs( SVIOEntryHostStructPtrList& p_IOEntries )
{
	if( Lock() )
	{
		for( size_t i = 0; i < p_IOEntries.size(); i++ )
		{
			SVIOEntryHostStructPtr pIOEntry = p_IOEntries[i];

			// Check if output is enabled for this call
			if( !( pIOEntry.empty() ) && pIOEntry->m_Enabled )
			{
				SVOutputObject* pOutput = NULL;

				SVGuidSVOutputObjectPtrMap::iterator l_Iter = m_OutputObjects.find( pIOEntry->m_IOId );

				if( l_Iter != m_OutputObjects.end() )
				{
					pOutput	= l_Iter->second;

					pOutput->Reset();
				}
			}
		}

		Unlock();

		return TRUE;
	}// end if

	return FALSE;
}// end ResetOutputs

BOOL SVOutputObjectList::WriteOutput( SVIOEntryStruct pIOEntry, long lDataIndex, bool p_ACK, bool p_NAK )
{
	_variant_t l_Variant;
	CString strValue;

	if( Lock() )
	{

		// We are only writing one output, don't worry if it is marked enabled for this call
		if( !( pIOEntry.empty() ) )
		{
			if( pIOEntry.m_IOEntryPtr->m_ObjectType == IO_DIGITAL_OUTPUT )
			{
				WriteDigitalOutput( pIOEntry, lDataIndex, p_ACK, p_NAK );
			}// end if
			else if( pIOEntry.m_IOEntryPtr->m_ObjectType == IO_REMOTE_OUTPUT )
			{
				SVOutputObject* pOutput = NULL;

				SVGuidSVOutputObjectPtrMap::iterator l_Iter = m_OutputObjects.find( pIOEntry.m_IOEntryPtr->m_IOId );

				if( l_Iter != m_OutputObjects.end() )
				{
					pOutput	= l_Iter->second;
				}
				
				if( lDataIndex == -1 )
				{
					((SVVariantValueObjectClass*)pIOEntry.m_IOEntryPtr->m_pValueObject)->GetDefaultValue( l_Variant );
				}
				else
				{
					pIOEntry.m_IOEntryPtr->m_pValueObject->GetValue( lDataIndex, l_Variant );
				}

				pOutput->Write( &l_Variant );
			}// end else if

		}// end if

		Unlock();

		return TRUE;
	}// end if

	return FALSE;
}// end WriteOutput

BOOL SVOutputObjectList::WriteOutput( SVIOEntryHostStructPtr pIOEntry, long lDataIndex, bool p_ACK, bool p_NAK )
{
	_variant_t l_Variant;
	CString strValue;

	if( Lock() )
	{

		// We are only writing one output, don't worry if it is marked enabled for this call
		if( !( pIOEntry.empty() ) )
		{
			if( pIOEntry->m_ObjectType == IO_DIGITAL_OUTPUT )
			{
				SVIOEntryStruct l_Entry;

				l_Entry.m_EntryValid = true;
				l_Entry.m_IOEntryPtr = pIOEntry;

				WriteDigitalOutput( l_Entry, lDataIndex, p_ACK, p_NAK );
			}// end if
			else if( pIOEntry->m_ObjectType == IO_REMOTE_OUTPUT )
			{
				SVOutputObject* pOutput = NULL;

				SVGuidSVOutputObjectPtrMap::iterator l_Iter = m_OutputObjects.find( pIOEntry->m_IOId );

				if( l_Iter != m_OutputObjects.end() )
				{
					pOutput	= l_Iter->second;
				}
				
				if( lDataIndex == -1 )
				{
					((SVVariantValueObjectClass*)pIOEntry->m_pValueObject)->GetDefaultValue( l_Variant );
				}
				else
				{
					pIOEntry->m_pValueObject->GetValue( lDataIndex, l_Variant );
				}

				pOutput->Write( &l_Variant );
			}// end else if

		}// end if

		Unlock();

		return TRUE;
	}// end if

	return FALSE;
}// end WriteOutput

BOOL SVOutputObjectList::WriteOutputValue( SVIOEntryHostStructPtr pIOEntry, const _variant_t& p_rValue )
{
	SVOutputObject* pOutput = NULL;

	if( Lock() )
	{
		// We are only writing one output, don't worry if it is marked enabled for this call
		if( !( pIOEntry.empty() ) )
		{
			SVGuidSVOutputObjectPtrMap::const_iterator l_Iter = m_OutputObjects.find( pIOEntry->m_IOId );

			if( l_Iter != m_OutputObjects.end() )
			{
				pOutput = l_Iter->second;
			}
		}

		if( pOutput != NULL )
		{
			if( SV_IS_KIND_OF( pOutput, SVDigitalOutputObject ) )
			{
				pOutput->Write( p_rValue );
			}// end if
			else if( SV_IS_KIND_OF( pOutput, SVRemoteOutputObject ) )
			{
				pOutput->Write( p_rValue );
			}// end else if

		}// end if

		Unlock();

		return TRUE;
	}// end if

	return FALSE;
}

BOOL SVOutputObjectList::ResetOutput( SVIOEntryHostStructPtr pIOEntry )
{
	SVOutputObject	*pOutput;

	if( Lock() )
	{
		// Check if output is enabled for this call
		if( !( pIOEntry.empty() ))
		{
			pOutput	= m_OutputObjects[ pIOEntry->m_IOId ];

			bool l_bEnable = pIOEntry->m_Enabled;
			if( !l_bEnable )	// Act as if enabled when Module Ready or Raid Error 
			{
				CString l_strName = pOutput->GetName();
				if( l_strName.Compare("Module Ready") == 0 
					|| l_strName.Compare("Raid Error Indicator") == 0 )
				{
					l_bEnable = true;
				}
			}

			if( l_bEnable )
			{
				pOutput->Reset();
			}
		}// end if

		Unlock();

		return TRUE;
	}// end if

	return FALSE;
}// end ResetOutput


BOOL SVOutputObjectList::FillOutputs( SVIOEntryHostStructPtrList& ppIOEntries )
{
	ppIOEntries.clear();

	if( Lock() )
	{
		SVGuidSVOutputObjectPtrMap::const_iterator l_Iter = m_OutputObjects.begin();

		while( l_Iter != m_OutputObjects.end() )
		{
			SVOutputObject* pOutput = l_Iter->second;

			SVIOEntryHostStructPtr pIOEntry;

			pIOEntry = new SVIOEntryHostStruct;

			pIOEntry->m_IOId = pOutput->GetUniqueObjectID();
			
			if( SV_IS_KIND_OF( pOutput, SVDigitalOutputObject ) )
				pIOEntry->m_ObjectType = IO_DIGITAL_OUTPUT;
			else if( SV_IS_KIND_OF( pOutput, SVRemoteOutputObject ) )
				pIOEntry->m_ObjectType = IO_REMOTE_OUTPUT;

			ppIOEntries.push_back( pIOEntry );

			++l_Iter;
		}// end for

		Unlock();

		return TRUE;
	}// end if

	return FALSE;
}// end FillOutputs

bool SVOutputObjectList::RenameInspection( CString& OldInspection, CString& NewInspectionName)
{
	bool l_bRet = FALSE;
	SVGuidSVOutputObjectPtrMap::const_iterator l_Iter = m_OutputObjects.begin();

	while( l_Iter != m_OutputObjects.end() )
	{
		SVOutputObject* l_pOutput = l_Iter->second;

		CString l_strName = l_pOutput->GetName();

		int l_iPos = l_strName.Find('.');
		CString l_strObjInspName = l_strName.Left( l_iPos );

		if( l_strObjInspName.Compare( OldInspection ) == 0 )
		{
			CString l_strNewName = NewInspectionName + l_strName.Mid( l_iPos );
			l_pOutput->SetName( l_strNewName );
			l_bRet = TRUE;
		}

		++l_Iter;
	}
	return l_bRet;
}


HRESULT SVOutputObjectList::RemoveUnusedOutputs( const StringVect& p_aStrInspNames, const StringVect& p_astrPPQNames )
{
	typedef std::deque<SVDigitalOutputObject*> SVDigitalOutputPtrList;
	typedef std::map<long, SVDigitalOutputPtrList> SVDigitalOutputChannelMap;
	typedef std::set<CString> SVStringSet;

	HRESULT l_Status = S_OK;

	if( Lock() )
	{
		// if sizes are the same, Loop through inspection names and check for differences.
		// if there is a difference, then search for the old name in the output object list.
		// once we find the oldname in the output object list, replace with the new name.
		SVStringSet l_ValidObjects;

		for( size_t l_lIndex = 0 ; l_lIndex < p_aStrInspNames.size() ; l_lIndex++ )
		{
			// for each output object search for the inspection name in the new names.
			// if found
			CString l_strInspName = p_aStrInspNames[l_lIndex];

			SVGuidSVOutputObjectPtrMap::const_iterator l_Iter = m_OutputObjects.begin();

			while( l_Iter != m_OutputObjects.end() )
			{
				SVOutputObject* l_pOutput = l_Iter->second;

				CString l_strName = l_pOutput->GetName();

				int l_iPos = l_strName.Find('.');
				CString l_strObjInspName = l_strName.Left( l_iPos );

				if( l_strObjInspName.Compare( l_strInspName ) == 0 )
				{
					l_ValidObjects.insert( l_strName );
				}

				++l_Iter;
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
				SVOutputObject* l_pOutput = l_Iter->second;

				CString l_strName;

				if( l_pOutput != NULL )
				{
					l_strName = l_pOutput->GetName();
				}

				bool l_Remove = l_strName.IsEmpty();

				if( ! l_Remove )
				{
					bool l_Skip = false;

					// Check if the name starts with a valid PPQ_X prefix. Skip these outputs.
					for(StringVect::const_iterator it = p_astrPPQNames.begin() ; it != p_astrPPQNames.end() ; ++it)
					{
						if( l_strName.Find( *it ) == 0 )
						{
							l_Skip = true;
							break;
						}
					}

					l_Skip = l_Skip || ( l_strName.Find( _T("Module Ready") ) >= 0 );
					l_Skip = l_Skip || ( l_strName.Find( _T("Raid Error Indicator") ) >= 0 );

					if( ! l_Skip )
					{
						SVStringSet::const_iterator l_OkIter = l_ValidObjects.find( l_strName );

						l_Remove = ( l_OkIter == l_ValidObjects.end() );
						l_Remove = l_Remove || OutputIsNotValid( l_pOutput->GetName() );
					}
				}

				if( l_Remove )
				{
					l_Iter = m_OutputObjects.erase( l_Iter );

					if( l_pOutput != NULL )
					{
						delete l_pOutput;
					}
				}
				else
				{
					if( l_pOutput != NULL )
					{
						SVDigitalOutputObject* l_pDigital = dynamic_cast< SVDigitalOutputObject* >( l_pOutput );

						if( l_pDigital != NULL )
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

					if( l_pOutput != NULL && SVString( _T("Module Ready") ) == l_pOutput->GetName() )
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
			l_Status = SV_FATAL_SVOBSERVER_2006_DUPLICATE_DISCRETE_OUTPUT;
		}

		Unlock();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

bool SVOutputObjectList::OutputIsNotValid( SVString p_strName )
{
	bool l_bRet = true;
	
	SVObjectClass* l_pObject;
	HRESULT l_hr = SVObjectManagerClass::Instance().GetObjectByDottedName( p_strName, l_pObject );
	// Check if the object exists.
	if( l_pObject != NULL )
	{
		//if( l_pObject->GetPublicAttribute(
		if( l_pObject->ObjectAttributesSet() & SV_PUBLISHABLE )
			l_bRet = false;
	}
	// Return true if object does not exist or is not published.
	return l_bRet;
}

DWORD SVOutputObjectList::processMessage( DWORD DwMessageID, DWORD DwMessageValue, DWORD DwMessageContext )
{
	DWORD DwResult = NULL;
	// Try to process message by yourself...

	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;
	switch( dwPureMessageID )
	{
		// handle renaming of toolset variables with regards to outputs
		case SVMSGID_OBJECT_RENAMED:
		{
			if( Lock() )
			{
				SVObjectClass* pObject = reinterpret_cast<SVObjectClass*>(DwMessageValue);
				CString strOldName = (LPCTSTR) DwMessageContext;

				int iWhere;
				CString strCurName;
				CString strNewName;
				CString strTemp;
				CString strRoot;

				// Search the list of outputs
				if( pObject )
				{
					SVGuidSVOutputObjectPtrMap::iterator l_Iter = m_OutputObjects.begin();

					while( l_Iter != m_OutputObjects.end() )
					{
						SVOutputObject* l_pOutput = l_Iter->second;

						strCurName = l_pOutput->GetName();
						strTemp.Format( "%s", strOldName );
						iWhere = strCurName.Find( strTemp );

						if( iWhere != -1 )
						{
							strNewName = pObject->GetName();
 							strTemp  = strCurName.Left( iWhere  );
							strTemp += strNewName;
							strRoot  = strTemp;
							strTemp += strCurName.Mid( iWhere + strOldName.GetLength()  );

							if( 0 == strcmp( strRoot, pObject->GetCompleteObjectName() ) )
							{
								l_pOutput->SetName( strTemp );
							}// end if

						}// end if
						++l_Iter;
					}// end for
				}

				Unlock();
			}
			else
			{
				DwResult = E_FAIL;
			}

			break;
		}// end case

	}// end switch

	return( SVObjectClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );

}

HRESULT SVOutputObjectList::WriteDigitalOutput( SVIOEntryStruct& pIOEntry, long lDataIndex, bool p_ACK, bool p_NAK )
{
	HRESULT l_Status = S_OK;

	SVDigitalOutputObject* pOutput = NULL;

	l_Status = GetOutput( pIOEntry.m_IOEntryPtr->m_IOId, pOutput );

	if( pOutput != NULL )
	{
		bool bValue = false;

		if( 0 <= lDataIndex )
		{
			double dValue = 0.0;

			SVString l_String = pOutput->GetName();

			if( p_ACK || l_String.Left( 3 ).Compare( _T( "PPQ" ) ) == 0 )
			{
				pIOEntry.m_IOEntryPtr->m_pValueObject->GetValue( lDataIndex, dValue );

				bValue = ( dValue != 0.0 );
			}
			else
			{
				pIOEntry.m_IOEntryPtr->m_pValueObject->SetValue( lDataIndex, dValue );
			}

			if( pOutput->IsCombined() )
			{
				if( pOutput->GetCombinedValue() )
				{
					pIOEntry.m_CombinedValue = bValue && p_ACK;
				}
				else
				{
					pIOEntry.m_CombinedValue = bValue || p_NAK;
				}
			}
		}

		if( lDataIndex == -1 )
		{
			BOOL l_Temp = false;

			((SVBoolValueObjectClass*)pIOEntry.m_IOEntryPtr->m_pValueObject)->GetDefaultValue( l_Temp );

			bValue = ( l_Temp != FALSE );
		}
		else if( pOutput->IsCombined() )
		{
			bValue = pIOEntry.m_CombinedValue;
		}
		else
		{
			double dValue;
			pIOEntry.m_IOEntryPtr->m_pValueObject->GetValue( lDataIndex, dValue );
			bValue = ( dValue != 0.0 );
		}

		pOutput->Write( bValue );
	}

	return l_Status;
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVOutputObjectList.cpp_v  $
 * 
 *    Rev 1.5   07 Aug 2013 13:24:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Renamed SVIOEntryHostStructPtrVector to SVIOEntryHostStructPtrList.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   01 Aug 2013 10:49:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  849
 * SCR Title:  Modify the Remove Unused Outputs function to check PPQ names
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed parameters on RemoveUnusedOutputs to const & for performance and safety.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   30 Jul 2013 13:07:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  849
 * SCR Title:  Modify the Remove Unused Outputs function to check PPQ names
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added parameter (list of valid PPQs)to the function RemoveUnusedOutputs. This allows us to remove outputs  associated with a deleted PPQ.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Jun 2013 15:37:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 017.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.49   21 May 2013 13:39:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  822
 * SCR Title:  Remove DDE Input and Output functionality from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Remove Dead Code
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 May 2013 20:11:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.48   16 Apr 2013 15:35:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  822
 * SCR Title:  Remove DDE Input and Output functionality from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed DDE functionality from the source code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:16:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.47   04 Sep 2012 14:20:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  787
 * SCR Title:  Fix problem when Discrete I/O does not change when forced attribute is set
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified Reset Output to include Module Ready and Raid Error Indicator.  This function is only called from the GUI therefore there will be no run-time impact.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.46   16 Aug 2012 17:01:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the input and output list to use new variant parameter in the read and write operations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.45   16 Aug 2012 09:39:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added additional check. Output list items must be published otherwise they get removed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.44   15 Aug 2012 14:41:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new Singleton version of the SVIOConfigurationInterfaceClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.43   14 Aug 2012 15:56:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated output list functionality to only allow unique elements to the output list and notify user when duplicates are detected.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.42   02 Jul 2012 17:23:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.41   04 Jan 2012 07:55:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  743
 * SCR Title:  Fix Output Object list names.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed the skip logic in the RemoveUnusedOutputs function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.40   20 Dec 2011 14:58:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  741
 * SCR Title:  Fix IO List when changing Inspection Name
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added function to rename the inspection part of the output object fully qualified name.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.39   19 Apr 2011 16:18:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix issues with IO Assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.38   19 Apr 2011 10:28:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix issues with combinded outputs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.37   18 Apr 2011 10:36:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to move the shared data for the IO Entry data to a different class to fix issues with PPQ and Inspection shared data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   21 Jan 2011 09:39:00   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   fixed issue with renaming tools
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   17 Dec 2010 10:07:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to not clear PPQ level variables on a NAK.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   16 Dec 2010 11:16:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed but in unassigned size variable in write outputs functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   15 Dec 2010 14:05:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated combind logic back into Output List from the PPQ.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   15 Dec 2010 10:04:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix connections and synchronization issues with the IO Sub-system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   13 Dec 2010 11:25:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Change the usage of _variant_t to clear the variable after use to eliminate invalid variant type for assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   08 Dec 2010 13:15:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   05 Nov 2010 11:08:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove redundent data objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   21 Oct 2010 11:02:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change internal value object to variant value object from double value object.  Added persistance to configuration object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   29 Jun 2010 14:08:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to change object validity test and casting changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   24 May 2010 08:27:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  692
 * SCR Title:  Fix issues when adding an output to the system and SVObserver will not inspect
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated rebuild functionality to not do a double lock and a single unlock.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   16 Feb 2010 10:22:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the locking functionality to make all methods consuistant.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   27 Oct 2009 18:53:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated code to fix issues with disconnecting cameras and invalid image behavior.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   24 Sep 2009 09:32:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed compiler warning about type comparision mismatch.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   18 Aug 2009 08:46:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix problem with trigger toggle and output delays.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   30 Jul 2009 12:14:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   14 Jul 2009 14:58:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use new data manager objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   22 Jan 2007 10:36:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  588
 * SCR Title:  Add the RAID Error Bit for the digital IO
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added the new RaidError bit
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   19 Jul 2005 14:49:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented new value object Get/Set
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   29 Jun 2005 07:30:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  493
 * SCR Title:  Fix Bug in SVOutputObjectList
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added RemoveUnusedOutputs Function
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   16 Jun 2004 07:47:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  410
 * SCR Title:  Fix DDE IO
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated WriteOutput and WriteOutputs methods to get SVDWordValueObjects as unsigned decimals instead of strings.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   11 Jun 2004 07:50:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  410
 * SCR Title:  Fix DDE IO
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed problem with DDE State.  State will now appear as a PPQ variable PPQ_X.State
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   17 Dec 2003 14:14:22   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  381
 * SCR Title:  Combine PPQ Data to Inspection Data for a Digital Output
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Fixed Trigger Toggle and other outputs that are written at times different than the rest of the outputs so that they no longer conflict.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   25 Sep 2003 14:29:10   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  381
 * SCR Title:  Combine PPQ Data to Inspection Data for a Digital Output
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Add code to support the combining of digital outputs with PPQ values.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   18 Aug 2003 16:02:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed SVDigitalIOControlClass and replaced it with using SVIOConfigurationInterfaceClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   08 Jul 2003 10:18:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated Create, WriteOutput, and WriteOutputs method with new I/O interface class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   25 Apr 2003 11:25:48   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  341
 * SCR Title:  DDE Input and Outputs are no longer working correctly
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Fixed several problems so that the DDE inputs and outputs will work like they used to in pre SVO 4.00 versions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   22 Apr 2003 13:37:30   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   16 Apr 2003 17:31:42   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  341
 * SCR Title:  DDE Input and Outputs are no longer working correctly
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made modifications to the DDE inputs/outputs. Some objects are now available again and fixed some bugs with existing code. State output and PPQ inputs are again available as DDE outputs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   20 Mar 2003 10:30:10   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  332
 * SCR Title:  Module Ready not available after being unassigned and other small I/O bugs
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed variant to on call VariantInit once and VariantClear in the loop to clean them up.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   19 Mar 2003 15:20:40   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  332
 * SCR Title:  Module Ready not available after being unassigned and other small I/O bugs
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed some problems with input/outputs and the Go online/Go offline process. Also, made changes to force the PPQ to always have valid inputs and outputs even without an inspection attached.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   14 Mar 2003 17:15:30   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  332
 * SCR Title:  Module Ready not available after being unassigned and other small I/O bugs
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Modified code to reset an output just before it is removed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   05 Mar 2003 13:42:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  332
 * SCR Title:  Module Ready not available after being unassigned and other small I/O bugs
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Fixed the code so that Module Ready is available if it isn't currently assigned.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 Jan 2003 11:43:10   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added new code to fully enable Remote Inputs in the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Dec 2002 20:34:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  304
 * SCR Title:  Fix crash after deleting a PPQ and its attached inspection(s)
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated access function to prevent access violation during executation.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:50:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
