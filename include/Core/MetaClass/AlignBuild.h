//===- AlignBuild.h - Declare Align build template -----------------*- C++ -*-===//
//
//                     The NGX Server Infrastructure
//
// This file is distributed under the MIT Open Source License. See LICENSE.TXT
// for detail.
//
//===-------------------------------------------------------------------------===//
//
//  This file declares AlignBuildTemplate whitch enable build object with
//  page aligned memory
//===-------------------------------------------------------------------------===//

template<class T, size_t DEFAULT_BUILD_SIZE = DefaultConfig::BUILD_DEFAULT_SIZE>
class AlignBuild {
public:
    static int Build(T *&Item, size_t Size = DEFAULT_BUILD_SIZE) {

        void *TempPointer;

        int Code = posix_memalign(&TempPointer, Size, Size);
        Item = nullptr;

        if (Code != 0) {
            return Code;
        }

        Item = new(TempPointer) T(Size);
        return 0;
    };

    static int Destroy(T *&Item) {

        if (nullptr == Item) {
            return 0;
        }

        free((void *) Item);

        Item = nullptr;
        return 0;
    };
};
