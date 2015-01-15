//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVResult.h
//* .File Name       : $Workfile:   SVResult.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   14 Jan 2015 16:43:46  $
//******************************************************************************

#pragma once

#include "SVContainerLibrary/SVVector.h"
#include "ISVCancel.h"
#include "SVTaskObjectInterfaceClass.h"
#include "SVTaskObjectList.h"

class SVRangeClass;
class SVIPDoc;

enum
{
	SV_DEFAULT_MAX_BLOB_NUMBER			= 100,
	SV_DEFAULT_TEXT_HEIGHT				= 20,
	SV_DEFAULT_NUMBER_OF_INTENSITIES	= 256,
	SV_NUMBER_OF_MATCH_STRINGS			= 1,
};

////////////////////////////////////////////////////////////////////////////////
// .Title       : Base Class of module SVResult 
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :27.05.1997 RO			First Implementation
//	:06.06.1998 RO			Supplements for sub results -> change deriving from
//							CObject to CArray
//	:11.06.1998 RO			Add Calculate( SVDataObjectClass* ) member function
//	:19.01.1999 RO			Supplements for 3.0, Change Request ...
//							Is now derived from SVTaskObjectClass instead of
//							SVObjectClass 
////////////////////////////////////////////////////////////////////////////////
class SVResultClass : public SVTaskObjectListClass, public ISVCancel
{
	SV_DECLARE_CLASS( SVResultClass );

public:
	SVResultClass ( 
		BOOL BCreateDefaultTaskList = FALSE, 
		SVObjectClass* POwner = NULL, 
		int StringResourceID = IDS_CLASSNAME_SVRESULT 
	);

	virtual ~SVResultClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );

	// ISVCancel interface
	virtual bool CanCancel();
	virtual HRESULT GetCancelData(SVCancelData*& rpData);
	virtual HRESULT SetCancelData(SVCancelData* pData);

	BOOL IsFailed();
	BOOL IsWarned();
	BOOL IsGood();

	// derived class overrides...

	virtual SVRangeClass* GetResultRange();
	virtual CRect Draw( HDC DC, CRect R );

	virtual BOOL Run(SVRunStatusClass& RRunStatus);

protected:
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

	void init();

protected:
	// Passed, if TRUE ( Reset Value: FALSE )
	SVBoolValueObjectClass	passed;
	// Warned, if TRUE ( Reset Value: TRUE )
	SVBoolValueObjectClass	warned;
	// Failed, if TRUE ( Reset Value: TRUE )
	SVBoolValueObjectClass	failed;
	
};

////////////////////////////////////////////////////////////////////////////////
// .Title       : Class SVAnalyzeFeatureClass
// -----------------------------------------------------------------------------
// .Description : e.g. This base class capsules an Image and provides its ...
//              :
//              :
// -----------------------------------------------------------------------------
// .Export
//	 Public Method				Description
//  :
//  :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :27.05.1997 RO			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
// SEJ (OLD) class SVAnalyzeFeatureClass : public CObject, public SVObjectClass
class SVAnalyzeFeatureClass : public SVTaskObjectClass
{
	SV_DECLARE_CLASS( SVAnalyzeFeatureClass );

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
// Standard constructor
	SVAnalyzeFeatureClass( LPCSTR ObjectName = "Unknown Analyze Feature" );
// Standard destructor
	virtual ~SVAnalyzeFeatureClass();

//******************************************************************************
// Operator(s):
//******************************************************************************
public:
////////////////////////////////////////////////////////////////////////////////
// Create Operator
////////////////////////////////////////////////////////////////////////////////
	BOOL Create( LPCSTR ObjectName, long F );

	void create( int MaxNumber = SV_DEFAULT_MAX_BLOB_NUMBER, SVRangeClass* R = NULL );

////////////////////////////////////////////////////////////////////////////////
// is... Operator
////////////////////////////////////////////////////////////////////////////////
	BOOL IsFailed();
	BOOL IsWarned();

//******************************************************************************
// Operation(s) Of Writing Access:
//******************************************************************************
public:
	void setFeatureResultRange( SVRangeClass* R );

//******************************************************************************
// Operation(s) Of Reading Access:
//******************************************************************************
public:
	SVRangeClass*	getFeatureResultRange();
	double*			getResultArray();
	long			getFeature();
	TCHAR*			GetResultArrayString( int N );

//******************************************************************************
// Data Element(s):
//******************************************************************************
protected:
	long			feature;
	double*			resultArray;
	SVRangeClass*	featureResultRange;
	int				resultArraySize;
	BOOL			failed;
	BOOL			warned;

