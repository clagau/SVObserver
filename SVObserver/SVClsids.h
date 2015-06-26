//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVClsids
//* .File Name       : $Workfile:   SVClsids.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   19 Dec 2014 14:05:40  $
//******************************************************************************

#ifndef SVCLSIDS_H
#define SVCLSIDS_H

////////////////////////////////////////////////////////////////////////////////
// SVObserver Class IDs (Guids)
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// NOTE: Add new GUIDs at the bottom.
////////////////////////////////////////////////////////////////////////////////

// GUID for the SVValueObjectClass
extern /*const*/ GUID SVValueObjectClassGuid;

// GUID for the SVBoolValueObjectClass
extern /*const*/ GUID SVBoolValueObjectClassGuid;

// GUID for the SVDWordValueObjectClass
extern /*const*/ GUID SVDWordValueObjectClassGuid;

// GUID for the SVDoubleValueObjectClass
extern /*const*/ GUID SVDoubleValueObjectClassGuid;

// GUID for the SVLongValueObjectClass
extern /*const*/ GUID SVLongValueObjectClassGuid;

// GUID for the SVPointValueObjectClass
extern /*const*/ GUID SVPointValueObjectClassGuid;

// GUID for the SVDPointValueObjectClass
extern /*const*/ GUID SVDPointValueObjectClassGuid;

// GUID for the SVByteValueObjectClass
extern /*const*/ GUID SVByteValueObjectClassGuid;

// GUID for the SVStringValueObjectClass
extern /*const*/ GUID SVStringValueObjectClassGuid;  // 16 Aug 1999 - frb.

// GUID for the SVStringValueObjectClass
extern /*const*/ GUID SVStaticStringValueObjectClassGuid;

// GUID for the SVFileNameValueObjectClass
extern /*const*/ GUID SVFileNameValueObjectClassGuid;	//	11-Apr-2001 - SES

//GUID for the SVInt64ValueObjectClass
extern /*const*/ GUID SVInt64ValueObjectClassGuid;

//GUID for the SVCharValueObjectClass
extern /*const*/ GUID SVCharValueObjectClassGuid;

extern /*const*/ GUID SVTimerValueObjectClassGuid;

extern /*const*/ GUID SVVariantValueObjectClassGuid;

// GUID for the SVImageClass
extern /*const*/ GUID SVImageClassGuid;

// GUID for the SVMainImageClass
extern /*const*/ GUID SVMainImageClassGuid;

// GUID for the SVTaskObjectClass
extern /*const*/ GUID SVTaskObjectClassGuid;

// GUID for the SVObjectAppClass
extern /*const*/ GUID SVObjectAppClassGuid;

// GUID for the Embedded Objects of the SVTaskObjectClass
extern /*const*/ GUID SVTaskObjectClassIsObjectValidGuid;

// GUID for the SVTaskObjectClass
extern /*const*/ GUID SVTaskObjectListClassGuid;

// GUID for the SVInspectionProcess
extern /*const*/ GUID SVInspectionProcessGuid;

// GUID for the SVConfigurationObject
extern /*const*/ GUID SVConfigurationObjectGuid;

// GUID for the RootObject
extern /*const*/ GUID RootObjectGuid;

// GUID for the SVIOController
extern /*const*/ GUID SVIOControllerGuid;

// GUID for the SVToolSetClass
extern /*const*/ GUID SVToolSetClassGuid;

// GUID for the SVToolSetEnabled
extern /*const*/ GUID SVToolSetEnabledObjectGuid;

// GUID for the SVToolClass
extern /*const*/ GUID SVToolClassGuid;

// GUID for the Embedded Objects of the SVTooClass
extern /*const*/ GUID SVToolEnabledObjectGuid;

// GUID for the Embedded Objects of the SVToolClass
extern /*const*/ GUID SVToolSelectedForOperatorMoveObjectGuid;

//GUID for the Embedded object for SVToolClass
extern /*const*/ GUID SVToolTimeGuid;

//GUID for the Embedded object for SVToolClass
extern /*const*/ GUID SVMinToolSetTimeGuid;
extern /*const*/ GUID SVMaxToolSetTimeGuid;

// GUID for the SVWindowToolClass
extern /*const*/ GUID SVWindowToolClassGuid;

// GUID for the SVMathToolClass
extern /*const*/ GUID SVMathToolClassGuid;

extern /*const*/ GUID SVStatisticsToolClassGuid;

// GUID for the SVAnalyzerClass
extern /*const*/ GUID SVAnalyzerClassGuid;

// GUID for the SVImageAnalyzerClass
extern /*const*/ GUID SVImageAnalyzerClassGuid;

// GUID for the SVBlobAnalyzerClass
extern /*const*/ GUID SVBlobAnalyzerClassGuid;

// GUID for the SVWhitePixelAnalyzerClass
extern /*const*/ GUID SVPixelAnalyzerClassGuid;

// GUID for the SVLuminanceAnalyzerClass
extern /*const*/ GUID SVLuminanceAnalyzerClassGuid;

// GUIDs for the SVHistogramAnalyzerClass/value objects
extern /*const*/ GUID SVHistogramAnalyzerClassGuid;
extern /*const*/ GUID SVHistogramValueObjectGuid;
extern /*const*/ GUID SVHistogramMeanObjectGuid;
extern /*const*/ GUID SVHistogramStdDevObjectGuid;
extern /*const*/ GUID SVHistogramRangeStartObjectGuid;
extern /*const*/ GUID SVHistogramRangeEndObjectGuid;
extern /*const*/ GUID SVHistogramScaleObjectGuid;
extern /*const*/ GUID SVHistogramRangeSizeObjectGuid;
extern /*const*/ GUID SVHistogramMaxPixelObjectGuid;
extern /*const*/ GUID SVHistogramMinPixelObjectGuid;
extern /*const*/ GUID SVHistogramPixelCountObjectGuid;
extern /*const*/ GUID SVHistogramImageObjectGuid;
extern /*const*/ GUID SVHistogramHighPeakObjectGuid;
extern /*const*/ GUID SVHistogramLowPeakObjectGuid;
extern /*const*/ GUID SVHistogramValleyObjectGuid;
extern /*const*/ GUID SVHistogramPeakThresholdObjectGuid;
extern /*const*/ GUID SVHistogramDefaultPeakObjectGuid;
extern /*const*/ GUID SVHistogramAccumulateObjectGuid;
extern /*const*/ GUID SVHistogramMinPeakHeightObjectGuid;
extern /*const*/ GUID SVHistogramValleyLowerBoundObjectGuid;
extern /*const*/ GUID SVHistogramValleyUpperBoundObjectGuid;
extern /*const*/ GUID SVHistogramValleyDefaultObjectGuid;
extern /*const*/ GUID SVHistogramFixedHeightValueObjectGuid;
extern /*const*/ GUID SVHistogramDynamicHeightObjectGuid;

// GUID for the SVLuminanceValueObject
extern /*const*/ GUID SVLuminanceValueObjectGuid;

// GUID for the SVCalcStdDevObject
extern /*const*/ GUID SVCalcStdDevObjectGuid;

// GUIDs for the Embedded Objects in the SVWhitePixelAnalyzeResultClass
extern /*const*/ GUID SVPixelColorIndexObjectGuid;
extern /*const*/ GUID SVPixelCountObjectGuid;

// GUID for the SVAnalyzeFeatureClass
extern /*const*/ GUID SVAnalyzeFeatureClassGuid;

extern /*const*/ GUID SVBlobEnabledFeaturesObjectGuid;
extern /*const*/ GUID SVNbrOfBlobsObjectGuid;
extern /*const*/ GUID SVMaxBlobDataArraySizeObjectGuid;
extern /*const*/ GUID SVSortFeatureObjectGuid;
extern /*const*/ GUID SVNbrOfBlobsFoundObjectGuid;

extern /*const*/ GUID SVBlobAreaObjectGuid;
extern /*const*/ GUID SVBlobBoxXMaxObjectGuid;
extern /*const*/ GUID SVBlobBoxXMinObjectGuid;
extern /*const*/ GUID SVBlobBoxYMaxObjectGuid;
extern /*const*/ GUID SVBlobBoxYMinObjectGuid;
extern /*const*/ GUID SVBlobBreadthObjectGuid;
extern /*const*/ GUID SVBlobCenterOfGravityXObjectGuid;
extern /*const*/ GUID SVBlobCenterOfGravityYObjectGuid;
extern /*const*/ GUID SVBlobConvexPerimeterObjectGuid;
extern /*const*/ GUID SVBlobFeretElongationObjectGuid;
extern /*const*/ GUID SVBlobFeretMaxAngleObjectGuid;
extern /*const*/ GUID SVBlobFeretMaxDiameterObjectGuid;
extern /*const*/ GUID SVBlobFeretMeanDiameterObjectGuid;
extern /*const*/ GUID SVBlobFeretMinAngleObjectGuid;
extern /*const*/ GUID SVBlobFeretMinDiameterObjectGuid;
extern /*const*/ GUID SVBlobFeretXObjectGuid;
extern /*const*/ GUID SVBlobFeretYObjectGuid;
extern /*const*/ GUID SVBlobFirstPointXObjectGuid;
extern /*const*/ GUID SVBlobFirstPointYObjectGuid;
extern /*const*/ GUID SVBlobLabelObjectGuid;
extern /*const*/ GUID SVBlobLengthObjectGuid;
extern /*const*/ GUID SVBlobNbrOfHolesObjectGuid;
extern /*const*/ GUID SVBlobPerimeterObjectGuid;
extern /*const*/ GUID SVBlobRoughnessObjectGuid;
extern /*const*/ GUID SVBlobSumPixelObjectGuid;
extern /*const*/ GUID SVBlobCompactnessObjectGuid;
extern /*const*/ GUID SVBlobNumberOfRunsObjectGuid;
extern /*const*/ GUID SVBlobXMinAtYMinObjectGuid;
extern /*const*/ GUID SVBlobXMaxAtYMaxObjectGuid;
extern /*const*/ GUID SVBlobYMinAtXMaxObjectGuid;
extern /*const*/ GUID SVBlobYMaxAtXMinObjectGuid;
extern /*const*/ GUID SVBlobElongationObjectGuid;
extern /*const*/ GUID SVBlobIntercept0ObjectGuid;
extern /*const*/ GUID SVBlobIntercept45ObjectGuid;
extern /*const*/ GUID SVBlobIntercept90ObjectGuid;
extern /*const*/ GUID SVBlobIntercept135ObjectGuid;
extern /*const*/ GUID SVBlobMomentX0Y1;
extern /*const*/ GUID SVBlobMomentX1Y0;
extern /*const*/ GUID SVBlobMomentX1Y1;
extern /*const*/ GUID SVBlobMomentX0Y2;
extern /*const*/ GUID SVBlobMomentX2Y0;
extern /*const*/ GUID SVBlobMomentCentralX0Y2;
extern /*const*/ GUID SVBlobMomentCentralX2Y0;
extern /*const*/ GUID SVBlobMomentCentralX1Y1;
extern /*const*/ GUID SVBlobAxisPrincipalAngle;
extern /*const*/ GUID SVBlobAxisSecondaryAngle;
extern /*const*/ GUID SVBlobEulerNumber;
extern /*const*/ GUID SVSortAscendingObjectGuid;
extern /*const*/ GUID SVExcludeFailedObjectGuid;

