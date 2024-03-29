////////////////////////////////////////////////////////////////////////////////
// Copyright 2011 Zheltovskiy Andrey                                           /
//                                                                             /
//   Licensed under the Apache License, Version 2.0 (the "License");           /
//   you may not use this file except in compliance with the License.          /
//   You may obtain a copy of the License at                                   /
//                                                                             /
//       http://www.apache.org/licenses/LICENSE-2.0                            /
//                                                                             /
//  Unless  required  by  applicable  law  or  agreed  to in writing, software /
//  distributed  under the License is distributed on an "AS IS" BASIS, WITHOUT /
//  WARRANTIES  OR  CONDITIONS OF ANY KIND, either express or implied. See the /
//  License  for  the  specific language governing permissions and limitations /
//  under the License.                                                         /
//                                                                             /
////////////////////////////////////////////////////////////////////////////////
// Simple logger to file/stdout                                                /
////////////////////////////////////////////////////////////////////////////////
#ifndef PJOURNAL_H_AZH
#define PJOURNAL_H_AZH

//#include <stdio.h>
//#include <string.h>
//#include <stdarg.h>
//#include <iostream>
//#include <pthread.h> //-pthread
//#include <time.h>
//#include "Atomic.hpp"
//#include "Lock.hpp"
//#include "PTypes.hpp"
//#include "GTypes.hpp"
//#include "IJournal.hpp"


