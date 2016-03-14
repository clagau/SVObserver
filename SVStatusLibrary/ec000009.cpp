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
