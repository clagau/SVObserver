//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
//Enums used in SVMatroxlibrary moved from SVMatroxEnum.h to reduce dependends between header files 
//******************************************************************************
#pragma once

#include <mil.h>

/**
@SVObjectName Matrox Buffer Type Enum

@SVObjectOverview This enum is used for creating a buffer

@SVObjectOperations None

*/
enum SVMatroxBufferTypeEnum
{
	SVUnknownBufferType		= 0x0000,
	SVBufferSize			= 0x00ff,
	SVBufferSigned          = 0x80000000,
	SVBufferFloat			= 0x40000000 | SVBufferSigned,
	SVBufferUnsigned		= 0x00000000,

	SV1BitUnsigned			= 0x000000001,

	SV8BitUnsigned			= 0x000000008,
	SV8BitSigned			= SV8BitUnsigned | SVBufferSigned,

	SV16BitUnsigned			= 0x000000010,
	SV16BitSigned			= SV16BitUnsigned | SVBufferSigned,

	SV32BitUnsigned			= 0x000000020,
	SV32BitSigned			= SV32BitUnsigned | SVBufferSigned,

};


/**
@SVObjectName Matrox Buffer Attribute Enum

@SVObjectOverview This enum is used for creating a buffer

@SVObjectOperations None

*/
enum SVMatroxBufferAttributeEnum
{
	SVBufAttUnknown					= 0x00000000L,

	// Specifies the buffer usage
	SVBufUsage						= 0x0000000fL,
	SVBufAttLut						= 0x00000001L, //0x00000100L,		// M_LUT
	SVBufAttImage					= 0x00000002L,		// M_IMAGE
	SVBufAttKernel					= 0x00000004L,		// M_KERNEL
	SVBufAttStructElement			= 0x00000008L,		// M_STRUCT_ELEMENT

	// Buffer Type
	SVBufType						= 0x000000f0L,
	SVBufAttCompress				= 0x00000010L,		// M_COMPRESS
	SVBufAttDisp					= 0x00000020L,		// M_DISP
	SVBufAttGrab					= 0x00000040L,		// M_GRAB
	SVBufAttProc					= 0x00000080L,		// M_PROC

	// Attributes that go with SVBufAttCompress
	SVBufJpeg						= 0x00003f00L,
	SVBufAttJpegLossless			= 0x00000100L,	// M_JPEG_LOSSLESS
	SVBufAttJpeg2000Lossless		= 0x00000200L,	// M_JPEG2000_LOSSLESS
	SVBufAttJpegLosslessInterlaced	= 0x00000400L,	// M_JPEG_LOSSLESS_INTERLACED
	SVBufAttJpegLossy				= 0x00000800L,	// M_JPEG_LOSSY
	SVBufAttJpeg2000Lossy			= 0x00001000L,	// M_JPEG2000_LOSSY
	SVBufAttJpegLossyInterlaced		= 0x00002000L,	// M_JPEG_LOSSY_INTERLACED

	// Attributes that affect creation
	SVBufAttrUsage					= 0x0000C000L,
	SVBufAttDib						= 0x00004000L,	// M_DIB
	SVBufAttDirectX					= 0x00008000L,	// M_DIRECTX

	// Location for buffer
	SVBufLoc						= 0x00030000L,
	SVBufAttOffBoard				= 0x00010000L,	// M_OFF_BOARD
	SVBufAttOnBoard					= 0x00020000L,	// M_ON_BOARD

	// Type of memory
	SVBufMemType					= 0x00300000L,
	SVBufAttNonPaged				= 0x00100000L,	// M_NON_PAGED
	SVBufAttPaged					= 0x00200000L,	// M_PAGED

	SVBufAttPacked					= 0x00400000L,	// M_PACKED
	SVBufAttPlanar					= 0x00800000L,	// M_PLANAR
	SVBufAttNoFlip					= 0x01000000L,	// M_NO_FLIP
	SVBufAttRgb16					= 0x02000000L,	// M_RGB16
	SVBufAttBgr24					= 0x04000000L,	// M_BGR24
	SVBufAttBgr32					= 0x08000000L,	// M_BGR32


	// Mono8
	//SVBuffAttImageProcPackedOffBoardDibPaged = SVBufAttImage | SVBufAttProc | SVBufAttPacked | SVBufAttOffBoard | SVBufAttDib | SVBufAttPaged | SVBufAttNoFlip,
	// RGB888
	SVBuffAttImageProcPackedOffBoardPagedBgr24 = SVBufAttImage | SVBufAttProc | SVBufAttPacked | SVBufAttOffBoard | SVBufAttPaged | SVBufAttBgr24 | SVBufAttNoFlip,
	SVBuffAttImageProcPackedOffBoardDibPagedBgr24 = SVBufAttImage | SVBufAttProc | SVBufAttDib | SVBufAttPacked | SVBufAttOffBoard | SVBufAttPaged | SVBufAttBgr24 | SVBufAttNoFlip,
	// RGB8888
	SVBufAttImageProcPackedOffBoardPagedBgr32 = SVBufAttImage | SVBufAttProc | SVBufAttPacked | SVBufAttOffBoard | SVBufAttPaged | SVBufAttBgr32 | SVBufAttNoFlip,
	SVBufAttImageProcPackedOffBoardDibPagedBgr32 = SVBufAttImage | SVBufAttProc | SVBufAttDib | SVBufAttPacked | SVBufAttOffBoard | SVBufAttPaged | SVBufAttBgr32 | SVBufAttNoFlip,

