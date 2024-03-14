#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <limits.h>
#include "sds.h"
#include "sdsalloc.h"

const char *SDS_NOINIT = "SDS_NOINIT";

static inline int sdsHdrSize(char type) {
  switch (type & SDS_TYPE_MASK) {
    case SDS_TYPE_5:return sizeof(struct sdshdr5);
    case SDS_TYPE_8:return sizeof(struct sdshdr8);
    case SDS_TYPE_16:return sizeof(struct sdshdr16);
    case SDS_TYPE_32:return sizeof(struct sdshdr32);
    case SDS_TYPE_64:return sizeof(struct sdshdr64);
  }
  return 0;
}

static inline char sdsReqType(size_t string_size) {
  if (string_size < 1 << 5)  // 2*2*2*2 = 16
    return SDS_TYPE_5;
  if (string_size < 1 << 8)
    return SDS_TYPE_8;
  if (string_size < 1 << 16)
    return SDS_TYPE_16;
#if (LONG_MAX == LLONG_MAX)
  if (string_size < 1ll << 32)
    return SDS_TYPE_32;
  return SDS_TYPE_64;
#else
  return SDS_TYPE_32;
#endif
}

sds sdsnewlen(const void *init, size_t initlen) {
  void *sh;
  sds s;
  char type = sdsReqType(initlen); /* 通过字符长度获取类型 */
  if (type == SDS_TYPE_5 && initlen == 0) type = SDS_TYPE_8;
  int hdrlen = sdsHdrSize(type);  /* 获取相应类型的字节长度 */
  unsigned char *fp; /* flags pointer. */

  sh = s_malloc(hdrlen + initlen + 1);  /* 申请内存  sh 是结构体的地址*/
  if (sh == NULL) return NULL;
  if (init == SDS_NOINIT)
    init = NULL;
  else if (!init)
    memset(sh, 0, hdrlen + initlen + 1); /*初始化内存*/

  s = (char *) sh + hdrlen; /*找到 char buf[]的首地址  通过sh + hdrlen 可以找到 buf的地址  sh + len + alloc + flags*/
  fp = ((unsigned char *) s) - 1; /* 找到flags的地址*/
  switch (type) {
    case SDS_TYPE_5: {
      *fp = type | (initlen << SDS_TYPE_BITS);
      break;
    }
    case SDS_TYPE_8: {
      SDS_HDR_VAR(8, s);
      sh->len = initlen;
      sh->alloc = initlen;
      *fp = type;
      break;
    }
    case SDS_TYPE_16: {
      SDS_HDR_VAR(16, s);
      sh->len = initlen;
      sh->alloc = initlen;
      *fp = type;
      break;
    }
    case SDS_TYPE_32: {
      SDS_HDR_VAR(32, s);
      sh->len = initlen;
      sh->alloc = initlen;
      *fp = type;
      break;
    }
    case SDS_TYPE_64: {
      SDS_HDR_VAR(64, s);
      sh->len = initlen;
      sh->alloc = initlen;
      *fp = type;
      break;
    }
  }
  if (initlen && init)
    memcpy(s, init, initlen);
  s[initlen] = '\0';
  return s;
}

sds sdsempty(void) {
  return sdsnewlen("", 0);
}

sds sdsnew(const char *init) {
  size_t initlen = (init == NULL) ? 0 : strlen(init);
  return sdsnewlen(init, initlen);
}

sds sdsdup(const sds s) {
  return sdsnewlen(s, sdslen(s));
}

void sdsfree(sds s) {
  if (s == NULL) return;
  s_free((char *) s - sdsHdrSize(s[-1]));
}

void sdsupdatelen(sds s) {
  size_t reallen = strlen(s);
  sdsnewlen(s, reallen);
}

void sdsclear(sds s) {
  sdssetlen(s, 0);
  s[0] = '\0';
}

