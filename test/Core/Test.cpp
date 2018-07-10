
extern int MemBlockTest();
extern int PoolTest();
extern int BufTest();
extern int ArrayTest();
extern int TimeTest();

int main() {

	MemBlockTest();
    PoolTest();
	BufTest();
	ArrayTest();
	TimeTest();
	return 0;
}
