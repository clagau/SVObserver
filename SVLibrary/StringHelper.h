// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : CameraLibraryGlobals
// * .File Name       : $Workfile:   StringHelper.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   17 Jul 2014 17:31:08  $
// ******************************************************************************

#pragma once

//Moved to precompiled header: #include <float.h>
//Moved to precompiled header: #include <math.h>

inline CString SVLoadString(UINT uiResourceIDS) {CString s; s.LoadString(uiResourceIDS);return s;}

inline CString AsString(short n) {CString s;s.Format("%d", n); return s;}
inline CString AsString(int n) {CString s;s.Format("%d", n); return s;}
inline CString AsString(long n) {CString s;s.Format("%d", n); return s;}
inline CString AsString(unsigned short n) {CString s;s.Format("%u", n); return s;}
inline CString AsString(unsigned int n) {CString s;s.Format("%u", n); return s;}
inline CString AsString(unsigned long n) {CString s;s.Format("%u", n); return s;}
inline CString AsString(double d) {CString s;s.Format("%f", d); return s;}

