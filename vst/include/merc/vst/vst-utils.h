#pragma once

#include <pluginterfaces/base/ftypes.h>
#include <stdexcept>

#define CHECK_TRESULT(tr) \
{ \
    Steinberg::tresult rrr{ tr }; \
    if (rrr != Steinberg::kResultOk && \
        rrr != Steinberg::kNotImplemented) throw std::runtime_error{ "Vst operation failed:\n" #tr }; \
}

#define CHECK_TRESULT_IMPLEMENTED(tr) \
{ \
    Steinberg::tresult rrr{ tr }; \
    if (rrr != Steinberg::kResultOk) throw std::runtime_error{ "Vst operation failed:\n" #tr }; \
}
