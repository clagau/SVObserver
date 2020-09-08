//*****************************************************************************
// \copyright COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the central location for strings used in the Definitions project and also if this string is used in different projects.
//******************************************************************************

#pragma once

namespace SvDef
{
constexpr char* WatershedMarkerImageConnectionName = _T("WatershedFilterMarkerImage");
constexpr char* SeedImageConnectionName = _T("SeedImage");
constexpr char* ImageAnalyzerImageName = _T("ImageAnalyzerImage");
constexpr char* cColorToolInputImage = _T("ColorToolInputImage");
constexpr char* PatternModelImageName = _T("Model Image");
constexpr char* PatternDontCareImageName = _T("Dont care Image");
constexpr char* MaskImageName = _T("Mask Image");
constexpr char* ReferenceImageName = _T("Reference Image");
constexpr char* TableClearEquationName = _T("ClearConditionalFormula");
constexpr char* cInputTag_SourceTable = _T("SourceTable");
constexpr char* GrayImageConnectionName = _T("Gray Image");
constexpr char* CustomFeatureEquationName = _T("CustomFeatureFormula");

constexpr char* cTrue = _T("TRUE");
constexpr char* cFalse = _T("FALSE");

constexpr char* SVO_PRODUCT_SVIM_X2_GD1A = _T("SVIM X2-GD1A");
constexpr char* SVO_PRODUCT_SVIM_X2_GD2A = _T("SVIM X2-GD2A");
constexpr char* SVO_PRODUCT_SVIM_X2_GD4A = _T("SVIM X2-GD4A");
constexpr char* SVO_PRODUCT_SVIM_X2_GD8A = _T("SVIM X2-GD8A");
constexpr char* SVO_PRODUCT_SVIM_X2_GD8A_NONIO = _T("SVIM Non I/O");
constexpr char* SVO_PRODUCT_SVIM_NEO1 = _T("SVIM NEO1");

constexpr char* cBucketTag = _T("Array");			// for backwards compatibility
constexpr char* cArrayTag = _T("Array_Elements");	// new style; one bucket, all array values

constexpr char* cModuleReady = _T("Module Ready");
constexpr char* cRaidErrorIndicator = _T("Raid Error Indicator");

constexpr char* cLinkName = _T(" Link");

constexpr char* Auto = _T("Auto");
constexpr char* Bicubic = _T("Bicubic");
constexpr char* Bilinear = _T("Bilinear");
constexpr char* NearestNeighbor = _T("Nearest Neighbor");
constexpr char* Enabled = _T("Enabled");
constexpr char* Disabled = _T("Disabled");
constexpr char* Fast = _T("Fast");
constexpr char* Precise = _T("Precise");
constexpr char* ResizeImage = _T("ResizeImage");

constexpr char* cPpqFixedName = _T("PPQ_");
constexpr char* cTriggerFixedName = _T("Trigger_");
constexpr char* cCameraFixedName = _T("Camera_");
constexpr char* cInspectionFixedName = _T("Inspection_");

constexpr char* cInputTag_SortColumn = _T("SortColumn");
constexpr char* cInputTag_ExcludeColumn = _T("ExcludeColumn");
constexpr char* cInputTag_NewColumn = _T("NewColumn");
constexpr char* cInputTag_LongResultValue = _T("LongResultValue");

constexpr char* ARCHIVE_TOOL_MEMORY_POOL_ONLINE_ASYNC_NAME = _T("ArchiveToolOnlineAsync");
constexpr char* ARCHIVE_TOOL_MEMORY_POOL_GO_OFFLINE_NAME = _T("ArchiveToolGoOffline");

constexpr char* SizeAdjustTextNone = _T("Manual");
constexpr char* SizeAdjustTextFormula = _T("Formula");
constexpr char* SizeAdjustTextFullSize = _T("Auto Fit");
} //namespace SvDef
