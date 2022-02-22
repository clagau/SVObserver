//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileSystemScanner
//* .File Name       : $Workfile:   SVFileSystemScanner.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:37:58  $
//******************************************************************************

#pragma once

template<typename Insertor>
class SVFileSystemScanner
{
public:
	static HRESULT ScanForFiles(LPCTSTR dirName, LPCTSTR filter, Insertor insertor, bool bScanSubDirs=false);
	static HRESULT ScanForDirectories(LPCTSTR dirName, LPCTSTR filter, Insertor insertor, bool bScanSubDirs=false);
};

#include "SVFileSystemScanner.inl"

