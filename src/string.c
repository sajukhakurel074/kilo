#include <kilo_string.h>

#if 0
static void *xmalloc( size_t size ){
     void *x = malloc(size);
     if ( !x ){
//          errExit("Unable to allocate memory!\n");
          perror("Unable to allocate Memory for Strings! ");
     }
     assert( x != NULL );
     return x;
}
#endif

static void *xcalloc(size_t num, size_t size){
     void *x = calloc( num, size );
     if ( !x ){
          perror("Unable to allocate Memory for Strings! ");
     } 
     return x;
}

static void *xrealloc( void *buff,size_t size ){
     void *new = realloc( buff,size );
     if ( !new ){
          perror("Unable to allocate Memory for Strings! ");
     }
     assert( new != NULL );
     return new;
}
char* _init_string( char *str ){
     if ( str != NULL ){
          fprintf(stderr,"Not a null string pointer!\n");
          assert( 0 );
          return NULL;
     } else {
          strHdr *new = xcalloc( 1,sizeof(strHdr ) + MIN_SIZE );
          assert ( new );
          new->len = 0;
          new->cap = MIN_SIZE;
          assert( new->cap >= new->len + 1 ); // the invariant for the whole string process
          return new->str;
     }
}

char *_init_string_with_size( char *str , size_t size ){
     if ( str != NULL ){
          fprintf(stderr,"Not a null string pointer!\n");
          assert( 0 );
          return NULL;
     } else {
          strHdr *new = xcalloc( 1,sizeof(strHdr ) + size );
          assert ( new );
          new->len = 0;
          new->cap = size;
          assert( new->cap >= new->len + 1 ); // the invariant for the whole string process
          return new->str;
     }
}

char *string_grow( strHdr *x, size_t new){
     char *y = NULL;
     if ( x == NULL ){
          str_init(y);
          x = str_hdr(y);
     }
     assert( x );
     size_t alloc_size = MAX( 2*(x->cap) + 1, MAX( MIN_SIZE, new ) );
     assert( alloc_size >= x->cap );
     strHdr *n = xrealloc( x , alloc_size + offsetof( strHdr, str) );
     n->cap = ( alloc_size );
     assert( n->cap >= n->len + 1 ); // the invariant for the whole string process
     return n->str;
}

char *_string_append(char *x, const char *y){
     size_t len = strlen(y);
     size_t size = str_size(x);
     if ( str_size(x) + len > str_cap(x) ){
          x = string_grow( str_hdr(x), size + len );
     }

     assert( str_cap(x) >= size + len );
     memcpy( x + str_len(x) , y , len + 1 );
     str_hdr(x)->len += len;
     return x;
}

char *_string_print(char *x, const char *fmt, ... ){
     va_list args;
     va_start(args,fmt);
     size_t cap = str_cap(x);
     size_t len = vsnprintf(x,cap,fmt,args);
     va_start(args,fmt);
     if ( ( len + 1 ) * sizeof(char)  > cap ){
          x = string_grow( str_hdr(x) , len + 1);
          cap = str_cap(x);
          len = vsnprintf(x,cap,fmt,args);
          assert( len <= cap);
     }
     va_end(args);
     str_hdr(x)->len = len;
     return x;
}

char *_string_app_print( char *x , const char *fmt ,... ){
     va_list args;
     va_start(args,fmt);
     size_t size = str_size(x);
     size_t cap = str_cap(x);
     size_t len = vsnprintf( x + size - 1, cap - size , fmt, args );
     va_start(args,fmt);
     if ( ( len + 1 )*sizeof(char) > cap - size  ){
          x = string_grow( str_hdr(x), cap + size + len );
          cap = str_cap(x);
          len = vsnprintf( x + size - 1, cap - size, fmt, args );
          assert( len <= cap - size );
     }
     va_end(args);
     str_hdr(x)->len += len;
     return x;
}
#if 0
// function deprecated until furthuer notice
char *_stringn_app_print( char *x , size_t n ,  const char *fmt ,... ){
     va_list args;
     va_start(args,fmt);
     size_t size = str_size(x);
     size_t cap = str_cap(x);
     size_t len = vsnprintf( x + size - 1, ( n < cap - size )?( n ):(cap-size) , fmt, args );
     va_start(args,fmt);
     if ( ( len + 1 )*sizeof(char) > cap - size  ){
          x = string_grow( str_hdr(x), cap + size + len );
          cap = str_cap(x);
          len = vsnprintf( x + size - 1, ( ( n ) < cap - size )?( n ):(cap-size), fmt, args );
          assert( len <= cap - size );
     }
     va_end(args);
     str_hdr(x)->len += len;
     return x;
}
#endif

