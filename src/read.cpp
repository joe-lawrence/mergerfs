/*
   The MIT License (MIT)

   Copyright (c) 2014 Antonio SJ Musumeci <trapexit@spawn.link>

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/

#include <fuse.h>

#include <errno.h>
#include <unistd.h>
#include <string.h>

#include <string>
#include <algorithm>

#include "config.hpp"
#include "ugid.hpp"
#include "fileinfo.hpp"

using std::string;

static
int
_read_controlfile(const string  readstr,
                  char         *buf,
                  const size_t  count)
{
  size_t n;

  n = std::min(count,readstr.length());

  memcpy(buf,readstr.data(),n);

  return (int)n;
}

static
int
_read(const int     fd,
      void         *buf,
      const size_t  count,
      const off_t   offset)
{
  int rv;

  rv = ::pread(fd,buf,count,offset);

  return ((rv == -1) ? -errno : rv);
}

namespace mergerfs
{
  namespace read
  {
    int
    read(const char            *fusepath,
         char                  *buf,
         size_t                 count,
         off_t                  offset,
         struct fuse_file_info *fi)
    {
      const config::Config &config = config::get();

      if(fusepath == config.controlfile)
        return _read_controlfile(config.readstr,
                                 buf,
                                 count);

      return _read(((FileInfo*)fi->fh)->fd,
                   buf,
                   count,
                   offset);
    }
  }
}
