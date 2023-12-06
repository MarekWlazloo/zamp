#ifndef ABSTRACTCOMCHANNEL_HH
#define ABSTRACTCOMCHANNEL_HH

/*!
 * \file 
 * \brief Zawiera definicję klasy abstrakcyjnej AbstractComChannel
 *
 *  Zawiera definicję klasy abstrakcyjnej AbstractComChannel.
 *  Wyznacza ona niezbędny interfejs klas pochodnych.
 */

#define PORT 6217

// #define DEBUG

#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <mutex>
#include "AbstractScene.hh"

   /*!
    * \brief Definiuje interfejs kanału komunikacyjnego z serwerem graficznym.
    *
    * Definiuje interfejs kanału komunikacyjnego z serwerem graficznym.
    * Interfejs ma pozwalać na bezpieczną komunikację w programie wielowątkowym.
    */
    class AbstractComChannel {
  std::mutex comBlock;
  int& socket;

public:
  AbstractComChannel(int& Socket) : socket(Socket) {}

  virtual ~AbstractComChannel() {}

    virtual void Init(int& Socket) {
        socket = Socket;
    }

    virtual int& GetSocket() const {
        return socket;
    }

  virtual void LockAccess() {
    comBlock.lock();
  }

  virtual void UnlockAccess() {
    comBlock.unlock();
  }

  virtual std::mutex &UseGuard() {
    return comBlock;
  }
};
    bool send(int Sk2Server, const char *msg)
    {
        ssize_t sent_count;
        ssize_t to_send = (ssize_t)strlen(msg);
        while ((sent_count = write(Sk2Server, msg, to_send)) > 0)
        {
            to_send -=sent_count;
            msg += sent_count;
        }
        if (sent_count < 0)
        {
            std::cerr << "ERROR sending" << std::endl;
            return false;
        }
        return true;
    }

    bool openConnection(int &Sk2Server)
    {
        struct sockaddr_in serverAdress;
        bzero((char *)&serverAdress, sizeof(serverAdress));
        serverAdress.sin_family=AF_INET;
        serverAdress.sin_addr.s_addr=inet_addr("127.0.0.1");
        serverAdress.sin_port=htons(PORT);

        Sk2Server = socket(AF_INET,SOCK_STREAM,0);

        if(Sk2Server<0)
        {
            std::cerr << "Cant open socket" << std::endl;
            return false;
        }

        if(connect(Sk2Server,(struct sockaddr*)&serverAdress,sizeof(serverAdress))<0)
        {
            std::cerr<<"Cant establish connection"<<std::endl;
            return false;
        }
        return true;
    }


#endif
