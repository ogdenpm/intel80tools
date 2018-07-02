;
; *-*-*   CNF286.CSD   *-*-*
;
;   Configure iSDM System Debug Monitor for iAPX 286-based boards.
;
;   Invocation: submit CNF286(configuration_file_name)
;
;        where configuration_file_name is the name of the
;        configuration file minus the extension.
;
;

asm286 %0.a28  pagewidth(132) macro(100) include(omf286.inc);

bnd286 &
    %0.obj, &
    cnfsdm.lb2  &
    object(%0.lnk) print(%0.mp1) name(%0) noload

bld286 &
   %0.lnk &
   object(%0) print(%0.mp2) &
   buildfile(cnf286.bld) bootload 


;
;  iSDM System Debug Monitor configured.
;
;
