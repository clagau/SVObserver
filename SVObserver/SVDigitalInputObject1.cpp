//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDigitalInputObject
//* .File Name       : $Workfile:   SVDigitalInputObject1.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:11:04  $
//******************************************************************************

#include "stdafx.h"
#include "SVDigitalInputObject1.h"

#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"

SVDigitalInputObject::SVDigitalInputObject( LPCSTR strObjectName )
:SVInputObject( strObjectName )
, m_lChannel( -1 )
, m_bForced( false )
, m_bInverted( true )
, m_bLastValue( false )
, m_bDefaultValue( false )
, m_bForcedValue( false )
{
	m_bCreated		= FALSE;
}

SVDigitalInputObject::SVDigitalInputObject( SVObjectClass *pOwner, int StringResourceID )
:SVInputObject( pOwner, StringResourceID )
, m_lChannel( -1 )
, m_bForced( false )
, m_bInverted( true )
, m_bLastValue( false )
, m_bDefaultValue( false )
, m_bForcedValue( false )
{
	m_bCreated		= FALSE;
}

SVDigitalInputObject::~SVDigitalInputObject()
{
	if( m_bCreated )
		Destroy();

	m_bCreated		= FALSE;
}

BOOL SVDigitalInputObject::Create()
{
	m_bCreated = TRUE;

	return TRUE;
}// end Create

BOOL SVDigitalInputObject::Destroy()
{
	m_bCreated = FALSE;

	return TRUE;
}// end Destroy

BOOL SVDigitalInputObject::IsCreated()
{
	return m_bCreated;
}// end IsCreated

HRESULT SVDigitalInputObject::Read( _variant_t& p_rValue )
{
	HRESULT l_Status = S_OK;
	bool l_bValue = false;

	l_Status = SVIOConfigurationInterfaceClass::Instance().GetDigitalInputValue( m_lChannel, l_bValue );

	if( l_Status == S_OK )
	{
		p_rValue = l_bValue;
	}
	else
	{
		p_rValue.Clear();
	}

	return l_Status;
}// end Read

BOOL SVDigitalInputObject::Force( bool bForce, bool bForcedValue )
{
	BOOL l_bOk = FALSE;

	m_bForced = bForce;
	m_bForcedValue = bForcedValue;

	l_bOk = SVIOConfigurationInterfaceClass::Instance().SetDigitalInputIsForced( m_lChannel, m_bForced ) == S_OK;
	l_bOk = SVIOConfigurationInterfaceClass::Instance().SetDigitalInputForcedValue( m_lChannel, m_bForcedValue ) == S_OK && l_bOk;

	return l_bOk;
}// end Force

BOOL SVDigitalInputObject::Invert( bool bInvert )
{
	BOOL l_bOk = FALSE;

	m_bInverted = bInvert;

	l_bOk = SVIOConfigurationInterfaceClass::Instance().SetDigitalInputIsInverted( m_lChannel, m_bInverted ) == S_OK;

	return l_bOk;
}// end Invert

bool SVDigitalInputObject::IsForced() const
{
	return m_bForced;
}// end IsForced

bool SVDigitalInputObject::GetForcedValue() const
{
	return m_bForcedValue;
}// end GetForcedValue

bool SVDigitalInputObject::GetValue() const
{
	return m_bLastValue;
}// end GetValue

bool SVDigitalInputObject::IsInverted() const
{
	return m_bInverted;
}// end IsInverted

BOOL SVDigitalInputObject::SetChannel( long lChannel )
{
	m_lChannel = lChannel;
	return TRUE;
}// end SetChannel

long SVDigitalInputObject::GetChannel() const
{
	return m_lChannel;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVDigitalInputObject1.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:11:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   16 Aug 2012 16:54:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change the read and write functionality to use a variant as the data type instead of a void pointer.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   15 Aug 2012 14:35:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new Singleton version of the SVIOConfigurationInterfaceClass.
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
 *    Rev 1.3   08 Jul 2003 09:11:46   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved IO to DLL
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Apr 2003 08:54:46   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 Jan 2003 11:36:26   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Modified the base InputObject class and its derived for easier RemoteInput support.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:43:52   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
