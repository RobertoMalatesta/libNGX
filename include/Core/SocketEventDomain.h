namespace ngx::Core {

    class SocketEventDomain : public EventDomain {
        protected:
            bool IsSocketReadAttached(Socket *S) {

                if (nullptr == S) {
                return false;
                }

                return S->ReadAttach == 1;
            }

            bool IsSocketWriteAttached(Socket *S) {
                if (nullptr == S) {
                    return false;
                }
                return S->WriteAttach == 1;
            }

            void SetSocketReadAttached(Socket *S, int Val) {
                if (nullptr == S) {
                    return;
                }
                S->ReadAttach = (Val == 1)? 1: 0;
                return;
            }

            void SetSocketWriteAttached(Socket *S, int Val) {
                if (nullptr == S) {
                    return;
                }
                S->WriteAttach = (Val == 1)? 1: 0;
                return;
            }
        public:
            SocketEventDomain(size_t PoolSize, int ThreadCount);
            EventError AttachSocket(Socket *S, SocketEventType Type) { return EventError(0);};
            EventError DetachSocket(Socket *S, SocketEventType Type) { return EventError(0);};
    };
}

