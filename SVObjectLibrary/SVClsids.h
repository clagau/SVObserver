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
#include "ObjectInterfaces/IBlobAnalyzer.h"
#pragma endregion Includes

////////////////////////////////////////////////////////////////////////////////
// SVObserver Class IDs (Guids)
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// NOTE: Add new GUIDs at the bottom.
////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************/
//classID
extern const GUID SVStringValueObjectClassGuid;


/*******************************************************************************/
//Embedded

// GUID for the Embedded Objects of the SVTaskObjectClass
extern const GUID SVTaskObjectClassIsObjectValidGuid;

// GUID for the SVToolSetEnabled
extern const GUID SVToolSetEnabledObjectGuid;

// GUID for the Embedded Objects of the SVTooClass
extern const GUID SVToolEnabledObjectGuid;

// GUID for the Embedded Objects of the SVToolClass
extern const GUID SVToolSelectedForOperatorMoveObjectGuid;

//GUID for the Embedded object for SVToolClass
extern const GUID SVToolPositionGuid;
extern const GUID SVToolTimeGuid;

extern const GUID FilterKernelGuid;

//GUID for the Embedded object for SVToolsetClass
extern const GUID SVMinToolSetTimeGuid;
extern const GUID SVMaxToolSetTimeGuid;
extern const GUID SVResetInspectionCountsGuid;
extern const GUID SVTriggerCountGuid;
extern const GUID SVPPQIndexGuid;
extern const GUID SVTriggerDeltaGuid;
extern const GUID SVTriggerToCompletionGuid;
extern const GUID SVTriggerToStartGuid;
extern const GUID SVTriggerToAcquisitionStartGuid;
extern const GUID SVAcquisitionTimeGuid;
extern const GUID ToolsetWidthGuid;
extern const GUID ToolsetHeightGuid;
extern const GUID EnableAuxiliaryExtentsObjectGuid;
extern const GUID ObjectIDGuid;
extern const GUID TriggerIndexGuid;
extern const GUID InspectedObjectIDGuid;
extern const GUID InspectedObjectIDLinkGuid;

// GUIDs for the SVHistogramAnalyzerClass/value objects
extern const GUID SVHistogramValueObjectGuid;
extern const GUID SVHistogramMeanObjectGuid;
extern const GUID SVHistogramStdDevObjectGuid;
extern const GUID SVHistogramRangeStartObjectGuid;
extern const GUID SVHistogramRangeEndObjectGuid;
extern const GUID SVHistogramScaleObjectGuid;
extern const GUID SVHistogramRangeSizeObjectGuid;
extern const GUID SVHistogramMaxPixelObjectGuid;
extern const GUID SVHistogramMinPixelObjectGuid;
extern const GUID SVHistogramPixelCountObjectGuid;
extern const GUID SVHistogramImageObjectGuid;
extern const GUID SVHistogramHighPeakObjectGuid;
extern const GUID SVHistogramLowPeakObjectGuid;
extern const GUID SVHistogramValleyObjectGuid;
extern const GUID SVHistogramPeakThresholdObjectGuid;
extern const GUID SVHistogramDefaultPeakObjectGuid;
extern const GUID SVHistogramAccumulateObjectGuid;
extern const GUID SVHistogramMinPeakHeightObjectGuid;
extern const GUID SVHistogramValleyLowerBoundObjectGuid;
extern const GUID SVHistogramValleyUpperBoundObjectGuid;
extern const GUID SVHistogramValleyDefaultObjectGuid;
extern const GUID SVHistogramFixedHeightValueObjectGuid;
extern const GUID SVHistogramDynamicHeightObjectGuid;

// GUID for the SVLuminanceValueObject
extern const GUID SVLuminanceValueObjectGuid;

// GUID for the SVCalcStdDevObject
extern const GUID SVCalcStdDevObjectGuid;

// GUIDs for the Embedded Objects in the SVWhitePixelAnalyzeResultClass
extern const GUID SVPixelColorIndexObjectGuid;
extern const GUID SVPixelCountObjectGuid;

extern const GUID SVBlobEnabledFeaturesObjectGuid;
extern const GUID SVNbrOfBlobsObjectGuid;
extern const GUID SVMaxBlobDataArraySizeObjectGuid;
extern const GUID SVSortFeatureObjectGuid;
extern const GUID SVNbrOfBlobsFoundObjectGuid;
extern const GUID SVSortAscendingObjectGuid;
extern const GUID SVExcludeFailedObjectGuid;
extern const GUID SVBlobColorGuid;

//new GUIDs for BlobAnalyzer for Use FIll, Fill Color, and Fill Type
extern const GUID SVBlobUseFillGuid;
extern const GUID SVBlobFillColorGuid;
extern const GUID SVBlobFillTypeGuid;

extern const GUID SVBlobFeatureGuids[SvOi::SV_NUMBER_OF_BLOB_FEATURES];

// GUIDs for the Embedded Objects in the SVStatisticsToolClass
extern const GUID SVStatMinObjectGuid;
extern const GUID SVStatMaxObjectGuid;
extern const GUID SVStatAverageObjectGuid;
extern const GUID SVStatStdDevObjectGuid;
extern const GUID SVStatVarianceObjectGuid;
extern const GUID SVStatTestValueObjectGuid;
extern const GUID SVStatNbrOfOccurancesObjectGuid;
extern const GUID SVStatPercentOfOccurancesObjectGuid;
extern const GUID	SVStatEnabledFeaturesObjectGuid;
extern const GUID SVStatVariableToMonitorObjectGuid;
extern const GUID SVStatVariableNameToMonitorObjectGuid;

