//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// enums and struct for object types
//******************************************************************************

#pragma once

#include "SVUtilityLibrary/SVGUID.h"

//TODO (MZA:2015/01/09): Add namespace to fit to the code rules. 
//								Not edit yet to avoid to change to much code in this prototype.
//namespace Seidenader
//{
//	namespace ObjectInterfaces
//	{
		enum SVObjectTypeEnum
		{
			SVNotSetObjectType						= 0x00000000,

			SVSystemObjectType						= 0x00000001,
			SVDigitizerObjectType					= 0x00000002,
			SVImageProcessingObjectType				= 0x00000003,

			SVInspectionObjectType = 0x00000004,

			SVFreezeSetObjectType					= 0x00000005,
			SVToolSetObjectType						= 0x00000006,

			SVToolObjectType						= 0x00000007,
			SVEquationObjectType					= 0x00000008,
			SVROIObjectType							= 0x00000009,
			SVImageArithmeticObjectType				= 0x0000000a,
			SVMathContainerObjectType				= 0x0000000b,
			SVRotateReferenceObjectType				= 0x0000000c,

			SVLineObjectType						= 0x00000010,
			SVImageObjectType						= 0x00000011,
			SVDataBufferClassObjectType				= 0x00000012,

			SVTriggerProcessingObjectType = 0x00000013,
			SVDigitizerProcessingObjectType = 0x00000014,

			SVExternalToolTaskObjectType            = 0x00000020,

			SVImageToLineProjectObjectType			= 0x00000030,

			SVTransformObjectType					= 0x00000040,	// 08 Feb 2000 - SEJ
			SVPolarTransformObjectType				= 0x00000041,	// RO_25Feb2000

			SVPPQObjectType							= 0x00000050,
			SVDisplayObjectType						= 0x00000051,
			SVInspectionProcessType					= 0x00000052,
			SVVirtualCameraType						= 0x00000053,
			SVTriggerObjectType						= 0x00000054,
			SVInputObjectListType					= 0x00000055,
			SVOutputObjectListType					= 0x00000056,
			SVIOControllerType                      = 0x00000057,
			SVRemoteOutputGroupType                 = 0x00000058,
			SVLinearEdgeProcessingObjectType        = 0x00000060,

			SVDWordValueObjectType					= 0x00000100,
			SVLongValueObjectType					= 0x00000101,
			SVDoubleValueObjectType					= 0x00000102,
			SVBoolValueObjectType					= 0x00000103,
			SVPointValueObjectType					= 0x00000104,
			SVByteValueObjectType					= 0x00000105,
			SVStringValueObjectType					= 0x00000106,
			SVEnumValueObjectType					= 0x00000107,
			SVDPointValueObjectType					= 0x00000108,
			SVInt64ValueObjectType					= 0x00000109,
			SVCharValueObjectType					= 0x0000010A,
			SVVariantValueObjectType				= 0x0000010B,

			SVOperatorObjectType					= 0x00001000,
			SVBinaryOperatorObjectType				= 0x00002000,
			SVUnaryOperatorObjectType				= 0x00003000,
			SVUnaryImageOperatorObjectType			= 0x00004000,
			SVFilterObjectType						= 0x00005000,
			SVUnaryImageOperatorListObjectType		= 0x00006000,

			SVAnalyzerObjectType					= 0x00010000,

			SVAcquisitionObjectType					= 0x00020000,

			SVResultObjectType						= 0x00100000,
			SVRangeObjectType						= 0x00200000,
			SVToolSizeAdjustTaskType				= 0x00300000,
		};

		// place all sub types here
		enum SVObjectSubTypeEnum
		{
			SVNotSetSubObjectType						= 0x00000000,

			// Image Subtypes:		
			SVMainImageObjectType						= 0x00000001,
			SVRGBMainImageObjectType					= 0x00000002,

			// System Subtypes:
			SVCorecoSystemObjectType					= 0x00000004,
			SVMatroxSystemObjectType					= 0x00000005,

			SVCorecoImageProcessingObjectType	= 0x00000006,
			SVMatroxImageProcessingObjectType	= 0x00000007,
			SVIntekImageProcessingObjectType	= 0x00000008,
			SVFileAcquisitionImageProcessingObjectType	= 0x00000009,

			// Tool Subtypes:
			SVWindowToolObjectType						= 0x00000010,
			SVGageToolObjectType						= 0x00000011,
			SVMathToolObjectType						= 0x00000012,
			SVToolBuildReferenceObjectType				= 0x00000013,
			SVToolProfileObjectType						= 0x00000014,
			SVToolImageObjectType						= 0x00000015,
			SVToolArchiveObjectType                     = 0x00000016,
			SVToolLoadImageObjectType					= 0x00000017,
			SVRoundWindowToolObjectType					= 0x00000018,
			SVStatisticsToolObjectType					= 0x00000019,
			SVToolAcquisitionObjectType					= 0x0000001a,
			SVToolFreezeObjectType						= 0x0000001b,
			SVTransformationToolObjectType				= 0x0000001c,	// 08 Feb 2000 - SEJ
			SVPolarTransformationToolObjectType			= 0x0000001d, 
			SVColorToolObjectType						= 0x0000001e,
			SVToolCylindricalObjectType					= 0x0000001f,
			SVPerspectiveToolObjectType                 = 0x00000020,
			SVExternalToolObjectType                    = 0x00000025,
			SVLinearToolObjectType						= 0x00000026,
			SVRemoteInputToolObjectType					= 0x00000027,
			SVShiftToolObjectType						= 0x00000028,

			// Analyzer Subtypes:	
			SVPixelAnalyzerObjectType				    = 0x00000100,
			SVBlobAnalyzerObjectType					= 0x00000101,
			SVLuminanceAnalyzerObjectType				= 0x00000102,
			SVOCRAnalyzerObjectType						= 0x00000103, // 27 Aug 1999 - frb.
			SVOCRGrayAnalyzerObjectType					= 0x00000104, // 27 Aug 1999 - frb.
			SVOCVAnalyzerObjectType						= 0x00000105, // 27 Aug 1999 - frb.
			SVHistogramAnalyzerObjectType				= 0x00000106,
			SVPixelAnalyzerResultObjectType		        = 0x00000200,
			SVPixelCountObjectType						= 0x00000201,

			SVLinePixelCountingAnalyzerObjectType		= 0x00000300,
			SVLineEdgeCountingAnalyzerObjectType		= 0x00000301,
			SVLineEdgePositionAnalyzerObjectType		= 0x00000302,
			SVLineLinearMeasurementAnalyzerObjectType	= 0x00000303,
			SVLineMaximumForegroundObjectAnalyzerObjectType = 0x00000304,
			SVLineMaximumBackgroundObjectAnalyzerObjectType = 0x00000305,
			SVLineMaximumObjectAnalyzerObjectType		= 0x00000306,

			SVLinearEdgeAProcessingObjectType = 0x00000380,
			SVLinearEdgeBProcessingObjectType = 0x00000381,

			SVLinearPixelCountingAnalyzerObjectType		        = 0x00000397,
			SVLinearEdgeCountingAnalyzerObjectType		        = 0x00000398,
			SVLinearEdgePositionAnalyzerObjectType		        = 0x00000399,
			SVLinearMeasurementAnalyzerObjectType	            = 0x0000039a,
			SVLinearMaximumForegroundObjectAnalyzerObjectType = 0x0000039b,
			SVLinearMaximumBackgroundObjectAnalyzerObjectType = 0x0000039c,
			SVLinearMaximumObjectAnalyzerObjectType		        = 0x0000039d,


			// Equation Subtypes:
			SVConditionalObjectType						= 0x00000400,
			SVMathEquationObjectType					= 0x00000401,
			SVUpperThresholdEquationObjectType			= 0x00000402,
			SVLowerThresholdEquationObjectType			= 0x00000403,
			SVAutoThresholdEquationObjectType			= 0x00000404,
			SVLUTEquationObjectType						= 0x00000405,	// RO_17Mar2000

			//Equation Subtypes for VAriableSize 	
			EQSizeWidthType								= 0x00000406,
			EQSizeHeightType							 = 0x00000407,
			EQSizePositionXType							= 0x00000408,
			EQSizePositionYType							 = 0x00000409,

			// Math Container Subtypes:
			SVEvaluateXObjectType						= 0x00000480,
			SVEvaluateYObjectType						= 0x00000481,
			SVEvaluateBetaObjectType					= 0x00000482,
			SVEvaluateObjectType						= 0x00000483,	// 08 Feb 2000 - SEJ
			SVEvaluateTranslationXObjectType			= 0x00000484,	// 08 Feb 2000 - SEJ
			SVEvaluateTranslationYObjectType			= 0x00000485,	// 08 Feb 2000 - SEJ
			SVEvaluateRotationXObjectType				= 0x00000486,	// 08 Feb 2000 - SEJ
			SVEvaluateRotationYObjectType				= 0x00000487,	// 08 Feb 2000 - SEJ
			SVEvaluateRotationAngleObjectType			= 0x00000488,	// 08 Feb 2000 - SEJ
			SVEvaluateCenterXObjectType					= 0x00000489,	// RO_27Feb2000
			SVEvaluateCenterYObjectType					= 0x0000048a,	// RO_27Feb2000
			SVEvaluateStartRadiusObjectType				= 0x0000048b,	// RO_27Feb2000
			SVEvaluateEndRadiusObjectType				= 0x0000048c,	// RO_27Feb2000
			SVEvaluateStartAngleObjectType				= 0x0000048d,	// RO_27Feb2000
			SVEvaluateEndAngleObjectType				= 0x0000048e,	// RO_27Feb2000
			
			// ROI Subtypes:
			SVLineROIObjectType							= 0x00000500,
			SVImageROIObjectType						= 0x00000501,

			// Transform Subtypes:
			SVImageTransformObjectType					= 0x00000600,	// 08 Feb 2000 - SEJ
			SVPointTransformObjectType					= 0x00000601,	// 08 Feb 2000 - SEJ

			// Polar Transform Subtypes:
			SVImagePolarTransformObjectType				= 0x00000700,	// RO_27Feb2000

			// Result Subtypes:
			SVResultPointXObjectType					= 0x00000800,
			SVResultPointYObjectType					= 0x00000801,
			SVResultLongObjectType						= 0x00000802,
			SVResultDoubleObjectType					= 0x00000803,
			SVResultDPointXObjectType					= 0x00000804,
			SVResultDPointYObjectType					= 0x00000805,
			SVResultVariantObjectType                   = 0x00000806,

			SVResultOCRObjectType                       = 0x00000900,  // 30 Aug 1999 - frb.
			SVResultOCRGrayObjectType                   = 0x00000901,  // 30 Aug 1999 - frb.
			SVResultOCVObjectType						= 0x00000902,  // 30 Aug 1999 - frb.
			SVResultBlobObjectType                      = 0x00000950,
			SVResultStringObjectType                    = 0x00000955,
			SVBarCodeAnalyzerObjectType					= 0x0000095a,
			SVResultBarCodeObjectType					= 0x0000095c,
			SVPatternAnalyzerObjectType					= 0x00000960,	// Sri 04-12-00		

			// Unary Image Operator List Subtypes:
			SVStdImageOperatorListObjectType			= 0x00001000,	// RO_20Mar2000
			SVInPlaceImageOperatorListObjectType		= 0x00001001,	// RO_20Mar2000
			SVLinearImageOperatorListObjectType		= 0x00001002,

			// Unary Image Operator Subtypes:
			SVThresholdObjectType						= 0x00002000,
			SVRoundMaskOperatorObjectType				= 0x00002001,
			SVUserMaskOperatorObjectType				= 0x00002002,
			SVClosingFilterObjectType					= 0x00002003,
			SVOpeningFilterObjectType					= 0x00002004,
			SVDilationFilterObjectType					= 0x00002005,
			SVErosionFilterObjectType					= 0x00002006,
			SVEdgeDetectFilterObjectType				= 0x00002007,
			SVEdgeDetect2FilterObjectType				= 0x00002008,
			SVLaplacianEdgeFilterObjectType				= 0x00002009,
			SVLaplacianEdge2FilterObjectType			= 0x0000200b,
			SVVertEdgeFilterObjectType					= 0x0000200c,
			SVHorzEdgeFilterObjectType					= 0x0000200d,
			SVSharpenFilterObjectType					= 0x0000200e,
			SVSharpen2FilterObjectType					= 0x0000200f,
			SVSmoothFilterObjectType					= 0x00002010,
			SVMaxMinusMinFilterObjectType				= 0x00002011,
			SVLUTOperatorObjectType						= 0x00002012,	// RO_16Mar2000
			SVCustomFilterObjectType					= 0x00002013,
			SVRankingFilterObjectType					= 0x00002014,
			SVSkeletonFilterObjectType                  = 0x00002015,
			SVWatershedFilterObjectType                 = 0x00002016,
			SVThinningFilterObjectType                  = 0x00002017,
			SVThickeningFilterObjectType                = 0x00002018,
			SVShapeMaskHelperObjectType					= 0x00002019,
			SVCustom2FilterObjectType					= 0x0000201a,
			// Operator SubTypes
			SVColorThresholdObjectType					= 0x00003000,

			// unknown
			SVRegressionTestModeObjectType				= 0x00011010,


			//...
			//...
			//...
		};

		////////////////////////////////////////////////////////////////////////////////
		// .Title       : SVObjectInfoStruct
		// -----------------------------------------------------------------------------
		// .Description : 
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
		//  :03.02.1999 RO			First Implementation
		////////////////////////////////////////////////////////////////////////////////
		struct SVObjectTypeInfoStruct
		{
			SVObjectTypeInfoStruct( SVObjectTypeEnum p_ObjectType=SVNotSetObjectType, long p_SubType=SVNotSetSubObjectType )
				: ObjectType( p_ObjectType ), SubType( p_SubType ), EmbeddedID( SVInvalidGUID ) {};

			SVObjectTypeInfoStruct( const SVGUID& p_rEmbeddedID, SVObjectTypeEnum p_ObjectType=SVNotSetObjectType, long p_SubType=SVNotSetSubObjectType )
				: ObjectType( p_ObjectType ), SubType( p_SubType ), EmbeddedID( p_rEmbeddedID ){};

			//******************************************************************************
			// Data Element(s):
			//******************************************************************************

			SVObjectTypeEnum	ObjectType;			// Main Type
			long				SubType;			// Sub Type

			// Embedded ID, if any.
			GUID				EmbeddedID;
		};
