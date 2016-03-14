//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageListClass
//* .File Name       : $Workfile:   SVImageListClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:53:08  $
//******************************************************************************

#include "stdafx.h"
#include "SVImageListClass.h"

SVImageListClass::SVImageListClass()
{
}

SVImageListClass::~SVImageListClass()
{
	RemoveAll();
}

BOOL SVImageListClass::RemoveImage( SVImageClass* PImage )
{
	AfxMessageBox( "BOOL SVImageListClass::RemoveImage( SVImageClass* PImage );\n\nDo not use this function!\nOut of use!" );
	return FALSE;
}

SVImageClass * SVImageListClass::GetAtByTool(int nIndex)   // 25 Jun 1999 - frb.
{
    SVImageClass * pImage = GetAt(nIndex);
    
	ASSERT(pImage);

    //
	// So we don't try to use this image that has been passed to a tool.
	//
	SetAt(nIndex,NULL);

#ifdef _DEBUG
    SVImageClass * pImageTest = GetAt(nIndex);
	ASSERT(pImageTest == NULL);
#endif //_DEBUG

	return pImage;
}

