#include "merc/global/paths.h"
#import <Foundation/Foundation.h>

namespace merc::global
{
    std::filesystem::path getLibraryDir(NSSearchPathDomainMask domain)
    {
        NSURL* libraryUrl = [[NSFileManager defaultManager] URLForDirectory:NSLibraryDirectory
                                                                   inDomain:domain
                                                          appropriateForURL:nil
                                                                     create:NO
                                                                      error:nil];
        return [[libraryUrl path] UTF8String];
    }

    std::filesystem::path getVstDefaultSystemDir()
    {
        return getLibraryDir(NSLocalDomainMask) / "Audio" / "Plug-Ins" / "VST3";
    }

    std::filesystem::path getVstDefaultUserDir()
    {
        return getLibraryDir(NSUserDomainMask) / "Audio" / "Plug-Ins" / "VST3";
    }
}
