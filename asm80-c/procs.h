void AsmComplete();
void BalanceError();
bool BlankAsmErrCode();
bool BlankMorPAsmErrCode();
bool ChkGenObj();
void ChkInvalidRegOperand();
void ChkLF();
void Close(word conn, apointer statusP);
void CloseF(word conn);
void CollectByte(byte rc);
void CommandError();
void Delete(pointer pathP, apointer statusP);
void DoPass();
void EmitXref(byte xrefMode, pointer name);
void Error(word ErrorNum);
void Exit();
void ExpressionError();
void FileError();
void FinishAssembly();
void FinishLine();
void FinishPrint();
void FlushM();
void Flushout();
void GetAsmFile();
byte GetCh();
byte GetChClass();
byte GetCmdCh();
void GetId(byte type);
void GetNum();
word GetNumVal();
byte GetPrec(byte topOp);
byte GetSrcCh();
void GetStr();
byte HaveTokens();
void IllegalCharError();
void InitLine();
void InitRecTypes();
void InsertByteInMacroTbl(byte c);
void InsertCharInMacroTbl(byte c);
void IoErrChk();
void IoError(pointer path);
bool IsComma();
bool IsCR();
bool IsGT();
bool IsLT();
bool IsPhase1();
bool IsPhase2Print();
bool IsReg(byte topOp);
bool IsRParen();
bool IsSkipping();
bool IsTab();
bool IsWhite();
void Load(pointer pathP, word LoadOffset, word swt, word entryP, apointer statusP);
void LocationError();
byte Lookup(byte tableId);
void MkCode(byte arg1b);
void MultipleDefError();
void Nest(byte arg1b);
void NestingError();
byte Nibble2Ascii(byte n);
byte NonHiddenSymbol();
byte NxtTokI();
void OperandError();
void Open(apointer connP, pointer pathP, word access, word echo, apointer statusP);
void OpenSrc();
void Outch(byte c);
void OutStrN(pointer s, byte n);
void Ovl11();
void Ovl8();
void ParseControlLines();
void PackToken();
void ParseControls();
void PhaseError();
pointer Physmem();
void PopToken();
void PrintCmdLine();
void PrintDecimal(word n);
void PrintLine();
void PushToken(byte type);
void Read(word conn, pointer buffP, word count, apointer actualP, apointer statusP);
void ReadM(word blk);
void ReinitFixupRecs();
void Rescan(word conn, apointer statusP);
void ResetData();
void RuntimeError(byte errCode);
word SafeOpen(pointer pathP, word access);
void Seek(word conn, word mode, apointer blockP, apointer byteP, apointer statusP);
void SetExpectOperands();
bool ShowLine();
void Skip2EOL();
void Skip2NextLine();
void SkipWhite();
void SkipWhite_2();
void SourceError(byte errCh);
void StackError();
bool StrUcEqu(pointer s, pointer t);
void Sub4291();
void Sub546F();
void Sub5CAD(word val, byte type);
void Sub7041_8447();
void Sub72A4(byte arg1b);
void Sub7327();
void Sub73AD();
void Sub7517();
void Sub753E();
void Sub75FF();
void Sub76CE();
void Sub770B();
void Sub7844();
void Sub787A();
void Sub78CE();
void SyntaxError();
void sysInit(int argc, char **argv);
bool TestBit(byte bitIdx, pointer bitVector);
void Tokenise();
void UndefinedSymbolError();
void UnNest(byte arg1b);
void UnpackToken(apointer src, pointer dst);
void UpdateSymbolEntry(word val, byte type);
void ValueError();
void Write(word conn, pointer buffP, word count, apointer statusP);
void WriteExtName();
void WriteM();
void WriteModend();
void WriteModhdr();
void WriteRec(pointer recP);

#define move(cnt, src, dst)	memcpy(dst, src, cnt)

void DumpSymbols(byte tableId);
void DumpOpStack();
void DumpTokenStack();