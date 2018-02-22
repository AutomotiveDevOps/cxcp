/*
 * pySART - Simplified AUTOSAR-Toolkit for Python.
 *
 * (C) 2007-2018 by Christoph Schueler <github.com/Christoph2,
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

#include <stdlib.h>
#include <stdint.h>

#include "xcp_tl.h"

#define XCP_PROTOCOL_VERSION_MAJOR       (1)
#define XCP_PROTOCOL_VERSION_RELEASE     (0)

#define XCP_DEBUG_BUILD         (1)
#define XCP_RELEASE_BUILD       (2)

#define XCP_ON                  (1)
#define XCP_OFF                 (0)


/*
**  Available Resources.
*/
#define XCP_RESOURCE_PGM        (16)
#define XCP_RESOURCE_STIM       (8)
#define XCP_RESOURCE_DAQ        (4)
#define XCP_RESOURCE_CAL_PAG    (1)


#define XCP_BYTE_ORDER_INTEL    (0)
#define XCP_BYTE_ORDER_MOTOROLA (1)

#define XCP_SLAVE_BLOCK_MODE    (0x40)

#define XCP_ADDRESS_GRANULARITY_0   (2)
#define XCP_ADDRESS_GRANULARITY_1   (4)

#define XCP_ADDRESS_GRANULARITY_BYTE    (0)
#define XCP_ADDRESS_GRANULARITY_WORD    (XCP_ADDRESS_GRANULARITY_0)
#define XCP_ADDRESS_GRANULARITY_DWORD   (XCP_ADDRESS_GRANULARITY_1)

#define XCP_OPTIONAL_COMM_MODE          (0x80)

#define XCP_MASTER_BLOCK_MODE           (1)
#define XCP_INTERLEAVED_MODE            (2)

#if defined(_MSC_VER)
#define INLINE __inline
#else
#define INLINE inline
#endif // defined(_MSC_VER)



typedef void (*Xcp_ServerCommandType)(Xcp_PDUType const * const pdu);

/*
** Global Types.
*/

typedef enum tagXcp_CommandType {
//
// STD
//
    //
    // Mandantory Commnands.
    //
    CONNECT                 = 0xFF,
    DISCONNECT              = 0xFE,
    GET_STATUS              = 0xFD,
    SYNCH                   = 0xFC,
    //
    // Optional Commands.
    //
    GET_COMM_MODE_INFO      = 0xFB,
    GET_ID                  = 0xFA,
    SET_REQUEST             = 0xF9,
    GET_SEED                = 0xF8,
    UNLOCK                  = 0xF7,
    SET_MTA                 = 0xF6,
    UPLOAD                  = 0xF5,
    SHORT_UPLOAD            = 0xF4,
    BUILD_CHECKSUM          = 0xF3,

    TRANSPORT_LAYER_CMD     = 0xF2,
    USER_CMD                = 0xF1,
//
// CAL
//
    //
    // Mandantory Commnands.
    //
    DOWNLOAD                = 0xF0,
    //
    // Optional Commands.
    //
    DOWNLOAD_NEXT           = 0xEF,
    DOWNLOAD_MAX            = 0xEE,
    SHORT_DOWNLOAD          = 0xED,
    MODIFY_BITS             = 0xEC,
//
// PAG
//
    //
    // Mandantory Commnands.
    //
    SET_CAL_PAGE            = 0xEB,
    GET_CAL_PAGE            = 0xEA,
    //
    // Optional Commands.
    //
    GET_PAG_PROCESSOR_INFO  = 0xE9,
    GET_SEGMENT_INFO        = 0xE8,
    GET_PAGE_INFO           = 0xE7,
    SET_SEGMENT_MODE        = 0xE6,
    GET_SEGMENT_MODE        = 0xE5,
    COPY_CAL_PAGE           = 0xE4,
//
// DAQ
//
    //
    // Mandantory Commnands.
    //
    CLEAR_DAQ_LIST          = 0xE3,
    SET_DAQ_PTR             = 0xE2,
    WRITE_DAQ               = 0xE1,
    SET_DAQ_LIST_MODE       = 0xE0,
    GET_DAQ_LIST_MODE       = 0xDF,
    START_STOP_DAQ_LIST     = 0xDE,
    START_STOP_SYNCH        = 0xDD,
    //
    // Optional Commands.
    //
    GET_DAQ_CLOCK           = 0xDC,
    READ_DAQ                = 0xDB,
    GET_DAQ_PROCESSOR_INFO  = 0xDA,
    GET_DAQ_RESOLUTION_INFO = 0xD9,
    GET_DAQ_LIST_INFO       = 0xD8,
    GET_DAQ_EVENT_INFO      = 0xD7,
    FREE_DAQ                = 0xD6,
    ALLOC_DAQ               = 0xD5,
    ALLOC_ODT               = 0xD4,
    ALLOC_ODT_ENTRY         = 0xD3,
//
// PGM
//
    //
    // Mandantory Commnands.
    //
    PROGRAM_START           = 0xD2,
    PROGRAM_CLEAR           = 0xD1,
    PROGRAM                 = 0xD0,
    PROGRAM_RESET           = 0xCF,
    //
    // Optional Commands.
    //
    GET_PGM_PROCESSOR_INFO  = 0xCE,
    GET_SECTOR_INFO         = 0xCD,
    PROGRAM_PREPARE         = 0xCC,
    PROGRAM_FORMAT          = 0xCB,
    PROGRAM_NEXT            = 0xCA,
    PROGRAM_MAX             = 0xC9,
    PROGRAM_VERIFY          = 0xC8,

} Xcp_CommandType;


