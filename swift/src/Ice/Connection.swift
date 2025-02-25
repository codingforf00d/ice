// Copyright (c) ZeroC, Inc.

import Foundation

/// The batch compression option when flushing queued batch requests.
public enum CompressBatch: UInt8 {
    /// Yes Compress the batch requests.
    case Yes = 0
    /// No Don't compress the batch requests.
    case No = 1
    /// BasedOnProxy Compress the batch requests if at least one request was made on a compressed proxy.
    case BasedOnProxy = 2
    public init() {
        self = .Yes
    }
}

/// An `Ice.InputStream` extension to read `CompressBatch` enumerated values from the stream.
extension InputStream {
    /// Read an enumerated value.
    ///
    /// - returns: `CompressBatch` - The enumerated value.
    public func read() throws -> CompressBatch {
        let rawValue: UInt8 = try read(enumMaxValue: 2)
        guard let val = CompressBatch(rawValue: rawValue) else {
            throw MarshalException("invalid enum value")
        }
        return val
    }

    /// Read an optional enumerated value from the stream.
    ///
    /// - parameter tag: `Int32` - The numeric tag associated with the value.
    ///
    /// - returns: `CompressBatch` - The enumerated value.
    public func read(tag: Int32) throws -> CompressBatch? {
        guard try readOptional(tag: tag, expectedFormat: .Size) else {
            return nil
        }
        return try read() as CompressBatch
    }
}

/// An `Ice.OutputStream` extension to write `CompressBatch` enumerated values to the stream.
extension OutputStream {
    /// Writes an enumerated value to the stream.
    ///
    /// parameter _: `CompressBatch` - The enumerator to write.
    public func write(_ v: CompressBatch) {
        write(enum: v.rawValue, maxValue: 2)
    }

    /// Writes an optional enumerated value to the stream.
    ///
    /// parameter tag: `Int32` - The numeric tag associated with the value.
    ///
    /// parameter _: `CompressBatch` - The enumerator to write.
    public func write(tag: Int32, value: CompressBatch?) {
        guard let v = value else {
            return
        }
        write(tag: tag, val: v.rawValue, maxValue: 2)
    }
}

/// Determines the behavior when manually closing a connection.
public enum ConnectionClose: UInt8 {
    /// Forcefully Close the connection immediately without sending a close connection protocol message to the peer
    /// and waiting for the peer to acknowledge it.
    case Forcefully = 0
    /// Gracefully Close the connection by notifying the peer but do not wait for pending outgoing invocations to
    /// complete. On the server side, the connection will not be closed until all incoming invocations have completed.
    case Gracefully = 1
    /// GracefullyWithWait Wait for all pending invocations to complete before closing the connection.
    case GracefullyWithWait = 2
    public init() {
        self = .Forcefully
    }
}

/// An `Ice.InputStream` extension to read `ConnectionClose` enumerated values from the stream.
extension InputStream {
    /// Read an enumerated value.
    ///
    /// - returns: `ConnectionClose` - The enumerated value.
    public func read() throws -> ConnectionClose {
        let rawValue: UInt8 = try read(enumMaxValue: 2)
        guard let val = ConnectionClose(rawValue: rawValue) else {
            throw MarshalException("invalid enum value")
        }
        return val
    }

    /// Read an optional enumerated value from the stream.
    ///
    /// - parameter tag: `Int32` - The numeric tag associated with the value.
    ///
    /// - returns: `ConnectionClose` - The enumerated value.
    public func read(tag: Int32) throws -> ConnectionClose? {
        guard try readOptional(tag: tag, expectedFormat: .Size) else {
            return nil
        }
        return try read() as ConnectionClose
    }
}

/// An `Ice.OutputStream` extension to write `ConnectionClose` enumerated values to the stream.
extension OutputStream {
    /// Writes an enumerated value to the stream.
    ///
    /// parameter _: `ConnectionClose` - The enumerator to write.
    public func write(_ v: ConnectionClose) {
        write(enum: v.rawValue, maxValue: 2)
    }

    /// Writes an optional enumerated value to the stream.
    ///
    /// parameter tag: `Int32` - The numeric tag associated with the value.
    ///
    /// parameter _: `ConnectionClose` - The enumerator to write.
    public func write(tag: Int32, value: ConnectionClose?) {
        guard let v = value else {
            return
        }
        write(tag: tag, val: v.rawValue, maxValue: 2)
    }
}

/// A collection of HTTP headers.
public typealias HeaderDict = [String: String]

/// Base class providing access to the connection details.
public protocol ConnectionInfo: AnyObject {
    /// The information of the underlying transport or null if there's no underlying transport.
    var underlying: ConnectionInfo? { get set }
    /// Whether or not the connection is an incoming or outgoing connection.
    var incoming: Bool { get set }
    /// The name of the adapter associated with the connection.
    var adapterName: String { get set }
    /// The connection id.
    var connectionId: String { get set }
}

