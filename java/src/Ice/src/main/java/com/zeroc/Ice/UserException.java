//
// Copyright (c) ZeroC, Inc. All rights reserved.
//

package com.zeroc.Ice;

/** Base class for exceptions defined in Slice. */
public abstract class UserException extends java.lang.Exception {
  /**
   * Returns the type id of this exception.
   *
   * @return The type id of this exception.
   */
  public abstract String ice_id();

  /**
   * Returns a string representation of this exception.
   *
   * @return A string representation of this exception.
   */
  @Override
  public String toString() {
    java.io.StringWriter sw = new java.io.StringWriter();
    java.io.PrintWriter pw = new java.io.PrintWriter(sw);
    com.zeroc.IceUtilInternal.OutputBase out = new com.zeroc.IceUtilInternal.OutputBase(pw);
    out.setUseTab(false);
    out.print(getClass().getName());
    out.inc();
    com.zeroc.IceInternal.ValueWriter.write(this, out);
    pw.flush();
    return sw.toString();
  }

  /**
   * @hidden
   */
  public void _write(OutputStream os) {
    os.startException();
    _writeImpl(os);
    os.endException();
  }

  /**
   * @hidden
   */
  public void _read(InputStream is) {
    is.startException();
    _readImpl(is);
    is.endException();
  }

  /**
   * @hidden
   */
  public boolean _usesClasses() {
    return false;
  }

  /**
   * @hidden
   */
  protected abstract void _writeImpl(OutputStream os);

  /**
   * @hidden
   */
  protected abstract void _readImpl(InputStream is);

  private static final long serialVersionUID = 0L;
}
