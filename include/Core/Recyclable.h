namespace ngx{
    namespace Core {
        class Recyclable : public Resetable, public Queue {
        public:
            Recyclable() : Queue() {};

            virtual void Reset() {};
        };
    }
}

