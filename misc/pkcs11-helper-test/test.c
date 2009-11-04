#ifdef _WIN32
#include <Windows.h>
#endif

#include <stdlib.h>
#include <pkcs11-helper-1.0/pkcs11h-token.h>
#include <pkcs11-helper-1.0/pkcs11h-certificate.h>
#include <stdio.h>
#include <time.h>

#ifdef _WIN32
double get_time(void) { return (GetTickCount()/(double)1000); }
#else
double get_time(void)
{
	struct timeval stime;
	gettimeofday (&stime, (struct timezone*)0);
	return (stime.tv_sec+((double)stime.tv_usec)/1000000);
}
#endif


static void
pkcs11_log_hook(
		IN void *const global_data,
		IN unsigned flags,
		IN const char *const format,
		IN va_list args
)
{
	vfprintf(stdout, format, args);
	fprintf(stdout, "\n");
	fflush(stdout);
}

int
main()
{
	CK_RV           rv;
	int i = 0;
	int old_token_count;
	int token_count;
	double start_time;
	pkcs11h_token_id_list_t tokens, temp;
	rv = pkcs11h_initialize();
	rv = pkcs11h_addProvider(PKCS11_PROVIDER, PKCS11_PROVIDER, TRUE, PKCS11H_PRIVATEMODE_MASK_AUTO,
				 PKCS11H_SLOTEVENT_METHOD_POLL, 1000 /* 1 second */, FALSE);
	if (rv != CKR_OK) {
	        fprintf(stdout, "add rv: %d\n", rv);
                exit(1);
        }
	if ((rv = pkcs11h_setLogHook(&pkcs11_log_hook, NULL)) != CKR_OK) {
		fprintf(stdout, "log hook rv: %d\n", rv);
		exit(1);
	}	
	pkcs11h_setLogLevel (PKCS11H_LOG_INFO);

	old_token_count = 0;
	for (i = 0; i <= 100; i++) {
	        printf("%f loop %-2d:",get_time(), i);
		start_time = get_time();
		token_count = 0;
		rv = pkcs11h_token_enumTokenIds(PKCS11H_ENUM_METHOD_RELOAD, &tokens);
		if (rv != CKR_OK) {
        		fprintf(stdout, "enum tokens rv: %d\n", rv);
        		exit(1);
                }
		for (temp = tokens; temp != NULL; temp = temp->next) {
			//fprintf(stdout, "Found token: %s\n", temp->token_id->label);
			token_count++;
		}
		if (token_count > old_token_count)
		        printf(" - Token inserted");
                else if (token_count < old_token_count)
                        printf(" - Token removed");
                old_token_count = token_count;
		printf(" - poll done in %f seconds\n", get_time() - start_time);
#ifdef _WIN32
		Sleep(1000);
#else
		sleep(1);
#endif
	}
	pkcs11h_terminate ();
}