/// An application can implement this interface to receive notifications when a connection closes.
///
/// This method is called by the connection when the connection is closed. If the callback needs more information
/// about the closure, it can call Connection.throwException.
///
/// - parameter _: `Connection?` The connection that closed.
public typealias CloseCallback = (Connection?) -> Void

/// The user-level interface to a connection.
public protocol Connection: AnyObject, CustomStringConvertible {
    /// Manually close the connection using the specified closure mode.
    ///
    /// - parameter _: `ConnectionClose` Determines how the connection will be closed.
    func close(_ mode: ConnectionClose) throws

    /// Create a special proxy that always uses this connection. This can be used for callbacks from a server to a
    /// client if the server cannot directly establish a connection to the client, for example because of firewalls. In
    /// this case, the server would create a proxy using an already established connection from the client.
    ///
    /// - parameter _: `Identity` The identity for which a proxy is to be created.
    ///
    /// - returns: `ObjectPrx` - A proxy that matches the given identity and uses this connection.
    func createProxy(_ id: Identity) throws -> ObjectPrx

    /// Explicitly set an object adapter that dispatches requests that are received over this connection. A client can
    /// invoke an operation on a server using a proxy, and then set an object adapter for the outgoing connection that
    /// is used by the proxy in order to receive callbacks. This is useful if the server cannot establish a connection
    /// back to the client, for example because of firewalls.
    ///
    /// - parameter _: `ObjectAdapter?` The object adapter that should be used by this connection to dispatch requests.
    /// The object adapter must be activated. When the object adapter is deactivated, it is automatically removed from
    /// the connection. Attempts to use a deactivated object adapter raise ObjectAdapterDeactivatedException
    func setAdapter(_ adapter: ObjectAdapter?) throws

    /// Get the object adapter that dispatches requests for this connection.
    ///
    /// - returns: `ObjectAdapter?` - The object adapter that dispatches requests for the connection, or null if no
    /// adapter is set.
    func getAdapter() -> ObjectAdapter?

    /// Get the endpoint from which the connection was created.
    ///
    /// - returns: `Endpoint` - The endpoint from which the connection was created.
    func getEndpoint() -> Endpoint

    /// Flush any pending batch requests for this connection. This means all batch requests invoked on fixed proxies
    /// associated with the connection.
    ///
    /// - parameter _: `CompressBatch` Specifies whether or not the queued batch requests should be compressed before
    /// being sent over the wire.
    func flushBatchRequests(
        _ compress: CompressBatch
    ) async throws

    /// Set a close callback on the connection. The callback is called by the connection when it's closed. The callback
    /// is called from the Ice thread pool associated with the connection. If the callback needs more information about
    /// the closure, it can call Connection.throwException.
    ///
    /// - parameter _: `CloseCallback?` The close callback object.
    func setCloseCallback(_ callback: CloseCallback?) throws

    /// Return the connection type. This corresponds to the endpoint type, i.e., "tcp", "udp", etc.
    ///
    /// - returns: `String` - The type of the connection.
    func type() -> String

    /// Return a description of the connection as human readable text, suitable for logging or error messages.
    ///
    /// - returns: `String` - The description of the connection as human readable text.
    func toString() -> String

    /// Returns the connection information.
    ///
    /// - returns: `ConnectionInfo` - The connection information.
    func getInfo() throws -> ConnectionInfo

    /// Set the connection buffer receive/send size.
    ///
    /// - parameter rcvSize: `Int32` The connection receive buffer size.
    ///
    /// - parameter sndSize: `Int32` The connection send buffer size.
    func setBufferSize(rcvSize: Int32, sndSize: Int32) throws

    /// Throw an exception indicating the reason for connection closure. For example,
    /// CloseConnectionException is raised if the connection was closed gracefully by the peer.
    /// This operation does nothing if the connection is not yet closed.
    func throwException() throws
}

/// Provides access to the connection details of an IP connection
public protocol IPConnectionInfo: ConnectionInfo {
    /// The local address.
    var localAddress: String { get set }
    /// The local port.
    var localPort: Int32 { get set }
    /// The remote address.
    var remoteAddress: String { get set }
    /// The remote port.
    var remotePort: Int32 { get set }
}

/// Provides access to the connection details of a TCP connection
public protocol TCPConnectionInfo: IPConnectionInfo {
    /// The connection buffer receive size.
    var rcvSize: Int32 { get set }
    /// The connection buffer send size.
    var sndSize: Int32 { get set }
}

/// Provides access to the connection details of a UDP connection
public protocol UDPConnectionInfo: IPConnectionInfo {
    /// The multicast address.
    var mcastAddress: String { get set }
    /// The multicast port.
    var mcastPort: Int32 { get set }
    /// The connection buffer receive size.
    var rcvSize: Int32 { get set }
    /// The connection buffer send size.
    var sndSize: Int32 { get set }
}

/// Provides access to the connection details of a WebSocket connection
public protocol WSConnectionInfo: ConnectionInfo {
    /// The headers from the HTTP upgrade request.
    var headers: HeaderDict { get set }
}
