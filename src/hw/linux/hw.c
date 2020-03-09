/*
 * BlueParrot XCP
 *
 * (C) 2007-2020 by Christoph Schueler <github.com/Christoph2,
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


#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "xcp.h"
#include "xcp_hw.h"

/*
**  Local Types.
*/

typedef struct tagHwStateType {
    struct timespec StartingTime;
} HwStateType;

 /*
 ** Local Defines.
 */
#define TIMER_PS_1NS    (1ULL)
#define TIMER_PS_10NS   (10ULL)
#define TIMER_PS_100NS  (100ULL)
#define TIMER_PS_1US    (1000ULL)
#define TIMER_PS_10US   (10000ULL)
#define TIMER_PS_100US  (100000ULL)
#define TIMER_PS_1MS    (1000000ULL)
#define TIMER_PS_10MS   (10000000ULL)
#define TIMER_PS_100MS  (100000000ULL)
#define TIMER_PS_1S     (1000000000ULL)

#define TIMER_MASK_1    (0x000000FFULL)
#define TIMER_MASK_2    (0x0000FFFFULL)
#define TIMER_MASK_4    (0xFFFFFFFFULL)

#define SIG SIGRTMIN

/*
**  Local Function Prototypes.
*/
static void XcpHw_InitLocks(void);
static void XcpHw_DeinitLocks();
static struct timespec Timespec_Diff(struct timespec start, struct timespec end);

/*
**  External Function Prototypes.
*/
void XcpTl_PrintConnectionInformation(void);

static void DisplayHelp(void);
static void SystemInformation(void);

void FlsEmu_Info(void);
void XcpDaq_Info(void);
void XcpDaq_PrintDAQDetails();

bool XcpHw_MainFunction();

void exitFunc(void);


/*
** Global Variables.
*/
pthread_t XcpHw_ThreadID[4];


/*
 * Local Types.
 */
typedef struct tagXcpHw_ApplicationStateType {
    pthread_mutex_t stateMutex;
    pthread_cond_t stateCond;
    uint32_t stateBitmap;
} XcpHw_ApplicationStateType;


/*
**  Local Variables.
*/
static HwStateType HwState = {0};
static pthread_mutex_t XcpHw_Locks[XCP_HW_LOCK_COUNT] = {PTHREAD_MUTEX_INITIALIZER};


static struct timespec XcpHw_TimerResolution = {0};
static timer_t XcpHw_AppMsTimer;
static unsigned long long XcpHw_FreeRunningCounter = 0ULL;


static XcpHw_ApplicationStateType XcpHw_ApplicationState = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, 0};

/*
**  Global Functions.
*/

static void handler(int sig, siginfo_t *si, void *uc)
{
    /* Note: calling printf() from a signal handler is not safe
     * (and should not be done in production programs), since
     * printf() is not async-signal-safe; see signal-safety(7).
     *  Nevertheless, we use printf() here as a simple way of
     * showing that the handler was called. */

    //printf("Caught signal %d %d\n", sig, XcpHw_GetTimerCounter());

    //print_siginfo(si);
    //signal(sig, SIG_IGN);
    XcpHw_SignalApplicationState(2);
}

