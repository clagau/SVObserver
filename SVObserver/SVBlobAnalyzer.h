//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBlobAnalyzer
//* .File Name       : $Workfile:   SVBlobAnalyzer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:43:26  $
//******************************************************************************

#ifndef SVBLOBANALYZERCLASS__INCLUDED
#define SVBLOBANALYZERCLASS__INCLUDED

#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVAnalyzer.h"
#include "SVValueObjectImpl.h"
#include "SVBlobAnalyzerResultDlg.h"

class SVLongResultClass;

enum { SV_MAX_NUMBER_OF_BLOBS = 100 };

enum SVBlobFeatureEnum
{
	SV_AREA = 0,            // 0
	SV_BOXX_MAX,            // 1
	SV_BOXX_MIN,            // 2
	SV_BOXY_MAX,            // 3
	SV_BOXY_MIN,            // 4
	SV_BREADTH,             // 5
	SV_CENTEROFGRAVITY_X,   // 6
	SV_CENTEROFGRAVITY_Y,   // 7
	SV_CONVEX_PERIMETER,    // 8
	SV_FERET_ELONGATION,    // 9
	SV_FERETMAX_ANGLE,      // 10
	SV_FERETMAX_DIAMETER,   // 11
	SV_FERETMEAN_DIAMETER,  // 12
	SV_FERETMIN_ANGLE,      // 13
	SV_FERETMIN_DIAMETER,   // 14
	SV_FERET_X,             // 15
	SV_FERET_Y,             // 16
	SV_FIRSTPOINT_X,        // 17
	SV_FIRSTPOINT_Y,        // 18
	SV_LABEL,               // 19
	SV_LENGTH,              // 20
	SV_NBROF_HOLES,         // 21
	SV_PERIMETER,           // 22
	SV_ROUGHNESS,           // 23
//- SV_SUM_PIXEL - This setting does not work on binary images, and is
//- therefore being removed from available features at this time.  The 
//- selection must still be available in the list in order to support
//- previous configurations, but will not appear in the list of available
//- features.  This is being enforced within 
//- SVBlobAnalyzeFeatureListBoxClass::Init ().
	SV_SUM_PIXEL,           // 24  // Will not work with thresholded images i.e.
	                               // binary images vs. grey scale images.
	//
	// v3.1 Added Blob Features
	//
// The "chain" values do not seem to want to be excluded from the list based
// on their pass/fail criteria. And since I don't know what they do, I am 
// removing them. jab

//	SV_NBRCHAINED_PIXELS,   // 26
//	SV_CHAIN_INDEX,         // 27
//	SV_CHAIN_X,             // 28
//	SV_CHAIN_Y,             // 29 
	SV_COMPACTNESS,         // 25
	SV_NBR_RUNS,            // 26
	SV_XMINAT_YMIN,         // 27
	SV_XMAXAT_YMAX,         // 28
	SV_YMINAT_XMAX,         // 29
	SV_YMAXAT_XMIN,         // 30
	SV_ELONGATION,          // 31
	SV_INTERCEPT_0,         // 32
	SV_INTERCEPT_45,        // 33
	SV_INTERCEPT_90,        // 34
	SV_INTERCEPT_135,       // 35
	SV_MOMENT_X0Y1,         // 36
	SV_MOMENT_X1Y0,         // 37
	SV_MOMENT_X1Y1,         // 38
	SV_MOMENT_X0Y2,         // 39
	SV_MOMENT_X2Y0,         // 40
	SV_CENTRAL_X0Y2,        // 41
	SV_CENTRAL_X2Y0,        // 42
	SV_CENTRAL_X1Y1,        // 43
	SV_AXISPRINCIPAL_ANGLE, // 44
	SV_AXISSECONDARY_ANGLE, // 45
	SV_EULER_NBR,           // 46
	SV_CENTER_X_SOURCE,     // 47
	SV_CENTER_Y_SOURCE,		// 48

	SV_TOPOF_LIST
};

enum { SV_NUMBER_OF_BLOB_FEATURES  = SV_TOPOF_LIST };

