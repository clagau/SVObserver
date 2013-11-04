//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SmallObj.h
//* .File Name       : $Workfile:   SmallObj.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 13:55:56  $
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2001 by Andrei Alexandrescu
// This code accompanies the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design 
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
// Permission to use, copy, modify, distribute and sell this software for any 
//     purpose is hereby granted without fee, provided that the above copyright 
//     notice appear in all copies and that both that copyright notice and this 
//     permission notice appear in supporting documentation.
// The author or Addison-Wesley Longman make no representations about the 
//     suitability of this software for any purpose. It is provided "as is" 
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// Last update: Jan 12, 2003
// changed SmallObject's op new from
// static void* operator new(VC_BROKEN_STD::size_t size);
// to 
// static void* operator new(VC_BROKEN_STD::size_t size,
//							 VC_BROKEN_STD::size_t dummy = 0);
// and removed the ugly #pragma warning(disable:4291)"
// Thanks to M.Yamada for the hint

#ifndef SMALLOBJ_INC_
#define SMALLOBJ_INC_

#include <cstddef>
#include <vector>
#include "Threads.h"
#include "Singleton.h"
#include "MSVC6Helpers.h"		// for apply-template

#ifndef DEFAULT_CHUNK_SIZE
#define DEFAULT_CHUNK_SIZE 4096
#endif

#ifndef MAX_SMALL_OBJECT_SIZE
#define MAX_SMALL_OBJECT_SIZE 64
#endif

namespace Loki
{
////////////////////////////////////////////////////////////////////////////////
// class FixedAllocator
// Offers services for allocating fixed-sized objects
////////////////////////////////////////////////////////////////////////////////

    class FixedAllocator
    {
    public: // VC7 access control BUG
        class Chunk
        {
            friend FixedAllocator;

            void Init(VC_BROKEN_STD::size_t blockSize, unsigned char blocks);
            void* Allocate(VC_BROKEN_STD::size_t blockSize);
            void Deallocate(void* p, VC_BROKEN_STD::size_t blockSize);
            void Reset(VC_BROKEN_STD::size_t blockSize, unsigned char blocks);
            void Release();
            unsigned char* pData_;
            unsigned char
                firstAvailableBlock_,
                blocksAvailable_;
        };
        
    private:
        // Internal functions        
        void DoDeallocate(void* p);
        Chunk* VicinityFind(void* p);
        
        // Data 
        VC_BROKEN_STD::size_t blockSize_;
        unsigned char numBlocks_;
        typedef std::vector<Chunk> Chunks;
        Chunks chunks_;
        Chunk* allocChunk_;
        Chunk* deallocChunk_;
        // For ensuring proper copy semantics
        mutable const FixedAllocator* prev_;
        mutable const FixedAllocator* next_;
        
    public:
        // Create a FixedAllocator able to manage blocks of 'blockSize' size
        explicit FixedAllocator(VC_BROKEN_STD::size_t blockSize = 0);
        FixedAllocator(const FixedAllocator&);
        FixedAllocator& operator=(const FixedAllocator&);
        ~FixedAllocator();
        
        void Swap(FixedAllocator& rhs);
        
        // Allocate a memory block
        void* Allocate();
        // Deallocate a memory block previously allocated with Allocate()
        // (if that's not the case, the behavior is undefined)
        void Deallocate(void* p);
        // Returns the block size with which the FixedAllocator was initialized
        VC_BROKEN_STD::size_t BlockSize() const
        { return blockSize_; }
        // Comparison operator for sorting 
		bool operator<(const FixedAllocator& rhs) const
		{
			return BlockSize() < rhs.BlockSize(); 
		}
        //bool operator<(VC_BROKEN_STD::size_t rhs) const
        //{ return BlockSize() < rhs; }
    };

	

////////////////////////////////////////////////////////////////////////////////
// class SmallObjAllocator
// Offers services for allocating small-sized objects
////////////////////////////////////////////////////////////////////////////////

    class SmallObjAllocator
    {
    public:
        SmallObjAllocator(
            VC_BROKEN_STD::size_t chunkSize, 
            VC_BROKEN_STD::size_t maxObjectSize);
    