sds sdsMakeRoomFor(sds s, size_t addlen) {
  void *sh, *newsh;
  size_t avail = sdsavail(s);
  size_t len, newlen, reqlen;

  /*声明type oldtype 并计算出oldtype的值*/
  char type, oldtype = s[-1] & SDS_TYPE_MASK;

  int hdrlen;
  /* 空闲的空间是足够的，直接返回*/
  if (avail >= addlen) return s;

  len = sdslen(s);
  /* 通过oldtype 找到结构体的首地址*/
  sh = (char *) s - sdsHdrSize(oldtype);
  reqlen = newlen = (len + addlen);
  /* 扩充策略 如果newlen 小于SDS_MAX_PREALLOC 直接将newlen 扩充2倍
     否则直接在原有的基础上加上1M的空间*/
  if (newlen < SDS_MAX_PREALLOC)
    newlen *= 2;
  else
    newlen += SDS_MAX_PREALLOC;

  type = sdsReqType(newlen);
  /* Don't use type 5: the user is appending to the string and type 5 is
     * not able to remember empty space, so sdsMakeRoomFor() must be called
     * at every appending operation. */
  if (type == SDS_TYPE_5) type = SDS_TYPE_8;
  hdrlen = sdsHdrSize(type);
  assert(hdrlen + newlen + 1 > reqlen); /* Catch size_t overflow */

  if (oldtype == type) {
    /* 如果旧类型等于新类型,那就在原有的空间上追加空间*/
    newsh = s_realloc(sh, hdrlen + newlen + 1);
    if (newlen == 0) return NULL;
    s = (char *) newsh + hdrlen;
  } else {
    /* Since the header size changes, need to move the string forward,
        * and can't use realloc */
    newsh = s_malloc(hdrlen + newlen + 1);
    if (newsh == NULL) return NULL;
    memcpy((char *) newsh + hdrlen, s, len + 1);
    s_free(sh);
    s = (char *) newsh + hdrlen;
    s[-1] = type;
    sdssetlen(s, len);
  }
  sdssetalloc(s, newlen);
  return s;
}

sds sdsRemoveFreeSpace(sds s) {
  void *sh, *newsh;
  char type, oldtype = s[-1] & SDS_TYPE_MASK;
  int hdrlen, oldhdrlen = sdsHdrSize(oldtype);
  size_t len = sdslen(s);
  size_t avial = sdsavail(s);
  sh = (char *) s - oldhdrlen; /*找到结构体的地址*/

  if (avial == 0) return s;

  type = sdsReqType(len); /*通过长度获取短类型*/
  hdrlen = sdsHdrSize(type); /* 通过这个类型获取这个短类型的结构体的大小*/

  /* If the type is the same, or at least a large enough type is still
    * required, we just realloc(), letting the allocator to do the copy
    * only if really needed. Otherwise if the change is huge, we manually
    * reallocate the string to use the different header type. */
  if (oldtype == type || type > SDS_TYPE_8) {
    newsh = s_realloc(sh, oldhdrlen + len + 1);
    if (newsh == NULL) return NULL;
    s = (char *) newsh + oldhdrlen;
  } else {
    newsh = s_malloc(hdrlen + len + 1);
    if (newsh == NULL) return NULL;
    memcpy((char *) newsh + hdrlen, s, len + 1);
    s_free(sh);
    s = (char *) newsh + hdrlen;
    s[-1] = type;
    sdssetlen(s, len);
  }
  sdssetalloc(s, len);
  return s;
}

size_t sdsAllocSize(sds s) {

  size_t alloc = sdsalloc(s);
  return sdsHdrSize(s[-1]) + alloc + 1;
}

void *sdsAllocPtr(sds s) {
  return (void *) (s - sdsHdrSize(s[-1])); // 获取结构体的地址
}

