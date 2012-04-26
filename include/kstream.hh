#pragma once

#include "pstream.hh"

class console_stream : public print_stream
{
public:
  constexpr console_stream(bool enabled = true)
    : print_stream(enabled) { }

protected:
  void write(char c);
  void write(sbuf buf);
};

extern console_stream console;

// Errors caused by user processes (page faults, failed system calls,
// etc.)
extern console_stream uerr;
