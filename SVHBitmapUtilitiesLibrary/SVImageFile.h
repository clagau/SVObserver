//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageFile
//* .File Name       : $Workfile:   SVImageFile.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:11:16  $
//******************************************************************************
#ifndef SVIMAGEFILE_H
#define SVIMAGEFILE_H

class SVImageFile
{
private:
	HBITMAP m_bitmapHandle;

public:
	SVImageFile();
	~SVImageFile();

	HBITMAP GetBitmapHandle() const;
	SIZE GetSize() const;
	int GetBitDepth() const;
	const unsigned char* GetBits() const;

	SVImageFile& operator=(HBITMAP bmpHandle);
	operator HBITMAP();

	void Destroy();
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVHBitmapUtilitiesLibrary\SVImageFile.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:11:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Sep 2008 10:41:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/