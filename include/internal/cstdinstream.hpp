/*
 * bit7z - A C++ static library to interface with the 7-zip DLLs.
 * Copyright (c) 2014-2021  Riccardo Ostani - All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * Bit7z is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with bit7z; if not, see https://www.gnu.org/licenses/.
 */

#ifndef CSTDINSTREAM_HPP
#define CSTDINSTREAM_HPP

#include <cstdint>
#include <istream>

#include <7zip/IStream.h>
#include <Common/MyCom.h>

#include "bittypes.hpp"

namespace bit7z {
    using std::istream;

    class CStdInStream : public IInStream, public CMyUnknownImp {
        public:
            explicit CStdInStream( istream& inputStream );

            MY_UNKNOWN_VIRTUAL_DESTRUCTOR( ~CStdInStream() ) = default;

            MY_UNKNOWN_IMP1( IInStream ) // NOLINT(modernize-use-noexcept)

            // IInStream
            STDMETHOD( Read )( void* data, UInt32 size, UInt32* processedSize ) override;

            STDMETHOD( Seek )( Int64 offset, UInt32 seekOrigin, UInt64* newPosition ) override;

        private:
            istream& mInputStream;
    };
}

#endif // CSTDINSTREAM_HPP
