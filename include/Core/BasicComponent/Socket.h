#ifndef SOCKET_H
#define SOCKET_H
namespace ngx {
    namespace Core {
        namespace BasicComponent {

            typedef union Address Address_t;
            union Address {
                struct sockaddr Addr4;
                struct sockaddr_in Addr4In;
                struct sockaddr_in6 Addr6In;

                Address() = default;

                explicit Address(struct sockaddr &Addr4): Addr4(Addr4) {}
                explicit Address(struct sockaddr_in &Addr4In): Addr4In(Addr4In) {}
                explicit Address(struct sockaddr_in6 &Addr6In): Addr6In(Addr6In) {}
                explicit Address(const char *HostName) { getHostByName(HostName, Addr4); }

                inline u_char addrFamily() const { return Addr4.sa_family; }
                inline bool isBad() const { return addrFamily() == AF_UNSPEC; }
                inline bool isIPv4() const { return addrFamily() == AF_INET; }
                inline bool isIPv6() const { return addrFamily() == AF_INET6; }
                inline bool isUnix() const { return addrFamily() == AF_UNIX; }

                static void getHostByName(const char *&HostName, struct sockaddr & Address) {

                    struct hostent *Ent= nullptr;

                    Ent=gethostbyname2(HostName, AF_INET);

                    if( Ent ==nullptr || Ent->h_length == 0) {
                        Address.sa_family = AF_UNSPEC;
                        return;
                    }

                    Address.sa_family = Ent->h_addrtype;
                    memcpy(Address.sa_data, Ent->h_addr_list[0], sizeof(Address.sa_data));
                }
            };

            class Socket : public EventEntity, public Achor {
            protected:
                int SocketFD = -1;

                SpinLock SocketLock;

                Address_t Address;

                Timer TimerNode;

                friend class SocketEventDomain;

            public:

                Socket():SocketFD(), Address() {};

                Socket(Address_t &Address);

                Socket(int SocketFD, Address_t &Address);

                inline int GetSocketFD() const {
                    return SocketFD;
                }

                inline void Lock() {
                    SocketLock.Lock();
                }

                inline void Unlock() {
                    SocketLock.Unlock();
                }

                inline bool TryLock() {
                    return SocketLock.TryLock();
                }

                SocketError SetSocketAddress(int SocketFD, Address_t &Address);

                SocketError SetNonBlock(bool On);

                SocketError SetNoDelay(bool On);

                static inline Socket *TimerToSocket(Timer *T) {
                    if (T == nullptr) {
                        return nullptr;
                    }
                    return (Socket * )((uintptr_t) T - (uintptr_t) (&((Socket *) 0)->TimerNode));
                }

                virtual RuntimeError HandleDomainEvent(EventType Type) = 0;

                virtual SocketError Close();
            };
        }   // BasicComponent
    }   // Core
}   // ngx

#endif