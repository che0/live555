/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// "liveMedia"
// Copyright (c) 1996-2012 Live Networks, Inc.  All rights reserved.
// A source that loops one file forever
// C++ header

#ifndef _BYTE_STREAM_LOOP_FILE_SOURCE_HH
#define _BYTE_STREAM_LOOP_FILE_SOURCE_HH

#ifndef _BYTE_STREAM_FILE_SOURCE_HH
#include "ByteStreamFileSource.hh"
#endif

class ByteStreamLoopFileSource: public FramedSource {
public:
  static ByteStreamLoopFileSource*
  createNew(UsageEnvironment& env, char const* fileName,
	    unsigned preferredFrameSize = 0, unsigned playTimePerFrame = 0);

  Boolean haveStartedNewFile() const { return fHaveStartedNewFile; }
  // True iff the most recently delivered frame was the first from a newly-opened file

  void seekToByteAbsolute(u_int64_t byteNumber, u_int64_t numBytesToStream = 0);
  void seekToEnd();
  
protected:
  ByteStreamLoopFileSource(UsageEnvironment& env, char const* fileName,
			    unsigned preferredFrameSize, unsigned playTimePerFrame);
	// called only by createNew()

  virtual ~ByteStreamLoopFileSource();

private:
  // redefined virtual functions:
  virtual void doGetNextFrame();

private:
  static void onSourceClosure(void* clientData);
  void onSourceClosure1();
  static void afterGettingFrame(void* clientData,
				unsigned frameSize, unsigned numTruncatedBytes,
                                struct timeval presentationTime,
				unsigned durationInMicroseconds);

private:
  unsigned fPreferredFrameSize;
  unsigned fPlayTimePerFrame;
  Boolean fHaveStartedNewFile;
  char const* fFileName;
  ByteStreamFileSource* fSource;
};

#endif
