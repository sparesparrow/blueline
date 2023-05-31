
### `Multicast`

- The socket options for multicast TTL and loopback are set to control the behavior of multicast packets.
- The socket is bound to any available IPv4 address and the specified multicast port.
- The socket joins the multicast group using the joinMulticastGroup function.
- Audio data is converted into a QByteArray stream using QDataStream.
- Audio stream is then sent to the multicast group using the writeDatagram function of the socket. 

