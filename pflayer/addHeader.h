extern "C" mainControllerStruct();
extern "C" void diskRequest(int fileNum, int pagenum, int requestType);
extern "C" void mainControllerStruct::writeBackCache();
