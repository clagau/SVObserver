// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOCMGlobals
// * .File Name       : $Workfile:   SVOCMGlobals.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   03 Sep 2014 15:54:44  $
// ******************************************************************************

#pragma once

template< typename SVTreeType >
HRESULT SVOCMLoadConfiguration( unsigned long ulSVOVersion, unsigned long &ulSECVersion, BSTR bstrFilename, SVTreeType& p_rTree );

template< typename SVTreeType >
HRESULT SVOCMSaveConfiguration( unsigned long ulSVOVersion, unsigned long &ulSECVersion, BSTR bstrFilename, SVTreeType& p_rTree );

template< typename SVTreeType >
HRESULT SVOCMCheckObsoleteItems( SVTreeType& p_rTree, CString& rItemType, int& errorCode );

#include "SVOCMGlobals.inl"