//GUID for new blob feature.  CenterOfBoundingBox
extern /*const*/ GUID SVBlobCenterOfBoundingBoxXGuid;
extern /*const*/ GUID SVBlobCenterOfBoundingBoxYGuid;

extern /*const*/ GUID SVBlobIsBlackGuid;

//new GUIDs for BlobAnalyzer for Use FIll, Fill Color, and Fill Type
extern /*const*/ GUID SVBlobUseFillGuid;
extern /*const*/ GUID SVBlobFillColorGuid;
extern /*const*/ GUID SVBlobFillTypeGuid;

// GUIDs for the Embedded Objects in the SVStatisticsToolClass
extern /*const*/ GUID SVStatMinObjectGuid;
extern /*const*/ GUID SVStatMaxObjectGuid;
extern /*const*/ GUID SVStatAverageObjectGuid;
extern /*const*/ GUID SVStatStdDevObjectGuid;
extern /*const*/ GUID SVStatVarianceObjectGuid;
extern /*const*/ GUID SVStatTestValueObjectGuid;
extern /*const*/ GUID SVStatNbrOfOccurancesObjectGuid;
extern /*const*/ GUID SVStatPercentOfOccurancesObjectGuid;
extern /*const*/ GUID SVStatEnabledFeaturesObjectGuid;
extern /*const*/ GUID SVStatVariableToMonitorObjectGuid;
extern /*const*/ GUID SVStatVariableNameToMonitorObjectGuid;

// GUID for the SVOperatorClass
extern /*const*/ GUID SVOperatorClassGuid;

// GUID for the SVBinaryOperatorClass
extern /*const*/ GUID SVBinaryOperatorClassGuid;

// GUID for the SVUnaryOperatorClass
extern /*const*/ GUID SVUnaryOperatorClassGuid;

// GUID for the SVUnaryImageOperatorClass
extern /*const*/ GUID SVUnaryImageOperatorClassGuid;

// GUID for the SVFilterClass
extern /*const*/ GUID SVFilterClassGuid;

// GUID for the SVThresholdClass
extern /*const*/ GUID SVThresholdClassGuid;

// GUIDs for the Embedded Objects in the SVThresholdClass
extern /*const*/ GUID  SVUpperThresholdObjectGuid;
extern /*const*/ GUID  SVLowerThresholdObjectGuid;
extern /*const*/ GUID  SVThresholdActivateObjectGuid;
extern /*const*/ GUID  SVUpperThresholdActivateObjectGuid;
extern /*const*/ GUID  SVLowerThresholdActivateObjectGuid;
extern /*const*/ GUID SVAutoThresholdObjectGuid;
extern /*const*/ GUID SVBlackBackgroundObjectGuid;
extern /*const*/ GUID SVAutoThresholdMultiplierObjectGuid;

// GUID for the SVSmoothFilterClass
extern /*const*/ GUID SVSmoothFilterClassGuid;

// GUID for the SVSharpenFilterClass
extern /*const*/ GUID SVSharpenFilterClassGuid;

// GUID for the SVSharpen2FilterClass
extern /*const*/ GUID SVSharpen2FilterClassGuid;

// GUID for the SVHorizEdgeFilterClass
extern /*const*/ GUID SVHorizEdgeFilterClassGuid;

// GUID for the SVVertEdgeFilterClass
extern /*const*/ GUID SVVertEdgeFilterClassGuid;

// GUID for the SVEdgeDetectFilterClass
extern /*const*/ GUID SVEdgeDetectFilterClassGuid;

// GUID for the SVEdgeDetect2FilterClass
extern /*const*/ GUID SVEdgeDetect2FilterClassGuid;

// GUID for the SVLaplacianEdgeFilterClass
extern /*const*/ GUID SVLaplacianEdgeFilterClassGuid;

// GUID for the SVLaplacianEdge2FilterClass
extern /*const*/ GUID SVLaplacianEdge2FilterClassGuid;

// GUID for the SVMaxMinusMinFilterClass
extern /*const*/ GUID SVMaxMinusMinFilterClassGuid;

// GUID for the SVErosionFilterClass
extern /*const*/ GUID SVErosionFilterClassGuid;

// GUID for the SVDilationFilterClass
extern /*const*/ GUID SVDilationFilterClassGuid;

// GUID for the SVOpeningFilterClass
extern /*const*/ GUID SVOpeningFilterClassGuid;

// GUID for the SVClosingFilterClass
extern /*const*/ GUID SVClosingFilterClassGuid;

// GUID for the SVCustomFilterClassGuid
extern /*const*/ GUID SVCustomFilterClassGuid;

// GUID for the Custom2FilterClassGuid
extern /*const*/ GUID Custom2FilterClassGuid;

// GUID for the Custom2FilterKernel
extern /*const*/ GUID Custom2FilterKernelGuid;

// GUIDs for the Embedded Objects of the SVCustomFilterClass
extern /*const*/ GUID SVCustomFilterCell01Guid;
extern /*const*/ GUID SVCustomFilterCell02Guid;
extern /*const*/ GUID SVCustomFilterCell03Guid;
extern /*const*/ GUID SVCustomFilterCell04Guid;
extern /*const*/ GUID SVCustomFilterCell05Guid;
extern /*const*/ GUID SVCustomFilterCell06Guid;
extern /*const*/ GUID SVCustomFilterCell07Guid;
extern /*const*/ GUID SVCustomFilterCell08Guid;
extern /*const*/ GUID SVCustomFilterCell09Guid;
extern /*const*/ GUID SVCustomFilterCell10Guid;
extern /*const*/ GUID SVCustomFilterCell11Guid;
extern /*const*/ GUID SVCustomFilterCell12Guid;
extern /*const*/ GUID SVCustomFilterCell13Guid;
extern /*const*/ GUID SVCustomFilterCell14Guid;
extern /*const*/ GUID SVCustomFilterCell15Guid;
extern /*const*/ GUID SVCustomFilterCell16Guid;
extern /*const*/ GUID SVCustomFilterCell17Guid;
extern /*const*/ GUID SVCustomFilterCell18Guid;
extern /*const*/ GUID SVCustomFilterCell19Guid;
extern /*const*/ GUID SVCustomFilterCell20Guid;
extern /*const*/ GUID SVCustomFilterCell21Guid;
extern /*const*/ GUID SVCustomFilterCell22Guid;
extern /*const*/ GUID SVCustomFilterCell23Guid;
extern /*const*/ GUID SVCustomFilterCell24Guid;
extern /*const*/ GUID SVCustomFilterCell25Guid;
extern /*const*/ GUID SVCustomFilterCell26Guid;
extern /*const*/ GUID SVCustomFilterCell27Guid;
extern /*const*/ GUID SVCustomFilterCell28Guid;
extern /*const*/ GUID SVCustomFilterCell29Guid;
extern /*const*/ GUID SVCustomFilterCell30Guid;
extern /*const*/ GUID SVCustomFilterCell31Guid;
extern /*const*/ GUID SVCustomFilterCell32Guid;
extern /*const*/ GUID SVCustomFilterCell33Guid;
extern /*const*/ GUID SVCustomFilterCell34Guid;
extern /*const*/ GUID SVCustomFilterCell35Guid;
extern /*const*/ GUID SVCustomFilterCell36Guid;
extern /*const*/ GUID SVCustomFilterCell37Guid;
extern /*const*/ GUID SVCustomFilterCell38Guid;
extern /*const*/ GUID SVCustomFilterCell39Guid;
extern /*const*/ GUID SVCustomFilterCell40Guid;
extern /*const*/ GUID SVCustomFilterCell41Guid;
extern /*const*/ GUID SVCustomFilterCell42Guid;
extern /*const*/ GUID SVCustomFilterCell43Guid;
extern /*const*/ GUID SVCustomFilterCell44Guid;
extern /*const*/ GUID SVCustomFilterCell45Guid;
extern /*const*/ GUID SVCustomFilterCell46Guid;
extern /*const*/ GUID SVCustomFilterCell47Guid;
extern /*const*/ GUID SVCustomFilterCell48Guid;
extern /*const*/ GUID SVCustomFilterCell49Guid;

extern /*const*/ GUID SVCustomFilterKernelWidthGuid;
extern /*const*/ GUID SVCustomFilterKernelHeightGuid;
extern /*const*/ GUID SVCustomFilterClippingGuid;
extern /*const*/ GUID SVCustomFilterAbsoluteGuid;
extern /*const*/ GUID SVCustomFilterTransformGuid;

// GUID for the SVRankingFilterClassGuid
extern /*const*/ GUID SVRankingFilterClassGuid;

// GUIDs for the Embedded Objects of the SVRankingFilterClass
extern /*const*/ GUID SVRankingFilterCell01Guid;
extern /*const*/ GUID SVRankingFilterCell02Guid;
extern /*const*/ GUID SVRankingFilterCell03Guid;
extern /*const*/ GUID SVRankingFilterCell04Guid;
extern /*const*/ GUID SVRankingFilterCell05Guid;
extern /*const*/ GUID SVRankingFilterCell06Guid;
extern /*const*/ GUID SVRankingFilterCell07Guid;
extern /*const*/ GUID SVRankingFilterCell08Guid;
extern /*const*/ GUID SVRankingFilterCell09Guid;
extern /*const*/ GUID SVRankingFilterCell10Guid;
extern /*const*/ GUID SVRankingFilterCell11Guid;
extern /*const*/ GUID SVRankingFilterCell12Guid;
extern /*const*/ GUID SVRankingFilterCell13Guid;
extern /*const*/ GUID SVRankingFilterCell14Guid;
extern /*const*/ GUID SVRankingFilterCell15Guid;
extern /*const*/ GUID SVRankingFilterCell16Guid;
extern /*const*/ GUID SVRankingFilterCell17Guid;
extern /*const*/ GUID SVRankingFilterCell18Guid;
extern /*const*/ GUID SVRankingFilterCell19Guid;
extern /*const*/ GUID SVRankingFilterCell20Guid;
extern /*const*/ GUID SVRankingFilterCell21Guid;
extern /*const*/ GUID SVRankingFilterCell22Guid;
extern /*const*/ GUID SVRankingFilterCell23Guid;
extern /*const*/ GUID SVRankingFilterCell24Guid;
extern /*const*/ GUID SVRankingFilterCell25Guid;
extern /*const*/ GUID SVRankingFilterCell26Guid;
extern /*const*/ GUID SVRankingFilterCell27Guid;
extern /*const*/ GUID SVRankingFilterCell28Guid;
extern /*const*/ GUID SVRankingFilterCell29Guid;
extern /*const*/ GUID SVRankingFilterCell30Guid;
extern /*const*/ GUID SVRankingFilterCell31Guid;
extern /*const*/ GUID SVRankingFilterCell32Guid;
extern /*const*/ GUID SVRankingFilterCell33Guid;
extern /*const*/ GUID SVRankingFilterCell34Guid;
extern /*const*/ GUID SVRankingFilterCell35Guid;
extern /*const*/ GUID SVRankingFilterCell36Guid;
extern /*const*/ GUID SVRankingFilterCell37Guid;
extern /*const*/ GUID SVRankingFilterCell38Guid;
extern /*const*/ GUID SVRankingFilterCell39Guid;
extern /*const*/ GUID SVRankingFilterCell40Guid;
extern /*const*/ GUID SVRankingFilterCell41Guid;
extern /*const*/ GUID SVRankingFilterCell42Guid;
extern /*const*/ GUID SVRankingFilterCell43Guid;
extern /*const*/ GUID SVRankingFilterCell44Guid;
extern /*const*/ GUID SVRankingFilterCell45Guid;
extern /*const*/ GUID SVRankingFilterCell46Guid;
extern /*const*/ GUID SVRankingFilterCell47Guid;
extern /*const*/ GUID SVRankingFilterCell48Guid;
extern /*const*/ GUID SVRankingFilterCell49Guid;

