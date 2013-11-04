//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : Err200_249
//* .File Name       : $Workfile:   ec200249.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:40:20  $
//******************************************************************************

#include "stdafx.h"
#include "SVErrorClass.h"

//
// Next error number to use 200003  - 16 Nov 1999 - frb.
//
int SVErrorClass::Err200_249 (int *found)
{

    *found = TRUE;
    
    switch (msvlLastProgramCd)
    {
		//
        case 20000:
            //switch (lastErrorCd & 0x0000ffff)
            //{
            //    case 20000:

                    msvlLastErrorAction = DISPLAY +
                                          ACKNOWLEDGE +
                                          LOG + 
									               TERMINATE +
                                          BRK;

                    msvlLastFacillity = SVR;
                    msvlLastMessageNbr = 0; //?????? - Message Text handle.
                    break;

            //    default:
            //        break;
            //}
            //break;

        default:

            *found = FALSE;
            break;
    }


    return 0;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVStatusLibrary\ec200249.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 17:40:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Sep 2009 15:41:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved file to this library to allow for multiple project inclusion.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   16 Feb 2005 13:01:12   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  440
   SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     changed header
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.1   17 Apr 2003 15:51:20   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
