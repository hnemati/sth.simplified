#include <stdio.h>
#include <ctype.h>

#include <string.h>
#include <stdlib.h>
#include <signal.h>


#include "icm/icmCpuManager.h"

#define MAX_CYCLES 0x00FFFFFF
#define MEMORY_DUMP_SIZE 32

extern void createPlatform(int debug, int verbose, icmProcessorP *cpu_, icmBusP *bus_) ;

extern void doLoadSymbols(const char *filename);

extern int getAddressFromSymbol(char *str, int *result);
extern char *getSymbolFromAddress(int adr);

/***********************************************************/
#define REGISTER_INDEX_CPSR 16
#define REGISTER_INDEX_SPSR 17
#define REGISTER_COUNT_NORMAL 16
#define REGISTER_COUNT_ALL (17 + 24)
#define REGISTERS_BANKED_START (REGISTER_INDEX_SPSR + 1)
#define REGISTERS_BANKED_END REGISTER_COUNT_ALL

static char *reg_names [] = {
    "R0", "R1", "R2", "R3", "R4","R5", "R6", "R7", "R8", "R9",
    "R10", "R11", "R12", "SP", "LR", "PC", "CPSR", "SPSR",
    
    "SP_USR", "LR_USR", "SPSR_USR",
    "SP_SVC", "LR_SVC", "SPSR_SVC",
    "SP_ABT", "LR_ABT", "SPSR_ABT",
    "SP_UNDEF", "LR_UNDEF", "SPSR_UNDEF",
    "SP_IRQ", "LR_IRQ", "SPSR_IRQ",        
    "SP_FIQ", "LR_FIQ", "SPSR_FIQ",
    "R8_FIQ", "R9_FIQ", "R10_FIQ", "R11_FIQ", "R12_FIQ",
};

static char *reg_names_lower [] = {
    "r0", "r1", "r2", "r3", "r4","r5", "r6", "r7", "r8", "r9",
    "r10", "r11", "r12", "sp", "lr", "pc", "cpsr", "spsr",
    
    "sp_usr", "lr_usr", "spsr_usr",
    "sp_svc", "lr_svc", "spsr_svc",
    "sp_abt", "lr_abt", "spsr_abt",
    "sp_undef", "lr_undef", "spsr_undef",
    "sp_irq", "lr_irq", "spsr_irq",        
    "sp_fiq", "lr_fiq", "spsr_fiq",
    "r8_fiq", "r9_fiq", "r10_fiq", "r11_fiq", "r12_fiq",
};

static Uns32 reg_values[REGISTER_COUNT_ALL] = { -1 };
static int reg_changed[REGISTER_COUNT_ALL] = {0 };
static int reg_valid[REGISTER_COUNT_ALL] = { 0 };

/***********************************************************/

int is_space(char c)
{
    return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
}

/* number parsing: since some ugly hacks are just to cute...
 * handle digits and hex startitg with 0x 
 */
int getNumberFromString(char *str, int *result)
{
    int n, ret, base, digits;
    if(str == 0) return 0;
    
    while( is_space( *str)) str++;
    if(*str == '\0') return 0; /* empty */
    
    if( str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        base = 16;        
        str += 2;
    } else {
        base = 10;
    }
    
    ret = 0;
    digits = 0;
    while( *str != '\0') {
        int c = *str++;
        if( is_space(c) && digits != 0) break; /* end of the number */
        
        if(c >= '0' && c <= '9') n = c - '0';
        else if( c >= 'A' && c <= 'F')n = c - 'A' + 10;
        else if( c >= 'a' && c <= 'f')n = c - 'a' + 10;
        else n = -1;
        
        if(n < 0 || n >= base) return 0; /* bad char */
        ret = ret * base + n;
        digits++;
    }
    
    if(digits == 0) return 0; /* nothing */
    
    *result = ret;
    return 1;
}

int getNumberFromRegister(icmProcessorP processor, char *str, int *result)
{    
    Uns32 reg;
    
    if(str != 0 && icmReadReg(processor, str, &reg)) {
        *result = reg;
        return 1;
    }
    return 0;
}

int getNumber(icmProcessorP processor, char *str, int *result)
{
    if(getNumberFromString(str, result)) return 1;
    if(getNumberFromRegister(processor, str, result)) return 1;    
    if(getAddressFromSymbol(str, result)) return 1;
    return 0;
}

/*
 * IO functions
 */