// GUIDs for the Embedded Objects in the SVThresholdClass
extern const GUID  SVUpperThresholdObjectGuid;
extern const GUID  SVLowerThresholdObjectGuid;
extern const GUID  SVThresholdActivateObjectGuid;
extern const GUID  SVUpperThresholdActivateObjectGuid;
extern const GUID  SVLowerThresholdActivateObjectGuid;
extern const GUID SVAutoThresholdObjectGuid;
extern const GUID SVBlackBackgroundObjectGuid;
extern const GUID SVAutoThresholdMultiplierObjectGuid;

// GUIDs for the Embedded Objects of the SVCustomFilterClass
extern const GUID SVCustomFilterCell01Guid;
extern const GUID SVCustomFilterCell02Guid;
extern const GUID SVCustomFilterCell03Guid;
extern const GUID SVCustomFilterCell04Guid;
extern const GUID SVCustomFilterCell05Guid;
extern const GUID SVCustomFilterCell06Guid;
extern const GUID SVCustomFilterCell07Guid;
extern const GUID SVCustomFilterCell08Guid;
extern const GUID SVCustomFilterCell09Guid;
extern const GUID SVCustomFilterCell10Guid;
extern const GUID SVCustomFilterCell11Guid;
extern const GUID SVCustomFilterCell12Guid;
extern const GUID SVCustomFilterCell13Guid;
extern const GUID SVCustomFilterCell14Guid;
extern const GUID SVCustomFilterCell15Guid;
extern const GUID SVCustomFilterCell16Guid;
extern const GUID SVCustomFilterCell17Guid;
extern const GUID SVCustomFilterCell18Guid;
extern const GUID SVCustomFilterCell19Guid;
extern const GUID SVCustomFilterCell20Guid;
extern const GUID SVCustomFilterCell21Guid;
extern const GUID SVCustomFilterCell22Guid;
extern const GUID SVCustomFilterCell23Guid;
extern const GUID SVCustomFilterCell24Guid;
extern const GUID SVCustomFilterCell25Guid;
extern const GUID SVCustomFilterCell26Guid;
extern const GUID SVCustomFilterCell27Guid;
extern const GUID SVCustomFilterCell28Guid;
extern const GUID SVCustomFilterCell29Guid;
extern const GUID SVCustomFilterCell30Guid;
extern const GUID SVCustomFilterCell31Guid;
extern const GUID SVCustomFilterCell32Guid;
extern const GUID SVCustomFilterCell33Guid;
extern const GUID SVCustomFilterCell34Guid;
extern const GUID SVCustomFilterCell35Guid;
extern const GUID SVCustomFilterCell36Guid;
extern const GUID SVCustomFilterCell37Guid;
extern const GUID SVCustomFilterCell38Guid;
extern const GUID SVCustomFilterCell39Guid;
extern const GUID SVCustomFilterCell40Guid;
extern const GUID SVCustomFilterCell41Guid;
extern const GUID SVCustomFilterCell42Guid;
extern const GUID SVCustomFilterCell43Guid;
extern const GUID SVCustomFilterCell44Guid;
extern const GUID SVCustomFilterCell45Guid;
extern const GUID SVCustomFilterCell46Guid;
extern const GUID SVCustomFilterCell47Guid;
extern const GUID SVCustomFilterCell48Guid;
extern const GUID SVCustomFilterCell49Guid;

extern const GUID SVCustomFilterKernelWidthGuid;
extern const GUID SVCustomFilterKernelHeightGuid;
extern const GUID SVCustomFilterClippingGuid;
extern const GUID SVCustomFilterAbsoluteGuid;
extern const GUID SVCustomFilterTransformGuid;

// GUIDs for the Embedded Objects of the SVRankingFilterClass
extern const GUID SVRankingFilterCell01Guid;
extern const GUID SVRankingFilterCell02Guid;
extern const GUID SVRankingFilterCell03Guid;
extern const GUID SVRankingFilterCell04Guid;
extern const GUID SVRankingFilterCell05Guid;
extern const GUID SVRankingFilterCell06Guid;
extern const GUID SVRankingFilterCell07Guid;
extern const GUID SVRankingFilterCell08Guid;
extern const GUID SVRankingFilterCell09Guid;
extern const GUID SVRankingFilterCell10Guid;
extern const GUID SVRankingFilterCell11Guid;
extern const GUID SVRankingFilterCell12Guid;
extern const GUID SVRankingFilterCell13Guid;
extern const GUID SVRankingFilterCell14Guid;
extern const GUID SVRankingFilterCell15Guid;
extern const GUID SVRankingFilterCell16Guid;
extern const GUID SVRankingFilterCell17Guid;
extern const GUID SVRankingFilterCell18Guid;
extern const GUID SVRankingFilterCell19Guid;
extern const GUID SVRankingFilterCell20Guid;
extern const GUID SVRankingFilterCell21Guid;
extern const GUID SVRankingFilterCell22Guid;
extern const GUID SVRankingFilterCell23Guid;
extern const GUID SVRankingFilterCell24Guid;
extern const GUID SVRankingFilterCell25Guid;
extern const GUID SVRankingFilterCell26Guid;
extern const GUID SVRankingFilterCell27Guid;
extern const GUID SVRankingFilterCell28Guid;
extern const GUID SVRankingFilterCell29Guid;
extern const GUID SVRankingFilterCell30Guid;
extern const GUID SVRankingFilterCell31Guid;
extern const GUID SVRankingFilterCell32Guid;
extern const GUID SVRankingFilterCell33Guid;
extern const GUID SVRankingFilterCell34Guid;
extern const GUID SVRankingFilterCell35Guid;
extern const GUID SVRankingFilterCell36Guid;
extern const GUID SVRankingFilterCell37Guid;
extern const GUID SVRankingFilterCell38Guid;
extern const GUID SVRankingFilterCell39Guid;
extern const GUID SVRankingFilterCell40Guid;
extern const GUID SVRankingFilterCell41Guid;
extern const GUID SVRankingFilterCell42Guid;
extern const GUID SVRankingFilterCell43Guid;
extern const GUID SVRankingFilterCell44Guid;
extern const GUID SVRankingFilterCell45Guid;
extern const GUID SVRankingFilterCell46Guid;
extern const GUID SVRankingFilterCell47Guid;
extern const GUID SVRankingFilterCell48Guid;
extern const GUID SVRankingFilterCell49Guid;

