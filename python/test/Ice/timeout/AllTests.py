#
# Copyright (c) ZeroC, Inc. All rights reserved.
#

import Ice
import Test
import sys
import threading


def test(b):
    if not b:
        raise RuntimeError("test assertion failed")


class CallbackBase:
    def __init__(self):
        self._called = False
        self._cond = threading.Condition()

    def called(self):
        with self._cond:
            self._called = True
            self._cond.notify()

    def check(self):
        with self._cond:
            while not self._called:
                self._cond.wait()
            self._called = False
            return True


class Callback(CallbackBase):
    def response(self):
        self.called()

    def exception(self, ex):
        test(False)

    def responseEx(self):
        test(False)

    def exceptionEx(self, ex):
        test(isinstance(ex, Ice.TimeoutException))
        self.called()


def connect(prx):
    # Establish connection with the given proxy (which might have a timeout
    # set and might sporadically fail on connection establishment if it's
    # too slow). The loop ensures that the connection is established by retrying
    # in case we can a ConnectTimeoutException
    nRetry = 10
    while --nRetry > 0:
        try:
            prx.ice_getConnection()
            break
        except Ice.ConnectTimeoutException:
            # Can sporadically occur with slow machines
            pass
    return prx.ice_getConnection()  # Establish connection


def allTests(helper, communicator):
    controller = Test.ControllerPrx.checkedCast(
        communicator.stringToProxy(
            "controller:{0}".format(helper.getTestEndpoint(num=1))
        )
    )
    test(controller is not None)

    try:
        allTestsWithController(helper, communicator, controller)
    except Exception:
        # Ensure the adapter is not in the holding state when an unexpected exception occurs to prevent
        # the test from hanging on exit in case a connection which disables timeouts is still opened.
        controller.resumeAdapter()
        raise


def allTestsWithController(helper, communicator, controller):
    sref = "timeout:{0}".format(helper.getTestEndpoint())
    obj = communicator.stringToProxy(sref)
    test(obj is not None)

    timeout = Test.TimeoutPrx.checkedCast(obj)
    test(timeout is not None)

    sys.stdout.write("testing connection timeout... ")
    sys.stdout.flush()
    #
    # Expect TimeoutException.
    #
    seq = bytes([0 for x in range(0, 10000000)])
    to = Test.TimeoutPrx.uncheckedCast(obj.ice_timeout(250))
    connect(to)
    controller.holdAdapter(-1)
    try:
        to.sendData(seq)
        test(False)
    except Ice.TimeoutException:
        pass  # Expected.
    controller.resumeAdapter()
    timeout.op()  # Ensure adapter is active.
    #
    # Expect success.
    #
    to = Test.TimeoutPrx.uncheckedCast(obj.ice_timeout(2000))
    controller.holdAdapter(100)
    try:
        seq2 = bytes([0 for x in range(0, 1000000)])
        to.sendData(seq2)
    except Ice.TimeoutException:
        test(False)
    print("ok")

    sys.stdout.write("testing invocation timeout... ")
    sys.stdout.flush()
    connection = obj.ice_getConnection()
    to = Test.TimeoutPrx.uncheckedCast(obj.ice_invocationTimeout(100))
    test(connection == to.ice_getConnection())
    try:
        to.sleep(1000)
        test(False)
    except Ice.InvocationTimeoutException:
        pass
    obj.ice_ping()
    to = Test.TimeoutPrx.uncheckedCast(obj.ice_invocationTimeout(1000))
    test(connection == to.ice_getConnection())
    try:
        to.sleep(100)
    except Ice.InvocationTimeoutException:
        test(False)
    test(connection == to.ice_getConnection())
    print("ok")

    controller.shutdown()
