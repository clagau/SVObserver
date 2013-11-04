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

#include "stdafx.h"
#include "SVInputObjectList.h"
#include "SVInfoStructs.h"
#include "SVValueObject.h"
#include "SVDigitalInputObject1.h"
#include "SVRemoteInputObject.h"
#include "SVCameraDataInputObject.h"

SVInputObjectList::SVInputObjectList( LPCSTR ObjectName )
: SVObjectClass( ObjectName ), m_bCreated( false )
{
}

SVInputObjectList::SVInputObjectList( SVObjectClass* POwner, int StringResourceID )
: SVObjectClass( POwner, StringResourceID ), m_bCreated( false )
{
}

SVInputObjectList::~SVInputObjectList()
{
	if( m_bCreated )
	{
		Destroy();
	}
}

BOOL SVInputObjectList::Create()
{
	outObjectInfo.ObjectTypeInfo.ObjectType = SVInputObjectListType;

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

BOOL SVInputObjectList::Destroy()
{
	try
	{
		if( m_bCreated )
		{
			Lock();

			SVGuidSVInputObjectPtrMap::iterator	l_Iter = m_InputObjects.begin();

			while( l_Iter != m_InputObjects.end() )
			{
				SVInputObject *pInput = l_Iter->second;

				l_Iter = m_InputObjects.erase( l_Iter );

				if( pInput != NULL )
				{
					delete pInput;
				}
			}

			Unlock();

			::DeleteCriticalSection( &m_hCriticalSection );

		}// end if
	}
	catch(...)
	{
		return FALSE;
	}

	m_bCreated = FALSE;
	return TRUE;
}// end Destroy

BOOL SVInputObjectList::Lock() const
{
	if( m_bCreated )
	{
		::EnterCriticalSection( const_cast< LPCRITICAL_SECTION >( &m_hCriticalSection ) );

		return TRUE;
	}// end if

	return FALSE;
}// end Lock

BOOL SVInputObjectList::Unlock() const
{
	if( m_bCreated )
	{
		::LeaveCriticalSection( const_cast< LPCRITICAL_SECTION >( &m_hCriticalSection ) );

		return TRUE;
	}// end if

	return FALSE;
}// end Unlock

HRESULT SVInputObjectList::AttachInput( SVInputObject* pInput )
{
	HRESULT l_Status = S_OK;

	if( pInput != NULL )
	{
		if( Lock() )
		{
			SVGuidSVInputObjectPtrMap::iterator	l_Iter = m_InputObjects.find( pInput->GetUniqueObjectID() );

			if( l_Iter == m_InputObjects.end() )
			{
				m_InputObjects[ pInput->GetUniqueObjectID() ] = pInput;
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

HRESULT SVInputObjectList::DetachInput( const SVGUID& p_rOutputID )
{
	HRESULT l_Status = S_OK;

	if( Lock() )
	{
		SVGuidSVInputObjectPtrMap::iterator	l_Iter = m_InputObjects.find( p_rOutputID );

		if( l_Iter != m_InputObjects.end() )
		{
			m_InputObjects.erase( l_Iter );
		}

		Unlock();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}// end RemoveInput

BOOL SVInputObjectList::ReadInputs( const SVIOEntryHostStructPtrList& p_rInputs, SVVariantBoolVector& p_rInputValues )
{
	size_t l_IOSize( p_rInputs.size() );
	size_t i( 0 );

	if( Lock() )
	{
		p_rInputValues.resize( l_IOSize );

		for( i = 0; i < l_IOSize; i++ )
		{
			SVIOEntryHostStructPtr pIOEntry = p_rInputs[i];

			// Check if output is enabled for this call
			if( !( pIOEntry.empty() ) && pIOEntry->m_Enabled )
			{
				SVInputObject *pInput = NULL;

				SVGuidSVInputObjectPtrMap::iterator	l_Iter = m_InputObjects.find( pIOEntry->m_IOId );

				if( l_Iter != m_InputObjects.end() )
				{
					pInput = l_Iter->second;
				}

				if( pInput != NULL )
				{
					p_rInputValues[ i ].second = ( pInput->Read( p_rInputValues[ i ].first ) == S_OK );
				}
				else
				{
					p_rInputValues[ i ].first.Clear();
					p_rInputValues[ i ].second = false;
				}
			}// end if
		}// end for

		Unlock();
		return TRUE;
	}// end if

	return FALSE;
}// end ReadInputs

BOOL SVInputObjectList::ReadInput( SVIOEntryStruct pIOEntry, _variant_t& p_rVariant )
{
	p_rVariant.Clear();

	BOOL l_Status = Lock();

	if( l_Status )
	{
		// Check if output is enabled for this call
		l_Status = ( !( pIOEntry.empty() ) && pIOEntry.m_IOEntryPtr->m_Enabled );

		if( l_Status )
		{
			SVGuidSVInputObjectPtrMap::iterator	l_Iter = m_InputObjects.find( pIOEntry.m_IOEntryPtr->m_IOId );

			l_Status = ( l_Iter != m_InputObjects.end() );

			if( l_Status )
			{
				SVInputObject* l_pInput = l_Iter->second;

				l_Status = ( l_pInput != NULL ) && ( l_pInput->Read( p_rVariant ) == S_OK ); 
			}
		}

		Unlock();
	}

	return l_Status;
}// end ReadInput

BOOL SVInputObjectList::FillInputs( SVIOEntryHostStructPtrList& p_IOEntries )
{
	p_IOEntries.clear();

	if( Lock() )
	{
		SVGuidSVInputObjectPtrMap::iterator	l_Iter = m_InputObjects.begin();

		while( l_Iter != m_InputObjects.end() )
		{
			SVInputObject *pInput = l_Iter->second;

			SVIOEntryHostStructPtr pIOEntry;

			pIOEntry = new SVIOEntryHostStruct;

			pIOEntry->m_IOId = pInput->GetUniqueObjectID();

			if( SV_IS_KIND_OF( pInput, SVDigitalInputObject ) )
				pIOEntry->m_ObjectType = IO_DIGITAL_INPUT;
			else if( SV_IS_KIND_OF( pInput, SVRemoteInputObject ) )
				pIOEntry->m_ObjectType = IO_REMOTE_INPUT;
			else if( SV_IS_KIND_OF( pInput, SVCameraDataInputObject ) )
				pIOEntry->m_ObjectType = IO_CAMERA_DATA_INPUT;

			p_IOEntries.push_back( pIOEntry );

			++l_Iter;
		}

		Unlock();
		return TRUE;
	}

	return FALSE;
}

BOOL SVInputObjectList::GetRemoteInputCount( long &lCount )
{
	long lTempCount;

	// Set to default of 0
	lCount = 0;
	lTempCount = 0;

	if( Lock() )
	{
		SVGuidSVInputObjectPtrMap::iterator	l_Iter = m_InputObjects.begin();

		while( l_Iter != m_InputObjects.end() )
		{
			SVInputObject *pInput = l_Iter->second;

			if( SV_IS_KIND_OF( pInput, SVRemoteInputObject ) )
				lTempCount++;

			++l_Iter;
		}

		lCount = lTempCount;

		Unlock();
		return TRUE;
	}

	return FALSE;
}

BOOL SVInputObjectList::SetRemoteInput( long lIndex, VARIANT vtValue )
{
	if( Lock() )
	{
		BOOL bFound = FALSE;

		SVGuidSVInputObjectPtrMap::iterator	l_Iter = m_InputObjects.begin();

		while( l_Iter != m_InputObjects.end() )
		{
			SVRemoteInputObject* pInput = dynamic_cast< SVRemoteInputObject* >( l_Iter->second );

			if( pInput != NULL )
			{
				bFound = ( lIndex == pInput->m_lIndex );

				if( bFound )
				{
					pInput->WriteCache( vtValue );

					break;
				}
			}

			++l_Iter;
		}

		Unlock();

		return bFound;
	}

	return FALSE;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVInputObjectList.cpp_v  $
 * 
 *    Rev 1.2   07 Aug 2013 13:27:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Renamed SVIOEntryHostStructPtrVector to SVIOEntryHostStructPtrList.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 May 2013 20:06:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   16 Apr 2013 14:24:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  822
 * SCR Title:  Remove DDE Input and Output functionality from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed DDE functionality from the source code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:59:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   21 Jan 2013 10:53:20   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised FillInputs methods to check for camera inputs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   16 Aug 2012 17:01:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the input and output list to use new variant parameter in the read and write operations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   15 Aug 2012 14:37:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new Singleton version of the SVIOConfigurationInterfaceClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   13 Aug 2012 14:27:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to change the functionality associated with the Input Object List to use a new method to get the input from the input list.  If the input does not exist in the input list, a NULL pointer is returned.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   26 Jul 2011 07:49:30   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  733
 * SCR Title:  fix bug with Remote Inputs not being avaliable
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed issues with Remote Inputs being put into the list multiple times. 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   19 Apr 2011 16:17:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix issues with IO Assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   18 Apr 2011 10:15:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to move the shared data for the IO Entry data to a different class to fix issues with PPQ and Inspection shared data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   15 Dec 2010 10:00:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix connections and synchronization issues with the IO Sub-system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   13 Dec 2010 11:23:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Change the usage of _variant_t to clear the variable after use to eliminate invalid variant type for assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   08 Dec 2010 12:51:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   14 Jul 2009 14:00:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new data manager objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   22 Apr 2003 10:25:08   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   20 Mar 2003 10:30:06   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  332
 * SCR Title:  Module Ready not available after being unassigned and other small I/O bugs
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed variant to on call VariantInit once and VariantClear in the loop to clean them up.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   19 Mar 2003 15:20:30   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  332
 * SCR Title:  Module Ready not available after being unassigned and other small I/O bugs
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed some problems with input/outputs and the Go online/Go offline process. Also, made changes to force the PPQ to always have valid inputs and outputs even without an inspection attached.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Jan 2003 13:42:20   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed bugs with SetInputs and SetToolParameterList from the SIAC.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 Jan 2003 11:43:12   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added new code to fully enable Remote Inputs in the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Dec 2002 20:37:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  304
 * SCR Title:  Fix crash after deleting a PPQ and its attached inspection(s)
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated appropriate methods to prevent access violation during runtime.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:46:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