extern const GUID SVRankingFilterRankingWidthGuid;
extern const GUID SVRankingFilterRankingHeightGuid;
extern const GUID SVRankingFilterRankingRankGuid;

// GUIDs for the Embedded Objects in the SVUnaryImageOperatorListClass
extern const GUID SVOutputImageObjectGuid;


///////////////////////////////////////////////////
// GUIDs for the RingBuffer-Tool
///////////////////////////////////////////////////
extern const GUID RingBuffer_DepthGuid;
extern const GUID RingBufferLink_DepthGuid;
extern const GUID RingBuffer_IndexGuid[];
extern const GUID RingBufferLink_IndexGuid[];
extern const GUID RingBuffer_FlagOfOutputImagesGuid;

// GUIDs for the Embedded Objects in the SVRangeClass
extern const GUID SVRangeClassFailHighObjectGuid;
extern const GUID SVRangeClassFailLowObjectGuid;
extern const GUID SVRangeClassWarnHighObjectGuid;
extern const GUID SVRangeClassWarnLowObjectGuid;
extern const GUID SVRangeClassFailHighIndirectObjectGuid;
extern const GUID SVRangeClassFailLowIndirectObjectGuid;
extern const GUID SVRangeClassWarnHighIndirectObjectGuid;
extern const GUID SVRangeClassWarnLowIndirectObjectGuid;

// GUIDs for the Embedded Objects in the SVOCVAnalyzeResultClass
extern const GUID SVOCVPerformOCRGuid;
extern const GUID SVOCVMatchLineNumberGuid;

// GUIDs for the Exposed OCR Results of the SVOCVAnalyzeResultClass
extern const GUID SVOCVHighestMatchScoreGuid;
extern const GUID SVOCVLowestMatchScoreGuid;
extern const GUID SVOCVAverageMatchScoreGuid;

// GUIDs for the Exposed OCR File Names of the SVOCRAnalyzeResultClass
extern const GUID SVOCVFontFileNameGuid;
extern const GUID SVOCVMatchStringFileNameGuid;
extern const GUID SVOCVConstraintsFileNameGuid;
extern const GUID SVOCVControlsFileNameGuid;
extern const GUID SVOCVUseMatchFileGuid;

// GUIDs for the Exposed OCV Results of the SVOCVAnalyzeResultClass
extern const GUID SVOCVMatchStringGuid;
extern const GUID SVOCVFoundStringGuid;
extern const GUID SVOCVCharacterValueGuid;
extern const GUID SVOCVExtentLeftGuid;
extern const GUID SVOCVExtentTopGuid;
extern const GUID SVOCVExtentWidthGuid;
extern const GUID SVOCVExtentHeightGuid;
extern const GUID SVOCVMatchScoreGuid;

extern const GUID SVEdgeParameters1ObjectGuid;
extern const GUID SVEdgeParameters2ObjectGuid;
extern const GUID SVEdgeParameters3ObjectGuid;
extern const GUID SVEdgeParameters4ObjectGuid;
extern const GUID SVEdgeParameters5ObjectGuid;
extern const GUID SVEdgeParameters6ObjectGuid;
extern const GUID SVEdgeParameters7ObjectGuid;
extern const GUID SVEdgeParameters8ObjectGuid;
extern const GUID SVEdgeParameters9ObjectGuid;
extern const GUID SVEdgeParameters10ObjectGuid;

extern const GUID SVArchiveFilePathObjectGuid;
extern const GUID SVArchiveImageGuidsObjectGuid;
extern const GUID SVArchiveImageFileRootPathGuid;
extern const GUID SVArchiveResultGuidsObjectGuid;
extern const GUID SVArchiveAppendArchiveFileGuid;
extern const GUID SVArchiveStopAtMaxImagesGuid;
extern const GUID SVArchiveUseTriggerCountForImagesGuid;
extern const GUID SVArchiveMaxImagesCountGuid;
extern const GUID SVArchiveMethodGuid;
extern const GUID SVArchiveUseHeadersGUID;
extern const GUID SVArchiveHeaderLabelGUID;
extern const GUID SVArchiveHeaderObjectGUID;
extern const GUID SVArchiveResultNamesObjectGuid;
extern const GUID SVArchiveImageNamesObjectGuid;

