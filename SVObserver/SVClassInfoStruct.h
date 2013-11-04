//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVClassInfoStruct.h
//* .File Name       : $Workfile:   SVClassInfoStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:53:34  $
//******************************************************************************

#ifndef SVCLASSINFOSTRUCT_H
#define SVCLASSINFOSTRUCT_H

#include "SVContainerLibrary/SVVector.h"
#include "SVObjectLibrary/SVObjectTypeInfoStruct.h"

class SVObjectClass;

class SVInterfaceListClass : public SVVector< SVObjectTypeInfoStruct >
{
public:
	SVInterfaceListClass();
	SVInterfaceListClass( const SVInterfaceListClass& RIn );

	SVInterfaceListClass operator=( const SVInterfaceListClass& RIn );

};

struct SVClassInfoStruct
{
	SVClassInfoStruct();
	SVClassInfoStruct( const SVClassInfoStruct& RClassInfo );
	virtual ~SVClassInfoStruct();
	SVClassInfoStruct operator=( const SVClassInfoStruct& RClassInfo );

	// Construct Object and set given ClassName...
	SVObjectClass* Construct();

	SVObjectTypeInfoStruct	ObjectTypeInfo;
	CString					ClassName;
	GUID					ClassId;

	// Must be in the same order, like the class defines its input interface!!!
	SVInterfaceListClass	DesiredInputInterface;
};

