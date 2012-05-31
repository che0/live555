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
// Implementation

#include "ByteStreamLoopFileSource.hh"

ByteStreamLoopFileSource
::ByteStreamLoopFileSource(UsageEnvironment& env, char const* fileName,
			    unsigned preferredFrameSize, unsigned playTimePerFrame)
  : FramedSource(env),
    fPreferredFrameSize(preferredFrameSize), fPlayTimePerFrame(playTimePerFrame),
    fHaveStartedNewFile(False) {
    
    fFileName = strDup(fileName);
    fSource = NULL;
}

ByteStreamLoopFileSource::~ByteStreamLoopFileSource() {
  Medium::close(fSource);
  delete fFileName;
}

ByteStreamLoopFileSource* ByteStreamLoopFileSource
::createNew(UsageEnvironment& env, char const* fileName,
	    unsigned preferredFrameSize, unsigned playTimePerFrame) {
  ByteStreamLoopFileSource* newSource
    = new ByteStreamLoopFileSource(env, fileName,
				    preferredFrameSize, playTimePerFrame);

  return newSource;
}

void ByteStreamLoopFileSource::doGetNextFrame() {
  if (fSource == NULL) {
    // The source hasn't been created yet.  Do this now:
    fSource = ByteStreamFileSource::createNew(envir(), fFileName, fPreferredFrameSize, fPlayTimePerFrame);
    if (fSource == NULL) {
      handleClosure(this);
      return;
    }
    fHaveStartedNewFile = True;
  }
  else
  {
    fHaveStartedNewFile = False;
  }
  
  fSource->getNextFrame(fTo, fMaxSize, afterGettingFrame, this, onSourceClosure, this);
}

void ByteStreamLoopFileSource
  ::afterGettingFrame(void* clientData,
		      unsigned frameSize, unsigned numTruncatedBytes,
		      struct timeval presentationTime,
		      unsigned durationInMicroseconds) {
  ByteStreamLoopFileSource* source = (ByteStreamLoopFileSource*)clientData;
  source->fFrameSize = frameSize;
  source->fNumTruncatedBytes = numTruncatedBytes;
  source->fPresentationTime = presentationTime;
  source->fDurationInMicroseconds = durationInMicroseconds;
  FramedSource::afterGetting(source);
}

void ByteStreamLoopFileSource::onSourceClosure(void* clientData) {
  ByteStreamLoopFileSource* source = (ByteStreamLoopFileSource*)clientData;
  source->onSourceClosure1();
}

void ByteStreamLoopFileSource::onSourceClosure1() {
  // This routine was called because the currently-read source was closed
  // (probably due to EOF).  Close this source down, and move to the
  // next one:
  Medium::close(fSource);
  fSource = NULL;

  // Try reading again:
  doGetNextFrame();
}
