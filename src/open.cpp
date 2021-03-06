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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string>
#include <vector>

#include "ugid.hpp"
#include "fileinfo.hpp"
#include "fs.hpp"
#include "config.hpp"
#include "assert.hpp"

using std::string;
using std::vector;
using mergerfs::FileInfo;
using mergerfs::Policy;

static
int
_open_controlfile(uint64_t &fh)
{
  fh = (uint64_t)new string;

  return 0;
}

static
int
_open(const fs::SearchFunc  searchFunc,
      const vector<string> &srcmounts,
      const string          fusepath,
      const int             flags,
      uint64_t             &fh)
{
  int      fd;
  fs::PathVector paths;

  searchFunc(srcmounts,fusepath,paths);
  if(paths.empty())
    return -ENOENT;

  fd = ::open(paths[0].full.c_str(),flags);
  if(fd == -1)
    return -errno;

  fh = (uint64_t)new FileInfo(fd,flags,paths[0].full);

  return 0;
}

namespace mergerfs
{
  namespace open
  {
    int
    open(const char            *fusepath,
         struct fuse_file_info *fileinfo)
    {
      const struct fuse_context *fc     = fuse_get_context();
      const config::Config      &config = config::get();
      const ugid::SetResetGuard  ugid(fc->uid,fc->gid);;

      if(fusepath == config.controlfile)
        return _open_controlfile(fileinfo->fh);

      return _open(*config.search,
                   config.srcmounts,
                   fusepath,
                   fileinfo->flags,
                   fileinfo->fh);
    }
  }
}
