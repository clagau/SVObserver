//*****************************************************************************
// \copyright COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the central location for strings used in the Definitions project and also if this string is used in different projects.
//******************************************************************************

#pragma once

namespace SvDef
{
const TCHAR* const WatershedMarkerImageConnectionName = _T("WatershedFilterMarkerImage");
const TCHAR* const SeedImageConnectionName = _T("SeedImage");
const TCHAR* const ImageAnalyzerImageName = _T("ImageAnalyzerImage");
const TCHAR* const cColorToolInputImage = _T("ColorToolInputImage");
const TCHAR* const PatternModelImageName = _T("Model Image");
const TCHAR* const PatternDontCareImageName = _T("Dont care Image");
const TCHAR* const MaskImageName = _T("Mask Image");
const TCHAR* const ReferenceImageName = _T("Reference Image");
const TCHAR* const TableClearEquationName = _T("ClearConditionalFormula");
const TCHAR* const cInputTag_SourceTable = _T("SourceTable");

const TCHAR* const cTrue = _T("TRUE");
const TCHAR* const cFalse = _T("FALSE");

const TCHAR* const SVO_PRODUCT_KONTRON_X2_GD1A = _T("SVIM X2-GD1A");
const TCHAR* const SVO_PRODUCT_KONTRON_X2_GD2A = _T("SVIM X2-GD2A");
const TCHAR* const SVO_PRODUCT_KONTRON_X2_GD4A = _T("SVIM X2-GD4A");
const TCHAR* const SVO_PRODUCT_KONTRON_X2_GD8A = _T("SVIM X2-GD8A");
const TCHAR* const SVO_PRODUCT_KONTRON_X2_GD8A_NONIO = _T("SVIM Non I/O");

const TCHAR* const cBucketTag = _T("Array");			// for backwards compatibility
const TCHAR* const cArrayTag = _T("Array_Elements");	// new style; one bucket, all array values

const TCHAR* const cModuleReady = _T("Module Ready");
const TCHAR* const cRaidErrorIndicator = _T("Raid Error Indicator");

const TCHAR* const cLinkName = _T(" Link");

const TCHAR* const Auto = _T("Auto");
const TCHAR* const Bicubic = _T("Bicubic");
const TCHAR* const Bilinear = _T("Bilinear");
const TCHAR* const NearestNeighbor = _T("Nearest Neighbor");
const TCHAR* const Enabled = _T("Enabled");
const TCHAR* const Disabled = _T("Disabled");
const TCHAR* const Fast = _T("Fast");
const TCHAR* const Precise = _T("Precise");
const TCHAR* const ResizeImage = _T("ResizeImage");

const TCHAR* const cPpqFixedName = _T("PPQ_");
const TCHAR* const cTriggerFixedName = _T("Trigger_");
const TCHAR* const cCameraFixedName = _T("Camera_");
const TCHAR* const cInspectionFixedName = _T("Inspection_");

const TCHAR* const cInputTag_SortColumn = _T("SortColumn");
const TCHAR* const cInputTag_ExcludeColumn = _T("ExcludeColumn");
const TCHAR* const cInputTag_NewColumn = _T("NewColumn");
const TCHAR* const cInputTag_LongResultValue = _T("LongResultValue");

const TCHAR* const ARCHIVE_TOOL_MEMORY_POOL_ONLINE_ASYNC_NAME = _T("ArchiveToolOnlineAsync");
const TCHAR* const ARCHIVE_TOOL_MEMORY_POOL_GO_OFFLINE_NAME = _T("ArchiveToolGoOffline");

const TCHAR* const SizeAdjustTextNone = _T("Manual");
const TCHAR* const SizeAdjustTextFormula = _T("Formula");
const TCHAR* const SizeAdjustTextFullSize = _T("Auto Fit");
} //namespace SvDef