void sdsIncrLen(sds s, ssize_t incr) {
  unsigned char flags = s[-1];

  size_t len;
  switch (flags & SDS_TYPE_MASK) {
    case SDS_TYPE_5: {
      unsigned char *fp = ((unsigned char *) s) - 1;
      /* 计算出 SDS 的首地址前一个字节的地址。这个字节用来存储 SDS 类型和长度信息。*/
      unsigned char oldlen = SDS_TYPE_5_LEN(flags);
      /*通过宏 SDS_TYPE_5_LEN 提取出当前 SDS 的长度信息。*/
      assert((incr > 0 && oldlen + incr < 32) || (incr < 0 && oldlen >= (unsigned int) (-incr)));
      /* 这里的 oldlen + incr < 32 表示在 SDS 类型为 SDS_TYPE_5 时，
       * 增加字符串长度 incr 后，新的长度不能超过 32。这是因为 SDS_TYPE_5 类型的字符串长度是用 5 个比特位来表示，
       * 最大值是 31（2^5 - 1），所以不能超过 32*/
      *fp = SDS_TYPE_5 | ((oldlen + incr) << SDS_TYPE_BITS);
      /* 低三位表示类型，高5位表示长度，将表示长度的字节移到合适的位置，然后将表示类型的字节和长度的字节整合到一起*/
      len = oldlen + incr;
      break;
    }
    case SDS_TYPE_8: {
      SDS_HDR_VAR(8, s); // sdshdr8 *sh = (void*)((s)-(sizeof(struct sdshdr8)))
      assert((incr >= 0 && sh->alloc - sh->len >= incr) || (incr < 0 && sh->len >= (unsigned int) (-incr)));
      len = (sh->len += incr);
      break;
    }
    case SDS_TYPE_16: {
      SDS_HDR_VAR(16, s);
      assert((incr >= 0 && sh->alloc - sh->len >= incr) || (incr < 0 && sh->len >= (unsigned int) (-incr)));
      len = (sh->len += incr);
      break;
    }
    case SDS_TYPE_32: {
      SDS_HDR_VAR(32, s);
      assert(
          (incr >= 0 && sh->alloc - sh->len >= (unsigned int) incr) || (incr < 0 && sh->len >= (unsigned int) (-incr)));
      len = (sh->len += incr);
      break;
    }
    case SDS_TYPE_64: {
      SDS_HDR_VAR(64, s);
      assert((incr >= 0 && sh->alloc - sh->len >= (uint64_t) incr) || (incr < 0 && sh->len >= (uint64_t) (-incr)));
      len = (sh->len += incr);
      break;
    }
    default: len = 0; /* Just to avoid compilation warnings. */
  }
  s[len] = '\0';
}

sds sdsgrowzero(sds s, size_t len) {
  size_t curlen = sdslen(s);

  if (len <= curlen) return s;
  s = sdsMakeRoomFor(s,len-curlen);
  if (s == NULL) return NULL;

  /* Make sure added region doesn't contain garbage */
  memset(s+curlen,0,(len-curlen+1)); /* also set trailing \0 byte */
  sdssetlen(s, len);
  return s;
}

sds sdscatlen(sds s, const void *t, size_t len) {
  size_t curlen = sdslen(s);

  s = sdsMakeRoomFor(s,len);
  if (s == NULL) return NULL;
  memcpy(s+curlen, t, len);
  sdssetlen(s, curlen+len);
  s[curlen+len] = '\0';
  return s;
}

sds sdscat(sds s, const char *t){
  return sdscatlen(s, t, strlen(t));
}

sds sdscatsds(sds s, const sds t){
  return sdscatlen(s, t, sdslen(t));
}

sds sdscpylen(sds s, const char *t, size_t len){
  if (sdsalloc(s) < len){
    s = sdsMakeRoomFor(s, len - sdslen(s));
    if (s == NULL) return NULL;
  }
  memcpy(s ,t, len);
  s[len] = '\0';
  sdssetlen(s, len);
  return s;
}

sds sdscpy(sds s, const char *t){
  return sdscpylen(s, t, strlen(t));
}

