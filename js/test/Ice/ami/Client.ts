//
// Copyright (c) ZeroC, Inc. All rights reserved.
//

import { Ice } from "ice";
import { Test } from "./Test.js";
import { TestHelper } from "../../Common/TestHelper.js";

const test = TestHelper.test;

export class Client extends TestHelper {
    async allTests() {
        const communicator = this.communicator();
        const out = this.getWriter();

        let sref = "test:" + this.getTestEndpoint();
        let obj = communicator.stringToProxy(sref);
        test(obj !== null);
        const p = Test.TestIntfPrx.uncheckedCast(obj);

        sref = "testController:" + this.getTestEndpoint(1);
        obj = communicator.stringToProxy(sref);
        test(obj !== null);
        const testController = Test.TestIntfControllerPrx.uncheckedCast(obj);

        out.write("testing batch requests with proxy... ");
        {
            const count = await p.opBatchCount();
            test(count === 0);
            const b1 = p.ice_batchOneway();
            const bf = b1.opBatch();
            test(bf.isCompleted());
            test(!bf.isSent());
            b1.opBatch();
            await b1.ice_flushBatchRequests();
            await p.waitForBatch(2);
            await b1.ice_flushBatchRequests();
        }
        out.writeLine("ok");

        out.write("testing batch requests with connection... ");
        {
            test((await p.opBatchCount()) === 0);
            const b1 = p.ice_batchOneway();
            await b1.opBatch();
            const bf = b1.opBatch();
            test(bf.isCompleted());
            await b1.ice_flushBatchRequests();
            test(await p.waitForBatch(2));
        }

        if ((await p.ice_getConnection()) !== null) {
            test((await p.opBatchCount()) == 0);
            const b1 = p.ice_batchOneway();
            await b1.opBatch();
            await b1.ice_getConnection().then((conn) => conn.close(Ice.ConnectionClose.GracefullyWithWait));
            await b1.ice_flushBatchRequests();
            test(await p.waitForBatch(1));
        }
        out.writeLine("ok");

        out.write("testing batch requests with communicator... ");
        {
            //
            // Async task - 1 connection.
            //
            test((await p.opBatchCount()) === 0);
            const b1 = Test.TestIntfPrx.uncheckedCast(
                await p.ice_getConnection().then((c) => c.createProxy(p.ice_getIdentity()).ice_batchOneway()),
            );
            await b1.opBatch();
            await b1.opBatch();

            await communicator.flushBatchRequests();
            test(await p.waitForBatch(2));
        }

        {
            //
            // Async task exception - 1 connection.
            //
            test((await p.opBatchCount()) === 0);
            const b1 = Test.TestIntfPrx.uncheckedCast(
                await p.ice_getConnection().then((c) => c.createProxy(p.ice_getIdentity()).ice_batchOneway()),
            );
            await b1.opBatch();
            await b1.ice_getConnection().then((c) => c.close(Ice.ConnectionClose.GracefullyWithWait));

            await communicator.flushBatchRequests();
            test((await p.opBatchCount()) == 0);
        }

        {
            //
            // Async task - 2 connections.
            //
            test((await p.opBatchCount()) === 0);
            const b1 = Test.TestIntfPrx.uncheckedCast(
                await p.ice_getConnection().then((c) => c.createProxy(p.ice_getIdentity()).ice_batchOneway()),
            );
            const b2 = Test.TestIntfPrx.uncheckedCast(
                await p
                    .ice_connectionId("2")
                    .ice_getConnection()
                    .then((c) => c.createProxy(p.ice_getIdentity()).ice_batchOneway()),
            );

            await b2.ice_getConnection(); // Ensure connection is established.
            await b1.opBatch();
            await b1.opBatch();
            await b2.opBatch();
            await b2.opBatch();

            await communicator.flushBatchRequests();
            test(await p.waitForBatch(4));
        }

        {
            //
            // AsyncResult exception - 2 connections - 1 failure.
            //
            // All connections should be flushed even if there are failures on some connections.
            // Exceptions should not be reported.
            //
            test((await p.opBatchCount()) === 0);
            const b1 = Test.TestIntfPrx.uncheckedCast(
                await p.ice_getConnection().then((c) => c.createProxy(p.ice_getIdentity()).ice_batchOneway()),
            );
            const b2 = Test.TestIntfPrx.uncheckedCast(
                await p
                    .ice_connectionId("2")
                    .ice_getConnection()
                    .then((c) => c.createProxy(p.ice_getIdentity()).ice_batchOneway()),
            );
            await b2.ice_getConnection(); // Ensure connection is established.
            await b1.opBatch();
            await b2.opBatch();
            await b1.ice_getConnection().then((c) => c.close(Ice.ConnectionClose.GracefullyWithWait));

            await communicator.flushBatchRequests();
            test(await p.waitForBatch(1));
        }

        {
            //
            // Async task exception - 2 connections - 2 failures.
            //
            // The sent callback should be invoked even if all connections fail.
            //
            test((await p.opBatchCount()) == 0);
            const b1 = Test.TestIntfPrx.uncheckedCast(
                await p.ice_getConnection().then((c) => c.createProxy(p.ice_getIdentity()).ice_batchOneway()),
            );
            const b2 = Test.TestIntfPrx.uncheckedCast(
                await p
                    .ice_connectionId("2")
                    .ice_getConnection()
                    .then((c) => c.createProxy(p.ice_getIdentity()).ice_batchOneway()),
            );
            await b2.ice_getConnection(); // Ensure connection is established.
            await b1.opBatch();
            await b2.opBatch();
            await b1.ice_getConnection().then((c) => c.close(Ice.ConnectionClose.GracefullyWithWait));
            await b2.ice_getConnection().then((c) => c.close(Ice.ConnectionClose.GracefullyWithWait));

            await communicator.flushBatchRequests();
            test((await p.opBatchCount()) === 0);
        }
        out.writeLine("ok");

        out.write("testing AsyncResult operations... ");
        {
            let r1;
            let r2;
            if (!TestHelper.isSafari()) {
                // Safari WebSocket implementation accepts lots of data before apply back-pressure
                // making this test very slow.
                await testController.holdAdapter();
                try {
                    r1 = p.op();
                    const seq = new Uint8Array(100000);
                    while (true) {
                        r2 = p.opWithPayload(seq);
                        if (r2.sentSynchronously()) {
                            await Ice.Promise.delay(0);
                        } else {
                            break;
                        }
                    }

                    if ((await p.ice_getConnection()) !== null) {
                        test(
                            (r1.sentSynchronously() && r1.isSent() && !r1.isCompleted()) ||
                                (!r1.sentSynchronously() && !r1.isCompleted()),
                        );

                        test(!r2.sentSynchronously() && !r2.isCompleted());

                        test(!r1.isCompleted());
                        test(!r2.isCompleted());
                    }
                } finally {
                    await testController.resumeAdapter();
                }

                await r1;
                test(r1.isSent());
                test(r1.isCompleted());

                await r2;
                test(r2.isSent());
                test(r2.isCompleted());

                test(r1.operation == "op");
                test(r2.operation == "opWithPayload");
            }

            {
                let r: Ice.AsyncResult<void> = p.ice_ping();
                test(r.operation === "ice_ping");
                test(r.communicator == communicator);
                test(r.proxy == p);
                await r;
            }

            //
            // Oneway
            //
            {
                let p2 = p.ice_oneway();
                let r: Ice.AsyncResult<void> = p2.ice_ping();
                test(r.operation === "ice_ping");
                test(r.communicator == communicator);
                test(r.proxy == p2);
                await r;

                //
                // Batch request via proxy
                //
                p2 = p.ice_batchOneway();
                p2.ice_ping();
                await p2.ice_flushBatchRequests();
            }

            {
                const con: Ice.Connection = p.ice_getCachedConnection();
                let p2 = p.ice_batchOneway();
                p2.ice_ping();
                await con.flushBatchRequests();

                p2 = p.ice_batchOneway();
                p2.ice_ping();
                await communicator.flushBatchRequests();
            }
        }

        if (!TestHelper.isSafari()) {
            // Safari WebSocket implementation accepts lots of data before apply backpressure
            // making this test very slow.
            await testController.holdAdapter();
            const seq = new Uint8Array(new Array(100000));
            let r;
            while (true) {
                r = p.opWithPayload(seq);
                if (r.sentSynchronously()) {
                    await Ice.Promise.delay(0);
                } else {
                    break;
                }
            }

            test(!r.isSent());

            const r1 = p.ice_ping();
            r1.then(
                () => test(false),
                (ex) => test(ex instanceof Ice.InvocationCanceledException),
            );

            const r2 = p.ice_id();
            r2.then(
                () => test(false),
                (ex) => test(ex instanceof Ice.InvocationCanceledException),
            );

            r1.cancel();
            r2.cancel();

            await testController.resumeAdapter();
            await r;

            test(!r1.isSent() && r1.isCompleted());
            test(!r2.isSent() && r2.isCompleted());
        }

        {
            await testController.holdAdapter();

            const r1 = p.op();
            const r2 = p.ice_id();

            await p.ice_oneway().ice_ping();

            r1.cancel();
            const r1Result = r1.then(
                () => test(false),
                (ex) => test(ex instanceof Ice.InvocationCanceledException),
            );

            r2.cancel();
            const r2Result = r2.then(
                () => test(false),
                (ex) => test(ex instanceof Ice.InvocationCanceledException),
            );

            await testController.resumeAdapter();

            await Promise.all([r1Result, r2Result]);
        }
        out.writeLine("ok");

        await p.shutdown();
    }

    async run(args: string[]) {
        let communicator: Ice.Communicator | null = null;
        try {
            [communicator, args] = this.initialize(args);
            await this.allTests();
        } finally {
            if (communicator) {
                await communicator.destroy();
            }
        }
    }
}
