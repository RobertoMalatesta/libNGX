namespace ngx::Core {

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
};