#define SDS_LLSTR_SIZE 21
int sdsll2str(char *s, long long value){
  char *p, aux;
  size_t l;
  unsigned long long v;
  if (value < 0){
    if (value != LLONG_MIN){
      v = -value;
    } else{
      v = ((unsigned long long) LLONG_MAX) + 1;
    }
  } else{
    v = value;
  }
  p = s;
  do {
    *p++ = '0' + (v % 10);
    v /= 10;
  } while (v);
  if (value < 0) *p++ = '-';
  l = p -s;
  *p = '\0';

  p -- ;
  while(s < p){
    aux = *s;
    *s = *p;
    *p = aux;
    s++;
    p--;
  }
  return l;
}

int sdsull2str(char *s, unsigned long long v){
  char *p, aux;
  size_t l;
  p = s;
  do {
    *p++ = '0' +(v % 10);
    v /= 10;
  } while (v);
  l = p -s;
  *p = '\0';
  p --;
  while(s < p){
    aux = *s;
    *s = *p;
    *p = aux;
    s++;
    p--;
  }
  return l;
}

sds sdsfromlonglong(long long value){
  char buf[SDS_LLSTR_SIZE];
  int len = sdsll2str(buf, value);
  return sdsnewlen(buf, len);
}

sds sdscatvprintf(sds s, const char *fmt, va_list ap){
  va_list cpy;
  char staticbuf[1024], *buf = staticbuf, *t;
  size_t buflen = strlen(fmt) * 2;
  int bufstrlen;
  /* We try to start using a static buffer for speed.
     * If not possible we revert to heap allocation. */
  /*
   * 声明一个静态字符数组 staticbuf，作为初始的字符缓冲区。
   * 然后声明一个字符指针 buf，初始指向 staticbuf。
   * 另外，声明一个字符指针 t，用于存储最终拼接后的 SDS 对象。
   */
  if (buflen > sizeof(staticbuf)){
    buf = s_malloc(buflen);
    if (buf == NULL) return NULL;
  } else {
    buflen = sizeof(staticbuf);
  }
  /* Alloc enough space for buffer and \0 after failing to
     * fit the string in the current buffer size. */
  while(1) {
    va_copy(cpy,ap); /* 将传递过来的可变参数 拷贝一份*/

   /* int vsnprintf(char *str, size_t size, const char *format, va_list ap);
    参数：str    -- 目标字符串。
    size    -- 最大格式化的字符长度。
    format  --  格式化模式
    arg     -- 可变参数列表对象，应呗<stdarg>中定义的va_start 宏初始化。
   */
    bufstrlen = vsnprintf(buf, buflen, fmt, cpy);

    va_end(cpy);
    if (bufstrlen < 0) { // 如果写入目标字符串失败
      if (buf != staticbuf) s_free(buf);
      return NULL;
    }
    /* 写入到 buf的长度 大于buflen */
    if (((size_t)bufstrlen) >= buflen) {
      /* 释放原有的空间，重新申请空间*/
      if (buf != staticbuf) s_free(buf);
      buflen = ((size_t)bufstrlen) + 1;
      buf = s_malloc(buflen);
      if (buf == NULL) return NULL;
      continue;
    }
    break;
  }

  /* Finally concat the obtained string to the SDS string and return it. */
  t = sdscatlen(s, buf, bufstrlen);
  if (buf != staticbuf) s_free(buf);
  return t;

}

sds sdscatprintf(sds s, const char *fmt, ...) {
  va_list ap;
  char *t;
  va_start(ap, fmt);
  t = sdscatvprintf(s,fmt,ap);
  va_end(ap);
  return t;
}

