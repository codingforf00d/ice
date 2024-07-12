//
// Copyright (c) ZeroC, Inc. All rights reserved.
//

#include "Util.h"
#include "Ice/DisableWarnings.h"
#include "Ice/Protocol.h"
#include "slice2py/PythonUtil.h"

#include <compile.h>
#include <cstddef>
#include <frameobject.h>

using namespace std;
using namespace Slice::Python;

namespace IcePy
{
    template<typename T> bool setVersion(PyObject* p, const T& version)
    {
        PyObjectHandle major = PyLong_FromLong(version.major);
        PyObjectHandle minor = PyLong_FromLong(version.minor);
        if (!major.get() || !minor.get())
        {
            return false;
        }
        if (PyObject_SetAttrString(p, "major", major.get()) < 0 || PyObject_SetAttrString(p, "minor", minor.get()) < 0)
        {
            return false;
        }
        return true;
    }

    template<typename T> bool getVersion(PyObject* p, T& v)
    {
        PyObjectHandle major = getAttr(p, "major", false);
        PyObjectHandle minor = getAttr(p, "minor", false);
        if (major.get())
        {
            major = PyNumber_Long(major.get());
            if (!major.get())
            {
                PyErr_Format(PyExc_ValueError, "version major must be a numeric value");
                return false;
            }
            long m = PyLong_AsLong(major.get());
            if (m < 0 || m > 255)
            {
                PyErr_Format(PyExc_ValueError, "version major must be a value between 0 and 255");
                return false;
            }
            v.major = static_cast<uint8_t>(m);
        }
        else
        {
            v.major = 0;
        }

        if (minor.get())
        {
            major = PyNumber_Long(minor.get());
            if (!minor.get())
            {
                PyErr_Format(PyExc_ValueError, "version minor must be a numeric value");
                return false;
            }
            long m = PyLong_AsLong(minor.get());
            if (m < 0 || m > 255)
            {
                PyErr_Format(PyExc_ValueError, "version minor must be a value between 0 and 255");
                return false;
            }
            v.minor = static_cast<uint8_t>(m);
        }
        else
        {
            v.minor = 0;
        }
        return true;
    }

    template<typename T> PyObject* createVersion(const T& version, const char* type)
    {
        PyObject* versionType = lookupType(type);

        PyObjectHandle obj = PyObject_CallObject(versionType, 0);
        if (!obj.get())
        {
            return 0;
        }

        if (!setVersion<T>(obj.get(), version))
        {
            return 0;
        }

        return obj.release();
    }

    template<typename T> PyObject* versionToString(PyObject* args, const char* type)
    {
        PyObject* versionType = IcePy::lookupType(type);
        PyObject* p;
        if (!PyArg_ParseTuple(args, "O!", versionType, &p))
        {
            return nullptr;
        }

        T v;
        if (!getVersion<T>(p, v))
        {
            return nullptr;
        }

        string s;
        try
        {
            s = IceInternal::versionToString<T>(v);
        }
        catch (...)
        {
            IcePy::setPythonException(current_exception());
            return nullptr;
        }
        return createString(s);
    }

    template<typename T> PyObject* stringToVersion(PyObject* args, const char* type)
    {
        char* str;
        if (!PyArg_ParseTuple(args, "s", &str))
        {
            return nullptr;
        }

        T v;
        try
        {
            v = IceInternal::stringToVersion<T>(str);
        }
        catch (...)
        {
            IcePy::setPythonException(current_exception());
            return nullptr;
        }

        return createVersion<T>(v, type);
    }

    char Ice_ProtocolVersion[] = "Ice.ProtocolVersion";
    char Ice_EncodingVersion[] = "Ice.EncodingVersion";
}

