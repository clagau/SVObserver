//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : TypeInfo
//* .File Name       : $Workfile:   LokiTypeInfo.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 13:55:18  $
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

#ifndef LOKITYPEINFO_INC_
#define LOKITYPEINFO_INC_

#include <typeinfo>
#include <cassert>
#include "Typelist.h"

namespace Loki
{
////////////////////////////////////////////////////////////////////////////////
// class TypeInfo
// Purpose: offer a first-class, comparable wrapper over std::type_info
////////////////////////////////////////////////////////////////////////////////

    class TypeInfo
    {
    public:
        // Constructors
        TypeInfo(); // needed for containers
        TypeInfo(const std::type_info&); // non-explicit

        // Access for the wrapped std::type_info
        const std::type_info& Get() const;
        // Compatibility functions
        bool before(const TypeInfo& rhs) const;
        const char* name() const;

    private:
        const std::type_info* pInfo_;
    };
    
// Implementation
    
    inline TypeInfo::TypeInfo()
    {
        class Nil {};
        pInfo_ = &typeid(Nil);
        assert(pInfo_);
    }
    
    inline TypeInfo::TypeInfo(const std::type_info& ti)
    : pInfo_(&ti)
    { assert(pInfo_); }
    
    inline bool TypeInfo::before(const TypeInfo& rhs) const
    {
        assert(pInfo_);
        return pInfo_->before(*rhs.pInfo_) != 0;
    }

    inline const std::type_info& TypeInfo::Get() const
    {
        assert(pInfo_);
        return *pInfo_;
    }
    
    inline const char* TypeInfo::name() const
    {
        assert(pInfo_);
        return pInfo_->name();
    }

// Comparison operators
    
    inline bool operator==(const TypeInfo& lhs, const TypeInfo& rhs)
    { return (lhs.Get() == rhs.Get()) != 0; }

    inline bool operator<(const TypeInfo& lhs, const TypeInfo& rhs)
    { return lhs.before(rhs); }

    inline bool operator!=(const TypeInfo& lhs, const TypeInfo& rhs)
    { return !(lhs == rhs); }    
    
    inline bool operator>(const TypeInfo& lhs, const TypeInfo& rhs)
    { return rhs < lhs; }
    
    inline bool operator<=(const TypeInfo& lhs, const TypeInfo& rhs)
    { return !(lhs > rhs); }
     
    inline bool operator>=(const TypeInfo& lhs, const TypeInfo& rhs)
    { return !(lhs < rhs); }
}

////////////////////////////////////////////////////////////////////////////////
// Change log:
// June 20, 2001: ported by Nick Thurn to gcc 2.95.3. Kudos, Nick!!!
////////////////////////////////////////////////////////////////////////////////

#endif // LOKITYPEINFO_INC_

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLoki\LokiTypeInfo.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 13:55:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 Jul 2004 08:50:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration - added PVCS header
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   13 Jul 2004 08:49:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration - added PVCS header
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/