int readLine(char *buffer, int size)
{
    int count;
    fgets(buffer, size-1, stdin);
    
    
    for(count = 0; count < size-1; count++) {
        if(buffer[count] == '\0' || buffer[count] == '\r' || buffer[count] == '\n')
            break;
    }
    
    /* empty line ? */
    if(count == 0) return 0;
    
    buffer[count] = '\0';
    return 1;
}

char *trimString(char *buffer) 
{
    char *a;
    
    while(is_space(*buffer) && *buffer != '\0') buffer++;
    
    a = buffer;
    while(*a != '\0') a++;
    
    a--;
    while(is_space(*a) && a > buffer)
        *a-- = '\0';
    
    return *buffer == '\0' ? 0 : buffer;
}

char *getNextString(char *buffer) 
{
    if(buffer == 0) return 0;
    
    while(!is_space(*buffer) && *buffer != '\0') buffer++;
    if(*buffer == '\0') return 0;
    
    *buffer++ = '\0';
    while(is_space(*buffer)) buffer++;
    
    if(*buffer == '\0') return 0;
    return buffer;    
}

/***********************************************************
 * Simulator functions
 ***********************************************************/
#define BREAKPOINT_COUNT 64
struct breakpoint{
    int adr;
    struct breakpoint *next, *prev;
};

static struct breakpoint *bp_root = 0;

struct breakpoint *findBreakpoint(int adr)
{
    struct breakpoint *tmp = bp_root;
    while(tmp) {
        if(tmp->adr == adr) return tmp;
        tmp = tmp->next;
    }
    return 0;
}

void listAllBreakpoints()
{
    struct breakpoint *tmp = bp_root;
    int i = 1;
    
    if(tmp) {
        printf("Current breakoints are\n");
        
        while(tmp) {
            char *symbol = getSymbolFromAddress(tmp->adr);
            if(symbol)                
                printf("  %d  - 0x%08lx - %s\n", i, tmp->adr, symbol);
            else
                printf("  %d  - 0x%08lx\n", i, tmp->adr);
            i++;
            tmp = tmp->next;
        }
        printf("\n");
    } else {
        printf("No breakpoints were set\n");
    }
}

int isBreakpoint(int adr)
{
    return findBreakpoint(adr) != 0;
}

int removeBreakpoint(icmProcessorP processor, int adr)
{
    struct breakpoint *bp = findBreakpoint(adr);    
    if(!bp) {
        printf("INTERNAL ERROR: could not find breakpoint at 0x%08lx\n", adr);
        return 0;
    }
    
    icmClearAddressBreakpoint(processor, (Addr) ( 0xFFFFFFFF & adr));
    
    if(bp->next) bp->next->prev = bp->prev;
    if(bp->prev) bp->prev->next = bp->next;
    else bp_root = bp->next;
    
    free(bp);
    return 1;
}

int setBreakpoint(icmProcessorP processor, int adr)
{    
    struct breakpoint *bp = findBreakpoint(adr);    
    if(bp) {
        printf("Breakpoint at 0x%08lx already exists, ignoring...\n", adr);
        return 0;
    }
    
    bp =  malloc( sizeof(struct breakpoint));
    if(!bp) return 0;
    
    icmSetAddressBreakpoint(processor, (Addr) ( 0xFFFFFFFF & adr));            
    bp->adr = adr;
    bp->next = bp_root;
    bp->prev = 0;
    if(bp_root) bp_root->prev = bp;
    bp_root = bp;
    return 1;
}

/***********************************************************
 * Simulator functions
 ***********************************************************/


int run_one_cycle(icmProcessorP processor, int bypass_breakpoints)
{
    Uns32 pc = (Uns32)icmGetPC(processor); 
    int reason;
    
    icmPrintf("0x%08x : %s\n", pc, icmDisassemble(processor, pc) );
    
    reason = icmSimulate(processor, 1);    
    
    // try to run the instruction again, now without the breakpoint
    if(reason == ICM_SR_BP_ADDRESS && bypass_breakpoints) {
        int found_bp = removeBreakpoint(processor, pc);
        reason = icmSimulate(processor, 1);
        if(found_bp) setBreakpoint(processor, pc);            
    }
    
    return reason;
}

int isSimulationEnded(int reason)
{
    return (reason == ICM_SR_HALT || reason == ICM_SR_EXIT);
}