// GUID for the Embedded Objects in the SVEquationClass
extern const GUID SVEquationEnabledObjectGuid;

// GUID for the Embedded Objects in the SVConditionalClass
extern const GUID SVConditionalResultObjectGuid;

// GUID for the Embedded Objects in the SVMathEquationClass
extern const GUID SVMathEquationResultObjectGuid;

//GUID for the SVLinearEdgesClassGuid
extern const GUID SVLinearEdgesClassGuid;

// GUIDs for the Embedded Objects in the SVLinearMeasurementAnalyzerClass and all related Line Analyzers
extern const GUID SVEdgeAObjectGuid;
extern const GUID SVEdgeBObjectGuid;
extern const GUID SVCenterObjectGuid;
extern const GUID SVWidthObjectGuid;

extern const GUID SVDPEdgeAObjectGuid;
extern const GUID SVDPEdgeBObjectGuid;
extern const GUID SVDPCenterObjectGuid;
extern const GUID SVDWidthObjectGuid;

extern const GUID SVLinearDistanceEdgeAObjectGuid;
extern const GUID SVLinearDistanceEdgeBObjectGuid;

// GUIDs for the Embedded Objects
extern const GUID SVXObjectGuid;
extern const GUID SVDXObjectGuid;
extern const GUID SVYObjectGuid;
extern const GUID SVDYObjectGuid;
extern const GUID SVValueObjectGuid;

// GUIDs for the Embedded Objects that represent the pass/fail/warn status
extern const GUID SVFailedObjectGuid;
extern const GUID SVWarnedObjectGuid;
extern const GUID SVPassedObjectGuid;
extern const GUID SVExplicitFailedObjectGuid;
extern const GUID SVPassedCountObjectGuid;
extern const GUID SVFailedCountObjectGuid;
extern const GUID SVWarnedCountObjectGuid;
extern const GUID SVEnabledCountObjectGuid;
extern const GUID SVProcessedCountObjectGuid;

// GUIDs for the Embedded Objects
extern const GUID SVBlackPixelObjectGuid;
extern const GUID SVWhitePixelObjectGuid;
extern const GUID SVEdgeCountObjectGuid;

// GUIDs for the Embedded Objects
extern const GUID SVColorObjectGuid;
extern const GUID SVStatusObjectGuid;

// GUID for Embedded Object
extern const GUID SVProjectAngleObjectGuid;
extern const GUID SVRotationAngleObjectGuid;
extern const GUID SVRotationPointXObjectGuid;
extern const GUID SVRotationPointYObjectGuid;
extern const GUID SVOutputRotationPointXObjectGuid;
extern const GUID SVOutputRotationPointYObjectGuid;

//GUID for Profile Orientation
extern const GUID SVProfileOrientationGuid;

//GUID for Linear Tool Use Profile Rotation
extern const GUID SVLinearToolUseRotationGuid;

// Perspective Tool TranslationX
extern const GUID SVTranslationXOffsetObjectGuid;

// Perspective Tool TranslationY
extern const GUID SVTranslationYOffsetObjectGuid;

// GUID for the SVWarpType Enum Value Object
extern const GUID SVWarpTypeObjectGuid;

// GUID for SVWarpAngle double value object
extern const GUID SVWarpAngleObjectGuid;

// GUID for Embedded Objects of Image Tool
extern const GUID SVEnableOffsetAObjectGuid;
extern const GUID SVOffsetAPointObjectGuid;
extern const GUID SVEnableOffsetBObjectGuid;
extern const GUID SVOffsetBPointObjectGuid;
extern const GUID SVArithmeticOperatorObjectGuid;

// GUID for Embedded Objects of SVUserMaskOperatorClass
extern const GUID SVMaskEnabledObjectGuid;
extern const GUID SVMaskUseImageMaskGuid;
extern const GUID SVMaskOperatorGuid;
extern const GUID SVMaskFillAreaGuid;
extern const GUID SVMaskFillColorGuid;

extern const GUID SVShapeMaskTypeGuid;
extern const GUID SVShapeMaskMaskAreaGuid;
extern const GUID SVShapeMaskPropertyCenterXGuid;
extern const GUID SVShapeMaskPropertyCenterYGuid;
extern const GUID SVShapeMaskPropertyWidthGuid;
extern const GUID SVShapeMaskPropertyHeightGuid;
extern const GUID SVShapeMaskPropertySideThicknessGuid;
extern const GUID SVShapeMaskPropertyTopBottomThicknessGuid;
extern const GUID SVShapeMaskPropertyOffsetGuid;
extern const GUID SVShapeMaskPropertySymmetryOrientationGuid;
extern const GUID SVShapeMaskAutoResizeGuid;

// GUID for Embedded Objects of SVLoadImageToolClass
extern const GUID SVPathNameObjectGuid;
extern const GUID SVContinuousReloadObjectGuid;

extern const GUID SVBarCodeObjectGuid;
extern const GUID SVRegExpressionObjectGuid;

