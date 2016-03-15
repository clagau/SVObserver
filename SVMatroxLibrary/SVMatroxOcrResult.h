// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxOcrResult
// * .File Name       : $Workfile:   SVMatroxOcrResult.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:25:04  $
// ******************************************************************************

#ifndef SV_MATROX_OCR_RESULT_H
#define SV_MATROX_OCR_RESULT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVMatroxTypedefs.h"

class SVMatroxOcrInterface;

/**
@SVObjectName Matrox Ocr Result

@SVObjectOverview This is a data class that holds a font object result identifier.

@SVObjectOperations This class has a function to detect if it is empty.  When this class is destroyed the matrox handle will be freed.

*/
class SVMatroxOcrResult  
{
	friend SVMatroxOcrInterface;
public:
	SVMatroxOcrResult();
	virtual ~SVMatroxOcrResult();
	bool empty() const;
	bool operator<(const SVMatroxOcrResult& p_rBuf) const {return m_OcrResultID < p_rBuf.m_OcrResultID;};

protected:
	SVMatroxIdentifier m_OcrResultID;

private:
	void LocalClear();
};

#endif // #ifndef SV_MATROX_OCR_RESULT_H