	SVBufAttImageProcDispDib		= SVBufAttProc | SVBufAttImage | SVBufAttDisp | SVBufAttDib,  //Nur für SVMaskEditor
	// Image Buffer Dib
	SVBufAttImageProcDib			= SVBufAttProc | SVBufAttImage | SVBufAttDib,
	// Image Buffer..
	SVBufAttImageProc				= SVBufAttProc | SVBufAttImage,
	// GrabImage Buffer..
	SVBufAttGrabImageProc		= SVBufAttGrab | SVBufAttImage | SVBufAttProc,
};

/**
@SVObjectName Matrox Value Enum

@SVObjectOverview This enum is used for values that may get stored with the configuration.

@SVObjectOperations None

*/
enum SVMatroxBufferValues
{
	SVValueAllBands						= 0xffffffffL,	// M_ALL_BAND(-1)
	SVValueDefault						= 0x10000000L,	// M_DEFAULT
	SVValueEnable						= -9997L,		// M_ENABLE
	SVValueDisable						= -9999L,		// M_DISABLE
	SVValueBinary						= 0x00001000L,	// M_BINARY
	SVValueGrayscale					= 0x00000200L,	// M_GREYSCALE
	SVValueTranaparent					= 0x01000059L,	// M_TRANSPARENT
	SVValueMirror							= 0x01000061L, // M_MIRROR
	SVValueImageToSkeleton				= -1L,			// M_TO_SKELETON
	SVValueAll							= 0x40000000L,	// M_ALL
	SVValueAny							= 0x11000000L,  // M_ANY
	SVValueVeryLow						= 0L,			// M_VERY_LOW
	SVValueLow							= 1L,			// M_LOW
	SVValueMedium						= 2L,			// M_MEDIUM
	SVValueHigh							= 3L,			// M_HIGH
	SVValueVeryHigh						= 4L,			// M_VERY_HIGH
	SVValueForeground					= 4L,			// M_FOREGROUND_VALUE
	SVValueForegroundBlack				= 0x100L,		// M_FOREGROUND_BLACK
	SVValueForegroundWhite				= 0x80L,		// M_FOREGROUND_WHITE
	SVValueBilinear						= 0x00000008L,	// M_BILINEAR
	SVValueBicubic						= 0x00000010L,  // M_BICUBIC
	SVValueNearestNeighbor				= 0x00000040L,	// M_NEAREST_NEIGHBOR
	SVValueTrue							= 1L,			// M_TRUE
	SVValueFalse						= 0L,			// M_FALSE
	SVValueMedian						= 0x10000L,		// M_MEDIAN
	SVValueDontCare						= 0x00008000L,	// M_DONT_CARE
	SVValueEncNum						= 0,			// M_ENC_NUM
	SVValueEncAscii						= 4,			// M_ENC_ASCII
	SVValueEncMode2						= 6,			// M_ENC_MODE2
	SVValueEncUpcAAddOn					= M_ENC_UPCA_ADDON, 
	SVValueEncUpcEAddOn					= M_ENC_UPCE_ADDON,
	SVValueEncStandard					= 11,			// M_ENC_STANDARD
	SVValueEncQRCodeModel1				= 0x00200000,	// M_ENC_QRCODE_MODEL1
	SVValueEncQRCodeModel2				= 0x00400000,	// M_ENC_QRCODE_MODEL2
	SVValueEccNone						= 4,			// M_ECC_NONE
	SVValueEccCheckDigit				= 5,			// M_ECC_CHECK_DIGIT
	SVValueEccReedSolomon				= M_ECC_REED_SOLOMON,
	
	SVValueEcc050						= 0,			// M_ECC_050
	SVValueEcc080						= 1,			// M_ECC_080
	SVValueEcc100						= 2,			// M_ECC_100
	SVValueEcc140						= 3,			// M_ECC_140
	SVValueEcc200						= 7,			// M_ECC_200
	SVValueEccReedSolomon0				= 10,
	SVValueEccReedSolomon1				= SVValueEccReedSolomon0 + 1,
	SVValueEccReedSolomon2				= SVValueEccReedSolomon0 + 2,
	SVValueEccReedSolomon3				= SVValueEccReedSolomon0 + 3,
	SVValueEccReedSolomon4				= SVValueEccReedSolomon0 + 4,
	SVValueEccReedSolomon5				= SVValueEccReedSolomon0 + 5,
	SVValueEccReedSolomon6				= SVValueEccReedSolomon0 + 6,
	SVValueEccReedSolomon7				= SVValueEccReedSolomon0 + 7,
	SVValueEccReedSolomon8				= SVValueEccReedSolomon0 + 8,
	SVValueEccH = 32,			// M_ECC_H	
	SVValueEccL = 29,			// M_ECC_L
	SVValueEccM = 30,			// M_ECC_M	
	SVValueEccQ = 31,			// M_ECC_Q
	SVValueENCAlpha						= 1,			// M_ENC_ALPHA
	SVValueENCAlphaNum					= 3,			// M_ENC_ALPHANUM
	SVValueENCAlphaNumPunc				= 2,			// M_ENC_ALPHANUM_PUNC
	SVValueENCAscii						= 4,			// M_ENC_ASCII
	SVValueENCIso8						= 5,			// M_ENC_ISO8
	SVValueENCMode2						= M_ENC_MODE2,
	SVValueENCMode3						= M_ENC_MODE3,
	SVValueENCMode4						= M_ENC_MODE4,
	SVValueENCMode5						= M_ENC_MODE5,
	SVValueENCMode6						= M_ENC_MODE6,
	SVValueENCGS1						= M_ENC_GS1_DATABAR_OMNI,
	SVValueENCGS1Trunc					= M_ENC_GS1_DATABAR_TRUNCATED,
	SVValueENCGS1Limited				= M_ENC_GS1_DATABAR_LIMITED,
	SVValueENCGS1Expanded				= M_ENC_GS1_DATABAR_EXPANDED,
	SVValueENCGS1Stacked				= M_ENC_GS1_DATABAR_STACKED,
	SVValueENCGS1StackedOmni			= M_ENC_GS1_DATABAR_STACKED_OMNI,
	SVValueENCGS1ExpandedStacked		= M_ENC_GS1_DATABAR_EXPANDED_STACKED,
};

