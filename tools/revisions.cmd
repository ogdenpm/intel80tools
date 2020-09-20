@echo off
setlocal
for %%I in (*) do if /I [%%~xI] neq [.exe] if [%%~nI] neq [] call filever %%I
