/* cJSON Types: */
#define JSON_FALSE 0
#define JSON_TRUE 1
#define JSON_NULL 2
#define JSON_NUMBER 3
#define JSON_STRING 4
#define JSON_ARRAY 5
#define JSON_OBJECT 6

#include "supportfunc.h"
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>

typedef struct JSON {
    int type;                   /* The type of the item, as above. */
    struct JSON* next;         //if JSON is a JSONarray or JSONobject
    struct JSON* child;

    char* valuestring;			/* The item's string, if type==JSON_STRING */
    int valueint;				/* The item's number, if type==JSON_TRUE||JSON_FALSE */
    double valuedouble;			/* The item's number, if type==JSON_NUMBER  */

    char* key;                // an string array to store keys
} JSON;

//declaritons
JSON *newJSON();
JSON *ParseJSON(const char *value);
JSON *ParseJSONFromFile(const char *file_name);
void PrintJSON(JSON *item);
void PrintJSONToFile(JSON *item, const char *file_name);
/* Create */
JSON *CreateNULL(void);
JSON *CreateTrue(void);
JSON *CreateFalse(void);
JSON *CreateBool(int b);
JSON *CreateNumber(double num);
JSON *CreateString(const char *string);
JSON *CreateArray(void);
JSON *CreateObject(void);
/* Append */
void AddItemToArray(JSON *array, JSON *item);
void AddItemToObject(JSON *object, const char *key, JSON *value);
/* Update */
void ReplaceItemInArray(JSON *array, int which, JSON *new_item);
void ReplaceItemInObject(JSON *object, const char *key, JSON *new_value);
/* Remove/Delete */
JSON *DetachItemFromArray(JSON *array, int which);
void *DeleteItemFromArray(JSON *array, int which);
JSON *DetachItemFromObject(JSON *object, const char *key);
void *DeleteItemFromObject(JSON *object, const char *key);
void DeleteJSON(JSON *item);
/* Duplicate */
JSON *Duplicate(JSON *item, int recurse);
/* Read */
JSON *GetItemInArray(JSON *array, int which);
JSON *GetItemInObject(JSON *object, const char *key);
JSON *GetItemInJSON(JSON *json, const char *path);
//own functions
void printObject(JSON* target,int superType);
JSON* arrayParse(char* value,unsigned long length);

JSON* stringParse(char* string,int length)
{
    JSON* tmp=NULL;
    char* result = calloc(length, 1);
    strncpy(result, string+1,length-2); //因为去掉了字符串两头的引号

    if (strcmp(result, "true") ==0) {
        tmp=CreateTrue();
    }
    else if(strcmp(result, "false") ==0){
        tmp=CreateFalse();
    }
    else if(strlen(result)==0){
        tmp=CreateNULL();
    }
    //here
    else if(string[0]=='\"'&&isNumber(result)){
        tmp=CreateNumber(atof(result));
    }
    else{
        tmp=CreateString(result);
    }
    free(result);
    return tmp;
}

JSON* objectParse(char* value,unsigned long length)
{
    if(length<=2)
        return CreateNULL();
    JSON* superJSON=NULL;
    superJSON=CreateObject();
    JSON* tmp=superJSON;
    int currentLength=0;
    char* currentPosition=value;

    for(int add=0;length-add>1;)
    {
        JSON* current=NULL;
        //Parse key here
        currentPosition++;
        add++;
        currentLength=getParseLength(currentPosition);
        char* Key=calloc(currentLength+2, 1);
        strncpy(Key, currentPosition+1, currentLength-2);
        currentPosition+=currentLength;
        add+=currentLength;
        //Parse over , key is Key
        //Parse value here
        currentPosition++;
        add++;
        currentLength=getParseLength(currentPosition);
        if(currentPosition[0]=='{')
            current=objectParse(currentPosition, currentLength);
        else if(currentPosition[0]=='[')
            current=arrayParse(currentPosition,currentLength);
        else
            current=stringParse(currentPosition,currentLength);
        currentPosition+=currentLength;
        add+=currentLength;
        //parse over
        //add key
        current->key=Key;
        //link
        if(tmp==superJSON)
        {
            tmp->child=current;
            tmp=tmp->child;
        }
        else
        {
            tmp->next=current;
            tmp=tmp->next;
        }
        //link over
    }

    return superJSON;
}

