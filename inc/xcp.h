/*
 * BlueParrot XCP
 *
 * (C) 2007-2021 by Christoph Schueler <github.com/Christoph2,
 *                                      cpu12.gems@googlemail.com>
 *
 * All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * s. FLOSS-EXCEPTION.txt
 */

#if !defined(__CXCP_H)
#define __CXCP_H

#include "xcp_types.h"
#include "xcp_macros.h"
#include "xcp_util.h"

#define XCP_PROTOCOL_VERSION_MAJOR       (1)
#define XCP_PROTOCOL_VERSION_RELEASE     (0)

#define XCP_TRANSPORT_LAYER_VERSION_MAJOR       (1)
#define XCP_TRANSPORT_LAYER_VERSION_RELEASE     (1)

#define XCP_ON                  (1)
#define XCP_OFF                 (0)

#define XCP_ON_CAN              (0)
#define XCP_ON_CANFD            (1)
#define XCP_ON_FLEXRAY          (2)
#define XCP_ON_USB              (3)
#define XCP_ON_ETHERNET         (4)
#define XCP_ON_SXI              (5)

#define XCP_DAQ_CONFIG_TYPE_STATIC      (0)
#define XCP_DAQ_CONFIG_TYPE_DYNAMIC     (1)
#define XCP_DAQ_CONFIG_TYPE_NONE        (3)

