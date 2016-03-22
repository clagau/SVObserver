//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : stdafx
//* .File Name       : $Workfile:   stdafx.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Jul 2014 13:56:08  $
//******************************************************************************

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Selten verwendete Teile der Windows-Header nicht einbinden.
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // Einige CString-Konstruktoren sind explizit.

//************************************
//! Precompiled headers section
//************************************
#pragma region Precompiled Headers
#include <afxwin.h>         // MFC-Kern- und Standardkomponenten
#include <afxcmn.h>         // MFC-Unterstützung für allgemeine Windows-Steuerelemente
#include <afxdtctl.h>       // MFC-Unterstützung für allgemeine Steuerelemente von Internet Explorer 4
#include <afxext.h>         // MFC-Erweiterungen
#pragma endregion Precompiled Headers