/*
   int reachedBreakpoint(icmProcessorP processor)
   {
   Uns32 pc = (Uns32)icmGetPC(processor);     
   char buffer[32];
   int tmp;
   
   for(;;) {
   printf("Stopped at 0x%08lx. Remove breakpoint now? [Yes/No/Bypass] ", pc);
   fflush(stdout);
   
   if(fgets(buffer, 32-1, stdin)) {
   switch(buffer[0]) {
   case 'y':
   case 'Y':
   removeBreakpoint(processor, pc);
   return ICM_SR_BP_ADDRESS;
   
   case 'n':
   case 'N':
   return ICM_SR_BP_ADDRESS;
   
   case 'b':
   case 'B':
   removeBreakpoint(processor, pc);
   tmp = doNextInstruction(processor, 0);
   setBreakpoint(processor, pc);
   return tmp;
   
   }
   }
   }    
   }
 */      

/* -- */
void doListProgram(icmProcessorP processor, char *arg1, char *arg2)
{
    int i, len;
    Uns32 pos, pc;
    
    pc = (Uns32)icmGetPC(processor);
    
    if(!arg1) 
        pos = pc - 8;        
    else if(getNumber(processor, arg1, & pos)) 
        pos &= ~3;
    else {
        printf("ERROR: unknown address/register\n");
        return;
    }
    
    if(!getNumberFromString(arg2, &len))
        len = 12;
    
    if(len < 4) 
        len = 4;
    
    for(i = 0; i < len; i++) {
        char *symbol = getSymbolFromAddress(pos);
        
        if(symbol)
            printf("%s:\n", symbol);
        
        printf("%s 0x%08x %s %s\n", 
               isBreakpoint(pos) ? "*b*" : "   ",
               pos, 
               pos == pc ? ">>>" : " : ",
               icmDisassemble(processor, pos));
        pos += 4;
    }
}

void doShowMemory(icmProcessorP processor, char *arg1, char *arg2, int show_symbols)
{
    int data, adr, i, count;
    int is_reg = 0, is_invalid;
    
    if(!getNumberFromString(arg1, & adr)) {
        if(!getNumberFromRegister(processor, arg1, &adr)) {
            if(!getAddressFromSymbol(arg1, &adr)) {
                printf("ERROR: need an address\n");
                return;            
            } /* else printf("symbol %s => %x\n", arg1, adr); */
        } /* else printf("reg %s => %x\n", arg1, adr); */
        is_reg = 1;        
    } /* else printf("number %s => %x\n", arg1, adr); */
    
    adr &= ~3;                
    int org_adr = adr;
    adr &= ~(4 * 4 - 1);
    
    if(!getNumberFromString(arg2, &count))
        count = MEMORY_DUMP_SIZE;
    
    if(count < 4)
        count = 4;
    
    /* make it start at x 16 address */
    count += (adr & 15) / 4;
    adr &= ~15;
    
    /* show previous line too */
    adr -= 16;
    count += 16 / 4;
    
    /* show full lines */
    if((count & 15) != 0) count = (count & (~15)) + 16;
    
    for(i = 0; i < count; i++) {
        char *symbol = show_symbols ? getSymbolFromAddress(adr) : 0;
        is_invalid = 0;
        
        if(symbol) {
            printf("\n%s:\n", symbol);
            printf("%08x: ", adr);            
        } else {
            if( (i & 3) == 0) 
                printf("\n%08x: ", adr);
        }
        
        if(!icmDebugReadProcessorMemory( processor, (Addr) (0xFFFFFFFF & adr), &data, 4, 1, ICM_HOSTENDIAN_HOST)) {
            is_invalid = 1;            
        }
        
        if(is_invalid)
            printf("%c????????", (adr == org_adr) ? '>' : ' ');
        else
            printf(" %c%08x", (adr == org_adr) ? '>' : ' ', data);
        adr += 4;
    }        
    printf("\n");        
    
}    

