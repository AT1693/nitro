/* =========================================================================
 * This file is part of except-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * except-c++ is free software; you can redistribute it and/or modify
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

#include "except/Throwable.h"

except::Throwable::Throwable(except::Context c)
{
    // Assign c's message to our internal one
    mMessage = c.getMessage();

    // Push context onto exception stack
    mTrace.pushContext(c);
}

except::Throwable::Throwable(const except::Throwable& t, except::Context c)
{
    // Copy t's exception stack and push c onto local one
    mTrace = t.getTrace();
    mTrace.pushContext(c);

    // Assign c's message as our internal one
    mMessage = c.getMessage();
}