JSON* arrayParse(char* value,unsigned long length)
{
    if(length<=2)
        return CreateNULL();
    JSON* superJSON=NULL;
    superJSON=CreateArray();
    JSON* tmp=superJSON;
    int currentLength=0;
    char* currentPosition=value;

    for(int add=0;length-add>1;)
    {
        JSON* current=NULL;
        currentPosition++;
        add++;
        currentLength=getParseLength(currentPosition);
        //Parse here
        if(currentPosition[0]=='{')
            current=objectParse(currentPosition, currentLength);
        else if(currentPosition[0]=='[')
            current=arrayParse(currentPosition,currentLength);
        else
            current=stringParse(currentPosition,currentLength);
        currentPosition+=currentLength;
        add+=currentLength;
        //parse over
        //link
        if(tmp==superJSON)
        {
            tmp->child=current;
            tmp=tmp->child;
        }
        else
        {
            tmp->next=current;
            tmp=tmp->next;
        }
        //link over
    }

    return superJSON;
}

void printArray(JSON* array,int superType)
{
    if (superType==JSON_OBJECT) {
        printf("\"%s\":",array->key);
    }
    printf("[");
    JSON* tmp=array->child;
    for(;tmp!=NULL;tmp=tmp->next)
    {
        if (tmp->type==JSON_NULL) {
            printf("null");
        }
        else if(tmp->type==JSON_TRUE){
            printf("true");
        }
        else if(tmp->type==JSON_FALSE){
            printf("false");
        }
        else if(tmp->type==JSON_NUMBER){
            printf("%.3f",tmp->valuedouble);
        }
        else if(tmp->type==JSON_OBJECT){
            printObject(tmp,JSON_ARRAY);
        }
        else if(tmp->type==JSON_ARRAY){
            printArray(tmp,JSON_ARRAY);
        }
        else{
            printf("\"%s\"",tmp->valuestring);
        }
        if(tmp->next!=NULL)
            printf(",");
    }
    printf("]");
}

void printObject(JSON* object,int superType)
{
    if (superType==JSON_OBJECT&&object->key!=NULL) {
        printf("\"%s\":",object->key);
    }
    printf("{");
    JSON* tmp=object->child;
    for(;tmp!=NULL;tmp=tmp->next)
    {
        if (tmp->type==JSON_NULL) {
            printf("\"%s\":null",tmp->key);
        }
        else if(tmp->type==JSON_TRUE){
            printf("\"%s\":true",tmp->key);
        }
        else if(tmp->type==JSON_FALSE){
            printf("\"%s\":false",tmp->key);
        }
        else if(tmp->type==JSON_NUMBER){
            printf("\"%s\":%.3f",tmp->key,tmp->valuedouble);
        }
        else if(tmp->type==JSON_OBJECT){
            printObject(tmp,JSON_OBJECT);
        }
        else if(tmp->type==JSON_ARRAY){
            printArray(tmp,JSON_OBJECT);
        }
        else{
            printf("\"%s\":\"%s\"",tmp->key,tmp->valuestring);
        }
        if(tmp->next!=NULL)
            printf(",");
    }
    printf("}");

}

JSON* newJSON()                 //allocate a new JSON and clear the menmory
{
    JSON* target=calloc(1, sizeof(JSON));
    target->child=NULL;
    target->key=NULL;
    target->next=NULL;
    target->valuedouble=0;
    target->valueint=-1;
    target->valuestring=NULL;
    target->type=JSON_NULL;
    return target;
}