void doWriteMemory(icmProcessorP processor, char *arg1)
{
    char buffer[1024];
    int adr, data, i;
    
    
    if(!getNumber(processor, arg1, & adr)) {
        printf("ERROR: need an address\n");
        return;
    } 
    adr &= ~3;
    
    for(;;) {
        fprintf(stdout, "0x%08lx := ", adr);
        fflush(stdout);
        
        if(!readLine(buffer, 1024)) return;
        
        if(strlen(buffer) < 1) return;
        
        if(!getNumberFromString(buffer, & data)) {
            printf("ERROR: bad number: '%s'\n", buffer);
        } else {
            if(!icmDebugWriteProcessorMemory( processor, (Addr) (0xFFFFFFFF & adr), &data, 4, 1, ICM_HOSTENDIAN_HOST)) {
                printf("ERROR: could not write 0x%08lx to address 0x%08lx\n", data, adr);
                return;
            } 
            
            // readback:
            if(!icmDebugReadProcessorMemory( processor, (Addr) (0xFFFFFFFF & adr), &data, 4, 1, ICM_HOSTENDIAN_HOST)) {
                printf("0x%08lx == ?????\n", adr);
            } else {                
                printf("0x%08lx == 0x%08x\n", adr, data);
            }
            adr += 4;
        }
    }
}

void print_status_register(char *name, unsigned int value)
{
    int tmp;
    char *tmp2;
    
    printf("%s: %08x", name, value);
    
    printf(" flags=%c%c%c%c%c",
           ( value & (1L << 31)) ? 'N' : '-',
           ( value & (1L << 30)) ? 'Z' : '-',
           ( value & (1L << 29)) ? 'C' : '-',
           ( value & (1L << 28)) ? 'V' : '-',
           ( value & (1L <<  5)) ? 'T' : '-'
           );
    
    printf(" IRQ %-8s", (value & 0x80) ? "disabled" : "enabled");
    printf(" FIQ %-8s", (value & 0x40) ? "disabled" : "enabled");
    
    tmp = value & 31;
    switch(tmp) {
    case 16: tmp2 = "user"; break;
    case 17: tmp2 = "FIQ"; break;
    case 18: tmp2 = "IRQ"; break;
    case 19: tmp2 = "supervisor"; break;
    case 23: tmp2 = "abort"; break;
    case 27: tmp2 = "undefined"; break;
    case 31: tmp2 = "system"; break;
    default: tmp2 = "??";
    }
    printf(" mode=%s", tmp2);
    
    printf("\n");                    
}

static Uns32 work_regs1[REGISTER_COUNT_ALL], work_regs2[REGISTER_COUNT_ALL];
static Uns32 *work1 = &work_regs1;
static Uns32 *work2 = &work_regs2;
void work_registers_get_state(icmProcessorP processor)
{
    int i;
    Uns32 *tmp;
    
    tmp = work1;
    work1 = work2;
    work2 = tmp;
    
    for(i = 0; i < REGISTER_COUNT_NORMAL-1; i++) {
        icmReadReg(processor, reg_names[i], & work1[i]);
    }
    
}

void work_registers_print_changed(icmProcessorP processor)
{
    int i;
    for(i = 0; i < REGISTER_COUNT_NORMAL-1; i++) {        
        if(work1[i] != work2[i]) {        
            printf("%8s=%08lx\n", reg_names[i], work1[i]);
        }
    }    
}


void doShowRegisters(icmProcessorP processor)
{
    unsigned int i, tmp;
    int user_mode = 1;
    
    // first, update all registers
    for(i = 0; i < REGISTER_COUNT_ALL; i++) {
        reg_valid[i] = icmReadReg(processor, reg_names[i], & tmp);
        reg_changed[i] = tmp != reg_values[i];
        reg_values[i] = tmp;
        
        if(i == REGISTER_INDEX_CPSR) {
            if((tmp & 31) != 16)
                user_mode = 0;
            else            
                break;          
        }
    }
    
    
    for(i = 0; i < REGISTER_COUNT_NORMAL; i++) {
        printf("%8s=%08lx%c",
               reg_changed[i] ? reg_names[i] : reg_names_lower[i],
               reg_values[i],
               (i & 3) == 3 ? '\n' : ' '
               );
    }    
    
    print_status_register(reg_changed[REGISTER_INDEX_CPSR] ? 
                          reg_names[REGISTER_INDEX_CPSR] : reg_names_lower[REGISTER_INDEX_CPSR], 
                          reg_values[REGISTER_INDEX_CPSR]);    
    
    if(!user_mode) {
        printf("Banked registers:\n");
        
        for(i = REGISTERS_BANKED_START; i < REGISTERS_BANKED_END; i++) {
            printf("%14s=", reg_changed[i] ? reg_names[i] : 
                   reg_names_lower[i]);
            
            if( reg_valid[i])
                printf("%08lx", reg_values[i]);
            else
                printf("%-8s",  "???");
            
            printf("%c", (i % 3) == 2 ? '\n' : ' ');                          
        }        
        printf("\n");
        
        print_status_register(reg_changed[REGISTER_INDEX_SPSR] ? 
                              reg_names[REGISTER_INDEX_SPSR] : reg_names_lower[REGISTER_INDEX_SPSR], 
                              reg_values[REGISTER_INDEX_SPSR]);    
    }
    printf("\n");
    
}



