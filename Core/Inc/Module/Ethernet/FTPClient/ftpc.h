#ifndef _FTPC_H_
#define _FTPC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <stdarg.h>
#include "stdio_private.h"
#include "socket.h"

#define F_APP_FTPC

/* If your target support a file system, you have to activate this feature and implement. */
#define F_FILESYSTEM

    /* Change to your Chipset Uart function, you have to activate this feature and implement.
     * Change!! -> Board_UARTGetCharBlocking()
     * Below is an example of a function of lpc_chip library. */
    // #define ftp_getc()	Board_UARTGetCharBlocking()

#ifdef F_FILESYSTEM
#include "ff.h"
#endif

    //	#ifndef	ftp_getc()
    //	#define Need_UARTGetCharBlocking_func
    //	#else
    //	/* Change library
    //	 * Change!! -> board_api.h,
    //	 * Below is an example of a function of lpc_chip library. */
    //	#include "board_api.h"
    //	#endif

#define LINELEN 100
#ifndef F_FILESYSTEM
#define _MAX_SS 2048
#endif

#define CTRL_SOCK 2
#define DATA_SOCK 3

/* FTP Responses */
#define R_150 150 /* File status ok; opening data conn */
#define R_200 200 /* 'Generic' command ok */
#define R_220 220 /* Service ready for new user. */
#define R_226 226 /* Closing data connection.  File transfer/abort successful */
#define R_227 227 /* Entering passive mode (h1,h2,h3,h4,p1,p2) */
#define R_230 230 /* User logged in, proceed */
#define R_331 331 /* User name okay, need password. */

#define TransferAscii 'A'
#define TransferBinary 'I'

enum ftpc_type {
    FTPC_ASCII_TYPE,
    FTPC_IMAGE_TYPE,
};

enum ftpc_datasock_state {
    FTPC_DATASOCK_IDLE,
    FTPC_DATASOCK_READY,
    FTPC_DATASOCK_START
};

enum ftpc_datasock_mode {
    FTPC_PASSIVE_MODE,
    FTPC_ACTIVE_MODE
};

enum CommandFirst {
    f_nocmd,
    f_dir,
    f_put,
    f_get,
};

enum CommandSecond {
    s_nocmd,
    s_dir,
    s_put,
    s_get,
};

typedef enum _COMMAND {
    CMD_DIR = 0,
    CMD_GET,
    CMD_PUT,
} COMMAND;

struct Command {
    enum CommandFirst First;
    enum CommandSecond Second;
};

struct ftpc {
    uint8_t control; /* Control stream */
    uint8_t data;    /* Data stream */

    enum ftpc_type type; /* Transfer type */

    enum ftpc_datasock_state dsock_state;
    enum ftpc_datasock_mode dsock_mode;

    char workingdir[LINELEN];
    char filename[LINELEN];

#ifdef F_FILESYSTEM
    FIL fil;    // FatFs File objects
    FRESULT fr; // FatFs function common result code
#endif
};

#ifndef un_I2cval
typedef union _ftpc_un_l2cval {
    uint32_t lVal;
    uint8_t cVal[4];
} ftpc_un_l2cval;
#endif

void ftpc_init(uint8_t *);
uint8_t ftpc_proc(uint8_t *, uint8_t *, uint8_t *, uint8_t *);
uint8_t ftpc_run(uint8_t *);
uint8_t ftpc_status(void);
void ftpc_service(COMMAND);
void ftpc_setFileName(uint8_t *, uint8_t *);
char proc_ftpc(char *);
int pportc(char *);
uint8_t *User_Keyboard_MSG(void);

#ifdef __cplusplus
}
#endif
#endif // _FTPC_H_
