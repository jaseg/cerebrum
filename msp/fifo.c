/*
* Copyright (c) 2012, Alexander I. Mykyta
* All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met: 
* 
* 1. Redistributions of source code must retain the above copyright notice, this
*    list of conditions and the following disclaimer. 
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution. 
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*==================================================================================================
* File History:
* NAME          DATE         COMMENTS
* Alex M.       04/07/2011   born
* 
*=================================================================================================*/

/**
* \addtogroup MOD_FIFO
* \{
**/

/**
* \file
* \brief Code for \ref MOD_FIFO "FIFO Datapipe"
* \author Alex Mykyta (amykyta3@gmail.com)
**/

#include <msp430.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "result.h"

#include "fifo.h"

//--------------------------------------------------------------------------------------------------
///\addtogroup FIFO_FUNCTIONS
///\{

/**
* \brief Initializes a new FIFO Datapipe
* \param [in] bufptr Pointer to the buffer to be used as storage for the FIFO
* \param [in] bufsize Size of the buffer in bytes
* \param [in] fifo Pointer to an empty #FIFO_t object
* \param [out] fifo Initialized FIFO object
* \return Nothing
**/
void fifo_init(FIFO_t *fifo, register void *bufptr, register size_t bufsize){
	fifo->bufptr = bufptr;
	fifo->bufsize = bufsize;
	fifo->rdidx = 0;
	fifo->wridx = 0;
#if(FIFO_LOG_MAX_USAGE == 1)
	fifo->max = 0;
#endif
}

//--------------------------------------------------------------------------------------------------
/**
* \brief Write data into the FIFO buffer
* \param [in] fifo Pointer to the #FIFO_t object
* \param [in] src Pointer to the data to be stored
* \param [in] size Number of bytes to be written to the FIFO
* \retval RES_OK 
* \retval RES_FULL Not enough space in FIFO for requested write operation
**/
RES_t fifo_write(FIFO_t *fifo, register void *src, register size_t size){
	register size_t wrcount;

		if(size > fifo_wrcount(fifo)){
			return(RES_FULL);
		}

		

		if((wrcount = fifo->bufsize - fifo->wridx) <= size){
			// write operation will wrap around in fifo
			// write first half of fifo
			memcpy(fifo->bufptr+fifo->wridx,src,wrcount);
			
			//wrap around and continue
			fifo->wridx = 0;
			size -= wrcount;
			src = (uint8_t*)src + wrcount;
		}

		if(size > 0){
			memcpy(fifo->bufptr+fifo->wridx,src,size);
			fifo->wridx += size;
		}
		
#if(FIFO_LOG_MAX_USAGE == 1)
		wrcount = fifo_rdcount(fifo);
		if(wrcount > fifo->max){
			fifo->max = wrcount;
		}
#endif
		
		
		return(RES_OK);
}

//--------------------------------------------------------------------------------------------------
/**
* \brief Read data from the FIFO buffer
* \param [in] fifo Pointer to the #FIFO_t object
* \param [out] dst Destination of the data to be read. A \c NULL pointer discards the data.
* \param [in] size Number of bytes to be read from the FIFO
* \retval RES_OK 
* \retval RES_PARAMERR Not enough bytes written in FIFO for requested read operation
**/
RES_t fifo_read(FIFO_t *fifo, register void *dst, register size_t size){
	register size_t rdcount;
	
	if(size > fifo_rdcount(fifo)){
		return(RES_PARAMERR);
	}
	
	if((rdcount = fifo->bufsize - fifo->rdidx) <= size){
		// read operation will wrap around in fifo
		// read first half of fifo
		if(dst != NULL){
			memcpy(dst,fifo->bufptr+fifo->rdidx,rdcount);
		}
		//wrap around and continue
		fifo->rdidx = 0;
		size -= rdcount;
		dst = (uint8_t*)dst + rdcount;
	}
	
	if(size > 0){
		if(dst != NULL){
			memcpy(dst,fifo->bufptr+fifo->rdidx,size);
		}
		fifo->rdidx += size;
	}
	
	return(RES_OK);
}
//--------------------------------------------------------------------------------------------------
/**
* \brief Read data from the FIFO buffer without advancing the read pointer
* \param [in] fifo Pointer to the #FIFO_t object
* \param [out] dst Destination of the data to be read.
* \param [in] size Number of bytes to be read from the FIFO
* \retval RES_OK 
* \retval RES_PARAMERR Not enough bytes written in FIFO for requested read operation
**/
RES_t fifo_peek(FIFO_t *fifo, register void *dst, register size_t size){
	register size_t rdcount;
	register size_t rdidx;
	
	if(size > fifo_rdcount(fifo)){
		return(RES_PARAMERR);
	}
	
	rdidx = fifo->rdidx;
	
	if((rdcount = fifo->bufsize - rdidx) <= size){
		// read operation will wrap around in fifo
		// read first half of fifo
		memcpy(dst,fifo->bufptr+rdidx,rdcount);
		//wrap around and continue
		rdidx = 0;
		size -= rdcount;
		dst = (uint8_t*)dst + rdcount;
	}
	
	if(size > 0){
		memcpy(dst,fifo->bufptr+rdidx,size);
		rdidx += size;
	}
	
	return(RES_OK);
}

//--------------------------------------------------------------------------------------------------
/**
* \brief Empties the FIFO
* \param [in] fifo Pointer to the #FIFO_t object
* \return Nothing
**/
void fifo_clear(FIFO_t *fifo){
	fifo->rdidx = 0;
	fifo->wridx = 0;
}

//--------------------------------------------------------------------------------------------------
/**
* \brief Get the number of bytes available for read in the FIFO
* \param [in] fifo Pointer to the #FIFO_t object
* \return Number of bytes
**/
size_t fifo_rdcount(FIFO_t *fifo){
	register size_t wridx,rdidx;
	wridx = fifo->wridx;
	rdidx = fifo->rdidx;
	
	if(wridx >= rdidx){
		return(wridx-rdidx);
	}else{
		return((fifo->bufsize-rdidx)+wridx);
	}
}

//--------------------------------------------------------------------------------------------------
/**
* \brief Get the number of bytes that can be written to the FIFO
* \param [in] fifo Pointer to the #FIFO_t object
* \return Number of bytes
**/
size_t fifo_wrcount(FIFO_t *fifo){
	register size_t wridx,rdidx;
	wridx = fifo->wridx;
	rdidx = fifo->rdidx;
	
	if(fifo->rdidx >= fifo->wridx+1){
		return(rdidx-wridx-1);
	}else{
		return((fifo->bufsize-wridx)+rdidx-1);
	}
}

///\}
///\}