void doSetBreakPoint(icmProcessorP processor, char *arg1)
{
    int adr;
    if(!getNumber(processor, arg1, &adr)) {        
        listAllBreakpoints();
        return;
    }
    
    /* XXX: this wont work on thumb */
    adr &= ~3;
    
    if(isBreakpoint(adr)) {
        removeBreakpoint(processor, adr);
        printf("REMOVED breakpoint from address 0x%08lx\n", adr);
    } else {
        setBreakpoint(processor, adr);
        printf("ADDED breakpoint from address 0x%08lx\n", adr);        
    }
    
    doListProgram(processor, arg1, 0);    
}

int doNextInstruction(icmProcessorP processor, char *arg1, int show_changes)
{
    int done = 0, reason;
    int i, count;
        
    if(!getNumberFromString(arg1, &count))
        count = 1;
    
    if(show_changes) {
        work_registers_get_state(processor);
    }
    
    for(i = 0; !done & i < count; i++) {        
        reason = run_one_cycle(processor, i == 0);                
        done = isSimulationEnded(reason);            
        
        /* print register changes */        
        if(show_changes) {
            work_registers_get_state(processor);
            work_registers_print_changed(processor);
        }
    }
    
    /* if e ran more than one instruction, show the listing */
    if(count > 1) {
        doListProgram(processor, 0, 0);      
    }
        
    return done;
}

int doRunUntilModeChange(icmProcessorP processor)
{
    int done = 0, reason, i = 0;
    int tmp, mode;
    
    icmReadReg(processor, "CPSR", & tmp);
    mode = tmp & 31;        
    
    do {
        reason = run_one_cycle(processor, i++ == 0);                
        done = isSimulationEnded(reason);            
        
        icmReadReg(processor, "CPSR", & tmp);
        tmp &= 31;
    } while(!done && tmp == mode);
    
    if(tmp != mode) printf("Mode switch: %02x -> %02x\n", mode, tmp);
    
    return done;
}
   

int doRunUntilInstruction(icmProcessorP processor, char *arg1)
{
    int i, count, done = 0;
    
    if(!arg1) {
        printf("ERROR: need an instruction\n");
    } else {    
        for(i = 0; !done && i < MAX_CYCLES; i++) {
            Uns32 currentPC = (Uns32)icmGetPC(processor); 
            char *tmp = (char *)icmDisassemble(processor, currentPC);
            if(i != 0 && strstr(tmp, arg1) == tmp + 9) {  /* +9 for inst in "8ff00abc mov r0, r12" */
                doListProgram(processor, 0, 0);
                break;
            }                                    
            done = isSimulationEnded(icmSimulate(processor, 1));
        }
        if(i == MAX_CYCLES)
            printf("ERROR: debugger giving up, could not find '%s' after %d cycles\n", arg1, i);
        else if(done)
            printf("ERROR: debugger could not find '%s' before termination\n", arg1);
    }
    return done;
}

int doRunUntilPosition(icmProcessorP processor, char *arg1)
{
    int i, count, adr, done = 0;
    
    if(!getNumberFromString(arg1, & adr)) {
        printf("ERROR: need an address\n");
    } else {
        for(i = 0; !done && i < MAX_CYCLES; i++) {
            Uns32 currentPC = (Uns32)icmGetPC(processor); 
            char *tmp = (char *)icmDisassemble(processor, currentPC);                    
            if(adr == currentPC) {
                doListProgram(processor, 0, 0);
                break;
            }                                    
            done = isSimulationEnded(icmSimulate(processor, 1));
        }
        
        if(i == MAX_CYCLES)
            printf("ERROR: debugger giving up, did not see 0x%08lx after %d cycles\n", adr, i);
        else if(done)
            printf("ERROR: debugger could not se 0x%08lx before termination\n", adr);
    }                
    return done;
}

