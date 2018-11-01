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

        struct Cursor : public Ref {
            BufferMemoryBlock *Block = nullptr;
            u_char *Position = nullptr;

            Cursor() = default;

            Cursor(BufferMemoryBlock *Block, u_char *Position);

            virtual uint32_t IncRef();

            virtual uint32_t DecRef();

            u_char operator*();

            bool operator~();

            Cursor &operator=(Cursor const &Right);
        };

        struct BoundCursor : public Cursor {
            Cursor Bound;
            BoundCursor operator+=(size_t Size);

            const BoundCursor operator++(int);

            BoundCursor operator+(size_t Size);

            BoundCursor operator-(size_t) = delete;

            BoundCursor &operator--() = delete;

            BoundCursor &operator-=(size_t Size) = delete;

            u_char operator[](uint16_t Offset);

            BoundCursor &operator=(BoundCursor const &Right);

            BoundCursor &SetLeft(Cursor &Cursor);
            BoundCursor &SetRight(Cursor &Bound);
        };

        struct Range {

            size_t RangeSize;
            BoundCursor LeftBound, RightBound;

            void IncRef();

            void DecRef();
        };

        class Buffer : public Resetable {
        protected:
            SpinLock Lock;
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

//            inline BufferCursor MoveCursor(BufferCursor Cursor, uint32_t Count = 1) {
//
//                BufferCursor TempCursor = Cursor;
//                SpinlockGuard LockGuard(&Lock);
//
//                while (Count > 0) {
//
//                    if (TempCursor.Position + Count > TempCursor.Block->End) {
//                        if (TempCursor.Block == WriteCursor.Block) {
//                            TempCursor.Block = nullptr;
//                            TempCursor.Position = nullptr;
//                            break;
//                        } else {
//                            Count -= TempCursor.Block->End - TempCursor.Position;
//                            TempCursor.Block = TempCursor.Block->GetNextBlock();
//                            TempCursor.Position = TempCursor.Block->Start;
//                        }
//                    } else {
//                        if (TempCursor.Block == WriteCursor.Block &&
//                            (TempCursor.Position + Count) >= WriteCursor.Position) {
//                            TempCursor.Block = nullptr;
//                            TempCursor.Position = nullptr;
//                            break;
//                        }
//                        TempCursor.Position += Count;
//                        Count = 0;
//                    }
//                }
//
//                return TempCursor;
//            }

            inline bool ReadByte(BoundCursor Cursor, uint32_t Offset, u_char &C1) {

                BoundCursor TempCursor = (Cursor + Offset);

                if (TempCursor.Position == nullptr) {
                    return false;
                }

                C1 = *TempCursor.Position;
                return true;
            }

            inline bool CmpByte(BoundCursor Cursor, uint32_t Offset, u_char C1) {

                u_char A1;

                return ReadByte(Cursor, Offset, A1) && A1 == C1;
            }

            inline bool ReadBytes2(BoundCursor Cursor, uint32_t Offset, u_char &C1, u_char &C2) {

                BoundCursor Cur1, Cur2 = (Cursor + Offset + 1);

                if (Cur2.Position == nullptr) {
                    return false;
                } else {

                    C2 = *(Cur2.Position);

                    if (Cursor.Block == Cur2.Block) {
                        C1 = *(Cur2.Position - 1);
                    } else {
                        return ReadByte(Cursor, Offset, C1);
                    }
                    return true;
                }
            }

            inline bool CmpByte2(BoundCursor Cursor, uint32_t Offset, u_char C1, u_char C2) {

                u_char A1, A2;

                return ReadBytes2(Cursor, Offset, A1, A2)
                       && A1 == C1
                       && A2 == C2;
            }

            inline bool
            ReadBytes4(BoundCursor Cursor, uint32_t Offset, u_char &C1, u_char &C2, u_char &C3, u_char &C4) {

                BoundCursor Cur4 = (Cursor + Offset + 3);

                if (Cur4.Position == nullptr) {
                    return false;
                } else {

                    C4 = *(Cur4.Position);

                    if (Cursor.Block == Cur4.Block) {
                        C1 = *(Cur4.Position - 3);
                        C2 = *(Cur4.Position - 2);
                        C3 = *(Cur4.Position - 1);
                    } else {
                        return ReadBytes2(Cursor, Offset, C1, C2) && ReadByte(Cursor, Offset + 2, C3);
                    }
                }
                return true;
            }

            inline bool CmpByte4(BoundCursor Cursor, uint32_t Offset, u_char C1, u_char C2, u_char C3, u_char C4) {

                u_char A1, A2, A3, A4;

                return ReadBytes4(Cursor, Offset, A1, A2, A3, A4)
                       && A1 == C1
                       && A2 == C2
                       && A3 == C3
                       && A4 == C4;
            }

            inline bool HasBytes(uint32_t Count = 1) {
                return (ReadCursor + 1).Position == nullptr;
            }

            virtual void Reset();

            virtual void GC();
        };

    }
}