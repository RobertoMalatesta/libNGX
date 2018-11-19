//===- Core.h - Declare and export all Core classes ---*- C++ -*-===//
//
//                     The NGX Server Infrastructure
//
// This file is distributed under the MIT Open Source License. See LICENSE.TXT
// for detail.
//
//===-------------------------------------------------------------------------===//
//
//  This file contains all exported classes` declaration
//
//===-------------------------------------------------------------------------===//

#include "ngx.h"
#include "Core/Arch/Arch.h"
#include "Core/MetaClass/MetaClass.h"
#include "Core/DataStructure/DataStructure.h"
#include "Core/BasicComponent/BasicComponent.h"

namespace ngx {
    namespace Core {

        using namespace MetaClass;
        using namespace DefaultConfig;
        using namespace DataStructure;
        using namespace BasicComponent;
        using namespace Arch::X86_64;


        class FSEntity;

        class FSTree;

        class EPollEventDomain;
    }
}

#include "Core/EPollEventDomain.h"
