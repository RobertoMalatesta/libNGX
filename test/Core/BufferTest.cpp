#include "Core/Core.h"
#include <cstdio>
#include <cstring>
#include <gtest/gtest.h>

using namespace std;
using namespace ngx::Core::Support;

int BufferTest1();
int BufferTest2();

int BufferTests() {
    BufferTest2();
    return 0;
}

int BufferTest1() {

    u_char c;
    Buffer buffer;
    BufferBuilder Builder(BUFFER_MEMORY_BLOCK_SIZE, 1000);
    BoundCursor Cursor;
    u_char Text[] = "This is a very long text!";

    Builder.BuildBuffer(buffer);

    buffer.ReadBytes(Text, sizeof(Text) - 1);
    buffer >> Cursor;

    while (!!Cursor) {
        printf("%c", *Cursor);
        Cursor += 1;
    }
    printf("\n");

    return 0;
}

int BufferTest2() {

    std::unique_ptr<WritableMemoryBuffer> pBuf = WritableMemoryBuffer::NewBuffer(1000,true);

    BufferWriter Writer(*pBuf);

    const char HelloWorld []= "HelloWorld";

    Writer.fromString(HelloWorld, sizeof(HelloWorld)-1);
    Writer.fromByte(' ');
    Writer.fromString(HelloWorld, sizeof(HelloWorld)-1);
    return 0;
}

TEST(MemoryBuffer, defaultConstructTets) {
    const char HelloWorld []= "HelloWorld\n";
    const char Comment []= "this is a simple test to make sure nullptr BufferStart can be processed properly\n";

    WritableMemoryBuffer Buffer;
    BufferWriter Writer(Buffer);

    Writer.fromString(HelloWorld, sizeof(HelloWorld)-1);
    Writer.fromByte('-');
    Writer.fromString(Comment, sizeof(Comment)-1);
    Writer.toFile(1);
}