// Used with MbufInquire / MbufControl "Get / Set"
/**
@SVObjectName Matrox Buffer Info Enum

@SVObjectOverview This enum is used for Getting and Setting buffer settings.

@SVObjectOperations None

*/
enum SVMatroxBufferInfoEnum
{
	SVBufInfoUnknown			= 0000000000L,

	// Buffer Inquire
	SVSizeX      = 0x00000001L,		// M_SIZE_X;
	SVSizeY      = 0x00000002L,		// M_SIZE_Y;
	SVSizeBand   = 0x00000004L,		// M_SIZE_BAND;
	SVType       = 0x00000008L,		// M_TYPE;
	SVWindowDC   = 0x00000010L,		// M_WINDOW_DC;
	SVAttribute  = 0x00000020L,		// M_ATTRIBUTE;
	SVBufMax     = 0x00000040L,		// M_MAX
	SVBufMin     = 0x00000080L,		// M_MIN
	SVDataFormat = 0x00000800L,		// M_DATA_FORMAT

	// Display buffer
	SVBufWindowDCAlloc = 0x00000100L,		// M_WINDOW_DC_ALLOC
	SVBufWindowDCFree  = 0x00000200L,		// M_WINDOW_DC_FREE
	SVBufModified      = 0x00000400L,		// M_MODIFIED

	// Kernel Control Flags
	SVKernelDefault = 0x00001000L,		// M_DEFAULT
	SVAbsoluteValue = 0x00002000L,		// M_ABSOLUTE_VALUE ;
	SVNormalization = 0x00004000L,		// M_NORMALIZATION_FACTOR ;
	SVBufOverScan   = 0x00008000L,		// M_OVERSCAN;
	SVBufSaturation = 0x00010000L,		// M_SATURATION;

	// Buffer Info Combination
	SVPitchByte = 0x00080000L,		// M_PITCH_BYTE;
	SVPitch     = 0x00100000L,		// M_PITCH;

	SVWindowDibHeader = 0x00200000L,		// M_BITMAPINFO;
	SVPhysicalAddress = 0x00400000L,		// M_PHYSICAL_ADDRESS 
	SVHostAddress     = 0x00800000L,		// M_HOST_ADDRESS;
	SVDirectXSurface  = 0x01000000L,		// M_DDRAW_SURFACE;

	// Parent Info
	SVParentID      = 0x02000000L,   // M_PARENT_ID
	SVParentOffsetX = 0x04000000L,   // M_PARENT_OFFSET_X
	SVParentOffsetY = 0x08000000L,   // M_PARENT_OFFSET_Y

	// Combination Constants
	SVHostAddressPitch = SVHostAddress | SVPitch,

};


// Mbuf Import / Export function
/**
@SVObjectName Matrox File Type Enum

@SVObjectOverview This enum is used to set the file type for importing and exporting images buffers.

@SVObjectOperations None

*/
enum SVMatroxFileTypeEnum
{
	SVFileUnknown				= -1,
	SVFileMIL					= 0L,			// M_MIL
	SVFileTiff					= 2L,			// M_TIFF
	SVFileBitmap				= 4L,			// M_BITMAP

};



// ******* Blob Enums 
/**
@SVObjectName Matrox Blob Criterion Enum

@SVObjectOverview This enum is used for the Matrox Blob Interface.

@SVObjectOperations None

*/
enum SVBlobSelectionEnum
{

	SVEBlobArea					= M_AREA,
	SVEBlobBoxXMax				= M_BOX_X_MAX,
	SVEBlobBoxXMin				= M_BOX_X_MIN,
	SVEBlobBoxYMax				= M_BOX_Y_MAX,
	SVEBlobBoxYMin				= M_BOX_Y_MIN,
	SVEBlobBreadth				= M_BREADTH,
	SVEBlobCompactness			= M_COMPACTNESS,
	SVEBlobConvexParameter		= M_CONVEX_PERIMETER,
	SVEBlobElongation			= M_ELONGATION,
	SVEBlobEulerNumber			= M_EULER_NUMBER,
	SVEBlobFeretElongation		= M_FERET_ELONGATION,
	SVEBlobFeretMaxAngle		= M_FERET_MAX_ANGLE,
	SVEBlobFeretMaxDia			= M_FERET_MAX_DIAMETER,
	SVEBlobFeretMeanDia			= M_FERET_MEAN_DIAMETER,
	SVEBlobFeretMinAngle		= M_FERET_MIN_ANGLE,
	SVEBlobFeretMinDia			= M_FERET_MIN_DIAMETER,
	SVEBlobFeretX				= M_FERET_X,
	SVEBlobFeretY				= M_FERET_Y,
	SVEBlobFirstPointX			= M_FIRST_POINT_X,
	SVEBlobFirstPointY			= M_FIRST_POINT_Y,
	SVEBlobGeneralFeret = M_FERET_GENERAL,
	SVEBlobGeneralMoment = M_MOMENT_GENERAL,
	SVEBlobIntercept			= M_INTERCEPT_0,
	SVEBlobIntercept45			= M_INTERCEPT_45,
	SVEBlobIntercept90			= M_INTERCEPT_90,
	SVEBlobIntercept135			= M_INTERCEPT_135,
	SVEBlobLabelValue			= M_LABEL_VALUE,
	SVEBlobLength				= M_LENGTH,
	SVEBlobNumberOfChainedPixels= M_NUMBER_OF_CHAINED_PIXELS,
	SVEBlobNumberOfHoles		= M_NUMBER_OF_HOLES,
	SVEBlobNumberOfRuns			= M_NUMBER_OF_RUNS,
	SVEBlobPerimeter			= M_PERIMETER,
	SVEBlobRoughness			= M_ROUGHNESS,
	SVEBlobXMaxAtYMax			= M_X_MAX_AT_Y_MAX,
	SVEBlobXMinAtYMin			= M_X_MIN_AT_Y_MIN,
	SVEBlobYMaxAtXMin			= M_Y_MAX_AT_X_MIN,
	SVEBlobYMinAtXMax			= M_Y_MIN_AT_X_MAX,
	SVEBlobSumPixel				= M_SUM_PIXEL,
	SVEBlobAxisPrincipalAngle	= M_AXIS_PRINCIPAL_ANGLE,
	SVEBlobAxisSecondaryAngle	= M_AXIS_SECONDARY_ANGLE,
	SVEBlobCenterOfGravityX		= M_CENTER_OF_GRAVITY_X,
	SVEBlobCenterOfGravityY		= M_CENTER_OF_GRAVITY_Y,

