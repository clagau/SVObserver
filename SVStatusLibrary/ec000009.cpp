//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : Err000_009
//* .File Name       : $Workfile:   ec000009.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:39:08  $
//******************************************************************************

#include "stdafx.h"
#include "SVErrorClass.h"

int SVErrorClass::Err000_009 (int *found)
{

    *found = TRUE;
    
    switch (msvlLastProgramCd)
    {
        case 1304:
           {
              msvlLastErrorAction = BRK;
              msvlLastFacillity = SVR;
              msvlLastMessageNbr = 0; // ??
              break;
           }
        case 1305:
           {
              msvlLastErrorAction = BRK;
              msvlLastFacillity = SVR;
              msvlLastMessageNbr = 0; // ??
              break;
           }

        case 1307:
            switch (msvlLastErrorCd & 0x0000ffff)
            {
                case 1304:
                case 1305:
                  {
                     msvlLastErrorAction = NO_ACTION;
                     msvlLastFacillity = SVR;
                     msvlLastMessageNbr = 0; // ??

                     msvlLastErrorCd = 0;

                     break;
                  }

                default:
                  {

                     msvlLastErrorAction = DISPLAY +
                                           ACKNOWLEDGE +
                                           LOG + 
                                           BRK;

                     msvlLastFacillity = SVR;
                     msvlLastMessageNbr = 0; // ??
                    break;
                  }
            }
            break;
        case 1310:
            switch (msvlLastErrorCd & 0x0000ffff)
            {
               case 1299:
//--------------- Already flagged.
                  {
                     msvlLastErrorAction = BRK;
                     msvlLastFacillity = SVR;
                     msvlLastMessageNbr = 0; // ??
                     break;

                  }
               case 1304:
               case 1305:
                  {
                     msvlLastErrorAction = NO_ACTION;
                     msvlLastFacillity = SVR;
                     msvlLastMessageNbr = 0; // ??

                     msvlLastErrorCd = 0;
                     break;
                  }
                default:
                  {

                     msvlLastErrorAction = DISPLAY +
                                           ACKNOWLEDGE +
                                           LOG + 
                                           BRK;

                     msvlLastFacillity = SVR;
                     msvlLastMessageNbr = 0; // ??
                    break;
                  }
            }
            break;
/*----- EXAMPLE ---------------------------------------------
        case 1000:
            switch (-lastErrorCd)
            {
                case 1000:

                    lastErrorAction = DISPLAY +
                                      ACKNOWLEDGE +
                                      LOG + 
                                      BRK;

                    lastFacillity = SVR;
                    lastMessageNbr = 0; //??????
                    break;

                default:
                    break;
            }
            break;
*/

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
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVStatusLibrary\ec000009.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 17:39:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Sep 2009 15:41:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved file to this library to allow for multiple project inclusion.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   16 Feb 2005 13:01:14   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  440
   SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     changed header
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   17 Apr 2003 15:51:12   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
