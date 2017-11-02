//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// enums and struct for object types
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVUtilityLibrary/SVGUID.h"
#pragma endregion Includes

//TODO (MZA:2015/01/09): Add namespace to fit to the code rules. 
//								Not edit yet to avoid to change to much code in this prototype.
//namespace SvOi
//{
		const DWORD cMaxStringSize = 512;

		//! These are special SVObserver variant VT types for SVPointValueObjectClass and SVDPointValueObjectClass
		const DWORD VT_POINT = 80;
		const DWORD VT_DPOINT = 81;

		enum SVObjectTypeEnum
		{
			SVNotSetObjectType						= 0x00000000,

			SVSystemObjectType						= 0x00000001,
			SVDigitizerObjectType					= 0x00000002,
			SVImageProcessingObjectType				= 0x00000003,

			SVInspectionObjectType					= 0x00000004,

			SVFreezeSetObjectType					= 0x00000005,
			SVToolSetObjectType						= 0x00000006,

			SVToolObjectType						= 0x00000007,
			SVEquationObjectType					= 0x00000008,
			SVImageArithmeticObjectType				= 0x0000000a,
			SVMathContainerObjectType				= 0x0000000b,
			SVRotateReferenceObjectType				= 0x0000000c,

			SVImageObjectType						= 0x00000011,
			SVDataBufferClassObjectType				= 0x00000012,

			SVTriggerProcessingObjectType			= 0x00000013,
			SVDigitizerProcessingObjectType			= 0x00000014,
			SVRootObjectType						= 0x00000015,

			SVExternalToolTaskObjectType            = 0x00000020,

			SVTransformObjectType					= 0x00000040,
			SVPolarTransformObjectType				= 0x00000041,

			SVPPQObjectType							= 0x00000050,
			SVDisplayObjectType						= 0x00000051,
			SVInspectionProcessType					= 0x00000052,
			SVVirtualCameraType						= 0x00000053,
			SVTriggerObjectType						= 0x00000054,
			SVInputObjectListType					= 0x00000055,
			SVOutputObjectListType					= 0x00000056,
			SVIOControllerType                      = 0x00000057,
			SVRemoteOutputGroupType					= 0x00000058,
			SVIoObjectType							= 0x00000059,
			SVLinearEdgeProcessingObjectType        = 0x00000060,
			SVBasicValueObjectType			        = 0x00000061,

			SVValueObjectType						= 0x00000200,

			SVOperatorObjectType					= 0x00001000,
			SVBinaryOperatorObjectType				= 0x00002000,
			SVUnaryOperatorObjectType				= 0x00003000,
			SVUnaryImageOperatorObjectType			= 0x00004000,
			SVFilterObjectType						= 0x00005000,
			SVUnaryImageOperatorListObjectType		= 0x00006000,
			TableObjectType							= 0x00007000,
			TableAnalyzerType						= 0x00007001,

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
			SVCameraImageTemplateObjectType				= 0x00000003,

			// System Subtypes:
			SVMatroxSystemObjectType					= 0x00000005,

			SVMatroxImageProcessingObjectType			= 0x00000007,
			SVFileAcquisitionImageProcessingObjectType	= 0x00000009,

			// Tool Subtypes:
			SVWindowToolObjectType						= 0x00000010,
			SVMathToolObjectType						= 0x00000012,
			SVToolImageObjectType						= 0x00000015,
			SVToolArchiveObjectType                     = 0x00000016,
			SVToolLoadImageObjectType					= 0x00000017,
			SVRoundWindowToolObjectType					= 0x00000018,
			SVStatisticsToolObjectType					= 0x00000019,
			SVToolAcquisitionObjectType					= 0x0000001a,
			SVToolFreezeObjectType						= 0x0000001b,
			SVTransformationToolObjectType				= 0x0000001c,
			SVPolarTransformationToolObjectType			= 0x0000001d, 
			SVColorToolObjectType						= 0x0000001e,
			SVToolCylindricalObjectType					= 0x0000001f,
			SVPerspectiveToolObjectType                 = 0x00000020,
			SVExternalToolObjectType                    = 0x00000025,
			SVLinearToolObjectType						= 0x00000026,
			SVRemoteInputToolObjectType					= 0x00000027,
			SVShiftToolObjectType						= 0x00000028,
			SVResizeToolObjectType						= 0x00000029,
			SVRingBufferToolObjectType					= 0x0000002a,
			SVTableToolObjectType						= 0x0000002b,
			SVTableAnalyzerToolObjectType				= 0x0000002c,

			SVEnvironmentObjectType						= 0x00000062,
			SVCameraObjectType							= 0x00000063,
			SVGlobalConstantObjectType					= 0x00000064,

			// Analyzer Subtypes:	
			SVPixelAnalyzerObjectType				    = 0x00000100,
			SVBlobAnalyzerObjectType					= 0x00000101,
			SVLuminanceAnalyzerObjectType				= 0x00000102,
			SVOCVAnalyzerObjectType						= 0x00000105,
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

			SVLinearEdgeAProcessingObjectType			= 0x00000380,
			SVLinearEdgeBProcessingObjectType			= 0x00000381,

			SVLinearPixelCountingAnalyzerObjectType		= 0x00000397,
			SVLinearEdgeCountingAnalyzerObjectType		= 0x00000398,
			SVLinearEdgePositionAnalyzerObjectType		= 0x00000399,
			SVLinearMeasurementAnalyzerObjectType	    = 0x0000039a,
			SVLinearMaximumForegroundObjectAnalyzerObjectType = 0x0000039b,
			SVLinearMaximumBackgroundObjectAnalyzerObjectType = 0x0000039c,
			SVLinearMaximumObjectAnalyzerObjectType		= 0x0000039d,


			// Equation Subtypes:
			SVConditionalObjectType						= 0x00000400,
			SVMathEquationObjectType					= 0x00000401,
			SVUpperThresholdEquationObjectType			= 0x00000402,
			SVLowerThresholdEquationObjectType			= 0x00000403,
			SVAutoThresholdEquationObjectType			= 0x00000404,
			SVLUTEquationObjectType						= 0x00000405,

			//Equation Subtypes for VAriableSize 	
			EQSizeWidthType								= 0x00000406,
			EQSizeHeightType							= 0x00000407,
			EQSizePositionXType							= 0x00000408,
			EQSizePositionYType							= 0x00000409,

			TableColumnEquationObjectType				= 0x0000040a,

			// Math Container Subtypes:
			SVEvaluateXObjectType						= 0x00000480,
			SVEvaluateYObjectType						= 0x00000481,
			SVEvaluateBetaObjectType					= 0x00000482,
			SVEvaluateObjectType						= 0x00000483,
			SVEvaluateTranslationXObjectType			= 0x00000484,
			SVEvaluateTranslationYObjectType			= 0x00000485,
			SVEvaluateRotationXObjectType				= 0x00000486,
			SVEvaluateRotationYObjectType				= 0x00000487,
			SVEvaluateRotationAngleObjectType			= 0x00000488,
			SVEvaluateCenterXObjectType					= 0x00000489,
			SVEvaluateCenterYObjectType					= 0x0000048a,
			SVEvaluateStartRadiusObjectType				= 0x0000048b,
			SVEvaluateEndRadiusObjectType				= 0x0000048c,
			SVEvaluateStartAngleObjectType				= 0x0000048d,
			SVEvaluateEndAngleObjectType				= 0x0000048e,
			
			// IO Subtypes:
			SVDigitalInputObjectType					= 0x00000500,
			SVDigitalOutputObjectType					= 0x00000501,
			SVRemoteInputObjectType						= 0x00000502,
			SVRemoteOutputObjectType					= 0x00000503,
			SVCameraDataInputObjectType					= 0x00000504,

			// Transform Subtypes:
			SVImageTransformObjectType					= 0x00000600,
			SVPointTransformObjectType					= 0x00000601,

			// Polar Transform Subtypes:
			SVImagePolarTransformObjectType				= 0x00000700,

			// Result Subtypes:
			SVResultPointXObjectType					= 0x00000800,
			SVResultPointYObjectType					= 0x00000801,
			SVResultLongObjectType						= 0x00000802,
			SVResultDoubleObjectType					= 0x00000803,
			SVResultDPointXObjectType					= 0x00000804,
			SVResultDPointYObjectType					= 0x00000805,
			SVResultVariantObjectType                   = 0x00000806,

			SVResultOCVObjectType						= 0x00000902,
			SVResultStringObjectType                    = 0x00000955,
			SVBarCodeAnalyzerObjectType					= 0x0000095a,
			SVResultBarCodeObjectType					= 0x0000095c,
			SVPatternAnalyzerObjectType					= 0x00000960,

			// Unary Image Operator List Subtypes:
			SVStdImageOperatorListObjectType			= 0x00001000,
			SVInPlaceImageOperatorListObjectType		= 0x00001001,
			SVLinearImageOperatorListObjectType			= 0x00001002,

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
			SVLUTOperatorObjectType						= 0x00002012,
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

			//TableObject SubTypes
			TableFillObjectType						= 0x00004000,
			TableCopyObjectType						= 0x00004001,

			TableAnalyzerSortType					= 0x00004010,
			TableAnalyzerExcludeType				= 0x00004011,
			TableAnalyzerLimitType					= 0x00004012,

			//ValueObject SubTypes
			SVDWordValueObjectType = 0x00005000,
			SVLongValueObjectType = 0x00005001,
			SVDoubleValueObjectType = 0x00005002,
			SVBoolValueObjectType = 0x00005003,
			SVPointValueObjectType = 0x00005004,
			SVByteValueObjectType = 0x00005005,
			SVStringValueObjectType = 0x00005006,
			SVEnumValueObjectType = 0x00005007,
			SVDPointValueObjectType = 0x00005008,
			SVInt64ValueObjectType = 0x00005009,
			SVCharValueObjectType = 0x0000500A,
			SVVariantValueObjectType = 0x0000500B,
			DoubleSortValueObjectType = 0x0000500C,

			// unknown
			SVRegressionTestModeObjectType				= 0x00011010,
		};

		struct SVObjectTypeInfoStruct
		{
			SVObjectTypeInfoStruct( SVObjectTypeEnum p_ObjectType=SVNotSetObjectType, SVObjectSubTypeEnum p_SubType=SVNotSetSubObjectType )
				: ObjectType( p_ObjectType ), SubType( p_SubType ), EmbeddedID( SV_GUID_NULL ) {};

			SVObjectTypeInfoStruct( const SVGUID& p_rEmbeddedID, SVObjectTypeEnum p_ObjectType=SVNotSetObjectType, SVObjectSubTypeEnum p_SubType=SVNotSetSubObjectType )
				: ObjectType( p_ObjectType ), SubType( p_SubType ), EmbeddedID( p_rEmbeddedID ){};

			//******************************************************************************
			// Data Element(s):
			//******************************************************************************

			SVObjectTypeEnum	ObjectType;			// Main Type
			SVObjectSubTypeEnum		SubType;		// Sub Type  

			// Embedded ID, if any.
			GUID				EmbeddedID;
		};
//} //namespace SvOi

