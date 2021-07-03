/*
* Copyright 2021 cod3b453
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice, this
*    list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* 3. Neither the name of the copyright holder nor the names of its contributors
*    may be used to endorse or promote products derived from this software
*    without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "MESCcli.h"

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

static char const * commands[] =
{
    "",             // Empty line
    "W i 1",        // Ideal write command
    "R i",          // Ideal read line
    "W u 5 ",       // Permitted write line
    "R u ",         // Permitted read line

    "W i -5",       // int
    "R i",

    "W u 7",        // uint
    "R u",

    "W f -2.3",     // float
    "R f",
    "W f +45.67",
    "R f",

    "X reset",      // function execution

    "I i 10",       // Increase
    "R i",

    "D f 12.5",     // Decrease
    "R f",
};

static int32_t  i;
static uint32_t u;
static float    f;

static void reset( void )
{
    fprintf( stdout, ">>> RESET <<<\n" );
}

extern int virt_uart_write( void * handle, void * data, uint16_t size );

void bist_cli( void )
{
    fprintf( stdout, "Starting CLI BIST\n" );

    cli_register_variable_rw( "i", &i, sizeof(i), CLI_VARIABLE_INT   );
    cli_register_variable_rw( "u", &u, sizeof(u), CLI_VARIABLE_UINT  );
    cli_register_variable_rw( "f", &f, sizeof(f), CLI_VARIABLE_FLOAT );

    cli_register_function( "reset", reset );

    cli_register_io( NULL, virt_uart_write );

    for ( uint32_t cmd = 0; cmd < (sizeof(commands) / sizeof(commands[0])); ++cmd )
    {
        fprintf( stdout, "Processing command line '%s'\n", commands[cmd] );

        char const * p = commands[cmd];
        int state;

        // Process command string
        for ( uint32_t chr = 0; (p[chr] != '\0') ; ++chr )
        {
            char const c = p[chr];

            state = cli_process( c );
            fprintf( stderr, "CLIState:%d\n", state );
        }

        // Commit command line
        state = cli_process( '\n' );
        fprintf( stderr, "CLIState:%d\n", state );
    }

    fprintf( stdout, "Finished CLI BIST\n" );
}

static int running = 1;

static void i_cli_quit( void )
{
    running = 0;
}

void i_cli( void )
{
    fprintf( stdout, "Starting CLI Interactive\n" );

    cli_register_variable_rw( "i", &i, sizeof(i), CLI_VARIABLE_INT   );
    cli_register_variable_rw( "u", &u, sizeof(u), CLI_VARIABLE_UINT  );
    cli_register_variable_rw( "f", &f, sizeof(f), CLI_VARIABLE_FLOAT );

    cli_register_function(  "reset", reset );
    cli_register_function(  "quit" , i_cli_quit );

    cli_register_io( NULL, virt_uart_write );

    fprintf( stdout, "INFO: Use 'X quit' to exit\n" );

    while (running)
    {
        int const ch = getchar();

        if (ch == EOF)
        {
            running = 0;
        }
        else
        {
            char const c = (char)ch;

            int const state = cli_process( c );
            fprintf( stderr, "CLIState:%d\n", state );
        }
    }

    fprintf( stdout, "Finished CLI Interactive\n" );
}
