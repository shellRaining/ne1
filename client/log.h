typedef enum _logType { CLIENT, SERVER, OTHER_CLIENT, ERROR, DEBUG } LogType;

void logMessage(LogType t, char* msg, ...);
