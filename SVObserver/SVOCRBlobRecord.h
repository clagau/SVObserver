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

#ifndef _SV_OCR_BLOB_RECORD_
#define _SV_OCR_BLOB_RECORD_

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

#endif   /* _SV_OCR_BLOB_RECORD_ */

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOCRBlobRecord.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:54:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   22 Apr 2003 12:42:10   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   17 Jul 2001 20:48:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  214
 * SCR Title:  Fix OCR analyzer Dr. Watson problem when calculating match score information.
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changes parameters for the AnalyzeBlobs method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   17 Jul 2001 20:48:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  214
 * SCR Title:  Fix OCR analyzer Dr. Watson problem when calculating match score information.
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changes parameters for the AnalyzeBlobs method.
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
 *    Rev 1.2   18 Jul 2000 15:48:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  169
 * SCR Title:  Add Match Score Information to OCR Tools
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   OCR Match Score Updates
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Jun 2000 09:41:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  162
 * SCR Title:  Upgrade Coreco Sapera from Version 2.10 to Version 3.00
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed all instances of obsolete WiT data types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 May 2000 14:44:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  133, 132
 * SCR Title:  Add OCR Gray Scale to SVObserver and Create OCR Gray Scale Font Training
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Upgrade Logical Vision WiT v5.3 to Logical Vision FastOCR v1.2.  Completely changed headers, libraries, and variable name types.  Functionality still the same as prior version.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:23:14   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   Nov 09 1999 09:44:52   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Add message response to 'GOING_ON_LINE' so match string file can be opened and a possible new match string can be retreived.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   Oct 20 1999 07:24:40   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Character blob x and y axis sort.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   Oct 18 1999 16:50:12   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   First changes for multi-axis character blob sorting.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   Sep 07 1999 08:51:04   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added OnValidate() and onRun() methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   02 Sep 1999 09:56:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Run logic
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   Aug 31 1999 19:21:48   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   OCR converted to 3.0 architecture.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   Aug 28 1999 10:18:00   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Add OCR Analyzer and Result.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/