void recursePrintToFile(FILE* file , JSON* item , int superType)
{
    static int depth=0;
    if (superType==JSON_OBJECT&&item->key!=NULL) {
        fprintf(file,"\"%s\": ",item->key);
    }
//    for(int y=0;y<depth;y++)
//    {
//        printf("    ");
//    }
    if(item->type==JSON_ARRAY)
    {
        fprintf(file,"[\r\n");
        depth++;
        JSON* tmp=item->child;
        for(;tmp!=NULL;tmp=tmp->next)
        {
            for(int y=0;y<depth;y++)
            {
                fprintf(file,"    ");
            }
            if (tmp->type==JSON_NULL) {
                fprintf(file,"null");
            }
            else if(tmp->type==JSON_TRUE){
                fprintf(file,"true");
            }
            else if(tmp->type==JSON_FALSE){
                fprintf(file,"false");
            }
            else if(tmp->type==JSON_NUMBER){
                fprintf(file,"%.3f",tmp->valuedouble);
            }
            else if(tmp->type==JSON_OBJECT||tmp->type==JSON_ARRAY){
                recursePrintToFile(file, tmp, item->type);
            }
            else{
                fprintf(file,"\"%s\"",tmp->valuestring);
            }
            if(tmp->next!=NULL)
                fprintf(file,",\r\n");
        }
        depth--;
        fprintf(file,"\r\n");
        for(int y=0;y<depth;y++)
        {
            fprintf(file,"    ");
        }
        fprintf(file,"]");
    }
    else
    {
        fprintf(file,"{\r\n");
        depth++;
        JSON* tmp=item->child;
        for(;tmp!=NULL;tmp=tmp->next)
        {
            for(int y=0;y<depth;y++)
            {
                fprintf(file,"    ");
            }
            if (tmp->type==JSON_NULL) {
                fprintf(file,"\"%s\": null",tmp->key);
            }
            else if(tmp->type==JSON_TRUE){
                fprintf(file,"\"%s\": true",tmp->key);
            }
            else if(tmp->type==JSON_FALSE){
                fprintf(file,"\"%s\": false",tmp->key);
            }
            else if(tmp->type==JSON_NUMBER){
                fprintf(file,"\"%s\": %.3f",tmp->key,tmp->valuedouble);
            }
            else if(tmp->type==JSON_OBJECT||tmp->type==JSON_ARRAY){
                recursePrintToFile(file, tmp, item->type);
            }
            else{
                fprintf(file,"\"%s\": \"%s\"",tmp->key,tmp->valuestring);
            }
            if(tmp->next!=NULL)
                fprintf(file,",\r\n");
        }
        depth--;
        fprintf(file,"\r\n");
        for(int y=0;y<depth;y++)
        {
            fprintf(file,"    ");
        }
        fprintf(file,"}");
    }
}

//Required Interfaces
/* Parse & Print */
JSON *ParseJSON(const char *value)
{
    JSON* tmp=NULL;
    char* myvalue=calloc(strlen(value)+1, 1);       //create a non-const string
    strcpy(myvalue, value);
    myvalue=noFormat(myvalue);
    //get my own chageable JSON string without space
    if(myvalue[0]=='{')
        tmp=objectParse(myvalue,getParseLength(myvalue));   //parse object
    else if(myvalue[0]=='[')
        tmp=arrayParse(myvalue,getParseLength(myvalue));    //parse array
    else
    {
        tmp=stringParse(myvalue,getParseLength(myvalue));
    }
    //here let's parse string or bool or number
    //ask Lin Fan if value is free from ""
    free(myvalue);
    //free the memory
    return tmp;
}

JSON *ParseJSONFromFile(const char *file_name)
{
    FILE* file;
    file=fopen(file_name, "r+");
    if(file==NULL)
        return NULL;
    fseek(file, 0, SEEK_SET);
    fseek(file, 0, SEEK_END);
    long fileLength = ftell(file);
    JSON* tmp;
    char* JSONContent=calloc(fileLength+4, 1);
    int index=0;
    fseek(file, 0, SEEK_SET);
    for(char tmp=fgetc(file);tmp!=EOF;index++)
    {
        JSONContent[index]=tmp;
        JSONContent[index+1]='\0';
        tmp=fgetc(file);
    }
//    printf("%s\n",JSONContent);
    tmp=ParseJSON(JSONContent);
    fclose(file);
    return  tmp;
}

void PrintJSON(JSON *item)
{
    if(item==NULL)
        return;
    if (item->type==JSON_NULL) {
        printf("null");
    }
    else if(item->type==JSON_TRUE){
        printf("true");
    }
    else if(item->type==JSON_FALSE){
        printf("false");
    }
    else if(item->type==JSON_NUMBER){
        printf("%.3f",item->valuedouble);
    }
    else if(item->type==JSON_OBJECT){
        printObject(item,item->type);
    }
    else if(item->type==JSON_ARRAY){
        printArray(item,item->type);
    }
    else{
        printf("\"%s\"",item->valuestring);
    }
}

void PrintJSONToFile(JSON *item, const char *file_name)
{
    if (item==NULL) {
        return;
    }
    FILE* outputFile;
    outputFile=fopen(file_name, "w+");
    if(outputFile==NULL)
        return;
    if (item->type==JSON_NUMBER) {
        fprintf(outputFile, "%.3f",item->valuedouble);
    }
    else if (item->type==JSON_TRUE){
        fprintf(outputFile, "true");
    }
    else if (item->type==JSON_FALSE){
        fprintf(outputFile, "false");
    }
    else if (item->type==JSON_NULL){
        fprintf(outputFile, "null");
    }
    else if (item->type==JSON_STRING){
        fprintf(outputFile, "%s",item->valuestring);
    }
    else if (item->type==JSON_ARRAY||item->type==JSON_OBJECT){
        recursePrintToFile(outputFile, item, item->type);
    }
    else;
    fclose(outputFile);
    return;
}

