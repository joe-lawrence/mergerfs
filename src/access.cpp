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

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <fuse.h>

#include <string>
#include <vector>

#include <unistd.h>
#include <errno.h>

#include "ugid.hpp"
#include "fs.hpp"
#include "config.hpp"
#include "assert.hpp"

using std::string;
using std::vector;
using mergerfs::Policy;

static
int
_access(const fs::SearchFunc  searchFunc,
        const vector<string> &srcmounts,
        const string          fusepath,
        const int             mask)
{
  int rv;
  fs::PathVector path;

  searchFunc(srcmounts,fusepath,path);
  if(path.empty())
    return -ENOENT;

  rv = ::eaccess(path[0].full.c_str(),mask);

  return ((rv == -1) ? -errno : 0);
}

namespace mergerfs
{
  namespace access
  {
    int
    access(const char *fusepath,
           int         mask)
    {
      const struct fuse_context *fc     = fuse_get_context();
      const config::Config      &config = config::get();
      const ugid::SetResetGuard  ugid(fc->uid,fc->gid);

      if(fusepath == config.controlfile)
        return _access(*config.search,
                       config.srcmounts,
                       "/",
                       mask);

      return _access(*config.search,
                     config.srcmounts,
                     fusepath,
                     mask);
    }
  }
}
