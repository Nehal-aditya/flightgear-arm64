// SPDX-FileCopyrightText: 2016 Torsten Dreyer
// SPDX-License-Identifier: GPL-2.0-or-later

#ifdef _WIN32
# include <direct.h> // for getcwd()
#else // !_WIN32
# include <unistd.h>
#endif

#include <string>

#include "ApplicationProperties.hxx"

using std::string;
using namespace std::string_literals;

double
ApplicationProperties::getDouble (const char *name, const double def) {
  const SGPropertyNode_ptr n (ApplicationProperties::Properties->getNode (name, false));
  if (n == NULL) {
    return def;
  }
  return n->getDoubleValue ();
}

SGPath
ApplicationProperties::GetCwd () {
    SGPath path("."s);
    char buf[512];
    char* cwd(getcwd(buf, 511));
    buf[511] = '\0';
    if (cwd) {
        path = SGPath::fromLocal8Bit(cwd);
    }
  return path;
}

SGPath
ApplicationProperties::GetRootPath (const char *sub) {
    if (sub != nullptr) {
        const SGPath subpath{std::string(sub)};

        // relative path to current working dir?
        if (subpath.isRelative()) {
            SGPath path(GetCwd());
            path.append(sub);
            if (path.exists()) {
                return path;
            }
        } else if (subpath.exists()) {
            // absolute path
            return subpath;
        }
    }

  // default: relative path to FGROOT
  SGPath path (ApplicationProperties::root);
  if (sub != NULL) {
    path.append (sub);
  }
  return path;
}

string ApplicationProperties::root = ".";
SGPropertyNode_ptr ApplicationProperties::Properties = new SGPropertyNode;
