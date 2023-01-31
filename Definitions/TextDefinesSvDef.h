//*****************************************************************************
// \copyright COPYRIGHT (c) 2016 by Körber Pharma Inspection GmbH. All Rights Reserved
// All Rights Reserved
/// \file TextDefinesSvDef.h
//*****************************************************************************
// This is the central location for strings used in the Definitions project and also if this string is used in different projects.
//******************************************************************************

#pragma once

namespace SvDef
{
constexpr const char* SourceImageInputName = _T("SourceImage");
constexpr const char* WatershedMarkerImageConnectionName = _T("WatershedFilterMarkerImage");
constexpr const char* SeedImageConnectionName = _T("SeedImage");
constexpr const char* ImageAnalyzerImageName = _T("ImageAnalyzerImage");
constexpr const char* cColorToolInputImage = _T("ColorToolInputImage");
constexpr const char* PatternModelImageName = _T("Model Image");
constexpr const char* PatternDontCareImageName = _T("Dont care Image");
constexpr const char* MaskImageName = _T("Mask Image");
constexpr const char* ReferenceImageName = _T("Reference Image");
constexpr const char* TableClearEquationName = _T("ClearConditionalFormula");
constexpr const char* cInputTag_SourceTable = _T("SourceTable");
constexpr const char* GrayImageConnectionName = _T("Gray Image");
constexpr const char* CustomFeatureEquationName = _T("CustomFeatureFormula");

constexpr const char* cTrue = _T("TRUE");
constexpr const char* cFalse = _T("FALSE");

constexpr const char* SVO_PRODUCT_SVIM_X2_GD1A = _T("SVIM X2-GD1A");
constexpr const char* SVO_PRODUCT_SVIM_X2_GD2A = _T("SVIM X2-GD2A");
constexpr const char* SVO_PRODUCT_SVIM_X2_GD4A = _T("SVIM X2-GD4A");
constexpr const char* SVO_PRODUCT_SVIM_X2_GD8A = _T("SVIM X2-GD8A");
constexpr const char* SVO_PRODUCT_SVIM_NEO = _T("SVIM NEO");

constexpr const char* cBucketTag = _T("Array");			// for backwards compatibility
constexpr const char* cArrayTag = _T("Array_Elements");	// new style; one bucket, all array values

constexpr const char* cModuleReady = _T("Module Ready");

constexpr const char* cLinkName = _T(" Link");

constexpr const char* Interpolate = _T("Interpolate");
constexpr const char* Bicubic = _T("Bicubic");
constexpr const char* Bilinear = _T("Bilinear");
constexpr const char* NearestNeighbor = _T("Nearest Neighbor");
constexpr const char* Enabled = _T("Enabled");
constexpr const char* Disabled = _T("Disabled");

constexpr const char* cPpqFixedName = _T("PPQ_");
constexpr const char* cTriggerFixedName = _T("Trigger_");
constexpr const char* cCameraFixedName = _T("Camera_");
constexpr const char* cInspectionFixedName = _T("Inspection_");

constexpr const char* cInputTag_SortColumn = _T("SortColumn");
constexpr const char* cInputTag_ExcludeColumn = _T("ExcludeColumn");
constexpr const char* cInputTag_NewColumn = _T("NewColumn");
constexpr const char* cInputTag_DeleteColumn = _T("DeleteColumn");
constexpr const char* cInputTag_LongResultValue = _T("LongResultValue");

constexpr const char* SizeAdjustTextNone = _T("Manual");
constexpr const char* SizeAdjustTextFormula = _T("Formula");
constexpr const char* SizeAdjustTextAutoFitSize = _T("Auto Fit");
constexpr const char* fileDlgFilterMilSupportedImageFilesTypes = _T("All Suitable Image Files (BMP, PNG, TIFF, MIM)|*.bmp; *.png; *.tif; *.mim|Bitmap Files(*.bmp)|*.bmp|PNG Files (*.png)|*.png|TIFF Files (*.tif)|*.tif|MIL Image Files (*.mim)|*.mim||");




static std::initializer_list<const char*> c_allInterpolationModeNames
{
	SvDef::Interpolate,
	SvDef::Bicubic,
	SvDef::Bilinear,
	SvDef::NearestNeighbor
};

static std::initializer_list<const char*> c_allUsedOverscanNames
{
	SvDef::Enabled,
	SvDef::Disabled
};

} //namespace SvDef
