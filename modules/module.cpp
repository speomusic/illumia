#include "illumia/module-instance.h"

namespace illumia
{
    constinit Steinberg::IPluginFactory2* moduleInstance{};
}

#if defined(WINDOWS)
#define INIT InitDLL
#define EXIT ExitDLL
#elif defined(MAC)
#define INIT bundleEntry
#define EXIT bundleExit
#endif

extern "C"
{
    SMTG_EXPORT_SYMBOL Steinberg::IPluginFactory* PLUGIN_API GetPluginFactory()
    {
        return illumia::moduleInstance;
    }

    SMTG_EXPORT_SYMBOL bool PLUGIN_API INIT()
    {
        return true;
    }

    SMTG_EXPORT_SYMBOL void PLUGIN_API EXIT()
    {
    }
}