void XcpHw_Init(void)
{
    int status;
    struct sigevent sev;

    timer_t timerid;
    struct itimerspec its;
    long long freq_nanosecs = 1000 * 1000 * 1000LL;
    sigset_t mask;
    struct sigaction sa;

    XcpHw_FreeRunningCounter = 0ULL;

    if (clock_getres(CLOCK_MONOTONIC_RAW, &XcpHw_TimerResolution) == -1) {
        XcpHw_ErrorMsg("XcpHw_Init::clock_getres()", errno);
    }

    if (clock_gettime(CLOCK_MONOTONIC_RAW, &HwState.StartingTime) == -1) {
        XcpHw_ErrorMsg("XcpHw_Init::clock_getres()", errno);
    }
    fflush(stdout);
#if 0
    _setmode(_fileno(stdout), _O_WTEXT);    /* Permit Unicode output on console */
    _setmode(_fileno(stdout), _O_U8TEXT);    /* Permit Unicode output on console */
#endif

    /* Establish handler for timer signal */
    printf("Establishing handler for signal %d\n", SIG);
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIG, &sa, NULL) == -1) {
        XcpHw_ErrorMsg("XcpHw_Init::sigaction()", errno);
    }

    /* Block timer signal temporarily */
    printf("Blocking signal %d\n", SIG);
    sigemptyset(&mask);
    sigaddset(&mask, SIG);
    if (sigprocmask(SIG_SETMASK, &mask, NULL) == -1) {
        XcpHw_ErrorMsg("XcpHw_Init::sigprocmask()", errno);
    }

    /* Create the timer */
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIG;
    sev.sigev_value.sival_ptr = &XcpHw_AppMsTimer;
    // Parameter sevp could be NULL!?
    if (timer_create(CLOCK_MONOTONIC, &sev, &XcpHw_AppMsTimer) == -1) {
        XcpHw_ErrorMsg("XcpHw_Init::timer_create()", errno);
    }

    /* Start the timer */
    its.it_value.tv_sec = freq_nanosecs / 1000000000;
    its.it_value.tv_nsec = freq_nanosecs % 1000000000;
    its.it_interval.tv_sec = its.it_value.tv_sec;
    its.it_interval.tv_nsec = its.it_value.tv_nsec;
    if (timer_settime(XcpHw_AppMsTimer, 0, &its, NULL) == -1) {
        XcpHw_ErrorMsg("XcpHw_Init::timer_settime()", errno);
    }

    if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1) {
        XcpHw_ErrorMsg("XcpHw_Init::sigprocmask()", errno);
    }

}

void XcpHw_Deinit(void)
{
    XcpHw_DeinitLocks();
}

void XcpHw_SignalApplicationState(uint32_t state)
{
    int status;

    status = pthread_mutex_lock(&XcpHw_ApplicationState.stateMutex);
    if (status != 0) {

    }
    XcpHw_ApplicationState.stateBitmap = state;
    status = pthread_cond_signal(&XcpHw_ApplicationState.stateCond);
    if (status != 0) {

    }
    status = pthread_mutex_unlock(&XcpHw_ApplicationState.stateMutex);
    if (status != 0) {

    }
}

void XcpHw_ResetApplicationState(uint32_t mask)
{
    int status;

    status = pthread_mutex_lock(&XcpHw_ApplicationState.stateMutex);
    if (status != 0) {

    }
    XcpHw_ApplicationState.stateBitmap &= ~mask;
    status = pthread_mutex_unlock(&XcpHw_ApplicationState.stateMutex);
    if (status != 0) {

    }
}

uint32_t XcpHw_WaitApplicationState(uint32_t mask)
{
    uint32_t result = 0UL;
    int status;
    struct timespec timeout;
    int match = 0;

    timeout.tv_sec = time(NULL) + 2;
    timeout.tv_nsec = 0;
    status = pthread_mutex_lock(&XcpHw_ApplicationState.stateMutex);
    if (status != 0) {

    }

    while ((XcpHw_ApplicationState.stateBitmap == 0) && (!match)) {
        //status = pthread_cond_timedwait(&XcpHw_ApplicationState.stateCond, &XcpHw_ApplicationState.stateMutex, &timeout);
        status = pthread_cond_wait(&XcpHw_ApplicationState.stateCond, &XcpHw_ApplicationState.stateMutex);
        if (status != 0) {
            printf("ERROR! %u[%s]\n", status, strerror(status));
        }
        match = (XcpHw_ApplicationState.stateBitmap & mask) != 0x00;
        result = XcpHw_ApplicationState.stateBitmap;
        printf("match: %u result: %u\n", match, result);
//        XcpHw_ApplicationState.stateBitmap = 0UL;
    }

    status = pthread_mutex_unlock(&XcpHw_ApplicationState.stateMutex);
    if (status != 0) {

    }
    return result;
}

