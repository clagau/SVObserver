// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOCMArchive
// * .File Name       : $Workfile:   SVOCMArchive.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   03 Sep 2014 15:51:26  $
// ******************************************************************************

#pragma once

//Moved to precompiled header: #include <comdef.h>
#include "SVMessage/SVMessage.h"

class SVOCMArchive
{
private:
	SVOCMArchive();
	virtual ~SVOCMArchive();

public:
	template< typename SVTreeType >
	static HRESULT CreateTreeFromConfigurationFile (unsigned long ulSVOVersion, 
		unsigned long ulSECVersion, 
		BSTR pstrFilename,
		SVTreeType& p_rTree);

	// CreateConfigurationFileFromTree () ---------------------------------------
	//
	// ulSVOVersion - input -
	//	  It is intended that this parameter contain the current 
	//   SVObserver version number.
	//
	// bstrRevisionHistory - input - 
	//   It is intended that this parameter contain the XML string which 
	//   represents the revision history as it was last pulled from the 
	//   configuration.  This revision history excludes the SVObserver instance.
	//	  The current SVObserver instance should not be added to the list until
	//   the actual save.  This should be done automatically by SVXMLClass.
	//
	//	  This parameter can be NULL if there is no revision history.
	//
	//	ulSECVersion - input - 
	//   It is intended that this parameter contain the SVObserver version file 
	//   format that is desired in the saved file.  This functionality is not 
	//   currently implemented, so this parameter is not used.  Potential future 
	//   implementation.
	template< typename SVTreeType >
	static HRESULT CreateConfigurationFileFromTree(
		unsigned long	ulSVOVersion, 
		BSTR			bstrRevisionHistory,
		unsigned long	ulSECVersion, 
		LPCTSTR			pstrFilename,
		SVTreeType& p_rTree);

	// GetSVOSavedVersionNumberFromConfigurationFile () -------------------------
	// This function currently only works on SVX files.
	static HRESULT GetSVOSavedVersionNumberFromConfigurationFile (
		BSTR				abstrFileName, 
		unsigned long*	apulSVOConfigVersion);

protected:
	//	FindVersion401_430 () ----------------------------------------------------
	// If a version number is not found, this function will set 
	// aplConfigurationVersion to zero.
	static HRESULT FindVersion401_430 (char* apczSourceString, 
		unsigned long* aplConfigurationVersion);

	//	FindVersion440_ () -------------------------------------------------------
	// If a version number is not found, this function will set 
	// aplConfigurationVersion to zero.
	static HRESULT FindVersion440_ (char* apczSourceString, 
		unsigned long* aplConfigurationVersion);

	// FindVersion440__Standard () ----------------------------------------------
	// Only called from FindVersion440_ ().  
	static HRESULT FindVersion440__Standard (char* apczSourceString, 
		unsigned long* aplConfigurationVersion);

	// FindVersion440__RevisionHistory () ---------------------------------------
	// Only called from FindVersion440_ ().  
	// If the version number can not be found within the configuration document
	// (FindVersion440__Standard), then we will search the XML document.  The 
	// only (legitimate) reason (at this time) that the version would be found
	// in the XML document but not the configuration document would be if the 
	// configuration document were encrypted.
	static HRESULT FindVersion440__RevisionHistory (char*				p_czpSourceString, 
		unsigned long*	p_ulpConfigurationVersion);

};

#include "SVOCMArchive.inl"

