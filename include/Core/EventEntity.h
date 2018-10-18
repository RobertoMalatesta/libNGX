namespace ngx::Core {

    class EventEntity {
        protected:
            PromiseCallback  *OnEventPromise = DiscardPromise;
    };

    union UserArgument {
        void *Ptr;
        unsigned int UInt;
    };

    struct EventPromiseArgs {
        UserArgument UserArguments[12];
    };
}