static struct timespec Timespec_Diff(struct timespec start, struct timespec end)
{
    struct timespec temp;

    if ((end.tv_nsec-start.tv_nsec) < 0) {
        temp.tv_sec = end.tv_sec-start.tv_sec - 1;
        temp.tv_nsec = 1000000000L + end.tv_nsec - start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return temp;
}

uint32_t XcpHw_GetTimerCounter(void)
{
    struct timespec now = {0};
    struct timespec dt = {0};
    unsigned long long timestamp;

    if (clock_gettime(CLOCK_MONOTONIC_RAW, &now) == -1) {
        XcpHw_ErrorMsg("XcpHw_Init::clock_getres()", errno);
    }

    dt = Timespec_Diff(HwState.StartingTime, now);

    XcpHw_FreeRunningCounter = ((unsigned long long)(dt.tv_sec) * (unsigned long long)1000 * 1000 * 1000) + ((unsigned long long)dt.tv_nsec);

    printf("\tTS: %llu\n", XcpHw_FreeRunningCounter);
    timestamp = XcpHw_FreeRunningCounter;
#if XCP_DAQ_TIMESTAMP_UNIT == XCP_DAQ_TIMESTAMP_UNIT_1NS
    timestamp /= TIMER_PS_1NS;
#elif XCP_DAQ_TIMESTAMP_UNIT == XCP_DAQ_TIMESTAMP_UNIT_10NS
    timestamp /= TIMER_PS_10NS;
#elif XCP_DAQ_TIMESTAMP_UNIT == XCP_DAQ_TIMESTAMP_UNIT_100NS
    timestamp /= TIMER_PS_100NS;
#elif XCP_DAQ_TIMESTAMP_UNIT == XCP_DAQ_TIMESTAMP_UNIT_1US
    timestamp /= TIMER_PS_1US;
#elif XCP_DAQ_TIMESTAMP_UNIT == XCP_DAQ_TIMESTAMP_UNIT_10US
    timestamp /= TIMER_PS_10US;
#elif XCP_DAQ_TIMESTAMP_UNIT == XCP_DAQ_TIMESTAMP_UNIT_100US
    timestamp /= TIMER_PS_100US;
#elif XCP_DAQ_TIMESTAMP_UNIT == XCP_DAQ_TIMESTAMP_UNIT_1MS
    timestamp /= TIMER_PS_1MS;
#elif XCP_DAQ_TIMESTAMP_UNIT == XCP_DAQ_TIMESTAMP_UNIT_10MS
    timestamp /= TIMER_PS_10MS;
//#elif XCP_DAQ_TIMESTAMP_UNIT == XCP_DAQ_TIMESTAMP_UNIT_100MS
//    timestamp /= TIMER_PS_100MS;
#else
#error Timestamp-unit not supported.
#endif // XCP_DAQ_TIMESTAMP_UNIT

#if XCP_DAQ_TIMESTAMP_SIZE == XCP_DAQ_TIMESTAMP_SIZE_1
    timestamp &= TIMER_MASK_1;
#elif XCP_DAQ_TIMESTAMP_SIZE == XCP_DAQ_TIMESTAMP_SIZE_2
    timestamp &= TIMER_MASK_2;
#elif XCP_DAQ_TIMESTAMP_SIZE == XCP_DAQ_TIMESTAMP_SIZE_4
    timestamp &= TIMER_MASK_4;
#else
#error Timestamp-size not supported.
#endif // XCP_DAQ_TIMESTAMP_SIZE

    return (uint32_t)timestamp;
}

static void XcpHw_InitLocks(void)
{
    uint8_t idx = UINT8(0);

    for (idx = UINT8(0); idx < XCP_HW_LOCK_COUNT; ++idx) {
        pthread_mutex_init(&XcpHw_Locks[idx], NULL);
    }
}

static void XcpHw_DeinitLocks(void)
{
    uint8_t idx = UINT8(0);

    for (idx = UINT8(0); idx < XCP_HW_LOCK_COUNT; ++idx) {
        pthread_mutex_destroy(&XcpHw_Locks[idx]);
    }
}

void XcpHw_AcquireLock(uint8_t lockIdx)
{
    if (lockIdx >= XCP_HW_LOCK_COUNT) {
        return;
    }
    pthread_mutex_lock(&XcpHw_Locks[lockIdx]);
}

void XcpHw_ReleaseLock(uint8_t lockIdx)
{
    if (lockIdx >= XCP_HW_LOCK_COUNT) {
        return;
    }
    pthread_mutex_unlock(&XcpHw_Locks[lockIdx]);
}

#if 0
DWORD XcpHw_UIThread()
{
    HANDLE * quit_event = (HANDLE *)param;

    XCP_FOREVER {
        if (!XcpHw_MainFunction(quit_event)) {
            break;
        }
        if (WaitForSingleObject(*quit_event, 0) == WAIT_OBJECT_0) {
            break;
        }
    }
    ExitThread(0);
    }
}
#endif

