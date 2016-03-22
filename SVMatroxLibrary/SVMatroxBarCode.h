// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxBarCode
// * .File Name       : $Workfile:   SVMatroxBarCode.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 14:56:36  $
// ******************************************************************************

#pragma once

#include "SVMatroxTypedefs.h"

class SVMatroxBarCodeInterface;


/**
@SVObjectName Matrox Bar Code

@SVObjectOverview This is a data class that holds the identifier for a bar code.

@SVObjectOperations The empty function is used to detect if the handle is empty.  When this class is destroyed the the LocalClear will free the matrox handle.

*/
class SVMatroxBarCode  
{
	friend SVMatroxBarCodeInterface;
public:
	SVMatroxBarCode();
	virtual ~SVMatroxBarCode();

	bool empty() const;
	SVMatroxBarCode& operator=( const SVMatroxBarCode& p_rhs );
	bool operator<(const SVMatroxBarCode& p_rBuf) const {return m_BarCodeId < p_rBuf.m_BarCodeId;};

protected:
	SVMatroxIdentifier m_BarCodeId;

private:
	void LocalClear();
};

