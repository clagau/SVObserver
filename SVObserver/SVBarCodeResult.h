//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBarCodeResultClass
//* .File Name       : $Workfile:   SVBarCodeResult.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Feb 2014 10:23:12  $
//******************************************************************************

#if !defined(AFX_SVBARCODERESULT_H__3A85515D_7B17_11D3_A7CE_00106F010A93__INCLUDED_)
#define AFX_SVBARCODERESULT_H__3A85515D_7B17_11D3_A7CE_00106F010A93__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVResultString.h"

#define  BC_MAX_ENTREES       10000  // Max number of lines allowed in the match file
#define  BC_CAR_RETURN        0x0D

class SVBarCodeResultClass : public SVStringResultClass  
{
	SV_DECLARE_CLASS(SVBarCodeResultClass);

public:
	bool m_bFailedToRead;
	BOOL onRun( SVRunStatusClass& RRunStatus );
	BOOL OnValidate();
	SVStringValueObjectClass* getRegExpression();
	BOOL CloseObject();
	BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	SVBarCodeResultClass(BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVBARCODEANALYZERESULT);
	virtual ~SVBarCodeResultClass();

	virtual LONG_PTR processMessage( DWORD DwMessageID, LONG_PTR DwMessageValue, LONG_PTR DwMessageContext );

	HRESULT LoadMatchStringFile();
	BOOL BuildHashTable(char *szBuffer);
	void InsertValueToTable(short nValue, int index);

	int CheckStringInTable(CString MatchString);

	SVBoolValueObjectClass msv_bUseSingleMatchString;
	SVBoolValueObjectClass msv_bUseMatchStringFile;
	SVFileNameValueObjectClass msv_szMatchStringFileName;
	SVLongValueObjectClass msv_lMatchStringLine;
	SVDoubleValueObjectClass m_dReadScore;
	virtual HRESULT ResetObject();

protected:
	SVStringValueObjectClass* getInputString();
	SVInObjectInfoStruct m_SVRegExpressionObjectInfo;

	long     m_lTotalBytes;
	int      m_nTotalCount;
	char     *m_pBuffer;
	long     m_lLowValue;
	long     m_lHighValue;
	char     *m_pDataArr[BC_MAX_ENTREES];
	short    *m_pIndexTable;
	double   m_dFactor;
};

#endif // !defined(AFX_SVBARCODERESULT_H__3A85515D_7B17_11D3_A7CE_00106F010A93__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVBarCodeResult.h_v  $
 * 
 *    Rev 1.1   01 Feb 2014 10:23:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed sendmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 09:42:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   17 Feb 2005 13:12:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   08 Sep 2004 11:15:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  444
 * SCR Title:  Change Bar Code Analyzer Pass / Fail Result
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified Constructor, CreateObject and OnRun to support new ValueObject ReadScore, and variable m_bFailedToRead flag.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   08 Jul 2004 10:38:00   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  431
 * SCR Title:  Fix problem with using match string files that are zero elements
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   put in check to make sure that if using a match string file that the number of elements is above 0.  if not, the inspection will not be allowed to go online
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   17 Apr 2003 16:54:10   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/