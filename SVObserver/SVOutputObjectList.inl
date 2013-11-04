//******************************************************************************
//* COPYRIGHT (c) 2010 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOutputObjectList
//* .File Name       : $Workfile:   SVOutputObjectList.inl  $
//* ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 13:16:24  $
//******************************************************************************

#ifndef SVOUTPUTOBJECTLIST_INL
#define SVOUTPUTOBJECTLIST_INL

#include "SVOutputObjectList.h"

template< typename SVOutputType >
HRESULT SVOutputObjectList::GetOutput( const SVGUID& p_rOutputID, SVOutputType*& p_rpOutput ) const
{
	HRESULT l_Status = S_OK;

	p_rpOutput = NULL;

	if( Lock() )
	{
		SVGuidSVOutputObjectPtrMap::const_iterator l_Iter = m_OutputObjects.find( p_rOutputID );

		if( l_Iter != m_OutputObjects.end() )
		{
			p_rpOutput = dynamic_cast< SVOutputType* >( l_Iter->second );

			if( p_rpOutput == NULL )
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

template< typename SVOutputType >
HRESULT SVOutputObjectList::GetOutput( LPCTSTR p_szName, SVOutputType*& p_rpOutput ) const
{
	HRESULT l_Status = S_OK;

	SVString l_Name = p_szName;

	p_rpOutput = NULL;

	if( Lock() )
	{
		SVGuidSVOutputObjectPtrMap::const_iterator l_Iter = m_OutputObjects.begin();

		while( p_rpOutput == NULL && l_Iter != m_OutputObjects.end() )
		{
			SVOutputObject* l_pOutput = l_Iter->second;

			if( l_pOutput != NULL && l_Name == l_pOutput->GetName() )
			{
				p_rpOutput = dynamic_cast< SVOutputType* >( l_pOutput );
			}

			++l_Iter;
		}

		if( p_rpOutput == NULL )
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

template< typename SVOutputType >
HRESULT SVOutputObjectList::GetOutputFlyweight( LPCTSTR p_szName, SVOutputType*& p_rpOutput )
{
	HRESULT l_Status = S_OK;

	p_rpOutput = NULL;

	if( Lock() )
	{
		SVString l_Name = p_szName;

		l_Status = GetOutput( p_szName, p_rpOutput );

		if( p_rpOutput == NULL )
		{
			p_rpOutput = new SVOutputType;

			if( p_rpOutput != NULL )
			{
				p_rpOutput->SetName( l_Name.c_str() );

				l_Status = AttachOutput( p_rpOutput );

				if( l_Status != S_OK )
				{
					delete p_rpOutput;

					p_rpOutput = NULL;
				}
			}
		}

		if( p_rpOutput == NULL )
		{
			if( l_Status == S_OK )
			{
				l_Status = E_INVALIDARG;
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

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOutputObjectList.inl_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:16:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Aug 2012 17:07:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated output list to be able to collect an output if it exists in the output list.  This adjust the behavior in the publish list to make sure the lists are updated correctly in the PPQ.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Aug 2012 14:41:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new Singleton version of the SVIOConfigurationInterfaceClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Dec 2010 10:04:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix connections and synchronization issues with the IO Sub-system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
