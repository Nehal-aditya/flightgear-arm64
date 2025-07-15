/* 
SPDX-Copyright: Keith Paterson 2025
SPDX-License-Identifier: GPL-2.0-or-later 
*/

#include "testStringUtils.hxx"

#include <Main/globals.hxx>

#include <simgear/timing/sg_time.hxx>

using namespace flightgear;

namespace FGTestApi {

namespace strings {
std::string getTimeString(int timeOffset)
{
    char ret[11];
    time_t rawtime = globals->get_time_params()->get_cur_time();
    rawtime = rawtime + timeOffset;
    tm* timeinfo = gmtime(&rawtime);
    strftime(ret, 11, "%w/%H:%M:%S", timeinfo);
    return ret;
}

} // End of namespace strings.

} // End of namespace FGTestApi.