string
IcePy::getString(PyObject* p)
{
    assert(p == Py_None || checkString(p));

    string str;
    if (p != Py_None)
    {
        PyObjectHandle bytes = PyUnicode_AsUTF8String(p);
        if (bytes.get())
        {
            char* s;
            Py_ssize_t sz;
            PyBytes_AsStringAndSize(bytes.get(), &s, &sz);
            str.assign(s, static_cast<size_t>(sz));
        }
    }
    return str;
}

bool
IcePy::getStringArg(PyObject* p, const string& arg, string& val)
{
    if (checkString(p))
    {
        val = getString(p);
    }
    else if (p != Py_None)
    {
        string funcName = getFunction();
        PyErr_Format(PyExc_ValueError, "%s expects a string for argument '%s'", funcName.c_str(), arg.c_str());
        return false;
    }
    return true;
}

PyObject*
IcePy::getAttr(PyObject* obj, const string& attrib, bool allowNone)
{
    PyObject* v = PyObject_GetAttrString(obj, attrib.c_str());
    if (v == Py_None)
    {
        if (!allowNone)
        {
            Py_DECREF(v);
            v = 0;
        }
    }
    else if (!v)
    {
        PyErr_Clear(); // PyObject_GetAttrString sets an error on failure.
    }

    return v;
}

string
IcePy::getFunction()
{
    //
    // Get name of current function.
    //
    PyFrameObject* f = PyEval_GetFrame();
    PyObjectHandle code = getAttr(reinterpret_cast<PyObject*>(f), "f_code", false);
    assert(code.get());
    PyObjectHandle func = getAttr(code.get(), "co_name", false);
    assert(func.get());
    return getString(func.get());
}

IcePy::PyObjectHandle::PyObjectHandle(PyObject* p) : _p(p) {}

IcePy::PyObjectHandle::PyObjectHandle(const PyObjectHandle& p) : _p(p._p) { Py_XINCREF(_p); }

IcePy::PyObjectHandle::~PyObjectHandle() { Py_XDECREF(_p); }

IcePy::PyObjectHandle&
IcePy::PyObjectHandle::operator=(PyObject* p)
{
    Py_XDECREF(_p);
    _p = p;
    return *this;
}

IcePy::PyObjectHandle&
IcePy::PyObjectHandle::operator=(const PyObjectHandle& p)
{
    if (this != &p)
    {
        Py_XDECREF(_p);
        _p = p._p;
        Py_XINCREF(_p);
    }
    return *this;
}

PyObject*
IcePy::PyObjectHandle::get() const
{
    return _p;
}

PyObject*
IcePy::PyObjectHandle::release()
{
    PyObject* result = _p;
    _p = nullptr;
    return result;
}

IcePy::PyException::PyException()
{
    PyObject* type;
    PyObject* e;
    PyObject* tb;

    PyErr_Fetch(&type, &e, &tb); // PyErr_Fetch clears the exception.
    PyErr_NormalizeException(&type, &e, &tb);

    _type = type;
    ex = e;
    _tb = tb;
}

IcePy::PyException::PyException(PyObject* p)
{
    ex = p;
    Py_XINCREF(p);
}

void
IcePy::PyException::raise()
{
    assert(ex.get());

    PyObject* userExceptionType = lookupType("Ice.UserException");
    PyObject* localExceptionType = lookupType("Ice.LocalException");

    // TODO: create better error messages.

    if (PyObject_IsInstance(ex.get(), userExceptionType))
    {
        string tb = getTraceback();
        if (!tb.empty())
        {
            throw Ice::UnknownUserException{__FILE__, __LINE__, tb};
        }
        else
        {
            PyObjectHandle name = PyObject_CallMethod(ex.get(), "ice_id", 0);
            PyErr_Clear();
            if (!name.get())
            {
                throw Ice::UnknownUserException{__FILE__, __LINE__, getTypeName()};
            }
            else
            {
                throw Ice::UnknownUserException{__FILE__, __LINE__, getString(name.get())};
            }
        }
    }
    else if (PyObject_IsInstance(ex.get(), localExceptionType))
    {
        raiseLocalException();
    }
    else
    {
        string tb = getTraceback();
        if (!tb.empty())
        {
            throw Ice::UnknownException{__FILE__, __LINE__, tb};
        }
        else
        {
            ostringstream ostr;

            ostr << getTypeName();

            IcePy::PyObjectHandle msg = PyObject_Str(ex.get());
            if (msg.get())
            {
                string s = getString(msg.get());
                if (!s.empty())
                {
                    ostr << ": " << s;
                }
            }

            throw Ice::UnknownException{__FILE__, __LINE__, ostr.str()};
        }
    }
}