extern const GUID SVBCTypeObjectGuid;
extern const GUID SVBCOrientationObjectGuid;
extern const GUID SVBCSkewNegativeObjectGuid;
extern const GUID SVBCSkewPositiveObjectGuid;
extern const GUID SVBCCellMinSizeObjectGuid;
extern const GUID SVBCCellMaxSizeObjectGuid;
extern const GUID SVBCCellNumberXObjectGuid;
extern const GUID SVBCCellNumberYObjectGuid;
extern const GUID SVBCSpeedObjectGuid;
extern const GUID SVBCThresholdObjectGuid;
extern const GUID SVBCStringSizeObjectGuid;
extern const GUID SVBCErrorCorrectionObjectGuid;
extern const GUID SVBCEncodingObjectGuid;
extern const GUID SVBCForegroundColorObjectGuid;
extern const GUID SVBCSaveStringInFileObjectGuid;
extern const GUID SVBCStringFileNameObjectGuid;
extern const GUID SVBCUseSingleMatchStringGuid;
extern const GUID SVBCUseMatchStringFileGuid;
extern const GUID SVBCMatchStringFileNameObjectGuid;
extern const GUID SVBCMatchStringLineGuid;
extern const GUID SVBCWarnOnFailedReadObjectGuid;
extern const GUID SVBCMatchScoreGuid;
extern const GUID SVBCTimeoutGuid;
extern const GUID SVWatershedFilterMinVariationGuid;
extern const GUID SVWatershedFilterControlFlagGuid;
extern const GUID SVWatershedFilterUseMarkerGuid;
extern const GUID SVThickeningFilterItterationsGuid;
extern const GUID SVThickeningFilterGrayOnGuid;
extern const GUID SVThinningFilterItterationsGuid;
extern const GUID SVGrayOnGuid;
extern const GUID SVBCUnevenGridGuid;

// GUID for Embedded Objects of SVUpperThresholdEquationClass
extern const GUID SVUpperThresholdEquationResultObjectGuid;

// GUID for Embedded Objects of SVLowerThresholdEquationClass
extern const GUID SVLowerThresholdEquationResultObjectGuid;

// GUID for Embedded Objects of SVAutoThresholdEquationClass
extern const GUID SVAutoThresholdEquationResultObjectGuid;

// GUIDs for Embedded Objects of SVThresholdClass
extern const GUID SVUseExternalATMObjectGuid;
extern const GUID SVUseExternalLTObjectGuid;
extern const GUID SVUseExternalUTObjectGuid;
extern const GUID SVRegressionTestModeGuid;


// GUIDs for Embedded Objects - Profile Tool Threshold flags and data variables
extern const GUID SVUseLowerThresholdSelectableObjectGuid;
extern const GUID SVUseLowerThresholdMaxMinusPercentDiffObjectGuid;
extern const GUID SVUseLowerThresholdMaxMinusOffsetObjectGuid;
extern const GUID SVUseLowerThresholdMinPlusOffsetObjectGuid;
extern const GUID SVLowerThresholdMaxMinusPercentDiffObjectGuid;
extern const GUID SVLowerThresholdMaxMinusOffsetObjectGuid;
extern const GUID SVLowerThresholdMinPlusOffsetObjectGuid;

extern const GUID SVUseUpperThresholdSelectableObjectGuid;
extern const GUID SVUseUpperThresholdMaxMinusPercentDiffObjectGuid;
extern const GUID SVUseUpperThresholdMaxMinusOffsetObjectGuid;
extern const GUID SVUseUpperThresholdMinPlusOffsetObjectGuid;
extern const GUID SVUpperThresholdMaxMinusPercentDiffObjectGuid;
extern const GUID SVUpperThresholdMaxMinusOffsetObjectGuid;
extern const GUID SVUpperThresholdMinPlusOffsetObjectGuid;

extern const GUID SVUseLowerThresholdSelectableBObjectGuid;
extern const GUID SVUseLowerThresholdMaxMinusPercentDiffBObjectGuid;
extern const GUID SVUseLowerThresholdMaxMinusOffsetBObjectGuid;
extern const GUID SVUseLowerThresholdMinPlusOffsetBObjectGuid;
extern const GUID SVLowerThresholdMaxMinusPercentDiffBObjectGuid;
extern const GUID SVLowerThresholdMaxMinusOffsetBObjectGuid;
extern const GUID SVLowerThresholdMinPlusOffsetBObjectGuid;

extern const GUID SVUseUpperThresholdSelectableBObjectGuid;
extern const GUID SVUseUpperThresholdMaxMinusPercentDiffBObjectGuid;
extern const GUID SVUseUpperThresholdMaxMinusOffsetBObjectGuid;
extern const GUID SVUseUpperThresholdMinPlusOffsetBObjectGuid;
extern const GUID SVUpperThresholdMaxMinusPercentDiffBObjectGuid;
extern const GUID SVUpperThresholdMaxMinusOffsetBObjectGuid;
extern const GUID SVUpperThresholdMinPlusOffsetBObjectGuid;

extern const GUID SVLinearThresholdMaxObjectGuid;
extern const GUID SVLinearThresholdMinObjectGuid;
extern const GUID SVLinearThresholdDeltaObjectGuid;

//GUID for the ShiftTool Mode
extern const GUID SVShiftToolModeGuid;
extern const GUID SVShiftToolReferenceXObjectGuid;
extern const GUID SVShiftToolReferenceYObjectGuid;
extern const GUID SVShiftToolEnableSourceImageExtentsGuid;


