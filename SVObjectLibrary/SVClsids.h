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

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <guiddef.h>
#pragma endregion Includes

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

// GUID for the DoubleSortValueObject
extern /*const*/ GUID DoubleSortValueObjectGuid;

// GUID for the SVLongValueObjectClass
extern /*const*/ GUID SVLongValueObjectClassGuid;

// GUID for the SVPointValueObjectClass
extern /*const*/ GUID SVPointValueObjectClassGuid;

// GUID for the SVDPointValueObjectClass
extern /*const*/ GUID SVDPointValueObjectClassGuid;

// GUID for the SVByteValueObjectClass
extern /*const*/ GUID SVByteValueObjectClassGuid;

// GUID for the SVStringValueObjectClass
extern /*const*/ GUID SVStringValueObjectClassGuid;

// GUID for the SVStringValueObjectClass
extern /*const*/ GUID SVStaticStringValueObjectClassGuid;

// GUID for the SVFileNameValueObjectClass
extern /*const*/ GUID SVFileNameValueObjectClassGuid;

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

//GUID for the Embedded object for SVToolsetClass
extern /*const*/ GUID SVMinToolSetTimeGuid;
extern /*const*/ GUID SVMaxToolSetTimeGuid;
extern /*const*/ GUID SVResetInspectionCountsGuid;
extern /*const*/ GUID SVTriggerCountGuid;
extern /*const*/ GUID SVLatestCompletionPPQIndexGuid;
extern /*const*/ GUID SVTriggerDeltaGuid;
extern /*const*/ GUID SVLastTriggerToPPQCompletionGuid;
extern /*const*/ GUID SVLastTriggerToStartGuid;
extern /*const*/ GUID ToolsetWidthGuid;
extern /*const*/ GUID ToolsetHeightGuid;

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
extern /*const*/ GUID RingBuffer_IndexGuid[];
extern /*const*/ GUID RingBufferLink_IndexGuid[];
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

// GUIDs for the Embedded Objects in the SVOCVAnalyzeResultClass
extern /*const*/ GUID SVOCVAnalyzerClassGuid;
extern /*const*/ GUID SVOCVAnalyzerResultClassGuid;
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

extern /*const*/ GUID SVEdgeParameters1ObjectGuid;
extern /*conts*/ GUID SVEdgeParameters2ObjectGuid;
extern /*conts*/ GUID SVEdgeParameters3ObjectGuid;
extern /*conts*/ GUID SVEdgeParameters4ObjectGuid;
extern /*conts*/ GUID SVEdgeParameters5ObjectGuid;
extern /*conts*/ GUID SVEdgeParameters6ObjectGuid;
extern /*conts*/ GUID SVEdgeParameters7ObjectGuid;
extern /*conts*/ GUID SVEdgeParameters8ObjectGuid;
extern /*conts*/ GUID SVEdgeParameters9ObjectGuid;
extern /*conts*/ GUID SVEdgeParameters10ObjectGuid;

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

//GUID for the SVLinearPixelCountingLineAnalyzerClass
extern /*const*/ GUID SVLinearPixelCountingLineAnalyzerClassGuid;

// GUID for the SVLinearEdgeCountingLineAnalyzerClass
extern /*const*/ GUID SVLinearEdgeCountingLineAnalyzerClassGuid;

// GUID for the SVLinearEdgePositionLineAnalyzerClass
extern /*const*/ GUID SVLinearEdgePositionLineAnalyzerClassGuid;

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

// GUID for the SVLinearMaximumBackgroundObjectLineAnalzerClass
extern /*const*/ GUID SVLinearMaximumBackgroundObjectLineAnalyzerClassGuid;

// GUID for the SVLinearMaximumForegroundObjectLineAnalyzerClass
extern /*const*/ GUID SVLinearMaximumForegroundObjectLineAnalyzerClassGuid;

// GUID for the SVLinearMaximumObjectLineAnalyzerClass
extern /*const*/ GUID SVLinearMaximumObjectLineAnalyzerClassGuid;

// GUIDs for the Embedded Objects
extern /*const*/ GUID SVBlackPixelObjectGuid;
extern /*const*/ GUID SVWhitePixelObjectGuid;
extern /*const*/ GUID SVEdgeCountObjectGuid;

// GUIDs for the Embedded Objects
extern /*const*/ GUID SVColorObjectGuid;
extern /*const*/ GUID SVStatusObjectGuid;

// GUID for the SVLinearImageToLineProjectClassGuid
extern /*const*/ GUID SVLinearImageToLineProjectClassGuid;

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
extern/*const*/ GUID SVThickeningFilterClassGuid;
extern/*const*/ GUID SVThickeningFilterItterationsGuid;
extern/*const*/ GUID SVThickeningFilterGrayOnGuid;
extern/*const*/ GUID SVThinningFilterClassGuid;
extern/*const*/ GUID SVThinningFilterItterationsGuid;
extern/*const*/ GUID SVThinningFilterGrayOnGuid;
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

extern /*const*/ GUID SVLinearThresholdMaxObjectGuid;
extern /*const*/ GUID SVLinearThresholdMinObjectGuid;

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
extern /*const*/ GUID SVExtentHeightScaleFactorObjectGuid;
extern /*const*/ GUID SVExtentWidthScaleFactorObjectGuid;