extern /*const*/ GUID SVRankingFilterRankingWidthGuid;
extern /*const*/ GUID SVRankingFilterRankingHeightGuid;
extern /*const*/ GUID SVRankingFilterRankingRankGuid;

// GUID for the SVUserMaskOperatorClass
extern /*const*/ GUID SVUserMaskOperatorClassGuid;

// GUIDs for the Unary Image Operator Lists...
// NOTE:
extern /*const*/ GUID SVStdImageOperatorListClassGuid;
extern /*const*/ GUID SVInPlaceImageOperatorListClassGuid;
extern /*const*/ GUID SVLinearImageOperatorListClassGuid;

// GUIDs for the Embedded Objects in the SVUnaryImageOperatorListClass
extern /*const*/ GUID SVOutputImageObjectGuid;

// GUID for the SVResultClass
extern /*const*/ GUID SVResultClassGuid;

// GUID for the SVRangeClass
extern /*const*/ GUID SVRangeClassGuid;

///////////////////////////////////////////////////
// GUIDs for the RingBuffer-Tool
///////////////////////////////////////////////////
extern /*const*/ GUID RingBufferToolGuid;
extern /*const*/ GUID RingBuffer_DepthGuid;
extern /*const*/ GUID RingBuffer_Index1Guid;
extern /*const*/ GUID RingBuffer_Index2Guid;
extern /*const*/ GUID RingBuffer_FlagOfOutputImagesGuid;

// GUIDs for the Embedded Objects in the SVRangeClass
extern /*const*/ GUID SVRangeClassFailHighObjectGuid;
extern /*const*/ GUID SVRangeClassFailLowObjectGuid;
extern /*const*/ GUID SVRangeClassWarnHighObjectGuid;
extern /*const*/ GUID SVRangeClassWarnLowObjectGuid;
extern/*const*/ GUID SVRangeClassFailHighIndirectObjectGuid;
extern/*const*/ GUID SVRangeClassFailLowIndirectObjectGuid;
extern/*const*/ GUID SVRangeClassWarnHighIndirectObjectGuid;
extern/*const*/ GUID SVRangeClassWarnLowIndirectObjectGuid;

// GUIDs for the Embedded Objects in the SVOCRAnalyzeResultClass
extern /*const*/ GUID SVOCRAnalyzerClassGuid;
extern /*const*/ GUID SVOCRAnalyzerResultClassGuid;
extern /*const*/ GUID SVMatchStringObjectGuid;
extern /*const*/ GUID SVFoundStringObjectGuid;
extern /*const*/ GUID SVOCRParametersObjectGuid;
extern /*const*/ GUID SVOCRBlobSortDirectionGuid;
extern /*const*/ GUID SVOCRPixelsForASpaceGuid;
extern /*const*/ GUID SVOCRMatchLineNumberGuid;

// GUIDs for the Exposed OCR Results of the SVOCRAnalyzeResultClass
extern /*const*/ GUID SVOCRHighestMatchScoreGuid;
extern /*const*/ GUID SVOCRLowestMatchScoreGuid;
extern /*const*/ GUID SVOCRAverageMatchScoreGuid;

// GUIDs for the Exposed OCR File Names of the SVOCRAnalyzeResultClass
extern /*const*/ GUID SVOCRFontFileNameGuid;
extern /*const*/ GUID SVOCRMatchStringFileNameGuid;

// GUIDs for the Embedded Objects in the SVOCRGrayAnalyzeResultClass
extern /*const*/ GUID SVOCRGrayAnalyzerClassGuid;        // New Def
extern /*const*/ GUID SVOCRGrayAnalyzerResultClassGuid;  // New Def
extern /*const*/ GUID SVOCRGrayParametersObjectGuid;
extern /*const*/ GUID SVOCRGrayMatchLineNumberGuid;

// GUIDs for the Exposed OCR Results of the SVOCRGrayAnalyzeResultClass
extern /*const*/ GUID SVOCRGrayHighestMatchScoreGuid;
extern /*const*/ GUID SVOCRGrayLowestMatchScoreGuid;
extern /*const*/ GUID SVOCRGrayAverageMatchScoreGuid;

// GUIDs for the Exposed OCR File Names of the SVOCRAnalyzeResultClass
extern /*const*/ GUID SVOCRGrayFontFileNameGuid;
extern /*const*/ GUID SVOCRGrayMatchStringFileNameGuid;

// GUIDs for the Embedded Objects in the SVOCVAnalyzeResultClass
extern /*const*/ GUID SVOCVAnalyzerClassGuid;        // New Def
extern /*const*/ GUID SVOCVAnalyzerResultClassGuid;  // New Def
extern /*const*/ GUID SVOCVPerformOCRGuid;
extern /*const*/ GUID SVOCVMatchLineNumberGuid;

// GUIDs for the Exposed OCR Results of the SVOCVAnalyzeResultClass
extern /*const*/ GUID SVOCVHighestMatchScoreGuid;
extern /*const*/ GUID SVOCVLowestMatchScoreGuid;
extern /*const*/ GUID SVOCVAverageMatchScoreGuid;

// GUIDs for the Exposed OCR File Names of the SVOCRAnalyzeResultClass
extern /*const*/ GUID SVOCVFontFileNameGuid;
extern /*const*/ GUID SVOCVMatchStringFileNameGuid;
extern /*const*/ GUID SVOCVConstraintsFileNameGuid;
extern /*const*/ GUID SVOCVControlsFileNameGuid;
extern /*const*/ GUID SVOCVUseMatchFileGuid;

// GUIDs for the Exposed OCV Results of the SVOCVAnalyzeResultClass
extern /*const*/ GUID SVOCVMatchStringGuid;
extern /*const*/ GUID SVOCVFoundStringGuid;
extern /*const*/ GUID SVOCVCharacterResultClassGuid;
extern /*const*/ GUID SVOCVCharacterValueGuid;
extern /*const*/ GUID SVOCVExtentLeftGuid;
extern /*const*/ GUID SVOCVExtentTopGuid;
extern /*const*/ GUID SVOCVExtentWidthGuid;
extern /*const*/ GUID SVOCVExtentHeightGuid;
extern /*const*/ GUID SVOCVMatchScoreGuid;

extern /*const*/ GUID SVEdgeParameters1ObjectGuid;     // 30 Sep 1999 - frb.
extern /*conts*/ GUID SVEdgeParameters2ObjectGuid;     // 30 Sep 1999 - frb.
extern /*conts*/ GUID SVEdgeParameters3ObjectGuid;     // 30 Sep 1999 - frb.
extern /*conts*/ GUID SVEdgeParameters4ObjectGuid;     // 30 Sep 1999 - frb.
extern /*conts*/ GUID SVEdgeParameters5ObjectGuid;     // 30 Sep 1999 - frb.
extern /*conts*/ GUID SVEdgeParameters6ObjectGuid;     // 30 Sep 1999 - frb.
extern /*conts*/ GUID SVEdgeParameters7ObjectGuid;     // 30 Sep 1999 - frb.
extern /*conts*/ GUID SVEdgeParameters8ObjectGuid;     // 30 Sep 1999 - frb.
extern /*conts*/ GUID SVEdgeParameters9ObjectGuid;     // 30 Sep 1999 - frb.
extern /*conts*/ GUID SVEdgeParameters10ObjectGuid;     // 30 Sep 1999 - frb.

extern /*const*/ GUID SVArchiveToolClassGuid;
extern /*const*/ GUID SVArchiveFilePathObjectGuid;
extern /*const*/ GUID SVArchiveImageGuidsObjectGuid;
extern /*const*/ GUID SVArchiveImageFileRootPathGuid;
extern /*const*/ GUID SVArchiveResultGuidsObjectGuid;
extern /*const*/ GUID SVArchiveAppendArchiveFileGuid;
extern /*const*/ GUID SVArchiveStopAtMaxImagesGuid;
extern /*const*/ GUID SVArchiveMaxImagesCountGuid;
extern /*const*/ GUID SVArchiveMethodGuid;
extern /*const*/ GUID SVArchiveUseHeadersGUID;
extern /*const*/ GUID SVArchiveHeaderLabelGUID;
extern /*const*/ GUID SVArchiveHeaderObjectGUID;
extern /*const*/ GUID SVArchiveResultNamesObjectGuid;
extern /*const*/ GUID SVArchiveImageNamesObjectGuid;

// GUID for the SVEquationClass
extern /*const*/ GUID SVEquationClassGuid;

// GUID for the Embedded Objects in the SVEquationClass
extern /*const*/ GUID SVEquationEnabledObjectGuid;

// GUID for the SVConditionalClass
extern /*const*/ GUID SVConditionalClassGuid;

// GUID for the Embedded Objects in the SVConditionalClass
extern /*const*/ GUID SVConditionalResultObjectGuid;

// GUID for the SVMathEquationClass
extern /*const*/ GUID SVMathEquationClassGuid;

// GUID for the Embedded Objects in the SVMathEquationClass
extern /*const*/ GUID SVMathEquationResultObjectGuid;

// GUID for the SVGageToolClass
extern /*const*/ GUID SVGageToolClassGuid;

// GUID for the SVPixelCountingLineAnalyzerClass
extern /*const*/ GUID SVPixelCountingLineAnalyzerClassGuid;

//GUID for the SVLinearPixelCountingLineAnalyzerClass
extern /*const*/ GUID SVLinearPixelCountingLineAnalyzerClassGuid;

// GUID for the SVROIClass
extern /*const*/ GUID SVROIClassGuid;

// GUID for the SVLineROIClass
extern /*const*/ GUID SVLineROIClassGuid;

// GUID for the SVImageROIClass
extern /*const*/ GUID SVImageROIClassGuid;

// GUID for the SVLineClass
extern /*const*/ GUID SVLineClassGuid;

// GUIDs for the Embedded Objects in the SVLineROIClass
extern /*const*/ GUID SVOutputLineObjectGuid;

// GUID for the SVLineAnalyzerClass
extern /*const*/ GUID SVLineAnalyzerClassGuid;

// GUID for the SVEdgeCountingLineAnalyzerClass
extern /*const*/ GUID SVEdgeCountingLineAnalyzerClassGuid;

// GUID for the SVLinearEdgeCountingLineAnalyzerClass
extern /*const*/ GUID SVLinearEdgeCountingLineAnalyzerClassGuid;

// GUID for the SVEdgePositionLineAnalyzerClass
extern /*const*/ GUID SVEdgePositionLineAnalyzerClassGuid;

// GUID for the SVLinearEdgePositionLineAnalyzerClass
extern /*const*/ GUID SVLinearEdgePositionLineAnalyzerClassGuid;

// GUID for the SVLinearMeasurementLineAnalyzerClass
extern /*const*/ GUID SVLinearMeasurementLineAnalyzerClassGuid;

// GUID for the SVLinearMeasurmentAnalyzerClassGuid
extern /*const*/ GUID SVLinearMeasurementAnalyzerClassGuid;

//GUID for the SVLinearEdgeProcessingClassGuid
extern /*const*/ GUID SVLinearEdgeProcessingClassGuid;

