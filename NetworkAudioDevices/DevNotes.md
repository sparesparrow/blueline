
### `Multicast`

- The socket options for multicast TTL and loopback are set to control the behavior of multicast packets.
- The socket is bound to any available IPv4 address and the specified multicast port.
- The socket joins the multicast group using the joinMulticastGroup function.
- Audio data is converted into a QByteArray stream using QDataStream.
- Audio stream is then sent to the multicast group using the writeDatagram function of the socket. 



### Synchronization Types


enum class SyncProto {
    NTP,       // Network Time Protocol : This is used to synchronize the clocks of computers over a network. It's not specific to media streams, but it's often used in conjunction with RTP to synchronize the playback of media streams on different devices.
    PTP,       // Precision Time Protocol : This is another protocol for clock synchronization. It's more accurate than NTP and is often used in professional audio and video applications.
    RTSP,      // Real-Time Streaming Protocol : This is a network control protocol designed for use in entertainment and communications systems to control streaming media servers. It's often used in conjunction with RTP.
    SDP,       // Session Description Protocol : This is a format for describing multimedia communication sessions for the purposes of session announcement, session invitation, and other forms of multimedia session initiation.
    SRTP,      // Secure Real-time Transport Protocol : This is a profile of RTP that adds encryption, message authentication and integrity, and replay protection to the RTP data.
    WebRTC,    // WebRTC : This is a free, open-source project that provides web browsers and mobile applications with real-time communication via simple application programming interfaces.
};