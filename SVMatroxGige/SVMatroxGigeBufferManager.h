//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeBufferManager
//* .File Name       : $Workfile:   SVMatroxGigeBufferManager.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:39:18  $
//******************************************************************************

#ifndef SVMATROXGIGEBUFFERMANAGER_H
#define SVMATROXGIGEBUFFERMANAGER_H

#include <boost/config.hpp>
#include <boost/utility.hpp>
#include "SVMatroxLibrary/SVMatroxSystem.h"

// Need to manage both Acquisition Buffers and Image Processing Buffers...
template <typename BufferCreator, typename BufferStrategy, typename BufferCollection>
class SVMatroxGigeBufferManager : public boost::noncopyable
{
	typedef typename BufferCollection::value_type BufferType;

private:
	BufferCreator m_creator;
	BufferStrategy m_bufferStrategy;
	BufferCollection m_Buffers;

public:
	SVMatroxGigeBufferManager();
	~SVMatroxGigeBufferManager();

	bool Init();
	bool Destroy();

	HRESULT CreateBuffer(const SVMatroxSystem& rSystem, unsigned long width, unsigned long height, unsigned long format, unsigned long& p_rlIndex);

	HRESULT DestroyBuffers();
	HRESULT DestroyBuffer(unsigned long handle);

	HRESULT Acquire(unsigned long handle);
	HRESULT Release(unsigned long handle);
	HRESULT IsInUse(unsigned long handle, bool& bInUse) const;

	bool IsValidBufferHandle(unsigned long handle) const;
	bool IsValidBuffer(unsigned long handle) const;

	SVMatroxGigeBufferStruct& Get(unsigned long handle);
	const SVMatroxGigeBufferStruct& Get(unsigned long handle) const;

	HRESULT BufferGetWidth( unsigned long handle, unsigned long& rWidth ) const;
	HRESULT BufferGetHeight( unsigned long handle, unsigned long& rHeight ) const;
	HRESULT BufferGetFormat( unsigned long handle, unsigned long& rFormat ) const;
	HRESULT BufferGetAddress( unsigned long handle, unsigned long& rAddress) const;
};

#include "SVMatroxGigeBufferManager.inl"

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxGige\SVMatroxGigeBufferManager.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:39:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 17:18:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
