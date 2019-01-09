//===- Collector.h - Collector Reusable Item for Fast Reuse --------*- C++ -*-===//
//
//                     The NGX Server Infrastructure
//
// This file is distributed under the MIT Open Source License. See LICENSE.TXT
// for detail.
//
//===-------------------------------------------------------------------------===//
//
//  Collector build and collect reusable item to provide fast reuse
//
//===-------------------------------------------------------------------------===//

/**
 *  @name Collector
 *
 *  @brief helps to collect unused Reusable item and provide fast reuse
 *
 * */
class Collector : public Achor {

protected:

    Queue CollectorSentinel;
    uint32_t CollectorSize;

public:

    Collector(uint32_t CollectorSize) : CollectorSentinel(), CollectorSize(CollectorSize) {
    }

    /** Set collector size */
    virtual RuntimeError SetCollectorSize(uint32_t Size) {
        CollectorSize = Size;
        return {0};
    }

    /** Get collector size */
    uint32_t GetCollectorSize() const {
        return CollectorSize;
    };

    /** Get a item from collector */
    virtual Reusable *Get() { return nullptr; };

    /** Put a item to collector */
    virtual void Put(Reusable *Item) {};
};