#if XCP_ENABLE_EXTERN_C_GUARDS == XCP_ON
#if defined(__cplusplus)
extern "C"
{
#endif  /* __cplusplus */
#endif /* XCP_EXTERN_C_GUARDS */

#include "xcp_config.h"

/*
** Configuration checks.
*/

/* Check for unsupported features. */
#if XCP_ENABLE_STIM  == XCP_ON
    #error STIM not supported yet.
#endif /* XCP_ENABLE_STIM */

#if XCP_DAQ_ENABLE_ALTERNATING == XCP_ON
    #error XCP_DAQ_ENABLE_ALTERNATING not supported yet.
#endif /* XCP_DAQ_ENABLE_ALTERNATING */

#if XCP_DAQ_ENABLE_PRIORITIZATION == XCP_ON
    #error DAQ priorization not supported yet.
#endif /* XCP_DAQ_ENABLE_PRIORITIZATION */

#if XCP_DAQ_ENABLE_BIT_OFFSET == XCP_ON
    #error DAQ doesnt support bit-offsets yet.
#endif /* XCP_DAQ_ENABLE_BIT_OFFSET */

#if XCP_DAQ_ENABLE_ADDR_EXT == XCP_ON
    #error DAQ doesnt support address extension.
#endif /* XCP_DAQ_ENABLE_ADDR_EXT */

#if XCP_DAQ_ENABLE_MULTIPLE_DAQ_LISTS_PER_EVENT == XCP_ON
    #error Multiple DAQ lists per event are not supported yet.
#endif /* XCP_DAQ_ENABLE_MULTIPLE_DAQ_LISTS_PER_EVENT */

#if XCP_ENABLE_MASTER_BLOCKMODE == XCP_ON
    #if XCP_ENABLE_DOWNLOAD_NEXT == XCP_OFF
        #error Master block-mode requires optional command 'downloadNext'.
    #endif /* XCP_ENABLE_DOWNLOAD_NEXT */
#endif /* XCP_ENABLE_MASTER_BLOCKMODE */

#if XCP_TRANSPORT_LAYER == XCP_ON_CAN

#if (!defined(XCP_ENABLE_CAN_FD)) || (XCP_ENABLE_CAN_FD == XCP_OFF)
    #ifdef XCP_MAX_CTO
        #undef XCP_MAX_CTO
    #endif /* XCP_MAX_CTO */
    #define XCP_MAX_CTO (8)

    #ifdef XCP_MAX_DTO
        #undef XCP_MAX_DTO
    #endif /* XCP_MAX_DTO */
    #define XCP_MAX_DTO (8)
#else
    #if (XCP_MAX_CTO < 8) || (XCP_MAX_CTO > 64)
        #error MaxCTO must be in range [8 .. 64]
    #endif

    #if (XCP_MAX_DTO < 8) || (XCP_MAX_DTO > 64)
        #error MaxDTO must be in range [8 .. 64]
    #endif

#endif

    #ifdef XCP_TRANSPORT_LAYER_LENGTH_SIZE
        #undef XCP_TRANSPORT_LAYER_LENGTH_SIZE
    #endif /* XCP_TRANSPORT_LAYER_LENGTH_SIZE */
    #define XCP_TRANSPORT_LAYER_LENGTH_SIZE (0)

    #ifdef XCP_TRANSPORT_LAYER_COUNTER_SIZE
        #undef XCP_TRANSPORT_LAYER_COUNTER_SIZE
    #endif /* XCP_TRANSPORT_LAYER_COUNTER_SIZE */
    #define XCP_TRANSPORT_LAYER_COUNTER_SIZE (0)

    #ifdef XCP_TRANSPORT_LAYER_CHECKSUM_SIZE
        #undef XCP_TRANSPORT_LAYER_CHECKSUM_SIZE
    #endif /* XCP_TRANSPORT_LAYER_CHECKSUM_SIZE */
    #define XCP_TRANSPORT_LAYER_CHECKSUM_SIZE (0)

#endif /* XCP_TRANSPORT_LAYER */


#if (XCP_ENABLE_GET_SEED == XCP_ON) && (XCP_ENABLE_UNLOCK == XCP_ON)
    #define XCP_ENABLE_RESOURCE_PROTECTION  (XCP_ON)
#elif (XCP_ENABLE_GET_SEED == XCP_OFF) && (XCP_ENABLE_UNLOCK == XCP_OFF)
    #define XCP_ENABLE_RESOURCE_PROTECTION  (XCP_OFF)
#elif (XCP_ENABLE_GET_SEED == XCP_ON) && (XCP_ENABLE_UNLOCK == XCP_OFF)
    #error GET_SEED requires UNLOCK
#elif (XCP_ENABLE_GET_SEED == XCP_OFF) && (XCP_ENABLE_UNLOCK == XCP_ON)
    #error UNLOCK requires GET_SEED
#endif

#if XCP_DAQ_CONFIG_TYPE == XCP_DAQ_CONFIG_TYPE_NONE
    #define XCP_DAQ_ENABLE_DYNAMIC_LISTS    XCP_OFF
    #define XCP_DAQ_ENABLE_STATIC_LISTS     XCP_OFF
#elif XCP_DAQ_CONFIG_TYPE == XCP_DAQ_CONFIG_TYPE_DYNAMIC
    #define XCP_DAQ_ENABLE_DYNAMIC_LISTS    XCP_ON
    #define XCP_DAQ_ENABLE_STATIC_LISTS     XCP_OFF
#elif XCP_DAQ_CONFIG_TYPE == XCP_DAQ_CONFIG_TYPE_STATIC
    #define XCP_DAQ_ENABLE_DYNAMIC_LISTS    XCP_OFF
    #define XCP_DAQ_ENABLE_STATIC_LISTS     XCP_ON
#endif /* XCP_DAQ_CONFIG_TYPE */

#if XCP_DAQ_ENABLE_PREDEFINED_LISTS == XCP_ON
    #define XCP_MIN_DAQ XcpDaq_PredefinedListCount
#else
    #define XCP_MIN_DAQ ((XcpDaq_ListIntegerType)0)
#endif /* XCP_DAQ_ENABLE_PREDEFINED_LISTS */

#if (XCP_ENABLE_DAQ_COMMANDS == XCP_ON) && (XCP_DAQ_CONFIG_TYPE == XCP_DAQ_CONFIG_TYPE_NONE) && (XCP_DAQ_ENABLE_PREDEFINED_LISTS == STD_OFF)
    #error Neither predefined nor configurable lists are enabled.
#endif

#if (XCP_ENABLE_DAQ_COMMANDS == XCP_ON) && (XCP_DAQ_MAX_EVENT_CHANNEL < 1)
    #error XCP_DAQ_MAX_EVENT_CHANNEL must be at least 1
#endif /* XCP_DAQ_MAX_EVENT_CHANNEL */

#if (XCP_ENABLE_DAQ_COMMANDS == XCP_ON) &&  (XCP_DAQ_ENABLE_DYNAMIC_LISTS == XCP_OFF)
    #if XCP_ENABLE_FREE_DAQ == XCP_ON
        #undef XCP_ENABLE_FREE_DAQ
        #define XCP_ENABLE_FREE_DAQ     XCP_OFF
    #endif /* XCP_ENABLE_FREE_DAQ */

    #if XCP_ENABLE_ALLOC_DAQ == XCP_ON
        #undef XCP_ENABLE_ALLOC_DAQ
        #define XCP_ENABLE_ALLOC_DAQ     XCP_OFF
    #endif /* XCP_ENABLE_ALLOC_DAQ */

    #if XCP_ENABLE_ALLOC_ODT == XCP_ON
        #undef XCP_ENABLE_ALLOC_ODT
        #define XCP_ENABLE_ALLOC_ODT     XCP_OFF
    #endif /* XCP_ENABLE_ALLOC_ODT */

    #if XCP_ENABLE_ALLOC_ODT_ENTRY == XCP_ON
        #undef XCP_ENABLE_ALLOC_ODT_ENTRY
        #define XCP_ENABLE_ALLOC_ODT_ENTRY     XCP_OFF
    #endif /* XCP_ENABLE_ALLOC_ODT_ENTRY */
#endif

#define XCP_DAQ_ODT_ENTRY_OFFSET ((1) + (1))    /* Currently fixed (only abs. ODT numbers supported). */
#define XCP_DAQ_MAX_ODT_ENTRY_SIZE  (XCP_MAX_DTO - XCP_DAQ_ODT_ENTRY_OFFSET)    /* Max. payload. */

#if XCP_TRANSPORT_LAYER == XCP_ON_CAN

#else
    #if XCP_ON_CAN_MAX_DLC_REQUIRED == XCP_ON
        #error XCP_ON_CAN_MAX_DLC_REQUIRED only applies to XCP_ON_CAN
    #endif /* XCP_ON_CAN_MAX_DLC_REQUIRED */
#endif /* XCP_TRANSPORT_LAYER == XCP_ON_CAN */

#if (XCP_TRANSPORT_LAYER_COUNTER_SIZE != 0) && (XCP_TRANSPORT_LAYER_COUNTER_SIZE != 1) && (XCP_TRANSPORT_LAYER_COUNTER_SIZE != 2)
    #error XCP_TRANSPORT_LAYER_COUNTER_SIZE must be 0, 1, or 2
#endif

#if (XCP_TRANSPORT_LAYER_LENGTH_SIZE != 0) && (XCP_TRANSPORT_LAYER_LENGTH_SIZE != 1) && (XCP_TRANSPORT_LAYER_LENGTH_SIZE !=2)
    #error XCP_TRANSPORT_LAYER_LENGTH_SIZE  must be 0, 1, or 2
#endif

#define XCP_TRANSPORT_LAYER_BUFFER_OFFSET   (XCP_TRANSPORT_LAYER_COUNTER_SIZE + XCP_TRANSPORT_LAYER_LENGTH_SIZE)

#if !defined(XCP_DAQ_ENABLE_WRITE_THROUGH)
    #define XCP_DAQ_ENABLE_WRITE_THROUGH    XCP_OFF
#endif

#if !defined(XCP_DAQ_ENABLE_RESET_DYN_DAQ_CONFIG_ON_SEQUENCE_ERROR)
    #define XCP_DAQ_ENABLE_RESET_DYN_DAQ_CONFIG_ON_SEQUENCE_ERROR   XCP_OFF
#endif

#if XCP_DAQ_MAX_DYNAMIC_ENTITIES < 256
#define XCP_DAQ_ENTITY_TYPE                         uint8_t
#elif XCP_DAQ_MAX_DYNAMIC_ENTITIES < 65536
#define XCP_DAQ_ENTITY_TYPE                         uint16_t
#else
#define XCP_DAQ_ENTITY_TYPE                         uint32_t
#endif

#define XCP_SET_ID(name) { UINT16(sizeof((name)) - UINT16(1)), (uint8_t const *)(name) }

#if !defined(XCP_MAX_BS)
    #define XCP_MAX_BS      (0)
#endif  /* XCP_MAX_BS */

#if !defined(XCP_MIN_ST)
    #define  XCP_MIN_ST     (0)
#endif  /* XCP_MIN_ST */

#if !defined(XCP_MAX_BS_PGM)
    #define XCP_MAX_BS_PGM  (0)
#endif  /* XCP_MAX_BS_PGM */

#if !defined(XCP_MIN_ST_PGM)
    #define  XCP_MIN_ST_PGM (0)
#endif  /* XCP_MIN_ST_PGM */

#define XCP_DOWNLOAD_PAYLOAD_LENGTH     ((XCP_MAX_CTO) - 2)

/*
 *  Available Resources.
 */
#define XCP_RESOURCE_PGM        ((uint8_t)16)
#define XCP_RESOURCE_STIM       ((uint8_t)8)
#define XCP_RESOURCE_DAQ        ((uint8_t)4)
#define XCP_RESOURCE_CAL_PAG    ((uint8_t)1)


/*
 * Comm Mode Basic.
 */
#define XCP_BYTE_ORDER_INTEL    ((uint8_t)0)
#define XCP_BYTE_ORDER_MOTOROLA ((uint8_t)1)

#define XCP_SLAVE_BLOCK_MODE    ((uint8_t)0x40)

#define XCP_ADDRESS_GRANULARITY_0   ((uint8_t)2)
#define XCP_ADDRESS_GRANULARITY_1   ((uint8_t)4)

#define XCP_ADDRESS_GRANULARITY_BYTE    ((uint8_t)0)
#define XCP_ADDRESS_GRANULARITY_WORD    (XCP_ADDRESS_GRANULARITY_0)
#define XCP_ADDRESS_GRANULARITY_DWORD   (XCP_ADDRESS_GRANULARITY_1)

#define XCP_OPTIONAL_COMM_MODE          ((uint8_t)0x80)

#define XCP_MASTER_BLOCK_MODE           ((uint8_t)1)
#define XCP_INTERLEAVED_MODE            ((uint8_t)2)

/*
 * GetID Mode.
 */
#define XCP_COMPRESSED_ENCRYPTED        UINT8(2)
#define XCP_TRANSFER_MODE               UINT8(1)

/*
 * Current Session Status.
 */
#define RESUME                          UINT8(0x80)
#define DAQ_RUNNING                     UINT8(0x40)
#define CLEAR_DAQ_REQ                   UINT8(0x08)
#define STORE_DAQ_REQ                   UINT8(0x04)
#define STORE_CAL_REQ                   UINT8(0x01)

/*
 * SetRequest Mode.
 */
#define XCP_CLEAR_DAQ_REQ               UINT8(8)
#define XCP_STORE_DAQ_REQ_RESUME        UINT8(4)
#define XCP_STORE_DAQ_REQ_NO_RESUME     UINT8(2)
#define XCP_STORE_CAL_REQ               UINT8(1)

/*
 * SetCalPage Mode.
 */
#define XCP_SET_CAL_PAGE_ALL            UINT8(0x80)
#define XCP_SET_CAL_PAGE_XCP            UINT8(0x02)
#define XCP_SET_CAL_PAGE_ECU            UINT8(0x01)



/* DAQ List Modes. */
#define XCP_DAQ_LIST_MODE_ALTERNATING       ((uint8_t)0x01)
#define XCP_DAQ_LIST_MODE_DIRECTION         ((uint8_t)0x02)
#define XCP_DAQ_LIST_MODE_TIMESTAMP         ((uint8_t)0x10)
#define XCP_DAQ_LIST_MODE_PID_OFF           ((uint8_t)0x20)
#define XCP_DAQ_LIST_MODE_SELECTED          ((uint8_t)0x40)
#define XCP_DAQ_LIST_MODE_STARTED           ((uint8_t)0x80)

/* DAQ Properties */
#define XCP_DAQ_PROP_OVERLOAD_EVENT         ((uint8_t)0x80)
#define XCP_DAQ_PROP_OVERLOAD_MSB           ((uint8_t)0x40)
#define XCP_DAQ_PROP_PID_OFF_SUPPORTED      ((uint8_t)0x20)
#define XCP_DAQ_PROP_TIMESTAMP_SUPPORTED    ((uint8_t)0x10)
#define XCP_DAQ_PROP_BIT_STIM_SUPPORTED     ((uint8_t)0x08)
#define XCP_DAQ_PROP_RESUME_SUPPORTED       ((uint8_t)0x04)
#define XCP_DAQ_PROP_PRESCALER_SUPPORTED    ((uint8_t)0x02)
#define XCP_DAQ_PROP_DAQ_CONFIG_TYPE        ((uint8_t)0x01)

/* DAQ Key Byte */
#define XCP_DAQ_KEY_IDENTIFICATION_FIELD_TYPE_1 ((uint8_t)0x80)
#define XCP_DAQ_KEY_IDENTIFICATION_FIELD_TYPE_0 ((uint8_t)0x40)
#define XCP_DAQ_KEY_ADDRESS_EXTENSION_DAQ       ((uint8_t)0x20)
#define XCP_DAQ_KEY_ADDRESS_EXTENSION_ODT       ((uint8_t)0x10)
#define XCP_DAQ_KEY_OPTIMISATION_TYPE_3         ((uint8_t)0x08)
#define XCP_DAQ_KEY_OPTIMISATION_TYPE_2         ((uint8_t)0x04)
#define XCP_DAQ_KEY_OPTIMISATION_TYPE_1         ((uint8_t)0x02)
#define XCP_DAQ_KEY_OPTIMISATION_TYPE_0         ((uint8_t)0x01)

/* DAQ Event Channel Properties */
#define XCP_DAQ_EVENT_CHANNEL_TYPE_DAQ      ((uint8_t)0x04)
#define XCP_DAQ_EVENT_CHANNEL_TYPE_STIM     ((uint8_t)0x08)

/* DAQ Consistency */
#define XCP_DAQ_CONSISTENCY_DAQ_LIST        ((uint8_t)0x40)
#define XCP_DAQ_CONSISTENCY_EVENT_CHANNEL   ((uint8_t)0x80)

/* DAQ Time Units */
#define XCP_DAQ_EVENT_CHANNEL_TIME_UNIT_1NS    UINT8(0)
#define XCP_DAQ_EVENT_CHANNEL_TIME_UNIT_10NS   UINT8(1)
#define XCP_DAQ_EVENT_CHANNEL_TIME_UNIT_100NS  UINT8(2)
#define XCP_DAQ_EVENT_CHANNEL_TIME_UNIT_1US    UINT8(3)
#define XCP_DAQ_EVENT_CHANNEL_TIME_UNIT_10US   UINT8(4)
#define XCP_DAQ_EVENT_CHANNEL_TIME_UNIT_100US  UINT8(5)
#define XCP_DAQ_EVENT_CHANNEL_TIME_UNIT_1MS    UINT8(6)
#define XCP_DAQ_EVENT_CHANNEL_TIME_UNIT_10MS   UINT8(7)
#define XCP_DAQ_EVENT_CHANNEL_TIME_UNIT_100MS  UINT8(8)
#define XCP_DAQ_EVENT_CHANNEL_TIME_UNIT_1S     UINT8(9)
#define XCP_DAQ_EVENT_CHANNEL_TIME_UNIT_1PS    UINT8(10)
#define XCP_DAQ_EVENT_CHANNEL_TIME_UNIT_10PS   UINT8(11)
#define XCP_DAQ_EVENT_CHANNEL_TIME_UNIT_100PS  UINT8(12)

/* DAQ list properties */
#define DAQ_LIST_PROPERTY_STIM          UINT8(8)
#define DAQ_LIST_PROPERTY_DAQ           UINT8(4)
#define DAQ_LIST_PROPERTY_EVENT_FIXED   UINT8(2)
#define DAQ_LIST_PROPERTY_PREDEFINED    UINT8(1)

/*
 * DAQ List Mode
 */
#define DAQ_CURRENT_LIST_MODE_RESUME            UINT8(0x80)
#define DAQ_CURRENT_LIST_MODE_RUNNING           UINT8(0x40)
#define DAQ_CURRENT_LIST_MODE_PID_OFF           UINT8(0x20)
#define DAQ_CURRENT_LIST_MODE_TIMESTAMP         UINT8(0x10)
#define DAQ_CURRENT_LIST_MODE_DIRECTION         UINT8(0x02)
#define DAQ_CURRENT_LIST_MODE_SELECTED          UINT8(0x01)

#define XCP_DAQ_PREDEFINDED_LIST_COUNT      (sizeof(XcpDaq_PredefinedLists) / sizeof(XcpDaq_PredefinedLists[0]))

/* DAQ Implementation Macros */
#define XCP_DAQ_DEFINE_ODT_ENTRY(meas)                      \
    {                                                       \
        {(const uint32_t)&(meas)},      sizeof((meas))      \
    }

/* DAQ Event Implementation Macros */
#define XCP_DAQ_BEGIN_EVENTS    const XcpDaq_EventType XcpDaq_Events[XCP_DAQ_MAX_EVENT_CHANNEL] = {
#define XCP_DAQ_END_EVENTS      };
#define XCP_DAQ_DEFINE_EVENT(name, props, timebase, cycle)  \
    {                                                       \
        (uint8_t const * const)(name),                      \
        sizeof((name)) - 1,                                 \
        (props),                                            \
        (timebase),                                         \
        (cycle),                                            \
    }


/*
 * PAG Processor Properties.
 */
#define XCP_PAG_PROCESSOR_FREEZE_SUPPORTED  UINT8(1)


/*
 * Page Properties.
 */
#define XCP_WRITE_ACCESS_WITH_ECU           UINT8(32)
#define XCP_WRITE_ACCESS_WITHOUT_ECU        UINT8(16)
#define XCP_READ_ACCESS_WITH_ECU            UINT8(8)
#define XCP_READ_ACCESS_WITHOUT_ECU         UINT8(4)
#define ECU_ACCESS_WITH_XCP                 UINT8(2)
#define ECU_ACCESS_WITHOUT_XCP              UINT8(1)

/*
**  PGM Capabilities.
*/
#define XCP_PGM_NON_SEQ_PGM_REQUIRED        UINT8(0x80)
#define XCP_PGM_NON_SEQ_PGM_SUPPORTED       UINT8(0x40)
#define XCP_PGM_ENCRYPTION_REQUIRED         UINT8(0x20)
#define XCP_PGM_ENCRYPTION_SUPPORTED        UINT8(0x10)
#define XCP_PGM_COMPRESSION_REQUIRED        UINT8(8)
#define XCP_PGM_COMPRESSION_SUPPORTED       UINT8(4)
#define XCP_PGM_FUNCTIONAL_MODE             UINT8(2)
#define XCP_PGM_ABSOLUTE_MODE               UINT8(1)

/*
**  XCPonCAN specific function-like macros.
*/
#define XCP_ON_CAN_IS_EXTENDED_IDENTIFIER(i)    (((i) & XCP_ON_CAN_EXT_IDENTIFIER) == XCP_ON_CAN_EXT_IDENTIFIER)
#define XCP_ON_CAN_STRIP_IDENTIFIER(i)          ((i) & (~XCP_ON_CAN_EXT_IDENTIFIER))

/*
**
*/
#define XCP_HW_LOCK_XCP     UINT8(0)
#define XCP_HW_LOCK_TL      UINT8(1)
#define XCP_HW_LOCK_DAQ     UINT8(2)

#define XCP_HW_LOCK_COUNT   UINT8(3)

/*
** Global Types.
*/

#if XCP_ENABLE_DAQ_COMMANDS == XCP_ON
typedef XCP_DAQ_ENTITY_TYPE XcpDaq_ListIntegerType;
typedef XCP_DAQ_ENTITY_TYPE XcpDaq_ODTIntegerType;
typedef XCP_DAQ_ENTITY_TYPE XcpDaq_ODTEntryIntegerType;
#endif /* XCP_ENABLE_DAQ_COMMANDS */

typedef enum tagXcp_CommandType {
/*
** STD
*/
    /*
    ** Mandatory Commands.
    */
    XCP_CONNECT                 = UINT8(0xFF),
    XCP_DISCONNECT              = UINT8(0xFE),
    XCP_GET_STATUS              = UINT8(0xFD),
    XCP_SYNCH                   = UINT8(0xFC),
    /*
    ** Optional Commands.
    */
    XCP_GET_COMM_MODE_INFO      = UINT8(0xFB),
    XCP_GET_ID                  = UINT8(0xFA),
    XCP_SET_REQUEST             = UINT8(0xF9),
    XCP_GET_SEED                = UINT8(0xF8),
    XCP_UNLOCK                  = UINT8(0xF7),
    XCP_SET_MTA                 = UINT8(0xF6),
    XCP_UPLOAD                  = UINT8(0xF5),
    XCP_SHORT_UPLOAD            = UINT8(0xF4),
    XCP_BUILD_CHECKSUM          = UINT8(0xF3),

    XCP_TRANSPORT_LAYER_CMD     = UINT8(0xF2),
    XCP_USER_CMD                = UINT8(0xF1),
/*
** CAL
*/
    /*
    ** Mandatory Commands.
    */
    XCP_DOWNLOAD                = UINT8(0xF0),
    /*
    ** Optional Commands.
    */
    XCP_DOWNLOAD_NEXT           = UINT8(0xEF),
    XCP_DOWNLOAD_MAX            = UINT8(0xEE),
    XCP_SHORT_DOWNLOAD          = UINT8(0xED),
    XCP_MODIFY_BITS             = UINT8(0xEC),
/*
** PAG
*/
    /*
    ** Mandatory Commands.
    */
    XCP_SET_CAL_PAGE            = UINT8(0xEB),
    XCP_GET_CAL_PAGE            = UINT8(0xEA),
    /*
    ** Optional Commands.
    */
    XCP_GET_PAG_PROCESSOR_INFO  = UINT8(0xE9),
    XCP_GET_SEGMENT_INFO        = UINT8(0xE8),
    XCP_GET_PAGE_INFO           = UINT8(0xE7),
    XCP_SET_SEGMENT_MODE        = UINT8(0xE6),
    XCP_GET_SEGMENT_MODE        = UINT8(0xE5),
    XCP_COPY_CAL_PAGE           = UINT8(0xE4),
/*
** DAQ
*/
    /*
    ** Mandatory Commands.
    */
    XCP_CLEAR_DAQ_LIST          = UINT8(0xE3),
    XCP_SET_DAQ_PTR             = UINT8(0xE2),
    XCP_WRITE_DAQ               = UINT8(0xE1),
    WRITE_DAQ_MULTIPLE          = UINT8(0xC7), /* NEW IN 1.1 */
    XCP_SET_DAQ_LIST_MODE       = UINT8(0xE0),
    XCP_GET_DAQ_LIST_MODE       = UINT8(0xDF),
    XCP_START_STOP_DAQ_LIST     = UINT8(0xDE),
    XCP_START_STOP_SYNCH        = UINT8(0xDD),
    /*
    ** Optional Commands.
    */
    XCP_GET_DAQ_CLOCK           = UINT8(0xDC),
    XCP_READ_DAQ                = UINT8(0xDB),
    XCP_GET_DAQ_PROCESSOR_INFO  = UINT8(0xDA),
    XCP_GET_DAQ_RESOLUTION_INFO = UINT8(0xD9),
    XCP_GET_DAQ_LIST_INFO       = UINT8(0xD8),
    XCP_GET_DAQ_EVENT_INFO      = UINT8(0xD7),
    XCP_FREE_DAQ                = UINT8(0xD6),
    XCP_ALLOC_DAQ               = UINT8(0xD5),
    XCP_ALLOC_ODT               = UINT8(0xD4),
    XCP_ALLOC_ODT_ENTRY         = UINT8(0xD3),
/*
** PGM
*/
    /*
    ** Mandatory Commands.
    */
    XCP_PROGRAM_START           = UINT8(0xD2),
    XCP_PROGRAM_CLEAR           = UINT8(0xD1),
    XCP_PROGRAM                 = UINT8(0xD0),
    XCP_PROGRAM_RESET           = UINT8(0xCF),
    /*
    ** Optional Commands.
    */
    XCP_GET_PGM_PROCESSOR_INFO  = UINT8(0xCE),
    XCP_GET_SECTOR_INFO         = UINT8(0xCD),
    XCP_PROGRAM_PREPARE         = UINT8(0xCC),
    XCP_PROGRAM_FORMAT          = UINT8(0xCB),
    XCP_PROGRAM_NEXT            = UINT8(0xCA),
    XCP_PROGRAM_MAX             = UINT8(0xC9),
    XCP_PROGRAM_VERIFY          = UINT8(0xC8)

} Xcp_CommandType;


typedef enum tagXcp_ReturnType {
    ERR_CMD_SYNCH           = UINT8(0x00), /* Command processor synchronization.                            S0 */

    ERR_CMD_BUSY            = UINT8(0x10), /* Command was not executed.                                     S2 */
    ERR_DAQ_ACTIVE          = UINT8(0x11), /* Command rejected because DAQ is running.                      S2 */
    ERR_PGM_ACTIVE          = UINT8(0x12), /* Command rejected because PGM is running.                      S2 */

    ERR_CMD_UNKNOWN         = UINT8(0x20), /* Unknown command or not implemented optional command.          S2 */
    ERR_CMD_SYNTAX          = UINT8(0x21), /* Command syntax invalid                                        S2 */
    ERR_OUT_OF_RANGE        = UINT8(0x22), /* Command syntax valid but command parameter(s) out of range.   S2 */
    ERR_WRITE_PROTECTED     = UINT8(0x23), /* The memory location is write protected.                       S2 */
    ERR_ACCESS_DENIED       = UINT8(0x24), /* The memory location is not accessible.                        S2 */
    ERR_ACCESS_LOCKED       = UINT8(0x25), /* Access denied, Seed & Key is required                         S2 */
    ERR_PAGE_NOT_VALID      = UINT8(0x26), /* Selected page not available                                   S2 */
    ERR_MODE_NOT_VALID      = UINT8(0x27), /* Selected page mode not available                              S2 */
    ERR_SEGMENT_NOT_VALID   = UINT8(0x28), /* Selected segment not valid                                    S2 */
    ERR_SEQUENCE            = UINT8(0x29), /* Sequence error                                                S2 */
    ERR_DAQ_CONFIG          = UINT8(0x2A), /* DAQ configuration not valid                                   S2 */

    ERR_MEMORY_OVERFLOW     = UINT8(0x30), /* Memory overflow error                                         S2 */
    ERR_GENERIC             = UINT8(0x31), /* Generic error.                                                S2 */
    ERR_VERIFY              = UINT8(0x32), /* The slave internal program verify routine detects an error.   S3 */

    /* NEW IN 1.1 */
    ERR_RESOURCE_TEMPORARY_NOT_ACCESSIBLE = UINT8(0x33),    /* Access to the requested resource is temporary not possible.   S3 */

    /* Internal Success Code - not related to XCP spec. */
    ERR_SUCCESS             = UINT8(0xff)
} Xcp_ReturnType;


typedef struct tagXcp_MtaType {
    uint8_t ext;
    uint32_t address;
} Xcp_MtaType;


#if XCP_ENABLE_DAQ_COMMANDS == XCP_ON
typedef struct tagXcpDaq_MtaType {
#if XCP_DAQ_ENABLE_ADDR_EXT == XCP_ON
    uint8_t ext;
#endif /* XCP_DAQ_ENABLE_ADDR_EXT */
    uint32_t address;
} XcpDaq_MtaType;


typedef enum tagXcpDaq_ProcessorStateType {
    XCP_DAQ_STATE_UNINIT            = 0,
    XCP_DAQ_STATE_CONFIG_INVALID    = 1,
    XCP_DAQ_STATE_CONFIG_VALID      = 2,
    XCP_DAQ_STATE_STOPPED           = 3,
    XCP_DAQ_STATE_RUNNING           = 4
} XcpDaq_ProcessorStateType;

typedef struct tagXcpDaq_ProcessorType {
    XcpDaq_ProcessorStateType state;
} XcpDaq_ProcessorType;

typedef struct tagXcpDaq_PointerType {
    XcpDaq_ListIntegerType daqList;
    XcpDaq_ODTIntegerType odt;
    XcpDaq_ODTEntryIntegerType odtEntry;
} XcpDaq_PointerType;
#endif /* XCP_ENABLE_DAQ_COMMANDS */


#if XCP_ENABLE_PGM_COMMANDS == XCP_ON
typedef enum tagXcpPgm_ProcessorStateType {
    XCP_PGM_STATE_UNINIT    = 0,
    XCP_PGM_IDLE            = 1,
    XCP_PGM_ACTIVE          = 2,
} XcpPgm_ProcessorStateType;

typedef struct tagXcpPgm_ProcessorType {
    XcpPgm_ProcessorStateType state;
} XcpPgm_ProcessorType;
#endif /* ENABLE_PGM_COMMANDS */


#if (XCP_ENABLE_SLAVE_BLOCKMODE == XCP_ON) || (XCP_ENABLE_MASTER_BLOCKMODE == XCP_ON)
typedef struct tagXcp_BlockModeStateType {
    bool blockTransferActive;
    uint8_t remaining;
} Xcp_BlockModeStateType;
#endif  /* XCP_ENABLE_SLAVE_BLOCKMODE */

#if XCP_ENABLE_STATISTICS == XCP_ON
typedef struct tagXcp_StatisticsType {
    uint32_t ctosReceived;
    uint32_t crosSend;
    uint32_t crosBusy;
} Xcp_StatisticsType;
#endif /* XCP_ENABLE_STATISTICS */


typedef struct tagXcp_StateType {
    /* TODO: replace with bitmap. */
    bool connected;
    bool busy;
    bool programming;
#if XCP_ENABLE_DAQ_COMMANDS == XCP_ON
    XcpDaq_ProcessorType daqProcessor;
    XcpDaq_PointerType daqPointer;
#endif /* XCP_ENABLE_DAQ_COMMANDS */
#if XCP_ENABLE_PGM_COMMANDS == XCP_ON
    XcpPgm_ProcessorType pgmProcessor;
#endif /* ENABLE_PGM_COMMANDS */
#if XCP_TRANSPORT_LAYER_COUNTER_SIZE != 0
    uint16_t counter;
#endif /* XCP_TRANSPORT_LAYER_COUNTER_SIZE */
    uint8_t mode;
#if XCP_ENABLE_RESOURCE_PROTECTION  == XCP_ON
    uint8_t resourceProtection;
    uint8_t seedRequested;
#endif /* XCP_ENABLE_RESOURCE_PROTECTION */
    Xcp_MtaType mta;
#if XCP_ENABLE_SLAVE_BLOCKMODE == XCP_ON
    Xcp_BlockModeStateType slaveBlockModeState;
#endif  /* XCP_ENABLE_SLAVE_BLOCKMODE */
#if XCP_ENABLE_MASTER_BLOCKMODE == XCP_ON
    Xcp_BlockModeStateType masterBlockModeState;
#endif  /* XCP_ENABLE_MASTER_BLOCKMODE */
#if XCP_ENABLE_STATISTICS == XCP_ON
    Xcp_StatisticsType statistics;
#endif /* XCP_ENABLE_STATISTICS */
} Xcp_StateType;

typedef enum tagXcp_DTOType {
    EVENT_MESSAGE           = 254,
    COMMAND_RETURN_MESSAGE  = 255
} Xcp_DTOType;

typedef enum tagXcp_ConnectionStateType {
    XCP_DISCONNECTED = 0,
    XCP_CONNECTED = 1
} Xcp_ConnectionStateType;


typedef enum tagXcp_SlaveAccessType {
    XCP_ACC_PGM = 0x40,
    XCP_ACC_DAQ = 0x02,
    XCP_ACC_CAL = 0x01
} Xcp_SlaveAccessType;


typedef struct tagXcp_PDUType {
    uint16_t len;
    uint8_t * data;
} Xcp_PDUType;


typedef struct tagXcp_GetIdType {
    uint16_t len;
    uint8_t const * name;
} Xcp_GetIdType;


#if XCP_ENABLE_DAQ_COMMANDS == XCP_ON
typedef struct tagXcpDaq_ODTEntryType {
    XcpDaq_MtaType mta;
#if XCP_DAQ_ENABLE_BIT_OFFSET == XCP_ON
    uint8_t bitOffset;
#endif /* XCP_DAQ_ENABLE_BIT_OFFSET */
    uint32_t length;
} XcpDaq_ODTEntryType;

typedef struct tagXcpDaq_ODTType {
    XcpDaq_ODTEntryIntegerType numOdtEntries;
    uint16_t firstOdtEntry;
} XcpDaq_ODTType;

typedef enum tagXcpDaq_DirectionType {
    XCP_DIRECTION_NONE,
    XCP_DIRECTION_DAQ,
    XCP_DIRECTION_STIM,
    XCP_DIRECTION_DAQ_STIM
} XcpDaq_DirectionType;


typedef struct tagXcpDaq_XcpDaq_DynamicListType {
    XcpDaq_ODTIntegerType numOdts;
    uint16_t firstOdt;
    uint8_t mode;
#if XCP_DAQ_ENABLE_PRESCALER == XCP_ON
    uint8_t prescaler;
    uint8_t  counter;
#endif /* XCP_DAQ_ENABLE_PRESCALER */
} XcpDaq_DynamicListType;


typedef struct tagXcpDaq_ListConfigurationType {
    XcpDaq_ODTIntegerType numOdts;
    uint16_t firstOdt;
} XcpDaq_ListConfigurationType;


typedef struct tagXcpDaq_ListStateType {
    uint8_t mode;
#if XCP_DAQ_ENABLE_PRESCALER == XCP_ON
    uint8_t prescaler;
    uint8_t  counter;
#endif /* XCP_DAQ_ENABLE_PRESCALER */
} XcpDaq_ListStateType;


typedef enum tagXcpDaq_EntityKindType {
    XCP_ENTITY_UNUSED,
    XCP_ENTITY_DAQ_LIST,
    XCP_ENTITY_ODT,
    XCP_ENTITY_ODT_ENTRY
} XcpDaq_EntityKindType;


typedef struct tagXcpDaq_EntityType {
    /*Xcp_DaqEntityKindType*/uint8_t kind;
    union {
        XcpDaq_ODTEntryType odtEntry;
        XcpDaq_ODTType odt;
        XcpDaq_DynamicListType daqList;
    } entity;
} XcpDaq_EntityType;


typedef struct tagXcpDaq_EventType {
    uint8_t const * const name;
    uint8_t nameLen;
    uint8_t properties;
    uint8_t timeunit;
    uint8_t cycle;
    /* unit8_t priority; */
} XcpDaq_EventType;


typedef struct tagXcpDaq_DtoBufferStateType {
    uint16_t numEntries;
    uint16_t front;
    uint16_t back;
    uint16_t allocated;
} XcpDaq_DtoBufferStateType;


typedef struct tagXcpDaq_MessageType {
    uint8_t dlc;
    uint8_t const * data;
} XcpDaq_MessageType;

#endif /* XCP_ENABLE_DAQ_COMMANDS */


typedef enum tagXcp_MemoryAccessType {
    XCP_MEM_ACCESS_READ,
    XCP_MEM_ACCESS_WRITE
} Xcp_MemoryAccessType;

typedef enum tagXcp_MemoryMappingResultType {
    XCP_MEMORY_MAPPED,
    XCP_MEMORY_NOT_MAPPED,
    XCP_MEMORY_ADDRESS_INVALID
} Xcp_MemoryMappingResultType;


typedef void(*Xcp_SendCalloutType)(Xcp_PDUType const * pdu);
typedef void (*Xcp_ServerCommandType)(Xcp_PDUType const * const pdu);


typedef struct tagXcp_1DArrayType {
    uint8_t length;
    uint8_t * data;
} Xcp_1DArrayType;


typedef struct tagXcp_OptionsType {
#if defined(KVASER_CAN)
    int dummy;
#elif defined(ETHER)
    bool ipv6;
    bool tcp;
    uint16_t port;
#elif defined(SOCKET_CAN)
    bool fd;
    char interface[64];
#endif
} Xcp_OptionsType;

/*
** Global User Functions.
*/
void Xcp_Init(void);
void Xcp_MainFunction(void);

/*
** Global Helper Functions.
*/
extern Xcp_OptionsType Xcp_Options;

void Xcp_DispatchCommand(Xcp_PDUType const * const pdu);

void Xcp_Disconnect(void);
Xcp_ConnectionStateType Xcp_GetConnectionState(void);
void Xcp_SetSendCallout(Xcp_SendCalloutType callout);
Xcp_MtaType Xcp_GetNonPagedAddress(void const * const ptr);
void Xcp_SetMta(Xcp_MtaType mta);
void Xcp_SetBusy(bool enable);
bool Xcp_IsBusy(void);
void Xcp_UploadSingleBlock(void);
Xcp_StateType * Xcp_GetState(void);


#if XCP_ENABLE_DAQ_COMMANDS == XCP_ON
/*
** DAQ Implementation Functions.
*/
void XcpDaq_Init(void);
Xcp_ReturnType XcpDaq_Free(void);
Xcp_ReturnType XcpDaq_Alloc(XcpDaq_ListIntegerType daqCount);
Xcp_ReturnType XcpDaq_AllocOdt(XcpDaq_ListIntegerType daqListNumber, XcpDaq_ODTIntegerType odtCount);
Xcp_ReturnType XcpDaq_AllocOdtEntry(XcpDaq_ListIntegerType daqListNumber, XcpDaq_ODTIntegerType odtNumber, XcpDaq_ODTEntryIntegerType odtEntriesCount);
XcpDaq_ListStateType * XcpDaq_GetListState(XcpDaq_ListIntegerType daqListNumber);
XcpDaq_ListConfigurationType const * XcpDaq_GetListConfiguration(XcpDaq_ListIntegerType daqListNumber);
XcpDaq_ODTEntryType * XcpDaq_GetOdtEntry(XcpDaq_ListIntegerType daqListNumber, XcpDaq_ODTIntegerType odtNumber, XcpDaq_ODTEntryIntegerType odtEntryNumber);
bool XcpDaq_ValidateConfiguration(void);
bool XcpDaq_ValidateList(XcpDaq_ListIntegerType daqListNumber);
bool XcpDaq_ValidateOdtEntry(XcpDaq_ListIntegerType daqListNumber, XcpDaq_ODTIntegerType odtNumber, XcpDaq_ODTEntryIntegerType odtEntry);
void XcpDaq_MainFunction(void);
void XcpDaq_AddEventChannel(XcpDaq_ListIntegerType daqListNumber, uint16_t eventChannelNumber);
void XcpDaq_CopyMemory(void * dst, void * src, uint32_t len);
XcpDaq_EventType const * XcpDaq_GetEventConfiguration(uint16_t eventChannelNumber);
void XcpDaq_TriggerEvent(uint8_t eventChannelNumber);
void XcpDaq_GetProperties(uint8_t * properties);
XcpDaq_ListIntegerType XcpDaq_GetListCount(void);
void XcpDaq_SetProcessorState(XcpDaq_ProcessorStateType state);
void XcpDaq_StartSelectedLists(void);
void XcpDaq_StopSelectedLists(void);
void XcpDaq_StopAllLists(void);
bool XcpDaq_GetFirstPid(XcpDaq_ListIntegerType daqListNumber, XcpDaq_ODTIntegerType * firstPID);
bool XcpDaq_DequeueMessage(XcpDaq_MessageType * msg);
void XcpDaq_SetPointer(XcpDaq_ListIntegerType daqListNumber, XcpDaq_ODTIntegerType odtNumber, XcpDaq_ODTEntryIntegerType odtEntryNumber);
/*
**  Predefined DAQ constants.
*/
#if XCP_DAQ_ENABLE_PREDEFINED_LISTS == XCP_ON
extern const XcpDaq_ODTEntryType XcpDaq_PredefinedOdtEntries[];
extern const XcpDaq_ODTType XcpDaq_PredefinedOdts[];
extern const XcpDaq_ListConfigurationType XcpDaq_PredefinedLists[];
extern const XcpDaq_ListIntegerType XcpDaq_PredefinedListCount;
extern XcpDaq_ListStateType XcpDaq_PredefinedListsState[];
#endif /* XCP_DAQ_ENABLE_PREDEFINED_LISTS */

extern const XcpDaq_EventType XcpDaq_Events[];

/*
** Debugging / Testing interface.
*/
#if XCP_BUILD_TYPE == XCP_DEBUG_BUILD
void XcpDaq_GetCounts(XCP_DAQ_ENTITY_TYPE * entityCount, XCP_DAQ_ENTITY_TYPE * listCount, XCP_DAQ_ENTITY_TYPE * odtCount);
uint16_t XcpDaq_TotalDynamicEntityCount(void);
XcpDaq_EntityType *  XcpDaq_GetDynamicEntities(void);
XcpDaq_EntityType * XcpDaq_GetDynamicEntity(uint16_t num);

uint8_t * XcpDaq_GetDtoBuffer(void);
#endif // XCP_BUILD_TYPE

#endif /* XCP_ENABLE_DAQ_COMMANDS */


/*
** PGM Functions.
*/
#if XCP_ENABLE_PGM_COMMANDS == XCP_ON
void XcpPgm_SetProcessorState(XcpPgm_ProcessorStateType state);
#endif /* ENABLE_PGM_COMMANDS */

/*
 * Checksum Methods.
 */
#define XCP_CHECKSUM_METHOD_XCP_ADD_11      (1)
#define XCP_CHECKSUM_METHOD_XCP_ADD_12      (2)
#define XCP_CHECKSUM_METHOD_XCP_ADD_14      (3)
#define XCP_CHECKSUM_METHOD_XCP_ADD_22      (4)
#define XCP_CHECKSUM_METHOD_XCP_ADD_24      (5)
#define XCP_CHECKSUM_METHOD_XCP_ADD_44      (6)
#define XCP_CHECKSUM_METHOD_XCP_CRC_16      (7)
#define XCP_CHECKSUM_METHOD_XCP_CRC_16_CITT (8)
#define XCP_CHECKSUM_METHOD_XCP_CRC_32      (9)
#define XCP_CHECKSUM_METHOD_XCP_USER_DEFINED    (0xff)

#define XCP_DAQ_TIMESTAMP_UNIT_1NS          (0)
#define XCP_DAQ_TIMESTAMP_UNIT_10NS         (1)
#define XCP_DAQ_TIMESTAMP_UNIT_100NS        (2)
#define XCP_DAQ_TIMESTAMP_UNIT_1US          (3)
#define XCP_DAQ_TIMESTAMP_UNIT_10US         (4)
#define XCP_DAQ_TIMESTAMP_UNIT_100US        (5)
#define XCP_DAQ_TIMESTAMP_UNIT_1MS          (6)
#define XCP_DAQ_TIMESTAMP_UNIT_10MS         (7)
#define XCP_DAQ_TIMESTAMP_UNIT_100MS        (8)
#define XCP_DAQ_TIMESTAMP_UNIT_1S           (9)
#define XCP_DAQ_TIMESTAMP_UNIT_1PS          (10)
#define XCP_DAQ_TIMESTAMP_UNIT_10PS         (11)
#define XCP_DAQ_TIMESTAMP_UNIT_100PS        (12)

#define XCP_DAQ_TIMESTAMP_SIZE_1            (1)
#define XCP_DAQ_TIMESTAMP_SIZE_2            (2)
#define XCP_DAQ_TIMESTAMP_SIZE_4            (4)

/*
**
*/
void Xcp_SendPdu(void);
uint8_t * Xcp_GetOutPduPtr(void);
void Xcp_SetPduOutLen(uint16_t len);
void Xcp_Send8(uint8_t len, uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t b5, uint8_t b6, uint8_t b7);

/*
**  Helpers.
*/
void Xcp_DisplayInfo(void);

void Xcp_CopyMemory(Xcp_MtaType dst, Xcp_MtaType src, uint32_t len);

uint8_t Xcp_GetByte(Xcp_PDUType const * const value, uint8_t offs);
uint16_t Xcp_GetWord(Xcp_PDUType const * const value, uint8_t offs);
uint32_t Xcp_GetDWord(Xcp_PDUType const * const value, uint8_t offs);

void Xcp_SetByte(Xcp_PDUType const * const pdu, uint8_t offs, uint8_t value);
void Xcp_SetWord(Xcp_PDUType const * const pdu, uint8_t offs, uint16_t value);
void Xcp_SetDWord(Xcp_PDUType const * const pdu, uint8_t offs, uint32_t value);

/*
**  Transport Layer Stuff.
*/
#define XCP_COMM_BUFLEN  ((XCP_MAX(XCP_MAX_CTO, XCP_MAX_DTO)) + XCP_TRANSPORT_LAYER_LENGTH_SIZE + XCP_TRANSPORT_LAYER_COUNTER_SIZE + XCP_TRANSPORT_LAYER_CHECKSUM_SIZE)

void XcpTl_Init(void);
void XcpTl_DeInit(void);
int16_t XcpTl_FrameAvailable(uint32_t sec, uint32_t usec);
void XcpTl_RxHandler(void);
void XcpTl_Send(uint8_t const * buf, uint16_t len);
void XcpTl_MainFunction(void);
void XcpTl_SaveConnection(void);
void XcpTl_ReleaseConnection(void);
bool XcpTl_VerifyConnection(void);
void XcpTl_FeedReceiver(uint8_t octet);
void XcpTl_TransportLayerCmd_Res(Xcp_PDUType const * const pdu);
void XcpTl_PrintConnectionInformation(void);

/*
**  Customization Stuff.
*/
bool Xcp_HookFunction_GetId(uint8_t id_type, char ** result, uint32_t * result_length);
bool Xcp_HookFunction_GetSeed(uint8_t resource, Xcp_1DArrayType * result);
bool Xcp_HookFunction_Unlock(uint8_t resource, Xcp_1DArrayType const * key);

bool Xcp_HookFunction_CheckMemoryAccess(Xcp_MtaType mta, uint32_t length, Xcp_MemoryAccessType access, bool programming);
Xcp_MemoryMappingResultType Xcp_HookFunction_AddressMapper(Xcp_MtaType * dst, Xcp_MtaType const * src);

/*
**  Hardware dependent stuff.
*/
void XcpHw_Init(void);
void XcpHw_Deinit(void);
uint32_t XcpHw_GetTimerCounter(void);
void XcpHw_AcquireLock(uint8_t lockIdx);
void XcpHw_ReleaseLock(uint8_t lockIdx);

extern Xcp_PDUType Xcp_PduIn;
extern Xcp_PDUType Xcp_PduOut;


#if (XCP_CHECKSUM_METHOD == XCP_CHECKSUM_METHOD_XCP_CRC_16) || (XCP_CHECKSUM_METHOD == XCP_CHECKSUM_METHOD_XCP_CRC_16_CITT)
typedef uint16_t Xcp_ChecksumType;
#elif XCP_CHECKSUM_METHOD == XCP_CHECKSUM_METHOD_XCP_CRC_32
typedef uint32_t Xcp_ChecksumType;
#elif XCP_CHECKSUM_METHOD == XCP_CHECKSUM_METHOD_XCP_ADD_11
typedef uint8_t Xcp_ChecksumType;
#elif (XCP_CHECKSUM_METHOD == XCP_CHECKSUM_METHOD_XCP_ADD_12) || (XCP_CHECKSUM_METHOD == XCP_CHECKSUM_METHOD_XCP_ADD_22)
typedef uint16_t Xcp_ChecksumType;
#elif (XCP_CHECKSUM_METHOD == XCP_CHECKSUM_METHOD_XCP_ADD_14) || (XCP_CHECKSUM_METHOD == XCP_CHECKSUM_METHOD_XCP_ADD_24) || \
      (XCP_CHECKSUM_METHOD == XCP_CHECKSUM_METHOD_XCP_ADD_44)
typedef uint32_t Xcp_ChecksumType;
#endif /* XCP_CHECKSUM_METHOD */

void Xcp_ChecksumInit(void);
Xcp_ChecksumType Xcp_CalculateChecksum(uint8_t const * ptr, uint32_t length, Xcp_ChecksumType startValue, bool isFirstCall);
void Xcp_ChecksumMainFunction(void);
void Xcp_SendChecksumPositiveResponse(Xcp_ChecksumType checksum);
void Xcp_SendChecksumOutOfRangeResponse(void);
void Xcp_StartChecksumCalculation(uint8_t const * ptr, uint32_t size);


#if XCP_ENABLE_EXTERN_C_GUARDS == XCP_ON
#if defined(__cplusplus)
}
#endif  /* __cplusplus */
#endif /* XCP_EXTERN_C_GUARDS */

#endif /* __CXCP_H */
