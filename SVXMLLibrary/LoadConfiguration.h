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

#pragma region Includes
#pragma endregion Includes

namespace  SvXml
{
	template< typename SVTreeType >
	HRESULT SVOCMLoadConfiguration(unsigned long &FileVersion, LPCTSTR Filename, SVTreeType& p_rTree);
} //namespace SvXml

#include "LoadConfiguration.inl"

