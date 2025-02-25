//
// Copyright (c) ZeroC, Inc. All rights reserved.
//

declare module "ice" {
    namespace Ice {
        /**
         * Base class providing access to the connection details.
         */
        class ConnectionInfo {
            /**
             * Constructs a new `ConnectionInfo` object.
             *
             * @param underlying - The information of the underlying transport, or `null` if there is no underlying transport.
             * @param incoming - Indicates whether the connection is incoming (`true`) or outgoing (`false`).
             * @param adapterName - The name of the adapter associated with the connection.
             * @param connectionId - The connection ID.
             */
            constructor(
                underlying?: Ice.ConnectionInfo,
                incoming?: boolean,
                adapterName?: string,
                connectionId?: string,
            );

            /**
             * The information of the underlying transport, or `null` if there is no underlying transport.
             */
            underlying: Ice.ConnectionInfo;

            /**
             * Indicates whether the connection is incoming (`true`) or outgoing (`false`).
             */
            incoming: boolean;

            /**
             * The name of the adapter associated with this connection.
             */
            adapterName: string;

            /**
             * The connection id.
             */
            connectionId: string;
        }

        /**
         * Callback invoked when the connection is closed. If additional information about the closure is needed,
         * the callback can call {@link Connection#throwException}.
         *
         * @param connection - The connection that was closed.
         */
        type CloseCallback = (connection: Ice.Connection) => void;

        /**
         * Determines the behavior when manually closing a connection.
         */
        class ConnectionClose extends Ice.EnumBase {
            /**
             * Closes the connection immediately without sending a close connection protocol message to the peer and
             * without waiting for the peer to acknowledge it.
             */
            static readonly Forcefully: ConnectionClose;

            /**
             * Closes the connection by notifying the peer but does not wait for pending outgoing invocations to
             * complete. On the server side, the connection will not be closed until all incoming invocations have
             * completed.
             */
            static readonly Gracefully: ConnectionClose;

            /**
             * Waits for all pending invocations to complete before closing the connection.
             */
            static readonly GracefullyWithWait: ConnectionClose;

            /**
             * Returns the enumerator corresponding to the given value.
             *
             * @param value - The numeric value of the enumerator.
             * @returns The enumerator corresponding to the given value.
             */
            static valueOf(value: number): ConnectionClose;
        }

        /**
         * The user-level interface to a connection.
         */
        interface Connection {
            /**
             * Manually closes the connection using the specified closure mode.
             *
             * @param mode - The mode that determines how the connection will be closed.
             * @returns A promise that resolves when the close operation is complete.
             *
             * @see {@link ConnectionClose}
             */
            close(mode: ConnectionClose): Promise<void>;

            /**
             * Creates a special proxy that always uses this connection. This is useful for callbacks from a server to a
             * client when the server cannot directly establish a connection to the client, such as in cases where firewalls
             * are present. In such scenarios, the server would create a proxy using an already established connection from the client.
             *
             * @param id - The identity for which the proxy is to be created.
             * @returns A proxy that matches the given identity and uses this connection.
             *
             * @see {@link setAdapter}
             */
            createProxy(id: Identity): Ice.ObjectPrx;

            /**
             * Explicitly sets an object adapter that dispatches requests received over this connection. A client can
             * invoke an operation on a server using a proxy and then set an object adapter for the outgoing connection
             * used by the proxy to receive callbacks. This is particularly useful when the server cannot establish a
             * connection back to the client, such as in scenarios involving firewalls.
             *
             * @param adapter - The object adapter that should be used by this connection to dispatch requests. The
             *                  object adapter must be activated. When the object adapter is deactivated, it is
             *                  automatically removed from the connection. Attempts to use a deactivated object adapter
             *                  raise an {@link ObjectAdapterDeactivatedException}.
             *
             * @see {@link createProxy}
             * @see {@link getAdapter}
             */
            setAdapter(adapter: Ice.ObjectAdapter | null): void;

            /**
             * Get the object adapter that dispatches requests for this connection.
             *
             * @returns The object adapter that dispatches requests for the connection, or null if no adapter is set.
             *
             * @see {@link setAdapter}
             */
            getAdapter(): Ice.ObjectAdapter;

            /**
             * Get the endpoint from which the connection was created.
             *
             * @returns The endpoint from which the connection was created.
             */
            getEndpoint(): Ice.Endpoint;

            /**
             * Flush any pending batch requests for this connection. This means all batch requests invoked on fixed proxies
             * associated with the connection.
             * @returns The asynchronous result object for the invocation.
             */
            flushBatchRequests(): Promise<void>;

            /**
             * Sets a close callback on the connection. The callback is invoked by the connection when it is closed.
             * If the callback needs more information about the closure, it can call {@link Connection#throwException}.
             *
             * @param callback - The close callback object.
             */
            setCloseCallback(callback: Ice.CloseCallback): void;

            /**
             * Returns the connection type, which corresponds to the endpoint type (e.g., "tcp", "udp", etc.).
             *
             * @returns The type of the connection.
             */
            type(): string;