void XcpTl_PostQuitMessage();

bool XcpHw_MainFunction()
{
#if 0
    HANDLE hStdin;
    DWORD cNumRead, fdwMode, idx;
    INPUT_RECORD irInBuf[128];
    KEY_EVENT_RECORD key;

    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    if (hStdin == INVALID_HANDLE_VALUE) {
        Win_ErrorMsg("GetStdHandle", GetLastError());
    }

    fdwMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
    if (!SetConsoleMode(hStdin, fdwMode)) {
        Win_ErrorMsg("SetConsoleMode", GetLastError());
    }

    WaitForSingleObject(hStdin, 1000);

    if (!GetNumberOfConsoleInputEvents (hStdin, &cNumRead)) {
        Win_ErrorMsg("PeekConsoleInput", GetLastError());
    } else {
        if (cNumRead) {
            if (!ReadConsoleInput(hStdin, irInBuf, 128, &cNumRead)) {
                Win_ErrorMsg("ReadConsoleInput", GetLastError());
            }
            for (idx = 0; idx < cNumRead; ++idx) {
            switch(irInBuf[idx].EventType) {
                    case KEY_EVENT:
                        key = irInBuf[idx].Event.KeyEvent;
//                        printf("KeyEvent: %x %x %x %u\n", key.wVirtualKeyCode, key.wVirtualScanCode, key.dwControlKeyState, key.bKeyDown);
                        if (key.bKeyDown) {
                            if (key.wVirtualKeyCode == VK_ESCAPE) {
                                SetEvent(*quit_event);
                                XcpTl_PostQuitMessage();
                                return XCP_FALSE;
                            }
                            if (key.wVirtualKeyCode == VK_F9) {
//                                printf("\tF9\n");
                            }
                            switch (tolower(key.uChar.AsciiChar)) {
                                case 'q':
                                    SetEvent(*quit_event);
                                    XcpTl_PostQuitMessage();
                                    return XCP_FALSE;
                                case 'h':
                                    DisplayHelp();
                                    break;
                                case 'i':
                                    SystemInformation();
                                    break;
                                case 'd':
                                    XcpDaq_PrintDAQDetails();
                                    break;
                            }
                        }
                        break;
                    default:
                        //MyErrorExit("unknown event type");
                        break;
                }
            }
        }
    }
#endif
    return XCP_TRUE;
}