typedef enum tagXcp_ReturnType {
    ERR_CMD_SYNCH           = 0x00, // Command processor synchronization.                            S0
                                    //
    ERR_CMD_BUSY            = 0x10, // Command was not executed.                                     S2
    ERR_DAQ_ACTIVE          = 0x11, // Command rejected because DAQ is running.                      S2
    ERR_PGM_ACTIVE          = 0x12, // Command rejected because PGM is running.                      S2
                                    //
    ERR_CMD_UNKNOWN         = 0x20, // Unknown command or not implemented optional command.          S2
    ERR_CMD_SYNTAX          = 0x21, // Command syntax invalid                                        S2
    ERR_OUT_OF_RANGE        = 0x22, // Command syntax valid but command parameter(s) out of range.   S2
    ERR_WRITE_PROTECTED     = 0x23, // The memory location is write protected.                       S2
    ERR_ACCESS_DENIED       = 0x24, // The memory location is not accessible.                        S2
    ERR_ACCESS_LOCKED       = 0x25, // Access denied, Seed & Key is required                         S2
    ERR_PAGE_NOT_VALID      = 0x26, // Selected page not available                                   S2
    ERR_MODE_NOT_VALID      = 0x27, // Selected page mode not available                              S2
    ERR_SEGMENT_NOT_VALID   = 0x28, // Selected segment not valid                                    S2
    ERR_SEQUENCE            = 0x29, // Sequence error                                                S2
    ERR_DAQ_CONFIG          = 0x2A, // DAQ configuration not valid                                   S2
                                    //
    ERR_MEMORY_OVERFLOW     = 0x30, // Memory overflow error                                         S2
    ERR_GENERIC             = 0x31, // Generic error.                                                S2
    ERR_VERIFY              = 0x32, // The slave internal program verify routine detects an error.   S3
} Xcp_ReturnType;

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


typedef struct tagXcp_StationIDType {
    uint16_t len;
    const uint8_t name[];
} Xcp_StationIDType;

typedef struct tagXcp_ODTEntryType {
    uint32_t address;
    uint8_t addressExtension;   /* optional */
    uint32_t length;    /* optional */
} Xcp_ODTEntryType;

typedef struct tagXcp_ODTType {
    /* NOTE: ODT sampling needs to be consistent, ie. atomic. */
    Xcp_ODTEntryType element[7];

} Xcp_ODTType;


typedef struct tagXcp_MtaType {
    uint8_t ext;
    uint32_t address;
} Xcp_MtaType;

/*
typedef struct tagXcp_DAQListType {

} Xcp_DAQListype;
*/

typedef void(*Xcp_SendCalloutType)(Xcp_PDUType const * pdu);

/*
** Global Functions.
*/
void Xcp_Init(void);
void Xcp_DispatchCommand(Xcp_PDUType const * const pdu);
void Xcp_SendCmo(Xcp_PDUType const * pdu);


Xcp_ConnectionStateType Xcp_GetConnectionState(void);
void Xcp_SetSendCallout(Xcp_SendCalloutType callout);
void Xcp_DumpMessageObject(Xcp_PDUType const * pdu);

#if !defined(LOBYTE)
#define LOBYTE(w)   ((w) & 0xff)
#endif

#if !defined(HIBYTE)
#define HIBYTE(w)   (((w)  & 0xff00) >> 8)
#endif

//#if 0
#if !defined(TRUE)
#define TRUE    (1)
#endif

#if !defined(FALSE)
#define FALSE   (0)
#endif
//#endif


#define XCP_CHECKSUM_METHOD_XCP_ADD_11      (1)
#define XCP_CHECKSUM_METHOD_XCP_ADD_12      (2)
#define XCP_CHECKSUM_METHOD_XCP_ADD_14      (3)
#define XCP_CHECKSUM_METHOD_XCP_ADD_22      (4)
#define XCP_CHECKSUM_METHOD_XCP_ADD_24      (5)
#define XCP_CHECKSUM_METHOD_XCP_ADD_44      (6)
#define XCP_CHECKSUM_METHOD_XCP_CRC_16      (7)
#define XCP_CHECKSUM_METHOD_XCP_CRC_16_CITT (8)
#define XCP_CHECKSUM_METHOD_XCP_CRC_32      (9)

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


uint16_t Xcp_GetWord(Xcp_PDUType const * const value, uint8_t offs);
uint32_t Xcp_GetDWord(Xcp_PDUType const * const value, uint8_t offs);

/*
** Hardware dependent stuff.
*/


//#include "xcp_hw.h"
#include "xcp_config.h"

#endif /* __CXCP_H */
