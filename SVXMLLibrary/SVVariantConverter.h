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
#ifndef INCL_SVVARIANTCONVERTER_H
#define INCL_SVVARIANTCONVERTER_H

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

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVXMLLibrary\SVVariantConverter.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:48:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Oct 2012 10:40:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code and projects to fix whitespace issues in the XML file and consolidate functionality for load and saving to this library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/