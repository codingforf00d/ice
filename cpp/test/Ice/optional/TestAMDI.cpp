//
// Copyright (c) ZeroC, Inc. All rights reserved.
//

#include <Ice/Ice.h>
#include <TestAMDI.h>

using namespace Test;
using namespace IceUtil;
using namespace Ice;
using namespace std;

InitialI::InitialI()
{
}

void
InitialI::shutdownAsync(function<void()> response,
                        function<void(exception_ptr)>, const Ice::Current& current)
{
    current.adapter->getCommunicator()->shutdown();
    response();
}

void
InitialI::pingPongAsync(shared_ptr<::Ice::Value> obj,
                        function<void(const shared_ptr<::Ice::Value>&)> response,
                        function<void(exception_ptr)>, const Ice::Current&)
{
    response(obj);
    if(dynamic_pointer_cast<MultiOptional>(obj))
    {
        // Break cyclic reference count
        dynamic_pointer_cast<MultiOptional>(obj)->k = shared_ptr<MultiOptional>();
    }
}

void
InitialI::opOptionalExceptionAsync(optional<int> a, optional<string> b, optional<shared_ptr<Test::OneOptional>> o,
                                   function<void()>,
                                   function<void(exception_ptr)> ex, const Ice::Current&)
{
    ex(make_exception_ptr(OptionalException(false, a, b, o)));
}

void
InitialI::opDerivedExceptionAsync(optional<int> a, optional<string> b, optional<shared_ptr<Test::OneOptional>> o,
                                  function<void()>,
                                  function<void(exception_ptr)> ex, const Ice::Current&)
{
    ex(make_exception_ptr(DerivedException(false, a, b, o, "d1", b, o, "d2")));
}

void
InitialI::opRequiredExceptionAsync(optional<int> a, optional<string> b, optional<shared_ptr<Test::OneOptional>> o,
                                   function<void()>,
                                   function<void(exception_ptr)> ex, const Ice::Current&)
{
    RequiredException e;
    e.a = a;
    e.b = b;
    e.o = o;
    if(b)
    {
        e.ss = b.value();
    }
    if(o)
    {
        e.o2 = o.value();
    }

    ex(make_exception_ptr(e));
}

void
InitialI::opByteAsync(optional<uint8_t> p1,
                      function<void(optional<uint8_t>, optional<uint8_t>)> response,
                      function<void(exception_ptr)>, const Ice::Current&)
{
    response(p1, p1);
}

void
InitialI::opBoolAsync(optional<bool> p1,
                      function<void(optional<bool>, optional<bool>)> response,
                      function<void(exception_ptr)>, const Ice::Current&)
{
    response(p1, p1);
}

void
InitialI::opShortAsync(optional<short> p1,
                            function<void(optional<short>, optional<short>)> response,
                            function<void(exception_ptr)>, const Ice::Current&)
{
    response(p1, p1);
}

void
InitialI::opIntAsync(optional<int> p1,
                     function<void(optional<int>, optional<int>)> response,
                     function<void(exception_ptr)>, const Ice::Current&)
{
    response(p1, p1);
}

void
InitialI::opLongAsync(optional<int64_t> p1,
                           function<void(optional<int64_t>, optional<int64_t>)> response,
                           function<void(exception_ptr)>, const Ice::Current&)
{
    response(p1, p1);
}

void
InitialI::opFloatAsync(optional<float> p1,
                            function<void(optional<float>, optional<float>)> response,
                            function<void(exception_ptr)>, const Ice::Current&)
{
    response(p1, p1);
}

void
InitialI::opDoubleAsync(optional<double> p1,
                             function<void(optional<double>, optional<double>)> response,
                             function<void(exception_ptr)>, const Ice::Current&)
{
    response(p1, p1);
}

void
InitialI::opStringAsync(optional<string> p1,
                             function<void(optional<string_view>, optional<string_view>)> response,
                             function<void(exception_ptr)>, const Ice::Current&)
{
    response(p1, p1);
}

