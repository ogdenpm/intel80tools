DECLARE	cmdch BASED cmd$p BYTE;
DECLARE	cmdbuf BASED cmd$p (1) BYTE;
DECLARE	pageTab1 BASED pageTab1$p (256) STRUCTURE (state BYTE, mpage BYTE);
DECLARE	pageTab2 BASED pageTab2$p (256) STRUCTURE (pageIdx BYTE, fileIdx BYTE);
DECLARE	inRecord BASED inRecord$p STRUCTURE (rectyp BYTE, reclen ADDRESS, record(1) BYTE);
DECLARE	outRecord BASED outRecord$p STRUCTURE (rectyp BYTE, reclen ADDRESS, record(1) BYTE);
