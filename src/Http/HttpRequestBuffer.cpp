#include "Http/Http.h"

using namespace ngx::Http;

void inline HttpParseRequestLine();
void inline HttpProcessRequestHeader();
void inline HttpValidateHost();
void inline HttpProcessRequestURI();
void inline ResetState();

HttpRequestBuffer::HttpRequestBuffer(BufferMemoryBlockRecycler *R, size_t BlockSize): Buffer(R, BlockSize) {

}

HttpRequestBuffer::~HttpRequestBuffer() {

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

    u_char t_c;
    while ( (t_c =ReadByte()) != 0) {
        printf("%c",t_c);
    }
    printf("\n");

    return RuntimeError(0);
}

void HttpRequestBuffer::ProcessHttpRequest() {
    
    
    
    
    
    
    
    
    
    
}