//	}  //namespace ObjectInterfaces
//} //namespace Seidenader

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObjectLibrary\SVObjectTypeInfoStruct.h_v  $
 * 
 *    Rev 1.1   24 Oct 2014 10:54:08   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  942
 * SCR Title:  Create new Custom2 Filter SVO-324 SVO-67 SVO-74
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added Unary Image Operator Subtype for Custom2 Filter
 * Changed SVObjectSubTypeEnum
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 17:01:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   05 Dec 2012 11:46:24   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   New SVObjectTypeEnum value for SVShiftToolObjectType
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   18 Jun 2012 17:13:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
 * Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   30 Jan 2012 10:33:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated object manager to reduce unnecessary methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   16 Jan 2012 15:04:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated object manager to include the new notification functionality to managed object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   07 Dec 2010 15:56:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   21 Oct 2008 14:26:50   jhanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  629
 * SCR Title:  Add Histogram analyzer
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Added SVHistogramAnalyzerObjectType
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   16 Sep 2008 13:04:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added SVFileAcquisitionImageProcessingObjectType
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   20 Jun 2007 13:27:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   08 Nov 2005 15:22:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update emun to have new Intek board class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   01 Aug 2005 11:41:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   removed VectorObject types
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   19 May 2005 14:43:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added SVShapeMaskHelperObjectType
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Mar 2005 14:07:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  478
 * SCR Title:  Add a new Perspective Warp Tool
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Object Type for Perspective Tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Mar 2005 11:56:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  477
 * SCR Title:  Add Cylindrical warp Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added SVToolCylindricalObjectType
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Mar 2005 11:56:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  477
 * SCR Title:  Add Cylindrical warp Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added SVToolCylindricalObjectType
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Feb 2005 14:59:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   First version of file for SVObjectLibrary
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
