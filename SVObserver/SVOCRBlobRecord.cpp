//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCRBlobRecord
//* .File Name       : $Workfile:   SVOCRBlobRecord.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Oct 2013 06:48:20  $
//******************************************************************************

#include "stdafx.h"
#include "SVOCRBlobRecord.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef __SORT_BLOBS_X_AXIS_ONLY
/////////////////////////////////////////////////////////////////////////////
//
//
//
SVOCRBlobRecord::SVOCRBlobRecord()
{
	csLabel.Empty();

	fScore = 0.0;
	fX = 0.0;
	fY = 0.0;
}

SVOCRBlobRecord::SVOCRBlobRecord(const SVOCRBlobRecord& svOCRBR)
{
	csLabel = svOCRBR.csLabel;

	fScore = svOCRBR.fScore;
	fX = svOCRBR.fX;
	fY = svOCRBR.fY;
}

SVOCRBlobRecord::~SVOCRBlobRecord()
{
}

const SVOCRBlobRecord& SVOCRBlobRecord::operator =(const SVOCRBlobRecord& svOCRBR)
{
	csLabel = svOCRBR.csLabel;

	fScore = svOCRBR.fScore;
	fX = svOCRBR.fX;
	fY = svOCRBR.fY;

	return *this;
}

BOOL SVOCRBlobRecord::CheckAgainst(const SVOCRBlobRecord& svOCRBR, enum enumBlobSortDirection eSortDirection)
{
	return CheckAgainst(&svOCRBR, eSortDirection);
}