void
IcePy::PyException::checkSystemExit()
{
    if (PyObject_IsInstance(ex.get(), PyExc_SystemExit))
    {
        handleSystemExit(ex.get()); // Does not return.
    }
}

void
IcePy::PyException::raiseLocalException()
{
    string typeName = getTypeName();

    if (typeName == "Ice.ObjectNotExistException")
    {
        throw Ice::ObjectNotExistException(__FILE__, __LINE__);
    }
    else if (typeName == "Ice.OperationNotExistException")
    {
        throw Ice::OperationNotExistException(__FILE__, __LINE__);
    }
    else if (typeName == "Ice.FacetNotExistException")
    {
        throw Ice::FacetNotExistException(__FILE__, __LINE__);
    }

    IcePy::PyObjectHandle exStr = PyObject_Str(ex.get());
    string message;
    if (exStr.get() && checkString(exStr.get()))
    {
        message = getString(exStr.get());
    }

    if (typeName == "Ice.UnknownLocalException")
    {
        throw Ice::UnknownLocalException{__FILE__, __LINE__, std::move(message)};
    }
    else if (typeName == "Ice.UnknownUserException")
    {
        throw Ice::UnknownUserException{__FILE__, __LINE__, std::move(message)};
    }
    else if (typeName == "Ice.UnknownException")
    {
        throw Ice::UnknownException{__FILE__, __LINE__, std::move(message)};
    }

    string tb = getTraceback();
    if (!tb.empty())
    {
        throw Ice::UnknownLocalException{__FILE__, __LINE__, tb};
    }
    else
    {
        throw Ice::UnknownLocalException{__FILE__, __LINE__, typeName};
    }
}

string
IcePy::PyException::getTraceback()
{
    if (!_tb.get())
    {
        return string();
    }

    //
    // We need the equivalent of the following Python code:
    //
    // import traceback
    // list = traceback.format_exception(type, ex, tb)
    //
    PyObjectHandle str = createString("traceback");
    PyObjectHandle mod = PyImport_Import(str.get());
    assert(mod.get()); // Unable to import traceback module - Python installation error?
    PyObject* func = PyDict_GetItemString(PyModule_GetDict(mod.get()), "format_exception");
    assert(func); // traceback.format_exception must be present.
    PyObjectHandle args = Py_BuildValue("(OOO)", _type.get(), ex.get(), _tb.get());
    assert(args.get());
    PyObjectHandle list = PyObject_CallObject(func, args.get());
    assert(list.get());

    string result;
    for (Py_ssize_t i = 0; i < PyList_GET_SIZE(list.get()); ++i)
    {
        string s = getString(PyList_GetItem(list.get(), i));
        result += s;
    }

    return result;
}

string
IcePy::PyException::getTypeName()
{
    PyObject* cls = reinterpret_cast<PyObject*>(ex.get()->ob_type);
    PyObjectHandle name = getAttr(cls, "__name__", false);
    assert(name.get());
    PyObjectHandle mod = getAttr(cls, "__module__", false);
    assert(mod.get());
    string result = getString(mod.get());
    result += ".";
    result += getString(name.get());
    return result;
}