void
InitialI::opMyEnumAsync(optional<Test::MyEnum> p1,
                             function<void(optional<Test::MyEnum>, optional<Test::MyEnum>)> response,
                             function<void(exception_ptr)>, const Ice::Current&)
{
    response(p1, p1);
}

void
InitialI::opSmallStructAsync(optional<Test::SmallStruct> p1,
                                  function<void(const optional<Test::SmallStruct>&, const optional<Test::SmallStruct>&)> response,
                                  function<void(exception_ptr)>, const Ice::Current&)
{
    response(p1, p1);
}

void
InitialI::opFixedStructAsync(optional<Test::FixedStruct> p1,
                                  function<void(const optional<Test::FixedStruct>&, const optional<Test::FixedStruct>&)> response,
                                  function<void(exception_ptr)>, const Ice::Current&)
{
    response(p1, p1);
}

void
InitialI::opVarStructAsync(optional<Test::VarStruct> p1,
                                function<void(const optional<Test::VarStruct>&, const optional<Test::VarStruct>&)> response,
                                function<void(exception_ptr)>, const Ice::Current&)
{
    response(p1, p1);
}

void
InitialI::opOneOptionalAsync(optional<shared_ptr<Test::OneOptional>> p1,
                                  function<void(const optional<shared_ptr<Test::OneOptional>>&, const optional<shared_ptr<Test::OneOptional>>&)> response,
                                  function<void(exception_ptr)>, const Ice::Current&)
{
    response(p1, p1);
}

void
InitialI::opMyInterfaceProxyAsync(optional<::MyInterfacePrx> p1,
                                       function<void(const optional<::MyInterfacePrx>&, const optional<::MyInterfacePrx>&)> response,
                                       function<void(exception_ptr)>, const Ice::Current&)
{
    response(p1, p1);
}

void
InitialI::opByteSeqAsync(optional<pair<const ::uint8_t*, const ::uint8_t*>> p1,
                              function<void(const optional<pair<const ::uint8_t*, const ::uint8_t*>>&, const optional<pair<const ::uint8_t*, const ::uint8_t*>>&)> response,
                              function<void(exception_ptr)>, const Ice::Current&)
{
    response(p1, p1);
}

void
InitialI::opBoolSeqAsync(optional<pair<const bool*, const bool*>> p1,
                              function<void(const optional<pair<const bool*, const bool*>>&, const optional<pair<const bool*, const bool*>>&)> response,
                              function<void(exception_ptr)>, const Ice::Current&)
{
    response(p1, p1);
}

void
InitialI::opShortSeqAsync(optional<pair<const short*, const short*>> p1,
                               function<void(const optional<pair<const short*, const short*>>&, const optional<pair<const short*, const short*>>&)> response,
                               function<void(exception_ptr)>, const Ice::Current&)
{
    response(p1, p1);
}

void
InitialI::opIntSeqAsync(optional<pair<const int*, const int*>> p1,
                             function<void(const optional<pair<const int*, const int*>>&, const optional<pair<const int*, const int*>>&)> response,
                             function<void(exception_ptr)>, const Ice::Current&)
{
    response(p1, p1);
}

void
InitialI::opLongSeqAsync(optional<pair<const int64_t*, const int64_t*>> p1,
                              function<void(const optional<pair<const int64_t*, const int64_t*>>&, const optional<pair<const int64_t*, const int64_t*>>&)> response,
                              function<void(exception_ptr)>, const Ice::Current&)
{
    response(p1, p1);
}

void
InitialI::opFloatSeqAsync(optional<pair<const float*, const float*>> p1,
                               function<void(const optional<pair<const float*, const float*>>&, const optional<pair<const float*, const float*>>&)> response,
                               function<void(exception_ptr)>, const Ice::Current&)
{
    response(p1, p1);
}

