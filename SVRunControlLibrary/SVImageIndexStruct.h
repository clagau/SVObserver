//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageIndexStruct
//* .File Name       : $Workfile:   SVImageIndexStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 16:20:12  $
//******************************************************************************

#ifndef SVIMAGEINDEXSTRUCT_H
#define SVIMAGEINDEXSTRUCT_H

#include "SVDataManagerLibrary/SVDataManagerHandle.h"

struct SVImageIndexStruct
{
	//This attribute holds the index value for the camera image.
	SVDataManagerHandle		m_CameraDMIndexHandle;
	//This attribute holds the index value for the result image.
	SVDataManagerHandle		m_ResultDMIndexHandle;
	//This attribute holds the index value for the published image.
	SVDataManagerHandle		m_PublishedResultDMIndexHandle;

	//This constructor calls the init method to initialize the attributes.
	SVImageIndexStruct()
	{
	}

	//This destructor clears the elements from the internal attributes.
	virtual ~SVImageIndexStruct()
	{
		clear();
	}

	//This method copies the index data from the provided object into this object.
	SVImageIndexStruct( const SVImageIndexStruct& p_rObject )
	{
		m_CameraDMIndexHandle.Assign( p_rObject.m_CameraDMIndexHandle, p_rObject.m_CameraDMIndexHandle.GetLockType() );
		m_ResultDMIndexHandle.Assign( p_rObject.m_ResultDMIndexHandle, p_rObject.m_ResultDMIndexHandle.GetLockType() );
		m_PublishedResultDMIndexHandle.Assign( p_rObject.m_PublishedResultDMIndexHandle, p_rObject.m_PublishedResultDMIndexHandle.GetLockType() );
	}

	//This method copies the index data from the provided object into this object.
	const SVImageIndexStruct& operator=( const SVImageIndexStruct& p_rObject )
	{
		if( this != &p_rObject )
		{
			m_CameraDMIndexHandle.Assign( p_rObject.m_CameraDMIndexHandle, p_rObject.m_CameraDMIndexHandle.GetLockType() );
			m_ResultDMIndexHandle.Assign( p_rObject.m_ResultDMIndexHandle, p_rObject.m_ResultDMIndexHandle.GetLockType() );
			m_PublishedResultDMIndexHandle.Assign( p_rObject.m_PublishedResultDMIndexHandle, p_rObject.m_PublishedResultDMIndexHandle.GetLockType() );
		}

		return *this;
	}

	//This method clears the elements from the internal attributes.
	void clear()
	{
		m_CameraDMIndexHandle.clear();
		m_ResultDMIndexHandle.clear();
		m_PublishedResultDMIndexHandle.clear();
	}

	//This method return the state of the object.
	bool IsNull()
	{
		return m_CameraDMIndexHandle.empty() && m_ResultDMIndexHandle.empty() && m_PublishedResultDMIndexHandle.empty();
	}

	//This operator returns whether the two object are not identical.
	friend bool operator != ( const SVImageIndexStruct& lhs, const SVImageIndexStruct& rhs )
	{
		return !(lhs == rhs);
	}
	//This operator returns whether the two object are identical.
	friend bool operator == ( const SVImageIndexStruct& lhs, const SVImageIndexStruct& rhs )
	{
		return    lhs.m_CameraDMIndexHandle.GetIndex() == rhs.m_CameraDMIndexHandle.GetIndex()
		       && lhs.m_ResultDMIndexHandle.GetIndex() == rhs.m_ResultDMIndexHandle.GetIndex()
		       && lhs.m_PublishedResultDMIndexHandle.GetIndex() == rhs.m_PublishedResultDMIndexHandle.GetIndex();
	}
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVRunControlLibrary\SVImageIndexStruct.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 16:20:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Feb 2010 11:05:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated old and added missing source code comments to existing code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Sep 2009 15:38:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   File moved to here to allow for multiple project inclusion.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 Jul 2009 13:26:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated image library to include the new data manager objects to track the indexes.  Updated DLL interface to use the new buffer transfer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   01 Nov 2005 14:42:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
