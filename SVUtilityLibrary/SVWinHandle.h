//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVWinHandle.h
//* .File Name       : $Workfile:   SVWinHandle.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Sep 2013 16:31:12  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <vector>

template < typename T >
struct SVWinHandle	// implements RAII for windows handles
{
	SVWinHandle() : m_t(nullptr) {}
	SVWinHandle(T rt) : m_t(rt) {}
	~SVWinHandle() {::CloseHandle(m_t);}
	const SVWinHandle<T>& operator = (T t) {m_t = t; return *this;}
	const SVWinHandle<T>& operator = (void* pv) {m_t = (T) pv; return *this;}
	operator T() {return m_t;}
private:
	// disable copy constructor and operator = 
	// because we want single ownership
	SVWinHandle(const SVWinHandle<T>& );
	const SVWinHandle<T>& operator = (const SVWinHandle<T>& );

	T m_t;
};

template <>
SVWinHandle<HICON>::~SVWinHandle() {::DestroyIcon(m_t);}

template <>
SVWinHandle<HBITMAP>::~SVWinHandle() {::DeleteObject(m_t);}

