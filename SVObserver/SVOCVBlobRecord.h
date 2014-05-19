//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCVBlobRecord
//* .File Name       : $Workfile:   SVOCVBlobRecord.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:07:26  $
//******************************************************************************

#include "SVTaskObject.h"

#ifndef SVOCVBLOBRECORD_H
#define SVOCVBLOBRECORD_H

class SVOCVCharacterResultClass : public SVTaskObjectClass
{
protected:
	SV_DECLARE_CLASS( SVOCVCharacterResultClass )

public:
	SVOCVCharacterResultClass(LPCSTR ObjectName );
	SVOCVCharacterResultClass(SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVOCVCHARACTERRESULTOBJECT );
	~SVOCVCharacterResultClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();
	void init();

	void HideResults();
	void UnhideResults();

protected:
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

//Attributes
public:
	SVCharValueObjectClass	 m_cvoLabelValue;

	SVDoubleValueObjectClass m_dvoOverlayLeft;
	SVDoubleValueObjectClass m_dvoOverlayTop;
	SVDoubleValueObjectClass m_dvoOverlayWidth;
	SVDoubleValueObjectClass m_dvoOverlayHeight;

	SVDoubleValueObjectClass m_dvoMatchScore;
};
typedef SVVector< SVOCVCharacterResultClass* > SVOCVCharacterResultArray;

#endif   /* SVOCVBLOBRECORD_H */

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVOCVBlobRecord.h_v  $
 * 
 *    Rev 1.2   15 May 2014 11:07:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified processMessage to use DWORD_PTR instead of LONG_PTR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2014 11:55:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:01:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 Nov 2010 14:46:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Sep 2003 13:07:50   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  384
 * SCR Title:  Add new Matrox OCR/OCV analyzer to SVObserver
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Created and integrated a new Matrox based OCR/OCV analyzer to SVObserver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
