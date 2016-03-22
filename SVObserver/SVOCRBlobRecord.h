//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCRBlobRecord
//* .File Name       : $Workfile:   SVOCRBlobRecord.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:54:48  $
//******************************************************************************

// undefine to use 4 axis blob sorts - right,left,up, and down
///#define __SORT_BLOBS_X_AXIS_ONLY    // X axis OCR blob sort only - 18 Oct 1999 - frb.

#pragma once

#include "wFast.h"

enum enumBlobSortDirection {
  SORT_BLOGS_NOT_DEFINED = 0,
	SORT_BLOBS_RIGHT = 1,
	SORT_BLOBS_LEFT = 2,
	SORT_BLOBS_UP = 3,
	SORT_BLOBS_DOWN = 4,
	SORT_BLOBS_SCORE = 5
};

#ifndef __SORT_BLOBS_X_AXIS_ONLY
/////////////////////////////////////////////////////////////////////////////
//
//
//
class SVOCRBlobRecord
{
public:
	SVOCRBlobRecord();
	SVOCRBlobRecord(const SVOCRBlobRecord& svOCRBR);
	~SVOCRBlobRecord();

	const SVOCRBlobRecord& operator =(const SVOCRBlobRecord& svOCRBR);

	BOOL CheckAgainst(const SVOCRBlobRecord& svOCRBR, enum enumBlobSortDirection eSortDirection);
	BOOL CheckAgainst(const SVOCRBlobRecord *svOCRBR, enum enumBlobSortDirection eSortDirection);

//Attributes
public:
	CString csLabel;  // one or more chars assigned to label OCR 'blob'

	double fScore;
	double fX;       // location of OCR 'blob' X direction
	double fY;
};

/////////////////////////////////////////////////////////////////////////////
//
//
//

class SVOCRBlobRecordArray: public CObArray
{
public:
	SVOCRBlobRecordArray();
	~SVOCRBlobRecordArray();

	BOOL FillArray(CorStringVector *plvcsvLabels, 
	               CorFpointVector *plvcfpvLocations, 
								 CorFloatVector  *plvcfvScores);
	void ClearArray();

	int Add(SVOCRBlobRecord * pRecord);
	SVOCRBlobRecord* GetAt(int nIndex) const;
	void SetAt(int nIndex, SVOCRBlobRecord* newElement);

	BOOL SortArray(enum  enumBlobSortDirection eSortDirection = SORT_BLOGS_NOT_DEFINED);

	BOOL AnalyzeBlobs( CString& resultStringFromOCR, 
	                   float *pfHigh, float *pfLow, float *pfAvg );

// Attributes
public:
	enum enumBlobSortDirection enumSortDirection;
	long lPixelsForASpace;

};


#endif // not __SORT_BLOBS_X_AXIS_ONLY


