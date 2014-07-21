@echo off
rem  deps_cp.bat  dest_path src_path path file1 file2
rem
if {%1}=={} (
  exit 1
  goto :EOF
)

SET CMD_OP=%1
shift

if {%CMD_CMD%}=={cp}    goto :CMD_CP
if {%CMD_CMD%}=={mv}    goto :CMD_MV
if {%CMD_CMD%}=={rm}    goto :CMD_RM
if {%CMD_CMD%}=={chmod} goto :CMD_CHMOD

:CMD_CP
  SET SRC_PATH=%2
  SET DEST_PATH=%1
  shift
  shift

  :CP_NEXT
    if {%1}=={} (
	  goto :EOF
    )
  
    if {%1}=={--path} (
      SET SRC_PATH=%3
      SET DEST_PATH=%2
      shift
      shift
      shift
      goto :CP_NEXT
    )
  
    if not exist %SRC_PATH% (
        echo %SRC_PATH% not found
        exit 1
        goto :EOF
    )

    if not exist %DEST_PATH% (
      mkdir %DEST_PATH%
    )	
  
    if not exist %SRC_PATH%\%1 (
        echo %1 at %SRC_PATH% not found
    ) else (
      echo copy %SRC_PATH%\%1 to %DEST_PATH%
      copy /y %SRC_PATH%\%1 %DEST_PATH%
    )

    shift

goto :CP_NEXT
  
:CMD_MV 
goto :EOF

:CMD_RM
goto :EOF