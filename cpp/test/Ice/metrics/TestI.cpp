//
// Copyright (c) ZeroC, Inc. All rights reserved.
//

#include "Ice/Ice.h"
#include "TestI.h"

using namespace std;
using namespace Ice;
using namespace Test;

void
MetricsI::op(const Current&)
{
}

void
MetricsI::fail(const Current& current)
{
    current.con->close(ConnectionClose::Forcefully);
}

void
MetricsI::opWithUserException(const Current&)
{
    throw UserEx();
}

void
MetricsI::opWithRequestFailedException(const Current&)
{
    throw ObjectNotExistException(__FILE__, __LINE__);
}

void
MetricsI::opWithLocalException(const Current&)
{
    throw SyscallException(__FILE__, __LINE__);
}

void
MetricsI::opWithUnknownException(const Current&)
{
    throw "TEST";
}

void
MetricsI::opByteS(ByteSeq, const Current&)
{
}

optional<ObjectPrx>
MetricsI::getAdmin(const Current& current)
{
    return current.adapter->getCommunicator()->getAdmin();
}

void
MetricsI::shutdown(const Current& current)
{
    current.adapter->getCommunicator()->shutdown();
}

ControllerI::ControllerI(const ObjectAdapterPtr& adapter) : _adapter(adapter) {}

void
ControllerI::hold(const Current&)
{
    _adapter->hold();
    _adapter->waitForHold();
}

void
ControllerI::resume(const Current&)
{
    _adapter->activate();
}
