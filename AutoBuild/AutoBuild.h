/// \copyright (c) 2012,2018 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file Autobuild.h
/// This is the version of the SVObserver application

#pragma once

// change the FALSE to TRUE for autoincrement of build number
#define INCREMENT_VERSION TRUE

#define SV_EXPAND( x ) #x
#define SV_STRINGIZE( x ) SV_EXPAND( x )

// FILEVER layout is as follows: a,b,c,d
//    a is the major version number
//    b is the minor version number
//    c is used for the revision number underlying the build (if zero it is ignored)
//    d is the alpha/beta/rc index:
//		if > 0 but < 255: beta
//		if > 1000: alpha
//		otherwise: release candidate

#define FILEVER        10,40,0,6
#define PRODUCTVER     FILEVER
#define STRFILEVER     SV_STRINGIZE( FILEVER )
#define STRPRODUCTVER  SV_STRINGIZE( PRODUCTVER )