//GUID for the SVLinearEdgeAProcessingClassGuid
extern /*const*/ GUID SVLinearEdgeAProcessingClassGuid;

//GUID for the SVLinearEdgeBProcessingClassGuid
extern /*const*/ GUID SVLinearEdgeBProcessingClassGuid;

//GUID for the SVLinearEdgesClassGuid
extern /*const*/ GUID SVLinearEdgesClassGuid;

// GUIDs for the Embedded Objects in the SVLinearMeasurementAnalyzerClass and all related Line Analyzers
extern /*const*/ GUID SVEdgeAObjectGuid;
extern /*const*/ GUID SVEdgeBObjectGuid;
extern /*const*/ GUID SVCenterObjectGuid;
extern /*const*/ GUID SVWidthObjectGuid;

extern /*const*/ GUID SVDPEdgeAObjectGuid;
extern /*const*/ GUID SVDPEdgeBObjectGuid;
extern /*const*/ GUID SVDPCenterObjectGuid;
extern /*const*/ GUID SVDWidthObjectGuid;

extern /*const*/ GUID SVLinearDistanceEdgeAObjectGuid;
extern /*const*/ GUID SVLinearDistanceEdgeBObjectGuid;

// GUID for the SVPointXResultClass
extern /*const*/ GUID SVPointXResultClassGuid;

// GUID for the SVPointYResultClass
extern /*const*/ GUID SVPointYResultClassGuid;

// GUID for the SVDPointXResultClass
extern /*const*/ GUID SVDPointXResultClassGuid;

// GUID for the SVDPointYResultClass
extern /*const*/ GUID SVDPointYResultClassGuid;

// GUID for the SVLongResultClass
extern /*const*/ GUID SVLongResultClassGuid;

// GUID for the SVDoubleResultClass
extern /*const*/ GUID SVDoubleResultClassGuid;

// GUID for the SVVariantResultClass
extern /*const*/ GUID SVVariantResultClassGuid;

// GUIDs for the Embedded Objects
extern /*const*/ GUID SVXObjectGuid;
extern /*const*/ GUID SVDXObjectGuid;
extern /*const*/ GUID SVYObjectGuid;
extern /*const*/ GUID SVDYObjectGuid;
extern /*const*/ GUID SVValueObjectGuid;

// GUIDs for the Embedded Objects that represent the pass/fail/warn status
extern /*const*/ GUID SVFailedObjectGuid;
extern /*const*/ GUID SVWarnedObjectGuid;
extern /*const*/ GUID SVPassedObjectGuid;
extern /*const*/ GUID SVExplicitFailedObjectGuid;
extern /*const*/ GUID SVPassedCountObjectGuid;
extern /*const*/ GUID SVFailedCountObjectGuid;
extern /*const*/ GUID SVWarnedCountObjectGuid;
extern /*const*/ GUID SVEnabledCountObjectGuid;
extern /*const*/ GUID SVProcessedCountObjectGuid;

// GUID for the SVMaximumBackgroundObjectLineAnalyzerClass
extern /*const*/ GUID SVMaximumBackgroundObjectLineAnalyzerClassGuid;

// GUID for the SVLinearMaximumBackgroundObjectLineAnalzerClass
extern /*const*/ GUID SVLinearMaximumBackgroundObjectLineAnalyzerClassGuid;

// GUID for the SVMaximumForegroundObjectLineAnalyzerClass
extern /*const*/ GUID SVMaximumForegroundObjectLineAnalyzerClassGuid;

// GUID for the SVLinearMaximumForegroundObjectLineAnalyzerClass
extern /*const*/ GUID SVLinearMaximumForegroundObjectLineAnalyzerClassGuid;

// GUID for the SVMaximumObjectLineAnalyzerClass
extern /*const*/ GUID SVMaximumObjectLineAnalyzerClassGuid;

// GUID for the SVLinearMaximumObjectLineAnalyzerClass
extern /*const*/ GUID SVLinearMaximumObjectLineAnalyzerClassGuid;

// GUIDs for the Embedded Objects
extern /*const*/ GUID SVBlackPixelObjectGuid;
extern /*const*/ GUID SVWhitePixelObjectGuid;
extern /*const*/ GUID SVEdgeCountObjectGuid;

// GUIDs for the Embedded Objects
extern /*const*/ GUID SVColorObjectGuid;
extern /*const*/ GUID SVStatusObjectGuid;

// GUID for the SVBuildReferenceToolClass
extern /*const*/ GUID SVBuildReferenceToolClassGuid;

// GUID for the SVImageToLineProjectClass
extern /*const*/ GUID SVImageToLineProjectClassGuid;

// GUID for the SVLinearImageToLineProjectClassGuid
extern /*const*/ GUID SVLinearImageToLineProjectClassGuid;

// GUID for the SVProfileToolClass
extern /*const*/ GUID SVProfileToolClassGuid;

// GUID for Embedded Object
extern /*const*/ GUID SVProjectAngleObjectGuid;
extern /*const*/ GUID SVRotationAngleObjectGuid;
extern /*const*/ GUID SVRotationPointXObjectGuid;
extern /*const*/ GUID SVRotationPointYObjectGuid;
extern /*const*/ GUID SVOutputRotationPointXObjectGuid;
extern /*const*/ GUID SVOutputRotationPointYObjectGuid;

//GUID for Profile Orientation
extern /*const*/ GUID SVProfileOrientationGuid;

//GUID for Linear Tool Use Profile Rotation
extern /*const*/ GUID SVLinearToolUseRotationGuid;

// GUID for the SVDataBufferClass
extern /*const*/ GUID SVDataBufferClassGuid;

// GUID for the SVCylindricalWarpToolClass
extern /*const*/ GUID SVCylindricalWarpToolClassGuid;

// GUID for the SVPerspectiveToolClass
extern /*const*/ GUID SVPerspectiveToolClassGuid;

// Perspective Tool TranslationX
extern /*const*/ GUID SVTranslationXOffsetObjectGuid;

// Perspective Tool TranslationY
extern /*const*/ GUID SVTranslationYOffsetObjectGuid;

// GUID for the SVWarpType Enum Value Object
extern /*const*/ GUID SVWarpTypeObjectGuid;

// GUID for SVWarpAngle double value object
extern /*const*/ GUID SVWarpAngleObjectGuid;

// GUID for the SVImageToolClass
extern /*const*/ GUID SVImageToolClassGuid;

// GUID for the SVImageArithmeticClass
extern /*const*/ GUID SVImageArithmeticClassGuid;

// GUID for Embedded Objects of Image Tool
extern /*const*/ GUID SVEnableOffsetAObjectGuid;
extern /*const*/ GUID SVOffsetAPointObjectGuid;
extern /*const*/ GUID SVEnableOffsetBObjectGuid;
extern /*const*/ GUID SVOffsetBPointObjectGuid;
extern /*const*/ GUID SVArithmeticOperatorObjectGuid;

// GUID for the SVMathContainerClass
extern /*const*/ GUID SVMathContainerClassGuid;

// GUID for the SVEvaluateXClass
extern /*const*/ GUID SVEvaluateXClassGuid;
// GUID for Embedded Objects of SVEvaluateXClass
extern /*const*/ GUID SVOutputEvaluateXEnabledObjectGuid;
extern /*const*/ GUID SVOutputEvaluateXResultObjectGuid;

// GUID for the SVEvaluateYClass
extern /*const*/ GUID SVEvaluateYClassGuid;
// GUID for Embedded Objects of SVEvaluateYClass
extern /*const*/ GUID SVOutputEvaluateYEnabledObjectGuid;
extern /*const*/ GUID SVOutputEvaluateYResultObjectGuid;

// GUID for the SVEvaluateBetaClass
extern /*const*/ GUID SVEvaluateBetaClassGuid;
// GUID for Embedded Objects of SVEvaluateBetaClass
extern /*const*/ GUID SVOutputEvaluateBetaEnabledObjectGuid;
extern /*const*/ GUID SVOutputEvaluateBetaResultObjectGuid;

// GUID for the SVRotateReferenceClass
extern /*const*/ GUID SVRotateReferenceClassGuid;
// GUID for Embedded Objects of SVRotateReferenceClass
extern /*const*/ GUID SVOutputLearnedXObjectGuid;
extern /*const*/ GUID SVOutputLearnedYObjectGuid;
extern /*const*/ GUID SVOutputLearnedBetaObjectGuid;

// GUID for Embedded Objects of SVUserMaskOperatorClass
extern /*const*/ GUID SVMaskEnabledObjectGuid;
extern /*const*/ GUID SVMaskUseImageMaskGuid;
extern /*const*/ GUID SVMaskOperatorGuid;
extern /*const*/ GUID SVMaskFillAreaGuid;
extern /*const*/ GUID SVMaskFillColorGuid;

extern /*const*/ GUID SVShapeMaskHelperClassGuid;
extern /*const*/ GUID SVShapeMaskTypeGuid;
extern /*const*/ GUID SVShapeMaskMaskAreaGuid;
extern /*const*/ GUID SVShapeMaskPropertyCenterXGuid;
extern /*const*/ GUID SVShapeMaskPropertyCenterYGuid;
extern /*const*/ GUID SVShapeMaskPropertyWidthGuid;
extern /*const*/ GUID SVShapeMaskPropertyHeightGuid;
extern /*const*/ GUID SVShapeMaskPropertySideThicknessGuid;
extern /*const*/ GUID SVShapeMaskPropertyTopBottomThicknessGuid;
extern /*const*/ GUID SVShapeMaskPropertyOffsetGuid;
extern /*const*/ GUID SVShapeMaskPropertySymmetryOrientationGuid;
extern /*const*/ GUID SVShapeMaskAutoResizeGuid;

// GUID for the SVLoadImageToolClass
extern /*const*/ GUID SVLoadImageToolClassGuid;
// GUID for Embedded Objects of SVLoadImageToolClass
extern /*const*/ GUID SVPathNameObjectGuid;
extern /*const*/ GUID SVContinuousReloadObjectGuid;

extern /*const*/ GUID SVBarCodeAnalyzerClassGuid;
extern /*const*/ GUID SVBarCodeObjectGuid;
extern /*const*/ GUID SVStringResultClassGuid;
extern /*const*/ GUID SVRegExpressionObjectGuid;
extern /*const*/ GUID SVBarCodeResultClassGuid;
extern /*const*/ GUID SVBCTypeObjectGuid;
extern /*const*/ GUID SVBCOrientationObjectGuid;
extern /*const*/ GUID SVBCSkewNegativeObjectGuid;
extern /*const*/ GUID SVBCSkewPositiveObjectGuid;
extern /*const*/ GUID SVBCCellMinSizeObjectGuid;
extern /*const*/ GUID SVBCCellMaxSizeObjectGuid;
extern /*const*/ GUID SVBCCellNumberXObjectGuid;
extern /*const*/ GUID SVBCCellNumberYObjectGuid;
extern /*const*/ GUID SVBCSpeedObjectGuid;
extern /*const*/ GUID SVBCThresholdObjectGuid;
extern /*const*/ GUID SVBCStringSizeObjectGuid;
extern /*const*/ GUID SVBCErrorCorrectionObjectGuid;
extern /*const*/ GUID SVBCEncodingObjectGuid;
extern /*const*/ GUID SVBCForegroundColorObjectGuid;
extern /*const*/ GUID SVBCSaveStringInFileObjectGuid;
extern /*const*/ GUID SVBCStringFileNameObjectGuid;
extern /*const*/ GUID SVBCUseSingleMatchStringGuid;
extern /*const*/ GUID SVBCUseMatchStringFileGuid;
extern /*const*/ GUID SVBCMatchStringFileNameObjectGuid;
extern /*const*/ GUID SVBCMatchStringLineGuid;
extern /*const*/ GUID SVBCWarnOnFailedReadObjectGuid;
extern /*const*/ GUID SVBCMatchScoreGuid;
extern /*const*/ GUID SVBCTimeoutGuid;
extern /*const*/ GUID SVSkeletonFilterClassGuid;
extern /*const*/ GUID SVWatershedFilterMinVariationGuid;
extern /*const*/ GUID SVWatershedFilterControlFlagGuid;
extern /*const*/ GUID SVWatershedFilterClassGuid;
extern /*const*/ GUID SVWatershedFilterUseMarkerGuid;
extern/*const*/ GUID SVThickeningFilterClassGuid ;
extern/*const*/ GUID SVThickeningFilterItterationsGuid ;
extern/*const*/ GUID SVThickeningFilterGrayOnGuid ;
extern/*const*/ GUID SVThinningFilterClassGuid ;
extern/*const*/ GUID SVThinningFilterItterationsGuid ;
extern/*const*/ GUID SVThinningFilterGrayOnGuid ;
extern/*const*/ GUID SVBCUnevenGridGuid;

