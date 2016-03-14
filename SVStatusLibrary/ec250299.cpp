//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : Err250_299
//* .File Name       : $Workfile:   ec250299.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:40:34  $
//******************************************************************************

#include "stdafx.h"
#include "SVErrorClass.h"

int SVErrorClass::Err250_299 (int *found)
{
    *found = FALSE;
/*----- EXAMPLE ---------------------------------------------
    *found = TRUE;
    
    switch (msvlLastProgramCd)
    {
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
        default:

            *found = FALSE;
            break;
    }
*/
    return 0;
}