BOOL SVOCRBlobRecord::CheckAgainst(const SVOCRBlobRecord *svOCRBR, enum enumBlobSortDirection eSortDirection)
{
	BOOL bOk = FALSE;

	switch (eSortDirection)
	{
		case SORT_BLOBS_RIGHT:
		{
			bOk = (svOCRBR->fX < fX);
			break;
		}
		case SORT_BLOBS_LEFT:
		{
			bOk = (fX < svOCRBR->fX);
			break;
		}
		case SORT_BLOBS_UP:     // Bottom to top
		{
			bOk = (fY < svOCRBR->fY);
			break;
		}
		case SORT_BLOBS_DOWN:   // Top to Bottom
		{
			bOk = (svOCRBR->fY < fY);
			break;
		}
		case SORT_BLOBS_SCORE:
		{
			bOk = (fScore < svOCRBR->fScore);
			break;
		}
		default:
		{
			break;
		}
	}

	return bOk;
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
SVOCRBlobRecordArray::SVOCRBlobRecordArray()
{
	enumSortDirection = SORT_BLOBS_RIGHT;
	lPixelsForASpace = -1;
}

SVOCRBlobRecordArray::~SVOCRBlobRecordArray()
{
	ClearArray();
}

BOOL SVOCRBlobRecordArray::FillArray(CorStringVector	*plvcsvLabels, 
                                     CorFpointVector	*plvcfpvLocations, 
							                       CorFloatVector		*plvcfvScores)
{
	BOOL bOk = plvcsvLabels != NULL && plvcfpvLocations != NULL && plvcfvScores != NULL;

	if ( bOk )
	{
		int nSize = CorObj_size(plvcsvLabels);
		CorString * pStrings = CorObj_mdData(plvcsvLabels);

		int nSize1 = CorObj_size(plvcfpvLocations);
		CorFpoint * pPts = CorObj_mdData(plvcfpvLocations);

		int nSize2 = CorObj_size(plvcfvScores);
		float * pFloat = CorObj_mdData(plvcfvScores);

		bOk = nSize <= nSize1 && nSize <= nSize2;
	
		if ( bOk )
		{
			for ( int i = 0; i < nSize; i++ )
			{
				SVOCRBlobRecord * pBlob = new SVOCRBlobRecord;

				pBlob->csLabel = pStrings[i];
				pBlob->fScore = pFloat[i];		
				pBlob->fX = pPts[i].x;
				pBlob->fY = pPts[i].y;

				Add(pBlob);
			}
		}
	}

	return bOk;
}

void SVOCRBlobRecordArray::ClearArray()
{
	while ( 0 < GetSize() )
	{
		SVOCRBlobRecord *pRecord = GetAt( 0 );
		if ( pRecord )
		{
			delete pRecord;
		}

		RemoveAt( 0 );
	}
}

int SVOCRBlobRecordArray::Add(SVOCRBlobRecord * pRecord)
{
	return static_cast<int>(CObArray::Add((CObject *)pRecord));
}

SVOCRBlobRecord* SVOCRBlobRecordArray::GetAt(int nIndex) const
{
	return (SVOCRBlobRecord *)CObArray::GetAt(nIndex);
}

void SVOCRBlobRecordArray::SetAt(int nIndex, SVOCRBlobRecord* newElement)
{
	CObArray::SetAt(nIndex,(CObject *)newElement);
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
BOOL SVOCRBlobRecordArray::SortArray(enum enumBlobSortDirection eSortDirection)
{
	BOOL bOk = TRUE;

	enum enumBlobSortDirection eDirection = eSortDirection;

	if (eDirection == SORT_BLOGS_NOT_DEFINED)
	{
		eDirection = enumSortDirection;
	}

	//
	// We now have the blob locations and the ocr returned results combined.
	// Do a simple 'bubble' sort on the required axis and direction.
	//
	BOOL bDone = FALSE;

	int nCount = static_cast<int>(GetSize());
	while ( !bDone )
	{
		bDone = TRUE;

		for( int i = 0; i < nCount-1; i++ )
		{
			SVOCRBlobRecord * pBlob1 = GetAt(i);
			SVOCRBlobRecord * pBlob2 = GetAt(i+1);
			//
			// Interchange if necessary.
			//
			if ( pBlob1->CheckAgainst(pBlob2,eDirection) )
			{
				SetAt(i,  pBlob2);
				SetAt(i+1,pBlob1);
				bDone = FALSE;
			}
		}
	}

	return bOk;
}

BOOL SVOCRBlobRecordArray::AnalyzeBlobs( CString& resultStringFromOCR, 
                                         float *pfHigh, float *pfLow, float *pfAvg)
{
	BOOL bOk = TRUE;

	//
	// Put a string together after the sort.
	//
	int nCount = static_cast<int>(GetSize());
	float fSum = 0.0;

	SortArray();

	*pfHigh = -1.0F;
	*pfLow  = 1.1F;
	*pfAvg  = 0.0F;

	resultStringFromOCR.Empty();

	//
	// Get the parameter for a ' ' between characters.  
	// -1 is 'not used' condition.
	//
	for ( int i = 0; i < nCount; i++ )
	{
		//
		// First add the character.
		//
		SVOCRBlobRecord * pBlob1 = GetAt(i);
		resultStringFromOCR += pBlob1->csLabel;


		fSum += (float)(pBlob1->fScore);

		if ( *pfHigh < pBlob1->fScore )
		{
			*pfHigh = (float)(pBlob1->fScore);
		}

		if ( pBlob1->fScore < *pfLow)
		{
			*pfLow = (float)(pBlob1->fScore);
		}

		//
		// Check to see if a 'space' char is assumed based on distance between
		// the centroids of the character 'blobs'.
		// Wit axis X positive left to right.
		// Wit axis Y positive top to bottom.
		//
		if((i < nCount-1) &&
		   (lPixelsForASpace > -1L))
		{
			long lPixelsBetweenCharacters;
			SVOCRBlobRecord * pBlob2 = GetAt(i+1);
			switch(enumSortDirection)
			{
				case SORT_BLOBS_RIGHT:
				{
					lPixelsBetweenCharacters = (long)(pBlob2->fX - pBlob1->fX);
					break;
				}
				case SORT_BLOBS_LEFT:    // Right to left
				{
					lPixelsBetweenCharacters = (long)(pBlob1->fX - pBlob2->fX);
					break;
				}
				case SORT_BLOBS_UP:     // Bottom to Top
				{
					lPixelsBetweenCharacters = (long)(pBlob1->fY - pBlob2->fY);
					break;
				}
				case SORT_BLOBS_DOWN:   // Top to bottom
				{
					lPixelsBetweenCharacters = (long)(pBlob2->fY - pBlob1->fY);
					break;
				}
				default:;
					lPixelsBetweenCharacters = -1L;
			}

			if ( lPixelsBetweenCharacters > lPixelsForASpace )
			{
				resultStringFromOCR += (TCHAR)' ';
			}
		}
	}

	if ( 0 < nCount )
	{
		*pfAvg = fSum / (float) nCount;
	}

	return bOk;
}
#endif // not __SORT_BLOBS_X_AXIS_ONLY

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVOCRBlobRecord.cpp_v  $
 * 
 *    Rev 1.1   02 Oct 2013 06:48:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 12:54:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   22 Apr 2003 12:42:08   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   20 Nov 2002 10:05:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed check from == to <=
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   04 Oct 2001 16:46:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  229
 * SCR Title:  Upgrade SVObserver to Version 3.34 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modified source code to remove compiler warning message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   17 Jul 2001 20:54:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  214
 * SCR Title:  Fix OCR analyzer Dr. Watson problem when calculating match score information.
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated FillArray method to verify accurate addition of character information to blob record array.
 * 
 * Updated ClearArray method to remove and destroy elements from the beginning of the array in a single pass.
 * 
 * Updated AnalyzeBlobs method to calculate the high, low and average match score information as well as build the match string.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   17 Jul 2001 20:54:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  214
 * SCR Title:  Fix OCR analyzer Dr. Watson problem when calculating match score information.
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated FillArray method to verify accurate addition of character information to blob record array.
 * 
 * Updated ClearArray method to remove and destroy elements from the beginning of the array in a single pass.
 * 
 * Updated AnalyzeBlobs method to calculate the high, low and average match score information as well as build the match string.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   28 Aug 2000 16:05:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  177
 * SCR Title:  Fix Automatic Load Last SEC Problem
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update FastOCR to prevent memory access violations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   18 Jul 2000 15:48:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  169
 * SCR Title:  Add Match Score Information to OCR Tools
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   OCR Match Score Updates
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Jun 2000 09:41:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  162
 * SCR Title:  Upgrade Coreco Sapera from Version 2.10 to Version 3.00
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed all instances of obsolete WiT data types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 May 2000 14:43:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  133, 132
 * SCR Title:  Add OCR Gray Scale to SVObserver and Create OCR Gray Scale Font Training
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Upgrade Logical Vision WiT v5.3 to Logical Vision FastOCR v1.2.  Completely changed headers, libraries, and variable name types.  Functionality still the same as prior version.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/