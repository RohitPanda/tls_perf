/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: Bernhard Jaeger
 *
 * Created on June 1, 2018, 2:17 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <curl/curl.h>

size_t write_data(char *buffer, size_t size, size_t nmemb, void *userp)
{
    /*Since we are not interested in the response just do nothing here.*/
    return size * nmemb;
}

/*
 * 
 */
int main(int argc, char** argv) 
{
    int c , bool3 = 0;
    char *url, *ip;
    int port;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL *curl;
    
    while ((c = getopt(argc, argv, "3u:p:")) != -1) 
    {
	switch (c) 
        {
        case '3':
            bool3 = 1;
            break;
        case 'u':
            url = optarg;
            break;
        case 'p':
            port = atoi(optarg);
            break;
        default:
	    fprintf(stderr,
		    "Options:\n"
		    "-3 To use TLS 1.3 instead of TLS 1.2\n"
                    "-u URL The Url to connect to\n"
                    "-p port The port to connect to\n");
	    exit(EXIT_FAILURE);
	}
    }
    curl = curl_easy_init();
    
    if(curl)
    {
        CURLcode res;
        curl_easy_setopt(curl, CURLOPT_URL, url); /*test for tls 1.3 https://tls13.akamai.io*/
        curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
        curl_easy_setopt(curl, CURLOPT_PORT, port);
        curl_easy_setopt(curl, CURLOPT_CAPATH, "/etc/ssl/certs");/*Need to use this since the standard ca path on my system seems to be broken*/
        //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);  /*Set the TLS version CURL_SSLVERSION_TLSv1_3 is the newest*/
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data); 
        res = curl_easy_perform(curl);
        if(res == CURLE_OK)
        {
            double connect_dns, connect_tcp, connect_tls;
            res = curl_easy_getinfo(curl, CURLINFO_NAMELOOKUP_TIME, &connect_dns);
            if(CURLE_OK == res) 
            {
                time_t rawtime;
		time(&rawtime);
                printf("%.3f;%li;%s;;", connect_dns * 1000.0, (long)rawtime, url);
                res = curl_easy_getinfo(curl, CURLINFO_PRIMARY_IP, &ip);
                if(CURLE_OK == res) 
                {
                    printf("%s;%d;", ip, port);
                    res = curl_easy_getinfo(curl, CURLINFO_APPCONNECT_TIME, &connect_tls);
                    if(CURLE_OK == res) 
                    {
                        long code;
                        printf("%.3f;", (connect_tls - connect_dns) * 1000.0);
                        res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
                        if(CURLE_OK == res)
                        {
                            printf("%3ld;TCP/",code);
                            if(bool3 == 1)
                            {
                                printf("TLS1.3;\n");
                            }
                            else
                            {
                                printf("TLS1.2;\n");
                            }
                        }
                    }
                    /*res = curl_easy_getinfo(curl, CURLINFO_CONNECT_TIME, &connect_tcp);
                    if(CURLE_OK == res) 
                    {
                        printf("\nTime it takes to do only TCP: %.6f s\n", connect_tcp - connect_dns);
                    }*/
                }
            }
            else
            {
                printf("\nUnsupported option.\n");
            }
        }
        else
        {
            printf("res: %i\n", (int)res);
        }
        curl_easy_cleanup(curl);
    }
    
    curl_global_cleanup();
    
    return (EXIT_SUCCESS);
}
