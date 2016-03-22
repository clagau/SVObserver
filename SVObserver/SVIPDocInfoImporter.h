//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc. Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIPDocInfoImporter
//* .File Name       : $Workfile:   SVIPDocInfoImporter.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:17:16  $
//******************************************************************************

#pragma once

class SVIPDoc;
class SVImportedInspectionInfo;

class SVIPDocInfoImporter
{
public:
	static void Import(SVIPDoc* pDoc, const SVImportedInspectionInfo& info);

private:
	SVIPDocInfoImporter();
	~SVIPDocInfoImporter();
};