	char*			resultString;
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVResult.h_v  $
 * 
 *    Rev 1.3   14 Jan 2015 16:43:46   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  979
 * SCR Title:  Provide additional options to input the feature range for the blob analyzer.
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Removed unused classes SVAnalyzeFeatureListClass and SVRangeDialogClass (replaced by RangeXDialogClass).
 *   Changed to use pragma once.
 *   Removed unnecessary items.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 May 2014 12:40:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed processMessage signature to use DWORD_PTR.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2014 12:03:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 14:55:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   04 Sep 2012 15:56:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed dead code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   18 Jul 2012 14:25:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   02 Jul 2012 17:33:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   09 Nov 2010 16:30:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   05 Nov 2010 14:58:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove the un-necessary IP View Control from the application.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   01 Jun 2010 11:03:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   09 Aug 2005 08:09:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   21 Jun 2005 08:29:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes now use accessor methods
 * value object functions now use HRESULT
 * cleaned up files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   17 Feb 2005 15:25:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   12 Jul 2004 11:58:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   made SVResultClass implement ISVCancel
 * cleaned up code
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   22 Apr 2003 14:54:58   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   20 Nov 2002 11:12:56   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed references to System
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   04 Apr 2002 10:22:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  286
 * SCR Title:  Upgrade SVObserver to Version 3.37 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code syntax to allow rational rose to reverse engineer properly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   Feb 03 2000 15:26:34   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  100
 * SCR Title:  Negative Range Values Not Allowed
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Change range checking of Fail High, Warn High, Warn Low, and Fail Low to include negative numbers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:31:28   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   Nov 10 1999 12:53:54   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  27
 * SCR Title:  Results Picker
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Add 'wait' cursor for result picker and remove 'dead' code for SVAvailableFeatureListClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   Oct 27 1999 15:14:12   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Fix bug in Build Reference which resulted in a Tool failure as a result of a bug in the translation of 'child' images.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   27 Sep 1999 16:23:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Moved SVRangeClass to SVRange.cpp and SVRange.h
 * Revised Constructor initialization not to create SVRangeClass.
 * Revised onRun Method.
 * Added Run Method to properly handle updating the
 * pas/fail/warn status in the result class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   Sep 23 1999 14:04:44   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  37
 * SCR Title:  Version 3.00 Beta 3 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Removed obsolete Serialize() code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   Sep 23 1999 10:15:50   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  35
 * SCR Title:  PPQ mode to wait for input data
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Removed Obsolete classes and functions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   Sep 10 1999 14:25:54   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Removed old SVBlobAnalyzerClass and SVBlobAnalyzerDialog code into separate files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   03 Sep 1999 14:33:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to make GetIPDocument public
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   02 Sep 1999 08:39:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised due to no requirement for sub results.
 * Revised Run Logic.
 * Removed code for resultColor 
 * (color now lives in SVObjectClass)
 * Revised to call immediate base class CreateObject.
 * Removed override of SetObjectDepth. 
 * (handled in SVObjectClass)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   30 Aug 1999 19:49:40   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert;  Robert Massinger
 * Change Description:  
 *   Introduced Input Interface Handling.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   30 Aug 1999 18:57:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Run method to use SVRunStatusClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   27 Aug 1999 18:59:18   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  Steve Jones
 * Change Description:  
 *   Working on PixelAnalyzerResultClass.
 * 
 * Changing White Pixel Result references to generic Pixel Result references.
 * 
 * 
 * 
 * Split SVWhitePixelAnalyzerResultClass off into a separate file, and renamed it to reflect a more generic functionallity.
 * 
 * SVPixelResultAnalyzer.cpp and SVPixelResultAnalyzer.h
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   27 Aug 1999 12:59:54   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert Massinger
 * Change Description:  
 *   Added Range Dialog as Setup Dialog to SVResult Base Class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   Aug 25 1999 22:50:20   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  1
 * SCR Title:  Integrate a common validity check for all tool base class inputs and outputs.
 * Checked in by:  Steve Jones
 * Change Description:  
 *   Revised Validate method to be OnValidate method.
 *  ( OnValidate - local scope validation).
 * Added Validate method (Routes to all owned objects)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   Aug 23 1999 18:23:50   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  Steve Jones
 * Change Description:  
 *   Split out SVResultListClass for better modularity.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
