// ******************************************************************************
// * COPYRIGHT (c) 2002 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOCMGlobals
// * .File Name       : $Workfile:   LoadConfiguration.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   03 Sep 2014 15:54:44  $
// ******************************************************************************

#pragma once

#pragma region Includes
#pragma endregion Includes

namespace  SvXml
{
class SVXMLMaterialsTree;
HRESULT SVOCMLoadConfiguration(unsigned long &FileVersion, LPCTSTR Filename, SVXMLMaterialsTree& p_rTree);
} //namespace SvXml