/* Create */
JSON *CreateNULL(void)
{
    JSON* tmp;
    tmp=newJSON();
    if(tmp==NULL)
        return tmp;
    tmp->type=JSON_NULL;
    return  tmp;
}

JSON *CreateTrue(void)
{
    JSON* tmp;
    tmp=newJSON();
    if(tmp==NULL)
        return tmp;
    tmp->type=JSON_TRUE;
    tmp->valueint=JSON_TRUE;
    return  tmp;
}

JSON *CreateFalse(void)
{
    JSON* tmp;
    tmp=newJSON();
    if(tmp==NULL)
        return tmp;
    tmp->type=JSON_FALSE;
    tmp->valueint=JSON_FALSE;
    return  tmp;
}

JSON *CreateBool(int b)
{
    JSON* tmp;
    tmp=newJSON();
    if(tmp==NULL)
        return tmp;
    if(b==JSON_FALSE)
    {
        tmp->type=JSON_FALSE;
        tmp->valueint=JSON_FALSE;
    }
    else
    {
        tmp->type=JSON_TRUE;
        tmp->valueint=JSON_TRUE;
    }
    return  tmp;
}

JSON *CreateNumber(double num)
{
    JSON* tmp;
    tmp=newJSON();
    if(tmp==NULL)
        return tmp;
    tmp->type=JSON_NUMBER;
    tmp->valuedouble=num;
    return tmp;
}

JSON *CreateString(const char *string)
{
    JSON* tmp;
    tmp=newJSON();
    if(tmp==NULL)
        return tmp;
    tmp->type=JSON_STRING;
    tmp->valuestring=calloc(strlen(string)+1, sizeof(char));
    strcpy(tmp->valuestring, string);
    return tmp;
}

JSON *CreateArray(void)
{
    JSON* tmp;
    tmp=newJSON();
    if(tmp==NULL)
        return tmp;
    tmp->type=JSON_ARRAY;
    return tmp;
}

JSON *CreateObject(void)
{
    JSON* tmp;
    tmp=newJSON();
    tmp->type=JSON_OBJECT;
    return tmp;
}

/* Append */
void AddItemToArray(JSON *array, JSON *item)
{
    JSON* tmp=array;
    if(tmp->child==NULL)
        tmp->child=item;
    else{
        tmp=tmp->child;
        while (tmp->next!=NULL) {
            tmp=tmp->next;
        }
        tmp->next=item;
    }
}

void AddItemToObject(JSON *object, const char *key, JSON *value){
    JSON* tmp=object;
    if(tmp->child==NULL)
        tmp->child=value;
    else{
        tmp=tmp->child;
        while (tmp->next!=NULL) {
            tmp=tmp->next;
        }
        tmp->next=value;
    }
    if(value->key!=NULL)
        free(value->key);
    value->key=calloc(strlen(key)+1, 1);
    strcpy(value->key, key);
}

/* Update */
void ReplaceItemInArray(JSON *array, int which, JSON *new_item)
{
    if(array->child==NULL)
        return;
    else{
        JSON* save=array->child;
        if (which==0) {
            new_item->next=save->next;
            DeleteJSON(save);
            array->child=new_item;
            return;
        }
        for(;which>1;which--){
            if(save->next==NULL)
                return;
            save=save->next;
        }
        if(save->next==NULL)
            return;
        new_item->next=save->next->next;
        DeleteJSON(save->next);
        save->next=new_item;
    }
}

void ReplaceItemInObject(JSON *object, const char *key, JSON *new_value)
{
    JSON* tmp=object;
    if(tmp->child==NULL)
        return;
    else{
        JSON* save=tmp->child;
        char* new_key=calloc(strlen(key)+1, 1);
        strcpy(new_key, key);
        deleteSpace(new_key);

        if(strcmp(save->key, new_key)==0)
        {
            new_value->next=save->next;
            DeleteJSON(save);
            object->child=new_value;
        }
        else
        {
            tmp=save;
            while (tmp->next!=NULL) {
                if(strcmp(tmp->next->key, new_key)==0)
                    break;
                tmp=tmp->next;
            }
            if(tmp->next==NULL){
                free(new_key);
                return;
            }
            new_value->next=tmp->next->next;
            DeleteJSON(tmp->next);
            tmp->next=new_value;
        }

        //update key
        if(new_value->key!=NULL)
            free(new_value->key);
        new_value->key=new_key;
    }
}

