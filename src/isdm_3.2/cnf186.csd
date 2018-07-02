;
; *-*-*   CNF186.CSD   *-*-*
;
;   Configure iSDM System Debug Monitor for iAPX 186-based boards.
;
;   Invocation: submit CNF186(configuration_file_name)
;
;        where configuration_file_name is the name of the
;        configuration file minus the extension.
;
;
;
asm86 %0.a86 macro(100) debug errorprint include(omf86.inc)

link86 &
    %0.obj, &
    cnfsdm.lib, &
    8087.lib to %0.lnk print(%0.mp1) 
;    
;
loc86 &
    %0.lnk &
    addresses(classes( &
       data(400h), &
       code(0f8240h),&
       restart (0fff80h))) &
       noinitcode print(%0.mp2) &
       start(montor_186) bootstrap  ;purge 
;
;
;
;  iSDM System Debug Monitor configured.
;
;
