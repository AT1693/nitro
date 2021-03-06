/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * sys-c++ is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#if !defined(WIN32)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "sys/OSUnix.h"
#include "sys/File.h"

std::string sys::OSUnix::getPlatformName() const
{
    struct utsname name;
    if (uname(&name) == -1)
        throw sys::SystemException("Uname failed");

    return FmtX("%s (%s): %s [build: %s]", name.sysname, name.machine,
                name.release, name.version);
}

std::string sys::OSUnix::getNodeName() const
{
    struct utsname name;
    if (uname(&name) == -1)
        throw sys::SystemException("Uname failed");

    return std::string(name.nodename);
}


bool sys::OSUnix::exists(const std::string& path) const
{
    struct stat info;
    if (stat(path.c_str(), &info) == -1)
        return false;
    return true;
}

bool sys::OSUnix::remove(const std::string& path) const
{
    if (isDirectory(path))
    {
        if (::rmdir(path.c_str()) == 0)
            return true;
    }
    else
    {
        if (::unlink(path.c_str()) == 0)
            return true;
    }
    return false;
}

bool sys::OSUnix::move(const std::string& path, 
                       const std::string& newPath) const
{
    return rename(path.c_str(), newPath.c_str()) == 0;
}

sys::Pid_T sys::OSUnix::getProcessId() const
{
    return ::getpid();
}

bool sys::OSUnix::makeDirectory(const std::string& path) const
{
    if (::mkdir(path.c_str(), 0777) == 0)
        return true;
    return false;
}

bool sys::OSUnix::isFile(const std::string& path) const
{
    struct stat info;
    if (stat(path.c_str(), &info) == -1)
        return false;
//        throw sys::SystemException("Stat failed");
    return (S_ISREG(info.st_mode)) ? (true) : (false);
}

bool sys::OSUnix::isDirectory(const std::string& path) const
{
    struct stat info;
    if (stat(path.c_str(), &info) == -1)
        return false;
//        throw sys::SystemException("Stat failed");
    return (S_ISDIR(info.st_mode)) ? (true) : (false);
}

std::string sys::OSUnix::getCurrentWorkingDirectory() const
{
    char buffer[PATH_MAX];
    if (!getcwd(buffer, PATH_MAX))
        throw sys::SystemException("Getcwd() failed");
    return std::string(buffer);
}

bool sys::OSUnix::changeDirectory(const std::string& path) const
{
    return chdir(path.c_str()) == 0 ? true : false;
}

std::string sys::OSUnix::getTempName(const std::string& path, 
                                     const std::string& prefix) const
{
    char fullPath[PATH_MAX + 1];

    strcpy(fullPath, path.c_str());
    strcat(fullPath, "/");
    strcat(fullPath, prefix.c_str());
    strcat(fullPath, "XXXXXX");
    if (mkstemp(fullPath) != -1)
        return fullPath;
    return std::string();
}

sys::Off_T sys::OSUnix::getSize(const std::string& path) const
{
    return sys::File(path).length();
}

sys::Off_T sys::OSUnix::getLastModifiedTime(const std::string& path) const
{
    return sys::File(path).lastModifiedTime();
}

void sys::OSUnix::millisleep(int milliseconds) const
{
    usleep(milliseconds * 1000);
}
std::string sys::OSUnix::getDSOSuffix() const
{
    return "so";
}

std::string sys::OSUnix::operator[](const std::string& s) const
{
    return getEnv(s);
}

std::string sys::OSUnix::getEnv(const std::string& s) const
{
    const char* p = getenv(s.c_str());
    if (p == NULL)
        throw sys::SystemException(
            Ctxt(FmtX("Unable to get unix environment variable %s", 
            s.c_str())));
    return std::string(p); 
}

void sys::OSUnix::setEnv(const std::string& var, 
                         const std::string& val,
                         bool overwrite)
{
    int ret;

#ifdef HAVE_SETENV
    ret = setenv(var.c_str(), val.c_str(), overwrite);
#else
    // putenv() will overwrite the value if it already exists, so if we don't
    // want to overwrite, we do nothing when getenv() indicates the variable's
    // already set
    if (overwrite || getenv(var.c_str()) == NULL)
    {
        // putenv() isn't guaranteed to make a copy of the string, so we need
        // to allocate it and let it leak.  Ugh.
        char* const strBuffer = new char[var.length() + 1 + val.length() + 1];
        ::sprintf(strBuffer, "%s=%s", var.c_str(), val.c_str());
        ret = putenv(strBuffer);
    }
    else
    {
        ret = 0;
    }
#endif
    if(ret != 0)
    {
        throw sys::SystemException(Ctxt(
                "Unable to set unix environment variable " + var));
    }
}

size_t sys::OSUnix::getNumCPUs() const
{
#ifdef _SC_NPROCESSORS_ONLN
    return sysconf(_SC_NPROCESSORS_ONLN);
#else
    throw except::NotImplementedException(Ctxt("Unable to get the number of CPUs"));
#endif
}

void sys::DirectoryUnix::close()
{
    if (mDir)
    {
        closedir( mDir);
        mDir = NULL;
    }
}
std::string sys::DirectoryUnix::findFirstFile(const std::string& dir)
{
    // First file is always . on Unix
    mDir = ::opendir(dir.c_str());
    if (mDir == NULL)
        return "";
    return findNextFile();
}

std::string sys::DirectoryUnix::findNextFile()
{
    struct dirent* entry = NULL;
    entry = ::readdir(mDir);
    if (entry == NULL)
        return "";
    return entry->d_name;
}

#endif
