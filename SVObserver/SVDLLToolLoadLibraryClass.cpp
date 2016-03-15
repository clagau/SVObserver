// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDLLToolLoadLibraryClass
// * .File Name       : $Workfile:   SVDLLToolLoadLibraryClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   21 Oct 2013 08:35:56  $
// ******************************************************************************
 
#include "Stdafx.h"
#include "SVDLLToolLoadLibraryClass.h"

SVMachineTypeEnum SVDLLToolLoadLibraryClass::CheckBitness( LPCTSTR p_szFile )
{
		//see http://www.microsoft.com/whdc/system/platform/firmware/PECOFF.mspx
		//offset to PE header is always at 0x3C
		//PE header starts with "PE\0\0" =  0x50 0x45 0x00 0x00
		//followed by 2-byte machine type field (see document above for enum)
		FILE* fh = NULL;
		SVMachineTypeEnum l_machineType = ImageFileUnknownMachine;

		errno_t err =  fopen_s( &fh, p_szFile, "rb");
		if( err == 0 )
		{
			fseek(fh, 0x3c, 0);	// seek to pe header start.
			_int32 peOffset;
			size_t bytes = fread(&peOffset, 4, 1, fh); // read 4 bytes to get offset.
			int iRet = fseek(fh, peOffset, 0 );
			_int32 peHead = 0;
			fread( &peHead, 4, 1, fh);
			if(peHead==0x00004550) // "PE\0\0", little-endian
			{
				fread( &l_machineType, 2, 1, fh);
			}
			fclose( fh );
		}
		return l_machineType;
}

