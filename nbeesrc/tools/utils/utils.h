/*****************************************************************************/
/*                                                                           */
/* Copyright notice: please read file license.txt in the NetBee root folder. */
/*                                                                           */
/*****************************************************************************/



// Allow including this file only once
#pragma once


#include <stdio.h>
#include <stdarg.h>


#ifdef __cplusplus
extern "C" {
#endif


/*!
	\brief It retrieves the error message after an error occurred in some calls of the operating system.

	This function is defined because of the different way errors are returned in UNIX
	and Win32. This function provides a consistent way to retrieve the error message
	(e.g. after a socket error occurred) on all the platforms.

	\param CallerString: a pointer to a user-allocated string which contains a message that has
	to be printed *before* the true error message. It could be, for example, 'this error
	comes from the recv() call at line 31'. It may be NULL.
	
	\param ErrBuf: a pointer to an user-allocated buffer that will contain the complete
	error message. This buffer has to be at least 'errbuflen' in length.
	It can be NULL; in this case the error cannot be printed.

	This function control that buffer overruns will not occur; moreover, the returned buffer
	is always '0' terminated.

	\param ErrBufSize: length of the buffer that will contains the error. The error message cannot be
	larger than 'errbuflen - 1' because the last char is reserved for the string terminator.

	\return No return values. The error message is returned in the 'string' parameter.

	\note This function is exported because there are calls (like accept() ) which are not managed 
	from inside the sockutils files.
*/
void nbGetLastError(const char *CallerString, char *ErrBuf, int ErrBufSize);


/*!
	\brief It retrieves the error message after an error occurred in some calls of the operating system.

	This function is basically the same as nbGetLastError(), but it defines three more parameters
	that are used to trace exactly where the error occurred (in terms of file name, function name
	and line).

	This function is defined because of the different way errors are returned in UNIX
	and Win32. This function provides a consistent way to retrieve the error message
	(e.g. after a socket error occurred) on all the platforms.

	\param File: name of the file in which this function has been invoked.

	\param Function: function in which this function has been invoked.

	\param Line: line in which this function has been invoked.

	\param CallerString: a pointer to a user-allocated string which contains a message that has
	to be printed *before* the true error message. It could be, for example, 'this error
	comes from the recv() call at line 31'. It may be NULL.
	
	\param ErrBuf: a pointer to an user-allocated buffer that will contain the complete
	error message. This buffer has to be at least 'errbuflen' in length.
	It can be NULL; in this case the error cannot be printed.

	This function control that buffer overruns will not occur; moreover, the returned buffer
	is always '0' terminated.

	\param ErrBufSize: length of the buffer that will contains the error. The error message cannot be
	larger than 'errbuflen - 1' because the last char is reserved for the string terminator.

	\return No return values. The error message is returned in the 'string' parameter.

	\note This function is exported because there are calls (like accept() ) which are not managed 
	from inside the sockutils files.
*/
void nbGetLastErrorEx(char *File, char *Function, int Line, const char *CallerString, char *ErrBuf, int ErrBufSize);


/*!
	\brief Safe-snprintf().

	This function has the same parameters of the snprintf(), but it prevents
	buffer overruns.

	This function guarantees that the returned string is always correctly 
	terminated ('\\0') and the number of characters printed does not exceed BufSize.

	\param Buffer: user-allocated buffer for output.
	\param BufSize: size of the previous buffer.
	\param Format: format-control string.

	\return The number of characters printed (not counting the last '\\0' 
	character), or -1 if the size of the buffer was not enough to contain
	the resulting string.

	\note This function checks if the Buffer is NULL; in this case it does not
	write anything.
*/
int ssnprintf(char* Buffer, int BufSize, const char *Format, ...);	


/*!
	\brief Safe-strncat().

	This function (more or less) has the same parameters of the strncat(), but it prevents
	buffer overruns. For instance, the parameter 'count' in strncat represents the
	maximum number of chars to be appended, and it does not represent the size of the
	resulting buffer.

	This function guarantees that the returned string is always correctly 
	terminated ('\\0) and the number of characters printed does not exceed BufSize.

	\param Buffer: NULL-terminated buffer in which 'Source' will be appended.
	\param BufSize: total size of the previous buffer.
	\param Source: NULL-terminated source string.

	\return The total number of characters (not counting the last '\\0' 
	character) available in the resulting string.

	\warning The last '\\0' character is not included in the number
	of characters copied.
*/
int sstrncat(char *Buffer, char *Source, int BufSize);


/*!
	\brief Safe-strncat().

	This function (more or less) has the same parameters of the strncat(), but it prevents
	buffer overruns. For instance, the parameter 'count' in strncat represents the
	maximum number of chars to be appended, and it does not represent the size of the
	resulting buffer.

	This function guarantees that the returned string is always correctly 
	terminated ('\\0) and the number of characters printed does not exceed BufSize.

	The parameter 'BufOccupancy' is used to jump immediately to the point in which
	we can append data; no checks are done on this parameter except for buffer
	overflows. For instance, if the previously appended string does not end at this
	offset, the strcat will not work properly.

	\param Buffer: NULL-terminated buffer in which 'Source' will be appended.
	\param BufSize: total size of the previous buffer.
	\param BufOccupancy: current number of bytes (not counting the last '\\0' 
	character) currently already present in the 'Buffer' string.
	\param Source: NULL-terminated source string.

	\return No return values; however, the 'BufOccupancy' is updated with the new
	occupancy, which takes into account the total number of characters (not counting 
	the last '\\0' character) available in the resulting string.
	This parameter can therefore be used 'as is' in the next calls to 
	this function (if multiple 'append' operations are needed).
*/
void sstrncat_ex(char *Buffer, int BufSize, int *BufOccupancy, const char *Source);


/*!
	\brief Safe-strncat() with 'escaping' capabilities.

	This function (more or less) has the same parameters of the strncat(), but it prevents
	buffer overruns. For instance, the parameter 'count' in strncat represents the
	maximum number of chars to be appended, and it does not represent the size of the
	resulting buffer.

	Furthermore, if an escape character is found in the Source string,
	it is copied in the output string prepending the 'AddEscapeChar' to it.
	E.g., if the EscapeChar is 'a', the 'AddEscapeChar is 'b' and the source
	string is 'abc', the data copied into 'Buffer' will be 'babc'.

	This function guarantees that the returned string is always correctly 
	terminated ('\\0) and the number of characters printed does not exceed BufSize.

	The parameter 'BufOccupancy' is used to jump immediately to the point in which
	we can append data; no checks are done on this parameter except for buffer
	overflows. For instance, if the previously appended string does not end at this
	offset, the strcat will not work properly.

	\param Buffer: NULL-terminated buffer in which 'Source' will be appended.
	\param BufSize: total size of the previous buffer.
	\param BufOccupancy: current number of bytes (not counting the last '\\0' 
	character) currently already present in the 'Buffer' string.
	\param Source: NULL-terminated source string.
	\param CharToBeEscaped: character that, when found in the 'Source' string, triggers
	the escaping.
	\param EscapeChar: character that is prepended to the 'CharToBeEscaped' when
	that is found in the input string.

	\return No return values; however, the 'BufOccupancy' is updated with the new
	occupancy, which takes into account the total number of characters (not counting 
	the last '\\0' character) available in the resulting string.
	This parameter can therefore be used 'as is' in the next calls to 
	this function (if multiple 'append' operations are needed).
*/
int sstrncatescape_ex(char *Buffer,  int BufSize, int *BufOccupancy, char CharToBeEscaped, char EscapeChar, const char *Source);



/*!
	\brief Safe (and fast) strncpy().

	This function has the same parameters of the strncpy(), but it prevents
	buffer overruns and it gets faster because it avoids copying void characters
	at the end of the string.

	This function guarantees that the returned string is always correctly 
	terminated ('\\0) and the number of characters printed does not exceed BufSize.

	\param Destination: NULL-terminated buffer in which 'Source' will be copied.
	\param DestSize: total size of the previous buffer.
	\param Source: NULL-terminated source string.

	\return The total number of characters (not counting the last '\\0' 
	character) available in the resulting string.

	\warning The last '\\0' character is not included in the number
	of characters copied.
*/
int sstrncpy(char *Destination, char *Source, int DestSize);


/*!
	\brief Case-insensitive strstr()

	This function has the same parameters of the strstr(), but it
	work in case-insensitive mode.

	\param String The source string, in which are are looking for 'Pattern'
	\param Pattern The pattern we are looking for

	\return A pointer to the first occurance of the requested pattern
	(if found), NULL otherwise.
*/
char *stristr(const char *String, const char *Pattern);



#ifdef __cplusplus
}
#endif