// GUID for Embedded Objects of SVEvaluateTranslationXClass
extern const GUID SVOutputEvaluateTranslationXResultObjectGuid;

// GUID for Embedded Objects of SVEvaluateTranslationYClass
extern const GUID SVOutputEvaluateTranslationYResultObjectGuid;

// GUID for Embedded Objects of SVEvaluateRotationXClass
extern const GUID SVOutputEvaluateRotationXResultObjectGuid;

// GUID for Embedded Objects of SVEvaluateRotationYClass
extern const GUID SVOutputEvaluateRotationYResultObjectGuid;

// GUID for Embedded Objects of SVEvaluateRotationAngleClass
extern const GUID SVOutputEvaluateRotationAngleResultObjectGuid;

// GUIDs for the Embeddeds of the SVTransformClass
extern const GUID SVPerformTranslationObjectGuid;
extern const GUID SVTranslationXObjectGuid;
extern const GUID SVTranslationYObjectGuid;
extern const GUID SVPerformRotationObjectGuid;
extern const GUID SVLearnedTranslationXObjectGuid;
extern const GUID SVLearnedTranslationYObjectGuid;
extern const GUID SVLearnedRotationXObjectGuid;
extern const GUID SVLearnedRotationYObjectGuid;
extern const GUID SVLearnedRotationAngleObjectGuid;

//GUID for the Transform Tool DisplacementX
extern const GUID SVImageTransformDisplacementXGuid;

//GUID for the Transform Tool DisplacementY
extern const GUID SVImageTransformDisplacementYGuid;

//GUID for the Top Result
extern const GUID SVTopResultObjectGuid;

//GUID for the Left Result
extern const GUID SVLeftResultObjectGuid;

//GUID for the Transform Tool SourceX
extern const GUID SVImageTransformSourceXGuid;

//GUID for the Transform Tool SourceY
extern const GUID SVImageTransformSourceYGuid;

// GUIDs for the Embeddeds of the SVImageTransformClass
extern const GUID SVUseExtentsOnlyObjectGuid;

// GUID for the SVConditionalToolSetDrawFlagObject
extern const GUID SVConditionalToolSetDrawFlagObjectGuid;

// GUID for the SVDrawToolFlagObject
extern const GUID SVConditionalToolDrawFlagObjectGuid;

// GUIDs for the SVImagePolarTransformClass and its Output Objects...
extern const GUID SVOutputCenterXObjectGuid;
extern const GUID SVOutputCenterYObjectGuid;
extern const GUID SVOutputStartRadiusObjectGuid;
extern const GUID SVOutputEndRadiusObjectGuid;
extern const GUID SVOutputStartAngleObjectGuid;
extern const GUID SVOutputEndAngleObjectGuid;
extern const GUID SVOutputInterpolationModeObjectGuid;
extern const GUID SVOutputUseFormulaObjectGuid;
extern const GUID SVOutputAngularMethodObjectGuid;

// Output Objects...
extern const GUID SVOutputEvaluateCenterXResultObjectGuid;
extern const GUID SVOutputEvaluateCenterYResultObjectGuid;
extern const GUID SVOutputEvaluateStartRadiusResultObjectGuid;
extern const GUID SVOutputEvaluateEndRadiusResultObjectGuid;
extern const GUID SVOutputEvaluateStartAngleResultObjectGuid;
extern const GUID SVOutputEvaluateEndAngleResultObjectGuid;

// GUIDs for Exposed Extents (primarily Images/Tools)
extern const GUID SVExtentRelativeLeftPositionObjectGuid;
extern const GUID SVExtentRelativeTopPositionObjectGuid;
extern const GUID SVExtentRelativeRightPositionObjectGuid;
extern const GUID SVExtentRelativeBottomPositionObjectGuid;
extern const GUID SVExtentWidthObjectGuid;
extern const GUID SVExtentHeightObjectGuid;
extern const GUID SVExtentHeightScaleFactorObjectGuid;
extern const GUID SVExtentWidthScaleFactorObjectGuid;

// GUIDs for Exposed Source Image Extents (primarily Images/Tools)
extern const GUID SVUpdateAuxiliaryExtentsObjectGuid;
extern const GUID SVAuxiliarySourceXObjectGuid;
extern const GUID SVAuxiliarySourceYObjectGuid;
extern const GUID SVAuxiliarySourceAngleObjectGuid;
extern const GUID SVAuxiliarySourceImageNameObjectGuid;
extern const GUID SVAuxiliaryDrawTypeObjectGuid;

// GUIDs for SVLUTEquationClass and its outputs...
extern const GUID SVLUTIndexVariableObjectGuid;
extern const GUID SVLUTEquationResultObjectGuid;
extern const GUID SVLUTEquationClipFlagObjectGuid;

// GUIDs for SVLUTOperatorClass and its outputs...
extern const GUID SVOutputLUTVectorObjectGuid;
extern const GUID SVUseLUTObjectGuid;
extern const GUID SVContinuousRecalcLUTObjectGuid;
extern const GUID SVLUTModeObjectGuid;
extern const GUID SVLUTUpperClipObjectGuid;
extern const GUID SVLUTLowerClipObjectGuid;
extern const GUID SVLUTMinInputObjectGuid;
extern const GUID SVLUTMaxInputObjectGuid;
extern const GUID SVLUTMaxOutputObjectGuid;
extern const GUID SVLUTMinOutputObjectGuid;

