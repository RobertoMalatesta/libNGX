namespace ngx {
    namespace Core {



        class SpinLock {
        protected:
            std::atomic_flag LockAtomic = ATOMIC_FLAG_INIT;
        public:
            SpinLock() {
                LockAtomic.clear();
            }

            virtual void Lock();

            virtual void Unlock();
        };

        class BigSpinlock : public SpinLock {
        public:
            BigSpinlock();

            virtual void Lock();
        };

        class SpinlockGuard {

        protected:
            SpinLock *Lock;
        public:
            SpinlockGuard(SpinLock *Lock) {
                this->Lock = Lock;
                Lock->Lock();
            }

            ~SpinlockGuard() {
                Lock->Unlock();
            }
        };

    }
};