/////////////////////////////////////////////////////////////////////////////
//
// Define a record for the constant attributes of a SVBlobFeature record.
//
struct SVBlobFeatureConstants
{
	SVBlobSelectionEnum  MILFeatureDef;
	GUID*   pEmbeddedID;
	int     NewStringResourceID;
};

enum SV_BLOB_FILL_COLOR_ENUM
{
	SV_BLOB_FILL_BLACK    = 0x0000,	// 0000 0000 0000 0000
	SV_BLOB_FILL_WHITE    = 0x00FF	// 0000 0000 1111 1111
};// end SV_BLOB_FILL_COLOR_ENUM

// Add String for SVEnumerateValueObjectClass
const LPCSTR g_strBlobFillColorEnums = 
                _T( "Black=0,White=255" );

enum SV_BLOB_FILL_TYPE_ENUM
{
	SV_BLOB_FILL_ALL       = 0x8000,
	SV_BLOB_FILL_INCLUDED  = 0x2000,
	SV_BLOB_FILL_EXCLUDED  = 0x1000
};

// Add String for SVEnumerateValueObjectClass
const LPCSTR g_strBlobFillTypeEnums = 
                _T( "All=32768,Included=8192,Excluded=4096" );

/////////////////////////////////////////////////////////////////////////////
//
//
class SVBlobAnalyzerClass : public SVImageAnalyzerClass
{
	SV_DECLARE_CLASS( SVBlobAnalyzerClass );

	friend class SVSetupDialogManager;

public:
	SVBlobAnalyzerClass(SVObjectClass* POwner = NULL , 
		                int StringResourceID = IDS_CLASSNAME_SVBLOBANALYZER);

	virtual ~SVBlobAnalyzerClass();
	virtual BOOL CloseObject();

	virtual BOOL CreateObject(SVObjectLevelCreateStruct* PCreateStructure);
	HRESULT ResetObject();

	DWORD BuildFeatureListID ();
	DWORD EnableFeature (SVBlobFeatureEnum aIndex);
	DWORD DisableFeature (SVBlobFeatureEnum aIndex);
	DWORD AllocateBlobResult ();

/*- SortBlobs () --------------------------------------------------------------*/
/*- This will create a sorted mapping of the items in the msvDblValueArray.    */
/*- The sort will be by the feature indicated in asvlSortFeature and the map   */
/*- will be returned as asvlSortMap. No array data will be moved. If we choose */
/*- to permit sorting on multiple features, it will just mean tracking         */
/*- multiple sort maps. -------------------------------------------------------*/
	DWORD SortBlobs (long asvlSortFeature, long* alSortMap, long p_lArraySize );

	DWORD AllocateResult (SVBlobFeatureEnum aFeatureIndex);
	DWORD FreeResult (SVBlobFeatureEnum aFeatureIndex);
	void RebuildResultObjectArray();

/*- GetResultObject () --------------------------------------------------------*/
/*- Each enabled feature is associated with its own result. This function      */
/*- will hand back the associated with the feature which is referenced by      */ 
/*- aFeatureIndex. ------------------------------------------------------------*/
	SVResultClass* GetResultObject(SVBlobFeatureEnum aFeatureIndex);
	SVLongResultClass* GetBlobResultObject();

	virtual BOOL onRun( SVRunStatusClass& RRunStatus );
	virtual BOOL OnValidate();

	virtual DWORD processMessage( DWORD DwMessageID, DWORD DwMessageValue, DWORD DwMessageContext );

	virtual BOOL IsPtOverResult(CPoint point );
	virtual	void DisplayAnalyzerResult( void);

	SVLongResultClass*           m_pResultBlob;

/*- ENABLED FEATURES ----------------------------------------------------------*/
/*- Since the CString member of the SVStringValueObject can not be directly    */
/*- accessed, the msvszFeaturesEnabled string acts at a workspace for          */
/*- manipulating the enabled features configuration.                           */
/*- msvPersistantFeaturesEnabled is required to preserve the values during     */
/*- scripting. ----------------------------------------------------------------*/
	SVStringValueObjectClass     msvPersistantFeaturesEnabled;
	TCHAR                        msvszFeaturesEnabled [SV_NUMBER_OF_BLOB_FEATURES + 1]; // Null Terminated
/*- End of ENABLED FEATURES. --------------------------------------------------*/

