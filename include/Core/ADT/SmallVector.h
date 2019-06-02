#ifndef LIBNGX_SMALLVECTOR_H
#define LIBNGX_SMALLVECTOR_H

namespace ngx {
namespace Core {
namespace Support {

template <class T, int InplaceSize>
class SmallVector {
protected:
    static const unsigned alignmet = ((sizeof(T) + 7)<<3)>>3;
    void *BeginX;
    unsigned Size, Capacity;
    alignas(alignmet) T Eles [InplaceSize];
    inline T *getFirstEl() { return &Eles[0]; }

public:
    using size_type = size_t;
    using value_type = T;
    using iterator = T *;
    using const_iterator = const T *;
    using reverse_iterator = std::reverse_iterator<iterator >;
    using const_reverse_iterator = std::reverse_iterator<const_iterator >;
    using reference = T &;
    using const_reference = const T &;
    using pointer = T *;
    using const_pointer = const T *;

    SmallVector() : Size(0), Capacity(0), BeginX(&Eles[0]){}
    ~SmallVector() {
        // destroy range
    }
    explicit SmallVector(size_t Size, const T &Value = T()) {
        // this -> assign(Size, Value)
    }
    SmallVector(const SmallVector &RHS)  {
        // if (!RHS.empty())
        //      this -> operatpr = (RHS);
    }
    SmallVector(const SmallVector &&RHS)  {
        // if (!RHS.empty())
        //      this -> operatpr = (::std::move(RHS)));
    }
    inline size_t size() const { return Size; }
    inline void set_size(size_type Size) const { this->Size = Size; }
    inline size_t capacity() const { return Capacity; }
    [[nodiscard]] bool empty() const { return !Size; }
    void grow();
    void grow_pod(void *FirstEl, size_t MinCapacity, size_t Size);
    inline bool is_small() const {return BeginX == getFirstEl(); }
    void reset_to_small();
    const SmallVector &operator=(SmallVector &RHS);
    const SmallVector &operator=(SmallVector &&RHS);
    iterator begin() { return (iterator)this->BeginX; }
    const_iterator begin() const { return (const_iterator)this->BeginX; }
    iterator end() { return begin() + size(); }
    const_iterator end() const { return begin() + size(); }
    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
    size_type size_in_bytes() const { return sizeof(T) * size(); }
    size_type max_size() const { return size_t(-1)/ sizeof(T); }
    size_t capacity_in_bytes() const { return capacity() * sizeof(T); }
    pointer data() { return pointer(begin()); }
    const_pointer data() const { return const_pointer(begin()); }
    reference operator[](size_type idx) {
        return reinterpret_cast<reference >( idx < size()? begin()[idx] : nullptr);
    }
    const_reference operator[](size_type idx) const {
        return reinterpret_cast<const_reference >(idx < size()? begin()[idx] : nullptr);
    }
    reference front() {
        return reinterpret_cast<reference >(empty()? begin()[0] : nullptr);
    }
    const_reference front() const {
        return reinterpret_cast<const_reference >(empty()? begin()[0] : nullptr);
    }
    reference back() {
        return reinterpret_cast<reference >(empty()? end()[-1] : nullptr);
    }
    const_reference back() const {
        return reinterpret_cast<const_reference >(empty()? end()[-1] : nullptr);
    }
    void push_back(const T &Ele) {
        if(unlikely(size() > capacity()))
            grow();
        ::new ((void *) end()) T(Ele);
        set_size(size()+1);
    }
    void push_back(const T &&Ele) {
        if(unlikely(size() > capacity()))
            grow();
        ::new ((void *) end()) T(std::move(Ele));
        set_size(size()+1);
    }
    void pop_back() {
        set_size(size()-1);
        end()->~T();
    }
    static void destroy_range(T *S, T *E) {
        while(S != E) {
            --E;
            E->~T();
        }
    }
};

} // Support
} // Core
} // ngx

#endif //LIBNGX_SMALLVECTOR_H
