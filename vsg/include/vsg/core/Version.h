/* <editor-fold desc="MIT License">

Copyright(c) 2018 Robert Osfield

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

</editor-fold> */

#pragma once

#include <vsg/core/Export.h>

#ifdef __cplusplus
extern "C"
{
#endif

    #define VSG_VERSION_MAJOR    0
    #define VSG_VERSION_MINOR    1
    #define VSG_VERSION_PATCH    0
    #define VSG_SOVERSION        1

    #define VSG_VERSION_STRING   "0.1.0"
    #define VSG_SOVERSION_STRING "1"

    #define VSG_MAX_DEVICES 1

    struct VsgVersion
    {
        unsigned int major;
        unsigned int minor;
        unsigned int patch;
        unsigned int soversion;
    };

    extern VSG_DECLSPEC struct VsgVersion vsgGetVersion();
    extern VSG_DECLSPEC const char* vsgGetVersionString();
    extern VSG_DECLSPEC const char* vsgGetSOVersionString();

    /// return 0 if the linked VSG was built as static library (default), 1 if the linked VSG library was built as shared/dynamic library.
    /// When building against a shared libraryTo ensure the correct selection of VSG_DECLSPEC (provided in vsg/core/Export.h) one must compile with the define VSG_SHARED_LIBRARY
    extern VSG_DECLSPEC int vsgBuiltAsSharedLibrary();

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
    inline bool operator < (const VsgVersion& lhs, const VsgVersion& rhs)
    {
        if (lhs.major < rhs.major) return true;
        if (lhs.major > rhs.major) return false;
        if (lhs.minor < rhs.minor) return true;
        if (lhs.minor > rhs.minor) return false;
        if (lhs.patch < rhs.patch) return true;
        if (lhs.patch > rhs.patch) return false;
        return lhs.soversion < rhs.soversion;
    }
#endif