	SVDoubleValueObjectClass     msvValue [SV_NUMBER_OF_BLOB_FEATURES];
	GUID                         m_guidResults[ SV_NUMBER_OF_BLOB_FEATURES ];

/*- msvPersistantNbrOfBlobs ---------------------------------------------------*/
/*- This value is for persistant storage only.  The "working" counterpart is   */
/*- msvlNbrOfBlobs. -----------------------------------------------------------*/
	SVLongValueObjectClass       m_lvoBlobSampleSize;//msvPersistantNbrOfBlobs;
	SVLongValueObjectClass       m_lvoMaxBlobDataArraySize;

	long                         m_lBlobSampleSize;//msvlNbrOfBlobs;
	long                         m_lMaxBlobDataArraySize;

/* msvSortFeature -------------------------------------------------------------*/
/* MIL value from the msvlMILFeatureDef array. This value determins which      */
/* feature should be sorted on. -----------------------------------------------*/
	SVLongValueObjectClass       msvSortFeature;
	SVBoolValueObjectClass       msvSortAscending;
// Exclude the Failed blobs from the result list.
	SVBoolValueObjectClass       msvbExcludeFailed;

	SVLongValueObjectClass       m_lvoNumberOfBlobsFound;
	long                         m_lNumberOfBlobsFound;
	long                         m_lNumberOfBlobsToProcess;
	int                          m_nBlobIndex;

	vector2d<double>             m_vec2dBlobResults;

/*- msvlSortMap ---------------------------------------------------------------*/
/*- Set by SortBlobs () during each onRun (), this value keeps the mapping of  */
/*- the blob sort order as determined by SortBlobs ().  All features can then  */
/*- be extracted based on the sort without needing to move large quantities of */
/*- data. ---------------------------------------------------------------------*/
	SVVector< long > msvlSortMap;

	DWORD                        msvlDefaultAttributes; 

	SVMatroxBlobFeatureList              msvFeatureListID;
	SVMatroxBlobResult                   msvResultBufferID;

	//new value objects for Fill Blob option
	SVBoolValueObjectClass       m_bvoFillBlobs;
	SVEnumerateValueObjectClass  m_evoBlobColor;
	SVEnumerateValueObjectClass  m_evoBlobType;

protected:
	virtual void init();
	virtual HRESULT onCollectOverlays(SVImageClass* p_pImage, SVExtentMultiLineStructCArray& p_rMultiLineArray );

/*- MapQuickSort () ----------------------------------------------------------*/
/*- This should not be called directly, but through SortBlobs () -------------*/
#ifndef _DEBUG
	DWORD MapQuickSort (double*    aSortArray, 
	                    long*      alSortMap,
	                    long       alBeginning,
	                    long       alEnd,
	                    BOOL       abAscending);
#else
	DWORD MapQuickSort (double*    aSortArray, 
	                    long*      alSortMap,
	                    long       alBeginning,
	                    long       alEnd,
	                    BOOL       abAscending,
	                    std::vector<double>& SVA);
#endif