char *_strn_append( char *dest, size_t n, char *src ){
     if ( str_size( dest ) + n > str_cap( dest ) ){
          dest = string_grow( str_hdr( dest ), str_cap(dest) * 2 );
     } 
     char *x  = dest + str_len( dest );
     for ( int i = 0; i < n ; i++ ){
          *x++ = *src++;
     }
     str_hdr(dest)->len += n;
     *x = 0;
     return dest;
}

char *_string_app_char( char *x, char c ){ // append a single character at the end of string
     if ( str_size( x ) + 1 > str_cap( x ) ){
          x = string_grow( str_hdr(x), str_cap(x) * 2 );
     }
     strHdr *s = str_hdr(x);
     x[ s->len ] = c;
     s->len++;
     x[ s->len ] = 0;
     return x;
}

static void swap( char *x, char *y ){
     char temp = *x;
     *x = *y;
     *y = temp;
}


void _delete_char_pos(char *str, size_t pos ){
     size_t len = str_len( str );
     // len is the index of the null character in the string, len - 1 is the last proper character
     if ( pos > len - 1 ) {
          return;
     } else {
          // At the end swaps the deleted character and the '\0' character
          for ( char *s = str + pos; s != str + len; s++ ){
               swap ( s, s+1 );
          }
     }
     ( str_hdr(str)->len )--;
}

char *_insert_char( char *str, char c, size_t pos ){
     if ( str_size( str ) + 1 > str_cap( str ) ){
          str = string_grow( str_hdr( str ), str_cap(str) << 2 );
     }
     size_t len = str_len( str );
     for ( char *s = str + len + 1 ; s != str + pos; s-- ){
          swap( s , s - 1 );
     }
     str[ pos ] = c;
     ( str_hdr( str )->len )++;
     return str;
}

void str_test(void){
     char *s1 = NULL;  
     str_init(s1);
     str_append(s1,"fuck");
     str_app_print( s1, "%s%d\n", "this",32 );
     str_print( s1, "%s\n","Working!");
     char *s2 = NULL;
     str_init( s2 );
     //strn_app_print( s2, 3, "%s","aaaa" );
     strn_app( s2,3,"aaaa"); 
     assert( strcmp( s2, "aaa" ) == 0 );
     str_app_char( s2,'f');
     str_app_char( s2,'u');
     str_app_char( s2,'c');
     str_app_char( s2,'k');
     assert( strcmp( s2,"aaafuck") == 0 );
     str_delete_char_pos( s2, 1 );
     assert( strcmp( s2,"aafuck" ) == 0 );
     str_delete_char_pos( s2, str_len(s2)-1 );
     assert( strcmp( s2, "aafuc" ) == 0 );
     str_insert_char( s2, 'b', 1 );
     assert( strcmp( s2, "abafuc" ) == 0 );
     char *s3 = NULL;
     str_init( s3 );
     str_delete_char_pos( s3, 0 );
     str_insert_char( s3, '3', 0 );
     str_insert_char( s3,'2',0);
     assert( strcmp( s3, "23" ) == 0 );
     //str_append(s1," this shit\n");
     //for ( int i = 0; i < 32; i++ ){
     //     str_append(s1,"!");
     //}
     //str_append(s1,"\n");
     //printf("%s",s1);
//     str_print(s1,"%s\n","fuck");
//     printf("%s",s1); 
     str_free(s1);
     str_free(s2);
}



