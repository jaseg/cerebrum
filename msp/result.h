/**
* \file
* \brief Defines some generic result codes
* \author Alex Mykyta
* \date 7/1/2011
* 
* This file includes some generic function responses that are used universally thoughout all of 
* the modules. See function-specific documentation for more detailed descriptions regarding the
* responses.
**/

#ifndef RESULT_H_
#define RESULT_H_

typedef enum{
	RES_OK,			///< Function executed successfully
	RES_FAIL,		///< Function failed to execute properly
	RES_INVALID,	///< Parameters returned are not valid
	RES_NOTFOUND,	///< Item requested was not found
	RES_FULL,		///< Buffer/Memory/other is full
	RES_UNDERRUN,	///< Buffer underrun has occurred
	RES_OVERRUN,	///< Buffer overrun has occured
	RES_PARAMERR,	///< Invalid input parameter
	RES_END,		///< Reached the end of a buffer
	RES_BUSY,		///< Device is busy
	RES_CANCEL,		///< Operation has been cancelled
	RES_UNKNOWN		///< Unknown Error
} RES_t;

#endif /*RESULT_H_*/