// GUIDs for Exposed Source Image Extents (primarily Images/Tools)
extern /*const*/ GUID SVUpdateAuxiliaryExtentsObjectGuid;
extern /*const*/ GUID SVAuxiliarySourceXObjectGuid;
extern /*const*/ GUID SVAuxiliarySourceYObjectGuid;
extern /*const*/ GUID SVAuxiliarySourceAngleObjectGuid;
extern /*const*/ GUID SVAuxiliarySourceImageNameObjectGuid;
extern /*const*/ GUID SVAuxiliaryDrawTypeObjectGuid;

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


///////////////////////////////////////////////////
// GUIDs for the ResizeTool
///////////////////////////////////////////////////
extern /*const*/ GUID SVResizeToolGuid;
extern /*const*/ GUID SVResizeInterpolationModeGuid;
extern /*const*/ GUID SVResizeOverscanGuid;
extern /*const*/ GUID SVResizePerformanceGuid;
extern /*const*/ GUID SVLogicalROIImageGuid;

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
extern /*const*/ GUID  ToolSizeAdjustSizeWidthModeGuid;		
extern /*const*/ GUID  ToolSizeAdjustSizeHeightModeGuid;
extern /*const*/ GUID  ToolSizeAdjustSizePositionXModeGuid;
extern /*const*/ GUID  ToolSizeAdjustSizePositionYModeGuid;
extern /*const*/ GUID EQAdjustSizeWidthGuid;						
extern /*const*/ GUID  EQAdjustSizeWidthResultGuid;	
extern /*const*/ GUID  EQAdjustSizeHeightGuid;					
extern /*const*/ GUID EQAdjustSizeHeightResultGuid;	
extern /*const*/ GUID EQAdjustSizePositionXGuid;						
extern /*const*/ GUID EQAdjustSizePositionXResultGuid;	
extern /*const*/ GUID EQAdjustSizePositionYGuid;						
extern /*const*/ GUID EQAdjustSizePositionYResultGuid;	

///////////////////////////////////////////////////
// GUIDs for the TableTool
///////////////////////////////////////////////////
extern /*const*/ GUID TableToolGuid;
extern /*const*/ GUID TableTool_MaxRowGuid;
extern /*const*/ GUID TableColumnEquationGuid;
extern /*const*/ GUID TableObjectGuid;
extern /*const*/ GUID TableFillObjectGuid;
extern /*const*/ GUID TableObject_NumberOfRowsGuid;
const int c_maxTableColumn = 50;
extern /*const*/ GUID TableColumnValueObjectGuid[c_maxTableColumn];
// Unique ID GUID for Global variables
extern /*const*/ GUID RootUidGuid;
extern /*const*/ GUID GlobalUidGuid;
// Unique ID GUID for Environment variables
extern /*const*/ GUID EnvironmentUidGuid;
extern /*const*/ GUID EnvironmentModelNumberUidGuid;
extern /*const*/ GUID EnvironmentWinKeyUidGuid;
extern /*const*/ GUID EnvironmentImageUpdateUidGuid;
extern /*const*/ GUID EnvironmentResultUpdateUidGuid;
extern /*const*/ GUID EnvironmentModeIsRunUidGuid;
extern /*const*/ GUID EnvironmentModeIsStopUidGuid;
extern /*const*/ GUID EnvironmentModeIsRegressionTestUidGuid;
extern /*const*/ GUID EnvironmentModeIsTestUidGuid;
extern /*const*/ GUID EnvironmentModeIsEditUidGuid;
extern /*const*/ GUID EnvironmentModeValueUidGuid;
// Unique base ID GUID for Camera variables always have 0x100 GUIDS free after the base GUID
extern /*const*/ GUID CameraBaseSerialNumberUidGuid;
extern /*const*/ GUID CameraBaseGainUidGuid;
extern /*const*/ GUID CameraBaseShutterUidGuid;
// Unique base ID GUID for PPQ variables always have 0x100 GUIDS free after the base GUID
extern /*const*/ GUID PpqBaseLengthUidGuid;


///////////////////////////////////////////////////
// GUIDs for the TableAnalyzerTool
///////////////////////////////////////////////////
extern /*const*/ GUID TableAnalyzerToolGuid;
extern /*const*/ GUID TableCopyObjectGuid;

extern /*const*/ GUID TableAnaylzerSortGuid;
extern /*const*/ GUID TableAnaylzerExcludeGuid;
extern /*const*/ GUID TableAnaylzerLimitGuid;
extern /*const*/ GUID TableAnaylzerSortIsASCGuid;
extern /*const*/ GUID TableAnaylzerExcludeHighGuid;
extern /*const*/ GUID TableAnaylzerExcludeHigh_LinkGuid;
extern /*const*/ GUID TableAnaylzerExcludeLowGuid;
extern /*const*/ GUID TableAnaylzerExcludeLow_LinkGuid;
extern /*const*/ GUID TableAnaylzerLimitValueGuid;
extern /*const*/ GUID TableAnaylzerLimitValue_LinkGuid;

////////////////////////////////////////////////////////////////////////////////
// Insert new GUIDs immediately before the previous line.
////////////////////////////////////////////////////////////////////////////////