PyObject*
IcePy::byteSeqToList(const Ice::ByteSeq& seq)
{
    PyObject* l = PyList_New(0);
    if (!l)
    {
        return 0;
    }

    for (Ice::ByteSeq::const_iterator p = seq.begin(); p != seq.end(); ++p)
    {
        PyObject* byte = PyLong_FromLong(std::to_integer<long>(*p));
        if (!byte)
        {
            Py_DECREF(l);
            return 0;
        }
        int status = PyList_Append(l, byte);
        Py_DECREF(byte); // Give ownership to the list.
        if (status < 0)
        {
            Py_DECREF(l);
            return 0;
        }
    }

    return l;
}

bool
IcePy::listToStringSeq(PyObject* l, Ice::StringSeq& seq)
{
    assert(PyList_Check(l));

    Py_ssize_t sz = PyList_GET_SIZE(l);
    for (Py_ssize_t i = 0; i < sz; ++i)
    {
        PyObject* item = PyList_GET_ITEM(l, i);
        if (!item)
        {
            return false;
        }
        string str;
        if (checkString(item))
        {
            str = getString(item);
        }
        else if (item != Py_None)
        {
            PyErr_Format(PyExc_ValueError, "list element must be a string");
            return false;
        }
        seq.push_back(str);
    }

    return true;
}

bool
IcePy::stringSeqToList(const Ice::StringSeq& seq, PyObject* l)
{
    assert(PyList_Check(l));

    for (Ice::StringSeq::const_iterator p = seq.begin(); p != seq.end(); ++p)
    {
        PyObject* str = Py_BuildValue("s", p->c_str());
        if (!str)
        {
            Py_DECREF(l);
            return false;
        }
        int status = PyList_Append(l, str);
        Py_DECREF(str); // Give ownership to the list.
        if (status < 0)
        {
            Py_DECREF(l);
            return false;
        }
    }

    return true;
}

bool
IcePy::tupleToStringSeq(PyObject* t, Ice::StringSeq& seq)
{
    assert(PyTuple_Check(t));

    int sz = static_cast<int>(PyTuple_GET_SIZE(t));
    for (int i = 0; i < sz; ++i)
    {
        PyObject* item = PyTuple_GET_ITEM(t, i);
        if (!item)
        {
            return false;
        }
        string str;
        if (checkString(item))
        {
            str = getString(item);
        }
        else if (item != Py_None)
        {
            PyErr_Format(PyExc_ValueError, "tuple element must be a string");
            return false;
        }
        seq.push_back(str);
    }

    return true;
}

bool
IcePy::dictionaryToContext(PyObject* dict, Ice::Context& context)
{
    assert(PyDict_Check(dict));

    Py_ssize_t pos = 0;
    PyObject* key;
    PyObject* value;
    while (PyDict_Next(dict, &pos, &key, &value))
    {
        string keystr;
        if (checkString(key))
        {
            keystr = getString(key);
        }
        else if (key != Py_None)
        {
            PyErr_Format(PyExc_ValueError, "context key must be a string");
            return false;
        }

        string valuestr;
        if (checkString(value))
        {
            valuestr = getString(value);
        }
        else if (value != Py_None)
        {
            PyErr_Format(PyExc_ValueError, "context value must be a string");
            return false;
        }

        context.insert(Ice::Context::value_type(keystr, valuestr));
    }

    return true;
}

bool
IcePy::contextToDictionary(const Ice::Context& ctx, PyObject* dict)
{
    assert(PyDict_Check(dict));

    for (Ice::Context::const_iterator p = ctx.begin(); p != ctx.end(); ++p)
    {
        PyObjectHandle key = createString(p->first);
        PyObjectHandle value = createString(p->second);
        if (!key.get() || !value.get())
        {
            return false;
        }
        if (PyDict_SetItem(dict, key.get(), value.get()) < 0)
        {
            return false;
        }
    }

    return true;
}

