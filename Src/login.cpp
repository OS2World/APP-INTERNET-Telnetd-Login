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

//
// Login.exe
//
// ------------------------------------------------------------------
//
// Replacement login procedure for telnetd
//

#define INCL_DOS

#include <os2.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <conio.h>
#include <fstream.h>
#include <crypt.h>
#include <netdb.h>

#define ETC "ETC"
#define HOSTNAME "HOSTNAME"
#define PASSWORD_FILE "users"
#define VERSION "Login.exe Version 1.1 by Paul Hethmon <phethmon@utk.edu>"
#define LAG_TIME 1000

int LookUp(char *pszPassFile, char *pszUserName, char *pszEncryptedPassWord);
int CreateUser(char *pszPassFile);
int DeleteUser(char *pszPassFile);

int main(int argc, char *argv[])
{
  char *pszPassFile;
  char *pszUserName;
  char *pszPassWord;
  char *pszEncryptedPassWord;
  char *pszHostName;
  char *pszCryptResult;
  PCSZ pcszEnvPtr;
  APIRET ulRc;
  int i, iRc;
  BOOL bCreateUser = FALSE;
  BOOL bDeleteUser = FALSE;

  pszPassFile = new char[260];
  pszUserName = new char[260];
  pszPassWord = new char[260];
  pszEncryptedPassWord = new char[260];
  pszHostName = new char[260];

  cout.setf(ios::unitbuf);

  i = 1;
  while (i < argc)  // check for command line args
    {
      if (stricmp(argv[i], "-c") == 0)  // create a user
        {
          bCreateUser = TRUE;
        }
      else if (stricmp(argv[i], "-d") == 0) // delete a user
        {
          bDeleteUser = TRUE;
        }
      else if (stricmp(argv[i], "-?") == 0) // print out help
        {
          cout << VERSION << endl;
          cout << "Usage: login [-c | -d | -?]" << endl;
          cout << "-c create user" << endl;
          cout << "-d delete user" << endl;
          cout << "-? show this help" << endl;
          return(1);
        }
      i++;
    }
      
  ulRc = DosScanEnv(ETC, &pcszEnvPtr);
  if (ulRc)
    {
      // Fail if ETC is not defined
      return(1);
    }
  strcpy(pszPassFile, pcszEnvPtr);

  strcat(pszPassFile, "\\");
  strcat(pszPassFile, PASSWORD_FILE); // create password file pathname

  iRc = gethostname(pszHostName, 260);  // first try socket interface
  if (iRc != 0)
    {
      ulRc = DosScanEnv(HOSTNAME, &pcszEnvPtr);  // next try environment
      if (ulRc)
        {
          strcpy(pszHostName, "Somewhere");      // no hostname defined
        }
      else
        {
          strcpy(pszHostName, pcszEnvPtr);
        }
    }

  if (bCreateUser == TRUE)
    {
      iRc = CreateUser(pszPassFile);
      cout << VERSION << endl;
      return (iRc);
    }

  if (bDeleteUser == TRUE)
    {
      iRc = DeleteUser(pszPassFile);
      cout << VERSION << endl;
      return(iRc);
    }

  cout << endl;
  cout << "Welcome to " << pszHostName << "!" << endl << endl;
  cout << "Please enter your login name: ";

  cin >> pszUserName;

  cout << "Please enter your password: ";
  i = 0;
  while (1)
    {
      pszPassWord[i] = _getch();
      if (pszPassWord[i] == '\r')   // they hit enter
        {
          pszPassWord[i] = NULL;
          break;
        }
      else if (pszPassWord[i] == '\b')   // backspace
        {
          i--;          // move back to last good character
          if (i == -1)
            {
              i = 0;    // don't go back too far
            }
          else
            {
              _putch('\b'); // move the cursor back
              _putch(' ');  // blank it out
              _putch('\b'); // move the cursor back
            }
        }
      else                               // just a character
        {
          i++;
          _putch('*');
        }
    }
  cout << endl;

  if (LookUp(pszPassFile, pszUserName, pszEncryptedPassWord))
    {
      cout << "User " << pszUserName << " not logged in." << endl;
      cout << VERSION << endl;
      DosSleep(LAG_TIME);  // stay on screen for awhile
      return(1);
    }

  pszCryptResult = crypt(pszPassWord, pszEncryptedPassWord);

  if (strcmp(pszCryptResult, pszEncryptedPassWord) == 0)
    {
      cout << "User " << pszUserName << " logged in." << endl;
      cout << VERSION << endl;
      return (0);     // they're ok
    }

  cout << "User " << pszUserName << " not logged in." << endl;
  cout << VERSION << endl;
  DosSleep(LAG_TIME);  // stay on screen for awhile

  return(1);
}

// ------------------------------------------------------------------

int LookUp(char *pszPassFile, char *pszUserName, char *pszEncryptedPassWord)
{
  ifstream ifIn;
  char buf[512];
  char field[512];
  int i, j;

  ifIn.open(pszPassFile, ios::nocreate);
  if (! ifIn)
    {
      // fail
      return (1);
    }

  while (! ifIn.eof() )
    {
      // look until we find it
      ifIn.getline(buf, 512, '\n');

      i = 0;
      j = 0;
      while( (buf[i] != '|') && (buf[i] != '\0') && (i < 512) )
        {
          field[j] = buf[i];
          i++;
          j++;
        }
      field[j] = NULL;

      if ( strcmp(pszUserName, field) == 0)
        {
          // we have a match, grab the password
          i++; // go past the pipe
          j = 0;
          while( (buf[i] != ' ') && (buf[i] != '\0') && (i < 512) )
            {
              pszEncryptedPassWord[j] = buf[i];
              i++;
              j++;
            }
          pszEncryptedPassWord[j] = NULL;

          ifIn.close();
          // good return
          return(0);
        }
    }

  ifIn.close();
  // didn't find them in the password file
  return(1);
}

