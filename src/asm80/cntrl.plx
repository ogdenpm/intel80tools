$IF MACRO
cntrlm: do;
$include(:f3:cntrlm.ipx)
$ELSE
cntrln: do;
$include(:f3:cntrln.ipx)
$ENDIF

declare controlTable(*) byte data(
            35h, 'DEBUG', 3Ah, 'MACRODEBUG',
            34h, 'XREF', 37h, 'SYMBOLS',
            36h, 'PAGING', 33h, 'TTY',
            25h, 'MOD85', 35h, 'PRINT',
            36h, 'OBJECT', 39h, 'MACROFILE',
            29h, 'PAGEWIDTH', 2Ah, 'PAGELENGTH',
              7, 'INCLUDE', 5, 'TITLE',
              4, 'SAVE', 7, 'RESTORE',
              5, 'EJECT', 14h, 'LIST',
            13h, 'GEN', 14h, 'COND');

declare (tokVal, savedCtlLIST, savedCtlGEN) byte,
    controlError bool;


ChkParen: procedure(arg1b) bool;
    declare arg1b byte;
    call SkipWhite;
    reget = 0;
    return arg1b + '(' = curChar;
end;



GetTok: procedure byte;
    tokBufLen = 0;
    tokType = O$NONE;
    if IsCR then
        return curChar;

    call SkipWhite$2;
    if curChar > 'A'-1 and 'Z'+1 > curChar or curChar > 'a'-1 and 'z'+1 > curChar then
    do;                            /* letter */
        call GetId(1);
        if BlankAsmErrCode and tokenSize(0) < 14 then
            call move(tokBufLen := tokenSize(0), .lineBuf, .tokBuf);
    end;
    else if curChar > '0'-1  and  '9'+1 > curChar then    /* digit ? */
    do;
        call GetNum;
        if BlankAsmErrCode then
        do;
            tokNumVal = GetNumVal;
            tokType = TT$NUM;
            tokBuf(0) = ' ';
        end;
    end;
    else if curChar = '''' then    /* string ? */
    do;
        call GetStr;
        if BlankAsmErrCode then
        do;
            tokBufLen = 64;
            if tokenSize(0) < 64 then
                tokBufLen = tokenSize(0);
            tokType = TT$STR;
            if tokBufLen > 0 then
                call move(tokBufLen, .lineBuf, .tokBuf);
        end;
    end;
    else
    do;
        tokBufLen = 1;
        tokBuf(0) = curChar;
        return curChar;
    end;

    call PopToken;
    return tokBuf(0);
end;



Sub6F07: procedure(arg1w) bool;
    declare arg1w address;
    declare pad address;

    if tokBufIdx = 0 then
        return FALSE;

    tokBuf(tokBufIdx) = ' ';
    tokBufLen = tokBufIdx;
    if IsWhite then
        return ChkParen(1);
    return TRUE;
end;

Sub6F39: procedure;
    call SkipWhite$2;

    do while 1;
        if IsRParen or IsWhite then
        do;
            if Sub6F07(.tokBuf(0)) then
                return;
            goto done;
        end;

        tokBuf(tokBufIdx) = curChar;
        tokBufIdx = tokBufIdx + 1;
        if tokBufIdx > 14 then
            goto done;
        curChar = GetCh;
    end;
done:
    curFileName$p = .tokBuf;
    call FileError;
end;


GetFileParam: procedure;
    tokBufIdx = 0;
    if not ChkParen(0) then    /* ( */
        call FileError;
    else
    do;
        call Sub6F39;
        call move(tokBufIdx + 1, .tokBuf, curFileName$p);
        if not ChkParen(1) then    /* ) */
            call FileError;
    end;
end;


GetMacroFileDrive: procedure;
    call SkipWhite$2;
    tokBufIdx = 0Dh;
    ii = 0;

    do while not IsRParen and ii < 4;
        asmax$ref(ii) = curChar;
        ii = ii + 1;
        curChar = GetCh;
    end;

    if IsRParen or IsWhite then
        if Sub6F07(.asmax$ref) then
            return;
    curFileName$p = .asmax$ref;
    tokBufIdx = 0;
    call FileError;
end;




GetControlNumArg: procedure bool;
    if ChkParen(0) then    /* ( */
    do;
        tokVal = GetTok;
        if tokType = TT$NUM then
            return ChkParen(1);    /* ) */
    end;
    return FALSE;
end;


LookupControl: procedure byte;
    declare (cmdIdx, cmdStartIdx) byte, 
        ctlVal bool,
        (cmdLen, ctlFlags, noInvalid) byte,
        (control$p, nextControl$p, ctlSeen$p) pointer;
    declare twoch address at(.tokBuf);
    declare ch based control$p byte;
    declare ctlSeen based ctlSeen$p byte;

    SetControl: procedure;
        if not noInvalid then
        do;
            controls(controlId) = ctlVal;
            if not ctlVal then
                controlId = 17;    /* no action needed */
        end;
    end;


    cmdLen = tokBufLen;
    cmdStartIdx = 0;
    ctlVal = TRUE;
    if twoch = 'ON' then    /* NO stored with bytes swapped */
    do;
        cmdStartIdx = 2;    /* don't match the NO in the table */
        ctlVal = FALSE;        /* control will be set as false */
        cmdLen = tokBufLen - 2;    /* length of string to match excludes the NO */
    end;

    control$p = .controlTable;
    controlId = 0;
    cmdIdx = cmdStartIdx;

    do while controlId < 20;
        nextControl$p = control$p + (ch and 0Fh) + 1;
        if (ch and 0Fh) = cmdLen then
        do;
            ctlFlags = ch;
            do while cmdIdx < tokBufLen;
                control$p = control$p + 1;
                if ch <> tokBuf(cmdIdx) then
                    cmdIdx = tokBufLen + 1;    /* cause early Exit */
                else
                    cmdIdx = cmdIdx + 1;    /* check next character */
            end;

            if cmdIdx = tokBufLen then        /* found it */
                goto found;
        end;
        control$p = nextControl$p;
        controlId = controlId + 1;
        cmdIdx = cmdStartIdx;
    end;
    return 255;                        /* not found */

found:
$IF NOT MACRO 
    if controlId = 1 or controlId = 12h then    /* MACRODEBUG or GEN */
    do;
        if scanCmdLine then            /* only valid on command line not $ line */
            MacroDebugOrGen = TRUE;
        else
            return 255;            /* Error otherwise */
    end;
$ENDIF
    if (noInvalid := (ctlFlags and 10h) <> 10h) and not ctlVal then
        return 255;    /* NO not supported */

    if (ctlFlags and 20h) <> 20h then        /* GENERAL control */
    do;
        call SetControl;
        return controlId;
    end;

    if not primaryValid then            /* is a PRIMARY control */
        return 255;

    ctlSeen$p = .controlSeen(controlId);        /* make sure we only see a primary once */
    if ctlSeen then
        return 255;
    ctlSeen = TRUE;
    call SetControl;
    return controlId;
end;

ProcessControl: procedure;
    if controlId >= 17 or controlId < 5 then
        return;

    do case controlId - 5;
/* 0 */        do;            /* TTY */
            ctlTTY = TRUE;
            return;
        end;
/* 1 */        do;            /* MOD85 */
            ctlMOD85 = TRUE;
            return;
        end;
/* 2 */        do;            /* PRINT */
            controlFileType = 2;
            curFileName$p = .lstFile;
            call GetFileParam;
            return;
        end;
/* 3 */        do;            /* OBJECT */
            controlFileType = 3;
            curFileName$p = .objFile;
            call GetFileParam;
            return;
        end;
/* 4 */        do;            /* MACROFILE */
            controlFileType = 3;
            if ChkParen(0) then    /* optional drive for tmp file */
                call GetMacroFileDrive;
            else
                reget = 1;
            ctlMACROFILE = TRUE;
            return;
        end;
/* 5 */        do;            /* PAGEWIDTH */
            if GetControlNumArg then
            do;
                ctlPAGEWIDTH = tokNumVal;
                if ctlPAGEWIDTH > 132 then
                    ctlPAGEWIDTH = 132;
                if ctlPAGEWIDTH < 72 then
                    ctlPAGEWIDTH = 72;
                return;
            end;
        end;
/* 6 */        do;            /* PAGELENGTH */
            if GetControlNumArg then
            do;
                ctlPAGELENGTH = tokNumVal;
                if ctlPAGELENGTH < 15 then
                    ctlPAGELENGTH = 15;
                return;
            end;
        end;
/* 7 */        do;            /* INCLUDE */
            if not needToOpenFile then
            do;
                controlFileType = 1;
                if fileIdx = 5 then
                    call StackError;
                else
                do;
                    fileIdx = fileIdx + 1;
                    curFileName$p = .files(fileIdx);
                    call GetFileParam;
                    needToOpenFile = TRUE;
                    if scanCmdLine then
                        includeOnCmdLine = TRUE;
                    return;
                end;
            end;
        end;
/* 8 */        do;            /* TITLE */
            if ChkParen(0) then
            do;
                tokVal = GetTok;
                if tokType = TT$STR and tokBufLen <> 0 then
                do;
                    if phase <> 1 or IsPhase1 and primaryValid then
                    do;
                        call move(tokBufLen, .tokBuf, .ctlTITLESTR);
                        ctlTITLESTR(titleLen := tokBufLen) = 0;
                        if ChkParen(1) then
                        do;
                            ctlTITLE = TRUE;
                            return;
                        end;
                    end;
                end;
            end;
        end;
/* 9 */        do;            /* SAVE */
            if saveIdx > 7 then
                call StackError;
            else
            do;
                call move(3, .ctlLIST, .saveStack + saveIdx * 3);
                saveIdx = saveIdx + 1;
                return;
            end;
        end;
/* 10 */    do;            /* RESTORE */
            if saveIdx > 0 then
            do;
                call move(3, .saveStack + (saveIdx := saveIdx - 1) * 3, .ctlLIST);
                return;
            end;
        end;
/* 11 */    do;            /* EJECT */
            ctlEJECT = ctlEJECT + 1;
            return;
        end;
    end;
    controlError = TRUE;
end;

ParseControls: procedure public;
    isControlLine = TRUE;
    ctlLISTChanged, savedCtlLIST = ctlLIST;
$IF MACRO
    savedCtlGEN = ctlGEN;
$ENDIF
    controlError = FALSE;

    do while GetTok <> CR and not controlError;
        if tokBuf(0) = ';' then        /* skip comments */
            call Skip2EOL;
        else if LookupControl = 255 then    /* Error ? */
            controlError = TRUE;
        else
            call ProcessControl;
    end;

    if controlError then
    do;
        if tokBuf(0) <> CR then
        do;
            reget = 0;
            call Skip2EOL;
        end;

        if scanCmdLine then
            call RuntimeError(2);    /* command Error */
        else
            call CommandError;
    end;

    call ChkLF;            /* eat the LF */
    if ctlLIST <> savedCtlLIST then
        ctlLISTChanged = TRUE;
$IF MACRO
    else if ctlGEN <> savedCtlGEN and expandingMacro then
        ctlLISTChanged = FALSE;
$ENDIF

    reget = 0;
end;

end;
