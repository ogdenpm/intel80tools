REM create a hard link the unix tools based on busybox
for %%i in (basename cat cmp cp date echo ls mkdir mv rm touch tr) do busybox64u ln -Tf busybox64u.exe %%i.exe 