	virtual void addDefaultInputObjects( BOOL BCallBaseClass = FALSE, SVInputInfoListClass* PInputListToFill = NULL );

private:
	void CreateArray();
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVBlobAnalyzer.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:43:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.27   04 Oct 2012 11:06:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Remove scope declaration on header methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.26   01 Aug 2012 12:49:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed all overlay collection functionality that uses data at a particular index.  The current overlay collection functionality collects the data in the inspection thread, so that all data is consistant based on the last executed inspection.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.25   18 Jul 2012 13:23:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.24   02 Jul 2012 16:36:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.23   10 Feb 2011 14:30:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.22   08 Nov 2010 14:35:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.21   01 Jun 2010 14:13:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   20 Oct 2008 11:07:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  635
 * SCR Title:  Blob Analyzer Bug Fixes
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Moved SVBlobAnalyzerResultDlg to it's own module(s).
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   24 Jul 2007 11:26:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade to Matrox Imaging Library 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   11 Nov 2005 10:14:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  526
 * SCR Title:  Fix performance problems with Blob Analyzer
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed the GetResultObject to use cached GUIDs instead of using the GETFIRST / GETNEXT messages
 * added RebuildResultObjectArray
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   13 Oct 2005 13:58:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed index 1 from GetValue calls.  Added processMessage method to call the ResetObject functionality.  Updated some value objects to force an owner reset.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   08 Aug 2005 15:30:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   19 Jul 2005 14:39:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  469
 * SCR Title:  Add array capability to the Blob Analyzer
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented support for Max Blob Data Array Size
 * renamed member variables to be more understandable
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   22 Jun 2005 11:36:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  469
 * SCR Title:  Add array capability to the Blob Analyzer
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented array blobs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   20 May 2005 07:36:14   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  487
 * SCR Title:  Add new blob feature for source extents of blob box center
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new blob features to get the center of the bounding box as releated to the Source Image Extents
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   17 Feb 2005 13:18:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   04 Dec 2003 12:58:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  397
 * SCR Title:  Blob Analyzer - Allow Max number of blobs to be set.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added code for Max Number of Blobs.  The value is now setable
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   29 Oct 2003 14:06:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  392
 * SCR Title:  Blob Analyzer have option for filling in the blobs
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   new value objects for FillBlobs, BlobColor and BlobType
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   17 Apr 2003 16:53:58   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   04 Apr 2002 08:56:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  286
 * SCR Title:  Upgrade SVObserver to Version 3.37 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update SVBlobAnalyzerClass to fix method definition to properly reverse engineer into rational rose.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   31 Jul 2001 12:48:46   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  219
 * SCR Title:  Fix Dr. Watson error with Blob Analyzer when the SVIM goes on-line.
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified code in "onRun" function so that limiting selected blobs
 * to 100 functions as required.   Also added debug code to the 
 * functions "SortBlobs" and "MapQuickSort".  The only difference
 * between this version of this file and the previous version is the 
 * file header info.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   31 Jul 2001 11:35:38   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  219
 * SCR Title:  Fix Dr. Watson error with Blob Analyzer when the SVIM goes on-line.
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   Apr 17 2001 15:16:08   steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen S. Steffan
 * Change Description:  
 *   Header file chages due to restructuring of include files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   19 Apr 2000 18:11:44   Jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  128
 * SCR Title:  Suport for 3.11 versioning
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   SVBlobFeatureEnum was added.
 * SVBlobFeature class was removed.
 * 
 * SVBlobAnalyzerClass was modified to support the use of SVBlobFeatureEnum.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   14 Apr 2000 10:04:00   sMarakkath
 * Project:  SVObserver
 * Change Request (SCR) nbr:  123, 122
 * SCR Title:  Add Grahpical Interface to show all blobs found
 * Checked in by:  sMarakkath;  Srithaj Marakkath
 * Change Description:  
 *   Added a result class object for the Number of Blobs found .
 * Added the functions 'AllocateBlobResult' and 'GetBlobResultObject'.
 * Added functions 'onUpdateFigure', 'IsPtOverResult' and 'DisplayAnalyzerResult'.
 * Added a new Dialog class 'SVBlobAnalyzerResultDlg'.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   03 Mar 2000 12:09:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  94
 * SCR Title:  New Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised declaration of init() method to be private and not virtual.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   Feb 16 2000 16:17:36   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  107
 * SCR Title:  Add More Blob Analyzer Features
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changed code to add 26 more Blob features that SVObserver can calculate using the MIL 6.0 library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 11:05:42   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   Nov 16 1999 11:07:40   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Removed M_SUM_PIXEL blob feature which caused a 'crash' on binarized images.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   05 Oct 1999 08:51:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added Embedded object for sort direction.
 * Corrected Sort problem
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   04 Oct 1999 15:06:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Corrected improperly dimensioned sortMap variable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   29 Sep 1999 10:28:10   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Finishing up Blob Analyzer.  Bringing sort feature out to the operator.  
 * 
 * Still buggy.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   24 Sep 1999 17:25:36   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   The Blob analyzer is integrated and semi functional.  Still needs debugging.  
 * 
 * Sorting is included.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Sep 1999 22:04:28   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Massive overhaul for new storage(scripting) techniques.  Should be pretty much done.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Sep 10 1999 13:57:12   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Files extracted from v2.42 code and changed to v3.00 architecture.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