// GUIDs for the Embeddeds of the SVPatternAnalyzerClass
extern const GUID SVpatAcceptThresholdObjectGuid;
extern const GUID SVpatCertainThresholdObjectGuid;
extern const GUID SVpatAccuracyObjectGuid;
extern const GUID SVpatMaxOccurancesObjectGuid;
extern const GUID SVpatSearchAngleModeObjectGuid;
extern const GUID SVpatSpeedObjectGuid;
extern const GUID SVpatModelImageFileGuid;
extern const GUID SVpatSearchAngleObjectGuid;
extern const GUID SVpatAngleDeltaNegObjectGuid;
extern const GUID SVpatAngleDeltaPosObjectGuid;
extern const GUID SVpatAngleToleranceObjectGuid;
extern const GUID SVpatAngleAccuracyObjectGuid;
extern const GUID SVpatAngleInterpolationObjectGuid;

extern const GUID SVpatCircularOverscanObjectGuid;
extern const GUID SVpatDontCareObjectGuid;
extern const GUID SVpatDontCareImageFileGuid;

extern const GUID SVpatPreliminaryAcceptanceThresholdObjectGuid;
extern const GUID SVpatFastFindObjectGuid;
extern const GUID SVpatModelStepObjectGuid;
extern const GUID SVpatModelWidthObjectGuid;
extern const GUID SVpatModelHeightObjectGuid;
extern const GUID SVpatModelCenterXObjectGuid;
extern const GUID SVpatModelCenterYObjectGuid;
extern const GUID SVpatDontCareWidthObjectGuid;
extern const GUID SVpatDontCareHeightObjectGuid;
extern const GUID SVpatBeginningResolutionLevelObjectGuid;
extern const GUID SVpatFinalResolutionLevelObjectGuid;
extern const GUID SVpatAdditionalCandidatesObjectGuid;
extern const GUID SVpatCandidateSpacingXMinObjectGuid;
extern const GUID SVpatCandidateSpacingYMinObjectGuid;

extern const GUID SVpatResultMatchScoreObjectGuid;
extern const GUID SVpatResultXObjectGuid;
extern const GUID SVpatResultYObjectGuid;
extern const GUID SVpatResultAngleObjectGuid;

extern const GUID SVpatResultNumFoundOccurancesObjectGuid;


// GUIDs for the Embeddeds of the SVColorToolClass
extern const GUID SVBand0ImageObjectGuid;
extern const GUID SVBand1ImageObjectGuid;
extern const GUID SVBand2ImageObjectGuid;
extern const GUID SVConvertToHSIObjectGuid;
extern const GUID SVHasROIObjectGuid;

extern const GUID SVColorThresholdEnabledObjectGuid;

// GUIDs for the Embeddeds of the SVColorThresholdClass
extern const GUID SVBand0UpperThresholdObjectGuid;
extern const GUID SVBand0LowerThresholdObjectGuid;
extern const GUID SVBand0ThresholdExcludeObjectGuid;
extern const GUID SVBand0ThresholdEnabledObjectGuid;
extern const GUID SVBand1UpperThresholdObjectGuid;
extern const GUID SVBand1LowerThresholdObjectGuid;
extern const GUID SVBand1ThresholdExcludeObjectGuid;
extern const GUID SVBand1ThresholdEnabledObjectGuid;
extern const GUID SVBand2UpperThresholdObjectGuid;
extern const GUID SVBand2LowerThresholdObjectGuid;
extern const GUID SVBand2ThresholdExcludeObjectGuid;
extern const GUID SVBand2ThresholdEnabledObjectGuid;

// GUIDs for the Embeddeds of the SVLinearEdgeProcessing
extern const GUID SVEdgeUpperThresholdValueGuid;

// GUIDs for the Embeddeds of the SVOCRGrayParamStruct
extern const GUID SVEdgeADirectionGuid;
extern const GUID SVEdgeAPolarisationGuid;
extern const GUID SVEdgeAEdgeSelectGuid;
extern const GUID SVEdgeAEdgeSelectThisValueGuid;
extern const GUID SVEdgeAIsFixedEdgeMarkerGuid;
extern const GUID SVEdgeAPositionGuid;
extern const GUID SVEdgeAPositionOffsetGuid;
extern const GUID SVEdgeALowerThresholdValueGuid;
extern const GUID SVEdgeAUpperThresholdValueGuid;

// GUIDs for the Embeddeds of the SVOCRGrayParamStruct
extern const GUID SVEdgeBDirectionGuid;
extern const GUID SVEdgeBPolarisationGuid;
extern const GUID SVEdgeBEdgeSelectGuid;
extern const GUID SVEdgeBEdgeSelectThisValueGuid;
extern const GUID SVEdgeBIsFixedEdgeMarkerGuid;
extern const GUID SVEdgeBPositionGuid;
extern const GUID SVEdgeBPositionOffsetGuid;
extern const GUID SVEdgeBLowerThresholdValueGuid;
extern const GUID SVEdgeBUpperThresholdValueGuid;
extern const GUID SVDrawCriteriaObjectGuid;


///////////////////////////////////////////////////
// GUIDs for the ResizeTool
///////////////////////////////////////////////////
extern const GUID SVResizeInterpolationModeGuid;
extern const GUID SVResizeOverscanGuid;
extern const GUID SVResizePerformanceGuid;
extern const GUID SVLogicalROIImageGuid;

