extern int MemBlockTest();

extern int PoolTest();

extern int LockTest();

extern int BufTest();

extern int ArrayTest();

extern int TimeTest();

extern int BufferTest();

extern int ThreadPoolTest();

extern int HashTest();

extern int CRCTest();

extern int RBTreeTest();

extern int RBTree2Test();

extern int TimerTreeTest();

extern int FSTreeTest();

extern int TCPSocketTest();

extern int EPollEventDomainTest();

int CoreTest() {

//	ArrayTest();
//	MemBlockTest();
//  PoolTest();
//  BufferTest();
//	BufTest();
//	ArrayTest();
//	TimeTest();
//	ThreadPoolTest();
//	LockTest();
//	HashTest();
//  CRCTest();
//  RBTreeTest();
    RBTree2Test();
//  TimerTreeTest();
//	FSTreeTest();
//	TCPSocketTest();
//  EPollEventDomainTest();
    return 0;
}