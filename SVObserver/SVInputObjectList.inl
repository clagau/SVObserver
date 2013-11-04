//******************************************************************************
//* COPYRIGHT (c) 2010 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInputObjectList
//* .File Name       : $Workfile:   SVInputObjectList.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   11 Jun 2013 15:26:16  $
//******************************************************************************

#ifndef SVINPUTOBJECTLIST_INL
#define SVINPUTOBJECTLIST_INL

#include "SVInputObjectList.h"

template< typename SVInputType >
HRESULT SVInputObjectList::GetInput( const SVGUID& p_rInputID, SVInputType*& p_rpInput ) const
{
	HRESULT l_Status = S_OK;

	p_rpInput = NULL;

	if( Lock() )
	{
		SVGuidSVInputObjectPtrMap::const_iterator l_Iter = m_InputObjects.find( p_rInputID );

		if( l_Iter != m_InputObjects.end() )
		{
			p_rpInput = dynamic_cast< SVInputType* >( l_Iter->second );

			if( p_rpInput == NULL )
			{
				l_Status = E_FAIL;
			}
		}
		else
		{
			l_Status = E_INVALIDARG;
		}

		Unlock();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

template< typename SVInputType >
HRESULT SVInputObjectList::GetInput( const SVString& p_rInputName, SVInputType*& p_rpInput ) const
{
	HRESULT l_Status = S_OK;

	p_rpInput = NULL;

	if( Lock() )
	{
		SVGuidSVInputObjectPtrMap::const_iterator	l_Iter;
		
		for( l_Iter = m_InputObjects.begin(); p_rpInput == NULL && l_Iter != m_InputObjects.end(); ++l_Iter )
		{
			SVInputObject* pInput = l_Iter->second;

			if ( pInput != NULL )
			{
				if ( p_rInputName == pInput->GetName() )
				{
					p_rpInput = dynamic_cast< SVInputType* >( pInput );
				}
			}
		}

		if( p_rpInput == NULL )
		{
			l_Status = E_INVALIDARG;
		}

		Unlock();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

template< typename SVInputType >
HRESULT SVInputObjectList::GetInputFlyweight( const SVString& p_rInputName, SVInputType*& p_rpInput )
{
	HRESULT l_Status = S_OK;

	p_rpInput = NULL;

	if( Lock() )
	{
		SVGuidSVInputObjectPtrMap::const_iterator	l_Iter;
		
		for( l_Iter = m_InputObjects.begin(); p_rpInput == NULL && l_Iter != m_InputObjects.end(); ++l_Iter )
		{
			SVInputObject* pInput = l_Iter->second;

			if ( pInput != NULL )
			{
				if ( p_rInputName == pInput->GetName() )
				{
					p_rpInput = dynamic_cast< SVInputType* >( pInput );
				}
			}
		}

		if( p_rpInput == NULL )
		{
			p_rpInput = new SVInputType;

			if( p_rpInput != NULL )
			{
				p_rpInput->SetName( p_rInputName.c_str() );

				if( AttachInput( p_rpInput ) != S_OK )
				{
					delete p_rpInput;
					p_rpInput = NULL;
				}
			}
		}

		if( p_rpInput == NULL )
		{
			l_Status = E_INVALIDARG;
		}

		Unlock();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVInputObjectList.inl_v  $
 * 
 *    Rev 1.1   11 Jun 2013 15:26:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 017.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   21 May 2013 13:02:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  824
 * SCR Title:  Add Remote Access to SVObserver Application Configuration Variables
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a new method to get an input if it exists and return NULL if it does not.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 11:00:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Aug 2012 14:37:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new Singleton version of the SVIOConfigurationInterfaceClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 Aug 2012 14:27:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to change the functionality associated with the Input Object List to use a new method to get the input from the input list.  If the input does not exist in the input list, a NULL pointer is returned.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Dec 2010 10:00:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix connections and synchronization issues with the IO Sub-system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
