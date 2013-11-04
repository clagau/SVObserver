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

#ifndef INCL_SVINSPECTIONIMPORTER_H
#define INCL_SVINSPECTIONIMPORTER_H

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

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVIPDocInfoImporter.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 11:17:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   05 Mar 2013 15:30:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  817
 * SCR Title:  Import/Export of Inspections
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