PyObject*
IcePy::lookupType(const string& typeName)
{
    string::size_type dot = typeName.rfind('.');
    assert(dot != string::npos);
    string moduleName = typeName.substr(0, dot);
    string name = typeName.substr(dot + 1);

    //
    // First search for the module in sys.modules.
    //
    PyObject* sysModules = PyImport_GetModuleDict();
    assert(sysModules);

    PyObject* module = PyDict_GetItemString(sysModules, const_cast<char*>(moduleName.c_str()));
    PyObject* dict;
    if (!module)
    {
        //
        // Not found, so we need to import the module.
        //
        PyObjectHandle h = PyImport_ImportModule(const_cast<char*>(moduleName.c_str()));
        if (!h.get())
        {
            return 0;
        }

        dict = PyModule_GetDict(h.get());
    }
    else
    {
        dict = PyModule_GetDict(module);
    }

    assert(dict);
    return PyDict_GetItemString(dict, const_cast<char*>(name.c_str()));
}

PyObject*
IcePy::createExceptionInstance(PyObject* type)
{
    assert(PyExceptionClass_Check(type));
    IcePy::PyObjectHandle args = PyTuple_New(0);
    if (!args.get())
    {
        return 0;
    }
    return PyEval_CallObject(type, args.get());
}

namespace
{
    // This function takes ownership of each PyObject* in args.
    template<size_t N>
    PyObject*
    createPythonException(const char* typeId, std::array<PyObject*, N> args, bool fallbackToLocalException = false)
    {
        PyObject* type = IcePy::lookupType(scopedToName(typeId));
        if (!type)
        {
            if (fallbackToLocalException)
            {
                type = IcePy::lookupType("Ice.LocalException");
            }
            else
            {
                for (PyObject* pArg : args)
                {
                    Py_DECREF(pArg);
                }

                ostringstream os;
                os << "unable to create Python exception class for type ID " << typeId;
                return PyObject_CallFunction(PyExc_Exception, "s", os.str().c_str());
            }
        }
        IcePy::PyObjectHandle pArgs = PyTuple_New(N);
        for (size_t i = 0; i < N; ++i)
        {
            // PyTuple_SetItem takes ownership of the args[i] reference.
            PyTuple_SetItem(pArgs.get(), static_cast<Py_ssize_t>(i), args[i]);
        }
        return PyEval_CallObject(type, pArgs.get());
    }
}

PyObject*
IcePy::convertException(std::exception_ptr exPtr)
{
    const char* const localExceptionTypeId = "::Ice::LocalException";

    // We cannot throw a C++ exception or raise a Python exception. If an error occurs while we are converting the
    // exception, we do our best to _return_ an appropriate Python exception.
    try
    {
        rethrow_exception(exPtr);
    }
    // First handle exceptions with extra fields we want to provide to Python users.
    catch (const Ice::AlreadyRegisteredException& ex)
    {
        std::array args{
            IcePy::createString(ex.kindOfObject()),
            IcePy::createString(ex.id()),
            IcePy::createString(ex.what())};

        return createPythonException(ex.ice_id(), std::move(args));
    }
    catch (const Ice::NotRegisteredException& ex)
    {
        std::array args{
            IcePy::createString(ex.kindOfObject()),
            IcePy::createString(ex.id()),
            IcePy::createString(ex.what())};

        return createPythonException(ex.ice_id(), std::move(args));
    }
    catch (const Ice::ConnectionAbortedException& ex)
    {
        std::array args{ex.closedByApplication() ? Py_True : Py_False, IcePy::createString(ex.what())};
        return createPythonException(ex.ice_id(), std::move(args));
    }
    catch (const Ice::ConnectionClosedException& ex)
    {
        std::array args{ex.closedByApplication() ? Py_True : Py_False, IcePy::createString(ex.what())};
        return createPythonException(ex.ice_id(), std::move(args));
    }
    catch (const Ice::RequestFailedException& ex)
    {
        std::array args{
            IcePy::createIdentity(ex.id()),
            IcePy::createString(ex.facet()),
            IcePy::createString(ex.operation()),
            IcePy::createString(ex.what())};

        return createPythonException(ex.ice_id(), std::move(args));
    }
    // Then all other exceptions.
    catch (const Ice::LocalException& ex)
    {
        std::array args{IcePy::createString(ex.what())};
        return createPythonException(ex.ice_id(), std::move(args), true);
    }
    catch (const std::exception& ex)
    {
        std::array args{IcePy::createString(ex.what())};
        return createPythonException(localExceptionTypeId, std::move(args));
    }
    catch (...)
    {
        std::array args{IcePy::createString("unknown C++ exception")};
        return createPythonException(localExceptionTypeId, std::move(args));
    }
}