// GUID for the SVUpperThresholdEquationClass
extern /*const*/ GUID SVUpperThresholdEquationClassGuid;
// GUID for Embedded Objects of SVUpperThresholdEquationClass
extern /*const*/ GUID SVUpperThresholdEquationResultObjectGuid;

// GUID for the SVLowerThresholdEquationClass
extern /*const*/ GUID SVLowerThresholdEquationClassGuid;
// GUID for Embedded Objects of SVLowerThresholdEquationClass
extern /*const*/ GUID SVLowerThresholdEquationResultObjectGuid;

// GUID for the SVAutoThresholdEquationClass
extern /*const*/ GUID SVAutoThresholdEquationClassGuid;
// GUID for Embedded Objects of SVAutoThresholdEquationClass
extern /*const*/ GUID SVAutoThresholdEquationResultObjectGuid;

// GUIDs for Embedded Objects of SVThresholdClass
extern /*const*/ GUID SVUseExternalATMObjectGuid;
extern /*const*/ GUID SVUseExternalLTObjectGuid;
extern /*const*/ GUID SVUseExternalUTObjectGuid;
extern /*const*/ GUID SVRegressionTestModeGuid;

extern /*const*/ GUID SVAcquisitionToolClassGuid;

// GUIDs for Embedded Objects - Profile Tool Threshold flags and data variables
extern /*const*/ GUID SVUseLowerThresholdSelectableObjectGuid;
extern /*const*/ GUID SVUseLowerThresholdMaxMinusPercentDiffObjectGuid;
extern /*const*/ GUID SVUseLowerThresholdMaxMinusOffsetObjectGuid;
extern /*const*/ GUID SVUseLowerThresholdMinPlusOffsetObjectGuid;
extern /*const*/ GUID SVLowerThresholdMaxMinusPercentDiffObjectGuid;
extern /*const*/ GUID SVLowerThresholdMaxMinusOffsetObjectGuid;
extern /*const*/ GUID SVLowerThresholdMinPlusOffsetObjectGuid;

extern /*const*/ GUID SVUseUpperThresholdSelectableObjectGuid;
extern /*const*/ GUID SVUseUpperThresholdMaxMinusPercentDiffObjectGuid;
extern /*const*/ GUID SVUseUpperThresholdMaxMinusOffsetObjectGuid;
extern /*const*/ GUID SVUseUpperThresholdMinPlusOffsetObjectGuid;
extern /*const*/ GUID SVUpperThresholdMaxMinusPercentDiffObjectGuid;
extern /*const*/ GUID SVUpperThresholdMaxMinusOffsetObjectGuid;
extern /*const*/ GUID SVUpperThresholdMinPlusOffsetObjectGuid;

extern /*const*/ GUID SVUseLowerThresholdSelectableBObjectGuid;
extern /*const*/ GUID SVUseLowerThresholdMaxMinusPercentDiffBObjectGuid;
extern /*const*/ GUID SVUseLowerThresholdMaxMinusOffsetBObjectGuid;
extern /*const*/ GUID SVUseLowerThresholdMinPlusOffsetBObjectGuid;
extern /*const*/ GUID SVLowerThresholdMaxMinusPercentDiffBObjectGuid;
extern /*const*/ GUID SVLowerThresholdMaxMinusOffsetBObjectGuid;
extern /*const*/ GUID SVLowerThresholdMinPlusOffsetBObjectGuid;

extern /*const*/ GUID SVUseUpperThresholdSelectableBObjectGuid;
extern /*const*/ GUID SVUseUpperThresholdMaxMinusPercentDiffBObjectGuid;
extern /*const*/ GUID SVUseUpperThresholdMaxMinusOffsetBObjectGuid;
extern /*const*/ GUID SVUseUpperThresholdMinPlusOffsetBObjectGuid;
extern /*const*/ GUID SVUpperThresholdMaxMinusPercentDiffBObjectGuid;
extern /*const*/ GUID SVUpperThresholdMaxMinusOffsetBObjectGuid;
extern /*const*/ GUID SVUpperThresholdMinPlusOffsetBObjectGuid;

extern /*const*/ GUID SVThresholdMaxObjectGuid;
extern /*const*/ GUID SVThresholdMinObjectGuid;

extern /*const*/ GUID SVLinearThresholdMaxObjectGuid;
extern /*const*/ GUID SVLinearThresholdMinObjectGuid;

extern /*const*/ GUID SVSingleLineAnalyzerClassGuid;
extern /*const*/ GUID SVDualLineAnalyzerClassGuid;

// GUID for the SVTransformationToolClass
extern /*const*/ GUID SVTransformationToolClassGuid;

// GUID for the SVTransformationToolClass
extern /*const*/ GUID SVShiftToolClassGuid;

//GUID for the ShiftTool Mode
extern /*const*/ GUID SVShiftToolModeGuid;
extern /*const*/ GUID SVShiftToolReferenceXObjectGuid;
extern /*const*/ GUID SVShiftToolReferenceYObjectGuid;
extern /*const*/ GUID SVShiftToolEnableSourceImageExtentsGuid;

// GUID for the SVExternalTool
extern /*const*/ GUID SVExternalToolGuid;

// GUID for the SVExternalToolTask
extern /*const*/ GUID SVExternalToolTaskGuid;

// GUID for the SVEvaluateClass
extern /*const*/ GUID SVEvaluateClassGuid;

// GUID for the SVEvaluateTranslationXClass
extern /*const*/ GUID SVEvaluateTranslationXClassGuid;

// GUID for the SVEvaluateTranslationYClass
extern /*const*/ GUID SVEvaluateTranslationYClassGuid;

// GUID for the SVEvaluateRotationXClass
extern /*const*/ GUID SVEvaluateRotationXClassGuid;

// GUID for the SVEvaluateRotationYClass
extern /*const*/ GUID SVEvaluateRotationYClassGuid;

// GUID for the SVEvaluateRotationAngleClass
extern /*const*/ GUID SVEvaluateRotationAngleClassGuid;

// GUID for Embedded Objects of SVEvaluateTranslationXClass
extern /*const*/ GUID SVOutputEvaluateTranslationXResultObjectGuid;

// GUID for Embedded Objects of SVEvaluateTranslationYClass
extern /*const*/ GUID SVOutputEvaluateTranslationYResultObjectGuid;

// GUID for Embedded Objects of SVEvaluateRotationXClass
extern /*const*/ GUID SVOutputEvaluateRotationXResultObjectGuid;

// GUID for Embedded Objects of SVEvaluateRotationYClass
extern /*const*/ GUID SVOutputEvaluateRotationYResultObjectGuid;

// GUID for Embedded Objects of SVEvaluateRotationAngleClass
extern /*const*/ GUID SVOutputEvaluateRotationAngleResultObjectGuid;

// GUID for the SVTransformClass
extern /*const*/ GUID SVTransformClassGuid;

// GUIDs for the Embeddeds of the SVTransformClass
extern /*const*/ GUID SVPerformTranslationObjectGuid;
extern /*const*/ GUID SVTranslationXObjectGuid;
extern /*const*/ GUID SVTranslationYObjectGuid;
extern /*const*/ GUID SVPerformRotationObjectGuid;
extern /*const*/ GUID SVLearnedTranslationXObjectGuid;
extern /*const*/ GUID SVLearnedTranslationYObjectGuid;
extern /*const*/ GUID SVLearnedRotationXObjectGuid;
extern /*const*/ GUID SVLearnedRotationYObjectGuid;
extern /*const*/ GUID SVLearnedRotationAngleObjectGuid;

// GUID for the SVImageTransformClass
extern /*const*/ GUID SVImageTransformClassGuid;

//GUID for the Transform Tool DisplacementX
extern /*const*/ GUID SVImageTransformDisplacementXGuid;

//GUID for the Transform Tool DisplacementY
extern /*const*/ GUID SVImageTransformDisplacementYGuid;

//GUID for the Top Result
extern /*const*/ GUID SVTopResultObjectGuid;

//GUID for the Left Result
extern /*const*/ GUID SVLeftResultObjectGuid;

//GUID for the Transform Tool SourceX
extern /*const*/ GUID SVImageTransformSourceXGuid;

//GUID for the Transform Tool SourceY
extern /*const*/ GUID SVImageTransformSourceYGuid;

// GUIDs for the Embeddeds of the SVImageTransformClass
extern /*const*/ GUID SVUseExtentsOnlyObjectGuid;

// GUID for the SVEnumerateValueObjectClass
extern /*const*/ GUID SVEnumerateValueObjectClassGuid;

// GUID for the SVConditionalToolSetDrawFlagObject
extern /*const*/ GUID SVConditionalToolSetDrawFlagObjectGuid;

// GUID for the SVDrawToolFlagObject
extern /*const*/ GUID SVConditionalToolDrawFlagObjectGuid;

// GUID for the SVPolarTransformClass
extern /*const*/ GUID SVPolarTransformClassGuid;

// GUID for the SVPolarTransformToolClass
extern /*const*/ GUID SVPolarTransformationToolClassGuid;

// GUIDs for the SVImagePolarTransformClass and its Output Objects...
extern /*const*/ GUID SVImagePolarTransformClassGuid;
extern /*const*/ GUID SVOutputCenterXObjectGuid;
extern /*const*/ GUID SVOutputCenterYObjectGuid;
extern /*const*/ GUID SVOutputStartRadiusObjectGuid;
extern /*const*/ GUID SVOutputEndRadiusObjectGuid;
extern /*const*/ GUID SVOutputStartAngleObjectGuid;
extern /*const*/ GUID SVOutputEndAngleObjectGuid;
extern /*const*/ GUID SVOutputInterpolationModeObjectGuid;
extern /*const*/ GUID SVOutputUseFormulaObjectGuid;
extern /*const*/ GUID SVOutputAngularMethodObjectGuid;

