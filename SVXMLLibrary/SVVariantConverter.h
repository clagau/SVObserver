//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVVariantConverter
//* .File Name       : $Workfile:   SVVariantConverter.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:48:20  $
//******************************************************************************
#pragma once

class SVVariantConverter
{
public:
	// TranslateVariant () ------------------------------------------------------
	// This function will allocate the associated BSTRs.
	// avpVariant - input -
	// abstrpValue - output -
	// abstrpType - output - 
	//
	//	Return Value - 
	//   2 - avpValue equals NULL
	//
	static HRESULT TranslateVariant(VARIANT* avpValue, BSTR* abstrpValue, BSTR* abstrpType);

	// RestoreVariant () ------------------------------------------------------
	// If necessary, this function will allocate BSTRs for VT_BSTR types.
	// abstrValue - input -
	// abstrType - input - 
	// avpVariant - output -
	static HRESULT RestoreVariant(BSTR abstrValue, BSTR abstrType, VARIANT* avpValue);

private:
	SVVariantConverter(); // Non constructable
	~SVVariantConverter();
};