void
InitialI::opDoubleSeqAsync(optional<pair<const double*, const double*>> p1,
                                function<void(const optional<pair<const double*, const double*>>&, const optional<pair<const double*, const double*>>&)> response,
                                function<void(exception_ptr)>, const Ice::Current&)
{
    response(p1, p1);
}

void
InitialI::opStringSeqAsync(optional<Test::StringSeq> p1,
                                function<void(const optional<Test::StringSeq>&, const optional<Test::StringSeq>&)> response,
                                function<void(exception_ptr)>, const Ice::Current&)
{
    response(p1, p1);
}

void
InitialI::opSmallStructSeqAsync(optional<pair<const Test::SmallStruct*, const Test::SmallStruct*>> p1,
                                     function<void(const optional<pair<const Test::SmallStruct*, const Test::SmallStruct*>>&, const optional<pair<const Test::SmallStruct*, const Test::SmallStruct*>>&)> response,
                                     function<void(exception_ptr)>, const Ice::Current&)
{
    response(p1, p1);
}

void
InitialI::opSmallStructListAsync(optional<pair<const Test::SmallStruct*, const Test::SmallStruct*>> p1,
                                      function<void(const optional<pair<const Test::SmallStruct*, const Test::SmallStruct*>>&, const optional<pair<const Test::SmallStruct*, const Test::SmallStruct*>>&)> response,
                                      function<void(exception_ptr)>, const Ice::Current&)
{
    response(p1, p1);
}

void
InitialI::opFixedStructSeqAsync(optional<pair<const Test::FixedStruct*, const Test::FixedStruct*>> p1,
                                     function<void(const optional<pair<const Test::FixedStruct*, const Test::FixedStruct*>>&, const optional<pair<const Test::FixedStruct*, const Test::FixedStruct*>>&)> response,
                                     function<void(exception_ptr)>, const Ice::Current&)
{
    response(p1, p1);
}

void
InitialI::opFixedStructListAsync(optional<pair<const Test::FixedStruct*, const Test::FixedStruct*>> p1,
                                 function<void(const optional<pair<const Test::FixedStruct*, const Test::FixedStruct*>>&, const optional<pair<const Test::FixedStruct*, const Test::FixedStruct*>>&)> response,
                                 function<void(exception_ptr)>, const Ice::Current&)
{
    response(p1, p1);
}

void
InitialI::opVarStructSeqAsync(optional<Test::VarStructSeq> p1,
                              function<void(const optional<Test::VarStructSeq>&, const optional<Test::VarStructSeq>&)> response,
                              function<void(exception_ptr)>, const Ice::Current&)
{
    response(p1, p1);
}

void
InitialI::opSerializableAsync(optional<Test::Serializable> p1,
                              function<void(const optional<Test::Serializable>&, const optional<Test::Serializable>&)> response,
                              function<void(exception_ptr)>, const Ice::Current&)
{
    response(p1, p1);
}

void
InitialI::opIntIntDictAsync(optional<Test::IntIntDict> p1,
                            function<void(const optional<Test::IntIntDict>&, const optional<Test::IntIntDict>&)> response,
                            function<void(exception_ptr)>, const Ice::Current&)
{
    response(p1, p1);
}

void
InitialI::opStringIntDictAsync(optional<Test::StringIntDict> p1,
                               function<void(const optional<Test::StringIntDict>&, const optional<Test::StringIntDict>&)> response,
                               function<void(exception_ptr)>, const Ice::Current&)
{
    response(p1, p1);
}

void
InitialI::opIntOneOptionalDictAsync(optional<Test::IntOneOptionalDict> p1,
                                    function<void(const optional<Test::IntOneOptionalDict>&, const optional<Test::IntOneOptionalDict>&)> response,
                                    function<void(exception_ptr)>, const Ice::Current&)
{
    response(p1, p1);
}

