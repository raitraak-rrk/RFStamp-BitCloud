/* Temporary workaround until stdPDS will be updated with splitted memory IDs range  */
#if (defined (PDS_ENABLE_WEAR_LEVELING) && (PDS_ENABLE_WEAR_LEVELING == 1))
  #include <wlPdsMemIds.h>
#else
  #include <stdPdsMemIds.h>
#endif // PDS_ENABLE_WEAR_LEVELING == 1
