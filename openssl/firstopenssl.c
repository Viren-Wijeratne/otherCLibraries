//Program takes in a message and a password, encrypts it with AES-128-CBC and outputs a ciphertext in base64
//Compile using: gcc firstopenssl.c -o firstopenssl -Wall -Wextra -lcrypto
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/kdf.h>
#include <openssl/core_names.h>
#include <openssl/params.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/bio.h>

#define OP_ENCODE 1
#define OP_DECODE 2

//Derive a key for the password provided, which then will be passed into EVP_EncryptInit()
int derivekey(const char *secret, unsigned char *out, size_t outlen){
    EVP_KDF *kdf = NULL;
    EVP_KDF_CTX *kctx = NULL;
    OSSL_PARAM params[4], *p = params;
    int rc = 0;
    
    kdf = EVP_KDF_fetch(NULL, "SSKDF", NULL);
    if(kdf == NULL){goto out;}
    kctx = EVP_KDF_CTX_new(kdf);
    if(kctx == NULL){goto out;}

    *p++ = OSSL_PARAM_construct_utf8_string(OSSL_KDF_PARAM_DIGEST, SN_sha256, strlen(SN_sha256));
    *p++ = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_KEY, secret, strlen(secret));
    *p++ = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_INFO, "key", strlen("key"));
    *p++ = OSSL_PARAM_construct_end();

    if(EVP_KDF_derive(kctx, out, outlen, params) <= 0){goto out;}

    rc = 1;

    out:    //I dont like it, makes code hard to read >:(
        EVP_KDF_CTX_free(kctx);
        EVP_KDF_free(kdf);
        return rc;
}
void print_hex(const unsigned char *data, size_t len){
    for(size_t i = 0; i < len; i++){
        fprintf(stdout, "%02x", data[i]);
    }
    fprintf(stdout,"\n");
}

void freeObjectandContext(EVP_CIPHER_CTX *ctx, EVP_CIPHER *evp){
    EVP_CIPHER_CTX_free(ctx);
    EVP_CIPHER_free(evp);

}

void freeObjectContextandBIO(EVP_CIPHER_CTX *ctx, EVP_CIPHER *evp, BIO *b64, BIO *mem){
    freeObjectandContext(ctx, evp);
    BIO_free(mem);
    BIO_free(b64);
}