void
IcePy::setPythonException(std::exception_ptr ex)
{
    PyObjectHandle p = convertException(ex);
    if (p.get())
    {
        setPythonException(p.get());
    }
}

void
IcePy::setPythonException(PyObject* ex)
{
    //
    // PyErr_Restore steals references to the type and exception.
    //
    PyObject* type = PyObject_Type(ex);
    assert(type);
    Py_INCREF(ex);
    PyErr_Restore(type, ex, 0);
}

void
IcePy::throwPythonException()
{
    PyException ex;
    ex.raise();
}

void
IcePy::handleSystemExit(PyObject* ex)
{
    //
    // This code is similar to handle_system_exit in pythonrun.c.
    //
    PyObjectHandle code;
    if (PyExceptionInstance_Check(ex))
    {
        code = getAttr(ex, "code", true);
    }
    else
    {
        code = ex;
        Py_INCREF(ex);
    }

    int status;
    if (PyLong_Check(code.get()))
    {
        status = static_cast<int>(PyLong_AsLong(code.get()));
    }
    else
    {
        PyObject_Print(code.get(), stderr, Py_PRINT_RAW);
        PySys_WriteStderr("\n");
        status = 1;
    }

    code = 0;
    Py_Exit(status);
}

PyObject*
IcePy::createIdentity(const Ice::Identity& ident)
{
    PyObject* identityType = lookupType("Ice.Identity");

    PyObjectHandle obj = PyObject_CallObject(identityType, 0);
    if (!obj.get())
    {
        return 0;
    }

    if (!setIdentity(obj.get(), ident))
    {
        return 0;
    }

    return obj.release();
}

bool
IcePy::checkIdentity(PyObject* p)
{
    PyObject* identityType = lookupType("Ice.Identity");
    return PyObject_IsInstance(p, identityType) == 1;
}

bool
IcePy::setIdentity(PyObject* p, const Ice::Identity& ident)
{
    assert(checkIdentity(p));
    PyObjectHandle name = createString(ident.name);
    PyObjectHandle category = createString(ident.category);
    if (!name.get() || !category.get())
    {
        return false;
    }
    if (PyObject_SetAttrString(p, "name", name.get()) < 0 || PyObject_SetAttrString(p, "category", category.get()) < 0)
    {
        return false;
    }
    return true;
}

bool
IcePy::getIdentity(PyObject* p, Ice::Identity& ident)
{
    assert(checkIdentity(p));
    PyObjectHandle name = getAttr(p, "name", true);
    PyObjectHandle category = getAttr(p, "category", true);
    if (name.get())
    {
        if (!checkString(name.get()))
        {
            PyErr_Format(PyExc_ValueError, "identity name must be a string");
            return false;
        }
        ident.name = getString(name.get());
    }
    if (category.get())
    {
        if (!checkString(category.get()))
        {
            PyErr_Format(PyExc_ValueError, "identity category must be a string");
            return false;
        }
        ident.category = getString(category.get());
    }
    return true;
}

PyObject*
IcePy::createProtocolVersion(const Ice::ProtocolVersion& v)
{
    return createVersion<Ice::ProtocolVersion>(v, Ice_ProtocolVersion);
}

