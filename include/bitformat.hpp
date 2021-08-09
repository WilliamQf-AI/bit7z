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

#ifndef BITFORMAT_HPP
#define BITFORMAT_HPP

#include <bitset>

#include <type_traits>

#include "bitcompressionmethod.hpp"
#include "bitguids.hpp"
#include "bittypes.hpp"

namespace bit7z {
    /**
     * @brief The FormatFeatures enum specifies the features supported by an archive file format.
     */
    enum struct FormatFeatures : unsigned {
        MULTIPLE_FILES    = 1 << 0,///< The format can compress/extract multiple files         (2^0 = 0000001)
        SOLID_ARCHIVE     = 1 << 1,///< The format supports solid archives                     (2^1 = 0000010)
        COMPRESSION_LEVEL = 1 << 2,///< The format is able to use different compression levels (2^2 = 0000100)
        ENCRYPTION        = 1 << 3,///< The format supports archive encryption                 (2^3 = 0001000)
        HEADER_ENCRYPTION = 1 << 4,///< The format can encrypt the file names                  (2^4 = 0010000)
        MULTIPLE_METHODS  = 1 << 5 ///< The format can use different compression methods       (2^6 = 0100000)
    };

    template< typename E >
    inline constexpr auto to_underlying( E e ) noexcept {
        return static_cast< std::underlying_type_t< E > >(e);
    }

    inline constexpr FormatFeatures operator|( FormatFeatures lhs, FormatFeatures rhs ) noexcept {
        return static_cast< FormatFeatures >( to_underlying( lhs ) | to_underlying( rhs ) );
    }

    inline constexpr auto operator&( FormatFeatures lhs, FormatFeatures rhs ) noexcept {
        return to_underlying( lhs ) & to_underlying( rhs );
    }

    /**
     * @brief The BitInFormat class specifies an extractable archive format.
     *
     * @note Usually, the user of the library should not create new formats and, instead,
     * use the ones provided by the BitFormat namespace.
     */
    class BitInFormat {
        public:
            //non-copyable
            BitInFormat( const BitInFormat& other ) = delete;

            BitInFormat& operator=( const BitInFormat& other ) = delete;

            //non-movable
            BitInFormat( BitInFormat&& other ) = delete;

            BitInFormat& operator=( BitInFormat&& other ) = delete;

            /**
             * @brief Constructs a BitInFormat object with the id value used by the 7z SDK.
             * @param value  the value of the format in the 7z SDK.
             */
            explicit constexpr BitInFormat( unsigned char value ) noexcept: mValue( value ) {}

            /**
             * @return the value of the format in the 7z SDK.
             */
            int value() const noexcept;

            /**
             * @return the GUID that identifies the file format in the 7z SDK.
             */
            GUID guid() const noexcept;

            /**
             * @param other  the target object to compare to.
             * @return true if this format is equal to "other".
             */
            bool operator==( BitInFormat const& other ) const noexcept;

            /**
             * @param other  the target object to compare to.
             * @return true if this format is not equal to "other".
             */
            bool operator!=( BitInFormat const& other ) const noexcept;

        private:
            const unsigned char mValue;
    };

    /**
     * @brief The BitInOutFormat class specifies a format available for creating new archives and extract old ones
     *
     * @note Usually, the user of the library should not create new formats and, instead,
     * use the ones provided by the BitFormat namespace
     */
    class BitInOutFormat : public BitInFormat {
        public:
            /**
             * @brief Constructs a BitInOutFormat object with a id value, an extension and a set of supported features
             *
             * @param value         the value of the format in the 7z SDK
             * @param ext           the default file extension of the archive format
             * @param defaultMethod the default compression method of the archive format.
             * @param features      the set of features supported by the archive format
             */
            constexpr BitInOutFormat( unsigned char value,
                                      const tchar* ext,
                                      BitCompressionMethod defaultMethod,
                                      FormatFeatures features ) noexcept
                : BitInFormat( value ), mExtension( ext ), mDefaultMethod( defaultMethod ), mFeatures( features ) {}

            //non-copyable
            BitInOutFormat( const BitInFormat& other ) = delete;

            BitInOutFormat& operator=( const BitInOutFormat& other ) = delete;

            //non-movable
            BitInOutFormat( BitInOutFormat&& other ) = delete;

            BitInOutFormat& operator=( BitInOutFormat&& other ) = delete;

            /**
             * @return the default file extension of the archive format
             */
            const tchar* extension() const noexcept;

            /**
             * @return the bitset of the features supported by the format
             */
            FormatFeatures features() const noexcept;