	SVEBlobMomentX0Y1			= M_MOMENT_X0_Y1,
	SVEBlobMomentX1Y0			= M_MOMENT_X1_Y0,
	SVEBlobMomentX1Y1			= M_MOMENT_X1_Y1,
	SVEBlobMomentX0Y2			= M_MOMENT_X0_Y2,
	SVEBlobMomentX2Y0			= M_MOMENT_X2_Y0,
	SVEBlobMomentCentralX0Y2	= M_MOMENT_CENTRAL_X0_Y2,
	SVEBlobMomentCentralX2Y0	= M_MOMENT_CENTRAL_X2_Y0,
	SVEBlobMomentCentralX1Y1	= M_MOMENT_CENTRAL_X1_Y1,
	SV_M_CENTER_BOUNDING_BOX_X = 116,
	SV_M_CENTER_BOUNDING_BOX_Y = 117,

};

/**
@SVObjectName Matrox Condition Enum

@SVObjectOverview This enum is used to control selections in both binarize Image operations and blob selection.

@SVObjectOperations None

*/
enum SVConditionEnum
{
	// Condition
	SVECondInRange				= M_IN_RANGE,
	SVECondOutRange				= M_OUT_RANGE,
	SVECondEqual				= M_EQUAL,
	SVECondNotEqual				= M_NOT_EQUAL,
	SVECondGreater				= M_GREATER,
	SVECondLess					= M_LESS,
	SVECondGreaterOrEqual		= M_GREATER_OR_EQUAL,
	SVECondLessOrEqual			= M_LESS_OR_EQUAL,
};

/**
@SVObjectName Matrox Blob Control Enum

@SVObjectOverview This enum is used to control criterion in a blob result.

@SVObjectOperations None

*/
enum SVBlobControlEnum
{
	// Selection 
	SVEBlobAll					= M_ALL_BLOBS,
	SVEBlobExcludeBlobs			= M_EXCLUDED_BLOBS,
	SVEBlobIncludeBlobs			= M_INCLUDED_BLOBS,
	SVEBlobSaveRuns				= M_SAVE_RUNS,
	SVEBlobIdentifier			= M_IDENTIFIER_TYPE,
};

/**
@SVObjectName Matrox Blob Operation Enum

@SVObjectOverview This enum is used to select blobs in a blob result.

@SVObjectOperations None

*/
enum SVBlobOperationEnum
{

	// Operation
	SVEBlobExclude				= M_EXCLUDE,
	SVEBlobExcludeOnly			= M_EXCLUDE_ONLY,
	SVEBlobInclude				= M_INCLUDE,
};

/**
@SVObjectName Matrox Bar Code Control Enum

@SVObjectOverview This enum is used to set and get information for the Matrox Bar Code..

@SVObjectOperations None

*/
enum SVBarCodeControlTypeEnum
{	
	//	SVBCBaseType				= 0x00000fff,

	SVBCStringType				= 0x10000000L,	// M_STRING
	SVBCTypeLong				= 0x20000000L,	// M_TYPE_LONG 

	SVBCCellNumberX				= 0x00000001L,	// M_CELL_NUMBER_X
	SVBCCellNumberY				= 0x00000002L,	// M_CELL_NUMBER_Y
	SVBCEncoding				= 0x00000004L,	// M_ENCODING
	SVBCErrorCorrection			= 0x00000008L,	// M_ERROR_CORRECTION
	SVBCCellSizeMin				= 0x00000010L,	// M_CELL_SIZE_MIN
	SVBCCellSizeMax				= 0x00000020L,	// M_CELL_SIZE_MAX
	SVBCSearchAngle				= 0x00000040L,	// M_SEARCH_ANGLE
	SVBCSearchAngleDeltaNeg		= 0x00000080L,	// M_SEARCH_ANGLE_DELTA_NEG
	SVBCSearchAngleDeltaPos		= 0x00000100L,	// M_SEARCH_ANGLE_DELTA_POS
	SVBCThreshold				= 0x00000200L,	// M_THRESHOLD
	SVBCCodeTimeout				= 0x00000400L,	// M_TIMEOUT
	SVBCBarCodeStatus			= 0x00000800L,	// M_STATUS
	SVBCBarCodeString			= 0x10001000L,	// M_STRING
	//-	JAB110708
	//	SVBCBarcodeStringSize		= 0x20002000L,	// M_STRING_SIZE
	SVBCBarcodeStringSize		= 0x00002000L,	// M_STRING_SIZE
	SVBCBarcodeScore			= 0x00004000L,	// M_SCORE
	SVBCSpeed					= 0x00008000L,	// M_SPEED
	SVBCForeGroundValue			= 0x00010000L,	// M_FOREGROUND_VALUE
	SVBCAutoThreshold			= 0x00020000L,	// M_THRESHOLD Set to M_DEFAULT
	SVBCAdaptiveThreshold       = 0x00040000L,	// M_THRESHOLD Set to M_ADAPTIVE
	SVBCUnEvenGrid				= 0x00080000L,  // M_DISTORTION set to M_UNEVEN_GRID_STEP 
};


