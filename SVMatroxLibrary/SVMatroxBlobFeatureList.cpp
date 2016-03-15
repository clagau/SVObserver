// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxBlobFeatureList
// * .File Name       : $Workfile:   SVMatroxBlobFeatureList.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 14:56:54  $
// ******************************************************************************

#include "stdafx.h"
#include "SVMatroxBlobFeatureList.h"
#include "SVMatroxBlobInterface.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVMatroxBlobFeatureList::SVMatroxBlobFeatureList()
{
	m_BlobFeatureListID = 0;
}

SVMatroxBlobFeatureList::~SVMatroxBlobFeatureList()
{
//	if( m_BlobFeatureListID != 0 )
//	{
//		LocalClear();
//	}
}

bool SVMatroxBlobFeatureList::empty() const
{
	return m_BlobFeatureListID == 0;
}

void SVMatroxBlobFeatureList::LocalClear()
{
	SVMatroxBlobInterface l_Interface;
	l_Interface.Destroy( *this );
}


