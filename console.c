// Console input and output.
// Input is from the keyboard or serial port.
// Output is written to the screen and serial port.
#include "types.h"
#include "defs.h"
#include "param.h"
#include "traps.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"

static void consputc(int);

static int panicked = 0;

static struct {
  struct spinlock lock;
  int locking;
} cons;

static void
printint(int xx, int base, int sign)
{
  static char digits[] = "0123456789abcdef";
  char buf[16];
  int i;
  uint x;

  if(sign && (sign = xx < 0))
    x = -xx;
  else
    x = xx;

  i = 0;
  do{
    buf[i++] = digits[x % base];
  }while((x /= base) != 0);

  if(sign)
    buf[i++] = '-';

  while(--i >= 0)
    consputc(buf[i]);
}
//PAGEBREAK: 50

// Print to the console. only understands %d, %x, %p, %s.
void
cprintf(char *fmt, ...)
{
  int i, c, locking;
  uint *argp;
  char *s;

  locking = cons.locking;
  if(locking)
    acquire(&cons.lock);

  if (fmt == 0)
    panic("null fmt");

  argp = (uint*)(void*)(&fmt + 1);
  for(i = 0; (c = fmt[i] & 0xff) != 0; i++){
    if(c != '%'){
      consputc(c);
      continue;
    }
    c = fmt[++i] & 0xff;
    if(c == 0)
      break;
    switch(c){
    case 'd':
      printint(*argp++, 10, 1);
      break;
    case 'x':
    case 'p':
      printint(*argp++, 16, 0);
      break;
    case 's':
      if((s = (char*)*argp++) == 0)
        s = "(null)";
      for(; *s; s++)
        consputc(*s);
      break;
    case '%':
      consputc('%');
      break;
    default:
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c);
      break;
    }
  }

  if(locking)
    release(&cons.lock);
}

void
panic(char *s)
{
  int i;
  uint pcs[10];

  cli();
  cons.locking = 0;
  // use lapiccpunum so that we can call panic from mycpu()
  cprintf("lapicid %d: panic: ", lapicid());
  cprintf(s);
  cprintf("\n");
  getcallerpcs(&s, pcs);
  for(i=0; i<10; i++)
    cprintf(" %p", pcs[i]);
  panicked = 1; // freeze other CPU
  for(;;)
    ;
}

//PAGEBREAK: 50
#define BACKSPACE 0x100
#define CRTPORT 0x3d4
static ushort *crt = (ushort*)P2V(0xb8000);  // CGA memory

static void
cgaputc(int c)
{
  int pos;

  // Cursor position: col + 80*row.
  outb(CRTPORT, 14);
  pos = inb(CRTPORT+1) << 8;
  outb(CRTPORT, 15);
  pos |= inb(CRTPORT+1);

  if(c == '\n')
    pos += 80 - pos%80;
  else if(c == BACKSPACE){
    if(pos > 0) --pos;
  } else
    crt[pos++] = (c&0xff) | 0x0700;  // black on white

  if(pos < 0 || pos > 25*80)
    panic("pos under/overflow");

  if((pos/80) >= 24){  // Scroll up.
    memmove(crt, crt+80, sizeof(crt[0])*23*80);
    pos -= 80;
    memset(crt+pos, 0, sizeof(crt[0])*(24*80 - pos));
  }

  outb(CRTPORT, 14);
  outb(CRTPORT+1, pos>>8);
  outb(CRTPORT, 15);
  outb(CRTPORT+1, pos);
  crt[pos] = ' ' | 0x0700;
}

void
consputc(int c)
{
  if(panicked){
    cli();
    for(;;)
      ;
  }

  if(c == BACKSPACE){
    uartputc('\b'); uartputc(' '); uartputc('\b');
  } else
    uartputc(c);
  cgaputc(c);
}

#define NULL ((void*)0)

typedef struct Node {
    char* string;
    struct Node* next;
} Node;

typedef struct {
    Node* head;
    Node* tail;
    int size;
    int capacity;
} CircularBuffer;

// Function to initialize the circular buffer
CircularBuffer* initCircularBuffer(int capacity) {
    CircularBuffer* cb = malloc(sizeof(CircularBuffer));
    cb->head = NULL;
    cb->tail = NULL;
    cb->size = 0;
    cb->capacity = capacity;
    return cb;
}

// Function to add a string to the circular buffer
void addString(CircularBuffer* cb, char* string) {
    Node* newNode = malloc(sizeof(Node));
    newNode->string = string; // Assuming you have a way to copy the string
    newNode->next = NULL;

    if (cb->size == 0) {
        cb->head = newNode;
        cb->tail = newNode;
    } else {
        cb->tail->next = newNode;
        cb->tail = newNode;
        if (cb->size == cb->capacity) {
            Node* temp = cb->head;
            cb->head = cb->head->next;
            free(temp);
        }
    }
    cb->size = (cb->size + 1) % cb->capacity;
}

void removeOldest(CircularBuffer* cb) {
    if (cb->size > 0) {
        Node* temp = cb->head;
        cb->head = cb->head->next;
        free(temp);
        cb->size--;
    }
}

int compareStrings(const char* str1,const char* str2) {
    while (*str1) {
		if (*str1 != *str2 || !*str2) {
			return 0; // Not a prefix if mismatch or B ends first
		}
		str1++;
		str2++;
	}
	return 1;
}