sds sdscatfmt(sds s, char const *fmt, ...){
  size_t initlen = sdslen(s);
  const char *f = fmt;
  long i;
  va_list ap;

  s = sdsMakeRoomFor(s, initlen + strlen(fmt) * 2);
  va_start(ap, fmt);
  f = fmt;
  i = initlen;
  while (*f){
    char next, *str;
    size_t l;
    long long num;
    unsigned long long unum;

    if (sdsavail(s) == 0){
      s = sdsMakeRoomFor(s, 1);
    }

    /* 函数调用时 fmt 对应的参数就是该串字符：
     * "Hello %s World %I,%I--",
     * 函数中重新申请了一个const char *f 指针指向 fmt,
     * while 循环是每次都会取出一个 f 串中的字符来进行读取判断字符是啥
     */
    switch (*f) {
      case '%':
        next = *(f+1);
        if (next == '\0') break;
        f++;
        switch (next) {
          case 's':
          case 'S':
            str = va_arg(ap, char *);
            l = (next == 's') ? strlen(str): sdslen(str);
            if (sdsavail(s) < l){
              s = sdsMakeRoomFor(s, l);
            }
            memcpy(s + i, str, l);
            sdsinclen(s, l);
            i += l;
            break;
          case 'i':
          case 'I':
            if (next == 'i')
              num = va_arg(ap,int);
            else
              num = va_arg(ap,long long);
            {
              char buf[SDS_LLSTR_SIZE];
              l = sdsll2str(buf,num);
              if (sdsavail(s) < l) {
                s = sdsMakeRoomFor(s,l);
              }
              memcpy(s+i,buf,l);
              sdsinclen(s,l);
              i += l;
            }
            break;
          case 'u':
          case 'U':
            if (next == 'u')
              unum = va_arg(ap,unsigned int);
            else
              unum = va_arg(ap,unsigned long long);
            {
              char buf[SDS_LLSTR_SIZE];
              l = sdsull2str(buf,unum);
              if (sdsavail(s) < l) {
                s = sdsMakeRoomFor(s,l);
              }
              memcpy(s+i,buf,l);
              sdsinclen(s,l);
              i += l;
            }
            break;
          default: /* Handle %% and generally %<unknown>. */
            s[i++] = next;
            sdsinclen(s,1);
            break;
        }
        break;
      default:
        s[i++] = *f;
        sdsinclen(s,1);
        break;
    }
    f++;
  }
  va_end(ap);

  /* Add null-term */
  s[i] = '\0';
  return s;
}

sds sdstrim(sds s, const char  *cset){
  char * end, *sp,*ep;
  /* 开始指针，结束指针*/

  size_t  len;
  sp = s;
  ep = end = s + sdslen(s) - 1; // 计算出最后一个字符的地址

  /* strchr() 用于查找字符串中的一个字符，并返回该字符在字符串中第一次出现的位置。*/
  while(sp <= end && strchr(cset, *sp))sp++;
  while(ep > sp && strchr(cset, *ep))ep--;
  len = (ep - sp) + 1;
  /*
   * void *memmove(void *str1, const void *str2, size_t n) 从 str2 复制 n 个字符到 str1，
   * 但是在重叠内存块这方面，memmove() 是比 memcpy() 更安全的方法。
   * 如果目标区域和源区域有重叠的话，memmove()
   * 能够保证源串在被覆盖之前将重叠区域的字节拷贝到目标区域中，复制后源区域的内容会被更改。
   * 如果目标区域与源区域没有重叠，则和 memcpy() 函数功能相同
   */
  if (s != sp) memmove(s, sp, len);
  s[len] = '\0';
  sdssetlen(s, len);
  return s;
}

void sdsrange(sds s, ssize_t start, ssize_t end){
  size_t  newlen, len = sdslen(s);

  if (len == 0) return;
  if (start < 0){
    start = len + start;
    if (start < 0) start = 0;
  }
  if (end < 0){
    end = len + end;
    if (end < 0)end = 0;
  }
  newlen = (start > end) ? 0 : (end - start) + 1;
  if (newlen != 0){
    if (start >= (ssize_t)len){
      newlen = 0;
    } else if (end >= (ssize_t)len){
      end = len - 1;
      newlen = (end - start) + 1;
    }
  }
  if (start && newlen)memmove(s, s +start, newlen);
  s[newlen] = 0;
  sdssetlen(s , newlen);
}