        void* Allocate(VC_BROKEN_STD::size_t numBytes);
        void Deallocate(void* p, VC_BROKEN_STD::size_t size);
    
    private:
        SmallObjAllocator(const SmallObjAllocator&);
        SmallObjAllocator& operator=(const SmallObjAllocator&);
        
        typedef std::vector<FixedAllocator> Pool;
        Pool pool_;
        FixedAllocator* pLastAlloc_;
        FixedAllocator* pLastDealloc_;
        VC_BROKEN_STD::size_t chunkSize_;
        VC_BROKEN_STD::size_t maxObjectSize_;
    };

////////////////////////////////////////////////////////////////////////////////
// class SmallObject
// Base class for polymorphic small objects, offers fast
//     allocations/deallocations
////////////////////////////////////////////////////////////////////////////////

    template
    <
        class ThreadingModel = DEFAULT_THREADING,
        VC_BROKEN_STD::size_t chunkSize = DEFAULT_CHUNK_SIZE,
        VC_BROKEN_STD::size_t maxSmallObjectSize = MAX_SMALL_OBJECT_SIZE
    >
    class SmallObject : public 
		Apply1<ThreadingModel, SmallObject<ThreadingModel, chunkSize, maxSmallObjectSize> >
    {
    	typedef typename Apply1<ThreadingModel, SmallObject<ThreadingModel, 
    			chunkSize, maxSmallObjectSize> > MyThreadingModel;
		
        struct MySmallObjAllocator : public SmallObjAllocator
        {
            MySmallObjAllocator() 
            : SmallObjAllocator(chunkSize, maxSmallObjectSize)
            {}
        };
        // The typedef below would make things much simpler, 
        //     but MWCW won't like it
        // typedef SingletonHolder<MySmallObjAllocator/*, CreateStatic, 
        //        DefaultLifetime, ThreadingModel*/> MyAllocator;
        
    public:
#ifdef new
#define MICROSOFT_GARBAGE_NEW new
#undef new
#endif
        static void* operator new(VC_BROKEN_STD::size_t size, VC_BROKEN_STD::size_t dummy = 0)
#ifdef MICROSOFT_GARBAGE_NEW
#define new MICROSOFT_GARBAGE_NEW
#endif
        {
#if (MAX_SMALL_OBJECT_SIZE != 0) && (DEFAULT_CHUNK_SIZE != 0)
            typename MyThreadingModel::Lock lock;
            (void)lock; // get rid of warning
            
            return SingletonHolder<MySmallObjAllocator, CreateStatic, 
                PhoenixSingleton>::Instance().Allocate(size);
#else
            return ::operator new(size);
#endif
        }
        static void operator delete(void* p, VC_BROKEN_STD::size_t size)
        {
#if (MAX_SMALL_OBJECT_SIZE != 0) && (DEFAULT_CHUNK_SIZE != 0)
            typename MyThreadingModel::Lock lock;
            (void)lock; // get rid of warning
            
            SingletonHolder<MySmallObjAllocator, CreateStatic, 
                PhoenixSingleton>::Instance().Deallocate(p, size);
#else
            ::operator delete(p, size);
#endif
        }
        virtual ~SmallObject() {}
    };

} // namespace Loki

////////////////////////////////////////////////////////////////////////////////
// Change log:
// June 20, 2001: ported by Nick Thurn to gcc 2.95.3. Kudos, Nick!!!
// Oct	11, 2002: ported by Benjamin Kaufmann to MSVC 6.0
////////////////////////////////////////////////////////////////////////////////

#endif // SMALLOBJ_INC_

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLoki\SmallObj.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 13:55:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   28 May 2010 12:57:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  694
 * SCR Title:  Upgrade SVObserver version for Chromis release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with includes and make them consistant.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   19 Jun 2007 15:51:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   04 Oct 2005 08:59:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   workaround for Microoft DEBUG_NEW macro (they #define the keyword new!!)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 Jul 2004 08:52:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration - added PVCS header
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
