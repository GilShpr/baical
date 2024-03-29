//******************************************************************************
// Copyright 2010, 2011, 2012  Zheltovskiy Andrey                              *
//                                                                             *
//   Licensed under the Apache License, Version 2.0 (the "License");           *
//   you may not use this file except in compliance with the License.          *
//   You may obtain a copy of the License at                                   *
//                                                                             *
//       http://www.apache.org/licenses/LICENSE-2.0                            *
//                                                                             *
//  Unless  required  by  applicable  law  or  agreed  to in writing, software *
//  distributed  under the License is distributed on an "AS IS" BASIS, WITHOUT *
//  WARRANTIES  OR  CONDITIONS OF ANY KIND, either express or implied. See the *
//  License  for  the  specific language governing permissions and limitations *
//  under the License.                                                         *
//                                                                             *
//******************************************************************************

#include <stdlib.h>


#include "GTypes.h"

#include "P7_Client.h"
#include "P7_Trace.h"
#include "P7_Telemetry.h"


int main(int i_iArgC, char* i_pArgV[])
{
    IP7_Client    *l_pClient    = NULL;
    IP7_Trace     *l_pTrace_1   = NULL;
    IP7_Trace     *l_pTrace_2   = NULL;
    tUINT32        l_dwIdx      = 0;
    IP7_Telemetry *l_pTelemetry = NULL;
    tUINT8         l_pTID       = 0;

    //create P7 client object
    l_pClient = P7_Create_Client(0);

    if (NULL == l_pClient)
    {
        goto l_lblExit;
    }

    //create P7 telemetry object
    l_pTelemetry = P7_Create_Telemetry(l_pClient, TM("Telemetry channel 1"));
    if (NULL == l_pTelemetry)
    {
        goto l_lblExit;
    }

    //register telemetry counter, it has values in range 0 ... 1023
    if (FALSE == l_pTelemetry->Create(TM("Group/counter 1"), 0, 1023, 1000, 1, &l_pTID))
    {
        goto l_lblExit;
    }

    //create P7 trace object 1
    l_pTrace_1 = P7_Create_Trace(l_pClient, TM("Trace channel 1"));
    if (NULL == l_pTrace_1)
    {
        goto l_lblExit;
    }

    //create P7 trace object 2
    l_pTrace_2 = P7_Create_Trace(l_pClient, TM("Trace channel 2"));
    if (NULL == l_pTrace_2)
    {
        goto l_lblExit;
    }

    //send few trace messages
    l_pTrace_1->P7_TRACE(0, TM("Test trace message #%d"), l_dwIdx ++);
    l_pTrace_1->P7_INFO(0, TM("Test info message #%d"), l_dwIdx ++);
    l_pTrace_1->P7_DEBUG(0, TM("Test debug message #%d"), l_dwIdx ++);
    l_pTrace_1->P7_WARNING(0, TM("Test warning message #%d"), l_dwIdx ++);
    l_pTrace_1->P7_ERROR(0, TM("Test error message #%d"), l_dwIdx ++);
    l_pTrace_1->P7_CRITICAL(0, TM("Test critical message #%d"), l_dwIdx ++);
    
    l_pTrace_2->P7_QTRACE(1, 0, TM("Test quick trace on channel %d"), 2);

    for (tUINT64 l_qwI = 0ULL; l_qwI < 1000000ULL; l_qwI ++)
    {
        l_pTelemetry->Add(l_pTID, (l_qwI & 0x3FF));
    }
l_lblExit:
    if (l_pTelemetry)
    {
        l_pTelemetry->Release();
        l_pTelemetry = NULL;
    }

    if (l_pTrace_1)
    {
        l_pTrace_1->Release();
        l_pTrace_1 = NULL;
    }

    if (l_pTrace_2)
    {
        l_pTrace_2->Release();
        l_pTrace_2 = NULL;
    }

    if (l_pClient)
    {
        l_pClient->Release();
        l_pClient = NULL;
    }

    return 0;
}

