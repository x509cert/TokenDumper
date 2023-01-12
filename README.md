# TokenDumper
A simple tool to dump process tokens.

Usage is:

## TokenDumper
This will dump the process token of the current process, which is TokenDumper running as you! Not very exciting.

## TokenDumper -l

That's an 'l' as in list. This will list all the processes on the current machine.

If you're looking for a specific process, you could do something like:

## TokenDumper -l | findstr notepad

## TokenDumper -p nnnn

This will dump the token of the process using it's process identity, which is a number.

## TokenDumper -h
Get help and version info.
