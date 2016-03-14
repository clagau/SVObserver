//******************************************************************************
//* .Module Name     : Autolink
//* .File Name       : $Workfile:   autolink.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 16:04:22  $
//******************************************************************************

#ifndef JSON_AUTOLINK_H_INCLUDED
# define JSON_AUTOLINK_H_INCLUDED

# include "config.h"

# ifdef JSON_IN_CPPTL
#  include <cpptl/cpptl_autolink.h>
# endif

# if !defined(JSON_NO_AUTOLINK)  &&  !defined(JSON_DLL_BUILD)  &&  !defined(JSON_IN_CPPTL)
#  define CPPTL_AUTOLINK_NAME "json"
#  undef CPPTL_AUTOLINK_DLL
#  ifdef JSON_DLL
#   define CPPTL_AUTOLINK_DLL
#  endif
#  include "autolink.h"
# endif

#endif // JSON_AUTOLINK_H_INCLUDED

