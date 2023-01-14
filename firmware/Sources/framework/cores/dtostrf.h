
#define dtostrf fixed_dtostrf

#ifdef __cplusplus
extern "C" {
#endif
// char *dtostrf (double val, signed char width, unsigned char prec, char *sout);

char *fixed_dtostrf(double __val, signed char __width, unsigned char __prec, char *__s);

#ifdef __cplusplus
}
#endif
