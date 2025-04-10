#include <stdio.h>
#include <pthread.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#if defined(__GNUC__) && (__GNUC__ > 2) && defined(__OPTIMIZE__)
# define likely(x)   __builtin_expect((x),1)
# define unlikely(x) __builtin_expect((x),0)
# define not_ok(x)   __builtin_expect(x != ret_ok, 0)
# define lt_ok(x)    __builtin_expect(x <  ret_ok, 0)
#else
# define likely(x)   (x)
# define unlikely(x) (x)
# define not_ok(x)   (x != ret_ok)
# define lt_ok(x)    (x <  ret_ok)
#endif

typedef unsigned int        cuint_t;

typedef enum {
	ret_no_sys          = -4,
	ret_nomem           = -3,
	ret_deny            = -2,
	ret_error           = -1,
	ret_ok              =  0,
	ret_eof             =  1,
	ret_eof_have_data   =  2,
	ret_not_found       =  3,
	ret_file_not_found  =  4,
	ret_eagain          =  5,
	ret_ok_and_sent     =  6
} ret_t;

typedef struct {
	char    *buf;        /**< Memory chunk           */
	cuint_t  size;       /**< Total amount of memory */
	cuint_t  len;        /**< Length of the string   */
} cherokee_buffer_t;

/* Declare and allocate a new struct.
 */
#define CHEROKEE_NEW_STRUCT(obj,type)                              \
	CHEROKEE_DCL_STRUCT(obj,type) = (CHEROKEE_MK_TYPE(type) *) \
	malloc (sizeof(CHEROKEE_MK_TYPE(type)));                   \
	return_if_fail (obj != NULL, ret_nomem)

#define REALLOC_EXTRA_SIZE 16

cherokee_buffer_t cherokee_bogonow_strgmt;
char *txt = "Hello";

ret_t cherokee_buffer_init (cherokee_buffer_t *buf)
{
	buf->buf  = NULL;
	buf->len  = 0;
	buf->size = 0;

	return ret_ok;
}

void cherokee_buffer_clean (cherokee_buffer_t *buf)
{
	if (buf->buf != NULL)
		buf->buf[0] = '\0';
	buf->len = 0;
}

static ret_t realloc_inc_bufsize (cherokee_buffer_t *buf, size_t incsize)
{
	char *pbuf;
	size_t newsize = buf->size + incsize + REALLOC_EXTRA_SIZE + 1;

	pbuf = (char *) realloc(buf->buf, newsize);
	if (unlikely (pbuf == NULL))
		return ret_nomem;
	buf->buf = pbuf;
	buf->size = (int) newsize;

	return ret_ok;
}

ret_t cherokee_buffer_add (cherokee_buffer_t *buf, const char *txt, size_t size)
{
	int free = buf->size - buf->len;

	if (unlikely (size <= 0))
		return ret_ok;

	// Get memory
	if ((cuint_t) free < (size+1)) {
		if (unlikely (realloc_inc_bufsize(buf, size - free)) != ret_ok)
			return ret_nomem;
	}

	// Copy
	memcpy (buf->buf + buf->len, txt, size);

	buf->len += size;
	buf->buf[buf->len] = '\0';

	return ret_ok;
}

static void *update_guts (void *arg) {
	cherokee_buffer_add(&cherokee_bogonow_strgmt, txt, strlen(txt));
	return NULL;
}

int main() {
	cherokee_buffer_init(&cherokee_bogonow_strgmt);
	int n = 10;

	for (int i = 0; i < 10; i++) {
		pthread_t thread1, thread2;

		cherokee_buffer_clean(&cherokee_bogonow_strgmt);
		pthread_create(&thread1, NULL, update_guts, NULL);
		pthread_create(&thread2, NULL, update_guts, NULL);

		pthread_join(thread1, NULL);
		pthread_join(thread2, NULL);

		printf("Run %d: buf = %s, len = %d \n", i, cherokee_bogonow_strgmt.buf, cherokee_bogonow_strgmt.len);
	}

	return 0;
}