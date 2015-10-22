#ifndef __LINUXSTUFF_H__
#define __LINUXSTUFF_H__

#if defined __unix__ && !(defined __APPLE__)
#define PLATFORM_LINUX 1

struct cmdlineSettingsStruct
{
    bool languageSet;
    unsigned int languageID;
    bool fullscreenSet;
    bool userFullScreen;
    bool aaSet;
    int antiAlias;
    bool debugModeSet;
    bool debugMode;
    bool listLanguages;
};

void printCmdlineUsage();
bool parseCmdlineParameters(int argc, char *argv[]);

#else

#undef PLATFORM_LINUX

#endif

#endif
