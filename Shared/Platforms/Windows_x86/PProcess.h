//******************************************************************************
// Copyright 2012  Zheltovskiy Andrey                                          *
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

#pragma once


class CProc
{
public:
    ///////////////////////////////////////////////////////////////////////////////
    //Get_ArgV
    static tXCHAR **Get_ArgV(tINT32 *o_pCount)
    {
        return CommandLineToArgvW(GetCommandLineW(), o_pCount);
    }//Get_ArgV


    ///////////////////////////////////////////////////////////////////////////////
    //Get_ArgV
    static tXCHAR **Get_ArgV(const tXCHAR *i_pCmdLine, tINT32 *o_pCount)
    {
        return CommandLineToArgvW(i_pCmdLine, o_pCount);
    }//Get_ArgV


    ///////////////////////////////////////////////////////////////////////////////
    //Free_ArgV
    static void Free_ArgV(tXCHAR **i_pArgV)
    {
        LocalFree(i_pArgV);
    }//Free_ArgV


    ////////////////////////////////////////////////////////////////////////////////
    //Get_Process_Time()
    //return a 64-bit value divided into 2 parts representing the number of 
    //100-nanosecond intervals since January 1, 1601 (UTC).
    static tBOOL Get_Process_Time(tUINT32 *o_pHTime, tUINT32 *o_pLTime)
    {
        FILETIME l_tProcess_Time = {0};
        FILETIME l_tStub_01      = {0};
        FILETIME l_tStub_02      = {0};
        FILETIME l_tStub_03      = {0};
        
        GetProcessTimes(GetCurrentProcess(), 
                        &l_tProcess_Time, 
                        &l_tStub_01,
                        &l_tStub_02,
                        &l_tStub_03
                        );
        
        *o_pHTime = l_tProcess_Time.dwHighDateTime;
        *o_pLTime = l_tProcess_Time.dwLowDateTime;

        return TRUE;
    }//Get_Process_Time()


    ////////////////////////////////////////////////////////////////////////////////
    //Get_Process_Name
    static tBOOL Get_Process_Name(tWCHAR *o_pName, tINT32 i_iMax_Len)
    {
        BOOL           l_bReturn       = FALSE;
        const tUINT32  l_dwMax_Len     = 32768;
        wchar_t       *l_pProcess_Path = NULL;
        wchar_t       *l_pProcess_Name = NULL;
        
        if (    (NULL == o_pName)
             || (64   >= i_iMax_Len)
           )
        {
            return FALSE;
        }

        l_pProcess_Path = new wchar_t[l_dwMax_Len];

        if (NULL == l_pProcess_Path)
        {
            return FALSE;
        }

        if (GetModuleFileNameW(GetModuleHandleW(NULL), 
                               l_pProcess_Path, 
                               l_dwMax_Len
                              )
           )
        {
            if (    (l_pProcess_Name = wcsrchr(l_pProcess_Path, L'\\'))
                 || (l_pProcess_Name = wcsrchr(l_pProcess_Path, L'/'))
               )
            {
                l_pProcess_Name ++;
                l_bReturn = TRUE;
            }
        }


        if (l_bReturn)
        {
            wcscpy_s((wchar_t*)o_pName, i_iMax_Len, l_pProcess_Name);
        }

        delete [] l_pProcess_Path;

        return l_bReturn;
    }//Get_Process_Name
    

    ////////////////////////////////////////////////////////////////////////////////
    //Get_Process_ID
    static tUINT32 Get_Process_ID()
    {
        return GetCurrentProcessId();
    }//Get_Process_ID


    ////////////////////////////////////////////////////////////////////////////////
    //Get_Thread_Id
    static tUINT32 Get_Thread_Id()
    {
        return GetCurrentThreadId();
    }//Get_Thread_Id


    ////////////////////////////////////////////////////////////////////////////////
    //Get_Processor
    static __forceinline tUINT32 Get_Processor()
    {
        #if defined(_M_X64) || defined(__amd64__) || defined(__amd64) || defined(_WIN64)
           //return 0xFFFF; //don't know how to get processor number ... yet.
           return GetCurrentProcessorNumber();
        #else
           _asm
           {
               mov eax, 1
               cpuid
               shr ebx, 24
               mov eax, ebx
           }
        #endif
    }//Get_Processor
};