class SVClassInfoStructListClass : public SVVector< SVClassInfoStruct >
{
public:
	SVClassInfoStructListClass();
	virtual ~SVClassInfoStructListClass();
	virtual int Find( const GUID& ClassID ); 
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVClassInfoStruct.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:53:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.27   08 Nov 2010 14:35:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.26   15 Dec 2009 15:13:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.25   16 Feb 2005 13:19:14   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files and removed enums
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.24   07 Sep 2004 09:47:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  446
 * SCR Title:  Add new filters to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Object Types for new Filters Watershed, Skeleton, Thinning, and Thickening
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.23   21 Jul 2004 08:59:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved SVExternalToolTaskObjectType from SVObjectSubTypeEnum to SVObjectTypeEnum
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.22   12 Jul 2004 10:15:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added types for External Tool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.21   07 Jul 2004 08:44:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  416
 * SCR Title:  Add Variant Value Object
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added SVVariantValueObjectType
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   04 Nov 2003 14:38:50   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  394
 * SCR Title:  Implement new RankingFilter for WindowTool that will also be remotely settable
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new ranking filter class that has a 1-7x1-7 customizable neighborhood.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   04 Nov 2003 14:11:38   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  378
 * SCR Title:  Implement new CustomFilter for WindowTools that will also be remotely settable
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Update the custom filter class to be 1-7x1-7 customizable kernel instead of a static 3x3 one.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   17 Sep 2003 12:45:58   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  384
 * SCR Title:  Add new Matrox OCR/OCV analyzer to SVObserver
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Created and integrated a new Matrox based OCR/OCV analyzer to SVObserver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   08 Jul 2003 10:15:56   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  378
 * SCR Title:  Implement new CustomFilter for WindowTools that will also be remotely settable
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new code and classes for the new custom filters objects and dialogs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   08 Jul 2003 08:53:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added Trigger and Digitizer enums
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   17 Apr 2003 17:08:10   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   10 Apr 2003 10:11:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  271
 * SCR Title:  Adding timer features for Tool Execution times.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added a new SVObjectTypeEnum for SVInt64ValueObjectType
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   19 Nov 2002 11:00:28   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added Enumerated types
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   08 Nov 2001 12:42:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  255
 * SCR Title:  Add Sub-pixel resolution to Profile and Line Gage Tool Analyzers
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added SVDPointValueObjectType, SVResultDPointXObjectType and SVResultDPointYObjectType for sub-pixel processing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   19 Apr 2001 20:30:52   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   06 Sep 2000 16:26:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  179
 * SCR Title:  Create Color SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to include color SVIM changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   16 May 2000 14:41:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  133, 132
 * SCR Title:  Add OCR Gray Scale to SVObserver and Create OCR Gray Scale Font Training
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Upgrade Logical Vision WiT v5.3 to Logical Vision FastOCR v1.2.  Completely changed headers, libraries, and variable name types.  Functionality still the same as prior version.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   11 May 2000 09:28:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  141
 * SCR Title:  View DDE Inputs/Outputs on Results View
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added #include for SVTemplate.h
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   14 Apr 2000 10:45:46   sMarakkath
 * Project:  SVObserver
 * Change Request (SCR) nbr:  120
 * SCR Title:  Pattern Analyzer
 * Checked in by:  sMarakkath;  Srithaj Marakkath
 * Change Description:  
 *   Added 'SVPatternAnalyzerObjectType' to SVObjectSubTypeEnum
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   23 Mar 2000 16:31:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  118
 * SCR Title:  Integration of LUT Operator
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added new vector Object Types.
 * Added some comments.
 * Added new LUT Operator type.
 * Added new LUT Equation type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   Mar 01 2000 11:43:06   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  113
 * SCR Title:  Integrate new Polar Unwrap Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *    Added new object types: 
 * SVPolarTransformObjectType, SVPolarTransformationToolObjectType,
 * SVImagePolarTransformObjectType, SVEvaluateCenterXObjectType,
 * SVEvaluateCenterYObjectType, SVEvaluateStartRadiusObjectType,
 * SVEvaluateEndRadiusObjectType, SVEvaluateStartAngleObjectType,
 * SVEvaluateEndAngleObjectType
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   Feb 24 2000 11:35:36   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  111
 * SCR Title:  Integrate Conditional Tool Set/Tool Drawing
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added new enumerated value object type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   21 Feb 2000 15:32:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  104
 * SCR Title:  New Build Reference Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added SVTransformObjectType to SVObjectTypeEnum.
 * Added SVTransformationToolObjectType,
 * SVEvaluateObjectType,				
 * SVEvaluateTranslationXObjectType,
 * SVEvaluateTranslationYObjectType,
 * SVEvaluateRotationXObjectType,
 * SVEvaluateRotationYObjectType,
 * SVEvaluateRotationAngleObjectType,
 * SVImageTransformObjectType,
 * and SVPointTransformObjectType
 * to SVObjectSubTypeEnum	
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   Feb 21 2000 10:51:34   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  109
 * SCR Title:  Freeze on Reject for v3.1
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added object types: Freeze Tool, Freeze Set, etc.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   07 Feb 2000 19:10:04   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  93
 * SCR Title:  Integrate Multiple Camera Image Input per IPD
 * Checked in by:  Robert;  Robert Massinger
 * Change Description:  
 *   Changed Type Numbering. Integrated new Types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 11:11:06   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   11 Nov 1999 17:12:12   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  58
 * SCR Title:  Implement Barcode Analyzer
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Implemented the Bar Code analyzer -- Note: the bar code analyzer requires MIL 6.0 or later.  Conditionals are included in the source code to verify the version. If a MIL library previous to 6.0 is used, bar code features are omitted without warning.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   28 Oct 1999 18:37:46   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  53
 * SCR Title:  Implement Regression Test Capability in SVObserver
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Implemented Regression Test
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   19 Oct 1999 17:41:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  48
 * SCR Title:  Threshold Page Formula
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added ThresholdEquationObjectTypes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   05 Oct 1999 10:14:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  39
 * SCR Title:  Integrate Statistics Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Add Statistics Tool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   29 Sep 1999 10:31:40   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  40
 * SCR Title:  Port Round Window Tool over from custom version 2.42.
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Added definition for  SVRoundWindowToolObjectType.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   Sep 24 1999 10:04:50   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  4
 * SCR Title:  Add "Load image" option
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added Object Type for Load Image Tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   Sep 23 1999 14:07:10   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  37
 * SCR Title:  Version 3.00 Beta 3 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Add version specific checks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   Sep 15 1999 15:53:22   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added sub type for Archive Tool Object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   Sep 14 1999 16:22:00   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added ClassIDs for Math Container, Evaluate X, Evaluate Y, EvaluateBeta, Rotate Reference.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   14 Sep 1999 16:14:48   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added object type SVResultBlobObjectType.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   Sep 13 1999 13:14:44   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added object type for Image Too, Image Arithmetic.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   Sep 10 1999 17:08:34   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added Type Profile Tool.
 * Added Type ImagetoLineProject.
 * Added Type Build Reference Tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   31 Aug 1999 10:23:50   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added SVDoubleResult
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   30 Aug 1999 18:46:44   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert;  Robert Massinger
 * Change Description:  
 *   Introduced Input Interface Handling.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   Aug 30 1999 10:08:42   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added SVResultOCRObjectType.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   29 Aug 1999 13:23:52   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert;  Robert Massinger
 * Change Description:  
 *   Added Object Types for
 * SVMaximumForegroundObjectLineAnalyzerClass
 * SVMaximumBackgroundObjectLineAnalyzerClass
 * SVMaximumObjectLineAnalyzerClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   27 Aug 1999 18:47:02   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  Steve Jones
 * Change Description:  
 *   Working on PixelAnalyzerResultClass.
 * Changing White Pixel Result references to generic Pixel Result references.
 * 
 * Changed 
 * SVWhitePixelAnalyzerResultObjectType to 
 * SVPixelAnalyzerResultObjectType 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   Aug 27 1999 17:17:48   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Add object type for OCR Analyzer.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   27 Aug 1999 13:23:20   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert Massinger
 * Change Description:  
 *   Added ObjectTypes for Point X Result, Point Y Result and Long Result.
 * Added Construct() to Class Info Struct.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   26 Aug 1999 14:22:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  Steve Jones
 * Change Description:  
 *   Added Guids and ObjectTypes/SubTypes for the SVConditionalClass and SVMathEquationClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   24 Aug 1999 18:32:12   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  Steve Jones
 * Change Description:  
 *   Replaced SVWhitePixelAnalyzerObjectType with SVPixelAnalyzerObjectType.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   24 Aug 1999 18:29:12   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert Massinger
 * Change Description:  
 *   Added necessary Object Types for Gage Tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   Aug 16 1999 15:40:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  Steve Jones
 * Change Description:  
 *   Revised to SVMainImageObjectType to be a SVObjectSubTypeEnum
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
