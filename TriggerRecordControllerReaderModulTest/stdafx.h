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


#include <array>
#include <chrono>
#include <comdef.h>
#include <ctime>
#include <deque>
#include <future>
#include <iostream>
#include <map>
#include <mutex>
#include <random>
#include <regex>
#include <set>
#include <tchar.h>
#include <thread>
#include <unordered_map>
#include <vector>

#include <mil.h>

#include <boost/function.hpp>							//Used by status library


// TODO: Hier auf zus�tzliche Header, die das Programm erfordert, verweisen.