void
InitialI::opClassAndUnknownOptionalAsync(shared_ptr<Test::A>,
                                         function<void()> response,
                                         function<void(exception_ptr)>, const Ice::Current&)
{
    response();
}

void
InitialI::sendOptionalClassAsync(bool, optional<shared_ptr<Test::OneOptional>>,
                                 function<void()> response,
                                 function<void(exception_ptr)>, const Ice::Current&)
{
    response();
}

void
InitialI::returnOptionalClassAsync(bool,
                                   function<void(const optional<shared_ptr<Test::OneOptional>>&)> response,
                                   function<void(exception_ptr)>, const Ice::Current&)
{
    response(make_shared<OneOptional>(53));
}

void
InitialI::opGAsync(shared_ptr<Test::G> g,
                   function<void(const shared_ptr<Test::G>&)> response,
                   function<void(exception_ptr)>, const Ice::Current&)
{
    response(g);
}

void
InitialI::opVoidAsync(function<void()> response,
                      function<void(exception_ptr)>, const Ice::Current&)
{
    response();
}

void
InitialI::opMStruct1Async(function<void(const OpMStruct1MarshaledResult&)> response,
                          function<void(exception_ptr)>,
                          const Ice::Current& current)
{
    response(OpMStruct1MarshaledResult(Test::SmallStruct(), current));
}

void
InitialI::opMStruct2Async(optional<SmallStruct> p1,
                         function<void(const OpMStruct2MarshaledResult&)> response,
                         function<void(exception_ptr)>,
                         const Ice::Current& current)
{
    response(OpMStruct2MarshaledResult(p1, p1, current));
}

void
InitialI::opMSeq1Async(function<void(const OpMSeq1MarshaledResult&)> response,
                       function<void(exception_ptr)>,
                       const Ice::Current& current)
{
    response(OpMSeq1MarshaledResult(Test::StringSeq(), current));
}

void
InitialI::opMSeq2Async(optional<Test::StringSeq> p1,
                       function<void(const OpMSeq2MarshaledResult&)> response,
                       function<void(exception_ptr)>,
                       const Ice::Current& current)
{
    response(OpMSeq2MarshaledResult(p1, p1, current));
}

void
InitialI::opMDict1Async(function<void(const OpMDict1MarshaledResult&)> response,
                        function<void(exception_ptr)>,
                        const Ice::Current& current)
{
    response(OpMDict1MarshaledResult(StringIntDict(), current));
}

void
InitialI::opMDict2Async(optional<StringIntDict> p1,
                        function<void(const OpMDict2MarshaledResult&)> response,
                        function<void(exception_ptr)>,
                        const Ice::Current& current)
{
    response(OpMDict2MarshaledResult(p1, p1, current));
}

void
InitialI::opMG1Async(function<void(const OpMG1MarshaledResult&)> response,
                     function<void(exception_ptr)>,
                     const Ice::Current& current)
{
    response(OpMG1MarshaledResult(std::make_shared<G>(), current));
}

void
InitialI::opMG2Async(optional<GPtr> p1,
                     function<void(const OpMG2MarshaledResult&)> response,
                     function<void(exception_ptr)>,
                     const Ice::Current& current)
{
    response(OpMG2MarshaledResult(p1, p1, current));
}

void
InitialI::supportsRequiredParamsAsync(function<void(bool)> response,
                                      function<void(exception_ptr)>, const Ice::Current&)
{
    response(false);
}

void
InitialI::supportsJavaSerializableAsync(function<void(bool)> response,
                                        function<void(exception_ptr)>, const Ice::Current&)
{
    response(true);
}

void
InitialI::supportsCsharpSerializableAsync(function<void(bool)> response,
                                          function<void(exception_ptr)>, const Ice::Current&)
{
    response(true);
}

void
InitialI::supportsNullOptionalAsync(function<void(bool)> response,
                                    function<void(exception_ptr)>, const Ice::Current&)
{
    response(true);
}
