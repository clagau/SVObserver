//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file AutoBuild.h
/// All Rights Reserved 
//*****************************************************************************
//******************************************************************************
/// \copyright (c) 2012,2018 by Seidenader Maschinenbau GmbH
/// \file Autobuild.h
/// All Rights Reserved
//******************************************************************************
/// The version for SVO Gateway application
//******************************************************************************

#pragma once

// change the FALSE to TRUE for autoincrement of build number
#define INCREMENT_VERSION TRUE

#define SV_EXPAND(x) #x
#define SV_STRINGIZE(x) SV_EXPAND(x)

// FILEVER lay out is as follows: a,b,c,d
//    a is the major version number
//    b is the minor version number
//    c is the beta version number - if c equals 0 or is greater than 254 then
//    the application is marked as Released
//    d is unused

#define FILEVER        8,20,0,036
#define PRODUCTVER FILEVER
#define STRFILEVER SV_STRINGIZE(FILEVER)
#define STRPRODUCTVER SV_STRINGIZE(PRODUCTVER)
