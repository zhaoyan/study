#include "Socket.h"
#include "InetAddress.h"

#include <assert.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

Socket::Socket(int sockfd)
  : sockfd_(sockfd)
{
  assert(sockfd_ >= 0);
}

Socket::~Socket()
{
  if (sockfd_ >= 0)
  {
    int ret = ::close(sockfd_);
    assert(ret == 0); (void)ret;
  }
}

void Socket::bindOrDie(const InetAddress& addr)
{
  int ret = ::bind(sockfd_, addr.get_sockaddr(), addr.length());
  if (ret)
  {
    perror("Socket::bindOrDie");
    abort();
  }
}

void Socket::listenOrDie()
{
  int ret = ::listen(sockfd_, SOMAXCONN);
  if (ret)
  {
    perror("Socket::listen");
    abort();
  }
}

int Socket::connect(const InetAddress& addr)
{
  return ::connect(sockfd_, addr.get_sockaddr(), addr.length());
}

void Socket::shutdownWrite()
{
  if (::shutdown(sockfd_, SHUT_WR) < 0)
  {
    perror("Socket::shutdownWrite");
  }
}

void Socket::setReuseAddr(bool on)
{
  int optval = on ? 1 : 0;
  if (::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR,
                   &optval, sizeof optval) < 0)
  {
    perror("Socket::setReuseAddr");
  }
}

void Socket::setTcpNoDelay(bool on)
{
  int optval = on ? 1 : 0;
  if (::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY,
               &optval, static_cast<socklen_t>(sizeof optval)) < 0)
  {
    perror("Socket::setTcpNoDelay");
  }
}

InetAddress Socket::getLocalAddr() const
{
  struct sockaddr_storage localaddr;
  socklen_t addrlen = sizeof localaddr;
  struct sockaddr* addr = reinterpret_cast<struct sockaddr*>(&localaddr);
  if (::getsockname(sockfd_, addr, &addrlen) < 0)
  {
    perror("Socket::getLocalAddr");
  }
  return InetAddress(*addr);
}

InetAddress Socket::getPeerAddr() const
{
  struct sockaddr_storage peeraddr;
  socklen_t addrlen = sizeof peeraddr;
  struct sockaddr* addr = reinterpret_cast<struct sockaddr*>(&peeraddr);
  if (::getpeername(sockfd_, addr, &addrlen) < 0)
  {
    perror("Socket::getPeerAddr");
  }
  return InetAddress(*addr);
}

int Socket::recv(void* buf, int len)
{
#ifdef TEMP_FAILURE_RETRY
  return TEMP_FAILURE_RETRY(::recv(sockfd_, buf, len, 0));
#else
  return ::recv(sockfd_, buf, len, 0);
#endif
}

int Socket::send(const void* buf, int len)
{
#ifdef TEMP_FAILURE_RETRY
  return TEMP_FAILURE_RETRY(::send(sockfd_, buf, len, 0));
#else
  return ::send(sockfd_, buf, len, 0);
#endif
}

Socket Socket::createTCP(sa_family_t family)
{
  int sockfd = ::socket(family, SOCK_STREAM | SOCK_CLOEXEC, IPPROTO_TCP);
  assert(sockfd >= 0);
  return Socket(sockfd);
}

Socket Socket::createUDP(sa_family_t family)
{
  int sockfd = ::socket(family, SOCK_DGRAM | SOCK_CLOEXEC, IPPROTO_UDP);
  assert(sockfd >= 0);
  return Socket(sockfd);
}