int doRun(icmProcessorP processor, char *arg1)
{
    int steps, count, reason, done;    
    Uns32 start_pc = (Uns32)icmGetPC(processor);
    
    if(! getNumberFromString(arg1, &steps))
        steps = MAX_CYCLES;
    
    icmSetICountBreakpoint(processor, (Uns64) steps);        
    
    for(count = 0, done = 0; !done; count++) {
        Uns32 end_pc;
        
        icmSimulatePlatform();
        reason = icmGetStopReason(processor);
        end_pc = (Uns32)icmGetPC(processor);
        
        if(reason == ICM_SR_BP_ADDRESS) {
            if(end_pc == start_pc && count == 0) {
                // first instruction was on a breakpoint :(
                reason = run_one_cycle(processor,1);
            } else done = 1;
        }
        
        if(reason == ICM_SR_BP_ICOUNT) done = 1;        
    }
    
    icmClearICountBreakpoint(processor);    
    doListProgram(processor, 0, 0);      
    
    if(reason == ICM_SR_BP_ICOUNT) {
        printf("NOTE: stopped simulation after %d cycles\n", steps);
    } else if(reason == ICM_SR_BP_ADDRESS) {            
        printf("Reached a breakpoint...\n");
    }    
    
    return isSimulationEnded(reason);
}

static int need_start_binary = True;
int doLoadBinary(icmProcessorP processor, char *filename )
{
    if(filename) {
        char *type = strrchr(filename , '.');
        char *at;
        int adr = 0;
        
        
        if(type == 0) {
            fprintf(stderr, "Unknown file type: '%s'\n", filename);
            return 0;
        }
        
        /* see if a load address is given */
        at = strchr(type, ':');        
        if(at) {
            *at++ = '\0';
            if(*at != '\0') {         
                if(!getNumberFromString( at, & adr)) {
                    adr = 0;
                    printf("ERROR: invalid load address: %s\n", at);
                } 
            }
        }
        
        /* load based on extension */
        if(!strcasecmp(type, ".fw") || !strcasecmp(type, ".bin")) { 
            int c;
            FILE *fp = fopen(filename, "rb");
            if(!fp) {
                fprintf(stderr, "Could not open '%s' for reading\n", filename);
                return 0;
            }
            
            printf("Loading %s to 0x%08lx\n", filename, adr);
            
            while(!feof(fp)) {
                c = 0;
                c |= (0xFF & fgetc(fp)) << 0;
                c |= (0xFF & fgetc(fp)) << 8;
                c |= (0xFF & fgetc(fp)) << 16;
                c |= (0xFF & fgetc(fp)) << 24;                
                if(!icmDebugWriteProcessorMemory( processor, (Addr) (0xFFFFFFFF & adr), &c, 4, 1, ICM_HOSTENDIAN_HOST)) {    
                    fprintf(stderr, "Could to write '%s' to address %08lx\n", filename, adr);
                    return 0;
                }
                adr += 4;
            }
            fclose(fp);
            return 1;
        } else if(!strcasecmp(type, ".elf")) {
            //processor, ELF file, virtual address, enable debug, start execution spec in object file
            int ret = icmLoadProcessorMemory(processor, filename, False, True, need_start_binary);
            if(!ret) return 0;
            need_start_binary = False;                
            doLoadSymbols(filename);        
            return 1;            
        } else {
            fprintf(stderr, "Unsupported file type: '%s'\n", filename);
            return 0;
        }
    } else {
        printf("No file given!\n");
    }
    
    return 0; /* failed */
}

void doShowHelp()
{
    printf("Valid commands are\n"
           "  n [c]            Next c instructions\n"
           "  N [c]            Next c instructions and show register updates\n"
           "  c                Next instruction until an ARM mode change is detected\n"
           "  g [c]            Go\n"           
           "  f inst           Run unitl instruction int. Example 'f str'\n"
           "  u adr            Run up to this address. Example: 'b 0x00001234'\n"
           "  b adr            Set breakpoint. Similair to 'u' but works with timers etc.\n"           
           "  r                Show registers\n"           
           "  m adr [len]      Dump memory contecnts. Example: 'm 0xFF00'\n"
           "  M adr [len]      Same as 'm' but includes symbols\n"
           "  w adr            Write data to memory starting at address adr.\n"
           "  s [adr] [len]    Show code\n"
           "  L filename       Load file to memory\n"
           "  q                Quit\n"
           );
    
}


void doTerminate()
{    
    printf ("\n**** End time: %Lf  ****\n\n", icmGetCurrentTime ());
    icmTerminate();
    exit(0);
}

