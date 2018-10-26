namespace ngx{
    namespace Core {

        static void DiscardPromise(void *, ThreadPool *) {
            printf("discard promise!");
        }

        class EventEntity {
        protected:
            PromiseCallback *OnEventPromise = DiscardPromise;
        };

        union UserArgument {
            void *Ptr;
            unsigned int UInt;
        };

        struct EventPromiseArgs {
            UserArgument UserArguments[12];
        };
    }
}
