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
#include "TriggerHandling/SVIOConfigurationInterfaceClass.h"
#include "SVValueObject.h"
#include "SVDigitalOutputObject.h"
#include "SVRemoteOutputObject.h"
#include "SVInfoStructs.h"
#include "SVObserver.h"
#include "SVPPQObject.h"
#include "SVInputObjectList.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVMessage\SVMessage.h"
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

BOOL SVOutputObjectList::Create()
{
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVOutputObjectListType;

	try
	{
		m_bCreated = ::InitializeCriticalSectionAndSpinCount( &m_hCriticalSection, 4000 );
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

BOOL SVOutputObjectList::WriteOutputs( SVIOEntryStructVector& p_IOEntries, long lDataIndex, bool p_ACK, bool p_NAK )
{
	
	SVOutputObject	*pStateOutput = nullptr;
	SVDigitalOutputObject	*pDigOutput = nullptr;
	_variant_t l_Variant;
	CString strValue;
	BOOL bValue = false;
	double dValue = 0.0;
	DWORD dwValue = 0;
	long lValue = 0;
	TCHAR *pStrValue = nullptr;

	if( Lock() )
	{
		pStateOutput = nullptr;

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
					SVOutputObject* pOutput = nullptr;

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

			SvTh::SVIOConfigurationInterfaceClass::Instance().GetDigitalOutputState( sizeof dwValue, (unsigned char *)&dwValue );

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
	
	SVOutputObject	*pStateOutput = nullptr;
	SVDigitalOutputObject	*pDigOutput = nullptr;
	_variant_t l_Variant;
	CString strValue;
	BOOL bValue = false;
	double dValue = 0.0;
	DWORD dwValue = 0;
	long lValue = 0;
	TCHAR *pStrValue = nullptr;

	if( Lock() )
	{
		pStateOutput = nullptr;

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
					SVOutputObject* pOutput = nullptr;

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

			SvTh::SVIOConfigurationInterfaceClass::Instance().GetDigitalOutputState( sizeof dwValue, (unsigned char *)&dwValue );

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
				SVOutputObject* pOutput = nullptr;

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
				SVOutputObject* pOutput = nullptr;

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
				SVOutputObject* pOutput = nullptr;

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
				SVOutputObject* pOutput = nullptr;

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
	SVOutputObject* pOutput = nullptr;

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

		if( SV_IS_KIND_OF( pOutput, SVDigitalOutputObject ) || SV_IS_KIND_OF( pOutput, SVRemoteOutputObject ) )
		{
			pOutput->Write( p_rValue );
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

				if( nullptr != l_pOutput )
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

					if( nullptr != l_pOutput )
					{
						delete l_pOutput;
					}
				}
				else
				{
					if( nullptr != l_pOutput )
					{
						SVDigitalOutputObject* l_pDigital = dynamic_cast< SVDigitalOutputObject* >( l_pOutput );

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

					if( nullptr != l_pOutput && SVString( _T("Module Ready") ) == l_pOutput->GetName() )
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

bool SVOutputObjectList::OutputIsNotValid( SVString p_strName )
{
	bool l_bRet = true;
	
	SVObjectClass* l_pObject;
	HRESULT l_hr = SVObjectManagerClass::Instance().GetObjectByDottedName( p_strName, l_pObject );
	// Check if the object exists.
	if( nullptr != l_pObject )
	{
		if( l_pObject->ObjectAttributesSet() & SV_PUBLISHABLE )
		{
			l_bRet = false;
		}
	}
	// Return true if object does not exist or is not published.
	return l_bRet;
}

DWORD_PTR SVOutputObjectList::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = SVMR_NOT_PROCESSED;
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

	SVDigitalOutputObject* pOutput = nullptr;

	l_Status = GetOutput( pIOEntry.m_IOEntryPtr->m_IOId, pOutput );

	if( nullptr != pOutput )
	{
		bool bValue = false;

		if( 0 <= lDataIndex )
		{
			double dValue = 0.0;

			SVString l_String = pOutput->GetName();

			if( p_ACK || l_String.substr( 0, 3 ).compare( _T( "PPQ" ) ) == 0 )
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

