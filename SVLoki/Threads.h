//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : Threads
//* .File Name       : $Workfile:   Threads.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 13:59:56  $
//******************************************************************************

#ifndef THREADS_H_
#define THREADS_H_
////////////////////////////////////////////////////////////////////////////////
// macro DEFAULT_THREADING
// Selects the default threading model for certain components of Loki
// If you don't define it, it defaults to single-threaded
// All classes in Loki have configurable threading model; DEFAULT_THREADING
// affects only default template arguments
////////////////////////////////////////////////////////////////////////////////

// Last update: Feb 20, 2003
// note: In this VC 6 port all template policies become non-templates with
// either member-template functions or a nested template struct named In

// Changed wrong ctor/dtor names in ObjectLevelLockable. 
// Thanks to Adi Shavit for pointing that out

#ifndef DEFAULT_THREADING
#define DEFAULT_THREADING /**/ ::Loki::SingleThreaded
#endif

namespace Loki
{
////////////////////////////////////////////////////////////////////////////////
// class template SingleThreaded
// Implementation of the ThreadingModel policy used by various classes
// Implements a single-threaded model; no synchronization
////////////////////////////////////////////////////////////////////////////////
	class SingleThreaded
    {
    public:
        template <class Host>
		struct In
		{
			struct Lock
			{
				Lock() {}
				explicit Lock(const SingleThreaded&) {}
			};
        
			typedef Host VolatileType;

			typedef int IntType; 

			static IntType AtomicAdd(volatile IntType& lval, IntType val)
			{ return lval += val; }
        
			static IntType AtomicSubtract(volatile IntType& lval, IntType val)
			{ return lval -= val; }

			static IntType AtomicMultiply(volatile IntType& lval, IntType val)
			{ return lval *= val; }
        
			static IntType AtomicDivide(volatile IntType& lval, IntType val)
			{ return lval /= val; }
        
			static IntType AtomicIncrement(volatile IntType& lval)
			{ return ++lval; }
        
			static IntType AtomicDecrement(volatile IntType& lval)
			{ return --lval; }

			// The following function uses the undefined variable val.
			// Moreover I couldn't find the function in the orginial loki-lib.
			// I therefore commented the function out
			/*
			static IntType AtomicDivide(volatile IntType& lval)
			{ return lval /= val; }
			*/
			static void AtomicAssign(volatile IntType & lval, IntType val)
			{ lval = val; }
        
			static void AtomicAssign(IntType & lval, volatile IntType & val)
			{ lval = val; }
		};
    };
    
//#ifdef _WINDOWS_

////////////////////////////////////////////////////////////////////////////////
// class template ObjectLevelLockable
// Implementation of the ThreadingModel policy used by various classes
// Implements a object-level locking scheme
////////////////////////////////////////////////////////////////////////////////

    
    struct ObjectLevelLockable
    {
		template <class Host>
		struct In
		{
		private:
			CRITICAL_SECTION mtx_;

		public:
			In()
			{
				::InitializeCriticalSection(&mtx_);
			}

			~In()
			{
				::DeleteCriticalSection(&mtx_);
			}

			class Lock;
			friend class Lock;
        
			class Lock
			{
				ObjectLevelLockable::In<Host>& host_;
            
				Lock(const Lock&);
				Lock& operator=(const Lock&);
				//Lock(); // buggy design
			public:

				explicit Lock(ObjectLevelLockable::In<Host>& host) : host_(host)
				{
					::EnterCriticalSection(&host_.mtx_);
				}
				~Lock()
				{
					::LeaveCriticalSection(&host_.mtx_);
				}
			};

			typedef volatile Host VolatileType;

			typedef LONG IntType; 

			static IntType AtomicIncrement(volatile IntType& lval)
			{ return InterlockedIncrement(&const_cast<IntType&>(lval)); }
        
			static IntType AtomicDecrement(volatile IntType& lval)
			{ return InterlockedDecrement(&const_cast<IntType&>(lval)); }
        
			static void AtomicAssign(volatile IntType& lval, IntType val)
			{ InterlockedExchange(&const_cast<IntType&>(lval), val); }
        
			static void AtomicAssign(IntType& lval, volatile IntType& val)
			{ InterlockedExchange(&lval, val); }
		};
    };
    
    struct ClassLevelLockable
    {
		template <class Host>
		struct In
		{
		public:
			struct Initializer;
			friend struct Initializer;
			struct Initializer
			{   
				
				CRITICAL_SECTION mtx_;

				Initializer()
				{
#if _WIN32_WINNT == 0x0500
					::InitializeCriticalSectionAndSpinCount(&mtx_, 4000);
#else
					::InitializeCriticalSection(&mtx_);
#endif
				}
				~Initializer()
				{
					::DeleteCriticalSection(&mtx_);
				}
			};
        
			static Initializer initializer_;

		public:
			class Lock;
			friend class Lock;
        
			class Lock
			{
				Lock(const Lock&);
				Lock& operator=(const Lock&);
			public:
				Lock()
				{
					::EnterCriticalSection(&initializer_.mtx_);
				}
				explicit Lock(ClassLevelLockable&)
				{
					::EnterCriticalSection(&initializer_.mtx_);
				}
				~Lock()
				{
					::LeaveCriticalSection(&initializer_.mtx_);
				}
			};

			typedef volatile Host VolatileType;

			typedef LONG IntType; 

			static IntType AtomicIncrement(volatile IntType& lval)
			{ return InterlockedIncrement(&const_cast<IntType&>(lval)); }
        
			static IntType AtomicDecrement(volatile IntType& lval)
			{ return InterlockedDecrement(&const_cast<IntType&>(lval)); }
        
			static void AtomicAssign(volatile IntType& lval, IntType val)
			{ InterlockedExchange(&const_cast<IntType&>(lval), val); }
        
			static void AtomicAssign(IntType& lval, volatile IntType& val)
			{ InterlockedExchange(&lval, val); }
		};
    };
    
    template <class Host>
	typename Loki::ClassLevelLockable::template In<Host>::Initializer 
	Loki::ClassLevelLockable::template In<Host>::initializer_;
    
//#endif    #ifdef _WINDOWS_
}

////////////////////////////////////////////////////////////////////////////////
// Change log:
// June 20, 2001: ported by Nick Thurn to gcc 2.95.3. Kudos, Nick!!!
// Oct	06,	2002: ported by Benjamin Kaufmann to MSVC 6.0
// Feb	20, 2003: corrected constructor parameter in ObjectLevelLockable::Lock
////////////////////////////////////////////////////////////////////////////////

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLoki\Threads.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 13:59:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   04 Oct 2005 09:05:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   removed bogus #ifdef _WINDOWS_
 * enabled spin count on critical section if used in a WINNT 0x0500 project
 * added Loki scope on ClassLevelLockable typename so VC6  would compile it
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 Jul 2004 08:52:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration - added PVCS header
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/