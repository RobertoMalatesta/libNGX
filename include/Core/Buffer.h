//===- Buffer.h - Buffer to provide block-chained buffer --------*- C++ -*-===//
//
//                     The NGX Server Infrastructure
//
// This file is distributed under the MIT Open Source License. See LICENSE.TXT
// for detail.
//
//===----------------------------------------------------------------------===//
//
//  This file provide Buffer facility to enable block based Buffer
//
//===----------------------------------------------------------------------===//

namespace ngx {
    namespace Core {

        struct Cursor : public Ref, public Achor {
            BufferMemoryBlock *Block = nullptr;
            u_char *Position = nullptr;

            Cursor() = default;

            virtual uint32_t IncRef();

            virtual uint32_t DecRef();

            u_char operator*() {
                return (!*this)? (u_char)'\0': *Position;
            }

            inline bool operator!() {
                return Block == nullptr || Position == nullptr;
            }

            inline Cursor &operator=(Cursor const &Right) {
                Block = Right.Block, Position = Right.Position;
                return *this;
            }
            inline bool operator==(Cursor const &Right) {
                return Block == Right.Block && Position == Right.Position;
            }
        };

        struct BoundCursor : public Cursor {
            Cursor Bound;

            inline BoundCursor operator+(size_t Size) {
                BoundCursor R = *this;

                while (Size > 0) {
                    if ((R.Position + Size) < R.Block->End) {
                        if (R.Block == R.Bound.Block && (R.Position + Size) >= R.Bound.Position) {
                            R.Block = nullptr, R.Position = nullptr;
                            break;
                        }
                        else {
                            R.Position += Size;
                            Size = 0;
                        }
                    } else {
                        Size -= (R.Block->End - R.Position);
                        R.Block = R.Block->GetNextBlock();
                        R.Position = R.Block->Start;
                    }
                }
                return R;
            }

            inline BoundCursor operator+=(size_t Size) {
                BoundCursor R = *this;
                *this = this->operator+(Size);
                return R;
            }

            inline BoundCursor operator++() {
                return *this = this -> operator+(1);
            }

            inline const BoundCursor operator++(int) {
                BoundCursor Ret = *this;
                *this = this->operator+(1);
                return Ret;
            }

            inline bool operator!() {
                return (Block == nullptr && Position == nullptr)
                       || (Block == Bound.Block && Position >= Bound.Position);
            }

            inline u_char operator[](uint16_t Offset) {
                BoundCursor Target = operator+(Offset);
                return *Target;
            }

            inline BoundCursor &operator=(BoundCursor const &Right) {
                Block = Right.Block, Position = Right.Position;
                Bound = Right.Bound;
                return *this;
            }

            inline BoundCursor operator-(size_t) = delete;

            inline BoundCursor &operator--() = delete;

            inline BoundCursor &operator-=(size_t Size) = delete;

            BoundCursor &SetLeft(Cursor &Cursor);

            BoundCursor &SetRight(Cursor &Bound);

            inline bool ReadByte(uint32_t Offset, u_char &C1) {

                BoundCursor TempCursor = (*this + Offset);

                if (TempCursor.Position == nullptr) {
                    return false;
                }

                C1 = *TempCursor.Position;
                return true;
            }

            inline bool CmpByte(uint32_t Offset, u_char C1) {

                u_char A1;
                return this->ReadByte(Offset, A1) && A1 == C1;
            }

            inline bool ReadBytes2(uint32_t Offset, u_char &C1, u_char &C2) {

                BoundCursor Cur1, Cur2 = (*this + (Offset + 1));

                if (Cur2.Position == nullptr) {
                    return false;
                } else {

                    C2 = *(Cur2.Position);

                    if (Block == Cur2.Block) {
                        C1 = *(Cur2.Position - 1);
                    } else {
                        return this->ReadByte(Offset, C1);
                    }
                    return true;
                }
            }

            inline bool CmpByte2(uint32_t Offset, u_char C1, u_char C2) {

                u_char A1, A2;

                return this->ReadBytes2(Offset, A1, A2)
                       && A1 == C1
                       && A2 == C2;
            }

            inline bool ReadBytes4(uint32_t Offset, u_char &C1, u_char &C2, u_char &C3, u_char &C4) {

                BoundCursor Cur4 = (*this + (Offset + 3));

                if (Cur4.Position == nullptr) {
                    return false;
                } else {

                    C4 = *(Cur4.Position);

                    if (Block == Cur4.Block) {
                        C1 = *(Cur4.Position - 3);
                        C2 = *(Cur4.Position - 2);
                        C3 = *(Cur4.Position - 1);
                    } else {
                        return this->ReadBytes2(Offset, C1, C2) && this->ReadByte(Offset + 2, C3);
                    }
                }
                return true;
            }

            inline bool CmpByte4(uint32_t Offset, u_char C1, u_char C2, u_char C3, u_char C4) {

                u_char A1, A2, A3, A4;

                return this->ReadBytes4(Offset, A1, A2, A3, A4)
                       && A1 == C1
                       && A2 == C2
                       && A3 == C3
                       && A4 == C4;
            }

        };

        struct Range {

            size_t RangeSize;
            BoundCursor LeftBound, RightBound;

            void IncRef();

            void DecRef();

            inline BoundCursor GetBoundCurosr() {
                BoundCursor Ret;
                Ret.Block = LeftBound.Block;
                Ret.Position = LeftBound.Position;
                Ret.Bound = RightBound;
                return Ret;
            }
        };

        struct HashRange : Range {
            uint32_t Code;

            uint32_t Hash(uint32_t Code = 0) {
                if (Code != 0) {
                    this->Code = Code;
                }
                return this->Code;
            };

            HashRange() : Range(), Code(0) {}
        };

        class Buffer : public Resetable {
        protected:
            size_t BlockSize = 0;
            BufferMemoryBlockRecycler *Recycler = nullptr;
            BoundCursor ReadCursor;
            Cursor WriteCursor;
            BufferMemoryBlock *HeadBlock = nullptr;


            friend class BufferBuilder;

        public:
            Buffer() = default;

            ~Buffer();

            Buffer &operator<<(BoundCursor &BC) &;

            Buffer &operator>>(BoundCursor &BC) &;

            RuntimeError WriteConnectionToBuffer(Connection *C);

            RuntimeError WriteDataToBuffer(u_char *PointerToData, size_t DataLength);

            inline bool HasBytes(uint32_t Count = 1) {
                return (ReadCursor + 1).Position == nullptr;
            }

            virtual void Reset();

            virtual void GC();
        };

    }
}