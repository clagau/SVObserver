//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDigitalOutputObject
//* .File Name       : $Workfile:   SVDigitalOutputObject1.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:11:20  $
//******************************************************************************

#include "stdafx.h"
#include "SVDigitalOutputObject1.h"

#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"

SVDigitalOutputObject::SVDigitalOutputObject( LPCSTR strObjectName )
:SVOutputObject( strObjectName )
, m_lChannel( -1 )
, m_bForced( false )
, m_bInverted( true )
, m_bCombined( false )
, m_bLastValue( false )
, m_bDefaultValue( false )
, m_bForcedValue( false )
, m_bCombinedACK( true )
{
	m_bCreated		= false;
}

SVDigitalOutputObject::SVDigitalOutputObject( SVObjectClass *pOwner, int StringResourceID )
:SVOutputObject( pOwner, StringResourceID )
, m_lChannel( -1 )
, m_bForced( false )
, m_bInverted( true )
, m_bCombined( false )
, m_bLastValue( false )
, m_bDefaultValue( false )
, m_bForcedValue( false )
, m_bCombinedACK( true )
{
	m_bCreated		= false;
}

SVDigitalOutputObject::~SVDigitalOutputObject()
{
	if( m_bCreated )
		Destroy();

	m_bCreated		= false;
}

BOOL SVDigitalOutputObject::Create()
{
	m_bCreated = true;
	
	return TRUE;
}

BOOL SVDigitalOutputObject::Destroy()
{
	m_bCreated = false;

	return TRUE;
}

HRESULT SVDigitalOutputObject::Write( const _variant_t& p_rValue )
{
	HRESULT l_Status = S_OK;

	m_bLastValue = p_rValue;

	l_Status = SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputValue( m_lChannel, m_bLastValue );

	return l_Status;
}

HRESULT SVDigitalOutputObject::Reset()
{
	HRESULT l_Status = S_OK;

	l_Status = SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputValue( m_lChannel, m_bDefaultValue );

	return l_Status;
}

BOOL SVDigitalOutputObject::Force( bool bForce, bool bForcedValue )
{
	BOOL l_bOk = FALSE;

	m_bForced = bForce;
	m_bForcedValue = bForcedValue;

	l_bOk = SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputIsForced( m_lChannel, m_bForced ) == S_OK;
	l_bOk = SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputForcedValue( m_lChannel, m_bForcedValue ) == S_OK && l_bOk;

	return l_bOk;
}

BOOL SVDigitalOutputObject::Invert( bool bInvert )
{
	BOOL l_bOk = FALSE;

	m_bInverted = bInvert;

	l_bOk = SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputIsInverted( m_lChannel, m_bInverted ) == S_OK;

	return l_bOk;
}

BOOL SVDigitalOutputObject::Combine( bool bCombine, bool bCombineACK )
{
	BOOL l_bOk = TRUE;

	m_bCombined = bCombine;
	m_bCombinedACK = bCombineACK;

	return l_bOk;
}

bool SVDigitalOutputObject::IsForced() const
{
	return m_bForced;
}

bool SVDigitalOutputObject::GetForcedValue() const
{
	return m_bForcedValue;
}

bool SVDigitalOutputObject::IsInverted() const
{
	return m_bInverted;
}

bool SVDigitalOutputObject::IsCombined() const
{
	return m_bCombined;
}

bool SVDigitalOutputObject::GetCombinedValue() const
{
	return m_bCombinedACK;
}

bool SVDigitalOutputObject::GetValue() const
{
	return m_bLastValue;
}

BOOL SVDigitalOutputObject::SetChannel( long lChannel )
{
	m_lChannel = lChannel;
	return TRUE;
}

long SVDigitalOutputObject::GetChannel() const
{
	return m_lChannel;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVDigitalOutputObject1.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:11:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   16 Aug 2012 16:54:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change the read and write functionality to use a variant as the data type instead of a void pointer.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   15 Aug 2012 14:35:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new Singleton version of the SVIOConfigurationInterfaceClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   15 Dec 2010 09:53:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix connections and synchronization issues with the IO Sub-system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   15 Dec 2009 15:56:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   21 Jun 2007 12:27:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   25 Sep 2003 14:33:58   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  381
 * SCR Title:  Combine PPQ Data to Inspection Data for a Digital Output
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Add code to support the combining of digital outputs with PPQ values.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Jul 2003 09:11:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved IO to DLL
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 09:03:04   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:43:56   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
