namespace ngx {
    namespace Core {


        using namespace std;

        class SpinLock {
        protected:
            atomic_flag LockAtomic = ATOMIC_FLAG_INIT;
        public:
            SpinLock() {
                LockAtomic.clear();
            }

            void Lock();

            void Unlock();
        };

        class BigSpinlock : public SpinLock {
        public:
            BigSpinlock();

            void Lock();
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