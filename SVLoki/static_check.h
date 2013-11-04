//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : static_check.h
//* .File Name       : $Workfile:   static_check.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 13:59:32  $
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

// Last update: June 20, 2001

#ifndef STATIC_CHECK_INC_
#define STATIC_CHECK_INC_

namespace Loki
{
////////////////////////////////////////////////////////////////////////////////
// Helper structure for the STATIC_CHECK macro
////////////////////////////////////////////////////////////////////////////////

    template<int> struct CompileTimeError;
    template<> struct CompileTimeError<true> {};

	namespace Private
	{
		template<int x> struct static_assert_test{};
		template <bool x> struct SizeError;

		template <> struct SizeError<true>{};
	}
}

#define STATIC_SIZE_ASSERT(Type, ExpSize)	\
typedef ::Loki::Private::static_assert_test<sizeof(::Loki::Private::SizeError	\
< (bool) (sizeof(Type) == ExpSize) >)> static_assert_typedef_

////////////////////////////////////////////////////////////////////////////////
// macro STATIC_CHECK
// Invocation: STATIC_CHECK(expr, id)
// where:
// expr is a compile-time integral or pointer expression
// id is a C++ identifier that does not need to be defined
// If expr is zero, id will appear in a compile-time error message.
////////////////////////////////////////////////////////////////////////////////

#define STATIC_CHECK(expr, msg) \
    { Loki::CompileTimeError<((expr) != 0)> ERROR_##msg; (void)ERROR_##msg; } 

#define FILE_SCOPE_STATIC_CHECK(expr, msg) \
struct check_##msg{ check_##msg() {Loki::CompileTimeError<((expr) != 0)> ERROR_##msg; (void)ERROR_##msg;} }; 


////////////////////////////////////////////////////////////////////////////////
// Change log:
// March 20, 2001: add extra parens to STATIC_CHECK - it looked like a fun 
//     definition
// June 20, 2001: ported by Nick Thurn to gcc 2.95.3. Kudos, Nick!!!
////////////////////////////////////////////////////////////////////////////////

#endif // STATIC_CHECK_INC_

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLoki\static_check.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 13:59:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   28 Dec 2004 08:25:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  467
 * SCR Title:  Fix missing bar code problem
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added FILE_SCOPE_STATIC_CHECK
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