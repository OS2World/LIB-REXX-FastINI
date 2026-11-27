/*---------------------------------------------------------------------------+
|     MODULE NAME:   RexxDll.h
|
|     DESCRIPTION:   Minimal REXX external-function support layer for
|                    FASTINI.C.  The original project used an IBM sample
|                    header named RexxDll.H which was not shipped with the
|                    released source.  This header restores the small set of
|                    entry-point and helper macros FASTINI.C needs.
|
|     ABI sources:
|       - External function handler signature:
|         os2tk45\h\rexxsaa.h:454-458 (RexxFunctionHandler)
|       - SHVBLOCK layout and RXSHV_* codes:
|         os2tk45\h\rexxsaa.h:140-175
|       - RexxVariablePool prototype:
|         os2tk45\h\rexxsaa.h:438
|       - RXSHV_NEWV success-equivalent result:
|         os2tk45\h\rexxsaa.h:157
|       - APIENTRY is _System:
|         os2tk45\h\os2def.h:45
|
|     The original 98.148 DLL returns 0 from the handler itself and reports
|     success/error through the supplied RXSTRING result buffer.  This
|     compatibility layer preserves that behaviour.
+---------------------------------------------------------------------------*/

#ifndef REXXDLL_H_INCLUDED
#define REXXDLL_H_INCLUDED

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef APIRET LongRc0IfOk;


/* REXX calls an external function as:
 *   ULONG APIENTRY fn(PUCHAR module, ULONG argc, PRXSTRING argv,
 *                     PSZ queue, PRXSTRING result)
 * (rexxsaa.h:454-458).
 */
#define REX_DECLARE_REXX_ENTRY_POINT(name)                                   \
    ULONG APIENTRY name(PUCHAR ModuleId, ULONG ArgCount, PRXSTRING ArgPtr,  \
                        PSZ QueueName, PRXSTRING RxFunctionRc)

#define MODULEID()                  ((PUCHAR)ModuleId)
#define REX_ASCIIZ_ARGV(n)          ((UCHAR *)(ArgPtr[(n)].strptr))
#define REX_RETURN_TO_CMD()         return (ULONG)0


/* Set a REXX variable with the value returned by an external function.
 * RXSHV_SYSET gives normal REXX symbol handling (rexxsaa.h:143).  The
 * original header treated RXSHV_NEWV (variable did not exist) as success.
 */
static LongRc0IfOk RexSetRexxVariable(PUCHAR VariableName, PUCHAR Value)
{
    SHVBLOCK Request;
    APIRET   Rc;
    ULONG    NameLen;
    ULONG    ValueLen;

    NameLen  = (ULONG)strlen((const char *)VariableName);
    ValueLen = (ULONG)strlen((const char *)Value);

    Request.shvnext     = NULL;
    MAKERXSTRING(Request.shvname,  (PCH)VariableName, NameLen);
    MAKERXSTRING(Request.shvvalue, (PCH)Value,        ValueLen);
    Request.shvnamelen  = NameLen;
    Request.shvvaluelen = ValueLen;
    Request.shvcode     = RXSHV_SYSET;
    Request.shvret      = RXSHV_OK;

    Rc = RexxVariablePool(&Request);
    if (Rc == RXSHV_NEWV)
        Rc = 0;

    return Rc;
}


/* The REXX interpreter supplies the result buffer (normally 256 bytes). */
static void RexSetUpOkRc(PRXSTRING RxFunctionRc)
{
    RXSTRING Rc;

    MAKERXSTRING(Rc, (PCH)"0", 1UL);
    *RxFunctionRc = Rc;
}


/* Report an error through the result RXSTRING, in the same shape used by
 * the original 98.148 DLL:
 *   Failure at <source-file>(<line>): <message>
 */
static void RexSetUpErrorRcImpl(PUCHAR ModuleId, ULONG LineNumber,
                                PRXSTRING RxFunctionRc, PSZ Format, ...)
{
    va_list Args;
    char    Message[512];
    char   *FileName;
    size_t  Used;

    FileName = strrchr((const char *)ModuleId, '\\');
    if (FileName == NULL)
        FileName = (char *)ModuleId;
    else
        FileName++;

    sprintf(Message, "Failure at %s(%lu): ", FileName, LineNumber);

    va_start(Args, Format);
    Used = strlen(Message);
    vsprintf(Message + Used, (const char *)Format, Args);
    va_end(Args);

    strcpy((char *)RxFunctionRc->strptr, Message);
    RxFunctionRc->strlength = (ULONG)strlen(Message);

    /* The original support layer echoed errors when REXXDLL and DISPLAY
     * were present in the environment. */
    if ((getenv("REXXDLL") != NULL) && (getenv("DISPLAY") != NULL))
        printf("\033[0;1;31m%s\033[0m\n", Message);
}


/* FASTINI.C calls RexSetUpErrorRc(module, result, format, ...).  The
 * original support header supplied __LINE__ at the call site. */
#define RexSetUpErrorRc(module, result, ...)                                 \
    RexSetUpErrorRcImpl((PUCHAR)(module), (ULONG)__LINE__,                   \
                        (PRXSTRING)(result), __VA_ARGS__)

#define RexCheckPassedVariableCountSetUpRcAndExitOnError(expected)           \
    do {                                                                     \
        if (ArgCount != (ULONG)(expected)) {                                 \
            RexSetUpErrorRcImpl(MODULEID(), (ULONG)__LINE__, RxFunctionRc,   \
                (PSZ)"Invalid number of arguments, got %lu, expected %lu",  \
                ArgCount, (ULONG)(expected));                                \
            REX_RETURN_TO_CMD();                                             \
        }                                                                    \
    } while (0)


#endif /* REXXDLL_H_INCLUDED */
