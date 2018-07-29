
extern int MemBlockTest();
extern int PoolTest();
extern int BufTest();
extern int ArrayTest();
extern int TimeTest();
extern int ThreadPoolTest();
extern int HashTest();
extern int CRCTest();

int main() {

	MemBlockTest();
    PoolTest();
	BufTest();
	ArrayTest();
//	TimeTest();
	ThreadPoolTest();
	HashTest();
    CRCTest();
	return 0;
}
