#include <stdbool.h>
typedef unsigned char byte;
typedef unsigned short address;
typedef byte *pointer;

#define	CR	0xd
#define	ERR203	203
#define	ERR208	208
#define	ERR210	210
#define	ERR217	217
#define	ERR218	218
#define	ERR225	225
#define	ERR226	226
#define	FILE_T	STRUCTURE(link ADDRESS, pathP ADDRESS, isopen BYTE, conn BYTE)
#define	LF	0xa
#define	LIBREC_T	STRUCTURE(LIBREC_T1,LIBREC_T2)
#define	LIBREC_T1	crc BYTE, type BYTE, len ADDRESS, bytesLeft ADDRESS
#define	LIBREC_T2	curBlk ADDRESS, curByte ADDRESS, posBlk ADDRESS, posByte ADDRESS
#define	LINE_T	STRUCTURE(next ADDRESS, len ADDRESS, text(1) BYTE)
#define	OBJEND	4
#define	SYMBOL_T	STRUCTURE(SYMBOL_T1)
#define	SYMBOL_T1	link ADDRESS, curCmd ADDRESS, moduleCnt ADDRESS, symLen BYTE, symbol(1) BYTE
extern address actual;
extern address argFilename;
extern address hashTable[1];
extern address inConn;
extern reset label;
extern address status;
void Close(conn, statusP); declare conn address, statusP address;
void Delete(pathP, statusP); declare pathP address, statusP address;
void Error(errorNum); declare errorNum address;
void Exit();
void Open(connP, pathP, access, echo, statusP);
        static address connP, pathP address, access address, echo address, statusP address; end;
address Pastfilename(chP); declare chP address;
void Read(conn, buffP, count, actualP, statusP);
        static address conn, buffP address, count address, actualP address, statusP address; end;
void Rename(oldP, newP, statusP);
          static address oldP, newP address, statusP address; end;
void Seek(conn, mode, blockP, byteP, statusP);
        static address conn, mode address, blockP address, byteP address, statusP address; end;
address Skipspc(chP); declare chP address;
void Write(conn, buffP, count, statusP);
         static address conn, buffP address, count address, statusP address; end;
