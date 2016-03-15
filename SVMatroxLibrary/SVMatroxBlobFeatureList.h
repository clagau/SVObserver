// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxBlobFeatureList
// * .File Name       : $Workfile:   SVMatroxBlobFeatureList.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 14:55:52  $
// ******************************************************************************

#ifndef SV_MATROX_BLOB_FEATURE_LIST_H
#define SV_MATROX_BLOB_FEATURE_LIST_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVMatroxTypedefs.h"

class SVMatroxBlobInterface;

/**
@SVObjectName Matrox Blob Feature List

@SVObjectOverview This is a data class that holds a blob analyzer identifier.

@SVObjectOperations The empty function is used to detect if the handle is empty.  When this class is destroyed the the LocalClear will free the matrox handle.

*/class SVMatroxBlobFeatureList  
{
	friend SVMatroxBlobInterface;
public:
	SVMatroxBlobFeatureList();
	virtual ~SVMatroxBlobFeatureList();
	bool operator<(const SVMatroxBlobFeatureList& p_rBuf) const {return m_BlobFeatureListID < p_rBuf.m_BlobFeatureListID;};

	bool empty() const;

protected:
	SVMatroxIdentifier m_BlobFeatureListID;

private:
	void LocalClear();
};

#endif // #ifndef SV_MATROX_BLOB_FEATURE_LIST_H

