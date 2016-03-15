// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxBlobResult
// * .File Name       : $Workfile:   SVMatroxBlobResult.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 14:59:42  $
// ******************************************************************************

#ifndef SV_MATROX_BLOB_RESULT_H
#define SV_MATROX_BLOB_RESULT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVMatroxTypedefs.h"

class SVMatroxBlobInterface;

/**
@SVObjectName Matrox Blob Result

@SVObjectOverview This is a data class that holds a blob result identifier.

@SVObjectOperations This class has a function to detect if it is empty.  When this class is destroyed the matrox handle will be freed.

*/
class SVMatroxBlobResult  
{
	friend SVMatroxBlobInterface;
public:
	SVMatroxBlobResult();
	virtual ~SVMatroxBlobResult();

	bool empty() const;
	bool operator<(const SVMatroxBlobResult& p_rBuf) const {return m_BlobResultId < p_rBuf.m_BlobResultId;};

protected:
	SVMatroxIdentifier m_BlobResultId;

private:
	void LocalClear();
};

#endif // #ifndef SV_MATROX_BLOB_RESULT_H