// GUIDs for more Evaluation Classes and their Output Objects...
extern /*const*/ GUID SVEvaluateCenterXClassGuid;
extern /*const*/ GUID SVEvaluateCenterYClassGuid;
extern /*const*/ GUID SVEvaluateStartRadiusClassGuid;
extern /*const*/ GUID SVEvaluateEndRadiusClassGuid;
extern /*const*/ GUID SVEvaluateStartAngleClassGuid;
extern /*const*/ GUID SVEvaluateEndAngleClassGuid;
// Output Objects...
extern /*const*/ GUID SVOutputEvaluateCenterXResultObjectGuid;
extern /*const*/ GUID SVOutputEvaluateCenterYResultObjectGuid;
extern /*const*/ GUID SVOutputEvaluateStartRadiusResultObjectGuid;
extern /*const*/ GUID SVOutputEvaluateEndRadiusResultObjectGuid;
extern /*const*/ GUID SVOutputEvaluateStartAngleResultObjectGuid;
extern /*const*/ GUID SVOutputEvaluateEndAngleResultObjectGuid;

// GUIDs for Exposed Extents (primarily Images/Tools)
extern /*const*/ GUID SVExtentRelativeLeftPositionObjectGuid;
extern /*const*/ GUID SVExtentRelativeTopPositionObjectGuid;
extern /*const*/ GUID SVExtentRelativeRightPositionObjectGuid;
extern /*const*/ GUID SVExtentRelativeBottomPositionObjectGuid;
extern /*const*/ GUID SVExtentWidthObjectGuid;
extern /*const*/ GUID SVExtentHeightObjectGuid;

// GUIDs for Exposed Source Image Extents (primarily Images/Tools)
extern /*const*/ GUID SVUpdateAuxilliaryExtentsObjectGuid;
extern /*const*/ GUID SVAuxilliarySourceXObjectGuid;
extern /*const*/ GUID SVAuxilliarySourceYObjectGuid;
extern /*const*/ GUID SVAuxilliarySourceAngleObjectGuid;
extern /*const*/ GUID SVAuxilliarySourceImageNameObjectGuid;
extern /*const*/ GUID SVAuxilliaryDrawTypeObjectGuid;

// GUIDs for Exposed Extents (primarily Lines)
extern /*const*/ GUID SVExtentRelativeX1PositionObjectGuid;
extern /*const*/ GUID SVExtentRelativeY1PositionObjectGuid;
extern /*const*/ GUID SVExtentRelativeX2PositionObjectGuid;
extern /*const*/ GUID SVExtentRelativeY2PositionObjectGuid;
extern /*const*/ GUID SVExtentLengthObjectGuid;

// GUIDs for SVLUTEquationClass and its outputs...
extern /*const*/ GUID SVLUTEquationClassGuid;
extern /*const*/ GUID SVLUTIndexVariableObjectGuid;
extern /*const*/ GUID SVLUTEquationResultObjectGuid;
extern /*const*/ GUID SVLUTEquationClipFlagObjectGuid;

// GUIDs for SVLUTOperatorClass and its outputs...
extern /*const*/ GUID SVLUTOperatorClassGuid;
extern /*const*/ GUID SVOutputLUTVectorObjectGuid;
extern /*const*/ GUID SVUseLUTObjectGuid;
extern /*const*/ GUID SVContinuousRecalcLUTObjectGuid;
extern /*const*/ GUID SVLUTModeObjectGuid;
extern /*const*/ GUID SVLUTUpperClipObjectGuid;
extern /*const*/ GUID SVLUTLowerClipObjectGuid;

// GUID for the SVPatternAnalyzerClass
extern /*const*/ GUID SVPatternAnalyzerClassGuid;

// GUIDs for the Embeddeds of the SVPatternAnalyzerClass
extern /*const*/ GUID SVpatAcceptThresholdObjectGuid;
extern /*const*/ GUID SVpatCertainThresholdObjectGuid;
extern /*const*/ GUID SVpatAccuracyObjectGuid;
extern /*const*/ GUID SVpatMaxOccurancesObjectGuid;
extern /*const*/ GUID SVpatSearchAngleModeObjectGuid;
extern /*const*/ GUID SVpatSpeedObjectGuid;
extern /*const*/ GUID SVpatModelImageFileGuid;
extern /*const*/ GUID SVpatSearchAngleObjectGuid;
extern /*const*/ GUID SVpatAngleDeltaNegObjectGuid;
extern /*const*/ GUID SVpatAngleDeltaPosObjectGuid;
extern /*const*/ GUID SVpatAngleToleranceObjectGuid;
extern /*const*/ GUID SVpatAngleAccuracyObjectGuid;
extern /*const*/ GUID SVpatAngleInterpolationObjectGuid;

extern /*const*/ GUID SVpatCircularOverscanObjectGuid;

extern /*const*/ GUID SVpatPreliminaryAcceptanceThresholdObjectGuid;
extern /*const*/ GUID SVpatFastFindObjectGuid;
extern /*const*/ GUID SVpatModelStepObjectGuid;
extern /*const*/ GUID SVpatBeginningResolutionLevelObjectGuid;
extern /*const*/ GUID SVpatFinalResolutionLevelObjectGuid;
extern /*const*/ GUID SVpatAdditionalCandidatesObjectGuid;
extern /*const*/ GUID SVpatCandidateSpacingXMinObjectGuid;
extern /*const*/ GUID SVpatCandidateSpacingYMinObjectGuid;

extern /*const*/ GUID SVpatResultMatchScoreObjectGuid;
extern /*const*/ GUID SVpatResultXObjectGuid;
extern /*const*/ GUID SVpatResultYObjectGuid;
extern /*const*/ GUID SVpatResultAngleObjectGuid;

extern /*const*/ GUID SVpatResultNumFoundOccurancesObjectGuid;

// GUIDs for Angular Profile Tool
extern /*const*/ GUID SVAngularProfileToolClassGuid;

// GUIDs for Linear   Tool 
extern /*const*/ GUID SVLinearToolClassGuid;

// GUIDs for the Embeddeds of the SVOCRAnalyzeResultClass
extern /*const*/ GUID SVOCRMatchLineNumberGuid;

// GUIDs for Color Tool
extern /*const*/ GUID SVColorToolClassGuid;

// GUIDs for the Embeddeds of the SVColorToolClass
extern /*const*/ GUID SVBand0ImageObjectGuid;
extern /*const*/ GUID SVBand1ImageObjectGuid;
extern /*const*/ GUID SVBand2ImageObjectGuid;
extern /*const*/ GUID SVConvertToHSIObjectGuid;

// GUIDs for Color Threshold
extern /*const*/ GUID SVColorThresholdClassGuid;

// GUIDs for the Embeddeds of the SVColorThresholdClass
extern /*const*/ GUID SVBand0UpperThresholdObjectGuid;
extern /*const*/ GUID SVBand0LowerThresholdObjectGuid;
extern /*const*/ GUID SVBand0ThresholdExcludeObjectGuid;
extern /*const*/ GUID SVBand1UpperThresholdObjectGuid;
extern /*const*/ GUID SVBand1LowerThresholdObjectGuid;
extern /*const*/ GUID SVBand1ThresholdExcludeObjectGuid;
extern /*const*/ GUID SVBand2UpperThresholdObjectGuid;
extern /*const*/ GUID SVBand2LowerThresholdObjectGuid;
extern /*const*/ GUID SVBand2ThresholdExcludeObjectGuid;

// GUIDs for the Embeddeds of the SVLinearEdgeProcessing
extern /*const*/ GUID SVEdgeUpperThresholdValueGuid;

// GUIDs for the Embeddeds of the SVOCRGrayParamStruct
extern /*const*/ GUID SVEdgeADirectionGuid;
extern /*const*/ GUID SVEdgeAPolarisationGuid;
extern /*const*/ GUID SVEdgeAEdgeSelectGuid;
extern /*const*/ GUID SVEdgeAEdgeSelectThisValueGuid;
extern /*const*/ GUID SVEdgeAIsFixedEdgeMarkerGuid;
extern /*const*/ GUID SVEdgeAPositionGuid;
extern /*const*/ GUID SVEdgeAPositionOffsetGuid;
extern /*const*/ GUID SVEdgeALowerThresholdValueGuid;
extern /*const*/ GUID SVEdgeAUpperThresholdValueGuid;

// GUIDs for the Embeddeds of the SVOCRGrayParamStruct
extern /*const*/ GUID SVEdgeBDirectionGuid;
extern /*const*/ GUID SVEdgeBPolarisationGuid;
extern /*const*/ GUID SVEdgeBEdgeSelectGuid;
extern /*const*/ GUID SVEdgeBEdgeSelectThisValueGuid;
extern /*const*/ GUID SVEdgeBIsFixedEdgeMarkerGuid;
extern /*const*/ GUID SVEdgeBPositionGuid;
extern /*const*/ GUID SVEdgeBPositionOffsetGuid;
extern /*const*/ GUID SVEdgeBLowerThresholdValueGuid;
extern /*const*/ GUID SVEdgeBUpperThresholdValueGuid;
extern /*const*/ GUID SVDrawCriteriaObjectGuid;
extern /*const*/ GUID SVResetInspectionCountsGuid;
extern /*const*/ GUID SVTriggerCountGuid;

extern /*const*/ GUID aSVVariantInputObjectGuid[];
extern /*const*/ GUID aSVVariantInputObjectNameGuid[];
extern /*const*/ GUID aSVVariantResultImageObjectGuid[];
extern /*const*/ GUID aSVVariantResultObjectGuid[];
extern /*const*/ GUID aSVVariantResultObjectNameGuid[];
extern /*const*/ GUID aSVDllDependencyFileNameGuid[];
extern /*const*/ GUID SVDllFileNameGuid;
extern /*const*/ GUID SVDllToolNameGuid;
extern /*const*/ GUID SVDllToolVersionGuid;

extern /*const*/ GUID SVLinearDataClassGuid;
extern /*const*/ GUID SVLinearAnalyzerClassGuid;

// Show All Overlays
extern /*const*/ GUID SVShowAllEdgeAOverlaysGuid;
extern /*const*/ GUID SVShowAllEdgeBOverlaysGuid;

// Source Image Names GUID
extern /*const*/ GUID SVSourceImageNamesGuid ;

// PLC Support GUIDs 
extern /*const*/ GUID SVPLCOutputObjectGUID ;
extern /*const*/ GUID SVPLCDataControllerGUID ;

extern /*const*/ GUID SVBCRawDataGuid;
extern /*const*/ GUID SVBCStringFormatGuid;
extern /*const*/ GUID SVBCThresholdTypeGuid;

extern /*const*/ GUID SVRemoteInputToolGuid;
extern /*const*/ GUID SVRemoteInputToolMatchedValueIdGuid;
extern /*const*/ GUID SVRemoteInputToolMatchedValueGuid;

extern /*const*/ GUID SVRemoteOutputObjectGUID;
extern /*static*/GUID SVRemoteOutputGroupGUID;
extern /*const*/ GUID SVRemoteOutputDataControllerGUID;

extern /*const*/ GUID SVToolCommentTypeObjectGuid;

extern /*const*/ GUID SVCameraTriggerTimestampGuid;
extern /*const*/ GUID SVCameraTriggerLineInStateGuid;
extern /*const*/ GUID SVStringHeaderGuid;
// GUIDs for ToolSizeAdjustTask ...
extern /*const*/ GUID ToolSizeAdjustTaskGuid;
extern  /*const*/ GUID  ToolSizeAdjustSizeWidthModeGuid;		
extern  /*const*/ GUID  ToolSizeAdjustSizeHeightModeGuid;
extern  /*const*/ GUID  ToolSizeAdjustSizePositionXModeGuid;
extern  /*const*/ GUID  ToolSizeAdjustSizePositionYModeGuid;
extern /*const*/ GUID EQAdjustSizeWidthGuid;						
extern	/*const*/ GUID  EQAdjustSizeWidthResultGuid;	
extern	/*const*/ GUID  EQAdjustSizeHeightGuid;					
extern	/*const*/ GUID EQAdjustSizeHeightResultGuid;	
extern	/*const*/ GUID EQAdjustSizePositionXGuid;						
extern	/*const*/ GUID EQAdjustSizePositionXResultGuid;	
extern/*const*/ GUID EQAdjustSizePositionYGuid;						
extern	/*const*/ GUID EQAdjustSizePositionYResultGuid;	



