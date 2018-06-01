namespace ngx::Core {

    /*
     * The basic MD5 functions.
     *
     * F and G are optimized compared to their RFC 1321 definitions for
     * architectures that lack an AND-NOT instruction, just like in
     * Colin Plumb's implementation.
     */

    #define F(x, y, z)  ((z) ^ ((x) & ((y) ^ (z))))
    #define G(x, y, z)  ((y) ^ ((z) & ((x) ^ (y))))
    #define H(x, y, z)  ((x) ^ (y) ^ (z))
    #define I(x, y, z)  ((y) ^ ((x) | ~(z)))

    /*
     * The MD5 transformation for all four rounds.
     */

    #define STEP(f, a, b, c, d, x, t, s)                                          \
        (a) += f((b), (c), (d)) + (x) + (t);                                      \
        (a) = (((a) << (s)) | (((a) & 0xffffffff) >> (32 - (s))));                \
        (a) += (b)

    /*
     * SET() reads 4 input bytes in little-endian byte order and stores them
     * in a properly aligned word in host byte order.
     *
     * The check for little-endian architectures that tolerate unaligned
     * memory accesses is just an optimization.  Nothing will break if it
     * does not work.
     */


    #define SET(n)      (*(uint32_t *) &p[n * 4])
    #define GET(n)      (*(uint32_t *) &p[n * 4])

    class Md5Context {
        private:
            uint64_t bytes;
            uint32_t a, b, c, d;
            u_char buffer[64];

            u_char *Md5Body (u_char *Data, size_t Size);
        public:
            Md5Context() {
                a = 0x67452301;
                b = 0xefcdab89;
                c = 0x98badcfe;
                d = 0x10325476;

                bytes = 0;
            }

            void Md5Update (u_char *Data, size_t Size);
            void Md5Final (u_char Result[16], size_t Size);
    };
}