void XcpHw_ParseCommandLineOptions(int argc, char **argv, XcpHw_OptionsType * options)
{
    int idx;
    char * arg;

    options->ipv6 = XCP_FALSE;
    options->tcp = XCP_TRUE;

    if (argc >= 2) {
        for(idx = 1; idx < argc; ++idx) {
            arg = argv[idx];
            if ((arg[0] != '/') && (arg[0] != '-')) {
                continue;
            }
            switch (arg[1]) {
                case '4':
                    options->ipv6 = XCP_FALSE;
                    break;
                case '6':
                    options->ipv6 = XCP_TRUE;
                    break;
                case 'u':
                    options->tcp = XCP_FALSE;
                    break;
                case 't':
                    options->tcp = XCP_TRUE;
                    break;
                case 'h':
                    break;
                default:
                    break;
            }
        }
    }
}

static void DisplayHelp(void)
{
    printf("\nh\t\tshow this help message\n");
    //printf("<ESC> or q\texit %s\n", __argv[0]);
    printf("<ESC> or q\texit \n");
    printf("i\t\tsystem information\n");
    printf("d\t\tDAQ configuration\n");
    /* printf("d\t\tReset connection\n"); */
}

static void SystemInformation(void)
{
#if XCP_ENABLE_STATISTICS == XCP_ON
    Xcp_StateType * state;
#endif /* XCP_ENABLE_STATISTICS */

    printf("\nSystem-Information\n");
    printf("------------------\n");
    XcpTl_PrintConnectionInformation();
    printf("MAX_CTO         : %d    MAX_DTO: %d\n", XCP_MAX_CTO, XCP_MAX_DTO);
    printf("Slave-Blockmode : %s\n", (XCP_ENABLE_SLAVE_BLOCKMODE == XCP_ON) ? "Yes" : "No");
    printf("Master-Blockmode: %s\n", (XCP_ENABLE_MASTER_BLOCKMODE == XCP_ON) ? "Yes" : "No");

#if XCP_ENABLE_CAL_COMMANDS == XCP_ON
    printf("Calibration     : Yes   Protected: %s\n", (XCP_PROTECT_CAL == XCP_ON)  ? "Yes" : "No");
#else
    printf("Calibration     : No\n");
#endif /* XCP_ENABLE_CAL_COMMANDS */

#if XCP_ENABLE_PAG_COMMANDS == XCP_ON
    printf("Paging          : Yes   Protected: %s\n", (XCP_PROTECT_PAG == XCP_ON)  ? "Yes" : "No");
#else
    printf("Paging          : No\n");
#endif /* XCP_ENABLE_PAG_COMMANDS */

#if XCP_ENABLE_DAQ_COMMANDS == XCP_ON
    printf("DAQ             : Yes   Protected: [DAQ: %s STIM: %s]\n", (XCP_PROTECT_DAQ == XCP_ON)  ?
           "Yes" : "No", (XCP_PROTECT_STIM == XCP_ON)  ? "Yes" : "No"
    );
#else
    printf("DAQ             : No\n");
#endif /* XCP_ENABLE_DAQ_COMMANDS */

#if XCP_ENABLE_PGM_COMMANDS
    printf("Programming     : Yes   Protected: %s\n", (XCP_PROTECT_PGM == XCP_ON)  ? "Yes" : "No");
#else
    printf("Programming     : No\n");
#endif /* XCP_ENABLE_PGM_COMMANDS */
    printf("\n");
    XcpDaq_Info();
    FlsEmu_Info();

#if XCP_ENABLE_STATISTICS == XCP_ON
    state = Xcp_GetState();
    printf("\nStatistics\n");
    printf("----------\n");
    printf("CTOs rec'd      : %d\n", state->statistics.ctosReceived);
    printf("CROs busy       : %d\n", state->statistics.crosBusy);
    printf("CROs send       : %d\n", state->statistics.crosSend);
#endif /* XCP_ENABLE_STATISTICS */
    printf("-------------------------------------------------------------------------------\n");
}

void XcpHw_ErrorMsg(char * const function, int errorCode)
{
    fprintf(stderr, "[%s] failed with: [%d] %s", function, errorCode, strerror(errorCode));
}

void Xcp_DisplayInfo(void)
{
    XcpTl_PrintConnectionInformation();
    printf("Press h for help.\n");
    fflush(stdout);
}