/**
@SVObjectName Bar Code Types Enum

@SVObjectOverview This enum is used when creating a barcode.

@SVObjectOperations None

*/
enum SVBarCodeTypesEnum
{
	SVCode39					= M_CODE39, 
	SVDataMatrix				= M_DATAMATRIX, 
	SVEan13						= M_EAN13, 
	SVMaxiCode					= M_MAXICODE, 
	SVInterleaved25				= M_INTERLEAVED25, 
	SVCode128					= M_CODE128, 
	SVBC412						= M_BC412, 
	SVCodeABar					= M_CODABAR, 
	SVPDF417					= M_PDF417, 
	SVPostNet					= M_POSTNET,
	SVPlanet					= M_PLANET,
	SVUpcA						= M_UPC_A, 
	SVUpcE						= M_UPC_E, 
	SVPharmaCode				= M_PHARMACODE, 
	SVGS1Code					= M_GS1_DATABAR,
	SVQRCode					= M_QRCODE
};

/**
@SVObjectName Bar Code Status Enum

@SVObjectOverview This enum is used when getting status from the barcode result.

@SVObjectOperations None

*/
enum SVBarCodeStatusEnum
{
	SVBCStatusReadOK		= 0,			// M_STATUS_READ_OK 
	SVBCStatusCRCFailed		= 1,			// M_STATUS_CRC_FAILED 
	SVBCStatusECCUnknown	= 2,			// M_STATUS_ECC_UNKNOWN 
	SVBCStatusENCUnknown	= 3,			// M_STATUS_ENC_UNKNOWN 
	SVBCStatusNotFound		= 6,			// M_STATUS_NOT_FOUND 
};

/**
@SVObjectName Matrox Display Control Enum

@SVObjectOverview This enum is used to setup a Matrox Display.

@SVObjectOperations None

*/
enum SVDisplayCntrlEnum
{
	SVDispDouble                    = 0x10000L, // Double Stuff
	SVDispUpdate					= 0x00001L, // 3199L, // M_UPDATE
	SVDispWindowMove				= 0x00002L, // 3061L, // M_WINDOW_MOVE
	SVDispWindowResize				= 0x00004L, // 3052L, // M_WINDOW_RESIZE
	SVDispWindowZoom				= 0x00010L, // 3051L, // M_WINDOW_ZOOM
	SVDispSelected					= 0x00020L, // 3006L, // M_SELECTED
	SVDispZoomFactorX				= 0x10040L, // 3002L, // M_ZOOM_FACTOR_X
	SVDispZoomFactorY				= 0x10080L, // 3003L, // M_ZOOM_FACTOR_Y 
	SVDispPanX						= 0x10100L, // 3000L, // M_PAN_OFFSET_X
	SVDispPanY						= 0x10200L, // 3001L, // M_PAN_OFFSET_Y
	SVDispWindowOvrWrite			= 0x00400L, // 3016L, // M_WINDOW_OVR_WRITE
	SVDispWindowOvrBufID			= 0x00800L, // 3015L, // M_WINDOW_OVR_BUF_ID
	SVDispKeyColor					= 0x01000L, // 3010L, // M_KEY_COLOR
};

// ************************** Ocr Related enums *********************
/**
@SVObjectName Matrox Ocr Type Enum

@SVObjectOverview This enum is used when Creating a Matrox Ocr.

@SVObjectOperations None

*/
enum SVOcrTypeEnum
{	
	// Font types
	SVBasics					= 0x000000ff,
	SVSemiM1292					= 0x1L,			// M_SEMI_M12_92 
	SVSemiM1388					= 0x2L,			// M_SEMI_M13_88 
	SVUserDefined				= 21L,			// M_USER_DEFINED 

	// Combination Constants
	SVCombine1					= 0xf0000000,
	SVConstrained				= 0x40000000L,	// M_CONSTRAINED 
	SVGeneral					= 0x20000000L,	// M_GENERAL 
	SVUserDefinedConstrained	= SVUserDefined | SVConstrained,
};

/**
@SVObjectName Matrox Ocr Init Flag Enum

@SVObjectOverview This enum is used when Creating a Matrox Ocr.

@SVObjectOperations None

*/
enum SVOcrInitFlag
{
	SVOcrForegroundBlack		= 0x100L,		// M_FOREGROUND_BLACK 
	SVOcrForegroundWhite		= 0x80L,		// M_FOREGROUND_WHITE 
};

/**
@SVObjectName Matrox Ocr Operation Enum

@SVObjectOverview This enum is used for the Ocr CopyFont, SaveFont, and RestoreFont.

@SVObjectOperations None

*/
enum SVOcrOperationEnum
{
	SVOcrOperation				= 0x00ff,
	SVOcrCopyFromFont			= 2L,			// M_COPY_FROM_FONT 
	SVOcrCopytoFont				= 1L,			// M_COPY_TO_FONT 
	SVOcrLoadConstraint			= 0x2L,			// M_LOAD_CONSTRAINT 
	SVOcrLoadControl			= 0x4L,			// M_LOAD_CONTROL 
	SVOcrDefault				= 0x10000000L,	// M_DEFAULT 
	SVOcrSave					= 0x100L,		// M_SAVE 
	SVOcrSaveConstraint			= 0x200L,		// M_SAVE_CONSTRAINT 
	SVOcrSaveControl			= 0x400L,		// M_SAVE_CONTROL 

