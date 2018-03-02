//*****************************************************************************
// \copyright COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the central location for strings used in the Definitions project and also if this string is used in different projects.
//******************************************************************************

#pragma once

namespace SvDef
{
	const TCHAR* const WatershedMarkerImageConnectionName = _T( "WatershedFilterMarkerImage" );
	const TCHAR* const ImageAnalyzerImageName = _T( "ImageAnalyzerImage" );
	const TCHAR* const cColorToolInputImage = _T("ColorToolInputImage");
	const TCHAR* const PatternModelImageName = _T( "Model Image" );
	const TCHAR* const PatternDontCareImageName = _T( "Dont care Image" );
	const TCHAR* const TableClearEquationName = _T("ClearConditionalFormula");
	const TCHAR* const cInputTag_SourceTable = _T( "SourceTable" );

	const TCHAR* const cTrue = _T("TRUE");
	const TCHAR* const cFalse = _T("FALSE");

	const TCHAR* const SVO_PRODUCT_KONTRON_X2_GD1A = _T( "SVIM X2-GD1A" );
	const TCHAR* const SVO_PRODUCT_KONTRON_X2_GD2A =  _T("SVIM X2-GD2A");
	const TCHAR* const SVO_PRODUCT_KONTRON_X2_GD4A = _T("SVIM X2-GD4A");
	const TCHAR* const SVO_PRODUCT_KONTRON_X2_GD8A =  _T("SVIM X2-GD8A");
	const TCHAR* const SVO_PRODUCT_KONTRON_X2_GD8A_NONIO = _T("SVIM Non I/O");

	const TCHAR* const cBucketTag = _T("Array");			// for backwards compatibility
	const TCHAR* const cArrayTag = _T("Array_Elements");	// new style; one bucket, all array values
} //namespace SvDef