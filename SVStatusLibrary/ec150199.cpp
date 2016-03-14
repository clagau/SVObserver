//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : Err150_199
//* .File Name       : $Workfile:   ec150199.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:40:06  $
//******************************************************************************

#include "stdafx.h"
#include "SVErrorClass.h"

int SVErrorClass::Err150_199 (int *found)
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

