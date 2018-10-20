namespace ngx::Core {

    struct BufferCursor {

        BufferMemoryBlock *Block;
        u_char *Position;

        void GetReference ();
        void PutReference ();
    };

    struct BufferRange {

        size_t RangeSize;
        BufferCursor LeftBound;
        BufferCursor RightBound;

        void GetReference ();
        void PutReference ();
    };

    class Buffer : public Resetable {
        protected:
            SpinLock Lock;
            size_t BlockSize = 0;
            BufferMemoryBlockRecycler *Recycler = nullptr;
            BufferCursor ReadCursor, WriteCursor;
            BufferMemoryBlock *HeadBlock = nullptr;
            friend class BufferBuilder;
            Buffer(BufferMemoryBlockRecycler *Recycler, size_t BlockSize = BUFFER_DEFAULT_BLOCK_SIZE);
        public:
            Buffer(Buffer &Copy);
            ~Buffer();
            RuntimeError WriteDataToBuffer(u_char *PointerToData, size_t DataLength);
            RuntimeError WriteConnectionToBuffer(Connection *C);

            inline BufferCursor GetReadCursor() { return ReadCursor; }

            inline BufferCursor MoveCursor(BufferCursor Cursor, uint32_t Count=1) {

                BufferCursor TempCursor = Cursor;
                SpinlockGuard LockGuard(&Lock);

                while (Count > 0) {

                    if (TempCursor.Position + Count > TempCursor.Block->End) {
                        if (TempCursor.Block == WriteCursor.Block) {
                            TempCursor.Block = nullptr;
                            TempCursor.Position = nullptr;
                            break;
                        }
                        else {
                            Count -= TempCursor.Block->End - TempCursor.Position;
                            TempCursor.Block = TempCursor.Block->GetNextBlock();
                            TempCursor.Position = TempCursor.Block->Start;
                        }
                    }
                    else {
                        if (TempCursor.Block == WriteCursor.Block &&
                            (TempCursor.Position + Count) >= WriteCursor.Position) {
                            TempCursor.Block = nullptr;
                            TempCursor.Position = nullptr;
                            break;
                        }
                        TempCursor.Position += Count;
                        Count = 0;
                    }
                }

                return TempCursor;
            }

            inline bool ReadByte(BufferCursor Cursor, uint32_t Offset, u_char &C1) {

                BufferCursor TempCursor = MoveCursor(Cursor, Offset);

                if (TempCursor.Position == nullptr) {
                    return false;
                }

                C1 = *TempCursor.Position;
                return true;
            }

            inline bool CmpByte(BufferCursor Cursor, uint32_t Offset, u_char C1) {

                u_char A1;

                return ReadByte(Cursor, Offset, A1) && A1 == C1;
            }

            inline bool ReadBytes2(BufferCursor Cursor, uint32_t Offset, u_char &C1, u_char &C2) {

                BufferCursor Cur1, Cur2;

                Cur2 = MoveCursor(Cursor, Offset + 1);

                if (Cur2.Position == nullptr) {
                    return false;
                } else {

                    C2 = *(Cur2.Position);

                    if (Cursor.Block == Cur2.Block) {
                        C1 = *(Cur2.Position - 1);
                    }
                    else {
                        return ReadByte(Cursor, Offset, C1);
                    }
                    return true;
                }
            }

            inline bool CmpByte2(BufferCursor Cursor, uint32_t Offset, u_char C1, u_char C2) {

                u_char A1, A2;

                return ReadBytes2(Cursor, Offset, A1, A2)
                        && A1 == C1
                        && A2 == C2;
            }

            inline bool ReadBytes4(BufferCursor Cursor, uint32_t Offset, u_char &C1, u_char &C2, u_char &C3, u_char &C4) {

                BufferCursor Cur1, Cur4;

                Cur4 = MoveCursor(Cursor, Offset + 3);

                if (Cur4.Position == nullptr) {
                    return false;
                }
                else {

                    C4 = *(Cur1.Position+3);

                    if (Cursor.Block == Cur4.Block) {
                        C1 = *Cur1.Position;
                        C2 = *(Cur1.Position+1);
                        C3 = *(Cur1.Position+2);
                    }
                    else {
                        return ReadBytes2(Cursor, Offset, C1, C2) && ReadByte(Cursor, Offset + 2, C3);
                    }
                }
                return true;
            }

            inline bool CmpByte4(BufferCursor Cursor, uint32_t Offset, u_char C1, u_char C2, u_char C3, u_char C4) {

                u_char A1, A2, A3, A4;

                return ReadBytes4(Cursor, Offset, A1, A2, A3, A4)
                        && A1 == C1
                        && A2 == C2
                        && A3 == C3
                        && A4 == C4;
            }

            inline bool HasBytes(uint32_t Count=1) {
                return MoveCursor(ReadCursor, 1).Position == nullptr;
            }

            virtual void Reset();
            virtual void GC();
    };
}