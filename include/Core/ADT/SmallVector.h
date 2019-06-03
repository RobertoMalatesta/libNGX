#include "config_header.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <memory>
#ifndef LIBNGX_SMALLVECTOR_H
#define LIBNGX_SMALLVECTOR_H

namespace ngx {
namespace Core {
namespace ADT {

template <typename T, int InplaceSize=32>
class SmallVector {
protected:
    static const unsigned alignmet = 8;
    void *BeginX;
    unsigned Size, Capacity;
    alignas(alignmet) T Eles [InplaceSize];
    void *getFirstEl() { return &Eles[0]; }
    void grow_pod(void *FirstEl, size_t MinCapacity, size_t TSize) {
        if (MinCapacity > UINT32_MAX) return;
        size_t NewCapacity = 2 * capacity() + 1;
        NewCapacity = std::min(std::max(NewCapacity, MinCapacity), size_t(UINT32_MAX));
        void *NewEles;
        if (BeginX==FirstEl) {
            NewEles = malloc(NewCapacity * TSize);
            if (!NewEles) return;
            // Copy the elements over.  No need to run dtors on PODs.
            ::memcpy(NewEles, BeginX, size() * TSize);
        } else {
            NewEles = realloc(BeginX, NewCapacity *TSize);
            if(!NewEles) return;
        }
        BeginX = NewEles;
        Capacity = NewCapacity;
    }

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

    SmallVector() : Size(0), Capacity(InplaceSize), BeginX(&Eles[0]){}
    ~SmallVector() {
        destroy_range(begin(), end());
        if (!is_small()) free(begin());
    }
    explicit SmallVector(size_t Size, const T &Value = T()) {
        assign(Size, Value);
    }
    SmallVector(const SmallVector &RHS)  {
         if (!RHS.empty())
             this -> operatpr = (RHS);
    }
    SmallVector(const SmallVector &&RHS)  {
         if (!RHS.empty())
              this -> operatpr = (::std::move(RHS));
    }
    inline size_t size() const { return Size; }
    inline void set_size(size_type Size) { this->Size = Size; }
    inline size_t capacity() const { return Capacity; }
    [[nodiscard]] bool empty() const { return !Size; }
    inline bool is_small() {return BeginX == this->getFirstEl(); }
    void reset_to_small() {
        BeginX=getFirstEl();
        Size = 0, Capacity = InplaceSize;
    }
    void grow(size_t MinSize=0) { grow_pod(getFirstEl(), MinSize, sizeof(T)); }
    const SmallVector &operator=(SmallVector &RHS) {
        if (this == &RHS) return *this;
        size_t RHSSize = RHS.size();
        size_t CurSize = size();
        if (CurSize >= RHSSize) {
            iterator NewEnd;
            if (RHSSize)
                NewEnd = std::copy(RHS.begin(), RHS.begin()+RHSSize, begin());
            else
                NewEnd = begin();
            destroy_range(NewEnd, end());
            set_size(RHSSize);
            return *this;
        }
        if (capacity() < RHSSize) {
            destroy_range(begin(), end());
            set_size(0);
            CurSize = 0;
            grow(RHSSize);
        } else if (CurSize) {
            std::copy(RHS.begin(), RHS.begin()+CurSize, begin());
        }
        uninitialized_copy(RHS.begin()+CurSize, RHS.end(), begin()+CurSize);
        set_size(RHSSize);
        return *this;
    }
    const SmallVector &operator=(SmallVector &&RHS) {
        if (this == &RHS) return *this;
        if (!RHS.is_small()) {
            destroy_range(begin(), end());
            if (!is_small()) free(begin());
            BeginX = RHS.BeginX;
            Size = RHS.Size;
            Capacity = RHS.Capacity;
            RHS.reset_to_small();
            return *this;
        }
        size_t RHSSize = RHS.size();
        size_t CurSize = size();
        if (CurSize > RHSSize) {
            iterator NewEnd = begin();
            if(RHSSize)
                NewEnd = std::move(RHS.begin(), RHS.end(), NewEnd);
            destroy_range(NewEnd, end());
            set_size(RHSSize);
            RHS.clear();
            return *this;
        }
        if (capacity() < RHSSize) {
            destroy_range(begin(), end());
            set_size(0);
            CurSize = 0;
            grow(RHSSize);
        } else if (CurSize) {
            std::move(RHS.begin(), RHS.end(), begin());
        }
        uninitialized_move(RHS.begin()+CurSize, RHS.end(), begin()+CurSize);
        set_size(RHSSize);
        RHS.clear();
        return *this;
    }
    iterator begin() { return (iterator)BeginX; }
    const_iterator begin() const { return (const_iterator)BeginX; }
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
    void clear() {
        destroy_range(begin(), end());
        Size = 0;
    }
    void resize(size_t N) {
        if (N<size()) {
            destroy_range(begin()+N, end());
            set_size(N);
        } else if (N>size()){
            if (capacity() < N) grow(N);
            for (auto I = end(), E = begin()+N; I!=E; ++I)
                new(&*I)T();
            set_size(N);
        }
    }
    void resize(size_t N, const T &NV) {
        if (N<size()) {
            destroy_range(begin()+N, end());
            set_size(N);
        } else if (N>size()){
            if (capacity() < N) grow(N);
            std::uninitialized_fill(end(), begin()+N, NV);
            set_size(N);
        }
    }
    void reserve(size_type N) {
        if (capacity() < N)
            grow(N);
    }
    void append(size_type NumInputs, const T &Elt) {
        if (NumInputs+size() > capacity())
            grow(NumInputs+size());
        std::uninitialized_fill_n(end(), NumInputs, Elt);
        set_size(NumInputs+size());
    }
    void assign(size_type NumElts, const T &Elt) {
        clear();
        if (capacity() < NumElts)
            grow(NumElts);
        set_size(NumElts);
        std::uninitialized_fill(begin(), end(), Elt);
    }
    static void destroy_range(T *S, T *E) {
        while(S != E) {
            --E;
            E->~T();
        }
    }
    static void uninitialized_move(const_iterator I, const_iterator E, pointer Dest) {
        std::uninitialized_copy(std::make_move_iterator(I), std::make_move_iterator(E), Dest);
    }
    static void uninitialized_copy(const_iterator I, const_iterator E, pointer Dest) {
        std::uninitialized_copy(I, E, Dest);
    }
};

} // Support
} // Core
} // ngx

#endif //LIBNGX_SMALLVECTOR_H
