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
#ifndef PATOMIC_H_AZH
#define PATOMIC_H_AZH

#ifdef __ARM_ARCH_5TEJ__

    //Source:
    //http://svn.dd-wrt.com/browser/src/linux/xscale/linux-2.6.23/arch/arm/kernel/entry-armv.S?rev=8329
    // Reference prototype:
    // 
    //      int __kernel_cmpxchg(int oldval, int newval, int *ptr)
    // 
    // Input:
    // 
    //      r0 = oldval
    //      r1 = newval
    //      r2 = ptr
    //      lr = return address
    // 
    // Output:
    // 
    //      r0 = returned value (zero or non-zero)
    //      C flag = set if r0 == 0, clear if r0 != 0
    // 
    // Clobbered:
    // 
    //      r3, ip, flags
    // 
    // Definition and user space usage example:
    // 
    //      typedef int (__kernel_cmpxchg_t)(int oldval, int newval, int *ptr);
    //      #define __kernel_cmpxchg (*(__kernel_cmpxchg_t *)0xffff0fc0)
    // 
    // Atomically store newval in *ptr if *ptr is equal to oldval for user space.
    // Return zero if *ptr was changed or non-zero if no exchange happened.
    // The C flag is also set if *ptr was changed to allow for assembly
    // optimization in the calling code.
    // 
    // Notes:
    // 
    //    - This routine already includes memory barriers as needed.
    // 
    //    - A failure might be transient, i.e. it is possible, although unlikely,
    //      that "failure" be returned even if *ptr == oldval.
    // 
    //Another implementations:
    //http://upc.lbl.gov/web-translator/2.8.0/runtime/inst/opt/include/gasnet_atomic_bits.h
    //http://upc.lbl.gov/web-translator/ - in last versions very interesting
    // For arm 6 & 7:
    //http://lxr.free-electrons.com/source/arch/arm/include/asm/atomic.h 

    #define ATOMIC_ADD(ptr, val) \
         ({ volatile register tINT32 *__ptr asm("r2") = (ptr); \
            register tINT32 __result asm("r1"); \
            asm volatile ( \
                "1: @ atomic_add\n\t" \
                "ldr     r0, [r2]\n\t" \
                "mov     r3, #0xffff0fff\n\t" \
                "add     lr, pc, #4\n\t" \
                "add     r1, r0, %2\n\t" \
                "add     pc, r3, #(0xffff0fc0 - 0xffff0fff)\n\t" \
                "bcc     1b" \
                : "=&r" (__result) \
                : "r" (__ptr), "rIL" (val) \
                : "r0","r3","ip","lr","cc","memory" ); \
            __result; })


    #define ATOMIC_SUB(ptr, val) \
         ({ volatile register tINT32 *__ptr asm("r2") = (ptr); \
            register tINT32 __result asm("r1"); \
            asm volatile ( \
                "1: @ atomic_add\n\t" \
                "ldr     r0, [r2]\n\t" \
                "mov     r3, #0xffff0fff\n\t" \
                "add     lr, pc, #4\n\t" \
                "sub     r1, r0, %2\n\t" \
                "add     pc, r3, #(0xffff0fc0 - 0xffff0fff)\n\t" \
                "bcc     1b" \
                : "=&r" (__result) \
                : "r" (__ptr), "rIL" (val) \
                : "r0","r3","ip","lr","cc","memory" ); \
            __result; })

//#ifdef __ARM_ARCH_5TEJ__
#elif defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_6__) 
    #define ATOMIC_ADD(ptr, val) \
        ({ int32_t tmp; \
           int result; \
           __asm__ __volatile__("@ atomic_add\n" \
          "1:	ldrex	%0, [%3]\n" \
          "	add	%0, %0, %4\n" \
          "	strex	%1, %0, [%3]\n" \
          "	teq	%1, #0\n" \
          "	bne	1b" \
            : "=&r" (result), "=&r" (tmp), "+Qo" (*ptr) \
            : "r" (ptr), "Ir" (val) \
            : "cc"); \
        result;})
    
    #define ATOMIC_SUB(ptr, val) \
        ({ unsigned long tmp; \
           int result; \
          __asm__ __volatile__("@ atomic_sub\n" \
          "1:	ldrex	%0, [%3]\n" \
          "	sub	%0, %0, %4\n" \
          "	strex	%1, %0, [%3]\n" \
          "	teq	%1, #0\n" \
          "	bne	1b" \
            : "=&r" (result), "=&r" (tmp), "+Qo" (*ptr) \
            : "r" (ptr), "Ir" (val) \
            : "cc"); \
        result;})
#else //#elif defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_6__) 
    
        #define ATOMIC_ADD(io_Val, i_Add) __sync_add_and_fetch(io_Val, i_Add)
        #define ATOMIC_SUB(io_Val, i_Add) __sync_sub_and_fetch(io_Val, i_Add)
    
#endif //#ifdef __ARM_ARCH_5TEJ__


#define  ATOMIC_INC(io_Val)         ATOMIC_ADD(io_Val, 1)
#define  ATOMIC_DEC(io_Val)         ATOMIC_SUB(io_Val, 1)

#endif //PATOMIC_H_AZH

