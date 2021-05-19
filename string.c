/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 * Spring 2021
 *
 * String Utilities
 */

#include "string.h"

/**
 * Sets the first n bytes pointed to by str to the value specified by c
 *
 * @param   dest - pointer the block of memory to set
 * @param   c    - value to set; passed as an integer but converted to unsigned
 * char when set
 * @param   n    - number of bytes to set
 * @return  pointer to the memory region being set; NULL on error
 */

//Parameters:
//pointer memory region returning pointer, c to intialize the memory,num of bytes to the memory region
void *sp_memset(void *dest, int c, size_t n) {
    /* !! Code Needed !! */

//using this function to intialize a region of memory to some known value

    int i;

     if(dest == '\0'){
         return '\0';
     }else{
        for (i = 0; i < n; i++){
            (char*)dest = c;
            dest = (char*)dest++;
        }
    }
    return dest;
}

/**
 * Copies n bytes from the block of memory pointed to by src to dest
 *
 * @param   dest - pointer to the destination block of memory
 * @param   src  - pointer to the source block of memory
 * @param   n    - number of bytes to read/set
 * @return  pointer to the destination memory region; NULL on error
 */
void *sp_memcpy(void *dest, const void *src, size_t n) {
    /* !! Code Needed !! */
    int i;
    
    char *_src = (char *)src;
    char *_dest = (char *)dest;
    if(dest == '\0'){
        return '\0';
    }else{
        for (i = 0; i < n;i++){
            *_dest = *_src;
        }
    }

    return dest;
}


/**
 * Copies the string pointed to by src to the destination dest
 *
 * @param  dest - pointer to the destination string
 * @param  src  - pointer to the source string
 * @return pointer to the destination string
 */

char *sp_strcpy(char *dest, const char *src){

	strcpy(dest, src); //Copies the string pointed to by src to the destination dest
	return dest;
}	

/**
 * Copies up to n characters from the source string src to the destination
 * string dest. If the length of src is less than that of n, the remainder
 * of dest up to n will be filled with NULL characters.
 *
 * @param  dest - pointer to the destination string
 * @param  src  - pointer to the source string
 * @param  n    - maximum number of characters to be copied
 * @return pointer to the destination string
 */
char *sp_strncpy(char *dest, const char *src, size_t n){

    int i;
	//Copies up to n characters from the source string src to the destination string dest
	if(strlen(src) >= n){	//check if it's more or equal to n
		for(i = 0; i < n; i++){
			*dest = *src;
			src = src++;
			dest = dest++;
		}
	}else{	//if less than n
		for(i = 0; i < strlen(src); i++){
			*dest = *src;
			src = src++;
			dest = dest++;
		}
		for(i = strlen(src); i < n; i++){ //fills the rest with NULL
			*dest = '\0';
			dest = dest++;
		}
	}
	return dest;
}

/**
 * Computes the length of the string str up to, but not including the null
 * terminating character
 *
 * @param  str - pointer to the string
 * @return length of the string
 */
size_t sp_strlen(const char *str){

    int i;
    i = strlen(str); //Computes the length of the string str up to, but not including the null terminating character
    return i; //return length

}

