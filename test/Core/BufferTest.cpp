#include "Core/Core.h"

#include <string.h>
#include <stdio.h>

using namespace ngx::Core;

int BufferTest() {

    u_char c;
    Buffer buffer;
    BufferBuilder Builder(4096, 1000);
    BoundCursor Cursor;
    u_char Text[] = "This is a very long text!";

    Builder.BuildBuffer(buffer);

    buffer.WriteDataToBuffer(Text, sizeof(Text) - 1);
    buffer >> Cursor;

    while (buffer.ReadByte(Cursor, 0, c)) {
        printf("%c", c);
        Cursor += 1;
    }
    printf("\n");

    return 0;
}