/***********************************************************
 * signal handlers
 ***********************************************************/

void int_handler(int sig)
{
    printf("Ctrl-C detected, existing...\n");
    fflush(stdout);
    
    doTerminate();
}



/***********************************************************
 * main
 ***********************************************************/

int main(int argc, const char **argv) 
{    
    int data, adr, i, debug = 0, verbose = 0;
    
    char *binaries[32];
    int binaries_count = 0;
    
    
    /* parse args */
    for(i = 1; i < argc; i++) {
        const char *arg = argv[i];
        
        if(*arg == '-') {
            if(!strcmp(arg, "-debug") || !strcmp(arg, "--debug") || !strcmp(arg, "-d"))
                debug = 1;
            else if(!strcmp(arg, "-verbose") || !strcmp(arg, "--verbose") || !strcmp(arg, "-V"))
                verbose = 1;
            else {
                fprintf(stderr, "Unknown option: %s\n", arg);
                return 3;
            }
        } else {
            if(binaries_count < 31)
                binaries[binaries_count++] = arg;
        }
    }
    
    if(binaries_count == 0) {
        fprintf(stderr, "Usage: %s [-d] [-V] <ELF file> [<additional elf files>]\n", argv[0]);
        return 3;        
    }
    
    icmBusP bus;
    icmProcessorP processor;
    
    /* load the platform with the first binary */
    createPlatform(debug, verbose, &processor, &bus);
    
    /* load the remaining binaries */
    for(i = 0; i < binaries_count; i++) {
        if(!doLoadBinary(processor, binaries[i])) {
            fprintf(stderr, "ERROR: unable to load the binary '%s'\n", binaries[i]);
            return 20;
        }
    }
    
    icmSimulationStarting();
    
    
    if (False == icmSetSimulationTimePrecision (0.00001)) {
        printf ("Couldn't set precision\n");
        return 0;
    }
    
    // dealing with warnings
    //    icmIgnoreMessage("ARM_MORPH_UCA");
    
    
    if(debug) {
        icmSimulatePlatform();    
        
    } else {
        Bool done = False;
        
        printf("\n"
               "***********************************************\n"
               " Welcome to the SICS ARM debugger, where we\n"
               " keep things _really_ simple...\n"
               "\n"
               " Binary = %s, Debug = %d, Verbose = %d\n"
               "***********************************************\n"
               "\n",
               binaries[0], debug, verbose
               );
        
        /*
         * set this as late as possible so OVP can't override
         */
        signal(SIGINT, int_handler);
        
        
        
        /*
         * the command loop
         */
        while(!done) { 
            int i, count;
            char *tmp, buffer[1024], *cmd, *arg1, *arg2, *arg3, *start;
            Uns32 currentPC;
            
            
            printf("Enter debugger command> ");                
            fflush(stdout);
            
            if(!readLine(buffer, 1024))
                continue;
            
            start = buffer;
            
            cmd = trimString(start);
            if(cmd == 0) continue;
            
            arg1 = getNextString(cmd);
            arg2 = getNextString(arg1);
            arg3 = getNextString(arg2);
            /* dummy = */ getNextString(arg3);
            
            switch(cmd[0]) {
            case 'n':
                done = doNextInstruction(processor, arg1, 0);
                break;
                
            case 'N':
                done = doNextInstruction(processor, arg1, 1);
                break;
                
            case 'c':
                done = doRunUntilModeChange(processor);
                break;
                
            case 'f':            
                done = doRunUntilInstruction(processor, arg1);
                break;
                
            case 'u':
                done = doRunUntilPosition(processor, arg1);
                break;
                
            case 'b':
                doSetBreakPoint(processor, arg1);
                break;
                
            case 'l':
                doLoadSymbols(arg1);
                break;
            case 's':
                doListProgram(processor, arg1, arg2);            
                break;
                
            case 'm':
                doShowMemory(processor, arg1, arg2, 0);
                break;
                
            case 'M':
                doShowMemory(processor, arg1, arg2, 1);
                break;
                
            case 'w':
                doWriteMemory(processor, arg1);
                break;
                
            case 'r':
                doShowRegisters(processor);
                break;     
                
            case 'g':
                doRun(processor, arg1);
                break;
                
            case 'L':
                doLoadBinary(processor, arg1);
                break;
                
            case 'q':
                done = True;
                break;
                
            default:
                doShowHelp();
            }
        } 
    }
    
    doTerminate();
    return 0;
}

