// **********************************************************************
//
// Copyright (c) 2003-2004 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_PATCH2_FILE_SERVER_I_H
#define ICE_PATCH2_FILE_SERVER_I_H

#include <IcePatch2/Util.h>
#include <IcePatch2/FileServer.h>

namespace IcePatch2
{

class FileServerI : public FileServer
{
public:

    FileServerI(const FileInfoSeq&);

    FileInfoSeq getFileInfoSeq(Ice::Int, Ice::Int, const Ice::Current&) const;

    ByteSeqSeq getChecksum2Seq(Ice::Int, const Ice::Current&) const;

    ByteSeqSeq getChecksum1Seq(const Ice::Current&) const;

    Ice::ByteSeq getChecksum0(const Ice::Current&) const;

    Ice::ByteSeq getFileCompressed(const std::string&, Ice::Int pos, Ice::Int num, const Ice::Current&) const;

private:

    const FileTree0 _tree0;
};

}

#endif