////////////////////////////////////////////////////////////////////////////////
// Insert new GUIDs immediately before the previous line.
////////////////////////////////////////////////////////////////////////////////

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVClsids.h_v  $
 * 
 *    Rev 1.6   19 Dec 2014 14:05:40   mEichengruen
 * Project:  SVObserver
 * Change Request (SCR) nbr:  979
 * SCR Title:  Provide additional options to input the feature range for the blob analyzer.
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   new classids for indirect values in Rangeclass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   12 Nov 2014 07:04:28   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  938
 * SCR Title:  Add Black Blob Mode to Blob Analyzer (SVO-336)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   add SVBlobIsBlackGuid
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   24 Oct 2014 11:31:34   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  942
 * SCR Title:  Create new Custom2 Filter SVO-324 SVO-67 SVO-74
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added Custom2 Filter CLSIDs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   17 Mar 2014 15:17:22   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  869
 * SCR Title:  Add PPQ and Environment Variables to Object Manager and Update Pickers
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added Root and Environment objects GUID for the Object Manager.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   27 Jan 2014 15:33:58   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  883
 * SCR Title:  Clip-mode for LUT formula mode
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Added SVLUTEquationClipFlagObjectGuid.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   11 Nov 2013 07:10:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  872
 * SCR Title:  Add Archive Tool Headers to Archive File
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add new Guids for StaticStringValueObject
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 09:53:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.83   27 Mar 2013 12:40:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  813
 * SCR Title:  Remove unused Class IDs from the SVCLSIDs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed un-used CLSIDs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.82   21 Jan 2013 10:30:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added guids for camera timestamp and line input state
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.81   10 Jan 2013 15:02:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Merged 6.01 branch code into main development code base.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.80   02 Jan 2013 15:43:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   new GUIDs for Shift Tool Mode
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.79   07 Dec 2012 10:34:44   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added new id's for the Shift tool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.78   01 Nov 2012 14:19:10   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  798
 * SCR Title:  Change Transformation Tool to have an option for translation only
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new GUID SVTranlationModeGuid
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.77   04 Sep 2012 14:54:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated class Id definitions to make the image definition the same as the value definition for the external tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.76.1.3   08 Jan 2013 16:42:24   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   new GUID for Shift Tool Enable Source Image Extents
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.76.1.2   02 Jan 2013 15:12:14   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added new GUIDs for Shift Tool class
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.76.1.1   05 Dec 2012 11:51:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added new id's for the Shift tool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.76.1.0   01 Nov 2012 13:36:24   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  798
 * SCR Title:  Change Transformation Tool to have an option for translation only
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new GUID SVTranlationModeGuid
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.76   05 Jul 2012 14:12:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  764
 * SCR Title:  Add tool comments to all tools in SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   New dialogs to allow tool comment editing
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.75   29 Jun 2012 15:03:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  771
 * SCR Title:  Upgrade to the Next Version of Matrox Imaging Library (MIL)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated resource file to add the new barcode parameter to fix an issues with MIL 9 processing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.74   18 Jun 2012 18:06:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Centralized the location of the configuration object identifier and removed synchronization problems.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.73   30 Jan 2012 11:09:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Modified source code to consolidate PLC and New Output stream functionality to I/O Controller.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.72   16 Jan 2012 15:13:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Created object Ids and added the remote command factory to SVObserver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.71   08 Dec 2010 07:45:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.70   28 May 2010 14:30:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  694
 * SCR Title:  Upgrade SVObserver version for Chromis release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with includes and make them consistant.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.69   31 Dec 2008 11:19:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  644
 * SCR Title:  Pattern Analyzer Enhancements and Bug Fixes
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Guids for new Pattern Analyzer embedded objects
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.68   10 Dec 2008 20:00:20   jhanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  629
 * SCR Title:  Add Histogram analyzer
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Additional GUIDs for Histogram Analyzer
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.67   14 Nov 2008 13:43:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   New IDs for PLC Data Classes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.66   21 Oct 2008 14:37:54   jhanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  629
 * SCR Title:  Add Histogram analyzer
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Added Guids for Histogram Analyzer
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.65   10 Sep 2008 16:33:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised include for SVGuid to include path
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.64   03 Jun 2008 14:24:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  627
 * SCR Title:  Add RawData object to the Barcode Analyzer and format read string
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Threshold Type Radio to Dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.63   30 May 2008 14:42:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  627
 * SCR Title:  Add RawData object to the Barcode Analyzer and format read string
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Resources for Bar Code Adaptive Threshold
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.62   29 May 2008 09:54:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  627
 * SCR Title:  Add RawData object to the Barcode Analyzer and format read string
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   added new GUIDs to support Raw Data and String Format for barcode.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.61   06 Mar 2008 15:09:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Resources to support Source Image Names for Data Definition lists
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.60   31 Mar 2006 09:19:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  554
 * SCR Title:  Add Shape Mask capability
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added SVShapeMaskAutoResizeGuid
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.59   03 Oct 2005 10:04:44   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  498
 * SCR Title:  Add individual timeout capability to Bar Code analyzers
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   new GUID's for barcode timeout
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.58   01 Sep 2005 12:02:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   New CLSIDs for Auxilliary Extents
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.57   17 Aug 2005 10:29:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   moved operator < to SVGuid.h in SVObjectLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.56   10 Aug 2005 07:37:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  487
 * SCR Title:  Add new blob feature for source extents of blob box center
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   New GUIDs for Transformation Tool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.55   27 Jul 2005 16:02:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added SVStatVariableNameToMonitorObjectGuid
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.54   27 Jul 2005 08:42:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  501
 * SCR Title:  Expose Cylindrical Warp and Perspective Warp Tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   ID for Warp Angle
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.53   19 Jul 2005 14:28:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  469
 * SCR Title:  Add array capability to the Blob Analyzer
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added SVMaxBlobDataArraySizeObjectGuid
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.52   14 Jul 2005 12:28:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added SVArchiveResultNamesObjectGuid and SVArchiveImageNamesObjectGuid
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.51   29 Jun 2005 16:03:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added SVMinToolSetTimeGuid and SVMaxToolSetTimeGuid
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.50   20 May 2005 12:22:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added SVArchiveMethodGuid
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.49   20 May 2005 08:01:24   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  487
 * SCR Title:  Add new blob feature for source extents of blob box center
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new GUIDs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.48   19 May 2005 14:50:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added Shape Mask guids
 * added operator <
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.47   19 May 2005 07:28:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  486
 * SCR Title:  Add Trigger Count to Tool Set
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add trigger count value object to tool set.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.46   18 May 2005 14:54:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  485
 * SCR Title:  Add a way to Reset Counts remotely
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Resources to support Remote Reset Counts.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.45   18 May 2005 13:58:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  484
 * SCR Title:  Display Mask on Source Image
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Resources to add draw criteria for mask overlay on source image.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.44   09 Mar 2005 13:20:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  478
 * SCR Title:  Add a new Perspective Warp Tool
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   New GUIDS for the New Perspective Warp Tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.43   09 Mar 2005 12:22:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  477
 * SCR Title:  Add Cylindrical warp Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added CylindricalWarp and WarpType
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.42   24 Feb 2005 10:23:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  449
 * SCR Title:  Modify Polar Un-Wrap Tool
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Made changes to use a Angular Method combo instead of a use new method check box.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.41   18 Feb 2005 11:14:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  463
 * SCR Title:  Add new Linear Measurement Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added missed ID.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.40   18 Feb 2005 10:58:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  463
 * SCR Title:  Add new Linear Measurement Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added new class ids for new linear tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.39   18 Feb 2005 09:52:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  449
 * SCR Title:  Modify Polar Un-Wrap Tool
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   New Class IDs to support UseNewAngularMethod in Polar Unwrap Tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.38   17 Feb 2005 13:23:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.37   08 Sep 2004 11:17:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  444
 * SCR Title:  Change Bar Code Analyzer Pass / Fail Result
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Guids for SVBCMatchScore and SVBCWarnOnFail
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.36   07 Sep 2004 09:50:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  446
 * SCR Title:  Add new filters to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Guids for new Filters Watershed, Skeleton, Thinning, and Thickening and supporting Value Objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.35   12 Jul 2004 10:16:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added guids for External Tool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.34   07 Jul 2004 08:45:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  416
 * SCR Title:  Add Variant Value Object
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added SVVariantValueObjectClassGuid
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.33   04 Nov 2003 14:38:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  394
 * SCR Title:  Implement new RankingFilter for WindowTool that will also be remotely settable
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new ranking filter class that has a 1-7x1-7 customizable neighborhood.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.32   04 Nov 2003 14:11:40   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  378
 * SCR Title:  Implement new CustomFilter for WindowTools that will also be remotely settable
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Update the custom filter class to be 1-7x1-7 customizable kernel instead of a static 3x3 one.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.31   29 Oct 2003 14:05:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  392
 * SCR Title:  Blob Analyzer have option for filling in the blobs
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new class id's for new value objects in the blob analyzer for FillBlob, BlobType, and BlobColor
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.30   08 Oct 2003 10:51:54   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed edge A and B user flags
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.29   17 Sep 2003 12:46:00   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  384
 * SCR Title:  Add new Matrox OCR/OCV analyzer to SVObserver
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Created and integrated a new Matrox based OCR/OCV analyzer to SVObserver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.28   07 Aug 2003 11:59:26   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  380
 * SCR Title:  Add Image Mask option
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new GUID for the new Mask type
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.27   08 Jul 2003 10:16:00   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  378
 * SCR Title:  Implement new CustomFilter for WindowTools that will also be remotely settable
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new code and classes for the new custom filters objects and dialogs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.26   17 Apr 2003 17:07:50   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.25   10 Apr 2003 10:12:42   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  271
 * SCR Title:  Adding timer features for Tool Execution times.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new GUID's - SVInt64ValueObjectClassGuid, SVTimerValueObjectClassGuid, SVToolTimeGuid
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.24   29 Jan 2003 11:49:46   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new GUIDs for new classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.23   19 Nov 2002 11:00:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added GUIDs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.22   08 Nov 2001 12:48:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  255
 * SCR Title:  Add Sub-pixel resolution to Profile and Line Gage Tool Analyzers
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new class IDs for new sub-pixel classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.21   07 Aug 2001 16:01:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  209
 * SCR Title:  Permit multiple match strings in the Grayscale OCR match string file
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added SVOCRGrayMatchLineNumberGuid to class id file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.21   07 Aug 2001 16:01:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  209
 * SCR Title:  Permit multiple match strings in the Grayscale OCR match string file
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added SVOCRGrayMatchLineNumberGuid to class id file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   12 Jun 2001 12:33:42   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Additional GUIDs added.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   19 Apr 2001 13:55:02   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Changes due to addition of the SVFileNameObjectClass and new GUID for the class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   06 Sep 2000 16:26:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  179
 * SCR Title:  Create Color SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to include color SVIM changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   18 Jul 2000 15:48:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  169
 * SCR Title:  Add Match Score Information to OCR Tools
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   OCR Match Score Updates
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   16 May 2000 14:41:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  133, 132
 * SCR Title:  Add OCR Gray Scale to SVObserver and Create OCR Gray Scale Font Training
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Upgrade Logical Vision WiT v5.3 to Logical Vision FastOCR v1.2.  Completely changed headers, libraries, and variable name types.  Functionality still the same as prior version.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   27 Apr 2000 08:41:52   Jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  130
 * SCR Title:  OCR Tool Modification
 * Checked in by:  sMarakkath;  Srithaj Marakkath
 * Change Description:  
 *   Added a new GUID 'SVOCRMatchLineNumberGuid'.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   26 Apr 2000 11:49:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  129
 * SCR Title:  Angular Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added GUID for the SVAngulatProfileToolClassGuid.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   19 Apr 2000 18:33:48   Jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  128
 * SCR Title:  Suport for 3.11 versioning
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   SVBlobXMinAtXMinObjectGuid was changed to 
 * SVBlobYMaxAtXMinObjectGuid.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   14 Apr 2000 09:27:56   sMarakkath
 * Project:  SVObserver
 * Change Request (SCR) nbr:  120
 * SCR Title:  Pattern Analyzer
 * Checked in by:  sMarakkath;  Srithaj Marakkath
 * Change Description:  
 *   Aded new Class Ids for pattern Analyzer objects
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   23 Mar 2000 16:32:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  118
 * SCR Title:  Integration of LUT Operator
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added new ClassIDs and ObjectIDs for:
 * SVInPlaceImageOperator, SVVectorObject,
 * SVLUTOperator, SVLUTEquation.
 * Revised SVUnaryImageOperatorListClassGuid to
 * SVStdImageOperatorListClassGuid.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   02 Mar 2000 16:45:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  112
 * SCR Title:  Expose Extents of Images
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added GUIDs for the Exposed Embedded Extents
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   Mar 01 2000 11:49:02   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  113
 * SCR Title:  Integrate new Polar Unwrap Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added new object types: 
 * SVPolarTransformObjectType, SVPolarTransformationToolObjectType,
 * SVImagePolarTransformObjectType, SVEvaluateCenterXObjectType,
 * SVEvaluateCenterYObjectType, SVEvaluateStartRadiusObjectType,
 * SVEvaluateEndRadiusObjectType, SVEvaluateStartAngleObjectType,
 * SVEvaluateEndAngleObjectType
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   Feb 24 2000 12:02:22   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  111
 * SCR Title:  Integrate Conditional Tool Set/Tool Drawing
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added ClassID for new Enumerate value object.
 * Added ObjectID for Draw flags of tool and tool set.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   Feb 21 2000 11:08:58   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  109
 * SCR Title:  Freeze on Reject for v3.1
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added ClassIds and Object Ids for Freeze Tool, Freeze Set, etc....
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   16 Feb 2000 17:35:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  104
 * SCR Title:  New Build Reference Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Guids for the Transformation Tool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   Feb 16 2000 16:05:24   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  107
 * SCR Title:  Add More Blob Analyzer Features
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added Class ID GUIDS for 26 Blob Analyzer features for v3.1.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   07 Feb 2000 19:15:42   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  93
 * SCR Title:  Integrate Multiple Camera Image Input per IPD
 * Checked in by:  Robert;  Robert Massinger
 * Change Description:  
 *   Added Acquisition Tool GUID.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   04 Feb 2000 09:09:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  94
 * SCR Title:  New Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Guids for Thresholding options on the line analyzers for the profile tool
 * Added GUids for the new SVSingleLineAnalyzer and the SVDualLineAnalyzer Classes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   02 Feb 2000 18:00:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  95
 * SCR Title:  Standard Deviation on Luminance Histogram
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added GUID for SVCalcStdDeviationObject
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   Feb 01 2000 09:46:10   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  90
 * SCR Title:  Save  Operator Tool Move Priviledge in Configuration
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added new GUID id for tool attribute  so the tool selected for operator tool move can be saved as part of the configuration.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 11:10:58   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.44   11 Nov 1999 17:11:52   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  58
 * SCR Title:  Implement Barcode Analyzer
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Implemented the Bar Code analyzer -- Note: the bar code analyzer requires MIL 6.0 or later.  Conditionals are included in the source code to verify the version. If a MIL library previous to 6.0 is used, bar code features are omitted without warning.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.43   28 Oct 1999 18:37:50   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  53
 * SCR Title:  Implement Regression Test Capability in SVObserver
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Implemented Regression Test
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.42   Oct 26 1999 15:04:44   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Limit Archived images to a user selectable number.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.41   Oct 22 1999 14:24:00   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Implement the capability to append to results archive, and include a time stamp for first record archived on each 'run'.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.40   Oct 22 1999 10:10:36   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Fix bugs in archive based on Beta 11 tests.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.39   Oct 20 1999 07:16:36   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changes to add x and y axis sorts.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.38   19 Oct 1999 17:40:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  48
 * SCR Title:  Threshold Page Formula
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added GUIDS for threshold class
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.37   Oct 14 1999 11:29:14   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Image archive changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   Oct 13 1999 14:40:46   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changes required to archive images into .bmp files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   07 Oct 1999 16:13:12   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  39
 * SCR Title:  Integrate Statistics Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added GUID for saving VariableToMonitor GUID in
 * SVStatisticsToolClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   06 Oct 1999 15:41:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  39
 * SCR Title:  Integrate Statistics Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed GUID for SVStatTotalExecutionObjectGuid
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   05 Oct 1999 10:14:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  39
 * SCR Title:  Integrate Statistics Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Add Statistics Tool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   05 Oct 1999 08:49:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added SVSortAscendingObjectGuid
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   Sep 30 1999 13:44:12   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  41
 * SCR Title:  Version 3.00 Beta 5 Versioning.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changes required to script edge control parameters.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   29 Sep 1999 10:34:24   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  40
 * SCR Title:  Port Round Window Tool over from custom version 2.42.
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Added  SVRoundWindowToolClassGuid	definition for Round Window Tool.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   27 Sep 1999 16:33:22   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added "Number of Blobs Found" as a value object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   24 Sep 1999 13:46:46   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added GUID SVSortFeatureObjectGuid for Blob Analyzer.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   Sep 24 1999 09:43:56   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  4
 * SCR Title:  Add "Load image" option
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added ClassIDS for Load Image Tool.
 * Added EmbeddedIDS for LoadImage Tool.
 * Added EmbeddedIDS for Counter.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   Sep 23 1999 09:11:36   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  35
 * SCR Title:  PPQ mode to wait for input data
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added Embedded ID for explicitFailed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   Sep 22 1999 18:16:34   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added code to implement the SVDoubleValueObject for the auto thresholding value for the Threshold operator.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   15 Sep 1999 22:09:30   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added  SVNbrOfBlobsObjectGuid for persistant storage within SVBlobAnalyzerClass.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   15 Sep 1999 17:09:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added GUID for SVUserMaskOperatorClass Embedded
 * Objects
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   15 Sep 1999 16:15:40   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added SVBlobEnabledFeaturesObjectGuid so that the enabled blob features would be stored during scripting.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   Sep 14 1999 16:21:58   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added ClassIDs for Math Container, Evaluate X, Evaluate Y, EvaluateBeta, Rotate Reference.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   14 Sep 1999 16:11:44   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added GUIDs for blob analyzer features.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   Sep 13 1999 13:19:06   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added ClassID for SVImageTook, SVImageArithmetic.
 * Added Embedded ID for Image Tool Outputs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   Sep 10 1999 17:01:00   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added ClassID Profile Tool.
 * Added Embedded ID Project Angle.
 * Added ClassID ImageToLineProject.
 * Added ClassID BuildReferenceTool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   02 Sep 1999 07:58:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added SVColorObjectGuid and SVStatusObjectGuid
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   Aug 31 1999 18:14:42   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added OCR parameters string object for object persistance via scripting.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   31 Aug 1999 10:54:28   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert;  Robert Massinger
 * Change Description:  
 *   Added EmbeddedIDs for
 * Black Pixel, White Pixel, Edge Count Objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   31 Aug 1999 10:23:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added SVDoubleResult
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   Aug 30 1999 10:04:20   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added GUIDs for OCR embedded values.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   29 Aug 1999 20:15:18   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  14
 * SCR Title:  Integrate luminance analyzer into window tool.
 * Checked in by:  Jim;  James A. Brown
 * Change Description:  
 *   Removed GUID SVLuminanceAnalyzeResultClassGuid;
 * 
 * SVLuminanceAnalyzerResultClass has been replaced by more generic SVLongResultClass.
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   29 Aug 1999 13:22:32   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert;  Robert Massinger
 * Change Description:  
 *   Added ClassIDs for
 * SVMaximumForegroundObjectLineAnalyzerClass
 * SVMaximumBackgroundObjectLineAnalyzerClass
 * SVMaximumObjectLineAnalyzerClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   29 Aug 1999 12:44:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  1
 * SCR Title:  Integrate a common validity check for all tool base class inputs and outputs.
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added generic passed, failed, warned Object GUIDs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   Aug 28 1999 10:09:34   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   No change detected.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   27 Aug 1999 12:17:52   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert Massinger
 * Change Description:  
 *   Added ClassIDs for PointX Result, PointY Result, Long Result,
 * Added EmbeddedIDs for Linear Measurement Analyzer and his Results.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   26 Aug 1999 14:22:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  Steve Jones
 * Change Description:  
 *   Added Guids and ObjectTypes/SubTypes for the SVConditionalClass and SVMathEquationClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   Aug 25 1999 16:26:32   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  27
 * SCR Title:  Results Picker
 * Checked in by:  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added GUID for SVStringValueObjectClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   25 Aug 1999 11:39:30   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  Steve Jones
 * Change Description:  
 *     Upgrading White Pixel Analyzer to new i/o object interface.  
 * 
 * Changing White Pixel Analyzer to more generic Pixel Analyzer.
 * 
 * 
 * 
 * Changed 
 * SVWhitePixelAnalyzerClassGuid to SVPixelAnalyzerClassGuid
 * and
 * SVWhitePixelAnalyzeResultClassGuid to 
 * SVWhiteAnalyzeResultClassGuid
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   23 Aug 1999 18:59:56   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert Massinger
 * Change Description:  
 *   Added ClassIDs for SVEdgeCountingLineAnalyzerClass, SVEdgePositionLineAnalyzerClass, SVLinearMeasurementLineAnalyzerClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   23 Aug 1999 15:51:48   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert Massinger
 * Change Description:  
 *   Added ClassID for SVLineAnalyzer.
 * Changed Classname of SVLinePixelCountingAnalyzerClass to SVPixelCountingLineAnalyzerClass.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Aug 1999 15:26:20   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert Massinger
 * Change Description:  
 *   Added ClassID for SVROIClass, SVLineROIClass, SVImageROIClass, SVLineClass
 * Added EmbeddedID for SVLineClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   Aug 21 1999 15:48:16   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert Massinger
 * Change Description:  
 *   Added ClassIDs for SVGageToolClass and SVLinePixelCountingAnalyzerClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Aug 17 1999 08:28:18   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  Steve Jones
 * Change Description:  
 *   Define new GUIDs for new "objects".  Group all GUIDs together,
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