            /**
             * @brief Checks if the format has a specific feature (see FormatFeatures enum)
             * @param feature   feature to be checked
             * @return true if the format has the feature, false otherwise
             */
            bool hasFeature( FormatFeatures feature ) const noexcept;

            /**
             * @return the default compression method of the archive format.
             */
            BitCompressionMethod defaultMethod() const noexcept;

        private:
            const tchar* mExtension;
            const BitCompressionMethod mDefaultMethod;
            const FormatFeatures       mFeatures;
    };

    /**
     * @brief The namespace BitFormat contains a set of archive formats usable with bit7z classes
     */
    namespace BitFormat {
#ifdef __cpp_inline_variables
#ifdef BIT7Z_AUTO_FORMAT
        inline constexpr BitInFormat Auto( 0x00 );
#endif

        inline constexpr BitInOutFormat Zip( 0x01, TSTRING( ".zip" ), BitCompressionMethod::Deflate,
                                             FormatFeatures::MULTIPLE_FILES | FormatFeatures::COMPRESSION_LEVEL |
                                             FormatFeatures::ENCRYPTION | FormatFeatures::MULTIPLE_METHODS );
        inline constexpr BitInOutFormat BZip2( 0x02, TSTRING( ".bz2" ), BitCompressionMethod::BZip2,
                                               FormatFeatures::COMPRESSION_LEVEL );
        inline constexpr BitInFormat    Rar( 0x03 );
        inline constexpr BitInFormat    Arj( 0x04 ); //-V112
        inline constexpr BitInFormat    Z( 0x05 );
        inline constexpr BitInFormat    Lzh( 0x06 );
        inline constexpr BitInOutFormat SevenZip( 0x07, TSTRING( ".7z" ), BitCompressionMethod::Lzma2,
                                                  FormatFeatures::MULTIPLE_FILES |
                                                  FormatFeatures::SOLID_ARCHIVE |
                                                  FormatFeatures::COMPRESSION_LEVEL |
                                                  FormatFeatures::ENCRYPTION |
                                                  FormatFeatures::HEADER_ENCRYPTION |
                                                  FormatFeatures::MULTIPLE_METHODS );
        inline constexpr BitInFormat    Cab( 0x08 );
        inline constexpr BitInFormat    Nsis( 0x09 );
        inline constexpr BitInFormat    Lzma( 0x0A );
        inline constexpr BitInFormat    Lzma86( 0x0B );
        inline constexpr BitInOutFormat Xz( 0x0C, TSTRING( ".xz" ), BitCompressionMethod::Lzma2,
                                            FormatFeatures::COMPRESSION_LEVEL );
        inline constexpr BitInFormat    Ppmd( 0x0D );
        inline constexpr BitInFormat    COFF( 0xC6 );
        inline constexpr BitInFormat    Ext( 0xC7 );
        inline constexpr BitInFormat    VMDK( 0xC8 );
        inline constexpr BitInFormat    VDI( 0xC9 );
        inline constexpr BitInFormat    QCow( 0xCA );
        inline constexpr BitInFormat    GPT( 0xCB );
        inline constexpr BitInFormat    Rar5( 0xCC );
        inline constexpr BitInFormat    IHex( 0xCD );
        inline constexpr BitInFormat    Hxs( 0xCE );
        inline constexpr BitInFormat    TE( 0xCF );
        inline constexpr BitInFormat    UEFIc( 0xD0 );
        inline constexpr BitInFormat    UEFIs( 0xD1 );
        inline constexpr BitInFormat    SquashFS( 0xD2 );
        inline constexpr BitInFormat    CramFS( 0xD3 );
        inline constexpr BitInFormat    APM( 0xD4 );
        inline constexpr BitInFormat    Mslz( 0xD5 );
        inline constexpr BitInFormat    Flv( 0xD6 );
        inline constexpr BitInFormat    Swf( 0xD7 );
        inline constexpr BitInFormat    Swfc( 0xD8 );
        inline constexpr BitInFormat    Ntfs( 0xD9 );
        inline constexpr BitInFormat    Fat( 0xDA );
        inline constexpr BitInFormat    Mbr( 0xDB );
        inline constexpr BitInFormat    Vhd( 0xDC );
        inline constexpr BitInFormat    Pe( 0xDD );
        inline constexpr BitInFormat    Elf( 0xDE );
        inline constexpr BitInFormat    Macho( 0xDF );
        inline constexpr BitInFormat    Udf( 0xE0 );
        inline constexpr BitInFormat    Xar( 0xE1 );
        inline constexpr BitInFormat    Mub( 0xE2 );
        inline constexpr BitInFormat    Hfs( 0xE3 );
        inline constexpr BitInFormat    Dmg( 0xE4 );
        inline constexpr BitInFormat    Compound( 0xE5 );
        inline constexpr BitInOutFormat Wim( 0xE6, TSTRING( ".wim" ), BitCompressionMethod::Copy,
                                             FormatFeatures::MULTIPLE_FILES );
        inline constexpr BitInFormat    Iso( 0xE7 );
        inline constexpr BitInFormat    Chm( 0xE9 );
        inline constexpr BitInFormat    Split( 0xEA );
        inline constexpr BitInFormat    Rpm( 0xEB );
        inline constexpr BitInFormat    Deb( 0xEC );
        inline constexpr BitInFormat    Cpio( 0xED );
        inline constexpr BitInOutFormat Tar( 0xEE, TSTRING( ".tar" ), BitCompressionMethod::Copy,
                                             FormatFeatures::MULTIPLE_FILES );
        inline constexpr BitInOutFormat GZip( 0xEF, TSTRING( ".gz" ), BitCompressionMethod::Deflate,
                                              FormatFeatures::COMPRESSION_LEVEL );
#else
#ifdef BIT7Z_AUTO_FORMAT
        extern const BitInFormat Auto;      ///< Automatic Format Detection (available only when compiling bit7z using the BIT7Z_AUTO_FORMAT preprocessor define)
#endif
        extern const BitInFormat Rar,       ///< RAR Archive Format
                                 Arj,       ///< ARJ Archive Format
                                 Z,         ///< Z Archive Format
                                 Lzh,       ///< LZH Archive Format
                                 Cab,       ///< CAB Archive Format
                                 Nsis,      ///< NSIS Archive Format
                                 Lzma,      ///< LZMA Archive Format
                                 Lzma86,    ///< LZMA86 Archive Format
                                 Ppmd,      ///< PPMD Archive Format
                                 COFF,      ///< COFF Archive Format
                                 Ext,       ///< EXT Archive Format
                                 VMDK,      ///< VMDK Archive Format
                                 VDI,       ///< VDI Archive Format
                                 QCow,      ///< QCOW Archive Format
                                 GPT,       ///< GPT Archive Format
                                 Rar5,      ///< RAR5 Archive Format
                                 IHex,      ///< IHEX Archive Format
                                 Hxs,       ///< HXS Archive Format
                                 TE,        ///< TE Archive Format
                                 UEFIc,     ///< UEFIc Archive Format
                                 UEFIs,     ///< UEFIs Archive Format
                                 SquashFS,  ///< SquashFS Archive Format
                                 CramFS,    ///< CramFS Archive Format
                                 APM,       ///< APM Archive Format
                                 Mslz,      ///< MSLZ Archive Format
                                 Flv,       ///< FLV Archive Format
                                 Swf,       ///< SWF Archive Format
                                 Swfc,      ///< SWFC Archive Format
                                 Ntfs,      ///< NTFS Archive Format
                                 Fat,       ///< FAT Archive Format
                                 Mbr,       ///< MBR Archive Format
                                 Vhd,       ///< VHD Archive Format
                                 Pe,        ///< PE Archive Format
                                 Elf,       ///< ELF Archive Format
                                 Macho,     ///< MACHO Archive Format
                                 Udf,       ///< UDF Archive Format
                                 Xar,       ///< XAR Archive Format
                                 Mub,       ///< MUB Archive Format
                                 Hfs,       ///< HFS Archive Format
                                 Dmg,       ///< DMG Archive Format
                                 Compound,  ///< COMPOUND Archive Format
                                 Iso,       ///< ISO Archive Format
                                 Chm,       ///< CHM Archive Format
                                 Split,     ///< SPLIT Archive Format
                                 Rpm,       ///< RPM Archive Format
                                 Deb,       ///< DEB Archive Format
                                 Cpio;      ///< CPIO Archive Format

        extern const BitInOutFormat Zip,        ///< ZIP Archive Format
                                    BZip2,      ///< BZIP2 Archive Format
                                    SevenZip,   ///< 7Z Archive Format
                                    Xz,         ///< XZ Archive Format
                                    Wim,        ///< WIM Archive Format
                                    Tar,        ///< TAR Archive Format
                                    GZip;       ///< GZIP Archive Format
#endif
    }


#ifdef BIT7Z_AUTO_FORMAT
#define DEFAULT_FORMAT = BitFormat::Auto
#else
#define DEFAULT_FORMAT
#endif

}
#endif // BITFORMAT_HPP
