//******************************************************************************
//* Copyright (c) 1998-2005
//* All Rights Reserved
//* Author           : Chris Maunder <chris@codeproject.com>
//* License          : The Code Project Open License (CPOL)
//* From             : http://www.codeproject.com/Articles/8/MFC-Grid-control
//******************************************************************************
//* .Module Name     : CellRange
//* .File Name       : $Workfile:   CellRange.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Oct 2014 10:32:30  $
//******************************************************************************

///////////////////////////////////////////////////////////////////////
// CellRange.h: header file
//
// MFC Grid Control - interface for the CCellRange class.
//
// Written by Chris Maunder <cmaunder@mail.com>
// Copyright (c) 1998-2002. All Rights Reserved.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name and all copyright 
// notices remains intact. 
//
// An email letting me know how you are using it would be nice as well. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
// For use with CGridCtrl v2.20+
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <algorithm>
// The code contained in this file is based on the original
// WorldCom Grid control written by Joe Willcoxson,
//      mailto:chinajoe@aol.com
//      http://users.aol.com/chinajoe

namespace Seidenader
{
	namespace GridCtrlLibrary
	{
		class CCellID
		{    
		// Attributes
		public:
			int row, col;

		// Operations
		public:
			explicit CCellID(int nRow = -1, int nCol = -1) : row(nRow), col(nCol) {}

			int IsValid() const { return (row >= 0 && col >= 0); }
			int operator==(const CCellID& rhs) const { return (row == rhs.row && col == rhs.col); }
			int operator!=(const CCellID& rhs) const { return !operator==(rhs); }
		};

		class CCellRange
		{ 
		public:
    
			CCellRange(int nMinRow = -1, int nMinCol = -1, int nMaxRow = -1, int nMaxCol = -1)
			{
				Set(nMinRow, nMinCol, nMaxRow, nMaxCol);
			}

			void Set(int nMinRow = -1, int nMinCol = -1, int nMaxRow = -1, int nMaxCol = -1);
    
			int  IsValid() const;
			int  InRange(int row, int col) const;
			int  InRange(const CCellID& cellID) const;
			int  Count() { return (m_nMaxRow - m_nMinRow + 1) * (m_nMaxCol - m_nMinCol + 1); }
    
			CCellID  GetTopLeft() const;
			CCellRange  Intersect(const CCellRange& rhs) const;
    
			int GetMinRow() const {return m_nMinRow;}
			void SetMinRow(int minRow) {m_nMinRow = minRow;}
    
			int GetMinCol() const {return m_nMinCol;}
			void SetMinCol(int minCol) {m_nMinCol = minCol;}
    
			int GetMaxRow() const {return m_nMaxRow;}
			void SetMaxRow(int maxRow) {m_nMaxRow = maxRow;}
    
			int GetMaxCol() const {return m_nMaxCol;}
			void SetMaxCol(int maxCol) {m_nMaxCol = maxCol;}

			int GetRowSpan() const {return m_nMaxRow - m_nMinRow + 1;}
			int GetColSpan() const {return m_nMaxCol - m_nMinCol + 1;}
    
			void operator=(const CCellRange& rhs);
			int  operator==(const CCellRange& rhs);
			int  operator!=(const CCellRange& rhs);
    
		protected:
			int m_nMinRow;
			int m_nMinCol;
			int m_nMaxRow;
			int m_nMaxCol;
		};

		inline void CCellRange::Set(int minRow, int minCol, int maxRow, int maxCol)
		{
			 m_nMinRow = minRow;
			 m_nMinCol = minCol;
			 m_nMaxRow = maxRow;
			 m_nMaxCol = maxCol;
		}

		inline void CCellRange::operator=(const CCellRange& rhs)
		{
			if (this != &rhs) Set(rhs.m_nMinRow, rhs.m_nMinCol, rhs.m_nMaxRow, rhs.m_nMaxCol);
		}

		inline int CCellRange::operator==(const CCellRange& rhs)
		{
			 return ((m_nMinRow == rhs.m_nMinRow) && (m_nMinCol == rhs.m_nMinCol) &&
					 (m_nMaxRow == rhs.m_nMaxRow) && (m_nMaxCol == rhs.m_nMaxCol));
		}

		inline int CCellRange::operator!=(const CCellRange& rhs)
		{
			 return !operator==(rhs);
		}

		inline int CCellRange::IsValid() const
		{
			 return (m_nMinRow >= 0 && m_nMinCol >= 0 && m_nMaxRow >= 0 && m_nMaxCol >= 0 &&
					 m_nMinRow <= m_nMaxRow && m_nMinCol <= m_nMaxCol);
		}

		inline int CCellRange::InRange(int row, int col) const
		{
			 return (row >= m_nMinRow && row <= m_nMaxRow && col >= m_nMinCol && col <= m_nMaxCol);
		}

		inline int CCellRange::InRange(const CCellID& cellID) const
		{
			 return InRange(cellID.row, cellID.col);
		}

		inline CCellID CCellRange::GetTopLeft() const
		{
			 return CCellID(m_nMinRow, m_nMinCol);
		}

		inline CCellRange CCellRange::Intersect(const CCellRange& rhs) const
		{
			 return CCellRange(std::max<int>(m_nMinRow,rhs.m_nMinRow), std::max<int>(m_nMinCol,rhs.m_nMinCol),
							   std::min<int>(m_nMaxRow,rhs.m_nMaxRow), std::min<int>(m_nMaxCol,rhs.m_nMaxCol));
		}
	} //namespace GridCtrlLibrary
} //namespace Seidenader

namespace SvGcl = Seidenader::GridCtrlLibrary;

