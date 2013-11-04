//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteInputObject
//* .File Name       : $Workfile:   SVRemoteInputObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   12 Jun 2013 15:20:12  $
//******************************************************************************

#include "stdafx.h"
#include "SVRemoteInputObject.h"
#include "SVUtilityLibrary/SVSAFEARRAY.h"

SVRemoteInputObject::SVRemoteInputObject( LPCSTR strObjectName )
: SVInputObject( strObjectName )
{
	m_bCreated = FALSE;
	m_lIndex   = -1;
	m_vtRemoteCache = 0.0;
}

SVRemoteInputObject::SVRemoteInputObject( SVObjectClass* POwner, int StringResourceID )
: SVInputObject( POwner, StringResourceID )
{
	m_bCreated = FALSE;
	m_lIndex   = -1;
	m_vtRemoteCache = 0.0;
}

SVRemoteInputObject::~SVRemoteInputObject()
{
	if( m_bCreated )
		Destroy();
}

BOOL SVRemoteInputObject::Create()
{
	m_bCreated = TRUE;

  ::InitializeCriticalSection( &m_hCriticalSection );

	return TRUE;
}// end Create

BOOL SVRemoteInputObject::Destroy()
{
	m_vtRemoteCache.Clear();

	::DeleteCriticalSection( &m_hCriticalSection );

	m_bCreated = FALSE;

	return TRUE;
}// end Destroy

BOOL SVRemoteInputObject::IsCreated()
{
	return m_bCreated;
}// end IsCreated

BOOL SVRemoteInputObject::Lock()
{
	::EnterCriticalSection( &m_hCriticalSection );

	return TRUE;
}// end Lock

BOOL SVRemoteInputObject::Unlock()
{
	::LeaveCriticalSection( &m_hCriticalSection );

	return TRUE;
}// end Unlock

HRESULT SVRemoteInputObject::Read( _variant_t& p_rValue )
{
	HRESULT l_Status = GetCache( p_rValue );

	return l_Status;
}// end Read

HRESULT SVRemoteInputObject::WriteCache( const _variant_t& p_rValue )
{
	HRESULT l_Status = S_OK;

	_variant_t l_Temp = 0.0;

	if( p_rValue.vt & VT_ARRAY )
	{
		SVSAFEARRAY l_SafeArray( p_rValue );

		if ( l_SafeArray.size() == 1 )
		{
			l_Status = l_SafeArray.GetElement( 0, l_Temp );
		}
		else
		{
			l_Status = E_INVALIDARG;
		}
	}
	else
	{
		l_Temp = p_rValue;
	}

	if( l_Status == S_OK )
	{
		::EnterCriticalSection( &m_hCriticalSection );

		m_vtRemoteCache = l_Temp;

		::LeaveCriticalSection( &m_hCriticalSection );
	}

	return l_Status;
}// end ReadCache

HRESULT SVRemoteInputObject::GetCache( _variant_t& p_rValue )
{
	HRESULT l_Status = S_OK;

	p_rValue.Clear();

	::EnterCriticalSection( &m_hCriticalSection );

	p_rValue = m_vtRemoteCache;

	::LeaveCriticalSection( &m_hCriticalSection );

	return l_Status;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVRemoteInputObject.cpp_v  $
 * 
 *    Rev 1.1   12 Jun 2013 15:20:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 018.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   22 May 2013 12:14:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  824
 * SCR Title:  Add Remote Access to SVObserver Application Configuration Variables
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated write cache method to fix issues with safearrays in variant.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 14:47:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   16 Aug 2012 16:54:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change the read and write functionality to use a variant as the data type instead of a void pointer.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   21 Oct 2010 11:20:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change internal value object to variant value object from double value object.  Added persistance to configuration object.  Added new dialog to edit the Remote Input Value.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   21 Jun 2007 14:46:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   22 Apr 2003 14:44:12   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   19 Mar 2003 10:08:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  332
 * SCR Title:  Module Ready not available after being unassigned and other small I/O bugs
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Changed VariantClear in constructors to VariantInit
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Jan 2003 13:42:24   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed bugs with SetInputs and SetToolParameterList from the SIAC.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 Jan 2003 11:36:30   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Modified the base InputObject class and its derived for easier RemoteInput support.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:52:42   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/