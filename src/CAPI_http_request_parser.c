#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "CAPI_internal.h"

// TODO: Handle edge case
// TODO: Check if valid http method
// TODO: Complete the parser implementation
// TODO: update the buffer size

#define GET_AND_CHECK_LINE(line, cursor, error_msg) do { \
    if ((line = CAPI_ChopWithDelimiter(&cursor, '\n')) == NULL) \
    { \
        CAPI_SetError(CAPI_ERR_REQUEST_PARSING, error_msg); \
        return CAPI_ERR_REQUEST_PARSING; \
    } \
} while(0)

#define GET_AND_CHECK_WORD(word, cursor, error_msg) do { \
    if ((word = CAPI_ChopWithDelimiter(&cursor, ' ')) == NULL \
            && (word = CAPI_ChopWithDelimiter(&cursor, '\0')) == NULL) \
    { \
        CAPI_SetError(CAPI_ERR_REQUEST_PARSING, error_msg); \
        return CAPI_ERR_REQUEST_PARSING; \
    } \
} while(0)

#define GET_AND_CHECK_KEY(word, cursor, error_msg) do { \
    if ((line = CAPI_ChopWithDelimiter(&cursor, ':')) == NULL) \
    { \
        CAPI_SetError(CAPI_ERR_REQUEST_PARSING, error_msg); \
        return CAPI_ERR_REQUEST_PARSING; \
    } \
} while(0)

char *CAPI_ChopWithDelimiter(char **buffer, char delimiter)
{
    char *chop_start = *buffer;
    char *chop_end = strchr(chop_start, delimiter);
    
    if (chop_end == NULL)
        return NULL;

    *chop_end = '\0';
    *buffer = chop_end + 1;
    
    return chop_start;
}

static CAPI_ErrorCode CAPI_ParseRequestLine(char *line, CAPI_HttpRequestHeader *header)
{
    char *cursor = line;
    char *word = NULL;

    GET_AND_CHECK_WORD(word, cursor, "Request parser: unable to extract the http method");
    header->http_method = word;
    
    GET_AND_CHECK_WORD(word, cursor, "Request parser: unable to extract the path");
    header->path = word;

    GET_AND_CHECK_WORD(word, cursor, "Request parser: unable to extract the http version");
    header->http_version = word;

    return CAPI_SUCCESS;
}

static CAPI_ErrorCode CAPI_ParseHostLine(char *line, CAPI_HttpRequestHeader *header)
{
    char *cursor = line;
    char *word = NULL;

    GET_AND_CHECK_KEY(word, cursor, "Request parser: unable to find the host");

    // TODO: Check the key (header key are case-insensitive)

    cursor += 1;
    GET_AND_CHECK_WORD(word, cursor, "Request parser: unable to find the host value");
   
    header->host = word;

    return CAPI_SUCCESS;
}

CAPI_ErrorCode CAPI_ParseHttpRequest(char *buffer, size_t buf_len, CAPI_HttpRequest *http_request)
{
    (void) buf_len;

    char *cursor = buffer;
    char *line = NULL;

    GET_AND_CHECK_LINE(line, cursor, "Request parser: unable to extract the request line");

    if (CAPI_ParseRequestLine(line, &http_request->header) != CAPI_SUCCESS)
        return CAPI_GetLastErrorCode();
    
    GET_AND_CHECK_LINE(line, cursor, "Request parser: unable to extract the host line");
    
    if (CAPI_ParseHostLine(line, &http_request->header) != CAPI_SUCCESS)
        return CAPI_GetLastErrorCode();

    return CAPI_SUCCESS;
}

