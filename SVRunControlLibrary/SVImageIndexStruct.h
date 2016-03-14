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

