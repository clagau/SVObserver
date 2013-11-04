//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLVFastOCR
//* .File Name       : $Workfile:   SVLVFastOCR.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 16:36:38  $
//******************************************************************************

#if !defined(AFX_SVLVFASTOCR_H__36354615_21B6_11D4_A8B7_00106F0309AB__INCLUDED_)
#define AFX_SVLVFASTOCR_H__36354615_21B6_11D4_A8B7_00106F0309AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _wAlign_h  // remove once wAlignUsr.h exists
#include "wFast.h"

typedef struct 
{
  CorImageVector imVec;
	CorStringVector strVec;
} SVWitFontParams;

typedef struct
{
  CorObjType type;
  SVWitFontParams *data;
  int W;
} SVWitFontParamsVector;

typedef struct
{
  CorObjType type;
  SVWitFontParams *data;
  int W, H;
  struct
	{
    int x, y, w, h;
  } roi;
} SVWitFontParamsImage;

typedef struct
{
  CorObjType type;
  SVWitFontParams *data;
  int W, H, D;
} SVWitFontParamsVolume;

#define CorObj_SVWitFontParams(o) ((SVWitFontParams *) ((o)->val.data))
#define CorObj_SVWitFontParamsImVec(sv) ((CorImageVector *) &((sv)->imVec))
#define CorObj_SVWitFontParamsStrVec(sv) ((CorStringVector *) &((sv)->strVec))

extern CorOpRtn glvLastCorOpRtn;
extern int SVLVopStatusCallback(CorOpRtn code, char *msg);

class SVLVFastOCR  
{
public:
	SVLVFastOCR();
	virtual ~SVLVFastOCR();

	CorOpRtn GetLastLVReturnCode();

	BOOL Init(void *hWnd, int (*opStatusCallback)(CorOpRtn, char *) = SVLVopStatusCallback);
	BOOL Destroy();

	CorObj *CreateLVObject(LPCTSTR pszType);
	CorObj *CreateLVObject(CorObjType lvType);
	CorObj *CreateLVStringObject(LPCTSTR pszString);
	BOOL InitializeLVObject(CorObj *plvObject, LPCTSTR pszType);
	BOOL InitializeLVObject(CorObj *plvObject, CorObjType lvType);
	BOOL InitializeLVStringObject(CorObj *plvObject, LPCTSTR pszString);
	BOOL ReleaseLVObject(CorObj *plvObject);
	BOOL DestroyLVObject(CorObj **plvObject);

	BOOL ReadLVImageFromFile(CorObj *plvObject, LPCTSTR pszFileName);
	BOOL ReadLVObjectFromFile(CorObj *plvObject, LPCTSTR pszFileName);
	BOOL WriteLVObjectToFile(CorObj *plvObject, LPCTSTR pszFileName);

	BOOL CopyLVObject(CorObj *plvFromObject, CorObj *plvToObject);

	BOOL ExtractLVImage(CorObj *plvImage, CorObj *plvROI, CorObj *plvOut);

	BOOL AddLVObjectToVector(CorObj *plvObject, CorVector *plvVector, int iItem = 0);
	BOOL GetLVObjectFromVector(CorObj *plvObject, CorVector *plvVector, int iItem);

protected:

private:
	void * mhWnd;

	void* mplvCorGuiHandle;
	CorOpRtn mlvReturnCode;
};

#endif // !defined(AFX_SVLVFASTOCR_H__36354615_21B6_11D4_A8B7_00106F0309AB__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOCRFontTraining\SVLVFastOCR.h_v  $
 * 
 *    Rev 1.0   24 Apr 2013 16:36:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/