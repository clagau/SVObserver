//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLVFastOCR
//* .File Name       : $Workfile:   SVLVFastOCR.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:25:44  $
//******************************************************************************

#ifndef SVLVFASTOCR_H
#define SVLVFASTOCR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct SVOCRParamStruct;
struct SVOCRGrayParamStruct;

#define _wAlign_h  // remove once wAlignUsr.h exists
#define _corObjObsolete_h
#include "wFast.h"
//#undef freeData

class SVLVFastOCR  
{
public:
	SVLVFastOCR();
	virtual ~SVLVFastOCR();

	CorOpRtn GetLastLVReturnCode();

	BOOL IsLVFastOCRLoaded();

	CorObjType GetLVObjectType(LPCTSTR pszType);


	CorObj *CreateLVObject(LPCTSTR pszType);
	CorObj *CreateLVObject(CorObjType lvType);
	void *CreateLVObjectData(LPCTSTR pszType);
	void *CreateLVObjectData(CorObjType lvType);
	CorObj *CreateLVStringObject(LPCTSTR pszString);
	BOOL InitializeLVObject(CorObj *plvObject, LPCTSTR pszType);
	BOOL InitializeLVObject(CorObj *plvObject, CorObjType lvType);
	BOOL InitializeLVStringObject(CorObj *plvObject, LPCTSTR pszString);
	BOOL ReleaseLVObject(CorObj *plvObject);
	BOOL ReleaseLVObjectData(void *plvObject, LPCTSTR pszType, LPCTSTR pszRefType);
	BOOL ReleaseLVObjectData(void *plvObject, CorObjType cotType, CorObjType cotRefType);
	BOOL ReleaseLVVector(CorVector *plvVector);
	BOOL DestroyLVObject(CorObj **plvObject);
	BOOL DestroyLVObjectData(void **plvObject, LPCTSTR pszType, LPCTSTR pszRefType);
	BOOL DestroyLVObjectData(void **plvObject, CorObjType cotType, CorObjType cotRefType);

	BOOL ReadLVImageFromFile(CorObj *plvObject, LPCTSTR pszFileName);
	BOOL ReadLVObjectFromFile(CorObj *plvObject, LPCTSTR pszFileName);
	BOOL WriteLVObjectToFile(CorObj *plvObject, LPCTSTR pszFileName);

	BOOL CopyLVObject(CorObj *plvFromObject, CorObj *plvToObject);
	BOOL CopyLVObjectData(void *pvSource, void *pvDest, CorObjType cotType, CorObjType cotRefType);

	BOOL ExtractLVImage(CorObj *plvImage, CorObj *plvROI, CorObj *plvOut);

	BOOL AddLVObjectToVector(CorObj *plvObject, CorVector *plvVector, int iItem = 0);
	BOOL GetLVObjectFromVector(CorObj *plvObject, CorVector *plvVector, int iItem);

	void *AddLVOpStatusToThread();

	BOOL InitializeLVOCRGray(CorOcrFont *pFont, CorFontModelGray **pFontModel);
	BOOL InitializeLVOCRPerim(CorOcrFont *pFont, CorFontModel **pFontModel, int iSamples);
	BOOL RunLVOCRGray(CorImage *pTarget, CorObj *pROI, CorFontModelGray *pFontModel, CorVector **pLabels, CorVector **pScores, CorVector **pLocations, SVOCRGrayParamStruct *psvOCRParameters);
	BOOL RunLVOCRPerim(CorImage *pTarget, CorObj *pROI, CorFontModel *pFontModel, CorVector **pLabels, CorVector **pScores, CorVector **pLocations, SVOCRParamStruct *psvOCRParameters);

protected:

private:
	CorOpRtn mlvReturnCode;

	void *GetFunctionAddress( LPCSTR szFunctionName );

	BOOL LoadLVDLLs();
	BOOL LoadLVFunctionAddresses();

	BOOL UnloadLVDLLs();
	BOOL UnloadLVFunctionAddresses();

	BOOL ConnectToLVFastOCR();
	BOOL DisconnectFromLVFastOCR();
};

#endif   // SVLVFASTOCR_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVLVFastOCR.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:25:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 11:06:56   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   28 Aug 2000 16:03:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  177
 * SCR Title:  Fix Automatic Load Last SEC Problem
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update FastOCR to prevent memory access violations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/