# Copyright (c) 2023 Arm Limited.
#
# SPDX-License-Identifier: MIT
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import sys
import os

VERSION = "v0.0-unreleased"


def make_version_file(build_args, git_hash):
    build_info = "\"arm_compute_version=%s Build options: %s Git hash=%s\"" % (
        VERSION, build_args, git_hash.strip())
    return build_info

def make_version_file_from_sconscript(build_args, git_hash):
    VERSION = "v0.0-unreleased"
    fp = None
    if os.path.exists("external/compute_library/SConscript"):
        fp = "external/compute_library/SConscript"
    elif os.path.exists("SConscript"):
        fp = "SConscript"
    if fp:
        with open(fp) as scons_file:
            for line in scons_file:
                if "VERSION = " in line:
                    VERSION = line.split("=")[-1].strip().replace("\"", "")
                    break
    return "\"arm_compute_version=%s Build options: %s Git hash=%s\"" % (
        VERSION, build_args, git_hash.strip())

if __name__ == "__main__":        
    if len(sys.argv) == 4 and sys.argv[3].lower() == "true":
        print(make_version_file_from_sconscript(sys.argv[1], sys.argv[2]))
    else:
        print(make_version_file(sys.argv[1], sys.argv[2]))
    
