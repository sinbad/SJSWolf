#define MAXCODES           5
#define CODE_NONE          0
#define CODE_SCAN          1
#define CODE_NAME          2
#define CODE_SPACESINGLE   3


typedef struct
 {
 char returnCode;
 char numBytes;
 char code[16];
 }CODE;

extern long numCodeWords;
extern CODE codeWord[MAXCODES];

char checkCode(char *codeBuf);
void setCodes(void);

