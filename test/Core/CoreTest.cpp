extern int MemBlockTest();

extern int PoolTest();

extern int LockTest();

extern int BufTest();

extern int ArrayTest();

extern int TimeTest();

extern int BufferTest();

extern int HashTest();

extern int CRCTest();

extern int DictionaryTest();

extern int TimerTreeTest();

extern int TCPSocketTest();

extern int EPollEventDomainTest();

int CoreTest() {

//	ArrayTest();
    MemBlockTest();
    PoolTest();
    BufferTest();
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