const int COUNT_OF_INPUT_OUTPUT_GUIDs = 50;

extern const GUID aInputObjectGUID[COUNT_OF_INPUT_OUTPUT_GUIDs];
extern const GUID aInputObject_LinkedGUID[COUNT_OF_INPUT_OUTPUT_GUIDs];
extern const GUID aSVVariantResultImageObjectGuid[];
extern const GUID aSVVariantResultObjectGuid[COUNT_OF_INPUT_OUTPUT_GUIDs];
extern const GUID aSVDllDependencyFileNameGuid[];
extern const GUID SVDllFileNameGuid;
extern const GUID SVDllToolNameGuid;
extern const GUID SVDllToolVersionGuid;

extern const GUID SVLinearDataClassGuid;

// Show All Overlays
extern const GUID SVShowAllEdgeAOverlaysGuid;
extern const GUID SVShowAllEdgeBOverlaysGuid;

// Source Image Names GUID
extern const GUID SVSourceImageNamesGuid;


extern const GUID SVBCRawDataGuid;
extern const GUID SVBCStringFormatGuid;
extern const GUID SVBCThresholdTypeGuid;

extern const GUID SVToolCommentTypeObjectGuid;

extern const GUID SVCameraTriggerTimestampGuid;
extern const GUID SVCameraTriggerLineInStateGuid;
extern const GUID SVStringHeaderGuid;
// GUIDs for ToolSizeAdjustTask ...
extern const GUID  ToolSizeAdjustSizeWidthModeGuid;		
extern const GUID  ToolSizeAdjustSizeHeightModeGuid;
extern const GUID  ToolSizeAdjustSizePositionXModeGuid;
extern const GUID  ToolSizeAdjustSizePositionYModeGuid;
extern const GUID EQAdjustSizeWidthResultGuid;	
extern const GUID EQAdjustSizeHeightResultGuid;	
extern const GUID EQAdjustSizePositionXResultGuid;	
extern const GUID EQAdjustSizePositionYResultGuid;	

///////////////////////////////////////////////////
// GUIDs for the TableTool
///////////////////////////////////////////////////
extern const GUID TableTool_MaxRowGuid;
extern const GUID TableObject_NumberOfRowsGuid;
const int c_maxTableColumn = 50;
extern const GUID TableColumnValueObjectGuid[c_maxTableColumn];
// Unique ID GUID for Global variables
extern const GUID RootUidGuid;
extern const GUID GlobalUidGuid;
// Unique ID GUID for Environment variables
extern const GUID EnvironmentUidGuid;
extern const GUID EnvironmentModelNumberUidGuid;
extern const GUID EnvironmentWinKeyUidGuid;
extern const GUID EnvironmentImageUpdateUidGuid;
extern const GUID EnvironmentResultUpdateUidGuid;
extern const GUID EnvironmentModeIsRunUidGuid;
extern const GUID EnvironmentModeIsStopUidGuid;
extern const GUID EnvironmentModeIsRegressionTestUidGuid;
extern const GUID EnvironmentModeIsTestUidGuid;
extern const GUID EnvironmentModeIsEditUidGuid;
extern const GUID EnvironmentModeValueUidGuid;
extern const GUID EnvironmentModeUidGuid;
extern const GUID EnvironmentAutoSaveUidGuid;
extern const GUID EnvironmentDiskProtectionUidGuid;
extern const GUID EnvironmentStartLastConfigUidGuid;
// Unique base ID GUID for Camera variables always have 0x100 GUIDS free after the base GUID
extern const GUID CameraBaseSerialNumberUidGuid;
extern const GUID CameraBaseGainUidGuid;
extern const GUID CameraBaseShutterUidGuid;
extern const GUID CameraBaseRegPathUidGuid;
extern const GUID CameraBaseRegFileUidGuid;
// Unique base ID GUID for PPQ variables always have 0x100 GUIDS free after the base GUID
extern const GUID PpqBaseLengthUidGuid;
extern const GUID PpqBaseTriggerCountUidGuid;

// Unique base ID GUID for IO-Entries variables always have 0x100 GUIDS free after the base GUID
extern const GUID DigitalInputUidGuid;
extern const GUID DigitalOutputUidGuid;
extern const GUID RemoteInputUidGuid;

///////////////////////////////////////////////////
// GUIDs for the TableAnalyzerTool
///////////////////////////////////////////////////
extern const GUID TableAnalyzerIndexObjectGuid;
extern const GUID TableAnaylzerSortIsASCGuid;
extern const GUID TableAnaylzerExcludeHighGuid;
extern const GUID TableAnaylzerExcludeHigh_LinkGuid;
extern const GUID TableAnaylzerExcludeLowGuid;
extern const GUID TableAnaylzerExcludeLow_LinkGuid;
extern const GUID TableAnaylzerLimitValueGuid;
extern const GUID TableAnaylzerLimitValue_LinkGuid;
// GUID for the LoopToolClass
extern const GUID MaxLoopsGuid; 
extern const GUID LoopCounterGuid;
extern const GUID LinkedLoopsGuid;
extern const GUID  LinkedLoops_LinkedGuid;
extern const GUID  LoopBreakGuid;
extern const GUID LoopBreak_LinkedGuid;

////////////////////////////////////////////////////////////////////////////////
// Insert new GUIDs immediately before the previous line.
////////////////////////////////////////////////////////////////////////////////

