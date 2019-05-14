extern int MemBlockTest();

extern int PoolTests();

extern int LockTest();

extern int BufTest();

extern int ArrayTest();

extern int TimeTest();

extern int BufferTests();

extern int HashTest();

extern int CRCTest();

extern int DictionaryTest();

extern int TimerTreeTest();

extern int TCPSocketTest();

extern int EPollEventDomainTest();

int CoreTest() {

//	ArrayTest();
//    MemBlockTest();
    PoolTests();
//    BufferTests();
//	TimeTest();
//	LockTest();
//	HashTest();
//  CRCTest();
//    DictionaryTest();
//  TimerTreeTest();
//	TCPSocketTest();
//  EPollEventDomainTest();
    return 0;
}