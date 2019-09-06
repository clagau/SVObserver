// stdafx.h: Includedatei f�r Standardsystem-Includedateien
// oder h�ufig verwendete projektspezifische Includedateien,
// die nur in unregelm��igen Abst�nden ge�ndert werden.
//

#pragma once

#include <stdio.h>
#include <tchar.h>
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // Einige CString-Konstruktoren sind explizit.
#define _AFX_NO_MFC_CONTROLS_IN_DIALOGS         // Unterst�tzung f�r MFC-Steuerelemente in Dialogfeldern entfernen

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Selten verwendete Komponenten aus Windows-Headern ausschlie�en
#endif

#include <afx.h>
#include <afxwin.h>         // MFC-Kern- und -Standardkomponenten
#include <afxext.h>         // MFC-Erweiterungen
#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC-Unterst�tzung f�r allgemeine Steuerelemente von Internet Explorer 4
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC-Unterst�tzung f�r allgemeine Windows-Steuerelemente
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <iostream>


#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // Einige CString-Konstruktoren sind explizit.

#include <atlbase.h>
#include <atlstr.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <comdef.h>
#include <ctime>
#include <deque>
#include <filesystem>
#include <fstream>
#include <future>
#include <map>
#include <mutex>
#include <random>
#include <regex>
#include <set>
#include <tchar.h>
#include <thread>
#include <vector>

#include <mil.h>

#include <boost/function.hpp>							//Used by status library


// TODO: Hier auf zus�tzliche Header, die das Programm erfordert, verweisen.
