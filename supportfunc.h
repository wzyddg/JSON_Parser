#ifndef JSONParser_14_supportfunc_h
#define JSONParser_14_supportfunc_h

int isSpace(char c)
{
    return (c=='\n'||c=='\r'||c=='\t'||c==' ');
}

int isFormatChar(char a)
{
    return (a=='['||a=='{'||a=='}'||a==']'||a==':'||a==',');
}

int isContentChar(char a)
{
    return !(a=='['||a=='{'||a=='}'||a==']'||a==':'||a==','||a=='\"');
}

int nextIsFormate(char* a)
{
    for(int y=1;y<strlen(a);y++)
    {
        if(isFormatChar(a[y]))
            return 1;
        if(a[y]=='\"')
            return 0;
    }
    return 0;
}

void deleteSpace(char* target)
{
    int spaceNumber=0;
    int index=0;
    int inString=0;

    for(;index<strlen(target);index++)
    {
        if(index==0){
            if(isFormatChar(target[index])){
                //                lastIsContent=0;
                inString=0;
            }
            else if (target[index]=='\"') {
                inString=1;
            }
            else if (isContentChar(target[index])&&!isSpace(target[index])){
                inString=1;
            }
            else if (isSpace(target[index])){
                inString=0;
            }
        }
        if(isContentChar(target[index])&&!isSpace(target[index]))
            inString=1;
        //如果是表示空的字符且不在串内
        if(isSpace(target[index])&&!inString)
            spaceNumber++;
        else
        {
            target[index-spaceNumber]=target[index];
        }
        //遇到引号则翻转是否在字符串内的判断变量
        if(target[index]=='\"')
            inString=!inString;
        //处理字符串结尾的空格
        if(nextIsFormate(target+index))
            inString=0;
    }
    target[index-spaceNumber]='\0';
}

char* noFormat(char* target) //把被省略的引号加上且用#代替，变成标准无格式JSON字符串
{
    deleteSpace(target);

    int quotationNumber = 0;   //number of added '\"'
    char* tmp = calloc(3*strlen(target)+1,1);

    for(int y=0;y<strlen(target);y++)
    {
        if(y==1&&isContentChar(tmp[0])){
            y=y-2;
            tmp[0]='\"';
            quotationNumber++;
            continue;
        }
        if(y+1==strlen(target))
        {
            tmp[y+quotationNumber]=target[y];
            break;
        }

        if(isFormatChar(target[y]))
        {
            if(isContentChar(target[y+1]))
            {
                tmp[y+quotationNumber]=target[y];
                tmp[y+quotationNumber+1]='\"';
                quotationNumber++;
            }
            else
            {
                tmp[y+quotationNumber]=target[y];
            }
            continue;
        }
        if(target[y]=='\"')
        {
            tmp[y+quotationNumber]=target[y];
            continue;
        }
        if(isContentChar(target[y]))
        {
            if(!isFormatChar(target[y+1]))
            {
                tmp[y+quotationNumber]=target[y];
            }
            else
            {
                tmp[y+quotationNumber]=target[y];
                tmp[y+quotationNumber+1]='\"';
                quotationNumber++;
            }
            continue;
        }
    }
    if(!isFormatChar(tmp[strlen(tmp)-1])&&tmp[strlen(tmp)-1]!='\"')
    {
        tmp[strlen(tmp)]='\"';
        tmp[strlen(tmp)+1]='\0';
    }
    free(target);
    target=malloc(sizeof(char)*(strlen(tmp)+1));
    strcpy(target, tmp);
    free(tmp);
    return target;
}

int getParseLength(char* JSONString)       //  获取递归调用的长度,定义start和over，配对即停止计数并返回
{
    int stack=0;
    int length=0;
    char start='\"';
    char over='\"';
    if(JSONString[0]=='\"'){
        start=JSONString[0];
        over=start;
    }
    else if(JSONString[0]=='{'){
        start='{';
        over='}';
    }
    else if(JSONString[0]=='['){
        start='[';
        over=']';
    }
    else{
        return 0;
    }
    //here
    if(start!='\"'){
        do
        {
            char tmp=JSONString[length];
            if(tmp==start)
                stack++;
            if(tmp==over)
                stack--;
            length++;
        }while (stack!=0);
    }
    else{
        for(length=1;length<strlen(JSONString);length++){
            if(JSONString[length]==JSONString[0]&&isFormatChar(JSONString[length+1]))
                return length+1;
        }
    }

    return length;
}

int isNumber(char* a)       //检测该字符串是否数字
{
    int everDot=0;
    for (int y=0; y<strlen(a); y++) {
        if(y==0){
            if(a[y]<'0'||a[y]>'9')
                return 0;
        }
        if(a[y]=='.'){
            if(everDot==1)
                return 0;
            else
                everDot=1;
        }
        else if(a[y]<'0'||a[y]>'9')
            return 0;
    }
    return 1;
}
#endif
