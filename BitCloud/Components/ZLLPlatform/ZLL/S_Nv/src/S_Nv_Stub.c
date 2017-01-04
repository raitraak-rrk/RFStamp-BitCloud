#ifndef _ENABLE_PERSISTENT_SERVER_
#include "S_Nv_Init.h"
#include "S_Nv_Bindings.h"
#include "S_Nv.h"
/** Initializes the component and tells which items are persistent against S_Nv_EraseAll(FALSE).
*/
void S_Nv_Init(void)
{}

/** Initializes an item with default data if it doesn't exist yet, or reads it when it does exist.

    \param id The item to initialize.
    \param itemLength The length of the item.
    \param pDefaultData Buffer to 'itemLength' bytes, or NULL.
    \returns S_Nv_ReturnValue_Ok, S_Nv_ReturnValue_DidNotExist or S_Nv_ReturnValue_PowerSupplyTooLow.

    If the item doesn't exist, it is written with the given default data and the status code
    S_Nv_ReturnValue_DidNotExist is returned. Otherwise, the default data is not used, the item
    is kept as it was but read into pDefaultData, and S_Nv_ReturnValue_Ok is returned.
    If NULL is passed for pDefaultData, and the item doesn't exist all data is initialized to 0xFF.

    \note pDefaultData must not point to constant data.
*/
S_Nv_ReturnValue_t S_Nv_ItemInit_Impl(S_Nv_ItemId_t id, uint16_t itemLength, void* pDefaultData)
{
  (void)id, (void)itemLength, (void)pDefaultData;
  return 0;
}

/** Writes an item.

    \param id The item to write.
    \param offset The offset in the item to start writing to.
    \param dataLength The length of the data to write.
    \param pData Pointer to 'dataLength' bytes to write.
    \returns S_Nv_ReturnValue_Ok, S_Nv_ReturnValue_DoesNotExist, S_Nv_ReturnValue_BeyondEnd or S_Nv_ReturnValue_PowerSupplyTooLow.

    If any of the bytes fall outside the item lenght as passed during S_Nv_ItemInit(), the whole
    write operation does not occur and S_Nv_ReturnValue_BeyondEnd is returned.
*/
S_Nv_ReturnValue_t S_Nv_Write_Impl(S_Nv_ItemId_t id, uint16_t offset, uint16_t dataLength, void* pData)
{
  (void)id, (void)dataLength, (void)pData, (void)offset;
  return 0;
}
#endif

#ifndef ATMEL_APPLICATION_SUPPORT
#include <N_ErrH.h>
#include <N_Log.h>

/** A fatal error occured.

    Not intended to be called directly. Use the macros instead.
*/
void N_ErrH_Fatal(const char* compId, uint16_t line)
{
  (void)compId, (void)line;
}

/** Prepares the following N_Log_Trace() function call
    \param compId The component ID
    \param level The log-level
    \param func The name of the function (for function logging) or NULL
    \note Not to be directly used, only through the N_LOG macro
*/
void N_Log_Prepare(const char* compId, N_Log_Level_t level, const char* func)
{
  (void)compId, (void)level, (void)func;
}

/** Print log message. Not to be directly used (only to be used via N_LOG macro)
    \pre N_Log_Prepare() must be called immediately before calling this function.
*/
void N_Log_Trace(const char* format, ...)
{
  (void)format;
}
#endif