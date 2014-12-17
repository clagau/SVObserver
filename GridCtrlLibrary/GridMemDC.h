//******************************************************************************
//* Copyright (c) 1996-1997
//* All Rights Reserved
//* Author           : Keith Rule keithr@europa.com
//* License          : The Code Project Open License (CPOL)
//* From             : http://www.codeproject.com/Articles/8/MFC-Grid-control
//******************************************************************************
//* .Module Name     : Titletip
//* .File Name       : $Workfile:   GridMemDC.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   16 Dec 2014 17:19:56  $
//******************************************************************************

// MemDC.h : header file
//

//////////////////////////////////////////////////
// CGridMemDC - memory DC
//
// Author: Keith Rule
// Email:  keithr@europa.com
// Copyright 1996-1997, Keith Rule
//
// You may freely use or modify this code provided this
// Copyright is included in all derived versions.
//
// History - 10/3/97 Fixed scrolling bug.
//                   Added print support.
//           25 feb 98 - fixed minor assertion bug
//
// This class implements a memory Device Context

#pragma once

namespace Seidenader
{
	namespace GridCtrlLibrary
	{
		class CGridMemDC : public CDC
		{
		public:

			// constructor sets up the memory DC
			CGridMemDC(CDC* pDC) : CDC()
			{
				ASSERT(pDC != NULL);

				m_pDC = pDC;
				m_pOldBitmap = NULL;
		#ifndef _WIN32_WCE_NO_PRINTING
				m_bMemDC = !pDC->IsPrinting();
		#else
				m_bMemDC = FALSE;
		#endif

				if (m_bMemDC)    // Create a Memory DC
				{
					pDC->GetClipBox(&m_rect);
					CreateCompatibleDC(pDC);
					m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
					m_pOldBitmap = SelectObject(&m_bitmap);
		#ifndef _WIN32_WCE
					SetWindowOrg(m_rect.left, m_rect.top);
		#endif
					// EFW - Bug fix - Fill background in case the user has overridden
					// WM_ERASEBKGND.  We end up with garbage otherwise.
					// CJM - moved to fix a bug in the fix.
					FillSolidRect(m_rect, pDC->GetBkColor());
				}
				else        // Make a copy of the relevent parts of the current DC for printing
				{
		#if !defined(_WIN32_WCE) || ((_WIN32_WCE > 201) && !defined(_WIN32_WCE_NO_PRINTING))
					m_bPrinting = pDC->m_bPrinting;
		#endif
					m_hDC       = pDC->m_hDC;
					m_hAttribDC = pDC->m_hAttribDC;
				}

			}

			// Destructor copies the contents of the mem DC to the original DC
			virtual ~CGridMemDC()
			{
				if (m_bMemDC)
				{
					// Copy the offscreen bitmap onto the screen.
					m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
								  this, m_rect.left, m_rect.top, SRCCOPY);

					//Swap back the original bitmap.
					SelectObject(m_pOldBitmap);
				} else {
					// All we need to do is replace the DC with an illegal value,
					// this keeps us from accidently deleting the handles associated with
					// the CDC that was passed to the constructor.
					m_hDC = m_hAttribDC = NULL;
				}
			}

			// Allow usage as a pointer
			CGridMemDC* operator->() {return this;}

			// Allow usage as a pointer
			operator CGridMemDC*() {return this;}

		private:
			CBitmap  m_bitmap;      // Offscreen bitmap
			CBitmap* m_pOldBitmap;  // bitmap originally found in CGridMemDC
			CDC*     m_pDC;         // Saves CDC passed in constructor
			CRect    m_rect;        // Rectangle of drawing area.
			BOOL     m_bMemDC;      // TRUE if CDC really is a Memory DC.
		};
	} //namespace GridCtrlLibrary
} //namespace Seidenader

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\GridCtrlLibrary\GridMemDC.h_v  $
 * 
 *    Rev 1.1   16 Dec 2014 17:19:56   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  933
 * SCR Title:  Add Filter Tab to Object Selector (SVO-377)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed destructor to virtual.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   24 Oct 2014 10:32:32   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  942
 * SCR Title:  Create new Custom2 Filter SVO-324 SVO-67 SVO-74
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 */