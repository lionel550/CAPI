#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "CAPI_internal.h"

// TODO: Handle edge case
// TODO: Check if valid http method
// TODO: Refactor
// TODO: Complete the parser implementation

static CAPI_ErrorCode CAPI_ParseRequestLine(char *line, CAPI_HttpRequest *request)
{
    char *line_cursor = line;

    char *word_end = strchr(line_cursor, ' ');
    if (word_end == NULL)
    {
        CAPI_SetErrorCode(CAPI_ERR_REQUEST_PARSING, "Request parser: unable to extract the http method");
        return CAPI_ERR_REQUEST_PARSING; 
    }
    *word_end = '\0';
    request->header.http_method = line_cursor;
    line_cursor = word_end + 1;
    
    word_end = strchr(line_cursor, ' ');
    if (word_end == NULL)
    {
        CAPI_SetErrorCode(CAPI_ERR_REQUEST_PARSING, "Request parser: unable to extract the path");
        return CAPI_ERR_REQUEST_PARSING; 
    }
    *word_end = '\0';
    request->header.path = line_cursor;
    line_cursor = word_end + 1;

    word_end = strchr(line_cursor, '\0');
    if (word_end == NULL)
    {
        CAPI_SetErrorCode(CAPI_ERR_REQUEST_PARSING, "Request parser: unable to extract the http version");
        return CAPI_ERR_REQUEST_PARSING; 
    }
    request->header.version = line_cursor;

    return CAPI_SUCCESS;
}

static CAPI_ErrorCode CAPI_ParseHostLine(char *line, CAPI_HttpRequest *request)
{
    char *line_cursor = line;

    char *word_end = strchr(line_cursor, ':');
    if (word_end == NULL)
    {
        CAPI_SetErrorCode(CAPI_ERR_REQUEST_PARSING, "Request parser: unable to find the host");
        return CAPI_ERR_REQUEST_PARSING; 
    }
    line_cursor = word_end + 2;

    word_end = strchr(line_cursor, '\0');

    if (word_end == NULL)
    {
        CAPI_SetErrorCode(CAPI_ERR_REQUEST_PARSING, "Request parser: unable to find the host value");
        return CAPI_ERR_REQUEST_PARSING; 
    }

    request->header.host = line_cursor;

    return CAPI_SUCCESS;
}

CAPI_ErrorCode CAPI_ParseHttpRequest(char *buffer, size_t buf_len, CAPI_HttpRequest *request)
{
    (void) buf_len;

    char *cursor = buffer;

    char *ptr = strchr(cursor, '\n');
    
    if (ptr == NULL)
    {
        CAPI_SetErrorCode(CAPI_ERR_REQUEST_PARSING, "Request parser: unable to extract the request line");
        return CAPI_ERR_REQUEST_PARSING;
    }
    
    *ptr = '\0';
    
    if (CAPI_ParseRequestLine(cursor, request) != CAPI_SUCCESS)
    {
        return CAPI_GetLastErrorCode();
    }
    
    cursor = ptr + 1;

    ptr = strchr(cursor, '\n');
    
    if (ptr == NULL)
    {
        CAPI_SetErrorCode(CAPI_ERR_REQUEST_PARSING, "Request parser: unable to extract the host line");
        return CAPI_ERR_REQUEST_PARSING;
    }
    
    *ptr = '\0';
    
    if (CAPI_ParseHostLine(cursor, request) != CAPI_SUCCESS)
    {
        return CAPI_GetLastErrorCode();
    }

    cursor = ptr + 1;

    return CAPI_SUCCESS;
}

