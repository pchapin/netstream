/*!
 * \file connection.hpp
 * \author Peter Chapin
 * \brief Interface to the connection classes.
 */

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "environ.hpp"
#include <string>

#if eOPSYS == eWIN32
#include <windows.h>
#endif

namespace netstream {

    //! Base class for connection types.
    /*!
     * Class Connection is an abstract base class from which various network connection types
     * can be defined. It supports the basic operations of read and write. At the moment it does
     * not offer much in the way of error handling.
     */
    class Connection {
    public:
        virtual ~Connection() { }

        //! Attempts to read the specified number of bytes from the connection.
        /*!
         * This method blocks until some data is available. It then fills the given buffer with
         * the data. This method might return before the buffer is entirely filled.
         *
         * \param buffer The location where data is to be stored.
         * \param count The number of bytes in the buffer.
         * \return The number of bytes read or zero on end-of-file.
         */
        virtual int read(char *buffer, int count) = 0;

        //! Writes the specified number of bytes into the connection.
        /*!
         * This method writes the given data into the connection. It might block if there is
         * insufficient buffer space in the kernel or if the network protocol requires blocking.
         *
         * \param buffer The location of the data to be written.
         * \param count The number of bytes to write.
         * \return The number of bytes written. A value other than count implies that an error
         * has occurred.
         */
        virtual int write(const char *buffer, int count) = 0;

        //! Return the connection's error status.
        /*!
         * \return True if the connection is operational; false if an error of some kind has
         * occurred.
         */
        virtual bool isOk( ) = 0;

        //! Get description of the latest failure.
        /*!
         * \return A human readable string describing the reason for the last error condition.
         */
        std::string get_failure( )
            { return( failure_reason ); }

    protected:
        std::string failure_reason;  //!< Human readable reason for last failure.
    };


    //! A class for TCP clients.
    class TCP_Connection : public Connection {
    public:
        //! Construct a TCP connection with a remote host.
        /*!
         * The constructor establishes a connection with a remote host.
         *
         * \param host The name of the remote host or the address in the form of a string. For
         * example "morning.ecet.vtc.edu" or "155.42.13.22" are both acceptable.
         * \param port The TCP port on the remote host as a number.
         */
        TCP_Connection( const char *host, unsigned short port );

        //! Construct a TCP connection using an existing open socket.
        /*!
         * This constructor can be used to create a TCP_Connection object from a previously
         * established connection. After this constructor returns the TCP_Connection object
         * "owns" the connection. Do not close the original copy of the socket handle.
         *
         * \param existing_connection The socket handle of a previously established connection.
         */
        #if eOPSYS == ePOSIX
        TCP_Connection( int existing_connection )
            : connection_handle( existing_connection ), is_open( true ) { }
        #elif eOPSYS == eWIN32
        TCP_Connection( SOCKET existing_connection )
            : connection_handle( existing_connection ), is_open( true ) { }
        #endif

        //! Destroys the connection.
        /*!
         * If the connection is open, it is closed.
         */
       ~TCP_Connection();

        virtual int read(char *buffer, int count);
        virtual int write(const char *buffer, int count);

        virtual bool isOk( )
            { return( is_open ); }

    private:
        #if eOPSYS == ePOSIX
        int connection_handle;
        #elif eOPSYS == eWIN32
        SOCKET connection_handle;
        #endif
        bool is_open;

        // Disable copying.
        TCP_Connection( const TCP_Connection & );
        TCP_Connection &operator=( const TCP_Connection & );
    };


    //! Base class for server sockets.
    /*!
     * Class Server is an abstract base class representing server listening endpoints.
     */
    class Server {
    public:
        virtual ~Server( ) { }

        //! Accept a connection.
        /*!
         * This method blocks until a client makes a connection. At that point it dynamically
         * allocates a Connection object of the appropriate concrete type and returns it. The
         * caller is responsible for deleting the Connection object at the appropriate time.
         */
        virtual Connection *accept( ) = 0;

        //! Return the server's error status.
        /*!
         * \return True if the server endpoint is operational; false if an error of some kind
         * has occurred.
         */
        virtual bool isOk( ) = 0;

        //! Get description of the latest failure.
        /*!
         * \return A human readable string describing the reason for the last error condition.
         */
        std::string get_failure( )
            { return( failure_reason ); }

    protected:
        std::string failure_reason;  //!< Human readable reason for last failure.
     };


    //! A class for TCP servers.
    class TCP_Server : public Server {
    public:
        //! Initialize a TCP server endpoint.
        /*!
         * The constructor initializes a TCP server by creating an appropriate socket and then
         * binding a port address to that socket, etc. This class only supports servers that
         * listen to all available interfaces.
         *
         * \param port The port on which the server will listen.
         */
        TCP_Server( unsigned short port );

        //! Destroys the server endpoint.
        /*!
         * Causes the server to stop listening and returns all allocated resources to the
         * kernel.
         */
       ~TCP_Server( );

        virtual TCP_Connection *accept( );

        virtual bool isOk( )
            { return( is_listening ); }

    private:
        #if eOPSYS == ePOSIX
        int listen_handle;
        #elif eOPSYS == eWIN32
        SOCKET listen_handle;
        #endif
        bool is_listening;

        // Disable copying.
        TCP_Server( const TCP_Server & );
        TCP_Server &operator=( const TCP_Server & );
    };

}

#endif
