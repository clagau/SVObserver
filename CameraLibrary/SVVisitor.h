// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : Visitor
// * .File Name       : $Workfile:   SVVisitor.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 13:51:52  $
// ******************************************************************************

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
// The author or Addison-Welsey Longman make no representations about the 
//     suitability of this software for any purpose. It is provided "as is" 
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// Last update: August 20, 2003
////////////////////////////////////////////////////////////////////////////////
// Change log:
// March 20: add default argument DefaultCatchAll to BaseVisitable
// June 20, 2001: ported by Nick Thurn to gcc 2.95.3. Kudos, Nick!!!
//
// August 20, 2003: SVResearch - Eric Beyeler modified (stripped down) visitor classes
//                               to the basics to compile with VC6.0 SP3
//                             - set default return type to HRESULT
////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace SvCam
{

////////////////////////////////////////////////////////////////////////////////
// class template BaseVisitor
// The base class of any Acyclic Visitor
////////////////////////////////////////////////////////////////////////////////

    class BaseVisitor
    {
    public:
        virtual ~BaseVisitor() {}
    };
    
////////////////////////////////////////////////////////////////////////////////
// class template Visitor
// The building block of Acyclic Visitor
////////////////////////////////////////////////////////////////////////////////

//    template <class T, typename R = void>
    template <class T, typename R = HRESULT>
    class Visitor
    {
    public:
        typedef R ReturnType;
        virtual ReturnType Visit(T&) = 0;
    };
    
////////////////////////////////////////////////////////////////////////////////
// class template BaseVisitable
////////////////////////////////////////////////////////////////////////////////

template <typename R, typename Visited>
struct DefaultCatchAll
{
    static R OnUnknownVisitor(Visited&, BaseVisitor&)
    { return R(); }
};

////////////////////////////////////////////////////////////////////////////////
// class template BaseVisitable
////////////////////////////////////////////////////////////////////////////////

    template 
    <
//        typename R = void, 
        typename R = HRESULT
//        template <typename, class> class CatchAll = DefaultCatchAll
    >
    class BaseVisitable
    {
    public:
        typedef R ReturnType;
        virtual ~BaseVisitable() {}
        virtual ReturnType Accept(BaseVisitor&) = 0;
        
    protected: // give access only to the hierarchy
        template <class T>
        static ReturnType AcceptImpl(T& visited, BaseVisitor& guest)
        {
            // Apply the Acyclic Visitor
            if (Visitor<T>* p = dynamic_cast<Visitor<T>*>(&guest))
            {
                return p->Visit(visited);
            }
            //return CatchAll<R, T>::OnUnknownVisitor(visited, guest);
			return ReturnType();
        }
    };

////////////////////////////////////////////////////////////////////////////////
// macro DEFINE_VISITABLE
// Put it in every class that you want to make visitable (in addition to 
//     deriving it from BaseVisitable<R>
////////////////////////////////////////////////////////////////////////////////

//    virtual ReturnType Accept(BaseVisitor& guest) 
#define DEFINE_VISITABLE() \
    virtual ReturnType Accept(SvCam::BaseVisitor& guest) \
    { return AcceptImpl(*this, guest); }

} // namespace SvCam