            /**
             * Return a description of the connection as human readable text, suitable for logging or error messages.
             *
             * @returns The description of the connection as human readable text.
             */
            toString(): string;

            /**
             * Retrieves the connection information.
             *
             * @returns The connection information.
             */
            getInfo(): Ice.ConnectionInfo;

            /**
             * Sets the connection buffer sizes for receiving and sending data.
             *
             * @param rcvSize - The size of the receive buffer in bytes.
             * @param sndSize - The size of the send buffer in bytes.
             */
            setBufferSize(rcvSize: number, sndSize: number): void;

            /**
             * Throw an exception indicating the reason for connection closure. For example,
             * {@link CloseConnectionException} is raised if the connection was closed gracefully, whereas
             * {@link ConnectionAbortedException}/{@link ConnectionClosedException} is raised if the connection was
             * manually closed by the application. This operation does nothing if the connection is not yet closed.
             */
            throwException(): void;
        }

        /**
         * Provides access to the connection details of an IP connection
         */
        class IPConnectionInfo extends ConnectionInfo {
            /**
             * Constructs a new `IPConnectionInfo` object.
             *
             * @param underlying - The information of the underlying transport, or `null` if there is no underlying transport.
             * @param incoming - Indicates whether the connection is incoming (`true`) or outgoing (`false`).
             * @param adapterName - The name of the adapter associated with the connection.
             * @param connectionId - The connection ID.
             * @param localAddress - The local IP address.
             * @param localPort - The local port number.
             * @param remoteAddress - The remote IP address.
             * @param remotePort - The remote port number.
             */
            constructor(
                underlying?: Ice.ConnectionInfo,
                incoming?: boolean,
                adapterName?: string,
                connectionId?: string,
                localAddress?: string,
                localPort?: number,
                remoteAddress?: string,
                remotePort?: number,
            );

            /**
             * The local address.
             */
            localAddress: string;

            /**
             * The local port.
             */
            localPort: number;

            /**
             * The remote address.
             */
            remoteAddress: string;

            /**
             * The remote port.
             */
            remotePort: number;
        }

        /**
         * Provides access to the connection details of a TCP connection
         */
        class TCPConnectionInfo extends IPConnectionInfo {
            /**
             * Constructs a new `TCPConnectionInfo` object.
             *
             * @param underlying - The information of the underlying transport, or `null` if there is no underlying
             *                     transport.
             * @param incoming - Indicates whether the connection is incoming (`true`) or outgoing (`false`).
             * @param adapterName - The name of the adapter associated with the connection.
             * @param connectionId - The connection ID.
             * @param localAddress - The local IP address.
             * @param localPort - The local port number.
             * @param remoteAddress - The remote IP address.
             * @param remotePort - The remote port number.
             * @param rcvSize - The receive buffer size in bytes.
             * @param sndSize - The send buffer size in bytes.
             */
            constructor(
                underlying?: Ice.ConnectionInfo,
                incoming?: boolean,
                adapterName?: string,
                connectionId?: string,
                localAddress?: string,
                localPort?: number,
                remoteAddress?: string,
                remotePort?: number,
                rcvSize?: number,
                sndSize?: number,
            );

            /**
             * The connection buffer receive size.
             */
            rcvSize: number;

            /**
             * The connection buffer send size.
             */
            sndSize: number;
        }

        /**
         * A collection of HTTP headers.
         */
        class HeaderDict extends Map<string, string> {}

        /**
         * Helper class for encoding a {@link HeaderDict} into an `OutputStream` and decoding a {@link HeaderDict} from an
         * `InputStream`.
         */
        class HeaderDictHelper {
            /**
             * Writes the {@link HeaderDict} value to the given `OutputStream`.
             *
             * @param outs - The `OutputStream` to write to.
             * @param value - The `HeaderDict` value to write.
             */
            static write(outs: OutputStream, value: HeaderDict): void;

            /**
             * Reads a {@link HeaderDict} value from the given `InputStream`.
             *
             * @param ins - The `InputStream` to read from.
             * @returns The read {@link HeaderDict} value.
             */
            static read(ins: InputStream): HeaderDict;
        }

        /**
         * Provides access to the connection details of a WebSocket connection.
         */
        class WSConnectionInfo extends ConnectionInfo {
            /**
             * Constructs a new `WSConnectionInfo` object.
             *
             * @param underlying - The information of the underlying transport, or `null` if there is no underlying
             *                     transport.
             * @param incoming - Indicates whether the connection is incoming (`true`) or outgoing (`false`).
             * @param adapterName - The name of the adapter associated with the connection.
             * @param connectionId - The connection ID.
             * @param headers - The headers from the HTTP upgrade request.
             */
            constructor(
                underlying?: Ice.ConnectionInfo,
                incoming?: boolean,
                adapterName?: string,
                connectionId?: string,
                headers?: HeaderDict,
            );

            /**
             * The headers from the HTTP upgrade request.
             */
            headers: HeaderDict;
        }
    }
}