PyObject*
IcePy::createEncodingVersion(const Ice::EncodingVersion& v)
{
    return createVersion<Ice::EncodingVersion>(v, Ice_EncodingVersion);
}

bool
IcePy::getEncodingVersion(PyObject* p, Ice::EncodingVersion& v)
{
    if (!getVersion<Ice::EncodingVersion>(p, v))
    {
        return false;
    }

    return true;
}

PyObject*
IcePy::callMethod(PyObject* obj, const string& name, PyObject* arg1, PyObject* arg2)
{
    PyObjectHandle method = PyObject_GetAttrString(obj, const_cast<char*>(name.c_str()));
    if (!method.get())
    {
        return 0;
    }
    return callMethod(method.get(), arg1, arg2);
}

PyObject*
IcePy::callMethod(PyObject* method, PyObject* arg1, PyObject* arg2)
{
    PyObjectHandle args;
    if (arg1 && arg2)
    {
        args = PyTuple_New(2);
        if (!args.get())
        {
            return 0;
        }
        PyTuple_SET_ITEM(args.get(), 0, incRef(arg1));
        PyTuple_SET_ITEM(args.get(), 1, incRef(arg2));
    }
    else if (arg1)
    {
        args = PyTuple_New(1);
        if (!args.get())
        {
            return 0;
        }
        PyTuple_SET_ITEM(args.get(), 0, incRef(arg1));
    }
    else if (arg2)
    {
        args = PyTuple_New(1);
        if (!args.get())
        {
            return 0;
        }
        PyTuple_SET_ITEM(args.get(), 0, incRef(arg2));
    }
    else
    {
        args = PyTuple_New(0);
        if (!args.get())
        {
            return 0;
        }
    }
    return PyObject_Call(method, args.get(), 0);
}

extern "C" PyObject*
IcePy_stringVersion(PyObject* /*self*/, PyObject* /*args*/)
{
    string s = ICE_STRING_VERSION;
    return IcePy::createString(s);
}

extern "C" PyObject*
IcePy_intVersion(PyObject* /*self*/, PyObject* /*args*/)
{
    return PyLong_FromLong(ICE_INT_VERSION);
}

extern "C" PyObject*
IcePy_currentProtocol(PyObject* /*self*/, PyObject* /*args*/)
{
    return IcePy::createProtocolVersion(Ice::currentProtocol);
}

extern "C" PyObject*
IcePy_currentProtocolEncoding(PyObject* /*self*/, PyObject* /*args*/)
{
    return IcePy::createEncodingVersion(Ice::currentProtocolEncoding);
}

extern "C" PyObject*
IcePy_currentEncoding(PyObject* /*self*/, PyObject* /*args*/)
{
    return IcePy::createEncodingVersion(Ice::currentEncoding);
}

extern "C" PyObject*
IcePy_protocolVersionToString(PyObject* /*self*/, PyObject* args)
{
    return IcePy::versionToString<Ice::ProtocolVersion>(args, IcePy::Ice_ProtocolVersion);
}

extern "C" PyObject*
IcePy_stringToProtocolVersion(PyObject* /*self*/, PyObject* args)
{
    return IcePy::stringToVersion<Ice::ProtocolVersion>(args, IcePy::Ice_ProtocolVersion);
}

extern "C" PyObject*
IcePy_encodingVersionToString(PyObject* /*self*/, PyObject* args)
{
    return IcePy::versionToString<Ice::EncodingVersion>(args, IcePy::Ice_EncodingVersion);
}

extern "C" PyObject*
IcePy_stringToEncodingVersion(PyObject* /*self*/, PyObject* args)
{
    return IcePy::stringToVersion<Ice::EncodingVersion>(args, IcePy::Ice_EncodingVersion);
}

extern "C" PyObject*
IcePy_generateUUID(PyObject* /*self*/, PyObject* /*args*/)
{
    string uuid = Ice::generateUUID();
    return IcePy::createString(uuid);
}
