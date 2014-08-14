//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedImage
//* .File Name       : $Workfile:   SVSharedImage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:08:36  $
//******************************************************************************
#pragma once

#include "SVShared.h"

struct SVSharedImage
{
	char_string m_Filename;
	long m_Status;

	void_allocator m_Allocator;

	SVSharedImage( const void_allocator& rAlloc );
	SVSharedImage( const char* Filename, int Status, const void_allocator& rAlloc );
	SVSharedImage( const SVSharedImage& rData );

	const SVSharedImage& operator=( const SVSharedImage& rData );

	static std::string filename(const std::string& rName, long SlotNumber, img::type ImageType, bool bReject=false);
	static img::type GetImageType(const std::string& rName);
};

typedef boost::interprocess::allocator<SVSharedImage, segment_manager_t> SVSharedImageAllocator;
typedef std::pair< const char_string, SVSharedImage > SVSharedImagePair;
typedef boost::interprocess::allocator< SVSharedImagePair, segment_manager_t > SVSharedImagePairAllocator;
typedef boost::interprocess::map< char_string, SVSharedImage, std::less< char_string >, SVSharedImagePairAllocator > SVSharedImageMap;
typedef boost::interprocess::allocator< SVSharedImageMap, segment_manager_t > SVSharedImageMapAllocator;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSharedMemoryLibrary\SVSharedImage.h_v  $
 * 
 *    Rev 1.0   14 Aug 2014 17:08:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 Dec 2011 13:39:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
