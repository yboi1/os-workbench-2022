#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

// 返回字符串长度
size_t strlen(const char *s) {  
  size_t str_Len = 0;
  while(*(s++)!='\0') ++str_Len;
  return str_Len;
}

// 复制字符串
char *strcpy(char *dst, const char *src) {
  while(*src!='\0'){
    *dst = *src;
    ++dst;
    ++src;
  }
  return dst;
}

// 复制字符串的前n个
char *strncpy(char *dst, const char *src, size_t n) {
  while(*src!='\0' && (n--)>0){
    *dst = *src;
    ++dst;
    ++src;
  }
  return dst;
}

// 字符串的拼接
char *strcat(char *dst, const char *src) {
  size_t *p = dst;
  while(*p!='\n') ++p;
  while(*src!='\0'){
    *p = *src;
    p++;
    src++;
  }
  *p='\0';
  return dst;
}

//字符串比较
int strcmp(const char *s1, const char *s2) {
  for(size_t i = 0;s1[i] || s2[i]; ++i) {
		if(s1[i] < s2[i]) { return -1; }
		else if(s1[i] > s2[i]) { return 1; }
	}
	return 0;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  for(size_t i = 0; i<n &&s1[i] || s2[i]; ++i) {
		if(s1[i] < s2[i]) { return -1; }
		else if(s1[i] > s2[i]) { return 1; }
	}
	return 0;
}

// 类似于替换
void *memset(void *s, int c, size_t n) {
  for(size_t i = 0; i != n; ++i) {
		((unsigned char*)s)[i] = (unsigned char)c;
	}
	return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  //首先申请相关大小的堆空间
	void *temp = malloc(n);
	assert(temp);

	memcpy(temp, src, n);
	memcpy(dst, temp, n);
	free(temp);
	return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  for(size_t i = 0; i != n; ++i) {
		((unsigned char*)out)[i] = ((const unsigned char*)in)[i];
	}
	return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  for(size_t i = 0; i != n; ++i) {
		if(((const unsigned char*)s1)[i] < ((const unsigned char*)s2)[i]) { return -1; }
		else if(((const unsigned char*)s1)[i] > ((const unsigned char*)s2)[i]) { return 1; }
	}
	return 0;
}

#endif
