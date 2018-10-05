/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#ifndef BINARYDATA_H_51287078_INCLUDED
#define BINARYDATA_H_51287078_INCLUDED

namespace BinaryData
{
    extern const char*   icon_png;
    const int            icon_pngSize = 15333;

    extern const char*   IRboton_png;
    const int            IRboton_pngSize = 866;

    extern const char*   UPB80_jpg;
    const int            UPB80_jpgSize = 92492;

    extern const char*   waves_gif;
    const int            waves_gifSize = 17048;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Number of elements in the namedResourceList array.
    const int namedResourceListSize = 4;

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes) throw();
}

#endif