	SVOcrAllChar				= 0x8000L,		// M_ALL_CHAR 

	SVOcrSort					= 0x00200000L,	// M_SORT 
	SVOcrRestore				= 0x10L,			// M_RESTORE

};


/**
@SVObjectName Matrox Ocr Control Enum

@SVObjectOverview This enum is used for the Matrox Ocr Font.

@SVObjectOperations None

*/
enum SVOcrControlEnum
{
	// MocrControl - For the charactersistics of the font
	SVOcrCharErase = 45L,   // M_CHAR_ERASE

	// MocrControl - For operation controls for read and verify operations
	SVOcrBlankCharacters              =  73L,   // M_BLANK_CHARACTERS
	SVOcrBrokenChar                   = 104L,   // M_BROKEN_CHAR
	SVOcrCharAcceptance               =  25L,   // M_CHAR_ACCEPTANCE
	SVOcrCharInvalid                  =  26L,   // M_CHAR_INVALID
	SVOcrContextConvert               = 535L,   // M_CONTEXT_CONVERT
	SVOcrMorphologicFiltering         =  43L,   // M_MORPHOLOGIC_FILTERING
	SVOcrSkipStringLocation           =  22L,   // M_SKIP_STRING_LOCATION
	SVOcrSpeed                        =   8L,   // M_SPEED
	SVOcrStringAcceptance             =  24L,   // M_STRING_ACCEPTANCE
	SVOcrStringAngleInterpolationMode =  88L,   // M_STRING_ANGLE_INTERPOLATION_MODE

	// MocrControl - For the characteristics of the target characters
	SVOcrCharPositionVariationX =  91L,   // M_CHAR_POSITION_VARIATION_X
	SVOcrCharPositionVariationY =  92L,   // M_CHAR_POSITION_VARIATION_Y
	SVStringAngle               =  87L,   // M_STRING_ANGLE
	SVOcrStringAngleDeltaNeg    = 106L,   // M_STRING_ANGLE_DELTA_NEG
	SVOcrStringAngleDeltaPos    = 107L,   // M_STRING_ANGLE_DELTA_POS
	SVOcrStringNumber           =  74L,   // M_STRING_NUMBER
	SVTargetCharSizeX           =  27L,   // M_TARGET_CHAR_SIZE_X
	SVTargetCharSizeY           =  28L,   // M_TARGET_CHAR_SIZE_Y
	SVOcrTargetCharSpacing      =  29L,   // M_TARGET_CHAR_SPACING
	SVOcrTextStringSeparator    = 109L,   // M_TEXT_STRING_SEPARATOR
	SVOcrThickenChar            =  84L,   // M_THICKEN_CHAR
	SVOcrTouchingChar           = 105L,   // M_TOUCHING_CHAR

	// MocrInquire - MocrAllocFont function parameter 
	SVFontType                =  31L,   // M_FONT_TYPE
	SVCharNumber              =  32L,   // M_CHAR_NUMBER
	SVCharCellSizeX           =  33L,   // M_CHAR_CELL_SIZE_X
	SVCharCellSizeY           =  34L,   // M_CHAR_CELL_SIZE_Y
	SVCharOffsetX             =  35L,   // M_CHAR_OFFSET_X
	SVCharOffsetY             =  36L,   // M_CHAR_OFFSET_Y
	SVOcrCharSizeX            =  37L,   // M_CHAR_SIZE_X
	SVOcrCharSizeY            =  38L,   // M_CHAR_SIZE_Y
	SVCharThickness           =  39L,   // M_CHAR_THICKNESS
	SVOcrStringSizeMax        =  62L,   // M_STRING_SIZE_MAX 
	SVOcrForegroundValue      =   4L,   // M_FOREGROUND_VALUE

	// MocrInquire - other parameters
	SVCharInFont            = 42L,   // M_CHAR_IN_FONT
	SVCharNumberInFont      = 47L,   // M_CHAR_NUMBER_IN_FONT
	SVOcrIsFontPreprocessed = 14L,   // M_PREPROCESSED

	SVContext       = 0x08000000L,   // M_CONTEXT
	SVOcrStringSize = 0x00008003L,   // M_STRING_SIZE

};

/**
@SVObjectName Matrox Ocr Result Enum

@SVObjectOverview This enum is used for the Matrox Ocr results.

@SVObjectOperations None

*/
enum SVOcrResultEnum
{
	// Result types
	SVOcrStringValidFlag		= 1L,			// M_STRING_VALID_FLAG  
	SVOcrString					= 3L,			// M_STRING
	SVOcrCharValidFlag			= 4L,			// M_CHAR_VALID_FLAG
	SVOcrCharScore				= 5L,			// M_CHAR_SCORE
	SVOcrCharPosX				= 6L,			// M_CHAR_POSITION_X
	SVOcrCharPosY				= 7L,			// M_CHAR_POSITION_Y
	SVOcrCharSpacing			= 14L,			// M_CHAR_SPACING
	SVOcrTextScore				= 15L,			// M_TEXT_SCORE
	SVOcrResultText				= 16L,			// M_TEXT
	SVOcrResultTextLen			= 17L,			// M_TEXT_LENGTH
	SVOcrStringAllocSize		= 18L,			// M_STRING_ALLOC_SIZE
	SVOcrThreshold				= 82L,			// M_THRESHOLD  
	SVOcrNbrString				= 74L,			// M_NB_STRING
	SVOcrResultStringSize		= 0x00008003L , // M_STRING_SIZE
	SVOcrStringScore			= 0x00001400L , // M_STRING_SCORE
	SVOcrSum					= 0x00004000L , // M_SUM
};

