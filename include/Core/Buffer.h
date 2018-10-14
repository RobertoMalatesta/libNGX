namespace ngx::Core {

    class BufferMemoryBlock : public MemoryBlock, public Recyclable {
        protected:
            BufferMemoryBlock *NextBlock;
            BufferMemoryBlock(size_t Size);
            ~BufferMemoryBlock();
            friend class Buffer;
        public:
            u_char *Start, *End, *Pos;

            static BufferMemoryBlock *Build(size_t Size);
            static void Destroy(BufferMemoryBlock **PointerToBlock);

            void SetNextBlock(BufferMemoryBlock *Next) { this->NextBlock = Next; }
            BufferMemoryBlock *GetNextBlock() { return NextBlock; }
    };

    struct BufferCursor {
        BufferMemoryBlock *Block;
        u_char *Position;
    };

    struct BufferRange {
       BufferCursor Cursor;
       size_t Length;
    };

    class Buffer : public Resetable {
        protected:
            SpinLock Lock;
            size_t BlockSize = 0;
            BufferMemoryBlockRecycler *Recycler = nullptr;
            BufferCursor ReadCursor, WriteCursor;
            BufferMemoryBlock *HeadBlock = nullptr, *CurrentBlock = nullptr;
        public:
            Buffer(BufferMemoryBlockRecycler *Recycler, size_t BlockSize = BUFFER_DEFAULT_BLOCK_SIZE);
            RuntimeError WriteDataToBuffer(u_char *PointerToData, size_t DataLength);

            inline BufferCursor MoveCursor(BufferCursor Cursor, uint32_t Count=1) {

                BufferCursor TempCursor = Cursor;
                Lock.Lock();

                while (TempCursor.Position + Count >= TempCursor.Block->End) {

                    if (TempCursor.Block == WriteCursor.Block) {
                        Lock.Unlock();
                        TempCursor.Position = nullptr;
                        return TempCursor;
                    }

                    Count -= (TempCursor.Block->End - TempCursor.Position);
                    TempCursor.Block = TempCursor.Block->GetNextBlock();

                    if (nullptr == TempCursor.Block) {
                        Lock.Unlock();
                        TempCursor.Position = nullptr;
                        return TempCursor;
                    }
                    TempCursor.Position = TempCursor.Block->Start;
                }

                TempCursor.Position += Count;

                if (TempCursor.Block == WriteCursor.Block && TempCursor.Position >= WriteCursor.Position) {
                    Lock.Unlock();
                    TempCursor.Position = nullptr;
                    return TempCursor;
                }

                Lock.Unlock();
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
                if (ReadByte(Cursor, Offset, A1)) {
                    return A1 == C1;
                }
                return false;
            }

            inline bool ReadBytes2(BufferCursor Cursor, uint32_t Offset, u_char &C1, u_char &C2) {
                BufferCursor Cur1, Cur2;

                Cur1 = MoveCursor(Cursor, Offset);
                Cur2 = MoveCursor(Cursor, Offset + 1);

                if (Cur2.Position == nullptr || Cur1.Position == nullptr) {
                    return false;
                }

                if (Cur1.Block == Cur2.Block) {
                    C1 = *Cur1.Position;
                    C2 = *(Cur1.Position+1);
                    return true;
                }
                else {
                    return ReadByte(Cur1, 0, C1) && ReadByte(Cur1, 1, C2);
                }
            }

            inline bool CmpByte2(BufferCursor Cursor, uint32_t Offset, u_char C1, u_char C2) {
                u_char A1, A2;
                if (ReadBytes2(Cursor, Offset, A1, A2)) {
                    return A1==C1 && A2 == C2;
                }
                return false;
            }

            inline bool ReadBytes4(BufferCursor Cursor, uint32_t Offset, u_char &C1, u_char &C2, u_char &C3, u_char &C4) {

                BufferCursor Cur1, Cur2;

                Cur1 = MoveCursor(Cursor, Offset);
                Cur2 = MoveCursor(Cursor, Offset + 3);

                if (Cur2.Position == nullptr || Cur1.Position == nullptr) {
                    return false;
                }

                if (Cur1.Block == Cur2.Block) {
                    C1 = *Cur1.Position;
                    C2 = *(Cur1.Position+1);
                    C3 = *(Cur1.Position+2);
                    C4 = *(Cur1.Position+3);
                    return true;
                }
                else {
                    return ReadByte(Cur1, 0, C1) &&
                            ReadByte(Cur1, 1, C2) &&
                            ReadByte(Cur1, 2, C3) &&
                            ReadByte(Cur1, 3, C4);
                }
            }

            inline bool CmpByte4(BufferCursor Cursor, uint32_t Offset, u_char C1, u_char C2, u_char C3, u_char C4) {

                u_char A1, A2, A3, A4;
                if (ReadBytes4(Cursor, Offset, A1, A2, A3, A4)) {
                    return A1==C1 && A2 == C2 && A3 == C3 && A4 == C4;
                }
                return false;
            }

            inline bool HasBytes(uint32_t Count=1) {
                return MoveCursor(ReadCursor, 1).Position == nullptr;
            }

            virtual void Reset() {};
    };

    class BufferMemoryBlockRecycler : public Recycler {
        protected:
            SpinLock Lock;
            size_t  BufferMemoryBlockSize;
        public:
            BufferMemoryBlockRecycler(size_t  BufferMemoryBlockSize, uint64_t RecyclerSize, MemAllocator *Allocator);
            size_t  GetBlockSize() { return this->BufferMemoryBlockSize; }
            BufferMemoryBlock *Get();
            void Put(BufferMemoryBlock *Item);
    };
}