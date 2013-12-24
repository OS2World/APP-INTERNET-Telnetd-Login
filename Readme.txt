// Login.exe. A replacement login.exe for IBM OS/2 telnetd
// Copyright (C) 1995-1996 Paul S. Hethmon and Hethmon Brothers

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

Readme file for Login.exe Version 1.1

Please see the information for Version 1.0 below
for most information.

Changes for Version 1.1:

I've changed the way bad usernames are handled from version 1.0. 
Now, when a bad username is entered, the password is still prompted
for and accepted. This follows the standard of not letting the hacker
know that a username is valid or invalid.

Also changed is that you may now use the backspace character
when entering passwords to erase mistakes. The previous behavior
just considered it another character.

Another change is that the program will delay slightly
when closing to let you read the login message. This is
useful for error messages since telnetd is quick to close
the connection on errors.

The last change I've made is to include the source
code in the distribution (with the exception of crypt.c).
Feel free to modify it to suit your needs. The code is
well commented (I hope) when its not clear what it does.
I know some serious C hackers will think its too wordy
and they could do it better, but to my way of thinking, I'll
write clear code I (and others) can understand and let the
compiler take care of optimizing. Besides the fact its only
a login program ;->

I welcome comments or suggestions.

Paul Hethmon
phethmon@utk.edu
Knoxville, Tennessee USA
February 28, 1996

// ------------------------------------------------------------------

Readme file for Login.exe Version 1.0

This program is a drop-in replacement for telnetd included
with OS/2. Since the base login.exe shipped with Warp Connect
(and TCP/IP 2.0) uses an environment variable, it's not very
secure from the standpoint of someone being able to physically
access your config.sys to find out your password.

This login.exe uses standard crypt to encrypt passwords. It also
allows multiple users to have individual passwords. This does
not enforce any sort of security on the user once that person
is logged in. They still have full access rights to the machine.

One part that is not included with this program is the crypt
dynamic link library. Due to the U.S. export laws, it's a crime
for me to export something which is already freely available in
all parts of the world. So, in order to use this login.exe, you'll
need to either compile the crypt sources into a dll or obtain
a crypt.dll from somewhere else.

As it turns out, a good place to locate a crypt.dll that will
work is with the NeoLogic Network Suite package. It's probably
available from the same place you retrieved this package from.

If you wish to compile the crypt source yourself, I've included
the header file and def file to do so. The source itself is available
on many anonymous ftp sites around the world.

I guess a short explanation would be in order also. The program
requires that you have the ETC environment variable set. This shouldn't
be a problem, tcp/ip sets it by default. The program will look
for a file called "users" in the ETC directory. This file has
the format of

username|password

where the password is encrypted by crypt.

In order to create this file, just run login.exe with an argument
of "-c" for create. It will prompt for a username and password. To
delete a user, run it with an argument of "-d", it will ask for
a username to delete. An argument of "-?" will print a short help
message. No argument will prompt you to login. It returns a return
code of "0" to telnetd to indicate that login was successful and a
"1" otherwise.

I'm releasing this as emailware. If you like it, tell me about it in
an email message.

thanks,

Paul Hethmon
phethmon@utk.edu
October 30, 1995


