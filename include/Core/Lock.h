namespace ngx::Core {

    using namespace std;

    class SpinLock {
        protected:
            atomic_flag LockAtomic = ATOMIC_FLAG_INIT;
        public:
        void Lock();
        void Unlock();
    };
};