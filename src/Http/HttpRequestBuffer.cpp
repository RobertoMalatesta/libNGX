#include "Http/Http.h"

using namespace ngx::Http;

void inline HttpParseRequestLine();
void inline HttpProcessRequestHeader();
void inline HttpValidateHost();
void inline HttpProcessRequestURI();
void inline ResetState();

HttpRequestBuffer::HttpRequestBuffer(BufferMemoryBlockRecycler *R, size_t BlockSize): Buffer(R, BlockSize) {

}

RuntimeError HttpRequestBuffer::WriteDataToBuffer(HttpConnection *C) {

    int SocketFd = C->GetSocketFD();
    u_char *PointerToData;
    size_t ReadLength;
    ssize_t RecievedSize;
    BufferMemoryBlock *TempBlock;

    while (true) {

        PointerToData = WriteCursor.Position;
        ReadLength = WriteCursor.Block->End - PointerToData;

        if (ReadLength == 0) {

            if (Recycler == nullptr) {
                TempBlock = BufferMemoryBlock::Build(BlockSize);
            }
            else {
                TempBlock = Recycler->Get();
            }

            if (TempBlock == nullptr) {
                return RuntimeError(ENOMEM, "Can not allocate BufferMemoryBlock when recv()");
            }
            WriteCursor.Block->SetNextBlock(TempBlock);
            WriteCursor.Block = TempBlock;
            PointerToData = WriteCursor.Position = WriteCursor.Block->Start;
            ReadLength = WriteCursor.Block->End - PointerToData;
        }

        RecievedSize = recv(SocketFd, PointerToData, ReadLength, 0);

        if (RecievedSize == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            }
            else {
                return RuntimeError(errno, "Failed to read from socket!");
            }
        }
        else if (RecievedSize > 0) {
            WriteCursor.Position += RecievedSize;
        }
        else {
            break;
        }
    }

    return RuntimeError(0);
}

HttpError HttpRequestBuffer::ProcessHttpRequest() {

    Lock.Lock();

    switch (State) {
        case HTTP_INIT_STATE:
            RequestLineState = RL_Start;
            ParseRequestLine();
            break;
        default:
            State = HTTP_INIT_STATE;
            return HttpError(EINVAL);
    }

    Lock.Unlock();
    return HttpError(0);
}

HttpError HttpRequestBuffer::ParseRequestLine() {

    u_char C, C1, C2, C3, C4, C5, C6, C7, C8;
    BufferCursor TempCursor = ReadCursor;

    while (ReadByte(ReadCursor, 0, C)) {
       switch (RequestLineState) {
           case RL_Start:
               if (C == CR || C == LF)
                   break;
               if ((C<'A'|| C>'Z') && C != '_' && C != '-') {
                   return HttpError(EINVAL, "Invalid method!");
               }
               if (ReadBytes4(ReadCursor, 0, C1, C2, C3, C4)) {
                   if (C4 == ' ') {
                       if (C1 == 'G' && C2 == 'E' && C3 == 'T') {
                           Method = GET;
                       }
                       if (C1 == 'P' && C2 == 'U' && C3 == 'T') {
                           Method = PUT;
                       }
                       else {
                           return HttpError(EINVAL, "Invalid method!");
                       }
                       ReadCursor = MoveCursor(ReadCursor, 4);
                   } else if (CmpByte(ReadCursor, 4, ' ')) {
                       if (C2 == 'O') {
                           if (C1 == 'P' && C3 == 'S' && C4 == 'T') {
                               Method = POST;
                           }
                           if (C1 == 'C' && C3 == 'P' && C4 == 'Y') {
                               Method = COPY;
                           }
                           if (C1 == 'M' && C3 == 'V' && C4 == 'E') {
                               Method = MOVE;
                           }
                           if (C1 == 'L' && C3 == 'C' && C4 == 'K') {
                               Method = LOCK;
                           }
                       } else if (C1 == 'H' && C2 == 'E' && C3 == 'A' && C4 == 'D') {
                           Method = HEAD;
                       }
                       else {
                           return HttpError(EINVAL, "Invalid method!");
                       }
                       ReadCursor = MoveCursor(ReadCursor, 5);
                   } else if (CmpByte(ReadCursor, 5, ' ')) {

                       if (C1 == 'M' && C2 == 'K' && C3 == 'C' && C4 == 'O' && CmpByte(ReadCursor, 4, 'L')) {
                           Method = MKCOL;
                       }
                       if (C1 == 'P' && C2 == 'A' && C3 == 'T' && C4 == 'C' && CmpByte(ReadCursor, 4, 'H')) {
                           Method = PATCH;
                       }
                       if (C1 == 'T' && C2 == 'R' && C3 == 'A' && C4 == 'C' && CmpByte(ReadCursor, 4, 'E')) {
                           Method = TRACE;
                       } else {
                           return HttpError(EINVAL, "Invalid method!");
                       }
                       ReadCursor = MoveCursor(ReadCursor, 6);
                   } else if (CmpByte(ReadCursor, 6, ' ')) {
                       if (C1 == 'D' && C2 == 'E' && C3 == 'L' && C4 == 'E' && CmpByte2(ReadCursor, 4, 'T', 'E')) {
                           Method = DELETE;
                       }
                       if (C1 == 'U' && C2 == 'N' && C3 == 'L' && C4 == 'O' && CmpByte2(ReadCursor, 4, 'C', 'K')) {
                           Method = UNLOCK;
                       }
                   } else if(C1 == 'O' && C2 == 'P' && C3 == 'T' && C4 == 'I' && CmpByte4(ReadCursor, 4, 'O', 'N', 'S', ' ')) {
                           Method = OPTIONS;
                   } else if (C1 == 'P' && C2 == 'R' && C3 == 'O' && C4 == 'P' &&
                        CmpByte4(ReadCursor, 4, 'F', 'I', 'N', 'D') && CmpByte(ReadCursor, 8, ' ')) {
                           Method = PROPFIND;
                   } else if (C1 == 'P' && C2 == 'R' && C3 == 'O' && C4 == 'P' &&
                          CmpByte4(ReadCursor, 4, 'P', 'A', 'T', 'C') && CmpByte2(ReadCursor, 8, 'H', ' ')) {
                       Method = PROPPATCH;
                   }
                   else {
                       return HttpError(EINVAL, "Invalid method!");
                   }
                   ReadCursor = MoveCursor(ReadCursor, 7);
               }
               RequestLineState = RL_Space_Before_Uri;
               break;
           default:
               printf("not handled!");
       }
    }
    return HttpError(0);
}

void HttpRequestBuffer::Reset() {
    State = HTTP_INIT_STATE;
    Buffer::Reset();
}

