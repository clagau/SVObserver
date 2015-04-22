//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDlgGraph
//* .File Name       : $Workfile:   SVDlgGraph.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:15:28  $
//******************************************************************************

#pragma once

#include "SVContainerLibrary\SVObjectArrayClassTemplate.h"

namespace Seidenader
{
	namespace SVOMFCLibrary
	{
		// Use this to define a function which is called,
		// while somebody pushes the left mouse button.
		typedef BOOL ( CALLBACK* SVProcMousePointFunc ) ( POINT, LPVOID );

		// Normalize Modes...
		enum SVNormalizeModeEnum
		{
			SVNormalize1D,	// Normalize x and y separately
			SVNormalize2D,	// Normalize x and y together ( square )
			SVNormalizeXYMinMax,// Normalize on specified X and Y max values
		};


		class SVDlgGraphClass : public CStatic
		{
		public:
			SVDlgGraphClass();

			virtual ~SVDlgGraphClass();

			BOOL SetColor(COLORREF rgb, BOOL bRedraw=TRUE);

			virtual BOOL SetPoints( const SvCl::SVObjectByteArrayClass& RByteVec );
			virtual BOOL SetPoints( const SvCl::SVObjectLongArrayClass& RLongVec );
			virtual BOOL SetPoints( const SvCl::SVObjectDoubleArrayClass& RDoubleVec );
			virtual BOOL SetPoints( const SvCl::SVObjectCPointArrayClass& RPointVec );

			BOOL SetMousePointProcFunc( SVProcMousePointFunc PFunc, LPVOID PUserData );	
			void SetNormalizeMode( SVNormalizeModeEnum NormalizeMode );
			void SetXYMinMax(long lXMin, long lYMin, long lXMax, long lYMax);

			BOOL Normalize();
			void Retransform( CPoint& RPoint );


			//{{AFX_VIRTUAL(SVDlgGraphClass)
			//}}AFX_VIRTUAL

		protected:
			//{{AFX_MSG(SVDlgGraphClass)
			afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
			afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
			afx_msg void OnMouseMove(UINT nFlags, CPoint point);
			afx_msg void OnPaint();
			//}}AFX_MSG

			DECLARE_MESSAGE_MAP()

		protected:

			SvCl::SVObjectCPointArrayClass pointVec;
			SVProcMousePointFunc pProcFunc;
			LPVOID pUserData;

			double xMult;
			double yMult;
			long xDisp;
			long yDisp;
			long yClient;
			long m_lXMin;
			long m_lYMin;
			long m_lXMax;
			long m_lYMax;

			COLORREF mrgbGraphColor;

			BOOL bButtonDown;

			SVNormalizeModeEnum normalizeMode;
		};
	} //SVOMFCLibrary
} //Seidenader

namespace SvOml = Seidenader::SVOMFCLibrary;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVDlgGraph.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:15:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   16 Dec 2009 09:01:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix the include and comment issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   05 Feb 2004 15:58:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed assumptions in displaying points
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   22 Apr 2003 09:02:34   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   10 Jan 2003 09:44:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added optional color specification
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   12 Jun 2001 13:36:10   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Changed all occurences of SVLongVector to SVObjectDoubleArrayClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   Apr 17 2001 15:43:50   steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen S. Steffan
 * Change Description:  
 *   Header file chages due to restructuring of include files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Mar 2000 14:48:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  118
 * SCR Title:  Integration of LUT Operator
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin. 
 * Provides Dialog graph for LUT page.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