void sdstolower(sds s) {
  size_t len = sdslen(s), j;

  for (j = 0; j < len; j++) s[j] = tolower(s[j]);
}

/* Apply toupper() to every character of the sds string 's'. */
void sdstoupper(sds s) {
  size_t len = sdslen(s), j;

  for (j = 0; j < len; j++) s[j] = toupper(s[j]);
}

int sdscmp(const sds s1, const sds s2){
  size_t l1, l2, minlen;
  int cmp;

  l1 = sdslen(s1);
  l2 = sdslen(s2);
  minlen = (l1 > l2)? l1:l2;
  cmp = memcmp(s1, s2, minlen);
  if (cmp == 0) return l1 > l2 ? 1 :(l1 < l2? -1 :0);
  return cmp;
}

sds * sdssplitlen(const char *s, ssize_t len, const char *sep, int seplen, int * count) {
  int elements = 0, slots = 5;
  long start = 0, j;
  sds *tokens;

  if (seplen < 1 || len <= 0) {
    *count = 0;
    return NULL;
  }

  tokens = s_malloc(sizeof(sds) * slots);
  if (tokens == NULL) return NULL;

  for (j = 0; j < (len-(seplen-1)); j++) {
    /* make sure there is room for the next element and the final one */
    if (slots < elements+2) {
      sds *newtokens;

      slots *= 2;
      newtokens = s_realloc(tokens,sizeof(sds)*slots);
      if (newtokens == NULL) goto cleanup;
      tokens = newtokens;
    }
    /* search the separator */
    if ((seplen == 1 && *(s+j) == sep[0]) || (memcmp(s+j,sep,seplen) == 0)) {
      tokens[elements] = sdsnewlen(s+start,j-start);
      if (tokens[elements] == NULL) goto cleanup;
      elements++;
      start = j+seplen;
      j = j+seplen-1; /* skip the separator */
    }
  }
  /* Add the final element. We are sure there is room in the tokens array. */
  tokens[elements] = sdsnewlen(s+start,len-start);
  if (tokens[elements] == NULL) goto cleanup;
  elements++;
  *count = elements;
  return tokens;
  cleanup:
  {
    int i;
    for (i = 0; i < elements; i++) sdsfree(tokens[i]);
    s_free(tokens);
    *count = 0;
    return NULL;
  }
}

void sdsfreesplitres(sds *tokens, int count) {
  if (!tokens) return;
  while(count--)
    sdsfree(tokens[count]);
  s_free(tokens);
}

sds sdscatrepr(sds s, const char *p, size_t len) {
  s = sdscatlen(s,"\"",1);
  while(len--) {
    switch(*p) {
      case '\\':
      case '"':
        s = sdscatprintf(s,"\\%c",*p);
        break;
      case '\n': s = sdscatlen(s,"\\n",2); break;
      case '\r': s = sdscatlen(s,"\\r",2); break;
      case '\t': s = sdscatlen(s,"\\t",2); break;
      case '\a': s = sdscatlen(s,"\\a",2); break;
      case '\b': s = sdscatlen(s,"\\b",2); break;
      default:
        if (isprint(*p))
          s = sdscatprintf(s,"%c",*p);
        else
          s = sdscatprintf(s,"\\x%02x",(unsigned char)*p);
        break;
    }
    p++;
  }
  return sdscatlen(s,"\"",1);
}

int is_hex_digit(char c){
  return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
      (c >= 'A' && c <= 'F');
}

int hex_digit_to_int(char c) {
  switch(c) {
    case '0': return 0;
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
    case 'a': case 'A': return 10;
    case 'b': case 'B': return 11;
    case 'c': case 'C': return 12;
    case 'd': case 'D': return 13;
    case 'e': case 'E': return 14;
    case 'f': case 'F': return 15;
    default: return 0;
  }
}