/* Remove/Delete */
JSON *DetachItemFromArray(JSON *array, int which)
{
    if(array->child==NULL)
        return NULL;
    JSON* tmp=array->child;
    if (which==0) {
        array->child=array->child->next;
        return tmp;
    }else{
        for(;which>1;which--) {
            tmp=tmp->next;
            if (tmp==NULL)
                return NULL;
        }
        JSON* save=tmp->next;
        tmp->next=save->next;
        return save;
    }
}

void *DeleteItemFromArray(JSON *array, int which)
{
    JSON* tmp = DetachItemFromArray(array, which);
    if(tmp==NULL)
        return tmp;
    DeleteJSON(tmp);
    return NULL;
}

JSON *DetachItemFromObject(JSON *object, const char *key)
{
    if(object->child==NULL)
        return NULL;
    JSON* tmp=object->child;
    char* new_key=calloc(strlen(key)+1, 1);
    strcpy(new_key, key);
    deleteSpace(new_key);
    if(tmp==NULL)
        return tmp;
    if(strcmp(tmp->key, new_key)==0)
    {
        object->child=tmp->next;
        free(new_key);
        return tmp;
    }
    for (; tmp->next!=NULL ; tmp=tmp->next) {
        if(strcmp(tmp->next->key, new_key)==0)
            break;
    }
    if(tmp->next==NULL){
        free(new_key);
        return NULL;
    }
    JSON* save=tmp->next;
    tmp->next=save->next;
    free(new_key);
    return save;
}

void *DeleteItemFromObject(JSON *object, const char *key)
{
    JSON* tmp = DetachItemFromObject(object, key);
    if(tmp==NULL)
        return tmp;
    DeleteJSON(tmp);
    return NULL;
}

void DeleteJSON(JSON* item)
{
    if(item==NULL)
        return;
    if(item->child!=NULL)
        DeleteJSON(item->child);
    if(item->key!=NULL)
        free(item->key);
    if(item->next==NULL)
        DeleteJSON(item->next);
    if(item->valuestring==NULL)
        free(item->valuestring);

    free(item);
}

/* Duplicate */
JSON *Duplicate(JSON *item, int recurse)
{
    if(item==NULL)
        return NULL;
    static int inRecurse=0;
    JSON* tmp=CreateNULL();
    //common
    tmp->type=item->type;
    tmp->valuedouble=item->valuedouble;
    tmp->valueint=item->valueint;
    if (item->valuestring!=NULL) {
        tmp->valuestring=calloc(strlen(item->valuestring)+1, 1);
        strcpy(tmp->valuestring, item->valuestring);
    }
    if (item->key!=NULL) {
        tmp->key=calloc(strlen(item->key)+1, 1);
        strcpy(tmp->key, item->key);
    }
    //common over
    if(recurse==1)
    {
        if(inRecurse>0)
        {
            tmp->next=Duplicate(item->next, 1);
        }
        inRecurse++;
        tmp->child=Duplicate(item->child, 1);
        inRecurse--;
    }
    else
    {
        tmp->next=NULL;
        tmp->child=item->child;
    }
    return tmp;
}

/* Read */
JSON *GetItemInArray(JSON *array, int which)
{
    JSON* tmp=array->child;
    for(;which>0;which--)
    {
        if (tmp==NULL) {
            break;
        }
        tmp=tmp->next;
    }
    return tmp;
}

JSON *GetItemInObject(JSON *object, const char *key)
{
    char* new_key=calloc(strlen(key)+1, 1);
    strcpy(new_key, key);
    JSON* tmp = object->child;
    for(;tmp!=NULL;tmp=tmp->next)
    {
        if(strcmp(tmp->key, new_key)==0)
            break;
    }
    return tmp;
}

JSON *GetItemInJSON(JSON *json, const char *path)
{
    JSON* tmp=json;
    char* myPath=calloc(strlen(path)+1, 1);
    strcpy(myPath, path);
    deleteSpace(myPath);
    char* subPath=calloc(strlen(path)+1, 1);
    for(int y=1;myPath[y]!='\0'&&myPath[y]!='/';)
    {
        for(int k=0;myPath[y]!='\0'&&myPath[y]!='/';)
        {
            subPath[k]=myPath[y];
            subPath[k+1]='\0';
            y++;
            k++;
        }
        y++;
        //subPath got
        //now get it
        if(tmp==NULL)
            return NULL;
        if(tmp->type==JSON_ARRAY)
            tmp=GetItemInArray(tmp, (int)atof(subPath));
        else if (tmp->type==JSON_OBJECT)
            tmp=GetItemInObject(tmp, subPath);
        else if (strcmp(tmp->key, subPath)==0)
            return tmp;
        else
            return NULL;
    }

    return tmp;
}