//************************** Pattern Module *****************

/**
@SVObjectName Matrox Pattern Model Allocation Type Enum

@SVObjectOverview This enum is used for specifying the Matrox Pattern Model Allocation parameter.

@SVObjectOperations None

*/
enum SVPatternModelAllocationTypeEnum
{
	SVPatModelTypeNormalized		= 0x00000002L, // M_NORMALIZED
	SVPatModelTypeCircularOverscan	= 0x00010000L  // M_CIRCULAR_OVERSCAN
};

/**
@SVObjectName Matrox Pattern Type Enum

@SVObjectOverview This enum is used for setting a Matrox Pattern Model.

@SVObjectOperations None

*/
enum SVPatternTypeEnum
{
	SVPatBaseType				= 0x0000ffff,
	SVPatResultType				= 0x01000000,
	// Model Parameter
	SVPatSearchAngle			= M_SEARCH_ANGLE,
	SVPatSearchAngleDeltaNeg	= M_SEARCH_ANGLE_DELTA_NEG,
	SVPatSearchAngleDeltaPos	= M_SEARCH_ANGLE_DELTA_POS,
	SVPatSearchAngleMode		= M_SEARCH_ANGLE_MODE,
	SVPatSearchAngleAccuracy	= M_SEARCH_ANGLE_ACCURACY,
	SVPatSearchAngleInterpMode	= M_SEARCH_ANGLE_INTERPOLATION_MODE,
	SVPatSearchAngleTolerance	= M_SEARCH_ANGLE_TOLERANCE,
}; 

/**
@SVObjectName Matrox Pattern Serch Parameter Enum

@SVObjectOverview This enum is used for setting Matrox Pattern Search Parameters.

@SVObjectOperations None

*/

enum SVPatternSearchParameterTypeEnum
{
	// Search Parameters (MpatSearchParameters)
	SVPatFirstLevel				= 31L, // M_FIRST_LEVEL
	SVPatLastLevel				= 32L, // M_LAST_LEVEL
	SVPatModelStep				= 33L, // M_MODEL_STEP
	SVPatFastFind				= 34L, // M_FAST_FIND
	SVPatMinSpacingX			= 35L, // M_MIN_SPACING_X
	SVPatMinSpacingY			= 36L, // M_MIN_SPACING_Y
	SVPatCoarseSearchAcceptance	= 41L, // M_COARSE_SEARCH_ACCEPTANCE
	SVPatExtraCandidates		= 46L, // M_EXTRA_CANDIDATES
};

/**
@SVObjectName Matrox Pattern Result Enum

@SVObjectOverview This enum is used for setting a Matrox Pattern Results.

@SVObjectOperations None

*/
enum SVPatternResultEnum
{
	// Result types
	SVCountList					= 0x00020000L , // M_COUNT_LIST
	SVEventList					= 0x00008000L , // M_EVENT_LIST
	SVExtremeList				= 0x00010000L , // M_EXTREME_LIST
	SVHistList					= 0x00002000L , // M_HIST_LIST
	SVProjList					= 0x00004000L , // M_PROJ_LIST

	SVPatAngle					= 0x00000800L , // M_ANGLE 
	SVPatIndex					= 0x00005400L , // M_MODEL_INDEX 
	SVPatPosX					= 0x00003400L , // M_POSITION_X 
	SVPatPosY					= 0x00004400L , // M_POSITION_Y 
	SVPatScore					= 0x00001400L , // M_SCORE 
};

enum SVImageOperationTypeEnum
{

	// Combination Constant
	SVImageSigned				= 0x80000000L, // M_SIGNED
	SVImageFloatType			= 0x40000000L | SVImageSigned, // M_FLOAT
	SVImageProcess				= 0x02000000,
	SVImageOperation			= 0x04000000,
	SVImageBasic				= 0x00000fff, 

	// Image Result Create Flags
	SVImageCountList			= 0x00020000L, // M_COUNT_LIST 
	SVImageEventList			= 0x00008000L, // M_EVENT_LIST 
	SVImageEventListFloat		= SVImageEventList | SVImageFloatType,
	SVImageExtreemList			= 0x00010000L, // M_EXTREME_LIST 
	SVImageExtreemListFloat		= SVImageExtreemList | SVImageFloatType,
	SVImageHistList				= 0x00002000L, // M_HIST_LIST 
	SVImageProjList				= 0x00004000L, // M_PROJ_LIST 
	SVImageProjListFloat		= SVImageProjList  | SVImageFloatType,


	// Image Processing Flags
	SVImageMultipleAccumulate	= 0x0004L , // M_MULTIPLY_ACCUMULATE_2
	SVImageAdd					= 0x0000L , // M_ADD 
	SVImageAnd					= 0x0017L , // M_AND
	SVImageDiv					= 0x0101L , // M_DIV 
	SVImageMax					= 0x04000000L , // M_MAX 
	SVImageMin					= 0x02000000L , // M_MIN
	SVImageMult					= 0x0100L , // M_MULT
	SVImageNand					= 0x001AL , // M_NAND
	SVImageNor					= 0x0019L , // M_NOR
	SVImageOr					= 0x0016L , // M_OR
	SVImageSub					= 0x0001L , // M_SUB
	SVImageSubAbs				= 0x0011L , // M_SUB_ABS 
	SVImageXNor					= 0x001BL , // M_XNOR 
	SVImageNot					= 0x001CL , // M_NOT