// ------------------------------------------------------------------

int CreateUser(char *pszPassFile)
{
  char *pszUserName;
  char *pszPassWord;
  char *pszEncryptedPassWord;
  char *pszCryptResult;
  char key[5];
  int i;
  ofstream ofOut;
  DATETIME dtInfo;
      
  pszUserName = new char[260];
  pszPassWord = new char[260];
  pszEncryptedPassWord = new char[260];

  cout << "Please enter a new user name: ";

  cin >> pszUserName;

  if (LookUp(pszPassFile, pszUserName, pszEncryptedPassWord) == 0)
    {
      cout << "That user already exists. Exiting." << endl;
      return(1);
    }

  cout << "Please enter the password: ";
  i = 0;
  while (1)
    {
      pszPassWord[i] = _getch();
      if (pszPassWord[i] == '\r')   // they hit enter
        {
          pszPassWord[i] = NULL;
          break;
        }
      else if (pszPassWord[i] == '\b')   // backspace
        {
          i--;          // move back to last good character
          if (i == -1)
            {
              i = 0;    // don't go back too far
            }
          else
            {
              _putch('\b'); // move the cursor back
              _putch(' ');  // blank it out
              _putch('\b'); // move the cursor back
            }
        }
      else                               // just a character
        {
          i++;
          _putch('*');
        }
    }
  cout << endl;

  cout << "Please enter the password again: ";
  i = 0;
  while (1)
    {
      pszEncryptedPassWord[i] = _getch();
      if (pszEncryptedPassWord[i] == '\r')   // they hit enter
        {
          pszEncryptedPassWord[i] = NULL;
          break;
        }
      else if (pszEncryptedPassWord[i] == '\b')   // backspace
        {
          i--;          // move back to last good character
          if (i == -1)
            {
              i = 0;    // don't go back too far
            }
          else
            {
              _putch('\b'); // move the cursor back
              _putch(' ');  // blank it out
              _putch('\b'); // move the cursor back
            }
        }
      else                               // just a character
        {
          i++;
          _putch('*');
        }
    }
  cout << endl;

  if (strcmp(pszEncryptedPassWord, pszPassWord) != 0)
    {
      // passwords don't match
      cout << "Your password's don't match. Exiting." << endl;
      return(1);
    }

  // now set the password
  DosGetDateTime(&dtInfo);
  srand(dtInfo.hundredths);
  key[0] = (rand()) % 26 + 65;
  key[1] = (rand()) % 26 + 97;
  key[2] = NULL;

  pszCryptResult = crypt(pszPassWord, key);

  ofOut.open(pszPassFile, ios::app);

  if (! ofOut)
    {
      cout << "User creation failed. Could not open password file." << endl;
      return (1);
    }

  ofOut << pszUserName << "|" << pszCryptResult << endl;

  ofOut.close();

  cout << "User " << pszUserName << " created." << endl;

  return (0);
}

// ------------------------------------------------------------------

int DeleteUser(char *pszPassFile)
{
  char *pszUserName;
  char *pszTmpFile;
  char *pszEncryptedPassWord;
  int i, j;
  ofstream ofOut;
  ifstream ifIn;
  char buf[512];
  char field[512];
  APIRET ulRc;
      
  pszUserName = new char[260];
  pszEncryptedPassWord = new char[260];
  pszTmpFile = new char[260];

  cout << "Please enter the user name to delete: ";

  cin >> pszUserName;

  if (LookUp(pszPassFile, pszUserName, pszEncryptedPassWord) > 0)
    {
      cout << "That user does not exist. Exiting." << endl;
      return(1);
    }

  ifIn.open(pszPassFile, ios::nocreate);
  if (! ifIn)
    {
      // fail
      cout << "User deletion failed. Could not open password file." << endl;
      return (1);
    }

  strcpy(pszTmpFile, pszPassFile);
  strcat(pszTmpFile, ".tmp");
  ofOut.open(pszTmpFile, ios::noreplace);
  if (!ofOut)
    {
      ifIn.close();
      // fail
      cout << "Could not create temporary file: " << pszTmpFile << endl;
      cout << "Please check for this filename and remove it if found." << endl;
      cout << "Exiting." << endl;
      return(1);
    }

  while (! ifIn.eof() )
    {
      // look until we find it
      ifIn.getline(buf, 512, '\n');

      i = 0;
      j = 0;
      while( (buf[i] != '|') && (buf[i] != '\0') && (i < 512) )
        {
          field[j] = buf[i];
          i++;
          j++;
        }
      field[j] = NULL;

      if ( strcmp(pszUserName, field) != 0)
        {
          // keep this user
          ofOut << buf << endl;
        }
    }
  
  ofOut.close();
  ifIn.close();

  DosDelete(pszPassFile);
  ulRc = DosMove(pszTmpFile, pszPassFile);
  if (ulRc)
    {
      cout << "Error removing user." << endl;
      cout << "ulrc = " << ulRc << endl;
      return(1);
    }

  cout << "User " << pszUserName << " deleted." << endl;

  return (0);
}


  


