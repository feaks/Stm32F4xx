/*
MIT License

Copyright(c) 2018 Liam Bindle

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <mqtt.h>
#include <stdio.h>
/** 
 * @file 
 * @brief Implements @ref mqtt_pal_sendall and @ref mqtt_pal_recvall and 
 *        any platform-specific helpers you'd like.
 * @cond Doxygen_Suppress
 */

#include <errno.h>

ssize_t mqtt_pal_sendall(mqtt_pal_socket_handle fd, const void* buf, size_t len, int flags) {
    size_t sent = 0;
    while(sent < len) {
        ssize_t tmp = FreeRTOS_send(fd, (char *)buf + sent, len - sent, flags);
        if (tmp < 1) {
						printf("MQTT_ERROR_SOCKET_ERROR  %d \n",tmp);
            return MQTT_ERROR_SOCKET_ERROR;

        }
				printf("mqtt send :%d \n",tmp);
        sent += (size_t) tmp;
    }
    return sent;
}    

ssize_t mqtt_pal_recvall(mqtt_pal_socket_handle fd, void* buf, size_t bufsz, int flags) {
    const void const *start = buf;
    ssize_t rv;
    do {
        rv = FreeRTOS_recv(fd, buf, bufsz, flags);
        if (rv > 0) {
            /* successfully read bytes from the socket */
            buf = (char *)buf+rv;
            bufsz -= rv;
        } else if (rv < 0 ) {
            /* an error occurred that wasn't "nothing to read". */
						printf("MQTT_ERROR_SOCKET_ERROR  %d \n",rv);
            return MQTT_ERROR_SOCKET_ERROR;
						
        }
				printf("mqtt recv :%d ",rv);
				
				for(uint32_t i=0;i<rv;i++)
					printf("%c",*((char *)buf +i));
				printf("\n");

    } while (rv > 0);
		
    return (uint32_t)buf - (uint32_t)start;
}




/** @endcond */