//******************************************************************************
//* Copyright (c) 1998-2005
//* All Rights Reserved
//* Author           : Chris Maunder <chris@codeproject.com>
//* License          : The Code Project Open License (CPOL)
//* From             : http://www.codeproject.com/Articles/8/MFC-Grid-control
//******************************************************************************
//* .Module Name     : GridURLCell
//* .File Name       : $Workfile:   GridURLCell.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Oct 2014 10:32:34  $
//******************************************************************************

// GridURLCell.h: interface for the CGridURLCell class.
//
//////////////////////////////////////////////////////////////////////


#pragma once

#pragma region Includes
#include "GridCellBase.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace GridCtrlLibrary
	{

		typedef struct 
		{
			LPCTSTR szURLPrefix;
			size_t  nLength;
		} URLStruct;

		class CGridURLCell : public CGridCell  
		{
			DECLARE_DYNCREATE(CGridURLCell)

		public:
			CGridURLCell();
			virtual ~CGridURLCell();

			virtual BOOL     Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd = TRUE);
			virtual BOOL     Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
			virtual LPCTSTR  GetTipText() { return NULL; }
			void SetAutoLaunchUrl(BOOL bLaunch = TRUE) { m_bLaunchUrl = bLaunch;	}
			BOOL GetAutoLaunchUrl() { return m_bLaunchUrl;	}

		protected:
			virtual BOOL OnSetCursor();
			virtual void OnClick(CPoint PointCellRelative);

			BOOL HasUrl(CString str);
			BOOL OverURL(CPoint& pt, CString& strURL);

		protected:
		#ifndef _WIN32_WCE
			static HCURSOR g_hLinkCursor;		// Hyperlink mouse cursor
			HCURSOR GetHandCursor();
		#endif
			static URLStruct g_szURIprefixes[];

		protected:
			COLORREF m_clrUrl;
			BOOL     m_bLaunchUrl;
			CRect    m_Rect;
		};
	} //namespace GridCtrlLibrary
} //namespace Seidenader

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\GridCtrlLibrary\GridURLCell.h_v  $
 * 
 *    Rev 1.0   24 Oct 2014 10:32:34   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  942
 * SCR Title:  Create new Custom2 Filter SVO-324 SVO-67 SVO-74
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 */