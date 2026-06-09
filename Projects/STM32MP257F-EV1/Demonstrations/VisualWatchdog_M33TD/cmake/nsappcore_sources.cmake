# Group all sources from Utility/M33TD_NSAppCore (except main.c/syscalls.c)
set(NSAPPCORE_SRC_FILES
    ${BASE_DIR}/../../../../Utilities/M33TD_NSAppCore/App/Src/btn_monitor_task.c
    ${BASE_DIR}/../../../../Utilities/M33TD_NSAppCore/App/Src/display_task.c
    ${BASE_DIR}/../../../../Utilities/M33TD_NSAppCore/App/Src/logger_task.c
    ${BASE_DIR}/../../../../Utilities/M33TD_NSAppCore/App/Src/nscoreapp_init.c
    ${BASE_DIR}/../../../../Utilities/M33TD_NSAppCore/App/Src/openamp_task.c
    ${BASE_DIR}/../../../../Utilities/M33TD_NSAppCore/App/Src/remoteproc_task.c
    ${BASE_DIR}/../../../../Utilities/M33TD_NSAppCore/App/Src/scmi_mgr_task.c
    ${BASE_DIR}/../../../../Utilities/M33TD_NSAppCore/App/Src/userapp_task.c
    ${BASE_DIR}/../../../../Utilities/M33TD_NSAppCore/App/Src/wdg_monitor_task.c
    ${BASE_DIR}/../../../../Utilities/M33TD_NSAppCore/Common/FaultMgr/backtrace.c
    ${BASE_DIR}/../../../../Utilities/M33TD_NSAppCore/Common/FaultMgr/fault_manager.c
)