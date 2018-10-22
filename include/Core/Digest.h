namespace ngx::Core {

    namespace Md5 {

        constexpr uint32_t F(uint32_t x, uint32_t y, uint32_t z) { return (z) ^ ((x) & ((y) ^ (z))); }

        constexpr uint32_t G(uint32_t x, uint32_t y, uint32_t z) { return (y) ^ ((z) & ((x) ^ (y))); }

        constexpr uint32_t H(uint32_t x, uint32_t y, uint32_t z) { return (x) ^ (y) ^ (z); }

        constexpr uint32_t I(uint32_t x, uint32_t y, uint32_t z) { return (y) ^ ((x) | ~(z)); }

        static uint32_t block[16];

#define STEP(f, a, b, c, d, x, t, s)                                              \
                (a) += f((b), (c), (d)) + (x) + (t);                                      \
                (a) = (((a) << (s)) | (((a) & 0xffffffff) >> (32 - (s))));                \
                (a) += (b)

        constexpr uint32_t SET(uint32_t n, const u_char *p) {

            if (LittleEnding && NonAligned) {
                return (*(uint32_t * ) & p[n * 4]);
            } else {
                return block[n] = ((uint32_t) p[n * 4])
                                  | ((uint32_t) p[n * 4 + 1] << 8)
                                  | ((uint32_t) p[n * 4 + 2] << 16)
                                  | ((uint32_t) p[n * 4 + 3] << 24);
            }
        }

        constexpr uint32_t GET(uint32_t n, const u_char *p) {
            if (LittleEnding && NonAligned) {
                return (*(uint32_t * ) & p[n * 4]);
            } else {
                return block[n];
            }
        };
    };

    class Md5Context {
    private:
        uint64_t bytes = 0;
        uint32_t a = 0x67452301, b = 0xefcdab89, c = 0x98badcfe, d = 0x10325476;
        u_char buffer[64];

        u_char *Md5Body(u_char *Data, size_t Size);

    public:
        Md5Context() = default;

        void Md5Update(u_char *Data, size_t Size);

        void Md5Final(u_char Result[16], size_t Size);
    };
}