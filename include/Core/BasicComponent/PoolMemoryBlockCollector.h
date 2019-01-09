//===- BufferBuilder.h - Config Buffer &  create buffer instance ---*- C++ -*-===//
//
//                     The NGX Server Infrastructure
//
// This file is distributed under the MIT Open Source License. See LICENSE.TXT
// for detail.
//
//===-------------------------------------------------------------------------===//
//
//  This file contains BufferBuilder specification to delegate Buffer construction
//  and configuration
//
//===-------------------------------------------------------------------------===//

/**
 *  @name Basic memory block
 *
 *  @brief Basic memory block structure.
 * */
class PoolMemoryBlockCollector : public Collector, MetaClass::CanReset {

};