sds *sdssplitargs(const char *line, int *argc) {
  const char *p = line;
  char *current = NULL;
  char **vector = NULL;

  *argc = 0;
  while(1) {
    /* skip blanks */
    while(*p && isspace(*p)) p++;
    if (*p) {
      /* get a token */
      int inq=0;  /* set to 1 if we are in "quotes" */
      int insq=0; /* set to 1 if we are in 'single quotes' */
      int done=0;

      if (current == NULL) current = sdsempty();
      while(!done) {
        if (inq) {
          if (*p == '\\' && *(p+1) == 'x' &&
              is_hex_digit(*(p+2)) &&
              is_hex_digit(*(p+3)))
          {
            unsigned char byte;

            byte = (hex_digit_to_int(*(p+2))*16)+
                hex_digit_to_int(*(p+3));
            current = sdscatlen(current,(char*)&byte,1);
            p += 3;
          } else if (*p == '\\' && *(p+1)) {
            char c;

            p++;
            switch(*p) {
              case 'n': c = '\n'; break;
              case 'r': c = '\r'; break;
              case 't': c = '\t'; break;
              case 'b': c = '\b'; break;
              case 'a': c = '\a'; break;
              default: c = *p; break;
            }
            current = sdscatlen(current,&c,1);
          } else if (*p == '"') {
            /* closing quote must be followed by a space or
             * nothing at all. */
            if (*(p+1) && !isspace(*(p+1))) goto err;
            done=1;
          } else if (!*p) {
            /* unterminated quotes */
            goto err;
          } else {
            current = sdscatlen(current,p,1);
          }
        } else if (insq) {
          if (*p == '\\' && *(p+1) == '\'') {
            p++;
            current = sdscatlen(current,"'",1);
          } else if (*p == '\'') {
            /* closing quote must be followed by a space or
             * nothing at all. */
            if (*(p+1) && !isspace(*(p+1))) goto err;
            done=1;
          } else if (!*p) {
            /* unterminated quotes */
            goto err;
          } else {
            current = sdscatlen(current,p,1);
          }
        } else {
          switch(*p) {
            case ' ':
            case '\n':
            case '\r':
            case '\t':
            case '\0':
              done=1;
              break;
            case '"':
              inq=1;
              break;
            case '\'':
              insq=1;
              break;
            default:
              current = sdscatlen(current,p,1);
              break;
          }
        }
        if (*p) p++;
      }
      /* add the token to the vector */
      vector = s_realloc(vector,((*argc)+1)*sizeof(char*));
      vector[*argc] = current;
      (*argc)++;
      current = NULL;
    } else {
      /* Even on empty input string return something not NULL. */
      if (vector == NULL) vector = s_malloc(sizeof(void*));
      return vector;
    }
  }

  err:
  while((*argc)--)
    sdsfree(vector[*argc]);
  s_free(vector);
  if (current) sdsfree(current);
  *argc = 0;
  return NULL;
}

sds sdsmapchars(sds s, const char *from, const char *to, size_t setlen) {
  size_t j, i, l = sdslen(s);

  for (j = 0; j < l; j++) {
    for (i = 0; i < setlen; i++) {
      if (s[j] == from[i]) {
        s[j] = to[i];
        break;
      }
    }
  }
  return s;
}

sds sdsjoin(char ** argv, int argc, char *sep){
  sds join = sdsempty();
  int j;

  for(j = 0; j < argc; j++){
    join = sdscat(join, argv[j]);
    if (j != argc - 1) join = sdscat(join, sep);
  }
  return join;
}

sds sdsjoinsds(sds *argv, int argc, const char *sep, size_t seplen) {
  sds join = sdsempty();
  int j;

  for (j = 0; j < argc; j++) {
    join = sdscatsds(join, argv[j]);
    if (j != argc-1) join = sdscatlen(join,sep,seplen);
  }
  return join;
}

void *sds_malloc(size_t size) { return s_malloc(size); }
void *sds_realloc(void *ptr, size_t size) { return s_realloc(ptr,size); }
void sds_free(void *ptr) { s_free(ptr); }