////////////////////////////////////////////////////////////////////////////////
class CJournal
    : public IJournal //virtual
{
    volatile tINT32    m_iRCnt;
    tLOCK              m_hCS;
    tUINT64            m_pCount[EFJOIRNAL_TYPES_COUNT];
    eFJournal_Type     m_eVerbosity;
    tUINT32            m_pLength;
    tXCHAR            *m_pBuffer;
    tXCHAR             m_pName[EFJOIRNAL_TYPES_COUNT][16];
public:
    ////////////////////////////////////////////////////////////////////////////
    CJournal()
        : m_iRCnt(1)
        , m_eVerbosity(EFJOIRNAL_TYPE_WARNING)
        , m_pLength(8192)
    {
        LOCK_CREATE(m_hCS);
        
        for (tUINT32 l_dwI = 0; l_dwI < EFJOIRNAL_TYPES_COUNT; l_dwI++)
        {
            m_pName[l_dwI][0] = 0;
            m_pCount[l_dwI]   = 0ULL;
        }
        
        strcpy(m_pName[EFJOIRNAL_TYPE_DEBUG],    "DEBUG  ");
        strcpy(m_pName[EFJOIRNAL_TYPE_INFO],     "INFORM.");
        strcpy(m_pName[EFJOIRNAL_TYPE_WARNING],  "WARNING");
        strcpy(m_pName[EFJOIRNAL_TYPE_ERROR],    "ERROR  ");
        strcpy(m_pName[EFJOIRNAL_TYPE_CRITICAL], "CRITIC.");
    }

        
    ////////////////////////////////////////////////////////////////////////////
    ~CJournal()
    {
        if (m_pBuffer)
        {
            delete [] m_pBuffer;
            m_pBuffer = NULL;
        }
        
        //if (m_iRCnt)
        //{
        //    printf("Error: CJournal reference counter is not 0");
        //}
        LOCK_DESTROY(m_hCS);
    }
    
    
    ////////////////////////////////////////////////////////////////////////////
    tBOOL Initialize(const tXCHAR *i_pName)
    {
        UNUSED_ARG(i_pName);

        if (m_pBuffer)
        {	
            return TRUE;
        }

        m_pBuffer = new tXCHAR[m_pLength];
        
        return m_pBuffer ? TRUE : FALSE;
    }
    
    
    ////////////////////////////////////////////////////////////////////////////
    void Set_Verbosity(eFJournal_Type i_eVerbosity)
    {
        LOCK_ENTER(m_hCS);
        m_eVerbosity = i_eVerbosity;
        LOCK_EXIT(m_hCS);
    }
    

    ////////////////////////////////////////////////////////////////////////////
    eFJournal_Type Get_Verbosity()
    {
        eFJournal_Type l_eReturn = EFJOIRNAL_TYPE_WARNING;

        LOCK_ENTER(m_hCS);
        l_eReturn =  m_eVerbosity;
        LOCK_EXIT(m_hCS);
        
        return l_eReturn;
    }
    
    
    ////////////////////////////////////////////////////////////////////////////
    tBOOL Log(eFJournal_Type i_eType, 
              const char    *i_pFunction, 
              tUINT32        i_dwLine,
              const tXCHAR  *i_pFormat, 
              ...
             )
    {
        tBOOL           l_bReturn = TRUE;
        time_t          l_tNow    = time(NULL);
        struct tm      *l_pTime   = localtime(&l_tNow);
        struct timespec l_sTime   = {0, 0};
        int             l_iRes    = 0;
        int             l_iOffs   = 0;
        tUINT64         l_qwMSec  = 0;
        
        LOCK_ENTER(m_hCS);

        if (    (i_eType < m_eVerbosity)
             || (NULL == m_pBuffer) 
             || (i_eType >= EFJOIRNAL_TYPES_COUNT)   
           )
        {
            l_bReturn = FALSE;
            goto l_lExit;
        }

        m_pCount[i_eType] ++;

        l_iRes = snprintf(m_pBuffer + l_iOffs, 
                          m_pLength - l_iOffs,
                          "[%s] : [%d]\n", 
                          i_pFunction,
                          (int)i_dwLine
                         );
        if (0 <= l_iRes)
        {
            l_iOffs += l_iRes;
        }

        clock_gettime(CLOCK_MONOTONIC, &l_sTime);

        l_qwMSec  = l_sTime.tv_sec;
        l_qwMSec *= 1000;
        l_qwMSec += l_sTime.tv_nsec/1000000;
                
        l_iRes = snprintf(m_pBuffer + l_iOffs, 
                          m_pLength - l_iOffs,
                          "%04d.%02d.%02d %02d:%02d:%02d.%03d: %s: ", 
                          l_pTime->tm_year + 1900,
                          l_pTime->tm_mon + 1,
                          l_pTime->tm_mday,
                          l_pTime->tm_hour,
                          l_pTime->tm_min,
                          l_pTime->tm_sec,
                          (int)(l_qwMSec % 1000),
                          m_pName[i_eType]
                         );
        if (0 <= l_iRes)
        {
            l_iOffs += l_iRes;
        }
       
        va_list l_pVA;
        va_start(l_pVA, i_pFormat);
        vsnprintf(m_pBuffer + l_iOffs, m_pLength - l_iOffs, i_pFormat, l_pVA);
        va_end(l_pVA);
        
        //std::cout << m_pBuffer << "\n";
        printf(m_pBuffer, 0); //0 to close warning from G++
        printf("\n");
        
       
    l_lExit:
        LOCK_EXIT(m_hCS);
        return l_bReturn;    
    }

    
    ////////////////////////////////////////////////////////////////////////////
    tUINT64 Get_Count(eFJournal_Type i_eType) 
    {
        tUINT64 l_qwReturn = 0ULL;

        if (i_eType >= EFJOIRNAL_TYPES_COUNT)   
        {
            return 0ULL;
        }

        LOCK_ENTER(m_hCS);
        l_qwReturn =  m_pCount[i_eType];
        LOCK_EXIT(m_hCS);

        return l_qwReturn;
    }
    
    
    ////////////////////////////////////////////////////////////////////////////
    tINT32  Add_Ref()         
    {
        return ATOMIC_INC(&m_iRCnt);
    }

    
    ////////////////////////////////////////////////////////////////////////////
    tINT32  Release()  
    {
        volatile tINT32 l_iReturn = ATOMIC_DEC(&m_iRCnt);

        if (0 >= l_iReturn)
        {
            delete this;
        }
        
        return l_iReturn;
    }
};

#endif //PJOURNAL_H_AZH
