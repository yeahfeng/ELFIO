/*
Copyright (C) 2001-2020 by Serge Lamikhov-Center

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

#ifndef ELFIO_MODINFO_HPP
#define ELFIO_MODINFO_HPP

#include <string>
#include <vector>

namespace ELFIO {

//------------------------------------------------------------------------------
template <class S> class modinfo_section_accessor_template
{
  public:
    //------------------------------------------------------------------------------
    modinfo_section_accessor_template( S* section_ )
        : modinfo_section( section_ )
    {
        process_section();
    }

    //------------------------------------------------------------------------------
    Elf_Word get_attribute_num() const { return content.size(); }

    //------------------------------------------------------------------------------
    bool
    get_attribute( Elf_Word no, std::string& field, std::string& value ) const
    {
        if ( no < content.size() ) {
            field = content[no].first;
            value = content[no].second;
            return true;
        }

        return false;
    }

    //------------------------------------------------------------------------------
    bool get_attribute( std::string field_name, std::string& value ) const
    {
        for ( auto i = content.begin(); i != content.end(); i++ ) {
            if ( field_name == i->first ) {
                value = i->second;
                return true;
            }
        }

        return false;
    }

    //------------------------------------------------------------------------------
    Elf_Word add_attribute( std::string field, std::string value )
    {
        Elf_Word current_position = 0;

        if ( modinfo_section ) {
            // Strings are addeded to the end of the current section data
            current_position = (Elf_Word)modinfo_section->get_size();

            std::string attribute = field + "=" + value;

            modinfo_section->append_data( attribute + '\0' );
            content.push_back(
                std::pair<std::string, std::string>( field, value ) );
        }

        return current_position;
    }

    //------------------------------------------------------------------------------
  private:
    void process_section()
    {
        const char* pdata = modinfo_section->get_data();
        if ( pdata ) {
            ELFIO::Elf_Xword i = 0;
            while ( i < modinfo_section->get_size() ) {
                while ( i < modinfo_section->get_size() && !pdata[i] )
                    i++;
                if ( i < modinfo_section->get_size() ) {
                    std::string                         info = pdata + i;
                    size_t                              loc  = info.find( '=' );
                    std::pair<std::string, std::string> attribute(
                        info.substr( 0, loc ), info.substr( loc + 1 ) );

                    content.push_back( attribute );

                    i += info.length();
                }
            }
        }
    }

    //------------------------------------------------------------------------------
  private:
    S*                                               modinfo_section;
    std::vector<std::pair<std::string, std::string>> content;
};

using modinfo_section_accessor = modinfo_section_accessor_template<section>;
using const_modinfo_section_accessor =
    modinfo_section_accessor_template<const section>;

} // namespace ELFIO

#endif // ELFIO_MODINFO_HPP