	// Image Operation
	SVImageGrayScale			= 0x0200L ,	// M_GRAYSCALE
	SVImageBinary				= 0x00001000L ,	// M_BINARY


	// Image Conversion Flags
	SVImageRGBToHLS				= 1L, // M_RGB_TO_HLS 
	SVImageHLSToRGB				= 3L, // M_HLS_TO_RGB 
	SVImageRGBToL				= 4L, // M_RGB_TO_L 

	// Image Operation
	SVPolarToRect				= 0x00000002L,		// M_POLAR_TO_RECTANGULAR
	SVRectToPolar				= 0x00000001L,		// M_RECTANGULAR_TO_POLAR

	// Image Interpolation
	SVBilinear					= 0x00000008L,	// M_BILINEAR 
	SVBiCubic					= 0x00000010L,	// M_BICUBIC
	SVNearestNeighbor			= 0x00000040L,	// M_NEAREST_NEIGHBOR 
	SVImageDefault				= 0x10000000L,	// M_DEFAULT

	// Image Result
	SVImageValue				= 0L,			// M_VALUE

	SVImageXOr					= 0x0018L,		// M_XOR 
	SVImagePass					= 0x0002L,		// M_PASS

	SVOverScanClear				= 0x00000080L,	// M_OVERSCAN_CLEAR
	SVOverScanEnable			= 0x00008000L,	// M_OVERSCAN_ENABLE
	SVImageDefaultKernel		= 0x00100000L,	// M_ID_OFFSET_OF_DEFAULT_KERNEL 
	SVOverScanDisable			= SVImageDefaultKernel >> 1L,// M_OVERSCAN_DISABLE
	SVSaturation				= 0x2000L,		// M_SATURATION

	// Combinations.

	SVImageAddSaturation		= SVImageAdd | SVSaturation,
	SVImageSubSaturation		= SVImageSub | SVSaturation,
	SVImageMulSaturation		= SVImageMult | SVSaturation,


	SVNearestNeighOverScanClear = SVNearestNeighbor | SVOverScanClear,
	SVNearestNeighOverScanEnable = SVNearestNeighbor | SVOverScanEnable,
	SVNearestNeighOverScanDisable = SVNearestNeighbor | SVOverScanDisable,

	SVBilinearOverScanClear = SVBilinear | SVOverScanClear,
	SVBilinearOverScanEnable = SVBilinear | SVOverScanEnable,
	SVBilinearOverScanDisable = SVBilinear | SVOverScanDisable,

	SVBiCubicOverScanClear = SVBiCubic | SVOverScanClear,
	SVBiCubicOverScanEnable = SVBiCubic | SVOverScanEnable,
	SVBiCubicOverScanDisable = SVBiCubic | SVOverScanDisable,
};


/**
@SVObjectName Matrox Image Flip Enum

@SVObjectOverview This enum is used to select the type of flip for the Flip function.

@SVObjectOperations None

*/
enum SVImageFlipEnum
{
	// Image Flip Flags
	SVImageFlipVertical			= 1L, // M_FLIP_VERTICAL 
	SVImageFlipHorizontal		= 2L, // M_FLIP_HORIZONTAL 
};


/**
@SVObjectName Matrox Image Water Shed Enum

@SVObjectOverview This enum is used to set behavior in the watershed filter.

@SVObjectOperations None

*/
enum SVImageWaterShedEnum
{
	SVImageWSWatershed			= 0x0001, // M_WATERSHED 
	SVImageWSBasin				= 0x0002, // M_BASIN 
	// Combination Constants
	SVImageWSMinimaFill			= 0x0004, // M_MINIMA_FILL 
	SVImageWSMaximaFill			= 0x0008, // M_MAXIMA_FILL 
	// Combination
	SVImageWSRegular			= 0x00020000L, // M_REGULAR 
	SVImageWSStraight			= 0x0080, // M_STRAIGHT_WATERSHED 
	// Combination
	SVImage4Connected			= 0x00000010L, // M_4_CONNECTED 
	SVImage8Connected			= 0x00000020L, // M_8_CONNECTED 
	// Combination
	SVImageSkipLastLevel		= 0x0100, // M_SKIP_LAST_LEVEL 
	// Combination
	// Combination
	SVImageFillSource			= 0x0200, // M_FILL_SOURCE 

	SVWaterShedDefault			= 0x10000000L, //M_DEFAULT
	// Combination
};

/**
@SVObjectName Matrox Image Water Shed Enum

@SVObjectOverview This enum is used to set behavior in the watershed filter.

@SVObjectOperations None

*/
enum SVFilterOperationEnum
{
	SVFilterOpEdge				= SVImageDefaultKernel + 7L,	// M_EDGE_DETECT 
	SVFilterOpEdgeDet2			= SVImageDefaultKernel + 8L,	// M_EDGE_DETECT2 
	SVFilterOpHorizEdge 		= SVImageDefaultKernel + 5L,	// M_HORIZ_EDGE 
	SVFilterOpLaplacianEdge 	= SVImageDefaultKernel + 1L,	// M_LAPLACIAN_EDGE 
	SVFilterOpLaplacianEdge2 	= SVImageDefaultKernel + 2L,	// M_LAPLACIAN_EDGE2 
	SVFilterOpSharpen 			= SVImageDefaultKernel + 3L,	// M_SHARPEN 
	SVFilterOpSharpen2 			= SVImageDefaultKernel + 4L,	// M_SHARPEN2 
	SVFilterOpSmooth 			= SVImageDefaultKernel + 0L,	// M_SMOOTH 
	SVFilterOpVertEdge			= SVImageDefaultKernel + 6L,	// M_VERT_EDGE 
};

