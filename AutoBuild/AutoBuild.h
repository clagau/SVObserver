//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : AutoBuild
//* .File Name       : $Workfile:   AutoBuild.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.28  $
//* .Check In Date   : $Date:   28 Jan 2015 11:14:06  $
//******************************************************************************

#pragma once

// change the FALSE to TRUE for autoincrement of build number
#define INCREMENT_VERSION TRUE

#define SV_EXPAND( x ) #x
#define SV_STRINGIZE( x ) SV_EXPAND( x )

// FILEVER lay out is as follows: a,b,c,d
//    a is the major version number
//    b is the minor version number
//    c is the beta version number - if c equals 0 or is greater than 254 then the application is marked as Released
//    d is used for the revision number underlying the build (if zero it is ignored)

#define FILEVER        10,10,0,003
#define PRODUCTVER     FILEVER
#define STRFILEVER     SV_STRINGIZE( FILEVER )
#define STRPRODUCTVER  SV_STRINGIZE( PRODUCTVER )