char* findMatchingString(CircularBuffer* cb, char* A) {
    Node* current = cb->tail;
    while (current != '\0') {
        if (compareStrings(A, current->string) == 1) {
            return current->string;
        }
        current = current->next;
        if (current == cb->head) {
            // Reached the oldest string, stop traversing
            break;
        }
    }
    return (char)0;
}



#define INPUT_BUF 128
struct {
  char buf[INPUT_BUF];
  uint r;  // Read index
  uint w;  // Write index
  uint e;  // Edit index
} input;

#define C(x)  ((x)-'@')  // Control-x

#define K 4
char clipboard[K]; // saved buffer of copy-paste
int clipboard_size;

void format_string(char* format_string){
	for (int i = 0;  format_string[i] != '\0'; i++){
		int c = format_string[i];
		if (c == 32){} // space
		else if (c >= 48 && c <= 57) // numbers
		{
			c += K;
			if (c >= 58){ // if numbers goes bigger than 9
				c += 7;
			}
			format_string[i] = (char)c;
		} 
		else if (c >= 97 && c <= 122) // CAPITAL LETERS to small letters
		{
			c -= 32;
			format_string[i] = (char)c;
		}
		else if (c >= 65 && c <= 90) // small letters to CAPITAL LETTERS
		{
			c += 32;
			format_string[i] = (char)c;
		}
		else { // default for special characters -> remove
			int j;
			for (j = i; format_string[j] != '\0'; j++) {
                format_string[j] = format_string[j + 1];
            }
		}
	}
}

void
consoleintr(int (*getc)(void))
{
  int c, doprocdump = 0;

  acquire(&cons.lock);
  while((c = getc()) >= 0){
    switch(c){
    case C('P'):  // Process listing.
      // procdump() locks cons.lock indirectly; invoke later
      doprocdump = 1;
      break;

    case 'C': case 'X': //copy & cut
      clipboard_size = input.e - input.w > 4 ? 4 : input.e - input.w;
      for (int i = 0; i < clipboard_size; i++) {
        clipboard[i] = input.buf[(input.e - clipboard_size + i) % INPUT_BUF];
      }

      if (c == 'X'){
        for (int i = 0; i < clipboard_size; i++){  
          if(input.e != input.w){
          input.e--;
          consputc(BACKSPACE);
          }
        }
      }
      break;
    case 'V': //paste
      	for (int i = 0; i < clipboard_size; i++) {
          input.buf[input.e++ % INPUT_BUF] = clipboard[i];
          consputc(clipboard[i]);
        }
      break;
      
    case 'E':
		char input_str[INPUT_BUF];
		int i = 0;
		while(input.e != input.w){
			input_str[i] = input.buf[i];
			input.e--;
			consputc(BACKSPACE);
			i++;
		}
		input_str[i] = '\0';

		format_string(input_str);
		
		i = 0;
		while(input_str[i] != '\0'){
			input.buf[input.e++ % INPUT_BUF] = input_str[i];
			consputc(input_str[i]);
			i++;
		}
      break;

    case '\t':
      // tab handler
      break;
    case C('U'):  // Kill line.
      while(input.e != input.w &&
            input.buf[(input.e-1) % INPUT_BUF] != '\n'){
        input.e--;
        consputc(BACKSPACE);
      }
      break;
    case C('H'): case '\x7f':  // Backspace
      if(input.e != input.w){
        input.e--;
        consputc(BACKSPACE);
      }
      break;
    default:
      if(c != 0 && input.e-input.r < INPUT_BUF){
        c = (c == '\r') ? '\n' : c;
        input.buf[input.e++ % INPUT_BUF] = c;
        consputc(c);
        if(c == '\n' || c == C('D') || input.e == input.r+INPUT_BUF){
          input.w = input.e;
          wakeup(&input.r);
        }
      }
      break;
    }
  }
  release(&cons.lock);
  if(doprocdump) {
    procdump();  // now call procdump() wo. cons.lock held
  }
}


int
consoleread(struct inode *ip, char *dst, int n)
{
  uint target;
  int c;

  iunlock(ip);
  target = n;
  acquire(&cons.lock);
  while(n > 0){
    while(input.r == input.w){
      if(myproc()->killed){
        release(&cons.lock);
        ilock(ip);
        return -1;
      }
      sleep(&input.r, &cons.lock);
    }
    c = input.buf[input.r++ % INPUT_BUF];
    if(c == C('D')){  // EOF
      if(n < target){
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        input.r--;
      }
      break;
    }
    *dst++ = c;
    --n;
    if(c == '\n')
      break;
  }
  release(&cons.lock);
  ilock(ip);

  return target - n;
}

int
consolewrite(struct inode *ip, char *buf, int n)
{
  int i;

  iunlock(ip);
  acquire(&cons.lock);
  for(i = 0; i < n; i++)
    consputc(buf[i] & 0xff);
  release(&cons.lock);
  ilock(ip);

  return n;
}

void
consoleinit(void)
{
  initlock(&cons.lock, "console");

  devsw[CONSOLE].write = consolewrite;
  devsw[CONSOLE].read = consoleread;
  cons.locking = 1;
  ioapicenable(IRQ_KBD, 0);
}
