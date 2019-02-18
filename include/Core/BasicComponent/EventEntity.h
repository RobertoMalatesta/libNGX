//===------------------ EventType.h - represent a Event ------------*- C++ -*-===//
//
//                     The NGX Server Infrastructure
//
// This file is distributed under the MIT Open Source License. See LICENSE.TXT
// for detail.
//
//===-------------------------------------------------------------------------===//
//
// Basic event type define
//
//===-------------------------------------------------------------------------===//

typedef uint16_t EventType;

const EventType ET_NONE = 0x0000;
const EventType ET_TIMER = 0x0001;
const EventType ET_ACCEPT = 0x0002;
const EventType ET_READ = 0x0004;
const EventType ET_WRITE = 0x0008;
const EventType ET_CLOSED = 0x0010;

class EventEntity {
protected:
    EventType Event = ET_NONE;
    EventType AttachedEvent = ET_NONE;
    Thread *BackendWorker = nullptr;

    friend class EventDomain;

public:
    RuntimeError PostJob(Job &J);
};
