namespace ngx{
    namespace Core {

        const bool LittleEnding = true;
        const bool NonAligned = true;
        static size_t PageSize = 4096;
        static uint32_t CacheLineSize = 64;
        const uint64_t TimeResolution = 1000;
        const uint32_t NumTimeSlot = 128;

    }
}