int main(int argc, char *argv[]){

    unsigned char key[16];
    unsigned char iv[16];
    char outmsg[1024 + EVP_MAX_BLOCK_LENGTH];
    
    char *message = argv[3];        //argv[3]
    int operation = 1;
    int outlen = 0;
    int tmplen = 0;


    if(argc < 3){
        fprintf(stderr,"wrong arguments\n");
        fprintf(stderr,"./firstopenssl <password> <operation> [\"message\"]\n");
        return 1;
    }

    if(!derivekey(argv[1], key, 16)){       //argv[1]
        fprintf(stderr, "Unable to derive key!\n");
        return 1;
    }

    if(!strcmp(argv[2], "encode")){         //argv[2]
        operation = OP_ENCODE;
    }else if(!strcmp(argv[2], "decode")){
        operation = OP_DECODE;
    }else{
        fprintf(stderr, "unsupported operation '%s'", argv[2]);
        return 1;
    }

    //EVP_CIPHER_fetch(): Fetches the cipher implementation for the given algorithm from any provider 
    EVP_CIPHER *evp = EVP_CIPHER_fetch(NULL, "AES-128-CBC", NULL);  
    if(evp == NULL){
        fprintf(stderr, "No evp!\n");
        ERR_print_errors_fp(stderr);
        EVP_CIPHER_free(evp);
        return 1;
    }

    //EVP_CIPHER_CTX_new(): Allocates and returns a cipher context.
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if(ctx == NULL){
        fprintf(stderr, "No context!\n");
        freeObjectandContext(ctx, evp);
        return 1;
    }


    //Main function for Encoding 
    if(operation == OP_ENCODE){
        //Function generates a 16 random bytes and store in iv[] (initial vector) 
        if(!RAND_bytes(iv, 16)){    
            fprintf(stderr, "Failed to generate IV");
            freeObjectandContext(ctx, evp);
            return 1;
        }

        //Sets up cipher context (ctx) for encryption with cipher type (evp)
        if(!EVP_EncryptInit(ctx, evp, key, iv)){
            fprintf(stderr, "Failed EVP_EncryptInit!\n");
            freeObjectandContext(ctx, evp);
            return 1;
        }

        memset(outmsg, 0, 1024);

        //Encrypts inl (strlen(message)) bytes from the buffer in (message) and writes the encrypted version to out (outmsg). 
        if(!EVP_EncryptUpdate(ctx, outmsg, &outlen, message, strlen(message))){
            fprintf(stderr, "Failed Encrypt Update!\n");
            freeObjectandContext(ctx, evp);
            return 1;
        }
        //If padding is enabled (the default) then EVP_EncryptFinal() encrypts the "final" data, that is any data that remains in a partial block. Uses standard block padding (aka PKCS padding)
        if(!EVP_EncryptFinal(ctx, outmsg + outlen, &tmplen)){
            //If extra padding is needed outmsg+outlen will point to the memory address where the padding should start, amount of padding is stored at tmplen 
            fprintf(stderr, "Failed final encrypt!\n");
            freeObjectandContext(ctx, evp);
            return 1;
        }
        outlen += tmplen;   //so outlen will be encrypted message + padding

        /*fprintf(stdout, "IV: %s\n", iv);
        fprintf(stdout, "Ciphertext: %s\n", outmsg);

        fprintf(stdout, "IV: ");
        print_hex(iv, sizeof(iv));

        fprintf(stdout, "Ciphertext: ");
        print_hex(outmsg, sizeof(outmsg));
        */

        //Turning the output into a base64 encoded message, as outmsg at this point doesnt use ascii characters as output
        BIO *b64 = BIO_new(BIO_f_base64());
        if((b64 == NULL) 
            || BIO_push(b64, BIO_new_fp(stdout, BIO_NOCLOSE)) == NULL
            || BIO_write(b64, iv, sizeof(iv)) <= 0
            || BIO_write(b64, outmsg, outlen) <= 0
            || BIO_flush(b64) == -1
            ){
            fprintf(stderr, "Failed to write base64 data\n");
            BIO_free_all(b64);
            freeObjectandContext(ctx, evp);
            return 1;
        }
        BIO_free_all(b64);
    }

else if(operation == OP_DECODE){

        BIO *b64 = BIO_new(BIO_f_base64());
        BIO *mem = BIO_new_mem_buf(message, strlen(message));   //Send the message to a buffer in memory 
        int rc = 0;

        if((b64 == NULL)
        ||mem == NULL
        || BIO_push(b64, mem) == NULL
        ){
            fprintf(stderr, "Unable set up base64 decoder\n");
            int rc = 1;
            freeObjectContextandBIO(ctx, evp, b64, mem);
            return 1;

        }

        char binmsg[1024];
        //Becuase we are reading from memory there will be a no newline, but by default base64 expects a newline in the input, so we have to set the flag BIO_FLAGS_BASE64_NO_NL to ignore default behavior
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL); 
        
        //Additionally the memory buffer BIO by default is expecting a EOF when it reaches the end of the memory buffer, but as we are using base64 the man page states that it doesnt handle EOFs properly so we tell it return 0 when done
        BIO_set_mem_eof_return(mem, 0);
        
        memset (binmsg, 0, 1024);
        
        rc = BIO_read(b64, binmsg, 1024);   //Filter the memory bio through the base64 encoder
        //message is now in its binary form

        if(rc <= 0){
            fprintf(stderr, "Failed to read message\n");
            ERR_print_errors_fp(stderr);
            rc = 1;
            freeObjectContextandBIO(ctx, evp, b64, mem);
            return 1;
        }

        memcpy(iv, binmsg, sizeof(iv)); //Copy the memory containing the iv from the start of the message to the binmsg buffer

        if(!EVP_DecryptInit(ctx, evp, key, iv)){
            fprintf(stderr, "Failed EVP_DecryptInit!\n");
            freeObjectContextandBIO(ctx, evp, b64, mem);
            return 1;
        }

        memset(outmsg, 0, 1024);
        if(!EVP_DecryptUpdate(ctx, outmsg, &outlen, &binmsg[16], rc -16)){ //Decode the message starting after iv, reduce length of message by 16 bytes
            fprintf(stderr, "Failed EVP_DecryptUpdate!\n");
            rc = 1;
            freeObjectContextandBIO(ctx, evp, b64, mem);
            return 1;
        }

        if(!EVP_DecryptFinal(ctx, &outmsg[outlen], &outlen)){
            fprintf(stderr, "Failed EVP_DecryptFinal!\n");
            rc = 1;
            freeObjectContextandBIO(ctx, evp, b64, mem);
            return 1;
        }

        if(outlen >= 1024){
            outmsg[1023] = '\0';    //Arranging the string to be read by fprintf
        }

        fprintf(stdout, "Output message is: %s\n", outmsg);

}
    return 0;
}
/*
* The output contains the cipher code + IV
 */