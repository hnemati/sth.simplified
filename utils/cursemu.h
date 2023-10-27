/***************************************************************************
 *                                                                          
 *  DO NOT CHANGE ANYTHING BETWEEN THIS LINE AND THE NEXT LINE THAT HAS THE 
 *  WORDS "KLAATU BARRATA NIKTO" ON IT                                      
 *                                                                          
 ***************************************************************************/
#ifndef X__CURSEMU__H                                                        
#define X__CURSEMU__H                                                        

#include <stdio.h>

#ifdef  linux
#define _POSIX_VERSION
#endif                

#ifndef _POSIX_VERSION
#include <sgtty.h>    
#define  USE_OLD_TTY  
#include <sys/ioctl.h>
#undef  USE_OLD_TTY   

#ifndef  CBREAK
#define CBREAK RAW
#endif            

#if !defined(sun) && !defined(sequent) && !defined(hpux) && \
    !defined(_AIX) && !defined(aix)                          
#include <strings.h>                                         
#define strchr index                                         
#else                                                        
#include <string.h>                                          
#endif                                                       
#else                                                        
#include <string.h>                                          
#include <termios.h>                                         
#endif                                                       

#include <errno.h>
#include <sys/types.h>
#include <pwd.h>      
#include <sys/time.h> 
#include <sys/file.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>     
#include <signal.h>    

/* Keep looking ... */

int _tty_ch;

#ifdef  _POSIX_VERSION
struct termios _tty;  
tcflag_t _res_iflg,   
    _res_lflg;        

#define cbreak()(_tty.c_lflag&=~ICANON, \
  tcsetattr( _tty_ch, TCSANOW, &_tty ))  

#define noecho()(_tty.c_lflag &= ~(ECHO|ICRNL), \
  tcsetattr( _tty_ch, TCSADRAIN, &_tty ))        

#define savetty()((void) tcgetattr(_tty_ch, &_tty), \
  _res_iflg = _tty.c_iflag, _res_lflg = _tty.c_lflag )

#define resetty()(_tty.c_iflag = _res_iflg, _tty.c_lflag = _res_lflg,\
  (void) tcsetattr(_tty_ch, TCSADRAIN, &_tty))                        

#define erasechar()(_tty.c_cc[VERASE])
#else                                 
struct sgttyb _tty;                   
int _res_flg;                         

#define cbreak()(_tty.sg_flags|=CBREAK, ioctl(_tty_ch, TIOCSETP, &_tty))

#define noecho()(_tty.sg_flags &= ~(ECHO|CRMOD), \
  ioctl( _tty_ch, TIOCSETP, &_tty ))              

#define savetty()((void) ioctl(_tty_ch, TIOCGETP, &_tty), \
  _res_flg = _tty.sg_flags )                               

#define resetty()(_tty.sg_flags = _res_flg, \
  (void) ioctl(_tty_ch, TIOCSETP, &_tty))    
#define erasechar()(_tty.sg_erase)           
#endif                                       

/* KLAATU BARRATA NIKTO */

#define  TERMCAP_LENGTH 1024

struct CtrlSeq
{             
  char termcap[ TERMCAP_LENGTH ];

  int numRows, numCols;

  /*  These pointers are indexes into the termcap buffer, and represent the
   *  control sequences neccessary to send to the terminal window to perform
   *  their appropriately named feature.
   */
  char *highlight,
       *endMode,            /* End highlight mode, and other modes. */
       *clearScr,
       *clearEol,
       *scrollRegion,
       *moveCursor,
       *deleteRow,
       *insertRow,
       *saveCursor,         /* Save the current cursor position */
       *restoreCursor;      /* Restore the saved cursor position */

  int dumbTerm,             /* 1 if the terminal is a dumb terminal */
      flush;                /* 1 if the emulation should flush stdout */
};

struct CtrlSeq ctrlSeq;

#define DEFAULT_COLS    80
#define DEFAULT_ROWS    24

void ce_flush( int toSet );
void ce_puts( char *str );
void ce_gotoRowCol( int row, int col );

void ce_writeStrRowCol( char *theText, int row, int col );
void ce_writeStr( char *theText );
void ce_writeCharRowCol( char theChar, int row, int col );
void ce_writeChar( char theChar );

void ce_clearScreen( void );
void ce_clearEol( void );

void ce_highlight( int on );
void ce_scrollRegion( int row1, int row2 );
void ce_deleteRow( int row );
void ce_insertRow( int row );
void ce_saveCursor( void );
void ce_restoreCursor( void );

int ce_getRows( void );
int ce_getCols( void );

#endif
