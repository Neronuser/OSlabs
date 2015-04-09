/* 
 * File:   main.cpp
 * Author: roman
 *
 * Created on March 9, 2015, 12:04 PM
 */
#include<stdio.h>

#include<stdlib.h>

#include<string.h>

#include<ctype.h>

#include<sys/types.h>
#include<iostream>
#include<dirent.h>
#define PROC_COMMAND_LENGTH 50
using namespace std;

struct procInfo {
    int pid;
    char comm[PROC_COMMAND_LENGTH];
    char stat[15];
    char uid[20];
};

procInfo* getProcsInfo(int *procRet)
{
    procInfo *info;
    DIR *dirPtr;
    FILE *filePtr;
    struct dirent *dirInfo;
    char pPath[20],pComm[PROC_COMMAND_LENGTH],pID[6],pStat[15],uID[20];
    int i=0;
    if(!(dirPtr=opendir("/proc")))
    { 
        printf("Cannot do opendir() !\n"); 
        return NULL;
    }
    // print all processes and save processes info
    info=(procInfo*)malloc((*procRet)*(sizeof(procInfo)));
    printf("PROC info:\n");
    printf("%3s%25.25s%6s%15s%21s\n","NUM","COMMAND","PID", "STATUS","UID");
    while(dirInfo=readdir(dirPtr))
    { 
        if(!isdigit(dirInfo->d_name[0]))
        //    cout << "Here" << endl;
            continue; 
        strcpy(pPath,"/proc/");
        strcat(pPath,dirInfo->d_name);
        strcat(pPath,"/status"); 
        if(!(filePtr=fopen(pPath,"r"))) 
            continue; 
        fscanf(filePtr,"%*s %99[^\n]",pComm);
        fscanf(filePtr,"%*s %99[^\n]",pStat);
        char temp[30]="abc";
        while(strcmp(temp,"Pid:")){
            fscanf(filePtr,"%s",temp);
        }
        fscanf(filePtr,"%s",pID);
        while(strcmp(temp,"Uid:")){
            fscanf(filePtr,"%s",temp);
        }
        fscanf(filePtr,"%99[^\n]",uID);
        if(i<*procRet) 
        { 
            strcpy(info[i].comm,pComm); 
            info[i].pid=atoi(pID);
            strcpy(info[i].stat,pStat);
            strcpy(info[i].uid,uID);
        } 
        i++; 
        printf("%3d%25.25s%6s%15s%21s\n",i,pComm,pID,pStat,uID); 
        fclose(filePtr);
    }
    if(i<*procRet) 
        *procRet=i;
    closedir(dirPtr);
    return info;
}

int main() {
    procInfo *result;
    int procs = 5;
    result = getProcsInfo(&procs);
}

