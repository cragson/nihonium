#pragma once
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

namespace StringUtils
{
    inline void remove_non_printable_chars_from_string( std::string& str )
    {
        str.erase(
            std::remove_if(
                str.begin(), 
                str.end(),
                [](unsigned char c){ return !std::isprint( c ); }
            ),
            str.end()
        );
    }

    constexpr auto string_to_lower = []( std::string& str )
	{
		std::ranges::transform( str, str.begin(), []( auto c )
		                        {
			                        return std::tolower( c );
		                        }
		);
	};

	constexpr auto vec_to_lower = []( std::vector< std::string >& vec )
	{
		std::ranges::for_each( vec, string_to_lower );
	};

	constexpr auto split_string = []( const std::string& user_input )
	{
		std::vector< std::string > parsed_args = {};

		if( user_input.empty() )
			return parsed_args;

		std::string parsed_input = {};

		for( const auto idx : user_input )
		{
			if( idx == ' ' )
			{
				parsed_args.push_back( parsed_input );

				parsed_input = "";
			}
			else
				parsed_input += idx;
		}

		// Need to push back one more time, to don't lose the last parsed word
		parsed_args.push_back( parsed_input );

		return parsed_args;
	};

    [[nodiscard]] inline auto str_to_ul( const std::string& input )
	{
        std::uintptr_t ret = {};

        try
        {
            ret = input.contains( "0x" ) ? std::stoull( input, nullptr, 16 ) : std::stoull( input, nullptr, 10 );
        }
        catch( std::exception e )
        {
            std::println( "[StringUtils] Caught exception from invalid stoull conversion: {}", input );
        }

        return ret;
	}

    [[nodiscard]] inline auto str_to_l( const std::string& input )
	{
        std::uintptr_t ret = {};

        try
        {
            ret = input.contains( "0x" ) ? std::stoll( input, nullptr, 16 ) : std::stoll( input, nullptr, 10 );
        }
        catch( std::exception e )
        {
            std::println( "[StringUtils] Caught exception from invalid stoull conversion: {}", input );
        }

        return ret;
	}

    // Credits: https://www.geeksforgeeks.org/remove-extra-spaces-string/
    inline void removeSpaces( std::string& str )
    {
        // n is length of the original string
        auto n = str.length();

        // i points to next position to be filled in
        // output string/ j points to next character
        // in the original string
        int i = 0, j = -1;

        // flag that sets to true is space is found
        bool spaceFound = false;

        // Handles leading spaces
        while( ++j < n && str[ j ] == ' ' );

        // read all characters of original string
        while( j < n )
        {
            // if current characters is non-space
            if( str[ j ] != ' ' )
            {
                // remove preceding spaces before dot,
                // comma & question mark
                if( ( str[ j ] == '.' || str[ j ] == ',' || str[ j ] == '?' ) && i - 1 >= 0 && str[ i - 1 ] == ' ' )
                    str[ i - 1 ] = str[ j++ ];

                else
                    // copy current character at index i
                    // and increment both i and j
                    str[ i++ ] = str[ j++ ];

                // set space flag to false when any
                // non-space character is found
                spaceFound = false;
            }
            // if current character is a space
            else if( str[ j++ ] == ' ' )
            {
                // If space is encountered for the first
                // time after a word, put one space in the
                // output and set space flag to true
                if( !spaceFound )
                {
                    str[ i++ ] = ' ';
                    spaceFound = true;
                }
            }
        }

        // Remove trailing spaces
        if( i <= 1 )
            str.erase( str.begin() + i, str.end() );
        else
            str.erase( str.begin() + i - 1, str.end() );
    }
}

