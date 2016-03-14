//******************************************************************************
//* COPYRIGHT (c) 2007 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxBuffer
//* .File Name       : $Workfile:   SVMatroxBuffer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:59:52  $
//******************************************************************************

#ifndef SV_MATROX_BUFFER_H
#define SV_MATROX_BUFFER_H

#include "SVMatroxBufferFacade.h"

/**
@SVObjectName Matrox Buffer

@SVObjectOverview This class is a data holder for a matrox buffer id.

@SVObjectOperations The empty function is used to detect if the handle is empty.  The freeing of the matrox handle held by this class is controlled externally to this class at the moment.

*/
class SVMatroxBuffer
{
public:
	friend class SVExternalToolTask;
	friend class SVMatroxBufferArray;
	friend class SVMatroxBufferInterface;
	friend class SVMatroxBarCodeInterface;
	friend class SVMatroxBlobInterface;
	friend class SVMatroxDigitizerInterface;
	friend class SVMatroxDisplayInterface;
	friend class SVMatroxGraphicsInterface;
	friend class SVMatroxImageInterface;
	friend class SVMatroxOcrInterface;
	friend class SVMatroxPatternInterface;
	friend class SVMatroxDisplayBuffer;

	SVMatroxBuffer();
	SVMatroxBuffer(const SVMatroxBuffer& p_rBuf);
	SVMatroxBuffer(const SVMatroxDisplayBuffer& p_rBuf);

	virtual ~SVMatroxBuffer();

	bool empty() const;

	void clear();

	void swap( SVMatroxBuffer& p_rBuf );

	const SVMatroxBuffer& operator=( const SVMatroxBuffer& p_rBuf );

	bool operator<( const SVMatroxBuffer& p_rBuf ) const;

	bool IsParent( const SVMatroxBuffer& p_rParent ) const;

protected:
	SVMatroxIdentifier GetIdentifier() const;

	SVMatroxBufferPtr m_BufferPtr;

};

#endif

